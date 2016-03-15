
#include "AUFrameworkBaseStdAfx.h"
#include "AUOsmDatabase.h"
#include "tlali-osm.h"
#include "AUNumerico.h"
#include "NBGestorArchivos.h"

AUOsmDatabase::AUOsmDatabase() : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabase::AUOsmDatabase")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUOsmDatabase")
	_nodes	= new(this) AUArregloNativoOrdenadoMutableP<STOsmDbNode>();
	_ways	= new(this) AUArregloNativoOrdenadoMutableP<STOsmDbWay>();
	_routes	= new(this) AUArregloNativoOrdenadoMutableP<STOsmDbRoute>();
	//
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUOsmDatabase::~AUOsmDatabase(void){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabase::~AUOsmDatabase")
	this->empty();
	if(_routes != NULL){ NBASSERT(_routes->conteo == 0) _routes->liberar(); _routes = NULL; }
	if(_ways != NULL){ NBASSERT(_ways->conteo == 0) _ways->liberar(); _ways = NULL; }
	if(_nodes != NULL){ NBASSERT(_nodes->conteo == 0) _nodes->liberar(); _nodes = NULL; }
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

const AUArregloNativoOrdenadoP<STOsmDbNode>* AUOsmDatabase::getNodes() const {
	return _nodes;
}

const AUArregloNativoOrdenadoP<STOsmDbWay>* AUOsmDatabase::getWays() const {
	return _ways;
}

const AUArregloNativoOrdenadoP<STOsmDbRoute>* AUOsmDatabase::getRoutes() const {
	return _routes;
}

//

void AUOsmDatabase::empty(){
	//Release routes
	{
		SI32 i; const SI32 count = _routes->conteo;
		for(i = 0; i < count;  i++){
			STOsmDbRoute* itm = _routes->elemPtr(i);
			STOsmDbRoute_release(itm);
		}
		_routes->vaciar();
	}
	//Release ways
	{
		SI32 i; const SI32 count = _ways->conteo;
		for(i = 0; i < count;  i++){
			STOsmDbWay* itm = _ways->elemPtr(i);
			STOsmDbWay_release(itm);
		}
		_ways->vaciar();
	}
	//Release nodes
	{
		SI32 i; const SI32 count = _nodes->conteo;
		for(i = 0; i < count;  i++){
			STOsmDbNode* itm = _nodes->elemPtr(i);
			STOsmDbNode_release(itm);
		}
		_nodes->vaciar();
	}
}

TlaSI32 AUOsmDatabase_ReadFromAUFileFunc(void* dstBuffer, const TlaSI32 sizeOfBlock, const TlaSI32 maxBlocksToRead, void* userData){
	return ((AUArchivo*)userData)->leer(dstBuffer, sizeOfBlock, maxBlocksToRead, (AUArchivo*)userData);
}

TlaSI32 AUOsmDatabase_WriteToAUFileFunc(const void* srcBuffer, const TlaSI32 sizeOfBlock, const TlaSI32 maxBlocksToWrite, void* userData){
	return ((AUArchivo*)userData)->escribir(srcBuffer, sizeOfBlock, maxBlocksToWrite, (AUArchivo*)userData);
}

bool AUOsmDatabase::loadFromFileXml(const char* filePath){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabase::loadFromFileXml")
	bool r = false;
	STTlaOsm osmFromXml;
	osmInit(&osmFromXml);
	AUArchivo* stream = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, filePath, ENArchivoModo_SoloLectura);
	if(stream == NULL){
		NBASSERT(0);
	} else {
		stream->lock();
		if(!osmLoadFromXmlStream(&osmFromXml, AUOsmDatabase_ReadFromAUFileFunc, stream)){
			NBASSERT(0);
		} else {
			this->empty();
			//Walk relations
			{
				STTlaRel data;
				if(osmGetNextRel(&osmFromXml, &data, NULL)){
					do{
						bool isRoute = false;
						//-----------
						//-- Determine if tag-pair: 'type = route'
						//-----------
						{
							STTlaTag tag;
							if(osmGetNextRelTag(&osmFromXml, &data, &tag, NULL)){
								do {
									if(AUCadena8::cadenasSonIguales(tag.name, "type")){
										if(AUCadena8::cadenasSonIguales(tag.value, "route")){
											isRoute = true;
											break;
										}
									}
								} while(osmGetNextRelTag(&osmFromXml, &data, &tag, &tag));
							}
						}
						//-----------
						//-- Load data
						//-- (if it is a route-relation)
						//-----------
						if(isRoute){
							STOsmDbRoute srchRoute;
							srchRoute.id = data.id;
							if(this->_routes->indiceDe(srchRoute) == -1){
								STOsmDbRoute newRoute;
								AUOsmDatabase::STOsmDbRoute_init(&newRoute, this);
								newRoute.id = data.id;
								//Read tags
								{
									STTlaTag tag;
									if(osmGetNextRelTag(&osmFromXml, &data, &tag, NULL)){
										do {
											if(AUCadena8::cadenasSonIguales(tag.name, "network")){
												if(newRoute.network != NULL) newRoute.network->liberar();
												newRoute.network = new(this) AUCadena8(tag.value);
											} else if(AUCadena8::cadenasSonIguales(tag.name, "ref")){
												if(newRoute.refName != NULL) newRoute.refName->liberar();
												newRoute.refName = new(this) AUCadena8(tag.value);
											} else if(AUCadena8::cadenasSonIguales(tag.name, "name")){
												if(newRoute.name != NULL) newRoute.name->liberar();
												newRoute.name = new(this) AUCadena8(tag.value);
											} else if(AUCadena8::cadenasSonIguales(tag.name, "from")){
												if(newRoute.from != NULL) newRoute.from->liberar();
												newRoute.from = new(this) AUCadena8(tag.value);
											} else if(AUCadena8::cadenasSonIguales(tag.name, "to")){
												if(newRoute.to != NULL) newRoute.to->liberar();
												newRoute.to = new(this) AUCadena8(tag.value);
											} else if(AUCadena8::cadenasSonIguales(tag.name, "operator")){
												if(newRoute.operatr != NULL) newRoute.operatr->liberar();
												newRoute.operatr = new(this) AUCadena8(tag.value);
											} else {
												//PRINTF_INFO("  Ignoring route-relation tag: '%s' = '%s'.\n", tag.name, tag.value);
											}
										} while(osmGetNextRelTag(&osmFromXml, &data, &tag, &tag));
									}
								}
								//Read all members (nodes and ways)
								{
									STTlaRelMember member;
									if(osmGetNextRelMember(&osmFromXml, &data, &member, NULL)){
										do {
											//Get member data
											{
												//Load node
												if(member.type[0] == 'n') if(member.type[1] == 'o') if(member.type[2] == 'd') if(member.type[3] == 'e') if(member.type[4] == '\0'){
													STTlaNode node;
													if(!osmGetNodeById(&osmFromXml, &node, member.ref)){
														PRINTF_WARNING("  WARNING, on route('%s') (rel-id %lld), node(%lld) doesnt exists.\n", (newRoute.refName != NULL ? newRoute.refName->str() : "unnamed"), data.id, member.ref);
													} else {
														STOsmDbNode srchNode;
														srchNode.id = node.id;
														if(this->_nodes->indiceDe(srchNode) == -1){
															this->privAddNode(&osmFromXml, &node);
														}
														{
															STOsmDbRouteNode routeNode;
															STOsmDbRouteNode_init(&routeNode, this);
															routeNode.id = node.id;
															if(AUCadena8::cadenasSonIguales(member.role, "platform")){
																routeNode.type = ENOsmDbMemNodeType_platform;
															} else if(AUCadena8::cadenasSonIguales(member.role, "stop")){
																routeNode.type = ENOsmDbMemNodeType_stop;
															} else {
																//PRINTF_INFO("  Ignoring node member role: '%s'.\n", member.role);
															}
															newRoute.nodesRefs->agregarElemento(routeNode);
														}
													}
												}
												//Load way
												if(member.type[0] == 'w') if(member.type[1] == 'a') if(member.type[2] == 'y') if(member.type[3] == '\0') {
													STTlaWay way;
													if(!osmGetWayById(&osmFromXml, &way, member.ref)){
														PRINTF_WARNING("  WARNING, on route('%s') (rel-id %lld), way(%lld) doesnt exists.\n", (newRoute.refName != NULL ? newRoute.refName->str() : "unnamed"), data.id, member.ref);
													} else {
														STOsmDbWay srchWay;
														srchWay.id = way.id;
														if(this->_ways->indiceDe(srchWay) == -1){
															this->privAddWay(&osmFromXml, &way);
														}
														{
															STOsmDbRouteWay routeWay;
															STOsmDbRouteWay_init(&routeWay, this);
															routeWay.id = way.id;
															if(AUCadena8::cadenasSonIguales(member.role, "backward")){
																routeWay.type = ENOsmDbMemWayType_backward;
															}
															newRoute.waysRefs->agregarElemento(routeWay);
														}
													}
												}
											}
										} while(osmGetNextRelMember(&osmFromXml, &data, &member, &member));
									}
								}
								this->_routes->agregarElemento(newRoute);
							} else {
								PRINTF_WARNING("  WARNING, route (rel-id %lld) was found again (ignoring after first found).\n", data.id);
							}
						}
					} while(osmGetNextRel(&osmFromXml, &data, &data));
				}
			}
			PRINTF_INFO("Xml loaded (%d routes, %d ways, %d nodes).\n", this->_routes->conteo, this->_ways->conteo, this->_nodes->conteo);
			r = true;
		}
		stream->unlock();
		stream->cerrar();
	}
	osmRelease(&osmFromXml);
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

void AUOsmDatabase::privAddNode(void* osmData, void* nodeData){
	STTlaOsm* osm = (STTlaOsm*)osmData;
	STTlaNode* node = (STTlaNode*)nodeData;
	STOsmDbNode newNode;
	STOsmDbNode_init(&newNode, this);
	newNode.id = node->id;
	newNode.lat = node->lat;
	newNode.lon = node->lon;
	//Read tags
	{
		STTlaTag tag;
		if(osmGetNextNodeTag(osm, node, &tag, NULL)){
			do {
				if(AUCadena8::cadenasSonIguales(tag.name, "name")){
					if(newNode.name != NULL) newNode.name->liberar();
					newNode.name = new(this) AUCadena8(tag.value);
				} else if(AUCadena8::cadenasSonIguales(tag.name, "official_status")){
					if(AUCadena8::cadenasSonIguales(tag.value, "IRTRAMMA:bus_station")){
						newNode.officialStatus = ENOsmDbNodeOStatus_busStation;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "IRTRAMMA:bus_stop")){
						newNode.officialStatus = ENOsmDbNodeOStatus_busStop;
					}
				} else {
					//PRINTF_INFO("  Ignoring node tag: '%s' = '%s'.\n", tag.name, tag.value);
				}
			} while(osmGetNextNodeTag(osm, node, &tag, &tag));
		}
	}
	this->_nodes->agregarElemento(newNode);
}

void AUOsmDatabase::privAddWay(void* osmData, void* wayData){
	STTlaOsm* osm = (STTlaOsm*)osmData;
	STTlaWay* way = (STTlaWay*)wayData;
	STOsmDbWay newWay;
	STOsmDbWay_init(&newWay, this);
	newWay.id = way->id;
	//Read nodes ref
	{
		const TlaSI64* arr; TlaSI32 count;
		if(osmGetWayNodes(osm, way, &arr, &count)){
			TlaSI32 i;
			for(i = 0; i < count; i++){
				//Verify if node exists
				{
					STTlaNode node;
					if(!osmGetNodeById(osm, &node, arr[i])){
						PRINTF_WARNING("  WARNING, on way(%lld), node(%lld) doesnt exists.\n", way->id, arr[i]);
					} else {
						STOsmDbNode srchNode;
						srchNode.id = arr[i];
						if(this->_nodes->indiceDe(srchNode) == -1){
							this->privAddNode(osm, &node);
						}
						newWay.nodes->agregarElemento(node.id);
					}
				}
			}
		}
	}
	//Read tags
	{
		STTlaTag tag;
		if(osmGetNextWayTag(osm, way, &tag, NULL)){
			do {
				if(AUCadena8::cadenasSonIguales(tag.name, "name")){
					if(newWay.name != NULL) newWay.name->liberar();
					newWay.name = new(this) AUCadena8(tag.value);
				} else if(AUCadena8::cadenasSonIguales(tag.name, "alt_name")){
					if(newWay.altName != NULL) newWay.altName->liberar();
					newWay.altName = new(this) AUCadena8(tag.value);
				} else if(AUCadena8::cadenasSonIguales(tag.name, "loc_name")){
					if(newWay.locName != NULL) newWay.locName->liberar();
					newWay.locName = new(this) AUCadena8(tag.value);
				} else if(AUCadena8::cadenasSonIguales(tag.name, "old_name")){
					if(newWay.oldName != NULL) newWay.oldName->liberar();
					newWay.oldName = new(this) AUCadena8(tag.value);
				} else if(AUCadena8::cadenasSonIguales(tag.name, "short_name")){
					if(newWay.shortName != NULL) newWay.shortName->liberar();
					newWay.shortName = new(this) AUCadena8(tag.value);
				} else if(AUCadena8::cadenasSonIguales(tag.name, "ref")){
					if(newWay.ref != NULL) newWay.ref->liberar();
					newWay.ref = new(this) AUCadena8(tag.value);
				} else if(AUCadena8::cadenasSonIguales(tag.name, "highway")){
					if(AUCadena8::cadenasSonIguales(tag.value, "construction")){
						newWay.highwayType = ENOsmDbHightway_construction;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "living_street")){
						newWay.highwayType = ENOsmDbHightway_livingStreet;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "primary")){
						newWay.highwayType = ENOsmDbHightway_primary;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "primary_link")){
						newWay.highwayType = ENOsmDbHightway_primaryLink;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "residential")){
						newWay.highwayType = ENOsmDbHightway_residential;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "secondary")){
						newWay.highwayType = ENOsmDbHightway_secondary;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "secondary_link")){
						newWay.highwayType = ENOsmDbHightway_secondaryLink;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "service")){
						newWay.highwayType = ENOsmDbHightway_service;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "tertiary")){
						newWay.highwayType = ENOsmDbHightway_tertiary;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "tertiary_link")){
						newWay.highwayType = ENOsmDbHightway_tertiaryLink;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "trunk")){
						newWay.highwayType = ENOsmDbHightway_trunk;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "trunk_link")){
						newWay.highwayType = ENOsmDbHightway_trunkLink;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "unclassified")){
						newWay.highwayType = ENOsmDbHightway_unclassified;
					} else {
						PRINTF_WARNING("  WARNING, ignoring way tag value: '%s' = '%s' (add to posibilities at code!).\n", tag.name, tag.value);
					}
				} else if(AUCadena8::cadenasSonIguales(tag.name, "lanes")){
					if(AUNumericoP<SI32, char>::esEntero(tag.value)){
						newWay.lanesCount = AUNumericoP<SI32, char>::aEntero(tag.value);
					} else {
						PRINTF_WARNING("  WARNING, way(%lld) tag value is not integer: '%s' = '%s'.\n", way->id, tag.name, tag.value);
					}
				} else if(AUCadena8::cadenasSonIguales(tag.name, "layer")){
					if(AUNumericoP<SI32, char>::esEntero(tag.value)){
						newWay.layer = AUNumericoP<SI32, char>::aEntero(tag.value);
					} else {
						PRINTF_WARNING("  WARNING, way(%lld) tag value is not integer: '%s' = '%s'.\n", way->id, tag.name, tag.value);
					}
				} else if(AUCadena8::cadenasSonIguales(tag.name, "maxspeed")){
					if(AUNumericoP<SI32, char>::esEntero(tag.value)){
						newWay.maxSpeed = AUNumericoP<SI32, char>::aEntero(tag.value);
					} else {
						PRINTF_WARNING("  WARNING, way(%lld) tag value is not integer: '%s' = '%s'.\n", way->id, tag.name, tag.value);
					}
				} else if(AUCadena8::cadenasSonIguales(tag.name, "oneway")){
					newWay.isOneWay = ENOsmDbIsOneWay_no; //Note: somethins we found values as 'yes1' (misstypings?)
					if(tag.value[0] == 'y') if(tag.value[1] == 'e') if(tag.value[2] == 's') newWay.isOneWay = ENOsmDbIsOneWay_yes;
				} else if(AUCadena8::cadenasSonIguales(tag.name, "surface")){
					if(AUCadena8::cadenasSonIguales(tag.value, "asphalt")){
						newWay.surfaceType = ENOsmDbSurface_asphalt;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "cobblestone")){
						newWay.surfaceType = ENOsmDbSurface_cobblestone;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "concrete")){
						newWay.surfaceType = ENOsmDbSurface_concrete;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "dirt")){
						newWay.surfaceType = ENOsmDbSurface_dirt;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "earth")){
						newWay.surfaceType = ENOsmDbSurface_earth;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "paved")){
						newWay.surfaceType = ENOsmDbSurface_paved;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "paving_stones")){
						newWay.surfaceType = ENOsmDbSurface_paving_stones;
					} else if(AUCadena8::cadenasSonIguales(tag.value, "unpaved")){
						newWay.surfaceType = ENOsmDbSurface_unpaved;
					} else {
						PRINTF_WARNING("  WARNING, ignoring way tag value: '%s' = '%s' (add to posibilities at code!).\n", tag.name, tag.value);
					}
				} else {
					//PRINTF_WARNING("  WARNING, ignoring way tag: '%s' = '%s'.\n", tag.name, tag.value);
				}
			} while(osmGetNextWayTag(osm, way, &tag, &tag));
		}
	}
	this->_ways->agregarElemento(newWay);
}

//
//Factories
//

void AUOsmDatabase::STOsmDbNode_init(STOsmDbNode* obj, AUObjeto* parent){
	obj->id		= 0;
	obj->lat	= 0.0f;
	obj->lon	= 0.0f;
	//Optionals (from tags)
	obj->name	= NULL;
	obj->officialStatus = ENOsmDbNodeOStatus_noDef;
	
}

void AUOsmDatabase::STOsmDbNode_release(STOsmDbNode* obj){
	obj->id		= 0;
	obj->lat	= 0.0f;
	obj->lon	= 0.0f;
	//Optionals (from tags)
	if(obj->name != NULL) obj->name->liberar(); obj->name = NULL;
	obj->officialStatus = ENOsmDbNodeOStatus_noDef ; //Node tag-pair: 'official_status = ???'
}

void AUOsmDatabase::STOsmDbWay_init(STOsmDbWay* obj, AUObjeto* parent){
	obj->id			= 0;
	obj->nodes		= new(parent) AUArregloNativoMutableP<SI64>();
	//Optionals (from tags)
	obj->name		= NULL;		//Way tag-pair: 'name = str'
	obj->altName	= NULL;		//Way tag-pair: 'alt_name = str', 'alt_name_1 = str', or 'alt_name_2 = str'
	obj->locName	= NULL;		//Way tag-pair: 'loc_name = str'
	obj->oldName	= NULL;		//Way tag-pair: 'old_name = str'
	obj->shortName	= NULL;		//Way tag-pair: 'short_name = str'
	obj->ref		= NULL;		//Way tag-pair: 'ref = str'
	obj->highwayType= ENOsmDbHightway_noDef;	//Way tag-pair: 'highway = str'
	obj->lanesCount	= 0;		//Way tag-pair: 'lanes = #'
	obj->layer		= 0;		//Way tag-pair: 'layer = #'
	obj->maxSpeed	= 0;		//Way tag-pair: 'maxspeed = #'
	obj->isOneWay	= ENOsmDbIsOneWay_noDef; //Way tag-pair: 'oneway = yes/no'
	obj->surfaceType= ENOsmDbSurface_noDef;
}

void AUOsmDatabase::STOsmDbWay_release(STOsmDbWay* obj){
	obj->id			= 0;
	if(obj->nodes != NULL) obj->nodes->liberar(); obj->nodes = NULL;
	//Optionals (from tags)
	if(obj->name != NULL) obj->name->liberar(); obj->name = NULL;
	if(obj->altName != NULL) obj->altName->liberar(); obj->altName = NULL;
	if(obj->locName != NULL) obj->locName->liberar(); obj->locName = NULL;
	if(obj->oldName != NULL) obj->oldName->liberar(); obj->oldName = NULL;
	if(obj->shortName != NULL) obj->shortName->liberar(); obj->shortName = NULL;
	if(obj->ref != NULL) obj->ref->liberar(); obj->ref = NULL;
	obj->highwayType= ENOsmDbHightway_noDef;
	obj->lanesCount	= 0;
	obj->layer		= 0;
	obj->maxSpeed	= 0;
	obj->isOneWay	= ENOsmDbIsOneWay_noDef;
	obj->surfaceType= ENOsmDbSurface_noDef;
}

void AUOsmDatabase::STOsmDbRoute_init(STOsmDbRoute* obj, AUObjeto* parent){
	obj->id			= 0;
	//Members
	obj->nodesRefs	= new(parent) AUArregloNativoMutableP<STOsmDbRouteNode>();
	obj->waysRefs	= new(parent) AUArregloNativoMutableP<STOsmDbRouteWay>();
	//Optionals (from tags)
	obj->network	= NULL;		//Relation tag-pair: 'network = str'
	obj->refName	= NULL;		//Relation tag-pair: 'ref = str'
	obj->name		= NULL;		//Relation tag-pair: 'name = str'
	obj->from		= NULL;		//Relation tag-pair: 'from = str'
	obj->to			= NULL;		//Relation tag-pair: 'to = str'
	obj->operatr	= NULL;		//Relation tag-pair: 'operator = str'
}

void AUOsmDatabase::STOsmDbRoute_release(STOsmDbRoute* obj){
	obj->id			= 0;
	//Members
	if(obj->nodesRefs != NULL) obj->nodesRefs->liberar(); obj->nodesRefs = NULL;
	if(obj->waysRefs != NULL) obj->waysRefs->liberar(); obj->waysRefs = NULL;
	//Optionals (from tags)
	if(obj->network != NULL) obj->network->liberar(); obj->network = NULL;
	if(obj->refName != NULL) obj->refName->liberar(); obj->refName = NULL;
	if(obj->name != NULL) obj->name->liberar(); obj->name = NULL;
	if(obj->from != NULL) obj->from->liberar(); obj->from = NULL;
	if(obj->to != NULL) obj->to->liberar(); obj->to = NULL;
	if(obj->operatr != NULL) obj->operatr->liberar(); obj->operatr = NULL;
}

void AUOsmDatabase::STOsmDbRouteNode_init(STOsmDbRouteNode* obj, AUObjeto* parent){
	obj->id		= 0;
	obj->type	= ENOsmDbMemNodeType_noDef;
}

void AUOsmDatabase::STOsmDbRouteNode_release(STOsmDbRouteNode* obj){
	obj->id		= 0;
	obj->type	= ENOsmDbMemNodeType_noDef;
}

void AUOsmDatabase::STOsmDbRouteWay_init(STOsmDbRouteWay* obj, AUObjeto* parent){
	obj->id		= 0;
	obj->type	= ENOsmDbMemWayType_noDef;
}

void AUOsmDatabase::STOsmDbRouteWay_release(STOsmDbRouteWay* obj){
	obj->id		= 0;
	obj->type	= ENOsmDbMemWayType_noDef;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUOsmDatabase)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUOsmDatabase, "AUOsmDatabase")
AUOBJMETODOS_CLONAR_NULL(AUOsmDatabase)







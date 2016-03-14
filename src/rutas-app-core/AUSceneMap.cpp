//
//  AUSceneMap.cpp
//  rutas-app-core
//
//  Created by Marcos Ortega on 11/03/2016.
//  Copyright (c) 2016 NIBSA. All rights reserved.
//

#include "rutas-app-core-pch.h"
#include "AUSceneMap.h"

//

#define MAP_SCALE 10000.0

AUSceneMap::AUSceneMap(const SI32 iEscena, IListenerScenes* scenes) : AUAppEscena(){
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSceneMap")
	_iScene		= iEscena;
	_scenes			= scenes;
	_animObjects	= new(this) AUAnimadorObjetoEscena();
	//
	_layerScene		= new(this) AUEscenaContenedor();
	_layerGUI		= new(this) AUEscenaContenedor();
	//
	_layerMap		= new(this) AUEscenaContenedor();
	_layerScene->agregarObjetoEscena(_layerMap);
	//Touch layer
	{
		_layerGUITouch			= new(this) AUEscenaContenedor();
		_layerGUITouch->establecerEscuchadorTouches(this, this);
		{
			AUEscenaSprite* touchSprite = NULL;
			touchSprite			= new(this)AUEscenaSprite(); _layerGUITouch->agregarObjetoEscena(touchSprite); touchSprite->liberar();
			touchSprite			= new(this)AUEscenaSprite(); _layerGUITouch->agregarObjetoEscena(touchSprite); touchSprite->liberar();
		}
		_layerGUI->agregarObjetoEscena(_layerGUITouch);
	}
	//
	_routesVisual	= new(this) AUArregloNativoOrdenadoMutableP<STSceneMapRoute>();
	_waysVisual		= new(this) AUArregloNativoOrdenadoMutableP<STSceneMapWay>();
	_nodesVisual	= new(this) AUArregloNativoOrdenadoMutableP<STSceneMapNode>();
	//
	_routesDb		= new(this) AUOsmDatabase();
	if(!_routesDb->loadFromFileXml("./rutasManagua.xml")){
		PRINTF_ERROR("Couldn load osm-xml-file.\n");
	} else {
		PRINTF_INFO("Success, osm-xml-file loaded.\n");
		//
		const AUArregloNativoOrdenadoP<STOsmDbNode>* nodes		= _routesDb->getNodes();
		const AUArregloNativoOrdenadoP<STOsmDbWay>* ways		= _routesDb->getWays();
		const AUArregloNativoOrdenadoP<STOsmDbRoute>* routes	= _routesDb->getRoutes();
		//Load nodes as visual objects
		/*{
			SI32 i; const SI32 count = nodes->conteo;
			for(i = 0; i < count; i++){
				const STOsmDbNode* node = nodes->elemPtr(i);
				STSceneMapNode newNode;
				STSceneMapNode_init(&newNode, this);
				newNode.id = node->id;
				newNode.marker->establecerTraslacion(node->lon * MAP_SCALE, node->lat * MAP_SCALE);
				_layerMap->agregarObjetoEscena(newNode.layer);
				_nodesVisual->agregarElemento(newNode);
			}
		}*/
		//Load ways as visual objects
		{
			SI32 i; const SI32 count = ways->conteo;
			for(i = 0; i < count; i++){
				const STOsmDbWay* wayRef = ways->elemPtr(i);
				STOsmDbWay srch;
				srch.id = wayRef->id;
				const SI32 iFound = ways->indiceDe(srch);
				if(iFound == -1){
					PRINTF_ERROR("Way %lld is missing.\n", srch.id);
					NBASSERT(0)
				} else {
					PRINTF_INFO("Way %lld found!\n", srch.id);
					const STOsmDbWay* way = ways->elemPtr(iFound);
					STSceneMapWay newWay;
					STSceneMapWay_init(&newWay, this);
					newWay.id = way->id;
					//Build new way by nodes
					{
						SI32 i; const SI32 count = way->nodes->conteo;
						for(i = 0; i < count; i++){
							STOsmDbNode srchNode;
							srchNode.id = way->nodes->elem(i);
							const SI32 iFound = nodes->indiceDe(srchNode);
							if(iFound == -1){
								PRINTF_ERROR("Node %lld is missing.\n", srchNode.id);
								NBASSERT(0)
							} else {
								const STOsmDbNode* node = nodes->elemPtr(iFound);
								NBASSERT(node->lon != 0.0 && node->lat != 0.0)
								newWay.nodesPath->agregarCoordenadaLocal(node->lon * MAP_SCALE, node->lat * MAP_SCALE);
							}
						}
						//Remove if there's not enough nodes to build a lines-secuence
						const SI32 nodesCount = newWay.nodesPath->puntosLocales()->conteo;
						if(nodesCount < 2){
							PRINTF_WARNING("Just %d nodes.\n", newWay.nodesPath->puntosLocales()->conteo);
							STSceneMapWay_release(&newWay);
						} else {
							//PRINTF_INFO("OK %d nodes on route.\n", newWay.nodesPath->puntosLocales()->conteo);
							if(nodesCount < 3) newWay.nodesPath->establecerTipo(ENEscenaFiguraTipo_Linea);
							newWay.layer->agregarObjetoEscena(newWay.nodesPath);
							_layerMap->agregarObjetoEscena(newWay.layer);
							_waysVisual->agregarElemento(newWay);
						}
					}
				}
			}
		}
		//Load routes as visual objects
		{
			SI32 i; const SI32 count = routes->conteo;
			for(i = 0; i < count; i++){
				const STOsmDbRoute* route = routes->elemPtr(i);
				STSceneMapRoute newRoute;
				STSceneMapRoute_init(&newRoute, this);
				newRoute.id = route->id;
				//Build secuense of lines, using nodes
				/*{
					SI32 i; const SI32 count = route->nodesRefs->conteo;
					for(i = 0; i < count; i++){
						const STOsmDbRouteNode* nodeRef = route->nodesRefs->elemPtr(i);
						STOsmDbNode srch;
						srch.id = nodeRef->id;
						const SI32 iFound = nodes->indiceDe(srch);
						if(iFound == -1){
							PRINTF_ERROR("Node %lld is missing.\n", srch.id);
							NBASSERT(0)
						} else {
							const STOsmDbNode* node = nodes->elemPtr(iFound);
							NBASSERT(node->lon != 0.0 && node->lat != 0.0)
							newRoute.nodesPath->agregarCoordenadaLocal(node->lon * MAP_SCALE, node->lat * MAP_SCALE);
						}
					}
					//Remove if there's not enough nodes to build a lines-secuence
					if(newRoute.nodesPath->puntosLocales()->conteo < 2){
						PRINTF_WARNING("Just %d nodes.\n", newRoute.nodesPath->puntosLocales()->conteo);
						NBASSERT(newRoute.nodesPath->contenedor() == NULL)
					} else {
						PRINTF_INFO("OK %d nodes on route.\n", newRoute.nodesPath->puntosLocales()->conteo);
						if(nodesCount < 3) newRoute.nodesPath->establecerTipo(ENEscenaFiguraTipo_Linea);
						newRoute.layer->agregarObjetoEscena(newRoute.nodesPath);
					}
				}*/
				//Build secuense of lines, using ways
				{
					SI32 i; const SI32 count = route->waysRefs->conteo;
					for(i = 0; i < count; i++){
						const STOsmDbRouteWay* wayRef = route->waysRefs->elemPtr(i);
						STOsmDbWay srch;
						srch.id = wayRef->id;
						const SI32 iFound = ways->indiceDe(srch);
						if(iFound == -1){
							PRINTF_ERROR("Way %lld is missing.\n", srch.id);
							NBASSERT(0)
						} else {
							PRINTF_INFO("Way %lld found!\n", srch.id);
							newRoute.waysIds->agregarElemento(srch.id);
						}
					}
				}
				//
				_layerMap->agregarObjetoEscena(newRoute.layer);
				_routesVisual->agregarElemento(newRoute);
			}
		}
	}
	//
	{
		const NBCajaAABB mapBox = _layerMap->cajaAABBLocalCalculada();
		PRINTF_INFO("Map scene area x(%f, %f) y(%f, %f) width(%f) height(%f).\n", mapBox.xMin, mapBox.xMax, mapBox.yMin, mapBox.yMax, (mapBox.xMax - mapBox.xMin), (mapBox.yMax - mapBox.yMin));
	}
	//Organizar
	this->privValidateMapTranslation(10000000.0f, -10000000.0f);
	this->privOrganizarContenido(false);
	//
	NBGestorAnimadores::agregarAnimador(this);
}

AUSceneMap::~AUSceneMap(){
	NBGestorAnimadores::quitarAnimador(this);
	//
	this->privEmpty();
	if(_routesVisual != NULL) { NBASSERT(_routesVisual->conteo == 0) _routesVisual->liberar(); _routesVisual = NULL; }
	if(_waysVisual != NULL) { NBASSERT(_waysVisual->conteo == 0) _waysVisual->liberar(); _waysVisual = NULL; }
	if(_nodesVisual != NULL) { NBASSERT(_nodesVisual->conteo == 0) _nodesVisual->liberar(); _nodesVisual = NULL; }
	if(_routesDb != NULL) _routesDb->liberar(NB_RETENEDOR_THIS); _routesDb = NULL;
	//
	if(_animObjects != NULL) _animObjects->liberar(NB_RETENEDOR_THIS); _animObjects = NULL;
	//Main layers
	if(_layerMap != NULL) _layerMap->liberar(NB_RETENEDOR_THIS); _layerMap = NULL;
	if(_layerGUITouch != NULL) _layerGUITouch->liberar(NB_RETENEDOR_THIS); _layerGUITouch = NULL;
	if(_layerGUI != NULL) _layerGUI->liberar(NB_RETENEDOR_THIS); _layerGUI = NULL;
	if(_layerScene != NULL) _layerScene->liberar(NB_RETENEDOR_THIS); _layerScene = NULL;
}

//

void AUSceneMap::privEmpty(){
	//Release routes
	{
		SI32 i; const SI32 count = _routesVisual->conteo;
		for(i = 0; i < count; i++){
			STSceneMapRoute* itm = _routesVisual->elemPtr(i);
			STSceneMapRoute_release(itm);
		}
		_routesVisual->vaciar();
	}
	//Release ways
	{
		SI32 i; const SI32 count = _waysVisual->conteo;
		for(i = 0; i < count; i++){
			STSceneMapWay* itm = _waysVisual->elemPtr(i);
			STSceneMapWay_release(itm);
		}
		_waysVisual->vaciar();
	}
	//Release nodes
	{
		SI32 i; const SI32 count = _nodesVisual->conteo;
		for(i = 0; i < count; i++){
			STSceneMapNode* itm = _nodesVisual->elemPtr(i);
			STSceneMapNode_release(itm);
		}
		_routesVisual->vaciar();
	}
}

//

void AUSceneMap::privValidateMapTranslation(const float xDelta, const float yDelta){
	const NBCajaAABB layerBox = NBGestorEscena::cajaProyeccionGrupo(_iScene, ENGestorEscenaGrupo_Escena);
	const NBCajaAABB mapBox = _layerMap->cajaAABBLocal();
	NBPunto pos = _layerMap->traslacion();
	pos.x += xDelta;
	pos.y += yDelta;
	if(pos.x + mapBox.xMin > layerBox.xMin){
		pos.x = layerBox.xMin - mapBox.xMin;
	} else if(pos.x + mapBox.xMax < layerBox.xMax){
		pos.x = layerBox.xMax - mapBox.xMax;
	}
	if(pos.y + mapBox.yMin > layerBox.yMin){
		pos.y = layerBox.yMin - mapBox.yMin;
	} else if(pos.y + mapBox.yMax < layerBox.yMax){
		pos.y = layerBox.yMax - mapBox.yMax;
	}
	_layerMap->establecerTraslacion(pos);
	//PRINTF_INFO("Traslation (%f, %f).\n", pos.x, pos.y);
}

//

void AUSceneMap::STSceneMapNode_init(STSceneMapNode* obj, AUObjeto* parent){
	obj->id = 0;
	obj->layer = new(parent) AUEscenaContenedor();
	obj->marker = new(parent) AUEscenaFigura(ENEscenaFiguraTipo_PoligonoCerrado);
	obj->marker->agregarCoordenadaLocal(-2.0f, -2.0f);
	obj->marker->agregarCoordenadaLocal(-2.0f, 2.0f);
	obj->marker->agregarCoordenadaLocal(2.0f, 2.0f);
	obj->marker->agregarCoordenadaLocal(2.0f, -2.0f);
	obj->layer->agregarObjetoEscena(obj->marker);
}

void AUSceneMap::STSceneMapNode_release(STSceneMapNode* obj){
	obj->id = 0;
	if(obj->layer != NULL){
		AUEscenaContenedor* parent = (AUEscenaContenedor*)obj->layer->contenedor();
		if(parent != NULL) parent->quitarObjetoEscena(obj->layer);
		obj->layer->liberar();
		obj->layer = NULL;
	}
	if(obj->marker != NULL) obj->marker->liberar(); obj->marker = NULL;
}

void AUSceneMap::STSceneMapWay_init(STSceneMapWay* obj, AUObjeto* parent){
	obj->id = 0;
	obj->layer = new(parent) AUEscenaContenedor();
	obj->nodesPath = new(parent) AUEscenaFigura(ENEscenaFiguraTipo_SecuenciaLineas);
	obj->layer->agregarObjetoEscena(obj->nodesPath);
}

void AUSceneMap::STSceneMapWay_release(STSceneMapWay* obj){
	obj->id = 0;
	if(obj->layer != NULL){
		AUEscenaContenedor* parent = (AUEscenaContenedor*)obj->layer->contenedor();
		if(parent != NULL) parent->quitarObjetoEscena(obj->layer);
		obj->layer->liberar();
		obj->layer = NULL;
	}
	if(obj->nodesPath != NULL) obj->nodesPath->liberar(); obj->nodesPath = NULL;
}

void AUSceneMap::STSceneMapRoute_init(STSceneMapRoute* obj, AUObjeto* parent){
	obj->id = 0;
	obj->layer = new(parent) AUEscenaContenedor();
	obj->waysIds = new(parent) AUArregloNativoMutableP<SI64>();
}

void AUSceneMap::STSceneMapRoute_release(STSceneMapRoute* obj){
	obj->id = 0;
	if(obj->layer != NULL){
		AUEscenaContenedor* parent = (AUEscenaContenedor*)obj->layer->contenedor();
		if(parent != NULL) parent->quitarObjetoEscena(obj->layer);
		obj->layer->liberar();
		obj->layer = NULL;
	}
	if(obj->waysIds != NULL) obj->waysIds->liberar(); obj->waysIds = NULL;
}

//

void AUSceneMap::puertoDeVisionModificado(const SI32 iEscena, const NBRectanguloI puertoVision){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUSceneMap::puertoDeVisionModificado")
	//Organizar
	this->privOrganizarContenido(false);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUSceneMap::organizarContenido(const bool notificarCambioAltura){
	this->privOrganizarContenido(notificarCambioAltura);
}

void AUSceneMap::privOrganizarContenido(const bool notificarCambioAltura){
	//
	_animObjects->purgarAnimaciones();
	//
	//Touch layer
	{
		const NBCajaAABB layerBox = NBGestorEscena::cajaProyeccionGrupo(_iScene, ENGestorEscenaGrupo_GUI);
		AUArreglo* childs = _layerGUITouch->hijos(); NBASSERT(childs->conteo == 2)
		((AUEscenaObjeto*)childs->elem(0))->establecerTraslacion(layerBox.xMin, layerBox.yMin);
		((AUEscenaObjeto*)childs->elem(1))->establecerTraslacion(layerBox.xMax, layerBox.yMax);
	}
}

//

bool AUSceneMap::escenaEnPrimerPlano(){
	return (_layerGUI->idEscena != -1);
}

void AUSceneMap::escenaColocarEnPrimerPlano(){
	NBColor8 color; NBCOLOR_ESTABLECER(color, 255, 255, 255, 255)
	NBGestorEscena::agregarObjetoCapa(_iScene, ENGestorEscenaGrupo_Escena, _layerScene, color);
	NBGestorEscena::agregarObjetoCapa(_iScene, ENGestorEscenaGrupo_GUI, _layerGUI, color);
	//
	NBGestorEscena::agregarEscuchadorCambioPuertoVision(_iScene, this);
	NBGestorAnimadores::agregarAnimador(this);
}

void AUSceneMap::escenaQuitarDePrimerPlano(){
	NBGestorAnimadores::quitarAnimador(this);
	NBGestorEscena::quitarEscuchadorCambioPuertoVision(_iScene, this);
	//
	NBGestorEscena::quitarObjetoCapa(_iScene, _layerGUI);
	NBGestorEscena::quitarObjetoCapa(_iScene, _layerScene);
}
//
bool AUSceneMap::escenaEstaAnimandoSalida(){
	if(_animObjects != NULL){
		return (_animObjects->conteoAnimacionesEjecutando() != 0);
	}
	return false;
}

void AUSceneMap::escenaAnimarSalida(){
	if(_animObjects == NULL){
		_animObjects = new(this) AUAnimadorObjetoEscena();
	}
	//
	//_animObjects->animarVisible(_obj, false, 0.25f, ENAnimPropVelocidad_Desacelerada);
}

void AUSceneMap::escenaAnimarEntrada(){
	if(_animObjects == NULL){
		_animObjects = new(this) AUAnimadorObjetoEscena();
	}
	//_obj->establecerVisible(false);
	//_animObjects->animarVisible(_obj, true, 0.25f, ENAnimPropVelocidad_Desacelerada);
}

bool AUSceneMap::escenaPermitidoGirarPantalla(){
	return false;
}

//
void AUSceneMap::appFocoExclusivoObtenido(){
	//
}

void AUSceneMap::appFocoExclusivoPerdido(){
	//
}

void AUSceneMap::appProcesarNotificacionLocal(const char* grp, const SI32 localId, const char* objTip, const SI32 objId){
	//
}
//
bool AUSceneMap::teclaPresionada(SI32 codigoTecla){
	return false;
}

bool AUSceneMap::teclaLevantada(SI32 codigoTecla){
	return false;
}

bool AUSceneMap::teclaEspecialPresionada(SI32 codigoTecla){
	return false;
}

//

void AUSceneMap::tickAnimacion(float segsTranscurridos){
	//
}

//TOUCHES

void AUSceneMap::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	//
}

void AUSceneMap::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	//PAN Map
	this->privValidateMapTranslation((posActualEscena.x - posAnteriorEscena.x), (posActualEscena.y - posAnteriorEscena.y));
}

void AUSceneMap::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	//
}


//

AUOBJMETODOS_CLASESID_MULTICLASE(AUSceneMap, AUAppEscena)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUSceneMap, "AUSceneMap", AUAppEscena)
AUOBJMETODOS_CLONAR_NULL(AUSceneMap)

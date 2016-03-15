
#include "AUFrameworkBaseStdAfx.h"
#include "AUOsmDatabaseAnalyzer.h"
#include "tlali-osm.h"
#include "NBGestorArchivos.h"

AUOsmDatabaseAnalyzer::AUOsmDatabaseAnalyzer() : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabaseAnalyzer::AUOsmDatabaseAnalyzer")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUOsmDatabaseAnalyzer")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUOsmDatabaseAnalyzer::~AUOsmDatabaseAnalyzer(void){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabaseAnalyzer::~AUOsmDatabaseAnalyzer")
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

SI32 AUOsmDatabaseAnalyzer::privIndexForStr(STOsmDbSortedStrs* dst, const char* str){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabaseAnalyzer::privIndexForStr")
	SI32 r = -1; STOsmDbSortedStr srch;
	const SI32 szBefore = dst->strValues->tamano();
	//Add to buffer
	dst->strValues->agregar('\0');
	dst->strValues->agregar(str);
	//Search existing
	srch.iStart		= (szBefore + 1);
	srch.size		= dst->strValues->tamano() - szBefore - 1; NBASSERT(srch.size >= 0)
	srch.buffer		= dst->strValues;
	srch.timesUsed	= 1;
	r				= dst->strPointers->indiceDe(srch);
	if(r == -1){
		//Add new string record
		r = dst->strPointers->agregarElemento(srch);
	} else {
		//Increment times used
		dst->strPointers->elemPtr(r)->timesUsed++;
		//Remove from buffer
		dst->strValues->quitarDerecha(srch.size + 1);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

SI32 AUOsmDatabaseAnalyzer::privIndexForStr(STOsmDbSortedStrs* dst, const AUCadena8* str){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabaseAnalyzer::privIndexForStr")
	SI32 r = -1; STOsmDbSortedStr srch;
	//Search existing
	srch.iStart		= 0;
	srch.size		= str->tamano();
	srch.buffer		= str;
	srch.timesUsed	= 1;
	r				= dst->strPointers->indiceDe(srch);
	if(r == -1){
		STOsmDbSortedStr data;
		data.iStart		= dst->strValues->tamano() + 1;
		data.size		= str->tamano();
		data.buffer		= dst->strValues;
		data.timesUsed	= 1;
		//Add string
		dst->strValues->agregar('\0');
		dst->strValues->agregar(str->str(), str->tamano());
		//Add new string record
		r = dst->strPointers->agregarElemento(data);
	} else {
		//Increment times used
		dst->strPointers->elemPtr(r)->timesUsed++;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

TlaSI32 AUOsmDatabaseAnalyzer_ReadFromAUFileFunc(void* dstBuffer, const TlaSI32 sizeOfBlock, const TlaSI32 maxBlocksToRead, void* userData){
	return ((AUArchivo*)userData)->leer(dstBuffer, sizeOfBlock, maxBlocksToRead, (AUArchivo*)userData);
}

TlaSI32 AUOsmDatabaseAnalyzer_WriteToAUFileFunc(const void* srcBuffer, const TlaSI32 sizeOfBlock, const TlaSI32 maxBlocksToWrite, void* userData){
	return ((AUArchivo*)userData)->escribir(srcBuffer, sizeOfBlock, maxBlocksToWrite, (AUArchivo*)userData);
}

bool AUOsmDatabaseAnalyzer::analyzePrintAllPosibleValuesPairs(const char* filePath) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabaseAnalyzer::analyzePrintAllPosibleValuesPairs")
	bool r = false;
	STTlaOsm osmFromXml;
	osmInit(&osmFromXml);
	AUArchivo* stream = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, filePath, ENArchivoModo_SoloLectura);
	if(stream == NULL){
		NBASSERT(0);
	} else {
		stream->lock();
		if(!osmLoadFromXmlStream(&osmFromXml, AUOsmDatabaseAnalyzer_ReadFromAUFileFunc, stream)){
			NBASSERT(0);
		} else {
			//Walk nodes
			PRINTF_INFO("------- NODES -------\n");
			{
				STTlaNode data; TlaSI32 count = 0;
				STOsmDbSortedStrs tagsPairs;
				AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				tagsPairs.strValues		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				tagsPairs.strPointers	= new(ENMemoriaTipo_Temporal) AUArregloNativoOrdenadoMutableP<STOsmDbSortedStr>();
				if(osmGetNextNode(&osmFromXml, &data, NULL)){
					do{
						count++;
						//Read tags
						{
							STTlaTag tag; TlaSI32 count = 0;
							if(osmGetNextNodeTag(&osmFromXml, &data, &tag, NULL)){
								do {
									count++;
									//Process tag-pair (name/value)
									{
										strTmp->vaciar();
										strTmp->agregar(tag.name);
										strTmp->agregar("::");
										strTmp->agregar(tag.value);
										AUOsmDatabaseAnalyzer::privIndexForStr(&tagsPairs, strTmp);
									}
								} while(osmGetNextNodeTag(&osmFromXml, &data, &tag, &tag));
							}
						}
					} while(osmGetNextNode(&osmFromXml, &data, &data));
				}
				//Print all tags-pairs
				{
					SI32 i; const SI32 count = tagsPairs.strPointers->conteo;
					for(i = 0; i < count; i++){
						const STOsmDbSortedStr* strDef = tagsPairs.strPointers->elemPtr(i);
						const char* str = &tagsPairs.strValues->str()[strDef->iStart];
						PRINTF_INFO("Node tag-pair: '%s' (x %d).\n", str, strDef->timesUsed);
					}
				}
				tagsPairs.strPointers->liberar();
				tagsPairs.strValues->liberar();
				strTmp->liberar();
			}
			//Walk ways
			PRINTF_INFO("------- WAYS -------\n");
			{
				STTlaWay data; TlaSI32 count = 0;
				STOsmDbSortedStrs tagsPairs;
				AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				tagsPairs.strValues		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				tagsPairs.strPointers	= new(ENMemoriaTipo_Temporal) AUArregloNativoOrdenadoMutableP<STOsmDbSortedStr>();
				if(osmGetNextWay(&osmFromXml, &data, NULL)){
					do{
						count++;
						//Read tags
						{
							STTlaTag tag; TlaSI32 count = 0;
							if(osmGetNextWayTag(&osmFromXml, &data, &tag, NULL)){
								do {
									count++;
									//Process tag-pair (name/value)
									{
										strTmp->vaciar();
										strTmp->agregar(tag.name);
										strTmp->agregar("::");
										strTmp->agregar(tag.value);
										AUOsmDatabaseAnalyzer::privIndexForStr(&tagsPairs, strTmp);
									}
								} while(osmGetNextWayTag(&osmFromXml, &data, &tag, &tag));
							}
						}
					} while(osmGetNextWay(&osmFromXml, &data, &data));
				}
				//Print all tags-pairs
				{
					SI32 i; const SI32 count = tagsPairs.strPointers->conteo;
					for(i = 0; i < count; i++){
						const STOsmDbSortedStr* strDef = tagsPairs.strPointers->elemPtr(i);
						const char* str = &tagsPairs.strValues->str()[strDef->iStart];
						PRINTF_INFO("Way tag-pair: '%s' (x %d).\n", str, strDef->timesUsed);
					}
				}
				tagsPairs.strPointers->liberar();
				tagsPairs.strValues->liberar();
				strTmp->liberar();
			}
			//Walk relations
			PRINTF_INFO("------- RELATIONS -------\n");
			{
				STTlaRel data; TlaSI32 count = 0;
				STOsmDbSortedStrs tagsPairs, typeRolesPairs;
				AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				tagsPairs.strValues		= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				tagsPairs.strPointers	= new(ENMemoriaTipo_Temporal) AUArregloNativoOrdenadoMutableP<STOsmDbSortedStr>();
				typeRolesPairs.strValues = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				typeRolesPairs.strPointers = new(ENMemoriaTipo_Temporal) AUArregloNativoOrdenadoMutableP<STOsmDbSortedStr>();
				if(osmGetNextRel(&osmFromXml, &data, NULL)){
					do{
						count++;
						//Read members
						{
							STTlaRelMember member; TlaSI32 count = 0;
							if(osmGetNextRelMember(&osmFromXml, &data, &member, NULL)){
								do {
									count++;
									//Process type-role-pair (type/role)
									{
										strTmp->vaciar();
										strTmp->agregar(member.type);
										strTmp->agregar("::");
										strTmp->agregar(member.role);
										AUOsmDatabaseAnalyzer::privIndexForStr(&typeRolesPairs, strTmp);
									}
								} while(osmGetNextRelMember(&osmFromXml, &data, &member, &member));
							}
						}
						//Read tags
						{
							STTlaTag tag; TlaSI32 count = 0;
							if(osmGetNextRelTag(&osmFromXml, &data, &tag, NULL)){
								do {
									count++;
									//Process tag-pair (name/value)
									{
										strTmp->vaciar();
										strTmp->agregar(tag.name);
										strTmp->agregar("::");
										strTmp->agregar(tag.value);
										AUOsmDatabaseAnalyzer::privIndexForStr(&tagsPairs, strTmp);
									}
								} while(osmGetNextRelTag(&osmFromXml, &data, &tag, &tag));
							}
						}
					} while(osmGetNextRel(&osmFromXml, &data, &data));
				}
				//Print all type-role-pairs
				{
					SI32 i; const SI32 count = typeRolesPairs.strPointers->conteo;
					for(i = 0; i < count; i++){
						const STOsmDbSortedStr* strDef = typeRolesPairs.strPointers->elemPtr(i);
						const char* str = &typeRolesPairs.strValues->str()[strDef->iStart];
						PRINTF_INFO("Relation type-role-pair: '%s' (x %d).\n", str, strDef->timesUsed);
					}
				}
				//Print all tags-pairs
				{
					SI32 i; const SI32 count = tagsPairs.strPointers->conteo;
					for(i = 0; i < count; i++){
						const STOsmDbSortedStr* strDef = tagsPairs.strPointers->elemPtr(i);
						const char* str = &tagsPairs.strValues->str()[strDef->iStart];
						PRINTF_INFO("Relation tag-pair: '%s' (x %d).\n", str, strDef->timesUsed);
					}
				}
				typeRolesPairs.strPointers->liberar();
				typeRolesPairs.strValues->liberar();
				tagsPairs.strPointers->liberar();
				tagsPairs.strValues->liberar();
				strTmp->liberar();
			}
			r = true;
		}
		stream->unlock();
		stream->cerrar();
	}
	osmRelease(&osmFromXml);
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

//


AUOBJMETODOS_CLASESID_UNICLASE(AUOsmDatabaseAnalyzer)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUOsmDatabaseAnalyzer, "AUOsmDatabaseAnalyzer")
AUOBJMETODOS_CLONAR_NULL(AUOsmDatabaseAnalyzer)





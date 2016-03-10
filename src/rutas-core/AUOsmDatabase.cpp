
#include "AUFrameworkBaseStdAfx.h"
#include "AUOsmDatabase.h"
#include "tlali-osm.h"

AUOsmDatabase::AUOsmDatabase() : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabase::AUOsmDatabase")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUOsmDatabase")
	//
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUOsmDatabase::~AUOsmDatabase(void){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabase::~AUOsmDatabase")
	//
	AU_GESTOR_PILA_LLAMADAS_POP
}

//


bool AUOsmDatabase::loadFromFileXml(const char* filePath){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUOsmDatabase::loadFromFileXml")
	bool r = false;
	STTlaOsm osmFromXml;
	osmInit(&osmFromXml);
	FILE* stream = fopen(filePath, "rb");
	if(stream == NULL){
		NBASSERT(0);
	} else {
		if(!osmLoadFromFileXml(&osmFromXml, stream)){
			NBASSERT(0);
		} else {
			//Walk nodes
			{
				STTlaNode data; TlaSI32 count = 0;
				if(osmGetNextNode(&osmFromXml, &data, NULL)){
					do{
						count++;
						//PRINTF_INFO("Node #%d: id(%lld) lat(%f) lon(%f).\n", count, data.id, data.lat, data.lon);
						//Read tags
						{
							STTlaTag tag; TlaSI32 count = 0;
							if(osmGetNextNodeTag(&osmFromXml, &data, &tag, NULL)){
								do {
									count++;
									//PRINTF_INFO("  Tag #%d: '%s' = '%s'.\n", count, tag.name, tag.value);
								} while(osmGetNextNodeTag(&osmFromXml, &data, &tag, &tag));
							}
						}
					} while(osmGetNextNode(&osmFromXml, &data, &data));
				}
				
			}
			//Walk ways
			{
				STTlaWay data; TlaSI32 count = 0;
				if(osmGetNextWay(&osmFromXml, &data, NULL)){
					do{
						count++;
						//PRINTF_INFO("Way #%d: id(%lld).\n", count, data.id);
						//Read nodes ref
						{
							const TlaSI64* arr; TlaSI32 count;
							if(osmGetWayNodes(&osmFromXml, &data, &arr, &count)){
								TlaSI32 i;
								for(i = 0; i < count; i++){
									//PRINTF_INFO("  Nd #%d: ref(%lld).\n", (i + 1), arr[i]);
									//Verify if node exists
									{
										STTlaNode node;
										if(!osmGetNodeById(&osmFromXml, &node, arr[i])){
											//PRINTF_INFO("  ERROR, on way(%lld), node(%lld) doesnt exists.\n", data.id, arr[i]);
										}
									}
								}
							}
						}
						//Read tags
						{
							STTlaTag tag; TlaSI32 count = 0;
							if(osmGetNextWayTag(&osmFromXml, &data, &tag, NULL)){
								do {
									count++;
									//PRINTF_INFO("  Tag #%d: '%s' = '%s'.\n", count, tag.name, tag.value);
								} while(osmGetNextWayTag(&osmFromXml, &data, &tag, &tag));
							}
						}
					} while(osmGetNextWay(&osmFromXml, &data, &data));
				}
			}
			//Walk relations
			{
				STTlaRel data; TlaSI32 count = 0;
				if(osmGetNextRel(&osmFromXml, &data, NULL)){
					do{
						count++;
						//PRINTF_INFO("Relation #%d: id(%lld).\n", count, data.id);
						//Read members
						{
							STTlaRelMember member; TlaSI32 count = 0;
							if(osmGetNextRelMember(&osmFromXml, &data, &member, NULL)){
								do {
									count++;
									//PRINTF_INFO("  Member #%d: type('%s') ref(%lld) role('%s').\n", count, member.type, member.ref, member.role);
									//Verify if member exists
									{
										TlaBOOL processed = TLA_FALSE;
										//Verify as node
										if(!processed) if(member.type[0] == 'n') if(member.type[1] == 'o') if(member.type[2] == 'd') if(member.type[3] == 'e') if(member.type[4] == '\0'){
											STTlaNode node;
											if(!osmGetNodeById(&osmFromXml, &node, member.ref)){
												//PRINTF_INFO("  ERROR, on relation(%lld), node(%lld) doesnt exists.\n", data.id, member.ref);
											}
											processed = TLA_TRUE;
										}
										//Verify as way
										if(!processed) if(member.type[0] == 'w') if(member.type[1] == 'a') if(member.type[2] == 'y') if(member.type[3] == '\0') {
											STTlaWay way;
											if(!osmGetWayById(&osmFromXml, &way, member.ref)){
												//PRINTF_INFO("  ERROR, on relation(%lld), way(%lld) doesnt exists.\n", data.id, member.ref);
											}
											processed = TLA_TRUE;
										}
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
									//PRINTF_INFO("  Tag #%d: '%s' = '%s'.\n", count, tag.name, tag.value);
								} while(osmGetNextRelTag(&osmFromXml, &data, &tag, &tag));
							}
						}
					} while(osmGetNextRel(&osmFromXml, &data, &data));
				}
			}
			r = true;
		}
		fclose(stream);
	}
	osmRelease(&osmFromXml);
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUOsmDatabase)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUOsmDatabase, "AUOsmDatabase")
AUOBJMETODOS_CLONAR_NULL(AUOsmDatabase)







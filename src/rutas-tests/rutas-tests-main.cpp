//
//  main.cpp
//  tests
//
//  Created by Marcos Ortega on 9/3/16.
//
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUFrameworkBase.h"
#include "AUOsmDatabase.h"
#include "AUOsmDatabaseAnalyzer.h"

//OSX: run at custom-path "$(SRCROOT)/../../../"

int main(int argc, const char * argv[]) {
	//
	NBGestorMemoria::inicializar(1, (1024 * 1024 * (2.2f)), 8191,	//Zonas de memoria NUCLEO/AGIL: cantidadPregeneradas, bytesMinPorZona, registrosIndicesMinimoPorZona
								 1, (1024 * 1024 * (1.1f)), 512);
	NBGestorArchivos::inicializar();
	//Analyze all posible parameters combinations on nodes/ways/relations
	/*if(!AUOsmDatabaseAnalyzer::analyzePrintAllPosibleValuesPairs("./rutasManagua.xml")){
		NBASSERT(false)
	} else {
		//
	}*/
	//Test loading "AUOsmDatabase"
	AUOsmDatabase* osmDb = new(ENMemoriaTipo_General) AUOsmDatabase();
	if(!osmDb->loadFromFileXml("./rutasManagua.xml")){
		NBASSERT(false)
	} else {
		//
	}
	osmDb->liberar();
	//
	PRINTF_INFO("End-of-program.\n");
	NBGestorArchivos::finalizar();
	NBGestorMemoria::finalizar();
    return 0;
}

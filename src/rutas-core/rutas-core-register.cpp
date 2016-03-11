//
//  GNavNucleoPrecompilado.cpp
//  GNavNucleo
//
//  Created by Nicaragua Binary on 13/03/14.
//  Copyright (c) 2014 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "rutas-core.h"
#include "rutas-core-register.h"

UI16 AUOsmDatabase::idTipoClase;
UI16 AUOsmDatabaseAnalyzer::idTipoClase;
//
#ifndef NB_METODO_INICIALIZADOR_CUERPO
#	error "Missing include. Macro NB_METODO_INICIALIZADOR_DEF is not defined yet!"
#endif

NB_METODO_INICIALIZADOR_CUERPO(rutas_core_register){
	PRINTF_INFO("+++++++++++++ rutas_core_register +++++++++++++++\n");
	NBGestorAUObjetos::registrarClase("AUOsmDatabase", &AUOsmDatabase::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUOsmDatabaseAnalyzer", &AUOsmDatabaseAnalyzer::idTipoClase);
};
  
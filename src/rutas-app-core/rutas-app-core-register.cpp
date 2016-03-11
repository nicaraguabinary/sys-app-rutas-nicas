//
//  rutas-app-core-register.cpp
//  rutas-app-core
//
//  Created by Nicaragua Binary on 11/03/2016.
//  Copyright (c) 2016 NIBSA. All rights reserved.
//

#include "rutas-app-core-pch.h"
#include "rutas-app-core.h"
#include "rutas-app-core-register.h"

UI16 AUScenesAdmin::idTipoClase;
UI16 AUSceneMap::idTipoClase;
//
#ifndef NB_METODO_INICIALIZADOR_CUERPO
#	error "Missing include. Macro NB_METODO_INICIALIZADOR_DEF is not defined yet!"
#endif

NB_METODO_INICIALIZADOR_CUERPO(rutas_app_core_register){
	PRINTF_INFO("+++++++++++++ rutas_app_core_register +++++++++++++++\n");
	NBGestorAUObjetos::registrarClase("AUScenesAdmin", &AUScenesAdmin::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUSceneMap", &AUSceneMap::idTipoClase);
};
  
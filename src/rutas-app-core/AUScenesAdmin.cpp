//
//  AUScenesAdmin.cpp
//  rutas-app-core
//
//  Created by Nicaragua Binary on 11/03/2016.
//  Copyright (c) 2016 NIBSA. All rights reserved.
//

#include "rutas-app-core-pch.h"
#include "AUScenesAdmin.h"
#include "AUSceneMap.h"

AUScenesAdmin::AUScenesAdmin(const SI32 iScene, const char* prefijoPaquetes, const char** paquetesCargar, const SI32 conteoPaquetesCargar) : AUAppEscenasAdmin(iScene, prefijoPaquetes, paquetesCargar, conteoPaquetesCargar)
	, _pilaEscenas(this)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::AUScenesAdmin")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUScenesAdmin")
	//NBASSERT(NBGestorRefranero::gestorInicializado());
	//-----------------------------------
	//SIMULAR SOLO EXISTENCIA DE PAQUETES
	//-----------------------------------
	//NBGestorArchivos::establecerSimularSoloRutasHaciaPaquetes(true);
	//
	_altoBarraSup				= 0.0f;
	_altoBarraInf				= 0.0f;
	//
	_cargarTransicionActual		= NULL;
	//
	_loadMap					= false;
	//
	//Current background
	NBColor8 color; NBCOLOR_ESTABLECER(color, 255, 255, 255, 255)
	{
		_bgLayer				= new(this) AUEscenaContenedor();
		//Touch layer
		_bgTouchLayer			= new(this) AUEscenaContenedor();
		_bgTouchLayer->establecerEscuchadorTouches(this, this);
		{
			AUEscenaSprite* touchSprite = NULL;
			touchSprite			= new(this)AUEscenaSprite(); _bgTouchLayer->agregarObjetoEscena(touchSprite); touchSprite->liberar();
			touchSprite			= new(this)AUEscenaSprite(); _bgTouchLayer->agregarObjetoEscena(touchSprite); touchSprite->liberar();
		}
		_bgLayer->agregarObjetoEscena(_bgTouchLayer);
		NBGestorEscena::agregarObjetoCapa(_iScene, ENGestorEscenaGrupo_Fondo, _bgLayer, color);
	}
	//
	NBGestorIdioma::establecerPrioridadIdioma(0, ENIdioma_ES);
	//
	_pantallaVertical			= true;
	//Load initial state
	this->loadMap();
	//
	this->privOrganize();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

AUScenesAdmin::~AUScenesAdmin(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::~AUScenesAdmin")
	//Background
	NBGestorEscena::quitarObjetoCapa(_iScene, _bgLayer);
	if(_bgTouchLayer != NULL) _bgTouchLayer->liberar(NB_RETENEDOR_THIS); _bgTouchLayer = NULL;
	if(_bgLayer != NULL) _bgLayer->liberar(NB_RETENEDOR_THIS); _bgLayer = NULL;
	//
	if(_cargarTransicionActual != NULL) _cargarTransicionActual->liberar(NB_RETENEDOR_THIS); _cargarTransicionActual = NULL;
	_pilaEscenas.vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUScenesAdmin::puertoDeVisionModificado(const SI32 iEscena, const NBRectanguloI puertoVision){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::puertoDeVisionModificado")
	this->privOrganize();
	AUAppEscenasAdmin::puertoDeVisionModificado(iEscena, puertoVision);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUScenesAdmin::privOrganize(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::privOrganize")
	//Touch layer
	{
		const NBCajaAABB layerBox = NBGestorEscena::cajaProyeccionGrupo(_iScene, ENGestorEscenaGrupo_Fondo);
		AUArreglo* childs = _bgTouchLayer->hijos(); NBASSERT(childs->conteo == 2)
		((AUEscenaObjeto*)childs->elem(0))->establecerTraslacion(layerBox.xMin, layerBox.yMin);
		((AUEscenaObjeto*)childs->elem(1))->establecerTraslacion(layerBox.xMax, layerBox.yMax);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

void AUScenesAdmin::appFocoExclusivoObtenido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::appFocoExclusivoObtenido")
	if(_pilaEscenas.conteo > 0){
		AUAppEscena* escn = (AUAppEscena*)_pilaEscenas.elem(_pilaEscenas.conteo - 1);
		escn->appFocoExclusivoObtenido();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUScenesAdmin::appFocoExclusivoPerdido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::appFocoExclusivoPerdido")
	//Guardar config
	//NBGestor::configSave();
	//Notificar a escena
	if(_pilaEscenas.conteo > 0){
		AUAppEscena* escn = (AUAppEscena*)_pilaEscenas.elem(_pilaEscenas.conteo - 1);
		escn->appFocoExclusivoPerdido();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUScenesAdmin::appProcesarNotificacionLocal(const char* grp, const SI32 localId, const char* objTip, const SI32 objId){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::procesarNotificacionLocal")
	if(_pilaEscenas.conteo > 0){
		AUAppEscena* escn = (AUAppEscena*)_pilaEscenas.elem(_pilaEscenas.conteo - 1);
		escn->appProcesarNotificacionLocal(grp, localId, objTip, objId);
		PRINTF_WARNING("Notificacion, CONSUMIENDO por '%s'.\n", escn->nombreUltimaClase());
	} else {
		PRINTF_WARNING("Notificacion, IGNORANDO, no hay escena en la pila.\n")
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

bool AUScenesAdmin::escenasTieneAccionesPendiente(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::escenasTieneAccionesPendiente")
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return ((_loadMap) && _cargarTransicionActual == NULL);
}

void AUScenesAdmin::escenasEjecutaAccionesPendientes(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::escenasEjecutaAccionesPendientes")
	if((_loadMap) && _cargarTransicionActual == NULL){
		PRINTF_INFO("Iniciando carga de escenas pendientes.\n");
		if(_cargarTipoTransicion == ENGameplayTransicionTipo_Negro){
			//-------------------------------------
			//Iniciar animacion de carga de recurso
			//-------------------------------------
			NBColor8 color; NBCOLOR_ESTABLECER(color, 255, 255, 255, 255)
			_cargarTransicionActual = new(this) AUAppTransicionConColorSolido(_iScene, this, color, ENGestorTexturaModo_cargaSegundoPlano);
			_cargarTransicionActual->tickTransicion(0.0f); //Inicializar
		} else if(_cargarTipoTransicion == ENGameplayTransicionTipo_Cortina){
			//-------------------------------------
			//Iniciar animacion de carga de recurso
			//-------------------------------------
			NBColor8 colorArriba, colorAbajo;
			NBCOLOR_ESTABLECER(colorArriba, 215, 125, 30, 255)
			NBCOLOR_ESTABLECER(colorAbajo, 242, 208, 49, 255)
			_cargarTransicionActual = new(this) AUAppTransicionConCortina(_iScene, this, ENGestorTexturaModo_cargaSegundoPlano, (_pilaEscenas.conteo > 0)/*animarEntrada*/,  colorAbajo, colorAbajo, colorArriba, colorArriba);
			_cargarTransicionActual->tickTransicion(0.0f); //Inicializar
		} else if(_cargarTipoTransicion == ENGameplayTransicionTipo_Fondo){
			//-------------------------------------
			//Iniciar animacion de carga de recurso
			//-------------------------------------
			_cargarTransicionActual = new(this) AUAppTransicionConFondo(_iScene, this, ENGestorTexturaModo_cargaSegundoPlano, _fondoRuta.str(), _fondoCapa, _fondoSprite, NULL, "Refranero/escena/fondos/objCosturaP.png");
			_cargarTransicionActual->tickTransicion(0.0f); //Inicializar
		} else if(_cargarTipoTransicion == ENGameplayTransicionTipo_Captura){
			//-------------------------------------
			//Iniciar animacion de carga de recurso
			//-------------------------------------
			_cargarTransicionActual = new(this) AUAppTransicionConCaptura(_iScene, this, ENGestorTexturaModo_cargaSegundoPlano);
			_cargarTransicionActual->tickTransicion(0.0f); //Inicializar
		} else {
			//-------------------------------------
			//Realizar inmediatamente la carga de recurso
			//-------------------------------------
			//Liberar escena actual
			this->escenasQuitarDePrimerPlano();
			this->escenasLiberar();
			//Cargar escena y precargar sus recursos
			NBGestorTexturas::modoCargaTexturasPush(ENGestorTexturaModo_cargaPendiente);
			this->escenasCargar();
			NBGestorTexturas::modoCargaTexturasPop();
			//Liberar los recursos que quedaron sin uso
			this->escenasLiberarRecursosSinUso();
			//Cargar recursos pendientes
			NBGestorTexturas::modoCargaTexturasPush(ENGestorTexturaModo_cargaSegundoPlano);
			while(NBGestorTexturas::texPendienteOrganizarConteo()!=0){ NBGestorTexturas::texPendienteOrganizarProcesar(9999); }
			NBGestorTexturas::modoCargaTexturasPop();
#			ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
			while(NBGestorSonidos::conteoBufferesPendientesDeCargar()!=0){ NBGestorSonidos::cargarBufferesPendientesDeCargar(9999); }
#			endif
			//Finalizar proceso
			NBGestorTexturas::generarMipMapsDeTexturas();
			//Colocar nueva escena en primer plano
			this->escenasColocarEnPrimerPlano();
			#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
			NBGestorMemoria::liberarZonasSinUso();
			#endif
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUScenesAdmin::escenasEnProcesoDeCarga(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::escenasEnProcesoDeCarga")
	return (_cargarTransicionActual != NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUScenesAdmin::escenasLiberarRecursosSinUso(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::escenasLiberarRecursosSinUso")
	NBGestorEscena::liberarRecursosCacheRenderEscenas();
	UI16 conteoLiberados;
	do {
		conteoLiberados = 0;
		conteoLiberados += NBGestorCuerpos::liberarPlantillasSinReferencias();
		conteoLiberados += NBGestorAnimaciones::liberarAnimacionesSinReferencias();
		conteoLiberados += NBGestorTexturas::liberarTexturasSinReferencias();
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		conteoLiberados += NBGestorSonidos::liberarBufferesSinReferencias();
#		endif
		//conteoLiberados +=  //PEDIENTE: aplicar autoliberaciones
	} while(conteoLiberados != 0);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUScenesAdmin::privFormatearVariablesCargasPendientes(){
	_loadMap			= false;
}

void AUScenesAdmin::loadMap(){
	this->privFormatearVariablesCargasPendientes();
	_loadMap				= true;
	_cargarTipoTransicion	= ENGameplayTransicionTipo_Negro;
}

//

void AUScenesAdmin::escenasColocarEnPrimerPlano(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::escenasColocarEnPrimerPlano")
	UI16 i; const UI16 conteo = _pilaEscenas.conteo;
	for(i = 0; i < conteo; i++){
		NBASSERT(_pilaEscenas.elemento[i]->esClase(AUAppEscena::idTipoClase))
		AUAppEscena* escena = (AUAppEscena*)_pilaEscenas.elemento[i];
		if((i+1) < conteo){
			escena->escenaQuitarDePrimerPlano();
		} else {
			escena->escenaColocarEnPrimerPlano();
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUScenesAdmin::escenasQuitarDePrimerPlano(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::escenasQuitarDePrimerPlano")
	UI16 i; const UI16 conteo = _pilaEscenas.conteo;
	for(i = 0; i < conteo; i++){
		NBASSERT(_pilaEscenas.elemento[i]->esClase(AUAppEscena::idTipoClase))
		AUAppEscena* escena = (AUAppEscena*)_pilaEscenas.elemento[i];
		escena->escenaQuitarDePrimerPlano();
	}
	NBGestorEscena::establecerColorFondo(_iScene, 0.0f, 0.0f, 0.0f, 1.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

bool AUScenesAdmin::escenasAnimandoSalida(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::escenasAnimandoSalida")
	bool r = false;
	if(_pilaEscenas.conteo > 0){
		NBASSERT(_pilaEscenas.elemento[_pilaEscenas.conteo - 1]->esClase(AUAppEscena::idTipoClase))
		AUAppEscena* escena = (AUAppEscena*)_pilaEscenas.elemento[_pilaEscenas.conteo - 1];
		r = (r || escena->escenaEstaAnimandoSalida());
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

void AUScenesAdmin::escenasAnimarSalida(){
	if(_pilaEscenas.conteo > 0){
		NBASSERT(_pilaEscenas.elemento[_pilaEscenas.conteo - 1]->esClase(AUAppEscena::idTipoClase))
		AUAppEscena* escena = (AUAppEscena*)_pilaEscenas.elemento[_pilaEscenas.conteo - 1];
		escena->escenaAnimarSalida();
	}
}

void AUScenesAdmin::escenasAnimarEntrada(){
	if(_pilaEscenas.conteo > 0){
		NBASSERT(_pilaEscenas.elemento[_pilaEscenas.conteo - 1]->esClase(AUAppEscena::idTipoClase))
		AUAppEscena* escena = (AUAppEscena*)_pilaEscenas.elemento[_pilaEscenas.conteo - 1];
		escena->escenaAnimarEntrada();
	}
}

void AUScenesAdmin::escenasLiberar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::escenasLiberar")
	if(_pilaEscenas.conteo > 0){
		NBASSERT(_pilaEscenas.elemento[_pilaEscenas.conteo - 1]->esClase(AUAppEscena::idTipoClase))
		AUAppEscena* escena = (AUAppEscena*)_pilaEscenas.elemento[_pilaEscenas.conteo - 1];
		NBASSERT(escena->conteoReferencias() == 1)
		escena->escenaQuitarDePrimerPlano();
		_pilaEscenas.quitarElementoEnIndice(_pilaEscenas.conteo - 1);
	}
	NBGestorEscena::establecerColorFondo(_iScene, 0.0f, 0.0f, 0.0f, 1.0f);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

void AUScenesAdmin::escenasCargar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::escenasCargar")
	CICLOS_CPU_TIPO cicloInicioCreacionEscena; CICLOS_CPU_HILO(cicloInicioCreacionEscena)
	if(_loadMap){
		AUSceneMap* newScene = new(this) AUSceneMap(_iScene, this); NB_DEFINE_NOMBRE_PUNTERO(newScene, "AUScenesAdmin::newScene")
		_pilaEscenas.agregarElemento(newScene);
		newScene->liberar(NB_RETENEDOR_THIS);
		_loadMap = false;
	}
	CICLOS_CPU_TIPO cicloFin; CICLOS_CPU_HILO(cicloFin)
	CICLOS_CPU_TIPO ciclosCpuPorSeg; CICLOS_CPU_POR_SEGUNDO(ciclosCpuPorSeg);
	PRINTF_INFO("TIEMPO DE CREACION DE ESCENA: %.2f segs\n", (float)(cicloFin-cicloInicioCreacionEscena)/(float)ciclosCpuPorSeg);
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

SI32 AUScenesAdmin::privCargarRecursoPendiente(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::privCargarRecursoPendiente")
	NBGestorTexturas::modoCargaTexturasPush(ENGestorTexturaModo_cargaSegundoPlano);
	if(NBGestorTexturas::texPendienteOrganizarConteo()!=0) NBGestorTexturas::texPendienteOrganizarProcesar(1);
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	else if(NBGestorSonidos::conteoBufferesPendientesDeCargar()!=0)		NBGestorSonidos::cargarBufferesPendientesDeCargar(1);
#	endif
	NBGestorTexturas::modoCargaTexturasPop();
	//Actualizar la presentacion
	SI32 cantidadRestante = 0;
	cantidadRestante += NBGestorTexturas::texPendienteOrganizarConteo();
#	ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
	cantidadRestante += NBGestorSonidos::conteoBufferesPendientesDeCargar();
#	endif
	//if(_cargaConteoRecursos!=0){
	//	privInlineIconosMotorMarcarProgresoCarga(((float)_cargaConteoRecursos-(float)cantidadRestante)/(float)_cargaConteoRecursos);
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return  cantidadRestante;
}

bool AUScenesAdmin::tickProcesoCarga(float segsTranscurridos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::tickProcesoCarga")
	bool r = false;
	if(_cargarTransicionActual != NULL){
		r = _cargarTransicionActual->tickTransicion(segsTranscurridos);
		if(!_cargarTransicionActual->ejecutandoTransicion()){
			_cargarTransicionActual->liberar(NB_RETENEDOR_THIS);
			_cargarTransicionActual = NULL;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return r;
}

//TECLAS

bool AUScenesAdmin::teclaPresionada(SI32 codigoTecla){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::teclaPresionada")
	bool consumida = false;
	if(_pilaEscenas.conteo > 0){
		AUAppEscena* escn = (AUAppEscena*)_pilaEscenas.elem(_pilaEscenas.conteo - 1);
		consumida = escn->teclaPresionada(codigoTecla);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return consumida;
}

bool AUScenesAdmin::teclaLevantada(SI32 codigoTecla){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::teclaLevantada")
	bool consumida = false;
	if(_pilaEscenas.conteo > 0){
		AUAppEscena* escn = (AUAppEscena*)_pilaEscenas.elem(_pilaEscenas.conteo - 1);
		consumida = escn->teclaLevantada(codigoTecla);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return consumida;
}

bool AUScenesAdmin::teclaEspecialPresionada(SI32 codigoTecla){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::teclaLevantada")
	bool consumida = false;
	if(_pilaEscenas.conteo > 0){
		AUAppEscena* escn = (AUAppEscena*)_pilaEscenas.elem(_pilaEscenas.conteo - 1);
		consumida = escn->teclaEspecialPresionada(codigoTecla);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return consumida;
}

//TOUCHES
void AUScenesAdmin::touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto){
	if(objeto == _bgTouchLayer){
		//
	}
}

void AUScenesAdmin::touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(objeto == _bgTouchLayer){
		//
	}
}

void AUScenesAdmin::touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto){
	if(objeto == _bgTouchLayer){
		//
	}
}

// EVENTOS

bool AUScenesAdmin::permitidoGirarPantalla(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::permitidoGirarPantalla")
	bool permitido = true;
	//
	AU_GESTOR_PILA_LLAMADAS_POP_3
	return permitido;
}

#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
void AUScenesAdmin::sndGrupoCambioHabilitado(const ENAudioGrupo grupoAudio, const bool nuevoHabilitado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::sndGrupoCambioHabilitado")
	//
	AU_GESTOR_PILA_LLAMADAS_POP_3
}
#endif

void AUScenesAdmin::anguloXYDispositivoCambiado(float angulo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_3("AUScenesAdmin::anguloXYDispositivoCambiado")
	//
	AU_GESTOR_PILA_LLAMADAS_POP_3
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUScenesAdmin)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUScenesAdmin, "AUScenesAdmin")
AUOBJMETODOS_CLONAR_NULL(AUScenesAdmin)







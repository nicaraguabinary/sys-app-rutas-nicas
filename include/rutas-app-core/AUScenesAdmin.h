//
//  AUScenesAdmin.h
//  rutas-app-core
//
//  Created by Nicaragua Binary on 11/03/2016.
//  Copyright (c) 2016 NIBSA. All rights reserved.
//

#ifndef AUScenesAdmin_H
#define AUScenesAdmin_H

#include "rutas-app-core-head.h"
#include "IListenerScenes.h"

enum ENGameplayTransicionTipo {
	ENGameplayTransicionTipo_Ninguna = 0,
	ENGameplayTransicionTipo_Captura,
	ENGameplayTransicionTipo_Fondo,
	ENGameplayTransicionTipo_Negro,
	ENGameplayTransicionTipo_Cortina,
	ENGameplayTransicionTipo_Refranero,
};

class AUScenesAdmin: public AUAppEscenasAdmin, public IListenerScenes
#ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
, public IEscuchadorGestionSonidos
#endif
, public IEscuchadorTouchEscenaObjeto
{
	public:
		AUScenesAdmin(const SI32 iEscena, const char* prefijoPaquetes, const char** paquetesCargar, const SI32 conteoPaquetesCargar);
		virtual ~AUScenesAdmin();
		//
		void					puertoDeVisionModificado(const SI32 iEscena, const NBRectanguloI puertoVision);
		//
		bool					escenasTieneAccionesPendiente();
		void					escenasEjecutaAccionesPendientes(); //las acciones (como carga de nivel) se acumulan como pendiente hasta que se mande a llamar a esta funcion en un modo-seguro.
		bool					tickProcesoCarga(float segsTranscurridos);
		//
		void					loadMap();
		//
		void					appFocoExclusivoObtenido();
		void					appFocoExclusivoPerdido();
		void					appProcesarNotificacionLocal(const char* grp, const SI32 localId, const char* objTip, const SI32 objId);
		//
		void					escenasCargar();
		void					escenasLiberar();
		void					escenasLiberarRecursosSinUso();
		bool					escenasEnProcesoDeCarga();
		void					escenasQuitarDePrimerPlano();
		void					escenasColocarEnPrimerPlano();
		bool					escenasAnimandoSalida();
		void					escenasAnimarSalida();
		void					escenasAnimarEntrada();
		//TECLAS
		bool					teclaPresionada(SI32 codigoTecla);
		bool					teclaLevantada(SI32 codigoTecla);
		bool					teclaEspecialPresionada(SI32 codigoTecla);
		//TOUCHES
		void					touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto);
		void					touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		void					touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		//
		bool					permitidoGirarPantalla();
#		ifndef CONFIG_NB_UNSUPPORT_AUDIO_IO
		virtual void			sndGrupoCambioHabilitado(const ENAudioGrupo grupo, const bool nuevoHabilitado);
#		endif
		void					anguloXYDispositivoCambiado(float angulo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		SI32					_iScene;
		bool					_pantallaVertical;
		//
		float					_altoBarraSup;
		float					_altoBarraInf;
		//ESCENAS
		ENGameplayTransicionTipo _cargarTipoTransicion;
		AUAppTransicion*		_cargarTransicionActual;
		bool					_loadMap;
		AUArregloMutable		_pilaEscenas; //AUAppEscena
		//Background
		AUEscenaContenedor*		_bgLayer;
		AUEscenaContenedor*		_bgTouchLayer;
		//
		void					privOrganize();
		void					privWallpaperOrganize(const SI32 index);
		//
		UI8						privCargarRecursosTransicionEscenas(SI32 indiceTransicion);
		void					privLiberarRecursosTrasicionEscenas();
		SI32					privCargarRecursoPendiente();
		inline void				privFormatearVariablesCargasPendientes();
};

#endif
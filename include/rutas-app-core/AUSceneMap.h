//
//  AUSceneMap.h
//  rutas-app-core
//
//  Created by Marcos Ortega on 11/03/2016.
//  Copyright (c) 2016 NIBSA. All rights reserved.
//

#ifndef AUSceneMap_H
#define AUSceneMap_H

#include "rutas-app-core-head.h"
#include "IListenerScenes.h"

typedef struct STSceneMapNode_ {
	SI64				id;
	AUEscenaContenedor*	layer;
	AUEscenaFigura*		marker;
	//
	bool operator==(const struct STSceneMapNode_ &other) const { return (id == other.id); }
	bool operator!=(const struct STSceneMapNode_ &other) const { return (id != other.id); }
	bool operator<(const struct STSceneMapNode_ &other) const { return (id < other.id); }
	bool operator<=(const struct STSceneMapNode_ &other) const { return (id <= other.id); }
	bool operator>(const struct STSceneMapNode_ &other) const { return (id > other.id); }
	bool operator>=(const struct STSceneMapNode_ &other) const { return (id >= other.id); }
} STSceneMapNode;

typedef struct STSceneMapWay_ {
	SI64				id;
	AUEscenaContenedor*	layer;
	AUEscenaFigura*		nodesPath;
	//
	bool operator==(const struct STSceneMapWay_ &other) const { return (id == other.id); }
	bool operator!=(const struct STSceneMapWay_ &other) const { return (id != other.id); }
	bool operator<(const struct STSceneMapWay_ &other) const { return (id < other.id); }
	bool operator<=(const struct STSceneMapWay_ &other) const { return (id <= other.id); }
	bool operator>(const struct STSceneMapWay_ &other) const { return (id > other.id); }
	bool operator>=(const struct STSceneMapWay_ &other) const { return (id >= other.id); }
} STSceneMapWay;

typedef struct STSceneMapRoute_ {
	SI64				id;
	AUEscenaContenedor*	layer;
	AUArregloNativoMutableP<SI64>* waysIds;
	//
	bool operator==(const struct STSceneMapRoute_ &other) const { return (id == other.id); }
	bool operator!=(const struct STSceneMapRoute_ &other) const { return (id != other.id); }
	bool operator<(const struct STSceneMapRoute_ &other) const { return (id < other.id); }
	bool operator<=(const struct STSceneMapRoute_ &other) const { return (id <= other.id); }
	bool operator>(const struct STSceneMapRoute_ &other) const { return (id > other.id); }
	bool operator>=(const struct STSceneMapRoute_ &other) const { return (id >= other.id); }
} STSceneMapRoute;

class AUSceneMap: public AUAppEscena, public IEscuchadorCambioPuertoVision, public IEscuchadorTouchEscenaObjeto, public NBAnimador {
	public:
		AUSceneMap(const SI32 iEscena, IListenerScenes* scenes);
		virtual				~AUSceneMap();
		//
		void				puertoDeVisionModificado(const SI32 iEscena, const NBRectanguloI puertoVision);
		//
		void				organizarContenido(const bool notificarCambioAltura);
		//
		bool				escenaEnPrimerPlano();
		void				escenaColocarEnPrimerPlano();
		void				escenaQuitarDePrimerPlano();
		//
		bool				escenaEstaAnimandoSalida();
		void				escenaAnimarSalida();
		void				escenaAnimarEntrada();
		bool				escenaPermitidoGirarPantalla();
		//
		void				appFocoExclusivoObtenido();
		void				appFocoExclusivoPerdido();
		void				appProcesarNotificacionLocal(const char* grp, const SI32 localId, const char* objTip, const SI32 objId);
		//
		bool				teclaPresionada(SI32 codigoTecla);
		bool				teclaLevantada(SI32 codigoTecla);
		bool				teclaEspecialPresionada(SI32 codigoTecla);
		//
		void				tickAnimacion(float segsTranscurridos);
		//TOUCHES
		void				touchIniciado(STGTouch* touch, const NBPunto &posTouchEscena, AUEscenaObjeto* objeto);
		void				touchMovido(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		void				touchFinalizado(STGTouch* touch, const NBPunto &posInicialEscena, const NBPunto &posAnteriorEscena, const NBPunto &posActualEscena, AUEscenaObjeto* objeto);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		SI32				_iScene;
		IListenerScenes*	_scenes;
		AUAnimadorObjetoEscena* _animObjects;
		//
		AUEscenaContenedor*	_layerScene;
		AUEscenaContenedor*	_layerGUI;
		AUEscenaContenedor*	_layerGUITouch;
		AUEscenaContenedor*	_layerMap;
		//
		AUOsmDatabase*		_routesDb;
		AUArregloNativoOrdenadoMutableP<STSceneMapRoute>*	_routesVisual;
		AUArregloNativoOrdenadoMutableP<STSceneMapWay>*		_waysVisual;
		AUArregloNativoOrdenadoMutableP<STSceneMapNode>*	_nodesVisual;
		//
		void				privOrganizarContenido(const bool notificarCambioAltura);
		void				privEmpty();
		void				privValidateMapTranslation(const float xDelta, const float yDelta);
		//
		static void			STSceneMapNode_init(STSceneMapNode* obj, AUObjeto* parent);
		static void			STSceneMapNode_release(STSceneMapNode* obj);
		static void			STSceneMapWay_init(STSceneMapWay* obj, AUObjeto* parent);
		static void			STSceneMapWay_release(STSceneMapWay* obj);
		static void			STSceneMapRoute_init(STSceneMapRoute* obj, AUObjeto* parent);
		static void			STSceneMapRoute_release(STSceneMapRoute* obj);
};

#endif

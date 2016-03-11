#ifndef AUOsmDatabase_H
#define AUOsmDatabase_H

#include "AUOsmDatabaseDefs.h"

class AUOsmDatabase : public AUObjeto {
	public:
		AUOsmDatabase(void);
		virtual ~AUOsmDatabase(void);
		//
		void	empty();
		bool	loadFromFileXml(const char* filePath);
		//
		const AUArregloNativoOrdenadoP<STOsmDbNode>*	getNodes() const;
		const AUArregloNativoOrdenadoP<STOsmDbWay>*		getWays() const;
		const AUArregloNativoOrdenadoP<STOsmDbRoute>*	getRoutes() const;
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUArregloNativoOrdenadoMutableP<STOsmDbNode>*	_nodes;
		AUArregloNativoOrdenadoMutableP<STOsmDbWay>*	_ways;
		AUArregloNativoOrdenadoMutableP<STOsmDbRoute>*	_routes;
		//
		void	privAddNode(void* osmData, void* nodeData);
		void	privAddWay(void* osmData, void* wayData);
		//
		//Factories
		//
		static void	STOsmDbNode_init(STOsmDbNode* obj, AUObjeto* parent);
		static void	STOsmDbNode_release(STOsmDbNode* obj);
		static void	STOsmDbWay_init(STOsmDbWay* obj, AUObjeto* parent);
		static void	STOsmDbWay_release(STOsmDbWay* obj);
		static void	STOsmDbRoute_init(STOsmDbRoute* obj, AUObjeto* parent);
		static void	STOsmDbRoute_release(STOsmDbRoute* obj);
		static void	STOsmDbRouteNode_init(STOsmDbRouteNode* obj, AUObjeto* parent);
		static void	STOsmDbRouteNode_release(STOsmDbRouteNode* obj);
		static void	STOsmDbRouteWay_init(STOsmDbRouteWay* obj, AUObjeto* parent);
		static void	STOsmDbRouteWay_release(STOsmDbRouteWay* obj);
};

#endif
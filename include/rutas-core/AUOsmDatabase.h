#ifndef AUOsmDatabase_H_INCLUIDO
#define AUOsmDatabase_H_INCLUIDO

#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUArregloNativoOrdenadoMutable.h"

typedef struct STOsmDbRange_ {
	SI32	start;
	SI32	size;
} STOsmDbRange;

typedef struct STOsmDbNode_ {
	SI32			id;
	double			lat;
	double			lon;
	STOsmDbRange	tags;
} STOsmDbNode;


typedef struct STOsmDbWay_ {
	SI32			id;
	STOsmDbRange	tags;
	STOsmDbRange	nodes;
} STOsmDbWay;

typedef struct STOsmDbRel_ {
	SI32	id;
} STOsmDbRel;

typedef struct STOsmDbRoot_ {
	//
} STOsmDbRoot;


class AUOsmDatabase : public AUObjeto {
	public:
		AUOsmDatabase(void);
		virtual ~AUOsmDatabase(void);
		//
		bool	loadFromFileXml(const char* filePath);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		STOsmDbRoot		_dataRoot;
};

#endif
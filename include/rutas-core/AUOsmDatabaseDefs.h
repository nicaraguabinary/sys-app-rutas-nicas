#ifndef AUOsmDatabaseDefs_H
#define AUOsmDatabaseDefs_H

//#include "AUFrameworkBase.h"
#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUArregloNativoOrdenadoMutable.h"

//---------------------------
//-- Nodes definitions
//---------------------------

//Official status
typedef enum ENOsmDbNodeOStatus_ {
	ENOsmDbNodeOStatus_noDef = 0,
	ENOsmDbNodeOStatus_busStation,	//Node tag-pair: 'official_status::IRTRAMMA:bus_station'
	ENOsmDbNodeOStatus_busStop,		//Node tag-pair: 'official_status::IRTRAMMA:bus_stop'
	ENOsmDbNodeOStatus_count
} ENOsmDbNodeOStatus;

//Node definition
typedef struct STOsmDbNode_ {
	SI64			id;
	double			lat;
	double			lon;
	//Optionals (from tags)
	AUCadena8*		name;				//Node tag-pair: 'name = ???'
	ENOsmDbNodeOStatus officialStatus;	//Node tag-pair: 'official_status = ???'
	//
	bool operator==(const struct STOsmDbNode_ &other) const { return (id == other.id); }
	bool operator!=(const struct STOsmDbNode_ &other) const { return (id != other.id); }
	bool operator<(const struct STOsmDbNode_ &other) const { return (id < other.id); }
	bool operator<=(const struct STOsmDbNode_ &other) const { return (id <= other.id); }
	bool operator>(const struct STOsmDbNode_ &other) const { return (id > other.id); }
	bool operator>=(const struct STOsmDbNode_ &other) const { return (id >= other.id); }
} STOsmDbNode;

//---------------------------
//-- Ways definitions
//---------------------------

//Highway
typedef enum ENOsmDbHightway_ {
	ENOsmDbHightway_noDef = 0,
	ENOsmDbHightway_construction,	//Way tag-pair: 'highway::construction'
	ENOsmDbHightway_livingStreet,	//Way tag-pair: 'highway::living_street'
	ENOsmDbHightway_primary,		//Way tag-pair: 'highway::primary'
	ENOsmDbHightway_primaryLink,	//Way tag-pair: 'highway::primary_link'
	ENOsmDbHightway_residential,	//Way tag-pair: 'highway::residential'
	ENOsmDbHightway_secondary,		//Way tag-pair: 'highway::secondary'
	ENOsmDbHightway_secondaryLink,	//Way tag-pair: 'highway::secondary_link'
	ENOsmDbHightway_service,		//Way tag-pair: 'highway::service'
	ENOsmDbHightway_tertiary,		//Way tag-pair: 'highway::tertiary'
	ENOsmDbHightway_tertiaryLink,	//Way tag-pair: 'highway::tertiary_link'
	ENOsmDbHightway_trunk,			//Way tag-pair: 'highway::trunk'
	ENOsmDbHightway_trunkLink,		//Way tag-pair: 'highway::trunk_link'
	ENOsmDbHightway_unclassified,	//Way tag-pair: 'highway::unclassified
	ENOsmDbHightway_count
} ENOsmDbHightway;

//Oneway
typedef enum ENOsmDbIsOneWay_ {
	ENOsmDbIsOneWay_noDef = 0,
	ENOsmDbIsOneWay_yes,			//Way tag-pair: 'oneway::yes'
	ENOsmDbIsOneWay_no,				//Way tag-pair: 'oneway::no'
} ENOsmDbIsOneWay;

//Surface
typedef enum ENOsmDbSurface_ {
	ENOsmDbSurface_noDef = 0,
	ENOsmDbSurface_asphalt,			//Way tag-pair: 'surface::asphalt'
	ENOsmDbSurface_cobblestone,		//Way tag-pair: 'surface::cobblestone'
	ENOsmDbSurface_concrete,		//Way tag-pair: 'surface::concrete'
	ENOsmDbSurface_dirt,			//Way tag-pair: 'surface::dirt'
	ENOsmDbSurface_earth,			//Way tag-pair: 'surface::earth'
	ENOsmDbSurface_paved,			//Way tag-pair: 'surface::paved'
	ENOsmDbSurface_paving_stones,	//Way tag-pair: 'surface::paving_stones'
	ENOsmDbSurface_unpaved,			//Way tag-pair: 'surface::unpaved'
	ENOsmDbSurface_count
} ENOsmDbSurface;

//Way definition
typedef struct STOsmDbWay_ {
	SI64				id;
	AUArregloNativoMutableP<SI64>* nodes;
	//Optionals (from tags)
	AUCadena8*			name;			//Way tag-pair: 'name = str'
	AUCadena8*			altName;		//Way tag-pair: 'alt_name = str', 'alt_name_1 = str', or 'alt_name_2 = str'
	AUCadena8*			locName;		//Way tag-pair: 'loc_name = str'
	AUCadena8*			oldName;		//Way tag-pair: 'old_name = str'
	AUCadena8*			shortName;		//Way tag-pair: 'short_name = str'
	AUCadena8*			ref;			//Way tag-pair: 'ref = str'
	ENOsmDbHightway		highwayType;	//Way tag-pair: 'highway = str'
	SI32				lanesCount;		//Way tag-pair: 'lanes = #'
	SI32				layer;			//Way tag-pair: 'layer = #'
	SI32				maxSpeed;		//Way tag-pair: 'maxspeed = #'
	ENOsmDbIsOneWay		isOneWay;		//Way tag-pair: 'oneway = yes/no'
	ENOsmDbSurface		surfaceType;	//Way tag-pair: 'surface = str'
	//
	bool operator==(const struct STOsmDbWay_ &other) const { return (id == other.id); }
	bool operator!=(const struct STOsmDbWay_ &other) const { return (id != other.id); }
	bool operator<(const struct STOsmDbWay_ &other) const { return (id < other.id); }
	bool operator<=(const struct STOsmDbWay_ &other) const { return (id <= other.id); }
	bool operator>(const struct STOsmDbWay_ &other) const { return (id > other.id); }
	bool operator>=(const struct STOsmDbWay_ &other) const { return (id >= other.id); }
} STOsmDbWay;

//---------------------------
//-- Routes definitions
//-- (relations where 'type = route')
//---------------------------

//Route node type
typedef enum ENOsmDbMemNodeType_ {
	ENOsmDbMemNodeType_noDef = 0,
	ENOsmDbMemNodeType_platform,	//Relation member: 'type = node && role = platform'
	ENOsmDbMemNodeType_stop,		//Relation member: 'type = node && role = stop'
	ENOsmDbMemNodeType_count
} ENOsmDbMemNodeType;

//Route node
typedef struct STOsmDbRouteNode_ {
	SI64				id;
	ENOsmDbMemNodeType	type;
} STOsmDbRouteNode;

//Route way type
typedef enum ENOsmDbMemWayType_ {
	ENOsmDbMemWayType_noDef = 0,
	ENOsmDbMemWayType_backward,		//Relation member: 'type = way && role = backward'
	ENOsmDbMemWayType_count
} ENOsmDbMemWayType;

//Route way
typedef struct STOsmDbRouteWay_ {
	SI64				id;
	ENOsmDbMemWayType	type;
} STOsmDbRouteWay;
	
//Relations where tag-pair: 'type = route'
typedef struct STOsmDbRoute_ {
	SI64	id;
	//Members
	AUArregloNativoMutableP<STOsmDbRouteNode>* nodes;
	AUArregloNativoMutableP<STOsmDbRouteWay>* ways;
	//Optionals (from tags)
	AUCadena8*			network;		//Relation tag-pair: 'network = str'
	AUCadena8*			refName;		//Relation tag-pair: 'ref = str'
	AUCadena8*			name;			//Relation tag-pair: 'name = str'
	AUCadena8*			from;			//Relation tag-pair: 'from = str'
	AUCadena8*			to;				//Relation tag-pair: 'to = str'
	AUCadena8*			operatr;		//Relation tag-pair: 'operator = str'
	//
	bool operator==(const struct STOsmDbRoute_ &other) const { return (id == other.id); }
	bool operator!=(const struct STOsmDbRoute_ &other) const { return (id != other.id); }
	bool operator<(const struct STOsmDbRoute_ &other) const { return (id < other.id); }
	bool operator<=(const struct STOsmDbRoute_ &other) const { return (id <= other.id); }
	bool operator>(const struct STOsmDbRoute_ &other) const { return (id > other.id); }
	bool operator>=(const struct STOsmDbRoute_ &other) const { return (id >= other.id); }
} STOsmDbRoute;


#endif
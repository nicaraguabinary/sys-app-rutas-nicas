#ifndef AUOsmDatabaseAnalyzer_H_INCLUIDO
#define AUOsmDatabaseAnalyzer_H_INCLUIDO

#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUArregloNativoOrdenadoMutable.h"

typedef struct STOsmDbSortedStr_ {
	SI32				iStart;	//Start of the string on the buffer.
	SI32				size;	//Size of the string (excluding the '\0').
	const AUCadena8*	buffer;	//Pointer to buffer (for comparing with other strings).
	SI32				timesUsed;	//Times appears
	//
	bool operator==(const struct STOsmDbSortedStr_ &other) const {
		SI32 i; const char* str; const char* strOther;
		if(size != other.size) return false; //Quick compare (by sizes)
		i = 0; str = &buffer->str()[iStart]; strOther = &other.buffer->str()[other.iStart];
		while(i < size){ if(str[i] != strOther[i]) return false; i++; }
		return true;
	}
	bool operator!=(const struct STOsmDbSortedStr_ &other) const {
		SI32 i; const char* str; const char* strOther;
		if(size != other.size) return true; //Quick compare (by sizes)
		i = 0; str = &buffer->str()[iStart]; strOther = &other.buffer->str()[other.iStart];
		while(i < size){ if(str[i] != strOther[i]) return true; i++; }
		return false;
	}
	bool operator<(const struct STOsmDbSortedStr_ &other) const {
		SI32 i; const char* str; const char* strOther;
		i = 0; str = &buffer->str()[iStart]; strOther = &other.buffer->str()[other.iStart];
		while(i < size && i < other.size){ if(str[i] < strOther[i]) return true; else if(str[i] > strOther[i]) return false; i++; }
		return (size < other.size);
	}
	bool operator<=(const struct STOsmDbSortedStr_ &other) const {
		SI32 i; const char* str; const char* strOther;
		i = 0; str = &buffer->str()[iStart]; strOther = &other.buffer->str()[other.iStart];
		while(i < size && i < other.size){ if(str[i] < strOther[i]) return true; else if(str[i] > strOther[i]) return false; i++; }
		return (size <= other.size);
	}
	bool operator>(const struct STOsmDbSortedStr_ &other) const {
		SI32 i; const char* str; const char* strOther;
		i = 0; str = &buffer->str()[iStart]; strOther = &other.buffer->str()[other.iStart];
		while(i < size && i < other.size){ if(str[i] > strOther[i]) return true; else if(str[i] < strOther[i]) return false; i++; }
		return (size > other.size);
	}
	bool operator>=(const struct STOsmDbSortedStr_ &other) const {
		SI32 i; const char* str; const char* strOther;
		i = 0; str = &buffer->str()[iStart]; strOther = &other.buffer->str()[other.iStart];
		while(i < size && i < other.size){ if(str[i] > strOther[i]) return true; else if(str[i] < strOther[i]) return false; i++; }
		return (size >= other.size);
	}
} STOsmDbSortedStr;

typedef struct STOsmDbSortedStrs_ {
	AUCadenaMutable8* strValues;
	AUArregloNativoOrdenadoMutableP<STOsmDbSortedStr>* strPointers;
} STOsmDbSortedStrs;


class AUOsmDatabaseAnalyzer : public AUObjeto {
	public:
		AUOsmDatabaseAnalyzer(void);
		virtual ~AUOsmDatabaseAnalyzer(void);
		//
		static bool	analyzePrintAllPosibleValuesPairs(const char* filePath);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		static SI32		privIndexForStr(STOsmDbSortedStrs* dst, const char* str);
		static SI32		privIndexForStr(STOsmDbSortedStrs* dst, const AUCadena8* str);
};

#endif
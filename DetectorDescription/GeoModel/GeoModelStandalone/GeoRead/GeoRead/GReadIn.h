/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * GeoModelReadIn.h
 *
 *  Created on: May 20, 2016
 *      Author: Riccardo Maria BIANCHI <rbianchi@cern.ch>
 */

#ifndef GEOMODELREADIN_H_
#define GEOMODELREADIN_H_

#include "GeoModelDBManager/GMDBManager.h"

#include "GeoModelKernel/GeoXF.h"


// C++ includes
#include <string>
#include <set>


// FWD declarations
class GeoVPhysVol;
class GeoPhysVol;
class GeoLogVol;
class GeoShape;
class GeoMaterial;
class GeoNameTag;
class GeoSerialDenominator;
class GeoAlignableTransform;
class GeoTransform;
class GeoSerialTransformer;
class GeoGraphNode;
//class GeoXF;
//class GeoXF::Function;

using namespace GeoGenfun;
using namespace GeoXF;

typedef const Function & TRANSFUNCTION;


namespace GeoModelPers {

class GReadIn {
public:

	GReadIn(GMDBManager* db, unsigned long* progress = nullptr);
	virtual ~GReadIn();

	// void printRecords();
	GeoPhysVol* buildGeoModel();

private:

//	static int dbCallbackPrint(void *NotUsed, int nColRow, char **fieldsRow, char **colNamesRow);
//	static int dbCallbackBuild(void *NotUsed, int nColRow, char **fieldsRow, char **colNamesRow);

	GeoPhysVol* buildGeoModelByCalls();
	GeoPhysVol* buildGeoModelOneGo();

	GeoPhysVol* loopOverAllChildren();

	GeoPhysVol* getRootVolume();

	GeoVPhysVol* parseChildren(GeoVPhysVol* vol, QMap<unsigned int, QStringList> children, int depth = 0);
	GeoVPhysVol* parseVPhysVol(QStringList values, QString nodeType, int depth = 0);
	GeoVPhysVol* buildVPhysVol(QString id, QString tableId, QString copyNumber);
	//GeoLogVol* parseLogVol(QStringList values);
	GeoLogVol* buildLogVol(QString id);
	//GeoShape* parseShape(QStringList values);
	GeoShape* buildShape(QString id);
	//GeoMaterial* parseMaterial(QStringList values);
	GeoMaterial* buildMaterial(QString id);
	GeoSerialDenominator* parseSerialDenominator(QStringList values);
	GeoSerialDenominator* buildSerialDenominator(QString id);
	GeoAlignableTransform* parseAlignableTransform(QStringList values);
	GeoAlignableTransform* buildAlignableTransform(QString id);
	GeoTransform* parseTransform(QStringList values);
	GeoTransform* buildTransform(QString id);
	GeoSerialTransformer* parseSerialTransformer(QStringList values);
	GeoSerialTransformer* buildSerialTransformer(QString id);
	// TRANSFUNCTION parseFunction(QStringList values);
	// TRANSFUNCTION parseFunction(const QStringList& values);
	// TRANSFUNCTION parseFunction(const int& id, const std::string& expr);
	TRANSFUNCTION parseFunction(const std::string& expr);
	TRANSFUNCTION buildFunction(QString id);
	GeoNameTag* parseNameTag(QStringList values);
	GeoNameTag* buildNameTag(QString id);


	GeoShape* getShape(QString id);

	void volAddHelper(GeoVPhysVol* vol, GeoGraphNode* volChild);

	bool isNodeBuilt(const QString id, const QString tableId, const QString copyNumber);
	GeoGraphNode* getNode(const QString id, const QString tableId, const QString copyNumber);
	void storeNode(const QString id, const QString tableId, const QString copyNumber, GeoGraphNode* node);

	void checkInputString(QString input);



	// input arguments
	GeoPhysVol* m_root;
	GMDBManager* m_dbManager;
	unsigned long* m_progress;
	bool m_deepDebug;


	// SQLite variables
//	sqlite3* _db;
//	char * _zErrMsg;
//	int _rc;
//	char const * _sql;
//	const char* _data;

	// data containers

	QHash<QString, QMap<unsigned int, QStringList>> m_allchildren; // key = "parentId:parentTable", item = list of children parameters, inserted by child position

	QHash<unsigned int, QStringList> m_physVols;
	QHash<unsigned int, QStringList> m_fullPhysVols;
	QHash<unsigned int, QStringList> m_logVols;
	QHash<unsigned int, QStringList> m_shapes;
	QHash<unsigned int, QStringList> m_materials;
	QHash<unsigned int, QStringList> m_transforms;
	QHash<unsigned int, QStringList> m_alignableTransforms;
	QHash<unsigned int, QStringList> m_serialDenominators;
	QHash<unsigned int, QStringList> m_serialTransformers;
	QHash<unsigned int, QStringList> m_functions;
	QHash<unsigned int, QStringList> m_nameTags;

	QHash<unsigned int, QString> m_tableid_tableName;

	QStringList m_root_vol_data;

	QHash<QString, GeoGraphNode*> m_memMap;

	std::set<std::string> m_unknown_shapes;

};

} /* namespace GeoModelPers */
#endif /* GEOMODELREADIN_H_ */

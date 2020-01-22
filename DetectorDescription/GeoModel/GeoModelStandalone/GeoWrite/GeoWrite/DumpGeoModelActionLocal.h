/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// author: Riccardo.Maria.Bianchi@cern.ch
// major updates:
// - Aug 2018
// - Sep 2018

#ifndef DumpGeoModelActionLocalLocal_H
#define DumpGeoModelActionLocalLocal_H

// local includes
#include "GeoModelDBManager/GMDBManager.h"

// GeoModel includes
#include "GeoModelKernel/GeoNodeAction.h"
#include "GeoModelKernel/GeoGraphNode.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoXF.h"
#include "GeoModelKernel/GeoAlignableTransform.h"

// Qt includes
//#include <QSqlDatabase>
#include <QStringList>
#include <QVariant>
#include <QString>
#include <QMap>

// TODO: to remove once the migration to Eigen is complete
// fwd declaration
namespace HepGeom {
	class Transform3D;
}


/**
 * \class DumpGeoModelActionLocal
 *
 * DumpGeoModelActionLocal acts on objects of the GeoModel tree
 * persitifying them offline.
 */
class DumpGeoModelActionLocal  : public GeoNodeAction
{
public:
	/**
	 * @brief Constructor
	 *
	 * Constructor sets up connection with db and opens it
	 * @param path - absolute path to db file
	 */
	DumpGeoModelActionLocal(GMDBManager& db);

	/**
	 * @brief Destructor
	 */
	~DumpGeoModelActionLocal();

	virtual void handlePhysVol (const GeoPhysVol *vol); //	Handles a physical volume.
	virtual void handleFullPhysVol (const GeoFullPhysVol *vol);
	virtual void handleSerialDenominator (const GeoSerialDenominator *sD); //	Handles a Serial Denominator.
	virtual void handleSerialTransformer (const GeoSerialTransformer *obj);
	virtual void handleTransform (const GeoTransform *);
	virtual void handleNameTag (const GeoNameTag *);

	void saveToDB();


private:

	// define copy constructor, needed for the GeoModelAction subclass
	DumpGeoModelActionLocal(const DumpGeoModelActionLocal &right);

	// define assignment operator, needed for the GeoModelAction subclass
	DumpGeoModelActionLocal & operator=(const DumpGeoModelActionLocal &right);

	void handleVPhysVolObjects(const GeoVPhysVol* vol);
	void handleReferencedVPhysVol (const GeoVPhysVol *vol); // Handles a physical volume referenced by a SerialTrasformer

	void showMemoryMap();

	QStringList getParentNode();

	QVariant storeShape(const GeoShape* shape);
	QVariant storeMaterial(const GeoMaterial* mat);
	QVariant storeElement(const GeoElement* el);
	QVariant storeTranform(const GeoTransform* node);

	QVariant storeObj(const GeoMaterial* pointer, const QString name, const QString density, const QString elements);
	QVariant storeObj(const GeoElement* pointer, const QString name, const QString symbol, const QString elZ, const QString elA);
	QVariant storeObj(const GeoShape* pointer, const QString type, const QString parameters);
	QVariant storeObj(const GeoLogVol* pointer, const QString name, const QVariant shapeId, const QVariant materialId);
	QVariant storeObj(const GeoPhysVol* pointer, const QVariant logvolId, const QVariant parentId = QVariant(), bool isRootVolume = false );
	QVariant storeObj(const GeoFullPhysVol* pointer, const QVariant logvolId, const QVariant parentId = QVariant(), bool isRootVolume = false );
	QVariant storeObj(const GeoSerialDenominator* pointer, const QString baseName);
	QVariant storeObj(const GeoSerialTransformer* pointer, const QVariant functionId, const QVariant volId, const QString volType, const unsigned int copies);
	QVariant storeObj(const GeoXF::Function* pointer, const QString expression);
	QVariant storeObj(const GeoTransform* pointer, const std::vector<double> parameters);
	QVariant storeObj(const GeoAlignableTransform* pointer, const std::vector<double> parameters);
	QVariant storeObj(const GeoNameTag* pointer, const QString name);

	unsigned int addRecord(std::vector<QStringList>* container, const QStringList values) const;
	QVariant addMaterial(const QString name, const QString density, const QString elements);
	QVariant addElement(const QString name, const QString symbol, const QString elZ, const QString elA);
	QVariant addNameTag(const QString name);
	QVariant addAlignableTransform(const std::vector<double> params);
	QVariant addTransform(const std::vector<double> params);
	QVariant addFunction(const QString expression);
	QVariant addSerialTransformer(const QVariant &funcId, const QVariant &physvolId, const QString volType, const unsigned int &copies);
	QVariant addShape(const QString &type, const QString &parameters);
	QVariant addSerialDenominator(const QString &baseName);
	QVariant addPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume = false);
	QVariant addFullPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume = false);
	QVariant addLogVol(const QString &name, const QVariant &shapeId, const QVariant &materialId);
    void addChildPosition(const QVariant parentId, const QString parentType, const QVariant childId, const unsigned int parentCopyNumber, const unsigned int childPos, const QString childType, const unsigned int childCopyN);

    unsigned int getChildPosition(QString parentId, QString parentType, unsigned int copyN);

    unsigned int setVolumeCopyNumber(QString volId, QString volType);
    unsigned int getLatestParentCopyNumber(QString parentId, QString parentType);

	void storeChildPosition(const QVariant parentId, const QString parentType, const QVariant childVol, const unsigned int parentCopyNumber, const unsigned int childPos, const QString childType, const unsigned int childCopyN);

	bool isAddressStored(const QString address);
	void storeAddress(const QString address, QVariant id);

	QVariant getStoredIdFromAddress(QString address);

	QString getAddressStringFromPointer(const GeoMaterial* pointer);
	QString getAddressStringFromPointer(const GeoElement* pointer);
	QString getAddressStringFromPointer(const GeoShape* pointer);
	QString getAddressStringFromPointer(const GeoLogVol* pointer);
	QString getAddressStringFromPointer(const GeoPhysVol* pointer);
	QString getAddressStringFromPointer(const GeoVPhysVol* pointer);
	QString getAddressStringFromPointer(const GeoSerialDenominator* pointer);
	QString getAddressStringFromPointer(const GeoSerialTransformer* pointer);
	QString getAddressStringFromPointer(const GeoXF::Function* pointer);
	QString getAddressStringFromPointer(const GeoTransform* pointer);
	QString getAddressStringFromPointer(const GeoNameTag* pointer);

	QString getQStringFromOss(std::ostringstream &oss);

	std::vector<double> getTransformParameters(HepGeom::Transform3D); // TODO: to be moved to Eigen (GeoTrf) and to be moved to an Utility class, so we can use it from TransFunctionRecorder as well.
	QString getShapeParameters(const GeoShape*);

	QString getGeoTypeFromVPhysVol(const GeoVPhysVol* vol);

	QString getIdFromNodeType(QString nodeType);

    QString m_dbpath;
	GMDBManager* m_dbManager;

	QMap<QString, QVariant> m_memMap; // TODO: maybe move to QHash??
	QHash<QString, unsigned int> m_memMap_Tables;
    QMap<QString, unsigned int> m_parentChildrenMap2; // TODO: clean name!
    QMap<QString, unsigned int> m_volumeCopiesMap;

	// keep track of the number of visited tree nodes
	unsigned int m_len;
	unsigned int m_len_nChild;

	bool m_rootVolumeFound;
	bool m_unconnectedTree;

	std::vector<QStringList> m_logVols;
	std::vector<QStringList> m_physVols;
	std::vector<QStringList> m_fullPhysVols;
	std::vector<QStringList> m_shapes;
	std::vector<QStringList> m_materials;
	std::vector<QStringList> m_elements;
	std::vector<QStringList> m_transforms;
	std::vector<QStringList> m_alignableTransforms;
	std::vector<QStringList> m_serialDenominators;
	std::vector<QStringList> m_serialTransformers;
	std::vector<QStringList> m_functions;
	std::vector<QStringList> m_nameTags;

	std::vector<QStringList> m_childrenPositions;
	QStringList m_rootVolume;

};

#endif // DumpGeoModelActionLocal_H

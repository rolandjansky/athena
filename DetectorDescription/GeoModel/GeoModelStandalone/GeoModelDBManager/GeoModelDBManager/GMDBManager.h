/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


// author: Riccardo.Maria.Bianchi@cern.ch
// major updates: Aug 2018

#ifndef GMDBManager_H
#define GMDBManager_H


// include Qt
#include <QSqlDatabase>
#include <QStringList>
#include <QVariant>
#include <QString>
#include <QMap>

// include C++
#include <iostream>

/**
 * \class GMDBManager
 *
 * \brief SQL Database Manager class for GeoModel
 *
 * GMDBManager sets up the connection with SQL database
 * and performs some basics queries.
 */
class GMDBManager
{
public:
	/**
	 * @brief Constructor
	 *
	 * Constructor sets up connection with db and opens it
	 * @param path - absolute path to db file
	 */
	GMDBManager(const QString& path);

	/**
	 * @brief Destructor
	 *
	 * Close the db connection
	 */
	~GMDBManager();


	bool isOpen() const;

	/**
	 * @brief Print names of all GeoPhysVol objects in db
	 */
	void printAllPhysVols() const;

	/**
	 * @brief Print names of all GeoFullPhysVol objects in db
	 */
	void printAllFullPhysVols() const;

	/**
	 * @brief Print names of all GeoLogVol objects in db
	 */
	void printAllLogVols() const;

	/**
	 * @brief Print names of all GeoMaterial objects in db
	 */
	void printAllMaterials() const;

	/**
	 * @brief Print names of all GeoElement objects in db
	 */
	void printAllElements() const;

	/**
	 * @brief Print names of all PhysVol objects in db
	 */
	void printAllShapes() const;

	/**
	 * @brief Print names of all GeoSerialDenominator objects in db
	 */
	void printAllSerialDenominators() const;


	void printAllSerialTransformers() const;
	void printAllFunctions() const;
	void printAllTransforms() const;
	void printAllAlignableTransforms() const;
	void printAllNameTags() const;

	/**
	 * @brief Print the db table storing all the children positions per parent
	 */
	void printAllChildrenPositions() const;

	/**
	 * @brief Print the db table storing all the children positions per parent
	 */
	void printAllNodeTypes() const;

	/**
	 * @brief Print the DB table storing the ID of the PhysVol that is the Root volume
	 */
	void printRootVolumeId() const;

	void printDBVersion() const;

	// ADD methods
	QVariant addShape(const QString &type, const QString &parameters);
	QVariant addMaterial(const QString &name, const QString &density, const QString &elements);
	QVariant addElement(const QString &name, const QString &symbol, const QString &elZ, const QString &elA);
	QVariant addLogVol(const QString &name, const QVariant &shapeId, const QVariant &materialId);
	QVariant addPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume = false);
	QVariant addFullPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume = false);
	QVariant addSerialDenominator(const QString &baseName);
	QVariant addFunction(const QString expression);
	QVariant addSerialTransformer(const QVariant &funcId, const QVariant &physvolId, const QString volType, const unsigned int &copies);
	QVariant addTransform(QVector<double> parameters);
	QVariant addAlignableTransform(QVector<double> parameters);
	QVariant addNameTag(const QString &name);


	bool addListOfRecords(const QString geoType, const std::vector<QStringList> records);
	bool addListOfRecordsToTable(const QString tableName, const std::vector<QStringList> records);
	bool addListOfRecordsToTableOld(const QString tableName, const std::vector<QStringList> records);
	bool addListOfChildrenPositions(const std::vector<QStringList> records);

	bool addRootVolume(const QStringList);
	void addChildPosition(const QVariant parentId, const QString parentType, const QVariant childId, const unsigned int parentCopyNumber, const unsigned int childPos, const QString childType, const unsigned int childCopyN);

	void addDBversion(const QString);

	// GET methods

    QString getDBFilePath();

	QStringList getRootPhysVol();

	QStringList getItem(QString geoType, unsigned int id);
	QStringList getItem(unsigned int tableId, unsigned int id);
	QStringList getItemFromTableName(QString tableName, unsigned int id);
	QStringList getItemAndType(unsigned int tableId, unsigned int id);

	QString getNodeTypeFromTableId(unsigned int id);

	QMap<unsigned int, QStringList> getVPhysVolChildren(const unsigned int id, const QString nodeType, const unsigned int copyN);

	/// methods to dump the DB
	QHash<QString, QMap<unsigned int, QStringList>> getChildrenTable();
	QHash<unsigned int, QStringList> getTableFromNodeType(QString nodeType);
	QHash<unsigned int, QString> getAll_TableIDsNodeTypes();
	QHash<QString, unsigned int> getAll_NodeTypesTableIDs();


private:

	bool initDB();
	bool createTables();

	void loadTestData(); // for debug only

	void loadTableNamesFromDB();
	QStringList getTableColNamesFromDB(QString tableName) const;

	QString getTableNameFromTableId(unsigned int tabId);
	QVariant getTableIdFromNodeType(QString nodeType);
	void storeNodeType(QString nodeType, QString tableName);
	QString getTableNameFromNodeType(QString nodeType);

	QSqlQuery selectAllFromTable(QString tableName) const;


	void storeTableColumnNames(QStringList input);
	QStringList getTableColumnNames(QString tableName);
	void printTableColNamesFromDB(QString tableName) const;

	void printAllRecords(QString tableName) const;

	int getTableColIndex(QString tableName, QString colName);

	bool storeRootVolume(QVariant id, QString nodeType);

	void showError(const QSqlError &err) const;

    QString m_dbpath;
	QSqlDatabase m_db;
	bool m_dbIsOK;

	bool m_deepDebug;

	QMap<QString, QStringList> m_tableNames;
	QMap<QString, QString> m_childType_tableName;
};

#endif // GMDBManager_H

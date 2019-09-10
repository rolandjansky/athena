/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// author: Riccardo.Maria.Bianchi@cern.ch
// major updates: Aug 2018

#include <GeoModelDBManager/GMDBManager.h>

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QDebug>


static std::string dbversion = "0.2.0";


GMDBManager::GMDBManager(const QString &path) : m_dbpath(path), m_dbIsOK(false),  m_deepDebug(false)
{
	qDebug() << "GMDBManager: constructor";

	#ifdef GEOREAD_DEEP_DEBUG
	  m_deepDebug = true;
 	#endif


	m_dbIsOK = true;

	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName(path);

	if (!m_db.open())
	{
		qDebug() << "Error: connection with database failed!";
		showError( m_db.lastError() );
		m_dbIsOK = false;
	}
	else
	{
		qDebug() << "Database: connection ok";
		m_dbIsOK = true;
	}


	// check if DB has tables, if not create them
	if (m_dbIsOK) {
		if ( ! (initDB()) ) {
			std::cout << "Error: database initialization failed" << std::endl;
			m_dbIsOK = false;
		}
	}

	// populate DB with fake data, only for debug
	//loadTestData();
}

GMDBManager::~GMDBManager()
{
	qDebug() << "GMDBManager: destructor";
	if (m_db.isOpen())
	{
		m_db.close();
	}
}

bool GMDBManager::isOpen() const
{
	if (! m_dbIsOK )
		return false;
	return m_db.isOpen();
}


void GMDBManager::printAllMaterials() const
{
	qDebug() << "printAllMaterials()";
	printAllRecords("Materials");
}
void GMDBManager::printAllShapes() const
{
	qDebug() << "printAllShapes()";
	printAllRecords("Shapes");
}

void GMDBManager::printAllSerialDenominators() const
{
	qDebug() << "printAllShapes()";
	printAllRecords("SerialDenominators");
}

void GMDBManager::printAllLogVols() const
{
	qDebug() << "printAllLogVols()";
	printAllRecords("LogVols");
}
void GMDBManager::printAllPhysVols() const
{
	qDebug() << "printAllPhysVols()";
	printAllRecords("PhysVols");
}
void GMDBManager::printAllFullPhysVols() const
{
	qDebug() << "printAllFullPhysVols()";
	printAllRecords("FullPhysVols");
}
void GMDBManager::printAllSerialTransformers() const
{
	qDebug() << "printAllSerialTransformers()";
	printAllRecords("SerialTransformers");
}
void GMDBManager::printAllFunctions() const
{
	qDebug() << "printAllFunctions()";
	printAllRecords("Functions");
}
void GMDBManager::printAllTransforms() const
{
	qDebug() << "printAllTransforms()";
	printAllRecords("Transforms");
}
void GMDBManager::printAllAlignableTransforms() const
{
	qDebug() << "printAllAlignableTransforms()";
	printAllRecords("AlignableTransforms");
}
void GMDBManager::printAllNameTags() const
{
	qDebug() << "printAllNameTags()";
	printAllRecords("NameTags");
}
void GMDBManager::printAllChildrenPositions() const
{
	qDebug() << "printAllChildrenPositions()";
	printAllRecords("ChildrenPositions");
}
void GMDBManager::printAllNodeTypes() const
{
	qDebug() << "printAllNodeTypes()";
	printAllRecords("GeoNodesTypes");
}
void GMDBManager::printRootVolumeId() const
{
	qDebug() << "printRootVolumeId()";
	printAllRecords("RootVolume");
}
void GMDBManager::printDBVersion() const
{
	qDebug() << "printDBVersion()";
	printAllRecords("dbversion");
}




void GMDBManager::printAllRecords(QString tableName) const
{
	std::cout << tableName.toStdString() << " in db:" << std::endl;

	// QSqlQuery query("SELECT * FROM " + tableName);
	QSqlQuery query = selectAllFromTable(tableName);

	int nCols = (m_tableNames[tableName]).size();

	// print table column names
	std::cout << "- " << m_tableNames[tableName].join(", ").toStdString() << std::endl;
	while (query.next())
	{
		std::cout << "* "; // TODO: move to a osstream: it's cleaner
		for( int ii=0; ii<nCols; ++ii)
		{
			if (! (ii==0) )
				std::cout << ", ";
			std::string valueStr = query.value(ii).toString().toStdString();
			// if (valueStr == "")
			// 	std::cout << "NULL"; // removed because we want to save the NULL to the DB, for consistency
			// else
				std::cout << valueStr;
		}
		std::cout << std::endl;
	}

	// TODO: I want to have a symbol like '---' to mean empty line when query gives 0 results.
	// but I guess query.size() is not the right method...
	//	qDebug() << "query size: " << query.size();
	//	if (query.size() == 0 || query.size() == -1)
	//		std::cout << "---" << std::endl;

}
QHash<unsigned int, QStringList> GMDBManager::getTableFromNodeType(QString nodeType)
{

	QString tableName = getTableNameFromNodeType(nodeType);

	QHash<unsigned int, QStringList> records;
	QStringList nodeParams;

	int nCols = (m_tableNames[tableName]).size();

	QSqlQuery query = selectAllFromTable(tableName);
	while (query.next()) {
		nodeParams.clear();
		unsigned int nodeId = query.value(0).toUInt();

		for( int ii=0; ii<nCols; ++ii) {
			nodeParams << query.value(ii).toString();
		}
		records[nodeId] = nodeParams;
	}
return records;
}


void GMDBManager::showError(const QSqlError &err) const
{
	qWarning() << "Unable to initialize Database" << "Error initializing database: " + err.text();
}

QVariant GMDBManager::addPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addPhysVol() - is root?" << isRootVolume;

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into PhysVols(logvol, parent) values(?, ?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(logVolId);
	q.addBindValue(parentPhysVolId);
	q.exec();

	QVariant lastInserted = q.lastInsertId();

	if (isRootVolume) {
		storeRootVolume(lastInserted, "GeoPhysVol");
	}

	return lastInserted;

}

QVariant GMDBManager::addFullPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addFullPhysVol() - is root?" << isRootVolume;

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into FullPhysVols(logvol, parent) values(?, ?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(logVolId);
	q.addBindValue(parentPhysVolId);
	q.exec();

	QVariant lastInserted = q.lastInsertId();

	if (isRootVolume) {
		storeRootVolume(lastInserted, "GeoFullPhysVol");
	}

	return lastInserted;

}



QVariant GMDBManager::addLogVol(const QString &name, const QVariant &shapeId, const QVariant &materialId)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addLogVol()";

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into LogVols(name, shape, material) values(?, ?, ?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(name);
	q.addBindValue(shapeId);
	q.addBindValue(materialId);
	q.exec();
	return q.lastInsertId();
}

QVariant GMDBManager::addMaterial(const QString &name)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addMaterial()";

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into Materials(name) values(?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(name);
	q.exec();
	return q.lastInsertId();
}

bool GMDBManager::addListOfChildrenPositions(const std::vector<QStringList> records)
{
    // NOTE: Choose the right function for your version of SQLite!!
	return addListOfRecordsToTable("ChildrenPositions", records); // newest SQLite versions
	//return addListOfRecordsToTableOld("ChildrenPositions", records); // old SQLite versions
}

bool GMDBManager::addListOfRecords(const QString geoType, const std::vector<QStringList> records)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addListOfRecords():" << geoType;

    QString tableName = m_childType_tableName[geoType];

	if (tableName.isEmpty()) {
        //qWarning() << "m_childType_tableName:" << m_childType_tableName;
		qWarning() << "ERROR!! could not retrieve tableName for node type " << geoType << "!! Aborting...";
		exit(1);
	}

	if (records.size() > 0 ) {
        // NOTE: Choose the right function!!
		return addListOfRecordsToTable(tableName, records); // newest SQLite versions
		//return addListOfRecordsToTableOld(tableName, records); // old SQLite versions
	}
	else
		qWarning() << "Info: no records to save for geoType '" << geoType << "'. Skipping...";
	return true;
}

// ***Note***
// the syntax built here below is more convenient
// but it is supported only in SQLite >= 3.7.11
// ...but SLC6 only has 3.7.9!
// here we build a query like this:
// queryStr = QString("INSERT INTO Materials (id, name) VALUES  (1,'Air'), (2,'Silicon'), (368,'ShieldSteel');");
//
bool GMDBManager::addListOfRecordsToTable(const QString tableName, const std::vector<QStringList> records)
{
	// get table columns and format them for query
	QString tableColString = "(" + m_tableNames[tableName].join(", ") + ")";

    unsigned int nRecords = records.size();
    qInfo() << "number of " << tableName << "records to insert into the DB:" << nRecords;

	// preparing the SQL query
	QString queryStr("INSERT INTO %1 %2 VALUES ");
	queryStr = queryStr.arg(tableName); // insert table name
	queryStr = queryStr.arg(tableColString); // insert table columns

	unsigned int nMat = nRecords;
	unsigned int id = 0;
	foreach(QStringList rec, records) {
        //qDebug() << "rec:" << rec;

		++id;
		QStringList items;

		foreach (QString item, rec) {
			items << '"' + item + '"';
		}
		QString values = items.join(",");
		queryStr += " (" + QString::number(id) + "," + values + ")";
		if (id != nMat)
			queryStr += ",";
		else
			queryStr += ";";

        }
    qDebug() << "queryStr:" << queryStr;

	// executing the SQL query
	QSqlQuery q;
	if (!q.exec(queryStr)) {
        qWarning() << "ERROR!!! SQL error:";
		showError(q.lastError());
		return false;
	}

	qDebug() << "DONE. The list of records have been inserted into the DB.";
	return true;
}



// ***Note***
// old syntax, for SQLite in SLC6
// here below we build the syntax for multiple INSERT
// compatible with old SQLite versions
// see: stackoverflow.com/questions/1609637
//
// we want to build a query like this:
//  queryStr = QString("INSERT INTO Materials (id, name) SELECT 1 as id, 'Air' as name UNION ALL SELECT 2,'Silicon' UNION ALL SELECT 368,'ShieldSteel' ")
//
bool GMDBManager::addListOfRecordsToTableOld(const QString tableName, const std::vector<QStringList> records)
{
	// get table columns and format them for query
	QString tableColString = "(" + m_tableNames[tableName].join(", ") + ")";



    QStringList colNames = m_tableNames[tableName];
    unsigned int nRecords = records.size();
    qInfo() << "number of " << tableName << "records to insert into the DB:" << nRecords;

    /*
     * SQLite has a limit on 'union' items, set at 500. So we have to split the items if we have more.
     * See: stackoverflow.com/questions/9527851/
     */
    unsigned int bunchSize = 500;
    if ( nRecords > bunchSize ) {
        qWarning() << "WARNING! " << nRecords << " records of type " << tableName << "to store in the DB in one call! Call limit is " << bunchSize << " --> We split them in bunches...";

        std::vector<QStringList> recordsCopy( records ); // TODO: maybe we should use a deque or queue, which have fast pop of first element?

        std::vector<QStringList>::const_iterator first;
        std::vector<QStringList>::const_iterator last;

        unsigned int start = 0;

        while ( recordsCopy.size() > 0 ) {

        // preparing the SQL query
        QString queryStr("INSERT INTO %1 %2 SELECT ");
	    queryStr = queryStr.arg(tableName); // insert table name
	    queryStr = queryStr.arg(tableColString); // insert table columns

        first = recordsCopy.begin();
        last  = recordsCopy.size() > bunchSize ? recordsCopy.begin() + bunchSize : recordsCopy.end();

        std::vector<QStringList> recordsBunch( first, last ); // we take the first 500 records
        recordsCopy.erase( first, last ); // we delete the first 500 records
        if (m_deepDebug) qDebug() << "start:" << start << "recordsBunch size:" << recordsBunch.size() << "- recordsCopy size after removal:" << recordsCopy.size();

	    // --- first record
        // outcome should be like: " 1 as id, 'Air' as name "
        unsigned int id = start+1; // set the first ID as 'start'
        QStringList recFirst = recordsBunch.front(); // access first record from vector
        recordsBunch.erase( recordsBunch.begin() ); // delete first record
        if (m_deepDebug) qDebug() << "after taking first record - recordsBunch size:" << recordsBunch.size();

        // first item in the first record (it is the ID)
        QString firstCol = colNames[0];
        queryStr += QString::number(id) + " as " + firstCol + ", ";

        // the other items in the first record
        unsigned int ii = 1;
        unsigned int nRecs = recFirst.size();
        foreach (QString rec, recFirst) {
            queryStr += "'" + rec + "'" + " as " + colNames[ii];
            if (ii != nRecs)
                queryStr += ","; // add coma, but not on latest
            queryStr += " "; // add space
            //qDebug() << "first element:" << ii << nRecs << queryStr;
            ++ii;
        }
        if (m_deepDebug) qDebug() << "first element query:" << queryStr;
        // --- other records
        // outcome should be: " UNION ALL SELECT 2,'Silicon' "


            foreach(QStringList recs, recordsBunch) {

							// DEBUG
							if (tableName == "Functions") {
								if (recs[1].length() > 65000) {
									std::cout << "LONG STRING! size: " << recs[1].length() << std::endl;
									std::cout << "LONG STRING! string: " << recs[1].toStdString() << std::endl << std::endl;
								}
							}
							//------

								++id;
                // put single quotes around items
                QStringList items;
                foreach (QString item, recs) {
			        		items << "'" + item + "'";
		        		}
                // join items into a string and add the SQL commands
                QString itemsStr = items.join(", ");
                queryStr += " UNION ALL SELECT " + QString::number(id) + ", " + itemsStr;
            }

        //qDebug() << "queryStr:" << queryStr;

	    // executing the SQL query
	    QSqlQuery q;
	    if (!q.exec(queryStr)) {
            qWarning() << "ERROR!!! SQL error:";
		    showError(q.lastError());
		    return false;
	    }
        qDebug() << bunchSize << "elements have been saved into the DB, starting at:" << start;

        start += bunchSize; // for the next iteration

      } // end of while

    } // end of if(>500)
    else {
    // preparing the SQL query
    QString queryStr("INSERT INTO %1 %2 SELECT ");
	queryStr = queryStr.arg(tableName); // insert table name
	queryStr = queryStr.arg(tableColString); // insert table columns

    unsigned int id = 0;
    // loop on all records
	foreach(QStringList recs, records) {

		++id;

        // first record
        // outcome should be like: " 1 as id, 'Air' as name "
        if (id == 1) {

            // first item if the record (it is the ID)
            QString firstCol = colNames.takeFirst();
            queryStr += QString::number(id) + " as " + firstCol + ", ";

            // the other items in the record
            unsigned int ii = 0;
            unsigned int nRecs = recs.size();
            foreach (QString rec, recs) {
                queryStr += "'" + rec + "'" + " as " + colNames[ii];
                if (ii != nRecs-1)
                    queryStr += ","; // add coma, but not on latest
                queryStr += " "; // add space
                //qDebug() << ii << nRecs << queryStr;
                ++ii;
            }

         }
        // other records
        // outcome should be: " UNION ALL SELECT 2,'Silicon' "
        else {

            // put single quotes around items
            QStringList items;
            foreach (QString item, recs) {
			    items << "'" + item + "'";
		    }
            // join items into a string and add the SQL commands
            QString itemsStr = items.join(", ");
            queryStr += " UNION ALL SELECT " + QString::number(id) + ", " + itemsStr;
        }

	} // end of: foreach(QStringList recs, records)

    //qDebug() << "queryStr:" << queryStr;

	// executing the SQL query
	QSqlQuery q;
	if (!q.exec(queryStr)) {
        qWarning() << "ERROR!!! SQL error:";
		showError(q.lastError());
		return false;
	}
} // end of (else > 500)

	qDebug() << "DONE. The list of " << nRecords << "records have been inserted into the DB.";
	return true;

}





QVariant GMDBManager::addShape(const QString &type, const QString &parameters)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addShape()";

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into Shapes(type, parameters) values(?, ?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(type);
	q.addBindValue(parameters);
	q.exec();
	return q.lastInsertId();
}

QVariant GMDBManager::addSerialDenominator(const QString &baseName)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addSerialDenominator("+baseName+")";

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into SerialDenominators(baseName) values(?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(baseName);
	q.exec();
	return q.lastInsertId();
}

QVariant GMDBManager::addNameTag(const QString &name)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addNameTag("+name+")";

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into NameTags(name) values(?)"))) {
		showError(q.lastError());
		return QVariant();
	}
	q.addBindValue(name);
	q.exec();
	return q.lastInsertId();
}

QVariant GMDBManager::addFunction(const QString expression)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addFunction()";

	// TEST
	std::cout << "Function - expression string len: " << expression.length();
	std::cout << "Function - expression: " << expression.toStdString() << std::endl << std::endl;
  //-----

	qDebug() << "expression:" << expression;

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into Functions(expression) values(?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(expression);
	q.exec();
	return q.lastInsertId();


}

QVariant GMDBManager::addSerialTransformer(const QVariant &funcId, const QVariant &physvolId, const QString physvolType, const unsigned int &copies)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addSerialTransformer()" << funcId << physvolId << copies;

	QVariant volTableId = getTableIdFromNodeType(physvolType);

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into SerialTransformers(funcId, volId, volTable, copies) values(?, ?, ?, ?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(funcId);
	q.addBindValue(physvolId);
	q.addBindValue(volTableId);
	q.addBindValue(copies);
	q.exec();
	return q.lastInsertId();
}

QVariant GMDBManager::addTransform(QVector<double> params)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addTransform()";

	// get the 12 matrix elements
	double xx = params[0];
	double xy = params[1];
	double xz = params[2];

	double yx = params[3];
	double yy = params[4];
	double yz = params[5];

	double zx = params[6];
	double zy = params[7];
	double zz = params[8];

	double dx = params[9];
	double dy = params[10];
	double dz = params[11];

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into Transforms(xx, xy, xz, yx, yy, yz, zx, zy, zz, dx, dy, dz) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(xx);
	q.addBindValue(xy);
	q.addBindValue(xz);
	q.addBindValue(yx);
	q.addBindValue(yy);
	q.addBindValue(yz);
	q.addBindValue(zx);
	q.addBindValue(zy);
	q.addBindValue(zz);
	q.addBindValue(dx);
	q.addBindValue(dy);
	q.addBindValue(dz);
	q.exec();
	return q.lastInsertId();
}


QVariant GMDBManager::addAlignableTransform(QVector<double> params)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addAlignableTransform()";

	// get the 12 matrix elements
	double xx = params[0];
	double xy = params[1];
	double xz = params[2];

	double yx = params[3];
	double yy = params[4];
	double yz = params[5];

	double zx = params[6];
	double zy = params[7];
	double zz = params[8];

	double dx = params[9];
	double dy = params[10];
	double dz = params[11];

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into AlignableTransforms(xx, xy, xz, yx, yy, yz, zx, zy, zz, dx, dy, dz) values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)"))) {
		showError(q.lastError());
		return QVariant();
	}

	q.addBindValue(xx);
	q.addBindValue(xy);
	q.addBindValue(xz);
	q.addBindValue(yx);
	q.addBindValue(yy);
	q.addBindValue(yz);
	q.addBindValue(zx);
	q.addBindValue(zy);
	q.addBindValue(zz);
	q.addBindValue(dx);
	q.addBindValue(dy);
	q.addBindValue(dz);
	q.exec();
	return q.lastInsertId();
}


bool GMDBManager::addRootVolume(const QStringList values)
{
	if (values.size() > 0 ) {
		QVariant volId = QVariant(values[0]);
		QString nodeType = values[1];
		return storeRootVolume( volId, nodeType );
	}
	else
		qFatal("Info: no records to save for RootVolume!");
	return false;
}

// insert a single parent-child relationship for a given parent volume
void GMDBManager::addChildPosition(const QVariant parentId, const QString parentType, const QVariant childId, const unsigned int parentCopyNumber, const unsigned int childPos, const QString childType, const unsigned int childCopyN)
{
	if (m_deepDebug) qDebug() << "GMDBManager::addChildPosition(" << parentId << "," << parentType << "," << QString::number(parentCopyNumber) << "," <<childPos << "," << childType << ", " << childId << ", " << QString::number(childCopyN) << ")";

	// get table name -- which the child ID refers to -- based on the child type
	QVariant parentTableId = getTableIdFromNodeType(parentType);
	QVariant childTableId = getTableIdFromNodeType(childType);

	QString tableName = "ChildrenPositions";
	QStringList cols = getTableColumnNames(tableName);

	QString queryStr = QString("insert into %1(%2, %3, %4, %5, %6, %7, %8) values(?, ?, ?, ?, ?, ?, ?)").arg(tableName);
	for (int i=0; i < cols.size(); ++i) {
		if (i != 0) // skip the "id" column
			queryStr = queryStr.arg( cols.at(i) );
	}
	if (m_deepDebug) qDebug() << "queryStr:" << queryStr;
	QSqlQuery q;
	if (!q.prepare(queryStr)) {
		showError(q.lastError());
		return;
	}
	qDebug() << "parent:" << parentId;
	q.addBindValue(parentId);
	q.addBindValue(parentTableId);
	q.addBindValue(parentCopyNumber);
	q.addBindValue(childPos);
	q.addBindValue(childTableId);
	q.addBindValue(childId);
	q.addBindValue(childCopyN);
	q.exec();
	return;
}

void GMDBManager::addDBversion(const QString version)
{
	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into dbversion(version) values(?)"))) {
		showError(q.lastError());
		return;
	}
	q.addBindValue(version);
	q.exec();
	return;
}


QStringList GMDBManager::getItem(QString geoType, unsigned int id)
{
	if (m_deepDebug) qDebug() << "GMDBManager::getItem(geoType, id)"<< geoType << QString::number(id);

	/* Get the right DB table */
	QString tableName = getTableNameFromNodeType(geoType);
	return getItemFromTableName(tableName, id);
}


QStringList GMDBManager::getItem(unsigned int tableId, unsigned int id)
{
	if (m_deepDebug) qDebug() << "GMDBManager::getItem(tableId, id)"<< QString::number(tableId) << QString::number(id);
	/* Get the right DB table */
	QString tableName = getTableNameFromTableId(tableId);
	return getItemFromTableName(tableName, id);

}

QStringList GMDBManager::getItemAndType(unsigned int tableId, unsigned int id)
{
	if (m_deepDebug) qDebug() << "GMDBManager::getItemAndType(tableId, id)"<< QString::number(tableId) << QString::number(id);
	/* Get the right DB table */
	QString tableName = getTableNameFromTableId(tableId);

	QString nodeType = getNodeTypeFromTableId(tableId);

	QStringList results;
	QStringList item =  getItemFromTableName(tableName, id);

	results << nodeType << item;

	return results;

}


QStringList GMDBManager::getItemFromTableName(QString tableName, unsigned int id)
{
	if (m_deepDebug) qDebug() << "GMDBManager::getItemFromTableName(tableName, id)"<< tableName << QString::number(id);
	/*
	 * 2. Get the object from DB
	 */
	// prepare a SQL string with the right table name
	QString queryStr = QString("SELECT * FROM %1 WHERE id = (?)").arg(tableName);
	// prepare the query
	QSqlQuery q;
	if (!q.prepare( queryStr )) {
		showError(q.lastError());
		return QStringList();
	}
	q.addBindValue(id);
	q.exec();

	QStringList params;

	// get the number of columns of the DB table
	int nCols = (m_tableNames[tableName]).size();

	while (q.next()) {

		for( int ii=0; ii<nCols; ++ii)
			params << q.value(ii).toString();
	}
	if (params.length()==0) {
		qWarning() << "WARNING!!" << "Item" << id << "does not exist in table" << tableName << "!!";
	}
	return params;
}

// get the list of children for a single GeoVPhysVol (i.e., GeoPhysVol or GeoFullPhysVol)
QMap<unsigned int, QStringList> GMDBManager::getVPhysVolChildren(const unsigned int id, const QString nodeType, const unsigned int parentCopyNumber)
{
	if (m_deepDebug) qDebug() << "GMDBManager::getVPhysVolChildren() - id:" << QString::number(id) << "- type:" << nodeType << "- copyN:" << QString::number(parentCopyNumber);

	QVariant tableId = getTableIdFromNodeType(nodeType);

	// get children and their positions
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT * FROM ChildrenPositions WHERE parentId = (?) AND parentTable = (?) AND parentCopyNumber = (?)"))) {
		showError(q.lastError());
		return QMap<unsigned int, QStringList>();
	}
	q.addBindValue(id);
	q.addBindValue(tableId);
	q.addBindValue(parentCopyNumber);
	q.exec();

	QMap<unsigned int, QStringList> children;
	QStringList childParams;

	// get the number of columns of the DB table
	int nCols = m_tableNames["ChildrenPositions"].size();

	while (q.next()) {

		childParams.clear();

		unsigned int childPos = q.value(3).toUInt();

		for( int ii=0; ii<nCols; ++ii)
			childParams << q.value(ii).toString();

		children[childPos] = childParams;
	}

	return children;
}


// Get all parent-children data from the database in one go
QHash<QString, QMap<unsigned int, QStringList>> GMDBManager::getChildrenTable()
{
	qDebug() << "GMDBManager::getChildrenTable()";

	QSqlQuery q = selectAllFromTable("ChildrenPositions");

	QHash< QString, QMap<unsigned int, QStringList> > all_children; // to store all children
	// QMap<unsigned int, QStringList> children; // to temporarily store the children of one parent
	QStringList childParams; // to temporarily store the children parameters

	// get the number of columns of the DB table
	int nCols = m_tableNames["ChildrenPositions"].size();
        qDebug() << "num of columns in childrenPos table" << nCols;

    // loop over all children's positions stored in the DB
	while (q.next()) {

		childParams.clear();

		QString parentId = q.value(1).toString();
		QString parentTable = q.value(2).toString();
		QString parentCopyNumber = q.value(3).toString();
		unsigned int childPos = q.value(4).toUInt();

		QString key = parentId + ":" + parentTable + ":" + parentCopyNumber;

		for( int ii=0; ii<nCols; ++ii)
			childParams << q.value(ii).toString();

		all_children[key][childPos] = childParams;
	}
	return all_children;
}



QVariant GMDBManager::getTableIdFromNodeType(QString nodeType)
{
	qDebug() << "GMDBManager::getTableIdFromNodeType("<< nodeType <<")";
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT id FROM GeoNodesTypes WHERE nodeType = (?)"))) {
		showError(q.lastError());
		return QVariant();
	}
	q.addBindValue(nodeType);
	q.exec();

	QVariant id;
	while (q.next()) {
		id = q.value(0);
		qDebug() << "id:" << id;
	}

	return id;
}

QString GMDBManager::getTableNameFromNodeType(QString nodeType)
{
	qDebug() << "GMDBManager::getTableNameFromNodeType("<< nodeType <<")";
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT tableName FROM GeoNodesTypes WHERE nodeType = (?)"))) {
		showError(q.lastError());
		return QString();
	}
	q.addBindValue(nodeType);
	q.exec();

	QString tableName;
	while (q.next()) {
		tableName = q.value(0).toString();
		qDebug() << "tableName:" << tableName;
	}

	return tableName;
}

// TODO: this and other methods could take data from in-memory maps, without asking to the DB all the times
QString GMDBManager::getTableNameFromTableId(unsigned int tabId)
{
	qDebug() << "GMDBManager::getTableNameFromTableId()";
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT tableName FROM GeoNodesTypes WHERE id = (?)"))) {
		showError(q.lastError());
		return QString();
	}
	q.addBindValue(tabId);
	q.exec();

	QString tableName;
	while (q.next()) {
		tableName = q.value(0).toString();
		qDebug() << "tableName:" << tableName;
	}

	return tableName;
}

QString GMDBManager::getNodeTypeFromTableId(unsigned int tabId)
{
	qDebug() << "GMDBManager::getNodeTypeFromTableId()";
	QSqlQuery q;
	if (!q.prepare(QLatin1String("SELECT nodeType FROM GeoNodesTypes WHERE id = (?)"))) {
		showError(q.lastError());
		return QString();
	}
	q.addBindValue(tabId);
	q.exec();

	QString nodeType;
	while (q.next()) {
		nodeType = q.value(0).toString();
		qDebug() << "tableName:" << nodeType;
	}

	return nodeType;
}


QHash<unsigned int, QString> GMDBManager::getAll_TableIDsNodeTypes()
{
	qDebug() << "GMDBManager::getAll_TableIDsNodeTypes()";

	QHash<unsigned int, QString> output;

	QSqlQuery q = selectAllFromTable("GeoNodesTypes");

	unsigned int id;
	QString nodeType;
	// QString tableName;
	while (q.next()) {
		id = q.value(0).toUInt();
		nodeType = q.value(1).toString();
		output[id] = nodeType;
	}
	return output;
}


QHash<QString, unsigned int> GMDBManager::getAll_NodeTypesTableIDs()
{
	qDebug() << "GMDBManager::getAll_NodeTypesTableIDs()";

	QHash<QString, unsigned int> output;

	QSqlQuery q = selectAllFromTable("GeoNodesTypes");

	unsigned int id;
	QString nodeType;
	// QString tableName;
	while (q.next()) {
		id = q.value(0).toUInt();
		nodeType = q.value(1).toString();
		output[nodeType] = id;
	}
	return output;
}


QSqlQuery GMDBManager::selectAllFromTable(QString tableName) const
{
	QSqlQuery q;

	QString queryStr = QString("SELECT * FROM %1");
	queryStr = queryStr.arg(tableName);

	if (!q.prepare(queryStr)) {
		showError(q.lastError());
		return QSqlQuery();
	}
	q.exec();
	return q;
}


bool GMDBManager::initDB()
{
	qDebug() << "GMDBManager::initDB()";


	// check if DB is empty
	qDebug() << "checking existing tables...";
	QSqlDatabase db = QSqlDatabase::database();
	QStringList tables = db.tables();
	if (tables.contains("LogVols", Qt::CaseInsensitive)
			&& tables.contains("PhysVols", Qt::CaseInsensitive)
			&& tables.contains("Materials", Qt::CaseInsensitive)
			&& tables.contains("Shapes", Qt::CaseInsensitive)
	) {
		qDebug() << "tables are present already. Skipping tables creation. Loading tables...";
		loadTableNamesFromDB();
		return true;
	}

	// if DB is empty, then create tables
	qDebug() << "DB file is empty. Creating tables...";
	bool tablesOK = createTables();

	// store DB version
	qDebug() << "Storing DB version:" << QString::fromStdString(dbversion);
	addDBversion(QString::fromStdString(dbversion));

	return tablesOK;

}

bool GMDBManager::createTables()
{
	qDebug() << "GMDBManager::createTables()";

	QStringList tab;
	QSqlQuery q;
	QString geoNode;
	QString tableName;
	QString queryStr;

	// create a table to store the relation between the types of GeoNodes and the name of the table
	tableName = "dbversion";
	tab << tableName << "id" << "version";
	queryStr = QString("create table %1(%2 integer primary key, %3 integer)");
	for (int i=0; i < tab.size(); ++i) {
		queryStr = queryStr.arg( tab.at(i) );
	}
	//qDebug() << "query:" << queryStr;
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	}
	storeTableColumnNames(tab);
	tab.clear();

	// create a table to store the relation between the types of GeoNodes and the name of the table
	tableName = "GeoNodesTypes";
	tab << tableName << "id" << "nodeType" << "tableName";
	storeTableColumnNames(tab);
	tab.clear();
	if (!q.exec(QLatin1String("create table GeoNodesTypes(id integer primary key, nodeType varchar, tableName varchar)"))) {
		showError(q.lastError());
		return false;
	}

	// ChildrenPositions table
	tableName = "ChildrenPositions";
	tab << tableName << "id" << "parentId" << "parentTable" << "parentCopyNumber" << "position" << "childTable" << "childId" << "childCopyNumber";
	queryStr = QString("create table %1(%2 integer primary key, %3 integer, %4 integer not null REFERENCES GeoNodesTypes(id), %5 integer, %6 integer, %7 integer not null REFERENCES GeoNodesTypes(id), %8 integer not null, %9 integer)"); // FIXME: add "NOT NULL" to copy numbers
	for (int i=0; i < tab.size(); ++i) {
		queryStr = queryStr.arg( tab.at(i) );
	}
	// qDebug() << "query:" << queryStr;
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	}
	storeTableColumnNames(tab);
	tab.clear();


	// RootVolume table
	tableName = "RootVolume";
	tab << tableName << "id" << "volId" << "volTable";
	queryStr = QString("create table %1(%2 integer primary key, %3 integer not null, %4 integer not null REFERENCES GeoNodesTypes(id))");
	for (int i=0; i < tab.size(); ++i) {
		queryStr = queryStr.arg( tab.at(i) );
	}
	//qDebug() << "query:" << queryStr;
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	}
	storeTableColumnNames(tab);
	tab.clear();


	// PhysVols table
	geoNode = "GeoPhysVol";
	tableName = "PhysVols";
	tab << tableName << "id" << "logvol" << "parent"; // FIXME: remove "parent" field, it is not used anymore
	storeTableColumnNames(tab);
	tab.clear();
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	if (!q.exec(QLatin1String("create table PhysVols(id integer primary key, logvol integer not null, parent integer)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// FullPhysVols table
	geoNode = "GeoFullPhysVol";
	tableName = "FullPhysVols";
	tab << tableName << "id" << "logvol" << "parent";
	storeTableColumnNames(tab);
	tab.clear();
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	if (!q.exec(QLatin1String("create table FullPhysVols(id integer primary key, logvol integer not null, parent integer)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// LogVols table
	geoNode = "GeoLogVol";
	tableName = "LogVols";
	tab << tableName << "id" << "name" << "shape" << "material";
	storeTableColumnNames(tab);
	tab.clear();
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	if (!q.exec(QLatin1String("create table LogVols(id integer primary key, name varchar, shape integer not null, material integer not null)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// Materials table
	geoNode = "GeoMaterial";
	tableName = "Materials";
	tab << tableName << "id" << "name";
	storeTableColumnNames(tab);
	tab.clear();
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	if (!q.exec(QLatin1String("create table Materials(id integer primary key, name varchar)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// Shapes table
	geoNode = "GeoShape";
	tableName = "Shapes";
	tab << tableName << "id" << "type" << "parameters";
	storeTableColumnNames(tab);
	tab.clear();
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	if (!q.exec(QLatin1String("create table Shapes(id integer primary key, type varchar, parameters varchar)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// SerialDenominators table
	geoNode = "GeoSerialDenominator";
	tableName = "SerialDenominators";
	tab << tableName << "id" << "baseName";
	storeTableColumnNames(tab);
	tab.clear();
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	if (!q.exec(QLatin1String("create table SerialDenominators(id integer primary key, baseName varchar)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// Functions table
	geoNode = "Function";
	tableName = "Functions";
	tab << tableName << "id" << "expression";
	storeTableColumnNames(tab);
	tab.clear();
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	if (!q.exec(QLatin1String("create table Functions(id integer primary key, expression varchar)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// SerialDenominators table
	geoNode = "GeoSerialTransformer";
	tableName = "SerialTransformers";
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	tab << tableName << "id" << "funcId" << "volId" << "volTable" << "copies";
	storeTableColumnNames(tab);
	queryStr = QString("create table %1(%2 integer primary key, %3 integer not null REFERENCES Functions(id), %4 integer not null, %5 integer not null REFERENCES GeoNodesTypes(id), %6 integer)");
	for (int i=0; i < tab.size(); ++i) {
		queryStr = queryStr.arg( tab.at(i) );
	}
	tab.clear();
	// qDebug() << "query:" << queryStr;
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}


	// Transforms table
	geoNode = "GeoTransform";
	tableName = "Transforms";
	tab << tableName << "id" << "xx" << "xy" << "xz" << "yx" << "yy" << "yz" << "zx" << "zy" << "zz" << "dx" << "dy" << "dz";
	storeTableColumnNames(tab);
	tab.clear();
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	if (!q.exec(QLatin1String("create table Transforms(id integer primary key, xx real, xy real, xz real, yx real, yy real, yz real, zx real, zy real, zz real, dx real, dy real, dz real)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// AlignableTransforms table
	geoNode = "GeoAlignableTransform";
	tableName = "AlignableTransforms";
	tab << tableName << "id" << "xx" << "xy" << "xz" << "yx" << "yy" << "yz" << "zx" << "zy" << "zz" << "dx" << "dy" << "dz";
	storeTableColumnNames(tab);
	tab.clear();
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	if (!q.exec(QLatin1String("create table AlignableTransforms(id integer primary key, xx real, xy real, xz real, yx real, yy real, yz real, zx real, zy real, zz real, dx real, dy real, dz real)"))) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

	// NameTags table
	geoNode = "GeoNameTag";
	tableName = "NameTags";
	m_childType_tableName[geoNode] = tableName; // store type-table relation
	tab << tableName << "id" << "name";
	storeTableColumnNames(tab);
	queryStr = QString("create table %1(%2 integer primary key, %3 varchar)");
	for (int i=0; i < tab.size(); ++i) {
		queryStr = queryStr.arg( tab.at(i) );
	}
	tab.clear();
	// qDebug() << "query:" << queryStr;
	if (!q.exec(queryStr)) {
		showError(q.lastError());
		return false;
	} else {
		storeNodeType(geoNode, tableName);
	}

    qDebug() << "m_childType_tableName:" << m_childType_tableName;
	qDebug() << "DONE. All tables created successfully.";
	return true;

}

void GMDBManager::loadTableNamesFromDB()
{
	qDebug() << "GMDBManager::loadTableNames()";
	QSqlDatabase db = QSqlDatabase::database();
	QStringList tables = db.tables();
	foreach (QString tab, tables) {
		QStringList tabColNames = getTableColNamesFromDB(tab);
		storeTableColumnNames(tabColNames);
	}
	// qDebug() << "m_tableNames:" << m_tableNames;
}

void GMDBManager::storeTableColumnNames(QStringList input)
{
	//	qDebug() << "GMDBManager::storeTableColumnNames()";
	if (! (input.isEmpty()) ) {
		QString tabName = input.takeFirst();
		m_tableNames[tabName] = input;
	}
}

QStringList GMDBManager::getTableColumnNames(QString tableName)
{
	//	qDebug() << "GMDBManager::getTableColumnNames()";
	return m_tableNames[tableName];
}

void GMDBManager::storeNodeType(QString nodeType, QString tableName)
{
	qDebug() << "GMDBManager::storeNodeType()";

	QSqlQuery q;
	if (!q.prepare(QLatin1String("insert into GeoNodesTypes(nodeType, tableName) values(?, ?)"))) {
		showError(q.lastError());
		return;
	}

	q.addBindValue(nodeType);
	q.addBindValue(tableName);
	q.exec();
	return;
}

void GMDBManager::loadTestData()
{

	qDebug() << "GMDBManager::loadTestData()";

	// create test data
	QSqlQuery q;
	//
	//	qDebug() << "Loading Shapes...";
	//	if (!q.prepare(QLatin1String("insert into Shapes(name) values(?)"))) {
	//		showError(q.lastError());
	//		return;
	//	}
	qDebug() << "Loading Shapes...";
	QVariant boxId    = addShape(QLatin1String("Box"), "");
	QVariant coneId   = addShape(QLatin1String("Cone"), "");
	QVariant sphereId = addShape(QLatin1String("Sphere"), "");

	qDebug() << "Loading Materials...";
	QVariant airId  = addMaterial(QLatin1String("Air"));
	QVariant ironId = addMaterial(QLatin1String("Iron"));
	QVariant leadId = addMaterial(QLatin1String("Lead"));

	qDebug() << "Loading LogVols...";
	QVariant worldLogId = addLogVol(QLatin1String("WorldLog"), boxId, airId);
	QVariant toyLogId  = addLogVol(QLatin1String("ToyLog"), coneId, leadId);

	qDebug() << "Loading PhysVols...";
	QVariant rootPhysId = addPhysVol(worldLogId, QVariant()); // no parent
	QVariant childPhysId = addPhysVol(toyLogId, rootPhysId);

	qDebug() << "DONE. Created and loaded test data.";
}


QStringList GMDBManager::getTableColNamesFromDB(QString tableName) const
{
	QStringList colNames;
	colNames << tableName;

	// fetch the driver
	QSqlDriver* driver = m_db.driver();
	QSqlRecord record = driver->record(tableName);
	int colN = record.count();
	for (int i = 0; i < colN; ++i)
		colNames << record.fieldName(i);
	return colNames;
}



void GMDBManager::printTableColNamesFromDB(QString tableName) const
{
	qDebug() << "GMDBManager::printTableColNames:" << tableName;

	// fetch the driver
	qDebug() << getTableColNamesFromDB(tableName);
}

bool GMDBManager::storeRootVolume(QVariant id, QString nodeType)
{
	qDebug() << "GMDBManager::storeRootVolume:" << id << nodeType;

	QVariant typeId = getTableIdFromNodeType(nodeType);

	QString tableName = "RootVolume";
	QStringList cols = getTableColumnNames(tableName);

	// prepare the query string
	QString queryStr = QString("insert into %1(%2, %3) values(?, ?)").arg(tableName);
	for (int i=0; i < cols.size(); ++i) {
		if (i != 0) // skip the "id" column
			queryStr = queryStr.arg( cols.at(i) );
	}
	//qDebug() << "queryStr:" << queryStr;
	// prepare the query
	QSqlQuery q;
	if (!q.prepare(queryStr)) {
		showError(q.lastError());
		return false;
	}
	q.addBindValue(id);
	q.addBindValue(typeId);
	// run the query
	q.exec();
	return true;
}

QStringList GMDBManager::getRootPhysVol()
{
	qDebug() << "GMDBManager::getRootPhysVol()";
	// get the ID of the ROOT vol from the table "RootVolume"
	// QSqlQuery query("SELECT * FROM RootVolume");
	QSqlQuery query = selectAllFromTable("RootVolume");

	QVariant id;
	QVariant typeId;
	while (query.next()) {
		id = query.value(1);
		typeId = query.value(2);
	}
	return getItemAndType(typeId.toUInt(), id.toUInt());
}

QString GMDBManager::getDBFilePath()
{
    return m_dbpath;
}


int GMDBManager::getTableColIndex(QString tableName, QString colName)
{
	//qDebug() << "GMDBManager::getTableColIndex()";
	QStringList colFields = m_tableNames[tableName];
	return colFields.lastIndexOf(colName);
}

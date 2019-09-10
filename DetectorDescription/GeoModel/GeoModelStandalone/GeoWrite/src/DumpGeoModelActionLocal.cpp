/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// author: Riccardo.Maria.Bianchi@cern.ch
// major updates: Aug 2018

// local includes
#include "GeoWrite/DumpGeoModelActionLocal.h"

// TFPersistification includes
#include "TFPersistification/TransFunctionPersistifier.h"


// GeoModelKernel includes
#include "GeoModelKernel/GeoNodePath.h"
#include "GeoModelKernel/GeoCountVolAction.h"
// GeoModelKernel shapes
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeUnion.h"

// Qt includes
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlDriver>
#include <QVector>
#include <QDebug>

// C++ includes
#include <sstream>


// CLHEP includes // TODO: to remove once the migration to Eigen is complete
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Matrix.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h" // TODO: to be removed when dropping CLHEP



// int DumpGeoModelActionLocal::getChildPosition(unsigned int parentId)
// {
// 	if ( ! (m_parentChildrenMap.contains(parentId)) ) {
// 		m_parentChildrenMap[parentId] = 0;
// 	}
// 	++m_parentChildrenMap[parentId];
// 	return m_parentChildrenMap[parentId];
// }



/// Get next child position available, given the parent type, id and copy number
unsigned int DumpGeoModelActionLocal::getChildPosition(const QString parentId, const QString parentType, const unsigned int copyN)
{
	qDebug() << "DumpGeoModelActionLocal::getChildPosition()";
	QString tableId = getIdFromNodeType(parentType);
	QString key = tableId + ":" + parentId + ":" + QString::number(copyN);

	if ( ! (m_parentChildrenMap2.contains(key)) ) {
		m_parentChildrenMap2[key] = 1;
	} else {
	    ++m_parentChildrenMap2[key];
    }

    qDebug() << "parent key:" << key << " [tableId:parentId:copyN] - pos: " <<  m_parentChildrenMap2[key];
	return m_parentChildrenMap2[key];
}

unsigned int DumpGeoModelActionLocal::setVolumeCopyNumber(QString volId, QString volType)
{
	qDebug() << "DumpGeoModelActionLocal::setVolumeCopyNumber()";
	QString tableId = getIdFromNodeType(volType);
	QString key = tableId + ":" + volId;

	if ( ! (m_volumeCopiesMap.contains(key)) ) {
		m_volumeCopiesMap[key] = 1;
	} else {
    	++m_volumeCopiesMap[key];
    }

    qDebug() << "volume key:" << key << " [tableId:volumeId] - copy number: " <<  m_volumeCopiesMap[key];
	return m_volumeCopiesMap[key];
}


unsigned int DumpGeoModelActionLocal::getLatestParentCopyNumber(QString parentId, QString parentType)
{
	qDebug() << "DumpGeoModelActionLocal::getLatestParentCopyNumber()";
	QString tableId = getIdFromNodeType(parentType);
	QString key = tableId + ":" + parentId;
    qDebug() << "key:" << key;

	if ( ! (m_volumeCopiesMap.contains(key)) ) {
		qFatal("ERROR!!! Something's wrong in storing the number of copies!");
	}

    qDebug() << "get latest parent copy number:" << key << " [tableId:parentId] - copy number: " <<  m_volumeCopiesMap[key];
	return m_volumeCopiesMap[key];
}



// int DumpGeoModelActionLocal::getNewIdByNodeType(QString nodeType)
// {
// 	if ( ! (m_idsMap.contains(nodeType)) ) {
// 		m_idsMap[nodeType] = 0;
// 	}
// 	++m_idsMap[nodeType];
// 	return m_idsMap[nodeType];
// }


void DumpGeoModelActionLocal::handlePhysVol (const GeoPhysVol *vol)
{
	qDebug() << "\nDumpGeoModelActionLocal::handlePhysVol(GeoPhysVol*)";
	handleVPhysVolObjects( vol );
}


void DumpGeoModelActionLocal::handleFullPhysVol (const GeoFullPhysVol *vol)
{
	qDebug() << "\nDumpGeoModelActionLocal::handleFullPhysVol( GeoFullPhysVol* )";
	handleVPhysVolObjects( vol );
}


void DumpGeoModelActionLocal::handleVPhysVolObjects(const GeoVPhysVol* vol)
{
	qDebug() << "DumpGeoModelActionLocal::handleVPhysVolObjects( GeoVPhysVol* )";


	// get the address string for the current volume
	QString address = getAddressStringFromPointer( vol );

	// variables used to persistify the object
	QVariant physId;

	// check the volume position in the geometry tree
	GeoNodePath* path = getPath();
	unsigned int len = path->getLength();
    const GeoVPhysVol* tailVol = path->getTail();
    const GeoVPhysVol* headVol = path->getHead();
    const GeoVPhysVol* upperVol = nullptr;
    if (len > 1)
        upperVol = path->getItem(len-2); // item(len-1) is THIS volume ==> the length is updated when visiting a VPhysVol node
    else
        upperVol = path->getItem(len-1); // ROOT volume
	qDebug() << "path: " << path << " - length: " << len << " - upper vol: " << upperVol << " - tail vol: " << tailVol << " - head vol: " << headVol;
	//	std::cout << "path: " << path << std::endl;

	// this below is performed until the root volume is found, then "length" is not used anymore,
	// because not updated when recursively visiting children of PhysVols: it is updated only when a PhysVol node is visited

	bool doGetParentNode = true;
	bool storeRootVolume = false;
	bool isRootVolume = false;


	if ( ! m_rootVolumeFound) {
		if (len > 1) {
			doGetParentNode = true; // TODO: is that needed????
		} else{
			qDebug() << "This is the Root volume!";
            isRootVolume = true;
			m_rootVolumeFound = true;
			storeRootVolume = true;
			doGetParentNode = false;
		}
	} else {
		doGetParentNode = true;
	}

	// When dumping an unconnected sub-tree, e.g. a PhysVol used only in a SerialTransformer,
	// we don't want to get the parent ID.
	// If we do, the ID of the parent of the SerialTransformer is returned, which is wrong.
	if (m_unconnectedTree) {
		doGetParentNode = false;
		qDebug() << "Handling an unconnected tree: we skip the parent-finding step...";
		// now, we reset the status,
		// otherwise all the children of the first referenced, unconnected volume
		// will be treated as unconnected as well
		qDebug() << "setting 'unconnected' to: false";
		m_unconnectedTree = false;
	}


	// // get the parent volume
	// QStringList parentList = getParentNode(); // you can use that, but you have to modify it to get len-2, instead of len-1, for VPhysVol nodes
	// QString parentId = parentList[0];
	// QString parentType = parentList[1];

	// get the parent volume, if this is not the Root volume or an unconnected sub-tree
	const GeoVPhysVol* parentNode = nullptr;
	QVariant parentId = "NULL";

    QString volTypeStr = "NNN";

	if (doGetParentNode) {

        bool isShared = vol->isShared();
        qDebug() << "is this node shared?" << isShared;

        //if (isShared)
            parentNode = upperVol;
        //else
		//    parentNode = dynamic_cast<const GeoVPhysVol*>( &(*(vol->getParent() ))); // this cannot be trust for shared nodes
        qDebug() << "parentNode address" << parentNode;

		if (parentNode) {
			QString parentAddress = getAddressStringFromPointer(parentNode);
			qDebug() << "==> parent's address:" << parentNode;

			if (isAddressStored(parentAddress))
				parentId = getStoredIdFromAddress(parentAddress);
			//		else
			//			qFatal("FATAL ERROR!!! - The parent node of this child should has been stored in the DB already, but it was not found!!");

			qDebug() << "==> parent's LogVol name:" << QString::fromStdString(parentNode->getLogVol()->getName());
		}
		else {
			qDebug() << "NULL parent node!! --> this node appeared unconnected.";
		}
	}

	// counting children
	unsigned int nChildren = vol->getNChildVols();
	qDebug() << "number of child physical volumes:" << nChildren;
	qDebug() << "[number of PhysVol and SerialTransformer child nodes:" << vol->getNChildVolAndST() << "]";

	//// for debug
	// GeoCountVolAction cv;
	// cv.setDepthLimit(1);
	// vol->exec(&cv);
	// int nChildCount = cv.getCount();
	// qDebug() << "number of child volumes:" << nChildCount;



	// check if this object has been stored already
	if (! isAddressStored(address)) {

		/*
		 * VPhysVol features:
		 * - 1 parent VPhysVol (if any)
		 * - 1 LogVol
		 * - 1 Material
		 * - 1 Shape
		 */

		// Note: PhysVol has no name. Its LogVol has a name.
		//	  const std::string name = vol->getLogVol()->getName();

		// LOGVOL
		const GeoLogVol* logVol = vol->getLogVol();
		const QString logName = QString::fromStdString(logVol->getName());
		qDebug() << "LogVol name:"  << logName;


		// MATERIAL
		const GeoMaterial * mat = vol->getLogVol()->getMaterial();
		// const QString matName = QString::fromStdString(mat->getName());
		// qDebug() << "material name:" << matName << ", address:" << mat;


		// SHAPE
		const GeoShape * shape = vol->getLogVol()->getShape();
		// const QString shapeType = QString::fromStdString(shape->type());
		// qDebug() << "shape name:" << shapeType  << ", address:" << shape;
		// // get shape parameters
		// QString shapePars = getShapeParameters(shape);


		/*
		 * STORE THE OBJECTS IN THE DB
		 */

		// store/get the Material object into/from the DB
		QVariant matId;
		// matId = storeObj(mat, matName);
		matId = storeMaterial(mat);

		// store/get the Shape object into/from the DB
		QVariant shapeId;
		shapeId = storeShape(shape);
		// shapeId = storeObj(shape, shapeType, shapePars);

		// store/get the LogVol object into/from the DB
		QVariant logvolId;
		logvolId = storeObj(logVol, logName, shapeId, matId);

		if (dynamic_cast<const GeoPhysVol*>(vol)) {
			qDebug() << "New PhysVol, storing it...";
			const GeoPhysVol* physVol = dynamic_cast<const GeoPhysVol*>(vol);
			// store the PhysVol volume into the DB
			physId = storeObj(physVol, logvolId, parentId, storeRootVolume); // with parent info
            volTypeStr = "GeoPhysVol";
			qDebug() << "PhysVol stored. Id:" << physId.toString();
		}
		else if (dynamic_cast<const GeoFullPhysVol*>(vol)) {
			qDebug() << "New FullPhysVol, storing it...";
			const GeoFullPhysVol* fullVol = dynamic_cast<const GeoFullPhysVol*>(vol);
			// store the FullPhysVol volume into the DB
			physId = storeObj(fullVol, logvolId, parentId, storeRootVolume); // with parent info
            volTypeStr = "GeoFullPhysVol";
			qDebug() << "FullPhysVol stored. Id:" << physId.toString();
		} else {
			qWarning() << "WARNING!! Unknown GeoVPhysVol type!!";
		}

	} else {
		//	qDebug() << "Volume stored already. It is a shared volume. Taking ID from memory map and moving to its physical children...";
		qDebug() << "Volume stored already. It is a shared volume. Now, we are handling a 'copy' of it. We take the ID from memory map...";
		physId = getStoredIdFromAddress(address);
        volTypeStr = getGeoTypeFromVPhysVol(vol);
	}

    // Now we get the 'copy number' for this volume,
    // to distinguish this volume from the other volumes created from the same shared node (if any)
    if (volTypeStr == "NULL") qFatal("ERROR!! volTypeStr is 'NULL'!!!");
    const unsigned int volCopyN = setVolumeCopyNumber(physId.toString(), volTypeStr);
    qDebug() << "physId: " << physId << "- volume copy number: " << volCopyN;

    if ( isRootVolume || parentId == "NULL") {
        qDebug() << "This is the RootVolume or the volume has 'NULL' parent (unconnected subtree?) - So, we do not store the child position for this volume!";
    } else {
	    // store the parent-child relationship in the DB
	    QString parentType = getGeoTypeFromVPhysVol(parentNode);
        // get the copy number of the parent
        const unsigned int parentCopyN = getLatestParentCopyNumber(parentId.toString(), parentType);
	    qDebug() << "PhysVol Id:" << physId << " - copyNumber:" << QString::number(parentCopyN);

	    QString childType  = getGeoTypeFromVPhysVol(vol);

	    storeChildPosition(parentId, parentType, physId, parentCopyN, getChildPosition( parentId.toString(), parentType, parentCopyN), childType, volCopyN);
    }
}


QString DumpGeoModelActionLocal::getGeoTypeFromVPhysVol(const GeoVPhysVol* vol)
{
	if (!vol)
		return QString("NULL");

	QString geoType;
	if (dynamic_cast<const GeoPhysVol*>(vol)) {
		qDebug() << "GeoType: GeoPhysVol";
		geoType = "GeoPhysVol";
	}
	else if (dynamic_cast<const GeoFullPhysVol*>(vol)) {
		qDebug() << "GeoType: GeoFullPhysVol";
		geoType = "GeoFullPhysVol";
	} else {
		qWarning() << "WARNING!! Unknown GeoVPhysVol type!!";
	}
	return geoType;
}


void DumpGeoModelActionLocal::handleSerialDenominator (const GeoSerialDenominator *node)
{
	qDebug() << "\nDumpGeoModelActionLocal::handleSerialDenominator(GeoSerialDenominator*)";

	QString address = getAddressStringFromPointer( node );

	std::string baseNameStr = node->getBaseName();
	QString baseName = QString::fromStdString(baseNameStr);
	qDebug() << "base name:" << baseName << "address:" << address;

	// variables used to persistify the object
	QVariant sdId;

	// get the parent volume
	QStringList parentList = getParentNode();
	QString parentId = parentList[0];
	QString parentType = parentList[1];
	unsigned int parentCopyN = getLatestParentCopyNumber(parentId, parentType);

	// check if this object has been stored already
	if (! isAddressStored(address)) {

		qDebug() << "New SerialDenominator, storing it...";

		/* STORE THE OBJECT IN THE DB */
		sdId = storeObj(node, baseName);
		qDebug() << "SerialDenominator stored. Id:" << sdId.toString();

	} else {
		sdId = getStoredIdFromAddress(address);
		qDebug() << "SerialDenominator already stored in the DB. Id:" << sdId.toString();
	}

	storeChildPosition(parentId, parentType, sdId, parentCopyN, getChildPosition( parentId, parentType, parentCopyN ), "GeoSerialDenominator", 0); // TODO: Check if the copyN=0 at the end is OK for nodes as Transforms, which cannot be used as parents, only as children!
}



void DumpGeoModelActionLocal::handleSerialTransformer (const GeoSerialTransformer *node)
{
	qDebug() << "\nDumpGeoModelActionLocal::handleSerialTransformer(GeoSerialTransformer*)";

	QString address = getAddressStringFromPointer( node );

	// variables used to persistify the object
	QVariant functionId;
	QVariant physvolId;
	QVariant physvolTable;
	unsigned int nCopies;
	QVariant stId;

	// get the parent volume
	QStringList parentList = getParentNode();
	QString parentId = parentList[0];
	QString parentType = parentList[1];
	unsigned int parentCopyN = getLatestParentCopyNumber(parentId, parentType);


	// check if this object has been stored already
	if (! isAddressStored(address)) {

		qDebug() << "New SerialTransformer, storing it...";

		/*
		 * Get Node characteristics
		 *
		 * SerialTransformer features:
		 * - 1 parent PhysVol (if any)
		 * - 1 GENFUNCTION
		 * - 1 PhysVolId
		 * - 1 PhysVolType
		 * - 1 number of copies
		 */
		// get linked function and number of copies
		const GeoXF::Function * func = node->getFunction();
		nCopies =  node->getNCopies();
		qDebug() << "n. of copies:" << QString::number(nCopies);

		// get linked VPhysVol volume
		const GeoVPhysVol *vol = &( *( node->getVolume() ) );
		// const GeoPhysVol* vol = dynamic_cast<const GeoPhysVol*>(volV);
		QString volType;
		if (dynamic_cast<const GeoPhysVol*>(vol)) {
			volType = "GeoPhysVol";
		} else if (dynamic_cast<const GeoFullPhysVol*>(vol)) {
			volType = "GeoFullPhysVol";
		} else {
			qWarning() << "ERROR!!! Unknown VPhysVol type!!";
		}

		/*
		 * Persistify the Function
		 */
	 	TransFunctionPersistifier persistifier;
        try {
            persistifier.persistify(*func);
        } catch (const std::runtime_error & error) {
            std::cout << "SEVERE WARNING!! Handling std::runtime_error! -->" << error.what() << std::endl;
        }
		QString expression = QString::fromStdString( persistifier.getCodedString() );
		qDebug() << "FUNCTION:" << expression;

		if (expression.size() == 0) {
				qFatal("FATAL ERROR!! Function expression is empty!! Aborting...");
				abort();
		}


		/*
		 * STORE/GET THE INNER OBJECTS IN THE DB
		 */

		// store/get the Function object in the DB
		functionId = storeObj(func, expression);

		// store/get the PhysVol volume in the DB

		/* Concerning the physVol referenced by the SerialTransformer:
		 *
		 * - if physvol is not stored in the DB already;
		 * - if physVol has no parent (==NULL)
		 * ==> it means that its children will never visited by the NodeAction executed on the root tree
		 *
		 * ==> So, we need to persitify it as a new tree, to get all its children visited and persistified
		 *
		 */
		qDebug() << "Handling the referenced VPhysVol, going into the sub-tree, if any...";
		handleReferencedVPhysVol(vol);

		qDebug() << "Storing the referenced VPhysVol...";
		QString physvolAddress = getAddressStringFromPointer(vol);
		physvolId = getStoredIdFromAddress(physvolAddress);


		/*
		 * STORE THE NODE INTO THE DB AND ITS POSITION WITHIN THE TREE
		 */
		// store the SerialTransformer volume in the DB
		stId = storeObj(node, functionId, physvolId, volType, nCopies);
		qDebug() << "SerialTransformer stored. Id:" << stId.toString();

	} else {
		stId = getStoredIdFromAddress(address);
		qDebug() << "SerialTransformer already stored in the DB. Id:" << stId.toString();
	}

	qDebug() << "Storing:" << parentId << parentType << stId;
	storeChildPosition(parentId, parentType, stId, parentCopyN, getChildPosition( parentId, parentType, parentCopyN ), "GeoSerialTransformer", 0); // TODO: Check if the copyN=0 at the end is OK for nodes as Transforms, which cannot be used as parents, only as children!
}




void DumpGeoModelActionLocal::handleTransform(const GeoTransform* node)
{
	qDebug() << "\nDumpGeoModelActionLocal::handleTransform(GeoTransform*)";

	QString address = getAddressStringFromPointer( node );

	// get the parent volume
	QStringList parentList = getParentNode();
	QString parentId = parentList[0];
	QString parentType = parentList[1];

    unsigned int parentCopyN = getLatestParentCopyNumber(parentId, parentType);

	// // check the volume position in the geometry tree
	// GeoNodePath* path = getPath();
	// unsigned int len = path->getLength();
	// qDebug() << "length: " << len;
	//
	// // reset the number of visited node, if len is different than before
	// if (len > m_len) {
	// 	m_len_nChild = 0;
	// 	m_len = len;
	// }
	// // update number of visited nodes if len is the same as before
	// else {
	// 	++m_len_nChild;
	// }
	//
	//
	// QVariant parentId = "NULL";
	// QVariant trId;
	//
	//
	// // get the parent volume, if this is not the Root volume
	// const GeoVPhysVol* parentNode = nullptr;
	// if (len > 1) {
	// 	parentNode = path->getItem(len-1); // i=(len-1) gives you the parent VPhysVol
	// 	QString parentAddress = getAddressStringFromPointer(parentNode);
	// 	qDebug() << "parent's address:" << parentNode;
	//
	// 	if (isAddressStored(parentAddress))
	// 		parentId = getStoredIdFromAddress(parentAddress);
	// 	else {
	// 		// qFatal("FATAL ERROR!!! - The parent node of this child should has been stored in the DB already, but it was not found!!");
	// 		qWarning() << "The parent node of this transform seems to not having be stored in the DB yet. [It is normal if the transformation is used for example only in the definition of a 'GeoShapeShift' instance]";
	// 	}
	// 	qDebug() << "- parent's LogVol name:" << QString::fromStdString(parentNode->getLogVol()->getName());
	// } else{
	// 	qWarning() << "ERROR!! Len == 1, but this cannot be the Root volume!";
	// }

	/*
	 * STORE THE OBJECT IN THE DB AND ITS PLACE WITHIN THE TREE
	 */

	 // store the transformation in the DB
	 QVariant trId = storeTranform(node);

	// Store the child-parent relationship

	if ( dynamic_cast<const GeoAlignableTransform*>(node) ) {
		qDebug() << "Storing a GeoAlignableTransform...";
		storeChildPosition(parentId, parentType, trId, parentCopyN, getChildPosition( parentId, parentType, parentCopyN ), "GeoAlignableTransform", 0); // TODO: Check if the copyN=0 at the end is OK for nodes as Transforms, which cannot be used as parents, only as children!
	}
	else if ( dynamic_cast<const GeoTransform*>(node) ) {
		qDebug() << "Storing a GeoTransform...";
		storeChildPosition(parentId, parentType, trId, parentCopyN, getChildPosition( parentId, parentType, parentCopyN ), "GeoTransform", 0); // TODO: Check if the copyN=0 at the end is OK for nodes as Transforms, which cannot be used as parents, only as children!
	}
	else {
		qWarning("Not implemented yet!!!! ");
	}

}


void DumpGeoModelActionLocal::handleNameTag(const GeoNameTag* node)
{
	qDebug() << "\nDumpGeoModelActionLocal::handleNameTag(GeoNameTag*)";

	std::string nameStr = node->getName();
	QString name = QString::fromStdString(nameStr);
	qDebug() << "name:" << name;

	QString address = getAddressStringFromPointer( node );

	// get the parent volume
	QStringList parentList = getParentNode();
	QString parentId = parentList[0];
	QString parentType = parentList[1];
    unsigned int parentCopyN = getLatestParentCopyNumber(parentId, parentType);

    // TODO: add "if stored"...

	/*
	 * STORE THE OBJECT IN THE DB AND ITS POSITION WITHIN THE TREE
	 */

	 // store the name tag in the DB
	 QVariant nameId = storeObj(node, name);

	 // Store the child-parent relationship
	 storeChildPosition(parentId, parentType, nameId, parentCopyN, getChildPosition( parentId, parentType, parentCopyN ), "GeoNameTag", 0);
}



//__________________________________________________
QStringList DumpGeoModelActionLocal::getParentNode()
{
    qDebug() << "DumpGeoModelActionLocal::getParentNode()";

	// check the current volume position in the geometry tree
	GeoNodePath* path = getPath();
	unsigned int len = path->getLength();
	qDebug() << "length: " << len;

	// reset the number of visited node, if len is different than before
	if (len > m_len) {
		m_len_nChild = 0;
		m_len = len;
	}
	// update number of visited nodes if len is the same as before
	else {
		++m_len_nChild;
	}

	// get the parent volume, if this is not the Root volume
	const GeoVPhysVol* parentNode = nullptr;
	QVariant parentId = "NULL";
	QString parentType = "NULL";

		if (len >= 1)
		{
			// get the parent node
			parentNode = path->getItem(len-1); // i=(len-1) gives you the parent VPhysVol

			if (parentNode) {

				// get the parent type
				parentType = getGeoTypeFromVPhysVol(parentNode);

				// get the parent memory address
				QString parentAddress = getAddressStringFromPointer(parentNode);
				qDebug() << "-- parent's address:" << parentNode;

				// get the id of the parent node, which should be stored already in the DB
				if (isAddressStored(parentAddress)) {
					parentId = getStoredIdFromAddress(parentAddress);
				}
				else {
					// qFatal("FATAL ERROR!!! - The parent node of this child should has been stored in the DB already, but it was not found!!");
					qWarning() << "The parent node of this child node seems to not having be stored in the DB yet! [It is normal if it is the root volume or a transformation node used for example only in the definition of a 'GeoShapeShift' instance]";
				}
				qDebug() << "-- parent's LogVol name:" << QString::fromStdString(parentNode->getLogVol()->getName());
			}
		}
		else{
			// qWarning() << "ERROR!! Len == 1, but this cannot be the Root volume!";
			qWarning() << "WARNING!! Len == 0, but this cannot be the Root volume!";
		}

		QStringList parentList;
		parentList << parentId.toString() << parentType;

		return  parentList;
}


//__________________________________________________________________
QVariant DumpGeoModelActionLocal::storeShape(const GeoShape* shape)
{
	const QString shapeType = QString::fromStdString(shape->type());
	qDebug() << "storeShape() - shape name:" << shapeType  << ", address:" << shape;
	// get shape parameters
	QString shapePars = getShapeParameters(shape);
	// store the shape in the DB and returns the ID
	return storeObj(shape, shapeType, shapePars);
}


//______________________________________________________________________
QVariant DumpGeoModelActionLocal::storeMaterial(const GeoMaterial* mat)
{
	const QString matName = QString::fromStdString(mat->getName());
	qDebug() << "storeMaterial() - material name:" << matName << ", address:" << mat;
	// store the material in the DB and returns the ID
	return storeObj(mat, matName);
}

//_______________________________________________________________________
QVariant DumpGeoModelActionLocal::storeTranform(const GeoTransform* node)
{
	/*
	 * STORE THE OBJECT IN THE DB
	 */

	 QString address = getAddressStringFromPointer( node );

	 QVariant trId;

	// check if this object has been stored already
	if (! isAddressStored(address)) {

		qDebug() << "New Transform, storing it...";


		HepGeom::Transform3D tr = Amg::EigenTransformToCLHEP( node->getTransform() );

		std::vector<double> params = getTransformParameters(tr);
		qDebug() << "Transform parameters:" << QVector<double>::fromStdVector(params);

		/*
		 * STORE THE OBJECT IN THE DB
		 */

		// store the object in the DB
		if ( dynamic_cast<const GeoAlignableTransform*>(node) ) {
			trId = storeObj(dynamic_cast<const GeoAlignableTransform*>(node), params);
			qDebug() << "AlignableTransform stored. Id:" << trId.toString();
		}
		else if ( dynamic_cast<const GeoTransform*>(node) ) {
			trId = storeObj(dynamic_cast<const GeoTransform*>(node), params);
			qDebug() << "Transform stored. Id:" << trId.toString();
		}
		else {
			qDebug() << "WARNING!!! - This type of transformation needs to be customized yet!!";
		}

	} else {
		trId = getStoredIdFromAddress(address);
		qDebug() << "Transform already stored in the DB. Id:" << trId.toString();
	}

	return trId;
}


void DumpGeoModelActionLocal::handleReferencedVPhysVol (const GeoVPhysVol *vol)
{
	qDebug() << "\nDumpGeoModelActionLocal::handleReferencedVPhysVol(GeoVPhysVol*)";

	// qDebug() << "PhysVol's LogVol name:" << QString::fromStdString(vol->getLogVol()->getName());

	// get the address string for the current volume
	QString address = getAddressStringFromPointer( vol );

	QVariant parentId = "NULL";

	qDebug() << "setting 'unconnected' to: false";
	m_unconnectedTree = false;

	// get the parent volume, if this is not the Root volume
	const GeoVPhysVol* parentNode = dynamic_cast<const GeoVPhysVol*>( &(*(vol->getParent() )));

	if (parentNode) {
		QString parentAddress = getAddressStringFromPointer(parentNode);
		qDebug() << "--> parent's address:" << parentNode;

		if (isAddressStored(parentAddress))
			parentId = getStoredIdFromAddress(parentAddress);
		//		else
		//			qFatal("FATAL ERROR!!! - The parent node of this child should has been stored in the DB already, but it was not found!!");

		qDebug() << "--> parent's LogVol name:" << QString::fromStdString(parentNode->getLogVol()->getName());
	}
	else {
		qDebug() << "NULL parent node!! --> it seems to be an unconnected subtree.";
		qDebug() << "setting 'unconnected' to: true";
		m_unconnectedTree = true;
	}

	// // counting children
	// unsigned int nChildren = vol->getNChildVols();
	// qDebug() << "number of child physical volumes:" << nChildren;
	// qDebug() << "[number of PhysVol and SerialTransformer child nodes:" << vol->getNChildVolAndST() << "]";
	//
	// GeoCountVolAction cv;
	// cv.setDepthLimit(1);
	// vol->exec(&cv);
	// int nChildCount = cv.getCount();
	// qDebug() << "number of child volumes:" << nChildCount;



	// check if this object has been stored already
	if (! isAddressStored(address)) {

		qDebug() << "This is a new root PhysVol node of an 'unconnected' tree, so we start another action on it to dump it into the DB...";

		// Dump the tree volumes into the DB
		vol->exec(this); // TODO: check if the new action overwrites the id of the volumes already in the DB...!!!

	} else {
		qDebug() << "The referenced volume has been stored already. Skipping...";
	}
	// at the end, we make sure we reset the status
	qDebug() << "setting 'unconnected' to: false";
	m_unconnectedTree = false;
}


// Get shape parameters
QString DumpGeoModelActionLocal::getShapeParameters(const GeoShape* shape)
{
	const QString shapeType = QString::fromStdString(shape->type());
	qDebug() << "shapeType:" << shapeType;

	QString shapePars = "";

	if (shapeType == "Box") {
		qDebug() << "get GeoBox parameters";
		QStringList pars;
		const GeoBox* box = dynamic_cast<const GeoBox*>(shape);
		pars << "XHalfLength=" + QString::number(box->getXHalfLength()) ;
		pars << "YHalfLength=" + QString::number(box->getYHalfLength()) ;
		pars << "ZHalfLength=" + QString::number(box->getZHalfLength()) ;
		shapePars = pars.join(";");
	} else if (shapeType == "Cons") {
		QStringList pars;
		const GeoCons* shapeIn = dynamic_cast<const GeoCons*>(shape);
		pars << "RMin1=" + QString::number(shapeIn->getRMin1()) ;
		pars << "RMin2=" + QString::number(shapeIn->getRMin2()) ;
		pars << "RMax1=" + QString::number(shapeIn->getRMax1()) ;
		pars << "RMax2=" + QString::number(shapeIn->getRMax2()) ;
		pars << "DZ=" + QString::number(shapeIn->getDZ()) ;
		pars << "SPhi=" + QString::number(shapeIn->getSPhi()) ;
		pars << "DPhi=" + QString::number(shapeIn->getDPhi()) ;
		shapePars = pars.join(";");
	} else if (shapeType == "Para") {
		QStringList pars;
		const GeoPara* shapeIn = dynamic_cast<const GeoPara*>(shape);
		pars << "XHalfLength=" + QString::number(shapeIn->getXHalfLength()) ;
		pars << "YHalfLength=" + QString::number(shapeIn->getYHalfLength()) ;
		pars << "ZHalfLength=" + QString::number(shapeIn->getZHalfLength()) ;
		pars << "Alpha=" + QString::number(shapeIn->getAlpha()) ;
		pars << "Theta=" + QString::number(shapeIn->getTheta()) ;
		pars << "Phi=" + QString::number(shapeIn->getPhi()) ;
		shapePars = pars.join(";");
	} else if (shapeType == "Pcon") {
		QStringList pars;
		const GeoPcon* shapeIn = dynamic_cast<const GeoPcon*>(shape);
		pars << "SPhi=" + QString::number(shapeIn->getSPhi());
		pars << "DPhi=" + QString::number(shapeIn->getDPhi());
		// get number of Z planes and loop over them
		const int nZplanes = shapeIn->getNPlanes();
		pars << "NZPlanes=" + QString::number(nZplanes);
		for (int i=0; i<nZplanes; ++i) {
			pars << "ZPos=" + QString::number(shapeIn->getZPlane(i));
			pars << "ZRmin=" + QString::number(shapeIn->getRMinPlane(i));
			pars << "ZRmax=" + QString::number(shapeIn->getRMaxPlane(i));
		}
		shapePars = pars.join(";");
	} else if (shapeType == "Pgon") {
		QStringList pars;
		const GeoPgon* shapeIn = dynamic_cast<const GeoPgon*>(shape);
		pars << "SPhi=" + QString::number(shapeIn->getSPhi()) ;
		pars << "DPhi=" + QString::number(shapeIn->getDPhi()) ;
		pars << "NSides=" + QString::number(shapeIn->getNSides()) ;
		// get number of Z planes and loop over them
		const int nZplanes = shapeIn->getNPlanes();
		pars << "NZPlanes=" + QString::number(nZplanes);
		for (int i=0; i<nZplanes; ++i) {
			pars << "ZPos=" + QString::number(shapeIn->getZPlane(i));
			pars << "ZRmin=" + QString::number(shapeIn->getRMinPlane(i));
			pars << "ZRmax=" + QString::number(shapeIn->getRMaxPlane(i));
		}
		shapePars = pars.join(";");
	} else if (shapeType == "SimplePolygonBrep") {
		QStringList pars;
		const GeoSimplePolygonBrep* shapeIn = dynamic_cast<const GeoSimplePolygonBrep*>(shape);
		pars << "DZ=" + QString::number(shapeIn->getDZ()) ;
		// get number of vertices and loop over them
		const int nVertices = shapeIn->getNVertices();
		pars << "NVertices=" + QString::number(nVertices);
		for (int i=0; i<nVertices; ++i) {
			pars << "xV=" + QString::number(shapeIn->getXVertex(i));
			pars << "yV=" + QString::number(shapeIn->getYVertex(i));
		}
		shapePars = pars.join(";");
	} else if (shapeType == "Trap") {
		QStringList pars;
		const GeoTrap* shapeIn = dynamic_cast<const GeoTrap*>(shape);
		pars << "ZHalfLength=" + QString::number(shapeIn->getZHalfLength()) ;
		pars << "Theta=" + QString::number(shapeIn->getTheta()) ;
		pars << "Phi=" + QString::number(shapeIn->getPhi()) ;
		pars << "Dydzn=" + QString::number(shapeIn->getDydzn()) ;
		pars << "Dxdyndzn=" + QString::number(shapeIn->getDxdyndzn()) ;
		pars << "Dxdypdzn=" + QString::number(shapeIn->getDxdypdzn()) ;
		pars << "Angleydzn=" + QString::number(shapeIn->getAngleydzn()) ;
		pars << "Dydzp=" + QString::number(shapeIn->getDydzp()) ;
		pars << "Dxdyndzp=" + QString::number(shapeIn->getDxdyndzp()) ;
		pars << "Dxdypdzp=" + QString::number(shapeIn->getDxdypdzp()) ;
		pars << "Angleydzp=" + QString::number(shapeIn->getAngleydzp()) ;
		shapePars = pars.join(";");
	} else if (shapeType == "Trd") {
		QStringList pars;
		const GeoTrd* shapeIn = dynamic_cast<const GeoTrd*>(shape);
		pars << "XHalfLength1=" + QString::number(shapeIn->getXHalfLength1()) ;
		pars << "XHalfLength2=" + QString::number(shapeIn->getXHalfLength2()) ;
		pars << "YHalfLength1=" + QString::number(shapeIn->getYHalfLength1()) ;
		pars << "YHalfLength2=" + QString::number(shapeIn->getYHalfLength2()) ;
		pars << "ZHalfLength=" + QString::number(shapeIn->getZHalfLength()) ;
		shapePars = pars.join(";");
	} else if (shapeType == "Tube") {
		QStringList pars;
		const GeoTube* tube = dynamic_cast<const GeoTube*>(shape);
		pars << "RMin=" + QString::number(tube->getRMin()) ;
		pars << "RMax=" + QString::number(tube->getRMax()) ;
		pars << "ZHalfLength=" + QString::number(tube->getZHalfLength()) ;
		shapePars = pars.join(";");
	} else if (shapeType == "Tubs") {
		QStringList pars;
		const GeoTubs* shapeIn = dynamic_cast<const GeoTubs*>(shape);
		pars << "RMin=" + QString::number(shapeIn->getRMin()) ;
		pars << "RMax=" + QString::number(shapeIn->getRMax()) ;
		pars << "ZHalfLength=" + QString::number(shapeIn->getZHalfLength()) ;
		pars << "SPhi=" + QString::number(shapeIn->getSPhi()) ;
		pars << "DPhi=" + QString::number(shapeIn->getDPhi()) ;
		shapePars = pars.join(";");
	}
	else if (shapeType == "Intersection") {
		qDebug() << "get GeoShapeIntersection parameters";
		QStringList pars;
		const GeoShapeIntersection* shapeIn = dynamic_cast<const GeoShapeIntersection*>(shape);
		// get the referenced Shape used in the 'union' operation, store it in the DB
		const GeoShape* shapeOpA = shapeIn->getOpA();
		QVariant shapeIdA = storeShape(shapeOpA);
		const GeoShape* shapeOpB = shapeIn->getOpB();
		QVariant shapeIdB = storeShape(shapeOpB);
		pars << "opA=" + QString::number( shapeIdA.toUInt() ) ;
		pars << "opB=" + QString::number( shapeIdB.toUInt() ) ;
		shapePars = pars.join(";");
	}
	else if (shapeType == "Shift") {
		qDebug() << "get GeoShapeShift parameters";
		QStringList pars;
		const GeoShapeShift* shapeIn = dynamic_cast<const GeoShapeShift*>(shape);

		// get the referenced Shape used in the 'shift' operation, store it in the DB
		const GeoShape* shapeOp = shapeIn->getOp();
		QVariant shapeId = storeShape(shapeOp);

		// get the Transformation, store it in the DB
		GeoTransform* transf = new GeoTransform( shapeIn->getX() );
		QVariant trId = storeTranform(transf);

		pars << "A=" + QString::number( shapeId.toUInt() ) ;
		pars << "X=" + QString::number( trId.toUInt() ) ;
		shapePars = pars.join(";");
	}
	else if (shapeType == "Subtraction") {
		qDebug() << "get GeoShapeSubtraction parameters";
		QStringList pars;
		const GeoShapeSubtraction* shapeIn = dynamic_cast<const GeoShapeSubtraction*>(shape);
		// get the referenced Shape used in the 'union' operation, store it in the DB
		const GeoShape* shapeOpA = shapeIn->getOpA();
		QVariant shapeIdA = storeShape(shapeOpA);
		const GeoShape* shapeOpB = shapeIn->getOpB();
		QVariant shapeIdB = storeShape(shapeOpB);
		pars << "opA=" + QString::number( shapeIdA.toUInt() ) ;
		pars << "opB=" + QString::number( shapeIdB.toUInt() ) ;
		shapePars = pars.join(";");
	}
	else if (shapeType == "Union") {
		qDebug() << "get GeoShapeUnion parameters";
		QStringList pars;
		const GeoShapeUnion* shapeIn = dynamic_cast<const GeoShapeUnion*>(shape);

		// get the referenced Shape used in the 'union' operation, store it in the DB
		const GeoShape* shapeOpA = shapeIn->getOpA();
		QVariant shapeIdA = storeShape(shapeOpA);
		const GeoShape* shapeOpB = shapeIn->getOpB();
		QVariant shapeIdB = storeShape(shapeOpB);

		pars << "opA=" + QString::number( shapeIdA.toUInt() ) ;
		pars << "opB=" + QString::number( shapeIdB.toUInt() ) ;
		shapePars = pars.join(";");
	}
	else {
		qDebug() << "WARNING!!! - Shape '" << shapeType << "' needs to be customized!!";
	}

  return shapePars;

}


std::vector<double> DumpGeoModelActionLocal::getTransformParameters(HepGeom::Transform3D tr)
{
	std::vector<double> vec;

	vec.push_back(tr.xx());
	vec.push_back(tr.xy());
	vec.push_back(tr.xz());

	vec.push_back(tr.yx());
	vec.push_back(tr.yy());
	vec.push_back(tr.yz());

	vec.push_back(tr.zx());
	vec.push_back(tr.zy());
	vec.push_back(tr.zz());

	vec.push_back(tr.dx());
	vec.push_back(tr.dy());
	vec.push_back(tr.dz());

	return vec;
}


DumpGeoModelActionLocal::DumpGeoModelActionLocal(GMDBManager &db)
{
	qDebug() << "DumpGeoModelActionLocal: constructor";

	// init variables
	m_len = 0;
	m_len_nChild = 0;
	m_rootVolumeFound = false;
	m_unconnectedTree = false;

	// init anche check the database handle
	m_dbManager = &db;
	if (m_dbManager->isOpen()) {
		qDebug() << "OK! Database is open!";
	}
	else {
		qDebug() << "Database is not open!";
	}

    m_dbpath = m_dbManager->getDBFilePath();

	// get DB metadata
	m_memMap_Tables = m_dbManager->getAll_NodeTypesTableIDs();
	// qDebug() << "m_memMap_Tables:" << m_memMap_Tables;

}

DumpGeoModelActionLocal::~DumpGeoModelActionLocal()
{
	// nothing to do so far...
}


void DumpGeoModelActionLocal::showMemoryMap()
{
	qDebug() << "DumpGeoModelActionLocal::showMemoryMap()";

	QMap<QString, QVariant>::const_iterator it = m_memMap.constBegin();
	while (it != m_memMap.constEnd()) {
		std::cout << it.key().toStdString() << ": " << it.value().toString().toStdString() << std::endl;
		++it;
	}
}



QVariant DumpGeoModelActionLocal::storeObj(const GeoMaterial* pointer, const QString name)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoMaterial*) - name:" << name << "address:" << pointer;

	QString address = getAddressStringFromPointer( pointer );
	QVariant materialId;

	if (! isAddressStored(address)) {
		qDebug() << "New Material! Storing it...";

		// materialId = m_dbManager->addMaterial(name);
		materialId = addMaterial(name);

		storeAddress( address, materialId );
	}
	else {
		qDebug() << "Material node stored already. Getting ID from the memory map...";
		materialId = getStoredIdFromAddress(address);
	}
	qDebug() << "materialId:" << materialId;
	return materialId;
}


QVariant DumpGeoModelActionLocal::storeObj(const GeoShape* pointer, const QString name, const QString parameters)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoShape*) - name:" << name << "address:" << pointer;

	QString address = getAddressStringFromPointer( pointer );

	QVariant shapeId;
	if (! isAddressStored(address)) {
		qDebug() << "New Shape! Storing it...";

		// shapeId = m_dbManager->addShape(name, parameters);
		shapeId = addShape(name, parameters);

		storeAddress( address, shapeId);
	}
	else {
		qDebug() << "Shape node stored already. Getting ID from the memory map...";
		shapeId = getStoredIdFromAddress(address);
	}
	qDebug() << "shapeId:" << shapeId;
	return shapeId;
}

QVariant DumpGeoModelActionLocal::storeObj(const GeoLogVol* pointer, const QString name, const QVariant shapeId, const QVariant materialId)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoLogVol*) - name:" << name << "address:" << pointer;

	QString address = getAddressStringFromPointer( pointer );

	QVariant logvolId;
	if (! isAddressStored(address)) {
		qDebug() << "New LogVol! Storing it...";

		// logvolId = m_dbManager->addLogVol(name, shapeId, materialId);
		logvolId = addLogVol(name, shapeId, materialId);

		storeAddress( address, logvolId );
	}
	else {
		qDebug() << "LogVol node stored already. Getting ID from the memory map...";
		logvolId = getStoredIdFromAddress(address);
	}
	qDebug() << "logvolId:" << logvolId;
	return logvolId;
}


QVariant DumpGeoModelActionLocal::storeObj(const GeoPhysVol* pointer, const QVariant logvolId, const QVariant parentId, bool isRootVolume)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoPhysVol*) - address:" << pointer << "- is root volume?" << isRootVolume;

	QString address = getAddressStringFromPointer( pointer );

	QVariant physvolId;
	if (! isAddressStored(address)) {
		qDebug() << "New PhysVol! Storing it...";

		// physvolId = m_dbManager->addPhysVol(logvolId, parentId, isRootVolume);
		physvolId = addPhysVol(logvolId, parentId, isRootVolume);

		storeAddress( address, physvolId );
	}
	else {
		qDebug() << "PhysVol node stored already. Getting ID from the memory map...";
		physvolId = getStoredIdFromAddress(address);
	}
	qDebug() << "physvolId:" << physvolId;
	return physvolId;
}

QVariant DumpGeoModelActionLocal::storeObj(const GeoFullPhysVol* pointer, const QVariant logvolId, const QVariant parentId, bool isRootVolume)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoFullPhysVol*) - address:" << pointer << "- is root volume?" << isRootVolume;

	QString address = getAddressStringFromPointer( pointer );

	QVariant physvolId;
	if (! isAddressStored(address)) {
		qDebug() << "New FullPhysVol! Storing it...";

		// physvolId = m_dbManager->addFullPhysVol(logvolId, parentId, isRootVolume);
		physvolId = addFullPhysVol(logvolId, parentId, isRootVolume);

		storeAddress( address, physvolId );
	}
	else {
		qDebug() << "FullPhysVol node stored already. Getting ID from the memory map...";
		physvolId = getStoredIdFromAddress(address);
	}
	qDebug() << "fullphysvolId:" << physvolId;
	return physvolId;
}

QVariant DumpGeoModelActionLocal::storeObj(const GeoSerialDenominator* pointer, const QString baseName)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoSerialDenominator*) - baseName:" << baseName << "address:" << pointer;

	QString address = getAddressStringFromPointer( pointer );
	QVariant id;

	if (! isAddressStored(address)) {
		qDebug() << "New SerialDenominator! Storing it...";

		// id = m_dbManager->addSerialDenominator(baseName);
		id = addSerialDenominator(baseName);

		storeAddress( address, id );
	}
	else {
		qDebug() << "SerialDenominator node stored already. Getting ID from the memory map...";
		id = getStoredIdFromAddress(address);
	}
	qDebug() << "ID:" << id;
	return id;
}


QVariant DumpGeoModelActionLocal::storeObj(const GeoSerialTransformer* pointer, const QVariant functionId, const QVariant volId, const QString volType, const unsigned int copies)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoSerialTransformer*):" << volId.toUInt() << volType << "- n. of copies: " << copies;

	QString address = getAddressStringFromPointer( pointer );
	QVariant id;

	if (! isAddressStored(address)) {
		qDebug() << "New SerialTransformer! Storing it...";

		// id = m_dbManager->addSerialTransformer(functionId, volId, volType, copies);
		id = addSerialTransformer(functionId, volId, volType, copies);

		storeAddress( address, id );
	}
	else {
		qDebug() << "SerialTransformer node stored already. Getting ID from the memory map...";
		id = getStoredIdFromAddress(address);
	}
	qDebug() << "ID:" << id;
	return id;
}

QVariant DumpGeoModelActionLocal::storeObj(const GeoXF::Function* pointer, const QString expression)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoXF::Function*)";

	QString address = getAddressStringFromPointer( pointer );
	QVariant id;

	if (! isAddressStored(address)) {
		qDebug() << "New GeoXF::Function! Storing it...";

		// id = m_dbManager->addFunction(expression);
		id = addFunction(expression);

		storeAddress( address, id );
	}
	else {
		qDebug() << "GeoXF::Function node stored already. Getting ID from the memory map...";
		id = getStoredIdFromAddress(address);
	}
	qDebug() << "ID:" << id;
	return id;
}

QVariant DumpGeoModelActionLocal::storeObj(const GeoTransform* pointer, std::vector<double> parameters)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoTransform*)";


	QString address = getAddressStringFromPointer( pointer );
	QVariant id;

	if (! isAddressStored(address)) {
		qDebug() << "New GeoXF::Function! Storing it...";

		// id = m_dbManager->addTransform( QVector<double>::fromStdVector(parameters) );
		id = addTransform( parameters );

		storeAddress( address, id );
	}
	else {
		qDebug() << "GeoTransform node stored already. Getting ID from the memory map...";
		id = getStoredIdFromAddress(address);
	}
	qDebug() << "ID:" << id;
	return id;
}

QVariant DumpGeoModelActionLocal::storeObj(const GeoAlignableTransform* pointer, std::vector<double> parameters)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoAlignableTransform*)";


	QString address = getAddressStringFromPointer( pointer );
	QVariant id;

	if (! isAddressStored(address)) {
		qDebug() << "New GeoXF::Function! Storing it...";

		// id = m_dbManager->addAlignableTransform( QVector<double>::fromStdVector(parameters) );
		id = addAlignableTransform( parameters );

		storeAddress( address, id );
	}
	else {
		qDebug() << "GeoAlignableTransform node stored already. Getting ID from the memory map...";
		id = getStoredIdFromAddress(address);
	}
	qDebug() << "ID:" << id;
	return id;
}


QVariant DumpGeoModelActionLocal::storeObj(const GeoNameTag* pointer, const QString name)
{
	qDebug() << "DumpGeoModelActionLocal::storeObj(GeoNameTag*) - name:" << name << "address:" << pointer;

	QString address = getAddressStringFromPointer( pointer );
	QVariant id;

	if (! isAddressStored(address)) {
		qDebug() << "New SerialDenominator! Storing it...";

		// id = m_dbManager->addNameTag(name);
		id = addNameTag(name);

		storeAddress( address, id );
	}
	else {
		qDebug() << "SerialDenominator node stored already. Getting ID from the memory map...";
		id = getStoredIdFromAddress(address);
	}
	qDebug() << "ID:" << id;
	return id;
}

/*
// OLD // FIXME: to be removed when all objects will be migrated to the new version
void DumpGeoModelActionLocal::storeChildPosition(const QVariant parentId, const QString parentType, const QVariant childId, const unsigned int childPos, const QString childType)
{
	qDebug() << "DumpGeoModelActionLocal::storeChildPosition()";
	// m_dbManager->addChildPosition(parentId, parentType, childId, childPos, childType);
	// if ()
	addChildPosition(parentId, parentType, childId, childPos, childType);
	return;
}
*/
// NEW
void DumpGeoModelActionLocal::storeChildPosition(const QVariant parentId, const QString parentType, const QVariant childId, const unsigned int parentCopyN, const unsigned int childPos, const QString childType, const unsigned int childCopyN)
{
	qDebug() << "DumpGeoModelActionLocal::storeChildPosition()";
	addChildPosition(parentId, parentType, childId, parentCopyN, childPos, childType, childCopyN); // FIXME: change the positions of the parameters to a more logical order, like: parentID, parentType, parentCopyN, childPos, ChildType, childId, childCopyN
	return;
}


unsigned int DumpGeoModelActionLocal::addRecord(std::vector<QStringList>* container, const QStringList values) const
{
	container->push_back(values);
	unsigned int idx = container->size(); // index of pushed element = size after pushing, to match ID starting at 1 in the DB
	return idx;
}

QVariant DumpGeoModelActionLocal::addMaterial(const QString name)
{
	qDebug() << "DumpGeoModelActionLocal::addMaterial(QString*) - name:" << name;
	std::vector<QStringList>* container = &m_materials;
	QStringList values;
	values << name;
	return QVariant( addRecord(container, values) );
}

QVariant DumpGeoModelActionLocal::addNameTag(const QString name)
{
	qDebug() << "DumpGeoModelActionLocal::addNameTag(QString*) - name:" << name;
	std::vector<QStringList>* container = &m_nameTags;
	QStringList values;
	values << name;
	return QVariant( addRecord(container, values) );
}

QVariant DumpGeoModelActionLocal::addSerialDenominator(const QString &baseName)
{
	qDebug() << "DumpGeoModelActionLocal::addSerialDenominator(QString*) - basename:" << baseName;
	std::vector<QStringList>* container = &m_serialDenominators;
	QStringList values;
	values << baseName;
	return QVariant( addRecord(container, values) );
}


QVariant DumpGeoModelActionLocal::addFunction(const QString expression)
{
	qDebug() << "DumpGeoModelActionLocal::addFunction(QString*) - expression:" << expression;
	std::vector<QStringList>* container = &m_functions;
	QStringList values;
	values << expression;
	return QVariant( addRecord(container, values) );
}


QVariant DumpGeoModelActionLocal::addAlignableTransform(const std::vector<double> params)
{
	qDebug() << "DumpGeoModelActionLocal::addAlignableTransform(QString*)";
	std::vector<QStringList>* container = &m_alignableTransforms;
	QStringList values;
	foreach(double par, params) {
		values << QString::number(par);
	}
	return QVariant( addRecord(container, values) );
}



QVariant DumpGeoModelActionLocal::addTransform(const std::vector<double> params)
{
	qDebug() << "DumpGeoModelActionLocal::addTransform(QString*)";
	std::vector<QStringList>* container = &m_transforms;
	QStringList values;
	foreach(double par, params) {
		values << QString::number(par);
	}
	return QVariant( addRecord(container, values) );
}

QString DumpGeoModelActionLocal::getIdFromNodeType( QString nodeType )
{
        qDebug() << "getIdFromNodeType(" << nodeType <<")";
		if (m_memMap_Tables.contains(nodeType))
			return QString::number(m_memMap_Tables.value(nodeType));
		else
			return QString("NULL");
}

QVariant DumpGeoModelActionLocal::addSerialTransformer(const QVariant &funcId, const QVariant &physvolId, const QString volType, const unsigned int &copies)
{
	qDebug() << "DumpGeoModelActionLocal::addSerialTransformer()";
	std::vector<QStringList>* container = &m_serialTransformers;
	QString volTypeID = getIdFromNodeType(volType);

	QStringList values;
	values << funcId.toString() << physvolId.toString() << volTypeID << QString::number(copies);

	return QVariant( addRecord(container, values) );
}

QVariant DumpGeoModelActionLocal::addShape(const QString &type, const QString &parameters)
{
	std::vector<QStringList>* container = &m_shapes;
	QStringList values;
	values << type << parameters;
	return QVariant( addRecord(container, values) );
}


QVariant DumpGeoModelActionLocal::addPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume)
{
	std::vector<QStringList>* container = &m_physVols;

	QStringList values;
	values << logVolId.toString() << parentPhysVolId.toString(); // TODO: we should remove the parent info: it's not complete because the type is missing (PhysVol or FullPhysVol) and it's redundant, because we store the childrenPositions. It's only useful for quick visual debug, by dumping the PhysVol DB table

	unsigned int idx = addRecord(container, values);

	if (isRootVolume) {
		QStringList rootValues;
		rootValues << QString::number(idx) << "GeoPhysVol";
		m_rootVolume = rootValues;
	}
	return QVariant(idx);
}


QVariant DumpGeoModelActionLocal::addFullPhysVol(const QVariant &logVolId, const QVariant &parentPhysVolId, bool isRootVolume)
{
	std::vector<QStringList>* container = &m_fullPhysVols;

	QStringList values;
	values << logVolId.toString() << parentPhysVolId.toString(); // TODO: we should remove the parent info: it's not complete because the type is missing (PhysVol or FullPhysVol) and it's redundant, because we store the childrenPositions. It's only useful for quick visual debug, by dumping the PhysVol DB table

	unsigned int idx = addRecord(container, values);

	if (isRootVolume) {
		QStringList rootValues;
		rootValues << QString::number(idx) << "GeoFullPhysVol";
		m_rootVolume = rootValues;
	}
	return QVariant(idx);
}

QVariant DumpGeoModelActionLocal::addLogVol(const QString &name, const QVariant &shapeId, const QVariant &materialId)
{
	std::vector<QStringList>* container = &m_logVols;
	QStringList values;
	values << name << shapeId.toString() << materialId.toString();
	return QVariant( addRecord(container, values) );
}

/*
// OLD // FIXME: to be removed when all objects will be migrated to the new version
void DumpGeoModelActionLocal::addChildPosition(const QVariant parentId, const QString parentType, const QVariant childId, const unsigned int childPos, const QString childType)
{
	std::vector<QStringList>* container = &m_childrenPositions;

	QString parentTableID = getIdFromNodeType(parentType);
	QString childTableID = getIdFromNodeType(childType);

	QStringList values;
	values << parentId.toString() << parentTableID << QString("") << QString::number(childPos) << childTableID << childId.toString() << QString("");
	addRecord(container, values);
	return;
}
*/

// NEW
void DumpGeoModelActionLocal::addChildPosition(const QVariant parentId, const QString parentType, const QVariant childId, const unsigned int parentCopyN, const unsigned int childPos, const QString childType, const unsigned int childCopyN)
{
	std::vector<QStringList>* container = &m_childrenPositions;

	QString parentTableID = getIdFromNodeType(parentType);
	QString childTableID = getIdFromNodeType(childType);

	QStringList values;
	values << parentId.toString() << parentTableID <<  QString::number(parentCopyN) << QString::number(childPos) << childTableID << childId.toString() << QString::number(childCopyN);
	addRecord(container, values);
	return;
}


void DumpGeoModelActionLocal::saveToDB()
{
	qDebug() << "DumpGeoModelActionLocal::savetoDB()";
    std::cout << "saving to file: " << m_dbpath.toStdString() << std::endl;

	m_dbManager->addListOfRecords("GeoMaterial", m_materials);
	m_dbManager->addListOfRecords("GeoNameTag", m_nameTags);
	m_dbManager->addListOfRecords("GeoAlignableTransform", m_alignableTransforms);
	m_dbManager->addListOfRecords("GeoTransform", m_transforms);
	m_dbManager->addListOfRecords("Function", m_functions);
	m_dbManager->addListOfRecords("GeoSerialTransformer", m_serialTransformers);
	m_dbManager->addListOfRecords("GeoShape", m_shapes);
	m_dbManager->addListOfRecords("GeoSerialDenominator", m_serialDenominators);
	m_dbManager->addListOfRecords("GeoPhysVol", m_physVols);
	m_dbManager->addListOfRecords("GeoFullPhysVol", m_fullPhysVols);
	m_dbManager->addListOfRecords("GeoLogVol", m_logVols);

	m_dbManager->addListOfChildrenPositions(m_childrenPositions);
	m_dbManager->addRootVolume(m_rootVolume);

	return;
}






void DumpGeoModelActionLocal::storeAddress(const QString address, QVariant id)
{
	qDebug() << "DumpGeoModelActionLocal::storeAddress(" << address << "," << id << ")";
	m_memMap.insert(address, id);
}

bool DumpGeoModelActionLocal::isAddressStored(const QString address)
{
	qDebug() << "DumpGeoModelActionLocal::isAddressStored(): " << address;
	//showMemoryMap(); // only for Debug
	return m_memMap.contains(address);
}


QVariant DumpGeoModelActionLocal::getStoredIdFromAddress(QString address)
{
	qDebug() << "DumpGeoModelActionLocal::getStoredIdFromAddress(): " << address;
	return m_memMap.value(address);
}

// get pointer string
QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoMaterial* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoMaterial*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}
// get pointer string
QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoShape* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoShape*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}
// get pointer string
QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoLogVol* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoLogVol*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}
// get pointer string
QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoPhysVol* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoPhysVol*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}
QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoVPhysVol* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoVPhysVol*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}
// get pointer string
QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoSerialDenominator* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoSerialDenominator*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}
QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoSerialTransformer* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoSerialTransformer*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}
QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoXF::Function* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoXF::Function*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}

QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoTransform* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoTransform*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}

QString DumpGeoModelActionLocal::getAddressStringFromPointer(const GeoNameTag* pointer)
{
	qDebug() << "DumpGeoModelActionLocal::getAddressStringFromPointer(GeoNameTag*)";
	std::ostringstream oss;
	oss << pointer;
	return getQStringFromOss(oss);
}

QString DumpGeoModelActionLocal::getQStringFromOss(std::ostringstream &oss)
{
	qDebug() << "DumpGeoModelActionLocal::getQStringFromOss()";
	std::string addr = oss.str();
	QString address = QString::fromStdString(addr);
	qDebug() << "address string:" << address;
	return address;
}


//bool DumpGeoModelActionLocal::isObjectStored(const GeoMaterial* pointer)
//{
//	qDebug() << "DumpGeoModelActionLocal::isObjectStored(const GeoMaterial*)";
//	return isAddressStored(getAddressStringFromPointer( pointer) );
//}
//
//bool DumpGeoModelActionLocal::isObjectStored(const GeoGraphNode* pointer)
//{
//	qDebug() << "DumpGeoModelActionLocal::isObjectStored(const GeoGraphNode*)";
////	return isAddressStored(address);
//}



// void DumpGeoModelActionLocal::handlePhysVol (const GeoPhysVol *vol)
// {
// 	qDebug() << "\nDumpGeoModelActionLocal::handlePhysVol(GeoPhysVol*)";
//
// 	// get the address string for the current volume
// 	QString address = getAddressStringFromPointer( vol );
//
// 	// variables used to persistify the object
// 	QVariant parentId;
// 	QVariant physId;
//
// 	// check the volume position in the geometry tree
// 	GeoNodePath* path = getPath();
// 	unsigned int len = path->getLength();
// 	qDebug() << "length: " << len;
// 	//	std::cout << "path: " << path << std::endl;
//
// //	const GeoVPhysVol* parentNode;
//
// 	// this below is performed until the root volume is found, then "length" is not used anymore,
// 	// because not updated when recursively visiting PhysVols
// 	bool getParentNode = true;
// 	bool storeRootVolume = false;
// 	if ( ! m_rootVolumeFound) {
// 		if (len > 1) {
// 			getParentNode = true;
// 		} else{
// 			qDebug() << "This is the Root volume!";
// 			m_rootVolumeFound = true;
// 			storeRootVolume = true;
// 			getParentNode = false;
// 		}
// 	} else {
// 		getParentNode = true;
// 	}
//
// 	// get the parent volume, if this is not the Root volume
// 	if (getParentNode) {
//
// 		const GeoPhysVol* parentNode = dynamic_cast<const GeoPhysVol*>( &(*(vol->getParent() )));
//
// 		if (parentNode) {
// 			QString parentAddress = getAddressStringFromPointer(parentNode);
// 			qDebug() << "parent's address:" << parentNode;
//
// 			if (isAddressStored(parentAddress))
// 				parentId = getStoredIdFromAddress(parentAddress);
// 			//		else
// 			//			qFatal("FATAL ERROR!!! - The parent node of this child should has been stored in the DB already, but it was not found!!");
//
// 			qDebug() << "parent's LogVol name:" << QString::fromStdString(parentNode->getLogVol()->getName());
// 		}
// 		else {
// 			qDebug() << "NULL parent node!!";
// 		}
// 	}
//
// 	// counting children
// 	unsigned int nChildren = vol->getNChildVols();
// 	qDebug() << "number of child physical volumes:" << nChildren;
// 	qDebug() << "[number of PhysVol and SerialTransformer child nodes:" << vol->getNChildVolAndST() << "]";
//
// 	GeoCountVolAction cv;
// 	cv.setDepthLimit(1);
// 	vol->exec(&cv);
// 	int nChildCount = cv.getCount();
// 	qDebug() << "number of child volumes:" << nChildCount;
//
//
//
// 	// check if this object has been stored already
// 	if (! isAddressStored(address)) {
//
// 		qDebug() << "New PhysVol, storing it...";
//
// 		/*
// 		 * PhysVol features:
// 		 * - 1 parent PhysVol (if any)
// 		 * - 1 LogVol
// 		 * - 1 Material
// 		 * - 1 Shape
// 		 */
//
// 		// Note: PhysVol has no name. Its LogVol has a name.
// 		//	  const std::string name = vol->getLogVol()->getName();
//
// 		// LOGVOL
// 		const GeoLogVol* logVol = vol->getLogVol();
// 		const QString logName = QString::fromStdString(logVol->getName());
// 		qDebug() << "LogVol name:"  << logName;
//
//
// 		// MATERIAL
// 		const GeoMaterial * mat = vol->getLogVol()->getMaterial();
// 		const QString matName = QString::fromStdString(mat->getName());
// 		qDebug() << "material name:" << matName << ", address:" << mat;
//
//
// 		// SHAPE
// 		const GeoShape * shape = vol->getLogVol()->getShape();
// 		const QString shapeType = QString::fromStdString(shape->type());
// 		qDebug() << "shape name:" << shapeType  << ", address:" << shape;
//
// 		// shape parameters
// 		QString shapePars = "";
// 		if (shapeType == "Box") {
// 			QStringList pars;
// 			const GeoBox* box = dynamic_cast<const GeoBox*>(shape);
// 			pars << "XHalfLength=" + QString::number(box->getXHalfLength()) ;
// 			pars << "YHalfLength=" + QString::number(box->getYHalfLength()) ;
// 			pars << "ZHalfLength=" + QString::number(box->getZHalfLength()) ;
// 			shapePars = pars.join(";");
// 		}
//
// 		/*
// 		 * STORE THE OBJECTS IN THE DB
// 		 */
//
// 		// store/get the Material object into/from the DB
// 		QVariant matId;
// 		matId = storeObj(mat, matName);
//
// 		// store/get the Shape object into/from the DB
// 		QVariant shapeId;
// 		shapeId = storeObj(shape, shapeType, shapePars);
//
// 		// store/get the LogVol object into/from the DB
// 		QVariant logvolId;
// 		logvolId = storeObj(logVol, logName, shapeId, matId);
//
// 		// store the PhysVol volume into the DB
// 		physId = storeObj(vol, logvolId, parentId, storeRootVolume); // with parent info
// 		qDebug() << "PhysVol stored. Id:" << physId.toString();
//
// 	} else {
// 		//	qDebug() << "Volume stored already. It is a shared volume. Taking ID from memory map and moving to its physical children...";
// 		qDebug() << "Volume stored already. It is a shared volume. Taking ID from memory map...";
// 		physId = getStoredIdFromAddress(address);
// 		qDebug() << "PhysVol Id:" << physId;
// 	}
//
//
// 	// store the parent-child relationship in the DB
// 	storeChildPosition(parentId, physId, getChildPosition( parentId.toUInt() ), "GeoPhysVol");
//

	/* Now we need to loop over children, to be sure all volumes are take.
	 That's because it seems that the action only goes to children that are 'attached'
	 to the tree with a parent-child relationship. For example I have a pInnerPassive PhysVol
	 added to a pPassive PhysVol; the latter is refeneced in a SerialTransformer.
	 pPassive is visited because of our implementation of the handleSerialTrasformer() method,
	 but the pInnerPassive is never visited.
	 */
	//	if (nChildren > 0) {
	//		qDebug() << "Looping over the children...";
	//
	//		for (unsigned int i=0; i<nChildren; ++i) {
	//
	//			unsigned int childPos = i;
	//
	//			// get the name of the i-th child volume
	//			QString childName =  QString::fromStdString(vol->getNameOfChildVol(i));
	//			qDebug() << "\t" << i << " childName:" << childName;
	//
	//			//	Returns the i-th child volume
	//
	//			// GEOPHYSVOL child
	//			if ( dynamic_cast<const GeoPhysVol*>( &(*(vol->getChildVol(i) ))) ) {
	//				qDebug() << "\t\t" << "the child" << i << "is a GeoPhysVol";
	//				const GeoVPhysVol *childVolV = &(*(vol->getChildVol(i)));
	//				const GeoPhysVol* childVol = dynamic_cast<const GeoPhysVol*>(childVolV);
	//
	//				handlePhysVol(childVol); // recursive call
	//
	//			} else {
	//				qCritical("\t\tERROR!! the child is not a persistified volume/node!!!");
	//				return;
	//			}
	//		}
	//	}

	//	if (nChildren > 0) {
	//		for (unsigned int i=0; i<nChildren; ++i) {
	//
	//			unsigned int childPos = i;
	//
	//			// get the name of the i-th child volume
	//			QString childName =  QString::fromStdString(vol->getNameOfChildVol(i));
	//			qDebug() << "\t" << i << " childName:" << childName;
	//
	//			//	Returns the i-th child volume
	//
	//			// GEOPHYSVOL child
	//			if ( dynamic_cast<const GeoPhysVol*>( &(*(vol->getChildVol(i) ))) ) {
	//				qDebug() << "\t\t" << "the child" << i << "is a GeoPhysVol";
	//				const GeoVPhysVol *childVolV = &(*(vol->getChildVol(i)));
	//				const GeoPhysVol* childVol = dynamic_cast<const GeoPhysVol*>(childVolV);
	//
	//
	//				// check if child volume has been stored already. If not, store it.
	//				if ( ! isAddressStored( getAddressStringFromPointer( childVol )) ) {
	//
	//					// MATERIAL
	//					const GeoMaterial * mat = childVol->getLogVol()->getMaterial();
	//					const QString matName = QString::fromStdString(mat->getName());
	//					qDebug() << "Child material name:" << matName;
	//
	//					// SHAPE
	//					const GeoShape * shape = childVol->getLogVol()->getShape();
	//					const QString shapeType = QString::fromStdString(shape->type());
	//					qDebug() << "Child shape name:" << shapeType;
	//
	//					// LOGVOL
	//					const GeoLogVol* logVol = childVol->getLogVol();
	//					const QString logName = QString::fromStdString(logVol->getName());
	//					qDebug() << "Child LogVol name:"  << logName;
	//
	//
	//					/*
	//					 * STORE THE OBJECTS IN THE DB
	//					 */
	//
	//					// store the Material object in the DB
	//					QVariant child_matId = storeObj(mat, matName);
	//
	//					// store the Shape object in the DB
	//					QVariant child_shapeId = storeObj(shape, shapeType);
	//
	//					// store the LogVol volume in the DB
	//					QVariant child_logvolId = storeObj(logVol, logName, child_shapeId, child_matId);
	//
	//					// store the PhysVol volume in the DB
	//					storeObj(childVol, child_logvolId, physId);
	//
	//					// store the parent-child relationship in the DB
	//					storeChildPosition(physId, childVol, childPos);
	//
	//				} else {
	//					qDebug() << "Child volume stored already. Moving to the next one...";
	//				}
	//
	//			}
	//			// GeoSerialDenominator child
	//			else if ( dynamic_cast<const GeoSerialDenominator*>( &(*(vol->getChildVol(i) ))) ) {
	//				qDebug() << "\t\t" << "the child" << i << "is a GeoSerialDenominator";
	//				//qWarning() << "\t\t" << "to be implemented!!!";
	//				const GeoVPhysVol *childVolV = &(*(vol->getChildVol(i)));
	//				const GeoSerialDenominator* childVol = dynamic_cast<const GeoSerialDenominator*>(childVolV);
	//
	//				std::string baseNameStr = childVol->getBaseName();
	//				QString baseName = QString::fromStdString(baseNameStr);
	//
	//				QString address = getAddressStringFromPointer( childVol );
	//
	//				qDebug() << "base name:" << baseName << "address:" << address;
	//
	//				QVariant sdId;
	//
	//				// check if this object has been stored already
	//				if (! isAddressStored(address)) {
	//					// store the SerialDenominator object in the DB
	//					qDebug() << "New SerialDenominator, storing it...";
	//					sdId = storeObj(childVol, baseName);
	//					qDebug() << "SerialDenominator stored. Id:" << sdId.toString();
	//
	//				} else {
	//					sdId = getStoredIdFromAddress(address);
	//					qDebug() << "SerialDenominator already stored in the DB. Got id:" << sdId.toString();
	//				}
	//
	//				// store the parent-child relationship in the DB
	//				storeChildPosition(physId, childVol, childPos);
	//
	//			}
	//			// GEOFULLPHYSVOL child
	//			else if ( dynamic_cast<const GeoFullPhysVol*>( &(*(vol->getChildVol(i) ))) ) {
	//				qDebug() << "\t\t" << "the child" << i << "is a GeoFullPhysVol";
	//				qWarning() << "\t\t" << "to be implemented!!!";
	//				//				const GeoVFullPhysVol *childVolV = &(*(vol->getChildVol(i)));
	//				//				const GeoFullPhysVol* childVol = dynamic_cast<const GeoFullPhysVol*>(childVolV);
	//			}
	//			else {
	//				qCritical("\t\tERROR!! the child is not a persistified volume/node!!!");
	//				return;
	//			}
	//
	//
	//			// examples:
	//			//const GeoVPhysVol *child=&(*(pv->getChildVol(i)));
	//			//
	//			//if ( dynamic_cast<const GeoLogVol*>( &(*(vol->getChildVol(i) ))) )
	//			//	qDebug() << "\t\t" << "the child" << i << "is a GeoLogVol";
	//
	//
	//
	//			/*
	//				unsigned int nchildvol = pvc->getNChildVols();
	//				     int lgg = 0;
	//				     int llay = 0;
	//				     std::string::size_type npos;
	//				     for (unsigned ich=0; ich<nchildvol; ++ich) {
	//				       PVConstLink pc = pvc->getChildVol(ich);
	//				       std::string childname = (pc->getLogVol())->getName();
	//				       if ((npos = childname.find("layer")) != std::string::npos ) {
	//				         llay ++;
	//				         //std::cerr<<" navigating RPC named "
	//				         //       <<tname<<" child "
	//				         //       <<ich<<" is a layer with tag "<<llay<<std::endl;
	//				         unsigned int nch1 = pc->getNChildVols();
	//				         lgg = 0;
	//				         for (unsigned ngv=0; ngv<nch1; ++ngv) {
	//				           PVConstLink pcgv = pc->getChildVol(ngv);
	//				           std::string childname1 = (pcgv->getLogVol())->getName();
	//				           if ((npos = childname1.find("gas volume")) != std::string::npos ) {
	//				         lgg ++;
	//				         //std::cerr<<" navigating RPC named "
	//				         //       <<tname<<" child "
	//				         //       <<ngv<<" is a gas volume  with tag "<<lgg<<std::endl;
	//				         PVConstLink pcgg = pcgv->getChildVol(0);
	//				         HepGeom::Transform3D trans = pvc->getXToChildVol(ich)*pc->getXToChildVol(ngv)*pcgv->getXToChildVol(0);
	//				         m_Xlg[llay-1][lgg-1] = Amg::CLHEPTransformToEigen(trans);
	//				           }
	//				         }
	//				       }
	//				     }
	//			 */
	//
	//		}
	//	}


	//	else {
	//		qDebug() << "Object stored already. Moving to next one...";
	//	}
// }

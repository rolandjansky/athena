/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
 * GeoModelReadIn.cpp
 *
 *  Created on: May 20, 2016
 *      Author: rbianchi
 */

// local includes
#include "GeoRead/GReadIn.h"

// SQLite
//#include "sqlite/sqlite3.h"
//#include <sqlite3.h>

// TFPersistification includes
#include "TFPersistification/TransFunctionInterpreter.h"

// GeoModelKernel
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoSerialTransformer.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoGraphNode.h"

// GeoModel shapes
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

// Units
#include "GeoModelKernel/Units.h"
#define SYSTEM_OF_UNITS GeoModelKernelUnits // so we will get, e.g., 'GeoModelKernelUnits::cm'

// TODO: to remove once the migration to Eigen is complete
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h" // TODO: to be removed when dropping CLHEP

//VP1Base
// TODO: we should get rid of VP1Base::VP1Msg dependency, since GeoRead should not depend on VP1 packages. Maybe we can move VP1Msg to a standalone package.
//#include "VP1Base/VP1Msg.h"

// Qt includes
#include <QDebug>


using namespace GeoGenfun;
using namespace GeoXF;


namespace GeoModelPers {

GReadIn::GReadIn(GMDBManager* db, unsigned long* progress) : m_progress(nullptr), m_deepDebug(false)
{
	qDebug() << "DumpGeoModelAction: constructor";

	#ifdef GEOREAD_DEEP_DEBUG
	  m_deepDebug = true;
 	#endif

        if ( progress != nullptr) {
	    m_progress = progress;
	}

	// set the geometry file
	m_dbManager = db;
	if (m_dbManager->isOpen()) {
		qDebug() << "OK! Database is open!";
	}
	else {
		qWarning() << "ERROR!! Database is NOT open!";
		return;
	}
}

GReadIn::~GReadIn() {
	// TODO Auto-generated destructor stub
}


///*
// * Arguments:
// *
// *   unused - Ignored in this case, see the documentation for sqlite3_exec
// *    count - The number of columns in the result set
// *     data - The row's data
// *  columns - The column names
// */
//int GReadIn::dbCallbackBuild(void* unused, int nColRow, char **fieldsRow, char **colNamesRow){
//
//	GReadIn* instance = (GReadIn*) unused;
//
//	for(int i=0; i<nColRow; i++){
//
//		std::string colName = colNamesRow[i];
//		std::string fieldRow = fieldsRow[i];
//
//		printf("Building %s = %s\n", colNamesRow[i], fieldsRow[i] ? fieldsRow[i] : "NULL");
//
//		if ( colName == "SHAPE" ) {
//
//			if (fieldRow == "Box")
//				instance->buildShape();
//		}
//
//	}
//	printf("\n");
//	return 0;
//}

// void GReadIn::buildShape()
// {
// 	//	const GeoBox *box = new GeoBox(1200*CLHEP::cm,1200*CLHEP::cm, 1200*CLHEP::cm);
// }




// void GReadIn::printRecords()
// {
// 	// FETCH RECORDS AND PRINT THEM
//
// }



GeoPhysVol* GReadIn::buildGeoModel()
{
	qDebug() << "GReadIn::buildGeoModel()";

	// return buildGeoModelByCalls();

	GeoPhysVol* rootVolume = buildGeoModelOneGo();

	if (m_unknown_shapes.size() > 0) {
		qWarning() << "WARNING!! There were unknwon shapes:";
		for ( auto it = m_unknown_shapes.begin(); it != m_unknown_shapes.end(); it++ ) {
			std::cout << "---> " << *it << std::endl;
		}
		std::cout << "Remember: unknown shapes are rendered with a dummy cube of 30cm side length." << std::endl;
		}

	return rootVolume;
}

/*
GeoPhysVol* GReadIn::buildGeoModelByCalls()
{
	QStringList rootValues = m_dbManager->getRootPhysVol();
	//	qDebug() << "rootValues:" << rootValues;

	QString nodeType = rootValues.takeFirst(); // it takes out the first element

	// get GeoModel node. Depth == -1 means "get all children volumes / all levels"
	// m_root = dynamic_cast<GeoPhysVol*>(parseVPhysVol(rootValues, nodeType, -1));
	m_root = dynamic_cast<GeoPhysVol*>(parseVPhysVol(rootValues, nodeType));

	// TODO: check if shared nodes are restored only one and then used when needed,
	// or they are restored with a new node in memory every time they are met

	return m_root;
}
*/

GeoPhysVol* GReadIn::buildGeoModelOneGo()
{
	qDebug() << "GReadIn::buildGeoModelOneGo()";

	// get all objects from the DB
	m_physVols = m_dbManager->getTableFromNodeType("GeoPhysVol");
	std::cout << "GeoPhysVol, read in." << std::endl;
	m_fullPhysVols = m_dbManager->getTableFromNodeType("GeoFullPhysVol");
	std::cout << "GeoFullPhysVol, read in." << std::endl;
	m_logVols = m_dbManager->getTableFromNodeType("GeoLogVol");
	std::cout << "GeoLogVol, read in." << std::endl;
	m_shapes = m_dbManager->getTableFromNodeType("GeoShape");
	std::cout << "GeoShape, read in." << std::endl;
	m_materials = m_dbManager->getTableFromNodeType("GeoMaterial");
	std::cout << "GeoMaterial, read in." << std::endl;
	m_functions = m_dbManager->getTableFromNodeType("Function");
	std::cout << "Function, read in." << std::endl;
	m_serialDenominators = m_dbManager->getTableFromNodeType("GeoSerialDenominator");
	std::cout << "GeoSerialDenominator, read in." << std::endl;
	m_serialTransformers = m_dbManager->getTableFromNodeType("GeoSerialTransformer");
	std::cout << "GeoSerialTransformer, read in." << std::endl;
	m_alignableTransforms = m_dbManager->getTableFromNodeType("GeoAlignableTransform");
	std::cout << "GeoAlignableTransform, read in." << std::endl;
	m_transforms = m_dbManager->getTableFromNodeType("GeoTransform");
	std::cout << "GeoTransform, read in." << std::endl;
	m_nameTags = m_dbManager->getTableFromNodeType("GeoNameTag");
	std::cout << "GeoNameTag, read in." << std::endl;
	// qDebug() << "physVols: " << m_physVols;
	// qDebug() << "fullPhysVols: " << m_fullPhysVols;

	// get DB metadata
	m_tableid_tableName = m_dbManager->getAll_TableIDsNodeTypes();
	std::cout << "DB metadata, read in." << std::endl;

	// get the children table from DB
	m_allchildren = m_dbManager->getChildrenTable();
	// qDebug() << "all children from DB:" << m_allchildren;
	std::cout << "children positions, read in." << std::endl;

	// get the root volume data
	m_root_vol_data = m_dbManager->getRootPhysVol();
	std::cout << "root volume data, read in." << std::endl;

	return loopOverAllChildren();
}


// //----------------------------------------
// GeoPhysVol* GReadIn::loopOverAllChildren()
// {
// 	QMessageBox msgBox;
// 	msgBox.setText("The document has been modified.");
// 	msgBox.exec();
// }

//----------------------------------------
GeoPhysVol* GReadIn::loopOverAllChildren()
{

  std::cout << "Looping over all children to build the GeoModel tree..." << std::endl;

  int nChildrenRecords = m_allchildren.size();

  // This should go in VP1Light, not in this library. The library could be used by standalone apps without a GUI
  /*
    QProgressDialog progress("Loading the geometry...", "Abort Loading", 0, nChildrenRecords, 0);
    progress.setWindowModality(Qt::WindowModal);
    progress.show();
  */

  // loop over parents' keys
  int counter = 0;
  foreach (const QString &parentKey, m_allchildren.keys() ) {

    /* //This should go in VP1Light as well!
       if (progress.wasCanceled()) {
       std::cout << "You aborted the loading of the geometry." << std::endl;

       QMessageBox msgBox;
       msgBox.setText("You aborted the loading of the geometry.");
       msgBox.setInformativeText("Do you want to really abort it?");
       msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
       msgBox.setDefaultButton(QMessageBox::No);
       int ret = msgBox.exec();

       switch (ret) {
       case QMessageBox::Yes:
       // Abort the loading of the geometry
       break;
       case QMessageBox::No:
       // Continue with the loading of the geometry
       progress.reset();
       default:
       // should never be reached
       break;
       }
       }
    */



    ++counter;
    std::cout.precision(0);
    if ( nChildrenRecords < 10000 && counter % 500 == 0 ) {
      float done = ( (float)counter / nChildrenRecords) * 100;
      std::cout << "\t" << std::fixed << counter << "children records processed [" << done << "%]" << std::endl;
      if ( m_progress != nullptr  ) {
        //progress.setValue(counter); // This hould go in VP1Light
        *m_progress = counter;
      }
    }
    else if ( nChildrenRecords > 10000 && counter % 2000 == 0 ) {
      float done = ( (float)counter / nChildrenRecords) * 100;
      std::cout << "\t" << std::fixed << counter << " children records processed [" << done << "%]" << std::endl;
      if ( m_progress != nullptr ) {
        //progress.setValue(counter); // This should go in VP1Light
        *m_progress = counter;
      }
    }
    if (m_deepDebug) qDebug() << "\nparent: " << parentKey << ':' << m_allchildren.value(parentKey) << "[parentId, parentType, parentCopyNumber, childPos, childType, childId, childCopyN]";

    // get the parent's details
    QStringList parentKeyItems = parentKey.split(":");
    QString parentId = parentKeyItems[0];
    QString parentTableId = parentKeyItems[1];
    QString parentCopyN = parentKeyItems[2];
    if (m_deepDebug) qDebug() << "parent ID:" << parentId << ", parent table ID:" << parentTableId << ", parent copy number:" << parentCopyN;

    bool isRootVolume = false;
    if (parentId == "NULL") {
      isRootVolume = true;
    }

    GeoVPhysVol* parentVol = nullptr;

    // build or get parent volume.
    // Using the parentCopyNumber here, to get a given instance of the parent volume
    if (!isRootVolume) {
      if (m_deepDebug) qDebug() << "get the parent volume...";
      parentVol = buildVPhysVol( parentId, parentTableId, parentCopyN);
    }


    // get the parent's children
    QMap<unsigned int, QStringList> children = m_allchildren.value(parentKey);



    // loop over children, sorted by child position automatically
    // "id", "parentId", "parentTable", "parentCopyNumber", "position", "childTable", "childId", "childCopyNumber"
    if (m_deepDebug) qDebug() << "parent volume has " << children.size() << "children. Looping over them...";
    foreach(QStringList child, children) {

      if (m_deepDebug) qDebug() << "child:" << child;

      // build or get child node
      QString childTableId = child[5];
      QString childId = child[6];
      QString childCopyN = child[7];

      QString childNodeType = m_tableid_tableName[childTableId.toUInt()];

      if (m_deepDebug) qDebug() << "childTableId:" << childTableId << ", type:" << childNodeType << ", childId:" << childId;

      if (childNodeType.isEmpty()) {
        qWarning("ERROR!!! childNodeType is empty!!! Aborting...");
        exit(1);
      }

      if (childNodeType == "GeoPhysVol") {
        if (m_deepDebug) qDebug() << "GeoPhysVol child...";
        GeoVPhysVol* childNode = dynamic_cast<GeoPhysVol*>(buildVPhysVol(childId, childTableId, childCopyN));
        if (!isRootVolume) volAddHelper(parentVol, childNode);
      }
      else if (childNodeType == "GeoFullPhysVol") {
        if (m_deepDebug) qDebug() << "GeoFullPhysVol child...";
        GeoVPhysVol* childNode = dynamic_cast<GeoFullPhysVol*>(buildVPhysVol(childId, childTableId, childCopyN));
        if (!isRootVolume) volAddHelper(parentVol, childNode);
      }
      else if (childNodeType == "GeoSerialDenominator") {
        if (m_deepDebug) qDebug() << "GeoSerialDenominator child...";
        GeoSerialDenominator* childNode = buildSerialDenominator(childId);
        if (!isRootVolume) volAddHelper(parentVol, childNode);
      }
      else if (childNodeType == "GeoAlignableTransform") {
        if (m_deepDebug) qDebug() << "GeoAlignableTransform child...";
        GeoAlignableTransform* childNode = buildAlignableTransform(childId);
        if (!isRootVolume) volAddHelper(parentVol, childNode);
      }
      else if (childNodeType == "GeoTransform") {
        if (m_deepDebug) qDebug() << "GeoTransform child...";
        GeoTransform* childNode = buildTransform(childId);
        if (!isRootVolume) volAddHelper(parentVol, childNode);
      }
      else if (childNodeType == "GeoSerialTransformer") {
        if (m_deepDebug) qDebug() << "GeoSerialTransformer child...";
        GeoSerialTransformer* childNode = buildSerialTransformer(childId);
        if (!isRootVolume) volAddHelper(parentVol, childNode);
      }
      else if (childNodeType == "GeoNameTag") {
        if (m_deepDebug) qDebug() << "GeoNameTag child...";
        GeoNameTag* childNode = buildNameTag(childId);
        if (!isRootVolume) volAddHelper(parentVol, childNode);
      }
      else {
        QString msg = "[" + childNodeType + "]" + QString(" ==> ERROR!!! - The conversion for this type of child node needs to be implemented, still!!!");
        qFatal("%s", msg.toLatin1().constData());
      }

    } // loop over all children
  } // loop over childrenPositions records

  // return the root volume
  return getRootVolume();
}


// GReadIn::buildNode( QStringList volParams, QString nodeType )
// {
// 		if( nodeType == "GeoPhysVol") {
//
// 		} else {
// 			qWarning() << "WARNING!! Node type Unknown!!";
// 		}
// }





// GeoPhysVol* GReadIn::getGeoModelHandle()
// {
// 	QStringList rootValues = m_dbManager->getRootPhysVol();
// 	//	qDebug() << "rootValues:" << rootValues;
//
// 	QString nodeType = rootValues.takeFirst(); // it takes out the first element
//
// 	// get GeoModel node. Depth == 1 means "get only first level children"
// 	m_root = dynamic_cast<GeoPhysVol*>(parseVPhysVol(rootValues, nodeType, 1));
//
// 	// TODO: check if shared nodes are restored only one and then used when needed,
// 	// or they are restored with a new node in memory every time they are met
//
// 	return m_root;
// }

/*
  GeoVPhysVol* GReadIn::parseChildren(GeoVPhysVol* vol, QMap<unsigned int, QStringList> children, int depth)
  {
  qDebug() << "GReadIn::parseChildren()";

  //qDebug() << "children:" << children;

  // get parent volume type
  QString volType;
  if (dynamic_cast<GeoPhysVol*>(vol))
  volType = "GeoPhysVol";
  else if (dynamic_cast<GeoFullPhysVol*>(vol))
  volType = "GeoFullPhysVol";
  else
  qWarning() << "ERROR!! Unknown vol type!!";

  QMap<unsigned int, QStringList>::const_iterator it; // we don't modify the map, so we use a const_iterator, which is is slightly faster as well.

  for ( it = children.begin(); it != children.end(); ++it) {
  qDebug() << "\nchild item:" << it.key() << ":" << it.value();

  // QString id = it.value()[0];
  // QString parentId = it.value()[1];
  // QString parentTableId = it.value()[2];
  // QString childPosition = it.value()[3];
  unsigned int childTableId = it.value()[4].toUInt();
  unsigned int childId = it.value()[5].toUInt();

  // QString parentTable = m_dbManager->getTableNameFromTableId(parentTableId.toUInt());

  QStringList child = m_dbManager->getItemAndType(childTableId, childId );
  qDebug() << "child:" << child;

  QString nodeType = child.takeFirst(); // it pops out the first element, leaving the other items in the list

  if (depth > 0) {
  depth = depth - 1; // decrease the depth by one
  }

  if (nodeType == "GeoPhysVol") {
  // if ( childId != parentId.toUInt() ) {
  if (child.length() > 0) {
  // build the node
  GeoPhysVol* volChild = dynamic_cast<GeoPhysVol*>(parseVPhysVol(child, nodeType, depth));
  // add the node to the parent
  volAddHelper(vol, volChild);
  } else {
  qWarning() << "WARNING!!!" << "The PhysVol" << childId << "is empty/not-existing!!! To be investigated why...! Skipping now...";
  }
  // } else {
  // 	qWarning() << "WARNING!!" << "childId and parentId are the same! That means an infinite loop!! To be investigated! Skipping now...";
  // }
  }
  else if (nodeType == "GeoFullPhysVol") {
  if (child.length() > 0) {
  GeoFullPhysVol* volChild = dynamic_cast<GeoFullPhysVol*>(parseVPhysVol(child, nodeType, depth));
  volAddHelper(vol, volChild);
  } else {
  qWarning() << "WARNING!!!" << "The PhysVol" << childId << "is empty/not-existing!!! To be investigated why...! Skipping now...";
  }
  }
  else if (nodeType == "GeoSerialDenominator") {
  GeoSerialDenominator* volChild = parseSerialDenominator(child);
  volAddHelper(vol, volChild);
  // vol->add(volChild);
  }
  else if (nodeType == "GeoAlignableTransform") {
  GeoAlignableTransform* volChild = parseAlignableTransform(child);
  volAddHelper(vol, volChild);
  // vol->add(volChild);
  }
  else if (nodeType == "GeoTransform") {
  GeoTransform* volChild = parseTransform(child);
  volAddHelper(vol, volChild);
  // vol->add(volChild);
  }
  else if (nodeType == "GeoSerialTransformer") {
  GeoSerialTransformer* volChild = parseSerialTransformer(child);
  volAddHelper(vol, volChild);
  // vol->add(volChild);
  }
  else if (nodeType == "GeoNodeTag") { // TODO: implement it!!
  QString msg = nodeType + QString(" GeoNodeTag needs to be implemented!!!");
  qWarning() << msg;
  }
  else {
  QString msg = nodeType + QString(" child-->GeoModel conversion for this type still needs to be implemented!!!");
  qWarning() << msg;
  }

  }
  // this below is to suppress the compiler warning,
  // but it should not be reached, so we use a qFatal
  qFatal("This code should not be reached!!! Aborting...");
  GeoVPhysVol* volnull = nullptr;
  return volnull;
  }
*/

void GReadIn::volAddHelper(GeoVPhysVol* vol, GeoGraphNode* volChild)
{
  if (dynamic_cast<GeoPhysVol*>(vol)) {
    GeoPhysVol* volume = dynamic_cast<GeoPhysVol*>(vol);
    volume->add(volChild);
  } else if (dynamic_cast<GeoFullPhysVol*>(vol)) {
    GeoFullPhysVol* volume = dynamic_cast<GeoFullPhysVol*>(vol);
    volume->add(volChild);
  }
}

/*
// Instantiate a PhysVol and get its children
GeoVPhysVol* GReadIn::parseVPhysVol(QStringList values, QString nodeType, int depth)
{
qDebug() << "GReadIn::parseVPhysVol()";

QString volId = values[0];
QString logVolId = values[1];
QString parentId = values[2];

qDebug() << "\tPhysVol-ID:" << volId;
qDebug() << "\tPhysVol-LogVol:" << logVolId;
qDebug() << "\tPhysVol-parentId:" << parentId;
qDebug() << "\tnodeType:" << nodeType;


// GET LOGVOL
// get logVol properties from the DB
QStringList paramsLog = m_dbManager->getItem("GeoLogVol", logVolId.toUInt());
qDebug() << "params:" << paramsLog;

// build the LogVol
GeoLogVol* logVol = parseLogVol(paramsLog);

// a pointer to the VPhysVol
GeoVPhysVol* vol = nullptr;

// BUILD THE PHYSVOL OR THE FULLPHYSVOL
if (nodeType == "GeoPhysVol")
vol = new GeoPhysVol(logVol);
else if (nodeType == "GeoFullPhysVol")
vol = new GeoFullPhysVol(logVol);
else
qWarning() << "ERROR!!! Unkonwn node type!! : " << nodeType;


// if we want to get the volume's children
if (depth != 0) {

// * check if the volume has children
// get the IDs of all children of this PhysVol volume
QMap<unsigned int, QStringList> children = m_dbManager->getVPhysVolChildren( volId.toUInt(), nodeType );
qDebug() << "children of the " << nodeType << "node with Id:" << volId << " --> " << children;
// if this PhysVol has children, build them and add them to it
if (children.size() > 0)
parseChildren(vol, children, depth);
else
qDebug() << "VPhysVol has no children";
}

return vol;
}
*/


void GReadIn::checkInputString(QString input)
{
  if (input.isEmpty() || input.isNull() || input == "NULL") {
    qWarning() << "ERROR!!! Input QString is empty or equal to 'NULL'!!! Aborting...";
    exit(1);
  }
}

// Instantiate a PhysVol and get its children
GeoVPhysVol* GReadIn::buildVPhysVol(QString id, QString tableId, QString copyN)
{
  if (m_deepDebug) qDebug() << "GReadIn::buildVPhysVol()" << id << tableId << copyN;

  checkInputString(id);
  checkInputString(tableId);

  // if previously built, return that
  if (isNodeBuilt(id, tableId, copyN)) {
    if (m_deepDebug) qDebug() << "getting the volume from memory...";
    return dynamic_cast<GeoVPhysVol*>(getNode(id, tableId, copyN));
  }

  if (m_deepDebug) qDebug() << "building a new volume...";

  // QString nodeType = m_dbManager->getNodeTypeFromTableId(tableId.toUInt());
  QString nodeType = m_tableid_tableName[tableId.toUInt()];

  // get the parent volume parameters
  // here we do not need to use copyN, since the actual volume is the same for all instances
  QStringList values;
  if (nodeType == "GeoPhysVol")
    values = m_physVols[id.toUInt()];
  else if (nodeType == "GeoFullPhysVol")
    values = m_fullPhysVols[id.toUInt()];



  QString volId = values[0];
  QString logVolId = values[1];
  //QString parentId = values[2]; // FIXME: delete it, it is not used any more

  if (m_deepDebug) {
    qDebug() << "\tPhysVol-ID:" << volId;
    qDebug() << "\tPhysVol-LogVol:" << logVolId;
    //qDebug() << "\tPhysVol-parentId:" << parentId;
    qDebug() << "\tnodeType:" << nodeType;
  }

  // GET LOGVOL
  GeoLogVol* logVol = buildLogVol(logVolId);

  // a pointer to the VPhysVol
  GeoVPhysVol* vol = nullptr;

  // BUILD THE PHYSVOL OR THE FULLPHYSVOL
  if (nodeType == "GeoPhysVol")
    vol = new GeoPhysVol(logVol);
  else if (nodeType == "GeoFullPhysVol")
    vol = new GeoFullPhysVol(logVol);
  else
    qWarning() << "ERROR!!! Unkonwn node type!! : " << nodeType;

  // storing the address of the newly built node
  storeNode(id, tableId, copyN, vol);

  return vol;
}


// Get the root volume
GeoPhysVol* GReadIn::getRootVolume()
{
  if (m_deepDebug) qDebug() << "GReadIn::getRootVolume()";
  QString id = m_root_vol_data[1];
  QString tableId = m_root_vol_data[2];
  QString copyNumber = "1"; // the Root volume has only one copy by definition
  return dynamic_cast<GeoPhysVol*>(buildVPhysVol(id, tableId, copyNumber));
}



/*
  GeoMaterial* GReadIn::parseMaterial(QStringList values)
  {
  qDebug() << "GReadIn::parseMaterial()";

  QString matId = values[0];
  QString matName = values[1];

  qDebug() << "\tMaterial-ID:" << matId << ", Material-name:" << matName;

  // TODO: Bogus densities.  Later: read from database.
  double densityOfAir=0.1;

  return new GeoMaterial(matName.toStdString(),densityOfAir);

  }
*/


GeoMaterial* GReadIn::buildMaterial(QString id)
{
  if (m_deepDebug) qDebug() << "GReadIn::buildMaterial()";
  QStringList values = m_materials[id.toUInt()];

  QString matId = values[0];
  QString matName = values[1];

  if (m_deepDebug) qDebug() << "\tMaterial-ID:" << matId << ", Material-name:" << matName;

  // TODO: Bogus densities.  Later: read from database.
  double densityOfAir=0.1;

  return new GeoMaterial(matName.toStdString(),densityOfAir);

}


/*
  GeoShape* GReadIn::parseShape(QStringList values)
  {
  qDebug() << "GReadIn::parseShape()";

  QString id = values[0];
  QString type = values[1];
  QString parameters = values[2];

  qDebug() << "\tShape-ID:" << id << ", Shape-type:" << type;

  if (type == "Box") {

  // needed parameters
  double XHalfLength;
  double YHalfLength;
  double ZHalfLength;

  // get parameters from DB string
  QStringList shapePars = parameters.split(";");
  foreach( QString par, shapePars) {
  QStringList vars = par.split("=");
  QString varName = vars[0];
  QString varValue = vars[1];

  if (varName == "XHalfLength") XHalfLength = varValue.toDouble() * SYSTEM_OF_UNITS::mm;
  if (varName == "YHalfLength") YHalfLength = varValue.toDouble() * SYSTEM_OF_UNITS::mm;
  if (varName == "ZHalfLength") ZHalfLength = varValue.toDouble() * SYSTEM_OF_UNITS::mm;
  }

  // return new GeoBox(5.0*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm);
  return new GeoBox(XHalfLength, YHalfLength, ZHalfLength);
  }
  else if (type == "Tube") {

  // needed parameters
  double RMin;
  double RMax;
  double ZHalfLength;

  // get parameters from DB string
  QStringList shapePars = parameters.split(";");
  foreach( QString par, shapePars) {
  QStringList vars = par.split("=");
  QString varName = vars[0];
  QString varValue = vars[1];

  if (varName == "RMin") RMin = varValue.toDouble() * SYSTEM_OF_UNITS::mm;
  if (varName == "RMax") RMax = varValue.toDouble() * SYSTEM_OF_UNITS::mm;
  if (varName == "ZHalfLength") ZHalfLength = varValue.toDouble() * SYSTEM_OF_UNITS::mm;
  }

  // return new GeoBox(5.0*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm);
  return new GeoTube(RMin, RMax, ZHalfLength);

  }
  else if (type == "Shift") {

  // needed parameters
  unsigned int shapeId;
  unsigned int transfId;

  // get parameters from DB string
  QStringList shapePars = parameters.split(";");
  foreach( QString par, shapePars) {
  QStringList vars = par.split("=");
  QString varName = vars[0];
  QString varValue = vars[1];

  if (varName == "A") shapeId = varValue.toUInt();
  if (varName == "X") transfId = varValue.toUInt();
  }

  // get the referenced shape
  const GeoShape* shapeA = getShape( QString::number(shapeId) );

  // get the referenced HepGeom::Transform3D
  QStringList transPars = m_dbManager->getItemFromTableName("Transforms", transfId);
  qDebug() << "child:" << transPars;
  GeoTransform* transf = parseTransform(transPars);
  const HepGeom::Transform3D transfX = transf->getTransform();

  // build and return the GeoShapeShift instance
  return new GeoShapeShift(shapeA, transfX);
  }
  else if (type == "Pcon") {

  // needed parameters
  unsigned int SPhi;
  unsigned int DPhi;

  // get parameters from DB string
  QStringList shapePars = parameters.split(";");
  foreach( QString par, shapePars) {
  QStringList vars = par.split("=");
  QString varName = vars[0];
  QString varValue = vars[1];

  if (varName == "SPhi") SPhi = varValue.toUInt();
  if (varName == "DPhi") DPhi = varValue.toUInt();
  }

  // return new shape
  return new GeoPcon(SPhi, DPhi);
  }
  else {
  QString msg = "WARNING!! - Shape '" + type + "' not implemented yet!!! Returning a dummy cube.";
  qWarning() << msg;
  return new GeoBox(30.0*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm); // FIXME: bogus shape. Use actual shape!
  }

  }
*/

GeoShape* GReadIn::buildShape(QString shapeId)
{
  if (m_deepDebug) qDebug() << "GReadIn::buildShape()";
  QStringList paramsShape = m_shapes[ shapeId.toUInt() ];

  QString id = paramsShape[0];
  QString type = paramsShape[1];
  QString parameters = paramsShape[2];

  if (m_deepDebug) qDebug() << "\tShape-ID:" << id << ", Shape-type:" << type;

  if (type == "Box") {
    // shape parameters
    double XHalfLength = 0.;
    double YHalfLength = 0.;
    double ZHalfLength = 0.;
    // get parameters from DB string
    QStringList shapePars = parameters.split(";");
    foreach( QString par, shapePars) {
      QStringList vars = par.split("=");
      QString varName = vars[0];
      QString varValue = vars[1];
      if (varName == "XHalfLength") XHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "YHalfLength") YHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "ZHalfLength") ZHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
    }
    return new GeoBox(XHalfLength, YHalfLength, ZHalfLength);
  }
  else if (type == "Cons") {
    // shape parameters
    double RMin1 = 0.;
    double RMin2 = 0.;
    double RMax1 = 0.;
    double RMax2 = 0.;
    double DZ = 0.;
    double SPhi = 0.;
    double DPhi = 0.;
    // get parameters from DB string
    QStringList shapePars = parameters.split(";");
    foreach( QString par, shapePars) {
      QStringList vars = par.split("=");
      QString varName = vars[0];
      QString varValue = vars[1];
      if (varName == "RMin1") RMin1 = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "RMin2") RMin2 = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "RMax1") RMax1 = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "RMax2") RMax2 = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "DZ") DZ = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "SPhi") SPhi = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "SPDPhihi") DPhi = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
    }
    return new GeoCons (RMin1, RMin2, RMax1, RMax2, DZ, SPhi, DPhi);
  }
  else if (type == "Para") {
    // shape parameters
    double XHalfLength = 0.;
    double YHalfLength = 0.;
    double ZHalfLength = 0.;
    double Alpha = 0.;
    double Theta = 0.;
    double Phi = 0.;
    // get parameters from DB string
    QStringList shapePars = parameters.split(";");
    foreach( QString par, shapePars) {
      QStringList vars = par.split("=");
      QString varName = vars[0];
      QString varValue = vars[1];
      if (varName == "XHalfLength") XHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "YHalfLength") YHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "ZHalfLength") ZHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "Alpha") Alpha = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "Theta") Theta = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
      if (varName == "Phi") Phi = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
    }
    return new GeoPara (XHalfLength, YHalfLength, ZHalfLength, Alpha, Theta, Phi);
  }
  else if (type == "Pcon") {
    // shape parameters
    double SPhi = 0.;
    double DPhi = 0.;
    unsigned int NZPlanes = 0;

    // get parameters from DB string
    QStringList shapePars = parameters.split(";");

    bool error = 0;
    QString par;
    QStringList vars;
    QString varName;
    QString varValue;

    GeoPcon* pcon = nullptr;

    int sizePars = shapePars.size();
    // check if we have more than 3 parameters
    if (sizePars > 3) {

      // get the three first GeoPcon parameters: the SPhi and DPhi angles, plus the number of Z planes
      for( int it=0; it < 3; it++) {
        par = shapePars[it];
        vars = par.split("=");
        varName = vars[0];
        varValue = vars[1];
        if (varName == "SPhi") SPhi = varValue.toDouble();
        if (varName == "DPhi") DPhi = varValue.toDouble();
        if (varName == "NZPlanes") NZPlanes = varValue.toDouble();
      }
      // build the basic GeoPcon shape
      pcon = new GeoPcon(SPhi, DPhi);

      // and now loop over the rest of the list, to get the parameters of all Z planes
      for (int it=3; it < sizePars; it++)
      {
        par = shapePars[it];
        vars = par.split("=");
        varName = vars[0];
        varValue = vars[1];
        // qInfo() << "it:" << it << "par:" << par << "varName:" << varName << "varValue:" << varValue;

        if (varName == "ZPos") {

          double zpos = varValue.toDouble();
          double rmin=0., rmax=0.;

          it++; // go to next variable

          par = shapePars[it];
          vars = par.split("=");
          varName = vars[0];
          varValue = vars[1];
          if (varName == "ZRmin") rmin = varValue.toDouble();
          else error = 1;
          it++; // go to next variable

          par = shapePars[it];
          vars = par.split("=");
          varName = vars[0];
          varValue = vars[1];
          if (varName == "ZRmax") rmax = varValue.toDouble();
          else error = 1;

          if(error) qWarning() << "ERROR! GeoPcon 'ZRmin' and 'ZRmax' values are not at the right place! --> " << shapePars;

          // add a Z plane to the GeoPcon
          pcon->addPlane(zpos, rmin, rmax);
        } else {
          error = 1;
          qWarning() << "ERROR! GeoPcon 'ZPos' value is not at the right place! --> " << shapePars;
        }
      }

      // sanity check on the resulting Pcon shape
      if( pcon->getNPlanes() != NZPlanes) {
        error = 1;
        qWarning() << "ERROR! GeoPcon number of planes: " << QString::number(pcon->getNPlanes()) << " is not equal to the original size! --> " << shapePars;
      }
      if(!pcon->isValid()) {
        error = 1;
        qWarning() << "ERROR! GeoPcon shape is not valid!! -- input: " << shapePars;
      }
    } // end if (size>3)
    else {
      qWarning() << "ERROR!! GeoPcon has no Z planes!! --> shape input parameters: " << shapePars;
      error = 1;
    }

    if(error) qFatal("GeoPcon shape error!!! Aborting...");

    return pcon;
  }
  else if (type == "Pgon") {
    // shape parameters
    double SPhi = 0.;
    double DPhi = 0.;
    unsigned int NSides = 0;
    unsigned int NZPlanes = 0;

    bool error = false;
    GeoPgon* pgon = nullptr;
    QString par;
    QStringList vars;
    QString varName;
    QString varValue;

    // get parameters from DB string
    QStringList shapePars = parameters.split(";");
    // qInfo() << "shapePars: " << shapePars; // debug

    int sizePars = shapePars.size();
    // check if we have more than 3 parameters
    if (sizePars > 3) {

      // get the first four GeoPgon parameters: the SPhi and DPhi angles, plus the number of Z planes
      for( int it=0; it < 4; it++) {
        par = shapePars[it];
        vars = par.split("=");
        varName = vars[0];
        varValue = vars[1];
        // qInfo() << "vars: " << vars; // for debug only
        if (varName == "SPhi") SPhi = varValue.toDouble();
        if (varName == "DPhi") DPhi = varValue.toDouble();
        if (varName == "NSides") NSides = varValue.toUInt();// * SYSTEM_OF_UNITS::mm;
        if (varName == "NZPlanes") NZPlanes = varValue.toDouble();

      }
      // build the basic GeoPgon shape
      pgon = new GeoPgon(SPhi, DPhi, NSides);

      // and now loop over the rest of the list, to get the parameters of all Z planes
      for (int it=4; it < sizePars; it++)
      {
        par = shapePars[it];
        vars = par.split("=");
        varName = vars[0];
        varValue = vars[1];
        // qInfo() << "it:" << it << "par:" << par << "varName:" << varName << "varValue:" << varValue;

        if (varName == "ZPos") {

          double zpos = varValue.toDouble();
          double rmin=0., rmax=0.;

          it++; // go to next variable

          par = shapePars[it];
          vars = par.split("=");
          varName = vars[0];
          varValue = vars[1];
          if (varName == "ZRmin") rmin = varValue.toDouble();
          else error = 1;
          it++; // go to next variable

          par = shapePars[it];
          vars = par.split("=");
          varName = vars[0];
          varValue = vars[1];
          if (varName == "ZRmax") rmax = varValue.toDouble();
          else error = 1;

          if(error) qWarning() << "ERROR! GeoPgon 'ZRmin' and 'ZRmax' values are not at the right place! --> " << shapePars;

          // add a Z plane to the GeoPgon
          pgon->addPlane(zpos, rmin, rmax);
        } else {
          error = 1;
          qWarning() << "ERROR! GeoPgon 'ZPos' value is not at the right place! --> " << shapePars;
        }
      }

      // sanity check on the resulting Pgon shape
      if( pgon->getNPlanes() != NZPlanes) {
        error = 1;
        qWarning() << "ERROR! GeoPgon number of planes: " << QString::number(pgon->getNPlanes()) << " is not equal to the original size! --> " << shapePars;
      }
      if(!pgon->isValid()) {
        error = 1;
        qWarning() << "ERROR! GeoPgon shape is not valid!! -- input: " << shapePars;
      }
    } // end if (size>3)
    else {
      qWarning() << "ERROR!! GeoPgon has no Z planes!! --> shape input parameters: " << shapePars;
      error = 1;
    }
    if(error) qFatal("GeoPgon shape error!!! Aborting...");
    return pgon;
  }
  else if (type == "SimplePolygonBrep")
  {
    //qInfo() << "Reading-in: SimplePolygonBrep: "; // debug
    // shape parameters
    double DZ = 0.;
    unsigned int NVertices = 0;
    double xV = 0.;
    double yV = 0.;

    bool error = 0;
    GeoSimplePolygonBrep* sh = nullptr;
    QString par;
    QStringList vars;
    QString varName;
    QString varValue;

    // get parameters from DB string
    QStringList shapePars = parameters.split(";");
    //qInfo() << "shapePars: " << shapePars; // debug

    int sizePars = shapePars.size();
    // check if we have more than 2 parameters
    if (sizePars > 2) {

      // get the first two GeoSimplePolygonBrep parameters: DZ and the number of vertices.
      for( int it=0; it < 2; it++) {
        par = shapePars[it];
        vars = par.split("=");
        varName = vars[0];
        varValue = vars[1];
        // qInfo() << "vars: " << vars; // for debug only
        if (varName == "DZ") DZ = varValue.toDouble();
        if (varName == "NVertices") NVertices = varValue.toDouble();
        //else if (varName == "NVertices") NVertices = varValue.toDouble();
        //else error = 1;
        //if(error) qWarning() << "ERROR! GeoSimplePolygonBrep parameters are not correctly stored! -->" << vars;

      }
      // build the basic GeoSimplePolygonBrep shape
      sh = new GeoSimplePolygonBrep(DZ);

      // and now loop over the rest of the list, to get the parameters of all vertices
      for (int it=2; it < sizePars; it++)
      {
        par = shapePars[it];
        vars = par.split("=");
        varName = vars[0];
        varValue = vars[1];
        if (varName == "xV") xV = varValue.toDouble();
        else error = 1;

        it++; // go to next variable (they come in pairs)

        par = shapePars[it];
        vars = par.split("=");
        varName = vars[0];
        varValue = vars[1];
        if (varName == "yV") yV = varValue.toDouble();
        else error = 1;

        if(error) qWarning() << "ERROR! GeoSimplePolygonBrep 'xVertex' and 'yVertex' values are not at the right place! --> " << shapePars;

        // add a Z plane to the GeoSimplePolygonBrep
        sh->addVertex(xV, yV);
      }
      // sanity check on the resulting shape
      if( sh->getNVertices() != NVertices) {
        error = 1;
        qWarning() << "ERROR! GeoSimplePolygonBrep number of planes: " << QString::number(sh->getNVertices()) << " is not equal to the original size! --> " << shapePars;
      }
      if(!sh->isValid()) {
        error = 1;
qWarning() << "ERROR! GeoSimplePolygonBrep shape is not valid!! -- input: " << shapePars;
			}
		} // end if (size>3)
		else {
			qWarning() << "ERROR!! GeoSimplePolygonBrep has no vertices!! --> shape input parameters: " << shapePars;
			error = 1;
		}
		if(error) qFatal("GeoSimplePolygonBrep shape error!!! Aborting...");
		return sh;

	}
	else if (type == "Trap") {
			// shape constructor parameters
			double ZHalfLength = 0.;
			double Theta = 0.;
			double Phi = 0.;
			double Dydzn = 0.;
			double Dxdyndzn = 0.;
			double Dxdypdzn = 0.;
			double Angleydzn = 0.;
			double Dydzp = 0.;
			double Dxdyndzp = 0.;
			double Dxdypdzp = 0.;
			double Angleydzp = 0.;
			// get parameters from DB string
			QStringList shapePars = parameters.split(";");
			foreach( QString par, shapePars) {
					QStringList vars = par.split("=");
					QString varName = vars[0];
					QString varValue = vars[1];
					if (varName == "ZHalfLength") ZHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Theta") Theta = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Phi") Phi = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Dydzn") Dydzn = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Dxdyndzn") Dxdyndzn = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Dxdypdzn") Dxdypdzn = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Angleydzn") Angleydzn = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Dydzp") Dydzp = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Dxdyndzp") Dxdyndzp = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Dxdypdzp") Dxdypdzp = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "Angleydzp") Angleydzp = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
			}
		return new GeoTrap (ZHalfLength, Theta, Phi, Dydzn, Dxdyndzn, Dxdypdzn, Angleydzn, Dydzp, Dxdyndzp, Dxdypdzp, Angleydzp);
	}
	else if (type == "Trd") {
			// shape constructor parameters
			double XHalfLength1 = 0.;
			double XHalfLength2 = 0.;
			double YHalfLength1 = 0.;
			double YHalfLength2 = 0.;
			double ZHalfLength = 0.;
			// get parameters from DB string
			QStringList shapePars = parameters.split(";");
			foreach( QString par, shapePars) {
					QStringList vars = par.split("=");
					QString varName = vars[0];
					QString varValue = vars[1];
					if (varName == "XHalfLength1") XHalfLength1 = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "XHalfLength2") XHalfLength2 = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "YHalfLength1") YHalfLength1 = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "YHalfLength2") YHalfLength2 = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
					if (varName == "ZHalfLength") ZHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
			}
		return new GeoTrd (XHalfLength1, XHalfLength2, YHalfLength1, YHalfLength2, ZHalfLength);
	}
	else if (type == "Tube") {
		// shape parameters
		double RMin = 0.;
		double RMax = 0.;
		double ZHalfLength = 0.;
		// get parameters from DB string
		QStringList shapePars = parameters.split(";");
		foreach( QString par, shapePars) {
			QStringList vars = par.split("=");
			QString varName = vars[0];
			QString varValue = vars[1];
			if (varName == "RMin") RMin = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
			if (varName == "RMax") RMax = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
			if (varName == "ZHalfLength") ZHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
		}
		return new GeoTube(RMin, RMax, ZHalfLength);
	}
	else if (type == "Tubs") {
		// shape parameters
		double RMin = 0.;
		double RMax = 0.;
		double ZHalfLength = 0.;
		double SPhi = 0.;
		double DPhi = 0.;
		// get parameters from DB string
		QStringList shapePars = parameters.split(";");
		foreach( QString par, shapePars) {
			QStringList vars = par.split("=");
			QString varName = vars[0];
			QString varValue = vars[1];
			if (varName == "RMin") RMin = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
			if (varName == "RMax") RMax = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
			if (varName == "ZHalfLength") ZHalfLength = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
			if (varName == "SPhi") SPhi = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
			if (varName == "DPhi") DPhi = varValue.toDouble();// * SYSTEM_OF_UNITS::mm;
		}
		return new GeoTubs (RMin, RMax, ZHalfLength, SPhi, DPhi);
	}
	else if (type == "Intersection") {
		// shape parameters
		unsigned int opA = 0;
		unsigned int opB = 0;
		// get parameters from DB string
		QStringList shapePars = parameters.split(";");
		foreach( QString par, shapePars) {
			QStringList vars = par.split("=");
			QString varName = vars[0];
			QString varValue = vars[1];
			if (varName == "opA") opA = varValue.toUInt();
			if (varName == "opB") opB = varValue.toUInt();
		}
		// get the referenced shape
		const GeoShape* shapeA = buildShape( QString::number(opA) );
		const GeoShape* shapeB = buildShape( QString::number(opB) );
		// build and return the GeoShapeShift instance
		return new GeoShapeIntersection(shapeA, shapeB);
	}
	else if (type == "Shift") {
		// shape parameters
		unsigned int shapeId = 0;
		unsigned int transfId = 0;
		// get parameters from DB string
		QStringList shapePars = parameters.split(";");
		foreach( QString par, shapePars) {
			QStringList vars = par.split("=");
			QString varName = vars[0];
			QString varValue = vars[1];
			if (varName == "A") shapeId = varValue.toUInt();
			if (varName == "X") transfId = varValue.toUInt();
		}
		// get the referenced shape
		// const GeoShape* shapeA = getShape( QString::number(shapeId) );
		const GeoShape* shapeA = buildShape( QString::number(shapeId) );
		// get the referenced HepGeom::Transform3D
		QStringList transPars = m_dbManager->getItemFromTableName("Transforms", transfId);
		if (m_deepDebug) qDebug() << "child:" << transPars;
		GeoTransform* transf = parseTransform(transPars);
		const GeoTrf::Transform3D transfX = transf->getTransform();
		// build and return the GeoShapeShift instance
		return new GeoShapeShift(shapeA, transfX);
	}
	else if (type == "Subtraction") {
		// shape parameters
		unsigned int opA = 0;
		unsigned int opB = 0;
		// get parameters from DB string
		QStringList shapePars = parameters.split(";");
		foreach( QString par, shapePars) {
			QStringList vars = par.split("=");
			QString varName = vars[0];
			QString varValue = vars[1];
			if (varName == "opA") opA = varValue.toUInt();
			if (varName == "opB") opB = varValue.toUInt();
		}
		// get the referenced shape

		//if (VP1Msg::debug())
    		//VP1Msg::messageDebug("GeoSubstraction - building shape A...\n");

		const GeoShape* shapeA = buildShape( QString::number(opA) );

		/*
		if (VP1Msg::debug()) {
			QString msg = QString::fromStdString(shapeA->type());
    			VP1Msg::messageDebug("GeoSubstraction - built shape A: "+msg);
    			VP1Msg::messageDebug("GeoSubstraction - building shape B...\n");
    		}
		*/

		const GeoShape* shapeB = buildShape( QString::number(opB) );

		/*
		if (VP1Msg::debug()) {
			QString msg = QString::fromStdString(shapeB->type());
    			VP1Msg::messageDebug("GeoSubstraction - built shape B: "+msg+"\n");
    		}
		*/

		// build and return the GeoShapeShift instance
		return new GeoShapeSubtraction(shapeA, shapeB);
	}
	else if (type == "Union") {
		// shape parameters
		unsigned int opA = 0;
		unsigned int opB = 0;
		// get parameters from DB string
		QStringList shapePars = parameters.split(";");
		foreach( QString par, shapePars) {
			QStringList vars = par.split("=");
			QString varName = vars[0];
			QString varValue = vars[1];
			if (varName == "opA") opA = varValue.toUInt();
			if (varName == "opB") opB = varValue.toUInt();
		}
		if (opA == 0 || opB == 0) std::cout << "ERROR! 'GeoUnion' shape: opA or opB have not been properly intialized!" << std::endl;
		// get the referenced shape
		const GeoShape* shapeA = buildShape( QString::number(opA) );
		const GeoShape* shapeB = buildShape( QString::number(opB) );
		// build and return the GeoShapeShift instance
		return new GeoShapeUnion(shapeA, shapeB);
	}
	else {
		// QString msg = "WARNING!! - Shape '" + type + "' not implemented yet!!! Returning a dummy cube.";
		// qWarning(msg.toStdString().c_str());
		m_unknown_shapes.insert(type.toStdString()); // save unknwon shapes for later warning message
		return new GeoBox(30.0*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm); // FIXME: bogus shape. Use actual shape!
	}

}



/*
GeoLogVol* GReadIn::parseLogVol(QStringList values)
{
	qDebug() << "GReadIn::parseLogVol()";

	QString logVolName = values[1];

	// GET LOGVOL SHAPE
	QString shapeId = values[2];

	// qDebug() << "shape Id:" << shapeId;
	// QStringList paramsShape = m_dbManager->getItem("GeoShape", shapeId.toUInt());
  // qDebug() << "shape parameters from DB: " << paramsShape;
	// GeoShape* shape = parseShape(paramsShape);

	GeoShape* shape = getShape(shapeId);

	// GET LOGVOL MATERIAL
	QString matId = values[3];
	QStringList paramsMat = m_materials[ matId.toUInt() ];
	GeoMaterial* mat = parseMaterial(paramsMat);

	// instantiate teh GeoLogVol
	return new GeoLogVol(logVolName.toStdString(), shape, mat);

}
*/


GeoLogVol* GReadIn::buildLogVol(QString logVolId)
{
	if (m_deepDebug) qDebug() << "GReadIn::buildLogVol()";

	// get logVol properties from the DB
	QStringList values = m_logVols[logVolId.toUInt()];
	if (m_deepDebug) qDebug() << "params:" << values;

	// build the LogVol
	QString logVolName = values[1];

	// GET LOGVOL SHAPE
	QString shapeId = values[2];
	GeoShape* shape = buildShape(shapeId);

	// GET LOGVOL MATERIAL
	QString matId = values[3];
	if (m_deepDebug) qDebug() << "material Id:" << matId;
	GeoMaterial* mat = buildMaterial(matId);

	return new GeoLogVol(logVolName.toStdString(), shape, mat);

}

/*
GeoShape* GReadIn::getShape(QString shapeId){
	qDebug() << "shape Id:" << shapeId;
	QStringList paramsShape = m_dbManager->getItem("GeoShape", shapeId.toUInt());
	qDebug() << "shape parameters from DB: " << paramsShape;
	GeoShape* shape = parseShape(paramsShape);
	return shape;
}
*/


GeoSerialDenominator* GReadIn::buildSerialDenominator(QString id)
{
	if (m_deepDebug) qDebug() << "GReadIn::buildSerialDenominator()";
	return parseSerialDenominator( m_serialDenominators[id.toUInt()] );
}

GeoSerialDenominator* GReadIn::parseSerialDenominator(QStringList values)
{
	if (m_deepDebug) qDebug() << "GReadIn::parseSerialDenominator()";
	QString id = values[0];
	QString baseName = values[1];
	if (m_deepDebug) qDebug() << "\tID:" << id << ", base-name:" << baseName;
	return new GeoSerialDenominator(baseName.toStdString());
}

GeoAlignableTransform* GReadIn::buildAlignableTransform(QString id)
{
	if (m_deepDebug) qDebug() << "GReadIn::buildAlignableTransform()";
	return parseAlignableTransform( m_alignableTransforms[id.toUInt()] );
}

GeoAlignableTransform* GReadIn::parseAlignableTransform(QStringList values)
{
	if (m_deepDebug) qDebug() << "GReadIn::parseAlignableTransform()";

	QString id = values.takeFirst(); // it pops out the first element, leaving the other items in the list

	// get the 12 matrix elements
	double xx = values[0].toDouble();
	double xy = values[1].toDouble();
	double xz = values[2].toDouble();

	double yx = values[3].toDouble();
	double yy = values[4].toDouble();
	double yz = values[5].toDouble();

	double zx = values[6].toDouble();
	double zy = values[7].toDouble();
	double zz = values[8].toDouble();

	double dx = values[9].toDouble();
	double dy = values[10].toDouble();
	double dz = values[11].toDouble();

	// TODO: move to GeoModelKernel::GeoTrf (Eigen)
	// build a rotation matrix with the first 9 elements
	CLHEP::HepRotation R;
	R.set(CLHEP::Hep3Vector(xx,yx,zx),
			CLHEP::Hep3Vector(xy,yy,zy),
			CLHEP::Hep3Vector(xz,yz,zz));
	// build a translation vector with the last 3 elements
	CLHEP::Hep3Vector D(dx,dy,dz);
	// build the Transformation
	const HepGeom::Transform3D xf(R, D);

	return new GeoAlignableTransform( Amg::CLHEPTransformToEigen(xf) );
}

GeoTransform* GReadIn::buildTransform(QString id)
{
	if (m_deepDebug) qDebug() << "GReadIn::buildTransform()";
	return parseTransform( m_transforms[id.toUInt()] );
}

GeoTransform* GReadIn::parseTransform(QStringList values)
{
	if (m_deepDebug) qDebug() << "GReadIn::parseTransform()";
	if (m_deepDebug) qDebug() << "values:" << values;

	QString id = values.takeFirst(); // it pops out the first element, leaving the other items in the list

	// get the 12 matrix elements
	double xx = values[0].toDouble();
	double xy = values[1].toDouble();
	double xz = values[2].toDouble();

	double yx = values[3].toDouble();
	double yy = values[4].toDouble();
	double yz = values[5].toDouble();

	double zx = values[6].toDouble();
	double zy = values[7].toDouble();
	double zz = values[8].toDouble();

	double dx = values[9].toDouble();
	double dy = values[10].toDouble();
	double dz = values[11].toDouble();

	// build a rotation matrix with the first 9 elements
	// TODO: move to GeoModelKernel GeoTrf (Eigen)
	CLHEP::HepRotation R;
	R.set(CLHEP::Hep3Vector(xx,yx,zx),
			CLHEP::Hep3Vector(xy,yy,zy),
			CLHEP::Hep3Vector(xz,yz,zz));
	// build a translation vector with the last 3 elements
	CLHEP::Hep3Vector D(dx,dy,dz);
	// build the Transformation
	const HepGeom::Transform3D xf(R, D);

	return new GeoTransform( Amg::CLHEPTransformToEigen(xf) );
}

/*
GeoSerialTransformer* GReadIn::parseSerialTransformer(QStringList values)
{
	qDebug() << "GReadIn::parseSerialTransformer()";
	qDebug() << "values:" << values;

	QString id = values[0];
	QString functionId = values[1];
	QString physVolId = values[2];
	QString physVolTableIdStr = values[3];
	QString copies = values[4];

	unsigned int physVolTableId = physVolTableIdStr.toUInt();

	// QString physVolType = m_dbManager->getNodeTypeFromTableId(physVolTableId);
	QString physVolType = m_tableid_tableName[physVolTableId];

	qDebug() << "\tID:" << id << ", functionId:" << functionId << ", physVolId:" << physVolId << ", physVolTableId:" << physVolTableId << ", copies:" << copies;

	// GET FUNCTION
	qDebug() << "function Id:" << functionId;
	QStringList paramsFunc = m_dbManager->getItem("Function", functionId.toUInt());
	// TRANSFUNCTION func = parseFunction(paramsFunc);
	TRANSFUNCTION func = parseFunction(paramsFunc[1].toStdString());

	// GET PHYSVOL
	qDebug() << "referenced physVol - Id:" << physVolId << ", type:" << physVolType;
	QStringList paramsPhysVol = m_dbManager->getItem(physVolTableId, physVolId.toUInt());
	const GeoVPhysVol* physVol = parseVPhysVol(paramsPhysVol, physVolType);
	//qDebug() << "physVol:" << physVol << ", function:" << &func;

	// get PhysVol or FullPhysVol pointer and return the SerialTransformer
	if (dynamic_cast<const GeoFullPhysVol*>(physVol)) {
		const GeoFullPhysVol* vol = dynamic_cast<const GeoFullPhysVol*>(physVol);
		return new GeoSerialTransformer(vol, &func, copies.toUInt() );
	}
	const GeoPhysVol* vol = dynamic_cast<const GeoPhysVol*>(physVol);
	return new GeoSerialTransformer(vol, &func, copies.toUInt() );

}
*/

GeoSerialTransformer* GReadIn::buildSerialTransformer(QString nodeId)
{
	if (m_deepDebug) qDebug() << "GReadIn::buildSerialTransformer()";

	QStringList values = m_serialTransformers[nodeId.toUInt()];
	if (m_deepDebug) qDebug() << "values:" << values;

	// std::cout <<"ST * " << values[0].toStdString() << " " << values[1].toStdString() << " " << values[2].toStdString() << std::endl;
	if (m_deepDebug) qDebug() << "ST * " << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << ", " << values[4];

	QString id = values[0];
	QString functionId = values[1];
	QString physVolId = values[2];
	QString physVolTableIdStr = values[3];
	QString copies = values[4];

	unsigned int physVolTableId = physVolTableIdStr.toUInt();

	QString physVolType = m_tableid_tableName[physVolTableId];

	if (m_deepDebug) qDebug() << "\tID:" << id << ", functionId:" << functionId << ", physVolId:" << physVolId << ", physVolTableId:" << physVolTableId << ", copies:" << copies;

	// GET FUNCTION
	if (m_deepDebug) qDebug() << "function Id:" << functionId;
	TRANSFUNCTION func = buildFunction(functionId);

	// GET PHYSVOL
	if (m_deepDebug) qDebug() << "referenced physVol - Id:" << physVolId << ", type:" << physVolType << "tableId:" << physVolTableIdStr;
	const GeoVPhysVol* physVol = buildVPhysVol(physVolId, physVolTableIdStr, "1"); // we use "1" as default copyNumber: taking the first copy of the VPhysVol as the referenced volume
	//qDebug() << "physVol:" << physVol << ", function:" << &func;

	// get PhysVol or FullPhysVol pointer and return the SerialTransformer
	if (dynamic_cast<const GeoFullPhysVol*>(physVol)) {
		const GeoFullPhysVol* vol = dynamic_cast<const GeoFullPhysVol*>(physVol);
		return new GeoSerialTransformer(vol, &func, copies.toUInt() );
	}
	const GeoPhysVol* vol = dynamic_cast<const GeoPhysVol*>(physVol);
	return new GeoSerialTransformer(vol, &func, copies.toUInt() );

}

TRANSFUNCTION GReadIn::buildFunction(QString id)
{
	if (m_deepDebug) qDebug() << "GReadIn::buildFunction()";

	// return parseFunction( m_functions[id.toUInt()] );

	// return parseFunction( m_functions[id.toUInt()] );

	QStringList values = m_functions[id.toUInt()];
	// return parseFunction( values[0].toUInt(), values[1].toStdString() );
	return parseFunction( values[1].toStdString() );


}

// TRANSFUNCTION GReadIn::parseFunction(QStringList values)
// {
// 	qDebug() << "GReadIn::parseFunction()";
// 	qDebug() << "values:" << values;
// 	QString id = values[0];
// 	QString expression = values[1];
// 	if (expression.size() == 0) {
// 			qFatal("FATAL ERROR!! Function expression is empty!! Aborting...");
// 			abort();
// 	}
// 	TransFunctionInterpreter interpreter;
// 	qDebug() << "expression:" << expression;
// 	TFPTR func=interpreter.interpret( expression.toStdString() );
// 	qDebug() << "expression interpreted";
// 	return *(func.release()); // make func returns a pointer to the managed object and releases the ownership, then get the object dereferencing the pointer
// }


// TRANSFUNCTION GReadIn::parseFunction(const QStringList& values)
// {
// 	qDebug() << "GReadIn::parseFunction(const QStringList& values)";
// 	qDebug() << "values:" << values;
// 	int id = values[0].toUInt();
// 	std::string expression = values[1].toStdString();
// 	if (expression.size() == 0) {
// 			qFatal("FATAL ERROR!! Function expression is empty!! Aborting...");
// 			abort();
// 	}
// 	TransFunctionInterpreter interpreter;
// 	qDebug() << "expression:" << expression;
// 	TFPTR func=interpreter.interpret( expression );
// 	qDebug() << "expression interpreted";
// 	return *(func.release()); // make func returns a pointer to the managed object and releases the ownership, then get the object dereferencing the pointer
// }


// TRANSFUNCTION GReadIn::parseFunction(const int& id, const std::string& expr)
TRANSFUNCTION GReadIn::parseFunction(const std::string& expr)
{
	if (m_deepDebug) qDebug() << "GReadIn::parseFunction(const std::string& expr)";
	// qDebug() << "id:" << Qstring::number(id) << " - expression: " << QString::fromStdString(expr);
	if (m_deepDebug) qDebug() << "expression: " << QString::fromStdString(expr);

	if (expr.empty()) {
			qFatal("FATAL ERROR!! Function expression is empty!! Aborting...");
			abort();
	}

	TransFunctionInterpreter interpreter;
	if (m_deepDebug) qDebug() << "expression:" << QString::fromStdString(expr);
	TFPTR func=interpreter.interpret( expr );
	if (m_deepDebug) qDebug() << "expression interpreted";
	return *(func.release()); // make func returns a pointer to the managed object and releases the ownership, then get the object dereferencing the pointer
}

GeoNameTag* GReadIn::buildNameTag(QString id)
{
	if (m_deepDebug) qDebug() << "GReadIn::buildNameTag()";
	return parseNameTag( m_nameTags[id.toUInt()] );
}

GeoNameTag* GReadIn::parseNameTag(QStringList values)
{
	if (m_deepDebug) qDebug() << "GReadIn::parseNameTag()";
	QString id = values[0];
	QString name = values[1];
	if (m_deepDebug) qDebug() << "nameTag:" << name;
	return new GeoNameTag(name.toStdString());
}


bool GReadIn::isNodeBuilt(const QString id, const QString tableId, const QString copyNumber)
{
	// qDebug() << "GReadIn::isNodeBuilt(): " << id << tableId << copyNumber;
	QString key = id + ":" + tableId + ":" + copyNumber;
	return m_memMap.contains(key);
}


GeoGraphNode* GReadIn::getNode(const QString id, const QString tableId, const QString copyN)
{
	if (m_deepDebug) qDebug() << "GReadIn::getNode(): " << id << tableId << copyN;
	QString key = id + ":" + tableId + ":" + copyN;
	return m_memMap[key];
}

void GReadIn::storeNode(const QString id, const QString tableId, const QString copyN, GeoGraphNode* node)
{
	if (m_deepDebug) qDebug() << "GReadIn::storeNode(): " << id << tableId << copyN << node;
	QString key = id + ":" + tableId + ":" + copyN;
	m_memMap[key] = node;
}


} /* namespace GeoModelPers */

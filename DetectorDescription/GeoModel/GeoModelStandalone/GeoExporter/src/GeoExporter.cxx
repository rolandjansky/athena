/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////////
//                                                                   //
//  Implementation of class GeoExporter                              //
//                                                                   //
//  Author: Riccardo Maria BIANCHI <riccardo.maria.bianchi@cern.ch>  //
//                                                                   //
//  Initial version: Sep 2017                                        //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include "GeoExporter/GeoExporter.h"
#include "VP1Base/VP1Msg.h"

#include "VP1Utils/VP1JobConfigInfo.h"
#include "VP1Utils/VP1SGAccessHelper.h"

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "GeoModelKernel/GeoBox.h"

#include "GeoModelUtilities/GeoModelExperiment.h"

#include "GeoModelDBManager/GMDBManager.h"

#include "GeoModelWrite/WriteGeoModel.h" //DumpGeoModelActionLocal.h"

#include <QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtNetwork/QSslSocket>
#include <QProcessEnvironment>

#include <cassert>
#include <iostream>

#include <boost/range/irange.hpp>



//____________________________________________________________________
class GeoExporter::Imp {
public:
  Imp() {};

  //We hold the arguments here until init is called:
  StoreGateSvc* sg;
  StoreGateSvc* detstore;
  ISvcLocator* svclocator;
  IToolSvc*toolSvc;
};


//____________________________________________________________________
GeoExporter::GeoExporter(StoreGateSvc* sg,StoreGateSvc* detstore,
	       ISvcLocator* svclocator,IToolSvc*toolSvc)
 : m_d(new Imp)
{
  m_d->sg = sg;
  m_d->detstore = detstore;
  m_d->svclocator = svclocator;
  m_d->toolSvc = toolSvc;
}

//____________________________________________________________________
GeoExporter::~GeoExporter()
{
  delete m_d; m_d=0;
}

//____________________________________________________________________
bool GeoExporter::argumentsAreValid() const
{
  //Athena pointers:

  if (!m_d->sg) {
    VP1Msg::message("ERROR: Null pointer to event store.");
    return false;
  }
  if (!m_d->detstore) {
    VP1Msg::message("ERROR: Null pointer to detector store.");
    return false;
  }

  return true;
}


//____________________________________________________________________
void GeoExporter::init()
{
  VP1Msg::messageDebug("Start of GeoExporter::init()...");

  VP1Msg::message("");
  VP1Msg::message("===================================================");
  VP1Msg::message("               Launching the GeoExporter");
  VP1Msg::message("===================================================");
  VP1Msg::message("");

  static int argc=1;
  static char *argv[2];

  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("ATLAS");
  QCoreApplication::setApplicationName("GeoExporter");

  VP1Msg::message("Accessing the ATLAS geometry...");
  StoreGateSvc* detstore = m_d->detstore;
 //Get the world volume:
  const GeoModelExperiment * theExpt = nullptr;
  if (!VP1SGAccessHelper(detstore).retrieve(theExpt,"ATLAS")) {
    std::cout << "Error: Could not retrieve the ATLAS GeoModelExperiment from detector store" << std::endl; // TODO: move to ATH_MSG_
    //ATH_MSG_FATAL ("Error: Could not retrieve the ATLAS GeoModelExperiment from detector store");
    //return StatusCode::FAILURE;
    return; // TODO: move to Return statuscode
  }
  // GET ATLAS GEOMETRY
  PVConstLink world(theExpt->getPhysVol());

  // ### Get user's settings ###
  QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
  // -- get Detector Description tag
  QString default_detdescrtag = environment.value("DUMPGEODETDESCRTAGDEFAULT");
  QString user_detdescrtag = environment.value("DUMPGEODETDESCRTAG");
  if ("1"==default_detdescrtag) {
    VP1Msg::message("The user did not specify a DetDescrTag - Using the default one: " + user_detdescrtag);
    }
  else {
    VP1Msg::message("User's settings - DetDescrTag: " + user_detdescrtag);
    }
  // -- get sub-systems settings
  bool user_noid = environment.value("DUMPGEO_NOID").toInt();
  bool user_nocalo = environment.value("DUMPGEO_NOCALO").toInt();
  bool user_nomuon = environment.value("DUMPGEO_NOMUON").toInt();
  QString user_subsystems_filters = "" + QString(((user_noid) ? "-noID" : "")) + QString(((user_nocalo) ? "-noCalo" : "")) + QString(((user_nomuon) ? "-noMuon" : ""));

  // -- get GeoModel Treetop filter // FIXME: check and update this!!
  QString user_treetopfilter = environment.value("DUMPGEOTREETOPFILTER");
  VP1Msg::message("User's settings - GeoModel Treetop filter: " + user_treetopfilter);
  // -- get Overwrite flag
  auto getOWFlag = [&environment]() -> bool { bool okConv; int flag = environment.value("DUMPGEOOVERWRITE").toInt(&okConv, 10); return (okConv && 1==flag) ? true : false; }; // convert the "0"/"1" env var string to bool with a lambda
  bool user_overwrite = getOWFlag();
  VP1Msg::message("User's settings - Overwrite filter: " + QString::number(user_overwrite) );

  // Get list of Treetop names from the TREETOPFILTER
  QStringList user_treetopslist;
  if ( ! user_treetopfilter.isEmpty() )
    user_treetopslist = user_treetopfilter.split(',');

  /*
  // Debug: get list of managers
  std::cout << "List of GeoModel managers: " << std::endl;
  std::vector<std::string> managersList = theExpt->getListOfManagers();
  for (auto const& mm : managersList)
    {
        const GeoVDetectorManager* manager = theExpt->getManager(mm);
        unsigned int nTreetops = manager->getNumTreeTops();
        std::cout << mm << " - n.Treetops: " << nTreetops << std::endl;

        if ( nTreetops > 0 && user_treetopslist.contains(QString::fromStdString(mm)) ) {

            for(unsigned int i=0; i < nTreetops; ++i) {

                PVConstLink treetop(manager->getTreeTop(i));

                // DEBUG: dive into the Treetop
                if (mm=="BeamPipe") {
                GeoVolumeCursor av(treetop);
                while (!av.atEnd()) {
                    std::cout << i << " - name: "  << av.getName() << "\n";
                    av.next(); // increment volume cursor.
                } // end while
                }
            } // end for
        } // end if
    } // end for
  */

  std::cout << "Creating the SQLite DB file..." << std::endl;
  QString path = "";
  if (user_detdescrtag.isEmpty())
    path = "geometry.db";
  else {
    //qDebug() << "user_treetopslist:" << user_treetopslist;
    if ( user_treetopslist.isEmpty() )
        path = "geometry-" + user_detdescrtag + user_subsystems_filters + ".db";
    else
        path = "geometry-" + user_detdescrtag + "-" + user_treetopslist.join("-") + ".db";  // FIXME: check and update!
  }

  // check if path exists and if yes: Is it a file and no directory?
   bool fileExists = QFileInfo::exists(path) && QFileInfo(path).isFile();
  if (fileExists) {
    if (user_overwrite) {
        VP1Msg::message("Removing the existing dump file ("+path+")...");
        QFile file (path);
        file.remove();
    } else if ( !user_overwrite ) {
        VP1Msg::messageWarningAllRed("The output file ("+path+") is already present in the current folder, but you don't use the '-f' flag to overwrite it. The program will be stopped. Please remove or move the existing file to another folder, or use the '-f' flag to replace it.");
        throw "existing output file";
    }
  }
  // open the DB connection
  GMDBManager db(path);

  // check the DB connection
  if (db.isOpen())
      qDebug() << "OK! Database is open!";
  else {
      qDebug() << "Database ERROR!! Exiting...";
      return;
  }

   std::cout << "Dumping the GeoModel geometry to the DB file..." << std::endl;
  // Dump the tree volumes into a DB
  GeoModelIO::WriteGeoModel dumpGeoModelGraph(db); // init the GeoModel node action
  world->exec(&dumpGeoModelGraph); // visit all GeoModel nodes
  std::cout << "Saving the GeoModel tree to the DB." << std::endl;
  dumpGeoModelGraph.saveToDB(); // save to the SQlite DB file
  std::cout << "DONE. Geometry saved." <<std::endl;

  std::cout << "\nTest - list of all the GeoMaterial nodes in the persistified geometry:" << std::endl;
  db.printAllMaterials();
  std::cout << "\nTest - list of all the GeoElement nodes in the persistified geometry:" << std::endl;
  db.printAllElements();

  VP1Msg::messageDebug("end of GeoExporter::init().");
}

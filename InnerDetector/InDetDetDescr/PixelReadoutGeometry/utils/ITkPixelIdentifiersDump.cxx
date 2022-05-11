/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Tadej Novak
 * @brief Dump ITk Pixel identifiers
 */

// Framework includes
#include <AthenaBaseComps/AthService.h>
#include <AthenaBaseComps/AthAlgTool.h>
#include <AsgMessaging/MessageCheck.h>
#include <GaudiKernel/IAppMgrUI.h>

// Pixel includes
#include <InDetIdentifier/PixelID.h>
#include <InDetReadoutGeometry/SiDetectorElement.h>
#include <PixelReadoutGeometry/PixelModuleDesign.h>
#include <PixelReadoutGeometry/PixelDetectorManager.h>

// std
#include <fstream>

// messaging
ANA_MSG_HEADER(ITkPixel)
ANA_MSG_SOURCE(ITkPixel, "Identifiers")
using namespace ITkPixel;


int main(int argc, char *argv[])
{
  ANA_CHECK_SET_TYPE (int); // makes ANA_CHECK return ints if exiting function

  std::string outFileName{"ITkPixelIdentifiers.dat"};
  if (argc > 1) {
    outFileName = argv[1];
  }

  IAppMgrUI *appMgr = Gaudi::createApplicationMgr();
  SmartIF<IProperty> propMgr = appMgr;
  ANA_CHECK( propMgr->setProperty( "EvtSel",         "NONE" ) );
  ANA_CHECK( propMgr->setProperty( "JobOptionsType", "FILE" ) );
  ANA_CHECK( propMgr->setProperty( "JobOptionsPath", "PixelReadoutGeometry/ITkPixelIdentifiersDumpConfig.txt" ) );

  ANA_CHECK( appMgr->configure() );
  ANA_CHECK( appMgr->initialize() );

  ServiceHandle<StoreGateSvc> detectorStore("DetectorStore", "ITkPixelIdentifiers");

  const InDetDD::PixelDetectorManager *detManager{};
  ANA_CHECK( detectorStore->retrieve(detManager, "ITkPixel") );
  const InDetDD::SiDetectorElementCollection* elements{detManager->getDetectorElementCollection()};

  const PixelID *pixelID{};
  ANA_CHECK( detectorStore->retrieve(pixelID, "PixelID") );

  std::ofstream fout{outFileName};
  fout << "# "
       << "Hash" << "\t"
       << "ID" << "\t"
       << "B/EC" << "\t"
       << "Layer" << "\t" 
       << "phi" << "\t"
       << "eta" << "\t"
       << "FEs" << std::endl;

  for (unsigned int moduleHash{}; moduleHash < pixelID->wafer_hash_max(); moduleHash++) {
    IdentifierHash wafer_hash = IdentifierHash(moduleHash);
    Identifier wafer_id = pixelID->wafer_id(wafer_hash);
    const InDetDD::SiDetectorElement *element = elements->getDetectorElement(wafer_hash);
    const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());

    fout << moduleHash << "\t"
         << wafer_id.get_compact() << "\t"
         << pixelID->barrel_ec(wafer_id) << "\t"
         << pixelID->layer_disk(wafer_id) << "\t"
         << pixelID->phi_module(wafer_id) << "\t"
         << pixelID->eta_module(wafer_id) << "\t"
         << p_design->numberOfCircuits() << std::endl;
  }

  fout.close();

  ANA_CHECK( appMgr->finalize() );
  ANA_CHECK( appMgr->terminate() );

  return 0;
}

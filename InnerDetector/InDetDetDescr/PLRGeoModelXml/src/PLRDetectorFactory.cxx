/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PLRDetectorFactory.h"
#include "PLRGmxInterface.h"
#include "PLROptions.h"

#include <AthenaPoolUtilities/CondAttrListCollection.h>
#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelKernel/GeoPhysVol.h>
#include <GeoModelUtilities/DecodeVersionKey.h>
#include <GeoModelXml/Gmx2Geo.h>
#include <InDetReadoutGeometry/Version.h>
#include <PathResolver/PathResolver.h>
#include <PixelReadoutGeometry/PixelModuleDesign.h>
#include <RDBAccessSvc/IRDBAccessSvc.h>
#include <RDBAccessSvc/IRDBRecord.h>
#include <RDBAccessSvc/IRDBRecordset.h>
#include <ReadoutGeometryBase/SiCommonItems.h>
#include <StoreGate/StoreGateSvc.h>


namespace InDetDD
{

PLRDetectorFactory::PLRDetectorFactory(AthenaComps *athenaComps,
                                       SiCommonItems *commonItems,
                                       const PLROptions &options)
 : DetectorFactoryBase(athenaComps),
   m_commonItems(commonItems),
   m_options(options)
{
  // Create the detector manager... should allow the name to be set
  m_detectorManager = new PixelDetectorManager(detStore(), m_options.detectorName());
}


void PLRDetectorFactory::create(GeoPhysVol *world)
{
  ATH_MSG_INFO("C R E A T E   W O R L D");

  PLRGmxInterface gmxInterface(m_detectorManager, m_commonItems, &m_moduleTree);
  // To set up solid geometry only, without having to worry about sensitive detectors etc., and get loads of debug output,
  // comment out above line and uncomment the following line; also, switch header files above.
  // GmxInterface gmxInterface;

  int flags{};
  std::string gmxInput;

  if (m_options.gmxFilename().empty()) {
    ATH_MSG_INFO("gmxFilename not set; getting .gmx from Geometry database Blob not supported");
  } else {
    flags = 0;
    gmxInput = PathResolver::find_file(m_options.gmxFilename(), "DATAPATH");
    if (gmxInput.empty()) { // File not found
      std::string errMessage("PLRDetectorFactory::create: Unable to find file " + m_options.gmxFilename() +
                             " with PathResolver; check filename and DATAPATH environment variable");
      throw std::runtime_error(errMessage);
    }
  }

  Gmx2Geo gmx2Geo(gmxInput, world, gmxInterface, flags);

  // Add the tree-top to the detector manager.
  unsigned int nChildren = world->getNChildVols();
  bool foundVolume = false;

  for (int iChild = nChildren - 1; iChild>=0; --iChild) {
    if( world->getNameOfChildVol(iChild) == "PLR") {
      // The * converts from a ConstPVLink to a reference to a GeoVPhysVol;
      // the & takes its address.
      foundVolume = true;
      m_detectorManager->addTreeTop(&*world->getChildVol(iChild));
      break;
    }
  }

  if (!foundVolume) ATH_MSG_ERROR("Could not find a logicalVolume named \"PLR\"- this is required to provide the Envelope!");

  doNumerology();

  m_detectorManager->initNeighbours();
}

PixelDetectorManager * PLRDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}

void PLRDetectorFactory::doNumerology()
{
  InDetDD::SiNumerology n;

  ATH_MSG_INFO("\n\nPLR Numerology:\n===============\n\nNumber of parts is " << m_moduleTree.nParts());
}

} // namespace InDetDD

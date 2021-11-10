/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMPrimeOptions.h"
#include "BCMPrimeGmxInterface.h"
#include "BCMPrimeDetectorFactory.h"

#include <BCMPrimeReadoutGeometry/BCMPrimeDetectorManager.h>
#include <GeoModelKernel/GeoPhysVol.h>
#include <GeoModelXml/Gmx2Geo.h>
#include <PathResolver/PathResolver.h>


namespace InDetDD
{

BCMPrimeDetectorFactory::BCMPrimeDetectorFactory(AthenaComps *athenaComps,
                                                 const BCMPrimeOptions &options)
 : DetectorFactoryBase(athenaComps),
   m_options(options)
{
  // Create the detector manager
  m_detectorManager = new InDetDD::BCMPrimeDetectorManager(detStore(), m_options.detectorName());
}

void BCMPrimeDetectorFactory::create(GeoPhysVol *world)
{
  ATH_MSG_INFO( "C R E A T E   W O R L D" );

  BCMPrimeGmxInterface gmxInterface(m_detectorManager);
  // To set up solid geometry only, without having to worry about sensitive detectors etc., and get loads of debug output,
  // comment out above line and uncomment the following line; also, switch header files above.
  // GmxInterface gmxInterface;

  int flags{};
  std::string gmxInput;

  if (m_options.gmxFilename().empty()) {
    ATH_MSG_ERROR( "gmxFilename not set; getting .gmx from Geometry database Blob not supported");
    // gmxInput = getBlob();
  } else {
    flags = 0;
    gmxInput = PathResolver::find_file(m_options.gmxFilename(), "DATAPATH");
    if (gmxInput.empty()) { // File not found
      std::string errMessage("BCMPrimeDetectorFactory::create: Unable to find file " + m_options.gmxFilename() +
                             " with PathResolver; check filename and DATAPATH environment variable");
      throw std::runtime_error(errMessage);
    }
  }

  Gmx2Geo gmx2Geo(gmxInput, world, gmxInterface, flags);

  // Add the tree-top to the detector manager.
  unsigned int nChildren = world->getNChildVols();
  bool foundVolume = false;

  for (int iChild = nChildren - 1; iChild>=0; --iChild) {
    if (world->getNameOfChildVol(iChild) == "BCMPrime") {
      // The * converts from a ConstPVLink to a reference to a GeoVPhysVol;
      // the & takes its address.
      foundVolume = true;
      m_detectorManager->addTreeTop(&*world->getChildVol(iChild));
      break;
    }
  }

  if (!foundVolume) ATH_MSG_ERROR("Could not find a logicalVolume named \"BCMPrime\" - this is required to provide the Envelope!");
}

BCMPrimeDetectorManager * BCMPrimeDetectorFactory::getDetectorManager() const
{
  return m_detectorManager;
}

} // namespace InDetDD

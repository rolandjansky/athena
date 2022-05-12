/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMPrimeDetectorTool.h"
#include "BCMPrimeGmxInterface.h"

#include <BCMPrimeReadoutGeometry/BCMPrimeDetectorManager.h>

#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelKernel/GeoPhysVol.h>
#include <GeoModelUtilities/GeoModelExperiment.h>


BCMPrimeDetectorTool::BCMPrimeDetectorTool(const std::string &type,
                                           const std::string &name,
                                           const IInterface *parent)
  : GeoModelXmlTool(type, name, parent)
{
}


StatusCode BCMPrimeDetectorTool::create()
{
  // retrieve the common stuff
  ATH_CHECK(createBaseTool());

  GeoModelExperiment *theExpt = nullptr;
  ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));

  //
  // Check the availability
  //
  std::string node{"InnerDetector"};
  std::string table{"BCMPrimeXDD"};
  if (!isAvailable(node, table)) {
    ATH_MSG_ERROR("No BCMPrime geometry found. BCMPrime can not be built.");
    return StatusCode::FAILURE;
  }
  //
  // Create the detector manager
  //
  // The * converts a ConstPVLink to a ref to a GeoVPhysVol
  // The & takes the address of the GeoVPhysVol
  GeoPhysVol *world = &*theExpt->getPhysVol();
  auto *manager = new InDetDD::BCMPrimeDetectorManager(&*detStore(), m_detectorName);

  InDetDD::BCMPrimeGmxInterface gmxInterface(manager);

  // Load the geometry, create the volume,
  // and then find the volume index within the world to allow it to be added
  // last two arguments are the location in the DB to look for the clob
  // (may want to make those configurables)
  int childIndex = createTopVolume(world, gmxInterface, node, table);
  if (childIndex != -1) { //-1 represents an error state from the above method
    manager->addTreeTop(&*world->getChildVol(childIndex));
  } else {
    ATH_MSG_FATAL("Could not find the Top Volume!!!");
    return StatusCode::FAILURE;
  }

  // set the manager
  m_detManager = manager;

  ATH_CHECK(detStore()->record(m_detManager, m_detManager->getName()));
  theExpt->addManager(m_detManager);

  return StatusCode::SUCCESS;
}


StatusCode BCMPrimeDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::BCMPrimeDetectorManager>::ID(), m_detManager->getName());
  if (proxy) {
    proxy->reset();
    m_detManager = nullptr;
  }
  return StatusCode::SUCCESS;
}


StatusCode BCMPrimeDetectorTool::registerCallback()
{
  //
  //  Register call-back for software alignment
  //
  if (m_alignable) {
    std::string folderName = "/ITk/Align";
    if (detStore()->contains<AlignableTransformContainer>(folderName)) {
      ATH_MSG_DEBUG( "Registering callback on AlignableTransformContainer with folder " << folderName );
      const DataHandle<AlignableTransformContainer> atc;
      StatusCode sc = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
      if (sc.isFailure()) {
        ATH_MSG_ERROR( "Could not register callback on AlignableTransformContainer with folder " <<
                  folderName );
      }
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_WARNING( "Unable to register callback on AlignableTransformContainer with folder " <<
                 folderName << ", Alignment disabled (only if no Run2 scheme is loaded)!" );
    }
  } else {
    ATH_MSG_INFO( "Alignment disabled. No callback registered" );
    // We return failure otherwise it will try and register a GeoModelSvc callback associated with this callback.
  }
  return StatusCode::SUCCESS;
}


StatusCode BCMPrimeDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys))
{
  //
  //  The call-back routine, which just calls the real call-back routine from the manager.
  //
  if (!m_detManager) {
    ATH_MSG_WARNING( "Manager does not exist" );
    return StatusCode::FAILURE;
  }
  if (m_alignable) {
    return m_detManager->align(I, keys);
  } else {
    ATH_MSG_DEBUG( "Alignment disabled. No alignments applied" );
    return StatusCode::SUCCESS;
  }
}

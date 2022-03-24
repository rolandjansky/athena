/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PLRDetectorTool.h"
#include "PLRGmxInterface.h"

#include <PixelReadoutGeometry/PixelDetectorManager.h>

#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelKernel/GeoPhysVol.h>
#include <GeoModelUtilities/GeoModelExperiment.h>
#include <SGTools/DataProxy.h>


PLRDetectorTool::PLRDetectorTool(const std::string &type,
                                 const std::string &name,
                                 const IInterface *parent)
 : GeoModelXmlTool(type, name, parent)
{
}


StatusCode PLRDetectorTool::create()
{
  // retrieve the common stuff
  ATH_CHECK(createBaseTool());

  GeoModelExperiment *theExpt = nullptr;
  ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));
  const PixelID *idHelper = nullptr;
  ATH_CHECK(detStore()->retrieve(idHelper, "PixelID"));

  m_commonItems = std::make_unique<InDetDD::SiCommonItems>(idHelper);

  //
  // Check the availability
  //
  std::string node{"InnerDetector"};
  std::string table{"PLRXDD"};
  if (!isAvailable(node, table)) {
    ATH_MSG_ERROR("No PLR geometry found. PLR can not be built.");
    return StatusCode::FAILURE;
  }
  //
  // Create the detector manager
  //
  // The * converts a ConstPVLink to a ref to a GeoVPhysVol
  // The & takes the address of the GeoVPhysVol
  GeoPhysVol *world = &*theExpt->getPhysVol();
  auto *manager = new InDetDD::PixelDetectorManager(&*detStore(), m_detectorName);
  manager->addFolder(m_alignmentFolderName);

  InDetDD::PLRGmxInterface gmxInterface(manager, m_commonItems.get(), &m_moduleTree);

  // Load the geometry, create the volume,
  // and then find the volume index within the world to allow it to be added
  // last two arguments are the location in the DB to look for the clob
  // (may want to make those configurables)
  int childIndex = createTopVolume(world, gmxInterface, node, table);
  if (childIndex != -1) { //-1 represents an error state from the above method
    manager->addTreeTop(&*world->getChildVol(childIndex));
    // if it were implemented, we would do Numerology here
    // doNumerology(manager);
    manager->initNeighbours();
  } else {
    ATH_MSG_FATAL("Could not find the Top Volume!!!");
    return StatusCode::FAILURE;
  }

  // set the manager
  m_detManager = manager;

  ATH_CHECK(detStore()->record(m_detManager, m_detManager->getName()));
  theExpt->addManager(m_detManager);

  // Create a symLink to the SiDetectorManager base class so it can be accessed as either SiDetectorManager or
  // PixelDetectorManager
  const InDetDD::SiDetectorManager *siDetManager = m_detManager;
  ATH_CHECK(detStore()->symLink(m_detManager, siDetManager));

  return StatusCode::SUCCESS;
}


StatusCode PLRDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::PixelDetectorManager>::ID(),m_detManager->getName());
  if (proxy) {
    proxy->reset();
    m_detManager = nullptr;
  }
  return StatusCode::SUCCESS;

}


StatusCode PLRDetectorTool::registerCallback ATLAS_NOT_THREAD_SAFE ()
{
  //
  // Register call-back for software alignment
  //
  if (m_alignable) {
    if (detStore()->contains<AlignableTransformContainer>(m_alignmentFolderName)) {
      ATH_MSG_DEBUG("Registering callback on AlignableTransformContainer with folder " << m_alignmentFolderName);
      const DataHandle<AlignableTransformContainer> atc;
      StatusCode sc = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, m_alignmentFolderName);
      if (sc.isFailure()) {
        ATH_MSG_ERROR("Could not register callback on AlignableTransformContainer with folder "
                      << m_alignmentFolderName);
        return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_WARNING("Unable to register callback on AlignableTransformContainer with folder "
                      << m_alignmentFolderName << ", Alignment disabled (only if no Run2 scheme is loaded)!");
    }
  } else {
    ATH_MSG_INFO("Alignment disabled. No callback registered");
    // We return failure otherwise it will try and register a GeoModelSvc callback associated with this callback.
  }
  return StatusCode::SUCCESS;
}


StatusCode PLRDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys))
{
  //
  // The call-back routine, which just calls the real call-back routine from the manager.
  //
  if (!m_detManager) {
    ATH_MSG_WARNING("Manager does not exist");
    return StatusCode::FAILURE;
  }
  if (m_alignable) {
    return m_detManager->align(I, keys);
  } else {
    ATH_MSG_DEBUG("Alignment disabled. No alignments applied");
    return StatusCode::SUCCESS;
  }
}


void PLRDetectorTool::doNumerology()
{
  InDetDD::SiNumerology n;

  ATH_MSG_INFO("\n\nPLR Numerology:\n===============\n\nNumber of parts is " << m_moduleTree.nParts());
}

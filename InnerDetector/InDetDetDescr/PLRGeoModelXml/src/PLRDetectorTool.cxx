/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PLRDetectorTool.h"
#include "PixelGeoModelXml/PixelGmxInterface.h"
#include <PixelReadoutGeometry/PixelDetectorManager.h>

#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelUtilities/GeoModelExperiment.h>
#include <GeoModelUtilities/DecodeVersionKey.h>
#include <GeoModelKernel/GeoPhysVol.h>
#include <SGTools/DataProxy.h>
#include <StoreGate/StoreGateSvc.h>


PLRDetectorTool::PLRDetectorTool(const std::string &type,
                                 const std::string &name,
                                 const IInterface *parent)
 : GeoModelXmlTool(type, name, parent)
{
}


StatusCode PLRDetectorTool::create()
{
 //retrieve the common stuff
  ATH_CHECK(createBaseTool());
  // Get the detector configuration.
  ATH_CHECK(m_geoModelSvc.retrieve());

  GeoModelExperiment *theExpt = nullptr;
  ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));
  const PixelID *idHelper = nullptr;
  ATH_CHECK(detStore()->retrieve(idHelper, "PixelID"));

  m_commonItems = std::make_unique<InDetDD::SiCommonItems>(idHelper);

  //
  // Get the version
  //
  DecodeVersionKey versionKey(&*m_geoModelSvc, "InnerDetector");
  if (versionKey.tag() == "AUTO"){
    ATH_MSG_ERROR("Atlas version tag is AUTO. You must set a version-tag like ATLAS_P2_ITK-00-00-00.");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO((versionKey.custom() ? "Building custom " : "Building ")
               << "PLR with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node() );
  //
  // Get the Database Access Service and from there the pixel version tag
  //
  std::string plrVersionTag = m_rdbAccessSvc->getChildTag("Pixel", versionKey.tag(), versionKey.node());
  ATH_MSG_INFO("PLR Version: " << plrVersionTag);
  //
  // Check if pixel version tag is empty. If so, then the pixel cannot be built.
  // This may or may not be intentional. We just issue an INFO message.
  //
  if (plrVersionTag.empty()) {
    ATH_MSG_INFO("No PLR Version. PLR will not be built.");
    return StatusCode::SUCCESS;
  }
  //
  // Create the PixelDetectorFactory
  //
  // The * converts a ConstPVLink to a ref to a GeoVPhysVol
  // The & takes the address of the GeoVPhysVol
  GeoPhysVol *world = &*theExpt->getPhysVol();
  m_detManager  = createManager(world);

  if (!m_detManager) {
    ATH_MSG_ERROR("PLRDetectorManager not found");
    return StatusCode::FAILURE;
  }

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

InDetDD::PixelDetectorManager * PLRDetectorTool::createManager(GeoPhysVol * theWorld){

  InDetDD::PixelDetectorManager * theManager = new InDetDD::PixelDetectorManager(&*detStore(), m_detectorName);
  //in the factory we would set the version here and write it to the mgr... is it really necessary???
  //Leave it out for now until we find we need it...

  theManager->addFolder(m_alignmentFolderName);

  InDetDD::ITk::PixelGmxInterface gmxInterface(theManager, m_commonItems.get(), &m_moduleTree);

  //Load the geometry, create the volume, 
  //and then find the volume index within the world to allow it to be added
  //last two arguments are the location in the DB to look for the clob
  //(may want to make those configurables)
  int childIndex = createTopVolume(theWorld,gmxInterface, "PLR", "PLRXDD");
  if(childIndex != -1){ //-1 represents an error state from the above method
   theManager->addTreeTop(&*theWorld->getChildVol(childIndex));
   //if it were implemented, we would do Numerology here
   //doNumerology(theManager);
   theManager->initNeighbours();
  }
  else ATH_MSG_FATAL("Could not find the Top Volume!!!");
 
  return theManager;
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


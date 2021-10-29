/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "PLRDetectorFactory.h"
#include "PLRDetectorTool.h"
#include "PLROptions.h"

#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelUtilities/GeoModelExperiment.h>
#include <SGTools/DataProxy.h>
#include <StoreGate/StoreGateSvc.h>


PLRDetectorTool::PLRDetectorTool(const std::string &type,
                                 const std::string &name,
                                 const IInterface *parent)
 : GeoModelTool(type, name, parent)
{
}


StatusCode PLRDetectorTool::create()
{
  //
  // Retrieve all services except LorentzAngleSvc, which has to be done later
  //

  // Get the detector configuration.
  ATH_CHECK(m_geoDbTagSvc.retrieve());
  ATH_CHECK(m_rdbAccessSvc.retrieve());

  GeoModelExperiment *theExpt;
  ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));
  const PixelID *idHelper;
  ATH_CHECK(detStore()->retrieve(idHelper, "PixelID"));

  //
  // Get their interfaces to pass to the DetectorFactory
  //
  auto athenaComps = std::make_unique<InDetDD::AthenaComps>("PLRGeoModelXml");
  athenaComps->setDetStore(&*(detStore()));
  athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
  athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);

  m_commonItems = std::make_unique<InDetDD::SiCommonItems>(idHelper);
  //
  // Get options from python
  //
  InDetDD::PLROptions options;
  options.setAlignable(m_alignable);
  options.setGmxFilename(m_gmxFilename);
  options.setDetectorName(m_detectorName);

  //
  // Create the PLRDetectorFactory
  //
  // The * converts a ConstPVLink to a ref to a GeoVPhysVol
  // The & takes the address of the GeoVPhysVol
  GeoPhysVol *world = &*theExpt->getPhysVol();
  InDetDD::PLRDetectorFactory thePLR(athenaComps.get(), m_commonItems.get(), options);
  thePLR.create(world);
  //
  // Get the manager from the factory and store it in the detector store.
  //
  m_manager = thePLR.getDetectorManager();

  if (!m_manager) {
    ATH_MSG_ERROR("PixelDetectorManager not found; not created in PLRDetectorFactory? (PLR uses PixelDetectorManager)");
    return(StatusCode::FAILURE);
  }

  ATH_CHECK(detStore()->record(m_manager, m_manager->getName()));
  theExpt->addManager(m_manager);

  // Create a symLink to the SiDetectorManager base class so it can be accessed as either SiDetectorManager or
  // PixelDetectorManager
  const InDetDD::SiDetectorManager *siDetManager = m_manager;
  ATH_CHECK(detStore()->symLink(m_manager, siDetManager));

  return StatusCode::SUCCESS;
}


StatusCode PLRDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::PixelDetectorManager>::ID(),m_manager->getName());
  if (proxy) {
    proxy->reset();
    m_manager = nullptr;
  }
  return StatusCode::SUCCESS;
}


StatusCode PLRDetectorTool::registerCallback()
{
  //
  // Register call-back for software alignment
  //
  if (m_alignable) {
    std::string folderName = "/Indet/Align"; //DEF? BCM has: folderName = "/ITk/Align"
    if (detStore()->contains<AlignableTransformContainer>(folderName)) {
      ATH_MSG_DEBUG("Registering callback on AlignableTransformContainer with folder " << folderName);
      const DataHandle<AlignableTransformContainer> atc;
      StatusCode sc = detStore()->regFcn(&IGeoModelTool::align, dynamic_cast<IGeoModelTool *>(this), atc, folderName);
      if (sc.isFailure()) {
        ATH_MSG_ERROR("Could not register callback on AlignableTransformContainer with folder "
                      << folderName);
        return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_WARNING("Unable to register callback on AlignableTransformContainer with folder "
                      << folderName << ", Alignment disabled (only if no Run2 scheme is loaded)!");
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
  if (!m_manager) {
    ATH_MSG_WARNING("Manager does not exist");
    return StatusCode::FAILURE;
  }
  if (m_alignable) {
    return m_manager->align(I, keys);
  } else {
    ATH_MSG_DEBUG("Alignment disabled. No alignments applied");
    return StatusCode::SUCCESS;
  }
}

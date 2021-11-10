/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "StripDetectorFactory.h"
#include "StripDetectorTool.h"
#include "StripOptions.h"

#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelUtilities/DecodeVersionKey.h>
#include <GeoModelUtilities/GeoModelExperiment.h>
#include <SGTools/DataProxy.h>
#include <StoreGate/StoreGateSvc.h>


namespace ITk
{

StripDetectorTool::StripDetectorTool(const std::string &type,
                                     const std::string &name,
                                     const IInterface *parent)
 : GeoModelTool(type, name, parent)
{
}


StatusCode StripDetectorTool::create()
{
  //
  // Retrieve all services except LorentzAngleSvc, which has to be done later
  //

  // Get the detector configuration.
  ATH_CHECK(m_geoModelSvc.retrieve());
  ATH_CHECK(m_geoDbTagSvc.retrieve());
  ATH_CHECK(m_rdbAccessSvc.retrieve());

  GeoModelExperiment *theExpt;
  ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));
  const SCT_ID *idHelper;
  ATH_CHECK(detStore()->retrieve(idHelper, "SCT_ID"));

  //
  // Get their interfaces to pass to the DetectorFactory
  //
  auto athenaComps = std::make_unique<InDetDD::AthenaComps>("StripGeoModelXml");
  athenaComps->setDetStore(&*(detStore()));
  athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
  athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);

  m_commonItems = std::make_unique<InDetDD::SiCommonItems>(idHelper);
  //
  // Get options from python
  //
  InDetDD::ITk::StripOptions options;
  options.setAlignable(m_alignable);
  options.setGmxFilename(m_gmxFilename);
  options.setDetectorName(m_detectorName);
  //
  // Get the version
  //
  DecodeVersionKey versionKey(&*m_geoModelSvc, "SCT");
  if (versionKey.tag() == "AUTO"){
    ATH_MSG_ERROR("Atlas version tag is AUTO. You must set a version-tag like ATLAS_P2_ITK-00-00-00.");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO((versionKey.custom() ? "Building custom " : "Building ")
               << "ITk Strip with Version Tag: "<< versionKey.tag() << " at Node: " << versionKey.node());
  //
  // Get the Database Access Service and from there the SCT version tag
  //
  std::string sctVersionTag = m_rdbAccessSvc->getChildTag("SCT", versionKey.tag(), versionKey.node());
  ATH_MSG_INFO("ITk Strip Version: " << sctVersionTag);
  //
  // Check if SCT version tag is empty. If so, then the SCT cannot be built.
  // This may or may not be intentional. We just issue an INFO message.
  //
  if (sctVersionTag.empty()) {
    ATH_MSG_INFO("No ITk Strip Version. ITk Strip will not be built.");
    return StatusCode::SUCCESS;
  }
  //
  // Create the SCT_DetectorFactory
  //
  // The * converts a ConstPVLink to a ref to a GeoVPhysVol
  // The & takes the address of the GeoVPhysVol
  GeoPhysVol *world = &*theExpt->getPhysVol();
  InDetDD::ITk::StripDetectorFactory theSCT(athenaComps.get(), m_commonItems.get(), options);
  theSCT.create(world);
  //
  // Get the manager from the factory and store it in the detector store.
  //
  m_manager = theSCT.getDetectorManager();

  if (!m_manager) {
    ATH_MSG_ERROR("SCT_DetectorManager not found; not created in SCT_DetectorFactory?");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(detStore()->record(m_manager, m_manager->getName()));
  theExpt->addManager(m_manager);

  // Create a symLink to the SiDetectorManager base class so it can be accessed as either SiDetectorManager or
  // SCT_DetectorManager
  const InDetDD::SiDetectorManager *siDetManager = m_manager;
  ATH_CHECK(detStore()->symLink(m_manager, siDetManager));

  return StatusCode::SUCCESS;
}


StatusCode StripDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::SCT_DetectorManager>::ID(),m_manager->getName());
  if (proxy) {
    proxy->reset();
    m_manager = nullptr;
  }
  return StatusCode::SUCCESS;
}


StatusCode StripDetectorTool::registerCallback()
{
  //
  //  Register call-back for software alignment
  //
  if (m_alignable) {
    std::string folderName = "/Indet/Align";
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


StatusCode StripDetectorTool::align(IOVSVC_CALLBACK_ARGS_P(I, keys)){
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

} // namespace ITk

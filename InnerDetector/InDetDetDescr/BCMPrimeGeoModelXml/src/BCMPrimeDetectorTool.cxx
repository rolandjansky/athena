/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "BCMPrimeDetectorFactory.h"
#include "BCMPrimeDetectorTool.h"
#include "BCMPrimeOptions.h"

#include <DetDescrConditions/AlignableTransformContainer.h>
#include <GeoModelUtilities/GeoModelExperiment.h>
#include <SGTools/DataProxy.h>
#include <StoreGate/StoreGateSvc.h>


BCMPrimeDetectorTool::BCMPrimeDetectorTool(const std::string &type,
                                           const std::string &name,
                                           const IInterface *parent)
  : GeoModelTool(type, name, parent)
{
}


StatusCode BCMPrimeDetectorTool::create()
{
  //
  //   Retrieve all services
  //
  // Get the detector configuration.
  ATH_CHECK(m_geoDbTagSvc.retrieve());
  ATH_CHECK(m_rdbAccessSvc.retrieve());
  GeoModelExperiment *theExpt = nullptr;
  ATH_CHECK(detStore()->retrieve(theExpt, "ATLAS"));

  //
  //  Get their interfaces to pass to the DetectorFactory
  //
  auto athenaComps = std::make_unique<InDetDD::AthenaComps>("BCMPrimeGeoModelXml");
  athenaComps->setDetStore(&*(detStore()));
  athenaComps->setRDBAccessSvc(&*m_rdbAccessSvc);
  athenaComps->setGeoDbTagSvc(&*m_geoDbTagSvc);

  //
  //  Get options from python
  //
  InDetDD::BCMPrimeOptions options;
  options.setAlignable(m_alignable);
  options.setGmxFilename(m_gmxFilename);
  options.setDetectorName(m_detectorName);

  //
  // Create the BCMPrimeDetectorFactory
  //
  // The * converts a ConstPVLink to a ref to a GeoVPhysVol
  // The & takes the address of the GeoVPhysVol
  GeoPhysVol *world = &*theExpt->getPhysVol();
  InDetDD::BCMPrimeDetectorFactory bcmPrime(athenaComps.get(), options);
  bcmPrime.create(world);

  //
  // Get the manager from the factory and store it in the detector store.
  //
  m_manager = bcmPrime.getDetectorManager();

  if (!m_manager) {
    ATH_MSG_ERROR( "PixelDetectorManager not found; not created in BCMPrimeDetectorFactory?" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK(detStore()->record(m_manager, m_manager->getName()));
  theExpt->addManager(m_manager);

  return StatusCode::SUCCESS;
}


StatusCode BCMPrimeDetectorTool::clear()
{
  SG::DataProxy* proxy = detStore()->proxy(ClassID_traits<InDetDD::BCMPrimeDetectorManager>::ID(),m_manager->getName());
  if (proxy) {
    proxy->reset();
    m_manager = nullptr;
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
  if (!m_manager) {
    ATH_MSG_WARNING( "Manager does not exist" );
    return StatusCode::FAILURE;
  }
  if (m_alignable) {
    return m_manager->align(I, keys);
  } else {
    ATH_MSG_DEBUG( "Alignment disabled. No alignments applied" );
    return StatusCode::SUCCESS;
  }
}

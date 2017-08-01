/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "Identifier/Identifier.h"
#include "InDetSimData/InDetSimData.h"

#include "InDetOverlay/InDetSDOOverlay.h"

InDetSDOOverlay::InDetSDOOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator)
{
  declareProperty("do_TRT", m_do_TRT=true);
  declareProperty("do_TRT_background", m_do_TRT_background=true);
  declareProperty("mainInputTRTKey", m_mainInputTRTKey);
  declareProperty("overlayInputTRTKey", m_overlayInputTRTKey);
  declareProperty("mainOutputTRTKey", m_outputTRTKey);

  declareProperty("do_SCT", m_do_SCT=true);
  declareProperty("do_SCT_background", m_do_SCT_background=true);
  declareProperty("mainInputSCTKey", m_mainInputSCTKey);
  declareProperty("overlayInputSCTKey", m_overlayInputSCTKey);
  declareProperty("mainOutputSCTKey", m_outputSCTKey);

  declareProperty("do_Pixel", m_do_Pixel=true);
  declareProperty("do_Pixel_background", m_do_Pixel_background=true);
  declareProperty("mainInputPixelKey", m_mainInputPixelKey);
  declareProperty("overlayInputPixelKey", m_overlayInputPixelKey);
  declareProperty("mainOutputPixelKey", m_outputPixelKey);
}

//================================================================
StatusCode InDetSDOOverlay::overlayInitialize()
{
  ATH_MSG_INFO("InDetSDOOverlay initialized");

  // Check and initialize keys
  ATH_CHECK( m_mainInputTRTKey.initialize() );
  ATH_CHECK( m_overlayInputTRTKey.initialize() );
  ATH_CHECK( m_outputTRTKey.initialize() );
  ATH_CHECK( m_mainInputSCTKey.initialize() );
  ATH_CHECK( m_overlayInputSCTKey.initialize() );
  ATH_CHECK( m_outputSCTKey.initialize() );
  ATH_CHECK( m_mainInputPixelKey.initialize() );
  ATH_CHECK( m_overlayInputPixelKey.initialize() );
  ATH_CHECK( m_outputPixelKey.initialize() );

  return StatusCode::SUCCESS;
}

//================================================================
StatusCode InDetSDOOverlay::overlayFinalize()
{
  ATH_MSG_INFO("InDetSDOOverlay finalized");
  return StatusCode::SUCCESS;
}

//================================================================
StatusCode InDetSDOOverlay::overlayExecute()
{
  ATH_MSG_DEBUG("InDetSDOOverlay::execute() begin");

  if (m_do_TRT) {
    ATH_MSG_VERBOSE("Retrieving data input TRT SDO collection");
    SG::ReadHandle<InDetSimDataCollection> dataContainer(m_mainInputTRTKey);
    if (m_do_TRT_background && !dataContainer.isValid()) {   
      ATH_MSG_WARNING("Could not get data TRT SDO collection \"" << m_mainInputTRTKey.key() << "\"");
    }

    ATH_MSG_VERBOSE("Retrieving MC input TRT SDO collection");
    SG::ReadHandle<InDetSimDataCollection> mcContainer(m_overlayInputTRTKey);
    if (!mcContainer.isValid()) {
      ATH_MSG_WARNING("Could not get MC TRT SDO collection \"" << m_overlayInputTRTKey.key() << "\"");
    }

    SG::WriteHandle<InDetSimDataCollection> outputContainer(m_outputTRTKey);
    outputContainer = CxxUtils::make_unique<InDetSimDataCollection>();

    if ((!m_do_TRT_background || dataContainer.isValid()) && mcContainer.isValid() && outputContainer.isValid()) {
      ATH_MSG_VERBOSE("Doing TRT SDO overlay");

      const InDetSimDataCollection &mcSDO = *mcContainer;
      InDetSimDataCollection &outputSDO = *outputContainer;

      // Copy over MC
      ATH_MSG_VERBOSE("Inserting MC SDO, size=" << mcSDO.size());
      outputSDO.insert(mcSDO.begin(), mcSDO.end());
  
      // Merge with background
      if (m_do_TRT_background) {
        const InDetSimDataCollection &dataSDO = *dataContainer;
        ATH_MSG_VERBOSE("Inserting data SDO, size=" << dataSDO.size());

        for (const auto &entry : dataSDO) {
          auto it = outputSDO.find(entry.first);
          if (it != outputSDO.end()) {
            // merge necessary
            InDetSimData outSimData = it->second;
            std::vector<InDetSimData::Deposit> depositsVector(outSimData.getdeposits());
            depositsVector.insert(depositsVector.end(), entry.second.getdeposits().begin(), entry.second.getdeposits().end());

            it->second = InDetSimData(depositsVector);
          } else {
            outputSDO.insert(entry);
          }
        }
      }

      ATH_MSG_VERBOSE("Output SDO size " << outputSDO.size());
    }
  }

  if (m_do_SCT) {
    ATH_MSG_VERBOSE("Retrieving MC input SCT SDO collection");
    std::auto_ptr<InDetSimDataCollection> ap(m_storeGateMC->retrievePrivateCopy<InDetSimDataCollection>(m_overlayInputSCTKey.key()));

    if (!m_storeGateOutput->record(ap, m_outputSCTKey.key()).isSuccess()) {
      ATH_MSG_WARNING("Problem recording SCT SDO collection");
    }
  }

  if (m_do_Pixel) {
    ATH_MSG_VERBOSE("Retrieving MC input Pixel SDO collection");
    std::auto_ptr<InDetSimDataCollection> ap(m_storeGateMC->retrievePrivateCopy<InDetSimDataCollection>(m_overlayInputPixelKey.key()));

    if (!m_storeGateOutput->record(ap, m_outputPixelKey.key()).isSuccess()) {
      ATH_MSG_WARNING("Problem recording Pixel SDO collection");
    }
  }

  ATH_MSG_DEBUG("InDetSDOOverlay::execute() end");
  return StatusCode::SUCCESS;
}

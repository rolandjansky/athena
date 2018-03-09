/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CxxUtils/make_unique.h"
#include "Identifier/Identifier.h"
#include "InDetSimData/InDetSimData.h"
#include "InDetSimData/InDetSimDataCollection.h"

#include "InDetOverlay/InDetSDOOverlay.h"

InDetSDOOverlay::InDetSDOOverlay(const std::string &name, ISvcLocator *pSvcLocator) :
  OverlayAlgBase(name, pSvcLocator)
{
  declareProperty("do_TRT", m_do_TRT=true);
  declareProperty("do_TRT_background", m_do_TRT_background=true);
  declareProperty("mainInputTRTKey", m_mainInputTRTKey="TRT_SDO_Map");
  declareProperty("overlayInputTRTKey", m_overlayInputTRTKey="TRT_SDO_Map");
  declareProperty("mainOutputTRTKey", m_outputTRTKey="TRT_SDO_Map");

  declareProperty("do_SCT", m_do_SCT=true);
  declareProperty("do_SCT_background", m_do_SCT_background=true);
  declareProperty("mainInputSCTKey", m_mainInputSCTKey="SCT_SDO_Map");
  declareProperty("overlayInputSCTKey", m_overlayInputSCTKey="SCT_SDO_Map");
  declareProperty("mainOutputSCTKey", m_outputSCTKey="SCT_SDO_Map");

  declareProperty("do_Pixel", m_do_Pixel=true);
  declareProperty("do_Pixel_background", m_do_Pixel_background=true);
  declareProperty("mainInputPixelKey", m_mainInputPixelKey="PixelSDO_Map");
  declareProperty("overlayInputPixelKey", m_overlayInputPixelKey="PixelSDO_Map");
  declareProperty("mainOutputPixelKey", m_outputPixelKey="PixelSDO_Map");
}

//================================================================
StatusCode InDetSDOOverlay::overlayInitialize()
{
  ATH_MSG_INFO("InDetSDOOverlay initialized");
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
    SG::ReadHandle<InDetSimDataCollection> dataContainer(m_mainInputTRTKey, m_storeGateData->name());
    if (m_do_TRT_background && !dataContainer.isValid()) {   
      ATH_MSG_WARNING("Could not get data TRT SDO collection \"" << m_mainInputTRTKey << "\"");
    }

    ATH_MSG_VERBOSE("Retrieving MC input TRT SDO collection");
    SG::ReadHandle<InDetSimDataCollection> mcContainer(m_overlayInputTRTKey, m_storeGateMC->name());
    if (!mcContainer.isValid()) {
      ATH_MSG_WARNING("Could not get MC TRT SDO collection \"" << m_overlayInputTRTKey << "\"");
    }

    SG::WriteHandle<InDetSimDataCollection> outputContainer(m_outputTRTKey, m_storeGateOutput->name());
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
    std::auto_ptr<InDetSimDataCollection> ap(m_storeGateMC->retrievePrivateCopy<InDetSimDataCollection>(m_overlayInputSCTKey));

    if (!m_storeGateOutput->record(ap, m_outputSCTKey).isSuccess()) {
      ATH_MSG_WARNING("Problem recording SCT SDO collection");
    }
  }

  if (m_do_Pixel) {
    ATH_MSG_VERBOSE("Retrieving MC input Pixel SDO collection");
    std::auto_ptr<InDetSimDataCollection> ap(m_storeGateMC->retrievePrivateCopy<InDetSimDataCollection>(m_overlayInputPixelKey));

    if (!m_storeGateOutput->record(ap, m_outputPixelKey).isSuccess()) {
      ATH_MSG_WARNING("Problem recording Pixel SDO collection");
    }
  }

  ATH_MSG_DEBUG("InDetSDOOverlay::execute() end");
  return StatusCode::SUCCESS;
}

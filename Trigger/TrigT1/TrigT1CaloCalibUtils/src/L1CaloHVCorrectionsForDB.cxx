/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloHVCorrectionsForDB.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloRxLayersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloHVCorrectionsContainer.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArRecUtils/LArHVCorrTool.h"

L1CaloHVCorrectionsForDB::L1CaloHVCorrectionsForDB(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_triggerTowerCollectionName("TriggerTowers"),
    m_caloCellContainerName("AllCalo"),
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_cells2tt("LVL1::L1CaloCells2TriggerTowers/L1CaloCells2TriggerTowers"),
    m_jmTools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools"),
    m_LArHVCorrTool("LArHVCorrTool"),
    m_rxLayersContainer(0),
    m_hvCorrectionsContainer(0),
    m_firstEvent(true)
{
    declareProperty("TriggerTowerCollectionName", m_triggerTowerCollectionName);
    declareProperty("CaloCellContainerName", m_caloCellContainerName);
    declareProperty("LArHVCorrTool",m_LArHVCorrTool);
}

L1CaloHVCorrectionsForDB::~L1CaloHVCorrectionsForDB()
{
}

StatusCode L1CaloHVCorrectionsForDB::initialize()
{
    StatusCode sc;

    sc = m_ttTool.retrieve();
    if(sc.isFailure()){
      msg(MSG::ERROR) << "Cannot retrieve L1TriggerTowerTool" << endreq;
     return sc;
    }

    sc = m_cells2tt.retrieve();
    if(sc.isFailure()){
      msg(MSG::ERROR) << "Cannot retrieve L1CaloCells2TriggerTowers" << endreq;
     return sc;
    }

    sc = m_jmTools.retrieve();
    if(sc.isFailure()){
      msg(MSG::ERROR) << "Cannot retrieve L1CaloOfflineTriggerTowerTools" << endreq;
      return sc;
    }    

    sc = m_LArHVCorrTool.retrieve();
    if(sc.isFailure()){
      msg(MSG::ERROR) << "Cannot retrieve LArHVCorrTool" << endreq;
      return sc;
    }
    m_jmTools->LArHV(m_LArHVCorrTool);

    m_rxLayersContainer.reset(new L1CaloRxLayersContainer());
    m_hvCorrectionsContainer.reset(new L1CaloHVCorrectionsContainer());

    return StatusCode::SUCCESS;
}

StatusCode L1CaloHVCorrectionsForDB::execute()
{
    if (m_firstEvent) {

	StatusCode sc;

        const TriggerTowerCollection *triggerTowerCollection(0);
        sc = evtStore()->retrieve(triggerTowerCollection, m_triggerTowerCollectionName);
        if(!sc.isSuccess()) {
            msg(MSG::ERROR) << "Cannot retrieve TriggerTowerCollection '"
                  << m_triggerTowerCollectionName << "' from StoreGate." << endreq;
            return sc;
        }

        const CaloCellContainer *caloCellContainer(0);
        sc = evtStore()->retrieve(caloCellContainer, m_caloCellContainerName);
        if(!sc.isSuccess()) {
            msg(MSG::ERROR) << "Cannot retrieve CaloCellContainer '" << m_caloCellContainerName
                  << "'." << endreq;
            return sc;
        }

        // init trigger tower to cell mapping - needed each event?
        if(!m_cells2tt->initCaloCellsTriggerTowers(*caloCellContainer)) {
            msg(MSG::ERROR) << "Can not initialize L1CaloCells2TriggerTowers with CaloCellContainer '"
                  << m_caloCellContainerName << "." << endreq;
            return StatusCode::FAILURE;
        }

        LVL1::TriggerTower *tt;
        TriggerTowerCollection::const_iterator p_itTT = triggerTowerCollection->begin();
        TriggerTowerCollection::const_iterator p_itTTEnd = triggerTowerCollection->end();
        std::vector<unsigned int> coolIdByRx;
	std::vector<std::vector<int> > layernamesByRx;
	std::vector<std::vector<int> > layerNcellsByRx;
	std::vector<float> meanScaleByRx;
	std::vector<std::vector<float> > affectedCellsFByRx;
	std::vector<std::vector<float> > layerMeansByRx;
	std::vector<int> affectedCells;
	std::vector<float>::const_iterator affIt;
	std::vector<float> layerMeans;
	int hvCount = 0;
	int rxCount = 0;

        for(;p_itTT != p_itTTEnd; ++p_itTT) {
          tt = *p_itTT;
	  for (int sample = 0; sample < 2; ++sample) { // Loop over samples (em/had)
	    const bool isTile = (sample == 1 && fabs(tt->eta()) < 1.5);
	    if (isTile) continue;

	    if (sample == 0) {     //em
	      coolIdByRx         = m_jmTools->emRxId(tt);
	      layernamesByRx     = m_jmTools->emTTCellsLayerNamesByReceiver(tt);
	      layerNcellsByRx    = m_jmTools->emNCellsByReceiverByLayer(tt);
	      meanScaleByRx      = m_jmTools->emNonNominalMeanScaleByReceiver(tt);
	      affectedCellsFByRx = m_jmTools->emNCellsNonNominalByReceiverByLayer(tt);
	      layerMeansByRx     = m_jmTools->emNonNominalMeanScaleByReceiverByLayer(tt);
	    } else {               //had
	      coolIdByRx         = m_jmTools->hadRxId(tt);
	      layernamesByRx     = m_jmTools->hadTTCellsLayerNamesByReceiver(tt);
	      layerNcellsByRx    = m_jmTools->hadNCellsByReceiverByLayer(tt);
	      meanScaleByRx      = m_jmTools->hadNonNominalMeanScaleByReceiver(tt);
	      affectedCellsFByRx = m_jmTools->hadNCellsNonNominalByReceiverByLayer(tt);
	      layerMeansByRx     = m_jmTools->hadNonNominalMeanScaleByReceiverByLayer(tt);
	    }
	    const unsigned int nRx = coolIdByRx.size();
	    if (nRx != layernamesByRx.size()     ||
	        nRx != layerNcellsByRx.size()    ||
		nRx != meanScaleByRx.size()      ||
		nRx != affectedCellsFByRx.size() ||
		nRx != layerMeansByRx.size()) {
              msg(MSG::ERROR) << "Vectors by receiver have inconsistent size" << endreq;
	      return StatusCode::FAILURE;
            }
	    rxCount += nRx;
	    for (unsigned int i = 0; i < nRx; ++i) { // Loop over receivers
	      const unsigned int        coolId(coolIdByRx[i]);
	      const std::vector<int>&   layernames(layernamesByRx[i]);
	      const std::vector<int>&   layerNcells(layerNcellsByRx[i]);
	      const float               meanScale(meanScaleByRx[i]);
	      const std::vector<float>& affectedCellsF(affectedCellsFByRx[i]);
	      const std::vector<float>& layerMeans(layerMeansByRx[i]);

	      const unsigned int nLayers = layernames.size();
	      if (nLayers != layerNcells.size()) {
	        msg(MSG::ERROR) << "layernames/layerNcells inconsistent size" << endreq;
	        return StatusCode::FAILURE;
              }
	      if (nLayers == 0 || nLayers > 4) {
	        //msg(MSG::ERROR) << "Unexpected number of layers: " << nLayers << endreq;
	        //return StatusCode::FAILURE;
		msg(MSG::WARNING) << "Unexpected number of layers: " << nLayers
		                  << "  eta/phi: " << tt->eta() << "/" << tt->phi()
				  << "  sample: " << sample
				  << "  Receiver: " << i << endreq;
	        continue;
              }
	      for (unsigned int j = 0; j < nLayers; ++j) {
	        if (layerNcells[j] == 0) {
	          msg(MSG::ERROR) << "Layer " << j << " has no cells" << endreq;
	          return StatusCode::FAILURE;
                }
	        for (unsigned int k = 0; k < j; ++k) {
		  if (layernames[j] == layernames[k]) {
		    msg(MSG::ERROR) << "Duplicate layernames" << endreq;
	            return StatusCode::FAILURE;
                  }
	        }
	      }

	      L1CaloRxLayers l1caloRxLayersSample(coolId, layernames, layerNcells);
	      m_rxLayersContainer->addRxLayers(coolId, l1caloRxLayersSample);

	      if (nLayers != affectedCellsF.size()) {
	        msg(MSG::ERROR) << "layernames/affectedCellsF inconsistent size" << endreq;
	        return StatusCode::FAILURE;
              }
	      if (nLayers != layerMeans.size()) {
	        msg(MSG::ERROR) << "layernames/layerMeans inconsistent size" << endreq;
	        return StatusCode::FAILURE;
              }
	      float sum = 0.;
	      float ncells = 0;
	      for (unsigned int j = 0; j < nLayers; ++j) {
	        sum += layerMeans[j]*layerNcells[j];
	        ncells += layerNcells[j];
	      }
	      if (ncells > 0.) sum /= ncells;
	      const float tol = 1.e-4;
	      if (fabs(sum - meanScale) > tol) {
	        msg(MSG::ERROR) << "Total and layer means inconsistent: "
		                << meanScale << " " << sum << endreq;
	        return StatusCode::FAILURE;
              }
	      bool affected = (meanScale != 1.);
	      if (!affected) {
	        for (unsigned int j = 0; j < layerMeans.size(); ++j) {
	          if (layerMeans[j] != 1.) affected = true;
                }
              }

	      if (affected) {
	        affectedCells.clear();
	        for (affIt = affectedCellsF.begin(); affIt != affectedCellsF.end(); ++affIt) {
	            affectedCells.push_back((int)(*affIt));
                }
		L1CaloHVCorrections l1caloHVCorrectionsSample(coolId, meanScale, affectedCells, layerMeans);
		m_hvCorrectionsContainer->addHVCorrections(coolId, l1caloHVCorrectionsSample);
		hvCount++;
              }
            }
	  }
        }
	const unsigned int ttsize = triggerTowerCollection->size();
	const unsigned int exsize = 3584;
	if (ttsize != exsize) {
	    msg(MSG::ERROR) << "First event has " << ttsize
	                    << " TriggerTowers, expected " << exsize << endreq;
	    return StatusCode::FAILURE;
        } else {
	    msg(MSG::INFO) << "Number of TriggerTowers in first event is "
	                   << ttsize << "  Number of Receivers is " << rxCount << endreq;
	}
	msg(MSG::INFO) << "Number of HV Correction entries is " << hvCount << endreq;
	m_firstEvent = false;

    }
      
    return StatusCode::SUCCESS;
}

StatusCode L1CaloHVCorrectionsForDB::finalize()
{
    DataObject* dObj = m_rxLayersContainer->makePersistent();
    if (dObj) {
      CondAttrListCollection* coll = dynamic_cast<CondAttrListCollection*>(dObj);
      if (coll) {
        StatusCode sc;
        sc = detStore()->record(coll, m_rxLayersContainer->coolOutputKey());
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "Failed to record RxLayersContainer" << endreq;
	    return sc;
        }
      } else {
        msg(MSG::ERROR) << "Could not cast to CondAttrListCollection" << endreq;
	return StatusCode::FAILURE;
      }
    } else {
      msg(MSG::ERROR) << "makePersistent failed for RxLayersContainer" << endreq;
      return StatusCode::FAILURE;
    }
    dObj = m_hvCorrectionsContainer->makePersistent();
    if (dObj) {
      CondAttrListCollection* coll = dynamic_cast<CondAttrListCollection*>(dObj);
      if (coll) {
        StatusCode sc;
        sc = detStore()->record(coll, m_hvCorrectionsContainer->coolOutputKey());
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "Failed to record HVCorrectionsContainer" << endreq;
	    return sc;
        }
      } else {
        msg(MSG::ERROR) << "Could not cast to CondAttrListCollection" << endreq;
	return StatusCode::FAILURE;
      }
    } else {
      msg(MSG::ERROR) << "makePersistent failed for HVCorrectionsContainer" << endreq;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

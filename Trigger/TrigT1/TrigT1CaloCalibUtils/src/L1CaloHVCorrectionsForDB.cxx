/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibUtils/L1CaloHVCorrectionsForDB.h"

#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "TrigT1CaloCalibConditions/L1CaloRxLayersContainer.h"
#include "TrigT1CaloCalibConditions/L1CaloHVCorrectionsContainer.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloOfflineTriggerTowerTools.h"
#include "TrigT1CaloCalibToolInterfaces/IL1CaloCells2TriggerTowers.h"
#include "TrigT1CaloToolInterfaces/IL1TriggerTowerTool.h"
#include "TrigT1CaloEvent/TriggerTowerCollection.h"
#include "CaloEvent/CaloCellContainer.h"

L1CaloHVCorrectionsForDB::L1CaloHVCorrectionsForDB(const std::string& name, ISvcLocator *pSvcLocator)
  : AthAlgorithm(name, pSvcLocator),
    m_triggerTowerCollectionName("TriggerTowers"),
    m_caloCellContainerName("AllCalo"),
    m_ttTool("LVL1::L1TriggerTowerTool/L1TriggerTowerTool"),
    m_jmTools("LVL1::L1CaloOfflineTriggerTowerTools/L1CaloOfflineTriggerTowerTools", this),
    m_rxLayersContainer(0),
    m_hvCorrectionsContainer(0),
    m_firstEvent(true)
{
    declareProperty("TriggerTowerCollectionName", m_triggerTowerCollectionName);
    declareProperty("CaloCellContainerName", m_caloCellContainerName);
}

L1CaloHVCorrectionsForDB::~L1CaloHVCorrectionsForDB()
{
}

StatusCode L1CaloHVCorrectionsForDB::initialize()
{
    ATH_CHECK( m_ttTool.retrieve() );
    ATH_CHECK( m_jmTools.retrieve() );

    ATH_CHECK( m_scaleCorrKey.initialize() );
    ATH_CHECK( m_cablingKey.initialize() );
    m_jmTools->LArHV(m_scaleCorrKey, m_cablingKey);

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
                  << m_triggerTowerCollectionName << "' from StoreGate." << endmsg;
            return sc;
        }

        const CaloCellContainer *caloCellContainer(0);
        sc = evtStore()->retrieve(caloCellContainer, m_caloCellContainerName);
        if(!sc.isSuccess()) {
            msg(MSG::ERROR) << "Cannot retrieve CaloCellContainer '" << m_caloCellContainerName
                  << "'." << endmsg;
            return sc;
        }

        // init trigger tower to cell mapping - needed each event?
        m_jmTools->caloCells(caloCellContainer);
        
        const LVL1::TriggerTower *tt;
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
              msg(MSG::ERROR) << "Vectors by receiver have inconsistent size" << endmsg;
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
	        msg(MSG::ERROR) << "layernames/layerNcells inconsistent size" << endmsg;
	        return StatusCode::FAILURE;
              }
	      if (nLayers == 0 || nLayers > 4) {
	        //msg(MSG::ERROR) << "Unexpected number of layers: " << nLayers << endmsg;
	        //return StatusCode::FAILURE;
		msg(MSG::WARNING) << "Unexpected number of layers: " << nLayers
		                  << "  eta/phi: " << tt->eta() << "/" << tt->phi()
				  << "  sample: " << sample
				  << "  Receiver: " << i << endmsg;
	        continue;
              }
	      for (unsigned int j = 0; j < nLayers; ++j) {
	        if (layerNcells[j] == 0) {
	          msg(MSG::ERROR) << "Layer " << j << " has no cells" << endmsg;
	          return StatusCode::FAILURE;
                }
	        for (unsigned int k = 0; k < j; ++k) {
		  if (layernames[j] == layernames[k]) {
		    msg(MSG::ERROR) << "Duplicate layernames" << endmsg;
	            return StatusCode::FAILURE;
                  }
	        }
	      }

	      L1CaloRxLayers l1caloRxLayersSample(coolId, std::vector<int>(layernames), std::vector<int>(layerNcells));
	      m_rxLayersContainer->addRxLayers(coolId, std::move(l1caloRxLayersSample));

	      if (nLayers != affectedCellsF.size()) {
	        msg(MSG::ERROR) << "layernames/affectedCellsF inconsistent size" << endmsg;
	        return StatusCode::FAILURE;
              }
	      if (nLayers != layerMeans.size()) {
	        msg(MSG::ERROR) << "layernames/layerMeans inconsistent size" << endmsg;
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
		                << meanScale << " " << sum << endmsg;
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
		L1CaloHVCorrections l1caloHVCorrectionsSample(coolId, meanScale, std::move(affectedCells), std::vector<float>(layerMeans));
		m_hvCorrectionsContainer->addHVCorrections(coolId, std::move(l1caloHVCorrectionsSample));
		hvCount++;
              }
            }
	  }
        }
	const unsigned int ttsize = triggerTowerCollection->size();
	const unsigned int exsize = 3584;
	if (ttsize != exsize) {
	    msg(MSG::ERROR) << "First event has " << ttsize
	                    << " TriggerTowers, expected " << exsize << endmsg;
	    return StatusCode::FAILURE;
        } else {
	    msg(MSG::INFO) << "Number of TriggerTowers in first event is "
	                   << ttsize << "  Number of Receivers is " << rxCount << endmsg;
	}
	msg(MSG::INFO) << "Number of HV Correction entries is " << hvCount << endmsg;
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
            msg(MSG::ERROR) << "Failed to record RxLayersContainer" << endmsg;
	    return sc;
        }
      } else {
        msg(MSG::ERROR) << "Could not cast to CondAttrListCollection" << endmsg;
	return StatusCode::FAILURE;
      }
    } else {
      msg(MSG::ERROR) << "makePersistent failed for RxLayersContainer" << endmsg;
      return StatusCode::FAILURE;
    }
    dObj = m_hvCorrectionsContainer->makePersistent();
    if (dObj) {
      CondAttrListCollection* coll = dynamic_cast<CondAttrListCollection*>(dObj);
      if (coll) {
        StatusCode sc;
        sc = detStore()->record(coll, m_hvCorrectionsContainer->coolOutputKey());
        if (sc.isFailure()) {
            msg(MSG::ERROR) << "Failed to record HVCorrectionsContainer" << endmsg;
	    return sc;
        }
      } else {
        msg(MSG::ERROR) << "Could not cast to CondAttrListCollection" << endmsg;
	return StatusCode::FAILURE;
      }
    } else {
      msg(MSG::ERROR) << "makePersistent failed for HVCorrectionsContainer" << endmsg;
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonCoinHierarchy/TgcCoinHierarchyClassifyTool.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonTrigCoinData/TgcCoinDataContainer.h"
#include "MuonPrepRawData/TgcPrepDataContainer.h"

using Muon::TgcCoinData;
using Muon::TgcCoinDataCollection;
using Muon::TgcCoinDataContainer;
using Muon::TgcPrepData;
using Muon::TgcPrepDataCollection;
using Muon::TgcPrepDataContainer;

namespace Trigger {
  TgcCoinHierarchyClassifyTool::TgcCoinHierarchyClassifyTool(const std::string& t, const std::string& n, 
                                                             const IInterface* p)
    : AthAlgTool(t, n, p)
  {
    declareProperty("tgcCoinDataContainerNamePrior", m_tgcCoinDataContainerName[0]="TrigT1CoinDataCollectionPriorBC");
    declareProperty("tgcPrepDataContainerNamePrior", m_tgcPrepDataContainerName[0]="TGC_MeasurementsPriorBC");
    declareProperty("tgcCoinDataContainerNameCurr" , m_tgcCoinDataContainerName[1]="TrigT1CoinDataCollection");
    declareProperty("tgcPrepDataContainerNameCurr" , m_tgcPrepDataContainerName[1]="TGC_Measurements");
    declareProperty("tgcCoinDataContainerNameNext" , m_tgcCoinDataContainerName[2]="TrigT1CoinDataCollectionNextBC");
    declareProperty("tgcPrepDataContainerNameNext" , m_tgcPrepDataContainerName[2]="TGC_MeasurementsNextBC");
  }

  StatusCode TgcCoinHierarchyClassifyTool::queryInterface(const InterfaceID& riid, void** ppvIf) {
    ATH_MSG_DEBUG("queryInterface()");

    if(ITgcCoinHierarchyClassifyTool::interfaceID().versionMatch(riid)) {
      *ppvIf = dynamic_cast<ITgcCoinHierarchyClassifyTool*>(this); 
      addRef();
      ATH_MSG_DEBUG("InterfaceID successfully matched with ITgcCoinHierarchyClassifyTool one.");
      return StatusCode::SUCCESS;
    }

    return AthAlgTool::queryInterface(riid, ppvIf);
  }

  StatusCode TgcCoinHierarchyClassifyTool::initialize() {
    ATH_MSG_DEBUG("initialize()");
    ATH_CHECK( m_idHelperSvc.retrieve() );
    return StatusCode::SUCCESS;
  }

  StatusCode TgcCoinHierarchyClassifyTool::classify(std::vector<TgcCoinHierarchyTriggerSector*>* pTrigSectorCombs) {
    ATH_MSG_DEBUG("classify()");

    // Clear std::vector<TgcCoinHierarchyTriggerSector*>. We don't need old information
    if(pTrigSectorCombs->size()>0) pTrigSectorCombs->clear();

    // Initialization for every event  
    for(unsigned int iSIDE=0; iSIDE<TgcCoinHierarchy::NSIDES; iSIDE++) {
      for(unsigned int iREGION=0; iREGION<TgcCoinHierarchy::NREGIONS; iREGION++) {
        for(unsigned int iPHI=0; iPHI<TgcCoinHierarchy::NPHIS; iPHI++) {
          for(unsigned int iTIMING=0;iTIMING<TgcCoinHierarchy::NTIMING;iTIMING++){
            m_idVector[iSIDE][iREGION][iPHI][iTIMING] = -1;
          }
        }  
      }
    }

    unsigned int sizeTgcCoinData = 0;

    for(unsigned int ibc=0;ibc<TgcCoinHierarchy::NTIMING;ibc++){
      TgcCoinHierarchy::TIMING bc = (ibc==0)?TgcCoinHierarchy::PRIOR:
	(ibc==1)?TgcCoinHierarchy::CURR:
	(ibc==2)?TgcCoinHierarchy::NEXT:
	TgcCoinHierarchy::UNKNOWN;

      if(bc==TgcCoinHierarchy::UNKNOWN){
        ATH_MSG_FATAL("Timing Setup has problem. TIMING:" << bc << " indexBcid:" << ibc);
        return StatusCode::FAILURE;
      }


      // Retrieving data from StoreGate and record them as std::vector<TgcCoinHierarchy> 
      // Coincidence part 
      // Retrive TgcCoinDataContainer
      const TgcCoinDataContainer* coinColl = 0;
      StatusCode sc = evtStore()->retrieve(coinColl, m_tgcCoinDataContainerName[ibc]);
      if(sc.isFailure() || !coinColl) {
        ATH_MSG_WARNING("No TGC coincidence PRD container (" << m_tgcCoinDataContainerName[ibc] << ") found");
        return StatusCode::SUCCESS;
      } 
      // Loop over TgcCoinDataContainer
      TgcCoinDataContainer::const_iterator ic   = coinColl->begin();
      TgcCoinDataContainer::const_iterator ic_e = coinColl->end();
      for(; ic!=ic_e; ic++) {
        TgcCoinDataCollection::const_iterator icc   = (*ic)->begin();
        TgcCoinDataCollection::const_iterator icc_e = (*ic)->end();
        for(; icc!=icc_e; icc++) {
          int type = (*icc)->type();
          // Check type is valid Muon::TgcCoinData::CoinDataType 
          if(type!=TgcCoinData::TYPE_SL && type!=TgcCoinData::TYPE_HIPT && type!=TgcCoinData::TYPE_TRACKLET) continue;

          // Check trigger sector is valid
          unsigned int phi = (*icc)->phi();
          if(phi==0 || phi>=TgcCoinHierarchy::NPHIS) continue;

          unsigned int isAside = static_cast<unsigned int>((*icc)->isAside());
          unsigned int isForward = static_cast<unsigned int>((*icc)->isForward());

          // This coincidence belongs to a new trigger sector. 
          // A new TgcCoinHierarchyTriggerSector will be created
          if(m_idVector[isAside][isForward][phi][ibc]<0) {
            ATH_MSG_DEBUG("New trigger sector " << (!isAside ? "C" : "A") << (!isForward ? "E" : "F") << phi << " " 
                          <<  pTrigSectorCombs->size()); 
            m_idVector[isAside][isForward][phi][ibc] = pTrigSectorCombs->size();

            TgcCoinHierarchyTriggerSector* newTrigSectorComb = new TgcCoinHierarchyTriggerSector;
            if(!newTrigSectorComb) {
              ATH_MSG_FATAL("new TgcCoinHierarchyTriggerSector falied in classify()");
              return StatusCode::FAILURE;
            }

            // The created new TgcCoinHierarchyTriggerSector is added to std::vector<TgcCoinHierarchyTriggerSector*>*
            pTrigSectorCombs->push_back(newTrigSectorComb);
          }

          // Get the corresponding TgcCoinHierarchyTriggerSector 
          TgcCoinHierarchyTriggerSector* p_TrigSectorComb = pTrigSectorCombs->at(m_idVector[isAside][isForward][phi][ibc]);
          if(!p_TrigSectorComb) {
            ATH_MSG_FATAL("Correspond TgcCoinHierarchyTriggerSector not found in coincidence part. " 
                          << "This should never happen.");
            return StatusCode::FAILURE;
          }
        
          // Add this coincidence to the corresponding TgcCoinHierarchyTriggerSector
          TgcCoinHierarchy::COINCIDENCE p_CoinType = m_TCH.CoinDataTypeToCOINCIDENCE((*icc)->type(), (*icc)->isStrip());
          if(p_CoinType==TgcCoinHierarchy::NOTCOINCIDENCE) continue; 
          p_TrigSectorComb->setTiming(bc);
          p_TrigSectorComb->addCoincidence(*icc, p_CoinType);
          sizeTgcCoinData++;

          // Dump TgcCoinData
          ATH_MSG_DEBUG(*(*icc));
        }
      }

    }
    
    // Hit part 
    unsigned int sizeTgcPrepData = 0;

    for(unsigned int ibc=0;ibc<TgcCoinHierarchy::NTIMING;ibc++){
      TgcCoinHierarchy::TIMING bc = (ibc==0)?TgcCoinHierarchy::PRIOR:
	(ibc==1)?TgcCoinHierarchy::CURR:
	(ibc==2)?TgcCoinHierarchy::NEXT:
	TgcCoinHierarchy::UNKNOWN;

      if(bc==TgcCoinHierarchy::UNKNOWN){
        ATH_MSG_FATAL("Timing Setup has problem. TIMING:" << bc << " indexBcid:" << ibc);
        return StatusCode::FAILURE;
      }


      // Retrieve TgcPrepDataContainer
      const TgcPrepDataContainer* hitColl = 0;
      StatusCode sc = evtStore()->retrieve(hitColl, m_tgcPrepDataContainerName[ibc]);
      if(sc.isFailure() || !hitColl) {
        ATH_MSG_WARNING("No TGC hit PRD container (" << m_tgcPrepDataContainerName[ibc] << ") found");
        return StatusCode::SUCCESS;
      } 
      // Loop over TgcPrepDataContainer
      TgcPrepDataContainer::const_iterator ih   = hitColl->begin();
      TgcPrepDataContainer::const_iterator ih_e = hitColl->end();
      for(; ih!=ih_e; ih++) {
        TgcPrepDataCollection::const_iterator ihh   = (*ih)->begin();
        TgcPrepDataCollection::const_iterator ihh_e = (*ih)->end();
        for(; ihh!=ihh_e; ihh++) {
          Identifier p_identify = (*ihh)->identify();
          TgcCoinHierarchy::STATION p_station = getSTATION(p_identify);
          if(p_station==TgcCoinHierarchy::NOTSTATION) continue; 
          unsigned int p_isAside = 0;
          unsigned int p_isForward = 0;
          unsigned int p_phi = 0;

          // Get the trigger sector to which this hit belongs
          bool isOK = getTriggerSector(p_identify, p_isAside, p_isForward, p_phi);
          if(!isOK) continue;

          // Check that the trigger sector has any coincidence. If not found, the hit will be skipped 
          if(m_idVector[p_isAside][p_isForward][p_phi][ibc]<0) continue;

          // Get the corresponding TgcCoinHierarchyTriggerSector
          TgcCoinHierarchyTriggerSector* p_TrigSectorComb 
            = pTrigSectorCombs->at(m_idVector[p_isAside][p_isForward][p_phi][ibc]);
          if(!p_TrigSectorComb) {
            ATH_MSG_FATAL("Correspond TgcCoinHierarchyTriggerSector not found in hit part. This should never happen.");
            return StatusCode::FAILURE;
          }

          // Add this hit to the corresponding TgcCoinHierarchyTriggerSector
          TgcCoinHierarchy::TYPE p_type = m_TCH.isStripToTYPE(m_idHelperSvc->tgcIdHelper().isStrip(p_identify));
          p_TrigSectorComb->setTiming(bc);
          p_TrigSectorComb->addHit(*ihh, p_type, p_station);
          sizeTgcPrepData++;
        }
      }
    }
    
    ATH_MSG_DEBUG("The number of sectors with TgcCoinData is " << pTrigSectorCombs->size() << 
                  " the number of TgcCoinData is " << sizeTgcCoinData <<
                  " the number of TgcPrepData is " << sizeTgcPrepData); 

    return StatusCode::SUCCESS;
  }

  TgcCoinHierarchy::STATION TgcCoinHierarchyClassifyTool::getSTATION(Identifier identify) const {
    if(!m_idHelperSvc->isTgc(identify)) return TgcCoinHierarchy::NOTSTATION;

    int stationName = m_idHelperSvc->tgcIdHelper().stationName(identify);
    return m_TCH.stationNameToSTATION(stationName);
  }

  bool TgcCoinHierarchyClassifyTool::getTriggerSector(Identifier identify, 
                                                      unsigned int& isAside, 
                                                      unsigned int& isForward, 
                                                      unsigned int& phi) const {
    // Check that Identifier is one of TGC
    if(!m_idHelperSvc->isTgc(identify)) return false;

    // Check that Identifier is in TGC Big Wheel
    TgcCoinHierarchy::STATION station = getSTATION(identify);
    if(station==TgcCoinHierarchy::NOTSTATION) return false;

    int stationName = m_idHelperSvc->tgcIdHelper().stationName(identify);
    isForward = static_cast<unsigned int>(m_TCH.stationNameToIsForward(stationName));
    isAside = static_cast<unsigned int>(m_idHelperSvc->tgcIdHelper().stationEta(identify)>0);

    phi = static_cast<unsigned int>(m_idHelperSvc->tgcIdHelper().stationPhi(identify));
    if(phi==0 || phi>=TgcCoinHierarchy::NPHIS) return false;

    return true;
  }

  bool TgcCoinHierarchyClassifyTool::isStrip(Identifier identify) const {
    return m_idHelperSvc->tgcIdHelper().isStrip(identify);
  } 

} // end of namespace Trigger

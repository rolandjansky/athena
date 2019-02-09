/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT, PESA algorithms
 * @Package: TrigL2MissingET
 * @class  : L2CaloMissingET
 *
 * @brief  PESA algorithm that takes the LVL2 MissingET from LVL1
 *
 * @author  Denis Oliveira Damazio - Brookhaven National Laboratory
 *
 * File and Version Information:
 * $Id: T2CaloMissingET.h 740613 2016-04-15 14:48:44Z damazio $
 **********************************************************************************/



#ifndef TRIGL2MISSINGET_L2CALOMISSINGET_H
#define TRIGL2MISSINGET_L2CALOMISSINGET_H


#include "TrigInterfaces/AllTEAlgo.h"
#include "GaudiKernel/ToolHandle.h"
#include "LArRecEvent/LArFebEnergyCollection.h"
#include "IRegionSelector/RegSelEnums.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArReadoutModuleService.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "TrigT2CaloCommon/ITrigDataAccess.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "LArCabling/LArOnOffIdMapping.h"
#include "LArRecConditions/LArFebRodMapping.h"


#include "xAODTrigMissingET/TrigMissingET.h"

#include <vector>

class MsgStream;
class TrigTimerSvc;


namespace HLT {
  class TriggerElement;
}


class T2CaloMissingET : public HLT::AllTEAlgo
  {
  public:

    T2CaloMissingET(const std::string& name, ISvcLocator* pSvcLocator); //!< std Gaudi Algorithm constructor

    HLT::ErrorCode hltInitialize(); //!< hlt initialize, doing nothing here
    HLT::ErrorCode hltFinalize();   //!< hlt finalize, doing nothing here

    /**
     * @brief implementation of the abstract hltExecute method in HLT::AllTEAlgo.
     *
     * @param input outer vector describeds the different input TE types,
                    here we expect: 1st LVL1 energy TE;
                    inner vector provides all TE instances of the given type
     * @param output the output TE type
     */
    HLT::ErrorCode hltExecute(std::vector<std::vector<HLT::TriggerElement*> >& input,
			      unsigned int output);

    HLT::ErrorCode hltEndEvent() { m_useCachedResult = false; m_met_feature = 0; m_cachedTE=0; return HLT::OK; };

    // virtual bool reset() {m_useCachedResult = false; m_met_feature = 0; m_cachedTE=0; AllTEAlgo::reset(); return true;}
    
    HLT::ErrorCode init(xAOD::TrigMissingET *met); 

    using HLT::AllTEAlgo::prepareRobRequests;
    HLT::ErrorCode prepareRobRequests(const std::vector<HLT::TEConstVec>& inputs );

  private:

     SG::ReadCondHandleKey<LArOnOffIdMapping> m_cablingKey{this,"CablingKey","LArOnOffIdMap","SG Key of LArOnOffIdMapping object"};
     SG::ReadCondHandleKey<LArFebRodMapping>  m_RodKey{this, "FebRodKey", "LArFebRodMap", "SG ROD mapping key"};

     bool m_useCachedResult;

     std::string m_featureLabel; //!< label for the MET feature in the HLT Navigation
     std::string m_L2L1featureLabel; //!< label for the L2=l1 MET feature 

     ToolHandle<ITrigDataAccess>   m_data;
     MsgStream* m_log;
     ServiceHandle<ITrigTimerSvc> m_timersvc;
     float m_etaWidth,m_phiWidth;
     TrigTimer* m_timer[2];
     float m_Ex, m_Ey, m_Ez, m_met, m_E, m_Et;
//     float m_ExT, m_EyT, m_EzT;
     bool m_one_by_one;
     float m_l1metDiffCut;
     std::vector<DETID> m_detid;
     bool m_ReadL2L1;

     xAOD::TrigMissingET* m_met_feature;    //!< internal caching: missing E_T feature of the first execution
     HLT::TriggerElement* m_cachedTE; //!< internal caching: output TE from the first exectution

     const LArOnlineID *m_LArOnlineID;
     const CaloCell_ID *m_CaloCell_ID;
     LArReadoutModuleService m_larROModSvc;

     HLT::TriggerElement* makeSeeding(std::vector<std::vector<HLT::TriggerElement*> >& input, unsigned int type_out ) ;
     std::vector<float> m_tile_lba; 
     std::vector<float> m_tile_lbc; 
     std::vector<float> m_tile_eba; 
     std::vector<float> m_tile_ebc; 
     std::vector<unsigned char> m_tile_nlba; 
     std::vector<unsigned char> m_tile_nlbc; 
     std::vector<unsigned char> m_tile_neba; 
     std::vector<unsigned char> m_tile_nebc; 
     std::vector<uint32_t> m_vec_robs;
};

#endif


/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGL2CALORINGERFEX_H
#define TRIGL2CALORINGERFEX_H

///std include(s)
#include <string>
#include <vector>

///Base from trigger
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/TrigTimer.h"

///xAOD include(s)
#include "xAODTrigRinger/TrigRingerRings.h"
#include "xAODTrigRinger/TrigRNNOutput.h"
#include "xAODTrigCalo/TrigEMCluster.h"

/// Luminosity tool
#include "LumiBlockComps/ILumiBlockMuTool.h"

// ringer libs
#include "TrigMultiVarHypo/tools/procedures/IModel.h"
#include "TrigMultiVarHypo/tools/procedures/INormalization.h"
#include "TrigMultiVarHypo/tools/common/RingerReader.h"

#include<memory>

class TrigL2CaloRingerFex: public HLT::FexAlgo {
 
  public:

    TrigL2CaloRingerFex(const std::string & name, ISvcLocator* pSvcLocator);
    virtual ~TrigL2CaloRingerFex()
    {;}

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* /*inputTE*/, HLT::TriggerElement* outputTE);
    HLT::ErrorCode hltFinalize();

  protected:
    ///Time monitoring
    TrigTimer* m_storeTimer;
    TrigTimer* m_normTimer;
    TrigTimer* m_totalTimer;
    TrigTimer* m_propagateTimer;

  private:

    /* Helper functions for feature extraction */
    const xAOD::TrigRingerRings* get_rings(const HLT::TriggerElement* te);
    /* Helper functions for feature extraction */
    const xAOD::TrigEMCluster* get_cluster(const HLT::TriggerElement* te);

    std::string m_calibPath;
    ///feature keys
    std::string m_hlt_feature;
    std::string m_feature;
    std::string m_key;

    float       m_lumiCut;
    float       m_output;
    bool        m_useLumiTool;
    
    //LumiTool
    ToolHandle<ILumiBlockMuTool>  m_lumiBlockMuTool;
    
    ///Discriminator holder 
    std::vector<std::shared_ptr<Ringer::IModel>>          m_discriminators;
    std::vector<std::shared_ptr<Ringer::INormalization>>  m_preproc; 
    
    Ringer::RingerReader m_reader;

};
//!===============================================================================================
/// get the cluster inside of container
const xAOD::TrigEMCluster* TrigL2CaloRingerFex::get_cluster(const HLT::TriggerElement* te) {
    const xAOD::TrigEMCluster *pattern = nullptr;
    HLT::ErrorCode status = getFeature(te, pattern, m_feature);
    return (status == HLT::OK) ? pattern : nullptr;
}
//!===============================================================================================
// get the ringer rings inside of container
const xAOD::TrigRingerRings* TrigL2CaloRingerFex::get_rings(const HLT::TriggerElement* te){
    const xAOD::TrigRingerRings *pattern = nullptr;
    HLT::ErrorCode status = getFeature(te, pattern, m_feature);
    return (status == HLT::OK) ? pattern : nullptr;
}
//!===============================================================================================
#endif /* TRIGL2CALORINGERHYPO_H */

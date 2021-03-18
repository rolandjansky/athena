/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOTOOLINC_H
#define TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOTOOLINC_H 1

#include "xAODTrigCalo/TrigEMCluster.h"

#include "xAODTrigRinger/TrigRingerRings.h"
#include "TrigMultiVarHypo/tools/RingerSelectorTool.h"
#include "RingerSelectorTools/tools/onnx/RingerSelectorTool.h"
#include "AthOnnxruntimeService/IONNXRuntimeSvc.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaFastCaloHypoTool.h"

/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigEgammaFastCaloHypoToolInc : public extends<AthAlgTool, ITrigEgammaFastCaloHypoTool> { 
  
  public: 

    TrigEgammaFastCaloHypoToolInc( const std::string& type, const std::string& name, const IInterface* parent );
 
    virtual ~TrigEgammaFastCaloHypoToolInc();

    virtual StatusCode initialize() override;
 
    virtual StatusCode decide( std::vector<ITrigEgammaFastCaloHypoTool::FastClusterInfo>& input )  const override;
 
    virtual bool decide( const ITrigEgammaFastCaloHypoTool::FastClusterInfo& i ) const override;
 


 
  private:

    bool decide_cutbased( const ITrigEgammaFastCaloHypoTool::FastClusterInfo& i ) const;
    bool decide_ringer( const ITrigEgammaFastCaloHypoTool::FastClusterInfo& i ) const;
    int findCutIndex( float eta ) const;
   

    HLT::Identifier m_decisionId;
    Ringer::RingerSelectorTool        m_selectorTool;
    Ringer::onnx::RingerSelectorTool  m_onnxSelectorTool;
    ToolHandle<ILumiBlockMuTool>      m_lumiBlockMuTool;

    
    Gaudi::Property<std::string>      m_configFile{this, "ConfigFile", "", "Ringer Run3 Calib Path"};  
    Gaudi::Property<std::string>      m_constantsCalibPath{this, "ConstantsCalibPath", "", "Ringer Run2 Constants Calib Path"};  
    Gaudi::Property<std::string>      m_thresholdsCalibPath{this, "ThresholdsCalibPath", "", "Ringer Run2 Thresholds Calib Path"};  
    Gaudi::Property< bool >           m_useRinger { this, "UseRinger", false , "Use Ringer Selection" };
    Gaudi::Property< bool >           m_useRun3 { this, "UseRun3"  , false , "Use Ringer Onnx Selection (for Run3)" };

 
    //Calorimeter electron ID  cuts
    Gaudi::Property< std::vector<float> > m_etabin { this, "EtaBins", {} , "Bins of eta" }; //!<  selection variable for L2 calo selection:eta bins
    Gaudi::Property< std::vector<float> > m_eTthr { this, "ETthr", {}, "ET Threshold" };
    Gaudi::Property< std::vector<float> > m_eT2thr { this, "ET2thr", {}, "Second layer ET threshold" };
    Gaudi::Property< std::vector<float> > m_hadeTthr { this, "HADETthr", {}, "" };
    Gaudi::Property< std::vector<float> > m_hadeT2thr { this, "HADET2thr", {}, "" };
    Gaudi::Property< std::vector<float> > m_carcorethr { this, "CARCOREthr", {}, "" };
    Gaudi::Property< std::vector<float> > m_caeratiothr { this, "CAERATIOthr", {}, "" };
    Gaudi::Property< std::vector<float> > m_F1thr { this, "F1thr", {}, "" };
    Gaudi::Property< std::vector<float> > m_WETA2thr { this, "WETA2thr", {}, "" };
    Gaudi::Property< std::vector<float> > m_WSTOTthr { this, "WSTOTthr", {}, "" };
    Gaudi::Property< std::vector<float> > m_F3thr { this, "F3thr", {}, "" };
    Gaudi::Property< float >              m_detacluster { this, "dETACLUSTERthr", 0. , "" };
    Gaudi::Property< float >              m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  
    Gaudi::Property< bool >               m_acceptAll { this, "AcceptAll", false , "Ignore selection" };
    Gaudi::Property<float>                m_emEtCut{this,"EtCut", 0.0, "Et threshold"};  
 

    ToolHandle< GenericMonitoringTool >       m_monTool{ this, "MonTool", "", "Monitoring tool" };
    ServiceHandle< AthONNX::IONNXRuntimeSvc > m_onnxSvc{ this, "ONNXRuntimeSvc", "AthONNX::ONNXRuntimeSvc", "Name of the service to use" };
    

}; 

#endif //> !TRIGEGAMMAHYPO_TRIGEGAMMAFASTCALOHYPOTOOLINC_H

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TrigEgammaForwardFastCaloHypoTool_h
#define TrigEgammaForwardFastCaloHypoTool_h

#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "RingerSelectorTools/IAsgRingerSelectorTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "ITrigEgammaForwardFastCaloHypoTool.h"

/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/

class TrigEgammaForwardFastCaloHypoTool : public extends<AthAlgTool, ITrigEgammaForwardFastCaloHypoTool> { 
  
  public: 

    TrigEgammaForwardFastCaloHypoTool( const std::string& type, const std::string& name, const IInterface* parent );
 
    virtual ~TrigEgammaForwardFastCaloHypoTool();

    virtual StatusCode initialize() override;
 
    virtual StatusCode decide( std::vector<ITrigEgammaForwardFastCaloHypoTool::FastClusterInfo>& input )  const override;
 
    virtual bool decide( const ITrigEgammaForwardFastCaloHypoTool::FastClusterInfo& i ) const override;
 


 
  private:
   

    HLT::Identifier m_decisionId;
    ToolHandle<ILumiBlockMuTool>      m_lumiBlockMuTool;


 
    Gaudi::Property< bool >               m_acceptAll { this, "AcceptAll", false , "Ignore selection" };
    Gaudi::Property<float>                m_emEtCut{this,"EtCut", 0.0, "Et threshold"};  
 

    ToolHandle< GenericMonitoringTool >       m_monTool{ this, "MonTool", "", "Monitoring tool" };
    ToolHandle<Ringer::IAsgRingerSelectorTool>m_ringerTool{ this, "RingerSelector", "", "Ringer tool" };


}; 

#endif //> !TRIGEGAMMAHYPO_TRIGEGAMMAForwardFastCaloHYPOTOOLINC_H

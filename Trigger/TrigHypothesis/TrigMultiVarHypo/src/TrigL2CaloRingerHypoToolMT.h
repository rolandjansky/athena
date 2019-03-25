/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2CALORINGERHYPOTOOLMT_H
#define TRIGL2CALORINGERHYPOTOOLMT_H 1

#include "GaudiKernel/Property.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"
#include "xAODTrigRinger/TrigRingerRings.h"
#include "TrigMultiVarHypo/tools/RingerSelectorTool.h"
#include "LumiBlockComps/ILumiBlockMuTool.h"

class TrigL2CaloRingerHypoToolMT : virtual public ::AthAlgTool
{ 
  public: 
    TrigL2CaloRingerHypoToolMT( const std::string& type, 
                                const std::string& name, 
                                const IInterface* parent );

    virtual ~TrigL2CaloRingerHypoToolMT();
    
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;


    
    bool decideOnSingleObject( const xAOD::TrigRingerRings* ringerShape ) const;


    struct RingerInfo {
      TrigCompositeUtils::Decision* decision;
      const xAOD::TrigRingerRings* ringerShape;
    };

    StatusCode decide( std::vector<RingerInfo>& decisions )  const;

  private:
    
    Ringer::RingerSelectorTool        m_selectorTool;
    ToolHandle<ILumiBlockMuTool>      m_lumiBlockMuTool;
    ToolHandle<GenericMonitoringTool> m_monTool{ this, "MonTool", "", "Monitoring tool" };
    HLT::Identifier                   m_decisionId;
    Gaudi::Property<bool>             m_acceptAll{ this, "AcceptAll", false, "Ignore selection" };
    Gaudi::Property<double>           m_emEtCut{this,"EtCut", 0.0, "Et threshold"};
    Gaudi::Property<std::string>      m_constantsCalibPath{this, "ConstantsCalibPath", "", "Constants Calib Path"};
    Gaudi::Property<std::string>      m_thresholdsCalibPath{this, "ThresholdsCalibPath", "", "Thresholds Calib Path"};

}; 

//:DECLARE_TOOL_FACTORY( TrigL2CaloRingerHypoToolMT )
#endif //> ! TRIGL2CALORINGERTOOL_H

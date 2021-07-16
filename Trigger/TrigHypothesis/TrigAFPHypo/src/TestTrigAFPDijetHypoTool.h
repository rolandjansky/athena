/* 
   Copyright! (C) 2002-2021 CERN for the benefit of the ATLAS collaboration 
*/
#ifndef TRIGAFPHYPO_TestTrigAFPDijetHypoTool_H
#define TRIGAFPHYPO_TestTrigAFPDijetHypoTool_H

#include "Gaudi/Property.h" 
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/HLTIdentifier.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODJet/Jet.h"
#include "xAODForward/AFPTrackContainer.h"
#include "xAODForward/AFPTrack.h"
#include "AthenaMonitoringKernel/Monitored.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"
#include "IAFPProtonTransportTool.h"
#include "AFPProtonTransportTool.h"

class TestTrigAFPDijetHypoTool : virtual public ::AthAlgTool
{
 public:
  TestTrigAFPDijetHypoTool(const std::string &type,                
			   const std::string &name,                
			   const IInterface *parent);

  virtual StatusCode initialize() override;

  struct TrigAFPDijetHypoInfo
  {
    const TrigCompositeUtils::DecisionIDContainer previousDecisionIDs;
    const xAOD::JetContainer *jetInputCollection;
    const xAOD::AFPTrackContainer *afpTracksInputCollection;
    TrigCompositeUtils::Decision *decision;
  };

  StatusCode decide(TrigAFPDijetHypoInfo &decisions) const;

 private:
  HLT::Identifier m_decisionId;

  //Proton transport parameterizaton file for A side
  Gaudi::Property<std::string> m_protonTransportParamFileNameA{this,"protonTransportParamFileName1","final_parameterization_b1.txt","Name of the file with proton transport parameterization for A side"};
  //Proton transport parameterizaton file for C side 
  Gaudi::Property<std::string> m_protonTransportParamFileNameC{this,"protonTransportParamFileName2","final_parameterization_b2.txt","Name of the file with proton transport parameterization C side"};
  //Proton transport parameterizaton object used in the proton position prediction for ATLAS A side
  ToolHandle<IAFPProtonTransportTool> m_transportBeamA;
  ///Proton transport parameterizaton object used in the proton position prediction for ATLAS C side
  ToolHandle<IAFPProtonTransportTool> m_transportBeamC;
  ///Threshold for the radius distance between parameterization and measurements (in mm)
  Gaudi::Property<float> m_maxProtonDist{this,"maxProtonDist",2.0,"Cut on distance between measured and extrapolated proton"};
  ///Threshold for the x distance between parameterization and measurements (in mm)
  Gaudi::Property<float> m_maxProtonDiff_x{this,"maxProtonDiff_x",2.5,"Cut on x difference between measured and extrapolated proton"};
  ///Threshold for the y distance between parameterization and measurements (in mm)                                 
  Gaudi::Property<float> m_maxProtonDiff_y{this,"maxProtonDiff_y",2.5,"Cut on y difference between measured and extrapolated proton"};
  Gaudi::Property<float> m_beamEnergy{this,"beamEnergy",6500.0,"Energy of one beam i.e. half of centre-of-mass energy"};       ///< energy of one beam i.e. half of centre-of-mass energy
  float m_totalEnergy = 2*m_beamEnergy;      ///< beams centre-of-mass energy 2*#m_beamEnergy
  ///Shift in x position between parameterization and measurements (in mm)
 Gaudi::Property<float> m_protonPosShift_x{this,"protonPosShift_x",-97.0,"Expected difference in x position between measured and predicted proton"};
 ///Shif in y position between parameterization and measurements (in mm)
 Gaudi::Property<float> m_protonPosShift_y{this,"protonPosShift_y",5.0,"Expected difference in y position between measured and predicted proton"};
 ///Beam alignment corrections in the x position of ATLAS A side (in mm)
 Gaudi::Property<float> m_alignmentCorrection_nearA{this,"alignmentCorrection_nearA",0.0,"Shift due to alignment in x for protons in near station A"};
 ///Beam alignment corrections in the x position of ATLAS C side (in mm)          
 Gaudi::Property<float> m_alignmentCorrection_nearC{this,"alignmentCorrection_nearC",0.0,"Shift due to alignment in x for protons in near station C"};
 ///Variable to convert from MeV to GeV                   
 const float m_GeV = 0.001;
 // monitoring                                             
 ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "", "Monitoring tool" };

};

#endif // TRIGAFPHYPO_TestTrigAFPDijetHypoTool_H

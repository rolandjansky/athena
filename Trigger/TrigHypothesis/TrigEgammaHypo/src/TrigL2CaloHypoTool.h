/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H
#define TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H 1

//#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaMonitoring/GenericMonitoringTool.h"
#include "DecisionHandling/HLTIdentifier.h"
#include "DecisionHandling/TrigCompositeUtils.h"


static const InterfaceID IID_TrigL2CaloHypoTool("TrigL2CaloHypoTool", 1, 0);

/**
 * @class Implementation of the Egamma selection for CaloClusters
 * @brief 
 **/



class TrigL2CaloHypoTool
  : virtual public ::AthAlgTool
{ 
 public: 


  TrigL2CaloHypoTool( const std::string& type, 
		      const std::string& name, 
		      const IInterface* parent );

  virtual ~TrigL2CaloHypoTool();
  StatusCode initialize() override;
  StatusCode finalize() override;

  static const InterfaceID& interfaceID();

  struct Input {
    Input(TrigCompositeUtils::Decision* d, const TrigRoiDescriptor* r, const xAOD::TrigEMCluster* c)
    : decision(d), roi(r), cluster(c) {}
    TrigCompositeUtils::Decision* decision;
    const TrigRoiDescriptor* roi;
    const xAOD::TrigEMCluster* cluster;
  };

  /**
   * @brief decides upon all clusters
   * Note it is for a reason a non-virtual method, it is an interface in gaudi sense and implementation.
   * There will be many tools called often to perform this quick operation and we do not want to pay for polymorphism which we do not need to use.
   * Will actually see when N obj hypos will enter the scene
   **/
  StatusCode decide( std::vector<Input>& input )  const;
  
 private:
  HLT::Identifier m_decisionId;

  
  bool singleObjectDecision( const Input& i, int selectionIndex = 0 ) const;


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
  Gaudi::Property< float > m_detacluster { this, "dETACLUSTERthr", 0. , "" };
  Gaudi::Property< float > m_dphicluster { this, "dPHICLUSTERthr", 0. , "" };  
  Gaudi::Property< bool > m_acceptAll { this, "AcceptAll", false , "Ignore selection" };
  Gaudi::Property< int > m_multiplicity { this, "Multiplicity", 1, "Multiplicity, when >1 all the cuts need to be duplicated" };

  ToolHandle<GenericMonitoringTool> m_monTool { this, "MonTool", "GenericMonitoringTool/MonTool", "Monitoring tool" };
}; 

inline const InterfaceID& TrigL2CaloHypoTool::interfaceID() 
{ 
   return IID_TrigL2CaloHypoTool; 
}


#endif //> !TRIGEGAMMAHYPO_TRIGL2CALOHYPOTOOL_H

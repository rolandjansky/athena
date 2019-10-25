/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETREC_JETCLUSTERER_H
#define JETREC_JETCLUSTERER_H

#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"

#include "JetInterface/IJetProvider.h"

#include "JetRec/PseudoJetContainer.h"
#include "JetRec/JetFromPseudojet.h"


#include "fastjet/PseudoJet.hh"
#include "fastjet/AreaDefinition.hh"
#include "fastjet/JetDefinition.hh"


class JetClusterer
: public asg::AsgTool,
  virtual public IJetProvider {
ASG_TOOL_CLASS(JetClusterer, IJetProvider)

public:

  JetClusterer(std::string name);

  virtual StatusCode initialize() override;

  /// Return the final jets. Can return a void pointer if an error occurs.
  virtual xAOD::JetContainer* build() const;


protected:

  /// Build the area definition when running with area calculation. The seedsok flag is set to false when error occurs when retrieving event/run number to initiate the seeds.
  fastjet::AreaDefinition buildAreaDefinition(bool &seedsok) const ;
    
  /// Handle to EventInfo. This is used to get the evt&run number to set the Ghost area random seeds.
  SG::ReadHandleKey<xAOD::EventInfo> m_eventinfokey{"EventInfo"};

  /// Handle Input PseudoJetContainer
  SG::ReadHandleKey<PseudoJetContainer> m_inputPseudoJets= {this, "InputPseudoJets", "inputpseudojet", "input constituents"};

  /// used to build the key under which the final PJ will be stored in evtStore() 
  std::string m_finalPSeudoJets;
  
  // Job options.
  Gaudi::Property<std::string>  m_jetalg {this, "JetAlgorithm", "AntiKt", "alg type : AntiKt, Kt, CA..."};
  Gaudi::Property<float> m_jetrad        {this, "JetRadius", 0.4 , "jet size parameter"}; 
  Gaudi::Property<float> m_ptmin         {this, "PtMin", 0.0, "pT min in MeV"};
  Gaudi::Property<float> m_ghostarea     {this, "GhostArea", 0.0, "Area for ghosts. 0==>no ghosts."};
  Gaudi::Property<int> m_ranopt          {this, "RandomOption", 0, "Rand option: 0=fj default, 1=run/event"};

  Gaudi::Property<int> m_inputType       {this, "JetInputType", 0, "input type as in xAOD::JetInput (see xAODJet/JetContainerInfo.h)"};

  
  // internal data set from properties during initialize()
  fastjet::JetAlgorithm m_fjalg;

  
  // support of variable R to be transferred to a dedicated IJetProvider
  // Gaudi::Property<float> m_minrad        {this, "VariableRMinRadius", -1.0, "Variable-R min radius" };
  // Gaudi::Property<float> m_massscale     {this, "VariableRMassScale", -1.0, "Variable-R mass scale" };
  // bool m_isVariableR;  
  // bool isVariableR()const { return m_isVariableR;}
  
};


namespace JetClustererHelper {

  /// Fill seeds vector from run & event number. This functio is separated from the class so it's easier to re-use and test
  void seedsFromEventInfo(const xAOD::EventInfo* ei, std::vector<int> &seeds);
  
}

#endif

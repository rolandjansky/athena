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
#include "fastjet/JetDefinition.hh"


class JetClusterer
: public asg::AsgTool,
  virtual public IJetProvider {
ASG_TOOL_CLASS(JetClusterer, IJetProvider)

public:

  JetClusterer(std::string name);

  virtual StatusCode initialize() override;

  virtual xAOD::JetContainer* build() const;


protected:

  SG::ReadHandleKey<xAOD::EventInfo> m_eventinfokey{"EventInfo"};


  SG::ReadHandleKey<PseudoJetContainer> m_inputPseudoJets= {this, "InputPseudoJets", "inputpseudojet", "input constituents"};

  // Job options.
  Gaudi::Property<std::string>  m_jetalg  {this, "JetAlgorithm", "AntiKt", "alg type : AntiKt, Kt, CA..."};
  Gaudi::Property<float> m_jetrad {this,  "JetRadius", 0.4 , "jet size parameter"}; 
  Gaudi::Property<float> m_minrad {this , "VariableRMinRadius", -1.0, "Variable-R min radius" };
  Gaudi::Property<float> m_massscale {this , "VariableRMassScale", -1.0, "Variable-R mass scale" };

  Gaudi::Property<float> m_ptmin {this, "PtMin", 0.0, "pT min in MeV"};
  Gaudi::Property<float> m_ghostarea {this, "GhostArea", 0.0, "Area for ghosts. 0==>no ghosts."};
  Gaudi::Property<int> m_ranopt {this, "RandomOption", 0, "Rand option: 0=fj default, 1=run/event"};

  Gaudi::Property<int> m_inputType {this, "JetInputType", {}, "input type"};
  
  // Data
  fastjet::JetAlgorithm m_fjalg;
  bool m_isVariableR;


  JetFromPseudojet m_jetFromPseudoJet;
  
  bool isVariableR()const { return m_isVariableR;}
  

};

#endif

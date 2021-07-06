/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoCommon/Types.h"
#include "L1TopoCommon/Exception.h"

// Configuration
std::string
TCS::inputTypeAsString(TCS::inputTOBType_t type) {
  if(type == TCS::CLUSTER) return "Clusters";
  else if(type == TCS::TAU) return "Taus";
  else if(type == TCS::EEM) return "eEms";
  else if(type == TCS::JET) return "Jets";
  else if(type == TCS::JTAU) return "jTaus";
  else if(type == TCS::JLARGERJET) return "jLargeRJets";
  else if(type == TCS::JJET) return "jJets";
  else if(type == TCS::MET) return "MET";
  else if(type == TCS::MUON) return "Muons";
  else if(type == TCS::MUONNEXTBC) return "MuonsNextBC";
  else if(type == TCS::LATEMUON) return "LateMuons";
  else return "None";
}


TCS::inputTOBType_t
TCS::inputType(const std::string& input) {

   if ( input == "Clusters" || input == "ClusterTobArray" || input == "EmTobArray" )
      return TCS::CLUSTER;

   if ( input == "eEM" || input == "eEMTobArray" || input == "eEmTobs" )
      return TCS::EEM;

   if ( input == "jEM" || input == "jEMTobArray" || input == "jEmTobs" )
      return TCS::CLUSTER;

   if ( input == "eTau" || input == "eTauTobArray" || input == "eTauTobs" )
      return TCS::CLUSTER;

   if ( input == "jTau" || input == "jTauTobArray" || input == "jTauTobs" )
      return TCS::JTAU;

   if ( input == "Jets" || input == "JetTobArray" )
      return TCS::JET;

   if ( input == "jJet" || input == "jJetTobArray" || input == "jJetTobs" )
      return TCS::JJET;

   if ( input == "jLargeRJet" || input == "jLargeRJetTobArray" || input == "jLargeRJetTobs" )
      return TCS::JLARGERJET;

   if ( input == "gJet" || input == "gJetTobArray" || input == "gJetTobs" )
      return TCS::JET;

   if ( input == "gLargeRJet" || input == "gLargeRJetTobArray" || input == "gLargeRJetTobs" )
      return TCS::JET;

   if ( input == "Muons" || input == "MuonTobArray" || input == "MuonTobs")
      return TCS::MUON;

   if ( input == "MuonsNextBC" || input == "MuonNextBCTobArray" )
      return TCS::MUONNEXTBC;
          
   if ( input == "LateMuons" || input == "LateMuonTobArray" || input == "LateMuonTobs")
      return TCS::LATEMUON;

   if ( input == "MuonsNextBC" || input == "MuonNextBCTobArray" )
      return TCS::MUONNEXTBC;
          
   if ( input == "Taus" || input == "TauTobArray" )
      return TCS::TAU;
          
   if ( input == "MET" || input == "MetTobArray" || input == "MetTobs" )
      return TCS::MET;
          
   if ( input == "SumEt" || input == "SumEtTobArray" || input == "SumEtTobs" )
      return TCS::MET;
          

   TCS_EXCEPTION("L1TopoCommon: unknown input type " + input);
   
   return TCS::NONE;
}

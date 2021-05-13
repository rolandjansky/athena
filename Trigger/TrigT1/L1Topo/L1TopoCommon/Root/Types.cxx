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
  else if(type == TCS::JET) return "Jets";
  else if(type == TCS::JJET) return "jJets";
  else if(type == TCS::MET) return "MET";
  else if(type == TCS::MUON) return "Muons";
  else if(type == TCS::MUONNEXTBC) return "MuonsNextBC";
  else if(type == TCS::LATEMUON) return "LateMuons";
  else return "None";
}


TCS::inputTOBType_t
TCS::inputType(const std::string& input) {

   if ( input == "Clusters" || input == "EmTobArray" )
      return TCS::CLUSTER;

   if ( input == "eEM" || input == "eEMTobArray" || input == "eEMTob" )
      return TCS::CLUSTER;

   if ( input == "jEM" || input == "jEMTobArray" || input == "jEMTob" )
      return TCS::CLUSTER;

   if ( input == "eTau" || input == "eTauTobArray" || input == "eTauTob" )
      return TCS::TAU;

   if ( input == "jTau" || input == "jTauTobArray" || input == "jTauTob" )
      return TCS::TAU;

   if ( input == "Jets" || input == "JetTobArray" )
      return TCS::JET;

   if ( input == "jJet" || input == "jJetTobArray" || input == "jJetTob" )
      return TCS::JJET;

   if ( input == "jLargeRJet" || input == "jLargeRJetTobArray" || input == "jLargeRJetTob" )
      return TCS::JET;

   if ( input == "gJet" || input == "gJetTobArray" || input == "gJetTob" )
      return TCS::JET;

   if ( input == "gLargeRJet" || input == "gLargeRJetTobArray" || input == "gLargeRJetTob" )
      return TCS::JET;

   if ( input == "Muons" || input == "MuonTobArray" )
      return TCS::MUON;

   if ( input == "MuonsNextBC" || input == "MuonNextBCTobArray" )
      return TCS::MUONNEXTBC;
          
   if ( input == "LateMuons" || input == "LateMuonTobArray" )
      return TCS::LATEMUON;

   if ( input == "MuonsNextBC" || input == "MuonNextBCTobArray" )
      return TCS::MUONNEXTBC;
          
   if ( input == "Taus" || input == "TauTobArray" )
      return TCS::TAU;
          
   if ( input == "MET" || input == "MetTobArray" )
      return TCS::MET;
          
   if ( input == "SumEt" || input == "SumEtTobArray" || input == "SumEtTob" )
      return TCS::MET;
          

   TCS_EXCEPTION("L1TopoCommon: unknown input type " + input);
   
   return TCS::NONE;
}

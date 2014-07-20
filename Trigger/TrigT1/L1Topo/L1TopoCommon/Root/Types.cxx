/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoCommon/Types.h"
#include "L1TopoCommon/Exception.h"

// Configuration
std::string
TCS::inputTypeAsString(TCS::inputTOBType_t type) {
  if(type == TCS::CLUSTER) return "Clusters";
  else if(type == TCS::TAU) return "Taus";
  else if(type == TCS::JET) return "Jets";
  else if(type == TCS::MET) return "MET";
  else if(type == TCS::MUON) return "Muons";
  else return "None";
}


TCS::inputTOBType_t
TCS::inputType(const std::string& input) {

   if ( input == "Clusters" || input == "ClusterIn" )
      return TCS::CLUSTER;

   if ( input == "Jets" || input == "JetIn" )
      return TCS::JET;

   if ( input == "Muons" || input == "MuonIn" )
      return TCS::MUON;
          
   if ( input == "Taus" || input == "TauIn" )
      return TCS::TAU;
          
   if ( input == "MET" || input == "MetIn" )
      return TCS::MET;
          

   TCS_EXCEPTION("L1TopoCommon: unknown input type " + input);
   
   return TCS::NONE;
}

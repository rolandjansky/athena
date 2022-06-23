/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoCommon/Types.h"
#include "L1TopoCommon/Exception.h"

// Configuration
std::string
TCS::inputTypeAsString(TCS::inputTOBType_t type) {
  if(type == TCS::CLUSTER) return "Clusters";
  else if(type == TCS::TAU) return "Taus";
  else if(type == TCS::EEM) return "eEms";
  else if(type == TCS::JEM) return "jEms";
  else if(type == TCS::JET) return "Jets";
  else if(type == TCS::ETAU) return "eTaus";
  else if(type == TCS::JTAU) return "jTaus";
  else if(type == TCS::CTAU) return "cTaus";
  else if(type == TCS::JLJET) return "jLJets";
  else if(type == TCS::GLJET) return "gLJets";
  else if(type == TCS::JJET) return "jJets";
  else if(type == TCS::GJET) return "gJets";
  else if(type == TCS::MET) return "MET";
  else if(type == TCS::MUON) return "Muons";
  else if(type == TCS::MUONNEXTBC) return "MuonsNextBC";
  else if(type == TCS::LATEMUON) return "LateMuons";
  else if(type == TCS::JXE ) return "jXE";
  else if(type == TCS::JXEC ) return "jXEC";
  else if(type == TCS::JXEPERF ) return "jXEPerf";
  else if(type == TCS::JTE ) return "jTE";
  else if(type == TCS::JTEC ) return "jTEC";
  else if(type == TCS::JTEFWD ) return "jTEFWD";
  else if(type == TCS::JTEFWDA ) return "jTEFWDA";
  else if(type == TCS::JTEFWDC ) return "jTEFWDC";
  else if(type == TCS::GXEJWOJ ) return "gXEJWOJ";
  else if(type == TCS::GXENC ) return "gXENC";
  else if(type == TCS::GXERHO ) return "gXERHO";
  else if(type == TCS::GMHT ) return "gMHT";
  else if(type == TCS::GTE ) return "gTE";
  else return "None";
}


TCS::inputTOBType_t
TCS::inputType(const std::string& input) {

   if ( input == "Clusters" || input == "ClusterTobArray" || input == "EmTobArray" )
      return TCS::CLUSTER;

   if ( input == "eEM" || input == "eEMTobArray" || input == "eEmTobs" )
      return TCS::EEM;

   if ( input == "jEM" || input == "jEMTobArray" || input == "jEmTobs" )
      return TCS::JEM;

   if ( input == "eTau" || input == "eTauTobArray" || input == "eTauTobs" || input == "eTAU" )
      return TCS::ETAU;

   if ( input == "jTau" || input == "jTauTobArray" || input == "jTauTobs" || input == "jTAU" )
      return TCS::JTAU;

   if ( input == "cTau" || input == "cTauTobArray" || input == "cTauTobs" || input == "cTAU" )
      return TCS::CTAU;

   if ( input == "Jets" || input == "JetTobArray" )
      return TCS::JET;

   if ( input == "jJ" || input == "jJetTobArray" || input == "jJetTobs" || input == "jJets" )
      return TCS::JJET;

   if ( input == "gJ" || input == "gJetTobArray" || input == "gJetTobs" || input == "gJets" )
      return TCS::GJET;

   if ( input == "jLJ" || input == "jLJetTobArray" || input == "jLJetTobs" || input == "jLJets" )
      return TCS::JLJET;

   if ( input == "gLJ" || input == "gLJetTobArray" || input == "gLJetTobs" || input == "gLJets" )
      return TCS::GLJET;

   if ( input == "Muons" || input == "MuonTobArray" || input == "MuonTobs" || input == "MU" )
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

   if ( input == "jXE" || input == "jXETobs" )
      return TCS::JXE;

   if ( input == "jXEC")
      return TCS::JXEC;

   if ( input == "jXEPerf")
      return TCS::JXEPERF;

   if ( input == "jTE")
      return TCS::JTE;

   if ( input == "jTEC")
      return TCS::JTEC;

   if ( input == "jTEFWD")
      return TCS::JTEFWD;

   if ( input == "jTEFWDA")
      return TCS::JTEFWDA;

   if ( input == "jTEFWDC")
      return TCS::JTEFWDC;
 
   if ( input == "gXEJWOJ")
      return TCS::GXEJWOJ;

   if ( input == "gXENC")
      return TCS::GXENC;

   if ( input == "gXERHO")
      return TCS::GXERHO;
 
   if ( input == "gMHT")
      return TCS::GMHT;
 
   if ( input == "gTE")
      return TCS::GTE;
          
   TCS_EXCEPTION("L1TopoCommon: unknown input type " + input);
   
   return TCS::NONE;
}

TCS::outputTopoType_t
TCS::outputType(const std::string& output) {
  if (output == "LegacyTopo0")
    {return TCS::LEGACYTOPO0;}
  else if (output == "LegacyTopo1")
    {return TCS::LEGACYTOPO1;}
  else if (output == "Topo2El")
    {return TCS::TOPO2EL;}
  else if (output == "Topo3El")
    {return TCS::TOPO3EL;}
  else if (output == "Topo1Opt0")
    {return TCS::TOPO1OPT0;}
  else if (output == "Topo1Opt1")
    {return TCS::TOPO1OPT1;}
  else if (output == "Topo1Opt2")
    {return TCS::TOPO1OPT2;}
  else if (output == "Topo1Opt3")
    {return TCS::TOPO1OPT3;}
  else
    {
      TCS_EXCEPTION("L1TopoCommon: unknown output type " + output);
      return TCS::UNDEF;
    }
}

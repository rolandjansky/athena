/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJet/JetContainerInfo.h"
#include <string>
#include <map>

namespace xAOD {
  
  namespace JetAlgorithmType {

    static const std::string c_unknownAlg = "UnknownAlg";

    /// Map associating string names to enumeration values
    ///
    /// Note that multiple string names may be associated to the same
    /// enum value.
    ///
    static const std::map<std::string, ID> nameToIDmap = {
      // Names in the ATLAS convention
      { "Kt",            JetAlgorithmType::kt_algorithm },
      { "CamKt",         JetAlgorithmType::cambridge_algorithm },
      { "AntiKt",        JetAlgorithmType::antikt_algorithm },
      { "GenKt",         JetAlgorithmType::genkt_algorithm },
      { "CamKtPassive",  JetAlgorithmType::cambridge_for_passive_algorithm },
      { "GenKtPassive",  JetAlgorithmType::genkt_for_passive_algorithm },
      { "EEKt",          JetAlgorithmType::ee_kt_algorithm },
      { "EEGenKt",       JetAlgorithmType::ee_genkt_algorithm },
      // Names as FastJet understands them (in lower case)
      { "kt",            JetAlgorithmType::kt_algorithm },
      { "cambridge",     JetAlgorithmType::cambridge_algorithm },
      { "antikt",        JetAlgorithmType::antikt_algorithm },
      { "genkt",         JetAlgorithmType::genkt_algorithm },
      { "cambridge_for_passive", JetAlgorithmType::cambridge_for_passive_algorithm },
      { "genkt_for_passive",     JetAlgorithmType::genkt_for_passive_algorithm },
      { "ee_kt",         JetAlgorithmType::ee_kt_algorithm },
      { "ee_genkt",      JetAlgorithmType::ee_genkt_algorithm },
      // Technical entries
      { "FastJetPlugin", JetAlgorithmType::plugin_algorithm },
      { c_unknownAlg,    JetAlgorithmType::undefined_jet_algorithm }
    };

    /// Map associating enum values to string names
    ///
    /// Note that this is *not* an inverse of the previous map! Here
    /// the association from enum values to strings is 1-to-1. While
    /// the previous map associates multiple strings to the same enum
    /// value.
    ///
    /// Also note that the association is to the "ATLAS names", and
    /// not to the "FastJet names".
    ///
    static const std::map<ID, std::string> idToNamemap = {
      { JetAlgorithmType::kt_algorithm,                    "Kt" },
      { JetAlgorithmType::cambridge_algorithm,             "CamKt" },
      { JetAlgorithmType::antikt_algorithm,                "AntiKt" },
      { JetAlgorithmType::genkt_algorithm,                 "GenKt" },
      { JetAlgorithmType::cambridge_for_passive_algorithm, "CamKtPassive" },
      { JetAlgorithmType::genkt_for_passive_algorithm,     "GenKtPassive" },
      { JetAlgorithmType::ee_kt_algorithm,                 "EEKt" },
      { JetAlgorithmType::ee_genkt_algorithm,              "EEGenKt" },
      { JetAlgorithmType::plugin_algorithm,                "FastJetPlugin" },
      { JetAlgorithmType::undefined_jet_algorithm,         c_unknownAlg }
    };

    const std::string& algName( ID id ) {

      auto itr = idToNamemap.find( id );
      if( itr != idToNamemap.end() ) {
        return itr->second;
      }
      return c_unknownAlg;
    }

    ID algId( const std::string& n ) {

      auto itr = nameToIDmap.find( n );
      if( itr != nameToIDmap.end() ) {
	return itr->second;
      }
      return undefined_jet_algorithm;
    }

  }// JetAlgorithmType namespace

  namespace JetInput {

    static const std::string c_unCategorized = "Uncategorized";

    /// Map associating string names to enumeration values
    ///
    /// Note that multiple string names may be associated to the same
    /// enum value. Even though currently they aren't.
    ///
    static const std::map<std::string, Type> nameToTypemap = {
      { "LCTopo",                LCTopo },
      { "EMTopo",                EMTopo },
      { "LCTopoOrigin",          LCTopoOrigin },
      { "EMTopoOrigin",          EMTopoOrigin },
      { "TopoTower",             TopoTower },
      { "Tower",                 Tower },
      { "Jet",                   Jet },
      { "Truth",                 Truth },
      { "TruthWZ",               TruthWZ },
      { "TruthDressedWZ",        TruthDressedWZ },
      { "TruthCharged",          TruthCharged },
      { "Track",                 Track },
      { "PFlow",                 PFlow },
      { "LCPFlow",               LCPFlow },
      { "EMPFlow",               EMPFlow },
      { "EMCPFlow",              EMCPFlow },
      { "TrackCaloCluster",      TrackCaloCluster },
      { "EMTopoOriginSK",        EMTopoOriginSK },
      { "EMTopoOriginCS",        EMTopoOriginCS },
      { "EMTopoOriginCSSK",      EMTopoOriginCSSK },
      { "EMTopoOriginVorSK",     EMTopoOriginVorSK },
      { "EMTopoOriginTime",      EMTopoOriginTime },
      { "EMTopoOriginSKTime",    EMTopoOriginSKTime },
      { "EMTopoOriginCSSKTime",  EMTopoOriginCSSKTime },
      { "EMTopoOriginVorSKTime", EMTopoOriginVorSKTime },
      { "LCTopoOriginSK",        LCTopoOriginSK },
      { "LCTopoOriginCS",        LCTopoOriginCS },
      { "LCTopoOriginCSSK",      LCTopoOriginCSSK },
      { "LCTopoOriginVorSK",     LCTopoOriginVorSK },
      { "EMPFlowSK",             EMPFlowSK },
      { "EMPFlowCS",             EMPFlowCS },
      { "EMPFlowCSSK",           EMPFlowCSSK },
      { "EMPFlowVorSK",          EMPFlowVorSK },
      { "EMPFlowTime",           EMPFlowTime },
      { "EMPFlowSKTime",         EMPFlowSKTime },
      { "EMPFlowCSSKTime",       EMPFlowCSSKTime },
      { "EMPFlowVorSKTime",      EMPFlowVorSKTime },
      { "HI",                    HI },
      { "HIClusters", 		 HIClusters },
      { c_unCategorized,         Uncategorized }
    };

    /// Map associating enum values to string names
    ///
    /// Note that this is *not* necessarily an inverse of the previous
    /// map! Here the association from enum values to strings is 1-to-1.
    /// While the previous map may associate multiple strings to the
    /// same enum value.
    ///
    static const std::map<Type, std::string> typeToNamemap {
      { LCTopo,                "LCTopo" },
      { EMTopo,                "EMTopo" },
      { LCTopoOrigin,          "LCTopoOrigin" },
      { EMTopoOrigin,          "EMTopoOrigin" },
      { TopoTower,             "TopoTower" },
      { Tower,                 "Tower" },
      { Jet,                   "Jet" },
      { Truth,                 "Truth" },
      { TruthWZ,               "TruthWZ" },
      { TruthDressedWZ,        "TruthDressedWZ" },
      { TruthCharged,          "TruthCharged" },
      { Track,                 "Track" },
      { PFlow,                 "PFlow" },
      { LCPFlow,               "LCPFlow" },
      { EMPFlow,               "EMPFlow" },
      { EMCPFlow,              "EMCPFlow" },
      { TrackCaloCluster,      "TrackCaloCluster" },
      { EMTopoOriginSK,        "EMTopoOriginSK" },
      { EMTopoOriginCS,        "EMTopoOriginCS" },
      { EMTopoOriginCSSK,      "EMTopoOriginCSSK" },
      { EMTopoOriginVorSK,     "EMTopoOriginVorSK" },
      { EMTopoOriginTime,      "EMTopoOriginTime" },
      { EMTopoOriginSKTime,    "EMTopoOriginSKTime" },
      { EMTopoOriginCSSKTime,  "EMTopoOriginCSSKTime" },
      { EMTopoOriginVorSKTime, "EMTopoOriginVorSKTime" },
      { LCTopoOriginSK,        "LCTopoOriginSK" },
      { LCTopoOriginCS,        "LCTopoOriginCS" },
      { LCTopoOriginCSSK,      "LCTopoOriginCSSK" },
      { LCTopoOriginVorSK,     "LCTopoOriginVorSK" },
      { EMPFlowSK,             "EMPFlowSK" },
      { EMPFlowCS,             "EMPFlowCS" },
      { EMPFlowCSSK,           "EMPFlowCSSK" },
      { EMPFlowVorSK,          "EMPFlowVorSK" },
      { EMPFlowTime,           "EMPFlowTime" },
      { EMPFlowSKTime,         "EMPFlowSKTime" },
      { EMPFlowCSSKTime,       "EMPFlowCSSKTime" },
      { EMPFlowVorSKTime,      "EMPFlowVorSKTime" },
      { HI,                    "HI" },
      { HIClusters, 	       "HIClusters" },
      { Uncategorized,         c_unCategorized }
    };

    bool isValidConstitType( Type t ) {

      auto itr = typeToNamemap.find( t );
      return ( ( itr != typeToNamemap.end() ) && ( t != Uncategorized ) );
    }

    const std::string& typeName( Type id ) {

      auto itr = typeToNamemap.find( id );
      if( itr != typeToNamemap.end() ) {
        return itr->second;
      }
      return c_unCategorized;
    }

    Type inputType( const std::string& n ) {

      auto itr = nameToTypemap.find( n );
      if( itr != nameToTypemap.end() ) {
	return itr->second;
      }
      return Uncategorized;
    }

  } // namespace JetInput

  namespace JetTransform {

    std::string name(Type type) {
      switch(type) {
        case UnknownTransform: return "UnknownTransform";
        case NoTransform:      return "NoTransform";
        case Trim:             return "Trim";
        case Prune:            return "Prune";
        case MassDrop:         return "MassDrop";
        case KtRecluster:      return "KtRecluster";
        case CamKtRecluster:   return "CamKtRecluster";
        case AntiKtRecluster:  return "AntiKtRecluster";
      }
      return "UnknownTransform";
    }

    Type type(const std::string& name) {
      if ( name == "UnknownTransform" ) return NoTransform;
      if ( name == "NoTransform" )      return NoTransform;
      if ( name == "Trim" )             return Trim;
      if ( name == "Prune" )            return Prune;
      if ( name == "MassDrop" )         return MassDrop;
      if ( name == "KtRecluster" )      return KtRecluster;
      if ( name == "CamKtRecluster" )   return CamKtRecluster;
      if ( name == "AntiKtRecluster" )  return AntiKtRecluster;
      return UnknownTransform;
    }

  }  // end namespace JetTransform
  
}

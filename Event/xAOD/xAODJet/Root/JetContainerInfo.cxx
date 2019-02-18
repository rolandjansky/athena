/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODJet/JetContainerInfo.h"
#include "CxxUtils/checker_macros.h"
#include <string>
#include <map>

namespace xAOD {
  
  namespace JetAlgorithmType {
    std::map<std::string, ID> nameToIDmap;
    std::map<ID, std::string> idToNamemap;

    const std::string c_unknonwAlg = "UnknwonAlg";
    
    void initMap  ATLAS_NOT_THREAD_SAFE  (){
      static bool inited = false;
      if(inited) return;
      // name in the atlas convention
      nameToIDmap["Kt"] = JetAlgorithmType::kt_algorithm;
      nameToIDmap["CamKt"] = JetAlgorithmType::cambridge_algorithm;
      nameToIDmap["AntiKt"] = JetAlgorithmType::antikt_algorithm;
      nameToIDmap["GenKt"] = JetAlgorithmType::genkt_algorithm;
      nameToIDmap["CamKtPassive"] = JetAlgorithmType::cambridge_for_passive_algorithm;
      nameToIDmap["GenKtPassive"] = JetAlgorithmType::genkt_for_passive_algorithm;
      nameToIDmap["EEKt"] =        JetAlgorithmType::ee_kt_algorithm;
      nameToIDmap["EEGenKt"] = JetAlgorithmType::ee_genkt_algorithm;      
      
      // name as in fastjet (lower case)
      nameToIDmap["kt"] = JetAlgorithmType::kt_algorithm;
      nameToIDmap["cambridge"] = JetAlgorithmType::cambridge_algorithm;
      nameToIDmap["antikt"] = JetAlgorithmType::antikt_algorithm;
      nameToIDmap["genkt"] = JetAlgorithmType::genkt_algorithm;
      nameToIDmap["cambridge_for_passive"] = JetAlgorithmType::cambridge_for_passive_algorithm;
      nameToIDmap["genkt_for_passive"] = JetAlgorithmType::genkt_for_passive_algorithm;
      nameToIDmap["ee_kt"] =        JetAlgorithmType::ee_kt_algorithm;
      nameToIDmap["ee_genkt"] = JetAlgorithmType::ee_genkt_algorithm;      

      nameToIDmap["FastJetPlugin"] = JetAlgorithmType::plugin_algorithm;      
      nameToIDmap[c_unknonwAlg] = JetAlgorithmType::undefined_jet_algorithm;      

      std::map<std::string, ID>::iterator it = nameToIDmap.begin();
      std::map<std::string, ID>::iterator itE = nameToIDmap.end();
      for( ; it!=itE; ++it) idToNamemap[ it->second ] = it->first;      
    }

  

  
  const std::string & algName  ATLAS_NOT_THREAD_SAFE  (ID id){
    initMap();
    if( idToNamemap.find( id ) != idToNamemap.end() ) return idToNamemap[id];
    return c_unknonwAlg;
      
 
  }
  ID algId  ATLAS_NOT_THREAD_SAFE  (const std::string & n){
    initMap();
    if( nameToIDmap.find( n ) != nameToIDmap.end() ) return nameToIDmap[n];
    return undefined_jet_algorithm;
  }

  }// JetAlgorithmType namespace


  namespace JetInput {
    std::map<std::string, Type> nameToTypemap;
    std::map<Type, std::string> typeToNamemap;

    const std::string c_unCategorized = "Uncategorized";
    
    void initMap  ATLAS_NOT_THREAD_SAFE  (){
      static bool inited = false;
      if(inited) return;
      // name in the atlas convention
      nameToTypemap["LCTopo"] =                LCTopo;
      nameToTypemap["EMTopo"] =                EMTopo;
      nameToTypemap["LCTopoOrigin"] =          LCTopoOrigin;
      nameToTypemap["EMTopoOrigin"] =          EMTopoOrigin;
      nameToTypemap["TopoTower"] =             TopoTower;
      nameToTypemap["Tower"] =                 Tower;
      nameToTypemap["Truth"] =                 Truth;
      nameToTypemap["TruthWZ"] =               TruthWZ;
      nameToTypemap["TruthDressedWZ"] =        TruthDressedWZ;
      nameToTypemap["TruthCharged"] =          TruthCharged;
      nameToTypemap["Track"] =                 Track;
      nameToTypemap["PFlow"] =                 PFlow;
      nameToTypemap["LCPFlow"] =               LCPFlow;
      nameToTypemap["EMPFlow"] =               EMPFlow;
      nameToTypemap["EMCPFlow"] =              EMCPFlow;
      nameToTypemap["TrackCaloCluster"] =      TrackCaloCluster;
      nameToTypemap["EMTopoOriginSK"] =        EMTopoOriginSK;
      nameToTypemap["EMTopoOriginCS"] =        EMTopoOriginCS;
      nameToTypemap["EMTopoOriginCSSK"] =      EMTopoOriginCSSK;
      nameToTypemap["EMTopoOriginVorSK"] =     EMTopoOriginVorSK;
      nameToTypemap["EMTopoOriginTime"] =      EMTopoOriginTime;
      nameToTypemap["EMTopoOriginSKTime"] =    EMTopoOriginSKTime;
      nameToTypemap["EMTopoOriginCSSKTime"] =  EMTopoOriginCSSKTime;
      nameToTypemap["EMTopoOriginVorSKTime"] = EMTopoOriginVorSKTime;
      nameToTypemap["LCTopoOriginSK"] =        LCTopoOriginSK;
      nameToTypemap["LCTopoOriginCS"] =        LCTopoOriginCS;
      nameToTypemap["LCTopoOriginCSSK"] =      LCTopoOriginCSSK;
      nameToTypemap["LCTopoOriginVorSK"] =     LCTopoOriginVorSK;
      nameToTypemap["EMPFlowSK"] =             EMPFlowSK;
      nameToTypemap["EMPFlowCS"] =             EMPFlowCS;
      nameToTypemap["EMPFlowCSSK"] =           EMPFlowCSSK;
      nameToTypemap["EMPFlowVorSK"] =          EMPFlowVorSK;
      nameToTypemap["EMPFlowTime"] =           EMPFlowTime;
      nameToTypemap["EMPFlowSKTime"] =         EMPFlowSKTime;
      nameToTypemap["EMPFlowCSSKTime"] =       EMPFlowCSSKTime;
      nameToTypemap["EMPFlowVorSKTime"] =      EMPFlowVorSKTime;
      nameToTypemap["HI"] =                    HI;
      nameToTypemap[c_unCategorized] =         Uncategorized;

      std::map<std::string, Type>::iterator it = nameToTypemap.begin();
      std::map<std::string, Type>::iterator itE = nameToTypemap.end();
      for( ; it!=itE; ++it) typeToNamemap[ it->second ] = it->first;      
    }

  bool isValidConstitType(Type t) { 
    const static std::unordered_set<Type> validJetContitTypes = { LCTopo, EMTopo,
                                                                  LCTopoOrigin, EMTopoOrigin, EMTopoOriginTime,
                                                                  LCTopoOriginSK, EMTopoOriginSK, EMTopoOriginSKTime,
                                                                  LCTopoOriginCS, EMTopoOriginCS,
                                                                  LCTopoOriginVor, EMTopoOriginVor,
                                                                  LCTopoOriginCSSK, EMTopoOriginCSSK, EMTopoOriginCSSKTime,
                                                                  LCTopoOriginVorSK, EMTopoOriginVorSK, EMTopoOriginVorSKTime,
                                                                  EMPFlowSK, EMPFlowSKTime,
                                                                  EMPFlowCS, EMPFlowCSSK, EMPFlowCSSKTime,
                                                                  EMPFlowVor, EMPFlowVorSK, EMPFlowVorSKTime
                                                                };
    return validJetContitTypes.count(t)==1;
  } 

  const std::string & typeName  ATLAS_NOT_THREAD_SAFE  (Type id){
    initMap();
    if( typeToNamemap.find( id ) != typeToNamemap.end() ) return typeToNamemap[id];
    return c_unCategorized ;     
  }

  Type inputType  ATLAS_NOT_THREAD_SAFE  (const std::string & n){
    initMap();
    if( nameToTypemap.find( n ) != nameToTypemap.end() ) return nameToTypemap[n];
    return Uncategorized;
  }


  }

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

    Type type(std::string name) {
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

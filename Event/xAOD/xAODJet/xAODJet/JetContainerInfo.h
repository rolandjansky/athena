// this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODJET_JETALGORITHMTYPE_H
#define XAODJET_JETALGORITHMTYPE_H

/// ///////////////////////////////
/// This header defines various enums which caracterize a JetContainer
/// This includes definition for the algorithm types and input types.
///  
/// 
/// Also provided are utility function to translate these enum to/from strings.
///
/// ///////////////////////////////

#include <string>
#include <unordered_set>

namespace xAOD {


  namespace JetAlgorithmType{
    /// ////////////////////////////////////////
    /// JetAlgorithmType::ID defines most common physics jet finding algorithms used in Atlas.
    /// It follows fastjet's enum.
    enum ID {
      // fastjet enums
      kt_algorithm = 0, 
      cambridge_algorithm = 1, 
      antikt_algorithm = 2, 
      genkt_algorithm = 3,
      cambridge_for_passive_algorithm = 11, 
      genkt_for_passive_algorithm = 13, 
      ee_kt_algorithm = 50, 
      ee_genkt_algorithm = 53,
      plugin_algorithm = 99, 
      undefined_jet_algorithm = 999 
      // nothing more for now...
    };    
    
    /// Converts a JetAlgorithmType::ID into a string
    const std::string& algName(ID id);
    
    /// Converts a string into a JetAlgorithmType::ID
    ID algId(const std::string & n);
    
  }

  namespace JetInput {

    enum Type {
      LCTopo,
      EMTopo,
      TopoTower,
      Tower,
      Truth,      
      TruthWZ,
      Track,
      PFlow,      
      LCPFlow,      // LC PFlow
      EMPFlow,      // EM Pflow at EM scale
      EMCPFlow,     // EM Pflow calibrated to LC scale
      Jet,
      LCTopoOrigin,
      EMTopoOrigin,
      TrackCaloCluster,
      TruthDressedWZ, // Truth jets without prompt e/mu (or dressed photons) or prompt gammas
      EMTopoOriginSK,
      EMTopoOriginCS,
      EMTopoOriginVor,
      EMTopoOriginCSSK,
      EMTopoOriginVorSK,
      LCTopoOriginSK,
      LCTopoOriginCS,
      LCTopoOriginVor,
      LCTopoOriginCSSK,
      LCTopoOriginVorSK,
      EMPFlowSK,
      EMPFlowCS,
      EMPFlowVor,
      EMPFlowCSSK,
      EMPFlowVorSK,
      TruthCharged, // Truth jets with only charged particles
      EMTopoOriginTime,
      EMTopoOriginSKTime,
      EMTopoOriginCSSKTime,
      EMTopoOriginVorSKTime,
      EMPFlowTime,
      EMPFlowSKTime,
      EMPFlowCSSKTime,
      EMPFlowVorSKTime,
      HI,
      HIClusters,
      Other = 100,
      Uncategorized= 1000
    };

    bool isValidConstitType(Type t);
    
    const std::string& typeName(Type t);

    Type inputType(const std::string &n);
  }

  namespace JetTransform {
    /// Enum for types of jet transformations.
    enum Type {
      UnknownTransform = 0,
      NoTransform      = 1,
      Trim             = 2,
      Prune            = 3,
      MassDrop         = 4,
      KtRecluster      = 5,
      CamKtRecluster   = 6,
      AntiKtRecluster  = 7
    };
    /// Convert jet transformation enum to string.
    std::string name(Type t);
    /// Convert string to jet transformation enum.
    Type type(const std::string& name);
  }

}
#endif

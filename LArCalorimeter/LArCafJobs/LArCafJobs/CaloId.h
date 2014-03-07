/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class LArCaloId
   @brief Liquid Argon class for standalone storage of cell information
*/

#ifndef LArCaloId_H
#define LArCaloId_H

#include "TString.h"

class TH2;
class TH2D;
class TProfile2D;

namespace LArSamples {

  enum CaloId { 
    FCAL_C = -5, HEC_C = -4, EMEC_INNER_C = -3, EMEC_OUTER_C = -2, EMB_C = -1,
    UNKNOWN_CALO = 0,
    EMB_A = 1, EMEC_OUTER_A = 2, EMEC_INNER_A = 3, HEC_A = 4, FCAL_A = 5,
    EMEC_C = -10, EMEC_A = 10, EMB = 101, EMEC = 102, EMEC_OUTER = 103, EMEC_INNER = 104, 
    HEC = 105, FCAL = 106, EM = 107, PRECISION_EM = 108
  };
       
  enum PartitionId { 
    EMB_A_PARTITION = 1, EMB_C_PARTITION = 2, EMEC_A_PARTITION = 3, EMEC_C_PARTITION = 4, 
    HEC_A_PARTITION = 5, HEC_C_PARTITION = 6, FCAL_A_PARTITION = 7, FCAL_C_PARTITION = 8, UNKNOWN_PARTITION = 0 
  };

  
  class Id {
    
    public:
          
      typedef LArSamples::CaloId CaloId;
      typedef LArSamples::PartitionId PartitionId;

      static CaloId caloId(TString str);
      static TString str(CaloId id);
      static CaloId summary(CaloId id);

      static PartitionId partitionId(TString str);
      static TString str(PartitionId id);
      static CaloId summary(PartitionId id);
            
      static TString summary_str(CaloId id) { return str(summary(id)); }
      static PartitionId partition(CaloId id);
      static bool matchCalo(CaloId id, CaloId idSpec);
      
      static short sign(CaloId id);
      static CaloId mirror(CaloId id);
  };
}

#endif

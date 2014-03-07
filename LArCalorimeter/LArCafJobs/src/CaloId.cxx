/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/CaloId.h"

#include "TH2.h"
#include "TProfile2D.h"
#include "TMath.h"
#include <vector>

using namespace LArSamples;


TString Id::str(CaloId id)
{
  switch (id) {
    case FCAL_C : return "FCAL_C";
    case HEC_C :  return "HEC_C";
    case EMEC_OUTER_C : return "EMEC_OUTER_C";
    case EMEC_INNER_C : return "EMEC_INNER_C";
    case EMEC_C : return "EMEC_C";
    case EMB_C :  return "EMB_C";
    case UNKNOWN_CALO : return "UNKNOWN_CALO";
    case EMB_A :  return "EMB_A";
    case EMEC_INNER_A : return "EMEC_INNER_A";
    case EMEC_OUTER_A : return "EMEC_OUTER_A";
    case EMEC_A : return "EMEC_A";
    case HEC_A :  return "HEC_A";
    case FCAL_A : return "FCAL_A";
    
    case EMB  : return "EMB";
    case EMEC : return "EMEC";
    case EMEC_OUTER : return "EMEC_OUTER";
    case EMEC_INNER : return "EMEC_INNER";
    case HEC  : return "HEC";
    case FCAL : return "FCAL";
    case EM : return "EM";
    case PRECISION_EM : return "PRECISION_EM";
  }

  return "UNKNOWN_CALO";
}

CaloId Id::caloId(TString str)
{
  if (str == "FCAL_C") return FCAL_C;
  if (str == "HEC_C")  return HEC_C;
  if (str == "EMEC_OUTER_C") return EMEC_OUTER_C;
  if (str == "EMEC_INNER_C") return EMEC_INNER_C;
  if (str == "EMEC_C") return EMEC_C;
  if (str == "EMB_C")  return EMB_C;
  if (str == "UNKNOWN_CALO") return UNKNOWN_CALO;
  if (str == "EMB_A")  return EMB_A;
  if (str == "EMEC_INNER_A") return EMEC_INNER_A;
  if (str == "EMEC_OUTER_A") return EMEC_OUTER_A;
  if (str == "EMEC_A") return EMEC_A;
  if (str == "HEC_A")  return HEC_A;
  if (str == "FCAL_A") return FCAL_A;
   
  if (str == "EMB")  return EMB;
  if (str == "EMEC") return EMEC;
  if (str == "EMEC_OUTER") return EMEC_OUTER;
  if (str == "EMEC_INNER") return EMEC_INNER;
  if (str == "HEC")  return HEC;
  if (str == "FCAL") return FCAL;
  if (str == "EM") return EM;
  if (str == "PRECISION_EM") return PRECISION_EM;

  return UNKNOWN_CALO;
}


CaloId Id::summary(CaloId id)
{
  switch (id) {
    case FCAL_C : return FCAL;
    case HEC_C :  return HEC;
    case EMEC_OUTER_C : return EMEC;
    case EMEC_INNER_C : return EMEC;
    case EMEC_C : return EMEC;
    case EMB_C :  return EMB;
    case UNKNOWN_CALO : return UNKNOWN_CALO;
    case EMB_A :  return EMB;
    case EMEC_OUTER_A : return EMEC;
    case EMEC_INNER_A : return EMEC;
    case EMEC_A : return EMEC;
    case HEC_A :  return HEC;
    case FCAL_A : return FCAL;
    
    case EMB  : return EMB;
    case EMEC : return EMEC;
    case EMEC_OUTER : return EMEC;
    case EMEC_INNER : return EMEC;
    case HEC  : return HEC;
    case FCAL : return FCAL;
    case EM : return EM;
    case PRECISION_EM : return PRECISION_EM;
  }

  return UNKNOWN_CALO;
}


TString Id::str(PartitionId id)
{
  switch (id) {
    case EMB_C_PARTITION :  return "EMB_C_PARTITION";
    case EMB_A_PARTITION :  return "EMB_A_PARTITION";
    case EMEC_C_PARTITION : return "EMEC_C_PARTITION";
    case EMEC_A_PARTITION : return "EMEC_A_PARTITION";
    case UNKNOWN_PARTITION: return "UNKNOWN_PARTITION";
    case HEC_C_PARTITION :  return "HEC_C_PARTITION";
    case HEC_A_PARTITION :  return "HEC_A_PARTITION";
    case FCAL_C_PARTITION : return "FCAL_C_PARTITION";
    case FCAL_A_PARTITION : return "FCAL_A_PARTITION";
  }

  return "UNKNOWN_PARTITION";
}


PartitionId Id::partitionId(TString str)
{
  if (str == "FCAL_C_PARTITION") return FCAL_C_PARTITION;
  if (str == "HEC_C_PARTITION")  return HEC_C_PARTITION;
  if (str == "EMEC_C_PARTITION") return EMEC_C_PARTITION;
  if (str == "EMB_C_PARTITION")  return EMB_C_PARTITION;
  if (str == "UNKNOWN_PARTITION") return UNKNOWN_PARTITION;
  if (str == "EMB_A_PARTITION")  return EMB_A_PARTITION;
  if (str == "EMEC_A_PARTITION") return EMEC_A_PARTITION;
  if (str == "HEC_A_PARTITION")  return HEC_A_PARTITION;
  if (str == "FCAL_A_PARTITION") return FCAL_A_PARTITION;

  return UNKNOWN_PARTITION;
}


CaloId Id::summary(PartitionId id)
{
  switch (id) {
    case FCAL_C_PARTITION : return FCAL;
    case HEC_C_PARTITION :  return HEC;
    case EMEC_C_PARTITION : return EMEC;
    case EMB_C_PARTITION :  return EMB;
    case UNKNOWN_PARTITION : return UNKNOWN_CALO;
    case EMB_A_PARTITION :  return EMB;
    case EMEC_A_PARTITION : return EMEC;
    case HEC_A_PARTITION :  return HEC;
    case FCAL_A_PARTITION : return FCAL;
  }

  return UNKNOWN_CALO;
}


PartitionId Id::partition(CaloId id)
{
  switch (id) {
    case FCAL_C : return FCAL_C_PARTITION;
    case HEC_C :  return HEC_C_PARTITION;
    case EMEC_OUTER_C : return EMEC_C_PARTITION;
    case EMEC_INNER_C : return EMEC_C_PARTITION;
    case EMEC_C : return EMEC_C_PARTITION;
    case EMB_C :  return EMB_C_PARTITION;
    case UNKNOWN_CALO : return UNKNOWN_PARTITION;
    case EMB_A :  return EMB_A_PARTITION;
    case EMEC_INNER_A : return EMEC_A_PARTITION;
    case EMEC_OUTER_A : return EMEC_A_PARTITION;
    case EMEC_A : return EMEC_A_PARTITION;
    case HEC_A :  return HEC_A_PARTITION;
    case FCAL_A : return FCAL_A_PARTITION;
    
    case EMB  : return UNKNOWN_PARTITION;
    case EMEC : return UNKNOWN_PARTITION;
    case EMEC_OUTER : return UNKNOWN_PARTITION;
    case EMEC_INNER : return UNKNOWN_PARTITION;
    case HEC  : return UNKNOWN_PARTITION;
    case FCAL : return UNKNOWN_PARTITION;
    case EM : return UNKNOWN_PARTITION;
    case PRECISION_EM : return UNKNOWN_PARTITION;
  }

  return UNKNOWN_PARTITION;
}


bool Id::matchCalo(CaloId id, CaloId idSpec)
{
  if (id == idSpec) return true;
  if (idSpec == EMB && (id == EMB_A || id == EMB_C)) return true;
  if (idSpec == EMEC && (matchCalo(id, EMEC_A) || matchCalo(id, EMEC_C))) return true;
  if (idSpec == EMEC_A && (id == EMEC_INNER_A || id == EMEC_OUTER_A)) return true;
  if (idSpec == EMEC_C && (id == EMEC_INNER_C || id == EMEC_OUTER_C)) return true;
  if (idSpec == EMEC_OUTER && (id == EMEC_OUTER_A || id == EMEC_OUTER_C)) return true;
  if (idSpec == EMEC_INNER && (id == EMEC_INNER_A || id == EMEC_INNER_C)) return true;
  if (idSpec == HEC && (id == HEC_A || id == HEC_C)) return true;
  if (idSpec == FCAL && (id == FCAL_A || id == FCAL_C)) return true;
  if (idSpec == EM && (matchCalo(id, EMB) || matchCalo(id, EMEC))) return true;
  if (idSpec == PRECISION_EM && (matchCalo(id, EMB) || matchCalo(id, EMEC_OUTER))) return true;
 
  return false;
}


short Id::sign(CaloId id)
{
  switch (id) {
    case FCAL_C : return -1;
    case HEC_C :  return -1;
    case EMEC_OUTER_C : return -1;
    case EMEC_INNER_C : return -1;
    case EMEC_C : return -1;
    case EMB_C :  return -1;
    case UNKNOWN_CALO : return 0;
    case EMB_A :  return +1;
    case EMEC_INNER_A : return +1;
    case EMEC_OUTER_A : return +1;
    case EMEC_A : return +1;
    case HEC_A :  return +1;
    case FCAL_A : return +1;
    
    case EMB  : return 0;
    case EMEC : return 0;
    case EMEC_OUTER : return 0;
    case EMEC_INNER : return 0;
    case HEC  : return 0;
    case FCAL : return 0;
    case EM : return 0;
    case PRECISION_EM : return 0;
  }

  return 0;
}

CaloId Id::mirror(CaloId id)
{
  switch (id) {
    case FCAL_C : return FCAL_A;
    case HEC_C :  return HEC_A;
    case EMEC_OUTER_C : return EMEC_OUTER_A;
    case EMEC_INNER_C : return EMEC_INNER_A;
    case EMEC_C : return EMEC_A;
    case EMB_C :  return EMB_A;
    case UNKNOWN_CALO : return UNKNOWN_CALO;
    case EMB_A :  return EMB_C;
    case EMEC_INNER_A : return EMEC_INNER_C;
    case EMEC_OUTER_A : return EMEC_OUTER_C;
    case EMEC_A : return EMEC_C;
    case HEC_A :  return HEC_C;
    case FCAL_A : return FCAL_C;
    
    case EMB  : return EMB;
    case EMEC : return EMEC;
    case EMEC_OUTER : return EMEC_OUTER;
    case EMEC_INNER : return EMEC_INNER;
    case HEC  : return HEC;
    case FCAL : return FCAL;
    case EM : return EM;
    case PRECISION_EM : return PRECISION_EM;
  }

  return UNKNOWN_CALO;
}


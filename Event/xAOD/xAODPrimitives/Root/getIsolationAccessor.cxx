/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

// Local include(s):
#include "xAODPrimitives/tools/getIsolationAccessor.h"

// System include(s):
#include <iostream>

/// Helper macro for Accessor objects
#define DEFINE_ACCESSOR(TYPE)                                 \
  case xAOD::Iso::TYPE:                                       \
{                                                             \
  static const SG::AuxElement::Accessor< float > a( #TYPE);   \
  return &a;                                                  \
}                                                             \
break;


namespace xAOD {

const SG::AuxElement::Accessor< float >*
  getIsolationAccessor( Iso::IsolationType type ) {
    switch( type ) {
      //EtCone variables      
      DEFINE_ACCESSOR( etcone20 );
      DEFINE_ACCESSOR( etcone30 );
      DEFINE_ACCESSOR( etcone40 );
      //PtCone variables      
      DEFINE_ACCESSOR( ptcone20 );
      DEFINE_ACCESSOR( ptcone30 ); 
      DEFINE_ACCESSOR( ptcone40 ); 
      DEFINE_ACCESSOR( ptcone50 );
      // TopoEtcone variables:
      DEFINE_ACCESSOR( topoetcone20 );
      DEFINE_ACCESSOR( topoetcone30 );
      DEFINE_ACCESSOR( topoetcone40 );
      // Mini-Isolation (http://arxiv.org/abs/1007.2221) variables:
      DEFINE_ACCESSOR(ptvarcone20);
      DEFINE_ACCESSOR(ptvarcone30);
      DEFINE_ACCESSOR(ptvarcone40);
      // Ptcone variables for high mu:
      DEFINE_ACCESSOR(ptcone20_Nonprompt_All_MaxWeightTTVA_pt500);
      DEFINE_ACCESSOR(ptcone30_Nonprompt_All_MaxWeightTTVA_pt500);
      DEFINE_ACCESSOR(ptcone40_Nonprompt_All_MaxWeightTTVA_pt500);
      DEFINE_ACCESSOR(ptcone20_Nonprompt_All_MaxWeightTTVA_pt1000);
      DEFINE_ACCESSOR(ptcone30_Nonprompt_All_MaxWeightTTVA_pt1000);
      DEFINE_ACCESSOR(ptcone40_Nonprompt_All_MaxWeightTTVA_pt1000);
      DEFINE_ACCESSOR(ptcone20_Nonprompt_All_MaxWeightTTVALooseCone_pt500);
      DEFINE_ACCESSOR(ptcone30_Nonprompt_All_MaxWeightTTVALooseCone_pt500);
      DEFINE_ACCESSOR(ptcone40_Nonprompt_All_MaxWeightTTVALooseCone_pt500);
      DEFINE_ACCESSOR(ptcone20_Nonprompt_All_MaxWeightTTVALooseCone_pt1000);
      DEFINE_ACCESSOR(ptcone30_Nonprompt_All_MaxWeightTTVALooseCone_pt1000);
      DEFINE_ACCESSOR(ptcone40_Nonprompt_All_MaxWeightTTVALooseCone_pt1000);
      // Mini-Isolation (http://arxiv.org/abs/1007.2221) variables for high mu:
      DEFINE_ACCESSOR(ptvarcone20_Nonprompt_All_MaxWeightTTVA_pt500);
      DEFINE_ACCESSOR(ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt500);
      DEFINE_ACCESSOR(ptvarcone40_Nonprompt_All_MaxWeightTTVA_pt500);
      DEFINE_ACCESSOR(ptvarcone20_Nonprompt_All_MaxWeightTTVA_pt1000);
      DEFINE_ACCESSOR(ptvarcone30_Nonprompt_All_MaxWeightTTVA_pt1000);
      DEFINE_ACCESSOR(ptvarcone40_Nonprompt_All_MaxWeightTTVA_pt1000);
      DEFINE_ACCESSOR(ptvarcone20_Nonprompt_All_MaxWeightTTVALooseCone_pt500);
      DEFINE_ACCESSOR(ptvarcone30_Nonprompt_All_MaxWeightTTVALooseCone_pt500);
      DEFINE_ACCESSOR(ptvarcone40_Nonprompt_All_MaxWeightTTVALooseCone_pt500);
      DEFINE_ACCESSOR(ptvarcone20_Nonprompt_All_MaxWeightTTVALooseCone_pt1000);
      DEFINE_ACCESSOR(ptvarcone30_Nonprompt_All_MaxWeightTTVALooseCone_pt1000);
      DEFINE_ACCESSOR(ptvarcone40_Nonprompt_All_MaxWeightTTVALooseCone_pt1000);
      // neutral eflow
      DEFINE_ACCESSOR(neflowisol20);
      DEFINE_ACCESSOR(neflowisol30);
      DEFINE_ACCESSOR(neflowisol40);
    default:
      std::cerr << "xAOD::Primitives  ERROR Unknown float IsolationType ("
        << type << ") requested" << std::endl;
      return nullptr;
    }
  }
}// namespace xAOD

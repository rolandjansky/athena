/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENTTPCNV_JETEVENTTPCNVCNVDICT_H
#define JETEVENTTPCNV_JETEVENTTPCNVCNVDICT_H


// DEBUG !!!!
//#define private public
//#define protected public

#include "JetEventTPCnv/JetMomentMap_p6.h" 
#include "JetEventTPCnv/JetMomentMapCollection_p6.h" 

#include "JetEventTPCnv/JetCollection_tlp6.h"
#include "JetEventTPCnv/JetCollection_p6.h"
#include "JetEventTPCnv/Jet_p6.h"

#include "JetEventTPCnv/JetCollection_tlp5.h"
#include "JetEventTPCnv/JetCollection_p5.h"
#include "JetEventTPCnv/Jet_p5.h"

#include "JetEventTPCnv/JetCollection_tlp2.h"
#include "JetEventTPCnv/JetCollection_p4.h"
#include "JetEventTPCnv/Jet_p4.h"
#include "JetEventTPCnv/JetCollection_p3.h"
#include "JetEventTPCnv/Jet_p3.h"
#include "JetEventTPCnv/JetCollection_p2.h"
#include "JetEventTPCnv/Jet_p2.h"
#include "JetEventTPCnv/JetCollection_p1.h"
#include "JetEventTPCnv/Jet_p1.h"
#include "JetEventTPCnv/JetKeyDescriptor_p1.h"
#include "JetEventTPCnv/JetKeyDescriptorCollection_p1.h"
#include "JetEventTPCnv/JetKeyDescriptorCollection_tlp1.h"

#include "JetEventTPCnv/JetSampling_p1.h"
#include "JetEventTPCnv/JetSamplingCollection_p1.h"
#include "JetEventTPCnv/JetSampling_p2.h"
#include "JetEventTPCnv/JetSamplingCollection_p2.h"

#include "JetEventTPCnv/JetAssociationBase_p1.h"

#include "JetEventTPCnv/ParticleJet_p1.h"
#include "JetEventTPCnv/ParticleJetContainer_p1.h"

// DEBUG !!!!
//#undef protected
//#undef private



namespace {
  struct GCCXML_DUMMY_INSTANTIATION_JETEVENTTPCNV {

    std::vector<JetKeyDescriptor_p1> m_jetKeyDescriptorp1;
    std::vector<JetKeyDescriptorCollection_p1> m_jetKeyDescriptorCp1;

    std::vector<JetMomentMap_p6> m_jetMomentMapp6;

    std::vector<Jet_p6> m_jetp6;
    std::vector<JetCollection_p6> m_jetCp6;

    std::vector<Jet_p5> m_jetp5;
    std::vector<JetCollection_p5> m_jetCp5;

    std::vector<Jet_p4> m_jetp4;
    std::vector<JetCollection_p4> m_jetCp4;

    std::vector<JetAssociationBase_p1> m_jetAss;

  };
}

#endif //JETEVENTTPCNV_JETEVENTTPCNVCNVDICT_H

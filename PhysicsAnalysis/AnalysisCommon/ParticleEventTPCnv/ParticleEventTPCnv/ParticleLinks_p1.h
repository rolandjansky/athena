////////////////////////////////////////////////////////////////// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleLinks_p1.h 
// Header file for class ParticleLinks_p1
// persistent form of ParticleLinks
//
// Author: S. Protopopescu <serban@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENTTPCNV_PARTICLELINKS_P1_H
#define PARTICLEEVENTTPCNV_PARTICLELINKS_P1_H 1
 
// AthenaPoolCnvSvc includes
#include "DataModelAthenaPool/DataLink_p1.h"
#include "ParticleEventTPCnv/SelectedParticles_p1.h"

class ParticleLinks_p1{ 
 public:
    ParticleLinks_p1(){;}
   ~ParticleLinks_p1() = default;
 private:
    SelectedParticles_p1 m_sel;
    DataLink_p1 m_containerLink;
 public:
    void setSelectedParticles_p1(const SelectedParticles_p1& sel){m_sel=sel;}
    void setDataLink_p1(const DataLink_p1& containerLink){m_containerLink=containerLink;}
    const DataLink_p1* dl_p1() const {return &m_containerLink;}
    const SelectedParticles_p1* sel() const {return &m_sel;}
};

#endif // !PARTICLEEVENTTPCNV_PARTICLELINKS_P1_H

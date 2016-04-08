/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGEVENT_JETTAGEVENTDICT_H
#define JETTAGEVENT_JETTAGEVENTDICT_H

#include "JetTagEvent/JetConstituent.h"
#include "JetTagEvent/TrackConstituents.h"
#include "JetTagEvent/TrackAssociation.h"
#include "JetTagEvent/ISvxConstituent.h"
#include "JetTagEvent/ISvxAssociation.h"

// This needs to be here to work around a reflex dictionary generation problem.
// Otherwise, you'll get
// Warning in <TClass::TClass>: no dictionary for class DataVector<ParticleJet,DataVector<ParticleBase,DataVector_detail::NoBase> > is available
struct GCCXML_DUMMY_INSTANTIATION_JETTAGEVENT {
  DataVector<Rec::TrackParticle> m_dummy1;
  DataVector<int> m_dummy2;
};


#endif

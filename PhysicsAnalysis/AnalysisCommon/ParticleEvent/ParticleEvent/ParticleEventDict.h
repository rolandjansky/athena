///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleEventDict.h 
// Header for dict. 'bootstrapping'
// Author  : Sebastien Binet
/////////////////////////////////////////////////////////////////// 
#ifndef PARTICLEEVENT_PARTICLEEVENTDICT_H
#define PARTICLEEVENT_PARTICLEEVENTDICT_H

#include "ParticleEvent/NeutrinoContainer.h" 
#include "ParticleEvent/ParticleBaseContainer.h"
#include "ParticleEvent/CompositeParticleContainer.h"
#include "ParticleEvent/IConstituent.h"
#include "ParticleEvent/ParticleImpl.h"
#include "ParticleEvent/ParticleSigStateImpl.h"
#include "ParticleEvent/SelectedParticles.h"
#include "ParticleEvent/ParticleBaseLinks.h"
#include "ParticleEvent/IParticleLinks.h"
#include "ParticleEvent/TrackParticleAssocs.h"
#include "ParticleEvent/INav4MomToTrackParticleAssocs.h"
#include "ParticleEvent/ParticleShallowClone.h"
#include "ParticleEvent/ParticleShallowCloneContainer.h"

#include "ParticleEvent/Lib/Base.h"
#include "FourMom/Lib/P4ImplPxPyPzE.h"
#include "FourMom/Lib/P4ImplEEtaPhiM.h"
#include "FourMom/Lib/P4ImplIPtCotThPhiM.h"

namespace ParticleEventDict{

  struct temp {
    std::vector<Analysis::IConstituent*> m_constituents;
    ParticleImpl<Navigable<DataVector<INavigable4Momentum>,double,std::vector<std::pair<ElementLink<DataVector<INavigable4Momentum> >,double> > >,P4ImplPxPyPzE,ParticleEvent::Base> m_dum2;
    ParticleLinks<ParticleBaseContainer> m_pb;
 ElementLink<ParticleBaseContainer> m_elpb;
    ParticleLinks<IParticleContainer> m_ip;
    CompositeParticle::Base_t m_dum3;
    ParticleSigStateImpl<Navigable<DataVector<INavigable4Momentum>,double,std::vector<std::pair<ElementLink<DataVector<INavigable4Momentum> >,double> > >,P4ImplPxPyPzE,ParticleEvent::Base> m_dum4;

    // instantiations for egamma, muon and taujet
    typedef ParticleImpl<NavigableTerminalNode,
			 P4ImplEEtaPhiM,
			 ParticleEvent::Base> PImpl_t_1;
    PImpl_t_1 m_impl1;
    typedef ParticleImpl<NavigableTerminalNode,
			 P4ImplIPtCotThPhiM,
			 ParticleEvent::Base> PImpl_t_2;
    PImpl_t_2 m_impl2;

    // for neutrino
    typedef ParticleImpl<NavigableTerminalNode,
			 P4ImplPxPyPzE,
			 ParticleEvent::Base> PImpl_t_3;
    PImpl_t_3 m_impl3;
    
    //TrackParticle to TrackParticle Association
    std::vector< DataLink<TrackParticleAssocs> >           m_assocStores;
    DataLink<TrackParticleAssocs>                          m_assocsLink;
    TrackParticleAssocs::AssociationObjectIterator         m_aoi;
    TrackParticleAssocs::AssociationVectorIterator         m_avi;
    TrackParticleAssocs                                    m_assocs;
    TrackParticleAssocs::asso_list                         m_assocs_asso_list;
    TrackParticleAssocs::asso_list::const_iterator         m_assocs_asso_list_const_itr;
    TrackParticleAssocs::asso_list::iterator               m_assocs_asso_list_itr;

    //INav4Mom to TrackParticle Association
    std::vector< DataLink<INav4MomToTrackParticleAssocs> >   m_inavtotpassocStores;
    DataLink<INav4MomToTrackParticleAssocs>                  m_inavtotpassocsLink;
    INav4MomToTrackParticleAssocs::AssociationObjectIterator m_inavtotpaoi;
    INav4MomToTrackParticleAssocs::AssociationVectorIterator m_inavtotpavi;
    INav4MomToTrackParticleAssocs                            m_inavtotpassocs;
    INav4MomToTrackParticleAssocs::asso_list                 m_inavtotpassocs_asso_list;
    INav4MomToTrackParticleAssocs::asso_list::const_iterator m_inavtotpassocs_asso_list_const_itr;
    INav4MomToTrackParticleAssocs::asso_list::iterator       m_inavtotpassocs_asso_list_itr;

    // ParticleShallowClone and Container
    Analysis::ParticleShallowClone m_particleShallowClone;
    ParticleShallowCloneContainer m_particleShallowCloneCont;

 };
} // end ParticleEventDict namespace

#endif // PARTICLEEVENT_PARTICLEEVENTDICT_H

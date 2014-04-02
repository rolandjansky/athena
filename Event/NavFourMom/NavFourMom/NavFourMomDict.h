///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NAVFOURMOM_NAVFOURMOMDICT_H
#define NAVFOURMOM_NAVFOURMOMDICT_H

#include <list>
#include "NavFourMom/I4MomentumContainer.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/IParticleContainer.h"
#include "NavFourMom/INav4MomAssocs.h"
#include "NavFourMom/IParticleLink.h"
#include "NavFourMom/INav4MomLink.h"
#include "NavFourMom/IParticleLinkContainer.h"
#include "NavFourMom/INav4MomLinkContainer.h"
#include "NavFourMom/Nav4MomWrapper.h"
#include "FourMom/Lib/P4ImplPxPyPzE.h"
#include "Navigation/Navigable.h"

namespace NavFourMomDict{

  // force the dictionary to be created.
  struct temp {

    // el-links
    INav4MomLink m_inav4momLink;
    IParticleLink m_iparticleLink;

    INav4MomLinkContainer  m_inav4momLinkCont;
    IParticleLinkContainer m_iparticleLinkCont;

    DataVector_detail::VirtBases<INavigable4Momentum> m_virtBase;
    DataVector_detail::VirtBases<IParticle>           m_virtBase2;
    std::vector< DataLink<INav4MomAssocs> >           m_assocStores;
    DataLink<INav4MomAssocs>                          m_assocsLink;
    INav4MomAssocs::AssociationObjectIterator         m_aoi;
    INav4MomAssocs::AssociationVectorIterator         m_avi;
    Navigable<DataVector<INavigable4Momentum> >       m_ndvni;
    Navigable<DataVector<INavigable4Momentum> >::object_iter       m_ndvnii;
    Navigable<DataVector<INavigable4Momentum>,double >       m_ndvni_d;
    Navigable<DataVector<INavigable4Momentum>,double >::object_iter       m_ndvnii_d;

    std::vector<INavigable4Momentum*>                 m_inav4moms;
    std::vector<IParticle*>                           m_iparts;
    std::vector<const IParticle*>                     m_const_iparts;
    Nav4MomWrapper<Navigable<DataVector<INavigable4Momentum>,double,std::vector<std::pair<ElementLink<DataVector<INavigable4Momentum> >,double> > >,P4ImplPxPyPzE> m_dum1;

    // for INav4MomAssocs api
    INav4MomAssocs m_assocs;
    INav4MomAssocs::asso_list m_assocs_asso_list;
    INav4MomAssocs::asso_list::const_iterator m_assocs_asso_list_const_itr;
    INav4MomAssocs::asso_list::iterator m_assocs_asso_list_itr;
  };


}


#endif 

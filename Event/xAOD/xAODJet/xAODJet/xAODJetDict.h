// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODJetDict.h 796773 2017-02-12 19:01:20Z sschramm $
#ifndef XAODJET_XAODJETDICT_H
#define XAODJET_XAODJETDICT_H

// Needed to successfully generate the dictionary in standalone mode:
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

// System include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"

// Local include(s):
#include "xAODJet/JetContainer.h"
#include "xAODJet/versions/JetContainer_v1.h"
#include "xAODJet/versions/JetAuxContainer_v1.h"
#include "xAODJet/versions/JetTrigAuxContainer_v1.h"
#include "xAODJet/versions/JetTrigAuxContainer_v2.h"
#include "xAODJet/versions/Jet_v1.h"
#include "xAODJet/JetTypes.h"

namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODJET {
      // Instantiate the container for gccxml:
      xAOD::JetContainer_v1 c1;

      // Smart pointers to Jet_v1:
      DataLink< xAOD::JetContainer_v1 > e1;
      std::vector< DataLink< xAOD::JetContainer_v1 > > e2;
      ElementLink< xAOD::JetContainer_v1 > e3;
      std::vector< ElementLink< xAOD::JetContainer_v1 > > e4;
      std::vector< std::vector< ElementLink< xAOD::JetContainer_v1 > > > e5;

      // Instantiations to create the EL member dictionaries correctly:
      xAOD::IParticleContainer pc1;
      ElementLink<xAOD::IParticleContainer> e10;

     std::pair<std::string,std::vector<const xAOD::IParticle*> > bug1;
     std::pair<std::string,std::vector<float> > bug2;
     std::pair<std::string,std::vector<std::vector<int> > > bug3;
     std::pair<std::string,std::vector<std::vector<float> > > bug4;

     // momentums     
     //xAOD::JetFourMom_t jet4mom;
     std::vector<xAOD::JetFourMom_t> vjet4mom;

     //std::vector<const xAOD::IParticle*> vpart; not yet : not sure this belongs elsewhere ?
     //
   };
}


template float xAOD::Jet_v1::getAttribute<float> (xAOD::JetAttribute::AttributeID) const ; 
template int xAOD::Jet_v1::getAttribute<int> (xAOD::JetAttribute::AttributeID) const ; 
template xAOD::JetFourMom_t xAOD::Jet_v1::getAttribute<xAOD::JetFourMom_t> (xAOD::JetAttribute::AttributeID) const ; 
template std::vector<float> xAOD::Jet_v1::getAttribute< std::vector<float> > (xAOD::JetAttribute::AttributeID) const ;
template std::vector<int> xAOD::Jet_v1::getAttribute< std::vector<int> > (xAOD::JetAttribute::AttributeID) const ;

template float xAOD::Jet_v1::getAttribute<float> (const std::string &) const ; 
template int xAOD::Jet_v1::getAttribute<int> (const std::string &) const ; 
template xAOD::JetFourMom_t xAOD::Jet_v1::getAttribute<xAOD::JetFourMom_t> (const std::string &) const ; 
template std::vector<float> xAOD::Jet_v1::getAttribute< std::vector<float> > (const std::string &) const ;
template std::vector<int> xAOD::Jet_v1::getAttribute< std::vector<int> > (const std::string &) const ;


template void xAOD::Jet_v1::setAttribute<float>(const std::string &, const float& ) ;
template void xAOD::Jet_v1::setAttribute<int>(const std::string &, const int& ) ;

template void xAOD::Jet_v1::setAttribute<std::vector<float> >(const std::string &, const std::vector<float>& ) ;
template void xAOD::Jet_v1::setAttribute<std::vector<int> >(const std::string &, const std::vector<int>& ) ;



template std::vector<const xAOD::IParticle*> xAOD::Jet_v1::getAssociatedObjects<xAOD::IParticle >( xAOD::JetAttribute::AssoParticlesID ) const;
template std::vector<const xAOD::IParticle*> xAOD::Jet_v1::getAssociatedObjects<xAOD::IParticle >( const std::string &  ) const ;

template void xAOD::Jet_v1::setAssociatedObjects<xAOD::IParticle >( const std::string &, const std::vector<const xAOD::IParticle*>& ) ;


template const xAOD::IParticle* xAOD::Jet_v1::getAssociatedObject<xAOD::IParticle>( const std::string &) const ;
template const xAOD::IParticle* xAOD::Jet_v1::getAssociatedObject<xAOD::IParticle>( xAOD::JetAttribute::AssoParticlesID ) const ;
template void xAOD::Jet_v1::setAssociatedObject<xAOD::IParticle>( const std::string &,  const xAOD::IParticle* ) ;



#endif // XAODJET_XAODJETDICT_H

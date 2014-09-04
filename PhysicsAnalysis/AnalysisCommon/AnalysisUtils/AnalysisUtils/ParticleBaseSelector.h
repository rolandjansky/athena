/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************
ParticleBaseSelector.h:  
   Example of a Selector class with ParticleBaseContainer
   Note that for every type of Container there is a ParticleLinks<Container>
   that needs an id for the store gate and a uuid for an output file. See for example;
   ParticleEvent/ParticleBaseLinks.h
   ParticleEvent/selection.xml

When running the selector you have the options to generate a SelectedParticles object
and/or a ParticleLinks<Container> object. The SelectorBase expects a Container key
and keys for output SelectedParticles and/or ParticleLinks. You can also specify an input
key for SelectedParticles or for ParticleLinks (but not both)..
                 
          S. Protopopescu 11-DEC-2008
 
***********************************************************************/

#ifndef PARTICLEBASESELECTOR_H
#define PARTICLEBASESELECTOR_H

#include "AnalysisUtils/SelectorBase.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "ParticleEvent/ParticleBaseContainer.h"
#include "ParticleEvent/ParticleBaseLinks.h"


////////////////////////////////////////////////////////////////
//
//  Selection can be made on:
// 
//  PtMin (default 15 GeV, expects GeV)
//  PtMax (default 10000 GeV, expects GeV)
//  EtaMin (eta, default -2.5)
//  EtaMax (eta, default 2.5)
//  AbsEtaMin (default 0)
//  AbsEtaMax (|eta|, default 2.5)
//
//  These parameters are in SelectorBase
//
///////////////////////////////////////////////////////////////////

class ParticleBaseSelector : public SelectorBase<ParticleBaseSelector>{

 public:

    typedef ParticleBaseContainer Container;
    typedef ParticleBase          Contained;
    typedef ParticleBaseLinks     Links;
    typedef ParticleBaseLink     Link;

    ParticleBaseSelector(const std::string& name, ISvcLocator* pSvcLocator);
    ~ParticleBaseSelector(){;}
    StatusCode initialize();
    bool accept(const ParticleBaseLink& link);
    bool accept(const Contained* part); 

 private:
    double m_Emax;
    double m_Emin;
};


#endif

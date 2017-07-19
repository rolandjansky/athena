/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AnalysisUtils_page AnalysisUtils Package

The package which provides some utils for the AOD objects.

@authors Sebastien Binet, Tadashi Maeno


@section AnalysisUtils_AnalysisUtilsIntro Introduction

@section AnalysisUtils_AnalysisUtilsOverview Class Overview

  The AnalysisUtils package contains of following classes:
 
  - AnalysisCombination:

  - AnalysisLooper:

  - AnalysisMisc:

  - AnalysisPermutation:

  - AnalysisSelection:

  - IFilterCuts: this is a class which is used internally  by the filters to copy their cut properties one to another (via the IFilterCuts::setFilter(ifilter) method).
<i>Nota:</i> this class (contrary to IFilter) is not templated.

  - IFilterUtils: Implements a copy iterator (STL is missing this utility, though one could use the std::remove_if with the negative predicate).
The output destination container is filled with the current iterator on the input container, only if the predicate is true.

  - IFilter: this is the main interface to the filters. 
It declares the isAccepted method which will be overloaded by derived classes.
It inherits from IFilterCuts to be able to copy a set of cut properties without explicitly knowing the concrete implementation of the derived filter.

  - MomentumFilter: this class can filter objects upon their four-momentum properties. It can select particles based on their px,py,pz and energy as well as their mass, pt, eta or phi.
One can requires minimum or maximum cuts (or both).

  - PdgIdFilter: this class selects particles from their pdg id code.
Up to now, it is based on a ParticleCandidateList but it is very likely that internally it will be based on DecayPattern.
See FilterExample for a concrete example (in AnalysisExample package).

  - CategoryIdFilter: this class selects particles from their pdg id code.
This filter defines categories of particles (Boson, Lepton, Hadron, Meson, ...)
and accepts or not a particle if it belongs to at least one of the given 
categories it has been setup to match. You can build very specific filters by
combining multiple CategoryIdFilters.

  - ChargeFilter: this class can filter objects upon their charge.
One can only require a charge (not a range in charges).
But it could be extended to a range.

  - IIParticleFilter: this is a templated class which inherits the capabilities of the Charge- Momentum- and PdgIdFilter classes. 
So it can filter every objects which honours the IParticle interface. 
An object is accepted if all its properties (charge, pdg ID and momentum) fulfill the requirements.

  - IParticleFilter: this is the filter for IParticles. 
It resolves the templated class of IIParticleFilter to IParticle. So each class within the AOD packages should be compliant with this filter.
The IParticleFilter can select particles from their momentum, PDGId and charge.

  - McVtxFilter: this class allows to select HepMC::GenParticle based on the decay pattern.
It uses internally the DecayPattern class to build the filtering decision.
The basic <i>modus operandi</i> is to loop on HepMC::GenVertex (of a given HepMC::GenEvent) to select the ones which are of interest (and fulfill the decay pattern).

  - ParticleCandidateList: this is a simple wrapper for std::list<PDG::pidType> to model a list of particle identity candidates. 
This candidate list is used by the PdgIdFilter and McVtxFilter to select for particles which might fulfill some criterion (well in our case this is its Particle Data Group identity.

  - DecayParser: this is the class which parses and models decay patterns.
You can describe via a std::string the decay pattern you are looking for.
<i>Eg:</i> you just have to say "23->-11 + 11" to create a pattern which can then be used to filter for vertices where such a decay occurs.

  - FilterRange: it implements the range (ie: [min, max]) the filters will use to take their filtering decisions.
Internally it uses the Boost class interval but maybe one could use a SEAL class (FML/RangeSet or FML/Bound).

  - PhiFilterRange: it implements the range (ie: [min, max]) the filters will use to take their filtering decisions. 
It is a specialisation of FilterRange for the phi angles. It enforces the ATLAS policy for phi angles : \f$ \phi \in [-\pi,\pi[ \f$

  - ITruthParticleCnvTool: this is the interface for AlgTools which want to convert McEventCollection objects into a TruthParticleContainer objects. 
It is also used by the custom POOL converter for TruthParticleContainer to recreate the TruthParticleContainers on the fly (save some disk space).
ITruthParticleCnvTool also provides some helper functions related with mother/daughter relationships in the TruthParticle context.


@section AnalysisUtils_AnalysisUtilsSeeAlso See also

You may also be interested in:

   - \ref AnalysisTools

   - \ref AnalysisAssociation

   - \ref AnalysisExamples to see how these classes may be used : FilterExample



*/

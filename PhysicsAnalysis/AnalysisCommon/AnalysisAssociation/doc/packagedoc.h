/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page AnalysisAssociation_page AnalysisAssociation Package

This package provides classes to store associations between AOD classes as well as filters to build these associations.

@author Sebastien Binet


@section AnalysisAssociation_AnalysisAssociationIntro Introduction

The Association data classes are the objects which can build relations between (multiple) AOD objects. To ease the building of such links filters are provided. Up to now only four momentum-like cut variables can be constructed to filter on. Associations between @c IParticles and between @c INavigable4Momentum objects can be made persistent (<i>ie:</i> one can store them in a POOL file).

@section AnalysisAssociation_AnalysisAssociationOverview Class Overview

  The AnalysisAssociation package contains the following classes:
 
  - IAssocFilter : interface for the association filters. This class declares only one single method the derived classes will have to implement to their needs : the IAssocFilter::isAccepted( obj, asso) method. This method will return true or false whether the association can be made with regard to the cuts which have been specified.
  - AssocFilter : templated class which implements 4-momentum cuts for association filters.
  - IParticleAssocFilter: specialised class to work on IParticle data objects. It inherits from AssocFilter and set the templated classes T to IParticle.

@section AnalysisAssociation_AnalysisAssociationSeeAlso See also

You may also be interested in:

   - AnalysisUtils : IFilterCuts, IFilter<T>, MomentumFilter<T>, IParticleFilter and McVtxFilter





*/

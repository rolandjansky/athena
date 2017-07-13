/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkSpacePoint_page TrkSpacePoint Package
@author Veronique Boisvert, Maria.Jose.Costa@cern.ch, Martin.Siebel@cern.ch

@section TrkSpacePoint_TrkSpacePointIntro Introduction

This package contains the definition of the abstract base class 
SpacePoint and its collections and containers. The SpacePoint class
is derived from MeasurementBase. GlobalErrors are derived from LocalErrors
which have to be provided by the derived classes.

@section TrkSpacePoint_TrkSpacePointOverview Class Overview
  The TrkSpacePoint package contains the following classes:

   - SpacePoint: SpacePoint Class definition. The following methods are purely
     virtual:
     * cloneToWrappedPhi()
     * localErrorMatrix()
     * clone()
     * dump()
     derived classes are in InnerDetector/InDetRecEvent/SiSpacePoint/
   
   - SpacePointCollection: Class to contain all the SpacePoints of a detector 
   element

   - SpacePointContainer: Container of SpacePointCollection objects (derived 
   from IdentifiableContainer)

   - SpacePointOverlapCollection: Class to contain all the SCT overlap 
   SpacePoints, the other ones are in the SpacePointCollection.
 

*/

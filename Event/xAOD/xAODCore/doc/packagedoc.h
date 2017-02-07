/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page xAODCore_page xAODCore package

   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   $Revision: 568227 $
   $Date: 2013-11-01 10:28:22 +0100 (Fri, 01 Nov 2013) $

@section xAODCore_xAODCoreOverview Overview

   This package is meant to hold code that is useful in multiple xAOD EDM
   packages.

@section xAODCore_xAODCoreClasses Classes

   The most important classes/functions of the package are the following:
     - xAOD::AuxContainerBase: A common base class for simple container types
       that use "default ROOT I/O" for reading and writing auxiliary data.
     - xAOD::AddDVProxy::add: This function needs to be called on all the
       DataVector types that need to be read/written directly from/to a
       ROOT file.


*/

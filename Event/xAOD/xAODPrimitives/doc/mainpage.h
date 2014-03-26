/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage xAODPrimitives Package

   @author Edward Moyse <Edward.Moyse@cern.ch>
   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   $Revision: 582418 $
   $Date: 2014-02-10 16:03:46 +0100 (Mon, 10 Feb 2014) $

   @section xAODPrimitivesOverview Overview

   This package holds some definitions/helper code that is used by the
   different packages to describe similar properties of the reconstructed
   objects, like their isolation.

   @section xAODPrimitivesDefinitions Main Definitions

   The following are the main enumerations defined in the package:
     - xAOD::Iso::IsolationType: The types of all known isolation values
       saved into xAOD files.

   @section xAODPrimitivesHelpers Helper Functions

   The following are functions that are mainly supposed to be used by the
   xAOD EDM packages, and not the xAOD users directly:
     - xAOD::getIsolationAccessor: Helper function for accessing an
       Accessor object handling one of the isolation types.

   @htmlinclude used_packages.html

   @include requirements
*/

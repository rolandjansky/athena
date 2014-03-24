/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage TrigConfxAOD Package

   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   $Revision: 580638 $
   $Date: 2014-01-29 14:18:16 +0100 (Wed, 29 Jan 2014) $

   @section TrigConfxAODOverview Overiview

   This package holds code that can be used in- and outside of Athena to
   set up and interact with the trigger configuration metadata of xAOD files.

   @section TrigConfxAODClasses Main Classes

   The package holds two main components for Athena usage:
    - TrigConf::xAODMenuWriter: An algorithm that creates the event-wise and
      file-wise data that needs to be written to an xAOD file to store the
      trigger configuration in it.
    - TrigConf::xAODConfigSvc: An implementation of the TrigConf::ITrigConfigSvc
      interface that can provide the trigger configuration information using
      the "normal" Athena interfaces, when reading an xAOD file.

   @htmlinclude used_packages.html

   @include requirements
*/

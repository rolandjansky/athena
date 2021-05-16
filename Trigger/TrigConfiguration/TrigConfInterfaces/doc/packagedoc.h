/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrigConfInterfaces_page TrigConfInterfaces Package

   @author Joerg Stelzer        <Joerg.Stelzer@cern.ch>
   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   $Revision: 612008 $
   $Date: 2014-08-15 15:35:58 +0200 (Fri, 15 Aug 2014) $

@section TrigConfInterfaces_TrigConfInterfacesOverview Overview

   This package holds all the interfaces through which the trigger
   configuration services should be accessed both in- and outside of
   Athena.

@section TrigConfInterfaces_TrigConfInterfacesClasses Classes

   The following are the interfaces that can be used outside of Athena:
     - TrigConf::IILVL1ConfigSvc: Interface to services providing LVL1
       configuration data.
     - TrigConf::IIHLTConfigSvc: Interface to services providing HLT
       configuration data.

   The following are the Athena-specific interfaces that should be used
   as the template argument for ServiceHandle objects in order to retrieve
   configuration services in Athena:
     - TrigConf::ILVL1ConfigSvc: Interface providing LVL1 configuration data.
     - TrigConf::IHLTConfigSvc: Interface providing HLT configuration data.
     - TrigConf::ITrigConfigSvc: Interface providing all aspects of the
       trigger configuration for offline usage. This is the most often used
       interface for analysis purposes.

   And finally, there is an interface that can be used in the same way in
   and outside of Athena: TrigConf::ITrigConfigTool. This is an interface
   for dual-use ASG tools that are meant for trigger aware analysis.


*/

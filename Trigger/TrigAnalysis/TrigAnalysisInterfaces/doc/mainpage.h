/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage TrigAnalysisInterfaces package

   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   @section TrigAnalysisInterfacesIntro Introduction

   This package holds some tool interfaces that can be used in a trigger-aware
   analysis. The implementation of the interfaces is in other packages.
   Mostly in the TrigBunchCrossingTool package.

   @section TrigAnalysisInterfacesClasses Classes/Interfaces

   The following interfaces are defined in the package:
     - Trig::IBunchCrossingTool: A common interface for all the tools providing
       information about the bunch structure of the LHC.
     - Trig::IBunchCrossingConfProvider: An interface that allows the user
       to retrieve a full configuration of the currently loaded bunch structure
       from a tool. It's mainly used for writing the information as metadata
       into various types of output files.

   @htmlinclude used_packages.html

   @include requirements
*/

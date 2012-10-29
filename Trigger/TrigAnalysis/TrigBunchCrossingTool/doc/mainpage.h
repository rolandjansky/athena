/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @mainpage TrigBunchCrossingTool package

   @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>

   @section TrigBunchCrossingToolIntro Introduction

   This package holds the prototype implementation of the BunchCrossingTool. The
   purpose of this tool is to give analysts easy access to information about where
   the bunch crossing producing the current event was inside a bunch train.

   The tool has of course some other functionality as well, but this is the main
   use case.

   @section TrigBunchCrossingToolInstructions Usage instructions

   The package declares two public interfaces for the bunch crossing tools.
   The main part of the interface is
   in Trig::IIBunchCrossingTool. It is separated from the Athena specific
   Trig::IBunchCrossingTool to be able to implement completely Athena independent
   versions of the tool.

   To access one of the tool implementations in an Athena component, declare a
   ToolHandle to it like:

   <code>
   ToolHandle< Trig::IBunchCrossingTool > m_bcTool;
   </code>

   For convenience reasons the package also hosts a python function that can be used
   to set up a default configuration for all the available implementations of the
   interface.
   To use it, you can do something like:

   <code>
   from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool<br/>
   theJob.MyAlgorithm.MyBCTool = BunchCrossingTool()
   </code>

   @section TrigBunchCrossingToolConfInt The configuration provider interface

   The package also hosts another Athena interface: Trig::IBunchCrossingConfProvider.
   The purpose of this interface is to allow users to extract the bunch structure
   configuration for later analysis. Right now I envisage the D3PDMaker code as
   the main user of this interface.

   Usage instructions for this interface will be compiled later...

   @section TrigBunchCrossingToolTools The available tools

   The following implementations are currently available for the generic interface:

    - Trig::TrigConfBunchCrossingTool: Tool getting its configuration information
      from the trigger configuration service. Provides the
      Trig::IBunchCrossingConfProvider interface.
    - Trig::LHCBunchCrossingTool: Tool getting its configuration information from
      the TDAQ conditions DB. Provides the Trig::IBunchCrossingConfProvider
      interface.
    - Trig::MCBunchCrossingTool: Tool getting its configuration information from
      the MetaData of pileup MC files. Provices the Trig::IBunchCrossingConfProvider
      interface.
    - Trig::StaticBunchCrossingTool: A tool not needing connection to any database,
      as it only knows about some internally stored configurations. Mostly here
      for debugging purposes.

   @section TrigBunchCrossingToolImpl Implementation details

   The tools are implemented using a few smart classes:

    - Trig::BunchCrossing: A smart integer object for describing a BCID basically
    - Trig::BunchTrain: A collection of bunch crossings, describing a bunch train
    - Trig::MsgLogger: A new incarnation of the TMVA/SFrame logger class
    - Trig::BunchCrossingToolBase: Base class for all the tools providing the
      Trig::IIBunchCrossingTool interface
    - Trig::BunchCrossingConfProviderBase: Base class for all the tools providing
      the Trig::IBunchCrossingConfProvider interface

   Most of the heavy lifting is done by the common base class of all the implementations
   Trig::BunchCrossingToolBase.

   @htmlinclude used_packages.html

   @include requirements

*/

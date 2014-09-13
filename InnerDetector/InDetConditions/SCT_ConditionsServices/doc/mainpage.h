/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage SCT_ConditionsServices Package
@author shaun.roe@cern.ch


@section SCT_ConditionsServices Introduction

This package contains services which access and provide conditions data for SCT detector elements: modules, wafers and strips. The various services each have a 
specific purpose, but all inherit from ISCT_ConditionsSvc, which provides an interface (specifically, the isGood(...) methods) which can be used by the 
SCT_ConditionsSummarySvc to summarise the total status and report this to clients. SCT_ConditionsSummaryService in turn inherits from IInDetConditionsSvc, the
common interface for Pixels and TRT as well as SCT.

@section SCT_ConditionsOverview Class Overview
SCT_ConditionsServices contains the following classes:

   - ISCT_ConditionsSvc: Base class for the various conditions services, the interface provides the isGood() methods to be used by clients and fill methods (via 
   callback for the IOVDbSvc, and without callback for use with e.g. text files)
   
   - ISCT_ReadoutTool: Interface for the SCT_ReadoutTool, used to determine which chips are read out from a module, taking bypass/redundancy configuration
   into account
  
   - SCT_ByteStreamErrorsSvc: Used to convey errors in the bytestream. In addition to the common interface provided by ISCT_ConditionsSvc, it has methods for 
   creating error conditions for modules which display communication/datastream errors. These extra methods are used in the bytestream decoder, and the service
   then provides the 'isGood' methods to clients based on this information.
   
   - SCT_ConditionsSummarySvc: This class can be configured to loop over the other services in the package, providing an eventual status which is a summary
   of the component services. The services to be used are decided in job options.
   
   - SCT_ConfigurationConditionsSvc: Reads the configuration data for SCT modules which are stored in the COOL database in CoraCool format, and provides module
   or channel mask information to clients.
   
   - SCT_DCSConditionsSvc: Reads DCS data such as power supply status and temperature, and decides on this basis (and some internal criteria) whether a module
   is good or bad at a particular instant.
   
   - SCT_FlaggedConditionsSvc: A 'catch-all' class which is used, for example, in the clustering code, where a module may be declared bad if the number of 
   Raw Data Objects for that module exceeds some set threshold.
   
   - SCT_ModuleVetoSvc: A class which enables one to manually veto modules, either by providing the identifiers in job options or by specifying the module identifiers
   in the /SCT/Manual/BadModules folder of the Cool database.
   
   - SCT_MonitorConditionsSvc: Retrieves data from the database folder /SCT/Derived/Monitoring, which is created by monitoring jobs on the processing farm and contains
   data concerning occupancy/noise during data taking.
   
   - SCT_ReadCalibDataSvc: Accesses the calibration folders of the cool database, which are updated as a result of SCT calibration runs. Provides data on defects and noise
   determined by calibration.
   
   - SCT_SiliconConditionsSvc: Inheriting from ISiliconConditionsSvc (a common interface for Pixels and SCT), this gives information needed to calculate the Lorentz angle, 
   such as bias voltage, depletion voltage and silicon temperature. For the moment, the silicon depletion voltage is hard coded.
   
   - SCT_StripVetoSvc: Analogous to the SCT_ModuleVetoSvc, this provides a way to turn off individual strips manually via job options. Intended for testing and Monte Carlo 
   studies in particular.

@image html ISCT_ConditionsSvc.png

@image html SiliconConditions.png

**/
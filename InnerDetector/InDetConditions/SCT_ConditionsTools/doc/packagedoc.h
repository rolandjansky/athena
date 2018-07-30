/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_ConditionsTools_page SCT_ConditionsTools Package
@author shaun.roe@cern.ch, Susumu.Oda@cern.ch


@section SCT_ConditionsTools_SCT_ConditionsTools Introduction

This package contains tools which access and provide conditions data for SCT detector elements: modules, wafers, chips and strips. The various tools each have a 
specific purpose, but all inherit from ISCT_ConditionsTool, which provides an interface (specifically, the isGood(...) methods) which can be used by the 
SCT_ConditionsSummaryTool to summarise the total status and report this to clients. SCT_ConditionsSummaryTool in turn inherits from IInDetConditionsTool, the
common interface for Pixels and TRT as well as SCT.

   - Conditions data classes are defined in SCT_ConditionsData package. Those classes represent conditions data and are used in SCT_ConditionsAlgorithms and SCT_ConditionsTools.

   - Conditions algorithm classes are defined in SCT_ConditionsAlgorithms package. Those classes retrieve data from COOL database and write out conditions data in StoreGate.

   - Conditions tool classes are defined in this SCT_ConditionsTools package. Those classes are used to retrieved conditions data in StoreGate by clients.

   - Test algorithm classes are defined in SCT_ConditionsAlgorithms package. Those classes are used to test conditions data, conditions algorithms and conditions tools.

@section SCT_ConditionsTools_SCT_ConditionsOverview Class Overview
SCT_ConditionsTools contains the following classes:

   - ISCT_ConditionsTool: Base class for the various conditions tools, the interface provides the isGood() methods to be used by clients.
   
   - SCT_ReadoutTool: Interface for the SCT_ReadoutTool, used to determine which chips are read out from a module, taking bypass/redundancy configuration
   into account
  
   - SCT_ByteStreamErrorsTool: Used to convey errors in the bytestream. In addition to the common interface provided by ISCT_ConditionsTool, it has methods for 
   retrieving error conditions for modules which display communication/datastream errors. The tool provides the 'isGood' methods to clients based on this information.
   
   - SCT_ConditionsSummaryTool: This class can be configured to loop over the other tools in the package, providing an eventual status which is a summary
   of the component tools. The tools to be used are decided in job options.
   
   - SCT_ConfigurationConditionsTool: Reads the configuration data for SCT modules which are stored in the COOL database in CoraCool format, and provides module
   or channel mask information to clients.
   
   - SCT_DCSConditionsTool: Reads DCS data such as power supply status and temperature, and decides on this basis (and some internal criteria) whether a module
   is good or bad at a particular instant.
   
   - SCT_FlaggedConditionsTool: Used for retrieving bad wafers with more than 384 fired strips. Those bad wafers are declared by clustering algorithms/tools.
   
   - SCT_ModuleVetoTool: A class which enables one to manually veto modules, either by providing the identifiers in job options or by specifying the module identifiers
   in the /SCT/Manual/BadModules folder of the Cool database.
   
   - SCT_MonitorConditionsTool: Retrieves data from the database folder /SCT/Derived/Monitoring, which is created by monitoring jobs on the processing farm and contains
   data concerning occupancy/noise during data taking.
   
   - SCT_ReadCalibDataTool: Accesses the calibration folders of the cool database, which are updated as a result of SCT calibration runs. Provides data on defects and noise
   determined by calibration.
   
   - SCT_SiliconConditionsTool: Inheriting from ISiliconConditionsTool (a common interface for Pixels and SCT), this gives information needed to calculate the Lorentz angle, 
   such as bias voltage, depletion voltage and silicon temperature. For the moment, the silicon depletion voltage is hard coded.
   
   - SCT_StripVetoTool: Analogous to the SCT_ModuleVetoTool, this provides a way to turn off individual strips manually via job options. Intended for testing and Monte Carlo 
   studies in particular.

   - SCT_ChargeTrappingTool: Used for special digitisation with charge trapping.

   - SCT_ConditionsParamterTool: Used to give numerical parameters in standalone tests.

   - SCT_ElectricFieldTool: Used for special digitisation with charge trapping.

   - SCT_LinkMaskingTool: Analogous to the SCT_ModuleVetoTool, this provides a way to turn off individual Front-End links.

   - SCT_MajorityConditionsTool: Used for retrieving majority of DCS state in Barrel, Endcap A, Endcap C.

   - SCT_RadDamageSummaryTool: Used for special digitisation with radiation damage.

   - SCT_ReadCalibChipDataTool: Used for retrieving chip calibration parameters.

   - SCT_RODVetoTool: Analogous to the SCT_ModuleVetoTool, this provides a way to turn off individual ReadOut Drivers.

   - SCT_SensorTool: Provides depletion voltage, crystal orientation and manufacturer

   - SCT_TdaqEnabledTool: Provides enabled RODs (only for data)
**/

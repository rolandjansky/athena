/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigCollQuery_page TrigCollQuery Package

The package allowing Athena EventSelect and POOL Collection utilities to query TAG collections using trigger names, even though trigger information is stored in bit arrays.

@author Marcin.Nowak@cern.ch

@section TrigCollQuery_IntroductionTrigCollQuery Introduction
Due to a large number of triggers, storing each trigger in a separate attribute in TAG collections is impractical (and sometimes even impossible). Because of that, trigger information is stored in bit arrays, which in turn are stored as groups of 32-bit integer attributes in TAG collection.

TrigCollQuery package provides tools that allow Athena EventSelector and the standalone POOL Collection utilities to query TAG collections using well know trigger names without worrying about bit positions and the names of attributes storing the bit arrays.

     
@section TrigCollQuery_TrigCollQueryOverview Class Overview
  The TrigCollQuery package contains of following classes:

  - TrigCollQuery : The main class that can read the XML trigger configuration file and do the transformation of trigger query string replacing trigger names with bit operations.

  - TrigCollQueryTool : Implementation of IAthenaSelectorTool interface on top of TrigCollQuery. Allows usage of the trigger queries by Athena EventSelector.


@section TrigCollQuery_TrigCollQueryExecutables Tools
The TrigCollQuery package contains one command line tool - runCollUtil.exe. This tool is a wrapper for all standalone POOL Collection utilities. When put in front of a command that normally executes one of the POOL utilites, it will look for a -query option and search it for any trigger references, replacing them by bit operations.
If the first option (before the executable name of the collection utility) is -usectpword, queries for L1 trigger decision will use the old name of the L1 trigger attribute - "CTPWord". The current attribute name is "L1PassedTrigMaskTAV"
     
@section TrigCollQuery_QueryUsageTrigCollQuery Syntax of Trigger Queries
To query for trigged bit set, add the following statement to the query string:

TriggerPassed(TriggerName1[,TriggerName2,...])

Correspondingly, use a similar syntax to select events that that have not passed given triggers:

TriggerNotPassed(TriggerName1[,TriggerName2,...])

Both statements may appear more than once in the query string, but grouping them together may result in a more efficient query (if the trigger bits are stored in the same word/attribute).


     
@section TrigCollQuery_EventSelectorUsageTrigCollQuery Trigger-based TAG Event selection in Athena
To use a trigger condition in an TAG event selection query in an Athena job, add the following lines to the jonOptions file:

include("TrigCollQuery/TrigCollQuery_jobOptions.py")<br>
ServiceMgr.EventSelector.Query = "NLooseElectron>1 && TriggerPassed(L2_e105)"<br>


TrigCollQueryTool properties:

 - TrigBitsMapFile : the name of the XML file with trigger bit mappings. The file will be searched for in the $XMLPATH. <br> Example:<br>
   MyQueryTool.TrigBitsMapFile = "TriggerMenuXML/ChainTagMap.xml"

 - UseCTPWord : Boolean flag that tells the tool to use the old "CTPWord" attribute name for L1 trigger

@section TrigCollQuery_UtilityUsageTrigCollQuery Querying Triggers using POOL command-line Collection Utilities
To be able to query triggers when using POOL command line collection utilities like CollAppend, one needs to parse the query and translate trigger names into bit positions before passing it on to the utility. This is achieved using a "wrapper" application called runCollUtil.exe. Simply add the wrapper at the start of the command that invokes a POOL collection utility.

Example:<br>
runCollUtil.exe CollListAttrib -src fdr08_run1_MinBias_o1_r24_t3 RootCollection  -query 'TriggerPassed(L2_e12)' 


@section TrigCollQuery_TrigCollQueryTriggerMap The XML Trigger Configuration File
Mapping of the trigger names to bit locations in the TAGs is done according to trigger configuration stored by default in TriggerMenuXML/ChainTagMap.xml file. This file is part of Athena release since version 14.2.20 and is provided by Trigger/TriggerCommon/TriggerMenuXML package. It is currently assumed there are 3 trigger levels: L1, L2 and EF, and trigger bits are stored in TAG attributes called CTPWord, L2PassedTrigMask and EFPassedTrigMask.


@section TrigCollQuery_ExtrasTrigCollQuery Extra Pages

*/

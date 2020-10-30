/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_Cabling_page SCT_Cabling Package
@author shaun.roe@cern.ch


@section SCT_Cabling_SCT_CablingIntro Introduction

This package is meant to provide the mapping between online and offline 
identifiers for each detector element. By online identifier we mean the
way a detector element is uniquely identified in the bytestream (i.e.
the ROD number and the link of the ROD). It also provides the mapping
between a ROB and the offline identifiers which are included in that ROB.

The present clients are the BS converters and the Region Selector used in the 
HLT. 

@section SCT_Cabling_SCT_CablingOverview Class Overview
  SCT_Cabling contains the following classes:
  
   - ISCT_CablingTool: Pure Abstract Baseclass giving accessor methods for SCT_CablingTool. This is
   the main interface for clients.
    
   - SCT_CablingTool: Inheriting from ISCT_CablingTool and AthAlgTool: Provides
   accessor methods for the data structures it reads using SG::ReadCondHandle.
   The data are prepared by SCT_CablingCondAlgFromCoraCool or SCT_CablingCondAlgFromText.
   
   - SCT_OnlineId: A stand-alone class for the online id, which is an encoding of the rod and fibre used
   to readout a particular module.
   
   - SCT_SerialNumber: A stand-alone class for the serial number, which is a 14 digit code associated with
   each SCT module at its manufactire.

   - SCT_CablingData: A class to hold the data necessary for SCT_CablingTool.
   This class is filled by SCT_CablingCondAlgFromCoraCool or SCT_CablingCondAlgFromText for
   SCT_CablingTool.

   - SCT_CablingCondAlgFromCoraCool: A condition algorithm for SCT_CablingTool. This is used to fill the cabling from the database.

   - SCT_CablingCondAlgFromText: A condition algorithm for SCT_CablingTool. This is used to fill the cabling from a text file.

   - SCT_TestCablingAlg: A test algorithm which instantiates the cabling service and loops over all possible
   modules, calling methods to show the online Id and serial number for each one.
 
@internal
@image html structureDiag.png "SCT_Cabling class structure" width=8cm
@endinternal
   
@section SCT_Cabling_SCT_CablingToolDetail SCT_CablingTool in Detail
  SCT_CablingTool is accessed by clients through its ISCT_CablingTool interface. This provides access only to the 'getter' methods. The cabling data are prepared by a condition algorithm (SCT_CablingCondAlgFromCoraCool or SCT_CablingCondAlgFromText)from the database or a text file and the data are read by SCT_CablingTool using SG::ReadCondHandle.
   
   **/

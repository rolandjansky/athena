/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage SCT_Cabling Package
@author shaun.roe@cern.ch


@section SCT_CablingIntro Introduction

This package is meant to provide the mapping between online and offline 
identifiers for each detector element. By online identifier we mean the
way a detector element is uniquely identified in the bytestream (i.e.
the ROD number and the link of the ROD). It also provides the mapping
between a ROB and the offline identifiers which are included in that ROB.

The present clients are the BS converters and the Region Selector used in the 
HLT. 

@section SCT_CablingOverview Class Overview
  SCT_Cabling contains the following classes:
  
   - ISCT_CablingSvc: Pure Abstract Baseclass giving accessor methods for SCT_CablingSvc. This is
   the main interface for clients.
    
   - SCT_CablingSvc: Inheriting from ISCT_CablingSvc, IIncidentListener and Service: Provides both 
   accessor methods and filling methods for the data structures it holds.
   
   - ISCT_FillCabling: Interface for the SCT_Fillxxxx methods which are used to fill the data structures
   in SCT_CablingSvc.
   
   - SCT_FillCablingFromText: Inheriting from ISCT_FillCabling and Service, this is used to fill the 
   cabling data from a text file.
   
   - SCT_FillCablingFromCoraCool: Inheriting from ISCT_FillCabling and Service, this is used to fill the
   cabling from the database, in CoraCool or Cool Vector Payload format.
   
   - SCT_FillCablingFromCoolVector: Inheriting from ISCT_FillCabling and Service, this is used to fill the
   cabling from the database, in Cool Vector Payload format. In fact this is currently redundant, since the 
   necessary changes were accommodated in the SCT_FillCablingFromCoraCool class.
   
   - SCT_OnlineId: A stand-alone class for the online id, which is an encoding of the rod and fibre used
   to readout a particular module.
   
   - SCT_SerialNumber: A stand-alone class for the serial number, which is a 14 digit code associated with
   each SCT module at its manufactire.
   
   - SCT_TestCablingAlg: A test algorithm which instantiates the cabling service and loops over all possible
   modules, calling methods to show the online Id and serial number for each one.
 
@image html structureDiag.png "SCT_Cabling class structure" width=8cm
   
@section SCT_CablingSvcDetail SCT_CablingSvc in Detail
  SCT_CablingSvc is accessed by clients through its ISCT_CablingSvc interface. This provides access only to the 'getter' methods. In addition, it inherits from the IIncidentListener so that it can fill data (if appropriate) at the BeginRun incident. The 'setter' method is only accessible to users of the full class, in this case the fillers: SCT_FillCablingFromText and SCT_FillCablingFromCoraCool. These share a common baseclass and they are passed a pointer to the full SCT_CablingSvc to enable them to fill it. The decision as to which filler is to be instantiated is made by job options: the property 'DataSource' may be set to CORACOOL, in which case the database filler is used, or to a text filename, in which case the text filler is used.
  The filler classes may report whether they can fill during the initialize phase, or need to wait for 'BeginRun'. 

   
   **/
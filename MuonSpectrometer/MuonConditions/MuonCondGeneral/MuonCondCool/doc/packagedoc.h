/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonCondCool_page MuonCondCool Package
@author Monica.Verducci@cern.ch
                                                                                
@section MuonCondCool_MuonCondCoolIntro Introduction
                                                                                
Contains algorithms and services to put and retrieve data from the COOL Condition Database.

@section MuonCondCool_MuonCondCoolOverview Class Overview
The MuonCondCool package provides different algorithms to access to the Conditiond Databse for different Muon technologies.

- MdtReadWriteCoolStr, CscReadWriteCoolStr provide the methods to put data inside the folders (packing them before) 
and the possibility to read them back (unpacking). All these features are set up by JopOptions
with the flags Read/Write True.
- MdtCoolStrSvc, CscCoolStrSvc services related to the capability to read and store data inside the database
- Other utilities for the packing data and for tube status

                                                            

                                                                                
                                                                                
*/

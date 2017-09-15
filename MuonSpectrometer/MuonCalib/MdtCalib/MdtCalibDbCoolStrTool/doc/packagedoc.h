/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibDbCoolStrTool_page MdtCalibDbCoolStrTool Package
@author  Domizia.Orestano@cern.ch 
                                                                                
@section MdtCalibDbCoolStrTool_MdtCalibDbCoolStrToolIntro Introduction
This Tool is used by MdtCalibrationDbSvc to retreive calibration constants 
stored in COOL as CLOBS, unpack them and fill the transient classes used 
by the service.
The retreival is done through callbacks and the Tool takes care of setting the
proper IOV for the transient classes in order to ensure that they can be correctly accessed.
The packing of the CLOBS is performed when filling the DB through the MuonCondCool package.
                                                                                
Currently the default in MdtCalibSvc 
is still to access constants from ASCII files for all
the types of simulated data and this Tool is used mainly in the 
commissioning environment. See MdtCalibSvc documentation for more details on 
how to select it from your jobOptions.

@section MdtCalibDbCoolStrTool_MdtCalibDbCoolStrToolOverview Class Overview
The package contains a single Tool, 
MuonCalib::MdtCalibDbCoolStrTool, inheriting from MuonCalib::IMdtCalibDB.
                                                                                
In order to ensure the correct callback sequence we have registered the following functions:
- MuonCalib::MdtCalibDbCoolStrTool::loadTube to be called when the CondAttrListCollection for the DB folder containing the Tube information is updated
- MuonCalib::MdtCalibDbCoolStrTool::loadRt to be called when the CondAttrListCollection for the DB folder containing the Rt information is updated
- MdtCalibrationDbSvc::loadTube to be called after MuonCalib::MdtCalibDbCoolStrTool::loadTube (it will access the collections created by MuonCalib::MdtCalibDbCoolStrTool::loadTube)
- MdtCalibrationDbSvc::loadRt to be called after MuonCalib::MdtCalibDbCoolStrTool::loadRt (it will access the collections created by MuonCalib::MdtCalibDbCoolStrTool::loadRt)

Properties settable through jobOptions are:
- TubeFolder: the name of the DB Folder containing Tube information, which should correspond to the one listed in IOVDbSvc.Folders. 
Current default is /MDT/T0 
but be aware that folder name can change frequently 
- RtFolder: the name of the DB Folder containing Rt information, which should correspond to the one listed in IOVDbSvc.Folders.
Current default is /MDT/RPC
but be aware that folder name can change frequently 
- RT_InputFiles: RT to be used in absence of DB info. If the list contains more than one file only the first one is used (default value DC2_rt_default.dat)
- defaultT0: T0 value to be used in absence of DB info  (default value 800 ns)

                                                                                
*/

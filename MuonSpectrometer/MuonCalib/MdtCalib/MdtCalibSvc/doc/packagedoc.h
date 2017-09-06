/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MdtCalibSvc_page MdtCalibSvc package
@author Domizia.Orestano
original authors Niels van Eldik & Martin Woudstra, Stefano Rosati
 
@section MdtCalibSvc_MdtCalibSvcIntro Introduction
This package contains the services (MdtCalibrationSvc) 
responsible to provide to the
clients (reconstruction) the calibrated information (drift radius and its 
error) for the hits in Mdt chambers.   
 
An older version of service (MdtCalibSvc) is kept for TB applications.

MdtCalibrationSvc accesses the calibration constants through MdtCalibrationDbSvc.
MdtCalibrationDbSvc uses a Tool to retreive the constants: 

- MuonCalib::MdtCalibDbAsciiTool provides some default constants and the 
possibility of overwriting few of them from jobOptions or input files.
- MuonCalib::MdtCalibDbCoolStrTool provides access to the CLOBS in COOL.

By default the MuonCalib::MdtCalibDbAsciiTool is used. In order to use 
MuonCalib::MdtCalibDbCoolStrTool you need to add to your jobOptions the following 
lines:
\code

#for production DB

IOVDbSvc.Folders+=[CondDBCool.MDT+"/MDT/RT <tag>HEAD</tag>"]
IOVDbSvc.Folders+=[CondDBCool.MDT+"/MDT/T0 <tag>HEAD</tag>"]
from MdtCalibDbCoolStrTool.MdtCalibDbCoolStrToolConf import MuonCalib__MdtCalibDbCoolStrTool

#add between the parenthesis the properties of MdtCalibDbCoolStrTool that you 
#eventually would like to redefine (for example the folders if they are not
#the default ones). 
#See MdtCalibDbCoolStrTool for the list of properties and defaults.

MdtDbTool=MuonCalib__MdtCalibDbCoolStrTool()
ToolSvc+=MdtDbTool
from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationDbSvc

#add between the parenthesis the properties of MdtCalibrationDbSvc that you 
#eventually would like to redefine 

MdtCalib=MdtCalibrationDbSvc(DBTool=MdtDbTool)
\endcode

Both the Tools, when organizing the information in the detector store, and 
the MdtCalibrationDbSvc, when accessing it, make use of the mapping 
between detector elements and calibration regions provided by MdtCalibrationRegionSvc.
It is planned to insert the mapping in the DB but currently it is hardcoded in 
MdtCalibrationRegionSvc. By default a single region (a single Rt relation)
is foreseen for the whole spectrometer 

- when using MuonCalib::MdtCalibDbAsciiTool if a single RT ascii file is provided in input. The corresponding Rt relation is then used for all the chambers.
- when using MuonCalib::MdtCalibDbCoolStrTool if Rt conditions data are 
accessed from the folder "/MDT/RT". The last (if more than one is present) valid
Rt from that folder is then applied to all chambers. If no valid Rt is loaded
from DB then the one found in the ascii input file is used.     

One region per chamber is mapped 
- when using MuonCalib::MdtCalibDbAsciiTool if more than one RT ascii file 
is provided in input. See MuonCalib::MdtCalibDbAsciiTool for details.
- when using MuonCalib::MdtCalibDbCoolStrTool if Rt conditions data are accessed
from a folder different from "/MDT/RT". See MuonCalib::MdtCalibDbCoolStrTool for details. 

List of settable properties:
- for MdtCalibrationSvc:
   - is_testbeam (False by default) to use the old MdtCalibSvc 
   - is_simulation (False by default) for simulation of test beam data (old MdtCalibSvc) 
   - CorrectTof (True by default) to correct times for the particle time of flight (to be set to False on Cosmics!!!) 
- for MdtCalibrationDbSvc:
   - DBTool (a PublicToolHandle, default is 'MuonCalib::MdtCalibDbAsciiTool')


*/

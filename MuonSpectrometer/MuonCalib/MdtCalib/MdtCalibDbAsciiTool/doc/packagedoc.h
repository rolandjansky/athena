/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibDbAsciiTool_page MdtCalibDbAsciiTool Package
@author  Domizia.Orestano@cern.ch 
                                                                                
@section MdtCalibDbAsciiTool_MdtCalibDbAsciiToolIntro Introduction
This Tool is used by MdtCalibrationDbSvc to retreive some default 
calibration constants (from ASCII files declared in jobOptions and from jobOptions), 
unpack them and fill the transient classes used by the service.
No IOV is provided for these classes which are intended to be valid for 
all the processed events.
For IOV based access to conditions DB please configure MdtCalibrationSvc to use
MdtCalibDbCoolStrTool instead.
                                                                                
@section MdtCalibDbAsciiTool_MdtCalibDbAsciiToolOverview Class Overview
The package contains a single Tool, 
MuonCalib::MdtCalibDbAsciiTool, inheriting from MuonCalib::IMdtCalibDB.
The Tool default options are set in the constructor

- a single Rt function
for all the MDT chambers taken from the file DC2_rt_asciiDb.dat (default
option for the RT_InputFiles property) 

- all T0s set to 800 ns 

In addition for specific studies on simulated data it is possible to alter the 
T0s by shifting them (T0Shift property in ns) and/or smearing them with a 
gaussian of given sigma (T0Spread property in ns) and to distort
the default Rt function by a maximum amount in mm (RTShift property).

For commissioning data (not for commissioning simulation) setting to 
true the doCommissioning property  sets all T0s to a default which is 150 ns
and enables the usage of the next set of options 

- BMLT0: sets all the BML chambers T0s to the desired value

- BOLT0: sets all the BOL chambers T0s to the desired value

- etc...for a large number of types of chambers currently being commissioned
(see the full list in the genConf/MdtCalibDbAsciiToolConf.py file, produced under the root directory of the package after building).

- RT_InputFiles: if more than one file is listed it is assumed that one 
file for each chamber under study will be provided. It is then necessary to 
set the penultimate number in the header (first line) 
of each file to the hash identifier 
of the chamber. To find out the hash identifier XXX of a chamber 
do a trial run with few events 
setting the OutputLevel to VERBOSE and look in the output for the string 
"Adding chamber " followed by "adding tubes at XXX". 
                                                                                
                                                                                
*/

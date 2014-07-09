/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage LArOnlDbPrep Package

This package provides tools for preparing calibration data for online use.


@author Hong Ma <hma@bnl.gov>
@author Hucheng Chen <chc@bnl.gov>

@section  Data
LArOnlCalibData : data structure for online calibration data.

@section  Tool 
LArOnlDbPrepTool : this tool reads the offline conditions data objects, and 
prepare the online calibration data in the form of LArOnlCalibData

@section JobOptions

LArOnlineDB_Server.py : This runs on the Athena side, as the server.
LArOnlineDB_Client.py : This is the jobOptions to be sent from client side, 
and it converts the data structure into xml format, to be used by dsp.
*/

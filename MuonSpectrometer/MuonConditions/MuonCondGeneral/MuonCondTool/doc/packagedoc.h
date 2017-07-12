/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonCondTool_page MuonCondTool Package
@author Monica.Verducci@cern.ch
                                                                                
@section MuonCondTool_MuonCondToolIntro Introduction
This package contains the implementation of various tools derived from the
abstract MuonCalib::MuonCondITool classes.
                                                                                
@section MuonCondTool_MuonCondToolOverview Class Overview
The MuonCondTool package contains the following Tools  
- MuonCalib::MuonDetectorStatusDbTool: access information on the tube status directly by COOL Database Folders
- MuonCalib::MuonAlignmentDbTool:  access information on A and B lines from COOL Database Folders

Both tools register a callback to update in the TDS the data objetcs
  virtual StatusCode loadParameters(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadAlignALine(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadAlignBLine(IOVSVC_CALLBACK_ARGS);
        
  virtual StatusCode loadParameterStatus(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode loadTubeStatus(IOVSVC_CALLBACK_ARGS);

                                                                        
                                                                                
*/

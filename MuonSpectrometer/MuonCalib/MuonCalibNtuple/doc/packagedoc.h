/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonCalibNtuple_page MuonCalibNtuple Package
@author Niels.Van.Eldik@cern.ch, Zdenko.Van.Kesteren@cern.ch
                                                                                
@section MuonCalibNtuple_MuonCalibNtupleIntro Introduction
                                                                                
Contains tools to create ntuples at various levels of the calibration.

@section MuonCalibNtuple_MuonCalibNtupleOverview Class Overview
The MuonCalibNtuple package provides various classes implementing ntuples and branches, together with various helper classes used by the above ones.
These classes are used by MuonCalib::PatternNtupleMaker (MuonCalibTools) and by MuonCalib::MdtCalibNtupleMakerTool (MdtCalibTools) to create the ntuple containing all patterns for a given event or an ntuple per calibration region which is filled with segments, respectively.
                                                            
For the content of the CalibrationNtuple, please refer to the CalibrationNtuple wiki. There, a table can be found with the information on the different variables on the different branches of the CalibrationNtuple. The separate XxxNtupleBranch.h files do not contain information on tree content. 
                    
The branches are declared in the MuonCalib::MuonCalibBranchNtuple class, steering the filling of the several branches.

For the content of the CalibrationNtuple, see <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/MuonCalibNuptleContent"><span>Calibration Ntuple wiki</span></a>

                                                                                
                                                                                
*/

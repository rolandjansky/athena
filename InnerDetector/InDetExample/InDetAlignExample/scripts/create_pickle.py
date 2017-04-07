#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# STEP 1
# accumulating step

import pickle
f=open('reco1_01.pickle', 'w')
dict={
'inputRAWData': [ "first#/castor/cern.ch/grid/atlas/atlasscratchdisk/data10_7TeV/RAW/other/data10_7TeV.00165591.calibration_IDTracks.daq.RAW/data10_7TeV.00165591.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0001.data" ], # has to be a list,
'inputAlignmentConstants': "inputAlignmentConstants#",
'outputTFile': "OutputTFileNumber1#/tmp/lacuesta/matrixTFile1.root",
'outputMonitoringFile': "outputMonitoringFile1#/tmp/lacuesta/monitoringFile1.root"}
pickle.dump(dict,f)
f.close()

f=open('reco1_02.pickle', 'w')
dict={
'inputRAWData': [ "first#/castor/cern.ch/grid/atlas/atlasscratchdisk/data10_7TeV/RAW/other/data10_7TeV.00165591.calibration_IDTracks.daq.RAW/data10_7TeV.00165591.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0002.data" ], # has to be a list
'inputAlignmentConstants': "inputAlignmentConstants#",
'outputTFile': "OutputTFileNumber2#/tmp/lacuesta/matrixTFile2.root", 
'outputMonitoringFile': "outputMonitoringFile2#/tmp/lacuesta/monitoringFile2.root"
}
pickle.dump(dict,f)
f.close()

# STEP2 

import pickle
f=open('solve1.pickle', 'w')
dict={
'inputTFiles': [ "matrixInputTFile1#/tmp/lacuesta/matrixTFile1.root", "matrixInputTFile2#/tmp/lacuesta/matrixTFile2.root" ], 
'inputAlignmentConstants': "inputAlignmentConstants#",
'outputAlignmentConstants': "outputAlignmentConstants#/tmp/lacuesta/outputConstants.root",
'outputAlignmentConstantsdb': "outputAlignmentConstants#/tmp/lacuesta/outputConstants.db"
}
pickle.dump(dict,f)
f.close()

f=open('merge1.pickle', 'w')
dict={
'inputMonitoringFiles': [ "OutputTFileNumber1#/tmp/lacuesta/matrixTFile1.root", "OutputTFileNumber2#/tmp/lacuesta/matrixTFile2.root" ], 
'outputMonitoringMergedFile': "monitoringname#TotalMonitoring.root"
}
pickle.dump(dict,f)
f.close()


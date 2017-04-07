#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# STEP 1
# accumulating step

import pickle
f=open('reco1_01.pickle', 'w')
dict={
'inputRAWFiles': [{'dsn': 'data15_cos.00252404.physics_IDCosmic.merge.RAW','lfn': 'data15_cos.00252404.physics_IDCosmic.merge.RAW._lb0112._SFO-ALL._0001.1','pfn': 'root://castoratlas//castor/cern.ch/grid/atlas/tzero/prod1/perm/data15_cos/physics_IDCosmic/00252404/data15_cos.00252404.physics_IDCosmic.merge.RAW/data15_cos.00252404.physics_IDCosmic.merge.RAW._lb0112._SFO-ALL._0001.1'} ], # has to be a list,
'inputAlignmentConstants': "inputAlignmentConstants#",
'events':500,
'outputTFile': "OutputTFileNumber1#./data15_cos.00252404.physics_IDCosmic.matrix.ROOT.Iter0._lb0112._SFO-ALL._0001.1",
'outputMonitoringFile': "outputMonitoringFile1#./data15_cos.00252404.physics_IDCosmic.mon.ROOT.Iter0._lb0112._SFO-ALL._0001.1",
'iteration':0}
pickle.dump(dict,f)
f.close()

## f=open('reco1_02.pickle', 'w')
## dict={
## 'inputRAWData': [ "first#/castor/cern.ch/grid/atlas/atlasscratchdisk/data10_7TeV/RAW/other/data10_7TeV.00165591.calibration_IDTracks.daq.RAW/data10_7TeV.00165591.calibration_IDTracks.daq.RAW._lb0000._SFO-10._0002.data" ], # has to be a list
## 'inputAlignmentConstants': "inputAlignmentConstants#",
## 'outputTFile': "OutputTFileNumber2#/tmp/lacuesta/matrixTFile2.root", 
## 'outputMonitoringFile': "outputMonitoringFile2#/tmp/lacuesta/monitoringFile2.root"
## }
## pickle.dump(dict,f)
## f.close()

## # STEP2 
import pickle
f=open('solve1.pickle', 'w')
dict={
'inputTFiles': [{'dsn': 'data15_cos.00252404.physics_IDCosmic.matrix.ROOT.Iter0','lfn': 'data15_cos.00252404.physics_IDCosmic.matrix.ROOT.Iter0._lb0112._SFO-ALL._0001.1','pfn':"data15_cos.00252404.physics_IDCosmic.matrix.ROOT.Iter0._lb0112._SFO-ALL._0001.1"}],#, {'pfn':"matrixTFile2.root"}], 
'inputAlignmentConstants': "inputAlignmentConstants#",
'iteration':0,
'outputAlignmentConstants': "dsname#./outputConstants.root",
'outputAlignmentConstantsdb': "dsname#./outputConstants.db",
'outputAlignFiles': "dsname#./outputConstants.txt"}
pickle.dump(dict,f)
f.close()

'''dict={
'inputTFiles': [{'pfn':"matrixTFile1.root"}], 
 'inputAlignmentConstants': "inputAlignmentConstants#",
 'outputAlignmentConstants': "dsname#./outputConstants.root",
 'outputAlignmentConstantsdb': "dsname#./outputConstants.db"
 }
'''
## f=open('merge1.pickle', 'w')
## dict={
## 'inputMonitoringFiles': [ "OutputTFileNumber1#/tmp/lacuesta/matrixTFile1.root", "OutputTFileNumber2#/tmp/lacuesta/matrixTFile2.root" ], 
## 'outputMonitoringMergedFile': "monitoringname#TotalMonitoring.root"
## }
## pickle.dump(dict,f)
## f.close()



f=open('merge1.pickle', 'w')
dict={
'inputMonitoringFiles': [{'dsn': 'data15_cos.00252404.physics_IDCosmic.mon.ROOT.Iter0','lfn': 'data15_cos.00252404.physics_IDCosmic.mon.ROOT.Iter0._lb0112._SFO-ALL._0001.1','pfn':"data15_cos.00252404.physics_IDCosmic.mon.ROOT.Iter0._lb0112._SFO-ALL._0001.1"}],#, "OutputTFileNumber2#monitoringFile2.root" ], 
'outputMonitoringMergedFile': "monitoringname#TotalMonitoring.root"
}
pickle.dump(dict,f)
f.close()


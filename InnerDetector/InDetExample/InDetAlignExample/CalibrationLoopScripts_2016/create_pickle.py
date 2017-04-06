#!/usr/bin/python


# STEP 1
# accumulating step

import pickle
f=open('reco1.pickle', 'w')
dict={'inputRAWFiles': ['data16_13TeV.00297041.calibration_IDTracks.merge.RAW#data16_13TeV.00297041.calibration_IDTracks.merge.RAW._lb0300._SFO-ALL._0001.1'],
 'iteration': 0,
 'outputMonitoringFile': 'data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0._lb0101._SFO-ALL._0001',
 'outputTFile': 'data16_13TeV.00297041.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0#data16_13TeV.00297041.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0._lb0300._SFO-ALL._0001','maxEvents':100 }
pickle.dump(dict,f)
f.close()

import pickle
f=open('reco1_3.pickle', 'w')
dict={'inputRAWFiles': ['data15_13TeV.00284484.calibration_IDTracks.merge.RAW#data15_13TeV.00284484.calibration_IDTracks.merge.RAW._lb0101._SFO-ALL._0001.1'],
 'iteration': 3,
 'outputMonitoringFile': 'data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter3.c0._lb0101._SFO-ALL._0001',
 'outputTFile': 'data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter3.c0._lb0101._SFO-ALL._0001','maxEvents\
':100 }
pickle.dump(dict,f)
f.close()


import pickle
f=open('reco2.pickle', 'w')
dict={'inputRAWFiles': ['data15_13TeV.00284484.calibration_IDTracks.merge.RAW#data15_13TeV.00284484.calibration_IDTracks.merge.RAW._lb0102._SFO-ALL._0001.1'],
 'iteration': 2,
 'outputMonitoringFile': 'data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0._lb0102._SFO-ALL._0001',
 'outputTFile': 'data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0._lb0102._SFO-ALL._0001','maxEvents\
':100 }
pickle.dump(dict,f)
f.close()

import pickle
f=open('reco0.pickle', 'w')
dict={'inputRAWFiles': ['data15_13TeV.00284484.calibration_IDTracks.merge.RAW#data15_13TeV.00284484.calibration_IDTracks.merge.RAW._lb0100._SFO-ALL._0001.1'],
 'iteration': 2,
 'outputMonitoringFile': 'data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0._lb0100._SFO-ALL._0001',
 'outputTFile': 'data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0._lb0100._SFO-ALL._0001','maxEvents\
':100 }
pickle.dump(dict,f)
f.close()

'''
import pickle
f=open('reco1_01.pickle', 'w')
dict={
'inputRAWFiles': [{'dsn': 'data15_hi.00286711.calibration_IDTracks.merge.RAW','lfn': 'data15_hi.00286711.calibration_IDTracks.merge.RAW/data15_hi.00286711.calibration_IDTracks.merge.RAW._lb0150._SFO-ALL._0001.1','pfn': '/eos/atlas/atlastier0/rucio/data15_hi/calibration_IDTracks/00286711/data15_hi.00286711.calibration_IDTracks.merge.RAW/data15_hi.00286711.calibration_IDTracks.merge.RAW._lb0150._SFO-ALL._0001.1'} ], # has to be a list,
'inputAlignmentConstants': "inputAlignmentConstants#",
'events':100,
'skipEvents':0,
'outputTFile': "OutputTFileNumber1#./data15_hi.00286711.calibration_IDTracks.matrix.ROOT._lb0150._SFO-ALL._0001.1",
'outputMonitoringFile': "outputMonitoringFile1#./data15_hi.00286711.calibration_IDTracks.mon.ROOT._lb0150._SFO-ALL._0001.1",
'iteration':0}
pickle.dump(dict,f)
f.close()
'''


## # STEP2 
import pickle

dict={'inputTFiles': ['data16_13TeV.00297041.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0#data16_13TeV.00297041.calibration_IDTracks.idalignreco.ROOT_MATRIX.Iter0.c0._lb0300._SFO-ALL._0001'],
 'iteration': 0,
 'outputAlignFiles': 'data15_13TeV.00284484.calibration_IDTracks.idalignsolve.TAR_ALIGNFILES.Iter2.c0#data15_13TeV.00284484.calibration_IDTracks.idalignsolve.TAR_ALIGNFILES.Iter2.c0._0001',
 'outputAlignmentConstants': 'data15_13TeV.00284484.calibration_IDTracks.idalignsolve.ROOT_CONSTANTS.Iter2.c0#data15_13TeV.00284484.calibration_IDTracks.idalignsolve.ROOT_CONSTANTS.Iter2.c0._0001',
 'outputAlignmentConstantsdb': 'data15_13TeV.00284484.calibration_IDTracks.idalignsolve.ROOT_DB.Iter2.c0#data15_13TeV.00284484.calibration_IDTracks.idalignsolve.ROOT_DB.Iter2.c0._0001', 'nLBtoSkip': 0}

f=open('solve1.pickle', 'w')
'''dict={
'inputTFiles': [{'dsn': 'data15_13TeV.00286711.calibration_IDTracks.matrix.ROOT',
'lfn':'data15_hi.00286711.calibration_IDTracks.matrix.ROOT._lb0150._SFO-ALL._0001.1','pfn':"data15_hi.00286711.calibration_IDTracks.matrix.ROOT._lb0150._SFO-ALL._0001.1"}], 
'inputAlignmentConstants': "inputAlignmentConstants#",
'iteration':0,
'outputAlignmentConstants': "dsname#./outputConstants.root",
'outputAlignmentConstantsdb': "dsname#./outputConstants.db",
'outputAlignFiles': "dsname#./outputConstants.txt"}'''
pickle.dump(dict,f)
f.close()

'''dict={
'inputTFiles': [{'pfn':"data15_hi.00286711.calibration_IDTracks.mon.ROOT._lb0150._SFO-ALL._0001.1"}], 
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

f=open('merge0.pickle', 'w')
dict={'inputMonitoringFiles': ['data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0._lb0101._SFO-ALL._0001',
                          'data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter0.c0._lb0100._SFO-ALL._0001'],
 'outputMonitoringMergedFile': 'data15_13TeV.00284484.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0#data15_13TeV.00284484.calibration_IDTracks.idalignmerge.ROOT_MON.Iter0.c0._0001'}
pickle.dump(dict,f)
f.close()

f=open('merge3.pickle', 'w')
dict={'inputMonitoringFiles': ['data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter3.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter3.c0._lb0101._SFO-ALL._0001',
                          'data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter3.c0#data15_13TeV.00284484.calibration_IDTracks.idalignreco.ROOT_MON.Iter3.c0._lb0100._SFO-ALL._0001'],
 'outputMonitoringMergedFile': 'data15_13TeV.00284484.calibration_IDTracks.idalignmerge.ROOT_MON.Iter3.c0#data15_13TeV.00284484.calibration_IDTracks.idalignmerge.ROOT_MON.Iter3.c0._0001'}
pickle.dump(dict,f)
f.close()

f=open('merge1.pickle', 'w')
dict={
'inputMonitoringFiles': [ {'pfn':"data15_hi.00286711.calibration_IDTracks.mon.ROOT._lb0150._SFO-ALL._0001.1"}],#, "OutputTFileNumber2#monitoringFile2.root" ], 
'outputMonitoringMergedFile': "monitoringname#TotalMonitoring.root"
}
pickle.dump(dict,f)
f.close()

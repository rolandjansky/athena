############################################################
# AnalysisMaster.py
# These are the master job options for running your analysis
# Include your own job options as shown below ensuring you
# This is essentially the same file as in AnalysisExamples
############################################################

# We use "jp" to pass the input file to the userAlgs jO file defined below
# if you want to run over multiple files locally, use glob as shown below

from AthenaCommon.AthenaCommonFlags import jobproperties as jp
#jp.AthenaCommonFlags.FilesInput = ['/afs/cern.ch/atlas/maxidisk/d49/AOD.191045._001937.pool.root.1']
#jp.AthenaCommonFlags.FilesInput = ['/tmp/hartj/valid1.108494.Pythia_directJpsimu2p5mu2p5.merge.AOD.e835_s1310_s1300_r3373_r3375_tid736382_00/AOD.736382._000002.pool.root.1']
#jp.AthenaCommonFlags.FilesInput = ['/tmp/hartj/2011/myAOD.pool.root']
#jp.AthenaCommonFlags.FilesInput = ['/tmp/hartj/2011/myESD.pool.root']
jp.AthenaCommonFlags.FilesInput = ['/tmp/hartj/2012/myESD.pool.root']
#jp.AthenaCommonFlags.FilesInput = ['/tmp/hartj/2012/myAOD.pool.root']


# to run over multiple input files
##################
#DATAPATH = '/afs/cern.ch/atlas/maxidisk/d49/'
#from glob import glob
#INPUT = glob(DATAPATH + 'AOD*.root*')
#print INPUT
#jp.AthenaCommonFlags.FilesInput = INPUT

##################
#
from RecExConfig.RecFlags import rec

jp.AthenaCommonFlags.EvtMax=-1 # number of event to process
#StatusCodeSvc.AbortOnError=False
#rec.abortOnUncheckedStatusCode=False
rec.abortOnUncheckedStatusCode.set_Value_and_Lock(False)

# include your algorithm job options here

rec.UserAlgs=[ "JpsiVal_topOptions.py" ]
#rec.UserAlgs=[ "ZeeZmmOnAODExample_jobOptions_NEW.py" ] 
#rec.UserAlgs=[ "ZtautauExample_jobOptions_NEW.py" ] 
#rec.UserAlgs=[ "ttbarExample_jobOptions_NEW.py" ] 
#rec.UserAlgs=[ "VFitZmmOnAOD_jobOptions_NEW.py" ]

# Output log setting; this is for the framework in general
# You may over-ride this in your job options for your algorithm
rec.OutputLevel = INFO

# Control the writing of your own n-tuple in the alg's job options
# The following line only turns off the standard CBNT made by RecExCommon.
# and has no bearing on the ntuple that you make in your user job Options file.
#
rec.doCBNT = False

# for analysis you don't need to write out anything
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)

# to turn off perfmon - avoids writing out big ntuples
rec.doPerfMon=False

## need these two flags to turn on lumiblockmetadatatool and output metadatastore
rec.doDPD=True
rec.doFileMetaData=True

# main jobOption - must always be included
include ("RecExCommon/RecExCommon_topOptions.py")


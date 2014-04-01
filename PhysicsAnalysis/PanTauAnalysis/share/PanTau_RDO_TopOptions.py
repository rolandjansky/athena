#example of personal topOptions
#
# to use it  
# RecExCommon_links.sh  # once in a given directory
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#



#//////////////////////////////////
#   Import modules
#//////////////////////////////////

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
from tauRec.tauRecFlags import jobproperties as tauRecJP

#//////////////////////////////////
#   Input Files
#//////////////////////////////////

if athenaCommonFlags.FilesInput.isDefault(): # check if not already set upstream
    print("FilesInput was set to default value. will use predefined dataset")
    
    #--------------------------------  R D O
    #ZTauTau --- 1000 Events
    athenaCommonFlags.FilesInput += ['/tmp/limbach/mc10_7TeV.106052.PythiaZtautau.digit.RDO.e574_s934_s946_d426_tid213812_00/RDO.213812._000001.pool.root.1']
    
    #J1 --- 100 Events
    #athenaCommonFlags.FilesInput += ['/tmp/limbach/mc10_7TeV.105010.J1_pythia_jetjet.digit.RDO.e574_s1019_d425_tid211879_00/RDO.211879._000005.pool.root.1']
    
    #--------------------------------  A O D
    #ZTauTau
    #athenaCommonFlags.FilesInput += ['/tmp/limbach/mc11_7TeV.106052.PythiaZtautau.recon.AOD.e825_s1356_s1353_r3043_tid644919_00/AOD.644919._000135.pool.root.1']

athenaCommonFlags.EvtMax=10  # number of events to process run on all file


#//////////////////////////////////
#   Switch on algs that are 
#   switched off by default
#//////////////////////////////////

tauRecJP.tauRecFlags.doPanTau.set_Value_and_Lock(True)



#//////////////////////////////////
#   Main jobOptions
#//////////////////////////////////


### NOTE ############################################
# include my own algorithm(s). These jobo will be included at the end of topSequence configuration
# rec.UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ]
# these lines will be executed at the end of topSequence configuration
# rec.UserExecs=[ "from MyPackage.myAlgConf import myAlg","topSequence+=myAlg()" ]
# these lines will be executed after RecExCommon_flags.py has been imported, last chance for flag modification
# note that flag locking mechanism has a better chance to yield consistent configuration
# see https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerFlags
# see https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
# rec.UserFlags=[ "AODFlags.FastSimulation=False" ] 


### Last Change to change Flags! ###############################################

rec.doESD.set_Value_and_Lock(True) # Input = RDO: True
rec.doWriteESD.set_Value_and_Lock(False) # Input = RDO: False

#rec.readAOD.set_Value_and_Lock(True)

rec.doAOD.set_Value_and_Lock(True) # Input = RDO: True
rec.doWriteAOD.set_Value_and_Lock(True) # Input = RDO: True
#rec.doWriteTAG.set_Value_and_Lock(False)
#rec.doCBNT.set_Value_and_Lock(False)

recAlgs.doMuonIDStandAlone.set_Value_and_Lock(False)
recAlgs.doMuonIDCombined.set_Value_and_Lock(False)
recAlgs.doMuidLowPt.set_Value_and_Lock(False)
recAlgs.doMuGirl.set_Value_and_Lock(False)
recAlgs.doStaco.set_Value_and_Lock(False)
recAlgs.doMuTag.set_Value_and_Lock(False)
recAlgs.doTileMuID.set_Value_and_Lock(False)

rec.doTrigger.set_Value_and_Lock(False) # disable trigger (maybe necessary if detectors switched of)

rec.doTruth.set_Value_and_Lock(True)

#rec.OutputLevel.set_Value_and_Lock(DEBUG)

#rec.UserAlgs=["PanTauAnalysis/TauIdAnalysis_DecisionGetter.py"]


include ("RecExCommon/RecExCommon_topOptions.py")

# user modifier of properties should come here
#topSequence.myAlg.someProperty=3.14


#//////////////////////////////////
#   Import base PanTau script
#//////////////////////////////////

include("PanTauAnalysis/TauIdAnalysis_DecisionGetter.py")
#   NOTE: This should then be moved inside RecExCommon_topOptions.py






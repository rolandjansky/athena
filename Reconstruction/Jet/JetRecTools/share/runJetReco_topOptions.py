#========================================================================================
# Run the JetTowerBuilder algorithm during reconstruction from ESD
#========================================================================================

#----------------------------------------
# Return a file list given a directory
#----------------------------------------

import os,commands

def getDirectoryList(path):
    cmd  = 'ls %s/' % (path)
    file = ["%s/%s" % (path,i) for i in commands.getoutput(cmd).split('\n')]
    return file

def getMixedDirectoryList(path):
    cmd  = 'ls %s/*.root*' % (path)
    file = ["%s" % (i) for i in commands.getoutput(cmd).split('\n')]
    return file    
    
def getCastorDirectoryList(path):
    cmd  = 'nsls %s/ ' % (path)
    file = ["rfio:%s/%s" % (path,i) for i in commands.getoutput(cmd).split('\n')]
    return files

#-------------------------------
# Set MessageService Output
#-------------------------------

if not ('OutputLevel' in dir()):
    OutputLevel = INFO
    
MessageSvc.debugLimit  = 1000000
MessageSvc.OutputLevel = OutputLevel
MessageSvc.Format      = "% F%48W%S%7W%R%T %0W%M"       
         
#-------------------------------
# Number of events to process
#-------------------------------

if not ('EvtMax' in dir()):
    EvtMax=5

#-------------------------------
# Input / output configuration
#-------------------------------

if ('Data' in dir()):
    if Data == "J2":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J2')
    elif Data == "J3":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J3_2')
    elif Data == "J1":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J1')
    elif Data == "J4":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J4') 
    elif Data == "J0":
        PoolRDOInput  = getDirectoryList('/u1/fizisist/J0')
else:
    Data = "J4"
    PoolRDOInput  = getDirectoryList('/u1/fizisist/J4')
 
OutputDir     = "/u1/fizisist/JetTowerBuilder/"
PoolESDOutput = "%s/jetTowerBuilder.%s.%devt.ESD.pool.root" % (OutputDir,Data,EvtMax)
PoolAODOutput = "%s/jetTowerBuilder.%s.%devt.AOD.pool.root" % (OutputDir,Data,EvtMax)
OutputNTUP    = "%s/jetTowerBuilder.%s.%devt.AAN.root" % (OutputDir,Data,EvtMax)

#-------------------------------
# Schedule private algorithms
#-------------------------------

UserAlgs = ["JetTowerBuilder_jobOptions.py"]

#-----------------
# Global Setup
#-----------------
# File I/O
doCBNT     = False
doWriteAOD = True
doWriteESD = True
doWriteTAG = False
doAOD      = True 
doESD      = True 
doTAG      = False
# Reco Algorithms
doEgamma       = False
doMuon         = False
doJet          = True
doTauRec       = False
doObjMissingET = False
doMuGirl       = False
doMoore        = False
doTruth        = True
doAtlfast      = False
doTrigger      = False

#--------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#--------------------------------------------

#--------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#--------------------------------------------------

#-------------------------------
# Function to load CBNT for jets
#-------------------------------

def schedule_CBNTJet(jetColl):
 
  from JetRec.JetRecConf import CBNTAA_Jet
  from JetRec.JetRecConf import CBNTAA_JetTool
  from JetRec.JetRecConf import CBNTAA_JetCompositionTool   
  
  Jet = CBNTAA_Jet("CBNT_"+jetColl)  
  Jet.InputJetCollection = jetColl  
  Jet.BlockTag = jetColl          
  
  #if JetDetailedCBNT:    
  JetCompTool = CBNTAA_JetCompositionTool("JetCompTool")    
  Jet.Tools += [JetCompTool]  
    
  from AthenaCommon.AlgSequence import AlgSequence
  topSequence = AlgSequence()  
  topSequence.CBNT_AthenaAware  += Jet

#-------------------------------
# Scheduling CBNTAA algorithms
#-------------------------------  
  
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

theApp.Dlls += ["JetRec"]
JetDetailedCBNT = True
#schedule_CBNTJet("Cone4TowerJets",JetDetailedCBNT)
schedule_CBNTJet("Cone4TowerJets")
schedule_CBNTJet("Cone7TowerJets")
schedule_CBNTJet("Cone4TopoJets")
schedule_CBNTJet("Cone7TopoJets")
schedule_CBNTJet("TowersFromClusters0Jets")

#JetDetailedCBNT = True
#include( "JetRec/Cone7TowerJet_CBNT_jobOptions.py" )
#include( "JetRec/Cone4TowerJet_CBNT_jobOptions.py" )
#include( "JetRec/Cone4TopoJet_CBNT_jobOptions.py" )
#include( "JetRec/Cone7TopoJet_CBNT_jobOptions.py" )
#include( "JetRec/Cone7TruthJet_CBNT_jobOptions.py" )
#include( "JetRec/Cone4TruthJet_CBNT_jobOptions.py" )

#--------------------------------------------------------------
# NTuple Options
#--------------------------------------------------------------

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='" + OutputNTUP + "' OPT='RECREATE'"]

from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream                      = AANTupleStream()
AANTupleStream.ExtraRefNames        = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName           = OutputNTUP
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel          = INFO


# This is the job options that does AOD based skimming and thinning
###############################################################
#
# Job options file to run PhotonFilter
#
# Authors: Davide Costanzo, Stathes Paganis, Sven Menke, Anastopoulos Christos 
#
# Leonardo Carminati
#
#==============================================================
#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------

import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

athenaCommonFlags.FilesInput=[
#"ESD.pool.root"
"data09_900GeV.00142165.physics_L1CaloEM.merge.DESD_COLLCAND.f196_m326._lb0023-lb0067.1"
]

#--------------------------------------------------------------
# Try AutoConfiguration : in this way you don't need to 
# specify geometry, conditions... 
#--------------------------------------------------------------

from RecExConfig.InputFilePeeker import inputFileSummary
from RecExConfig.AutoConfiguration import ConfigureFromListOfKeys
ConfigureFromListOfKeys (['ProjectName',
                         'BeamType',
                         'RealOrSim',
                         'DoTruth',
                         'LumiFlags',
                         'FieldAndGeo',
                         'ConditionsTag',
                         'InputType'])
svcMgr.EventSelector.InputCollections=athenaCommonFlags.FilesInput()
theApp.EvtMax = -1 

include("RecExCond/AllDet_detDescr.py")

#--------------------------------------------------------------
# Specify if you want to use GRL
#--------------------------------------------------------------

UseGRLSelection = True # use xml good lumi blocks input file
# GoodRunsListVec  = [ 'allruns_collisions.xml' ]
## Find the most up-to-date good run list(s) here:
## https://twiki.cern.ch/twiki/bin/view/AtlasProtected/GoodRunListsForData
## and include the file name(s) in GoodRunsListVec list. Example:
GoodRunsListVec  = [ 'allruns_collisions.xml' ]

import popen2
import sys
import commands
SkimLog = logging.getLogger( "SkimLog" ) 

if (UseGRLSelection) :
        SkimLog.info("'UseGRLSelection' was selected: Checking if file(s) in GoodRunsListVec are present...")
        import os.path
        for XMLFile in GoodRunsListVec:
                if (os.path.isfile(XMLFile)):
                        SkimLog.info("'"+XMLFile+"' exists in "+os.path.abspath("."))
                else:
                        SkimLog.warning(XMLFile+" DOES NOT exists in "+os.path.abspath("."))
                        SkimLog.warning("##################################################")
                        SkimLog.warning("Disabling GRL selection!")
                        SkimLog.warning("##################################################")
                        UseGRLSelection = False
                        break
        if (UseGRLSelection):
                from GoodRunsLists.GoodRunsListsConf import *
                ToolSvc += GoodRunsListSelectorTool()
                GoodRunsListSelectorTool.GoodRunsListVec = GoodRunsListVec
                GoodRunsListSelectorTool.OutputLevel     = DEBUG


from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
topSequence = AlgSequence()
topSequence += AthSequencer("ModSequence1")
theJob = topSequence.ModSequence1

if (UseGRLSelection==True) :
        from GoodRunsListsUser.GoodRunsListsUserConf import *
        from AthenaCommon.AlgSequence import AlgSequence,AthSequencer
        theJob += GRLTriggerSelectorAlg('GRLTriggerAlg1')
        print "#######################################################################################"
        print "Lumi block selection will be attempted, if XML file(s) are present..."
        print "#######################################################################################"

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
theApp.EvtMax = -1

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = 3

#--------------------------------------------------------------
# Import Trigger related stuff
#--------------------------------------------------------------

from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.configurationSourceList=['ds']

from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter() 

###############################
# Load thinning service
###############################

from AthenaServices.Configurables import ThinningSvc
svcMgr += ThinningSvc()
# the output stream(s) to apply thinning on
svcMgr.ThinningSvc.Streams += ['StreamDPD']

include( "PyAnalysisCore/InitPyAnalysisCore.py")
include( "DPDUtils/ThinningSvcWrapper_jobOptions.py" )

#==============================================================
#
# End of job options file
#
#==============================================================

###############################################################
#--------------------------------------------------------------
#---   Secondary Write portion  ----- 
#--------------------------------------------------------------
###############################################################

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
from OutputStreamAthenaPool.OutputStreamAthenaPoolConf import AthenaPoolOutputStreamTool
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
StreamDPD = MSMgr.NewStream("StreamESD","SkimmedThin.ESD.pool.root")

from DPDUtils.DPDUtilsConf import  DPDPhotonFilter
filAlg=DPDPhotonFilter("DPDPhotonFilter")

#--------------------------------------------------------------
# Set Photon Filter parameters
#--------------------------------------------------------------

filAlg.UseTriggerSelection=False
filAlg.TriggerChain=("EF_g20")
#filAlg.TriggerChain=("EF_2g20,EF_g20")
filAlg.PhotEtCut=0
filAlg.PhotEtaCut=2.5
filAlg.PhotType="loose"

theJob+=filAlg
print topSequence

StreamDPD.AcceptAlgs(["DPDPhotonFilter"])

#Write everything in the new AOD/ESD

StreamDPD.Stream.TakeItemsFromInput=True
StreamDPD.Stream_FH.TakeItemsFromInput=True
 


# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#--------------------------------------------------------------------------------------------------
# options
#--------------------------------------------------------------------------------------------------
doRunOnGrid=False
#doGRL=True
#myGRL='data10_7TeV.periodAllYear.xml'
#--------------------------------------------------------------------------------------------------
from GaudiKernel.GaudiHandles import *
from GaudiKernel.Proxy.Configurable import *
import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
#ServiceMgr.MessageSvc.defaultLimit  = 9999999

#from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
# see details at https://svnweb.cern.ch/trac/atlasoff/browser/DataQuality/GoodRunsListsUser/trunk/share/GoodRunsListsUser_default_data.py#L13
#seq = AthSequencer("AthMasterSeq") 

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import glob

#--------------------------------------------------------------------------------------------------
if not doRunOnGrid:
    #addPath = '/afs/cern.ch/user/r/rkwee/scratch0/Insitu15/run/grid/'
    #athenaCommonFlags.FilesInput = []    
    #include('/afs/cern.ch/user/r/rkwee/scratch0/jO/inputESDs.py')
    athenaCommonFlags.FilesInput = [
    '/home/bcm_data/data10_7TeV.00158548.physics_MinBias.recon.ESD.f273/data10_7TeV.00158548.physics_MinBias.recon.ESD.f273._lb0200._0001.1'
        ]

    nEvents = -1
else:
    athenaCommonFlags.FilesInput = glob.glob("*ESD*")
    #addPath = ''
    nEvents = -1
    
ServiceMgr.EventSelector.InputCollections=athenaCommonFlags.FilesInput()
#--------------------------------------------------------------------------------------------------
ServiceMgr.EventSelector.SkipEvents = 0
theApp.EvtMax = nEvents
#--------------------------------------------------------------------------------------------------
from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']

from RecExConfig.InputFilePeeker import inputFileSummary
from AthenaCommon.GlobalFlags import globalflags

iKeys = inputFileSummary.keys()

print 'content of inputFileSummary'
for k in iKeys:
    if 0:
        print k,':',inputFileSummary[k]
    
if inputFileSummary['evt_type'][0] == 'IS_DATA' :
    globalflags.DataSource  = 'data'
if inputFileSummary['evt_type'][0] == 'IS_SIMULATION' :
    globalflags.DataSource  = 'geant4'

globalflags.DetDescrVersion = inputFileSummary['geometry']
#--------------------------------------------------------------
# build rootfile name 
runnumber = str(inputFileSummary['run_number'][0])
streamName = inputFileSummary['stream_tags'][0]['stream_type'] + '_' + inputFileSummary['stream_tags'][0]['stream_name']
    
rootfilename = 'bcm_bkgword' 

#--------------------------------------------------------------
# set up all detector description description
include ("RecExCond/AllDet_detDescr.py")
include('TrkDetDescrSvc/AtlasTrackingGeometrySvc.py')

#--------------------------------------------------------------------------------------------------
# Trigger
#--------------------------------------------------------------------------------------------------
#from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
#tdt = Trig__TrigDecisionTool("TrigDecisionTool")
#ToolSvc += tdt
#tdt.OutputLevel = INFO
#ToolSvc.TrigDecisionTool.Navigation.OutputLevel = INFO
#
#from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
#cfg = TriggerConfigGetter('ReadPool')


# Athena output level
# (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)"
ServiceMgr.MessageSvc.OutputLevel = 3

#--------------------------------------------------------------------------------------------------
# GRL
#--------------------------------------------------------------------------------------------------
#if doGRL:
#    # from twiki: https://twiki.cern.ch/twiki/bin/viewauth/Atlas/GoodRunsListsTutorial
#
#    from GoodRunsLists.GoodRunsListsConf import *
#    ToolSvc += GoodRunsListSelectorTool()
#    GoodRunsListSelectorTool.GoodRunsListVec  = [ addPath+'grls/'+myGRL ]    
#    GoodRunsListSelectorTool.OutputLevel = DEBUG
#
#    from GoodRunsListsUser.GoodRunsListsUserConf import *
#    seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
#    #seq.GRLTriggerAlg1.GoodRunsListArray = [ myGRL.rstrip('.xml') ]
#    seq.GRLTriggerAlg1.GoodRunsListArray = [ 'minbias_goodLumi_7TeV' ]
#

#-------------------------------------------------------------------------------------------------
# BcmBkgWordSeq seetings
#--------------------------------------------------------------------------------------------------
from BCM_BackgroundAlgs.BCM_BackgroundAlgsConf import BcmCollisionTimeAlg
#BcmBkgWordSeq = BcmBkgWordAlg( "BcmBkgWordSeq" )
#BcmBkgWordSeq.OutputLevel = DEBUG

#like this one could steer DeclarePropertiy
#MbTriggerPerf.doWriteToDB = False
#MbTriggerPerf.DatabaseFile = "./Matrix_"+rootfilename+".root"
#MbTriggerPerf.ContainerName = ""
#MbTriggerPerf.TrigChainName = "mb"

#.................................................................................................
# Add this algorithm to the sequence
topSequence += BcmCollisionTimeAlg( "BCM_BackgroundAlgs" )
topSequence.BCM_BackgroundAlgs.OutputLevel = INFO

#---------------------------------------------------------------------------------------------
# setup TTree registration Service
# save ROOT histograms 
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='"+rootfilename+".root' OPT='RECREATE'"]
#---------------------------------------------------------------------------------------------
# Set output level threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
# May also be set algorithm by algorithm
#--------------------------------------------------------------------------------------------
# topSequence.example.OutputLevel = INFO
MessageSvc.debugLimit = 10000
MessageSvc.infoLimit = 1000
MessageSvc.warningLimit = 1000
#---------------------------------------------------------------------------------------------
# Event related parameters
#---------------------------------------------------------------------------------------------


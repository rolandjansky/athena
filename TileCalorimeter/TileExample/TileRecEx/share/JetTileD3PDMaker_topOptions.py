# $Id: JetTileD3PDMaker_topOptions.py 445872 2011-06-28 11:07:15Z scole $
#
# @file   JetD3PDMaker/share/
# @author Haifeng Li <Haifeng.Li@cern.ch>
# @date   Feb. 11, 2009
# @brief  Top Job Option for Jet D3PD production. 
#


###################################################################3
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# TD start
athenaCommonFlags.FilesInput= ["data11_7TeV.00180124.physics_JetTauEtmiss.recon.ESD.f368._lb0334._SFO-12._0002.1"]
#athenaCommonFlags.FilesInput= ["/localscratch2/davidek/data11_7TeV.00180124.physics_JetTauEtmiss.recon.ESD.f368/data11_7TeV.00180124.physics_JetTauEtmiss.recon.ESD.f368._lb0504._SFO-10._0001.1"]


###################################################################3
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'JetD3PD_moje.root'


###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1

# Example of changing D3PD maker flags.
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
# TD start
D3PDMakerFlags.DoTrigger = False
# TD end

###################################################################3
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
include ("RecExCommon/RecExCommon_topOptions.py")
rec.AutoConfiguration=['everything']
#########################################################
#
# Configure the goodrunslist selector tool
#
## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()
job += AthSequencer("ModSequence1")

from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool()
# GoodRunsListSelectorTool.GoodRunsListVec = [ 'GRL_162690.xml' ]
# GoodRunsListSelectorTool.GoodRunsListVec = [ 'data10_1658xx_sasha_grl.xml' ]
# GoodRunsListSelectorTool.GoodRunsListVec = [ 'GRL_jetetmiss_repro.xml' ]
# GoodRunsListSelectorTool.GoodRunsListVec = [ 'GRL_2011_timing.xml' ]
GoodRunsListSelectorTool.GoodRunsListVec = [ 'data11_7TeV.periodAllYear__JetEtMiss.xml' ]

from GoodRunsListsUser.GoodRunsListsUserConf import *
job.ModSequence1 += GRLTriggerSelectorAlg('GRLTriggerAlg1')
# job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = [ 'MyLBCollection' ]
# job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = [ 'jetetmiss_7TeV' ]
# job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = [ 'SMjet_nomuon_7TeV_combined' ]
#job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = [ 'TD_stable_beams' ]
job.ModSequence1.GRLTriggerAlg1.GoodRunsListArray = [ 'JetEtMiss' ]

###################################################################3
# Make the D3PD.
#

###################################################################
#### Include the stuff by Toshi
####
from JetRec.JetGetters import *
from JetRec.JetRecConf import *
from JetRec.JetRecFlags import jetFlags
from JetRec.JetRec_defaults import *
from JetCalibTools.JetCalibToolsConf import *
from JetCalibTools.MakeCalibSequences import *
from JetCalibTools.SetupJetCalibrators import doJetOriginCorrection

from QcdD3PDMaker.QcdBeamSpotD3PDObject                 import QcdBeamSpotD3PDObject

#Use the minPt value below to set a minimum threshold on jet Pt
make_StandardJetGetter('AntiKt',0.4,'Topo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:OFFSET_ORIGIN_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt4TopoEMNewJets')
#make_StandardJetGetter('AntiKt',0.6,'Topo', doEMCut=False, minPt=7*GeV, doCalib=True, calibName='EM:OFFSET_ORIGIN_ETAJES', addJetQualityMoments=True, writeMoments=True, outputCollectionName='AntiKt6TopoEMNewJets')

from JetD3PDMaker.JetTileD3PD import JetTileD3PD
from JetD3PDMaker.JetTileD3PDObject import *
from JetD3PDMaker import JetD3PDMakerConf
#Use the following options to control the input collection and region cut#########
JetTileD3PD.inputSGkey='AntiKt4TopoEMNewJets'
JetD3PDMakerConf.JetTileFilterAlg.jetEtaMin=-2.0
JetD3PDMakerConf.JetTileFilterAlg.jetEtaMax=2.0
JetD3PDMakerConf.JetTileFilterAlg.jetPhiMin=-4.0
JetD3PDMakerConf.JetTileFilterAlg.jetPhiMax=4.0
#Note: Cut limits are set as [min,max)

#JetD3PDMaker.JetTileCellFillerTool.OutputLevel = DEBUG
#JetD3PDMaker.JetTileFilterAlg.OutputLevel = DEBUG
#from TileConditions.TileCondToolConf import *
#############
alg = JetTileD3PD ( tupleFileOutput )

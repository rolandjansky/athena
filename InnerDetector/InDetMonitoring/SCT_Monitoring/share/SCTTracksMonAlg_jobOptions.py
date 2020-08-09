#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

'''@file SCTTracksMonAlg_jobOptions.py
@author Susumu Oda
@date 2019-10-03
@brief Old style configuration of SCTTracksMonAlg
'''

from AthenaMonitoring import AthMonitorCfgHelperOld
helper = AthMonitorCfgHelperOld(DQMonFlags, "SCTTracksMonitor")

from SCT_Monitoring.SCT_MonitoringConf import SCTTracksMonAlg
myMonAlg = helper.addAlgorithm(SCTTracksMonAlg, "SCTTracksMonAlg")

if jobproperties.Beam.beamType()=='collisions':
  from AthenaMonitoring.FilledBunchFilterTool import GetFilledBunchFilterTool
  myMonAlg.FilterTools += [GetFilledBunchFilterTool()]

myMonAlg.TriggerChain = ''

# Set InDetTrackSummaryTool to TrackSummaryTool of SCTTracksMonAlg
myMonAlg.TrackSummaryTool = InDetTrackSummaryTool

doTrigger = False
if globalflags.DataSource == "data":
  from RecExConfig.RecFlags import rec
  if rec.doTrigger():
    doTrigger = True
myMonAlg.doTrigger = doTrigger

# Add a generic monitoring tool (a "group" in old language). The returned 
# object here is the standard GenericMonitoringTool.
myMonGroup = helper.addGroup(
    myMonAlg,
    "SCTTracksMonitor",
    "SCT/GENERAL/"
)

# Configure histograms
regionNames = ["EndCapC", "Barrel", "EndCapA"]
s_triggerNames = ["RNDM", "BPTX", "L1CAL", "TGC", "RPC", "MBTS", "COSM", "Calib"]
N_TRIGGER_TYPES = len(s_triggerNames)

myMonGroup.defineHistogram(varname="trk_N", # ; means alias
                           type="TH1F",
                           title="Number of tracks"+";Number of tracks",
                           path="tracks", # path cannot be "".
                           xbins=400, xmin=0., xmax=4000.)
myMonGroup.defineHistogram(varname="trk_chi2", # ; means alias
                           type="TH1F",
                           title="Track #chi^{2} div ndf"+";Number of track #chi^{2}/NDF",
                           path="tracks", # path cannot be "".
                           xbins=150, xmin=0., xmax=150.)
myMonGroup.defineHistogram(varname="trk_d0", # ; means alias
                           type="TH1F",
                           title="Track d0"+";d0 [mm]",
                           path="tracks", # path cannot be "".
                           xbins=160, xmin=-40., xmax=40.)
myMonGroup.defineHistogram(varname="trk_z0", # ; means alias
                           type="TH1F",
                           title="Track z0"+";z0 [mm]",
                           path="tracks", # path cannot be "".
                           xbins=200, xmin=-200., xmax=200.)
myMonGroup.defineHistogram(varname="trk_phi", # ; means alias
                           type="TH1F",
                           title="Track Phi"+";#phi [rad]",
                           path="tracks", # path cannot be "".
                           xbins=160, xmin=-4., xmax=4.)
myMonGroup.defineHistogram(varname="trk_pt", # ; means alias
                           type="TH1F",
                           title="Track P_{T}"+";P_{T} [GeV]",
                           path="tracks", # path cannot be "".
                           xbins=150, xmin=0., xmax=150.)

myMonGroup.defineHistogram(varname="trk_sct_hits", # ; means alias
                           type="TH1F",
                           title="SCT HITS per single Track"+";Num of Hits",
                           path="tracks", # path cannot be "".
                           xbins=sctMon.N_HIT_BINS, xmin=sctMon.FIRST_HIT_BIN, xmax=sctMon.LAST_HIT_BIN)
myMonGroup.defineHistogram(varname="trk_eta", # ; means alias
                           type="TH1F",
                           title="Track Eta"+";#eta",
                           path="tracks", # path cannot be "".
                           xbins=160, xmin=-4., xmax=4.)
                                           
myMonGroup.defineHistogram(varname="trackTriggers", # ; means alias
                           type="TH1I",
                           title="Tracks for different trigger types",
                           path="tracks", # path cannot be "".
                           xbins=N_TRIGGER_TYPES, xmin=-0.5, xmax=7.5, xlabels=s_triggerNames)
myMonGroup.defineHistogram(varname="region"+","+"hitsRegion"+";"+"SCTTrackRate", # ; means alias
                           type="TProfile",
                           title="Track per event for SCT regions",
                           path="tracks", # path cannot be "".
                           xbins=3, xmin=0.0, xmax=3.0, xlabels=regionNames)
myMonGroup.defineHistogram(varname="tracksPerRegion", # ; means alias
                           type="TH1F",
                           title="Number of tracks in eta regions",
                           path="tracks", # path cannot be "".
                           xbins=sctMon.N_REGIONS, xmin=0, xmax=sctMon.N_REGIONS, xlabels=regionNames)


for region in regionNames:
    myMonGroup.defineHistogram(varname="total"+region+"Residual",
                               title="Overall Residual Distribution for the "+region+";Residual [mm]",
                               type="TH1F", path="tracks",
                               xbins=100, xmin=-0.5, xmax=0.5)
    myMonGroup.defineHistogram(varname="total"+region+"Pull",
                               title="Overall Pull Distribution for the "+region+";Pull",
                               type="TH1F", path="tracks",
                               xbins=100, xmin=-5, xmax=5)
topSequence += helper.result()

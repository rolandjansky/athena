# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from RecExConfig.RecFlags  import rec

import traceback
import EventKernel.ParticleDataType

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.Include import include

from AthenaCommon.AlgSequence import AlgSequence,AthSequencer

from AthenaCommon.Constants import WARNING, INFO, ERROR, DEBUG

def LowPtJetFinder(seq=AlgSequence()):

    # first, check to see if we've already added the low pT jets to the sequence
    for i in seq:
        if i.getName() == "Kt5TopoJets":
            print "Kt5TopoJets already exist in this job - not configuring again."
            return
    
    from JetRec import JetGetters, JetRecConf
        
    finalT = JetRecConf.JetSignalSelectorTool('PAUJetFinalPtCut')
    finalT.UseTransverseEnergy   = True
    finalT.MinimumSignal         = 0.0

    
    from JetRec.JetGetters import JetGetterBadInput
    try:
        kt5topojets  = JetGetters.make_StandardJetAlg('Kt',0.5,'Topo',
                                                      doPostProcess = False,
                                                      doCalib = False,
                                                      jetMoments = [],
                                                      seq=seq)
    except JetGetterBadInput:
        print "Cannot configure kt5TopoJets getter - disabling."
        return
    
    finder = kt5topojets.AlgTools['JetFastJetFinder']
    finder.FastJetInterfaceTool.CalculateJetArea = True
    from PhotonAnalysisUtils.PhotonAnalysisUtilsFlags import PAUflags
    if PAUflags.UseVoronoiArea():
        finder.FastJetInterfaceTool.JetAreaMethod = 'VoronoiArea'


    if not PAUflags.UseRealData() and rec.doTruth() and PAUflags.FindLowPtTruthJets():
        try:
            mygetter3 = JetGetters.make_StandardJetAlg('Kt', .5, 'Truth',seq=seq)
        except Exception, msg:
            print "Bad input to JetGetter!  Not running jet finding."
            return

        mygetter3.AlgTools['JetFinalPtCut'].MinimumSignal = 0.
        mygetter3.AlgTools['JetFinalPtCut'].UseTransverseEnergy = True
        mygetter3.AlgTools['JetFastJetFinder'].FastJetInterfaceTool.CalculateJetArea = True

        if PAUflags.UseVoronoiArea():
            mygetter3.AlgTools['JetFastJetFinder'].FastJetInterfaceTool.JetAreaMethod = 'VoronoiArea'
    
    return kt5topojets

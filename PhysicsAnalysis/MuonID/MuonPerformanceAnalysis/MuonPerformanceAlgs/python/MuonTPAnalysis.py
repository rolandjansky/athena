# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# -*- coding: utf-8 -*-

#
# Massimiliano Bellomo
# June 2015
#
# Example job option fragment for running tag-and-probe tools in Athena from xAOD in Reco_tf
#

def MuonTPConfig(outputFilename,
                 doPlots = False,
                 doEff = False,
                 doClosure = False,
                 GRL = '',
                 IncludeMinimalCutTree=False):
    
    
    # turn off remaking of truth
    print "I was given an outputfilename of ",outputFilename
    from RecExConfig.RecFlags  import rec
    rec.doTruth.set_Value_and_Lock(False)
    from MuonPerformanceAlgs import MuonTPFlags
    from MuonPerformanceAlgs.MuonTPFlags import MuonTPFlags
    from AthenaCommon.JobProperties import jobproperties

    mtf = jobproperties.MuonTPFlags
    mtf.outputFilename = outputFilename
    mtf.doPlots = doPlots
    mtf.doEff = doEff
    mtf.GRL = GRL
    mtf.IncludeMinimalCutTree = IncludeMinimalCutTree

    rec.UserAlgs += ["MuonPerformanceAlgs/MuonTP_AnalysisAlgs.py"]


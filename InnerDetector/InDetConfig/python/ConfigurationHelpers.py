"""Disable everything but track reconstruction

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

def OnlyTrackingPreInclude(flags):
    """
    This will manually disable everything except for tracking.
    The purpose is to speed up overall execution by only running 
    tracking and nothing else
    """
    flags.Reco.EnableBTagging=False
    flags.Reco.EnableCombinedMuon=False
    flags.Reco.EnableEgamma=False
    flags.Reco.EnableJet=False
    flags.Reco.EnableTau=False
    flags.Reco.EnablePFlow=False
    flags.Reco.EnableTracking=True

    flags.Detector.EnableCalo=False
    flags.Detector.EnableMuon=False
    flags.Detector.EnableForward=False

    flags.Detector.GeometryLAr=False
    flags.Detector.GeometryTile=False
    flags.Detector.GeometryMBTS=False
    flags.Detector.GeometryCalo=False

    flags.Detector.GeometryCSC=False
    flags.Detector.GeometryMDT=False
    flags.Detector.GeometryRPC=False
    flags.Detector.GeometryTGC=False
    flags.Detector.GeometrysTGC=False
    flags.Detector.GeometryMM=False
    flags.Detector.GeometryMuon=False

    flags.Detector.GeometryLucid=False
    flags.Detector.GeometryZDC=False
    flags.Detector.GeometryALFA=False
    flags.Detector.GeometryAFP=False
    flags.Detector.GeometryFwdRegion=False
    flags.Detector.GeometryForward=False
    flags.Detector.GeometryForward=False

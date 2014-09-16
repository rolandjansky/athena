# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags

# import the base class
from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalTool
            
def LArAutoCorrTotalToolDefault (name="LArAutoCorrTotalToolDefault", **kw):
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArAutoCorrTotalTool(**kw)

    mlog = logging.getLogger( 'LArAutoCorrTotalToolDefault::__init__ ' )
    mlog.info("entering")

    # do the configuration
    if globalflags.DataSource()=='data':
        tool.MCSym = False
        tool.IsMC  = False
        tool.keyPedestal = "Pedestal"
    else:
        tool.MCSym = True
        tool.IsMC  = True


    from AthenaCommon.BeamFlags import jobproperties
    tool.deltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)                

    from LArROD.LArRODFlags import larRODFlags
    if larRODFlags.doOFCMixedOptimization():
        tool.UseMixedOFCOpt = True
        tool.NMinBias=jobproperties.Beam.numberOfCollisions()
        mlog.info("  setup for  Ncollisions %f   deltaBunch %f and OFC *MIXED* optimization" % (jobproperties.Beam.numberOfCollisions(), jobproperties.Beam.bunchSpacing()))
    else:        
        if larRODFlags.doOFCPileupOptimization():
            if larRODFlags.NumberOfCollisions():
                tool.NMinBias=larRODFlags.NumberOfCollisions()
                mlog.info("  setup for  Ncollisions %f   deltaBunch %f" % (larRODFlags.NumberOfCollisions(), jobproperties.Beam.bunchSpacing()))
            else:
                tool.NMinBias=jobproperties.Beam.numberOfCollisions()
                mlog.info("  setup for  Ncollisions %f   deltaBunch %f" % (jobproperties.Beam.numberOfCollisions(), jobproperties.Beam.bunchSpacing()))
        else:
            tool.NMinBias=0
            mlog.info("  setup for computing total noise autocorrelation without pileup")


    tool.NSamples = larRODFlags.nSamples()
    tool.firstSample = larRODFlags.firstSample()

    from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
    theADC2MeVTool = LArADC2MeVToolDefault()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theADC2MeVTool
    tool.ADC2MeVTool = theADC2MeVTool
    return tool

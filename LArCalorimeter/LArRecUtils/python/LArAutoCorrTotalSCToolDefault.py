# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.DetFlags import DetFlags

# import the base class
from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalTool
            
def LArAutoCorrTotalSCToolDefault (name="LArAutoCorrTotalSCToolDefault", **kw):
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArAutoCorrTotalTool(**kw)

    mlog = logging.getLogger( 'LArAutoCorrTotalSCToolDefault::__init__ ' )
    mlog.info("entering")

    tool.IsSuperCell = True
    # do the configuration
    if globalflags.DataSource()=='data':
        #return False
        tool.MCSym = False
        tool.IsMC  = False
    else:
        tool.MCSym = False
        tool.IsMC  = True
        tool.keyAutoCorr = "LArAutoCorrSC"
        tool.keyShape = "LArShapeSC"
        tool.keyNoise = "LArNoiseSC"
        tool.keyPedestal = "LArPedestalSC"
        tool.keyfSampl = "LARfSamplSC"

    from AthenaCommon.BeamFlags import jobproperties
    tool.deltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)                

    from LArROD.LArRODFlags import larRODFlags
    if larRODFlags.doOFCMixedOptimization():
        tool.UseMixedOFCOpt = True
        tool.NMinBias=jobproperties.Beam.numberOfCollisions()
        print 'Number of Ncollisions : ',jobproperties.Beam.numberOfCollisions()
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

    from LArRecUtils.LArADC2MeVSCToolDefault import LArADC2MeVSCToolDefault
    theADC2MeVTool = LArADC2MeVSCToolDefault()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theADC2MeVTool
    tool.ADC2MeVTool = theADC2MeVTool
    if not globalflags.DataSource()=='data':
       from IOVDbSvc.CondDB import conddb
       if ( conddb.isMC and not conddb.folderRequested('/LAR/IdentifierOfl/OnOffIdMap_SC') ) :
           conddb.addFolder("LAR_OFL","<tag>LARIdentifierOflOnOffIdMap_SC-000</tag>/LAR/IdentifierOfl/OnOffIdMap_SC")
       if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/fSampl') ) :
           conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCfSampl-000</tag>/LAR/ElecCalibMCSC/fSampl")
       if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Pedestal') ) :
           conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCPedestal-000</tag>/LAR/ElecCalibMCSC/Pedestal")
       if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Noise') ) :
           conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCNoise-000</tag>/LAR/ElecCalibMCSC/Noise")
       if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/Shape') ) :
           conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCShape-000</tag>/LAR/ElecCalibMCSC/Shape")
       if ( conddb.isMC and not conddb.folderRequested('/LAR/ElecCalibMCSC/AutoCorr') ) :
           conddb.addFolder("LAR_OFL","<tag>LARElecCalibMCSCAutoCorr-000</tag>/LAR/ElecCalibMCSC/AutoCorr")
    return tool

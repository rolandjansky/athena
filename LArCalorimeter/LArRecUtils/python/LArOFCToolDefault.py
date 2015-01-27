# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags

# import the base class
from LArRecUtils.LArRecUtilsConf import LArOFCTool
            
def LArOFCToolDefault (name="LArOFCToolDefault", **kw): 
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArOFCTool (**kw)

    mlog = logging.getLogger( 'LArOFCToolDefault::__init__ ' )
    mlog.info("entering")

    # do the configuration
    if globalflags.DataSource()=='data':
        tool.MCSym = False
        tool.IsMC  = False
        tool.FromDatabase = True
    else:
        tool.MCSym = True
        tool.IsMC  = True
        tool.FromDatabase = False
        from LArROD.LArRODFlags import larRODFlags

        tool.firstSample = larRODFlags.firstSample()

        tool.useHighestGainAutoCorr = larRODFlags.useHighestGainAutoCorr()
      
        if larRODFlags.doOFCMixedOptimization(): # kept for backward compatibility

            tool.UseDelta = 1 # only EMECIW/HEC/FCAL
            from AthenaCommon.BeamFlags import jobproperties
            tool.DeltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)
            mlog.info("  OFC *MIXED* optimization")

        else:

            tool.UseDelta = larRODFlags.UseDelta()

            if ( larRODFlags.UseDelta() == 0 ):
                mlog.info("  Standard OFC optimization computation")

            elif ( larRODFlags.UseDelta() == 1 ):
                mlog.info("  OFC optimization asking for no average shift as extra constraint only in EMECIW/HEC/FCAL")
                from AthenaCommon.BeamFlags import jobproperties
                tool.DeltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)

            elif ( larRODFlags.UseDelta() == 2 ):
                mlog.info("  OFC optimization asking for no average shift as extra constraint everywhere")
                from AthenaCommon.BeamFlags import jobproperties
                tool.DeltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)

            elif ( larRODFlags.UseDelta() == 3 ):
                mlog.info("  OFC optimization asking for no average shift as extra constraint only in EMECIW/HEC/FCAL1+high eta FCAL2-3")
                from AthenaCommon.BeamFlags import jobproperties
                tool.DeltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)
                
            else:
                tool.UseDelta = 0 ### avoid unforseed options

        ###
        #tool.Dump = True
        
    from LArRecUtils.LArAutoCorrTotalToolDefault import LArAutoCorrTotalToolDefault
    theAutoCorrTool = LArAutoCorrTotalToolDefault()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theAutoCorrTool
    tool.AutoCorrTool = theAutoCorrTool
    return tool
    

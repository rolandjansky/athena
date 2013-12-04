# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################################################
#
#  TileSimInfoConfigurator class  
#
#  used to set up the TileSimInfoLoader service
#
# Author: sanya.solodkov@cern.ch
#############################################################

from TileSimUtils.TileSimUtilsConf import TileSimInfoLoader

class TileSimInfoConfigurator( TileSimInfoLoader ):
    """
    Some class description should go here...
    """
    __slots__ = [ '_msg' ]

    #_______________________________________________________________
    def __init__(self, name="TileSimInfoConfigurator", **kwargs):

        #=== set the single service name
        svcName = "TileSimInfoLoader"

        #=== call baseclass constructor
        TileSimInfoLoader.__init__(self, svcName, **kwargs)

        #=== initialize logger with INFO level
        from AthenaCommon.Logging import logging
        self._msg = logging.getLogger( 'TileSimInfoConfigurator' )
        self._msg.setLevel(3)

        #=== add self to ServiceMgr
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr( svcMgr , svcName ):
            svcMgr += self

        #=== ensure the creation of service
        from AthenaCommon.AppMgr import theApp
        if not svcName in theApp.CreateSvc:
            theApp.CreateSvc += [ svcName ]

        #=== check for geometry version
        from AthenaCommon.GlobalFlags import jobproperties
        detDescrVersion = jobproperties.Global.DetDescrVersion()

        if (detDescrVersion.startswith("ctb") or
            detDescrVersion.startswith("CTB") or
            detDescrVersion.startswith("ATLAS-CTB") ) :

            self.TileTB = True
            self.DeltaTHit = [ 1. ]
            self.TimeCut = 200.5
            self.DoTOFCorrection = True

        elif (detDescrVersion.startswith("comm") or
              detDescrVersion.startswith("Comm") or
              detDescrVersion.startswith("ATLAS-Comm") ) :

            self.DeltaTHit = [ 1. ]
            self.DoTOFCorrection = False

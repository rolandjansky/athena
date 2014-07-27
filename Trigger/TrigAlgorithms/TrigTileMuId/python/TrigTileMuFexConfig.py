# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#from TrigTileMuId.TrigTileMuIdConf import TrigTileMuFex
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from TrigTileMuId.TrigTileMuIdMonitoring import *

#from AthenaCommon.AppMgr import *
try:
    from TrigTileMuId.TrigTileMuIdConf import TrigTileMuFex
except:
    mlog.error("could not load TrigTileMuIdConf")
    import traceback
    print traceback.format_exc()


class TrigTileMuFexConfig (TrigTileMuFex):
    __slot__ = []

    def __new__( cls, *args, **kwargs ):

       if len(args) == 2:
         newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1])] + list(args)
       else:
         newargs = ['%s_%s' % (cls.getType(),args[0]) ] + list(args)

       return super( TrigTileMuFexConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__(self, name, *args, **kwargs ):
        super( TrigTileMuFexConfig, self ).__init__( name )

        TrigTileMuFex.__init__(self, name)

        self.UseAthenaFieldService = True

        self.IDalgo = "IDSCAN" 
        if len(args) == 2:
          TrackAlgo = args[1]
          if (TrackAlgo=='SITRACK'):
            self.IDalgo = "SITRACK"
          if (TrackAlgo=='TRTXK'):
            self.IDalgo = "TRTXK"
          if (TrackAlgo=='TRTSEG'):
            self.IDalgo = "TRTSEG"

        #self.IDalgo="SITRACK"		# After "TrigSiTrack_Muon"
        self.Meth_ExtrapTileR = "DirectPars"
        #self.Meth_ExtrapTileR = "EMextraWay"
        self.DelPhi_Cut = 0.2
        self.DelEta_Cut = 0.1
        self.Pt_Cut = 2000.0
                        # Unit(Pt) : MeV

        self.GetTruthMuon = False
        #self.GetTruthMuon = True 

        validation = TrigTileMuFexValidationMonitoring()
        online     = TrigTileMuFexOnlineMonitoring()
        cosmic     = TrigTileMuFexCosmicMonitoring()

        from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 70]
 
        self.AthenaMonTools = [ validation, online, cosmic, time ]  

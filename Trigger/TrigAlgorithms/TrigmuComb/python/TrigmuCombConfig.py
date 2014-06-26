# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigmuComb.TrigmuCombConf import *
from TrigmuComb.TrigmuCombMonitoring import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ServiceMgr
from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConfig import *
from AthenaCommon.AppMgr import ToolSvc
#from CombinedMuonRefit.CombinedMuonRefitConf import ExtrapolateMuonToIPTool

ToolSvc += MuonBackExtrapolatorForAlignedDet()
ToolSvc += MuonBackExtrapolatorForMisalignedDet()
ToolSvc += MuonBackExtrapolatorForData()
#ToolSvc += ExtrapolateMuonToIPTool()

class TrigmuCombConfig (muComb):

    __slots__ = []
    
    def __new__( cls, *args, **kwargs ):
        newargs = ['%s_%s_%s' % (cls.getType(),args[0],args[1]) ] + list(args)
        return super( TrigmuCombConfig, cls ).__new__( cls, *newargs, **kwargs )

    def __init__( self, name, *args, **kwargs ):
        super( TrigmuCombConfig, self ).__init__( name )

        self.UseAthenaFieldService = True
        self.MuCombStrategy = 0
        self.UseBackExtrapolatorG4 = False
        self.MinPtTRK = 0.

        if ( args[0] == '900GeV' ):
            self.WinPt    = [4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
        else:		
            self.WinPt    = [6.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
                
        if globalflags.DetDescrVersion().find('CSC')!=-1:
            self.MuFastBarrelRes  = [0.03287,0.002487,3.5,-1.8,0.35,-0.017]
            self.MuFastEndcap1Res = [0.133,0.0001445,77.0,-47.0,9.8,-0.67]
            self.MuFastEndcap2Res = [0.203,0.002034,10.4,-5.21,1.14,-0.056]
            self.MuFastEndcap3Res = [0.08878,0.002208,98.0,-60.0,12.0,-0.80]
            self.MuFastEndcap4Res = [0.07195,0.001701,101.0,-61.0,12.0,-0.80]

            self.IDSCANBarrelRes  = [0.02169,0.0004186]
            self.IDSCANEndcap1Res = [0.03054,0.000325]
            self.IDSCANEndcap2Res = [0.03557,0.0005383]
            self.IDSCANEndcap3Res = [0.04755,0.0007718]
            self.IDSCANEndcap4Res = [0.06035,0.0001145]


        # Default backextrapolator is for MC Misaligned Detector
        if ( muonByteStreamFlags.RpcDataType()=='atlas' ):
            self.BackExtrapolatorLUT = MuonBackExtrapolatorForMisalignedDet()
        else:
            self.BackExtrapolatorLUT = MuonBackExtrapolatorForAlignedDet()	    

        #if globalflags.DataSource == 'data':
        #    self.BackExtrapolatorLUT = MuonBackExtrapolatorForData()
	    
        try:
            if ( args[1] in ['IDSCAN', 'SITRACK', 'TRTXK', 'TRTSEG', 'STRATEGY_A', 'STRATEGY_B', 'STRATEGY_C', 'STRATEGY_F', 'TrigFastTrackFinder_Muon'] ):
                #self.IDalgo='TrigFastTrackFinder_Muon'
                self.IDalgo=args[1]
        except IndexError:
            pass
	    
        validation = TrigmuCombValidationMonitoring()
        online     = TrigmuCombOnlineMonitoring()
        cosmic     = TrigmuCombCosmicMonitoring()
	
        time = TrigTimeHistToolConfig("Time")
        time.TimerHistLimits = [0, 5]
	
        self.AthenaMonTools = [ validation, online, cosmic, time ]

        def setDefaults(cls,handle):
            if hasattr(handle,'BackExtrapolatorLUT'):
                if handle.BackExtrapolatorLUT.name().find("AlignedBackExtrapolator")!=-1:
                    print self.name," using BackExtrapolatorLUT for Aligned Detector"
                if handle.BackExtrapolatorLUT.name().find("MisalignedBackExtrapolator")!=-1:
                    print self.name," using BackExtrapolatorLUT for Misligned Detector" 
                if handle.BackExtrapolatorLUT.name().find("DataBackExtrapolator")!=-1:
                    print self.name," using BackExtrapolatorLUT for Data" 

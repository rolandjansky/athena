# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from MuonRecFlags import muonRecFlags
from MooreFlags   import mooreFlags
from MuonboyFlags import muonboyFlags
from MuonStandaloneFlags import muonStandaloneFlags

from ConfiguredMuonRec import ParallelMuonRec
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

muonRecFlags.setDefaults()
mooreFlags.setDefaults()
muonboyFlags.setDefaults()
muonStandaloneFlags.setDefaults()

class MuonRec(ParallelMuonRec):
    def __init__(self, **kwargs):
        # initialise
        ParallelMuonRec.__init__(self,"MuonRec",[],enabled=True,**kwargs)
        # add reco configs
        #self.addConfig( "MuonRecExample.Muonboy.Muonboy",       enabled = muonRecFlags.doMuonboy() )
        #self.addConfig( "MuonRecExample.Moore.Moore",           enabled = muonRecFlags.doMoore() )
        self.addConfig( "MuonRecExample.MuonStandalone.MuonStandalone", enabled = muonRecFlags.doStandalone() )
        # and configure
        self.configure()

#--------------------------------------------------------------------------
# Configure prepared muon reconstruction configurations
#--------------------------------------------------------------------------
muonRec = MuonRec(applyResilience=athenaCommonFlags.AllowIgnoreConfigError())

#--------------------------------------------------------------------------
# Print out full muon Reco config
#--------------------------------------------------------------------------
print muonRec


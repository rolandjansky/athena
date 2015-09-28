# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# AUTHOR:   Marcin.Wolter@cern.ch
# CREATED:  20 March 2008
#
# 23 Nov 2010: cleaning up (Noel Dawe)

from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import AlgSequence
#from AthenaCommon.AppMgr import ToolSvc

from RecExConfig.Configured import Configured
import traceback

def singleton(cls):

    log = logging.getLogger('%s::__init__'% cls.__name__)
    instances = {}
    def getinstance(*args, **kwargs):
        if cls in instances:
            log.warning("Attempting to construct more than one %s. Returning the singleton."% cls.__name__)
            return instances[cls]
        obj = cls(*args, **kwargs)
        instances[cls] = obj
        return obj
    return getinstance

# to disable singleton behaviour comment out the following line:
@singleton
class TauDiscriGetter(Configured):

    def __init__(self, name = "TauDiscriminant",
                       container = "TauJets",
                       sequence = None,
                       do_upstream_algs = False,
                       do_only_fakebits = False,
                       msglevel = 3):

        self.sequence = sequence
        if sequence is None:
            self.sequence = AlgSequence()
        self.name = name
        self.container = container
        self.upstream_algs = do_upstream_algs
        self.only_fakebits = do_only_fakebits # temporary hack
        self.msglevel = msglevel
        Configured.__init__(self)

    def configure(self):

        from AthenaCommon.AppMgr import ToolSvc

        mlog = logging.getLogger('TauDiscriGetter::configure:')

        try:
            from TauDiscriminant.TauDiscriminantConf import TauDiscriBuilder
            TauDiscriBuilder.OutputLevel = self.msglevel
            tauDiscriBuilder = TauDiscriBuilder(
                self.name,
		        container = self.container)
        except Exception:
            mlog.error("could not find TauDiscriBuilder")
            print traceback.format_exc()
            return False

        tools = []

        if not self.only_fakebits:
            """
            ID variable calculator
            """
            try:
                from TauDiscriminant.TauDiscriminantConf import TauIDVarCalculator
                TauIDVarCalculator.OutputLevel = self.msglevel
                tauIDVarCalc = TauIDVarCalculator()
                tools.append(tauIDVarCalc)
            except Exception:
                mlog.error("could not find TauIDVarCalculator in TauDiscriminant")
                print traceback.format_exc()
                return False
            """
            Muon veto
            """
            try:
                from TauDiscriminant.TauDiscriminantConf import TauMuonVeto
                TauMuonVeto.OutputLevel = self.msglevel
                tauMuonVeto = TauMuonVeto()
                tools.append(tauMuonVeto)
            except Exception:
                mlog.error("could not find TauMuonVeto in TauDiscriminant")
                print traceback.format_exc()
                return False
            """
            BDT tau-jet identification
            """
            try:
                from TauDiscriminant.TauDiscriminantConf import TauJetBDT
                TauJetBDT.OutputLevel = self.msglevel
                taujetBDT = TauJetBDT(jetBDT="jet.BDT.bin",
                                      jetSigTrans="sig.trans.jet.BDT.root",
                                      jetBkgTrans="",
                                      jetSigBits="sig.bits.jet.BDT.txt",
                                      jetBkgBits="bkg.bits.jet.BDT.txt")
                tools.append(taujetBDT)
            except Exception:
                mlog.error("could not find TauJetBDT in TauDiscriminant")
                print traceback.format_exc()
                return False
            """
            BDT electron veto
            """
            try:
                from TauDiscriminant.TauDiscriminantConf import TauEleBDT
                TauEleBDT.OutputLevel = self.msglevel
                taueleBDT = TauEleBDT(eleBDT="ele.BDT.bin",
                                      eleBits="", eleBitsRoot="cuts.eBDT.root")
                tools.append(taueleBDT)
            except Exception:
                mlog.error("could not find TauEleBDT in TauDiscriminant")
                print traceback.format_exc()
                return False


        tauDiscriBuilder.tools = tools

        self.sequence += tauDiscriBuilder
        return True

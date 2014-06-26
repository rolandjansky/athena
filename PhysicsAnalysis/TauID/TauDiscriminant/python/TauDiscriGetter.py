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
                       container = "TauRecContainer",
                       sequence = None,
                       do_upstream_algs = False,
                       do_only_fakebits = False,
                       do_Pi0           = True, # Temporary, turns on the dumping of pi0 scores
                       msglevel = 3):

        self.sequence = sequence
        if sequence is None:
            self.sequence = AlgSequence()
        self.name = name
        self.container = container
        self.upstream_algs = do_upstream_algs
        self.only_fakebits = do_only_fakebits # temporary hack
        self.do_Pi0 = do_Pi0 # Temporary, turns on the dumping of pi0 scores
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
            Cut-based tau-jet identification
            """
#            try:
#                from TauDiscriminant.TauDiscriminantConf import TauCuts
#                TauCuts.OutputLevel = self.msglevel
#                tauCuts = TauCuts(cuts = "cuts.txt")
#                tools.append(tauCuts)
#            except Exception:
#                mlog.error("could not find TauCuts in TauDiscriminant")
#                print traceback.format_exc()
#                return False
            """
            Cut-based electron veto
            """
            try:
                from TauDiscriminant.TauDiscriminantConf import TauCutsEleVeto
                TauCutsEleVeto.OutputLevel = self.msglevel
                tauCutsEleVeto = TauCutsEleVeto()
                tools.append(tauCutsEleVeto)
            except Exception:
                mlog.error("could not find TauCutsEleVeto in TauDiscriminant")
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


            try:
                from TauDiscriminant.TauDiscriminantConf import TauPi0BDT
                TauPi0BDT.OutputLevel = self.msglevel
                taupi0BDT = TauPi0BDT(pi0BDTPrimary="pi0Primary.BDT.bin",
                                      pi0BDTSecondary="pi0Secondary.BDT.bin")
                tools.append(taupi0BDT)
            except Exception:
                mlog.error("could not find TauPi0BDT in TauDiscriminant")
                print traceback.format_exc()
                return False



            """
            Likelihood tau-jet identification
            """
            try:
                from TauDiscriminant.TauDiscriminantConf import TauLLH
                TauLLH.OutputLevel = self.msglevel
                tauLLH = TauLLH()
                tools.append(tauLLH)
            except Exception:
                mlog.error("could not find TauLLH in TauDiscriminant")
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

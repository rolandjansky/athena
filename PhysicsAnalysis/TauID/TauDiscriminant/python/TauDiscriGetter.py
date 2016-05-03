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


def getTauDiscriminantTools(mlog, only_fakebits=False):

    tools = []

    from AthenaCommon.AppMgr import ToolSvc
    print 'Fetching TauDiscriminantTools'

    if not only_fakebits:
        """
        ID variable calculator
        """
        try:
            from TauDiscriminant.TauDiscriminantConf import TauIDVarCalculator            
            #TauIDVarCalculator.OutputLevel = self.msglevel
            tauIDVarCalc = TauIDVarCalculator()
            tools.append(tauIDVarCalc)
        except :
            mlog.error("could not find TauIDVarCalculator in TauDiscriminant")
            return []
        """
        BDT tau-jet identification
        """
        try:
            from TauDiscriminant.TauDiscriminantConf import TauJetBDT
            #TauJetBDT.OutputLevel = self.msglevel
            taujetBDT = TauJetBDT(jetBDT="TauJetBDT2016Correct.bin",
                                  jetSigTrans="",
                                  jetSigBits="TauJetBDT2016FinalCuts_v1.txt")
            tools.append(taujetBDT)
        except :
            mlog.error("could not find TauJetBDT in TauDiscriminant")
            return []
        """
        BDT tau-jet flattener
        """
        try:
            from TauDiscriminant.TauDiscriminantConf import TauScoreFlatteningTool
            #TauScoreFlatteningTool.OutputLevel = self.msglevel
            tauscoreflatteningTool = TauScoreFlatteningTool(FlatteningFile="TauJetBDT2016FinalFlat_v1.root",
                                                            DefineWPs=False,
                                                            
                                                            SigEffWPVeryLoose1P=0.95,
                                                            SigEffWPLoose1P=0.85,
                                                            SigEffWPMedium1P=0.75,
                                                            SigEffWPTight1P=0.60,
                                                            
                                                            SigEffWPVeryLoose3P=0.95,
                                                            SigEffWPLoose3P=0.75,
                                                            SigEffWPMedium3P=0.60,
                                                            SigEffWPTight3P=0.45)

            tools.append(tauscoreflatteningTool)
            
        except :
            mlog.error("could not find TauScoreFlatteningTool in TauDiscriminant")
            return []
        """
        BDT electron veto
        """
        try:
            from TauDiscriminant.TauDiscriminantConf import TauEleBDT
            #TauEleBDT.OutputLevel = self.msglevel
            taueleBDT = TauEleBDT(eleBDT="ele.BDT.bin",
                                  eleBits="", eleBitsRoot="cuts.eBDT.root")
            tools.append(taueleBDT)
        except :
            mlog.error("could not find TauEleBDT in TauDiscriminant")
            return []
        """
        EleOLR decorator
        """
        try:
            from TauDiscriminant.TauDiscriminantConf import TauEleOLRDecorator
            #TauEleOLRDecorator.OutputLevel = self.msglevel
            taueleOLRdecorator = TauEleOLRDecorator(ElectronContainerName="Electrons",
                                                    EleOLRFile="2016EVeto.root")
            tools.append(taueleOLRdecorator)
        except :
            mlog.error("could not find TauEleOLRDecorator in TauDiscriminant")
            return []

        return tools

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

        tools = getTauDiscriminantTools(mlog, only_fakebits=do_only_fakebits)
            
        tauDiscriBuilder.tools = tools

        self.sequence += tauDiscriBuilder
        return True

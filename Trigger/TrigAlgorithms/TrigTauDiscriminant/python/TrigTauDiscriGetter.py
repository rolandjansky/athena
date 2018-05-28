# AUTHOR:   Marcin.Wolter@cern.ch
# CREATED:  20 March 2008
# 
# 23 Nov 2010: cleaning up (Noel Dawe)

from AthenaCommon.Logging import logging
from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

from AthenaCommon.AppMgr import ToolSvc

import traceback

from TrigTauDiscriminant.TrigTauDiscriminantConf import TrigTauDiscriBuilder
from TriggerJobOpts.TriggerFlags import TriggerFlags

import ROOT
from tauRec.tauRecFlags import tauFlags

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

#@singleton

class TrigTauDiscriGetter(TrigTauDiscriBuilder):
    __slots__ = [ '_mytools']
    def __init__(self, name = "TrigTauDiscriminant"):
         super( TrigTauDiscriGetter , self ).__init__( name )

         #monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
         from TrigTauDiscriminant.TrigTauDiscriminantMonitoring import TrigTauDiscriminantValidationMonitoring, TrigTauDiscriminantOnlineMonitoring 
         validation = TrigTauDiscriminantValidationMonitoring()        
         online     = TrigTauDiscriminantOnlineMonitoring()
         
         from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
         time = TrigTimeHistToolConfig("Time")
         
         self.AthenaMonTools = [ time, validation, online ]
         
         self.Tools = [self.VarCalculatorSet(), self.BDTtoolset()]
    

    def BDTtoolset(self):
        from TauDiscriminant.TauDiscriminantConf import TauJetBDT
        return TauJetBDT(inTrigger = True,
                         calibFolder = 'TrigTauRec/00-11-01/',
                         jetBDT = "trigger.jet.BDT.bin",
                         jetSigBits = "trigger.sig.bits.jet.BDT.txt")

    def VarCalculatorSet(self):
        from tauRecTools.tauRecToolsConf import TauIDVarCalculator
        tauVarCalc = TauIDVarCalculator()
        tauVarCalc.inTrigger = True
        tauVarCalc.calibFolder = 'TrigTauRec/00-11-01/'
        return tauVarCalc


class TrigTauDiscriGetter2015(TrigTauDiscriBuilder):
    __slots__ = [ '_mytools']
    def __init__(self, name = "TrigTauDiscriminant2015"):
         super( TrigTauDiscriGetter2015 , self ).__init__( name )

         #monitoring part. To switch off do in topOption TriggerFlags.enableMonitoring = []
         from TrigTauDiscriminant.TrigTauDiscriminantMonitoring import TrigTauDiscriminantValidationMonitoring, TrigTauDiscriminantOnlineMonitoring 
         validation = TrigTauDiscriminantValidationMonitoring()        
         online     = TrigTauDiscriminantOnlineMonitoring()
         
         from TrigTimeMonitor.TrigTimeHistToolConfig import TrigTimeHistToolConfig
         time = TrigTimeHistToolConfig("Time")
         
         self.AthenaMonTools = [ time, validation, online ]
         
         self.Tools = [self.VarCalculatorSet()] + self.BDTtoolset()
    

    def BDTtoolset(self):

        if TriggerFlags.run2Config == '2016':
            from TauDiscriminant.TauDiscriminantConf import TauJetBDT
            bdt_set = TauJetBDT(
                inTrigger = True,
                calibFolder = 'TrigTauRec/00-11-01/',
                jetBDT = "bdt.2016.bin",
                jetSigBits = "trigger.sig.bits.jet.BDT.txt")
            return [bdt_set]
        
        else:
            from tauRecTools.tauRecToolsConf import TauJetBDTEvaluator
            # BDT evaluators 1p / mp
            bdt_1p = TauJetBDTEvaluator(
                name = "TrigTauJetBDT1P", 
                calibFolder='tauRecTools/00-02-00/',
                weightsFile='vars2016_pt_gamma_1p_isofix.root', 
                inTrigger=True,
                minNTracks=0, maxNTracks=1)
            bdt_mp = TauJetBDTEvaluator(
                name = "TrigTauJetBDTMP", 
                calibFolder='tauRecTools/00-02-00/',
                weightsFile='vars2016_pt_gamma_3p_isofix.root', 
                inTrigger=True,
                minNTracks=2, maxNTracks=1000)
            
            import PyUtils.RootUtils as ru
            ROOT = ru.import_root()
            import cppyy
            cppyy.loadDictionary('xAODTau_cDict')
            from tauRecTools.tauRecToolsConf import TauWPDecorator
            # wp creators 1p / mp
            wp_decorator = TauWPDecorator(
                name = "TrigTauJetWPDecorator",
                calibFolder='TrigTauRec/00-11-01/',
                inTrigger = True,
                flatteningFile1Prong = "FlatJetBDT1P_trigger_v1.root", 
                flatteningFile3Prong = "FlatJetBDT3P_trigger_v1.root", 
                CutEnumVals=[
                    ROOT.xAOD.TauJetParameters.JetBDTSigVeryLoose, 
                    ROOT.xAOD.TauJetParameters.JetBDTSigLoose,
                    ROOT.xAOD.TauJetParameters.JetBDTSigMedium, 
                    ROOT.xAOD.TauJetParameters.JetBDTSigTight],
                SigEff1P = [0.995, 0.99, 0.97, 0.90],
                SigEff3P = [0.995, 0.94, 0.88, 0.78],
                ScoreName = "BDTJetScore",
                NewScoreName = "BDTJetScoreSigTrans",
                DefineWPs = True)
            return [bdt_1p, bdt_mp, wp_decorator]


    def VarCalculatorSet(self):
        from tauRecTools.tauRecToolsConf import TauIDVarCalculator
        tauVarCalc = TauIDVarCalculator()
        tauVarCalc.inTrigger = True
        tauVarCalc.calibFolder = 'TrigTauRec/00-11-01/'
        return tauVarCalc

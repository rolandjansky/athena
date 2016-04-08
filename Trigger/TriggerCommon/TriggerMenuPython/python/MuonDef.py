# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
#
# SliceDef file for muon chains/signatures
#
#########################################################################
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)

logMuonDef = logging.getLogger("TriggerMenuPython.MuonDef")

from AthenaCommon import CfgGetter
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

import re

from TriggerJobOpts.TriggerFlags            import TriggerFlags
from TriggerMenuPython.Lvl1Flags            import Lvl1Flags

from MuonSliceFlags                         import MuonSliceFlags

from TriggerMenuPython.HltConfig import *

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo
from TrigmuFast.TrigmuFastConfig import *
from TrigIDSCAN.TrigIDSCAN_Config import TrigIDSCAN_Muon, TrigIDSCAN_Muon_NoTRT, TrigIDSCAN_Cosmics, TrigIDSCAN_muonIso
from TrigSiTrack.TrigSiTrack_Config import TrigSiTrack_Muon, TrigSiTrack_Muon_NoTRT, TrigSiTrack_muonIso
from TrigL2TRTSegFinder.TrigTRTSegFinder_Config import TrigTRTSegFinder_Muon
from TrigmuIso.TrigmuIsoConfig import *
from TrigMuonHypo.TrigL2MuonOverlapRemoverConfig import *
from TrigMuonHypo.TrigEFMuonOverlapRemoverConfig import * 

from TrigMuonHypo.TrigMuonHypoConfig import *
from InDetTrigRecExample.EFInDetConfig import *
from TrigMuonEF.TrigMuonEFConfig import *
#from TrigMuonEF.TrigMuonEFCosmicConfig import *
from TrigMuGirl.TrigMuGirlConfig import *
from TrigmuRoI.TrigmuRoIConfig import *

# lazy loading on-demand
def TrigmuCombConfig(algName,idInput):
    from TrigmuComb.TrigmuCombConfig import TrigmuCombConfig as TrigmuCombConfigured
    return TrigmuCombConfigured(algName,idInput)


theTrigmuFastMuon = TrigmuFastConfig('Muon')

theTrigmuFast900GeV = TrigmuFastConfig('900GeV')
theTrigmuFastOutBCID = TrigmuFastConfig('OutBCID')
theTrigmuFastOutBCIDMuonEcut4 = TrigmuFastConfig('OutBCIDMuonEcut4')
theTrigmuFastCAL = TrigmuFastConfig('Mcal')

theTrigmuFastMuonEcut4   = TrigmuFastConfig('MuonEcut4')

theTrigTRTSegFinder_Muon = TrigTRTSegFinder_Muon()

theTrigMuonEFTrackIsoAnnulus = TrigMuonEFTrackIsolationAnnulusConfig()

# L2 muon SA default
_GLOBAL_L2_SA_ALG_DEFAULT = 'l2muonSA'
### _GLOBAL_L2_SA_ALG_DEFAULT = 'muFast'

# regular expression to extract the number from the Threshold string (i.e. it removed the trailing "GeV")
_thrRE = re.compile(r"(?P<thr>[0-9]+)GeV")

# regular expression to extract threshold, lowPtTight, isolation from the signature identifier
_sigRE = re.compile(r"mu(?P<muThr>\d+)(?P<lowPtTight>T)?(?P<iso>i[0-9a-z]*)?(?P<postfix>.*)")

def getMufastThr(muThr):
    if muThr == '4GeV':
        return '4GeV_v11a'
    elif muThr == '2GeV':
        return '2GeV'
    else:
        return '6GeV_v11a'


def getMucombThr(muThr):
    match = _thrRE.match(muThr)
    if match:
        thr = int(match.group("thr"))
        if thr >= 24:
            return '22GeV'
        else:
            return muThr
            
    else:
        logMuon.error("No MuComb Threshold available for muThr=%r", muThr)

    return ''


    
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonA
theTrigL2SiTrackFinder_MuonA = TrigL2SiTrackFinder_MuonA()

from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonB
theTrigL2SiTrackFinder_MuonB = TrigL2SiTrackFinder_MuonB()

from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonC
theTrigL2SiTrackFinder_MuonC = TrigL2SiTrackFinder_MuonC()

##FTK
from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonF
theTrigL2SiTrackFinder_MuonF = TrigL2SiTrackFinder_MuonF()


from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoA
theTrigL2SiTrackFinder_muonIsoA = TrigL2SiTrackFinder_muonIsoA()

from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoB
theTrigL2SiTrackFinder_muonIsoB = TrigL2SiTrackFinder_muonIsoB()

from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_muonIsoC
theTrigL2SiTrackFinder_muonIsoC = TrigL2SiTrackFinder_muonIsoC()

#merging
try:
  from TrigL2SiTrackFinder.TrigL2SiTrackFinder_Config import TrigL2SiTrackFinder_MuonBC
  theTrigL2SiTrackFinder_MuonBC = TrigL2SiTrackFinder_MuonBC()
except:
  theTrigL2SiTrackFinder_MuonBC = False


theTrigIDSCAN_Muon = TrigIDSCAN_Muon()
theTrigIDSCAN_Muon_NoTRT = TrigIDSCAN_Muon_NoTRT()
#theTrigIDSCAN_Cosmics = TrigIDSCAN_Cosmics()
theTrigIDSCAN_muonIso = TrigIDSCAN_muonIso()
theTrigSiTrack_muonIso = TrigSiTrack_muonIso()
#theTrigSiTrack_Tile = TrigSiTrack_Tile()

theMuIsoConfig = muIsoConfig('HighPT','IDSCAN')
theMuIsoConfig.IDalgo="IDSCAN"

theMuIsoConfig_SITRACK = muIsoConfig('HighPT','SITRACK')
theMuIsoConfig_SITRACK.IDalgo="SITRACK" 

theMuIsoConfig_L2StarA = muIsoConfig('HighPT', 'STRATEGY_A')
theMuIsoConfig_L2StarA.IDalgo = "STRATEGY_A" 
theMuIsoConfig_L2StarB = muIsoConfig('HighPT', 'STRATEGY_B')
theMuIsoConfig_L2StarB.IDalgo ="STRATEGY_B"
theMuIsoConfig_L2StarC = muIsoConfig('HighPT', 'STRATEGY_C')
theMuIsoConfig_L2StarC.IDalgo ="STRATEGY_C"

#MSonly IDScan
theMuIsoConfig_MSonly = muIsoConfig('MSonly','IDSCAN')
theMuIsoConfig_MSonly.MuIsoStrategy=2
theMuIsoConfig_MSonly.IDalgo="IDSCAN"

#LowPt
theMuIsoConfig_LowPT = muIsoConfig('LowPT','IDSCAN')
theMuIsoConfig_LowPT.IDalgo="IDSCAN"



def getTrigMuonEFTrackIsolation(postfix=""):
    name = "TrigMuonEFTrackIsolation"
    if postfix:
        name += '_'
        name += postfix
    return TrigMuonEFTrackIsolationConfig(name)


theTrigEFIDInsideOut_MuonIso = TrigEFIDSequence("MuonIso","muonIso","InsideOut") 
theTrigEFIDInsideOut_Muon_MergedDP = TrigEFIDSequence("Muon","muon","DataPrep")


# TrigL2MuonSA instance
from TrigL2MuonSA.TrigL2MuonSAConfig import *

#for monitoring chain with vtx cut
from TrigMinBias.TrigMinBiasConfig import *



###########################################################################
#Basic chain class, with isolation hypo
###########################################################################
class L2EFChain_mu(L2EFChainDef):
    def __init__(self, sig_id,
                 l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                 ef_chain_name, ef_chain_counter,
                 l2_inputTEs, config,
                 prescale=1, pass_through=0, rerun=0):

        self._excludedSignatures = []

        self._initOK = True
        try:
            L2EFChainDef.__init__(self, sig_id,
                                  l2_chain_name, l2_chain_counter,
                                  l2_lower_chain_name,
                                  ef_chain_name, ef_chain_counter,
                                  l2_inputTEs, config,
                                  prescale, pass_through, rerun)

        except:
            self._initOK = False
            self.sig_id = sig_id # still set sig_id for printout later on
            global athenaCommonFlags
            if not athenaCommonFlags.AllowIgnoreConfigError:
                raise
            else:
                import traceback
                traceback.print_exc()
                logMuonDef.error("Chain "+sig_id+" failed configuration. Will be removed from list of chains.")

        

    class L2Config:
        def __init__(self, suffix, MufastHypo_1, MucombHypo_1, MuisolHypo_1=None):
            self.suffix = suffix
            self.MufastHypo_1 = MufastHypo_1
            self.MucombHypo_1 = MucombHypo_1
            self.MuisolHypo_1 = MuisolHypo_1


    class EFConfig:
        def __init__(self, suffix, TrigMuonHypo_1):
            self.suffix = suffix
            self.TrigMuonHypo_1 = TrigMuonHypo_1


    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig


    def excludeSignature(self,sig):
        self._excludedSignatures.append(sig)

    def defineSignatures(self):
        L2conf = self.config.L2Config
        EFconf = self.config.EFConfig

        if L2conf:
            for l2 in self.l2chain.sequences:
                te = l2.outputTE
                if te not in self._excludedSignatures:
                    self.addL2Signature([te])

        if EFconf:
            for ef in self.efchain.sequences:
                te = ef.outputTE
                if te not in self._excludedSignatures:
                    self.addEFSignature([te])


    def defineTErenaming(self):
        L2conf = self.config.L2Config
        EFconf = self.config.EFConfig
        self.TErenamingMap = {}
        if L2conf:
            L2suf = L2conf.suffix
            for l2 in self.l2chain.sequences:
                te = l2.outputTE
                self.TErenamingMap[te] = mergeRemovingOverlap(te, L2suf)

        if EFconf:
            EFsuf = EFconf.suffix
            for ef in self.efchain.sequences:
                te = ef.outputTE
                self.TErenamingMap[te] = mergeRemovingOverlap(te, EFsuf)
        

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:SingleMuon','BW:Muon']
        self.trigger_type = ['hello']


    def getNextL2InputTE(self):
        if self.l2chain.sequences:
            return self.l2chain.sequences[-1].outputTE
        elif self.inputTEs:
            return self.inputTEs[0]
        else:
            return ''


    def getNextEFInputTE(self):
        if self.efchain.sequences:
            return self.efchain.sequences[-1].outputTE
        else:
            return self.getNextL2InputTE()



################################################################################
# class L2EFChain_mu_autoconfig
# Author: Martin Woudstra
#
# Required positional arguments to contructor (in order):
# sig_id : signature unique identifier
# ef_chain_counter: signature unique number
#
# Optional 3rd argument, but required in some cases (class will tell when needed):
# l2_chain_counter
#
# The following optional named arguments can be supplied to the contructor.
# Defaults are determined from the signature name.
# muThr : pT threshold (EF Combined). Determines muFast and muComb threshold as well (from muThr via fixed mapping)
# l2_input_TEs
# l2_lower_chain_name
# l2_chain_name
# ef_chain_name
# l2_overlapRemoval
# ef_overlapRemoval
# l2_iso
# ef_iso
# l2_id_alg
# l2_sa_alg
# ef_mu_alg   Possible values listed in _EF_MU_ALGS
# l2_alg_name: instance name for muFast, muComb, muCombHypo
#
################################################################################
class L2EFChain_mu_autoconfig(L2EFChain_mu):

    # possible values for ef_mu_alg
    _EF_MU_ALGS = ( 'TMEFfirst', # TrigMuSuperEF running TrigMuonEF first
                    'MGfirst',   # TrigMuSuperEF running TrigMuGirl first
                    'TMEFonly',  # TrigMuSuperEF running TrigMuonEF only
                    'MGonly',    # TrigMuSuperEF running TrigMuGirl only
                    'both',      # TrigMuSuperEF running both TrigMuonEF and TrigMuGirl
                    'SuperEF',   # TrigMuSuperEF running chosen default config
                    'MuonEF',    # TrigMuonEF sequence
                    'MG',        # TrigMuGirl
                    'slow',      # TrigMuGirlSlow
                    )
    _EF_MU_ALG_default = 'SuperEF'

    _EF_ALGS = _EF_MU_ALGS

    #_L2_ID_ALGS = ( 'L2StarA', 'L2StarB', 'L2StarC', 'IDSCAN' )
    #E.P.
    _L2_ID_ALGS = ( 'L2StarA', 'L2StarB', 'L2StarC', 'IDSCAN',"FTK" )
    _L2_ID_ALG_default = "L2StarA"

    _L2_SA_ALGS = ( 'muFast', 'l2muonSA', 'slow_outOfTime', 'slow1', 'slow' )
    _L2_SA_ALG_default = _GLOBAL_L2_SA_ALG_DEFAULT

    _L2_CB_ALGS = ( 'muComb', 'slow' )
    _L2_CB_ALG_default = 'muComb'

    _L2_ALGS = _L2_ID_ALGS + _L2_SA_ALGS + _L2_CB_ALGS
    
    class L2Config:
        def __init__(self, **kwargs):
            self.suffix         = kwargs["l2_suffix"]
            self.hypoPostfix    = kwargs["l2_hypoPostfix"]
            self.muThr          = kwargs["muThr"]
            self.iso            = kwargs["l2_iso"]
            self.overlapRemoval = kwargs["l2_overlapRemoval"]
            self.alg_name       = kwargs["l2_alg_name"]
            self.id_alg         = kwargs["l2_id_alg"]
            self.sa_alg         = kwargs["l2_sa_alg"]
            self.cb_alg         = kwargs["l2_cb_alg"]


    class EFConfig:
        def __init__(self, **kwargs):
            self.suffix      = kwargs["ef_suffix"]
            self.hypoPostfix = kwargs["ef_hypoPostfix"]
            self.muThr       = kwargs["muThr"]
            self.iso         = kwargs["ef_iso"]
            self.mu_alg      = kwargs["ef_mu_alg"]

    
    def __init__(self, sig_id, ef_chain_counter, l2_chain_counter=None,
                 prescale=1, pass_through=0, rerun=0,
                 **kwargs):
        kwargs["ef_chain_counter"] = ef_chain_counter
        if l2_chain_counter: kwargs["l2_chain_counter"] = l2_chain_counter

        self.setDefaultArgs(sig_id, kwargs)

        l2_config = self.__class__.L2Config(**kwargs)
        ef_config = self.__class__.EFConfig(**kwargs)
        config = self.__class__.Config(l2_config,ef_config)

        L2EFChain_mu.__init__(self, sig_id,
                              kwargs["l2_chain_name"], kwargs["l2_chain_counter"], kwargs["l2_lower_chain_name"],
                              kwargs["ef_chain_name"], kwargs["ef_chain_counter"],
                              kwargs["l2_inputTEs"], config,
                              prescale, pass_through, rerun)

    # end of __init__


    def initOK(self):
        return self._initOK

        
    def setDefaultArgs(self, sig_id, kwargs):
        global logMuonDef, _sigRE

        # set some sensible defaults based on signature
        sigMatch = _sigRE.match(sig_id)
        if sigMatch is None:
            raise RuntimeError( "Signature %r not supported by class %s" % (sig_id,self.__class__.__name__) )

        printArgs = [ ] # for printout of default values set

        self.setDefaultThresholdArgs(sig_id,kwargs,printArgs,sigMatch)
        self.setDefaultInputArgs    (sig_id,kwargs,printArgs,sigMatch)
        self.setDefaultIsolationArgs(sig_id,kwargs,printArgs,sigMatch)
        self.setDefaultL2AlgArgs    (sig_id,kwargs,printArgs,sigMatch)
        self.setDefaultEFAlgArgs    (sig_id,kwargs,printArgs,sigMatch)
        self.setDefaultChainNames   (sig_id,kwargs,printArgs,sigMatch)

        # make standard TErenaming suffices
        l2_chain_name = kwargs["l2_chain_name"]
        if l2_chain_name.startswith("L2"):
            kwargs["l2_suffix"] = l2_chain_name[2:]  # remove leading "L2"
        else:
            kwargs["l2_suffix"] = l2_chain_name

        ef_chain_name = kwargs["ef_chain_name"]
        if ef_chain_name.startswith("EF"):
            kwargs["ef_suffix"] = ef_chain_name[2:]  # remove leading "EF"
        else:
            kwargs["ef_suffix"] = ef_chain_name

        if "l2_chain_counter" not in kwargs:
            # if L2 different from EF, require explicit l2_chain_counter
            if kwargs["ef_suffix"] != kwargs["l2_suffix"]:
                logMuonDef.error("Signature %r: L2_suffix(=%r) different from EF_suffix(=%r) requires explicit l2_chain_counter=<some_number> in the signature definition which should be the same as other signatures with l2_chain_name=%r" % (sig_id, kwargs["l2_suffix"], kwargs["ef_suffix"], kwargs["l2_chain_name"] ) )
            # by default, use same counters for L2 and EF
            kwargs["l2_chain_counter"] = kwargs["ef_chain_counter"]
            

        # determine hypo name (in reality only part of full name)
        hypoPostfix='Muon'
        for ef_mu_alg in self._EF_MU_ALGS:
            ef_mu_sig = '_' + ef_mu_alg
            if ef_mu_sig in sig_id:
                hypoPostfix += ef_mu_sig
        kwargs["l2_hypoPostfix"] = hypoPostfix
        kwargs["ef_hypoPostfix"] = hypoPostfix

        # printout of some values that were auto-determined
        autoDefaults = []
        for n in printArgs:
            v = kwargs[n]
            if v: autoDefaults.append( "%s=%r" % (n,v) )

        if autoDefaults:
            logMuonDef.debug( "Signature=%r auto-defaults: %s", sig_id, ' '.join(autoDefaults) )
    # end of setDefaultArgs()


    def setDefaultThresholdArgs(self,sig_id,kwargs,printArgs,sigMatch):
        if "muThr" not in kwargs:
            muThr = sigMatch.group("muThr")
            kwargs["muThrVal"] = int(muThr)
            kwargs["muThr"] = muThr+"GeV"
            printArgs.append("muThr")
        else:
            thrMatch = _thrRE.match(kwargs["muThr"])
            if not thrMatch:
                logMuonDef.error("signature %r: Could not determine muThr from %r", sig_id, kwargs["muThr"])
                self._initOK = False
                return
            
            muThrVal=int(thrMatch.group("thr"))
            if not muThrVal:
                logMuonDef.error("signature %r: Could not determine muThr from %r", sig_id, kwargs["muThr"])
                self._initOK = False
                return

            kwargs["muThrVal"] = muThrVal
    # end of setDefaultThresholdArgs()


    def setDefaultInputArgs(self,sig_id,kwargs,printArgs,sigMatch):
        muThrVal = kwargs["muThrVal"]
        lowPtTight=sigMatch.group("lowPtTight")

        
        if "l2_inputTEs" not in kwargs:
            if "_loose" in sig_id:
                logMuonDef.error("signature %r: Don't know how to handle '_loose'. Specify l2_inputTEs in definition" % sig_id) 
            elif muThrVal >= 15:
                if "_tight1" in sig_id:
                    l2_inputTEs = ["MU20"]
                elif "_tight" in sig_id:
                    l2_inputTEs = ["MU15"]
                elif "_medium" in sig_id:
                    l2_inputTEs = ["MU11"]
                else:
                    l2_inputTEs = ["MU10"]
            elif TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv1Menus() or \
                     TriggerFlags.triggerMenuSetup() in Lvl1Flags.HIv2Menus():
                l2_inputTEs = ['MU0']
            elif muThrVal >= 10:
                l2_inputTEs = ["MU10"]
            elif muThrVal >= 6:
                l2_inputTEs = ["MU6"]
            elif lowPtTight:
                l2_inputTEs = ["MU4"]
            else:
                l2_inputTEs = ["MU0"]

            kwargs["l2_inputTEs"] = l2_inputTEs
            printArgs.append("l2_inputTEs")
        else:
            l2_inputTEs = kwargs["l2_inputTEs"]
            if type(l2_inputTEs) == str:
                # turn single input TE string into a list
                kwargs["l2_inputTEs"] = [ l2_inputTEs ]


        if "l2_lower_chain_name" not in kwargs:
            if len(l2_inputTEs) != 1:
                logMuonDef.error("signature %r: Can not determine l2_lower_chain_name from l2_inputTEs=%r."
                                 " Specify explicitly in definion" % (sig_id,l2_inputTEs) )
                self._initOK = False

            if l2_inputTEs[0]:
                kwargs["l2_lower_chain_name"] = "L1_" + l2_inputTEs[0]
            else:
                kwargs["l2_lower_chain_name"] = ''
            printArgs.append("l2_lower_chain_name")

    # end of setDefaultInputArgs()


    def setDefaultIsolationArgs(self,sig_id,kwargs,printArgs,sigMatch):
        iso = sigMatch.group("iso")
        if iso: logMuonDef.debug( "signature=%r ==> iso=%r" , sig_id, iso )

        if "l2_iso" not in kwargs or "ef_iso" not in kwargs:
            if iso: logMuonDef.debug( "signature=%r ==> iso=%r" , sig_id, iso )
            if not iso:  # either "" or None
                ef_iso=None
                l2_iso=None
            elif iso == 'i' or iso == 'i5':
                ef_iso='RelEFOnlyMedium'
                l2_iso=None
            elif iso == 'i4':
                ef_iso='Tight'
                l2_iso='tight'
            elif iso == 'i3':
                ef_iso='EFOnlyTight'
                l2_iso=None
            elif iso == 'i2':
                ef_iso='Loose'
                l2_iso='loose'
            elif iso == 'i1':
                ef_iso='EFOnlyLoose'
                l2_iso=None
            elif iso == 'it':
                ef_iso='TauCP'
                l2_iso='tauCP'    
            else:
                logMuonDef.error("signature %r : don't know what to do with isolation %r" % (sig_id,iso) )
                self._initOK = False
                
            # only set to default if not specified in definition
            kwargs.setdefault("l2_iso",l2_iso)
            kwargs.setdefault("ef_iso",ef_iso)
            if l2_iso is not None: printArgs.append("l2_iso")
            if ef_iso is not None: printArgs.append("ef_iso")
    # end of setDefaultIsolationArgs()

    # private helper function
    def _setDefaultAlgArg(self,sig_id,kwargs,printArgs,varName):
        if varName in kwargs: return
            
        varUpper = varName.upper()
        alg_list = getattr(self,'_%sS' % varUpper)
        # try to determine it from the sig_id
        for alg in alg_list:
            # first match is chosen
            alg_id = '_' + alg
            if alg_id in sig_id:
                kwargs[varName] = alg
                printArgs.append(varName)
                break

        else: # of for-loop: no match in sig_id
            kwargs[varName] = getattr(self,'_%s_default' % varUpper)
        

    def setDefaultL2AlgArgs(self,sig_id,kwargs,printArgs,sigMatch):

        # L2 Muon Standalone Algorithm
        self._setDefaultAlgArg(sig_id,kwargs,printArgs,"l2_sa_alg")
        # L2 Inner Detector Algorithm
        self._setDefaultAlgArg(sig_id,kwargs,printArgs,"l2_id_alg")
        # L2 Muon Combined Algorithm
        self._setDefaultAlgArg(sig_id,kwargs,printArgs,"l2_cb_alg")

        if 'l2_alg_name' not in kwargs:
            muThrVal = kwargs["muThrVal"]
            # special configuration for low pT chains
            if muThrVal <= 4:
                kwargs['l2_alg_name'] = '900GeV'  # special config for low pT
                printArgs.append("l2_alg_name")
            else:
                kwargs['l2_alg_name'] = 'Muon'  # default config

        if "l2_overlapRemoval" not in kwargs:
            kwargs["l2_overlapRemoval"] = 'wOvlpRm' in sig_id
            printArgs.append("l2_overlapRemoval")
    # end of setDefaultL2AlgArgs()


    def setDefaultEFAlgArgs(self,sig_id,kwargs,printArgs,sigMatch):
        # EF Muon Algorithm
        self._setDefaultAlgArg(sig_id,kwargs,printArgs,"ef_mu_alg")


    def setDefaultChainNames(self,sig_id,kwargs,printArgs,sigMatch):
        #### set L2 chain name and suffix, removing any EF modifiers from the signature ID
        l2_chain_name = kwargs.get("l2_chain_name")
        if l2_chain_name is None:
            # start without isolation part if L2 does not have isolation
            if not kwargs["l2_iso"]:
                muThr = sigMatch.group("muThr")
                lowPtTight=sigMatch.group("lowPtTight")
                postfix = sigMatch.group("postfix")
                sig_id_noIso = "mu"+muThr
                if lowPtTight: sig_id_noIso += lowPtTight
                if postfix   : sig_id_noIso += postfix
                l2_chain_name=sig_id_noIso
            else:
                l2_chain_name=sig_id

            # strip signature of all EF-only specific modifiers
            for ef_alg in self._EF_ALGS:
                if ef_alg in self._L2_ALGS: continue
                l2_chain_name=l2_chain_name.replace('_' + ef_alg,"")
            # finally, prefix with L2
            l2_chain_name = "L2_" + l2_chain_name
            kwargs["l2_chain_name"] = l2_chain_name
            printArgs.append("l2_chain_name")

        #### set EF chain name and suffix
        ef_chain_name = kwargs.get("ef_chain_name")
        if ef_chain_name is None:
            ef_chain_name = "EF_" + sig_id
            kwargs["ef_chain_name"] = ef_chain_name
            printArgs.append("ef_chain_name")

 
    def defineSequences(self, config):
        self.defineL2Sequence(config.L2Config)
        self.defineEFSequence(config.EFConfig)


    def defineL2Sequence(self,L2conf):
        if L2conf.sa_alg == "slow_outOfTime":
            self.addL2Sequence('',
                               [ TrigmuRoIConfig() ],
                               'L2_mu_out_of_time')
            self.excludeSignature(self.getNextL2InputTE())

        self.addL2StandaloneSequence(L2conf)
        
        if L2conf.overlapRemoval:
            self.addL2Sequence(self.getNextL2InputTE(),
                               [ TrigL2MuonOverlapRemoverConfig('Mufast','nominal') ],
                               'L2_muon_standalone_wOvlpRm')

        self.addL2CombinedSequence(L2conf)
        
        if L2conf.overlapRemoval:
            self.addL2Sequence(self.getNextL2InputTE(),
                               [ TrigL2MuonOverlapRemoverConfig('Mucomb','nominal') ],
                               'L2_muon_combined_wOvlpRm')
    
        if L2conf.iso:
            self.addL2Sequence(self.getNextL2InputTE(),                               
                               [ theMuIsoConfig_L2StarA, MuisoHypoConfig(L2conf.hypoPostfix, L2conf.iso) ],
                               #[ theMuIsoConfig, MuisoHypoConfig(L2conf.hypoPostfix, L2conf.iso) ],
                               'L2_mui')
            
    # end of defineL2Sequence()


    def addL2StandaloneSequence(self,L2conf):
        muFastThr=getMufastThr(L2conf.muThr)
        slowThr=L2conf.muThr+'_slow'
        if L2conf.sa_alg == "muFast":
            sa_alg = TrigmuFastConfig(L2conf.alg_name)
            sa_hyp = MufastHypoConfig(L2conf.alg_name, muFastThr)
        elif L2conf.sa_alg == "slow":
            sa_alg = theTrigmuFastMuonEcut4
            sa_hyp = MufastStauHypoConfig('Muon', slowThr)
        elif L2conf.sa_alg == "slow1":
            sa_alg = theTrigmuFastMuonEcut4
            sa_hyp = MufastStauHypoConfig('Tight', slowThr)
        elif L2conf.sa_alg == "slow_outOfTime":
            sa_alg = theTrigmuFastOutBCIDMuonEcut4
            sa_hyp = MufastStauHypoConfig('Muon', slowThr)
        elif L2conf.sa_alg == "l2muonSA":
            sa_alg = TrigL2MuonSAConfig(L2conf.alg_name)
            sa_hyp = MufastHypoConfig(L2conf.alg_name, muFastThr)
        else:
            logMuonDef.error("Signature %r: unknown L2 sa_alg:%r" % (self.sig_id,L2conf.sa_alg) )
            self._initOK = False
            return

        self.addL2Sequence(self.getNextL2InputTE(),
                           [ sa_alg, sa_hyp ],
                           'L2_muon_standalone')
    # end of addL2StandaloneSequence()


    def addL2CombinedSequence(self,L2conf):
        # Select the Inner Detector Algorithm
        if L2conf.id_alg == "L2StarA":
            id_alg_output = "STRATEGY_A"
            if L2conf.iso:
                id_alg = theTrigL2SiTrackFinder_muonIsoA
            else:
                id_alg = theTrigL2SiTrackFinder_MuonA
        elif L2conf.id_alg == "L2StarB":
            id_alg_output = "STRATEGY_B"
            if L2conf.iso:
                id_alg = theTrigL2SiTrackFinder_muonIsoB
            else:
                id_alg = theTrigL2SiTrackFinder_MuonB
        elif L2conf.id_alg == "L2StarC":
            id_alg_output = "STRATEGY_C"
            if L2conf.iso:
                id_alg = theTrigL2SiTrackFinder_muonIsoC
            else:
                id_alg = theTrigL2SiTrackFinder_MuonC
        elif L2conf.id_alg == "IDSCAN":
            id_alg_output = "IDSCAN"
            if L2conf.iso:
                id_alg = theTrigIDSCAN_muonIso
            else:
                id_alg = theTrigIDSCAN_Muon
        #E.P.
        elif L2conf.id_alg == "FTK":
            id_alg_output = "STRATEGY_F"
            id_alg = theTrigL2SiTrackFinder_MuonF
                
        else:
            logMuonDef.error("Signature %r: unknown L2 id_alg:%r" % (self.sig_id,L2conf.id_alg) )
            self._initOK = False
            return

        # Select the Combined Muon Algorithm
        if L2conf.cb_alg == 'muComb':
            cb_alg = TrigmuCombConfig(L2conf.alg_name, id_alg_output)
            cb_hyp = MucombHypoConfig(L2conf.alg_name, getMucombThr(L2conf.muThr))
        elif 'slow' in L2conf.cb_alg:
            slowThr=L2conf.muThr+'_slow'
            cb_alg = TrigmuCombConfig(L2conf.alg_name, id_alg_output)
            cb_hyp = MucombStauHypoConfig(L2conf.alg_name, slowThr)
        else:
            logMuonDef.error("Signature %r: unknown L2 cb_alg:%r" % (self.sig_id,L2conf.cb_alg) )
            self._initOK = False
            return
        
        self.addL2Sequence(self.getNextL2InputTE(),
                           [ id_alg, cb_alg, cb_hyp ],
                           'L2_mu')
    # end of addL2CombinedSequence():


    def defineEFSequence(self,EFconf):
        self.addEFSequence(self.getNextEFInputTE(),
                           TrigEFIDInsideOut_Muon().getSequence(),
                           'EFID_mu')

        if EFconf.iso:
            MuOutputTE = 'EF_mu_combined'
        else:
            MuOutputTE = 'EF_mu'
        

        if EFconf.mu_alg in [ 'SuperEF', 'TMEFfirst', 'MGfirst', 'TMEFonly', 'MGonly', 'both' ]:  # all SuperEF variants
            self.addEFTrigMuSuperEFSequence(EFconf,MuOutputTE)
        elif EFconf.mu_alg == 'slow':
            self.addEFTrigMuGirlSlowSequence(EFconf,MuOutputTE)
        elif EFconf.mu_alg == 'MuonEF':
            self.addEFTrigMuonEFSequence(EFconf,MuOutputTE)
        elif EFconf.mu_alg == 'MG':
            self.addEFTrigMuGirlSequence(EFconf,MuOutputTE)
        else:
            logMuonDef.warning("signature %r: unknown EF Muon algorithm %r, not adding any", self.sig_id, EFconf.mu_alg )
            self._initOK = False
            return

        if EFconf.iso:
            self.addEFSequence(self.getNextEFInputTE(),
                               theTrigEFIDInsideOut_MuonIso.getSequence(), 
                               'EF_muI_efid')

            if EFconf.mu_alg == self._EF_MU_ALG_default:
                isoPostfix = "" # no postfix for default
            else:
                isoPostfix = EFconf.mu_alg  # just to get separate monitoring histograms
            self.addEFSequence(self.getNextEFInputTE(),
                               [ getTrigMuonEFTrackIsolation(isoPostfix),
                                 TrigMuonEFTrackIsolationHypoConfig(EFconf.hypoPostfix,EFconf.iso)],
                               'EF_mu') 

    # end of defineEFSequence


    def addEFTrigMuSuperEFSequence(self,EFconf,outputTE):
        if EFconf.mu_alg == 'SuperEF':
            mu_alg = CfgGetter.getAlgorithm("TrigMuSuperEF")
        else:
            mu_alg = CfgGetter.getAlgorithm("TrigMuSuperEF_"+EFconf.mu_alg)
        
        self.addEFSequence(self.getNextEFInputTE(),
                           [ mu_alg, TrigMuonEFCombinerHypoConfig(EFconf.hypoPostfix,EFconf.muThr) ],
                           outputTE)


    def addEFTrigMuonEFSequence(self,EFconf,outputTE):
        self.addEFSequence(self.getNextEFInputTE(),
                           [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFonly"),
                            TrigMuonEFCombinerHypoConfig(EFconf.hypoPostfix,EFconf.muThr)],
                           outputTE)


    def addEFTrigMuGirlSequence(self,EFconf,outputTE):
        self.addEFSequence(self.getNextEFInputTE(),
                           [TrigMuGirlConfig('test'), TrigMuGirlHypoConfig('Muon',EFconf.muThr)],
                           outputTE)


    def addEFTrigMuGirlSlowSequence(self,EFconf,outputTE):
        self.addEFSequence(self.getNextEFInputTE(),
                           [TrigMuGirlConfig('Slow'), TrigMuGirlStauHypoConfig('Muon', EFconf.muThr)],
                           outputTE)



###########################################################################
# To test L2/EF merging.Run all EF algo to L2
###########################################################################
class L2EFChain_mu_L2EFMerged(L2EFChain_mu_autoconfig):


    def addEFSequence(self, inputTEs, algos, outputTE):
        self.addL2Sequence( inputTEs, algos, outputTE)


###########################################################################
# EF Full Scan
###########################################################################
class L2EFChain_mu_Seeded_EFFS:
    def __init__(self, sig_id, single_chain, fs_chain, efcounter):
        self.sig_id    = sig_id
        self.l2_chain  =  single_chain.l2_chain_name
        self.ef_single_chains = [single_chain.ef_chain_name]
        self.ef_fs_chains     = ['EF_' + fs_chain]
        self.efcounter = efcounter
        
    def generateMenu(self, triggerPythonConfig):
        
        mlog = logging.getLogger( 'MuonDef.py' )  ## get the logger
        l2chain = triggerPythonConfig.getHLTChain(self.l2_chain)
        ef_single_parts = [ triggerPythonConfig.getHLTChain(i) for i in self.ef_single_chains  ]
        ef_fs_parts     = [ triggerPythonConfig.getHLTChain(i) for i in self.ef_fs_chains  ]
        if None in ef_single_parts+ef_fs_parts:  ## some action if simple chains are not found
            mlog.error("Basic chains used to built combined chain: "+self.sig_id+" are not present.")
            return
        
        efchain = HLTChain(chain_name='EF_'+self.sig_id, chain_counter=str(self.efcounter), lower_chain_name=self.l2_chain, level='EF', prescale='1', pass_through='0', rerun_prescale='0')

        efchain.addStreamTag('muons')
        if 'mu18_tight_Jpsi_EFFS' in self.sig_id:
            efchain.addStreamTag('Bphysics')

        efchain.mergeAndAppendChains_Order(ef_single_parts+ef_fs_parts)
        
        #add groups to the muon chains
        efchain.addGroup(["RATE:SingleMuon",'BW:Muon'])
        
        triggerPythonConfig.registerHLTChain(self.sig_id, l2chain)
        triggerPythonConfig.registerHLTChain(self.sig_id, efchain)


###########################################################################
# EFFSonly
###########################################################################
class L2EFChain_mu_EFFSonly(L2EFChain_mu):

    def __init__(self, sig_id, counter, n_muons, EFCB_FS_Hypo,
                 l2_lower_chain_name=None,prescale=1, pass_through=0, rerun=0):
        
        self.sig_id  = sig_id
        self.counter = counter
        self.n_muons = n_muons
        self.l2_lower_chain_name=l2_lower_chain_name

        suffix = '_' + sig_id
        
        l2_chain_name = 'L2_' + sig_id
        l2_chain_counter = counter
        if self.l2_lower_chain_name is None:
            l2_lower_chain_name = ''
        ef_chain_name = 'EF_' + sig_id
        ef_chain_counter = counter
        l2_inputTEs = ['']
        
        config = self.Config(self.L2Config(suffix), self.EFConfig(suffix,EFCB_FS_Hypo))
        
        L2EFChain_mu.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through, rerun)
        
    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            pass
        
    class EFConfig:
        def __init__(self, suffix, EFCB_FS_Hypo):
            self.suffix = suffix
            self.EFCB_FS_Hypo = EFCB_FS_Hypo
            pass
        
    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig
                                                                                                        
    def defineSequences(self, config):
        EFconf = config.EFConfig
        
        if self.ef_chain_name.find('mu2') >= 0:
            theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV', '0GeV')
        else:
            theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '2GeV', '2GeV')

        self.addEFSequence('',
                           [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                           [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA"),
                            theTrigMuonEFSA_FS_Hypo],
                           'EF_SA_FS')
        self.addEFSequence('EF_SA_FS',
                           [CfgGetter.getAlgorithm("TrigMuonEFFSRoiMaker")],
                           'EF_SAR_FS')
        self.addEFSequence('EF_SAR_FS',
                           TrigEFIDInsideOut_Muon().getSequence(),
                           'EF_FStracksMuon')
        self.addEFSequence('EF_FStracksMuon',
                           [TrigMuonEFCombinerConfig('TrigMuonEFCombinerPerROI')],
                           #can be added to reduce combinatorics in the aggregator, but makes comparisons to old setup harder
                           #TrigMuonEFCombinerHypoConfig('TrigMuonEFCombinerHypoForFS','8GeV')],
                           'EF_CB_FS_single')
                           # hypo code using the RoiAggregator
        self.addEFSequence('EF_CB_FS_single',
                           [TrigMuonEFRoiAggregatorConfig('TrigMuonEFFSRoiAggregator'),
                            EFconf.EFCB_FS_Hypo],
                           'EF_CB_FS')

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        self.TErenamingMap = {
            'EF_CB_FS': mergeRemovingOverlap('EF_CB_FS', EFsuf),
            }



# the version run until 2012, kept temporarily for comparing with new one above
class L2EFChain_mu_EFFSonly_2012(L2EFChain_mu_EFFSonly):

    def defineSequences(self, config):
        EFconf = config.EFConfig
        
        if self.ef_chain_name.find('mu2') >= 0:
            theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '0GeV', '0GeV')
        else:
            theTrigMuonEFSA_FS_Hypo = TrigMuonEFExtrapolatorMultiHypoConfig('Muon', '2GeV', '2GeV')

        theTrigMuonEFCB_FS = TrigMuonEFCombinerConfig("TrigMuonEFCombiner_SeededFS")
        theTrigMuonEFCB_FS.IdTracks = "InDetTrigTrackSlimmer_FullScan_EFID"
	
        self.addEFSequence('',
                           [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                           [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA"),
                            theTrigMuonEFSA_FS_Hypo],
                           'EF_SA_FS')

        self.addEFSequence('EF_SA_FS',
                           TrigEFIDInsideOut_FullScan().getSequence(),
                           'EF_FStracksMuon')
        self.addEFSequence('EF_FStracksMuon',
                           [theTrigMuonEFCB_FS,
                            EFconf.EFCB_FS_Hypo],
                           'EF_CB_FS')

        
        
###########################################################################
# EFFSonly
###########################################################################
class L2EFChain_mu_EFFSonly_HI(L2EFChain_mu):

    def __init__(self, sig_id, counter,
                 EFCB_FS_Hypo,
                 l2_inputTEs=None, l2_lower_chain_name=None,
                 n_muons=1, prescale=1, pass_through=0, rerun=0):
        
        self.sig_id  = sig_id
        self.counter = counter
        self.n_muons = n_muons
        
        suffix = '_' + sig_id
        
        l2_chain_name = 'L2_' + sig_id
        l2_chain_counter = counter
        #l2_lower_chain_name = ''
        ef_chain_name = 'EF_' + sig_id
        ef_chain_counter = counter
        #l2_inputTEs = ['']
        
        config = self.Config(self.L2Config(suffix), self.EFConfig(suffix,EFCB_FS_Hypo))
        
        L2EFChain_mu.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through, rerun)
        
    class L2Config:
        def __init__(self, suffix):
            self.suffix = suffix
            pass
        
    class EFConfig:
        def __init__(self, suffix, EFCB_FS_Hypo):
            self.suffix = suffix
            self.EFCB_FS_Hypo = EFCB_FS_Hypo
            pass
        
    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig
                                                                                                        
    def defineSequences(self, config):
        EFconf = config.EFConfig
        
        self.addEFSequence('',
                           [PESA__DummyUnseededAllTEAlgo("EFDummyAlgo")]+
                           [CfgGetter.getAlgorithm("TrigMuSuperEF_FSSA"),
                            EFconf.EFCB_FS_Hypo],
                           'EF_SA_FS')

    def defineTErenaming(self):
        L2suf = self.config.L2Config.suffix
        EFsuf = self.config.EFConfig.suffix
        if self.n_muons == 1:
            self.TErenamingMap = {
                'EF_SA_FS1': mergeRemovingOverlap('EF_CB_FS1', EFsuf),
                }
        else:
            self.TErenamingMap = {
                'EF_SA_FS': mergeRemovingOverlap('EF_CB_FS', EFsuf),
                }
        pass   


###########################################################################
# MSonly, IDTrkNoCut
###########################################################################
class L2EFChain_mu_MSonly(L2EFChain_mu):


    def __init__(self, sig_id, ef_chain_counter, l2_chain_counter=None,
                 l2_inputTEs=None, l2_lower_chain_name=None,
                 prescale=1, pass_through=0, rerun=0):
        global _sigRE

        sigMatch = _sigRE.match(sig_id)
        if sigMatch is None:
            raise RuntimeError( "Signature %r not supported by class %s" % (sig_id,self.__class__.__name__) )

        muThr  = sigMatch.group("muThr")
        if muThr==None:
            raise RuntimeError( "Signature %r not supported by class %s" % (sig_id,self.__class__.__name__) )

        threshold = muThr + 'GeV'
        if 'barrel' in sig_id :
            threshold = threshold + '_barrelOnly'

##fix for mu8_IDTrkNoCut ##savannah##99179
        if threshold=='8GeV':
            threshold='6GeV'    

        muFastThreshold = threshold + '_v11a'



        if l2_chain_counter==None:
            l2_chain_counter = ef_chain_counter

        l2_chain_name = 'L2_' + sig_id
        ef_chain_name = 'EF_' + sig_id

        prefix = '_' + sig_id

        if l2_inputTEs == None: 
            if "_tight" in sig_id:
                l2_inputTEs = ["MU15"]
            elif "_medium" in sig_id:
                l2_inputTEs = ["MU11"]
            else:
                muThrVal = int(muThr)
                if muThrVal >= 10:
                    l2_inputTEs = ["MU10"]
                elif muThrVal >= 6:
                    l2_inputTEs = ["MU6"]
                else:
                    l2_inputTEs = ["MU4"]

        if l2_lower_chain_name == None: 
            l2_lower_chain_name = "L1_" + l2_inputTEs[0]


        self.doOverlapRemoval = 'wOvlpRm' in sig_id
        self.doIDTrk    = 'IDTrkNoCut' in sig_id 
        

        self.algoConf = 'Muon'
        if muThr == '4' :
            self.algoConf = '900GeV'

        config = L2EFChain_mu.Config(L2EFChain_mu.L2Config(prefix,MufastHypoConfig(self.algoConf, muFastThreshold),None),
                                     L2EFChain_mu.EFConfig(prefix,TrigMuonEFExtrapolatorHypoConfig(self.algoConf,threshold)))

        L2EFChain_mu.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through, rerun)

                                 
    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig

        if _GLOBAL_L2_SA_ALG_DEFAULT == 'l2muonSA':
            l2_sa_alg = TrigL2MuonSAConfig(self.algoConf)
        elif _GLOBAL_L2_SA_ALG_DEFAULT == 'muFast':
            l2_sa_alg = theTrigmuFastMuon
            if self.algoConf != 'Muon':
                l2_sa_alg = theTrigmuFast900GeV

        self.addL2Sequence(self.inputTEs[0],
                           [ l2_sa_alg, L2conf.MufastHypo_1],
                           'L2_mu_standalone')

        if self.doOverlapRemoval:
            self.addL2Sequence(['L2_mu_standalone'],
                               [ TrigL2MuonOverlapRemoverConfig('Mufast','nominal') ],
                               'L2_mu_standalone_wOvlpRm')

        if self.doIDTrk:
            if "_R1" in self.sig_id:
                trkseq = [ theTrigIDSCAN_Muon, TrigSiTrack_Muon(),
                           theTrigTRTSegFinder_Muon,
                           theTrigL2SiTrackFinder_MuonA,
                           theTrigL2SiTrackFinder_MuonB,
                           theTrigL2SiTrackFinder_MuonC,
                           ]
                if theTrigL2SiTrackFinder_MuonBC:
                    trkseq.append(theTrigL2SiTrackFinder_MuonBC)
                    
                self.addL2Sequence(self.getNextL2InputTE(),
                                   trkseq+[TrigmuCombConfig(self.algoConf, 'IDSCAN'),
                                           MucombHypoConfig(self.algoConf, 'passthrough')],
                                   'L2_mu')
            else:
                trkseq = [ theTrigL2SiTrackFinder_MuonA,
                           theTrigL2SiTrackFinder_MuonB,
                           theTrigL2SiTrackFinder_MuonC,
                           ]
                if theTrigL2SiTrackFinder_MuonBC:
                    trkseq.append(theTrigL2SiTrackFinder_MuonBC)

                self.addL2Sequence(self.getNextL2InputTE(),
                                   trkseq,
                                   'L2_mu')
            
        #### EF sequences
        if self.doIDTrk:
            last_four_character=self.sig_id[-4:]

            # run ID + combined
            if "_IDT" not in last_four_character:
                self.addEFSequence(self.getNextEFInputTE(),
                                   TrigEFIDInsideOut_Muon().getSequence(),
                                   'EFID_mu')
            else:
                trkseq = list(theTrigEFIDInsideOut_Muon_MergedDP.getSequence())
                try:
                    from TrigFastTrackFinder.TrigFastTrackFinder_Config import TrigFastTrackFinder_Muon
                    trkseq.append(TrigFastTrackFinder_Muon())
                except ImportError:
                    pass

                theTrigEFIDInsideOutMerged_Muon = TrigEFIDSequence("Muon","muon","InsideOutMerged")
                trkseq.append(theTrigEFIDInsideOutMerged_Muon.getSequence())
                self.addEFSequence(self.getNextEFInputTE(),
                                   trkseq,
                                   'EFID_mu')
                
            self.addEFSequence(self.getNextEFInputTE(),
                               TrigEFIDOutsideInTRTOnly_Muon().getSequence(),
                               'EF_trt')
            # NB: Run combined reco but use standalone Hypo
            self.addEFSequence(self.getNextEFInputTE(),
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_TMEFonly"), EFconf.TrigMuonHypo_1],
                               'EF_SA')
            # Run Combined Hypo in passthrough without a FEX (re-using FEX of previous step)
            self.addEFSequence(self.getNextEFInputTE(),
                               [TrigMuonEFCombinerHypoConfig(self.algoConf,'passthrough')],
                               'EF_mu')
        else: # not self.doIDTrk
            self.addEFSequence(self.getNextEFInputTE(),
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_SAonly"), EFconf.TrigMuonHypo_1],
                               'EF_SA')

            if self.doOverlapRemoval:
                # muon standalone only
                self.addEFSequence(self.getNextEFInputTE(),
                                   [ TrigEFMuonOverlapRemoverConfig('MuExtr','loose') ],
                                   'EF_SA_wEFOvlpRm')


###########################################################################
# Muon calibration streamer
###########################################################################
class L2EFChain_mu_cal(L2EFChain_mu):

    _teRE    = re.compile("L1_(?P<teName>[^_]+)")

    def __init__(self, sig_id, l2_chain_counter, l2_lower_chain_name):
        
        teMatch = self._teRE.match(l2_lower_chain_name)
        if teMatch is None:
            raise RuntimeError( "Signature %r not supported by class %s" % (sig_id,self.__class__.__name__) )

        teName = teMatch.group("teName")
        l2_inputTEs = [ teName ]

        l2_chain_name = 'L2_' + sig_id
        ef_chain_name = 'EF_' + sig_id

        prefix = '_' + sig_id

        config = L2EFChain_mu.Config(L2EFChain_mu.L2Config(prefix,None,None),None)

        L2EFChain_mu.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter,
                              l2_lower_chain_name,
                              '', -1 ,
                              l2_inputTEs, config)
    # end of __init__

                                 
    def defineSequences(self, config):
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigmuFastCAL, MufastCALHypoConfig('MuonCal','Reject') ],
                           'L2_mu_cal')
    
    def defineStreamGroupTriggerType(self):
        L2EFChain_mu.defineStreamGroupTriggerType(self)
        self.physics_streams = []
        self.calib_streams = ['Muon_Calibration']
        

###########################################################################
# mu_NoAlg
###########################################################################
class L2EFChain_mu_NoAlg(L2EFChainDef):
    class L2Config:
        def __init__(self):
            self.suffix = ''

    class EFConfig:
        def __init__(self):
            self.suffix = ''

    class Config:
        def __init__(self, L2Config, EFConfig):
            self.L2Config = L2Config
            self.EFConfig = EFConfig
            
    def __init__(self, sig_id,
                 ef_chain_counter, l2_lower_chain_name,
                 prescale=1, pass_through=0):

        l2_chain_name = "L2_" + sig_id
        ef_chain_name = "EF_" + sig_id
        l2_chain_counter = ef_chain_counter

        L2EFChainDef.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              [], self.Config(self.L2Config(),self.EFConfig()),
                              prescale, pass_through)

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        if('EMPTY' in self.l2_lower_chain_name):
            self.groups = ['RATE:Cosmic_Muon','BW:Muon']
        else:
            self.groups = ['RATE:SingleMuon','BW:Muon']

###########################################################################
# out of time
###########################################################################
class L2EFChain_out_of_time(L2EFChain_mu):
    class L2Config:
        def __init__(self, suffix, MufastHypo_1):
            self.suffix = suffix
            self.MufastHypo_1 = MufastHypo_1
            pass

    class EFConfig:
        def __init__(self, suffix):
            self.suffix = suffix
            pass

    def defineSequences(self, config):
        L2conf = config.L2Config
        EFconf = config.EFConfig
        # L2 sequences
        self.addL2Sequence('', 
                           [ TrigmuRoIConfig()], 
                           'L2_mu_out_of_time')
        self.excludeSignature('L2_mu_out_of_time')

        self.addL2Sequence('L2_mu_out_of_time', 
                           [ theTrigmuFastOutBCID, L2conf.MufastHypo_1], 
                           'L2_mu_standalone') 

    def defineStreamGroupTriggerType(self):
        self.physics_streams = ['muons']
        self.calib_streams = []
        self.groups = ['RATE:SingleMuon','BW:Muon']


###########################################################################
# muCombTag_noEF
###########################################################################
class L2EFChain_mu_muCombTag_NoEF(L2EFChain_mu):

    _sigRE = re.compile("mu(?P<muThr>\d+)(?P<lowPtTight>T)?(?P<postfix>.*)")
    _teRE  = re.compile("L1_(?P<teName>.*)")

    def __init__(self, sig_id, ef_chain_counter, l2_lower_chain_name, l2_chain_counter=None,
                 prescale=1, pass_through=0, rerun=0):

        sigMatch = self._sigRE.match(sig_id)
        teMatch  = self._teRE.match(l2_lower_chain_name)
        if sigMatch is None or teMatch is None:
            raise RuntimeError( "Signature %r not supported by class %s" % (sig_id,self.__class__.__name__) )

        muThr  = sigMatch.group("muThr")
        teName = teMatch.group("teName")
        if muThr==None or teName==None :
            raise RuntimeError( "Signature %r not supported by class %s" % (sig_id,self.__class__.__name__) )

        threshold = muThr + 'GeV'
        l2_inputTEs = [ teName ]

        if l2_chain_counter==None:
            l2_chain_counter = ef_chain_counter

        l2_chain_name = 'L2_' + sig_id
        ef_chain_name = 'EF_' + sig_id

        prefix = '_' + sig_id

        self.algoConf = 'Muon'
        if muThr == '4' :
            self.algoConf = '900GeV'

        config = L2EFChain_mu.Config(L2EFChain_mu.L2Config(prefix,None,MucombHypoConfig(self.algoConf, threshold)),
                                     L2EFChain_mu.EFConfig(prefix,None))
        
        L2EFChain_mu.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through, rerun)

    def defineSequences(self, config):
        L2conf = config.L2Config
        self.addL2Sequence(self.inputTEs[0],
                           [ theTrigL2SiTrackFinder_MuonA, TrigmuCombConfig(self.algoConf, 'STRATEGY_A'), L2conf.MucombHypo_1],
                           'L2_mu')


###########################################################################
# NSW emulation chain
###########################################################################
class L2EFChain_mu_NSW_emulation(L2EFChain_mu):
    
    _noL2RE = re.compile("NoL2")
    _noEFRE = re.compile("NoEF")
    
    def __init__(self, sig_id, ef_chain_counter, l2_lower_chain_name=None, l2_chain_counter=None,
                 prescale=1, pass_through=0, rerun=0):
        
        sigMatch = _sigRE.match(sig_id)
        muThr = sigMatch.group("muThr")
        if muThr==None :
            raise RuntimeError( "Signature %r not supported by class %s" % (sig_id,self.__class__.__name__) )
        
        self.noL2Match = self._noL2RE.search(sig_id)
        self.noEFMatch = self._noEFRE.search(sig_id)
        
        if muThr == '11':
            l2_inputTEs = ["MU11"]
        elif muThr == '15':
            l2_inputTEs = ["MU15"]
        elif muThr == '20':
            l2_inputTEs = ["MU20"]
        else:
            raise RuntimeError("Threshold Signature %r not supported by class %s" % (sig_id,self.__class__.__name__) )
        
        if l2_chain_counter==None:
            l2_chain_counter = ef_chain_counter
            
        l2_lower_chain_name = 'L1_' + l2_inputTEs[0]
        l2_chain_name = 'L2_' + sig_id
        ef_chain_name = 'EF_' + sig_id
        
        prefix = '_' + sig_id
        
        config = L2EFChain_mu.Config(L2EFChain_mu.L2Config(prefix,None,None),
                                     L2EFChain_mu.EFConfig(prefix,None))
        
        L2EFChain_mu.__init__(self, sig_id,
                              l2_chain_name, l2_chain_counter, l2_lower_chain_name,
                              ef_chain_name, ef_chain_counter,
                              l2_inputTEs, config,
                              prescale, pass_through, rerun)
        
    def defineSequences(self, config):
        if self.noL2Match == None :
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, MufastNSWHypoConfig('Muon') ],
                               'L2_mu')
        else :
            self.addL2Sequence(self.inputTEs[0],
                               [ theTrigmuFastMuon, MufastHypoConfig('Muon','passthrough') ],
                               'L2_mu')
            
        if self.noEFMatch == None :
            self.addEFSequence('L2_mu',
                               [CfgGetter.getAlgorithm("TrigMuSuperEF_SAonly"), TrigMuonEFExtrapolatorNSWHypoConfig('Muon')],
                               'EF_SA')
            
                
#####################################################################
    
if __name__ == '__main__':
    triggerPythonConfig = TriggerPythonConfig('hlt.xml', None)
    for m in Muons:
        m.generateMenu(triggerPythonConfig)
    triggerPythonConfig.writeConfigFiles()
    

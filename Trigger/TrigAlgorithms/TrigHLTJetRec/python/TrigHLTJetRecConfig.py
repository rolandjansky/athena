# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV
from TrigHLTJetRec.TrigHLTJetRecConf import (TrigHLTJetRec,
                                             TriggerPseudoJetGetter,
                                             TrigHLTCellDiagnostics,
                                             TrigHLTClusterDiagnostics,
                                             TrigHLTJetDiagnostics,
                                             TrigHLTHypoDiagnostics,
                                             TrigHLTPSvsFSDiagnostics)
from JetRec.JetRecConf import JetRecTool
from JetRec.JetRecConf import (JetFromPseudojet,
                               JetFinder)

# Import message level flags.
from GaudiKernel.Constants import (DEBUG,
                                   INFO)

# Import the jet reconstruction control flags.
from JetRec.JetRecFlags import jetFlags
# Flag to show messges while running.
#   0 - no messages
#   1 - Indicates which jetrec tool is running
#   2 - Details about jetrec execution including which modfier
#   3 - Plus messages from the finder
#   4 - Plus messages from the jet builder
jetFlags.debug = 0
#jetFlags.debug = 4



def _getJetBuildTool(merge_param, ptmin, ptminFilter):
    """Set up offline tools"""

    msg = 'Naming convention breaks with merge param %d' % merge_param
    int_merge_param = int(10 * merge_param)
    assert 10 * merge_param == int_merge_param, msg

    assert merge_param > 0.
    # ------------ originally from myjets.py
    from JetRec.JetRecStandard import jtm

    # useTriggerStore is now obsolete 3/7/2014 PS.
    # Flag to signal the appropriate xAOD aux container
    # for xAOD jet containers used in the trigger environment.
    # jtm.useTriggerStore = True

    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
    try:
        triggerPseudoJetGetter = jtm.triggerPseudoJetGetter
    except AttributeError:
        # Add TriggerPSeudoJEtGetter to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContanerInfo.h
        jtm += TriggerPseudoJetGetter('triggerPseudoJetGetter', Label='LCTopo')
        triggerPseudoJetGetter = jtm.triggerPseudoJetGetter
        
    mygetters = [triggerPseudoJetGetter]
    jtm.gettersMap["mygetters"] = mygetters
    # print jtm.gettersMap.keys()
    
    # Build a new list of jet modifiers.
    mymods = [
        #  "applyCalibrationTool", 
        jtm.nsubjettiness,
        jtm.jetens,
        jtm.caloqual_cluster,
        jtm.bchcorrclus,
        jtm.width
        ]

    jtm.modifiersMap["mymods"] = mymods
    # print "DEBUG TrigHLTJetRecConfig line MUST be removed"
    # jtm.modifiersMap["mymods"] = []

    
    name = 'MyAntiKt%dEMTopoJets' % int_merge_param
    # jetBuildTool = _get_jetfinder(name, jtm)

    # if not jetBuildTool:
    # print 'making a jetrec', name
    jtm.addJetFinder(name,
                     "AntiKt",
                     merge_param,
                     "mygetters",
                     "mymods",
                     ghostArea=0.01,
                     rndseed=0,
                     isTrigger=True,
                     ptmin=ptmin,
                     ptminFilter=ptminFilter
                     )
        
    jetBuildTool = jtm.trigjetrecs[-1]
    # jetBuildTool = jtm.jetrecs[0]         #//added temporarily
    
    # ------------myjets.py import JetAlgorithm here. Copy the code
    # from JetAlgorithm
    # Add the tool runner. It runs the jetrec tools.
    rtools = []
    if jetFlags.useCells():
        rtools += [jtm.missingcells]
    if jetFlags.useTracks:
        rtools += [jtm.tracksel]
        rtools += [jtm.tvassoc]
        rtools += jtm.jetrecs

    # from JetRec.JetRecConf import JetToolRunner
    # jtm += JetToolRunner("jetrun", Tools=rtools)
    # jetrun = jtm.jetrun

    # print 'TrigHLTJetRec: jetFlags.debug', jetFlafs.debug
    # if jetFlags.debug > 0:
    #    jtm.setOutputLevel(jetBuildTool, DEBUG)

    return jetBuildTool, triggerPseudoJetGetter

def _get_jetfinder(name, jtm):
    """Return the object in the jetrecs list if it has the required name, None
    otherwise."""

    jetrecs = getattr(jtm, 'jetrecs')
    if not jetrecs:
        return None

    jetrec = [j for j in jetrecs if j.name() == name]
    if not jetrec:
        return None

    assert len(jetrecs) == 1
    return jtm.jetrecs[0]
    
class TrigHLTJetRec_AntiKt04(TrigHLTJetRec):
    """Supply a specific merge parameter for the anti-kt algorithm"""
    def __init__(self,
                 name="TrigHLTJetRec_AntiKt04",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV):
        TrigHLTJetRec.__init__(self, name)
        self.jetBuildTool, self.pseudoJetGetter = _getJetBuildTool(
            0.4,
            ptmin=ptmin,
            ptminFilter=ptminFilter
            )

        # if jetFlags.debug > 0:
        #    self.OutputLevel = DEBUG
        # else:
        # self.OutputLevel = INFO

        

class TrigHLTJetRec_AntiKt10(TrigHLTJetRec):
    """Supply a specific merge parameter for the anti-kt algorithm"""
    def __init__(self,  name="TrigHLTJetRec_AntiKt10",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV):
        TrigHLTJetRec.__init__(self, name)

        self.jetBuildTool, self.pseudoJetGetter = _getJetBuildTool(
            1.0,
            ptmin=ptmin,
            ptminFilter=ptminFilter)
        
        # if jetFlags.debug > 0:
        #    self.OutputLevel = DEBUG
        # else:
        # self.OutputLevel = INFO
        

class TrigHLTCellDiagnostics_named(TrigHLTCellDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTCellDiagnostics_named"):
        TrigHLTCellDiagnostics.__init__(self, name)
        self.chainName = chain_name
        

class TrigHLTClusterDiagnostics_named(TrigHLTClusterDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTClusterDiagnostics_named"):
        TrigHLTClusterDiagnostics.__init__(self, name)
        self.chainName = chain_name
        

class TrigHLTJetDiagnostics_named(TrigHLTJetDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTJetDiagnostics_named"):
        TrigHLTJetDiagnostics.__init__(self, name)
        self.chainName = chain_name

class TrigHLTHypoDiagnostics_named(TrigHLTHypoDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTHypoDiagnostics_named"):
        TrigHLTHypoDiagnostics.__init__(self, name)
        self.chainName = chain_name
          
        
class TrigHLTPSvsFSDiagnostics_named(TrigHLTPSvsFSDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTPSvsFSDiagnostics_named"):
        TrigHLTPSvsFSDiagnostics.__init__(self, name)
        self.chainName = chain_name

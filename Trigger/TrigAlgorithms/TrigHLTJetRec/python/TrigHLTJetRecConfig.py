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



def _getJetBuildTool(merge_param,
                     ptmin,
                     ptminFilter,
                     do_jes,
                     cluster_calib,
                     do_minimalist_setup):
    """Set up offline tools. do_minimalist_setup controls whether
    jetRecTool is set up with the minimum required jet modifiers.
    The code can be exercised with do_minimalist_setup=False to ensure
    that it possible to run with with a more complex set of modifiers."""

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
        Label = cluster_calib + 'Topo'
        jtm += TriggerPseudoJetGetter('triggerPseudoJetGetter', Label=Label)
        triggerPseudoJetGetter = jtm.triggerPseudoJetGetter
        
    mygetters = [triggerPseudoJetGetter]
    jtm.gettersMap["mygetters"] = mygetters
    # print jtm.gettersMap.keys()
    
    # Build a new list of jet modifiers.
    mymods = []

    # 19/9/2014 no jes yet
    if do_jes:
        # Tool to apply post jet clustering calibration
        mymods.append("applyCalibrationTool:jes")

    if not do_minimalist_setup:
        # add in extra modofiers. This allows monitoring the ability
        # to run with the extra modifiers, which may be required in the
        # future.
        mymods.extend([
                jtm.nsubjettiness,
                jtm.jetens,
                jtm.caloqual_cluster,
                jtm.bchcorrclus,
                jtm.width
                ])

    jtm.modifiersMap["mymods"] = mymods
    
    # name = 'MyAntiKt%d%sTopoJets' % (int_merge_param, cluster_calib)
    name = 'TrigAntiKt%d%sTopoJets' % (int_merge_param, cluster_calib)

    def findjetBuildTool():
        for jr in jtm.trigjetrecs:
            if jr.OutputContainer == name:
                # jr.OutputContainer is a string, here used to identify the
                # object
                return jr
        return None

    jetBuildTool = findjetBuildTool()

    if jetBuildTool is None:
        print 'adding new jet finder ', name
        try:
            jetBuildTool = jtm.addJetFinder(name,
                                            "AntiKt",
                                            merge_param,
                                            "mygetters",
                                            "mymods",
                                            # non-zero ghostArea: calcjet area
                                            # for pileup subtraction.
                                            ghostArea=0.01,
                                            rndseed=0,
                                            isTrigger=True,
                                            ptmin=ptmin,
                                            ptminFilter=ptminFilter
                                            )
        except Exception, e:
            print 'error adding jet finder %s' % name
            for jr in jtm.trigjetrecs:
                print jr
            raise e

    else:
        print 'found jet finder ', name

    # jetBuildTool = jtm.trigjetrecs[-1]
    
    # ------------myjets.py import JetAlgorithm here. Copy the code
    # from JetAlgorithm
    # Add the tool runner. It runs the jetrec tools.


    # rtools deal with tracks
    # rtools = []
    # if jetFlags.useCells():
    #     rtools += [jtm.missingcells]
    # if jetFlags.useTracks:
    #   rtools += [jtm.tracksel]
    #    rtools += [jtm.tvassoc]
    #    rtools += jtm.jetrecs

    return jetBuildTool, triggerPseudoJetGetter


class TrigHLTJetRec_param(TrigHLTJetRec):
    """Supply a specific merge parameter for the anti-kt algorithm"""
    def __init__(self,
                 name,
                 alg="AntiKt",
                 merge_param="04",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 do_jes=False,
                 cluster_calib='EM',
                 do_minimalist_setup=True,
                 ):
        TrigHLTJetRec.__init__(self,
                               # name='TrigHLTJetRec_%s%s' % (alg, merge_param),
                               name=name,
                               cluster_calib=cluster_calib)

        self.jetBuildTool, self.pseudoJetGetter = _getJetBuildTool(
            float(int(merge_param))/10.,
            ptmin=ptmin,
            ptminFilter=ptminFilter,
            do_jes=do_jes,
            cluster_calib=cluster_calib,
            do_minimalist_setup=do_minimalist_setup,
            )


#  class TrigHLTJetRec_AntiKt04 is maintained only for the HT configuration
class TrigHLTJetRec_AntiKt04(TrigHLTJetRec):
    """Supply a specific merge parameter for the anti-kt algorithm"""
    def __init__(self,
                 name="TrigHLTJetRec_oldconfig_AntiKt04",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 do_jes=False,
                 cluster_calib='EM',
                 do_minimalist_setup=True):
        TrigHLTJetRec.__init__(self, name)
        self.jetBuildTool, self.pseudoJetGetter = _getJetBuildTool(
            0.4,
            ptmin=ptmin,
            ptminFilter=ptminFilter,
            do_jes=do_jes,
            cluster_calib=cluster_calib,
            do_minimalist_setup=do_minimalist_setup,
            )

        # if jetFlags.debug > 0:
        #    self.OutputLevel = DEBUG
        # else:
        # self.OutputLevel = INFO

        

#  class TrigHLTJetRec_AntiKt10 is maintained only for the HT configuration
class TrigHLTJetRec_AntiKt10(TrigHLTJetRec):
    """Supply a specific merge parameter for the anti-kt algorithm"""
    def __init__(self,  name="TrigHLTJetRec_oldconfig_AntiKt10",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 do_jes=False,
                 cluster_calib='EM',
                 do_minimalist_setup=True):
        
        TrigHLTJetRec.__init__(self, name)

        self.jetBuildTool, self.pseudoJetGetter = _getJetBuildTool(
            1.0,
            ptmin=ptmin,
            ptminFilter=ptminFilter,
            do_jes=do_jes,
            cluster_calib=cluster_calib,
            do_minimalist_setup=True)
        
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

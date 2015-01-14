# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV
# from TrigHLTJetRec.TrigHLTJetRecConf import (TrigHLTJetRec,
#                                             TriggerPseudoJetGetter,
#                                             TrigHLTCellDiagnostics,
#                                             TrigHLTClusterDiagnostics,
#                                             TrigHLTJetDiagnostics,
#                                             TrigHLTHypoDiagnostics,
#                                             TrigHLTPSvsFSDiagnostics,
#                                             TrigHLTEnergyDensity,
#                                             TrigHLTJetRec_Cluster,
#                                             TrigHLTJetRec_Jet,
#                                         )

import TrigHLTJetRecConf
from JetRec.JetRecConf import JetRecTool
from JetRec.JetRecConf import (JetFromPseudojet,
                               JetFinder)

from EventShapeTools.EventDensityConfig import configEventDensityTool

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


from JetRec.JetRecStandard import jtm
from JetRec.JetRecCalibrationFinder import jrcf

cfg_file = "JES_Full2012dataset_Preliminary_Trigger_NoPileup.config"
jrcf.configDict["triggerNoPileup"] = cfg_file

from EventShapeTools.EventShapeToolsConf import EventDensityTool

# quasi cut and past of configEventDensityTool in EventDensityConfig.py
# that creates the name in the format expected by the pileup calibration tool.
def configHLTEventDensityTool( name, pjGetter, radius, **options ):
    """ options can be used to pass any EventDensityTool properties 
    """
    # Set default and passed properties for the EventDensityTool
    toolProperties = dict(
        JetAlgorithm        = "Kt",
        JetRadius           = radius,
        JetInput            = pjGetter,
        AbsRapidityMin      = 0.0,
        AbsRapidityMax      = 2.0,
        AreaDefinition      = "Voronoi",
        VoronoiRfact        = 0.9,
        OutputContainer     = "HLTKt"+str(int(10*radius))+pjGetter.Label + "EventShape",
    )
    # Override properties with user-supplied options.
    toolProperties.update( options)
    # Build the tool :
    return EventDensityTool(name, **toolProperties)
    

def _getJetBuildTool(merge_param,
                     ptmin,
                     ptminFilter,
                     jet_calib,
                     cluster_calib,
                     do_minimalist_setup,
                     name=''):
    """Set up offline tools. do_minimalist_setup controls whether
    jetRecTool is set up with the minimum required jet modifiers.
    The code can be exercised with do_minimalist_setup=False to ensure
    that it possible to run with with a more complex set of modifiers."""

    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm

    msg = 'Naming convention breaks with merge param %d' % merge_param
    int_merge_param = int(10 * merge_param)
    assert 10 * merge_param == int_merge_param, msg

    assert merge_param > 0.

    _is_calibration_supported(int_merge_param, jet_calib, cluster_calib)

    mygetters = [_getTriggerPseudoJetGetter(cluster_calib)]
    jtm.gettersMap["mygetters"] = mygetters
    # print jtm.gettersMap.keys()
    
    # tell the offline code which calibration is requested
    calib_str = {'jes': 'calib:j:triggerNoPileup:HLTKt4',
                 'subjes': 'calib:aj:trigger:HLTKt4',
                 'sub': 'calib:a:trigger:HLTKt4'}.get(jet_calib, '')
    
    mymods = [calib_str] if calib_str else []
    

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
    # DEBUG DEBUG DEBUG
    # mymods = []
    # DEBUG DEBUG DEBUG

    jtm.modifiersMap["mymods"] = mymods
    
    # name = 'TrigAntiKt%d%sTopoJets' % (int_merge_param, cluster_calib)

    if not name:
        name = 'TrigAntiKt%d%s%sTopoJets' % (int_merge_param,
                                             cluster_calib,
                                             jet_calib)

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
            print 'error adding new jet finder %s' % name
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

    return jetBuildTool


def _is_calibration_supported(int_merge_param, jet_calib, cluster_calib):
    """Check if the calibration is supported by the offline code"""

    do_jes = 'jes' in jet_calib

    bad_combinations = [
        {'merge_param': 10, 'do_jes': True, 'cluster_calib': 'EM'}]
    
    if {'merge_param': int_merge_param,
        'do_jes': do_jes,
        'cluster_calib': cluster_calib} in bad_combinations:

        m = 'TrigHLTJetRecConfig._getJetBuildTool: no calibration support'\
            ' for merge_param %s, cluster calib %s, jes calibration: %s' % (
                str(int_merge_param), str(cluster_calib), str(do_jes))
        raise RuntimeError(m)


def _getTriggerPseudoJetGetter(cluster_calib):
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]

    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm

    label = cluster_calib + 'Topo'
    pjg_name = 'triggerPseudoJetGetter_%s' % label

    try:
        triggerPseudoJetGetter = getattr(jtm, pjg_name)
    except AttributeError:
        # Add TriggerPSeudoJetGetter to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContainerInfo.h
        pjg = TriggerPseudoJetGetter(pjg_name, label=label)

        # adds arg to Tool Service and returns arg
        jtm.add(pjg)
        triggerPseudoJetGetter = getattr(jtm, pjg_name)

    return triggerPseudoJetGetter

    
def _getEnergyDensityTool(name, pjGetter, radius, **options):

    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm
    
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
    try:
        energyDensityTool = getattr(jtm, name)
    except AttributeError:
        # Add the EnergyDensity to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContainerInfo.h
        energyDensityTool = configHLTEventDensityTool(name,
                                                      pjGetter,
                                                      radius,
                                                      **options)
        jtm += energyDensityTool
        energyDensityTool = getattr(jtm, name)
        print 'TrigHLTJetRecConfig._getEnergyDensityTool '\
            'Added energyDensity tools "%s" to jtm' % name

    return energyDensityTool

    
def _getPseudoJetSelectorAll(name, **options):

    # set up a tool to select all pseudo jets
    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm
    
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
    try:
        selector = getattr(jtm, name)
    except AttributeError:
        # Add the PseudoJetSelectorAll to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContainerInfo.h
        selector = TrigHLTJetRecConf.PseudoJetSelectorAll(name=name)
        jtm += selector
        selector = getattr(jtm, name)
        print 'TrigHLTJetRecConfig._getPseudoJetSelectorAll '\
            'Added selector "%s" to jtm' % name

    return selector


def _getPseudoJetSelectorEtaPt(name, etaMax, ptMin):

    # set up a tool to select all pseudo jets
    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm
    
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
    try:
        selector = getattr(jtm, name)
    except AttributeError:
        # Add the PseudoJetSelectorEtaPt to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContainerInfo.h
        selector = PseudoJetSelectorEtaPt(name, etaMax, ptMin)

        jtm += selector
        selector = getattr(jtm, name)
        print 'TrigHLTJetRecConfig._getPseudoJetSelectorEtaPt '\
            'Added selector "%s" to jtm' % name

    return selector

    
    
def _getIParticleSelectorAll(name, **options):

    # set up a tool to select all pseudo jets
    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm
    
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
    try:
        selector = getattr(jtm, name)
    except AttributeError:
        # Add the IParticleSelectorAll to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContainerInfo.h
        selector = TrigHLTJetRecConf.IParticleSelectorAll(name=name)
        jtm += selector
        selector = getattr(jtm, name)
        print 'TrigHLTJetRecConfig._getIParticleSelectorAll '\
            'Added selector "%s" to jtm' % name

    return selector


def _getIParticleSelectorEtaPt(name, **kwds):

    # set up a tool to select all pseudo jets
    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm
    
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
    try:
        selector = getattr(jtm, name)
    except AttributeError:
        # Add the IParticleSelectorEtaPt to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContainerInfo.h
        selector = IParticleSelectorEtaPt(name, **kwds)

        jtm += selector
        selector = getattr(jtm, name)
        print 'TrigHLTJetRecConfig._getIParticleSelectorEtaPt '\
            'Added selector "%s" to jtm' % name

    return selector

class TrigHLTJetRec_param(TrigHLTJetRecConf.TrigHLTJetRec):
    """ DEPRECATED ALGORITHM
    Supply a specific merge parameter for the anti-kt algorithm"""

    def __init__(self,
                 name,
                 alg="AntiKt",
                 merge_param="04",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 jet_calib='subjes',
                 cluster_calib='EM',
                 do_minimalist_setup=True,
                 output_collection_label='defaultJetCollection'
                 ):
        # TrigHLTJetRecConf.TrigHLTJetRec.__init__(
        #    self,
        #    # name=name + '_DEPRECATED',
        #    name=name,
        #    cluster_calib=cluster_calib)

        print 'TrigHLTJetRec_param is DEPRECATED'

        TrigHLTJetRecConf.TrigHLTJetRec.__init__(self, name=name)

        self.cluster_calib = cluster_calib

        self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        self.jetBuildTool = _getJetBuildTool(
            float(int(merge_param))/10.,
            ptmin=ptmin,
            ptminFilter=ptminFilter,
            jet_calib=jet_calib,
            cluster_calib=cluster_calib,
            do_minimalist_setup=do_minimalist_setup,
            )

        self.output_collection_label = output_collection_label


class TrigHLTJetRecFromCluster(TrigHLTJetRecConf.TrigHLTJetRecFromCluster):
    """Supply a specific merge parameter for the anti-kt algorithm"""
    def __init__(self,
                 name,
                 alg="AntiKt",
                 merge_param="04",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 jet_calib='subjes',
                 cluster_calib='EM',
                 do_minimalist_setup=True,
                 output_collection_label='defaultJetCollection',
                 pseudojet_labelindex_arg='PseudoJetLabelMapTriggerFromCluster'
                 ):
        TrigHLTJetRecConf.TrigHLTJetRecFromCluster.__init__(self, name = name)

        self.cluster_calib = cluster_calib
        self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        
        self.iParticleSelector = _getIParticleSelectorAll(
            'iParticleSelectorAll')

        self.jetBuildTool = _getJetBuildTool(
            float(int(merge_param))/10.,
            ptmin=ptmin,
            ptminFilter=ptminFilter,
            jet_calib=jet_calib,
            cluster_calib=cluster_calib,
            do_minimalist_setup=do_minimalist_setup,
            name=name
            )

        self.output_collection_label = output_collection_label
        self.pseudojet_labelindex_arg = pseudojet_labelindex_arg

class TrigHLTJetRecFromJet(TrigHLTJetRecConf.TrigHLTJetRecFromJet):
    """Run jet reclustering. Jet calibration is off, merge parameter 10."""
    def __init__(self,
                 name,
                 alg='AntiKt',
                 merge_param='10',
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 jet_calib='nojcalib',  # no calibration to be done
                 cluster_calib='EM',
                 do_minimalist_setup=True,
                 output_collection_label='reclusteredJets',  # do not use this
                 pseudojet_labelindex_arg='PseudoJetLabelMapTriggerFromJet',
                 ptMinCut=15000.,
                 etaMaxCut=3.2,
                 ):
        TrigHLTJetRecConf.TrigHLTJetRecFromJet.__init__(self, name=name)

        self.cluster_calib = cluster_calib
        self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        self.iParticleSelector = _getIParticleSelectorEtaPt(
            'iParticleSelectorEtaPt_%d_%d' % (int(10 * etaMaxCut),
                                              int(ptMinCut)),
            **{'etaMax': etaMaxCut, 'ptMin': ptMinCut})


        self.jetBuildTool = _getJetBuildTool(
            float(int(merge_param))/10.,
            ptmin=ptMinCut,
            ptminFilter=ptminFilter,
            jet_calib=jet_calib,
            cluster_calib=cluster_calib,
            do_minimalist_setup=do_minimalist_setup,
            )

        self.output_collection_label = output_collection_label
        self.pseudojet_labelindex_arg = pseudojet_labelindex_arg


#  class TrigHLTJetRec_AntiKt04 is maintained only for the HT configuration
class TrigHLTJetRec_AntiKt04(TrigHLTJetRecConf.TrigHLTJetRec):
    """Supply a specific merge parameter for the anti-kt algorithm"""
    def __init__(self,
                 name="TrigHLTJetRec_DEPRECATED_AntiKt04",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 do_jes=False,
                 cluster_calib='EM',
                 do_minimalist_setup=True):
        TrigHLTJetRecConf.TrigHLTJetRec.__init__(self, name)

        self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        self.jetBuildTool = _getJetBuildTool(
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
class TrigHLTJetRec_AntiKt10(TrigHLTJetRecConf.TrigHLTJetRec):
    """Supply a specific merge parameter for the anti-kt algorithm"""
    def __init__(self,  name="TrigHLTJetRec_DEPRECATED_AntiKt10",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 do_jes=False,
                 cluster_calib='EM',
                 do_minimalist_setup=True):
        
        TrigHLTJetRecConf.TrigHLTJetRec.__init__(self, name)

        self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        self.jetBuildTool  = _getJetBuildTool(
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


class TrigHLTCellDiagnostics_named(TrigHLTJetRecConf.TrigHLTCellDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTCellDiagnostics_named"):
        TrigHLTJetRecConf.TrigHLTCellDiagnostics.__init__(self, name)
        self.chainName = chain_name
        

class TrigHLTClusterDiagnostics_named(
        TrigHLTJetRecConf.TrigHLTClusterDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTClusterDiagnostics_named"):
        TrigHLTJetRecConf.TrigHLTClusterDiagnostics.__init__(self, name)
        self.chainName = chain_name
        

class TrigHLTJetDiagnostics_named(TrigHLTJetRecConf.TrigHLTJetDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTJetDiagnostics_named"):
        TrigHLTJetRecConf.TrigHLTJetDiagnostics.__init__(self, name)
        self.chainName = chain_name

class TrigHLTHypoDiagnostics_named(TrigHLTJetRecConf.TrigHLTHypoDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTHypoDiagnostics_named"):
        TrigHLTJetRecConf.TrigHLTHypoDiagnostics.__init__(self, name)
        self.chainName = chain_name
          
        
class TrigHLTPSvsFSDiagnostics_named(TrigHLTJetRecConf.TrigHLTPSvsFSDiagnostics):
    """Supply a chain name used to label output files"""
    def __init__(self,  chain_name, name="TrigHLTPSvsFSDiagnostics_named"):
        TrigHLTJetRecConf.TrigHLTPSvsFSDiagnostics.__init__(self, name)
        self.chainName = chain_name


class TrigHLTEnergyDensity(TrigHLTJetRecConf.TrigHLTEnergyDensity):
    """Supply a specific merge parameter for the anti-kt algorithm"""

    # cluster_calib: EM or LC
    def __init__(self, name, cluster_calib, ed_merge_param): 
        TrigHLTJetRecConf.TrigHLTEnergyDensity.__init__(self, name=name)
        
        # share the pseudojet getter betwen the algorithm an the tool.
        # the alg passes pseudojets to the pseudojetgetter. The
        # energy density tool obtains the oseudojets from the pseudojet getter.
        pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        # attribute operator causes type conversion.
        # e.g. instance conversion from <TriggerPseudoJetGetter/
        # ToolSvc.triggerPseudoJetGetter_EMTopo at 0x3444620>
        # to PublicToolHandle('TriggerPseudoJetGetter/
        # triggerPseudoJetGetter_EMTopo')
        self.pseudoJetGetter = pseudoJetGetter  

        # ed_merge_param is a float e.g. 0.4
        self.energyDensityTool = _getEnergyDensityTool(
            'jetTriggerEnergyDensityTool_%s' % cluster_calib,
            pseudoJetGetter,
            ed_merge_param)


# Data scouting algorithm
class TrigHLTJetDSSelector(TrigHLTJetRecConf.TrigHLTJetDSSelector):
    def __init__(self, name,
                 jetCollectionName,
                 jetPtThreshold,
                 maxNJets,
                 **kwds):
        TrigHLTJetRecConf.TrigHLTJetDSSelector.__init__(self, name, **kwds)
        self.jetCollectionName = jetCollectionName
        self.jetPtThreshold = jetPtThreshold
        self.maxNJets = maxNJets


class TriggerPseudoJetGetter(TrigHLTJetRecConf.TriggerPseudoJetGetter):
    def __init__(self, name, label):
        TrigHLTJetRecConf.TriggerPseudoJetGetter.__init__(self, name)        
        self.Label = label


class PseudoJetSelectorEtaPt(TrigHLTJetRecConf.PseudoJetSelectorEtaPt):
    def __init__(self, name, etaMax, ptMin):
        TrigHLTJetRecConf.PseudoJetSelectorEtaPt.__init__(self, name)        
        self.etaMax = etaMax
        self.ptMin = ptMin


class IParticleSelectorEtaPt(TrigHLTJetRecConf.IParticleSelectorEtaPt):
    def __init__(self, name, **kwds):
        TrigHLTJetRecConf.IParticleSelectorEtaPt.__init__(self, name, **kwds)


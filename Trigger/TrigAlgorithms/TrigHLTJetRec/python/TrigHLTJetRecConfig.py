# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.SystemOfUnits import GeV

import TrigHLTJetRecConf
from JetRec.JetRecConf import JetRecTool
from JetRec.JetRecConf import (JetFromPseudojet,
                               JetFinder,JetToolRunner)

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
# Calibration Configurarion file (now defined in offline code)
#cfg_file = "JES_Full2012dataset_Preliminary_Trigger_NoPileup.config"
#jrcf.configDict["triggerNoPileup"] = cfg_file

from EventShapeTools.EventShapeToolsConf import EventDensityTool

# quasi cut and paste of configEventDensityTool in EventDensityConfig.py
# that creates the name in the format expected by the pileup calibration tool.


def _getEventShapeSGKey(radius, pseudoJetGetterLabel):
    return  "HLTKt"+str(int(10*radius))+pseudoJetGetterLabel + "EventShape"


def configHLTEventDensityTool(name,
                              pjGetter,
                              OutputContainer,
                              JetAlgorithm='Kt',
                              radius=0.4,
                              AbsRapidityMin=0.0,
                              AbsRapidityMax=2.0,
                              AreaDefinition='Voronoi',
                              VoronoiRfact=0.9,
                              **options):
    """Set up the energy denisty tool. Defaults can be overridden by
    adding arguments to **options"""


    toolProperties = dict(
        JetAlgorithm        = JetAlgorithm,
        JetRadius           = radius,
        JetInput            = pjGetter,
        AbsRapidityMin      = AbsRapidityMin,
        AbsRapidityMax      = AbsRapidityMax,
        AreaDefinition      = AreaDefinition,
        VoronoiRfact        = VoronoiRfact,
        OutputContainer     = OutputContainer,
    )

    # Build the tool :
    return EventDensityTool(name, **toolProperties)
    


def _getTrimmedJetCalibrationModifier(jet_calib,int_merge_param,cluster_calib,rclus,ptfrac):
    from JetRec.JetRecCalibrationFinder import jrcf
    
    # Full list of arguments to jrcf (jet rec calib finder) are:
    #   alg = algorithm, AntiKt
    #   rad = radius, 1.0
    #   inp = constituent+input type, LCTopoTrimmedPtFrac5SmallR20
    #   seq = calibration sequence, EtaJES_JMS
    #   config = configuration key, triggerTrim
    #   evsprefix = event shape prefix, HLTKt4
    #
    # We will assume this is alg = AntiKt
    # We will assume this is trimmed (per the function name)
    # We will assume that this is JES+JMS calibration
    # The rest we can get from arguments
    # Calibration string should be in following form: calib:seq:config:evsprefix

    # Prepare an error message in case it is needed
    error = 'TrigHLTJetRecConfig._getTrimmedJetCalibrationModifier: '\
            'request calibration is not supported (%s), '
    error += ('merge_param %s, cluster_calib %s, jet_calib %s'\
            % (str(int_merge_param), str(cluster_calib), str(jet_calib)))


    # Check the easy case of no calibration
    if jet_calib == 'nojcalib':
        return None

    # If this is EM, then there is currently no calibration
    # If we got past nojcalib and it's EM, then this is an error
    if cluster_calib != 'LC':
        raise RuntimeError(error%("Trimmed calibrations only exist for LC"))

    # If we are here, then this is LC and it's not nojcalib
    # In this case, the only sane option is jes (but we interpret this as JES+JMS)
    if jet_calib != 'jes':
        raise RuntimeError(error%("only jes calibration is supported for LC"))

    # We only have calibrations for R=1.0 jets right now
    if int_merge_param != 10:
        raise RuntimeError(error%("only R=1.0 is supported"))

    # We only have calibrations for rclus=0.2, ptfrac=0.05
    # However, we use ptfrac=0.04 to avoid resolution problems with the ptfrac=0.05 calibrations
    if rclus != 0.2 or ptfrac != 0.04:
        raise RuntimeError(error%("only rclus=0.2 and ptfrac=0.04 (in place of 0.05) are supported"))
    
    # If we got here, everything checks out
    # Do a generic build from the arguments
    # This is to be future-proof, as right now there is only a single calibration option...

    alg="AntiKt"
    rad=float(int_merge_param)/10.
    #inp="%sTopoTrimmedPtFrac%sSmallR%s"%(cluster_calib,str(int(ptfrac*100+0.1)),str(int(rclus*100+0.1)))
    inp="%sTopoTrimmedPtFrac%sSmallR%s"%(cluster_calib,str(int(0.05*100+0.1)),str(int(rclus*100+0.1))) #hardcoded for now due to 0.04 vs 0.05 choice
    seq="jm"
    config="triggerTrim"
    evsprefix="HLTKt4"

    # Build the modifier
    try:
        calibmod = jrcf.find(alg, rad, inp, seq, config, evsprefix)
    except Exception, e:
        print "Exception raised within jrcf when finding the trimmed jet calib modifier"
        raise e

    # Check if we found it or not
    if not calibmod:
        raise RuntimeError(error%("failed to find trimmed jet calib modifier"))
    
    print "Adding calibration modifier: ",str(calibmod)
    return calibmod



def _getIsData():
    # From Joerg on trig dev list, July 12 2017
    from AthenaCommon.GlobalFlags import globalflags
    return globalflags.DataSource() == 'data'


def _getJetBuildTool(merge_param,
                     ptmin,
                     ptminFilter,
                     jet_calib,
                     cluster_calib,
                     do_minimalist_setup,
                     name='',
                     outputLabel=''):
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

    # Ensure the calibration is valid
    _is_calibration_supported(int_merge_param, jet_calib, cluster_calib)
    
    mygetters = [_getTriggerPseudoJetGetter(cluster_calib)]
    jtm.gettersMap["mygetters"] = mygetters
    # print jtm.gettersMap.keys()

    # in situ calibration step is only for data, not MC
    # this string here allows the following code to be data/MC unaware
    inSitu = 'i' if _getIsData() else ''

    # tell the offline code which calibration is requested
    calib_str = {'jes': 'calib:j:triggerNoPileup:HLTKt4',
                 'subjes': 'calib:aj:trigger:HLTKt4',
                 'sub': 'calib:a:trigger:HLTKt4',
                 'subjesIS': 'calib:ajg%s:trigger2016:HLTKt4'%(inSitu)}.get(jet_calib, '')

    # with S Schramm very early 18/4/2016
    mymods = [jtm.jetens]
    if calib_str: mymods.append(calib_str)
    mymods.append(jtm.caloqual_cluster)
    if outputLabel!='triggerTowerjets': #towers don't have cluster moments
        mymods.append(jtm.clsmoms)

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
                                            rndseed=1,
                                            isTrigger=True,
                                            ptmin=ptmin,
                                            ptminFilter=ptminFilter
                                            )
            
            
        except Exception, e:
            print 'error adding new jet finder %s' % name
            for jr in jtm.trigjetrecs:
                print jr
            raise e

   # else:
    #    print 'found jet finder ', name

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

def _getJetTrimmerTool (merge_param,
                        jet_calib, 
                        cluster_calib, 
                        ptfrac,
                        rclus, 
                        name="",
                        ):
    """Set up offline trimmer tools. Offline trimmer and modifiers
    make heavy use of dynamic stores, so all modifiers other than
    calibration are currently disabled."""


    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm

    msg = 'Naming convention breaks with merge param %d' % merge_param
    int_merge_param = int(10 * merge_param)
    assert 10 * merge_param == int_merge_param, msg

    assert merge_param > 0.

    # Add the jet modifiers now
    mymods = [jtm.jetens]
    
    # Currently we need to manually create the calibration tool
    # This is not done automatically by the trimming tool (as it is during jet finding)
    # Note, this requires some hard-coding and assumptions!
    try:
        calibmod  = _getTrimmedJetCalibrationModifier(jet_calib,
                                                      int_merge_param,
                                                      cluster_calib,
                                                      rclus,
                                                      ptfrac
                                                      )
        if calibmod: mymods.append(calibmod)
    except Exception, e:
        print 'Error building trimmed jet calibration modifier for %s' % name
        raise e

    jtm.modifiersMap["mymods"] = mymods
 

    if not name:
        name = 'TrigAntiKt%d%s%sTopoTrimmedJets' % (int_merge_param,
                                                    cluster_calib,
                                                    jet_calib)
        
    def findJetTrimmerTool():
        for jr in jtm.trigjetrecs:
            if jr.OutputContainer == name:
                # jr.OutputContainer is a string, here used to identify the object
                return jr
            return None
        
    jetTrimmerTool = findJetTrimmerTool()
        
    if jetTrimmerTool is None:
        print 'adding new jet trimmer ', name
        try:
            
            # Create the builder
            jetTrimmerTool = jtm.addJetTrimmer(name,
                                               rclus,
                                               ptfrac,
                                               name+'notrim',
                                               modifiersin = "mymods", #"groomed"
                                               isTrigger=True,
                                               doArea=False, # Not needed, we don't use jet area after trimming (speeds up execution)
                                               )
            
            # Create a special builder for the groomer to use (trigger-specific need)
            if not hasattr(jtm,"trigjblda"):
                jtm.addJetBuilderWithArea(TrigHLTJetRecConf.TrigJetFromPseudojet("trigjblda",
                                                                                 Attributes = ["ActiveArea", "ActiveArea4vec"]
                                                                                ))

            # Add the special builder to the tool
            from AthenaCommon.AppMgr import ToolSvc
            getattr(ToolSvc,name+"Groomer").unlock()
            getattr(ToolSvc,name+"Groomer").JetBuilder = jtm.trigjblda
            getattr(ToolSvc,name+"Groomer").lock()

            # For debugging
            #getattr(ToolSvc,name+"Groomer").unlock()
            #getattr(ToolSvc,name+"Groomer").OutputLevel = 1
            #ToolSvc.trigjblda.unlock()
            #ToolSvc.trigjblda.OutputLevel = 1

        except Exception, e:
            print 'error adding new jet trimmer %s' % name
            for jr in jtm.trigjetrecs:
                print jr
            raise e    

    return jetTrimmerTool
            
            
            
            
        
        
        

def _is_calibration_supported(int_merge_param, jet_calib, cluster_calib):
    """Check if the calibration is supported by the offline code"""

    do_jes = 'jes' in jet_calib
    do_insitu = 'IS' in jet_calib

    bad_combinations = [
        {'merge_param': 10, 'do_jes': True, 'cluster_calib': 'EM'}]
    
    if {'merge_param': int_merge_param,
        'do_jes': do_jes,
        'cluster_calib': cluster_calib} in bad_combinations:

        m = 'TrigHLTJetRecConfig._getJetBuildTool: no calibration support'\
            ' for merge_param %s, cluster calib %s, jes calibration: %s' % (
                str(int_merge_param), str(cluster_calib), str(do_jes))
        raise RuntimeError(m)

    if do_insitu and not (do_jes and int_merge_param == 4):
        m = 'TrigHLTJetRecConfig._getJetBuildTool: no insitu calibration support'\
            ' for merge_param %s, cluster calib %s, jes calibration: %s' % (
                str(int_merge_param), str(cluster_calib), str(do_jes))
        raise RuntimeError(m)


def _getTriggerPseudoJetGetter(cluster_calib):
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]

    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm

    # The 'Label' must be one of the values found in JetContainerInfo.h
    # LC or EM
    label = cluster_calib + 'Topo'
    pjg_name = 'triggerPseudoJetGetter_%s' % label

    try:
        triggerPseudoJetGetter = getattr(jtm, pjg_name)
    except AttributeError:
        # Add TriggerPSeudoJetGetter to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        pjg = TrigHLTJetRecConf.TriggerPseudoJetGetter(pjg_name, Label=label)

        # adds arg to Tool Service and returns arg
        jtm.add(pjg)
        triggerPseudoJetGetter = getattr(jtm, pjg_name)

    return triggerPseudoJetGetter

    
def _getEnergyDensityTool(toolname, **options):

    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm
    
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
    try:
        energyDensityTool = getattr(jtm, toolname)
    except AttributeError:
        # Add the EnergyDensity to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContainerInfo.h
        energyDensityTool = configHLTEventDensityTool(toolname, **options)
        jtm += energyDensityTool
        energyDensityTool = getattr(jtm, toolname)
        print 'TrigHLTJetRecConfig._getEnergyDensityTool '\
            'Added energyDensity tools "%s" to jtm' % toolname

    return energyDensityTool

    
def _getPseudoJetSelectorAll(toolname, **options):

    # set up a tool to select all pseudo jets
    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm
    
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
    try:
        selector = getattr(jtm, toolname)
    except AttributeError:
        # Add the PseudoJetSelectorAll to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContainerInfo.h
        selector = TrigHLTJetRecConf.PseudoJetSelectorAll(name=toolname)
        jtm += selector
        selector = getattr(jtm, toolname)
        print 'TrigHLTJetRecConfig._getPseudoJetSelectorAll '\
            'Added selector "%s" to jtm' % toolname

    return selector


def _getPseudoJetSelectorEtaPt(toolname, **kwds):

    # set up a tool to select all pseudo jets
    # declare jtm as global as this function body may modify it
    # with the += operator
    global jtm
    
    # Build a new list of jet inputs. original: mygetters = [jtm.lcget]
    try:
        selector = getattr(jtm, toolname)
    except AttributeError:
        # Add the PseudoJetSelectorEtaPt to the JetTool Manager,
        # which pushes it to the ToolSvc in __iadd__
        # This is done in the same as PseudoJetGetter is added in
        # JetRecStandardTools.py.
        # The 'Label' must be one of the values found in JetContainerInfo.h
        selector = PseudoJetSelectorEtaPt(toolname, **kwds)

        jtm += selector
        selector = getattr(jtm, toolname)
        print 'TrigHLTJetRecConfig._getPseudoJetSelectorEtaPt '\
            'Added selector "%s" to jtm' % toolname

    return selector

# class TrigHLTJetRec_param(TrigHLTJetRecConf.TrigHLTJetRec):
#     """ DEPRECATED ALGORITHM
#     remove from TriggerMEnu.python.jetDefInstantiator
#     before removing from TrigHLTJetRecConfig.py
#     Supply a specific merge parameter for the anti-kt algorithm"""
# 
#     def __init__(self,
#                  name,
#                  alg="AntiKt",
#                  merge_param="04",
#                  ptmin=7.0 * GeV,
#                  ptminFilter=7.0 * GeV,
#                  jet_calib='subjes',
#                  cluster_calib='EM',
#                  do_minimalist_setup=True,
#                  output_collection_label='defaultJetCollection'
#                  ):
#         # TrigHLTJetRecConf.TrigHLTJetRec.__init__(
#         #    self,
#         #    # name=name + '_DEPRECATED',
#         #    name=name,
#         #    cluster_calib=cluster_calib)
# 
#         print 'TrigHLTJetRec_param is DEPRECATED'
# 
#         TrigHLTJetRecConf.TrigHLTJetRec.__init__(self, name=name)
# 
#         self.cluster_calib = cluster_calib
# 
#         self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)
# 
#         self.jetBuildTool = _getJetBuildTool(
#             float(int(merge_param))/10.,
#             ptmin=ptmin,
#             ptminFilter=ptminFilter,
#             jet_calib=jet_calib,
#             cluster_calib=cluster_calib,
#             do_minimalist_setup=do_minimalist_setup,
#             )
# 
#         self.output_collection_label = output_collection_label


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
                 pseudojet_labelindex_arg='PseudoJetLabelMapTriggerFromCluster',
                 ):
        
        TrigHLTJetRecConf.TrigHLTJetRecFromCluster.__init__(self, name = name)
        
      
        self.cluster_calib = cluster_calib
        self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)
        
        
        self.iPseudoJetSelector = _getPseudoJetSelectorAll(
            'iPseudoJetSelectorAll')
        
        self.jetBuildTool = _getJetBuildTool(
            float(int(merge_param))/10.,
            ptmin=ptmin,
            ptminFilter=ptminFilter,
            jet_calib=jet_calib,
            cluster_calib=cluster_calib,
            do_minimalist_setup=do_minimalist_setup,
            name=name,
            )
        print 'after jetbuild'
        
        self.output_collection_label = output_collection_label
        self.pseudojet_labelindex_arg = pseudojet_labelindex_arg
        print 'end of jets'

################### Groomer class ##################################



class TrigHLTJetRecGroomer(TrigHLTJetRecConf.TrigHLTJetRecGroomer):
    """Configure building and grooming of jets as one step"""
    def __init__(self,
                 name,
                 alg="AntiKt",
                 merge_param="10",
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 jet_calib='subjes',
                 cluster_calib='LC',
                 do_minimalist_setup=True,
                 output_collection_label='defaultJetCollection',
                 pseudojet_labelindex_arg='PseudoJetLabelMapTriggerFromCluster',
                 rclus= 0.2,
                 ptfrac= 0.04,
                 ):
        
        TrigHLTJetRecConf.TrigHLTJetRecGroomer.__init__(self, name = name)
        
      
        self.cluster_calib = cluster_calib
        self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)
        
        
        self.iPseudoJetSelector = _getPseudoJetSelectorAll('iPseudoJetSelectorAll')
        
        self.jetBuildTool = _getJetBuildTool(float(int(merge_param))/10.,
                                             ptmin=ptmin,
                                             ptminFilter=ptminFilter,
                                             jet_calib='nojcalib',
                                             cluster_calib=cluster_calib,
                                             do_minimalist_setup=do_minimalist_setup,
                                             name=name+'notrim',
                                             )
        print 'after trimming jetbuild'
        
        self.jetTrimTool = _getJetTrimmerTool(merge_param=float(int(merge_param))/10.,
                                              jet_calib=jet_calib,
                                              cluster_calib=cluster_calib,
                                              rclus=rclus,
                                              ptfrac=ptfrac,
                                              name=name,
                                              )
            
        self.output_collection_label = output_collection_label
        self.pseudojet_labelindex_arg = pseudojet_labelindex_arg
        print 'end of trimming jets'


################### Frome Jet class ################################
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
                 etaMaxCut=10.0, # no effect, to be removed
                 ):
        TrigHLTJetRecConf.TrigHLTJetRecFromJet.__init__(self, name=name)

        self.cluster_calib = cluster_calib
        self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        self.iPseudoJetSelector = _getPseudoJetSelectorEtaPt(
            'iPseudoJetSelectorEtaPt_%d_%d' % (int(10 * etaMaxCut),
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
        print 'Jet from jets'

class TrigHLTJetRecFromTriggerTower(
        TrigHLTJetRecConf.TrigHLTJetRecFromTriggerTower):
    """Jets from trigger towers"""
    def __init__(self,
                 name,
                 alg='AntiKt',
                 merge_param='10',
                 ptmin=7.0 * GeV,
                 ptminFilter=7.0 * GeV,
                 jet_calib='nojcalib',  # no calibration to be done
                 cluster_calib='EM',
                 do_minimalist_setup=True,
                 output_collection_label='triggerTowerjets',  # do not use this
                 pseudojet_labelindex_arg='PseudoJetLabelMapTriggerFromTriggerTower',
                 ptMinCut=15000.,
                 etaMaxCut=3.2,
                 ):
        TrigHLTJetRecConf.TrigHLTJetRecFromTriggerTower.__init__(self,
                                                                 name=name)

        self.cluster_calib = cluster_calib
        self.pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        self.iPseudoJetSelector = _getPseudoJetSelectorEtaPt(
            'iPseudoJetSelectorEtaPt_%d_%d' % (int(10 * etaMaxCut),
                                               int(ptMinCut)),
            **{'etaMax': etaMaxCut, 'ptMin': ptMinCut})


        self.jetBuildTool = _getJetBuildTool(
            float(int(merge_param))/10.,
            ptmin=ptMinCut,
            ptminFilter=ptminFilter,
            jet_calib=jet_calib,
            cluster_calib=cluster_calib,
            do_minimalist_setup=do_minimalist_setup,
            outputLabel='triggerTowerjets'
            )

        self.output_collection_label = output_collection_label
        self.pseudojet_labelindex_arg = pseudojet_labelindex_arg


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


class TrigHLTJetDebug(TrigHLTJetRecConf.TrigHLTJetDebug):
    """Dump various values for debugging purposes"""
    def __init__(self,
                 chain_name,
                 cluster_calib,
                 ed_merge_param,
                 name="TrigHLTJetDebug"):

        TrigHLTJetRecConf.TrigHLTJetDebug.__init__(self, name)
        self.chainName = chain_name
        pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        # allow the debug Alg to access the EventShape object
        # by a direct StoreGate retireve. TtigHLTEnergyDensity writes
        # this object directly to SG without passing via navigation.
        self.eventshape_key = _getEventShapeSGKey(ed_merge_param,
                                                  cluster_calib + 'Topo') 


class TrigHLTEnergyDensity(TrigHLTJetRecConf.TrigHLTEnergyDensity):
    """Supply a specific merge parameter for the anti-kt algorithm"""

    
    def __init__(self,
                 name,
                 # cluster_calib: follow naming convention EM or LC
                 cluster_calib='EM',
                 ed_merge_param=0.4, 
                 AbsRapidityMin=0.0,
                 AbsRapidityMax=2.0,
                 JetAlgorithm='Kt',
                 AreaDefinition='Voronoi',
                 VoronoiRfact=0.9,
                 toolname_stub='jetTriggerEnergyDensityTool',
                 caloClusterContainerSGKey='',
                 eventShapeSGKey=None,
             ): 

        TrigHLTJetRecConf.TrigHLTEnergyDensity.__init__(
            self,
            name=name,
            caloClusterContainerSGKey=caloClusterContainerSGKey)

        from TrigHLTJetRec.TrigHLTEnergyDensityMonitoring import(
            TrigHLTEnergyDensityValidationMonitoring,
            TrigHLTEnergyDensityOnlineMonitoring,
            TrigHLTEnergyDensityCosmicMonitoring)

        validation = TrigHLTEnergyDensityValidationMonitoring()
        online = TrigHLTEnergyDensityOnlineMonitoring()
        cosmic = TrigHLTEnergyDensityCosmicMonitoring()
        self.AthenaMonTools = [ validation, online, cosmic ]
        # share the pseudojet getter betwen the algorithm an the tool.
        # the alg passes pseudojets to the pseudojetgetter. The
        # energy density tool obtains the pseudojets from the pseudojet getter.
        pseudoJetGetter = _getTriggerPseudoJetGetter(cluster_calib)

        # store cluster_calib is used by TrigEnergyDensity (C++)
        # to set the CaloCluster local calibration: EM, LC
        self.cluster_calib = cluster_calib

        # attribute operator causes type conversion.
        # e.g. instance conversion from <TriggerPseudoJetGetter/
        # ToolSvc.triggerPseudoJetGetter_EMTopo at 0x3444620>
        # to PublicToolHandle('TriggerPseudoJetGetter/
        # triggerPseudoJetGetter_EMTopo')
        self.pseudoJetGetter = pseudoJetGetter  

        options = dict(AbsRapidityMin=AbsRapidityMin,
                       AbsRapidityMax=AbsRapidityMax,
                       JetAlgorithm=JetAlgorithm,
                       radius=ed_merge_param,
                       pjGetter=pseudoJetGetter,
                       AreaDefinition=AreaDefinition,
                       VoronoiRfact=VoronoiRfact,
                       )

        # ed_merge_param is a float e.g. 0.4
        # name='jetTriggerEnergyDensityTool_%s' % cluster_calib
        toolname='%s_%s' % (toolname_stub, cluster_calib)
        options.update(toolname=toolname)

        OutputContainer = None
        if eventShapeSGKey is None:
            # jets: eventShapeSGKey is calculated here
            self.eventShapeSGKey = _getEventShapeSGKey(
                ed_merge_param,
                cluster_calib + 'Topo')
        else:
            # other: eventShapeSGKey is passed in
            self.eventShapeSGKey = eventShapeSGKey
            
        options.update(OutputContainer=self.eventShapeSGKey)
        self.energyDensityTool = _getEnergyDensityTool(**options)

        self.energyDensity = 0

class TrigHLTSoftKiller(TrigHLTJetRecConf.TrigHLTSoftKiller):
    """Supply a specific grid configuration for SoftKiller"""

    def __init__(self,
                 name,
                 cluster_calib="LC",
                 sk_grid_param_eta=0.4,
                 sk_grid_param_phi=0.4,
                 output_collection_label='SKclusters',  # do not use this
                ):

        TrigHLTJetRecConf.TrigHLTSoftKiller.__init__(self,name=name)
        self.output_collection_label = output_collection_label

        # TODO create and configure offline SoftKiller tool here, pass it to our tool
        # Use cluster_calib, sk_grid_param_eta, and sk_grid_param_phi to configure the offline tool
        print "SK: %s, %f, %f"%(cluster_calib,sk_grid_param_eta,sk_grid_param_phi)

        print "SK clusters from clusters"
                                                     

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


class PseudoJetSelectorEtaPt(TrigHLTJetRecConf.PseudoJetSelectorEtaPt):
    def __init__(self, name, etaMax, ptMin):
        TrigHLTJetRecConf.PseudoJetSelectorEtaPt.__init__(self, name)        
        self.etaMax = etaMax
        self.ptMin = ptMin


class PseudoJetSelectorEtaPt(TrigHLTJetRecConf.PseudoJetSelectorEtaPt):
    def __init__(self, name, **kwds):
        TrigHLTJetRecConf.PseudoJetSelectorEtaPt.__init__(self, name, **kwds)


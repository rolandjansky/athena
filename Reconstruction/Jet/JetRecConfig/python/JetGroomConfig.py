# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
JetGroomConfig: A helper module for configuring jet grooming         
Author: TJ Khoo, P-A Delsart                                                      
"""                                                                    

########################################################################

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetGroomConfig')

from AthenaConfiguration.ComponentFactory import CompFactory

import JetRecConfig.JetModConfig as modH
from JetRecConfig.JetRecConfig import buildJetModifierList,  addJetClusteringToComponent, JetRecCfg



def instantiateGroomingAliases( groomdef ):
    """ Instantiate all the modifier aliases contained in this groomdef.
    At the same time fills the internal _prereqDic and _prereqOrder containers.
    
    This functions 
      * assumes jetdef is not 'locked' 
      * implies calls to recursives function constH.aliasToInputDef and modH.aliasToModDef
    """
    
    for mod in groomdef.modifiers:
        modInstance = modH.aliasToModDef(mod, groomdef)
        groomdef._prereqDic['mod:'+mod] = modInstance
        groomdef._prereqOrder.append('mod:'+mod)


########################################################################
#
def getJetGroomAlg(groomdef,monTool=None):
    """Returns a configured JetRecAlg set-up to perform the grooming defined by 'groomdef' 
    ('monTool' is a temporary placeholder, it is expected to be used in the trigger in the future) 
    """
    jetlog.debug("Configuring grooming alg \"jetalg_{0}\"".format(groomdef.fullname()))


    # the grooming tool (a IJetProvider instance)
    groomClass = CompFactory.getComp(groomdef.tooltype)
    groomer = groomClass(groomdef.groomalg,
                         UngroomedJets = groomdef.ungroomeddef.fullname(),
                         ParentPseudoJets = groomdef.ungroomeddef._internalAtt['finalPJContainer'],
                         **groomdef.properties)
    # get JetModifier list
    mods = buildJetModifierList(groomdef)

    # put everything together in a JetRecAlg
    jetname = groomdef.fullname()
    jra = CompFactory.JetRecAlg(
        "jetrecalg_"+jetname,
        Provider = groomer,
        Modifiers = mods,
        OutputContainer = jetname,
        MonTool = monTool)
    
    
    from JetRecConfig.JetRecConfig import autoconfigureModifiers
    autoconfigureModifiers(jra.Modifiers, jetname)
    
    return jra

def getJetGroomAlg_jetAlg(groomdef,pjsin,monTool=None):
    """Returns a configured grooming algs based on the old JetAlgorithm

    We keep this function for compatibility with trigger config. When the trigger switches to JetRecAlg, this can be removed.
    """
    jetname = groomdef.fullname()
    jetlog.debug("Configuring JetAlgorithm \"jetalg_{0}\"".format(jetname))
    
    from . import JetRecConfig
    builder = JetRecConfig.getJetBuilder()

    groomClass = CompFactory.getComp(groomdef.tooltype)
    groomer = groomClass(groomdef.groomalg,
                         JetBuilder = builder,
                         **groomdef.properties)

    
    
    mods = buildJetModifierList(groomdef)
    
    rectool = CompFactory.JetRecTool(jetname,
                                     JetGroomer=groomer,
                                     InputContainer=groomdef.ungroomeddef.fullname(),
                                     OutputContainer=jetname,
                                     JetPseudojetRetriever=CompFactory.JetPseudojetRetriever("jpjretriever"),
                                     JetModifiers=mods)
    
    if monTool: rectool.MonTool = monTool
    
    jetalg = CompFactory.JetAlgorithm("jetalg_"+jetname)
    jetalg.Tools = [rectool]
    return jetalg

def addGroomToComponent(components,groomdef, configFlags, ):
    """Instantiate and schedule all the algorithms needed to run the grooming alg 'groomdef' and
    add them in the ComponentAccumulator 'components'

    This function is meant to be called from the top-level JetRecConfig.JetRecCfg
    (groomdef is expected to be non locked and will be modified).
    """
    sequenceName = groomdef.fullname()
    
    # Translate modifier aliases into JetModifier config instances.
    #  ( This also detects input dependencies, see below)
    instantiateGroomingAliases(groomdef)

    # Transfer the input & ghost dependencies onto the parent jet alg,
    # so they are handled when instatiating the parent jet algs
    groomdef.ungroomeddef.ghostdefs += [ g.split(':')[1] for g in groomdef._prereqOrder if g.startswith('ghost:')]
    groomdef.ungroomeddef.extrainputs += [ g.split(':')[1] for g in groomdef._prereqOrder if g.startswith('input:')]

    jetlog.info("Scheduling parent alg {} for {} ".format(groomdef.ungroomeddef.fullname(), groomdef.fullname()))
    # now instantiate the parent jet alg.
    # (we always want it even if the parent jets are already in the input file because
    #  we need to rebuild the pseudoJet)
    addJetClusteringToComponent(components, groomdef.ungroomeddef, configFlags, sequenceName)
    
    groomalg = getJetGroomAlg(groomdef)

    components.addEventAlgo(groomalg, sequenceName)

    jetlog.info("Scheduled JetAlgorithm instance \"jetalg_{0}\"".format(groomdef.fullname()))

    return components


if __name__=="__main__":

    # Setting needed for the ComponentAccumulator to do its thing
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=True

    # Config flags steer the job at various levels
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ASG/mc16_13TeV.410501.PowhegPythia8EvtGen_A14_ttbar_hdamp258p75_nonallhad.merge.AOD.e5458_s3126_r9364_r9315/AOD.11182705._000001.pool.root.1"]
    ConfigFlags.Concurrency.NumThreads = 1
    ConfigFlags.Concurrency.NumConcurrentEvents = 1
    ConfigFlags.lock()

    # Get a ComponentAccumulator setting up the fundamental Athena job
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg=MainServicesCfg(ConfigFlags)

    # Add the components for reading in pool files
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    # Add the components from our jet reconstruction job
    from StandardJetDefs import AntiKt10LCTopo
    from JetGrooming import JetTrimming
    AntiKt10LCTopo.ptminfilter = 100e3
    AntiKt10LCTopoTrimmedPtFrac5SmallR20 = JetTrimming(AntiKt10LCTopo,"AntiKt10LCTopoJets",smallR=0.2,ptfrac=0.05)
    cfg.merge(JetRecCfg(AntiKt10LCTopoTrimmedPtFrac5SmallR20,ConfigFlags))

    cfg.printConfig(withDetails=False,summariseProps=True)

    cfg.run(maxEvents=10)

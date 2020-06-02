# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# JetGroomConfig: A helper module for configuring jet grooming         #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################

from AthenaCommon import Logging
jetlog = Logging.logging.getLogger('JetGroomConfig')

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

import six

########################################################################
# Get a jet groomer class given a tool name and the grooming definition object
# The pjsin argument is for forwards compatibility when we switch to
# using JetRecAlg
#
def getJetGroomer(groomdef,pjsin):
    tooltype = groomdef.groomspec["ToolType"]
    toolprops = {key:val for key,val in six.iteritems (groomdef.groomspec) if key not in ["groomalg","ToolType"]}
    return tooltype(groomdef.basename,**toolprops)


########################################################################
# Function for configuring the jet algorithm and groomers, given the
# set of dependencies
#
def getJetGroomAlg(jetname,groomdef,pjsin,modlist):
    jetlog.debug("Configuring JetAlgorithm \"jetalg_{0}\"".format(jetname))

    from . import JetRecConfig
    builder = JetRecConfig.getJetBuilder()

    groomer = getJetGroomer(groomdef,pjsin)
    groomer.JetBuilder = builder

    from . import JetModConfig
    mods = []
    # Dependency resolution should be done externally
    for moddef,modspec in modlist:
        mod = JetModConfig.getModifier(groomdef,moddef,modspec)
        mods.append(mod)

    rectool = CompFactory.JetRecTool(jetname,
                                     JetGroomer=groomer,
                                     InputContainer=groomdef.ungroomedname,
                                     OutputContainer=jetname,
                                     JetPseudojetRetriever=CompFactory.JetPseudojetRetriever("jpjretriever"),
                                     JetModifiers=mods)

    jetalg = CompFactory.JetAlgorithm("jetalg_"+jetname)
    jetalg.Tools = [rectool]

    return jetalg

########################################################################
# Top-level function for running jet grooming
# Will first run the jet finder, unless the input jets are
# found in the input file
def JetGroomCfg(groomdef, configFlags, jetnameprefix="",jetnamesuffix=""):
    jetsfullname = jetnameprefix+groomdef.basename+jetnamesuffix+"Jets"
    jetlog.info("Setting up to find {0}".format(jetsfullname))

    sequencename = jetsfullname

    components = ComponentAccumulator()
    from AthenaCommon.AlgSequence import AthSequencer
    components.addSequence( AthSequencer(sequencename) )

    # Check if the ungroomed jets exist in the input file.
    # If not, we need to configure their reconstruction.
    filecontents = configFlags.Input.Collections
    if groomdef.ungroomedname not in filecontents:
        from . import JetRecCfg
        components.merge(JetRecCfg(groomdef.ungroomeddef, configFlags,
                                   jetnameoverride=groomdef.ungroomedname))
    else:
        # FIXME: Need to schedule rebuilding of pseudojets
        pass

    # FIXME: Add calls to JetModConfig.getFinalModifierListAndPrereqs
    components.addEventAlgo(getJetGroomAlg(jetsfullname,groomdef,groomdef.modifiers))

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
    cfg.merge(JetGroomCfg(AntiKt10LCTopoTrimmedPtFrac5SmallR20,ConfigFlags))

    cfg.printConfig(withDetails=False,summariseProps=True)

    cfg.run(maxEvents=10)

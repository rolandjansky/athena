# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# jobOption fragment to schedule jetbuilding in a runII config style standard reconstruction,
# BUT using the new runIII jet config


from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt4EMTopo, AntiKt4Truth
from JetRecConfig.StandardLargeRJets import AntiKt10LCTopo_noVR
from JetRecConfig.JetRecConfig import getJetDefAlgs, reOrderAlgs

from JetRecConfig.StandardJetConstits import stdConstitDic


##
# Temporary hack : JetConstituentModSequence for EMPFlow seems to be scheduled
# somewhere else in the standard reco chain with a different alg name as the alg created by JetRecConfig helpers.
# The trick below will make so the new helpers do NOT schedule a JetConstituentModSequence and thus avoid conflicts.
stdConstitDic.EMPFlow._locked = False
stdConstitDic.EMPFlow.inputname = stdConstitDic.EMPFlow.containername


# the Standard list of jets to run :
jetdefs = [AntiKt4EMTopo, AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt4Truth, AntiKt10LCTopo_noVR]

# we'll remember the EventDensity collections we create.
evtDensities = []

#--------------------------------------------------------------
# Create the jet algs from the jet definitions
#--------------------------------------------------------------

for jd in jetdefs:
    algs, jetdef_i = getJetDefAlgs(ConfigFlags, jd, True)
    algs = reOrderAlgs( [a for a in algs if a is not None])
    for a in algs:
        topSequence += conf2toConfigurable(a)
        if "EventDensityAthAlg" in a.getType():
            evtDensities.append( str(a.EventDensityTool.OutputContainer) )



#--------------------------------------------------------------
# Build output container list.
#--------------------------------------------------------------

for jetdef in jetdefs:
    jetFlags.jetAODList += [ f"xAOD::JetContainer#{jetdef.fullname()}" ]
    auxprefix = ""
    # if jetrec.Trigger:
    #   auxprefix = "Trig"
    jetFlags.jetAODList += [ f"xAOD::Jet{auxprefix}AuxContainer#{jetdef.fullname()}Aux." ,
                             f"xAOD::JetAuxContainer#{jetdef.fullname()}Aux.-PseudoJet"]
    

if jetFlags.useTracks() and jetFlags.useTopo():
    jetFlags.jetAODList += [ "xAOD::CaloClusterContainer#EMOriginTopoClusters" ,
                             "xAOD::CaloClusterContainer#LCOriginTopoClusters" ,
                             "xAOD::ShallowAuxContainer#LCOriginTopoClustersAux.",
                             "xAOD::ShallowAuxContainer#EMOriginTopoClustersAux."] 
    jetFlags.jetAODList += [ "xAOD::PFOContainer#CHSChargedParticleFlowObjects",
                             "xAOD::PFOContainer#CHSNeutralParticleFlowObjects",
                             "xAOD::ShallowAuxContainer#CHSChargedParticleFlowObjectsAux.",
                             "xAOD::ShallowAuxContainer#CHSNeutralParticleFlowObjectsAux."] 

# save event shapes set with the JetAlgorithm
for edname in evtDensities:
    jetFlags.jetAODList += [ f"xAOD::EventShape#{edname}", 
                             f"xAOD::EventShapeAuxInfo#{edname}Aux.",]


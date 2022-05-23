# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt4EMTopo, AntiKt4Truth
from JetRecConfig.StandardLargeRJets import AntiKt10LCTopo_noVR
from JetRecConfig.JetRecConfig import JetRecCfg






def addJetsToOutput(flags,jetdefs, toAOD=True, toESD=True):
    """Write out the jet containers as defined by jetdefs (a list of JetDefinition).
    
    In Run3 we don't write out jets in AOD : this function is left for convenience and testing purpose.
    """
    result = ComponentAccumulator()

    #--------------------------------------------------------------
    # Build output container list.
    #--------------------------------------------------------------
    jetList = []

    for jetdef in jetdefs:
        jetList += [ f"xAOD::JetContainer#{jetdef.fullname()}" ,
                     f"xAOD::JetAuxContainer#{jetdef.fullname()}Aux.-PseudoJet"]
        # Not sure if this trigger special AuxContainer is obsolete in Run3 ?
        # if trigger:
        #   auxprefix = "Trig"
        #   jetAODList += [ f"xAOD::Jet{auxprefix}AuxContainer#{jetdef.fullname()}Aux." ,

    # Store event shapes when jets are being stored to output
    jetList += ["xAOD::EventShape#Kt4EMPFlowEventShape",
                "xAOD::EventShapeAuxInfo#Kt4EMPFlowEventShapeAux.",
                "xAOD::EventShape#Kt4EMTopoOriginEventShape",
                "xAOD::EventShapeAuxInfo#Kt4EMTopoOriginEventShapeAux.",
                "xAOD::EventShape#Kt4LCTopoOriginEventShape",
                "xAOD::EventShapeAuxInfo#Kt4LCTopoOriginEventShapeAux."]

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD, addToAOD
    if toESD:
        result.merge(addToESD(flags, jetList))
    if toAOD:
        result.merge(addToAOD(flags, jetList))

    return result

def JetRecoSteeringCfg(flags):
    result = ComponentAccumulator()
    
    # the Standard list of jets to run :
    jetdefs = [AntiKt4EMTopo, AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt4Truth, AntiKt10LCTopo_noVR]

    from JetRecConfig.JetConfigFlags import jetInternalFlags
    # We're in Reco job : propagate this info to the runIII jet config
    # (see JetConfigFlags.py for motivations on this way of doing)
    jetInternalFlags.isRecoJob = True
    
    #--------------------------------------------------------------
    # Create the jet algs from the jet definitions
    #--------------------------------------------------------------
    for jd in jetdefs:
        result.merge(JetRecCfg(flags, jd))

    if flags.Output.doWriteAOD and flags.Jet.WriteToAOD:
        result.merge(addJetsToOutput(flags, jetdefs, toAOD=True, toESD=False))
    if flags.Output.doWriteESD:
        result.merge(addJetsToOutput(flags, jetdefs, toAOD=False, toESD=True))

    return result

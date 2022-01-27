# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from JetRecConfig.StandardSmallRJets import AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt4EMTopo, AntiKt4Truth
from JetRecConfig.StandardLargeRJets import AntiKt10LCTopo
from JetRecConfig.JetRecConfig import JetRecCfg


def JetRecoSteeringCfg(flags):
    result = ComponentAccumulator()

    # the Standard list of jets to run :
    jetdefs = [AntiKt4EMTopo, AntiKt4EMPFlow, AntiKt4LCTopo, AntiKt4Truth, AntiKt10LCTopo]

    #--------------------------------------------------------------
    # Create the jet algs from the jet definitions
    #--------------------------------------------------------------

    for jd in jetdefs:
        result.merge(JetRecCfg(flags, jd))


    return result


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

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD, addToAOD
    if toESD:
        result.merge(addToESD(flags, jetList))
    if toAOD:
        result.merge(addToAOD(flags, jetList))

    return result

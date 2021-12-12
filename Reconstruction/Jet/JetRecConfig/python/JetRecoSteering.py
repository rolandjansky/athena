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

    #--------------------------------------------------------------
    # Build output container list.
    #--------------------------------------------------------------
    jetAODList = []
    jetESDList = []

    for jetdef in jetdefs:
        jetAODList += [ f"xAOD::JetContainer#{jetdef.fullname()}" ]
        auxprefix = ""
        # if trigger:
        #   auxprefix = "Trig"
        jetAODList += [ f"xAOD::Jet{auxprefix}AuxContainer#{jetdef.fullname()}Aux." ,
                        f"xAOD::JetAuxContainer#{jetdef.fullname()}Aux.-PseudoJet"]

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD, addToAOD
    result.merge(addToESD(flags, jetAODList+jetESDList))
    result.merge(addToAOD(flags, jetAODList))

    return result

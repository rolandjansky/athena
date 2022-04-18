# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#listOfScheduledAlgs = [ 'JetAlgorithm/jetalg_ConstitModCorrectPFOCHS_EMPFlow',
#                        'PseudoJetAlgorithm/PseudoJetAlgForIsoNFlow',
#                        'EventDensityAthAlg/CentralDensityForNFlowIsoAlg',
#                        'EventDensityAthAlg/ForwardDensityForNFlowIsoAlg',
#                        'IsolationBuilder/PFlowIsolationBuilder',
#                        'JetAlgorithm/jetalg_ConstitModCorrectPFOCSSKCHS_EMPFlowCSSK',
#                        'PseudoJetAlgorithm/PseudoJetAlgForIsoCSSKNFlow',
#                        'EventDensityAthAlg/CentralDensityForCSSKNFlowIsoAlg',
#                        'EventDensityAthAlg/ForwardDensityForCSSKNFlowIsoAlg',
#                        'IsolationBuilder/CSSKPFlowIsolationBuilder' ]

def makeEGammaCommonIso():
    from DerivationFrameworkCore.DerivationFrameworkMaster import (
        DerivationFrameworkJob )
    from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from IsolationAlgs.IsolationSteeringDerivConfig import (
        IsolationSteeringDerivCfg )

    addIsoVar = ''
    densityList = []
    densityDict = {}
    for inT in ['','CSSK']:
        if not hasattr(DerivationFrameworkJob, inT+'PFlowIsolationBuilder'):
            CAtoGlobalWrapper(IsolationSteeringDerivCfg, ConfigFlags,
                                  inType = 'EMPFlow'+inT)

        for eta in ['Central','Forward']:
            # I do not understand why this is needed
            densityDict.update({
                "NeutralParticle"+inT+"FlowIso"+eta+"EventShape":"xAOD::EventShape",
                "NeutralParticle"+inT+"FlowIso"+eta+"EventShapeAux":"xAOD::EventShapeAuxInfo"})
            densityList += ["NeutralParticle"+inT+"FlowIso"+eta+"EventShape.Density"]

        suff = '' if len(inT) == 0 else '_'+inT
        addIsoVar += \
        f".neflowisol20{suff}.neflowisol30{suff}.neflowisol40{suff}" + \
        f".neflowisolcoreConeEnergyCorrection{suff}" + \
        f".neflowisolcoreConeSCEnergyCorrection{suff}"

    return addIsoVar,densityList,densityDict

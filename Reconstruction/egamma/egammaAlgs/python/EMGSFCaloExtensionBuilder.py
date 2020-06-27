# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate the EMGSFCaloExtensionBuilder with default configuration"
__author__ = "Christos"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from egammaRec import egammaKeys
from egammaTrackTools.egammaTrackToolsFactories import (
    EMLastCaloExtensionTool, EMParticleCaloExtensionTool)

EMGSFCaloExtensionBuilder = AlgFactory(
    egammaAlgsConf.EMGSFCaloExtensionBuilder,
    name='EMGSFCaloExtensionBuilder',
    LastCaloExtensionTool=EMLastCaloExtensionTool,
    PerigeeCaloExtensionTool=EMParticleCaloExtensionTool,
    GSFPerigeeCache='GSFPerigeeCaloExtension',
    GSFLastCache='GSFLastCaloExtension',
    GFFTrkPartContainerName=egammaKeys.outputTrackParticleKey())

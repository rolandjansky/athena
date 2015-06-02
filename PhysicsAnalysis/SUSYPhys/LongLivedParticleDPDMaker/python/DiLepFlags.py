# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

import AthenaCommon.SystemOfUnits as Units

primRPVLLDESDM = jobproperties.PrimaryDPDFlags_RPVLLStream

class DiLep_containerFlags(JobProperty):
    statusOn               = True
    photonCollectionName   = 'Photons'
    electronCollectionName = 'Electrons'
    muonCollectionName     = 'Muons'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_containerFlags)

class DiLep_singlePhotonFilterFlags(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 150*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = None
    cutIsEM      = 'Loose'
    trigger      = 'HLT_g140_loose'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_singlePhotonFilterFlags)

class DiLep_singlePhotonFilterFlags_addph(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 10*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = None
    cutIsEM      = 'Loose'
    trigger      = None
    pass
primRPVLLDESDM.add_JobProperty(DiLep_singlePhotonFilterFlags_addph)

class DiLep_singlePhotonFilterFlags_addel(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 10*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = 2.0*Units.mm
    cutIsEM      = None
    trigger      = None
    pass
primRPVLLDESDM.add_JobProperty(DiLep_singlePhotonFilterFlags_addel)

class DiLep_singlePhotonFilterFlags_addmu(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 10*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = 1.5*Units.mm
    trigger      = None
    pass
primRPVLLDESDM.add_JobProperty(DiLep_singlePhotonFilterFlags_addmu)

class DiLep_singleElectronFilterFlags(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 140*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = 2.0*Units.mm
    cutIsEM      = None
    trigger      = 'HLT_g140_loose'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_singleElectronFilterFlags)

class DiLep_singleMuonFilterFlags(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 60*Units.GeV
    cutEtaMax    = 1.07
    cutd0Min     = 1.5*Units.mm
    trigger      = 'HLT_mu60_0eta105_msonly'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_singleMuonFilterFlags)

class DiLep_diPhotonFilterFlags(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 50*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = None
    cutIsEM      = 'Loose'
    trigger      = 'HLT_2g50_loose'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_diPhotonFilterFlags)

class DiLep_diElectronFilterFlags(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 50*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = 2.0*Units.mm
    cutIsEM      = None
    trigger      = 'HLT_2g50_loose'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_diElectronFilterFlags)

class DiLep_diEgammaFilterFlags_electron(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 50*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = 2.0*Units.mm
    cutIsEM      = None
    trigger      = 'HLT_2g50_loose'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_diEgammaFilterFlags_electron)

class DiLep_diEgammaFilterFlags_photon(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 50*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = None
    cutIsEM      = 'Loose'
    trigger      = None
    pass
primRPVLLDESDM.add_JobProperty(DiLep_diEgammaFilterFlags_photon)

class DiLep_diLooseEgammaFilterFlags_electron(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 50*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = None
    cutIsEM      = 'Loose'
    trigger      = 'HLT_2g50_loose'
    pass
primRPVLLDESDM.add_JobProperty(DiLep_diLooseEgammaFilterFlags_electron)

class DiLep_diLooseEgammaFilterFlags_photon(JobProperty):
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False    
    cutEtMin     = 50*Units.GeV
    cutEtaMax    = 2.5
    cutd0Min     = None
    cutIsEM      = 'Loose'
    trigger      = None
    pass
primRPVLLDESDM.add_JobProperty(DiLep_diLooseEgammaFilterFlags_photon)

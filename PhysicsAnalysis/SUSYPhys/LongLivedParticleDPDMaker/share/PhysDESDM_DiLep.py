# ##########################################################################################
# Electron/photon/muon augmentation
# Use the derivation framework tools directly
# ##########################################################################################

# Cache the container names (used throughout)
photons   = primRPVLLDESDM.DiLep_containerFlags.photonCollectionName
electrons = primRPVLLDESDM.DiLep_containerFlags.electronCollectionName
muons     = primRPVLLDESDM.DiLep_containerFlags.muonCollectionName

# Electron d0
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__RpvElectronD0Tool
DiLepElectronD0 = DerivationFramework__RpvElectronD0Tool( name           = "DiLepElectronD0",
                                                          CollectionName = electrons,
                                                          SGPrefix       = "DiLep" + electrons,
                                                        )
ToolSvc += DiLepElectronD0

# Muon d0
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__RpvMuonD0Tool
DiLepMuonD0 = DerivationFramework__RpvMuonD0Tool( name           = "DiLepMuonD0",
                                                  CollectionName = muons,
                                                  SGPrefix       = "DiLep" + muons,
                                                )
ToolSvc += DiLepMuonD0

# Muon IsMS
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__RpvMuonMSTool
DiLepMuonMS = DerivationFramework__RpvMuonMSTool( name           = "DiLepMuonMS",
                                                  CollectionName = muons,
                                                  SGPrefix       = "DiLep" + muons,
                                                )
ToolSvc += DiLepMuonMS

# PROBLEM! Classes I want to use are not available in AtlasProduction
# For now, I copied the classes to this package

# Photon IsEM
# from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__EGammaPassSelectionWrapper
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__RpvEgammaIDTool
DiLepPhotonSelection = DerivationFramework__RpvEgammaIDTool( name               = "DiLepPhotonSelection",
                                                             SelectionVariables = ["Loose", "Tight"],
                                                             CollectionName     = photons,
                                                             SGPrefix           = "DiLep" + photons,
                                                           )
ToolSvc += DiLepPhotonSelection

# Electron IsEM
DiLepElectronSelection = DerivationFramework__RpvEgammaIDTool( name               = "DiLepElectronSelection",
                                                               SelectionVariables = ["Loose", "Medium", "Tight"],
                                                               CollectionName     = electrons,
                                                               SGPrefix           = "DiLep" + electrons,
                                                             )
ToolSvc += DiLepElectronSelection

# Kernel for the augmentation tools
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
topSequence += DerivationFramework__DerivationKernel( "RPVLLAugmentationKernel",
                                                      AugmentationTools = [DiLepElectronD0, DiLepMuonD0, DiLepMuonMS, DiLepPhotonSelection, DiLepElectronSelection],
                                                    )

# ##########################################################################################
# Selection algorithms setup
# ##########################################################################################

def EgammaSelectionString(flags, container, number = 1):
    """Construct electron and photon selection string based on the flags provided (from DiLepFlags)
    """

    cutList = []
    if flags.cutEtMin:  cutList.append('%s.pt > %s'          % (container, flags.cutEtMin))
    if flags.cutEtaMax: cutList.append('abs(%s.eta) < %s'    % (container, flags.cutEtaMax))
    if flags.cutd0Min:  cutList.append('abs(DiLep%sD0) > %s' % (container, flags.cutd0Min))
    if flags.cutIsEM:   cutList.append('DiLep%s%s'           % (container, flags.cutIsEM))

    cutString = 'count(' + ' && '.join(cutList) + ') > %i' % (number-1)
    if flags.trigger: cutString = flags.trigger + ' && ' + cutString

    print cutString
    return cutString

def MuonSelectionString(flags, container, number = 1):
    """Construct muon selection string based on the flags provided (from DiLepFlags)
    """

    cutList_combined = []
    if flags.cutEtMin:  cutList_combined.append('%s.pt > %s'          % (container, flags.cutEtMin))
    if flags.cutEtaMax: cutList_combined.append('abs(%s.eta) < %s'    % (container, flags.cutEtaMax))
    if flags.cutd0Min:  cutList_combined.append('abs(DiLep%sD0) > %s' % (container, flags.cutd0Min))
    cutList_combined.append('DiLep%sisCombined == 1' % (container))

    cutList_msonly = []
    if flags.cutEtMin:  cutList_msonly.append('%s.pt > %s'       % (container, flags.cutEtMin))
    if flags.cutEtaMax: cutList_msonly.append('abs(%s.eta) < %s' % (container, flags.cutEtaMax))
    cutList_msonly.append('DiLep%sisCombined == 0' % (container))
    cutList_msonly.append('DiLep%sisMS == 1'       % (container))

    cutString = 'count((' + ' && '.join(cutList_combined) + ') || (' + ' && '.join(cutList_msonly) + ')) > %i' % (number-1)
    if flags.trigger: cutString = flags.trigger + ' && ' + cutString

    print cutString
    return cutString

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool as skimtool
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel

# ##########################################################################################
# Single photon filter
# ##########################################################################################

DiLepSinglePhotonFilterTool = skimtool( name       = "DiLepSinglePhotonFilterTool",
                                        expression = EgammaSelectionString(primRPVLLDESDM.DiLep_singlePhotonFilterFlags, photons, 1),
                                      )
ToolSvc     += DiLepSinglePhotonFilterTool
topSequence += kernel( "RPVLL_DiLep_SinglePhotonFilterKernel",
                       SkimmingTools = [DiLepSinglePhotonFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_SinglePhotonFilterKernel"])

# ##########################################################################################
# Single photon + X filter
# ##########################################################################################

pString  = EgammaSelectionString(primRPVLLDESDM.DiLep_singlePhotonFilterFlags, photons, 1)
apString = EgammaSelectionString(primRPVLLDESDM.DiLep_singlePhotonFilterFlags_addph, photons, 2) # 2 is not a bug! (double counting)
aeString = EgammaSelectionString(primRPVLLDESDM.DiLep_singlePhotonFilterFlags_addel, electrons, 1)
amString = MuonSelectionString(primRPVLLDESDM.DiLep_singlePhotonFilterFlags_addmu, muons, 1)
DiLepSinglePhotonXFilterTool = skimtool( name       = "DiLepSinglePhotonXFilterTool",
                                         expression = pString + ' && (' + apString + ' || ' + aeString + ' || ' + amString + ')',
                                       )
ToolSvc     += DiLepSinglePhotonXFilterTool
topSequence += kernel( "RPVLL_DiLep_SinglePhotonXFilterKernel",
                       SkimmingTools = [DiLepSinglePhotonXFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_SinglePhotonXFilterKernel"])

# ##########################################################################################
# Single electron filter
# ##########################################################################################

DiLepSingleElectronFilterTool = skimtool( name       = "DiLepSingleElectronFilterTool",
                                          expression = EgammaSelectionString(primRPVLLDESDM.DiLep_singleElectronFilterFlags, electrons, 1),
                                        )
ToolSvc     += DiLepSingleElectronFilterTool
topSequence += kernel( "RPVLL_DiLep_SingleElectronFilterKernel",
                       SkimmingTools = [DiLepSingleElectronFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_SingleElectronFilterKernel"])

# ##########################################################################################
# Single muon filter
# ##########################################################################################

DiLepSingleMuonFilterTool = skimtool( name       = "DiLepSingleMuonFilterTool",
                                      expression = MuonSelectionString(primRPVLLDESDM.DiLep_singleMuonFilterFlags, muons, 1),
                                    )
ToolSvc     += DiLepSingleMuonFilterTool
topSequence += kernel( "RPVLL_DiLep_SingleMuonFilterKernel",
                       SkimmingTools = [DiLepSingleMuonFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_SingleMuonFilterKernel"])

# ##########################################################################################
# Double photon filter
# ##########################################################################################

DiLepDiPhotonFilterTool = skimtool( name       = "DiLepDiPhotonFilterTool",
                                    expression = EgammaSelectionString(primRPVLLDESDM.DiLep_diPhotonFilterFlags, photons, 2),
                                  )
ToolSvc     += DiLepDiPhotonFilterTool
topSequence += kernel( "RPVLL_DiLep_DiPhotonFilterKernel",
                       SkimmingTools = [DiLepDiPhotonFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiPhotonFilterKernel"])

# ##########################################################################################
# Double electron filter
# ##########################################################################################

DiLepDiElectronFilterTool = skimtool( name       = "DiLepDiElectronFilterTool",
                                      expression = EgammaSelectionString(primRPVLLDESDM.DiLep_diElectronFilterFlags, electrons, 2),
                                    )
ToolSvc     += DiLepDiElectronFilterTool
topSequence += kernel( "RPVLL_DiLep_DiElectronFilterKernel",
                       SkimmingTools = [DiLepDiElectronFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiElectronFilterKernel"])

# ##########################################################################################
# Double electron+photon filters
# ##########################################################################################

eString = EgammaSelectionString(primRPVLLDESDM.DiLep_diEgammaFilterFlags_electron, electrons, 1)
gString = EgammaSelectionString(primRPVLLDESDM.DiLep_diEgammaFilterFlags_photon, photons, 1)
DiLepDiEgammaFilterTool = skimtool( name       = "DiLepDiEgammaFilterTool",
                                    expression = eString + ' && ' + gString,
                                  )
ToolSvc     += DiLepDiEgammaFilterTool
topSequence += kernel( "RPVLL_DiLep_DiEgammaFilterKernel",
                       SkimmingTools = [DiLepDiEgammaFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiEgammaFilterKernel"])

eLString = EgammaSelectionString(primRPVLLDESDM.DiLep_diLooseEgammaFilterFlags_electron, electrons, 1)
gLString = EgammaSelectionString(primRPVLLDESDM.DiLep_diLooseEgammaFilterFlags_photon, photons, 1)
DiLepDiLooseEgammaFilterTool = skimtool( name       = "DiLepDiLooseEgammaFilterTool",
                                         expression = eLString + ' && ' + gLString,
                                       )
ToolSvc     += DiLepDiLooseEgammaFilterTool
topSequence += kernel( "RPVLL_DiLep_DiLooseEgammaFilterKernel",
                       SkimmingTools = [DiLepDiLooseEgammaFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiLooseEgammaFilterKernel"])


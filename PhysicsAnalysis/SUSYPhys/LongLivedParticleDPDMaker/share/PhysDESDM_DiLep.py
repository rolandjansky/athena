from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__DiLepFilters as filters
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__DiLepSkim as skim

# ##########################################################################################
# DiLep filter definitions
# ##########################################################################################

# define cut values of the filters
ToolSvc += filters( "DiLepFilters",
                    
                    SiPhTriggers   = primRPVLLDESDM.DiLep_FilterFlags.SiPhTriggers,
                    DiPhTriggers   = primRPVLLDESDM.DiLep_FilterFlags.DiPhTriggers,
                    SiMuTriggers   = primRPVLLDESDM.DiLep_FilterFlags.SiMuTriggers,
                    SiMuBaTriggers = primRPVLLDESDM.DiLep_FilterFlags.SiMuBaTriggers,
                    
                    ElEtaMax       = primRPVLLDESDM.DiLep_FilterFlags.ElEtaMax,
                    PhEtaMax       = primRPVLLDESDM.DiLep_FilterFlags.PhEtaMax,
                    MuEtaMax       = primRPVLLDESDM.DiLep_FilterFlags.MuEtaMax,
                    MuBaEtaMax     = primRPVLLDESDM.DiLep_FilterFlags.MuBaEtaMax,
                    
                    ElD0Min        = primRPVLLDESDM.DiLep_FilterFlags.ElD0Min,
                    MuD0Min        = primRPVLLDESDM.DiLep_FilterFlags.MuD0Min,
                    
                    SiElPtMin      = primRPVLLDESDM.DiLep_FilterFlags.SiElPtMin,
                    SiPhPtMin      = primRPVLLDESDM.DiLep_FilterFlags.SiPhPtMin,
                    SiPhXPtMin     = primRPVLLDESDM.DiLep_FilterFlags.SiPhXPtMin,
                    SiMuPtMin      = primRPVLLDESDM.DiLep_FilterFlags.SiMuPtMin,
                    SiMuBaPtMin    = primRPVLLDESDM.DiLep_FilterFlags.SiMuBaPtMin,
                    DiElPtMin      = primRPVLLDESDM.DiLep_FilterFlags.DiElPtMin,
                    DiPhPtMin      = primRPVLLDESDM.DiLep_FilterFlags.DiPhPtMin,
                    DiElPhPtMin    = primRPVLLDESDM.DiLep_FilterFlags.DiElPhPtMin,
                    DiLoElPhPtMin  = primRPVLLDESDM.DiLep_FilterFlags.DiLoElPhPtMin
                  )

# ##########################################################################################
# Single electron skim tool
# ##########################################################################################

DiLepSiElectronFilterTool = skim("RPVLL_DiLep_SiElectronSkim", Filter = 1)
ToolSvc     += DiLepSiElectronFilterTool
topSequence += kernel( "RPVLL_DiLep_SiElectronFilterKernel",
                       SkimmingTools = [DiLepSiElectronFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_SiElectronFilterKernel"])

# ##########################################################################################
# Single photon + X skim tool
# ##########################################################################################

DiLepSiPhotonXFilterTool = skim("RPVLL_DiLep_SiPhotonXSkim", Filter = 2)
ToolSvc     += DiLepSiPhotonXFilterTool
topSequence += kernel( "RPVLL_DiLep_SiPhotonXFilterKernel",
                       SkimmingTools = [DiLepSiPhotonXFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_SiPhotonXFilterKernel"])

# ##########################################################################################
# Single muon skim tool
# ##########################################################################################

DiLepSiMuonFilterTool = skim("RPVLL_DiLep_SiMuonSkim", Filter = 3)
ToolSvc     += DiLepSiMuonFilterTool
topSequence += kernel( "RPVLL_DiLep_SiMuonFilterKernel",
                       SkimmingTools = [DiLepSiMuonFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_SiMuonFilterKernel"])

# ##########################################################################################
# Single muon (barrel only) skim tool
# ##########################################################################################

DiLepSiMuonBaFilterTool = skim("RPVLL_DiLep_SiMuonBaSkim", Filter = 4)
ToolSvc     += DiLepSiMuonBaFilterTool
topSequence += kernel( "RPVLL_DiLep_SiMuonBaFilterKernel",
                       SkimmingTools = [DiLepSiMuonBaFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_SiMuonBaFilterKernel"])

# ##########################################################################################
# Di electron skim tool
# ##########################################################################################

DiLepDiElectronFilterTool = skim("RPVLL_DiLep_DiElectronSkim", Filter = 5)
ToolSvc     += DiLepDiElectronFilterTool
topSequence += kernel( "RPVLL_DiLep_DiElectronFilterKernel",
                       SkimmingTools = [DiLepDiElectronFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiElectronFilterKernel"])

# ##########################################################################################
# Di photon skim tool
# ##########################################################################################

DiLepDiPhotonFilterTool = skim("RPVLL_DiLep_DiPhotonSkim", Filter = 6)
ToolSvc     += DiLepDiPhotonFilterTool
topSequence += kernel( "RPVLL_DiLep_DiPhotonFilterKernel",
                       SkimmingTools = [DiLepDiPhotonFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiPhotonFilterKernel"])

# ##########################################################################################
# Di electron+photon skim tools
# ##########################################################################################

DiLepDiElPhFilterTool = skim("RPVLL_DiLep_DiElPhSkim", Filter = 7)
ToolSvc     += DiLepDiElPhFilterTool
topSequence += kernel( "RPVLL_DiLep_DiElPhFilterKernel",
                       SkimmingTools = [DiLepDiElPhFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiElPhFilterKernel"])

DiLepDiLoElPhFilterTool = skim("RPVLL_DiLep_DiLoElPhSkim", Filter = 8)
ToolSvc     += DiLepDiLoElPhFilterTool
topSequence += kernel( "RPVLL_DiLep_DiLoElPhFilterKernel",
                       SkimmingTools = [DiLepDiLoElPhFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiLoElPhFilterKernel"])

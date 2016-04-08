from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel as kernel
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__DiLepFilters as filters
from LongLivedParticleDPDMaker.LongLivedParticleDPDMakerConf import DerivationFramework__DiLepSkim as skim

# ##########################################################################################
# DiLep filter definitions
# ##########################################################################################

# define cut values of the filters
ToolSvc += filters( "DiLepFilters",
                    
                    SiPhTrigger   = "HLT_g140_loose",
                    DiPhTrigger   = "HLT_2g50_loose",
                    SiMuTrigger   = "HLT_mu60_0eta105_msonly",
                    
                    ElEtaMax      = 2.5,
                    PhEtaMax      = 2.5,
                    MuEtaMax      = 2.5,
                    MuBaEtaMax    = 1.07,
                    
                    ElD0Min       = 2.0,
                    MuD0Min       = 1.5,
                    
                    SiElPtMin     = 140.,
                    SiPhPtMin     = 150.,
                    SiPhXPtMin    =  10.,
                    SiMuPtMin     =  60.,
                    DiElPtMin     =  50.,
                    DiPhPtMin     =  50.,
                    DiElPhPtMin   =  50.,
                    DiLoElPhPtMin =  50.
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
# Di electron skim tool
# ##########################################################################################

DiLepDiElectronFilterTool = skim("RPVLL_DiLep_DiElectronSkim", Filter = 4)
ToolSvc     += DiLepDiElectronFilterTool
topSequence += kernel( "RPVLL_DiLep_DiElectronFilterKernel",
                       SkimmingTools = [DiLepDiElectronFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiElectronFilterKernel"])

# ##########################################################################################
# Di photon skim tool
# ##########################################################################################

DiLepDiPhotonFilterTool = skim("RPVLL_DiLep_DiPhotonSkim", Filter = 5)
ToolSvc     += DiLepDiPhotonFilterTool
topSequence += kernel( "RPVLL_DiLep_DiPhotonFilterKernel",
                       SkimmingTools = [DiLepDiPhotonFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiPhotonFilterKernel"])

# ##########################################################################################
# Di electron+photon skim tools
# ##########################################################################################

DiLepDiElPhFilterTool = skim("RPVLL_DiLep_DiElPhSkim", Filter = 6)
ToolSvc     += DiLepDiElPhFilterTool
topSequence += kernel( "RPVLL_DiLep_DiElPhFilterKernel",
                       SkimmingTools = [DiLepDiElPhFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiElPhFilterKernel"])

DiLepDiLoElPhFilterTool = skim("RPVLL_DiLep_DiLoElPhSkim", Filter = 7)
ToolSvc     += DiLepDiLoElPhFilterTool
topSequence += kernel( "RPVLL_DiLep_DiLoElPhFilterKernel",
                       SkimmingTools = [DiLepDiLoElPhFilterTool],
                     )
RPVLLfilterNames.extend(["RPVLL_DiLep_DiLoElPhFilterKernel"])

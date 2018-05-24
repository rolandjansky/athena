theApp.EvtMax = 1000
import AthenaRootComps.ReadAthenaxAODHybrid
algseq =  CfgMgr.AthSequencer("AthAlgSeq")
algseq += CfgMgr.ParticleSelectionAlg("DirectIOAthAnalysisARTAlg",
                                      OutputLevel     = VERBOSE,
                                      InputContainer  = "Muons",
                                      OutputContainer = 'MyMuons',
                                      Selection       = "Muons.pt > 15.0*GeV"
                                      )

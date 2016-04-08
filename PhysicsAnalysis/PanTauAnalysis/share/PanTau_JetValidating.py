class JetValidating:

    
    def __init__ (self, JetAlgName, JetCont="Cone4TruthJets", JetRecCont="Cone4H1TopoJets"):
        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        from AthenaCommon.AppMgr import ToolSvc
        
        ##### TauTools
        #from TauTools.TauToolsConf import TauToolAthenaExample
        #tauToolExample = TauToolAthenaExample()
        #topSequence += tauToolExample
        #tauToolExample.OutputLevel  = INFO #DEBUG
        #from TauTools.TauToolsConf import AraAlgToolWrapper_TauExamples_
        #tauToolExample.TheTool                      = AraAlgToolWrapper_TauExamples_("AraAlgToolWrapper_TauExamples_")
        #tauToolExample.TheTool.MCParticleContainer  = "SpclMC"
        #tauToolExample.TheTool.TauJetContainer      = "TauRecContainer"
        #tauToolExample.TheTool.DeltaRCut            = 0.2
        ##### TauValidation
        from TauValidation.TauValidationConf import TauValATHENAExample
        #tauValAthEx = TauValATHENAExample( )
        #topSequence += tauValAthEx
        
        ## Containers
        #tauValAthEx.TruthParticleContainer = "SpclMC"
        #tauValAthEx.TauJetContainer        = "PanTauJetsCone4"
        #tauValAthEx.JetContainer           = "Cone4TruthJets"
        #tauValAthEx.JetRecContainer        = "Cone4H1TopoJets"
        #tauValAthEx.EleContainer           = "ElectronAODCollection"
        #tauValAthEx.MuonContainer          = "StacoMuonCollection"
        #tauValAthEx.TrackParticleContainer = "TrackParticleCandidate"
        #tauValAthEx.RunOverBkgSample = RunOverBkgSample
        
        ##Cut values
        #tauValAthEx.DeltaRMatchCut   = 0.2
        #tauValAthEx.LeadingChPiPtCut = 6.0
        #tauValAthEx.UseGoodCharge    = True
        #tauValAthEx.UseElectronVeto  = False
        #tauValAthEx.UseMuonVeto      = False
        
        #tauValAthEx.ROOTFileName         = "TauValidation.root"
        ##tauValAthEx.OutputLevel          = DEBUG
        ########### now the same for TauRec
        #tauValAthEx = TauValATHENAExample( "TauValAthExTauRec" )
        #topSequence += tauValAthEx
        
        ## Containers
        #tauValAthEx.TruthParticleContainer = "SpclMC"
        #tauValAthEx.TauJetContainer        = "TauRecContainer"
        #tauValAthEx.JetContainer           = "Cone4TruthJets"
        #tauValAthEx.JetRecContainer        = "Cone4H1TopoJets"
        #tauValAthEx.EleContainer           = "ElectronAODCollection"
        #tauValAthEx.MuonContainer          = "StacoMuonCollection"
        #tauValAthEx.TrackParticleContainer = "TrackParticleCandidate"
        #tauValAthEx.RunOverBkgSample = RunOverBkgSample
        
        ##Cut values
        #tauValAthEx.DeltaRMatchCut   = 0.2
        #tauValAthEx.LeadingChPiPtCut = 6.0
        #tauValAthEx.UseGoodCharge    = True
        #tauValAthEx.UseElectronVeto  = False
        #tauValAthEx.UseMuonVeto      = False
        
        #tauValAthEx.ROOTFileName         = "TauValidation-TauRec.root"
        #tauValAthEx.OutputLevel          = INFO #DEBUG
        
        
        tauValAthEx = TauValATHENAExample("TauValAthExPanTauNoChargeCut" )
        topSequence += tauValAthEx
        
        # Containers
        tauValAthEx.TruthParticleContainer = "SpclMC"
        tauValAthEx.TauJetContainer        = "PanTauJetsCone4"
        tauValAthEx.JetContainer           = "Cone4TruthJets"
        tauValAthEx.JetRecContainer        = "Cone4H1TopoJets"
        tauValAthEx.EleContainer           = "ElectronAODCollection"
        tauValAthEx.MuonContainer          = "StacoMuonCollection"
        tauValAthEx.TrackParticleContainer = "TrackParticleCandidate"
        #tauValAthEx.RunOverBkgSample = RunOverBkgSample
        
        #Cut values
        tauValAthEx.DeltaRMatchCut   = 0.2
        tauValAthEx.LeadingChPiPtCut = 6.0
        tauValAthEx.UseGoodCharge    = False
        tauValAthEx.UseElectronVeto  = False
        tauValAthEx.UseMuonVeto      = False
        
        tauValAthEx.ROOTFileName         = "TauValidationNoChargeCut.root"
        #tauValAthEx.OutputLevel          = DEBUG
        ########## now the same for TauRec
        tauValAthEx = TauValATHENAExample( "TauValAthExTauRecNoChargeCut" )
        topSequence += tauValAthEx
        
        # Containers
        tauValAthEx.TruthParticleContainer = "SpclMC"
        tauValAthEx.TauJetContainer        = "TauJets"
        tauValAthEx.JetContainer           = "Cone4TruthJets"
        tauValAthEx.JetRecContainer        = "Cone4H1TopoJets"
        tauValAthEx.EleContainer           = "ElectronAODCollection"
        tauValAthEx.MuonContainer          = "StacoMuonCollection"
        tauValAthEx.TrackParticleContainer = "TrackParticleCandidate"
        #tauValAthEx.RunOverBkgSample = RunOverBkgSample
        
        #Cut values
        tauValAthEx.DeltaRMatchCut   = 0.2
        tauValAthEx.LeadingChPiPtCut = 6.0
        tauValAthEx.UseGoodCharge    = False
        tauValAthEx.UseElectronVeto  = False
        tauValAthEx.UseMuonVeto      = False
        
        tauValAthEx.ROOTFileName         = "TauValidation-TauRecNoChargeCut.root"
        tauValAthEx.OutputLevel          = INFO #DEBUG                
    #end def __init__
#end class
        

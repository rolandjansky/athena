# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from TrigTauRec.TrigTauRecConf import TrigTauRecMerged
from TrigTauRec.TrigTauRecMonitoring import tauMonitoringCaloOnlyMVA,  tauMonitoringPrecisionMVA

class TrigTauRecMerged_TauCaloOnlyMVA (TrigTauRecMerged) :

        def __init__(self, name = "TrigTauRecMerged_TauCaloOnlyMVA"):
            super( TrigTauRecMerged_TauCaloOnlyMVA , self ).__init__( name )
            self.MonTool = tauMonitoringCaloOnlyMVA()
            self._mytools = [] 
          
            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            taualgs.setPrefix("TrigTauCaloOnlyMVA_")


            # Only include tools needed for calo pre-selection

            # Set seedcalo energy scale (Full RoI)
            tools.append(taualgs.getJetSeedBuilder())
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)
            tools.append(taualgs.getTauAxis())
            # Decorate the clusters
            tools.append(taualgs.getTauClusterFinder())
            tools.append(taualgs.getTauVertexedClusterDecorator())
            # Calibrate to TES
            tools.append(taualgs.getEnergyCalibrationLC(caloOnly=True))
            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))
            # Compute MVA TES (ATR-17649), stores MVA TES as default tau pt()
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())

            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'

            self.Tools = tools

class TrigTauRecMerged_TauPrecisionMVA (TrigTauRecMerged) :

        def __init__(self, name = "TrigTauRecMerged_TauPrecisionMVA", doTrackBDT=False, doRNN=False, doLLP=False):
        
            super( TrigTauRecMerged_TauPrecisionMVA , self ).__init__( name )
            self.MonTool = tauMonitoringPrecisionMVA()

            import TrigTauRec.TrigTauAlgorithmsHolder as taualgs
            tools = []

            # using same prefix as in TauPrecision sequence should be safe if tools with different configurations have different names
            # e.g. TauTrackFinder in 2016 using dz0=2mm instead of 1mm in 2017
            taualgs.setPrefix("TrigTau_")

            # Include full set of tools

            # Associate RoI vertex or Beamspot to tau - don't use TJVA
            tools.append(taualgs.getTauVertexFinder(doUseTJVA=False)) #don't use TJVA by default
            # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)       

            tools.append(taualgs.getTauAxis())
            
            # tightened to 0.75 mm for tracktwoMVA (until the track BDT can be used)
            tools.append(taualgs.getTauTrackFinder(applyZ0cut=True, maxDeltaZ0=0.75, prefix='TrigTauTightDZ_'))            
            
            # Decorate the clusters
            tools.append(taualgs.getTauClusterFinder())
            tools.append(taualgs.getTauVertexedClusterDecorator())

            if doTrackBDT:                
                # BDT track classification
                tools.append(taualgs.getTauTrackClassifier())

            # Calibrate to calo TES
            tools.append(taualgs.getEnergyCalibrationLC())

            # Compute MVA TES (ATR-17649), stores MVA TES as default tau pt()
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())

            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))

            # Lifetime variables
            tools.append(taualgs.getTauVertexVariables())
            # Variables combining tracking and calorimeter information
            tools.append(taualgs.getTauCommonCalcVars())
            # Cluster-based sub-structure, with dRMax also
            tools.append(taualgs.getTauSubstructure())
            #Run either nominal or LLP rnn, not both 

            from AthenaCommon.Logging import log
            if doLLP and doRNN:
               log.warning( "WARNING doLLP and doRNN flags both set to True -> will set doRNN to False")
               doRNN = False
               

            if doLLP:
                # RNN tau ID for displaced tau signatures (placeholder configs)
                tools.append(taualgs.getTauJetRNNEvaluator(NetworkFile0P="llpdev/net_experimental_llz_0p.json",
                                                           NetworkFile1P="llpdev/net_experimental_llz_1p.json",
                                                           NetworkFile3P="llpdev/net_experimental_llz_mp.json",
                                                           MaxTracks=10,
                                                           MaxClusters=6,
                                                           MaxClusterDR=1.0,
                                                           suffix = "_LLP"))
                # flattened RNN score and WP
                tools.append(taualgs.getTauWPDecoratorJetLLP())       
            elif doRNN:
                # RNN tau ID
                tools.append(taualgs.getTauJetRNNEvaluator(NetworkFile0P="rnnid_config_0p_v3.json",
                                                           NetworkFile1P="rnnid_config_1p_v3.json",
                                                           NetworkFile3P="rnnid_config_mp_v3.json",
                                                           MaxTracks=10, 
                                                           MaxClusters=6,
                                                           MaxClusterDR=1.0))
                # flattened RNN score and WP
                tools.append(taualgs.getTauWPDecoratorJetRNN())

            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = 'TrigTauRec/00-11-02/'

            self.Tools = tools

            ## add beam type flag
            from AthenaCommon.BeamFlags import jobproperties
            self.BeamType = jobproperties.Beam.beamType()



# this is the newJO fragment
class TrigTauDefaultsKeys:
    VertexContainer = 'PrimaryVertices'
    TrackContainer ='InDetTrackParticles'
    LargeD0TrackContainer ='InDetLargeD0TrackParticles'


def TrigTauRecMergedOnlyMVACfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    acc = ComponentAccumulator()
    # prepare tools 
    tools = []
    # Set seedcalo energy scale (Full RoI)
    tools.append(CompFactory.JetSeedBuilder())

    # Set LC energy scale (0.2 cone) and intermediate axis (corrected for vertex: useless at trigger)
    tools.append(CompFactory.TauAxisSetter(ClusterCone = 0.2,
                                           VertexCorrection = False))
    # Decorate the clusters
    tools.append(CompFactory.TauClusterFinder(JetVertexCorrection = False)) # TODO use JetRec.doVertexCorrection once available

    tools.append(CompFactory.TauVertexedClusterDecorator(SeedJet = flags.Tau.SeedJetCollection))

    # Calibrate to TES
    tools.append(CompFactory.TauCalibrateLC(calibrationFile = flags.Tau.CalibrateLCConfig,
                                            Key_vertexInputContainer = ""))
    # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
    from AthenaCommon.SystemOfUnits import GeV
    tools.append(CompFactory.TauCellVariables(StripEthreshold = 0.2*GeV,
                                                CellCone = 0.2,
                                                VertexCorrection = False))
    # Compute MVA TES (ATR-17649), stores MVA TES as default tau pt()
    tools.append(CompFactory.MvaTESVariableDecorator(Key_vertexInputContainer='',
                                                     EventShapeKey='',
                                                     VertexCorrection = False))
    tools.append(CompFactory.MvaTESEvaluator(WeightFileName = flags.Tau.MvaTESConfig))

    for tool in tools:
        tool.inTrigger = True
        tool.calibFolder = 'TrigTauRec/00-11-02/'


            ## add beam type flag
    alg = CompFactory.TrigTauRecMerged("TrigTauRecMergedOnlyMVA",
                                        BeamType=flags.Beam.Type, 
                                        Tools=tools)

    alg.Key_trackPartInputContainer = ''
    alg.Key_trigJetSeedOutputKey = 'HLT_jet_seed' 
    alg.Key_trigTauJetInputContainer = ''
    alg.Key_trigTauJetOutputContainer = 'HLT_TrigTauRecMerged_CaloMVAOnly'
    alg.Key_trigTauTrackInputContainer = ''
    alg.Key_trigTauTrackOutputContainer = 'HLT_tautrack_dummy' 
    alg.Key_vertexInputContainer = ''
    alg.clustersKey = 'HLT_TopoCaloClustersLC'
    alg.L1RoIKey = 'HLT_TAURoI'
    alg.RoIInputKey = 'UpdatedCaloRoI'
    acc.addEventAlgo(alg)

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags.Input.Files = defaultTestFiles.RAW
    flags.lock()

    acc = TrigTauRecMergedOnlyMVACfg(flags)
    acc.printConfig(withDetails=True, summariseProps=True)
    acc.wasMerged() # do not run, do not save, we just want to see the config

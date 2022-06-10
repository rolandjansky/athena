# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
            # Calculate cell-based quantities: strip variables, EM and Had energies/radii, centFrac, isolFrac and ring energies
            tools.append(taualgs.getCellVariables(cellConeSize=0.2))
            # Compute MVA TES (ATR-17649), stores MVA TES as default tau pt()
            tools.append(taualgs.getMvaTESVariableDecorator())
            tools.append(taualgs.getMvaTESEvaluator())

            from AthenaConfiguration.AllConfigFlags import ConfigFlags
            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = ConfigFlags.Trigger.Offline.Tau.tauRecToolsCVMFSPath

            self.Tools = tools

class TrigTauRecMerged_TauPrecisionMVA (TrigTauRecMerged) :

        def __init__(self, name = "TrigTauRecMerged_TauPrecisionMVA", doTrackBDT=False, doLLP=False):
        
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

            from AthenaCommon.Logging import log
            if doTrackBDT:
                # BDT track classification is deprecated, RNN track classification feasibility under study
                log.warning( "BDT track classifier is deprecated and won't be scheduled")

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

            # RNN tau ID, either nominal or LLP
            tools.append(taualgs.getTauJetRNNEvaluator(LLP = doLLP))
            # flattened RNN score and WP
            tools.append(taualgs.getTauWPDecoratorJetRNN(LLP = doLLP))

            from AthenaConfiguration.AllConfigFlags import ConfigFlags
            for tool in tools:
                tool.inTrigger = True
                tool.calibFolder = ConfigFlags.Trigger.Offline.Tau.tauRecToolsCVMFSPath

            self.Tools = tools

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
    tools.append(CompFactory.TauClusterFinder(UseOriginalCluster = False)) # TODO use JetRec.doVertexCorrection once available

    tools.append(CompFactory.TauVertexedClusterDecorator(SeedJet = flags.Trigger.Offline.Tau.SeedJetCollection))

    # Calibrate to TES
    # non-MVA TES calibration is deprecated and should be phased out
    tools.append(CompFactory.TauCalibrateLC(calibrationFile = flags.Trigger.Offline.Tau.CalibrateLCConfig,
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
    tools.append(CompFactory.MvaTESEvaluator(WeightFileName = flags.Trigger.Offline.Tau.MvaTESConfig))

    for tool in tools:
        tool.inTrigger = True
        tool.calibFolder = flags.Trigger.Offline.Tau.tauRecToolsCVMFSPath


    ## add beam type flag
    alg = CompFactory.TrigTauRecMerged("TrigTauRecMergedOnlyMVA",
                                        Tools=tools)

    alg.Key_trackPartInputContainer = ''
    alg.Key_trigJetSeedOutputKey = 'HLT_jet_seed' 
    alg.Key_trigTauJetInputContainer = ''
    alg.Key_trigTauJetOutputContainer = 'HLT_TrigTauRecMerged_CaloMVAOnly'
    alg.Key_trigTauTrackInputContainer = ''
    alg.Key_trigTauTrackOutputContainer = 'HLT_tautrack_dummy' 
    alg.Key_vertexInputContainer = ''
    alg.clustersKey = 'HLT_TopoCaloClustersLC'
    alg.RoIInputKey = 'UpdatedCaloRoI'
    acc.addEventAlgo(alg)

    return acc


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags.Input.Files = defaultTestFiles.RAW
    flags.lock()

    acc = TrigTauRecMergedOnlyMVACfg(flags)
    acc.printConfig(withDetails=True, summariseProps=True)
    acc.wasMerged() # do not run, do not save, we just want to see the config

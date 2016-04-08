
import AthenaCommon.SystemOfUnits as Units


# set up the job properties
from PanTauAnalysis.Class_InformationHandler import InformationHandler
infoHandler = InformationHandler()


# copy the configuration from cellbased if requested
from PanTauAnalysis.Config_PanTau import config_PanTau
from tauRec.tauRecFlags import jobproperties as config_TauRec
if config_PanTau.UseDefaultCellBasedConfig == True:
    
    # pi0-BDT cuts
    config_PanTau.CellBased_EtaBinned_Pi0MVACut_1prong.set_Value_and_Lock( config_TauRec.tauRecFlags.pi0MVACuts_1prong() )
    config_PanTau.CellBased_EtaBinned_Pi0MVACut_3prong.set_Value_and_Lock( config_TauRec.tauRecFlags.pi0MVACuts_mprong() )
    
    # Et cuts
    config_PanTau.TauConstituents_Selection_Neutral_EtaBinned_EtCut.set_Value_and_Lock( config_TauRec.tauRecFlags.pi0EtCuts() )
    config_PanTau.TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut.set_Value_and_Lock( config_TauRec.tauRecFlags.pi0EtCuts() )
    
    #print(config_PanTau.CellBased_EtaBinned_Pi0MVACut_1prong)
    #print(config_PanTau.CellBased_EtaBinned_Pi0MVACut_3prong)
    #print(config_PanTau.TauConstituents_Selection_Neutral_EtaBinned_EtCut)
    #print(config_PanTau.TauConstituents_Selection_Pi0Neut_EtaBinned_EtCut)
    
    # Placeholder for future config
    #config_PanTau.foobar.set_Value_And_Lock( config_TauRec. )
    
#end



# Create tools
# ==================================================================
from AthenaCommon.AppMgr import ToolSvc
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_InformationStore
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_InputConverter
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_TauConstituentGetter
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_TauConstituentSelector
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_FeatureExtractor
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_HelperFunctions
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_ModeDiscriminator
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_DecayModeDeterminator
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_FourMomentumCalculator
from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_DetailsArranger
from PanTauAlgs.PanTauAlgsConf import PanTau__Alg_SeedBuilder

from TrkVertexFitterUtils.TrkVertexFitterUtilsConf import Trk__TrackToVertexIPEstimator
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
theAtlasExtrapolator=AtlasExtrapolator()
ToolSvc+=theAtlasExtrapolator

python_Tool_TrackToVertexIPEstimator = Trk__TrackToVertexIPEstimator(Extrapolator = theAtlasExtrapolator)
ToolSvc += python_Tool_TrackToVertexIPEstimator

# ===> Information Store
python_Tool_InformationStore = PanTau__Tool_InformationStore(   "PanTau_InformationStore",
                                                                Infos_Int       = infoHandler.m_Infos_Int,
                                                                Infos_Double    = infoHandler.m_Infos_Double,
                                                                Infos_VecDouble = infoHandler.m_Infos_VecDouble,
                                                                Infos_String    = infoHandler.m_Infos_String,
                                                                Infos_VecString = infoHandler.m_Infos_VecString)
python_Tool_InformationStore.OutputLevel = INFO
ToolSvc += python_Tool_InformationStore

# ===> Helper Functions
python_Tool_HelperFunctions = PanTau__Tool_HelperFunctions("PanTau_HelperFunctions")
python_Tool_HelperFunctions.OutputLevel = INFO
ToolSvc += python_Tool_HelperFunctions

# ===> Input Converter
python_Tool_InputConverter  = PanTau__Tool_InputConverter("PanTau_InputConverter",
                                                            Tool_InformationStore = python_Tool_InformationStore,
                                                            Tool_HelperFunctions  = python_Tool_HelperFunctions)
python_Tool_InputConverter.OutputLevel = INFO
ToolSvc += python_Tool_InputConverter


# ===> Tau Constituent Getter
python_Tool_TauConstituentGetter = PanTau__Tool_TauConstituentGetter(   "PanTau_TauConstituentGetter",
                                                                        Tool_InformationStore = python_Tool_InformationStore,
                                                                        Tool_InputConverter   = python_Tool_InputConverter,
                                                                        Tool_HelperFunctions  = python_Tool_HelperFunctions)
python_Tool_TauConstituentGetter.OutputLevel = INFO
ToolSvc += python_Tool_TauConstituentGetter

# ===> Tau Constituent Selector
python_Tool_TauConstituentSelector = PanTau__Tool_TauConstituentSelector( "PanTau_TauConstituentSelector",
                                                                        Tool_InformationStore = python_Tool_InformationStore)
python_Tool_TauConstituentSelector.OutputLevel = INFO
ToolSvc += python_Tool_TauConstituentSelector


# ===> Tau Feature Extractor
python_Tool_FeatureExtractor = PanTau__Tool_FeatureExtractor(   "PanTau_FeatureExtractor",
                                                                Tool_HelperFunctions            = python_Tool_HelperFunctions,
                                                                Tool_TrackToVertexIPEstimator   = python_Tool_TrackToVertexIPEstimator,
                                                                Tool_InformationStore           = python_Tool_InformationStore
                                                                )
python_Tool_FeatureExtractor.OutputLevel = INFO
ToolSvc += python_Tool_FeatureExtractor


# ===> Tau Four Momentum Calculator
python_Tool_FourMomentumCalculator = PanTau__Tool_FourMomentumCalculator(   "PanTau_FourMomentumCalculator",
                                                                            Tool_InformationStore           = python_Tool_InformationStore)
python_Tool_FourMomentumCalculator.OutputLevel = INFO
ToolSvc += python_Tool_FourMomentumCalculator


# ===> Details arranger tool
python_Tool_DetailsArranger = PanTau__Tool_DetailsArranger( "PanTau_DetailsArranger",
                                                            Tool_InformationStore           = python_Tool_InformationStore)
python_Tool_DetailsArranger.OutputLevel = INFO
ToolSvc += python_Tool_DetailsArranger


# ================================================
# Decay Mode Separation Tools
#
# One tool for each mode + one tool to use them
# need to have the Discri Tools for each input alg, because
# based on the input alg, the weight and reference files differ

from PanTauAnalysis.Config_PanTau import config_PanTau
List_InputAlgs      = config_PanTau.Names_InputAlgorithms()

for curInAlg in List_InputAlgs:
    print("TopOptions_NewPanTau: Adding PanTau algorithms for input alg: " + curInAlg)
    
    
    # ===> create the discri tools for this input algorithm
    python_Tool_ModeDiscri_1p0n_vs_1p1n = PanTau__Tool_ModeDiscriminator(   "PanTau_ModeDiscri_1p0n_vs_1p1n_" + curInAlg,
                                                                            Name_InputAlg           = curInAlg,
                                                                            Name_ModeCase           = "1p0n_vs_1p1n",
                                                                            Tool_InformationStore   = python_Tool_InformationStore,
                                                                            Tool_HelperFunctions    = python_Tool_HelperFunctions)
    python_Tool_ModeDiscri_1p0n_vs_1p1n.OutputLevel = INFO
    ToolSvc += python_Tool_ModeDiscri_1p0n_vs_1p1n
    
    python_Tool_ModeDiscri_1p1n_vs_1pXn = PanTau__Tool_ModeDiscriminator(   "PanTau_ModeDiscri_1p1n_vs_1pXn_" + curInAlg,
                                                                            Name_InputAlg           = curInAlg,
                                                                            Name_ModeCase           = "1p1n_vs_1pXn",
                                                                            Tool_InformationStore   = python_Tool_InformationStore,
                                                                            Tool_HelperFunctions    = python_Tool_HelperFunctions)
    python_Tool_ModeDiscri_1p1n_vs_1pXn.OutputLevel = INFO
    ToolSvc += python_Tool_ModeDiscri_1p1n_vs_1pXn
    
    python_Tool_ModeDiscri_3p0n_vs_3pXn = PanTau__Tool_ModeDiscriminator(   "PanTau_ModeDiscri_3p0n_vs_3pXn_" + curInAlg,
                                                                            Name_InputAlg           = curInAlg,
                                                                            Name_ModeCase           = "3p0n_vs_3pXn",
                                                                            Tool_InformationStore   = python_Tool_InformationStore,
                                                                            Tool_HelperFunctions    = python_Tool_HelperFunctions)
    python_Tool_ModeDiscri_3p0n_vs_3pXn.OutputLevel = INFO
    ToolSvc += python_Tool_ModeDiscri_3p0n_vs_3pXn
    
    # ===> Tau Decay Mode Determinator for current input alg
    Name_DecayModeDeterminator = "PanTau_DecayModeDeterminator_" + curInAlg
    python_Tool_DecayModeDeterminator = PanTau__Tool_DecayModeDeterminator( Name_DecayModeDeterminator,
                                                                            Tool_InformationStore               = python_Tool_InformationStore,
                                                                            Tool_ModeDiscriminator_1p0n_vs_1p1n = python_Tool_ModeDiscri_1p0n_vs_1p1n,
                                                                            Tool_ModeDiscriminator_1p1n_vs_1pXn = python_Tool_ModeDiscri_1p1n_vs_1pXn,
                                                                            Tool_ModeDiscriminator_3p0n_vs_3pXn = python_Tool_ModeDiscri_3p0n_vs_3pXn)
    python_Tool_DecayModeDeterminator.OutputLevel = INFO
    ToolSvc += python_Tool_DecayModeDeterminator
    
    
    # ===> The main algorithm
    Name_SeedBuilder = "PanTau_SeedBuilder_" + curInAlg
    python_Alg_SeedBuilder = PanTau__Alg_SeedBuilder(  Name_SeedBuilder,
                                                Name_InputAlg               = curInAlg,
                                                Tool_InformationStore       = python_Tool_InformationStore,
                                                Tool_TauConstituentGetter   = python_Tool_TauConstituentGetter,
                                                Tool_TauConstituentSelector = python_Tool_TauConstituentSelector,
                                                Tool_FeatureExtractor       = python_Tool_FeatureExtractor,
                                                Tool_DecayModeDeterminator  = python_Tool_DecayModeDeterminator,
                                                Tool_FourMomentumCalculator = python_Tool_FourMomentumCalculator,
                                                Tool_DetailsArranger        = python_Tool_DetailsArranger)
    python_Alg_SeedBuilder.OutputLevel = INFO
    topSequence += python_Alg_SeedBuilder
    
    
#end for loop over input algs




#end of file


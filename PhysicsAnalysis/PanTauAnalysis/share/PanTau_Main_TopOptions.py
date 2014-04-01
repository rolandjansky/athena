
#/////////////////////////////////////
#//    Imports
#/////////////////////////////////////


from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.GlobalFlags  import globalflags
import AthenaPoolCnvSvc.ReadAthenaPool

from RecExConfig.RecFlags import rec
from RecExConfig.InputFilePeeker import inputFileSummary

from PanTauAnalysis.PanTauFlags import panTauFlags
from PanTauAnalysis.PanTauDPDFlags import panTauDPDFlags
from PanTauAnalysis.PanTauValidationFlags import panTauValidationFlags
from PanTauAnalysis.PanTauSeedBuilderAlg_Flags import PanTauSeedBuilderAlgFlags as PSB_Flags

#/////////////////////////////////////
#//    Set flags
#/////////////////////////////////////

#Validation flags
doTauValidation = False # this should be added to PanTauFlags, no?
panTauValidationFlags.DoFeatureDefaultSmearing = False
panTauValidationFlags.DoSeedEfficiency = False              # FIXME: this alg requires the TauSeedCollection to be present
panTauValidationFlags.DoFeatureWriter = False               # FIXME: this alg requires the TauSeedCollection to be present
panTauValidationFlags.DoSubClusterEfficiency = False        # FIXME: this alg requires the TauSeedCollection to be present

panTauDPDFlags.FilterEvents = False
panTauDPDFlags.UseGRL = False
panTauDPDFlags.TriggerSelectionExpression = 'L1_TAU5'


#/////////////////////////////////////
#//    Process flags
#/////////////////////////////////////


### Feature List & Variable Smearing ##################################
include( "PanTauAnalysis/featureList.py" )
#if panTauValidationFlags.DoFeatureDefaultSmearing() :
    
#else :
    ##include( "PanTauAnalysis/featureList-nodefaultShift-short.py" )
    #include( "PanTauAnalysis/featureList-nodefaultShift.py" )
allFeatureNames=[]
allFeatureDefaultValues=[]
allfeatureDefaultValueSmearingSigma=[]
allFeatureTypes=[]
for k, v in featureDefaultValue.iteritems():
    allFeatureNames.append(k)
    allFeatureDefaultValues.append(v)
    if k in featureDefaultValueSmearingSigma :
        allfeatureDefaultValueSmearingSigma.append(featureDefaultValueSmearingSigma[k])
    else :
        print "PanTau_Main_TopOptions ERROR: no sigma for default value smearing for feature ", k
        allfeatureDefaultValueSmearingSigma.append(0.)
    if k in featureType:
        allFeatureTypes.append(featureType[k])
    else:
        print "PanTau_Main_TopOptions ERROR: no type for feature called ", k

panTauValidationFlags.AllFeatureNames                       = allFeatureNames
panTauValidationFlags.AllFeatureDefaultValues               = allFeatureDefaultValues
panTauValidationFlags.AllFeatureDefaultValueSmearingSigma   = allfeatureDefaultValueSmearingSigma
panTauValidationFlags.AllFeatureTypes                       = allFeatureTypes

### Create Algorithm Sequence ##################################

topSequence = AlgSequence()


### Check for data / mc ##################################

if globalflags.DataSource()=='data' :
    panTauDPDFlags.UseGRL = True
else  :
    panTauDPDFlags.UseGRL = False


#add Truth Taus to SG if doTruth is required
if rec.doTruth():
    #get decay mode information: dump true taus to TruthTaus in SG in special format to get decay mode
    from TauTrackTools.TauTrackToolsConf import TauID__TruthTauCreatorAlg
    TruthContName = ""
    if rec.readESD():
        TruthContName = "INav4MomTruthEvent"
    if rec.readAOD():
        TruthContName = "SpclMC"
        
    print("PanTau_Main_TopOptions: updated TauID__TruthTauCreatorAlg to be " + TruthContName)
    TauID__TruthTauCreatorAlg.InputTruthParticleContainer = TruthContName
    topSequence += TauID__TruthTauCreatorAlg()
#end if


### Run Time Histograms ##################################
if PSB_Flags.Config_DoRunTimeHists():
    include("PanTauAnalysis/PanTau_RunTimeHists_TopOptions.py")

### Seed- and Jet Building ##################################

IntMapTest = {}
IntMapTest["intA"] = 42;
IntMapTest["intB"] = 4711;

DoubleMapTest = {}
DoubleMapTest["doubleA"] = 13.37;
DoubleMapTest["doubleB"] = 47.11;

StringMapTest = {}
StringMapTest["strA"] = "stringA";
StringMapTest["strB"] = "stringB";

from PanTauAlgs.PanTauAlgsConf import PanTau__Tool_InformationStore
Tool_InformationStore = PanTau__Tool_InformationStore(  'TestToolInformationStore',
                                                        Infos_Int       = IntMapTest,
                                                        Infos_Double    = DoubleMapTest,
                                                        Infos_String    = StringMapTest)


include ("PanTauAnalysis/PanTau_Seeding.py")

if panTauFlags.DoTauJetBuilding() :
    include ("PanTauAnalysis/PanTau_JetBuilding.py")
    if doTauValidation:
        include ("PanTauAnalysis/PanTau_JetValidating.py")
#   NOTE: JetBuildingClass: Builds tau jets including particle ID (via DiscriminatorTools, DiscrTools) with TMVA
#   NOTE: JetValidatingClass: Framework for TauValidation rtt


ListOfJetAlgs = []
ListOfJetAlgs += ['AntiKt4']
#   NOTE: Somewhat historic. Adding More JetAlgorithms to the list will create output for each Jet Alg.
#   NOTE: However, now the seeds are taken from tauRec candidates, so there is not much choice

for iJetAlg in ListOfJetAlgs:
    Seeding = TauSeeding(iJetAlg)

    if panTauFlags.DoTauJetBuilding():
        Building = JetBuilding(iJetAlg)
        
        if doTauValidation:
            Validating = JetValidating(iJetAlg)



#end of file


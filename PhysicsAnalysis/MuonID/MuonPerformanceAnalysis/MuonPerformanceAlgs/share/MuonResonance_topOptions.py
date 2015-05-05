from glob import glob
from optparse import OptionParser

# MuonResonance_topOptions.py
#
# Specify input file.
#include ('MuonPerformanceAlgs/share/Zmumu_20.1.3.3.py')

path_testDATA = '/afs/cern.ch/user/f/fcardill/Qualification/MuonValidation/Testfiles/DATA/TEST/data12_8TeV.00204158.physics_Muons.merge.AOD.r6499_p2319_tid05173685_00/'
InputAODList = glob(path_testDATA+'*.root*')


print "Input files:"
for infile in InputAODList: print infile

#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = InputAODList

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (-1 for all)
#theApp.EvtMax = 200
theApp.EvtMax = -1
print "Maximum number of events to be processed:",theApp.EvtMax

# Some general environment variables 
#Resonances = ['Z', 'Jpsi', 'Y']
Resonances = ['Z']
doCorrection = False
doEfficiency = False
doSystematics = False

#Systematics = ['MUONSFSTAT', 'MUONSFSYS', 'MUONS_ID', 'MUONS_MS', 'MUONS_SCALE']
Systematics = ['MUONS_ID', 'MUONS_MS']

print "Resonances ::", Resonances


#--------------------------------------------------------------
# Configure algorithm.
#--------------------------------------------------------------

# Allow messge service more than default 500 lines.
ServiceMgr.MessageSvc.infoLimit = 100000

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()


##########################################################################################
# Add the Zmm TP algorithm using ID tracks as probes and matching to muons
from MuonPerformanceAlgs.MuonPerformanceAlgsConf import MuonResonanceAlg
job += MuonResonanceAlg("MuonResonanceAlg")
job.MuonResonanceAlg.MuonContainerName   = "Muons"

#Create efficiency and correction tools
from MuonEfficiencyCorrections.MuonEfficiencyCorrectionsConf import CP__MuonEfficiencyScaleFactors
SFtool = CP__MuonEfficiencyScaleFactors("MCPTool")
SFtool.WorkingPoint = "CBandST"
SFtool.DataPeriod = "2012"
SFtool.OutputLevel=INFO

from MuonMomentumCorrections.MuonMomentumCorrectionsConf import CP__MuonCalibrationAndSmearingTool
CalibrationTool = CP__MuonCalibrationAndSmearingTool()
CalibrationTool.Year = 'Data12'
CalibrationTool.Release = 'Rel17.2Sum13'
CalibrationTool.OutputLevel=INFO

from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonSelectionTool
SelectionTool = CP__MuonSelectionTool()
SelectionTool.OutputLevel=INFO

from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
TriggerTool = Trig__TrigDecisionTool("TrigDecTool")
ToolSvc += TriggerTool
TriggerTool.TrigDecisionKey="xTrigDecision"
TriggerTool.OutputLevel=INFO

from TrigMuonMatching.TrigMuonMatchingConf import Trig__TrigMuonMatching
MatchingTool = Trig__TrigMuonMatching("TrigMatchTool");
MatchingTool.dRmin = 0.1
MatchingTool.LVL1dRmin = 0.2
MatchingTool.TriggerTool = TriggerTool 
MatchingTool.OutputLevel=INFO

# Create an instance of the Resonance selection tool for Zmumu
from MuonResonanceTools.MuonResonanceToolsConf import MuonResonanceSelectionTool 
ToolSvc += MuonResonanceSelectionTool("MuonZSelectionTool")
ToolSvc.MuonZSelectionTool.PtCut = 20000
ToolSvc.MuonZSelectionTool.EtaCut = 2.5
ToolSvc.MuonZSelectionTool.IsoCaloCut = 0.12
ToolSvc.MuonZSelectionTool.IsoTrkCut = 0.12
ToolSvc.MuonZSelectionTool.Sigd0Cut = 99.
ToolSvc.MuonZSelectionTool.z0Cut = 99.
ToolSvc.MuonZSelectionTool.Max_d0 = 2.0
ToolSvc.MuonZSelectionTool.Max_z0 = 10.0
ToolSvc.MuonZSelectionTool.UseIDCuts = True
ToolSvc.MuonZSelectionTool.ScaleFactorTool = SFtool
ToolSvc.MuonZSelectionTool.MuonSelectionTool = SelectionTool
ToolSvc.MuonZSelectionTool.MuonCalibrationTool = CalibrationTool
ToolSvc.MuonZSelectionTool.TriggerDecisionTool = TriggerTool 
ToolSvc.MuonZSelectionTool.TriggerMatchingTool = MatchingTool
ToolSvc.MuonZSelectionTool.TriggerList = ["HLT_mu26_imedium"]
ToolSvc.MuonZSelectionTool.Calibrate = doCorrection
ToolSvc.MuonZSelectionTool.EfficiencyCorr = doEfficiency
ToolSvc.MuonZSelectionTool.OutputLevel=INFO

# Create an instance of the Resonance selection tool for lower resonances
ToolSvc += MuonResonanceSelectionTool("MuonYSelectionTool")
ToolSvc.MuonYSelectionTool.PtCut = 5000
ToolSvc.MuonYSelectionTool.EtaCut = 2.5
ToolSvc.MuonYSelectionTool.IsoCaloCut = 0.12
ToolSvc.MuonYSelectionTool.IsoTrkCut = 0.12
ToolSvc.MuonYSelectionTool.Sigd0Cut = 99.
ToolSvc.MuonYSelectionTool.z0Cut = 99.
ToolSvc.MuonYSelectionTool.Max_d0 = 3.0
ToolSvc.MuonYSelectionTool.Max_z0 = 10.0
ToolSvc.MuonYSelectionTool.UseIDCuts = True
ToolSvc.MuonYSelectionTool.ScaleFactorTool = SFtool
ToolSvc.MuonYSelectionTool.MuonSelectionTool = SelectionTool
ToolSvc.MuonYSelectionTool.MuonCalibrationTool = CalibrationTool
ToolSvc.MuonYSelectionTool.TriggerDecisionTool = TriggerTool
ToolSvc.MuonYSelectionTool.TriggerMatchingTool = MatchingTool
ToolSvc.MuonYSelectionTool.TriggerList = []
ToolSvc.MuonYSelectionTool.Calibrate = doCorrection
ToolSvc.MuonYSelectionTool.EfficiencyCorr = doEfficiency
ToolSvc.MuonYSelectionTool.OutputLevel=INFO

# Create an instance of the Resonance selection tool for lower resonances
ToolSvc += MuonResonanceSelectionTool("MuonJpsiSelectionTool")
ToolSvc.MuonJpsiSelectionTool.PtCut = 5000
ToolSvc.MuonJpsiSelectionTool.EtaCut = 2.5
ToolSvc.MuonJpsiSelectionTool.IsoCaloCut = 0.12
ToolSvc.MuonJpsiSelectionTool.IsoTrkCut = 0.12
ToolSvc.MuonJpsiSelectionTool.Sigd0Cut = 99.
ToolSvc.MuonJpsiSelectionTool.z0Cut = 99.
ToolSvc.MuonJpsiSelectionTool.Max_d0 = 2.0
ToolSvc.MuonJpsiSelectionTool.Max_z0 = 10.0
ToolSvc.MuonJpsiSelectionTool.UseIDCuts = True
ToolSvc.MuonJpsiSelectionTool.ScaleFactorTool = SFtool
ToolSvc.MuonJpsiSelectionTool.MuonSelectionTool = SelectionTool
ToolSvc.MuonJpsiSelectionTool.MuonCalibrationTool = CalibrationTool
ToolSvc.MuonJpsiSelectionTool.TriggerDecisionTool = TriggerTool
ToolSvc.MuonJpsiSelectionTool.TriggerMatchingTool = MatchingTool
ToolSvc.MuonJpsiSelectionTool.TriggerList = []
ToolSvc.MuonJpsiSelectionTool.Calibrate = doCorrection
ToolSvc.MuonJpsiSelectionTool.EfficiencyCorr = doEfficiency
ToolSvc.MuonJpsiSelectionTool.OutputLevel=INFO


# Create instances of the Resonance pairing tool
from MuonResonanceTools.MuonResonanceToolsConf import MuonResonancePairingTool 
# Zmumu
ToolSvc += MuonResonancePairingTool("ZmumuResonancePairingTool")
ToolSvc.ZmumuResonancePairingTool.HighMassWindow = 106000
ToolSvc.ZmumuResonancePairingTool.LowMassWindow = 76000
ToolSvc.ZmumuResonancePairingTool.OutputLevel=INFO

# Y
ToolSvc += MuonResonancePairingTool("YmumuResonancePairingTool")
ToolSvc.YmumuResonancePairingTool.HighMassWindow = 13000
ToolSvc.YmumuResonancePairingTool.LowMassWindow = 7000
ToolSvc.YmumuResonancePairingTool.OutputLevel=INFO

#J/Psi
ToolSvc += MuonResonancePairingTool("JpsimumuResonancePairingTool")
ToolSvc.JpsimumuResonancePairingTool.HighMassWindow = 4000
ToolSvc.JpsimumuResonancePairingTool.LowMassWindow = 2700
ToolSvc.JpsimumuResonancePairingTool.OutputLevel=INFO


include ("MuonResonanceTools/MuonZmumuBinning.py")
include ("MuonResonanceTools/MuonYmumuBinning.py")
include ("MuonResonanceTools/MuonJpsimumuBinning.py")

# Create an instance of the resonance main tool using other tools, for Zmumu
from MuonResonanceTools.MuonResonanceToolsConf import MuonResonanceTool
ToolSvc += MuonResonanceTool("MuonResonanceZTool")
ToolSvc.MuonResonanceZTool.MuonResonanceSelectionTool  = ToolSvc.MuonZSelectionTool
ToolSvc.MuonResonanceZTool.MuonResonancePairingTool    = ToolSvc.ZmumuResonancePairingTool
ToolSvc.MuonResonanceZTool.Binning = MuonZBinning
ToolSvc.MuonResonanceZTool.HistoPrefix = "Zmm_"
ToolSvc.MuonResonanceZTool.DoSystematics = doSystematics
ToolSvc.MuonResonanceZTool.SysNames = Systematics
ToolSvc.MuonResonanceZTool.OutputLevel=INFO

# for Ymumu
ToolSvc += MuonResonanceTool("MuonResonanceYTool")
ToolSvc.MuonResonanceYTool.MuonResonanceSelectionTool  = ToolSvc.MuonYSelectionTool
ToolSvc.MuonResonanceYTool.MuonResonancePairingTool    = ToolSvc.YmumuResonancePairingTool
ToolSvc.MuonResonanceYTool.Binning = MuonYBinning
ToolSvc.MuonResonanceYTool.HistoPrefix = "Ymm_"
ToolSvc.MuonResonanceYTool.DoSystematics = doSystematics
ToolSvc.MuonResonanceYTool.SysNames = Systematics
ToolSvc.MuonResonanceYTool.OutputLevel=INFO

# for Jpsimumu
ToolSvc += MuonResonanceTool("MuonResonanceJpsiTool")
ToolSvc.MuonResonanceJpsiTool.MuonResonanceSelectionTool  = ToolSvc.MuonJpsiSelectionTool
ToolSvc.MuonResonanceJpsiTool.MuonResonancePairingTool    = ToolSvc.JpsimumuResonancePairingTool
ToolSvc.MuonResonanceJpsiTool.Binning = MuonJpsiBinning
ToolSvc.MuonResonanceJpsiTool.HistoPrefix = "Jpsimm_"
ToolSvc.MuonResonanceJpsiTool.DoSystematics = doSystematics
ToolSvc.MuonResonanceJpsiTool.SysNames = Systematics
ToolSvc.MuonResonanceJpsiTool.OutputLevel=INFO



# Add the tool to the reso alg
for i in range(len(Resonances)):
    if Resonances[i].find('Z')>-1: job.MuonResonanceAlg.MuonResonanceTools += [ToolSvc.MuonResonanceZTool]
    elif Resonances[i].find('Jpsi')>-1: job.MuonResonanceAlg.MuonResonanceTools += [ToolSvc.MuonResonanceJpsiTool ]
    elif Resonances[i].find('Y')>-1: job.MuonResonanceAlg.MuonResonanceTools += [ToolSvc.MuonResonanceYTool ]
job.MuonResonanceAlg.OutputLevel=INFO

# Add HistSvc
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["MUONRES DATAFILE='muonresonances.root' OPT='RECREATE'"]




from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

#--------------------------------------------------------------
# FTK algorithm inclusions
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc 
from PyJobTransforms.trfUtils import findFile
from TrigFTKSim.TrigFTKSimConf import *
from TrigFTKBankGen.TrigFTKBankGenConf import *

print "Add FTK_SGHitInput tool"
FTKSGHit = FTK_SGHitInput()
FTKSGHit.ReadTruthTracks=True
ToolSvc += FTKSGHit

FTKBankGen= FTKBankGenAlgo("FTKBankGenAlgo", OutputLevel=FATAL)
FTKBankGen.const_test_mode = False;
FTKBankGen.SaveClusterContent = False;
FTKBankGen.DiagClustering = True;
#FTKBankGen.PixelClusteringMode = 1;
FTKBankGen.DuplicateGanged = True;
FTKBankGen.GangedPatternRecognition = False;

FTKBankGen.PT_THRESHOLD = 0.8; # GeV
FTKBankGen.TRAIN_PDG = 13; # muon ID
FTKBankGen.BarrelOnly = False;
FTKBankGen.rawMode = 1;

FTKBankGen.par_c_max=0.0007;
FTKBankGen.par_c_min=-0.0007;
FTKBankGen.par_c_slices=250;

FTKBankGen.par_phi_max=1;
FTKBankGen.par_phi_min=-1;
FTKBankGen.par_phi_slices=100;

FTKBankGen.par_d0_max=2.2;
FTKBankGen.par_d0_min=-2.2;
FTKBankGen.par_d0_slices=250;

FTKBankGen.par_z0_max=120;
FTKBankGen.par_z0_min=-120;
FTKBankGen.par_z0_slices=100;

FTKBankGen.par_eta_max=3;
FTKBankGen.par_eta_min=-3;
FTKBankGen.par_eta_slices=100;

alg += FTKBankGen

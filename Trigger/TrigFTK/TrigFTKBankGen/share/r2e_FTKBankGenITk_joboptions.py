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
FTKSGHit.ReadTruthTracks  = True
FTKSGHit.OutputLevel      = INFO # DEBUG
FTKSGHit.DoOutFileRawHits = True
ToolSvc += FTKSGHit

FTKBankGen = FTKBankGenAlgo("FTKBankGenAlgo", OutputLevel=DEBUG)
FTKBankGen.pmap_path = findFile( os.environ['DATAPATH'] , "ftk_configuration/map_files/raw_ITkExample.pmap" )
FTKBankGen.rmap_path = findFile( os.environ['DATAPATH'] , "ftk_configuration/map_files/raw_ITkExample.tmap" )
FTKBankGen.IBLMode   = 0
FTKBankGen.ITkMode   = True

FTKBankGen.const_test_mode          = False;
FTKBankGen.Clustering               = False;
FTKBankGen.SaveClusterContent       = False;
FTKBankGen.DiagClustering           = True;
FTKBankGen.PixelClusteringMode      = 0;
FTKBankGen.Ibl3DRealistic           = False;
FTKBankGen.DuplicateGanged          = True;
FTKBankGen.GangedPatternRecognition = False;

FTKBankGen.PT_THRESHOLD = 4.0; # GeV
FTKBankGen.TRAIN_PDG = 13; # Traing samples particle ID  
#FTKBankGen.NEvent = event; # Event number
FTKBankGen.NBanks = 96;
FTKBankGen.BarrelOnly = True;
FTKBankGen.rawMode = 1;
FTKBankGen.UseIdentifierHash = False;

FTKBankGen.par_c_max=0.000125;
FTKBankGen.par_c_min=-0.000125;
FTKBankGen.par_c_slices=250;

FTKBankGen.par_phi_max=0.159155; # 0.5 / Pi
FTKBankGen.par_phi_min=0.095493; # 0.3 / Pi
FTKBankGen.par_phi_slices=10;

FTKBankGen.par_d0_max=2.0;
FTKBankGen.par_d0_min=-2.0;
FTKBankGen.par_d0_slices=250;

FTKBankGen.par_z0_max=150;
FTKBankGen.par_z0_min=-150;
FTKBankGen.par_z0_slices=100;

FTKBankGen.par_eta_max=0.3;
FTKBankGen.par_eta_min=0.1;
FTKBankGen.par_eta_slices=10;

alg += FTKBankGen  # 1 alg, named "MyNewPackage"

# For debugging FPE bugs
#from RecExConfig.RecFlags import rec
#rec.doFloatingPointException=True



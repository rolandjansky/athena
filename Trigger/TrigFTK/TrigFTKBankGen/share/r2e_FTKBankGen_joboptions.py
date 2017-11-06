from AthenaCommon.AlgSequence import AlgSequence
alg = AlgSequence()

#--------------------------------------------------------------
# FTK algorithm inclusions
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ToolSvc 
from TrigFTKSim.TrigFTKSimConf import *


print "Add FTK_SGHitInput tool"
FTKSGHit = FTK_SGHitInput()
FTKSGHit.ReadTruthTracks=True
ToolSvc += FTKSGHit


#FTKSGHit.OutputLevel=DEBUG
# print "Add FTKDetector tool"
# FTKDet = FTKDetectorTool()
# FTKDet.doBadModuleMap = False # True to produce or False to read an existing one
# FTKDet.useBadModuleMap = False # may disable the use of dead modules mapping
# #FTKDet.BadSSMapPath = "badSSmap.bmap"
# #FTKDet.FTK_BadModuleMapPath = "badModulemap_FTK.bmap"
# #FTKDet.ATLAS_BadModuleMapPath = "badModulemap_ATLAS.bmap"
# ToolSvc += FTKDet
# #FTKDet.OutputLevel=DEBUG

from TrigFTKBankGen.TrigFTKBankGenConf import *

#FTKBankGen= FTKBankGenAlgo("FTKBankGenAlgo", OutputLevel=DEBUG)
FTKBankGen= FTKBankGenAlgo("FTKBankGenAlgo", OutputLevel=FATAL)
#MC11 for 8L
#FTKBankGen.pmap_path = "../../../TrigFTKSim/config/map_file/raw_8Lc.pmap"
#FTKBankGen.rmap_path = "../../../TrigFTKSim/config/map_file/raw_11L.tmap"

#MC11 for 11L
#FTKBankGen.pmap_path = "../../../TrigFTKSim/config/map_file/raw_11L.pmap"
#FTKBankGen.rmap_path = "../../../TrigFTKSim/config/map_file/raw_11L.tmap"

from PyJobTransforms.trfUtils import findFile
pmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl.pmap')
print "Using PMAP:", pmap_path
rmap_path = findFile(os.environ['DATAPATH'], 'ftk_configuration/map_files/raw_12Libl.tmap')
print "Using RMAP:", rmap_path

#MC11 for IBL
FTKBankGen.pmap_path = pmap_path
FTKBankGen.rmap_path = rmap_path

#MC12 for IBL
#FTKBankGen.pmap_path = "../../../TrigFTKSim/config/map_file/raw_8LcIblEXPtoIBL.pmap"
#FTKBankGen.rmap_path = "../../../TrigFTKSim/config/map_file/raw_11L.imap2"

FTKBankGen.const_test_mode = False;
#FTKBankGen.const_test_mode = True;
#FTKBankGen.sector_dir_path = "/afs/cern.ch/work/t/tomoya/public/my_const_11L"
#FTKBankGen.gcon_dir_path = "/afs/cern.ch/work/t/tomoya/public/my_const_11L"
FTKBankGen.sector_dir_path = "/afs/cern.ch/work/t/tomoya/public/user.tompkins.tdaqtdr_base_8LcIbl123_20130805"
FTKBankGen.gcon_dir_path = "/afs/cern.ch/work/t/tomoya/public/user.tompkins.tdaqtdr_base_8LcIbl123_20130805"

FTKBankGen.Clustering = False;
FTKBankGen.SaveClusterContent = False;
FTKBankGen.DiagClustering = True;
#FTKBankGen.SctClustering = False;
FTKBankGen.PixelClusteringMode = 1;
FTKBankGen.Ibl3DRealistic = False;
FTKBankGen.DuplicateGanged = True;
FTKBankGen.GangedPatternRecognition = False;

FTKBankGen.PT_THRESHOLD = 0.8;#GeV
FTKBankGen.TRAIN_PDG = 13;#Traing samples particle ID  
#FTKBankGen.NEvent = event;#event number
FTKBankGen.NBanks = 64;
FTKBankGen.BarrelOnly = False;
FTKBankGen.rawMode = 1;
FTKBankGen.UseIdentifierHash = True;

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

alg += FTKBankGen  # 1 alg, named "MyNewPackage"

#-----------------------------------------------------------------------------
# save ROOT histograms and Tuple
#-----------------------------------------------------------------------------
#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#Stream1 = AthenaPoolOutputStream( "Stream1" )
#Stream1.OutputFile =   "SimplePoolFile_Complete.root"

#from GaudiSvc.GaudiSvcConf import THistSvc
#ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Output = [ "MyNewPackage DATAFILE='testAug21.root' OPT='RECREATE'" ]

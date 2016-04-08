# (0.) configure the THistStream
from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
if not hasattr(ServiceMgr, 'THistSvc'):
    ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output = ["SolenoidTest DATAFILE='hurz.root' OPT='RECREATE'"]

# configure the field service
#from MagFieldServices.MagFieldServicesConf import MagField__AtlasFieldSvc
#ServiceMgr += MagField__AtlasFieldSvc("AtlasFieldSvc");
ServiceMgr.AtlasFieldSvc.FullMapFile = "/afs/cern.ch/user/m/masahiro/public/BFieldMap/bfieldmap_7730_20400.root"
ServiceMgr.AtlasFieldSvc.SoleMapFile = "/afs/cern.ch/user/m/masahiro/public/BFieldMap/bfieldmap_7730_0.root"
ServiceMgr.AtlasFieldSvc.ToroMapFile = "/afs/cern.ch/user/m/masahiro/public/BFieldMap/bfieldmap_0_20400.root"
ServiceMgr.AtlasFieldSvc.UseDCS = False
ServiceMgr.AtlasFieldSvc.UseSoleCurrent = 7730
ServiceMgr.AtlasFieldSvc.UseToroCurrent = 0 # 20400

# (1.) setup the SolenoidTest algorithm
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from MagFieldUtils.MagFieldUtilsConf import MagField__SolenoidTest
testFull = 1000	# =1000 to measure the speed of the full 3d field
testFast = 1000	# =1000 to measure the speed of the fast 2d field
testOld  = 0	# =1000 to measure the speed of the old field
testHist = 100	# =100 to produce ROOT file to compare full vs. fast
if testFull:
    topSequence += MagField__SolenoidTest('SolenoidTestFull')
    topSequence.SolenoidTestFull.UseFullField = True
    topSequence.SolenoidTestFull.UseFastField = False
    topSequence.SolenoidTestFull.UseOldField = False
    topSequence.SolenoidTestFull.WriteNtuple = False
    topSequence.SolenoidTestFull.Derivatives = False
    topSequence.SolenoidTestFull.StepsR = testFull
    topSequence.SolenoidTestFull.StepsZ = testFull
    topSequence.SolenoidTestFull.StepsPhi = testFull
if testFast:
    topSequence += MagField__SolenoidTest('SolenoidTestFast')
    topSequence.SolenoidTestFast.UseFullField = False
    topSequence.SolenoidTestFast.UseFastField = True
    topSequence.SolenoidTestFull.UseOldField = False
    topSequence.SolenoidTestFast.WriteNtuple = False
    topSequence.SolenoidTestFast.Derivatives = False
    topSequence.SolenoidTestFast.StepsR = testFast
    topSequence.SolenoidTestFast.StepsZ = testFast
    topSequence.SolenoidTestFast.StepsPhi = testFast
if testOld:
    topSequence += MagField__SolenoidTest('SolenoidTestOld')
    topSequence.SolenoidTestOld.UseFullField = False
    topSequence.SolenoidTestOld.UseFastField = False
    topSequence.SolenoidTestOld.UseOldField = True
    topSequence.SolenoidTestOld.WriteNtuple = False
    topSequence.SolenoidTestOld.Derivatives = False
    topSequence.SolenoidTestOld.StepsR = testOld
    topSequence.SolenoidTestOld.StepsZ = testOld
    topSequence.SolenoidTestOld.StepsPhi = testOld
if testHist:
    topSequence += MagField__SolenoidTest('SolenoidTestHist')
    topSequence.SolenoidTestHist.UseFullField = True
    topSequence.SolenoidTestHist.UseFastField = True
    topSequence.SolenoidTestHist.UseOldField = True
    topSequence.SolenoidTestHist.WriteNtuple = True
    topSequence.SolenoidTestHist.Derivatives = True
    topSequence.SolenoidTestHist.MinimumZ = -3000.
    topSequence.SolenoidTestHist.MaximumZ =  3000.
    topSequence.SolenoidTestHist.MinimumR =     0.
    topSequence.SolenoidTestHist.MaximumR =  1200.
    topSequence.SolenoidTestHist.StepsR = testHist
    topSequence.SolenoidTestHist.StepsZ = testHist
    topSequence.SolenoidTestHist.StepsPhi = testHist


### LAr - Birks' law variation
if hasattr(ServiceMgr, "EMBPresamplerCalculator"):
    ServiceMgr.EMBPresamplerCalculator.BirksLaw=True
    ServiceMgr.EMBPresamplerCalculator.Birksk=0.0486*1.2

if hasattr(ServiceMgr, "EMBCalculator"):
    ServiceMgr.EMBCalculator.BirksLaw=True
    ServiceMgr.EMBCalculator.Birksk=0.0486*1.2

if hasattr(ServiceMgr, "EMECPosInnerWheelCalculator"):
    ServiceMgr.EMECPosInnerWheelCalculator.BirksLaw=True
    ServiceMgr.EMECPosInnerWheelCalculator.Birksk=0.0486*1.2

if hasattr(ServiceMgr, "EMECNegInnerWheelCalculator"):
    ServiceMgr.EMECNegInnerWheelCalculator.BirksLaw=True
    ServiceMgr.EMECNegInnerWheelCalculator.Birksk=0.0486*1.2

if hasattr(ServiceMgr, "EMECPosOuterWheelCalculator"):
    ServiceMgr.EMECPosOuterWheelCalculator.BirksLaw=True
    ServiceMgr.EMECPosOuterWheelCalculator.Birksk=0.0486*1.2

if hasattr(ServiceMgr, "EMECNegOuterWheelCalculator"):
    ServiceMgr.EMECNegOuterWheelCalculator.BirksLaw=True
    ServiceMgr.EMECNegOuterWheelCalculator.Birksk=0.0486*1.2

if hasattr(ServiceMgr, "EMECPresamplerCalculator"):
    ServiceMgr.EMECPresamplerCalculator.BirksLaw=True
    ServiceMgr.EMECPresamplerCalculator.Birksk=0.0486*1.2

if hasattr(ServiceMgr, "EMECPosBackOuterBarretteCalculator"):
    ServiceMgr.EMECPosBackOuterBarretteCalculator.BirksLaw=True
    ServiceMgr.EMECPosBackOuterBarretteCalculator.Birksk=0.0486*1.2

if hasattr(ServiceMgr, "EMECNegBackOuterBarretteCalculator"):
    ServiceMgr.EMECNegBackOuterBarretteCalculator.BirksLaw=True
    ServiceMgr.EMECNegBackOuterBarretteCalculator.Birksk=0.0486*1.2

### LAr Sampling Fraction Overrides
from IOVDbSvc.CondDB import conddb
#conddb.addOverride("/LAR/ElecCalibMC/fSampl","LARElecCalibMCfSampl-IOVDEP-02") this one contains these new valies starting from IOV 330000 0
conddb.addOverride("/LAR/ElecCalibMC/fSampl","LARElecCalibMCfSampl-G4106-22056-v2") # Same sampling fractions for all IoVs

### Tile - Birks' law variation
import AthenaCommon.SystemOfUnits as Units
gramsPerMeVcmSq = Units.g/(Units.MeV*Units.cm2)

if hasattr(ServiceMgr, "TileGeoG4SDCalc"):
    ServiceMgr.TileGeoG4SDCalc.birk1 = 0.0130 * gramsPerMeVcmSq * 1.54
    ServiceMgr.TileGeoG4SDCalc.birk2 = 0.0

### Tile Sampling Fraction Overrides
from IOVDbSvc.CondDB import conddb
conddb.addOverride("/TILE/OFL02/CALIB/SFR","TileOfl02CalibSfr-SIM-04") # G4 10.6-based eta-dependent sampling fractions including Birks' constant tuning starting from IOV 330000 0
#conddb.addOverride("/TILE/OFL02/CALIB/SFR","TileOfl02CalibSfr-SIM-XX") # As TileOfl02CalibSfr-SIM-04 but for all IoVs.

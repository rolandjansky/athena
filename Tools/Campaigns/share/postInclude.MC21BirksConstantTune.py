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
conddb.addOverride("/LAR/ElecCalibMC/fSampl","LARElecCalibMCfSampl-G4106-v1-FTFP_BERT_BIRK")

### Tile - Birks' law variation
import AthenaCommon.SystemOfUnits as Units
gramsPerMeVcmSq = Units.g/(Units.MeV*Units.cm2)

if hasattr(ServiceMgr, "TileGeoG4SDCalc"):
    ServiceMgr.TileGeoG4SDCalc.birk1 = 0.0130 * gramsPerMeVcmSq * 1.54
    ServiceMgr.TileGeoG4SDCalc.birk2 = 0.0

### Tile Sampling Fraction Overrides
if 'tileInfoConfigurator' in dir():
    tileInfoConfigurator.EmScaleA=33.432914

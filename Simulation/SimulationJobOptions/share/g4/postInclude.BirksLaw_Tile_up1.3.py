## postInclude.TileBirks.py
## Switch off Birks Law corrections entirely
# ServiceMgr.TileGeoG4SDCalc.DoBirk=False
import AthenaCommon.SystemOfUnits as Units
gramsPerMeVcmSq = Units.g/(Units.MeV*Units.cm2)
## exp. values from NIM 80 (1970) 239-244
ServiceMgr.TileGeoG4SDCalc.birk1 = 0.0130 * gramsPerMeVcmSq * 1.3 # 30 % up w.r.t. default value
ServiceMgr.TileGeoG4SDCalc.birk2 = 9.6e-6 * gramsPerMeVcmSq * gramsPerMeVcmSq * 0.0 # set to zero 
#EOF

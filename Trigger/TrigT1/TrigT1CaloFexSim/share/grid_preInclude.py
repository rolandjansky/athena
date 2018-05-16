from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock("geant4") 
userRunLumiOverride={"run":240020, "startmu":190.0, "endmu":210.0, "stepmu":1.0, "lb":1, "starttstamp":1410020000}
from CaloIdentifier import SUBCALO
SUBCALO.NSUBCALO=6

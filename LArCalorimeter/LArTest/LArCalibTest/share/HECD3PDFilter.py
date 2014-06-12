from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from LArCalibTest.HECNoiseD3PDFlags import HECNoiseflags
from LArCalibTest.HECNoiseD3PDMaker import HECNoiseD3PDMaker

filAlg=HECNoiseD3PDMaker("HECNoiseFilter", NtupleFileName = HECNoiseflags.FileName())
topSequence  += filAlg


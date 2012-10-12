
from RecExConfig.RecFlags import rec
from AODFix.AODFix import AODFix_Init,AODFix_addMetaData
from AODFix.AODFixFlags import aodfix

#reading RDO
print "reading RDO:"
rec.readRDO.set_Value(True)
rec.doApplyAODFix.set_Value(False)        
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.set_Value("n/a")
AODFix_Init()
# AODFix_addMetaData()

print " read fileFlags: RDO/ESD/AOD  : ",rec.readRDO(),rec.readESD(),rec.readAOD()
print " doESD/doAOD flags            : ",rec.doESD(),rec.doAOD()
print " file produced with           : ",rec.inputFileFromAthenaVersion.get_Value()
print " file was fixed with AODFix v : ",aodfix.AODFixVersion.get_Value()
print " schedule AODFix              : ",rec.doApplyAODFix()


rec.doApplyAODFix.unlock()
rec.readRDO.unlock()
rec.readRDO.set_Value(False)
rec.readESD.set_Value(True)
rec.doESD.set_Value(False)
rec.doAOD.set_Value(False)
rec.inputFileFromAthenaVersion.unlock()
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.unlock()
aodfix.AODFixVersion.set_Value("n/a")


#reading ESD, not fixed
print "reading ESD, doESD=False:"
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/maxidisk/d33/releases/16.0.2.3/def_rdotoesd_test1.ESD.pool.root"]
from RecExConfig.InputFilePeeker import inputFileSummary
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.set_Value("n/a")
AODFix_Init()
print " read fileFlags: RDO/ESD/AOD  : ",rec.readRDO(),rec.readESD(),rec.readAOD()
print " doESD/doAOD flags            : ",rec.doESD(),rec.doAOD()
print " file produced with           : ",rec.inputFileFromAthenaVersion.get_Value()
print " file was fixed with AODFix v : ",aodfix.AODFixVersion.get_Value()
print " schedule AODFix              : ",rec.doApplyAODFix()


rec.doApplyAODFix.unlock()
rec.readRDO.unlock()
rec.readRDO.set_Value(False)
rec.readESD.set_Value(True)
rec.doESD.set_Value(True)
rec.doAOD.set_Value(False)
rec.inputFileFromAthenaVersion.unlock()
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.unlock()
aodfix.AODFixVersion.set_Value("n/a")

#reading ESD, not fixed
print "reading ESD, doESD=True:"
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/maxidisk/d33/releases/16.0.2.3/def_rdotoesd_test1.ESD.pool.root"]
from RecExConfig.InputFilePeeker import inputFileSummary
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.set_Value("n/a")
AODFix_Init()
print " read fileFlags: RDO/ESD/AOD  : ",rec.readRDO(),rec.readESD(),rec.readAOD()
print " doESD/doAOD flags            : ",rec.doESD(),rec.doAOD()
print " file produced with           : ",rec.inputFileFromAthenaVersion.get_Value()
print " file was fixed with AODFix v : ",aodfix.AODFixVersion.get_Value()
print " schedule AODFix              : ",rec.doApplyAODFix()


rec.doApplyAODFix.unlock()
rec.readRDO.unlock()
rec.readRDO.set_Value(False)
rec.readESD.set_Value(False)
rec.readAOD.set_Value(True)
rec.doESD.set_Value(False)
rec.doAOD.set_Value(False)
rec.inputFileFromAthenaVersion.unlock()
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.unlock()
aodfix.AODFixVersion.set_Value("n/a")

#reading AOD, not fixed
print "reading AOD, doAOD=False:"
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/maxidisk/d33/releases/16.0.2.3/def_esdtoaod_test1.AOD.pool.root"]
from RecExConfig.InputFilePeeker import inputFileSummary
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.set_Value("n/a")
AODFix_Init()
print " read fileFlags: RDO/ESD/AOD  : ",rec.readRDO(),rec.readESD(),rec.readAOD()
print " doESD/doAOD flags            : ",rec.doESD(),rec.doAOD()
print " file produced with           : ",rec.inputFileFromAthenaVersion.get_Value()
print " file was fixed with AODFix v : ",aodfix.AODFixVersion.get_Value()
print " schedule AODFix              : ",rec.doApplyAODFix()


rec.doApplyAODFix.unlock()
rec.readRDO.unlock()
rec.readRDO.set_Value(False)
rec.readESD.set_Value(False)
rec.readAOD.set_Value(True)
rec.doESD.set_Value(False)
rec.doAOD.set_Value(True)
rec.inputFileFromAthenaVersion.unlock()
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.unlock()
aodfix.AODFixVersion.set_Value("n/a")

#reading ESD, not fixed
print "reading ESD, doESD=True:"
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/atlas/maxidisk/d33/releases/16.0.2.3/def_rdotoesd_test1.ESD.pool.root"]
from RecExConfig.InputFilePeeker import inputFileSummary
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.set_Value("n/a")
AODFix_Init()
print " read fileFlags: RDO/ESD/AOD  : ",rec.readRDO(),rec.readESD(),rec.readAOD()
print " doESD/doAOD flags            : ",rec.doESD(),rec.doAOD()
print " file produced with           : ",rec.inputFileFromAthenaVersion.get_Value()
print " file was fixed with AODFix v : ",aodfix.AODFixVersion.get_Value()
print " schedule AODFix              : ",rec.doApplyAODFix()



print "release 17.2.6.3: RecoFix during RAWtoESD and AODFix during AODto anything"
print "in R2E:"
rec.doApplyAODFix.unlock()
rec.readRDO.unlock()
rec.readRDO.set_Value(True)
rec.readESD.set_Value(False)
rec.readAOD.set_Value(False)
rec.doESD.set_Value(False)
rec.doAOD.set_Value(False)
rec.inputFileFromAthenaVersion.unlock()
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.unlock()
aodfix.AODFixVersion.set_Value("n/a")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=["/afs/cern.ch/user/s/seuster/rig/referencefiles/dataStreams_high_mu/data12_8TeV/data12_8TeV.00201280.physics_JetTauEtmiss.merge.RAW._lb0143._SFO-ALL._0001.1"]
from RecExConfig.InputFilePeeker import inputFileSummary
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.set_Value("n/a")
AODFix_Init()
print " read fileFlags: RDO/ESD/AOD  : ",rec.readRDO(),rec.readESD(),rec.readAOD()
print " doESD/doAOD flags            : ",rec.doESD(),rec.doAOD()
print " file produced with           : ",rec.inputFileFromAthenaVersion.get_Value()
print " file was fixed with AODFix v : ",aodfix.AODFixVersion.get_Value()
print " schedule AODFix              : ",rec.doApplyAODFix()

print "in A2anything:"
rec.doApplyAODFix.unlock()
rec.readRDO.unlock()
rec.readRDO.set_Value(False)
rec.readESD.set_Value(False)
rec.readAOD.set_Value(True)
rec.doESD.set_Value(False)
rec.doAOD.set_Value(False)
rec.inputFileFromAthenaVersion.unlock()
rec.inputFileFromAthenaVersion.set_Value("n/a")
aodfix.AODFixVersion.unlock()
aodfix.AODFixVersion.set_Value("n/a")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
# athenaCommonFlags.FilesInput=["/afs/cern.ch/user/s/seuster/rig/referencefiles/dataStreams_high_mu/data12_8TeV/data12_8TeV.00201280.physics_JetTauEtmiss.merge.RAW._lb0143._SFO-ALL._0001.1"]
# from RecExConfig.InputFilePeeker import inputFileSummary
rec.inputFileFromAthenaVersion.set_Value_and_Lock("AtlasProduction-17.2.6.3")
aodfix.AODFixVersion.set_Value_and_Lock("AODFix_r172r1_RAW")
AODFix_Init()
print " read fileFlags: RDO/ESD/AOD  : ",rec.readRDO(),rec.readESD(),rec.readAOD()
print " doESD/doAOD flags            : ",rec.doESD(),rec.doAOD()
print " file produced with           : ",rec.inputFileFromAthenaVersion.get_Value()
print " file was fixed with AODFix v : ",aodfix.AODFixVersion.get_Value()
print " schedule AODFix              : ",rec.doApplyAODFix()

import sys
sys.exit(0)


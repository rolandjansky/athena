# steering file for BS->ESD step


from RecExConfig.RecFlags import rec
rec.doTrigger  .set_Value_and_Lock( False )
rec.doESD      .set_Value_and_Lock( False )
rec.doAOD      .set_Value_and_Lock( False )
rec.doWriteAOD .set_Value_and_Lock( False )
rec.doWriteTAG .set_Value_and_Lock( False )
rec.doCBNT     .set_Value_and_Lock( False )
rec.doTruth    .set_Value_and_Lock( False )
rec.doPerfMon  .set_Value_and_Lock( False )

rec.readRDO    .set_Value_and_Lock( True )
rec.doWriteESD .set_Value_and_Lock( True )

BSRDOInput=["/afs/cern.ch/atlas/project/rig/data/data10_7TeV.00152166.physics_MinBias.merge.RAW._lb0206._0001.1"]

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
acf.FilesInput = BSRDOInput
acf.EvtMax = 10


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")



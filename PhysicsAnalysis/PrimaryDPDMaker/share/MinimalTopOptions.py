##=============================================================================
## Simplest job options. For test & development. ---  David Cote, August 2009
##=============================================================================

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as af
#af.FilesInput=['/afs/cern.ch/user/g/gencomm/w0/cote/data09_cos.00121512.physics_L1Calo.daq.RAW._lb0000._SFO-1._0001.data']
af.FilesInput=['/afs/cern.ch/user/k/kkoeneke/scratch0/data/AOD/mc08.106050.PythiaZee_1Lepton.recon.AOD.e347_s462_r541_tid028686/AOD.028686._19800.pool.root.1']
#af.FilesInput=['/afs/cern.ch/user/c/cote/scratch0/Pool/data08_cosmag.0087863.ESD.pool.root']

from AthenaCommon.GlobalFlags import globalflags
af.EvtMax=10

from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']

rec.doDPD   = True
rec.DPDMakerScripts.append("PrimaryDPDMaker/MinimalDPD.py")

# The job starts here!
include ("RecExCommon/RecExCommon_topOptions.py")

##Final debugging options...
#ServiceMgr.StoreGateSvc.Dump = True     # Prints the content of StoreGate (EventStore) 
#ServiceMgr.MetaDataStore.Dump = True    # Prints output metadata store content
#ServiceMgr.InputMetaDataStore.Dump=True # Prints input metadata store content



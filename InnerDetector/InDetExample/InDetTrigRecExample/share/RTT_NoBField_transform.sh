export STAGE_SVCCLASS=atlasgroupdisk
Reco_trf.py \
    inputBSFile_r2r=rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00121513/physics_IDCosmic/data09_cos.00121513.physics_IDCosmic.daq.RAW._lb0000._SFO-1._0001.data,rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00121513/physics_IDCosmic/data09_cos.00121513.physics_IDCosmic.daq.RAW._lb0000._SFO-1._0002.data,rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00121513/physics_IDCosmic/data09_cos.00121513.physics_IDCosmic.daq.RAW._lb0000._SFO-1._0003.data,rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00121513/physics_IDCosmic/data09_cos.00121513.physics_IDCosmic.daq.RAW._lb0000._SFO-1._0004.data,rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00121513/physics_IDCosmic/data09_cos.00121513.physics_IDCosmic.daq.RAW._lb0000._SFO-1._0005.data,rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00121513/physics_IDCosmic/data09_cos.00121513.physics_IDCosmic.daq.RAW._lb0000._SFO-1._0006.data,rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00121513/physics_IDCosmic/data09_cos.00121513.physics_IDCosmic.daq.RAW._lb0000._SFO-1._0007.data,rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00121513/physics_IDCosmic/data09_cos.00121513.physics_IDCosmic.daq.RAW._lb0000._SFO-1._0008.data,rfio:/castor/cern.ch/grid/atlas/DAQ/2009/00121513/physics_IDCosmic/data09_cos.00121513.physics_IDCosmic.daq.RAW._lb0000._SFO-1._0009.data \
    outputBSFile='raw.data._0001.data'     \
    outputHIST_TRIGEXPERTFile=trig-expert.root     \
    maxEvents=-1     \
    trigStream=IDCosmic \
    autoConfiguration=FieldAndGeo,BeamType,ConditionsTag   \
    preInclude=RecExCommission/RecExCommissionRepro.py,RecExCommission/MinimalCommissioningSetup.py,RecJobTransforms/TrigConfigForCosmic08RerunHLTRepro.py\
    preExec='HLTMonDoOfflineTauTTP=False; doMonitoring=False'   \
    outputESDFile=data09_cosmag.00121513.ESD.pool.root          \
    outputCBNT=myCBNT.ntuple.root     \
    --ignoreunknown

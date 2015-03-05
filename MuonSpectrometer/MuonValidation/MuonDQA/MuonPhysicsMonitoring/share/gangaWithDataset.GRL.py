## BASIC GANGA RECIPE FOR ANALYSING A SINGLE DATASET
## READ NOTES BELOW BEFOR USE
## James.Catmore@cern.ch
j = Job()
j.name="TP_B" 
j.application=Athena()
j.application.option_file=['RunMuonPhysMonAuto.py']
j.inputsandbox=['MyLBCollection.xml'] 
j.application.prepare(athena_compile=False)
j.application.atlas_dbrelease='LATEST'

j.splitter=DQ2JobSplitter()
#j.splitter.numsubjobs=100
j.splitter.numfiles=3
j.inputdata=DQ2Dataset()
j.inputdata.type = 'FILE_STAGER'
j.inputdata.dataset=[ 

    ### Period B:
    "data10_7TeV.00153565.physics_MuonswBeam.merge.AOD.r1297_p161/",
    "data10_7TeV.00153599.physics_MuonswBeam.merge.AOD.r1297_p161/",
    "data10_7TeV.00154810.physics_MuonswBeam.merge.AOD.r1297_p161/",
    "data10_7TeV.00154813.physics_MuonswBeam.merge.AOD.r1297_p161/",
    "data10_7TeV.00154815.physics_MuonswBeam.merge.AOD.r1297_p161/",
    "data10_7TeV.00154817.physics_MuonswBeam.merge.AOD.r1297_p161/",
    "data10_7TeV.00154822.physics_MuonswBeam.merge.AOD.r1299_p161/",
    "data10_7TeV.00155073.physics_MuonswBeam.merge.AOD.r1299_p161/",
    "data10_7TeV.00155112.physics_MuonswBeam.merge.AOD.r1299_p161/",
    "data10_7TeV.00155116.physics_MuonswBeam.merge.AOD.r1299_p161/",
    "data10_7TeV.00155160.physics_MuonswBeam.merge.AOD.r1299_p161/" ] 

#     ### Period C:
#     "data10_7TeV.00155228.physics_MuonswBeam.merge.AOD.f259_m486",
#     "data10_7TeV.00155280.physics_MuonswBeam.merge.AOD.f259_m486",
#     "data10_7TeV.00155569.physics_MuonswBeam.merge.AOD.f260_m491",
#     "data10_7TeV.00155634.physics_MuonswBeam.merge.AOD.f260_m496",
#     "data10_7TeV.00155669.physics_MuonswBeam.merge.AOD.f260_m496",
#     "data10_7TeV.00155678.physics_MuonswBeam.merge.AOD.f261_m496",
#     "data10_7TeV.00155697.physics_MuonswBeam.merge.AOD.f261_m496" ]


j.outputdata=DQ2OutputDataset()
j.outputdata.outputdata=['MuonPhysMon.root']
j.backend=LCG()
j.backend.requirements.cloud='DE'
j.submit()
## NOTES
## 
## 1. Replace j.application.option_file='MyJobOptions...' with the path to your job options
## 2. j.application.max_events is the number of events to process per subjob
## 3. Replace j.inputdata.dataset="mc08...." with the name of your dataset copied from AMI
## 4. Replace j.outputdata.outputdata=['MyNtuple.root'] with the name of the output ntuple set in your job options
## 5. Select a cloud to send your jobs to  - options are IT,ES,FR,DE,UK,CA,NL,TW. Note that NG (NorduGrid) and US clouds need different
## backends (Panda() for US sites and NG() for NorduGrid). In these cases you don't specify a cloud. 

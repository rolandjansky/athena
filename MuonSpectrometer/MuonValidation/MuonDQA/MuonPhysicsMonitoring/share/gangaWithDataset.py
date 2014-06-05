## BASIC GANGA RECIPE FOR ANALYSING A SINGLE DATASET
## READ NOTES BELOW BEFOR USE
## James.Catmore@cern.ch
j = Job()
j.name="TP-mc" 
j.application=Athena()
j.application.option_file=['RunMuonPhysMonAuto.py']
j.application.prepare(athena_compile=False)
j.splitter=DQ2JobSplitter()
#j.splitter.numsubjobs=100
j.splitter.numfiles=3

j.inputdata=DQ2Dataset()
j.inputdata.type = 'FILE_STAGER'
j.inputdata.dataset=[ "mc09_7TeV.108496.Pythia_directJpsimu0mu0.merge.AOD.e540_s765_s767_r1302_r1306/" ]
j.inputdata.number_of_files = 300

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

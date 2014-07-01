myDatasets[] #list of AMI datasets. 

j = Job()
j.application = Athena()
j.application.atlas_dbrelease = 'LATEST'
j.application.option_file = ['L1CaloPprFineTimeMonitoring_jobOptions.py' ]
j.name = 'FineTimingJob'
j.application.prepare()
j.inputdata = DQ2Dataset()
j.inputdata.dataset = myDatasets
j.outputdata = DQ2OutputDataset()
j.splitter = DQ2JobSplitter()
j.splitter.numsubjobs = 10
j.merger = AthenaOutputMerger()

j.application.athena_compile = False
j.backend = Panda()
j.backend.requirements = PandaRequirements()
j.backend.requirements.enableMerge = True
j.submit()

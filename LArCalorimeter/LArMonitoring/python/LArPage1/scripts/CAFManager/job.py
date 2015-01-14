# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# author: N. Berger (Nicolas.Berger@cern.ch)
#
# An example of a JobType config file, used to register a new JobType in the CAFManager


from DataClasses import JobTypeSpec
job = JobTypeSpec()

job.setName('athjob')
job.setTransform('test.py')
job.addInputs('/afs/cern.ch/user/n/nberger/scratch0/HITS*root.1', 'RAW', 1, 3)
job.setOutputDir('/afs/cern.ch/user/l/larcomm/w0/digitMon/output/AthJob')
job.addOutput('output.root', 'POOL')
job.setConfigId(0)
job.setNamingScheme('jobId')


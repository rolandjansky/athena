#!/usr/bin/env python

# Read the submission directory as a command line argument. You can
# extend the list of arguments with your private ones later on.
import optparse

#import AthenaPoolCnvSvc.ReadAthenaPool #read xAOD files 

parser = optparse.OptionParser()
parser.add_option( '-s', '--submission-dir', dest = 'submission_dir',
                   action = 'store', type = 'string', default = 'submitDir',
                   help = 'Submission directory for EventLoop' )
parser.add_option('-t', '--type', dest = 'type',
                  action = 'store', type = 'string', default = 'MC',
                  help = 'Job type. (MC, AFII, DATA)' )
parser.add_option('-d', '--daod', dest = 'daod',
                  action = 'store', type = 'int', default = '0',
                  help = 'input DAOD type. Do not specify for xAOD input' )
parser.add_option('-m', '--maxEvts', dest = 'maxEvts',
                  action = 'store', type = 'int', default = '500',
                  help = 'Max events (-1 is all)' )
( options, args ) = parser.parse_args()

# Set up (Py)ROOT.
import ROOT
ROOT.xAOD.Init().ignore()

# Set up the sample handler object. See comments from the C++ macro
# for the details about these lines.
import os
sh = ROOT.SH.SampleHandler()
sh.setMetaString( 'nc_tree', 'CollectionTree' )

cvmfsInputArea = '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/ARTInput'
inputFile = ''

if options.daod == 0:
    if options.type == "AFII":
        inputFile = os.getenv( 'ASG_TEST_FILE_MC_AFII' )
    elif options.type == "DATA":
        inputFile = os.getenv( 'ASG_TEST_FILE_DATA' )
    else:
        inputFile = os.getenv( 'ASG_TEST_FILE_MC' )
else:
    inputFile = '%s/DAOD_%sSUSY%s.art.merge.root' % (cvmfsInputArea, options.type == "DATA" and "data18" or "mc16", options.daod)
    
basePath = os.path.basename(inputFile)
sample_dir = os.path.dirname(os.path.abspath(inputFile))
mother_dir = os.path.dirname(sample_dir)       
sh_list = ROOT.SH.DiskListLocal(mother_dir)
ROOT.SH.scanDir(sh, sh_list, basePath, os.path.basename(sample_dir))
sh.printContent()

# Create an EventLoop job.
job = ROOT.EL.Job()
job.sampleHandler( sh )
job.options().setDouble( ROOT.EL.Job.optMaxEvents, options.maxEvts )

# Create the algorithm's configuration. Note that we'll be able to add
# algorithm property settings here later on.
from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
config = AnaAlgorithmConfig( 'SUSYToolsAlg' )

config.DoSyst = True
config.DataSource = 1
config.PRWLumiCalc = []
config.PRWConfigs = []


# Running over DAOD_PHYSVAL
if options.daod == 0:
    config.PRWLumiCalc = [
        "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root",
        "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20170720/physics_25ns_20.7.lumicalc.OflLumi-13TeV-009.root"
        ]

    if options.type == "AFII":
        config.DataSource = 2
        config.PRWConfigs = ["/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/mc16a_defaults_buggy.NotRecommended.prw.root"]
    elif options.type == "DATA":
        config.DataSource = 0
        config.PRWConfigs = [
            "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15ab_defaults.NotRecommended.prw.root",
            "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/PileupReweighting/mc15c_v2_defaults.NotRecommended.prw.root"
            ]
    else:
        config.PRWConfigs = ["/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/merged_prw_mc16a_latest.root"]

# Running over SUSY DAOD (uses data18/mc16e inputs)
else:
    config.PRWLumiCalc = [
        "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20180924/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-001.root"
        ]
    if options.type == "DATA":
        config.DataSource = 0
    else:
        config.UsePRWAutoconfig = True

job.algsAdd( config )
# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, options.submission_dir )

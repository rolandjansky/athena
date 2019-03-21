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
                  action = 'store', type = 'string', default = 'MCa',
                  help = 'Job type. (MCa, MCd, MCe, AFII, DATA)' )
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
    if options.type == "AFII": # Always mc16e for now
        inputFile = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.AFII.46.mc16e.PHYSVAL.pool.root'
    elif options.type == "DATA":
        inputFile = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/data18_13TeV.00348403.physics_Main.merge.AOD.artDAOD.PHYSVAL.pool.root'
    elif options.type == "MCa":
        inputFile = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.FS.46.mc16a.PHYSVAL.pool.root'
    elif options.type == "MCd":
        inputFile = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.FS.46.mc16d.PHYSVAL.pool.root'
    elif options.type == "MCe":
        inputFile = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.artDAODmc16e.PHYSVAL.pool.root'
    else:
        print ("Unknown option! Exiting")
        exit(-1)
else:
    ### This uses inputs from ART AthDerivation outputs. For now, it is always mc16e
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
config.UsePRWAutoconfig = True
if options.type != "DATA" :
    config.mcChannel = 410470

# Running over '18-like periods
if options.type == "MCe" or options.type == "AFII" or options.type == "DATA" :
    config.mcCampaign = "mc16e"
    config.PRWLumiCalc = [
        "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20181111/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-001.root"
        ]
    if options.type == "AFII":
        config.DataSource = 2
    elif options.type == "DATA":
        config.DataSource = 0

elif options.type=="MCd":
    config.mcCampaign = "mc16d"
    config.PRWLumiCalc = [
        "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root"
        ]

elif options.type=="MCa":
    config.mcCampaign = "mc16a"
    config.PRWLumiCalc = [
        "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root",
        "/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root"
        ]

job.algsAdd( config )
# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, options.submission_dir )

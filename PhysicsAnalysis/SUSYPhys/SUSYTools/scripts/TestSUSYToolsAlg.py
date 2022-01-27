#!/usr/bin/env python

# Read the submission directory as a command line argument. You can
# extend the list of arguments with your private ones later on.
import sys
import optparse
from glob import glob

#import AthenaPoolCnvSvc.ReadAthenaPool #read xAOD files

parser = optparse.OptionParser()
parser.add_option('--log-level', dest = 'log_level', default = 'INFO', choices = ['ALWAYS','FATAL','ERROR','WARNING','INFO','DEBUG','VERBOSE']) 
parser.add_option('--dosyst', dest = 'dosyst', default = False, action = 'store_true')
parser.add_option( '-s', '--submission-dir', dest = 'submission_dir', default = 'submitDir', help = 'Submission directory for EventLoop' )
parser.add_option('-t', '--type', dest = 'type', default = 'mc16e', help = 'Job type. (mc16a, mc16d, mc16e, data18)' )
parser.add_option('--AFII', dest = 'AFII', default = False, action = 'store_true' )
parser.add_option('-d', '--daod', dest = 'daod', type = 'int', default = 0, help = 'input DAOD type. Do not specify for xAOD input' )
parser.add_option('-f', '--flav', dest = 'flav', default = 'PHYSVAL', help = 'input DAOD flavour' )
parser.add_option('-m', '--maxEvts', dest = 'maxEvts', type = 'int', default = 500, help = 'Max events (-1 is all)' )
parser.add_option('-M', '--maxEvtsManual', dest = 'maxEvtsManual', type = 'int')
parser.add_option('-p', '--ptag', dest = 'ptag', default = 'p4631', help = 'ptag' )
parser.add_option('--grl', dest = 'grl')
parser.add_option('--inputDir', dest = 'inputDir')
parser.add_option('--inputFile', dest = 'inputFile')
parser.add_option('--overwrite', dest = 'overwrite', default = False, action = 'store_true' )
( options, args ) = parser.parse_args()
print("Configured input data ptag: %s"%(options.ptag))
ptageqdata = {'p4016':'p4017','p4095':'p4096','p4237':'p4238','p4441':'p4441','p4631':'p4441','p4853':'p4853'}
if 'data' in options.type and options.ptag in ptageqdata: 
   options.ptag = ptageqdata[options.ptag]
   print("Overriding ptag to equivalent data ptag: -> %s"%(options.ptag))
print("Configured input data type: %s"%(options.type))
print("Configured input data DAOD flavour: %s"%('SUSY%d'%options.daod if options.daod>0 else options.flav))
print("Configured input data sim type: %s"%('FullSim' if not options.AFII else 'AFII'))

# Set up (Py)ROOT.
import ROOT
ROOT.xAOD.Init().ignore()

# for logging
outputlvl = {'INFO':ROOT.MSG.INFO,'DEBUG':ROOT.MSG.DEBUG,'VERBOSE':ROOT.MSG.VERBOSE,'FATAL':ROOT.MSG.FATAL,'ALWAYS':ROOT.MSG.ALWAYS,'ERROR':ROOT.MSG.ERROR,'WARNING':ROOT.MSG.WARNING}

# Set up the sample handler object. See comments from the C++ macro
# for the details about these lines.
import os
sh = ROOT.SH.SampleHandler()
sh.setMetaString( 'nc_tree', 'CollectionTree' )

import shutil
if options.overwrite and os.path.exists(options.submission_dir): shutil.rmtree(options.submission_dir)

# set up file sources
cvmfsInputArea = [
'/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SUSYTools/',
'/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/ARTInput/',
]
inputFiles = {}
inputFiles['mc16e_AFII'] = 'DAOD_PHYSVAL.mc16_13TeV.410470.AFII_mc16e_%s.PHYSVAL.pool.root'%(options.ptag)
inputFiles['mc16a']      = 'DAOD_PHYSVAL.mc16_13TeV.410470.FS_mc16a_%s.PHYSVAL.pool.root'%(options.ptag)
inputFiles['mc16d']      = 'DAOD_PHYSVAL.mc16_13TeV.410470.FS_mc16d_%s.PHYSVAL.pool.root'%(options.ptag)
inputFiles['mc16e']      = 'DAOD_PHYSVAL.mc16_13TeV.410470.FS_mc16e_%s.PHYSVAL.pool.root'%(options.ptag)
inputFiles['mc20a']      = 'DAOD_mc20aPHYS.%s.art.merge.root'%(options.ptag)
inputFiles['mc20d']      = 'DAOD_mc20dPHYS.%s.art.merge.root'%(options.ptag)
inputFiles['mc20e']      = 'DAOD_mc20ePHYS.%s.art.merge.root'%(options.ptag)
inputFiles['data18']     = 'DAOD_PHYSVAL.data18_13TeV.348403.data18_%s.PHYSVAL.pool.root'%(options.ptag)
if options.flav=='PHYS':
   inputFiles['data18']  = 'DAOD_data18PHYS.%s.art.merge.root'%(options.ptag)
if options.daod == 0 and not '%s%s'%(options.type,'_AFII' if options.AFII else '') in inputFiles: sys.exit('No input file configured for type %s%s. Exiting.'%(options.type,'_AFII' if options.AFII else ''))

inputDir = ''
inputFile = ''

# configure test type
if options.daod == 0 and not options.flav=='PHYS':
    inputDir = cvmfsInputArea[0] 
    ifile = options.type + ('_AFII' if options.AFII else '')
    inputFile = inputFiles[ifile] if ifile in inputFiles else ''
else:
    inputDir = cvmfsInputArea[1]
    inputFile = 'DAOD_%s%s%s.%s.art.merge.root'%(options.type,'%s%d'%(options.flav,options.daod) if options.flav=='SUSY' else options.flav,'AFII' if options.AFII else '',options.ptag)

if options.inputDir: inputDir = options.inputDir
if options.inputFile: inputFile = options.inputFile

print("Using inputDir: ",inputDir)
print("Using inputFile:",inputFile)
ROOT.SH.ScanDir().filePattern(inputFile).scan(sh, inputDir)
sh.printContent()

# Create an EventLoop job.
job = ROOT.EL.Job()
job.sampleHandler( sh )
job.options().setDouble( ROOT.EL.Job.optMaxEvents, options.maxEvts )

# Create the algorithm's configuration. Note that we'll be able to add
# algorithm property settings here later on.
from AnaAlgorithm.AnaAlgorithmConfig import AnaAlgorithmConfig
config = AnaAlgorithmConfig( 'SUSYToolsAlg' )

config.DoSyst = options.dosyst
config.DataSource = 1
config.OutputLevel = outputlvl[options.log_level]
config.PRWLumiCalc = []
config.UsePRWAutoconfig = True
if options.type != 'data18' :
    config.mcChannel = 410470

# set datasource if AtlasFastII
if options.AFII: 
   config.DataSource = 2

# set mcCampaign
if 'mc' in options.type:
   config.mcCampaign = options.type
else:
   config.mcCampaign = 'mc16e'
   config.DataSource = 0

# set lumicalc info
PRWLumiCalc = {}
PRWLumiCalc['mc16a'] = ['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root', 
                        '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root']
PRWLumiCalc['mc16d'] = ['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root']
PRWLumiCalc['mc16e'] = ['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190318/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root']
PRWLumiCalc['mc20a'] = ['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data16_13TeV/20180129/PHYS_StandardGRL_All_Good_25ns_297730-311481_OflLumi-13TeV-009.root',
                        '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data15_13TeV/20170619/PHYS_StandardGRL_All_Good_25ns_276262-284484_OflLumi-13TeV-008.root']
PRWLumiCalc['mc20d'] = ['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data17_13TeV/20180619/physics_25ns_Triggerno17e33prim.lumicalc.OflLumi-13TeV-010.root']
PRWLumiCalc['mc20e'] = ['/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/GoodRunsLists/data18_13TeV/20190318/ilumicalc_histograms_None_348885-364292_OflLumi-13TeV-010.root']

config.PRWLumiCalc = PRWLumiCalc[config.mcCampaign]

if options.grl: config.GRLFiles = options.grl.split(',')
if options.maxEvtsManual: config.maxEvts = options.maxEvtsManual

# submit
job.algsAdd( config )
# Run the job using the direct driver.
driver = ROOT.EL.DirectDriver()
driver.submit( job, options.submission_dir )

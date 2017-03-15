# $Id: beamspotExample.py,v 1.15 2008-08-29 12:56:52 lacuesta Exp $
# Sample python script to determine beamspot.
# Written by Juerg Beringer in April 2008.

from InDetAlignExample.InDetAlignJobRunner import InDetAlignJobRunner as JobRunner
import os

# Change the following two lines for your environment: workdir is where
# you'd like your jobs to run, and mycmthome is where you keep the
# requirements (or rather setup.sh) file for the test release you
# are going to use.
try: OutputPath
except:
    workdir = os.getcwd()+'/bs'
    FirstBS = True
else:
    print "Output path get from superscript - BS"
    if FirstBS:
        workdir = OutputPath+'/FirstBs'
    else:
        workdir = OutputPath+'/LastBs'
    
mycmthome = '$HOME/athena/14.2.2X.Y.rel_3/'

#
####################################################################
# Run jobs producing AOD files
####################################################################
#
try: sGlobalTag
except:
    GlobalTag = "OFLCOND-CSC-00-00-00"
else:
    GlobalTag = sGlobalTag
    print "GlobalTag get from superscript and set to '",GlobalTag,"'"

print
print "Producing AOD files ..."
if FirstBS:
    aodrunner = JobRunner(startdirTemplate=workdir+'/%(jobname)s',
                          batchType='LSF',
                          batchQueue='atlasidali',
                          logmail='moles,sevilla',
                          #logmail='beringer,lacuesta,moles,sevilla',
                          EvtMax=-1,
                          DetDescrVersion="ATLAS-CSC-02-00-00",
                          GlobalTag=GlobalTag,
                          release='14.2.2X.Y,rel_3,AtlasTier0,opt,32,setup,runtime',
                          cmthome=mycmthome,
                          jobnameTemplate='bsaod%(jobnr)03i',
                          joboptionPath=os.getcwd()+'/InDetAlignBeamspotAOD.py',
                          doReadBS=True)
else:
    scriptTemplate = """#!/bin/sh
    source %(cmthome)s/setup.sh -tag=%(release)s
    mkdir -p %(rundir)s
    cd %(rundir)s
    pool_insertFileToCatalog %(alignmentFile)s
    athena.py %(configfile)s %(joboptionPath)s
    """
    aodrunner = JobRunner(startdirTemplate=workdir+'/%(jobname)s',
                          batchType='LSF',
                          batchQueue='atlasidali',
                          #logmail='beringer,lacuesta',
                          logmail='moles,sevilla',
                          EvtMax=-1,
                          DetDescrVersion="ATLAS-CSC-02-00-00",
                          GlobalTag=GlobalTag,
                          alignmentFile = cogpoolfile,
                          release='14.2.2X.Y,rel_3,AtlasTier0,opt,32,setup,runtime',
                          cmthome=mycmthome,
                          jobnameTemplate='bsaod%(jobnr)03i',
                          scriptTemplate = scriptTemplate,
                          joboptionPath=os.getcwd()+'/InDetAlignBeamspotAOD.py',
                          doReadBS=True)
    
        
# Preliminary - just for testing
# HOW DO WE KNOW WHAT FILES TO RUN OVER?
# dir = 'rfio:/castor/cern.ch/user/h/hawkings/calibstream/fdr1/'
dir = '/afs/cern.ch/user/a/atlidali/w0/data/FDR2_IDCalibStream/'
fileList=os.listdir(dir)
for fname in fileList:
    if fname.rfind('data')!=-1:
        aodrunner.addFiles([dir+fname])
##dir = 'rfio:/castor/cern.ch/user/h/hawkings/calibstream/fdr1/'
##aodrunner.addFiles([dir+'idcalib_5802_fdr1_0001.data'])
## aodrunner.addFiles([dir+'idcalib_5802_fdr1_0003.data'])
## aodrunner.addFiles([dir+'idcalib_5802_fdr1_0004.data'])

## dir = '/afs/cern.ch/user/a/atlidali/w0/FDR2_IDCalibStream/'
## aodrunner.addFiles([dir+'CSC.007422.singlepart_singlepi_pt10_50_Calib_1000_1.data'])
## aodrunner.addFiles([dir+'CSC.007422.singlepart_singlepi_pt10_50_Calib_1000_2.data'])
## aodrunner.addFiles([dir+'CSC.007422.singlepart_singlepi_pt10_50_Calib_1000_3.data'])
aodrunner.run()
aodrunner.wait()
outputFiles = aodrunner.getOutputFiles()
print
print "The following AOD files were produced:"
print outputFiles

#
####################################################################
# Determine beamspot using AOD files (single job over all AOD files)
####################################################################
#
print
print "Calculating beamspot on AOD files..."

finder = JobRunner(startdirTemplate=workdir,
                   outputfileTemplate='%(startdir)s/beamspot.db',
                   batchType='LSF',
                   batchQueue='atlasidali',
                   logmail='moles,sevilla',
                   #logmail='lacuesta,beringer',
                   EvtMax=-1,
                   release='14.2.2X.Y,rel_3,AtlasTier0,opt,32,setup,runtime',
                   useNLumiBlocksPerBeamspot=5,
                   maxCountPerBeamspot=-1,
                   cmthome=mycmthome,
                   jobnameTemplate='bsfind',
                   joboptionPath=os.getcwd()+'/InDetAlignBeamspotFinder.py')

finder.addFiles(outputFiles)
finder.setOption('filesPerJob', len(outputFiles))
finder.run()
finder.wait()

print
os.system('ls -l %s' % workdir)


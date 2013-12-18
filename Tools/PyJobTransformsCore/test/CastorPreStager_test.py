# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from PyJobTransformsCore.FilePreStager import theFileStagerRobot
theFileStagerRobot.setLoggerLevel(0) # all

from PyJobTransformsCore import CastorPreStager
from PyJobTransformsCore.trfutil import FileList

# use FileArg for expansion of coded filelist
fileList = FileList.expandStringToList( "rfio:/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005001.pythia_minbias/simul/v12000003/mc12.005001.pythia_minbias.simul.HITS.v12000003._[00001-4].pool.root" )
fileList += [ "/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005001.pythia_minbias/simul/v12000003/mc12.005001.pythia_minbias.simul.HITS.v12000003._00006.pool.root",
              "castor:/castor/cern.ch/atlas/csc/valiprod/sampleA/mc12/005001.pythia_minbias/simul/v12000003/mc12.005001.pythia_minbias.simul.HITS.v12000003._00007.pool.root" ]

fileList += [ "any_file_not_in_castor" ]

filesToStage = theFileStagerRobot.addFilesToStagerIfNeeded( fileList )
print "Added %s files for pre-staging" % len(filesToStage)
for f in filesToStage: print f
theFileStagerRobot.waitUntilAllFilesStaged()

#!/afs/cern.ch/sw/lcg/external/Python/2.6.5/i686-slc5-gcc43-opt/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, getReferenceRun, createFilesForRun

#look on castor for files and create joboptions
def main():
  dataFile=getReferenceRun.lastAvailableFile()
  print "The data file is %s"%dataFile
  createFilesForRun.main(dataFile)
  print "Now execute the following: \nexport STAGE_SVCCLASS=atlcal\ncp copytoshare/* ../share/\ncd ../share\nathena SCTCalib_topOptions.py"
  return 0

if __name__ == "__main__":
  sys.exit(main())
   
   

#!/usr/bin/env python

import os

HAMMERCLOUD_FILE = "mc15_13TeV.423202.Pythia8B_A14_CTEQ6L1_Jpsie3e13.merge.AOD.e3869_s2608_s2183_r6630_r6264"
PROJECTS = ["AthAnalysis", "AthDerivation", "AthSimulation", "Athena", "AnalysisBase"]

#PWD = os.environ['PWD']
#DIRNAME = "test"

TFILE_OPEN_TURLs = [
  {"ROOT":
    ("root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # LRZ-LMU
     "root://grid-dc.rzg.mpg.de:1094//pnfs/rzg.mpg.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # MPPMU
     "root://dcache-atlas-xrootd.desy.de:1094//pnfs/desy.de/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # DESY-Hamburg
     "root://lcg-se0.ifh.de:1094//pnfs/ifh.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # DESY-Zeuthen
     "root://atlasxrootd-kit.gridka.de:1094//pnfs/gridka.de/atlas/disk-only/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Karlsruhe
     "root://sedoor1.bfg.uni-freiburg.de:1094//pnfs/bfg.uni-freiburg.de/data/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Freiburg
     "root://se-goegrid.gwdg.de:1094//pnfs/gwdg.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Goettingen
     "root://grid-se.physik.uni-wuppertal.de:1094//pnfs/physik.uni-wuppertal.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Wuppertal
    )
  },
  {"DAVS":
    ("davs://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # LRZ-LMU
     "davs://grid-dav.rzg.mpg.de:2880/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", #MPPMU
     "davs://dcache-atlas-webdav.desy.de:2880/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # DESY-Hamburg
     "davs://lcg-se0.ifh.de:2880/pnfs/ifh.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # DESY-Zeuthen
     "davs://atlaswebdav-kit.gridka.de:2880/pnfs/gridka.de/atlas/disk-only/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Karlsruhe
     "davs://webdav.bfg.uni-freiburg.de:2880/pnfs/bfg.uni-freiburg.de/data/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Freiburg
     "davs://se-goegrid.gwdg.de:2880/pnfs/gwdg.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Goettingen
     "davs://grid-se.physik.uni-wuppertal.de:2881/pnfs/physik.uni-wuppertal.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Wuppertal
    )
  },
  {"HTTPS":
    ("https://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # LRZ-LMU
     "https://grid-dav.rzg.mpg.de:2880/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # MPPMU
     "https://dcache-atlas-webdav.desy.de:2880/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # DESY-Hamburg
     "https://lcg-se0.ifh.de:2880/pnfs/ifh.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # DESY-Zeuthen
     "https ://atlaswebdav-kit.gridka.de:2880/pnfs/gridka.de/atlas/disk-only/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Karlsruhe
     "https://webdav.bfg.uni-freiburg.de:2880/pnfs/bfg.uni-freiburg.de/data/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Freiburg
     "https://se-goegrid.gwdg.de:2880/pnfs/gwdg.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Goettingen
     "https://grid-se.physik.uni-wuppertal.de:2881/pnfs/physik.uni-wuppertal.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Wuppertal
    )
  },
]

#def writeTFileOpen(athena_project):
  #name = os.path.join("test", "test_DirectIO" + athena_project + "ART_TFileOpen.py")
  #print "\tGenerating ...",name
  #outfile = open(name,'w')

  ## generate frame of test scripts
  #outstring = """#!/usr/bin/env python\n
## art-description: DirectIO{project}ART TFile::Open
## art-type: grid
## art-include: {branch}/{project}\n
#import ROOT
#import os\n""".format(project=athena_project, branch='21.0' if athena_project == 'Athena' else '21.3' if athena_project == 'AthSimulation' else '21.2')
  ## loop over turl list
  #for i in xrange(len(TFILE_OPEN_TURLs)):
    ## get values of dictionaries
    #for key, value in TFILE_OPEN_TURLs[i].items():
      ## add turl to filenames string
      #filenames = ""
      #for j in xrange(len(value)):
        #filenames += value[j]
        #if j < len(value)-1: filenames += " | "
      #outstring += """
#f = ROOT.TFile.Open(\"{turl}\")
#n = f.GetName()
#if f: print(\"art-result: 0 DirectIO{project}ART_TFileOpen_protocol_{protocol}\")
#else: print(\"art-result: 1 DirectIO{project}ART_TFileOpen_protocol_{protocol}\")
#del f
#os.system(\"pool_insertFileToCatalog {{}}\".format(n))\n""".format(turl=filenames, project=athena_project, protocol=key)
      #filenames = ""
  
  ## back to main test scripts
  #outfile.write(outstring)
  #outfile.close()
  #os.system("chmod +x " + name)

#def parse_options():
  #import argparse
  #parser = argparse.ArgumentParser()
  
  #parser.add_argument("athena_project", help="Specify the Athena release flavour (Default=AthAnalysis)", type=str, nargs="*" )
  #parser.add_argument("-o", "--tfile-open", help="Generate ART scripts for TFile::Open tests", action="store_true", dest="open_tests")
  
  #options = parser.parse_args()
  #return options

def writeTFileOpen():
  name = os.path.join("test", "test_DirectIOART_TFileOpen.py")
  print "\tGenerating ...",name
  outfile = open(name,'w')

  # generate frame of test scripts
  outstring = """#!/usr/bin/env python\n
# art-description: DirectIOART TFile::Open
# art-type: grid\n
import ROOT
import os\n"""
  # loop over turl list
  for i in xrange(len(TFILE_OPEN_TURLs)):
    # get values of dictionaries
    for key, value in TFILE_OPEN_TURLs[i].items():
      # add turl to filenames string
      filenames = ""
      for j in xrange(len(value)):
        filenames += value[j]
        if j < len(value)-1: filenames += " | "
      outstring += """
f = ROOT.TFile.Open(\"{turl}\")
n = f.GetName()
if f: print(\"art-result: 0 DirectIOART_TFileOpen_protocol_{protocol}\")
else: print(\"art-result: 1 DirectIOART_TFileOpen_protocol_{protocol}\")
del f
os.system(\"pool_insertFileToCatalog {{}}\".format(n))\n""".format(turl=filenames, protocol=key)
      filenames = ""
  
  # back to main test scripts
  outfile.write(outstring)
  outfile.close()
  os.system("chmod +x " + name)

def parse_options():
  import argparse
  parser = argparse.ArgumentParser()
  
  parser.add_argument("athena_project", help="Specify the Athena release flavour (Default=AthAnalysis)", type=str, nargs="*" )
  parser.add_argument("-o", "--tfile-open", help="Generate ART scripts for TFile::Open tests", action="store_true", dest="open_tests")
  
  options = parser.parse_args()
  return options

def main():
  #TODO: Extend to AthAnalysis & AthDerivation algorithms
  
  opts = parse_options()
  
  if not os.path.exists("./test/"):
    os.makedirs("./test/")
  
  for i in xrange(len(opts.athena_project)):
    if not opts.athena_project[i] in PROJECTS:
      print "\tAthena project \"",opts.athena_project[i],"\" not intended for DirectIOART!"
      continue
    
    #if opts.open_tests:
      #writeTFileOpen(opts.athena_project[i])
    
  if opts.open_tests:
    writeTFileOpen()
    

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print "\nexiting"
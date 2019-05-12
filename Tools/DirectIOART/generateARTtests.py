#!/usr/bin/env python

import os

DAOD_FILE = "data18_13TeV:DAOD_MUON0.14316152._000001.pool.root.1"
RAW_FILE = "data15_13TeV:data15_13TeV.00284285.physics_Main.daq.RAW"
RDO_FILE = "mc16_13TeV:mc16_13TeV.361108.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.recon.RDO.e3601_s3126_r9546"
HAMMERCLOUD_FILE = "mc15_13TeV.423202.Pythia8B_A14_CTEQ6L1_Jpsie3e13.merge.AOD.e3869_s2608_s2183_r6630_r6264" #NOTE: needs update to mc16(?)
PROJECTS = ["AthAnalysis", "AthDerivation", "AthSimulation", "Athena", "AnalysisBase"]

XAODCHECKER_TURLS = {
  "ROOT"  : "root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlaslocalgroupdisk/rucio/data18_13TeV/e6/c2/DAOD_MUON0.14316152._000001.pool.root.1", # LRZ-LMU,
  "DAVS"  : "davs://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlaslocalgroupdisk/rucio/data18_13TeV/e6/c2/DAOD_MUON0.14316152._000001.pool.root.1",   # LRZ-LMU
  "HTTPS" : "https://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlaslocalgroupdisk/rucio/data18_13TeV/e6/c2/DAOD_MUON0.14316152._000001.pool.root.1",  # LRZ-LMU
}

RECO_TF_TURLS = {
  "RDO" : (
    {"ROOT"  : "root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc16_13TeV/90/96/RDO.11373415._000001.pool.root.1"}, # LRZ-LMU
    {"DAVS"  : "davs://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc16_13TeV/90/96/RDO.11373415._000001.pool.root.1"  }, # LRZ-LMU
    {"HTTPS" : "https://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc16_13TeV/90/96/RDO.11373415._000001.pool.root.1" }, # LRZ-LMU
  ),
  "RAW" : (
    {"ROOT"  : "root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/data15_13TeV/71/42/data15_13TeV.00284285.physics_Main.daq.RAW._lb0856._SFO-1._0003.data" }, # LRZ-LMU
    {"DAVS"  : "davs://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/data15_13TeV/71/42/data15_13TeV.00284285.physics_Main.daq.RAW._lb0856._SFO-1._0003.data" }, # LRZ-LMU
    {"HTTPS" : "https://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/data15_13TeV/71/42/data15_13TeV.00284285.physics_Main.daq.RAW._lb0856._SFO-1._0003.data" }, # LRZ-LMU
  ),
}

TFILE_OPEN_TURLs = [
  {"ROOT":
    (
     "root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",            # LRZ-LMU
     "root://grid-dc.rzg.mpg.de:1094//pnfs/rzg.mpg.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                       # MPPMU
     "root://dcache-atlas-xrootd.desy.de:1094//pnfs/desy.de/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                      # DESY-Hamburg
     "root://lcg-se0.ifh.de:1094//pnfs/ifh.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                                   # DESY-Zeuthen
     "root://atlasxrootd-kit.gridka.de:1094//pnfs/gridka.de/atlas/disk-only/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                # Karlsruhe
     "root://sedoor1.bfg.uni-freiburg.de:1094//pnfs/bfg.uni-freiburg.de/data/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",               # Freiburg
     "root://se-goegrid.gwdg.de:1094//pnfs/gwdg.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                              # Goettingen
     "root://grid-se.physik.uni-wuppertal.de:1094//pnfs/physik.uni-wuppertal.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Wuppertal
    )
  },
  {"DAVS":
    (
     "davs://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",             # LRZ-LMU
     "davs://grid-dav.rzg.mpg.de:2880/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                                           # MPPMU
     "davs://dcache-atlas-webdav.desy.de:2880/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                                         # DESY-Hamburg
     "davs://lcg-se0.ifh.de:2880/pnfs/ifh.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                                   # DESY-Zeuthen
     "davs://atlaswebdav-kit.gridka.de:2880/pnfs/gridka.de/atlas/disk-only/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                # Karlsruhe
     "davs://webdav.bfg.uni-freiburg.de:2880/pnfs/bfg.uni-freiburg.de/data/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                # Freiburg
     "davs://se-goegrid.gwdg.de:2880/pnfs/gwdg.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                              # Goettingen
     "davs://grid-se.physik.uni-wuppertal.de:2881/pnfs/physik.uni-wuppertal.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Wuppertal
    )
  },
  {"HTTPS":
    (
     "https://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",             # LRZ-LMU
     "https://grid-dav.rzg.mpg.de:2880/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                                           # MPPMU
     "https://dcache-atlas-webdav.desy.de:2880/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                                         # DESY-Hamburg
     "https://lcg-se0.ifh.de:2880/pnfs/ifh.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                                   # DESY-Zeuthen
     "https://atlaswebdav-kit.gridka.de:2880/pnfs/gridka.de/atlas/disk-only/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                # Karlsruhe
     "https://webdav.bfg.uni-freiburg.de:2880/pnfs/bfg.uni-freiburg.de/data/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                # Freiburg
     "https://se-goegrid.gwdg.de:2880/pnfs/gwdg.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1",                              # Goettingen
     "https://grid-se.physik.uni-wuppertal.de:2881/pnfs/physik.uni-wuppertal.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1", # Wuppertal
    )
  },
]

def write_RecoTF(opts):
  # in case of "Athena"
  if "Athena" in opts.athena_project:
    # loop over turl list of RDO input files
    for item in RECO_TF_TURLS["RDO"]:
      name = os.path.join("test", "test_directioart_athena_recotf_with_rdo_{}.sh".format(item.keys()[0].lower()))
      print("\tGenerating ... {}".format(name))
      outfile = open(name,'w')
      # generate test scripts
      outstring = """#!/bin/bash\n
# art-description: DirectIOART Athena Reco_tf.py inputFile:RDO protocol={protocol}
# art-type: grid
# art-output: *.pool.root
# art-include: 21.0/Athena
# art-include: master/Athena\n
set -e\n
Reco_tf.py --AMI q221 --inputRDOFile {turl} --outputRDO_TRIGFile art.pool.root\n
echo \"art-result: $? DirectIOART_Athena_RecoTF_inputRDO_protocol_{protocol}\"""".format(turl=item.values()[0], protocol=item.keys()[0])
      outfile.write(outstring)
      outfile.close()
      os.system("chmod +x " + name)
      # if AthenaMP
      if opts.mp:
        name = os.path.join("test", "test_directioart_athenamp_recotf_with_rdo_{}.sh".format(item.keys()[0].lower()))
        print("\tGenerating ... {}".format(name))
        outfile = open(name,'w')
        # generate test scripts
        outstring = """#!/bin/bash\n
# art-description: DirectIOART AthenaMP Reco_tf.py inputFile:RDO protocol={protocol}
# art-type: grid
# art-output: *.pool.root
# art-include: 21.0/Athena
# art-include: master/Athena\n
set -e\n
export ATHENA_PROC_NUMBER=2
Reco_tf.py --AMI q221 --inputRDOFile {turl} --outputRDO_TRIGFile art.pool.root\n
echo \"art-result: $? DirectIOART_AthenaMP_RecoTF_inputRDO_protocol_{protocol}\"""".format(turl=item.values()[0], protocol=item.keys()[0])
        outfile.write(outstring)
        outfile.close()
        os.system("chmod +x " + name)
    
    # loop over turl list if RAW input files
    for item in RECO_TF_TURLS["RAW"]:
      name = os.path.join("test", "test_directioart_athena_recotf_with_raw_{}.sh".format(item.keys()[0].lower()))
      print("\tGenerating ... {}".format(name))
      outfile = open(name,'w')
      # generate test scripts
      outstring = """#!/bin/bash\n
# art-description: DirectIOART Athena Reco_tf.py inputFile:RAW protocol={protocol}
# art-type: grid
# art-output: *.pool.root
# art-include: 21.0/Athena
# art-include: master/Athena\n
set -e\n
Reco_tf.py --AMI q431 --inputBSFile {turl} --outputESDFile art.pool.root\n
echo \"art-result: $? DirectIOART_Athena_RecoTF_inputBS_protocol_{protocol}\"""".format(turl=item.values()[0], protocol=item.keys()[0])
      outfile.write(outstring)
      outfile.close()
      os.system("chmod +x " + name)
      # if AthenaMP
      if opts.mp:
        name = os.path.join("test", "test_directioart_athenamp_recotf_with_raw_{}.sh".format(item.keys()[0].lower()))
        print("\tGenerating ... {}".format(name))
        outfile = open(name,'w')
        # generate test scripts
        outstring = """#!/bin/bash\n
# art-description: DirectIOART AthenaMP Reco_tf.py inputFile:RAW protocol={protocol}
# art-type: grid
# art-output: *.pool.root
# art-include: 21.0/Athena
# art-include: master/Athena\n
set -e\n
export ATHENA_PROC_NUMBER=2
Reco_tf.py --AMI q431 --inputBSFile {turl} --outputESDFile art.pool.root\n
echo \"art-result: $? DirectIOART_AthenaMP_RecoTF_inputBS_protocol_{protocol}\"""".format(turl=item.values()[0], protocol=item.keys()[0])
        outfile.write(outstring)
        outfile.close()
        os.system("chmod +x " + name)
  
  # in case of "AthDerivation"
  if "AthDerivation" in opts.athena_project:
    # loop over turl list
    for i in xrange(len(TFILE_OPEN_TURLs)):
      # get values of dictionaries
      for key, value in TFILE_OPEN_TURLs[i].items():
        name = os.path.join("test", "test_directioart_athderivation_recotf_with_aod_{}.sh".format(key.lower()))
        print("\tGenerating ... {}".format(name))
        outfile = open(name,'w')
        # generate test scripts
        outstring = """#!/bin/bash\n
# art-description: DirectIOART AthDerivation Reco_tf.py inputFile:RDO protocol={protocol} reductionConf:TEST1
# art-type: grid
# art-output: *.pool.root
# art-include: 21.2/AthDerivation\n
set -e\n
Reco_tf.py --inputAODFile {turl} --outputDAODFile art.pool.root --reductionConf TEST1 --maxEvents 100\n
echo \"art-result: $? DirectIOART_AthDerivation_RecoTF_inputAOD_protocol_{protocol}_TEST1\"""".format(turl=value[0], protocol=key)
        outfile.write(outstring)
        outfile.close()
        os.system("chmod +x " + name)

def write_AthAnalysis_TestAlg():
  # loop over turl list
  for i in xrange(len(TFILE_OPEN_TURLs)):
    # get values of dictionaries
    for key, value in TFILE_OPEN_TURLs[i].items():
      name = os.path.join("test", "test_directioart_athanalysis_alg_{}.sh".format(key.lower()))
      print("\tGenerating ... {}".format(name))
      outfile = open(name,'w')
      # generate tests scripts
      outstring = """#!/bin/bash\n
# art-description: DirectIOART AthAnalysis ParticleSelectionAlg protocol={protocol}
# art-type: grid
# art-output: *.pool.root
# art-include: 21.2/AthAnalysis\n
set -e\n
athena --filesInput=\"{turl}\" DirectIOART/DirectIOARTAthAnalysisJobOptions.py\n
echo \"art-result: $? DirectIOART_AthAnalysis_ParticleSelectionAlg_protocol_{protocol}\"""".format(turl=value[0], protocol=key)
      outfile.write(outstring)
      outfile.close()
      os.system("chmod +x " + name)

def write_AnalysisBase_xAODChecker():
  #loop over turl list
  for key, value in XAODCHECKER_TURLS.items():
    name = os.path.join("test", "test_directioart_analysisbase_xaodchecker_{}.sh".format(key.lower()))
    print("\tGenerating ... {}".format(name))
    outfile = open(name,'w')
    # generate test scripts
    outstring = """#!/bin/bash\n
# art-description: DirectIOART AnalysisBase xAODChecker protocol={protocol}
# art-type: grid
# art-include: 21.2/AnalysisBase\n
set -e\n
xAODChecker {turl}\n
echo \"art-result: $? DirectIOART_AnalysisBase_xAODChecker_protocol_{protocol}\"\n
PFC_FILE=PoolFileCatalog.xml
touch $PFC_FILE
echo \"<?xml version=\\"1.0\\" encoding=\\"UTF-8\\" standalone=\\"no\\" ?>\" >> $PFC_FILE
echo \"<!-- Edited By POOL -->\" >> $PFC_FILE
echo \"<!DOCTYPE POOLFILECATALOG SYSTEM \\"InMemory\\">\" >> $PFC_FILE
echo \"<POOLFILECATALOG>\" >> $PFC_FILE
echo \"\" >> $PFC_FILE
echo \"</POOLFILECATALOG>\" >> $PFC_FILE""".format(turl=value, protocol=key)
    outfile.write(outstring)
    outfile.close()
    os.system("chmod +x " + name)

def writeTFileOpen():
  name = os.path.join("test", "test_directioart_tfileopen.py")
  print("\tGenerating ... {}".format(name))
  outfile = open(name,'w')

  # generate frame of test scripts
  outstring = """#!/usr/bin/env python\n
# art-description: DirectIOART TFile::Open
# art-type: grid
# art-include: master/Athena
# art-include: master/AthSimulation
# art-include: 21.0/Athena
# art-include: 21.0/AthSimulation
# art-include: 21.2/AthAnalysis
# art-include: 21.2/AthDerivation
# art-include: 21.2/AnalysisBase
# art-include: 21.2/AnalysisTop
# art-include: 21.3/Athena
# art-include: 21.9/Athena\n
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
if f: print(\"art-result: 0 DirectIOART_TFileOpen_protocol_{protocol}\")
else: print(\"art-result: 1 DirectIOART_TFileOpen_protocol_{protocol}\")
del f\n""".format(turl=filenames, protocol=key)
      filenames = ""
  
  # back to main test scripts
  outstring += """
outstr = \"\"\"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>
<!-- Edited By POOL -->
<!DOCTYPE POOLFILECATALOG SYSTEM \"InMemory\">
<POOLFILECATALOG>

</POOLFILECATALOG>\"\"\"
with open(\"PoolFileCatalog.xml\", \"w\") as outfile:
  outfile.write(outstr)"""
  
  outfile.write(outstring)
  outfile.close()
  os.system("chmod +x " + name)

def parse_options():
  import argparse
  parser = argparse.ArgumentParser()
  
  parser.add_argument("athena_project", help="Specify the Athena release flavour", type=str, nargs="*" )
  parser.add_argument("-o", "--tfile-open", help="Generate ART scripts for TFile::Open tests", action="store_true", dest="open_tests")
  parser.add_argument("-m", "--mp", help="Generate ART tests for AthenaMP", action="store_true")
  
  options = parser.parse_args()
  return options

def main():
  #TODO: Extend to AnalysisBase test algorithms
  
  opts = parse_options()
  
  if not os.path.exists("./test/"):
    os.makedirs("./test/")
  
  for i in xrange(len(opts.athena_project)):
    if not opts.athena_project[i] in PROJECTS:
      print "\tAthena project \"",opts.athena_project[i],"\" not intended for DirectIOART!"
      continue
  
  if opts.mp and not "Athena" in opts.athena_project:
    opts.mp = False
  
  if opts.open_tests:
    writeTFileOpen()
  
  if "AthAnalysis" in opts.athena_project:
    write_AthAnalysis_TestAlg()
  
  if "Athena" in opts.athena_project or "AthDerivation" in opts.athena_project:
    write_RecoTF(opts)
  
  if "AnalysisBase" in opts.athena_project:
    write_AnalysisBase_xAODChecker()

if __name__ == "__main__":
  try:
    main()
  except KeyboardInterrupt:
    print "\nexiting"

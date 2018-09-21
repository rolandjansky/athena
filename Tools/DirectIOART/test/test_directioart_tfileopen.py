#!/usr/bin/env python

# art-description: DirectIOART TFile::Open
# art-type: grid
# art-include master/Athena
# art-include 21.0/Athena
# art-include 21.0/AthSimulation
# art-include 21.2/AthAnalysis
# art-include 21.2/AthDerivation
# art-include 21.2/AnalysisBase
# art-include 21.2/AnalysisTop
# art-include 21.3/Athena
# art-include 21.9/Athena

import ROOT
import os

f = ROOT.TFile.Open("root://lcg-lrz-rootd.grid.lrz.de:1094/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | root://grid-dc.rzg.mpg.de:1094//pnfs/rzg.mpg.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | root://dcache-atlas-xrootd.desy.de:1094//pnfs/desy.de/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | root://lcg-se0.ifh.de:1094//pnfs/ifh.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | root://atlasxrootd-kit.gridka.de:1094//pnfs/gridka.de/atlas/disk-only/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | root://sedoor1.bfg.uni-freiburg.de:1094//pnfs/bfg.uni-freiburg.de/data/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | root://se-goegrid.gwdg.de:1094//pnfs/gwdg.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | root://grid-se.physik.uni-wuppertal.de:1094//pnfs/physik.uni-wuppertal.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1")
if f: print("art-result: 0 DirectIOART_TFileOpen_protocol_ROOT")
else: print("art-result: 1 DirectIOART_TFileOpen_protocol_ROOT")
del f

f = ROOT.TFile.Open("davs://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | davs://grid-dav.rzg.mpg.de:2880/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | davs://dcache-atlas-webdav.desy.de:2880/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | davs://lcg-se0.ifh.de:2880/pnfs/ifh.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | davs://atlaswebdav-kit.gridka.de:2880/pnfs/gridka.de/atlas/disk-only/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | davs://webdav.bfg.uni-freiburg.de:2880/pnfs/bfg.uni-freiburg.de/data/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | davs://se-goegrid.gwdg.de:2880/pnfs/gwdg.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | davs://grid-se.physik.uni-wuppertal.de:2881/pnfs/physik.uni-wuppertal.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1")
if f: print("art-result: 0 DirectIOART_TFileOpen_protocol_DAVS")
else: print("art-result: 1 DirectIOART_TFileOpen_protocol_DAVS")
del f

f = ROOT.TFile.Open("https://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | https://grid-dav.rzg.mpg.de:2880/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | https://dcache-atlas-webdav.desy.de:2880/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | https://lcg-se0.ifh.de:2880/pnfs/ifh.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | https://atlaswebdav-kit.gridka.de:2880/pnfs/gridka.de/atlas/disk-only/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | https://webdav.bfg.uni-freiburg.de:2880/pnfs/bfg.uni-freiburg.de/data/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | https://se-goegrid.gwdg.de:2880/pnfs/gwdg.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 | https://grid-se.physik.uni-wuppertal.de:2881/pnfs/physik.uni-wuppertal.de/data/atlas/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1")
if f: print("art-result: 0 DirectIOART_TFileOpen_protocol_HTTPS")
else: print("art-result: 1 DirectIOART_TFileOpen_protocol_HTTPS")
del f

outstr = """<?xml version="1.0" encoding="UTF-8" standalone="no" ?>
<!-- Edited By POOL -->
<!DOCTYPE POOLFILECATALOG SYSTEM "InMemory">
<POOLFILECATALOG>

</POOLFILECATALOG>"""
with open("PoolFileCatalog.xml", "w") as outfile:
  outfile.write(outstr)
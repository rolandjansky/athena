# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

i = options['outevents']

athenaCommonFlags.EvtMax = i
athenaCommonFlags.PoolHitsOutput.set_Off()

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = i

import AthenaCommon.AtlasUnixGeneratorJob

from TruthIO.TruthIOConf import HepMCReadFromFile
read = HepMCReadFromFile()
read.InputFile = options['outfile']
topSequence += read

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("OutStream")
outStream.ItemList  = ["EventInfo#*", "McEventCollection#*"]

outfile = options['outfile']
if outfile.endswith(".ascii") :
    outfile = outfile[:-6]
outStream.OutputFile = outfile+".pool.root"

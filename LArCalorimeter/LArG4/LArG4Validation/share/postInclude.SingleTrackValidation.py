# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from LArG4Validation.LArG4ValidationConf import SingleTrackValidation
topSequence += SingleTrackValidation()
 
# The following lines are to construct our ntuple:

if "RootFileName" not in dir() :
        RootFileName = "sp.tuple.root"
 
NtupleSvc = Service( "NTupleSvc" )
NtupleSvc.Output = [ "FILE DATAFILE=\'" + RootFileName + "\' OPT='NEW'" ]
 
theApp.Dlls   +=  [  "RootHistCnv"   ]
theApp.HistogramPersistency = "ROOT"

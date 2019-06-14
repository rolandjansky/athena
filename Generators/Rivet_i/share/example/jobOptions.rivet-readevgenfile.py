# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Example job option script to run Rivet inside Athena
## using events read in from an EVGEN POOL/ROOT file.
##
## Author: James Monk <jmonk@hep.ucl.ac.uk>
## Author: Andy Buckley <andy.buckley@cern.ch>

include("GeneratorUtils/StdAnalysisSetup.py")
theApp.EvtMax = 1000

## Specify input evgen files
import glob, AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ["/afs/cern.ch/atlas/offline/ProdData/16.6.X/16.6.7.6/minbias-pythia8-7000.evgen.pool.root"]

## Now set up Rivet
from Rivet_i.Rivet_iConf import Rivet_i
topAlg += Rivet_i("Rivet")
topAlg.Rivet.Analyses = ["EXAMPLE", "MC_GENERIC"]
topAlg.Rivet.Analyses += ["ATLAS_2012_I1082936"]

## Configure ROOT file output
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output = ["Rivet DATAFILE='Rivet.root' OPT='RECREATE'"]
#svcMgr.MessageSvc.OutputLevel = ERROR

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## Example job option script to run an event generator
## and Rivet inside Athena
##
## Author: James Monk <jmonk@hep.ucl.ac.uk>
## Author: Andy Buckley <andy.buckley@cern.ch>

include("GeneratorUtils/StdEvgenSetup.py")
theApp.EvtMax = 1000

## Configure and add an event generator to the alg seq
from Pythia8_i.Pythia8_iConf import Pythia8_i
topAlg += Pythia8_i("Pythia8")
topAlg.Pythia8.CollisionEnergy = 7000.0
topAlg.Pythia8.Commands += ['HardQCD:all = on', 'PhaseSpace:pTHatMin = 30.0']

## Now set up Rivet
from Rivet_i.Rivet_iConf import Rivet_i
topAlg += Rivet_i("Rivet")
topAlg.Rivet.Analyses = ["MC_GENERIC"]
#topAlg.Rivet.Analyses += ["ATLAS_2012_I1082936"]
#topAlg.Rivet.DoRootHistos = False
#topAlg.Rivet.OutputLevel = DEBUG

## Configure ROOT file output
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output = ["Rivet DATAFILE='Rivet.root' OPT='RECREATE'"]
#svcMgr.MessageSvc.OutputLevel = ERROR

#!/usr/bin/env python

from glob import glob
from AthenaCommon.Configurable import Configurable
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
# Set up logging and new style config
Configurable.configurableRun3Behavior = True

def GetCustomAthArgs():
    from argparse import ArgumentParser
    IDPVMparser = ArgumentParser(description='Parser for IDPVM configuration')
    IDPVMparser.add_argument("--filesInput")
    IDPVMparser.add_argument("--doTightPrimary", help='also run tight-primary plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doTracksInJets", help='also run tracks in jets', action='store_true', default=False)
    IDPVMparser.add_argument("--doTracksInBJets", help='also run tracks in jets', action='store_true', default=False)
    IDPVMparser.add_argument("--doTruthOrigin", help='make plots by track origin', action='store_true', default=False)
    IDPVMparser.add_argument("--doHitLevelPlots", help='make hit residual / eff plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doPerAuthor", help='make plots by track author', action='store_true', default=False)
    IDPVMparser.add_argument("--doExpertPlots", help='run additional expert-level plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doMuonMatchedTracks", help='run plots for tracks matched to true muons', action='store_true', default=False)
    IDPVMparser.add_argument("--doElectronMatchedTracks", help='run plots for tracks matched to true electrons', action='store_true', default=False)
    IDPVMparser.add_argument("--doTruthToRecoNtuple", help='output track-to-truth ntuple', action='store_true', default=False)
    IDPVMparser.add_argument("--disableDecoration", help='disable extra track and truth decoration if possible', action='store_true', default=False)
    IDPVMparser.add_argument("--hardScatterStrategy", help='Strategy to select the hard scatter. 0 = SumPt² 1 = SumPt , 2 = Sumptw', choices=["0","1", "2"], default="0")
    IDPVMparser.add_argument("--outputFile", help='Name of output file',default="M_output.root")
    IDPVMparser.add_argument("--HSFlag", help='Hard-scatter flag - decides what is used for truth matching', choices=['HardScatter', 'All', 'PileUp'],default="HardScatter")
    IDPVMparser.add_argument("--ancestorIDList", help='List of ancestor truth IDs to match.', default = [], nargs='+', type=int)
    return IDPVMparser.parse_args()

# Parse the arguments 
MyArgs = GetCustomAthArgs()

from InDetPhysValMonitoring.InDetPhysValFlags import ConfigFlags 
ConfigFlags.IDPVM.setTruthStrategy = MyArgs.HSFlag
ConfigFlags.IDPVM.doExpertOutput   = MyArgs.doExpertPlots
ConfigFlags.IDPVM.doPhysValOutput  = not MyArgs.doExpertPlots
ConfigFlags.IDPVM.doValidateTruthToRecoNtuple = MyArgs.doTruthToRecoNtuple
ConfigFlags.IDPVM.doValidateTracksInBJets = MyArgs.doTracksInBJets
ConfigFlags.IDPVM.doValidateTracksInJets = MyArgs.doTracksInJets
ConfigFlags.IDPVM.doValidateTightPrimaryTracks = MyArgs.doTightPrimary
ConfigFlags.IDPVM.doTruthOriginPlots = MyArgs.doTruthOrigin
ConfigFlags.IDPVM.doValidateMuonMatchedTracks = MyArgs.doMuonMatchedTracks
ConfigFlags.IDPVM.doValidateElectronMatchedTracks = MyArgs.doElectronMatchedTracks
ConfigFlags.IDPVM.doPerAuthorPlots = MyArgs.doPerAuthor
ConfigFlags.IDPVM.doHitLevelPlots = MyArgs.doHitLevelPlots
ConfigFlags.IDPVM.runDecoration = not MyArgs.disableDecoration
ConfigFlags.IDPVM.ancestorIDs = MyArgs.ancestorIDList
ConfigFlags.IDPVM.hardScatterStrategy = int(MyArgs.hardScatterStrategy)

ConfigFlags.Input.Files = glob(MyArgs.filesInput)

from AthenaConfiguration.MainServicesConfig import MainServicesCfg
acc = MainServicesCfg(ConfigFlags)
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
acc.merge(PoolReadCfg(ConfigFlags))

ConfigFlags.lock()

from InDetPhysValMonitoring.InDetPhysValMonitoringConfig import InDetPhysValMonitoringCfg
acc.merge(InDetPhysValMonitoringCfg(ConfigFlags))

# finally, set up the infrastructure for writing our output
from GaudiSvc.GaudiSvcConf import THistSvc
histSvc = CompFactory.THistSvc()
histSvc.Output += ["M_output DATAFILE='"+MyArgs.outputFile+"' OPT='RECREATE'"]
acc.addService(histSvc)

acc.printConfig(withDetails=True)

# Execute and finish
sc = acc.run(maxEvents=-1)
 
# Success should be 0
import sys
sys.exit(not sc.isSuccess())



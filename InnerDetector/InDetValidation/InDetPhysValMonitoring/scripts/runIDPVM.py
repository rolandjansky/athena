#!/usr/bin/env python

from glob import glob
from AthenaConfiguration.ComponentFactory import CompFactory

def GetCustomAthArgs():
    from argparse import ArgumentParser
    IDPVMparser = ArgumentParser(description='Parser for IDPVM configuration')
    IDPVMparser.add_argument("--filesInput", required=True)
    IDPVMparser.add_argument("--doLargeD0Tracks", help='also run LRT plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doMergedLargeD0Tracks", help='also run merged STD+LRT plots', action='store_true', default=False)
    IDPVMparser.add_argument("--doLoose", help='also run loose plots', action='store_true', default=False)
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
    IDPVMparser.add_argument("--truthMinPt", help='minimum truth particle pT', type=float, default=500)
    IDPVMparser.add_argument("--outputFile", help='Name of output file',default="M_output.root")
    IDPVMparser.add_argument("--HSFlag", help='Hard-scatter flag - decides what is used for truth matching', choices=['HardScatter', 'All', 'PileUp'],default="HardScatter")
    IDPVMparser.add_argument("--ancestorIDList", help='List of ancestor truth IDs to match.', default = [], nargs='+', type=int)
    IDPVMparser.add_argument("--GRL", help='Which GRL(s) to use, if any, when running on data', choices=['2015', '2016', '2017', '2018'], nargs='+', default=[])
    return IDPVMparser.parse_args()

# Parse the arguments 
MyArgs = GetCustomAthArgs()

from PhysValMonitoring.PhysValFlags import ConfigFlags
ConfigFlags.PhysVal.IDPVM.setTruthStrategy = MyArgs.HSFlag
ConfigFlags.PhysVal.IDPVM.doExpertOutput   = MyArgs.doExpertPlots
ConfigFlags.PhysVal.IDPVM.doPhysValOutput  = not MyArgs.doExpertPlots
ConfigFlags.PhysVal.IDPVM.doValidateTruthToRecoNtuple = MyArgs.doTruthToRecoNtuple
ConfigFlags.PhysVal.IDPVM.doValidateTracksInBJets = MyArgs.doTracksInBJets
ConfigFlags.PhysVal.IDPVM.doValidateTracksInJets = MyArgs.doTracksInJets
ConfigFlags.PhysVal.IDPVM.doValidateLooseTracks = MyArgs.doLoose
ConfigFlags.PhysVal.IDPVM.doValidateTightPrimaryTracks = MyArgs.doTightPrimary
ConfigFlags.PhysVal.IDPVM.doTruthOriginPlots = MyArgs.doTruthOrigin
ConfigFlags.PhysVal.IDPVM.doValidateMuonMatchedTracks = MyArgs.doMuonMatchedTracks
ConfigFlags.PhysVal.IDPVM.doValidateElectronMatchedTracks = MyArgs.doElectronMatchedTracks
ConfigFlags.PhysVal.IDPVM.doValidateLargeD0Tracks = MyArgs.doLargeD0Tracks
ConfigFlags.PhysVal.IDPVM.doValidateMergedLargeD0Tracks = MyArgs.doMergedLargeD0Tracks
ConfigFlags.PhysVal.IDPVM.doPerAuthorPlots = MyArgs.doPerAuthor
ConfigFlags.PhysVal.IDPVM.doHitLevelPlots = MyArgs.doHitLevelPlots
ConfigFlags.PhysVal.IDPVM.runDecoration = not MyArgs.disableDecoration
ConfigFlags.PhysVal.IDPVM.ancestorIDs = MyArgs.ancestorIDList
ConfigFlags.PhysVal.IDPVM.hardScatterStrategy = int(MyArgs.hardScatterStrategy)
ConfigFlags.PhysVal.IDPVM.truthMinPt = MyArgs.truthMinPt
ConfigFlags.PhysVal.IDPVM.GRL = MyArgs.GRL

ConfigFlags.Input.Files = []
for path in MyArgs.filesInput.split(','):
    ConfigFlags.Input.Files += glob(path)

ConfigFlags.lock()

from AthenaConfiguration.MainServicesConfig import MainServicesCfg
acc = MainServicesCfg(ConfigFlags)
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
acc.merge(PoolReadCfg(ConfigFlags))

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



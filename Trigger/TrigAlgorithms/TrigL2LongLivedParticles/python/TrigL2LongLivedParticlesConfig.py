# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from TrigL2LongLivedParticles.TrigL2LongLivedParticlesConf import MuonCluster
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class MuonClusterConfig(MuonCluster):
    __slots__ = []
    def __init__ (self, name="MuonClusterConfig"):
        super(MuonClusterConfig, self).__init__(name)

        # Monitoring
        monTool = GenericMonitoringTool('MonTool',
                                        HistPath = 'MuonRoIClusterTrigger/TrigAlgMoni')

        monTool.defineHistogram('CluEta', path='EXPERT', type='TH1F', title="MuClu: Eta of the muon RoI cluster; eta; nevents", xbins=60, xmin=-3., xmax=3.)
        monTool.defineHistogram('CluPhi', path='EXPERT', type='TH1F', title="MuClu: Phi of the muon RoI cluster; phi; nevents", xbins=32, xmin=-3.1415926535897931, xmax=3.1415926535897931)
        monTool.defineHistogram('NumRoi', opt='kAddBinsDynamically', path='EXPERT', type='TH1F', title="MuClu: RoIs in the muon RoI cluster; number of RoI; nevents", xbins=14, xmin=0., xmax=14.)
        monTool.defineHistogram('RoiEta', path='EXPERT', type='TH1F', title="MuClu: Eta of all Input Muon RoIs (From Vectors); eta; nevents", xbins=60, xmin=-3., xmax=3.)
        monTool.defineHistogram('RoiPhi', path='EXPERT', type='TH1F', title="MuClu: Phi of all Input Muon RoIs (From Vectors); phi; nevents", xbins=32, xmin=-3.1415926535897931, xmax=3.1415926535897931)

        monTool.defineHistogram('CluPhi,CluEta', path='EXPERT', type='TH2F', title='MuClu: 2D Phi vs Eta of RoI clusters; phi; eta', xbins=128, xmin=-3.1415926535897931, xmax=3.1415926535897931,  ybins=120, ymin=-3., ymax=3.)

        monTool.defineHistogram('dPhiCluSeed', path='EXPERT', type='TH1F', title='MuClu: dPhi between Cluster & Seed RoI; dPhi; nClusters', xbins=64, xmin=-0.8, xmax=0.8)
        monTool.defineHistogram('dEtaCluSeed', path='EXPERT', type='TH1F', title='MuClu: dEta between Cluster & Seed RoI; dEta; nClusters', xbins=60, xmin=-1., xmax=1.)
        monTool.defineHistogram('dRCluSeed', opt='kAddBinsDynamically', path='EXPERT', type='TH1F', title='MuClu: dR between Cluster & Seed RoI; dR; nClusters', xbins=60, xmin=0, xmax=0.8)

        monTool.defineHistogram('TIME_TrigAlg', opt='kAddBinsDynamically', path='EXPERT', type='TH1F', title='MuClu: Timing Variable for entire TrigAlg Execution; time\\ (\\mu s); nTrigAlgs', xbins=150, xmin=-50., xmax=100.)
        monTool.defineHistogram('TIME_TrigAlg_Clustering', opt='kAddBinsDynamically', path='EXPERT', type='TH1F', title='MuClu: Timing Variable for Clustering Loop in TrigAlg; time\\ (\\mu s); nTrigAlgs', xbins=150, xmin=-50., xmax=100.)

        self.MonTool = monTool

        # muClu Parameters
        self.DeltaR      = 0.4

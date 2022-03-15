# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#MUON CLUSTER HYPO ALG
################ Create Instance for Online Monitoring (basic histograms)
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool_v1

class TrigMuonClusterHypoAlgMonitoring(GenericMonitoringTool_v1):
    def __init__ (self, name="muCluHypo_Online", **kwargs):
        super(TrigMuonClusterHypoAlgMonitoring, self).__init__("MonTool",**kwargs)

        self.HistPath = 'MuonRoIClusterTrigger/HypoMoni'

        self.defineHistogram('AcceptAll', path='EXPERT', type='TH1F', title="MuCluHypo: Whether acceptAll is set; bool; nevents", xbins=2, xmin=0., xmax=2.)
        self.defineHistogram('nRoIEndCap', path='EXPERT', type='TH1F', title="MuCluHypo: Minimum Number of RoIs Required in Endcap Cluster; number of RoIs; nclusters", xbins=14, xmin=0., xmax=14.)
        self.defineHistogram('nRoIBarrel', path='EXPERT', type='TH1F', title="MuCluHypo: Minimum Number of RoIs Required in Barrel Cluster; number of RoIs; nclusters", xbins=14, xmin=0., xmax=14.)
        self.defineHistogram('maxEta', path='EXPERT', type='TH1F', title="MuCluHypo: Value of maxEta parameter; maxEta; nclusters", xbins=60, xmin=-3., xmax=3.)
        self.defineHistogram('midEta', path='EXPERT', type='TH1F', title="MuCluHypo: Value of midEta parameter; midEta; nclusters", xbins=60, xmin=-3., xmax=3.)
        self.defineHistogram('isPass', path='EXPERT', type='TH1F', title="MuCluHypo: Number of MuonRoIClusters accepted by the trigger; bool; nclusters", xbins=2, xmin=0., xmax=2.)

        self.defineHistogram('TIME_HypoAlg', path='EXPERT', type='TH1F', title='MuClusHypo: Timing Variable for entire HypoAlg Execution; time (us); nAlgs', xbins=100, xmin=-20., xmax=4000.)
        self.defineHistogram('TIME_HypoAlg_DecisionLoop', path='EXPERT', type='TH1F', title='MuClusHypo: Timing Variable for HypoAlg Loop over Decisions; time (us); nAlgs', xbins=100, xmin=-20., xmax=2000.)

#MUON CLUSTER HYPO TOOL
################ Create Instance for Online Monitoring (basic histograms)
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool_v1

class TrigMuonClusterHypoToolMonitoring(GenericMonitoringTool_v1):
    def __init__ (self, name="muCluHypoTool_Online", **kwargs):
        super(TrigMuonClusterHypoToolMonitoring, self).__init__("MonTool",**kwargs)

        self.HistPath = 'MuonRoIClusterTrigger/HypoToolMoni'

        self.defineHistogram('etaClust', path='EXPERT', type='TH1F', title="MuCluHypoTool: Eta of the muon RoI cluster; eta; nclusters", xbins=60, xmin=-3., xmax=3.)
        self.defineHistogram('phiClust', path='EXPERT', type='TH1F', title="MuCluHypoTool: Phi of the muon RoI cluster; phi; nclusters", xbins=32, xmin=-3.1415926535897931, xmax=3.1415926535897931)
        self.defineHistogram('numberRoI', path='EXPERT', type='TH1F', title="MuCluHypoTool: RoIs in Passing muon RoI clusters; number of RoI; nclusters", xbins=25, xmin=0., xmax=25.)

        self.defineHistogram('nRoIBarrel,nRoIBarrelPass', path='EXPERT', type='TH2F', title="MuCluHypoTool: nRoI in All Barrel Clusters vs nRoI in all Passing Clusters; number of Barrel RoI; number of Passing Barrel RoI", xbins=25, xmin=0., xmax=25., ybins=25, ymin=0., ymax=25.)
        self.defineHistogram('nRoIEndcap,nRoIEndcapPass', path='EXPERT', type='TH2F', title="MuCluHypoTool: nRoI in All Endcap Clusters vs nRoI in all Passing Clusters; number of Endcap RoI; number of Passing Endcap RoI", xbins=25, xmin=0., xmax=25., ybins=25, ymin=0., ymax=25.)

        self.defineHistogram('result', path='EXPERT', type='TH1F', title="MuCluHypoTool: Number of MuonRoIClusters accepted by the trigger; bool; nclusters", xbins=2, xmin=0., xmax=2.)
        self.defineHistogram('phiClust,etaClust', path='EXPERT', type='TH2F', title="MuCluHypoTool: 2D Phi vs Eta of All muon RoI clusters; phi; eta", xbins=128, xmin=-3.1415926535897931, xmax=3.1415926535897931,  ybins=120, ymin=-3., ymax=3.)
        self.defineHistogram('phiClustPass,etaClustPass', path='EXPERT', type='TH2F', title="MuCluHypoTool: 2D Phi vs Eta of Passing muon RoI clusters; phi; eta", xbins=128, xmin=-3.1415926535897931, xmax=3.1415926535897931,  ybins=120, ymin=-3., ymax=3.)

        self.defineHistogram('TIME_HypoTool', path='EXPERT', type='TH1F', title='MuClusHypoTool: Timing Variable for entire HypoTool Execution; time (us); nTools', xbins=100, xmin=-20., xmax=1500.)
        self.defineHistogram('TIME_HypoTool_GetCluster', path='EXPERT', type='TH1F', title='MuClusHypoTool: Timing Variable for HypoTool to Locate MuonRoICluster Object; time (us); nTools', xbins=100, xmin=-20., xmax=1500.)
        self.defineHistogram('TIME_HypoTool_Selection', path='EXPERT', type='TH1F', title='MuClusHypoTool: Timing Variable for HypoTool to Decide Pass/Fail; time (us); nTools', xbins=100, xmin=-20., xmax=1500.)

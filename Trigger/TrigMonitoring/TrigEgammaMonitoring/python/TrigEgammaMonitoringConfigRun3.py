#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file TrigEgammaMonitoringConfigRun3.py
@author D. Maximov
@date 2019-07-08
@brief Run 3 configuration builder. Histograms definitions taken from TrigEgammaPlotTool
'''
from AthenaCommon.Logging import logging
log_trigeg = logging.getLogger( 'TrigEgammaMonitoringConfigRun3' )

from TrigEgammaMonitoring.TrigEgammaMonitoringConfig import TrigEgammaMonToolBuilder

from TrigEgammaMonitoring.TrigEgammaMonitoringHistDefs import TH1F, TProfile, TH2F, TProfile2D

class TrigEgammaMonToolBuilderRun3(TrigEgammaMonToolBuilder):
    _parent_attrs = [
        'pp_mode',
        'pPb_mode',
        'HI_mode',
        'cosmic_mode',
        'mc_mode',
        'data_type',
        'basePath',
        'debugLevel',
        'detailLevel',
        'emulation',
        'derivation',
        'tagItems',
        'JpsitagItems',
        'electronList',
        'photonList',
        'tpList',
        'jpsiList',
        'mam',
    ]

    # Labels
    _label_trigstep = [
        "L2CaloCont",
        "L2Calo",
        "L2Cont",
        "L2",
        "L2TrackCont",
        "EFCaloCont",
        "EFCalo",
        "EFTrackCont",
        "EFTrack",
        "HLTCont",
        "HLT",
    ]

    _label_hltobj = [
        "ClusterCont",
        "Cluster",
        "ClusterMatch",
        "TrackCont",
        "Track",
        "TrackMatch",
        "PhotonCont",
        "Photon",
        "PhotonMatch",
        "ElectronCont",
        "Electron",
        "ElectronMatch",
    ]

    _label_hltte = [
        "L1Calo",
        "L2Calo",
        "L2",
        "EFCalo",
        "EFTrack",
        "HLT",
    ]

    _label_failisem = [
        "ClusterEtaRange", "ConversionMatch",
        "ClusterHadronicLeakage", "ClusterMiddleEnergy",
        "ClusterMiddleEratio37","ClusterMiddleEratio33",
        "ClusterMiddleWidth",
        "f3","ClusterStripsEratio","ClusterStripsDeltaEmax2",
        "ClusterStripsDeltaE","ClusterStripsWtot",
        "ClusterStripsFracm","ClusterStripsWeta1c",
        "empty14","ClusterStripsDEmaxs1",
        "TrackBlayer","TrackPixel","TrackSi","TrackA0","TrackMatchEta",
        "TrackMatchPhi","TrackMatchEoverP",
        "TrackTRTeProbabilityHT_Electron",
        "TrackTRThits","TrackTRTratio","TrackTRTratio90","TrackA0Tight",
        "TrackMatchEtaTight","Isolation","ClusterIsolation",
        "TrackIsolation",
        "No Track","No Cluster","No Object","Total",
    ]

    _label_failisemlh = [
        "Kinematic","NSi","NPix","NBlayer","Conversion","LH","A0",
        "dEta","dPhiRes","WstotHighET","EoverPHighEt",
        "No Track","No Cluster","No Object","Total"
    ]

    _label_ineffisem = [
        "L2Calo", "L2", "EFCalo",
        # "EFTrack",
        "Shower shape", "TrkClus", "Track", "TRT", "Track & TRT",
        "TrkClus & Trk & TRT", "IsEM", "Isolation", "isEMLH and Iso",
        "Track & Cluster", "No cluster", "No track", "No object",
        "Some object", "Unknown w/o electron", "Unknown w/ electron",
        "Sum", "Total"
    ]

    _label_ineffisemlh = [
        "L2Calo", "L2", "EFCalo",
        # "EFTrack",
        "Kinematic", "NSi", "NPix", "NBlayer", "Conversion", "LH", "A0",
        "dEta", "dPhiRes", "WstotHighET", "EoverPHighEt", "isEMLH",
        "Isolation", "isEMLH & Iso", "Track & Cluster", "No cluster",
        "No track", "No object", "Some object", "Unknown w/o electron",
        "Unknown w/ electron", "Sum", "Total"
    ]

    _label_isem_trk = [
        "TrackBlayer_Electron","TrackPixel_Electron",
        "TrackSi_Electron","TrackA0Tight_Electron","Total"
    ]
    _label_isem_trkclus = ["#eta match","#phi match",
        "E/p match","TrackMatchEtaTight_Electron","Total"
    ]
    _label_isem_trt = [
        "TrackTRTeProbabilityHT_Electron","TrackTRThits_Electron",
        "TrackTRTratio_Electron","TrackTRTratio90_Electron",
        "TrackA0Tight_Electron","Total"
    ]

    def __init__(self,**kwargs):
        TrigEgammaMonToolBuilder.__init__(self, **kwargs)
        log_trigeg.info('TrigEgammaMonToolBuilderRun3.__init__')

    def config(self):
        log_trigeg.info('TrigEgammaMonToolBuilderRun3.config')
        self._get_monitoring_mode_success = self.get_monitoring_mode()
        #print self._get_monitoring_mode_success
        if self._get_monitoring_mode_success == False:
            log_trigeg.warning("HLTMonTriggerList: Error getting monitoring mode, default monitoring lists will be used.")
        else:
            log_trigeg.info("Configuring for %s", self.data_type)
        self.setProperties()

        for a in self._parent_attrs:
            log_trigeg.info('{}: {}'.format(a, getattr(self,a)))

        #self.configureTools()
        log_trigeg.info('//// end of TrigEgammaMonToolBuilderRun3.config')

    def _setBinning(self, doJpsiee=False):
        """ Method to define all binning required that is variable"""

        # Binning as defined in TP framework
        coarse_eta_bins = [-2.47,-1.52,-1.37,-0.60,0.00,0.60,1.37,1.52,2.47] # 9 items

        coarse_et_bins = [4.,20.,30.,40.,50.,60.,150.] # 7 items

        default_eta_bins = [-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.80,-0.60,-0.10,0.00,
            0.10, 0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47] # 21 items

        default_et_bins = [4.,7.,10.,15.,20.,25.,30.,35.,40.,45.,50.,60.,80.,150.] # 14 items

        etabins = [-2.47,-2.37,-2.01,-1.81,-1.52,-1.37,-1.15,-0.8,-0.6,-0.1,
            0.0,0.1,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47] # 21 items

        # TH2 with variable bin x-Axis, but constant bin y-Axis takes only Double_t arrays
        etbins_Zee = [0.,2.,4.,6.,8.,10.,
            12.,14.,16.,18.,20.,22.,24.,26.,28.,
            30.,32.,34.,36.,38.,40.,42.,44.,46.,48.,50.,55.,60.,65.,70.,100.] # 31 items

        etbins_Jpsiee = [ 0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5, 4, 4.5,
            5, 5.5, 6, 6.5, 7, 7.5, 8, 8.5, 9, 9.5,
            10,10.5,11,11.5,12,12.5,13,13.5,14,14.5,
            15,15.5,16,16.5,17,17.5,18,18.5,19,19.5,
            20,20.5,21,21.5,22,22.5,23,23.5,24,24.5,
            25,25.5] # 52 items


        if doJpsiee:
            self._nEtbins=51;
            self._etbins = etbins_Jpsiee[0:self._nEtbins+1]
        else:
            self._nEtbins=30;
            self._etbins = etbins_Zee[0:self._nEtbins+1]

        # Define the binning
        self._nEtabins=20;
        self._ndefaultEtbins=13;
        self._ndefaultEtabins=20;
        self._ncoarseEtbins=6;
        self._ncoarseEtabins=8;

        #Fill the arrays
        self._etabins = etabins[0:self._nEtabins+1]
        self._defaultEtbins = default_et_bins[0:self._ndefaultEtbins+1]
        self._defaultEtabins = default_eta_bins[0:self._ndefaultEtabins+1]
        self._coarseEtbins = coarse_et_bins[0:self._ncoarseEtbins+1]
        self._coarseEtabins = coarse_eta_bins[0:self._ncoarseEtabins+1]


    def bookHistograms(self, histogramsRoot, helper):
        self._setBinning()

        for triggerChain in self.tagItems:
            self.defineExpertHistograms(histogramsRoot, helper, 'ExpertTrigEgammaMonAlg', triggerChain)

        for triggerChain in self.electronList:
            self.defineExpertHistograms(histogramsRoot, helper, 'ExpertTrigEgammaMonAlg', triggerChain)

        for triggerChain in self.photonList:
            self.defineExpertHistograms(histogramsRoot, helper, 'ExpertTrigEgammaMonAlg', triggerChain)

    def defineExpertHistograms(self, rootpath, helper, algnameprefix, triggerChain):

        ### STEP 2 ###
        # Adding an algorithm to the helper. Here, we will use the example
        # algorithm in the AthenaMonitoring package. Just pass the type to the
        # helper. Then, the helper will instantiate an instance and set up the
        # base class configuration following the inputFlags. The returned object
        # is the algorithm.
        #The added algorithm must exist as a .h file
        from TrigEgammaMonitoring.TrigEgammaMonitoringConf import TrigEgammaMonitorAlgorithm
        trigEgammaMonAlg = helper.addAlgorithm(TrigEgammaMonitorAlgorithm, algnameprefix+'__'+triggerChain)

        ### STEP 3 ###
        # Edit properties of a algorithm
        # some generic property
        # trigEgammaMonAlg.RandomHist = True
        # to enable a trigger filter, for example:
        trigEgammaMonAlg.TriggerChain = triggerChain
        # trigEgammaMonAlg.OutputLevel = 2 # DEBUG


        # # Then, add a tool that doesn't have its own configuration function. In
        # # this example, no accumulator is returned, so no merge is necessary.
        # from MyDomainPackage.MyDomainPackageConf import MyDomainTool
        # trigEgammaMonAlg.MyDomainTool = MyDomainTool()

        # Add a generic monitoring tool (a "group" in old language). The returned
        # object here is the standard GenericMonitoringTool.

        self.bookExpertHistos(trigEgammaMonAlg, rootpath+'/Expert/'+triggerChain, helper, triggerChain)

        trigMonGroup = helper.addGroup(
            trigEgammaMonAlg,
            'TrigEgammaMonitor',
            rootpath+'/Expert/'+triggerChain
        )

        ### STEP 5 ###
        # Configure histograms
        #NB! The histograms defined here must match the ones in the cxx file exactly

        trigMonGroup.defineHistogram('lb', title='Luminosity Block;lb;Events',
                                path='TestEgammaMonitor',xbins=1000,xmin=-0.5,xmax=999.5)

    def isL1trigger(self, trigStream):
        return trigStream.startswith('L1')

    def triggerType(self, trigStream):
        """Reproduce TrigEgammaAnalysisBaseTool::parseTriggerName behaviour"""

        if self.isL1trigger(trigStream):
            return 'electron'
        if trigStream.startswith('HLT_e'):
            return 'electron'
        if trigStream.startswith('HLT_g'):
            return 'photon'

        return None

    def triggerPid(self, trigStream):
        """trigStream looks like this 'HLT_e5_lhloose'"""
        return trigStream.split('_')[2]

    def createMonGroup(self, monAlg, helper, basePath, subCats, monnameprefix='TrigEgammaMonitor'):

        grpPath = '/'.join( (basePath,) + subCats )
        grpName = '_'.join( (monnameprefix,) + subCats )

        return helper.addGroup(monAlg, grpName, grpPath )

    def bookExpertHistos(self, monAlg, basePath, helper, triggerStream, monnameprefix='TrigEgammaMonitor'):
        self.bookL1Histos(monAlg, basePath, helper, triggerStream, monnameprefix)

        algnames = ["Efficiency"]
        if self.emulation:
            algnames.append("Emulation")

        # Loop over Efficiency (and or) Emulation
        for algname in algnames:
            _ehltMonGroup = self.createMonGroup(monAlg, helper, basePath, (algname, 'HLT') )

            self.bookEfficiencyHistos(_ehltMonGroup)
            self.bookEfficiency2DHistos(_ehltMonGroup)

            for subgroups in [ (algname,'L2Calo'), (algname, 'L2'), (algname,'EFCalo') ]:
                _subgrpMonGroup = self.createMonGroup(monAlg, helper, basePath, subgroups )
                self.bookEfficiencyHistos(_subgrpMonGroup)
                if self.detailLevel:
                    self.bookEfficiency2DHistos(_subgrpMonGroup)

            if self.triggerType(triggerStream) == 'electron':
                _ehltMonGroup.defineHistogram('lb', title='Luminosity Block;lb;Events',
                                path='TestEgammaMonitor',xbins=1000,xmin=-0.5,xmax=999.5)

                # Temporary stub of two monitored variables
                _ehltMonGroup.defineHistogram("eff_triggerstep,eff_triggerstep", type='TProfile', path='/', title="eff_triggerstep",
                                xbins=11, xmin=0, xmax=11, labels=self._label_trigstep)

                # Temporary stub of two monitored variables
                _ehltMonGroup.defineHistogram("eff_hltreco,eff_hltreco", type='TProfile', path='/', title="eff_hltreco",
                                xbins=12, xmin=0, xmax=12, labels=self._label_hltobj)


                pid = self.triggerPid(triggerStream)
                fail = "FailisEM" + pid
                ineffisEM = "IneffisEM" + pid

                if "LH" in pid:
                    self.addHistogram(_ehltMonGroup, TH1F(fail, fail, 15, 0, 15, labels=self._label_failisemlh))
                    self.addHistogram(_ehltMonGroup, TProfile(ineffisEM, ineffisEM, 26, 0, 26, labels=self._label_ineffisemlh))
                else:
                    self.addHistogram(_ehltMonGroup, TH1F(fail, fail, 36, 0, 36, labels=self._label_ineffisem))
                    self.addHistogram(_ehltMonGroup, TProfile(ineffisEM, ineffisEM, 22, 0, 22, labels=self._label_failisem))

                    ineffTrk = ineffisEM + "Trk"
                    ineffTrkClus = ineffisEM + "TrkClus"
                    ineffTRT = ineffisEM + "TRT"
                    self.addHistogram(_ehltMonGroup, TProfile(ineffTrk, ineffTrk, 6, 0, 6, labels=self._label_isem_trk))
                    self.addHistogram(_ehltMonGroup, TProfile(ineffTrkClus, ineffTrkClus, 5, 0, 5, labels=self._label_isem_trkclus))
                    self.addHistogram(_ehltMonGroup, TProfile(ineffTRT, ineffTRT, 5, 0, 5, labels=self._label_isem_trt))

            # end of trigInfo.trigType=="electron"

            if self.detailLevel:
                effdirs = [
                    ( algname, 'HLT', 'Loose'      ),
                    ( algname, 'HLT', 'Medium'     ),
                    ( algname, 'HLT', 'Tight'      ),
                    ( algname, 'HLT', 'LHLoose'    ),
                    ( algname, 'HLT', 'LHMedium'   ),
                    ( algname, 'HLT', 'LHTight'    ),
                    ( algname, 'HLT', 'LooseIso'   ),
                    ( algname, 'HLT', 'MediumIso'  ),
                    ( algname, 'HLT', 'TightIso'   ),
                    ( algname, 'HLT', 'LHLooseIso' ),
                    ( algname, 'HLT', 'LHMediumIso'),
                    ( algname, 'HLT', 'LHTightIso' ),
                ]
                for effdir in effdirs:
                    _effdirMonGroup = self.createMonGroup(monAlg, helper, basePath, effdir )
                    self.bookEfficiencyHistos(_effdirMonGroup)
                    self.bookEfficiency2DHistos(_effdirMonGroup)

            # end detailerhistos

        # end of loop over Efficiency (and or) Emulation

        # Distributions histograms
        dirnames = [
            self.createMonGroup(monAlg, helper, basePath, ('Distributions','Offline') ),
            self.createMonGroup(monAlg, helper, basePath, ('Distributions', 'HLT') ),
        ]
        for subgrp in dirnames:
            self.bookEgammaDistributionHistos(subgrp)
            if self.triggerType(triggerStream) == 'electron':
                self.bookElectronDistributionHistos(subgrp)
            self.addHistogram(subgrp, TH1F("rejection","N_{TE}; #Step",6,0,6, labels=self._label_hltte))


        _efcaloMonGroup = self.createMonGroup(monAlg, helper, basePath, ('Distributions', 'EFCalo') )
        dirnames.append(_efcaloMonGroup)
        self.addHistogram(_efcaloMonGroup, TH1F("energyBE0", "Cluster Energy BE0; E [GeV] ; Count", 50, 0., 100.))
        self.addHistogram(_efcaloMonGroup, TH1F("energyBE1", "Cluster Energy BE1; E [GeV] ; Count", 50, 0., 100.))
        self.addHistogram(_efcaloMonGroup, TH1F("energyBE2", "Cluster Energy BE2; E [GeV] ; Count", 50, 0., 100.))
        self.addHistogram(_efcaloMonGroup, TH1F("energyBE3", "Cluster Energy BE3; E [GeV] ; Count", 50, 0., 100.))
        self.addHistogram(_efcaloMonGroup, TH1F("energy", "Cluster Energy; E [GeV] ; Count", 50, 0., 100.))
        self.addHistogram(_efcaloMonGroup, TH1F("eta_calo", "eta_calo; eta_calo ; Count", 50, -2.47, 2.47))
        self.addHistogram(_efcaloMonGroup, TH1F("phi_calo", "phi_calo; phi_calo ; Count", 50, -3.14, 3.14))

        if self.triggerType(triggerStream) == 'photon':
            _l2phMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Distributions', 'L2Photon') )
            self.bookDistributionHistos(_l2phMonGroup)

        if self.triggerType(triggerStream) == 'electron':
            _l2elMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Distributions', 'L2Electron') )
            self.bookDistributionHistos(_l2elMonGroup)
            self.addHistogram(_l2elMonGroup, TH1F("trkClusDeta", "Trk Clus Deta; deta ; Count", 50, -0.5, 0.5))
            self.addHistogram(_l2elMonGroup, TH1F("trkClusDphi", "Trk Clus Dphi; dphi ; Count", 50, -0.5, 0.5))

        _l2caloMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Distributions', 'L2Calo') )
        dirnames.append(_l2caloMonGroup)

        #Book the kinematic plots for each trigger level
        for dirgrp in dirnames:
            self.bookDistributionHistos(dirgrp)

        # Resolution
        _hltresMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Resolutions', 'HLT') )
        self.bookResolutionHistos(_hltresMonGroup)
        if self.triggerType(triggerStream) == 'electron':
            self.bookElectronResolutionHistos(_hltresMonGroup)
            if ("iloose"    in triggerStream) or ("ivarloose" in triggerStream):
                self.bookElectronIsoResolutionHistos(_hltresMonGroup)

        if self.triggerType(triggerStream) == 'photon':
            self.bookPhotonResolutionHistos(_hltresMonGroup)
            if ("icaloloose"  in triggerStream) or ("icalovloose" in triggerStream) or ("icalotight"  in triggerStream):
                self.bookPhotonIsoResolutionHistos(_hltresMonGroup)

        self.bookExpertResolutionHistos(_hltresMonGroup)

        if self.detailLevel:
            _hltabsresMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'AbsResolutions', 'HLT') )
            self.bookAbsResolutionHistos(_hltabsresMonGroup)

        if self.detailLevel:
            _l2caloresMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Resolutions', 'L2Calo') )
            self.bookResolutionHistos(_l2caloresMonGroup)
            self.bookExpertL2CaloResolutionHistos(_l2caloresMonGroup)

        _l2calohltresMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Resolutions', 'L2Calo_vs_HLT') )
        self.bookResolutionHistos(_l2calohltresMonGroup)
        if self.detailLevel:
            self.bookExpertL2CaloResolutionHistos(_l2calohltresMonGroup)
    # end of bookExpertHistos

    def bookL1Histos(self, monAlg, basePath, helper, triggerStream, monnameprefix='TrigEgammaMonitor'):
        _l1caloeffMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Efficiency', 'L1Calo') )
        self.bookEfficiencyHistos(_l1caloeffMonGroup)
        self.bookEfficiency2DHistos(_l1caloeffMonGroup)

        if self.emulation:
            _l1caloemuMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Emulation', 'L1Calo') )
            self.bookEfficiencyHistos(_l1caloemuMonGroup)
            self.bookEfficiency2DHistos(_l1caloemuMonGroup)

        _l1calodistMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Distributions', 'L1Calo') )
        self.addHistogram(_l1calodistMonGroup, TH1F("energy", "Cluster Energy; E [GeV] ; Count", 100, 0., 200.))
        self.addHistogram(_l1calodistMonGroup, TH1F("roi_et", "RoI word Cluster Energy; E [GeV] ; Count", 100, 0., 200.))
        self.addHistogram(_l1calodistMonGroup, TH1F("emIso", "EM Isolation; E [GeV] ; Count", 50, -1., 20.))
        self.addHistogram(_l1calodistMonGroup, TH1F("hadCore", "Hadronic Isolation; E [GeV] ; Count", 50, -1., 20.))
        self.addHistogram(_l1calodistMonGroup, TH1F("eta", "eta; eta ; Count", 50, -2.5, 2.5))
        self.addHistogram(_l1calodistMonGroup, TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2))
        self.addHistogram(_l1calodistMonGroup, TH2F("emClusVsEta", "L1 Cluster Energy vs L1 #eta; #eta; E [GeV]; Count",
                    51, -2.55, 2.55,
                    60, 0, 60))
        self.addHistogram(_l1calodistMonGroup, TH2F("emClusVsEmIsol", "L1 Cluster Energy vs emIsol; emIsol [GeV]; E [GeV]; Count",
                    20, -0.1, 9.9,
                    60, 0, 60))
        self.addHistogram(_l1calodistMonGroup, TH2F("emClusVsHadCore", "L1 Cluster Energy vs hadCore; hadCore [GeV]; E [GeV]; Count",
                    10, -0.1, 4.9,
                    60, 0, 60))

        _roidistMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Distributions', 'RoI') )
        self.addHistogram(_roidistMonGroup, TH1F("roi_eta", "RoI #eta; #eta ; Count", 51, -2.55, 2.55))
        self.addHistogram(_roidistMonGroup, TH1F("roi_phi", "RoI #phi; #phi ; Count", 20, -3.2, 3.2))

        _l1caloresMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'Resolutions', 'L1Calo') )
        self.addHistogram(_l1caloresMonGroup, TH2F("res_etVsEta", "L1 cluster Energy resolution as function of L1 #eta; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                    50, -2.55, 2.55,
                    200, -1., 1.))

        _l1caloabsresMonGroup = self.createMonGroup(monAlg, helper, basePath, ( 'AbsResolutions', 'L1Calo') )
        self.addHistogram(_l1caloabsresMonGroup, TH2F("res_etVsEta", "L1 cluster Energy resolution as function of L1 #eta; #eta; E_{T}(on)-E_{T}(off); Count",
                    51, -2.55, 2.55,
                    200, -100., 100.))
    # end of bookL1Histos


    def addHistogram(self, monGroup, histDef):
        monGroup.defineHistogram(histDef.name, **histDef.kwargs)

    def bookAbsResolutionHistos(self, monGroup):
        self.addHistogram(monGroup, TH1F("res_pt", "HLT p_{T} resolution; (p_{T}(on)-p_{T}(off)) ; Count", 200, -1.5, 1.5))
        self.addHistogram(monGroup, TH1F("res_et", "HLT E_{T} resolution; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.5, 0.5))
        self.addHistogram(monGroup, TH1F("res_eta", "#eta resolution; (#eta(on)-#eta(off)) ; Count", 40, -0.001, 0.001))
        self.addHistogram(monGroup, TH1F("res_phi", "#phi resolution; (#phi(on)-#phi(off)) ; Count", 40, -0.001, 0.001))

        self.addHistogram(monGroup, TH2F("res_etVsEta", "HLT E_{T} resolution as function of #eta; #eta; (E_{T}(on)-E_{T}(off)); Count",
                    50, -2.47, 2.47,
                    200, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_etVsEt", "HLT E_{T} resolution as function of E_{T}; E_{T} [GeV]; (E_{T}(on)-E_{T}(off)); Count",
                    50, 0., 100.,
                    200, -0.1, 0.1))

        self.addHistogram(monGroup, TH2F("res_ptcone20_relVsEta", "HLT ptcone20/pt resolution as function of #eta; #eta; on-off; Count",
                    50, -2.47, 2.47,
                    200, -0.2, 0.2))
        self.addHistogram(monGroup, TH2F("res_ptcone20_relVsEt", "HLT ptcone20/pt resolution as function of E_{T}; E_{T} [GeV]; on-off; Count",
                    50, 0., 100.,
                    200, -0.2, 0.2))
        self.addHistogram(monGroup, TH2F("res_ptcone20VsMu", "HLT ptcone20 resolution as function of avg #mu; #mu; on-off; Count",
                    50, 0, 100,
                    200, -20, 20))
        self.addHistogram(monGroup, TH2F("res_ptcone20_relVsMu", "HLT ptcone20/pt resolution as function of avg #mu; #mu; on-off; Count",
                    50, 0, 100,
                    200, -0.2, 0.2))
        self.addHistogram(monGroup, TH2F("res_ptcone20_onVsOff", "online ptcone20 vs offline ptcone20; offline [MeV]; online [MeV]; Count",
                    200, 0.0, 10000.0,
                    200, 0.0, 10000.0))
        self.addHistogram(monGroup, TH2F("res_ptcone20_rel_onVsOff", "online ptcone20/pt vs offline ptcone20/pt; offline; online; Count",
                    200, 0.0, 0.2,
                    200, 0.0, 0.2))

        self.addHistogram(monGroup, TH1F("res_etInEta0", "HLT E_{T} resolution in #eta = [0#div1.37]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_etInEta1", "HLT E_{T} resolution in #eta = [1.37#div1.52]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_etInEta2", "HLT E_{T} resolution in #eta = [1.55#div1.8]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_etInEta3", "HLT E_{T} resolution in #eta = [1.8#div2.45]; (E_{T}(on)-E_{T}(off)) ; Count", 200, -0.1, 0.1))

        # Relative resolutions
        #self.addHistogram(monGroup, TH1F("res_e011", "e011 resolution; (e011(on)-e011(off)) ; Count", 50, -0.05, 0.05))
        #self.addHistogram(monGroup, TH1F("res_e132", "e132 resolution; (e132(on)-e132(off)) ; Count", 50, -0.05, 0.05))
        #self.addHistogram(monGroup, TH1F("res_e237", "e237 resolution; (e237(on)-e237(off)) ; Count", 50, -0.05, 0.05))
        #self.addHistogram(monGroup, TH1F("res_e277", "e277 resolution; (e277(on)-e277(off)) ; Count", 50, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_ethad", "ethad resolution; (ethad(on)-ethad(off)) ; Count", 100, -10, 10))
        self.addHistogram(monGroup, TH1F("res_ethad1", "ethad1 resolution; (ethad1(on)-ethad1(off)) ; Count", 100, -10, 10))
        self.addHistogram(monGroup, TH1F("res_Rhad", "Rhad resolution; (Rhad(on)-Rhad(off)) ; Count", 50, -10., 10.))
        self.addHistogram(monGroup, TH1F("res_Rhad1", "Rhad1; Rhad1 resolution; (Rhad1(on)-Rhad1(off));Count", 50, -10., 10.))
        self.addHistogram(monGroup, TH1F("res_Reta", "Reta resolution; (Reta(on)-Reta(off)) ; Count", 50, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_Rphi", "Rphi resolution; (Rphi(on)-Rphi(off)) ; Count", 50, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_weta1", "weta1 resolution; (weta1(on)-weta1(off)) ; Count", 50, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_weta2", "weta2 resolution; (weta2(on)-weta2(off)) ; Count", 50, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_f1", "f1 resolution; (f1(on)-f1(off)) ; Count", 50, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_f3", "f3 resolution; (f3(on)-f3(off)) ; Count", 50, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_eratio", "eratio resolution; (eratio(on)-eratio(off)) ; Count", 200, -0.001, 0.001))
        self.addHistogram(monGroup, TH1F("res_deta1", "deta1; deta1 ; (deta1(on)-deta1(off))", 100, -1., 1.))
        self.addHistogram(monGroup, TH1F("res_deta2", "deta2; deta2 ; (deta2(on)-deta2(off))", 100, -1., 1.))
        self.addHistogram(monGroup, TH1F("res_dphi2", "dphi2; dphi2 ; (dphi2(on)-dphi2(off))", 100, -1., 1.))
        self.addHistogram(monGroup, TH1F("res_dphiresc", "dphiresc; (dphires(on)-dphires(off)) ; Count", 100, -1., 1.))
        self.addHistogram(monGroup, TH1F("res_d0", "resolution d0; (d0(on)-d0(off)) ; Count", 100, -0.5, 0.5))
        self.addHistogram(monGroup, TH1F("res_d0sig", "resolution d0sig; (d0sig(on)-d0sig(off)) ; Count", 50, -10, 10))
        self.addHistogram(monGroup, TH1F("res_eprobht","resolution eProbHT; (eProbHT(on)-eProbHT(off)); Count",50, -1, 1))
        # TRT
        self.addHistogram(monGroup, TH2F("res_eprobhtVsPt", "eProbHT resolution as function of p_{T}; p_{T} [GeV]; (eprobHT(on)-eprobHT(off)); Count",
                    50, 0., 100.,
                    50, -1., 1.))
        self.addHistogram(monGroup, TH2F("res_eprobht_onVsOff", "online eprobHT vs offline eprobHT; offline ; online ; Count",
                    50, 0., 1.,
                    50, 0., 1.))
        self.addHistogram(monGroup, TH1F("res_nscthits","resolution nSCTHit; (nSCTHits(on)-nSCTHits(off); Count",20, -10, 10))
        self.addHistogram(monGroup, TH1F("res_npixhits","resolution nPixHit; (nPixHits(on)-nPixHits(off)); Count",10, -5, 5))
        self.addHistogram(monGroup, TH1F("res_ptcone20", "resolution ptcone20; ptcone20 (on-off); Count", 200, -20, 20))
        self.addHistogram(monGroup, TH1F("res_ptcone20_rel", "resolution ptcone20/pt; ptcone20/pt (on-off); Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_wtots1", "wtots1 resolution; (wtots1(on)-wtots1(off)) ; Count", 50, -0.05, 0.05))


    def bookResolutionHistos(self, monGroup):
        self.addHistogram(monGroup, TH1F("res_et", "E_{T} resolution; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 100, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_eta", "#eta resolution; (#eta(on)-#eta(off))/#eta(off) ; Count", 40, -0.001, 0.001))
        self.addHistogram(monGroup, TH1F("res_phi", "#phi resolution; (#phi(on)-#phi(off))/#phi(off) ; Count", 40, -0.001, 0.001))
        self.addHistogram(monGroup, TH2F("res_etVsEta", "E_{T} resolution as function of #eta; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                    25, -2.5, 2.5,
                    50, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_etVsEt", "E_{T} resolution as function of E_{T}; E_{T} [GeV]; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                    25, 0., 100.,
                    50, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_ethad", "ethad resolution; (ethad(on)-ethad(off))/ethad(off) ; Count", 20, -5, 5))
        self.addHistogram(monGroup, TH1F("res_ethad1", "ethad1 resolution; (ethad1(on)-ethad1(off))/ethad1(off) ; Count", 20, -5, 5))
        self.addHistogram(monGroup, TH1F("res_Rhad", "Rhad resolution; (Rhad(on)-Rhad(off))/Rhad(off) ; Count", 20, -10., 10.))
        self.addHistogram(monGroup, TH1F("res_Rhad1", "Rhad1; Rhad1 resolution; (Rhad1(on)-Rhad1(off))/Rhad1(off)", 20, -10., 10.))
        self.addHistogram(monGroup, TH1F("res_Reta", "Reta resolution; (Reta(on)-Reta(off))/Reta(off) ; Count", 20, -0.01, 0.01))
        self.addHistogram(monGroup, TH1F("res_Rphi", "Rphi resolution; (Rphi(on)-Rphi(off))/Rphi(off) ; Count", 20, -0.01, 0.01))
        self.addHistogram(monGroup, TH1F("res_weta1", "weta1 resolution; (weta1(on)-weta1(off))/weta1(off) ; Count", 20, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_weta2", "weta2 resolution; (weta2(on)-weta2(off))/weta2(off) ; Count", 20, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_f1", "f1 resolution; (f1(on)-f1(off))/f1(off) ; Count", 20, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_f3", "f3 resolution; (f3(on)-f3(off))/f3(off) ; Count", 20, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_eratio", "eratio resolution; (eratio(on)-eratio(off))/eratio(off) ; Count", 20, -0.001, 0.001))


    def bookElectronResolutionHistos(self, monGroup):
        # Electron
        self.addHistogram(monGroup, TH1F("res_pt", "p_{T} resolution; (p_{T}(on)-p_{T}(off))/p_{T}(off) ; Count", 120, -1.5, 1.5))
        self.addHistogram(monGroup, TH1F("res_deta1", "deta1; deta1 ; (deta1(on)-deta1(off))/deta1(off)", 100, -1., 1.))
        self.addHistogram(monGroup, TH1F("res_deta2", "deta2; deta2 ; (deta2(on)-deta2(off))/deta2(off)", 100, -1., 1.))
        self.addHistogram(monGroup, TH1F("res_dphi2", "dphi2; dphi2 ; (dphi2(on)-dphi2(off))/dphi2(off)", 100, -1., 1.))
        self.addHistogram(monGroup, TH1F("res_dphiresc", "dphiresc; (dphires(on)-dphires(off))/dphires(off) ; Count", 100, -1., 1.))
        self.addHistogram(monGroup, TH1F("res_d0", "resolution d0; (d0(on)-d0(off)) ; Count", 100, -0.5, 0.5))
        self.addHistogram(monGroup, TH1F("res_d0sig", "resolution d0sig; (d0sig(on)-d0sig(off)) ; Count", 50, -10, 10))
        self.addHistogram(monGroup, TH1F("res_eprobht","resolution eProbHT; (eProbHT(on)-eProbHT(off)); Count",50, -1, 1))
        self.addHistogram(monGroup, TH1F("res_nscthits","resolution nSCTHit; (nSCTHits(on)-nSCTHits(off); Count",20, -10, 10))
        self.addHistogram(monGroup, TH1F("res_npixhits","resolution nPixHit; (nPixHits(on)-nPixHits(off)); Count",10, -5, 5))
        # TRT
        self.addHistogram(monGroup, TH2F("res_eprobhtVsPt", "eProbHT resolution as function of p_{T}; p_{T} [GeV]; (eprobHT(on)-eprobHT(off)); Count",
                    50, 0., 100.,
                    50, -1., 1.))
        self.addHistogram(monGroup, TH2F("res_eprobht_onVsOff", "online eprobHT vs offline eprobHT; offline ; online ; Count",
                    50, 0., 1.,
                    50, 0., 1.))


    def bookElectronIsoResolutionHistos(self, monGroup):
        self.addHistogram(monGroup, TH1F("res_ptcone20", "resolution ptcone20; ptcone20 (on-off)/off; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_ptcone20_rel", "resolution ptcone20/pt; ptcone20/pt (on-off)/off; Count", 100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptcone20_relVsEta", "HLT ptcone20/pt resolution as function of #eta; #eta; (on-off)/off; Count",
                    50, -2.47, 2.47,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptcone20_relVsEt", "HLT ptcone20/pt resolution as function of E_{T}; E_{T} [GeV]; (on-off)/off; Count",
                    50, 0., 100.,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptcone20VsMu", "HLT ptcone20 resolution as function of avg #mu; #mu; (on-off)/off; Count",
                    50, 0, 100,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptcone20_relVsMu", "HLT ptcone20/pt resolution as function of avg #mu; #mu; (on-off)/off; Count",
                    50, 0, 100,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptcone20_onVsOff", "online ptcone20 vs offline ptcone20; offline [MeV]; online [MeV]; Count",
                    100, 0.0, 10000.0,
                    100, 0.0, 10000.0))
        self.addHistogram(monGroup, TH2F("res_ptcone20_rel_onVsOff", "online ptcone20/pt vs offline ptcone20/pt; offline; online; Count",
                    100, 0.0, 0.2,
                    100, 0.0, 0.2))

        self.addHistogram(monGroup, TH1F("res_ptvarcone20", "resolution ptvarcone20; ptvarcone20 (on-off)/off; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_ptvarcone20_rel", "resolution ptvarcone20/pt; ptvarcone20/pt (on-off)/off; Count", 100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptvarcone20_relVsEta", "HLT ptvarcone20/pt resolution as function of #eta; #eta; (on-off)/off; Count",
                    50, -2.47, 2.47,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptvarcone20_relVsEt", "HLT ptvarcone20/pt resolution as function of E_{T}; E_{T} [GeV]; (on-off)/off; Count",
                    50, 0., 100.,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptvarcone20VsMu", "HLT ptvarcone20 resolution as function of avg #mu; #mu; (on-off)/off; Count",
                    50, 0, 100,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptvarcone20_relVsMu", "HLT ptvarcone20/pt resolution as function of avg #mu; #mu; (on-off)/off; Count",
                    50, 0, 100,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_ptvarcone20_onVsOff", "online ptvarcone20 vs offline ptvarcone20; offline [MeV]; online [MeV]; Count",
                    100, 0.0, 10000.0,
                    100, 0.0, 10000.0))
        self.addHistogram(monGroup, TH2F("res_ptvarcone20_rel_onVsOff", "online ptvarcone20/pt vs offline ptvarcone20/pt; offline; online; Count",
                    100, 0.0, 0.2,
                    100, 0.0, 0.2))


    def bookPhotonResolutionHistos(self, monGroup):
        self.addHistogram(monGroup, TH1F("res_et_cnv", "HLT E_{T} resolution for converted Photons; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_et_uncnv", "HLT E_{T} resolution for unconverted Photons; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))

        self.addHistogram(monGroup, TH2F("res_cnv_etVsEta", "HLT E_{T} resolution as function of #eta for converted Photons; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                    50, -2.47, 2.47,
                    200, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_cnv_etVsEt", "HLT E_{T} resolution as function of E_{T} for converted Photons; E_{T} [GeV]; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                    50, 0., 100.,
                    200, -0.1, 0.1))

        self.addHistogram(monGroup, TH2F("res_uncnv_etVsEta", "HLT E_{T} resolution as function of #eta for unconverted Photons; #eta; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                    50, -2.47, 2.47,
                    200, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_uncnv_etVsEt", "HLT E_{T} resolution as function of E_{T} for unconverted Photons; E_{T} [GeV]; (E_{T}(on)-E_{T}(off))/E_{T}(off); Count",
                    50, 0., 100.,
                    200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_cnv_etInEta0", "HLT E_{T} resolution in #eta = [0#div1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_cnv_etInEta1", "HLT E_{T} resolution in #eta = [1.37#div1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_cnv_etInEta2", "HLT E_{T} resolution in #eta = [1.55#div1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_cnv_etInEta3", "HLT E_{T} resolution in #eta = [1.8#div2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))

        self.addHistogram(monGroup, TH1F("res_uncnv_etInEta0", "HLT E_{T} resolution in #eta = [0#div1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_uncnv_etInEta1", "HLT E_{T} resolution in #eta = [1.37#div1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_uncnv_etInEta2", "HLT E_{T} resolution in #eta = [1.55#div1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_uncnv_etInEta3", "HLT E_{T} resolution in #eta = [1.8#div2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))

    def bookPhotonIsoResolutionHistos(self, monGroup):
        self.addHistogram(monGroup, TH1F("res_topoetcone20", "resolution topoetcone20; ptcone20 (on-off)/off; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_topoetcone20_rel", "resolution topoetcone20/pt; ptcone20/pt (on-off)/off; Count", 100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_topoetcone20_relVsEta", "HLT topoetcone20/pt resolution as function of #eta; #eta; (on-off)/off; Count",
                    50, -2.47, 2.47,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_topoetcone20_relVsEt", "HLT topoetcone20/pt resolution as function of E_{T}; E_{T} [GeV]; (on-off)/off; Count",
                    50, 0., 100.,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_topoetcone20VsMu", "HLT topoetcone20 resolution as function of avg #mu; #mu; (on-off)/off; Count",
                    50, 0, 100,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_topoetcone20_relVsMu", "HLT topoetcone20/pt resolution as function of avg #mu; #mu; (on-off)/off; Count",
                    50, 0, 100,
                    100, -0.1, 0.1))
        self.addHistogram(monGroup, TH2F("res_topoetcone20_onVsOff", "online topoetcone20 vs offline topoetcone20; offline [GeV]; online [GeV]; Count",
                    100, -10.0, 10.0,
                    100, -10.0, 10.0))
        self.addHistogram(monGroup, TH2F("res_topoetcone20_rel_onVsOff", "online topoetcone20/pt vs offline topoetcone20/pt; offline; online; Count",
                    100, -0.5, 0.5,
                    100, -0.5, 0.5))
        self.addHistogram(monGroup, TH2F("res_topoetcone40_shift_onVsOff", "online topoetcone40-2.45 GeV vs offline topoetcone40-2.45 GeV; offline [GeV]; online [GeV]; Count",
                    100, -10.0, 10.0,
                    100, -10.0, 10.0))
        self.addHistogram(monGroup, TH2F("res_topoetcone40_shift_rel_onVsOff", "online (topoetcone40-2.45 GeV)/pt vs offline (topoetcone40-2.45 GeV)/pt; offline; online; Count",
                    100, -0.5, 0.5,
                    100, -0.5, 0.5))


    def bookExpertResolutionHistos(self, monGroup):

        self.addHistogram(monGroup, TH1F("res_etInEta0", "HLT E_{T} resolution in #eta = [0#div1.37]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_etInEta1", "HLT E_{T} resolution in #eta = [1.37#div1.52]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_etInEta2", "HLT E_{T} resolution in #eta = [1.55#div1.8]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("res_etInEta3", "HLT E_{T} resolution in #eta = [1.8#div2.45]; (E_{T}(on)-E_{T}(off))/E_{T}(off) ; Count", 200, -0.1, 0.1))


        # Relative resolutions
        #self.addHistogram(monGroup, TH1F("res_e011", "e011 resolution; (e011(on)-e011(off))/e011(off) ; Count", 50, -0.05, 0.05))
        #self.addHistogram(monGroup, TH1F("res_e132", "e132 resolution; (e132(on)-e132(off))/e132(off) ; Count", 50, -0.05, 0.05))
        #self.addHistogram(monGroup, TH1F("res_e237", "e237 resolution; (e237(on)-e237(off))/e237(off) ; Count", 50, -0.05, 0.05))
        #self.addHistogram(monGroup, TH1F("res_e277", "e277 resolution; (e277(on)-e277(off))/e277(off) ; Count", 50, -0.05, 0.05))
        self.addHistogram(monGroup, TH1F("res_wtots1", "wtots1 resolution; (wtots1(on)-wtots1(off))/wtots1(off) ; Count", 50, -0.05, 0.05))


        # Detailed resolution plots
        if self.detailLevel:
            self.addHistogram(monGroup, TH2F("res_ethadVsEta", "HLT E_{T} Had resolution as function of #eta; #eta; (ethad(on)-ethad(off))/ethad(off); Count",
                        50, -2.47, 2.47,
                        50, -0.5, 0.5))
            self.addHistogram(monGroup, TH2F("res_ethadVsEt", "HLT E_{T} Had resolution as function of E_{T}; E_{T} [GeV]; (ethad(on)-ethad(off))/ethad(off); Count",
                        50, 0., 100.,
                        50, -0.5, 0.5))
            self.addHistogram(monGroup, TH2F("res_ethad1VsEta", "HLT E_{T} Had1 resolution as function of #eta; #eta; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                        50, -2.47, 2.47,
                        50, -0.5, 0.5))
            self.addHistogram(monGroup, TH2F("res_ethad1VsEt", "HLT E_{T} Had1 resolution as function of E_{T}; E_{T} [GeV]; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                        50, 0., 100.,
                        50, -0.5, 0.5))
            self.addHistogram(monGroup, TH2F("res_RhadVsEta", "HLT E_{T} Rhad resolution as function of #eta; #eta; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                        50, -2.47, 2.47,
                        50, -10, 10))
            self.addHistogram(monGroup, TH2F("res_RhadVsEt", "HLT E_{T} RHad resolution as function of E_{T}; E_{T} [GeV]; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                        50, 0., 100.,
                        50, -10, 10))
            self.addHistogram(monGroup, TH2F("res_Rhad1VsEta", "HLT E_{T} Rhad1 resolution as function of #eta; #eta; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                        50, -2.47, 2.47,
                        50, -10, 10))
            self.addHistogram(monGroup, TH2F("res_Rhad1VsEt", "HLT E_{T} RHad1 resolution as function of E_{T}; E_{T} [GeV]; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                        50, 0., 100.,
                        50, -10, 10))
            self.addHistogram(monGroup, TH2F("res_RetaVsEta", "HLT Reta resolution as function of #eta; #eta; (Reta(on)-Reta(off))/Reta(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_RetaVsEt", "HLT Reta resolution as function of E_{T}; E_{T} [GeV]; (Reta(on)-Reta(off))/Reta(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_RphiVsEta", "HLT Rphi resolution as function of #eta; #eta; (Rphi(on)-Rphi(off))/Rphi(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_RphiVsEt", "HLT Rphi resolution as function of E_{T}; E_{T} [GeV]; (Rphi(on)-Rphi(off))/Rphi(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_weta1VsEta", "HLT weta1 resolution as function of #eta; #eta; (weta1(on)-weta1(off))/weta1(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_weta1VsEt", "HLT weta1 resolution as function of E_{T}; E_{T} [GeV]; (weta1(on)-weta1(off))/weta1(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_weta2VsEta", "HLT weta2 resolution as function of #eta; #eta; (weta2(on)-weta2(off))/weta2(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_weta2VsEt", "HLT weta2 resolution as function of E_{T}; E_{T} [GeV]; (weta2(on)-weta2(off))/weta2(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_wtots1VsEta", "HLT wtots1 resolution as function of #eta; #eta; (wtots1(on)-wtots1off))/wtots1(off); Count",
                        50,-2.47,2.47,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_wtots1VsEt", "HLT wtots1 resolution as function of E_{T}; E_{T} [GeV]; (wtots1(on)-wtots1(off))/wtots1(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_f1VsEta", "HLT f1 resolution as function of #eta; #eta; (f1(on)-f1(off))/f1(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_f1VsEt", "HLT f1 resolution as function of E_{T}; E_{T} [GeV]; (f1(on)-f1(off))/f1(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_f3VsEta", "HLT f3 resolution as function of #eta; #eta; (f3(on)-f3(off))/f3(off); Count",
                        50, -2.47, 2.47,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_f3VsEt", "HLT f3 resolution as function of E_{T}; E_{T} [GeV]; (f3(on)-f3(off))/f3(off); Count",
                        50, 0., 100.,
                        50, -0.05, 0.05))
            self.addHistogram(monGroup, TH2F("res_e2tsts1VsEta", "HLT e2tsts1 resolution as function of #eta; #eta; (e2tsts1(on)-e2tsts1(off))/e2tsts1(off); Count",
                        50, -2.47, 2.47,
                        50, -10, 10))
            self.addHistogram(monGroup, TH2F("res_e2tsts1VsEt", "HLT e2tsts1 resolution as function of E_{T}; E_{T} [GeV]; (e2tsts1(on)-e2tsts1(off))/e2tsts1(off); Count",
                        50, 0., 100.,
                        50, -10, 10))
            self.addHistogram(monGroup, TH2F("res_eratioVsEta", "HLT eratio resolution as function of #eta; #eta; (eratio(on)-eratio(off))/eratio(off); Count",
                        50, -2.47, 2.47,
                        50, -0.001, 0.001))
            self.addHistogram(monGroup, TH2F("res_eratioVsEt", "HLT eratio resolution as function of E_{T}; E_{T} [GeV]; (eratio(on)-eratio(off))/eratio(off); Count",
                        50, 0., 100.,
                        50, -0.001, 0.001))


    def bookExpertL2CaloResolutionHistos(self, monGroup):
        self.addHistogram(monGroup, TH2F("res_f3VsEta", "L2Calo f3 resolution as function of #eta; #eta; (f3(on)-f3(off))/f3(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_f3VsEt", "L2Calo f3 resolution as function of E_{T}; E_{T} [GeV]; (f3(on)-f3(off))/f3(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_f1VsEta", "L2Calo f1 resolution as function of #eta; #eta; (f1(on)-f1(off))/f1(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_f1VsEt", "L2Calo f1 resolution as function of E_{T}; E_{T} [GeV]; (f1(on)-f1(off))/f1(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_weta2VsEta", "L2Calo weta2 resolution as function of #eta; #eta; (weta2(on)-weta2(off))/weta2(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_weta2VsEt", "L2Calo weta2 resolution as function of E_{T}; E_{T} [GeV]; (weta2(on)-weta2(off))/weta2(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_weta1VsEta", "L2Calo weta1 resolution as function of #eta; #eta; (weta1(on)-weta1(off))/weta1(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_weta1VsEt", "L2Calo weta1 resolution as function of E_{T}; E_{T} [GeV]; (weta1(on)-weta1(off))/weta1(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_RetaVsEta", "L2Calo Reta resolution as function of #eta; #eta; (Reta(on)-Reta(off))/Reta(off); Count",
                    50, -2.47, 2.47,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_RetaVsEt", "L2Calo Reta resolution as function of E_{T}; E_{T} [GeV]; (Reta(on)-Reta(off))/Reta(off); Count",
                    50, 0., 100.,
                    50, -0.05, 0.05))
        self.addHistogram(monGroup, TH2F("res_Rhad1VsEta", "L2Calo E_{T} Rhad1 resolution as function of #eta; #eta; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                    50, -2.47, 2.47,
                    50, -10, 10))
        self.addHistogram(monGroup, TH2F("res_Rhad1VsEt", "L2Calo E_{T} RHad1 resolution as function of E_{T}; E_{T} [GeV]; (Rhad1(on)-Rhad1(off))/Rhad1(off); Count",
                    50, 0., 100.,
                    50, -10, 10))
        self.addHistogram(monGroup, TH2F("res_RhadVsEta", "L2Calo E_{T} Rhad resolution as function of #eta; #eta; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                    50, -2.47, 2.47,
                    50, -10, 10))
        self.addHistogram(monGroup, TH2F("res_RhadVsEt", "L2Calo E_{T} RHad resolution as function of E_{T}; E_{T} [GeV]; (Rhad(on)-Rhad(off))/Rhad(off); Count",
                    50, 0., 100.,
                    50, -10, 10))
        self.addHistogram(monGroup, TH2F("res_ethad1VsEta", "L2Calo E_{T} Had1 resolution as function of #eta; #eta; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                    50, -2.47, 2.47,
                    50, -0.5, 0.5))
        self.addHistogram(monGroup, TH2F("res_ethad1VsEt", "L2Calo E_{T} Had1 resolution as function of E_{T}; E_{T} [GeV]; (ethad1(on)-ethad1(off))/ethad1(off); Count",
                    50, 0., 100.,
                    50, -0.5, 0.5))
        self.addHistogram(monGroup, TH2F("res_ethadVsEta", "L2Calo E_{T} Had resolution as function of #eta; #eta; (ethad(on)-ethad(off))/ethad(off); Count",
                    50, -2.47, 2.47,
                    50, -0.5, 0.5))
        self.addHistogram(monGroup, TH2F("res_ethadVsEt", "L2Calo E_{T} Had resolution as function of E_{T}; E_{T} [GeV]; (ethad(on)-ethad(off))/ethad(off); Count",
                    50, 0., 100.,
                    50, -0.5, 0.5))
        self.addHistogram(monGroup, TH2F("res_eratioVsEta", "L2Calo eratio resolution as function of #eta; #eta; (eratio(on)-eratio(off))/eratio(off); Count",
                    50, -2.47, 2.47,
                    50, -0.001, 0.001))
        self.addHistogram(monGroup, TH2F("res_eratioVsEt", "L2Calo eratio resolution as function of E_{T}; E_{T} [GeV]; (eratio(on)-eratio(off))/eratio(off); Count",
                    50, 0., 100.,
                    50, -0.001, 0.001))

    def bookDistributionHistos(self, monGroup):
        self.addHistogram(monGroup, TH1F("et", "ET; ET [GeV] ; Count", 100, 0., 100.))
        self.addHistogram(monGroup, TH1F("eta", "eta; eta ; Count", self._nEtabins, self._etabins))
        self.addHistogram(monGroup, TH1F("phi", "phi; phi ; Count", 20, -3.2, 3.2))

    def bookEgammaDistributionHistos(self, monGroup):
        self.addHistogram(monGroup, TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 100, 0., 2000.))
        #self.addHistogram(monGroup, TH1F("e011", "e011; e011 ; Count", 165, -15., 150.))
        #self.addHistogram(monGroup, TH1F("e132", "e132; e132 ; Count", 165, -15., 150.))
        #self.addHistogram(monGroup, TH1F("e237", "e237; e237 ; Count", 215, -15., 200.))
        #self.addHistogram(monGroup, TH1F("e277", "e277; e277 ; Count", 215, -15., 200.))
        self.addHistogram(monGroup, TH1F("ethad", "ethad; ethad ; Count", 20, -10, 10))
        self.addHistogram(monGroup, TH1F("ethad1", "ethad1; ehad1 ; Count", 20, -10, 10))
        self.addHistogram(monGroup, TH1F("weta1", "weta1; weta1 ; Count", 12, 0.4, 1.))
        self.addHistogram(monGroup, TH1F("weta2", "weta2; weta2 ; Count", 20, 0., 0.02))
        #self.addHistogram(monGroup, TH1F("wtots1", "wtots1; wtots1 ; Count", 50, 0., 0.05))
        self.addHistogram(monGroup, TH1F("f1", "f1; f1 ; Count", 11, -0.1, 1.))
        self.addHistogram(monGroup, TH1F("f3", "f3; f3 ; Count", 21, -0.1, 0.2))
        self.addHistogram(monGroup, TH1F("Reta", "Reta; Reta ; Count", 15, 0., 1.5))
        self.addHistogram(monGroup, TH1F("Rphi", "Rphi; Rphi ; Count", 15, 0., 1.5))
        self.addHistogram(monGroup, TH1F("Rhad", "Rhad; Rhad ; Count", 35, -0.3, 0.3))
        self.addHistogram(monGroup, TH1F("Rhad1", "Rhad1; Rhad1 ; Count", 30, -0.3, 0.3))
        self.addHistogram(monGroup, TH1F("eratio","eratio; eratio; Count",20, 0, 2))
        self.addHistogram(monGroup, TH1F("topoetcone20", "topoetcone20; topoetcone20 [GeV] ; Count", 100, -10.0, 10.0))
        self.addHistogram(monGroup, TH1F("topoetcone20_rel", "topoetcone20/pt; topoetcone20/pt ; Count", 100, -0.5, 0.5))
        self.addHistogram(monGroup, TH1F("topoetcone40_shift", "topoetcone40-2.45 GeV; topoetcone40-2.45 GeV [GeV] ; Count", 100, -10.0, 10.0))
        self.addHistogram(monGroup, TH1F("topoetcone40_shift_rel", "(topoetcone40-2.45 GeV)/pt; (topoetcone40-2.45 GeV)/pt ; Count", 100, -0.5, 0.5))

    def bookElectronDistributionHistos(self, monGroup):
        self.addHistogram(monGroup, TH1F("pt", "p_{T}; p_{T} [GeV] ; Count", 100,0.,100.))
        self.addHistogram(monGroup, TH1F("deta1", "deta1; deta1 ; Count", 40, -0.01, 0.01))
        self.addHistogram(monGroup, TH1F("deta1_EMECA", "deta1 EMEC-A; deta1 ; Count", 40, -0.01, 0.01))
        self.addHistogram(monGroup, TH1F("deta1_EMECC", "deta1 EMEC-C; deta1 ; Count", 40, -0.01, 0.01))
        self.addHistogram(monGroup, TH1F("deta1_EMEBA", "deta1 EMEB-A; deta1 ; Count", 40, -0.01, 0.01))
        self.addHistogram(monGroup, TH1F("deta1_EMEBC", "deta1 EMEB-A; deta1 ; Count", 40, -0.01, 0.01))
        self.addHistogram(monGroup, TH1F("deta2", "deta2; deta2 ; Count", 40, -0.01, 0.01))
        self.addHistogram(monGroup, TH1F("dphi2", "dphi2; dphi2 ; Count", 40, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("dphiresc", "dphiresc; dphiresc ; Count", 40, -0.1, 0.1))
        self.addHistogram(monGroup, TH1F("d0", "d0; d0 ; Count", 40, -1, 1))
        self.addHistogram(monGroup, TH1F("d0sig", "d0sig; d0sig ; Count", 40, -10, 10))
        self.addHistogram(monGroup, TH1F("eprobht","eProbHT; eProbHT; Count",20, 0, 1.))
        self.addHistogram(monGroup, TH1F("nscthits","nSCTHit; nSCTHits; Count",20, 0, 20))
        self.addHistogram(monGroup, TH1F("npixhits","nPixHit; nPixHits; Count",10, 0, 10))
        self.addHistogram(monGroup, TH1F("charge","charge; charge; Count", 4,-2,2))
        self.addHistogram(monGroup, TH1F("ptcone20", "ptcone20; ptcone20; Count", 50, 0.0, 5.0))
        self.addHistogram(monGroup, TH1F("ptcone20_rel", "ptcone20/pt; ptcone20/pt; Count", 50, 0.0, 1.0))
        self.addHistogram(monGroup, TH1F("ptvarcone20", "ptcone20; ptcone20; Count", 50, 0.0, 5.0))
        self.addHistogram(monGroup, TH1F("ptvarcone20_rel", "ptcone20/pt; ptcone20/pt; Count", 50, 0.0, 1.0))
        self.addHistogram(monGroup, TH1F("lhtight_discriminant", "lh tight discriminant; lh tight discriminant; Count", 50, -5.,2.))
        self.addHistogram(monGroup, TH1F("lhmedium_discriminant", "lh medium discriminant; lh medium discriminant; Count", 50, -5.,2.))
        self.addHistogram(monGroup, TH1F("lhloose_discriminant", "lh loose discriminant; lh loose discriminant; Count", 50, -5.,2.))

        if self.detailLevel:
            addHistogram(monGroup, TH2F("deta1_vs_clusterEta", "HLT deta1 as function of cluster #eta; #eta; deta1; Count",
                        50, -2.47, 2.47,
                        90, -0.03, 0.03))


    def bookEfficiency2DHistos(self, monGroup):
        self.addHistogram(monGroup, TH2F("match_coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",
                self._ncoarseEtbins, self._coarseEtbins, self._ncoarseEtabins, self._coarseEtabins))
        self.addHistogram(monGroup, TH2F("coarse_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",
                self._ncoarseEtbins, self._coarseEtbins, self._ncoarseEtabins, self._coarseEtabins))
        self.addHistogram(monGroup, TProfile2D("eff_coarse_et_eta","#epsilon(#eta..E_{T}); E_{T} GeV ;#eta; Count",
                self._ncoarseEtbins, self._coarseEtbins, self._ncoarseEtabins, self._coarseEtabins))

        if self.detailLevel:
            self.addHistogram(monGroup, TH2F("match_et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",
                        self._ndefaultEtbins, self._defaultEtbins, self._ndefaultEtabins, self._defaultEtabins))
            self.addHistogram(monGroup, TH2F("et_eta","Trigger Matched Offline #eta vs et; E_{T} GeV ;#eta; Count",
                        self._ndefaultEtbins, self._defaultEtbins, self._ndefaultEtabins, self._defaultEtabins))
            self.addHistogram(monGroup, TProfile2D("eff_et_eta","#epsilon(#eta..E_{T}); E_{T} GeV ;#eta; Count",
                        self._ndefaultEtbins, self._defaultEtbins, self._ndefaultEtabins, self._defaultEtabins))


    def bookEfficiencyTProfile(self, monGroup):
        self.addHistogram(monGroup, TProfile("eff_pt", "#epsilon(p_T); p_{T} ; Efficiency", self._nEtbins, self._etbins))
        self.addHistogram(monGroup, TProfile("eff_et", "#epsilon(E_T); E_{T} [GeV] ; Efficiency", self._nEtbins, self._etbins))
        self.addHistogram(monGroup, TProfile("eff_highet", "#epsilon(E_T); E_{T} [GeV] ; Efficiency", 40, 0., 1000.))
        self.addHistogram(monGroup, TProfile("eff_eta", "#epsilon(#eta); #eta ; Efficiency", self._nEtabins, self._etabins))
        self.addHistogram(monGroup, TProfile("eff_phi", "#epsilon(#phi); #phi ; Efficiency", 20, -3.2, 3.2))
        self.addHistogram(monGroup, TProfile("eff_mu", "#epsilon(<#mu>); <#mu> ; Efficiency", 16, 0, 80))

        if self.detailLevel:
            self.addHistogram(monGroup, TProfile("eff_npvtx", "#epsilon(npvtx); npvtx ; Efficiency", 16, 0, 80))

    def bookEfficiencyHistos(self, monGroup):
        self.bookEfficiencyTProfile(monGroup)
        # Numerator
        self.addHistogram(monGroup, TH1F("match_pt", "Trigger Matched Offline p_{T}; p_{T} [GeV] ; Count", self._nEtbins, self._etbins))
        self.addHistogram(monGroup, TH1F("match_et", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", self._nEtbins, self._etbins))
        self.addHistogram(monGroup, TH1F("match_highet", "Trigger Matched Offline E_{T}; E_{T} [GeV]; Count", 40, 0., 1000.))
        self.addHistogram(monGroup, TH1F("match_eta", "Trigger Matched Offline #eta; #eta ; Count", self._nEtabins, self._etabins))
        self.addHistogram(monGroup, TH1F("match_phi", "Trigger Matched #phi; #phi ; Count", 20, -3.2, 3.2))
        self.addHistogram(monGroup, TH1F("match_mu", "Trigger Matched <#mu>; <#mu> ; Count", 16, 0, 80))

        if self.detailLevel:
            self.addHistogram(monGroup, TH1F("match_npvtx", "Trigger Matched npvtx; npvtx ; Count", 16, 0, 80))

        # Denominator
        self.addHistogram(monGroup, TH1F("pt", "Offline p_{T}; p_{T} [GeV] ; Count", self._nEtbins, self._etbins))
        self.addHistogram(monGroup, TH1F("et", "Offline E_{T}; E_{T} [GeV] ; Count", self._nEtbins, self._etbins))
        self.addHistogram(monGroup, TH1F("highet", "Offline E_{T}; E_{T} [GeV] ; Count", 40, 0., 1000.))
        self.addHistogram(monGroup, TH1F("eta", "Offline #eta; #eta ; Count", self._nEtabins, self._etabins))
        self.addHistogram(monGroup, TH1F("phi", "Offline #phi; #phi ; Count", 20, -3.2, 3.2))
        self.addHistogram(monGroup, TH1F("mu", "<#mu>; <#mu> ; Count", 16, 0, 80))
        if self.detailLevel:
            self.addHistogram(monGroup, TH1F("npvtx", "npvtx; npvtx ; Count", 16, 0, 80))

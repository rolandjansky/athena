#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
def JepJemMonitoringConfig(inputFlags):
    '''Function to configure LVL1 JepJem algorithm in the monitoring system.'''

    import math 
    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'JepJemMonitoringCfg')

    # get any algorithms
    JepJemMonAlg = helper.addAlgorithm(CompFactory.JepJemMonitorAlgorithm,'JepJemMonAlg')

    # add any steering
    groupName = 'JepJemMonitor' # the monitoring group name is also used for the package name
    JepJemMonAlg.PackageName = groupName
    maxSlices = 5
    JepJemMonAlg.s_maxSlices = maxSlices
    crates = 2  # number of JEM crates
    JepJemMonAlg.s_crates = crates
    modules = 16  # number of modules per crate
    JepJemMonAlg.s_modules = modules
    tobsPerJEM = 4  # maximum number of TOBs per JEM sent to CMX
    JepJemMonAlg.s_tobsPerJEM = tobsPerJEM

    # histogram paths
    mainDir = 'L1Calo'
    trigPath = 'JEM/'
    inputPath = trigPath + '/Input/'
    outputPath = trigPath + '/Output/'
    errorPath = trigPath + '/Errors/'
    monRoIPath = outputPath + '/RoI/'
    monESumPath = outputPath + '/EnergySums/'
    monHardPath = errorPath + '/Hardware/'

    # add monitoring algorithm to group, with group name and main directory
    myGroup = helper.addGroup(JepJemMonAlg, groupName, mainDir)

    ##### BINNINGS #####
    phibins_1d = 32
    phibins_2d = 32
    phimin = 0.
    phimax_1d = 2.*math.pi
    phimax_2d = 32.
    etabins_1d = [-4.9,-3.2,-2.9,-2.7,-2.4,-2.2,-2.0,-1.8,
                  -1.6,-1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,
                  0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,
                  2.0,2.2,2.4,2.7,2.9,3.2,4.9]
    etabins_2d = 32
    etamin_2d = -3.2
    etamax_2d = 3.2
    jebins = 256
    maxEnergyRange = 1024
    maxEnergySum = 16384

    # binning keyword argument dictionaries
    energy_bins = {
        'xbins': jebins - 1, 'xmin': 1, 'xmax': maxEnergyRange
    }
    eta_bins = {
        'xbins': etabins_1d
    }
    phi_bins = {
        'xbins': phibins_1d, 'xmin': phimin, 'xmax': phimax_1d
    }
    eta_phi_bins = {
        'xbins': etabins_2d, 'xmin': etamin_2d, 'xmax': etamax_2d,
        'ybins': phibins_2d, 'ymin': phimin,    'ymax': phimax_2d
    }

    # bin labels
    eta_phi_xlabels = [''] * 32
    for ch in range(-11, 12, 2):
        chan = ch
        if chan>=-1: chan += 1
        eta = chan/5. + 0.1
        eta_phi_xlabels[chan+16] = "{}/{:.2f}".format(chan, eta)
    eta_phi_xlabels[1]  = "-15/-3.05"
    eta_phi_xlabels[3]  = "-13/-2.55"
    eta_phi_xlabels[28] = "12/2.55"
    eta_phi_xlabels[30] = "14/3.05" 

    eta_phi_ylabels = [''] * 32
    phiBin     = math.pi/16.
    halfPhiBin = math.pi/32.
    for chan in range(0, 32, 2):
        rad = chan*phiBin + halfPhiBin
        eta_phi_ylabels[chan+1] = "{}/{:.2f}".format(chan, rad)
    eta_phi_ylabels[-1] = "etaVphi"

    eta_phi_bin_labels = {
        'xlabels': eta_phi_xlabels,
        'ylabels': eta_phi_ylabels
    }

    #### HISTOS ####
    # EM 1D
    myGroup.defineHistogram('phiJepJemJE_em;jem_em_1d_jetEl_Phi', 
                            title='EM TowerSum distribution per #phi  --  JEM input;#phi', type='TH1F', 
                            path=inputPath, cutmask='', **phi_bins)
    myGroup.defineHistogram('etaJepJemJE_em;jem_em_1d_jetEl_Eta', 
                            title='EM TowerSum distribution per #eta  --  JEM input;#eta', type='TH1F', 
                            path=inputPath,  cutmask='', **eta_bins)
    myGroup.defineHistogram('etJepJemJE_em;jem_em_1d_jetEl_Energy', 
                            title='TowerSum EM energy distribution  --  JEM input;em energy [GeV]', type='TH1F', 
                            path=inputPath, cutmask='', **energy_bins)

    # HAD 1D
    myGroup.defineHistogram('phiJepJemJE_had;jem_had_1d_jetEl_Phi', 
                            title='HAD TowerSum distribution per #phi  --  JEM input;#phi', type='TH1F', 
                            path=inputPath, cutmask='',
                            xbins=phibins_1d, xmin=phimin, xmax=phimax_1d)
    myGroup.defineHistogram('etaJepJemJE_had;jem_had_1d_jetEl_Eta', 
                            title='HAD TowerSum distribution per #eta  --  JEM input;#eta', type='TH1F',
                            path=inputPath, cutmask='', **eta_bins)
    myGroup.defineHistogram('etJepJemJE_had;jem_had_1d_jetEl_Energy', 
                            title='TowerSum HAD energy distribution  --  JEM input;had energy [GeV]', type='TH1F', 
                            path=inputPath, cutmask='', **energy_bins)

    # EM 2D
    myGroup.defineHistogram('etaScaledJepJemJE_em,phiScaledJepJemJE_em;jem_em_2d_etaPhi_jetEl_HitMapWeighted',
                            title='#eta - #phi map of EM TowerSum weighted with energy  --  JEM input', type='TH2F',
                            cutmask='', path=inputPath, **eta_phi_bins, **eta_phi_bin_labels,
                            weight='em_weight')

    # HAD 2D
    myGroup.defineHistogram('etaScaledJepJemJE_had,phiScaledJepJemJE_had;jem_had_2d_etaPhi_jetEl_HitMapWeighted',
                            title='#eta - #phi map of HAD TowerSum weighted with energy  --  JEM input', type='TH2F',
                            cutmask='', path=inputPath, **eta_phi_bins, **eta_phi_bin_labels,
                            weight='had_weight')


    # Number of triggered slice
    myGroup.defineHistogram('jem_1d_TriggeredSlice_num;jem_1d_TriggeredSlice', 
                            title='Number of the Triggered Slice for JE;#Slice', type='TH1F',
                            path=inputPath, cutmask='',
                            xbins=maxSlices, xmin=0, xmax=maxSlices)

    # JEM E_t Sums
    myGroup.defineHistogram('JEMEtSums_Ex;jem_1d_energy_SubSumsEx', 
                            title='JEM E_{x}^{JEM}  --  JEM DAQ;Ex [GeV]', type='TH1F', 
                            path=monESumPath, cutmask='',
                            xbins=jebins, xmin=0, xmax=maxEnergySum)
    myGroup.defineHistogram('JEMEtSums_Ey;jem_1d_energy_SubSumsEy', 
                            title='JEM E_{y}^{JEM}  --  JEM DAQ;Ey [GeV]', type='TH1F', 
                            path=monESumPath, cutmask='',
                            xbins=jebins, xmin=0, xmax=maxEnergySum)
    myGroup.defineHistogram('JEMEtSums_Et;jem_1d_energy_SubSumsEt', 
                            title='JEM E_{t}^{JEM}  --  JEM DAQ;Et [GeV]', type='TH1F', 
                            path=monESumPath, cutmask='',
                            xbins=jebins, xmin=0, xmax=maxEnergySum)

    # Timeslices
    for i in range(maxSlices):
        # EM
        myGroup.defineHistogram('etaSliceJepJemJE_em_num{0},phiSliceJepJemJE_em_num{0};jem_em_2d_etaPhi_jetEl_HitMapSlice{0}'.format(i),
                                title='#eta - #phi map of EM TowerSum for Timeslice {0}  --  JEM input'.format(i), type='TH2F',
                                cutmask='', path=inputPath, **eta_phi_bins, **eta_phi_bin_labels)
        # HAD
        myGroup.defineHistogram('etaSliceJepJemJE_had_num{0},phiSliceJepJemJE_had_num{0};jem_had_2d_etaPhi_jetEl_HitMapSlice{0}'.format(i),
                                title='#eta - #phi map of HAD TowerSum for Timeslice {0}  --  JEM input'.format(i), type='TH2F',
                                cutmask='', path=inputPath, **eta_phi_bins, **eta_phi_bin_labels)

    # Error Maps
    # ==============
    subStatus_labels = ["GLinkParity", "GLinkProtocol", "BCNMismatch", "FIFOOverflow", "ModuleError", "", "GLinkDown", "GLinkTimeout"]
    numberPairs_labels = [""] * 32
    for first in range(0, 2):
        bin = first * 16
        for second in range(0, 15, 2):
            numberPairs_labels[bin] = "{}/{}".format(first, second)
            bin += 2

    myGroup.defineHistogram('GLinkParityError,jemLoc;jepjem_2d_Status',
                            title='Error reports from JEM SubStatus Word;;Crate/Module', type='TH2F',
                            cutmask='', path=monHardPath,
                            xbins=8,xmin=0.,xmax=8.0,ybins=32,ymin=0.,ymax=32.0,
                            xlabels=subStatus_labels, ylabels=numberPairs_labels)
    # EM
    myGroup.defineHistogram('etaScaledJepJemJE_em,phiScaledJepJemJE_em;jem_em_2d_etaPhi_jetEl_Parity',
                            title='Jet Element EM Parity Errors;#eta;#phi', type='TH2F',
                            cutmask='parityErrorJepJE_em', path=monHardPath, **eta_phi_bins)
    myGroup.defineHistogram('etaScaledJepJemJE_em,phiScaledJepJemJE_em;jem_em_2d_etaPhi_jetEl_LinkDown',
                            title='Jet Element EM Link Down Errors;#eta;#phi', type='TH2F',
                            cutmask='linkDownErrorJepJE_em', path=monHardPath, **eta_phi_bins)
    # HAD
    myGroup.defineHistogram('etaScaledJepJemJE_had,phiScaledJepJemJE_had;jem_had_2d_etaPhi_jetEl_Parity',
                            title='Jet Element Had Parity Errors;#eta;#phi', type='TH2F',
                            cutmask='parityErrorJepJE_had', path=monHardPath, **eta_phi_bins)
    myGroup.defineHistogram('etaScaledJepJemJE_had,phiScaledJepJemJE_had;jem_had_2d_etaPhi_jetEl_LinkDown',
                            title='Jet Element Had Link Down Errors;#eta;#phi', type='TH2F',
                            cutmask='linkDownErrorJepJE_had', path=monHardPath, **eta_phi_bins)

    # Error Summaries
    NumberOfSummaryBins=5
    err_summary_bin_labels = ["EM parity", "Had parity", "EM link", "Had link", "JEM status"]

    myGroup.defineHistogram('jem_1d_ErrorSummary;jem_1d_ErrorSummary',
                            title='Summary of JEM Data Errors;;',type='TH1F',
                            cutmask='',path=monHardPath, xlabels=err_summary_bin_labels,
                            xbins=NumberOfSummaryBins, xmin=0.,xmax=NumberOfSummaryBins)

    # Number of Error Event Number Samples
    EventSamples=10
    myGroup.defineHistogram('GLinkParityError,jemLoc;jem_2d_ErrorEventNumbers',
                            title='JEM Error Event Numbers;Events with Error/Mismatch',type='TH2I',
                            cutmask='',path=monHardPath, ylabels=err_summary_bin_labels,
                            xbins=EventSamples,xmin=0,xmax=EventSamples,
                            ybins=NumberOfSummaryBins,ymin=0,ymax=NumberOfSummaryBins)

    # RoIs
    # ==========
    # bin labels
    eta_phi_roi_xlabels = [''] * 32
    for chan in range(-10, 12, 2):
        eta = chan/5.
        eta_phi_roi_xlabels[chan+15] = "{}/{:.2f}".format(chan, eta)
    eta_phi_roi_xlabels[1]  = "-14/-2.95"
    eta_phi_roi_xlabels[3]  = "-12/-2.45"
    eta_phi_roi_xlabels[27] = "12/2.45"
    eta_phi_roi_xlabels[29] = "14/2.95"
    eta_phi_roi_xlabels[31] = "16/4.05"

    eta_phi_roi_ylabels = [''] * 32
    phiBin = math.pi/16.
    for chan in range(0, 32, 2):
        rad = (chan + 1)*phiBin
        eta_phi_roi_ylabels[chan] = "{}/{:.2f}".format(chan, rad)
    eta_phi_roi_ylabels[-1] = "etaVphi"

    eta_phi_roi_bin_labels = {
        'xlabels': eta_phi_roi_xlabels,
        'ylabels': eta_phi_roi_ylabels
    }

    maxLarge = 1024
    maxSmall = 512
    myGroup.defineHistogram('jemRoiEnergyLg;jem_1d_roi_EnergyLg',
                            title='JEM TOB RoI Energy Large Window Size;', type='TH1F',
                            cutmask='', path=monRoIPath,
                            xbins=jebins, xmin=0., xmax=maxLarge)
    myGroup.defineHistogram('jemRoiEnergySm;jem_1d_roi_EnergySm',
                            title='JEM TOB RoI Energy Small Window Size;', type='TH1F',
                            cutmask='', path=monRoIPath,
                            xbins=jebins, xmin=0., xmax=maxSmall)
    # JEM TOB RoI TOBs per JEM
    myGroup.defineHistogram('jemRoiTOBsPerJEM;jem_1d_roi_TOBsPerJEM',
                            title='JEM TOB RoI TOBs per JEM;Number of TOBs', type='TH1F',
                            cutmask='', path=monRoIPath,
                            xbins=tobsPerJEM + 1, xmin=1, xmax=tobsPerJEM + 2)
    # RoI eta/phi maps
    myGroup.defineHistogram('jemRoiEta,jemRoiPhi;jem_2d_etaPhi_roi_Hitmap',
                            title='JEM TOB RoIs Hit Map;;', type='TH2F',
                            cutmask='', path=monRoIPath, **eta_phi_bins, **eta_phi_roi_bin_labels)
    myGroup.defineHistogram('jemRoiEtaWeighted,jemRoiPhiWeighted;jem_2d_etaPhi_roi_EtWeighted',
                            title='JEM TOB RoIs Hit Map Weighted by Energy;;', type='TH2F',
                            cutmask='', path=monRoIPath, **eta_phi_bins, **eta_phi_roi_bin_labels,
                            weight='jemRoiEnergyWeight')

    acc = helper.result()
    result.merge(acc)
    return result


if __name__ == '__main__':
    # For direct tests
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # set debug level for whole job
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO #DEBUG
    log.setLevel(INFO)

    # set input file and config options
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    import glob

    inputs = glob.glob('/eos/atlas/atlastier0/rucio/data18_13TeV/physics_Main/00354311/data18_13TeV.00354311.physics_Main.recon.ESD.f1129/data18_13TeV.00354311.physics_Main.recon.ESD.f1129._lb0013._SFO-8._0001.1')
    ConfigFlags.Input.Files = inputs
    ConfigFlags.Output.HISTFileName = 'JepJemMonitorOutput_LVL1.root'

    ConfigFlags.lock()
    ConfigFlags.dump() # print all the configs

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    JepJemMonitorCfg = JepJemMonitoringConfig(ConfigFlags)
    cfg.merge(JepJemMonitorCfg)

    # message level for algorithm
    JepJemMonitorCfg.getEventAlgo('JepJemMonAlg').OutputLevel = 2 # 1/2 INFO/DEBUG
    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=-1
    cfg.run(nevents)

#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
def JepCmxMonitoringConfig(inputFlags):
    '''Function to configure LVL1 JepCmx algorithm in the monitoring system.'''

    # get the component factory - used for getting the algorithms
    from AthenaConfiguration.AutoConfigFlags import GetFileMD
    from AthenaConfiguration.Enums import Format
    from AthenaConfiguration.ComponentFactory import CompFactory
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from TrigConfigSvc.TriggerConfigAccess import getL1MenuAccess

    result = ComponentAccumulator()

    # make the athena monitoring helper
    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'JepCmxMonitoringCfg')

    # Use metadata to check Run3 compatible trigger info is available  
    md = GetFileMD(inputFlags.Input.Files)
    inputContainsRun3FormatConfigMetadata = ("metadata_items" in md and any(('TriggerMenuJson' in key) for key in md["metadata_items"].keys()))
    if inputFlags.Input.Format is Format.POOL and not inputContainsRun3FormatConfigMetadata:
        # No L1 menu available in the POOL file.
        return helper.result()

    # get any algorithms
    JepCmxMonAlg = helper.addAlgorithm(CompFactory.JepCmxMonitorAlgorithm,'JepCmxMonAlg')

    # add any steering
    groupName = 'JepCmxMonitor' # the monitoring group name is also used for the package name
    JepCmxMonAlg.PackageName = groupName
    crates = 2  # Number of JEM crates
    JepCmxMonAlg.s_crates = crates
    modules = 16  # Number of modules per crate
    JepCmxMonAlg.s_modules = modules
    tobsPerJem = 4  # Maximum number of TOBs per JEM sent to CMX
    JepCmxMonAlg.s_tobsPerJem = tobsPerJem
    maxTobsPerCmx = 64  # Maximum number of TOBs per CMX plotted
    JepCmxMonAlg.s_maxTobsPerCmx = maxTobsPerCmx
    
    mainDir = 'L1Calo'
    trigPath = 'JEM_CMX/'
    inputPath = trigPath + '/Input/'
    outputPath = trigPath + '/Output/'
    errorPath = trigPath + '/Errors/'
    jetInPath = inputPath + 'Jet/'
    jetOutPath = outputPath + 'Jet/'
    energyInPath = inputPath + 'Energy/'
    energyOutPath = outputPath + 'Energy/'
    roiPath = outputPath + '/RoI/'
    errHardPath = errorPath + '/Hardware/'

    maxLarge = 1024.
    maxSmall = 512.
    maxSum = 16384.
    nbins = 256
    nFrame = 8
    nLoc = 4
    NumberOfSummaryBins = 4
    EventSamples = 10

    # trigger threshold labels
    l1menu = getL1MenuAccess(inputFlags)
    jetThresholdNames = thresholdLabels(l1menu, 'JET')
    xeThresholdNames = thresholdLabels(l1menu, 'XE')
    teThresholdNames = thresholdLabels(l1menu, 'TE')
    xsThresholdNames = thresholdLabels(l1menu, 'XS')

    subStatus_labels = ['GLinkParity', 'GLinkProtocol', 'BCNMismatch', 'FIFOOverflow', 'ModuleError', '', 'GLinkDown', 'GLinkTimeout']
    errSummary_labels = ['Jet status', 'Energy status', 'Jet parity', 'Energy parity']
    
    # predefined binnings
    moduleVsCrate_bins = {
        'xbins': modules, 'xmin': 0., 'xmax': modules, 'xlabels': numberLabels(modules),
        'ybins': crates, 'ymin': 0., 'ymax': crates, 'ylabels': numberLabels(crates)
    }
    xySums_bins = {'xbins': nbins, 'xmin': -maxSum, 'xmax': maxSum}
    etSums_bins = {'xbins': nbins, 'xmin': 0., 'xmax': 2*maxSum}
    energyOverflow_bins = {
        'xbins': 3, 'xmin': 0., 'xmax': 3., 'xlabels': ['Ex', 'Ey', 'Et'],
        'ybins': 3, 'ymin': 0., 'ymax': 3., 'ylabels': ['Remote', 'Local', 'Total'],
    }
    subStatusVsCrate_bins = {
        'xbins': crates, 'xmin': 0., 'xmax': crates, 'xlabels': numberLabels(crates),
        'ybins': len(subStatus_labels), 'ymin': 0., 'ymax': len(subStatus_labels), 'ylabels': subStatus_labels
    }

    # add monitoring algorithm to group, with group name and main directory 
    myGroup = helper.addGroup(JepCmxMonAlg, groupName, mainDir)

    #### HISTOS ####

    # CMX Jet TOBs
    myGroup.defineHistogram('jetTobEnergyLg;cmx_1d_tob_EnergyLg',
                            title='CMX-Jet TOB Energy Large Window Size', type='TH1F',
                            path=jetInPath, cutmask='',
                            xbins=nbins, xmin=0., xmax=maxLarge)
    myGroup.defineHistogram('jetTobEnergySm;cmx_1d_tob_EnergySm',
                            title='CMX-Jet TOB Energy Small Window Size', type='TH1F',
                            path=jetInPath,  cutmask='',
                            xbins=nbins, xmin=0., xmax=maxSmall)
    myGroup.defineHistogram('tobsPerJem;cmx_1d_tob_TOBsPerJEM',
                            title='CMX-Jet TOBs per JEM;Number of TOBs', type='TH1F',
                            path=jetInPath, cutmask='',
                            xbins=tobsPerJem + 1, xmin=1., xmax=tobsPerJem + 2,
                            xlabels=numberLabels(tobsPerJem + 1, 1))
    myGroup.defineHistogram('tobsPerCmx;cmx_1d_tob_TOBsPerCMX',
                            title='CMX-Jet TOBs per CMX;Number of TOBs', type='TH1F',
                            path=jetInPath, cutmask='',
                            xbins=maxTobsPerCmx, xmin=0., xmax=maxTobsPerCmx)
    myGroup.defineHistogram('jetTobHitmap_x,jetTobHitmap_y;cmx_2d_tob_Hitmap',
                            title='CMX-Jet TOBs Hit Map;Crate/Module;Frame/Local Coord', type='TH2F',
                            path=jetInPath, cutmask='',
                            xbins=32, xmin=0., xmax=32., ybins=32, ymin=0., ymax=32.,
                            xlabels=numberPairs(crates, modules, step=2), 
                            ylabels=numberPairs(nFrame, nLoc, step=2))
    # Energy SubSums
    e_subsum_bins = {'xbins': nbins, 'xmin': 0., 'xmax': maxSum}
    myGroup.defineHistogram('eSubSumsEx;cmx_1d_energy_SubSumsEx',
                            title='CMX E_{x}^{JEM};Ex [GeV]', type='TH1F',
                            path=energyInPath, cutmask='', **e_subsum_bins)
    myGroup.defineHistogram('eSubSumsEy;cmx_1d_energy_SubSumsEy',
                            title='CMX E_{y}^{JEM};Ey [GeV]', type='TH1F',
                            path=energyInPath, cutmask='', **e_subsum_bins)
    myGroup.defineHistogram('eSubSumsEt;cmx_1d_energy_SubSumsEt',
                            title='CMX E_{t}^{JEM};Et [GeV]', type='TH1F',
                            path=energyInPath, cutmask='', **e_subsum_bins)
    
    # -------------------------- CMX output to DAQ ---------------------------

    # jet outputs
    myGroup.defineHistogram('threshTotalMainVals;cmx_1d_thresh_TotalMainHits',
                            title='Main Jet Multiplicity per Threshold', type='TH1F',
                            path=jetOutPath, cutmask='', weight='threshTotalMainHits',
                            xbins=len(jetThresholdNames), xmin=0., xmax=len(jetThresholdNames), xlabels=jetThresholdNames)
    myGroup.defineHistogram('threshRoiOverflow;cmx_1d_thresh_RoIOverflow',
                            title='CMX-Jet Hits RoI Overflow', type='TH1F',
                            path=jetOutPath, cutmask='',
                            xbins=3, xmin=0., xmax=3., xlabels=['Remote', 'Local', 'Total'])
    myGroup.defineHistogram('topoDataChecksum;cmx_1d_topo_DataChecksum',
                            title='CMX-Jet Topo Output Data Checksum Non-zero;Crate', type='TH1F',
                            path=jetOutPath, cutmask='',
                            xbins=crates, xmin=0., xmax=crates, xlabels=numberLabels(crates))
    myGroup.defineHistogram('topoJemOccMap_mod,topoJemOccMap_cra;cmx_2d_topo_JEMOccupancyMap',
                            title='CMX-Jet Topo JEM Occupancy Maps;Module;Crate', type='TH2F',
                            weight='topoJemOccMap_hit',
                            path=jetOutPath, cutmask='', **moduleVsCrate_bins)
    myGroup.defineHistogram('topoJemOccCount_mod,topoJemOccCount_cra;cmx_2d_topo_JEMOccupancyCounts',
                            title='CMX-Jet Topo JEM Occupancy Counts Weighted;Module;Crate', type='TH2F',
                            weight='topoJemOccCount_hit',
                            path=jetOutPath, cutmask='', **moduleVsCrate_bins)
    myGroup.defineHistogram('topoTobsPerJem;cmx_1d_topo_TOBsPerJEM',
                            title='CMX-Jet Topo TOBs per JEM;Number of TOBs', type='TH1F',
                            path=jetOutPath, cutmask='',
                            xbins=7, xmin=1., xmax=8., xlabels=numberLabels(7, 1))
    myGroup.defineHistogram('topoTobsPerCmx;cmx_1d_topo_TOBsPerCMX',
                            title='CMX-Jet Topo TOBs per CMX;Number of TOBs', type='TH1F',
                            path=jetOutPath, cutmask='',
                            xbins=maxTobsPerCmx, xmin=0., xmax=maxTobsPerCmx)
    # energy outputs
    myGroup.defineHistogram('cmxMissingEt_vals;cmx_1d_energy_MissingEtHits',
                            title='MissingEt Multiplicity per Threshold Standard', type='TH1F',
                            path=energyOutPath, cutmask='', weight='cmxMissingEt_hits',
                            xbins=len(xeThresholdNames), xmin=0., xmax=len(xeThresholdNames), xlabels=xeThresholdNames)
    myGroup.defineHistogram('cmxSumEt_vals;cmx_1d_energy_SumEtHits',
                            title='SumEt Multiplicity per Threshold Standard', type='TH1F',
                            path=energyOutPath, cutmask='', weight='cmxSumEt_hits',
                            xbins=len(teThresholdNames), xmin=0., xmax=len(teThresholdNames), xlabels=teThresholdNames)
    myGroup.defineHistogram('cmxMissingEtSig_vals;cmx_1d_energy_MissingEtSigHits',
                            title='MissingEtSig Multiplicity per Threshold', type='TH1F',
                            path=energyOutPath, cutmask='', weight='cmxMissingEtSig_hits',
                            xbins=len(xsThresholdNames), xmin=0., xmax=len(xsThresholdNames), xlabels=xsThresholdNames)
    myGroup.defineHistogram('cmxMissingEtHitsRes_vals;cmx_1d_energy_MissingEtHitsRes',
                            title='MissingEt Multiplicity per Threshold Restricted', type='TH1F',
                            path=energyOutPath, cutmask='', weight='cmxMissingEtHitsRes_hits',
                            xbins=len(xeThresholdNames), xmin=0., xmax=len(xeThresholdNames), xlabels=xeThresholdNames)
    myGroup.defineHistogram('cmxSumEtHitsWt_vals;cmx_1d_energy_SumEtHitsWt',
                            title='SumEt Multiplicity per Threshold Weighted', type='TH1F',
                            path=energyOutPath, cutmask='', weight='cmxSumEtHitsWt_hits',
                            xbins=len(teThresholdNames), xmin=0., xmax=len(teThresholdNames), xlabels=teThresholdNames)
    # Output Et Sums
    myGroup.defineHistogram('cmxTotalEx;cmx_1d_energy_TotalEx',
                            title='E_{x}^{CMX} Standard;Ex [GeV]', type='TH1F',
                            path=energyOutPath, cutmask='', **xySums_bins)
    myGroup.defineHistogram('cmxTotalEy;cmx_1d_energy_TotalEy',
                            title='E_{y}^{CMX} Standard;Ey [GeV]', type='TH1F',
                            path=energyOutPath, cutmask='', **xySums_bins)
    myGroup.defineHistogram('cmxTotalEt;cmx_1d_energy_TotalEt',
                            title='SumE_{t}^{CMX} Standard;Et [GeV]', type='TH1F',
                            path=energyOutPath, cutmask='', **etSums_bins)
    myGroup.defineHistogram('cmxTotalExRes;cmx_1d_energy_TotalExRes',
                            title='E_{x}^{CMX} Restricted Eta Range;Ex [GeV]', type='TH1F',
                            path=energyOutPath, cutmask='', **xySums_bins)
    myGroup.defineHistogram('cmxTotalEyRes;cmx_1d_energy_TotalEyRes',
                            title='E_{y}^{CMX} Restricted Eta Range;Ey [GeV]', type='TH1F',
                            path=energyOutPath, cutmask='', **xySums_bins)
    myGroup.defineHistogram('cmxTotalEtRes;cmx_1d_energy_TotalEtRes',
                            title='SumE_{t}^{CMX} Restricted Eta Range;Et [GeV]', type='TH1F',
                            path=energyOutPath, cutmask='', **etSums_bins)
    myGroup.defineHistogram('cmxEnergyOverflowx,cmxEnergyOverflowy,cmxEnergyOverflowz;cmx_2d_energy_Overflow',
                            title='CMX Energy Overflow Rates Standard', type='TProfile2D',
                            path=energyOutPath, cutmask='', **energyOverflow_bins)
    myGroup.defineHistogram('cmxEnergyOverflowResx,cmxEnergyOverflowResy,cmxEnergyOverflowResz;cmx_2d_energy_OverflowRes',
                            title='CMX Energy Overflow Rates Restricted Eta Range', type='TProfile2D',
                            path=energyOutPath, cutmask='', **energyOverflow_bins)

    # --------------------------- CMX output to RoI --------------------------

    # RoI Et multiplicities
    myGroup.defineHistogram('roiMissingEtHits_var;cmx_1d_roi_MissingEtHits',
                            title='MissingEt Multiplicity per Threshold Normal', type='TH1F',
                            path=roiPath, cutmask='', weight='roiMissingEtHits_hit',
                            xbins=len(xeThresholdNames), xmin=0., xmax=len(xeThresholdNames), xlabels=xeThresholdNames)
    myGroup.defineHistogram('roiSumEtHits_var;cmx_1d_roi_SumEtHits',
                            title='SumEt Multiplicity per Threshold Normal', type='TH1F',
                            path=roiPath, cutmask='', weight='roiSumEtHits_hit',
                            xbins=len(teThresholdNames), xmin=0., xmax=len(teThresholdNames), xlabels=teThresholdNames)
    myGroup.defineHistogram('roiMissingEtSigHits_var;cmx_1d_roi_MissingEtSigHits',
                            title='MissingEtSig Multiplicity per Threshold', type='TH1F',
                            path=roiPath, cutmask='', weight='roiMissingEtSigHits_hit',
                            xbins=len(xsThresholdNames), xmin=0., xmax=len(xsThresholdNames), xlabels=xsThresholdNames)
    myGroup.defineHistogram('roiMissingEtHitsMask_var;cmx_1d_roi_MissingEtHitsMask',
                            title='MissingEt Multiplicity per Threshold Masked', type='TH1F',
                            path=roiPath, cutmask='', weight='roiMissingEtHitsMask_hit',
                            xbins=len(xeThresholdNames), xmin=0., xmax=len(xeThresholdNames), xlabels=xeThresholdNames)
    myGroup.defineHistogram('roiSumEtHitsMask_var;cmx_1d_roi_SumEtHitsMask',
                            title='SumEt Multiplicity per Threshold Masked', type='TH1F',
                            path=roiPath, cutmask='', weight='roiSumEtHitsMask_hit',
                            xbins=len(teThresholdNames), xmin=0., xmax=len(teThresholdNames), xlabels=teThresholdNames)
    # RoI Et Sums
    myGroup.defineHistogram('cmxRoiEx;cmx_1d_roi_Ex',
                            title='E_{x}^{CMX} Normal;Ex [GeV]', type='TH1F',
                            path=roiPath, cutmask='', **xySums_bins)
    myGroup.defineHistogram('cmxRoiEy;cmx_1d_roi_Ey',
                            title='E_{y}^{CMX} Normal;Ey [GeV]', type='TH1F',
                            path=roiPath, cutmask='', **xySums_bins)
    myGroup.defineHistogram('cmxRoiEt;cmx_1d_roi_Et',
                            title='SumE_{t}^{CMX} Normal;Et [GeV]', type='TH1F',
                            path=roiPath, cutmask='', **etSums_bins)
    myGroup.defineHistogram('cmxRoiExMask;cmx_1d_roi_ExMask',
                            title='E_{x}^{CMX} Masked;Ex [GeV]', type='TH1F',
                            path=roiPath, cutmask='', **xySums_bins)
    myGroup.defineHistogram('cmxRoiEyMask;cmx_1d_roi_EyMask',
                            title='E_{y}^{CMX} Masked;Ey [GeV]', type='TH1F',
                            path=roiPath, cutmask='', **xySums_bins)
    myGroup.defineHistogram('cmxRoiEtMask;cmx_1d_roi_EtMask',
                            title='SumE_{t}^{CMX} Masked;Et [GeV]', type='TH1F',
                            path=roiPath, cutmask='', **etSums_bins)

    # ---------------------------- S-Link errors - ----------------------------

    myGroup.defineHistogram('cmxThreshStatusErr_bit,cmxThreshStatusErr_crate;cmx_2d_thresh_Status',
                            title='Errors from CMX Jet SubStatus Word', type='TH2F',
                            path=errHardPath, cutmask='', **subStatusVsCrate_bins)
    myGroup.defineHistogram('cmxEnergyStatus_bit,cmxEnergyStatus_cra;cmx_2d_energy_Status',
                            title='Errors from CMX Energy SubStatus Word', type='TH2F',
                            path=errHardPath, cutmask='', **subStatusVsCrate_bins)
    myGroup.defineHistogram('threshParityTobJem,threshParityTobCrate;cmx_2d_thresh_ParityTob',
                            title='CMX Jet TOB Parity Errors;Module;Crate', type='TH2F',
                            path=errHardPath, cutmask='', **moduleVsCrate_bins)
    myGroup.defineHistogram('cmxThreshParityErr;cmx_1d_thresh_ParityRem',
                            title='CMX Jet Remote Hit Sums Parity Errors;Cable/Phase', type='TH1F',
                            path=errHardPath, cutmask='',
                            xbins=4, xmin=0., xmax=4., xlabels=numberPairs(1,1))
    myGroup.defineHistogram('cmxEnergyParityModx,cmxEnergyParityMody;cmx_2d_energy_ParityMod',
                            title='CMX Energy Module Sums Parity Errors;Crate/Module;Type/Phase', type='TH2F',
                            path=errHardPath, cutmask='',
                            xbins=19, xmin=0., xmax=19., xlabels=numberPairs(1, 19, step=2),
                            ybins=3, ymin=0., ymax=3., ylabels=['Ex/0', 'Ey/1', 'Et/2'])
    myGroup.defineHistogram('cmxParityRem;cmx_1d_energy_ParityRem',
                            title='CMX Energy Remote Crate Sums Parity Errors;Cable/Phase', type='TH1F',
                            path=errHardPath, cutmask='',
                            xbins=4, xmin=0., xmax=4., xlabels=numberPairs(1, 1))
    myGroup.defineHistogram('cmxTriggeredSlice;cmx_1d_TriggeredSlices',
                            title='Comparison of CMX Jet and Energy triggered slice numbers;Difference', type='TH1F',
                            path=errHardPath, cutmask='',
                            xbins=5, xmin=0., xmax=5., xlabels=numberLabels(5))
    # error summary for all CMXs in system
    myGroup.defineHistogram('cmxErrorSummary;cmx_1d_ErrorSummary',
                            title='Error Summary of CMX Jet and Energy', type='TH1F',
                            path=errHardPath, cutmask='',
                            xbins=
NumberOfSummaryBins, xmin=0., xmax=NumberOfSummaryBins, xlabels=errSummary_labels)
    
    myGroup.defineHistogram(';cmx_2d_ErrorEventNumbers',
                            title='JEM-CMX Error Event Numbers;Events with Error/Mismatch;',type='TH2I',
                            cutmask='', path=errHardPath,
                            xbins=EventSamples, xmin=0, xmax=EventSamples, xlabels=numberLabels(EventSamples, 1),
                            ybins=NumberOfSummaryBins, ymin=0, ymax=NumberOfSummaryBins, ylabels=errSummary_labels)

    acc = helper.result()
    result.merge(acc)
    return result


def numberLabels(num, start=0, step=1):
    """Generate numeric labels"""
    return [str(i + start) for i in range(0, num, step)]


def numberPairs(firstMax, secondMax, firstMin=0, secondMin=0, step=1, offset=0):
    """Generate number pair axis labels"""
    labels = [''] * firstMax * secondMax
    numSecond = secondMax - secondMin
    for first in range(firstMin, firstMax):
        bin = offset + first * numSecond
        for second in range(secondMin, secondMax, step):
            labels[bin] = '{}/{}'.format(first, second)
            bin += step
    return labels


def thresholdLabels(menu, thresholdType):
    """Get ordered threshold names from L1 menu for threshold type"""
    return [name for name, _ in sorted(menu.thresholds(thresholdType).items(), key=lambda x: x[1]['mapping'])]


if __name__=='__main__':
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
    ConfigFlags.Output.HISTFileName = 'JepCmxMonitorOutput_LVL1.root'

    ConfigFlags.lock()
    ConfigFlags.dump() # print all the configs

    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr.Dump = False

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    JepCmxMonitorCfg = JepCmxMonitoringConfig(ConfigFlags)
    cfg.merge(JepCmxMonitorCfg)

    # message level for algorithm
    JepCmxMonitorCfg.getEventAlgo('JepCmxMonAlg').OutputLevel = 2 # 1/2 INFO/DEBUG
    # options - print all details of algorithms, very short summary 
    cfg.printConfig(withDetails=False, summariseProps = True)

    nevents=-1
    cfg.run(nevents)

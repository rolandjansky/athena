#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

#from AthenaCommon.Include import include #needed for LArSuperCellBCIDEmAlg
#from AthenaCommon.AppMgr import ServiceMgr as svcMgr

def LArSuperCellMonConfigOld(inputFlags):
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import  LArSuperCellMonAlg

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArSuperCellMonAlgOldCfg')
    from AthenaCommon.BeamFlags import jobproperties
    if jobproperties.Beam.beamType() == 'cosmics':
       isCosmics=True
    else:
       isCosmics=False

    from AthenaCommon.GlobalFlags  import globalflags
    if globalflags.DataSource() == 'data':
       isMC=False
    else:
       isMC=True

    if not isMC:
        from LumiBlockComps.LBDurationCondAlgDefault import LBDurationCondAlgDefault
        LBDurationCondAlgDefault()
        from LumiBlockComps.TrigLiveFractionCondAlgDefault import TrigLiveFractionCondAlgDefault
        TrigLiveFractionCondAlgDefault()
        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgDefault
        LuminosityCondAlgDefault()

    from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
    CaloNoiseCondAlg()

    algo = LArSuperCellMonConfigCore(helper, LArSuperCellMonAlg,inputFlags,isCosmics, isMC)

    from AthenaMonitoring.AtlasReadyFilterTool import GetAtlasReadyFilterTool
    algo.ReadyFilterTool = GetAtlasReadyFilterTool()
    from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
    algo.BadLBTool = GetLArBadLBFilterTool()

    return helper.result()

def LArSuperCellMonConfig(inputFlags):
    from AthenaCommon.Logging import logging
    mlog = logging.getLogger( 'LArSuperCellMonConfig' )

    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArSuperCellMonAlgCfg')

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    cfg=ComponentAccumulator()

    if not inputFlags.DQ.enableLumiAccess:
       mlog.warning('This algo needs Lumi access, returning empty config')
       return cfg

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg.merge(LArGMCfg(inputFlags))
    from TileGeoModel.TileGMConfig import TileGMCfg
    cfg.merge(TileGMCfg(inputFlags))

    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    cfg.merge(DetDescrCnvSvcCfg(inputFlags))

    from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
    cfg.merge(LArCollisionTimeCfg(inputFlags))

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    cfg.merge(CaloNoiseCondAlgCfg(inputFlags))
    cfg.merge(CaloNoiseCondAlgCfg(inputFlags,noisetype="electronicNoise"))

    from LArROD.LArSCSimpleMakerConfig import LArSCSimpleMakerCfg, LArSuperCellBCIDEmAlgCfg
    cfg.merge(LArSCSimpleMakerCfg(inputFlags))
    cfg.merge(LArSuperCellBCIDEmAlgCfg(inputFlags))

    from AthenaConfiguration.ComponentFactory import CompFactory
    lArCellMonAlg=CompFactory.LArSuperCellMonAlg


    if inputFlags.Input.isMC is False:
       from LumiBlockComps.LuminosityCondAlgConfig import  LuminosityCondAlgCfg
       cfg.merge(LuminosityCondAlgCfg(inputFlags))
       from LumiBlockComps.LBDurationCondAlgConfig import  LBDurationCondAlgCfg
       cfg.merge(LBDurationCondAlgCfg(inputFlags))


    algname='LArSuperCellMonAlg'
    if inputFlags.Beam.Type == 'cosmics':
        algname=algname+'Cosmics'

    isCosmics = ( inputFlags.Beam.Type == 'cosmics' )
    algo = LArSuperCellMonConfigCore(helper, lArCellMonAlg  ,inputFlags, isCosmics, inputFlags.Input.isMC,  algname)

    #if not inputFlags.Input.isMC:
    #   from AthenaMonitoring.BadLBFilterToolConfig import LArBadLBFilterToolCfg
    #   algo.BadLBTool=cfg.popToolsAndMerge(LArBadLBFilterToolCfg(inputFlags))
    print("Check the Algorithm properties",algo)

    cfg.merge(helper.result())

    return cfg


def LArSuperCellMonConfigCore(helper, algclass, inputFlags, isCosmics=False, isMC=False, algname='LArSuperCellMonAlg'):


    LArSuperCellMonAlg = helper.addAlgorithm(algclass, algname)


    GroupName="LArSuperCellMon"
    LArSuperCellMonAlg.MonGroupName = GroupName

    LArSuperCellMonAlg.EnableLumi = True
    

    do2DOcc = True #TMP
    print(do2DOcc)



   #---single Group for non threshold histograms
    cellMonGroup = helper.addGroup(
        LArSuperCellMonAlg,
        GroupName,
        '/LAr/LArSuperCellMon_NoTrigSel/'

    )


    #--define histograms
    sc_hist_path='SC/'


    cellMonGroup.defineHistogram('superCellEt;h_SuperCellEt',
                                 title='Super Cell E_T [MeV]; MeV; # entries',
                                 type='TH1F', path=sc_hist_path,
                                 xbins =  100,xmin=0,xmax=50000)
    cellMonGroup.defineHistogram('superCellEta;h_SuperCellEta',
                                 title='Super Cell eta; #eta; # entries',
                                 type='TH1F', path=sc_hist_path,
                                 xbins =  100,xmin=-5,xmax=5)
    cellMonGroup.defineHistogram('superCelltime;h_SuperCelltime',
                                 title='Super Cell time [ns]; ns; # entries',
                                 type='TH1F', path=sc_hist_path,
                                 xbins = 100, xmin=-400,xmax=400)
    cellMonGroup.defineHistogram('superCellprovenance;h_SuperCellprovenance',
                                 title='Super Cell provenance; bitmask ; # entries',
                                 type='TH1F', path=sc_hist_path,
                                 xbins = 700, xmin=0,xmax=700)
    cellMonGroup.defineHistogram('BCID,superCellEt;h_SuperCellEt_vs_BCID',
                                 title='Super Cell ET [MeV] vs BCID ; BCID from train front; %',
                                 type='TH2F', path=sc_hist_path,
                                 xbins = 50, xmin=0,xmax=50,
                                 ybins = 80, ymin=-1000,ymax=1000)
    cellMonGroup.defineHistogram('BCID,superCellEtRef;h_SuperCellEtRef_vs_BCID',
                                 title='Super Cell ET [MeV] vs BCID ; BCID from train front; %',
                                 type='TH2F', path=sc_hist_path,
                                 xbins = 50, xmin=0,xmax=50,
                                 ybins = 80, ymin=-1000,ymax=1000)

    cellMonGroup.defineHistogram('resolution;h_SuperCellResolution',
                                 title='Super Cell reconstruction resolution ; %; # entries',
                                 type='TH1F', path=sc_hist_path,
                                 xbins = 70, xmin=-20,xmax=120)
    cellMonGroup.defineHistogram('resolutionPass;h_SuperCellResolutionPass',
                                 title='Super Cell reconstruction resolution for BCIDed ; %; # entries',
                                 type='TH1F', path=sc_hist_path,
                                 xbins = 70, xmin=-20,xmax=120)
    cellMonGroup.defineHistogram('superCellEt,resolution;h_SuperCellResolution_vs_ET',
                                 title='Super Cell reconstruction resolution vs ET ; [MeV]; %',
                                 type='TH2F', path=sc_hist_path,
                                 xbins = 100, xmin=0,xmax=50000,
                                 ybins = 70, ymin=-20,ymax=120)
    cellMonGroup.defineHistogram('superCellEta,resolutionHET;h_SuperCellResolution_vs_eta',
                                 title='Super Cell reconstruction resolution vs #eta ; #eta; %',
                                 type='TH2F', path=sc_hist_path,
                                 xbins = 100, xmin=-5,xmax=5,
                                 ybins = 40, ymin=-20,ymax=20)
    cellMonGroup.defineHistogram('BCID,resolution;h_SuperCellResolution_vs_BCID',
                                 title='Super Cell reconstruction resolution vs BCID ; BCID from train front; %',
                                 type='TH2F', path=sc_hist_path,
                                 xbins = 50, xmin=0,xmax=50,
                                 ybins = 80, ymin=-120,ymax=120)

    cellMonGroup.defineHistogram('superCellEtRef,superCellEt;h_SuperCellEtLin',
                                 title='Super Cell E_T Linearity; Ref SC E_T [MeV]; SC E_T [MeV]',
                                 type='TH2F', path=sc_hist_path,
                                 xbins =  100,xmin=0,xmax=50000,
                                 ybins =  100,ymin=0,ymax=50000)
    cellMonGroup.defineHistogram('superCelltimeRef,superCelltime;h_SuperCelltimeLin',
                                 title='Super Cell time Linearity; Ref SC time [ns]; SC time [ns]',
                                 type='TH2F', path=sc_hist_path,
                                 xbins = 100, xmin=-200,xmax=200,
                                 ybins = 100, ymin=-200,ymax=200)
    cellMonGroup.defineHistogram('superCellprovenanceRef,superCellprovenance;h_SuperCellprovenanceLin',
                                 title='Super Cell provenance Linearity; Ref SC bitmask ; SC bitmask',
                                 type='TH2F', path=sc_hist_path,
                                 xbins = 17, xmin=0,xmax=680,
                                 ybins = 17, ymin=0,ymax=680)
    cellMonGroup.defineHistogram('BCID;h_BCID',
                                 title='BCID from the front of the train; BCID ; # entries',
                                 type='TH1F', path=sc_hist_path,
                                 xbins = 120, xmin=0,xmax=120)

    sc_hist_path='SC_Layer/'
    for part in LArSuperCellMonAlg.LayerNames:
           partp='('+part+')'
           cellMonGroup.defineHistogram('superCellEt_'+part+';h_SuperCellEt'+part,
                                        title='Super Cell E_T [MeV] '+partp+'; MeV; # entries',
                                        type='TH1F', path=sc_hist_path,
                                        xbins =  100,xmin=0,xmax=50000)
           cellMonGroup.defineHistogram('superCellEta_'+part+';h_SuperCellEta'+part,
                                        title='Super Cell eta '+partp+'; #eta; # entries',
                                        type='TH1F', path=sc_hist_path,
                                        xbins =  100,xmin=-5,xmax=5)
           cellMonGroup.defineHistogram('superCelltime_'+part+';h_SuperCelltime'+part,
                                        title='Super Cell time [ns] '+partp+'; ns; # entries',
                                        type='TH1F', path=sc_hist_path,
                                        xbins = 100, xmin=-400,xmax=400)
           cellMonGroup.defineHistogram('superCellprovenance_'+part+';h_SuperCellprovenance'+part,
                                        title='Super Cell provenance '+partp+'; bitmask ; # entries',
                                        type='TH1F', path=sc_hist_path,
                                        xbins = 700, xmin=0,xmax=700)
           cellMonGroup.defineHistogram('BCID,superCellEt_'+part+';h_SuperCellET_vs_BCID'+part,
                                        title='Super Cell ET [MeV] vs BCID '+partp+'; BCID from train front; %',
                                        type='TH2F', path=sc_hist_path,
                                        xbins = 50, xmin=0,xmax=50,
                                        ybins = 100, ymin=-1000,ymax=1000)
           cellMonGroup.defineHistogram('BCID,superCellEtRef_'+part+';h_SuperCellRefET_vs_BCID'+part,
                                        title='Super Cell ET [MeV] vs BCID '+partp+'; BCID from train front; %',
                                        type='TH2F', path=sc_hist_path,
                                        xbins = 50, xmin=0,xmax=50,
                                        ybins = 100, ymin=-1000,ymax=1000)
        
           cellMonGroup.defineHistogram('resolution_'+part+';h_SuperCellResolution'+part,
                                        title='Super Cell reconstruction resolution '+partp+'; %; # entries',
                                        type='TH1F', path=sc_hist_path,
                                        xbins = 70, xmin=-20,xmax=120)
           cellMonGroup.defineHistogram('resolutionPass_'+part+';h_SuperCellResolutionPass'+part,
                                        title='Super Cell reconstruction resolution for BCIDed '+partp+'; %; # entries',
                                        type='TH1F', path=sc_hist_path,
                                        xbins = 70, xmin=-20,xmax=120)
           cellMonGroup.defineHistogram('superCellEt_'+part+',resolution_'+part+';h_SuperCellResolution_vs_ET'+part,
                                        title='Super Cell reconstruction resolution vs ET '+partp+'; [MeV]; %',
                                        type='TH2F', path=sc_hist_path,
                                        xbins = 100, xmin=0,xmax=50000,
                                        ybins = 70, ymin=-20,ymax=120)
           cellMonGroup.defineHistogram('superCellEta_'+part+',resolutionHET_'+part+';h_SuperCellResolution_vs_eta'+part,
                                        title='Super Cell reconstruction resolution vs #eta  '+partp+'; #eta; %',
                                        type='TH2F', path=sc_hist_path,
                                        xbins = 100, xmin=-5,xmax=5,
                                        ybins = 40, ymin=-20,ymax=20)
           cellMonGroup.defineHistogram('BCID,resolution_'+part+';h_SuperCellResolution_vs_BCID'+part,
                                        title='Super Cell reconstruction resolution vs BCID '+partp+'; BCID from train front; %',
                                        type='TH2F', path=sc_hist_path,
                                        xbins = 50, xmin=0,xmax=50,
                                        ybins = 80, ymin=-120,ymax=120)
        
           cellMonGroup.defineHistogram('superCellEtRef_'+part+',superCellEt_'+part+';h_SuperCellEtLin'+part,
                                        title='Super Cell E_T Linearity '+partp+'; Ref SC E_T [MeV]; SC E_T [MeV]',
                                        type='TH2F', path=sc_hist_path,
                                        xbins =  100,xmin=0,xmax=50000,
                                        ybins =  100,ymin=0,ymax=50000)
           cellMonGroup.defineHistogram('superCelltimeRef_'+part+',superCelltime_'+part+';h_SuperCelltimeLin'+part,
                                        title='Super Cell time Linearity '+partp+'; Ref SC time [ns]; SC time [ns]',
                                        type='TH2F', path=sc_hist_path,
                                        xbins = 100, xmin=-200,xmax=200,
                                        ybins = 100, ymin=-200,ymax=200)
           cellMonGroup.defineHistogram('superCellprovenanceRef_'+part+',superCellprovenance_'+part+';h_SuperCellprovenanceLin'+part,
                                        title='Super Cell provenance Linearity '+partp+'; Ref SC bitmask ; SC bitmask',
                                        type='TH2F', path=sc_hist_path,
                                        xbins = 17, xmin=0,xmax=680,
                                        ybins = 17, ymin=0,ymax=680)


           cellMonGroup.defineHistogram('cellEnergy_'+part+';CellEnergy_'+part,
                                        title='Cell Energy in ' +part+';Cell Energy [MeV];Cell Events',
                                        type='TH1F', path=sc_hist_path,
                                        xbins =  100,xmin=0,xmax=50000
                                        )
    LArSuperCellMonAlg.doDatabaseNoiseVsEtaPhi = True

    for part in LArSuperCellMonAlg.LayerNames:        
        
        cellMonGroup.defineHistogram('celleta_'+part+';NCellsActiveVsEta_'+part,
                                           title="No. of Active Cells in #eta for "+part+";cell #eta",
                                           type='TH1F', path=sc_hist_path,
                                           xbins = 100,xmin=-5,xmax=5
                                           )
        
        cellMonGroup.defineHistogram('cellphi_'+part+';NCellsActiveVsPhi_'+part,
                                           title="No. of Active Cells in #phi for "+part+";cell #phi",
                                           type='TH1F', path=sc_hist_path,
                                           xbins =  100,xmin=-5,xmax=5
                                           )

        cellMonGroup.defineHistogram('celleta_'+part+',cellphi_'+part+';DatabaseNoiseVsEtaPhi_'+part,
                                           title="Map of Noise Values from the Database vs (#eta,#phi) for "+part+";cell #eta;cell #phi",
                                           weight='cellnoisedb_'+part,
                                           cutmask='doDatabaseNoisePlot',
                                           type='TH2F', path="DatabaseNoise/", 
                                           xbins =  100,xmin=-5,xmax=5,
                                           ybins =  100,ymin=-5,ymax=5,
                                           merge='weightedAverage')
        


    return LArSuperCellMonAlg


if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Constants import WARNING
    from AthenaCommon.Logging import log
    log.setLevel(DEBUG)

    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    #from AthenaConfiguration.TestDefaults import defaultTestFiles
    #ConfigFlags.Input.Files = defaultTestFiles.ESD
    #ConfigFlags.Input.Files = ['/afs/cern.ch/work/l/lily/LAr/Rel22Dataset/valid1.345058.PowhegPythia8EvtGen_NNPDF3_AZNLO_ggZH125_vvbb.recon.ESD.e6004_e5984_s3126_d1623_r12488/ESD.24607649._000024.pool.root.1']
    ConfigFlags.Input.Files = ['/home/pavol/valid1/ESD.24607649._000024.pool.root.1']
    ConfigFlags.Input.Files = ['/tmp/damazio/SCMon/ESD.24607649._000024.pool.root.1']
    #ConfigFlags.Input.Files = ['/eos/atlas/user/b/bcarlson/Run3Tmp/data18_13TeV.00360026.physics_EnhancedBias.recon.ESD.r10978_r11179_r11185/ESD.16781883._001043.pool.root.1']
    # to test tier0 workflow:
    #ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/data15_13TeV.00278748.physics_ZeroBias.merge.RAW._lb0384._SFO-ALL._0001.1']

    #ConfigFlags.Calo.Cell.doPileupOffsetBCIDCorr=True
    ConfigFlags.Output.HISTFileName = 'LArSuperCellMonOutput.root'
    ConfigFlags.DQ.enableLumiAccess = True
    ConfigFlags.DQ.useTrigger = False
    ConfigFlags.DQ.Environment = 'tier0'
    ConfigFlags.lock()


    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    cfg = MainServicesCfg(ConfigFlags)
    print(cfg)
    print(dir(cfg))
    print(cfg.getServices())
    storeGateSvc = cfg.getService("StoreGateSvc")
    storeGateSvc.Dump=True
    print(storeGateSvc)

    # in case of tier0 workflow:
    #from CaloRec.CaloRecoConfig import CaloRecoCfg
    #cfg.merge(CaloRecoCfg(ConfigFlags))

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
    cfg.merge(BunchCrossingCondAlgCfg(ConfigFlags))

    cfg.merge(LArSuperCellMonConfig(ConfigFlags)) 

    f=open("LArSuperCellMon.pkl","wb")
    cfg.store(f)
    f.close()

    cfg.run(200,OutputLevel=WARNING) #use cfg.run() to run on all events

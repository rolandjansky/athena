#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory

def CaloBaselineMonConfig(inputFlags, isTopLevel=True):

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'CaloBaselineMonCfg')

    if not inputFlags.DQ.enableLumiAccess:
       print('This algo needs Lumi access, returning empty config')
       if isTopLevel:
          from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
          cfg=ComponentAccumulator()
          cfg.merge(helper.result())
          return cfg
       else:   
          return helper.result()

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg = LArGMCfg(inputFlags)
    from TileGeoModel.TileGMConfig import TileGMCfg
    cfg.merge(TileGMCfg(inputFlags))
    from LArCellRec.LArCollisionTimeConfig import LArCollisionTimeCfg
    cfg.merge(LArCollisionTimeCfg(inputFlags))
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    cfg.merge(CaloNoiseCondAlgCfg(inputFlags))

    caloBaselineMonAlg = helper.addAlgorithm(CompFactory.CaloBaselineMonAlg,'caloBaselineMonAlg')

    GroupName="CaloBaseLineMon"
    caloBaselineMonAlg.MonGroupName = GroupName

    caloBaselineMonAlg.EnableLumi = True
    
    partList = ["EM","HEC+FCal"]
    caloBaselineMonAlg.partionList = partList
    etaBins = [16,19]
    caloBaselineMonAlg.nbOfEtaBins = etaBins
    minEta = [0.,1.2]
    caloBaselineMonAlg.minimumEta = minEta
    maxEta = [3.2,5.]
    caloBaselineMonAlg.maximumEta = maxEta

    # config settings based on flags
    tmp_CaloBaselineMon = {"useBadLBTool":False,
                        "useReadyFilterTool":False,
                        "useLArNoisyAlg":False,
                        "useBeamBackgroundRemoval":False,
                        "useLArCollisionFilter":False,
                        "pedestalMon_BCIDmin":0,
                        "bcidtoolMon_BCIDmax":0}
    binlabels=["TotalEvents","ATLAS Ready","with Good LAr LB","with No LAr Collision","with No Beam Background", "with No Trigger Filter","with No LArError"] 
    if not (inputFlags.Common.isOnline == 'online' or inputFlags.Input.isMC ):
      tmp_CaloBaselineMon["useBadLBTool"]=True
      tmp_CaloBaselineMon["useReadyFilterTool"]=True
      tmp_CaloBaselineMon["useLArNoisyAlg"] = True
    
    # FIXME when trigger stream flag is added:
    #if rec.triggerStream()=='CosmicCalo':
    #  tmp_CaloBaselineMon["useLArCollisionFilter"] = True
    #  tmp_CaloBaselineMon["pedestalMon_BCIDmin"] = 40
    #  tmp_CaloBaselineMon["TriggerChain"] = "HLT_noalg_cosmiccalo_L1RD1_EMPTY"
    
    #if rec.triggerStream()=='ZeroBias':
    tmp_CaloBaselineMon["bcidtoolMon_BCIDmax"] = 144
    #tmp_CaloBaselineMon["TriggerChain"] = "HLT_noalg_zb_L1ZB"
    tmp_CaloBaselineMon["TriggerChain"] = ""

    from AthenaMonitoring.AtlasReadyFilterConfig import AtlasReadyFilterCfg
    from AthenaMonitoring.BadLBFilterToolConfig import LArBadLBFilterToolCfg

    caloBaselineMonAlg.useBadLBTool = tmp_CaloBaselineMon["useBadLBTool"]
    caloBaselineMonAlg.BadLBTool = cfg.popToolsAndMerge(LArBadLBFilterToolCfg(inputFlags))
    # FIXME Do not have yet new config for BunchCrossingTool, shoulkd be put back once available
    #caloBaselineMonAlg.BunchCrossingTool = BunchCrossingTool("TrigConf" if not inputFlags.Input.isMC else "MC")
    caloBaselineMonAlg.useReadyFilterTool = tmp_CaloBaselineMon["useReadyFilterTool"]
    caloBaselineMonAlg.ReadyFilterTool = cfg.popToolsAndMerge(AtlasReadyFilterCfg(inputFlags))
    caloBaselineMonAlg.useLArCollisionFilterTool = tmp_CaloBaselineMon["useLArCollisionFilter"]
    caloBaselineMonAlg.useLArNoisyAlg = tmp_CaloBaselineMon["useLArNoisyAlg"]
    caloBaselineMonAlg.useBeamBackgroundRemoval = tmp_CaloBaselineMon["useBeamBackgroundRemoval"]
    caloBaselineMonAlg.pedestalMon_BCIDmin = tmp_CaloBaselineMon["pedestalMon_BCIDmin"]
    caloBaselineMonAlg.bcidtoolMon_BCIDmax = tmp_CaloBaselineMon["bcidtoolMon_BCIDmax"]
    caloBaselineMonAlg.TriggerChain = tmp_CaloBaselineMon["TriggerChain"]
    if not caloBaselineMonAlg.useReadyFilterTool:
       binlabels[1] = "ATLAS Ready-OFF"
    if not caloBaselineMonAlg.useBadLBTool:
       binlabels[2] = "Good LAr LB-OFF"
    if not caloBaselineMonAlg.useLArCollisionFilterTool:
       binlabels[3] = "LAr collision-OFF"
    if not caloBaselineMonAlg.useBeamBackgroundRemoval:
       binlabels[4] = "Beam backgr.-OFF"
    if not caloBaselineMonAlg.useLArNoisyAlg:
       binlabels[5] = "LAr Error Veto-OFF"   

    # eta bins computation (should be tha same as in C++ code
    etaBinWidth = [None] * len(partList)
    for i in range(0,len(partList)):
        etaBinWidth[i] = (maxEta[i] - minEta[i]) / etaBins[i]
    # bool to decide which monitoring to do
    if caloBaselineMonAlg.pedestalMon_BCIDmin > 0:
      doPedestalMon = True
    else:  
      doPedestalMon = False
    if caloBaselineMonAlg.bcidtoolMon_BCIDmax > 0:
      doBcidtoolMon = True
    else:  
      doBcidtoolMon = False

    baselineGroup = helper.addGroup(
        caloBaselineMonAlg,
        GroupName,
        '/CaloMonitoring/'+GroupName+'/'
    )

    gen_hist_path='General/'

    from CaloMonitoring.CaloMonAlgBase import CaloBaseHistConfig
    CaloBaseHistConfig(baselineGroup,gen_hist_path,binlabels)  

    BCID0_nbins=3563
    LB_nbins=3000

    baselineGroup.defineHistogram('BCID;h1BCID_pedestalMon',
                                  title='BCID used for baseline monitoring;BCID;Nb of events / BCID',
                                  type='TH1I', path=gen_hist_path,
                                  xbins=BCID0_nbins+1, xmin=-0.5, xmax=BCID0_nbins+0.5)

    baselineGroup.defineHistogram('BCID;h1BCID_BCIDToolMon',
                                  title='BCID used for BCIDTool monitoring;BCID;Nb of events / BCID',
                                  type='TH1I', path=gen_hist_path,
                                  xbins=BCID0_nbins+1, xmin=-0.5, xmax=BCID0_nbins+0.5)

    part_hist_path='AllCalo'+tmp_CaloBaselineMon["TriggerChain"]+'/'
    idx=0
    for part in partList:
       if doPedestalMon:
         str_auxTitle = " Empty BCID > "+str(tmp_CaloBaselineMon["pedestalMon_BCIDmin"])+"BCID away from last train"
 
         baselineGroup.defineHistogram('pedEta_'+part+',sumPedEta_'+part+';hprof1d_pedestalMon_'+part+'_AllEta',
                           title='Pedestal baseline ( '+str_auxTitle+');Eta;E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]',
                           type='TProfile', path=part_hist_path,
                           xbins=etaBins[idx], xmin=minEta[idx], xmax=maxEta[idx])
 
         baselineGroup.defineHistogram('LB_'+part+',sumPedEta_'+part+';hprof1d_pedestalMon_'+part+'_LB',
                           title='Pedestal baseline ( '+str_auxTitle+');Luminosity block;E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]',
                           type='TProfile', path=part_hist_path,
                           xbins=LB_nbins, xmin=0, xmax=LB_nbins)
       if doBcidtoolMon:
         str_auxTitle = " BCID in bunch train  "
         baselineGroup.defineHistogram('bcidEta_'+part+',sumBCIDEta_'+part+';hprof1d_bcidtoolMon_'+part+'_AllEta',
                           title='BCIDTool baseline ( '+str_auxTitle+');Eta;E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]',
                           type='TProfile', path=part_hist_path,
                           xbins=etaBins[idx], xmin=minEta[idx], xmax=maxEta[idx])
 
         baselineGroup.defineHistogram('LB_'+part+',sumBCIDEta_'+part+';hprof1d_bcidtoolMon_'+part+'_LB',
                           title='BCIDTool baseline ( '+str_auxTitle+');Luminosity block;E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]',
                           type='TProfile', path=part_hist_path,
                           xbins=LB_nbins, xmin=0, xmax=LB_nbins)

    part_hist_path='/CaloMonitoring/'+GroupName+'/AllCalo'+tmp_CaloBaselineMon["TriggerChain"]+'/'
    idx=0
    for part in partList:
       darray = helper.addArray([etaBins[idx]],caloBaselineMonAlg,part)
       if doPedestalMon:
         str_auxTitle = " Empty BCID > "+str(tmp_CaloBaselineMon["pedestalMon_BCIDmin"])+"BCID away from last train"

         darray.defineHistogram('etaBCID_'+part+',sumPedEta_'+part+';hprof_pedestalMon_'+part,
                           title='Pedestal baseline ( '+str_auxTitle+');Luminosity block;E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]',
                           type='TProfile', path=part_hist_path,
                           xbins=BCID0_nbins+1, xmin=-0.5, xmax=BCID0_nbins+0.5)
       if doBcidtoolMon:
         str_auxTitle = " BCID in bunch train  "

         darray.defineHistogram('etaBCID_'+part+',sumBCIDEta_'+part+';hprof_bcidtoolMon_'+part,
                           title='BCIDTool baseline ( '+str_auxTitle+');Luminosity block;E_T/(#Delta#eta.#Delta#phi.#mu)[MeV]',
                           type='TProfile', path=part_hist_path,
                           xbins=BCID0_nbins+1, xmin=-0.5, xmax=BCID0_nbins+0.5)
       idx=idx+1


    #if isTopLevel:
    cfg.merge(helper.result())
    return cfg
    #else:   
    #   return helper.result()
    

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
    ConfigFlags.Input.Files = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/data15_13TeV.00278748.physics_ZeroBias.merge.RAW._lb0384._SFO-ALL._0001.1']
    #ConfigFlags.Input.isMC = True
    ConfigFlags.Output.HISTFileName = 'CaloBaselineMonOutput.root'
    ConfigFlags.DQ.enableLumiAccess = True
    ConfigFlags.DQ.useTrigger = True
    ConfigFlags.Calo.Cell.doPileupOffsetBCIDCorr=True
    ConfigFlags.lock()


    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    cfg = MainServicesCfg(ConfigFlags)

    #from CaloRec.CaloRecoConfig import CaloRecoCfg
    #cfg.merge(CaloRecoCfg(ConfigFlags))

    cfg.merge(CaloBaselineMonConfig(ConfigFlags,False)) 

    cfg.run(10,OutputLevel=WARNING) #use cfg.run() to run on all events

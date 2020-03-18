#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

def LArNoisyROMonConfig(inputFlags, inKey="", NoisyFEBDefStr="(>30 chan with Q>4000)"):

    # first configure known bad FEBs
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    cfg=ComponentAccumulator()
    from LArBadChannelTool.LArBadFebsConfig import LArKnownBadFebCfg, LArKnownMNBFebCfg
    cfg.merge(LArKnownBadFebCfg(inputFlags))
    cfg.merge(LArKnownMNBFebCfg(inputFlags))

    from AthenaMonitoring import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArNoisyROMonCfg')


    from LArMonitoring.LArMonitoringConf import LArNoisyROMonAlg
    larNoisyROMonAlg = helper.addAlgorithm(LArNoisyROMonAlg,'larNoisyROMonAlg')

    NoisyROMonGroupName="LArNoisyROMonGroup"

    from LArMonitoring.GlobalVariables import lArDQGlobals

    larNoisyROMonAlg.NoisyROGroupName=NoisyROMonGroupName
    larNoisyROMonAlg.SubDetNames=lArDQGlobals.SubDet[0:2]
    larNoisyROMonAlg.PartitionNames=lArDQGlobals.Partitions[0:4]

    #FIXME: only for testing
    larNoisyROMonAlg.storeLooseMNBFEBs=True
    if inKey != "":
       larNoisyROMonAlg.inputKey=inKey 


    noisyROGroup = helper.addGroup(
        larNoisyROMonAlg,
        NoisyROMonGroupName,
        '/LAr/',
        'run'
    )


    larNoisyRO_hist_path='NoisyRONewAlg/'

    # first trees
    noisyROGroup.defineTree('time,time_ns,algo;LArNoise',path=larNoisyRO_hist_path,
                            treedef='time/i:time_ns/i:algo/b')

    noisyROGroup.defineTree('candidate_MNB_time,candidate_MNB_time_ns,n_candidate_MNBTight_FEB,n_candidate_MNBTight_PsVeto_FEB,n_candidate_MNBLoose_FEB,v_candidate_MNBTightFEB,v_candidate_MNBTight_PsVetoFEB,v_candidate_MNBLooseFEB;CandidateMNBFebs',path=larNoisyRO_hist_path,
                            treedef='candidate_MNB_time/i:candidate_MNB_time_ns/i:n_candidate_MNBTight_FEB/i:n_candidate_MNBTight_PsVeto_FEB/i:n_candidate_MNBLoose_FEB/i:v_candidate_MNBTightFEB/vector<int>:v_candidate_MNBTight_PsVetoFEB/vector<int>:v_candidate_MNBLooseFEB/vector<int>')

    #then global histo
    noisyROGroup.defineHistogram('n_noisyFEBs;NoisyFEB',
                                  title='Number of noisy FEB '+ NoisyFEBDefStr + '  per event;# of noisy FEB',
                                  type='TH1I',
                                  path=larNoisyRO_hist_path,
                                  xbins=lArDQGlobals.noisyFEB_Bins,xmin=lArDQGlobals.noisyFEB_Min,xmax=lArDQGlobals.noisyFEB_Max)
    noisyROGroup.defineHistogram('LBN;LBN',type='TH1I',
                                 title='Event counter per LB', path=larNoisyRO_hist_path,
                                 xbins=lArDQGlobals.LB_Bins,xmin=lArDQGlobals.LB_Min,xmax=lArDQGlobals.LB_Max)

    for subdet in range(0,2): 
       hist_path='/LAr/'+larNoisyRO_hist_path+lArDQGlobals.SubDet[subdet]+'/'
       slot_low = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       slot_up  = lArDQGlobals.FEB_Slot[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       slot_n = int(slot_up - slot_low)
       ft_low = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][0] - 0.5
       ft_up  = lArDQGlobals.FEB_Feedthrough[lArDQGlobals.Partitions[subdet*2]][1] + 0.5
       ft_n = int(ft_up - ft_low)

       darray = helper.addArray([lArDQGlobals.Partitions[2*subdet:2*subdet+2]],larNoisyROMonAlg,lArDQGlobals.SubDet[subdet])
       darray.defineHistogram('slotBad:FTBad;KnownBadFEB', title='Known Bad FEBs:Slot:FT', 
                              type='TH2I', path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)

       darray.defineHistogram('slotMNB:FTMNB;KnownMNBFEB', title='Known MNB FEBs:Slot:FT', 
                              type='TH2I', path=hist_path,
                              xbins=slot_n,xmin=slot_low,xmax=slot_up,
                              ybins=ft_n, ymin=ft_low, ymax=ft_up)
    pass

    cfg.merge(helper.result())
    return cfg
    

if __name__=='__main__':

    # Setup the Run III behavior
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    # Setup logs
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Logging import log
    log.setLevel(DEBUG)


    # Set the Athena configuration flags
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
    createLArMonConfigFlags()

    nightly = '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q431/21.0/v1/'
    file = 'myESD.pool.root'
    ConfigFlags.Input.Files = [nightly+file]
    ConfigFlags.Input.isMC = False
    
    ConfigFlags.Output.HISTFileName = 'LArNoisyROMonitoringOutput.root'
    ConfigFlags.lock()

    # Initialize configuration object, add accumulator, merge, and run.
    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    # try NoisyRO algo 
    #first geometry
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    cfg.merge(LArGMCfg(ConfigFlags))
    cfg.merge(TileGMCfg(ConfigFlags))
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    cfg.merge(LArOnOffIdMappingCfg(ConfigFlags))
    # then NoisyROSummary creator
    from LArCellRec.LArNoisyROSummaryConfig import LArNoisyROSummaryCfg
    noisyROSumm = LArNoisyROSummaryCfg(ConfigFlags)
    noisyROSumm.getEventAlgo("LArNoisyROAlg").OutputKey="LArNoisyROSummary_recomputed"
    cfg.merge(noisyROSumm)
    # then monitoring algo
    noisemon=LArNoisyROMonConfig(ConfigFlags, inKey="LArNoisyROSummary_recomputed")
    cfg.merge(noisemon) 

    ConfigFlags.dump()
    f=open("NoisyROMonMaker.pkl","w")
    cfg.store(f)
    f.close()
   
    #cfg.run(20,OutputLevel=DEBUG) #use cfg.run() to run on all events
    cfg.run(20) #use cfg.run() to run on all events

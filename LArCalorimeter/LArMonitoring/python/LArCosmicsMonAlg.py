
#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#


def LArCosmicsMonConfigOld(inputFlags):
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelperOld
    from LArMonitoring.LArMonitoringConf import LArCosmicsMonAlg

    helper = AthMonitorCfgHelperOld(inputFlags, 'LArCosmicsMonAlgOldCfg')
    LArCosmicsMonConfigCore(helper, LArCosmicsMonAlg,inputFlags)
    return helper.result()

def LArCosmicsMonConfig(inputFlags):
    '''Function to configures some algorithms in the monitoring system.'''

    # The following class will make a sequence, configure algorithms, and link                                                                   
    # them to GenericMonitoringTools                                                                                                                                 
    
    from AthenaMonitoring.AthMonitorCfgHelper import AthMonitorCfgHelper
    helper = AthMonitorCfgHelper(inputFlags,'LArCosmicsMonAlgCfg')

    from AthenaConfiguration.ComponentFactory import CompFactory
    LArCosmicsMonConfigCore(helper, CompFactory.LArCosmicsMonAlg,inputFlags)

    return helper.result()


def LArCosmicsMonConfigCore(helper, algoinstance,inputFlags):


    from LArMonitoring.GlobalVariables import lArDQGlobals

    larCosmicsMonAlg = helper.addAlgorithm(algoinstance,'larCosmicsMonAlg')

    larCosmicsMonAlg.CosmicsMonGroupName = 'LarCosmicsMonGroup'
    larCosmicsMonAlg.MuonADCthreshold_EM_barrel = 30
    larCosmicsMonAlg.MuonADCthreshold_EM_endcap = 40
    larCosmicsMonAlg.MuonADCthreshold_HEC = 40
    larCosmicsMonAlg.MuonADCthreshold_FCAL = 40

    # adding BadChan masker private tool
    from AthenaCommon.Configurable import Configurable
    if Configurable.configurableRun3Behavior :
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        cfg=ComponentAccumulator()

        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelMaskerCfg
        acc= LArBadChannelMaskerCfg(inputFlags,problemsToMask=["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"],ToolName="BadLArRawChannelMask")
        larCosmicsMonAlg.LArBadChannelMask=acc.popPrivateTools()
        cfg.merge(acc)
    else :
        from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
        theLArBadChannelsMasker=LArBadChannelMasker("BadLArRawChannelMask")
        theLArBadChannelsMasker.DoMasking=True
        theLArBadChannelsMasker.ProblemsToMask=["deadReadout","deadPhys","short","almostDead","highNoiseHG","highNoiseMG","highNoiseLG","sporadicBurstNoise"]
        larCosmicsMonAlg.LArBadChannelMask=theLArBadChannelsMasker
        pass

    #mon group 
    cosmicMonGroup = helper.addGroup(
        larCosmicsMonAlg,
        larCosmicsMonAlg.CosmicsMonGroupName,
        '/LAr/',
        'run'
    )

    cosmic_path="Cosmics/"

    EM_bins=lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["C"]["2"]+lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["C"]["2"]+lArDQGlobals.Cell_Variables["etaRange"]["EMB"]["A"]["2"]+lArDQGlobals.Cell_Variables["etaRange"]["EMEC"]["A"]["2"]
    cosmicMonGroup.defineHistogram('mon_eta_EM,mon_phi;Muon2DHitsECAL',
                                   type='TH2F', 
                                   path=cosmic_path,
                                   title='Cosmics Seeds - Digit Max > '+str(int(larCosmicsMonAlg.MuonADCthreshold_EM_barrel))+'/'+str(int(larCosmicsMonAlg.MuonADCthreshold_EM_endcap))+' [ADC] in S2 Barrel/Endcap - EM;#eta cell;#phi cell;Number of Hits',
                                   xbins=EM_bins,
                                   ybins=lArDQGlobals.Cell_Variables["phiRange"]["EMB"]["A"]["2"])

    HEC_bins=lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["C"]["1"]+lArDQGlobals.Cell_Variables["etaRange"]["HEC"]["A"]["1"]
    cosmicMonGroup.defineHistogram('mon_eta_HEC,mon_phi;Muon2DHitsHCAL',
                                   type='TH2F', 
                                   path=cosmic_path,
                                   title='Cosmics Seeds - Digit Max > '+str(int(larCosmicsMonAlg.MuonADCthreshold_HEC))+' [ADC] in S1 HEC;#eta cell;#phi cell;Number of Hits',
                                   xbins=HEC_bins,
                                   ybins=lArDQGlobals.Cell_Variables["phiRange"]["HEC"]["A"]["1"])

    FCal_bins=lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["C"]["2"]+lArDQGlobals.Cell_Variables["etaRange"]["FCal"]["A"]["2"]
    cosmicMonGroup.defineHistogram('mon_eta_FCal,mon_phi;Muon2DHitsFCAL',
                                   type='TH2F', 
                                   path=cosmic_path,
                                   title='Cosmics Seeds - Digit Max > '+str(int(larCosmicsMonAlg.MuonADCthreshold_FCAL))+' [ADC] in S2 FCal;#eta cell;#phi cell;Number of Hits',
                                   xbins=FCal_bins,
                                   ybins=lArDQGlobals.Cell_Variables["phiRange"]["FCal"]["A"]["2"])




if __name__=='__main__':

   from AthenaConfiguration.AllConfigFlags import ConfigFlags
   from AthenaCommon.Logging import log
   from AthenaCommon.Constants import WARNING
   from AthenaCommon.Configurable import Configurable
   Configurable.configurableRun3Behavior=1
   log.setLevel(WARNING)


   from LArMonitoring.LArMonConfigFlags import createLArMonConfigFlags
   createLArMonConfigFlags()

   from AthenaConfiguration.TestDefaults import defaultTestFiles
   ConfigFlags.Input.Files = defaultTestFiles.RAW

   ConfigFlags.Output.HISTFileName = 'LArCosmicsMonOutput.root'
   ConfigFlags.DQ.enableLumiAccess = False
   ConfigFlags.DQ.useTrigger = False
   ConfigFlags.Beam.Type = 'collisions'
   ConfigFlags.lock()

   from CaloRec.CaloRecoConfig import CaloRecoCfg
   cfg=CaloRecoCfg(ConfigFlags)

   from LArCellRec.LArNoisyROSummaryConfig import LArNoisyROSummaryCfg
   cfg.merge(LArNoisyROSummaryCfg(ConfigFlags))

   cosm_acc = LArCosmicsMonConfig(ConfigFlags)
   cfg.merge(cosm_acc)

   ConfigFlags.dump()
   f=open("LArCosmicsMon.pkl","wb")
   cfg.store(f)
   f.close()

#   cfg.run(10,OutputLevel=WARNING)








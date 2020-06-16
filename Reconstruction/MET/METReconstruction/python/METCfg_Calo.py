# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#from METReconstruction.METRecoFlags import metFlags
from METReconstruction.METRecoCfg import BuildConfig, METConfig,getMETRecoTool,getMETRecoAlg,getRegionRecoTool
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def METCalo_Cfg(configFlags):
    sequencename = "METReconstruction_Calo"

    components = ComponentAccumulator()
    from AthenaCommon.AlgSequence import AthSequencer
    components.addSequence( AthSequencer(sequencename) )
    recotools=[]
    ############################################################################
    # EMTopo

    cfg_emt = METConfig('EMTopo',[BuildConfig('SoftClus','EMTopo')],
                    doRegions=True,
                    doOriginCorrClus=False
                    )

    #metFlags.METConfigs()[cfg_emt.suffix] = cfg_emt
    #metFlags.METOutputList().append(cfg_emt.suffix)
    #metFlags.METOutputList().append(cfg_emt.suffix+"Regions")
    recotool_emt = getMETRecoTool(cfg_emt)
    recotools.append(recotool_emt)


    ############################################################################
    # LocHadTopo
    
    cfg_lht = METConfig('LocHadTopo',[BuildConfig('SoftClus','LocHadTopo')],
                    doRegions=True,
                    doOriginCorrClus=False
                    )

    #metFlags.METConfigs()[cfg_lht.suffix] = cfg_lht
    #metFlags.METOutputList().append(cfg_lht.suffix)
    #metFlags.METOutputList().append(cfg_lht.suffix+"Regions")
    #recotool_lht = getMETRecoTool(cfg_lht)
    #recotools.append(recotool_lht)

    ############################################################################
    # Calo regions
    
    cfg_calo = METConfig('Calo',
                     [BuildConfig('CaloReg')],
                     doCells=True
                     )

    #metFlags.METConfigs()[cfg_calo.suffix] = cfg_calo
    #metFlags.METOutputList().append(cfg_calo.suffix)
    #recotool_caloreg= getMETRecoTool(cfg_calo)
    #regiontool_calo= getRegionRecoTool(cfg_calo)
    #recotools.append(recotool_caloreg)
    #recotools.append(regiontool_calo)
    #SO FAR THIS WON'T RUN, AND GIVES A KEY ERROR: 
    #File "/usera/sarahw/testareas/PostPHD/METDec2019/build/x86_64-centos7-gcc8-opt/python/METReconstruction/METRecoCfg.py", line 126, in getBuilder
    #tool.InputCollection = defaultInputKey[config.objType]
    #KeyError: 'CaloReg'
    #recoAlg_calo = getMETRecoAlg(algName='METRecoAlg_Calo',tools=recotools)
    recoAlg_calo = getMETRecoAlg(algName='METRecoAlg_Calo',configs={cfg_lht, cfg_calo})
    components.addEventAlgo(recoAlg_calo,sequencename)
    return components

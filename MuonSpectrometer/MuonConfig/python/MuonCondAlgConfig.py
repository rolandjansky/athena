# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

## Configuration Access to OFFLINE DB (COMP200)

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders

def MdtCondDbAlgCfg(flags, **kwargs):
    result  = ComponentAccumulator()
    folders = []
    if flags.Common.isOnline:
        return result ## avoid adding algo to the component accumulator
    else:
        kwargs["isOnline"] = False
        if flags.Input.isMC:
            kwargs['isData'] = False
            folders          = ["/MDT/DCS/DROPPEDCH", "/MDT/DCS/PSLVCHSTATE"]

            # TODO: probably will be used in the future but disable for now
            kwargs['ReadKey_MC_DE'] = ''
            kwargs['ReadKey_MC_DT'] = ''

            # disable the rest
            kwargs['ReadKey_DataR1_DC'] = ''
            kwargs['ReadKey_DataR1_HV'] = ''
            kwargs['ReadKey_DataR1_LV'] = ''
            kwargs['ReadKey_DataR1_V0'] = ''
            kwargs['ReadKey_DataR1_V1'] = ''
            kwargs['ReadKey_DataR2_HV'] = ''
            kwargs['ReadKey_DataR2_LV'] = ''
        else:
            kwargs['isData'] = True
            kwargs['isRun1'] = flags.IOVDb.DatabaseInstance == 'COMP200'
            kwargs['useRun1SetPoints'] = False
            if kwargs['isRun1'] and kwargs['useRun1SetPoints']:
                folders = ["/MDT/DCS/PSV0SETPOINTS", "/MDT/DCS/PSV1SETPOINTS"]

                # disable the rest
                kwargs['ReadKey_DataR1_DC'] = ''
                kwargs['ReadKey_DataR1_HV'] = ''
                kwargs['ReadKey_DataR1_LV'] = ''
                kwargs['ReadKey_DataR2_HV'] = ''
                kwargs['ReadKey_DataR2_LV'] = ''
            if kwargs['isRun1']:
                folders = ["/MDT/DCS/PSHVMLSTATE", "/MDT/DCS/PSLVCHSTATE", "/MDT/DCS/DROPPEDCH"]

                # disable the rest
                kwargs['ReadKey_DataR1_V0'] = ''
                kwargs['ReadKey_DataR1_V1'] = ''
                kwargs['ReadKey_DataR2_HV'] = ''
                kwargs['ReadKey_DataR2_LV'] = ''
            else:
                folders = ["/MDT/DCS/HV", "/MDT/DCS/LV"]

                # disable the rest
                kwargs['ReadKey_DataR1_DC'] = ''
                kwargs['ReadKey_DataR1_HV'] = ''
                kwargs['ReadKey_DataR1_LV'] = ''
                kwargs['ReadKey_DataR1_V0'] = ''
                kwargs['ReadKey_DataR1_V1'] = ''

            # disable MC folders
            kwargs['ReadKey_MC_DC'] = ''
            kwargs['ReadKey_MC_DE'] = ''
            kwargs['ReadKey_MC_DT'] = ''
            kwargs['ReadKey_MC_NC'] = ''
    
    alg = CompFactory.MdtCondDbAlg(**kwargs)
    result.merge( addFolders(flags, folders , detDb="DCS_OFL", className='CondAttrListCollection') )
    result.addCondAlgo(alg, primary = True)
    return result

def RpcCondDbAlgCfg(flags, **kwargs):
    result  = ComponentAccumulator()
    folders = []
    if flags.Common.isOnline:
        return result ## avoid adding algo to the component accumulator
        kwargs["isOnline"] = True
    else:
        kwargs["isOnline"] = False
        if flags.Input.isMC:
            kwargs['isData'] = False
            kwargs['ReadKey_DA_DP'] = ''
            kwargs['ReadKey_DA_OP'] = ''
        else:
            kwargs['isData'] = True
            kwargs['isRun1'] = flags.IOVDb.DatabaseInstance == 'COMP200'
            folders          = ["/RPC/DCS/DeadRopanels", "/RPC/DCS/OffRopanels"]
    alg = CompFactory.RpcCondDbAlg(**kwargs)
    result.merge( addFolders(flags, folders                     , detDb="DCS_OFL", className='CondAttrListCollection') )
    result.merge( addFolders(flags, ["/RPC/DQMF/ELEMENT_STATUS"], detDb="RPC_OFL", className='CondAttrListCollection') )
    result.addCondAlgo(alg)
    return result

def CscCondDbAlgCfg(flags, **kwargs):
    result  = ComponentAccumulator()
    pslope_from_db = False
    folders = ["/CSC/FTHOLD", "/CSC/NOISE", "/CSC/PED", "/CSC/RMS", "/CSC/STAT", "/CSC/T0BASE", "/CSC/T0PHASE"]
    scheme  = "CSC_OFL"
    kwargs['ReadKey_HV'] = '' # Never used at present 
    if flags.Common.isOnline:
        kwargs["isOnline"  ] = True
        kwargs['isData'    ] = True
        kwargs['ReadKey_FT'] = '/CSC/FTHOLD' # 'ConditionsStore+' prefix not necessarily needed in ReadKey
        kwargs['ReadKey_NO'] = '/CSC/NOISE'
        kwargs['ReadKey_PD'] = '/CSC/PED'
        if pslope_from_db:
            kwargs['ReadKey_PS'] = '/CSC/PSLOPE'
        kwargs['ReadKey_RM'] = '/CSC/RMS'
        kwargs['ReadKey_ST'] = '/CSC/STAT'
        kwargs['ReadKey_TB'] = ''
        kwargs['ReadKey_TP'] = ''
        folders = ["/CSC/ONL/FTHOLD", "/CSC/ONL/NOISE", "/CSC/ONL/PED", "/CSC/ONL/RMS", "/CSC/ONL/STAT"]
        if pslope_from_db:
            folders.append("/CSC/PSLOPE")
        scheme  = "CSC_ONL"
    else:
        if pslope_from_db:
            folders.append("/CSC/PSLOPE")

        kwargs["isOnline"] = False
        if flags.Input.isMC:
            kwargs['isData'] = False
        else:
            kwargs['isData'] = True
            kwargs['isRun1'] = flags.IOVDb.DatabaseInstance == 'COMP200'
    alg = CompFactory.CscCondDbAlg(**kwargs)
    result.merge( addFolders(flags, folders , detDb=scheme, className='CondAttrListCollection') )
    result.addCondAlgo(alg)
    return result

###def TgcCondDbAlgCfg(flags, **kwargs):
###    result  = ComponentAccumulator()
###    folders = [] # which folders?
###    if flags.Common.isOnline:
###        return result ## avoid adding algo to the component accumulator
###        kwargs["isOnline"] = True
###    else:
###        kwargs["isOnline"] = False
###        if flags.Input.isMC:
###            kwargs['isData'] = False
###        else:
###            kwargs['isData'] = True
###            kwargs['isRun1'] = flags.IOVDb.DatabaseInstance == 'COMP200'
###    alg = TgcCondDbAlg(**kwargs)
###    result.merge( addFolders(flags, folders , detDb="DCS_OFL", className='CondAttrListCollection') )
###    result.addCondAlgo(alg)
###    return result


def TgcDigitCondAlgCfg(flags):
    result  = ComponentAccumulator()
    result.addCondAlgo(CompFactory.TgcDigitASDposCondAlg())
    result.addCondAlgo(CompFactory.TgcDigitTimeOffsetCondAlg())
    result.addCondAlgo(CompFactory.TgcDigitCrosstalkCondAlg())

    if flags.Digitization.UseUpdatedTGCConditions:
        result.merge(addFolders(flags, ["/TGC/DIGIT/ASDPOS"], detDb="TGC_OFL", db="OFLP200", className="CondAttrListCollection"))
        # TODO: Below the explicit tag will be removed, once this is available in the global tag.
        result.merge(addFolders(flags, ["/TGC/DIGIT/TOFFSET"], tag='TgcDigitTimeOffset-00-01', detDb="TGC_OFL", db="OFLP200", className="CondAttrListCollection"))
        result.merge(addFolders(flags, ["/TGC/DIGIT/XTALK"], tag='TgcDigitXTalk-00-01', detDb="TGC_OFL", db="OFLP200", className="CondAttrListCollection"))
    else:  # need explicit tags, since the folder new and not defined at the presented global tag. It can be removed if the folder is available in all global tags.
        result.merge(addFolders(flags, ["/TGC/DIGIT/ASDPOS"], tag='TgcDigitAsdPos-00-01', detDb="TGC_OFL", db="OFLP200", className="CondAttrListCollection"))
        result.merge(addFolders(flags, ["/TGC/DIGIT/TOFFSET"], tag='TgcDigitTimeOffset-00-01', detDb="TGC_OFL", db="OFLP200", className="CondAttrListCollection"))
        result.merge(addFolders(flags, ["/TGC/DIGIT/XTALK"], tag='TgcDigitXTalk-00-01', detDb="TGC_OFL", db="OFLP200", className="CondAttrListCollection"))
    return result

def NswCalibDbAlgCfg(flags, **kwargs):
    result = ComponentAccumulator()

    ## online scenario
    if flags.Common.isOnline:
        kwargs['isData'] = True

        ## MM folders
        scheme  = "MDT_ONL"
        #folders = ["/MDT/Onl/MM/TIME/SIDEA", "/MDT/Onl/MM/CHARGE/SIDEA", \
        #           "/MDT/Onl/MM/TIME/SIDEC", "/MDT/Onl/MM/CHARGE/SIDEC"]
        #result.merge( addFolders(flags, folders, detDb=scheme, className='CondAttrListCollection') )
        
        # use specific folder tags for now
        result.merge( addFolders(flags, ["/MDT/Onl/MM/TIME/SIDEA"  ], detDb=scheme, className='CondAttrListCollection' , tag="MmTdoSideA-Const-3p73") )
        result.merge( addFolders(flags, ["/MDT/Onl/MM/TIME/SIDEC"  ], detDb=scheme, className='CondAttrListCollection' , tag="MmTdoSideC-Const-3p73") )
        result.merge( addFolders(flags, ["/MDT/Onl/MM/CHARGE/SIDEA"], detDb=scheme, className='CondAttrListCollection' , tag="MmPdoSideA-Const-9p0" ) )
        result.merge( addFolders(flags, ["/MDT/Onl/MM/CHARGE/SIDEC"], detDb=scheme, className='CondAttrListCollection' , tag="MmPdoSideC-Const-9p0" ) )

        kwargs["ReadKey_MM_SIDEA_TDO"] = "/MDT/Onl/MM/TIME/SIDEA"
        kwargs["ReadKey_MM_SIDEC_TDO"] = "/MDT/Onl/MM/TIME/SIDEC"
        kwargs["ReadKey_MM_SIDEA_PDO"] = "/MDT/Onl/MM/CHARGE/SIDEA"
        kwargs["ReadKey_MM_SIDEC_PDO"] = "/MDT/Onl/MM/CHARGE/SIDEC"


        ## sTGC folders
        scheme  = "TGC_ONL"
        #folders = ["/TGC/Onl/NSW/TIME/SIDEA", "/TGC/Onl/NSW/CHARGE/SIDEA", \
        #           "/TGC/Onl/NSW/TIME/SIDEC", "/TGC/Onl/NSW/CHARGE/SIDEC"]
        #result.merge( addFolders(flags, folders , detDb=scheme, className='CondAttrListCollection') )

        # use specific folder tags for now
        result.merge( addFolders(flags, [ "/TGC/Onl/NSW/TIME/SIDEA"  ], detDb=scheme, className='CondAttrListCollection' , tag="sTgcTdoSideA-Const-3p73"))
        result.merge( addFolders(flags, [ "/TGC/Onl/NSW/TIME/SIDEC"  ], detDb=scheme, className='CondAttrListCollection' , tag="sTgcTdoSideC-Const-3p73"))
        result.merge( addFolders(flags, [ "/TGC/Onl/NSW/CHARGE/SIDEA"], detDb=scheme, className='CondAttrListCollection' , tag="sTgcPdoSideA-Const-0p78"))
        result.merge( addFolders(flags, [ "/TGC/Onl/NSW/CHARGE/SIDEC"], detDb=scheme, className='CondAttrListCollection' , tag="sTgcPdoSideC-Const-0p78"))

        kwargs["ReadKey_STGC_SIDEA_TDO"] = "/TGC/Onl/NSW/TIME/SIDEA"
        kwargs["ReadKey_STGC_SIDEC_TDO"] = "/TGC/Onl/NSW/TIME/SIDEC"
        kwargs["ReadKey_STGC_SIDEA_PDO"] = "/TGC/Onl/NSW/CHARGE/SIDEA"
        kwargs["ReadKey_STGC_SIDEC_PDO"] = "/TGC/Onl/NSW/CHARGE/SIDEC"

    elif flags.Input.isMC:
        kwargs['isData'] = False

        ## MM folders
        scheme  = "MDT_OFL"
        #folders = ["/MDT/MM/TIME/SIDEA" , "/MDT/MM/CHARGE/SIDEA" , "/MDT/MM/THR/SIDEA" , \
        #           "/MDT/MM/TIME/SIDEC" , "/MDT/MM/CHARGE/SIDEC" , "/MDT/MM/THR/SIDEC" ]
        #result.merge( addFolders(flags, folders, detDb=scheme, className='CondAttrListCollection') )
        
        # use specific folder tags for now
        result.merge( addFolders(flags, ["/MDT/MM/TIME/SIDEA"  ], detDb=scheme, className='CondAttrListCollection' , tag="MmTdoSideA-Const-3p73") )
        result.merge( addFolders(flags, ["/MDT/MM/TIME/SIDEC"  ], detDb=scheme, className='CondAttrListCollection' , tag="MmTdoSideC-Const-3p73") )
        result.merge( addFolders(flags, ["/MDT/MM/CHARGE/SIDEA"], detDb=scheme, className='CondAttrListCollection' , tag="MmPdoSideA-Const-9p0" ) )
        result.merge( addFolders(flags, ["/MDT/MM/CHARGE/SIDEC"], detDb=scheme, className='CondAttrListCollection' , tag="MmPdoSideC-Const-9p0" ) )
        result.merge( addFolders(flags, ["/MDT/MM/THR/SIDEA"   ], detDb=scheme, className='CondAttrListCollection' , tag="MmThrSideA-Const-55p4") )
        result.merge( addFolders(flags, ["/MDT/MM/THR/SIDEC"   ], detDb=scheme, className='CondAttrListCollection' , tag="MmThrSideC-Const-55p4") )

        ## sTGC folders
        scheme  = "TGC_OFL"
        #folders = ["/TGC/NSW/TIME/SIDEA", "/TGC/NSW/CHARGE/SIDEA", "/TGC/NSW/THR/SIDEA", \
        #           "/TGC/NSW/TIME/SIDEC", "/TGC/NSW/CHARGE/SIDEC", "/TGC/NSW/THR/SIDEC"]
        #result.merge( addFolders(flags, folders , detDb=scheme, className='CondAttrListCollection') )

        # use specific folder tags for now
        result.merge( addFolders(flags, [ "/TGC/NSW/TIME/SIDEA"  ], detDb=scheme, className='CondAttrListCollection' , tag="sTgcTdoSideA-Const-3p73"))
        result.merge( addFolders(flags, [ "/TGC/NSW/TIME/SIDEC"  ], detDb=scheme, className='CondAttrListCollection' , tag="sTgcTdoSideC-Const-3p73"))
        result.merge( addFolders(flags, [ "/TGC/NSW/CHARGE/SIDEA"], detDb=scheme, className='CondAttrListCollection' , tag="sTgcPdoSideA-Const-0p78"))
        result.merge( addFolders(flags, [ "/TGC/NSW/CHARGE/SIDEC"], detDb=scheme, className='CondAttrListCollection' , tag="sTgcPdoSideC-Const-0p78"))
        result.merge( addFolders(flags, [ "/TGC/NSW/THR/SIDEA"   ], detDb=scheme, className='CondAttrListCollection' , tag="sTgcThrSideA-Const-52p7"))
        result.merge( addFolders(flags, [ "/TGC/NSW/THR/SIDEC"   ], detDb=scheme, className='CondAttrListCollection' , tag="sTgcThrSideC-Const-52p7"))

    ## offline
    else:
        kwargs['isData'] = True

        ## MM folders
        scheme  = "MDT_OFL"
        #folders = ["/MDT/MM/TIME/SIDEA", "/MDT/MM/CHARGE/SIDEA", \
        #           "/MDT/MM/TIME/SIDEC", "/MDT/MM/CHARGE/SIDEC"]
        #result.merge( addFolders(flags, folders, detDb=scheme, className='CondAttrListCollection') )
        
        # use specific folder tags for now
        result.merge( addFolders(flags, ["/MDT/MM/TIME/SIDEA"  ], detDb=scheme, className='CondAttrListCollection' , tag="MmTdoSideA-Const-3p73") )
        result.merge( addFolders(flags, ["/MDT/MM/TIME/SIDEC"  ], detDb=scheme, className='CondAttrListCollection' , tag="MmTdoSideC-Const-3p73") )
        result.merge( addFolders(flags, ["/MDT/MM/CHARGE/SIDEA"], detDb=scheme, className='CondAttrListCollection' , tag="MmPdoSideA-Const-9p0" ) )
        result.merge( addFolders(flags, ["/MDT/MM/CHARGE/SIDEC"], detDb=scheme, className='CondAttrListCollection' , tag="MmPdoSideC-Const-9p0" ) )
       
        ## sTGC folders
        scheme  = "TGC_OFL"
        #folders = ["/TGC/NSW/TIME/SIDEA", "/TGC/NSW/CHARGE/SIDEA", \
        #           "/TGC/NSW/TIME/SIDEC", "/TGC/NSW/CHARGE/SIDEC"]
        #result.merge( addFolders(flags, folders , detDb=scheme, className='CondAttrListCollection') )

        # use specific folder tags for now
        result.merge( addFolders(flags, [ "/TGC/NSW/TIME/SIDEA"  ], detDb=scheme, className='CondAttrListCollection' , tag="sTgcTdoSideA-Const-3p73"))
        result.merge( addFolders(flags, [ "/TGC/NSW/TIME/SIDEC"  ], detDb=scheme, className='CondAttrListCollection' , tag="sTgcTdoSideC-Const-3p73"))
        result.merge( addFolders(flags, [ "/TGC/NSW/CHARGE/SIDEA"], detDb=scheme, className='CondAttrListCollection' , tag="sTgcPdoSideA-Const-0p78"))
        result.merge( addFolders(flags, [ "/TGC/NSW/CHARGE/SIDEC"], detDb=scheme, className='CondAttrListCollection' , tag="sTgcPdoSideC-Const-0p78"))
    
    result.addCondAlgo(CompFactory.NswCalibDbAlg(**kwargs))
    return result

def NswPassivationDbAlgCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/MDT/MM/PASSIVATION", "MDT_OFL", className="CondAttrListCollection", tag="MmPassiv2022May19")) ## force explicit tag for now, to be removed later once folder tag is resolved via global tag
    alg = CompFactory.NswPassivationDbAlg("NswPassivationDbAlg", **kwargs)
    acc.addCondAlgo(alg)
    return acc

def MuonStationIntersectCondAlgCfg(flags, name='MuonStationIntersectCondAlg',**kwargs):
    # Has dependency IdHelperTool (which we ignore for now)
    result = ComponentAccumulator()
    result.merge(MdtCondDbAlgCfg(flags))
    if flags.Common.isOnline: kwargs.setdefault("MdtCondKey","")
    muon_station_intersect_condalg = CompFactory.MuonStationIntersectCondAlg(name=name, **kwargs)
    result.addCondAlgo(muon_station_intersect_condalg, primary=True)
    return result

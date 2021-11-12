# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
    result.addCondAlgo(alg)
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


def TgcDigitASDposCondAlgCfg(flags):
    result  = ComponentAccumulator()
    result.addCondAlgo(CompFactory.TgcDigitASDposCondAlg())
    result.merge(addFolders(flags, ["/TGC/DIGIT/ASDPOS"] , detDb="TGC_OFL", className="CondAttrListCollection"))
    return result

def NswCalibDbAlgCfg(flags, **kwargs):
    result = ComponentAccumulator()
    if flags.Common.isOnline:
        return result ## avoid adding algo to the component accumulator
    if flags.Input.isMC:
        kwargs['isData'  ] = False
        kwargs['isOnline'] = False
    else:
        kwargs['isData'  ] = True
        kwargs['isOnline'] = True if flags.Common.isOnline else False
    folders = ["/MDT/MM/TIME/SIDEA" , "/MDT/MM/CHARGE/SIDEA" , "/MDT/MM/VMM/SIDEA" , \
               "/MDT/MM/TIME/SIDEC" , "/MDT/MM/CHARGE/SIDEC" , "/MDT/MM/VMM/SIDEC" ]
    scheme  = "MDT_OFL"
    result.merge( addFolders(flags, folders , detDb=scheme, className='CondAttrListCollection') )
    folders = ["/TGC/NSW/TIME/SIDEA", "/TGC/NSW/CHARGE/SIDEA", "/TGC/NSW/VMM/SIDEA", \
               "/TGC/NSW/TIME/SIDEC", "/TGC/NSW/CHARGE/SIDEC", "/TGC/NSW/VMM/SIDEC"]
    scheme  = "TGC_OFL"
    result.merge( addFolders(flags, folders , detDb=scheme, className='CondAttrListCollection') )
    alg     = CompFactory.NswCalibDbAlg(**kwargs)
    result.addCondAlgo(alg)
    return result



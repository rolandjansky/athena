# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
#
# Import NSWCalibTools job properties
#
from AthenaCommon import CfgMgr

def MMCalibSmearingTool(name="MMCalibSmearingTool",extraFlags=None,**kwargs):
    kwargs["EtaSectors"] = [True,True, True, True]
    return CfgMgr.Muon__NSWCalibSmearingTool(name,**kwargs)

def STgcCalibSmearingTool(name="STgcCalibSmearingTool",extraFlags=None,**kwargs):
    kwargs["EtaSectors"] = [True,True, True, True, True, True]
    return CfgMgr.Muon__NSWCalibSmearingTool(name,**kwargs)

def NSWCalibTool(name="NSWCalibTool",extraFlags=None,**kwargs):
    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")
    from MuonCondAlg.MuonTopCondAlgConfigRUN2 import NswCalibDbAlg
    NswCalibAlg = NswCalibDbAlg("NswCalibDbAlg")
    if not hasattr(condSequence, "NswCalibDbAlg"):
        condSequence += NswCalibAlg
    from AthenaCommon.GlobalFlags import globalflags
    kwargs["isData"] = (globalflags.DataSource()=='data')
    if not kwargs["isData"]: ## peaking times for MC
        kwargs["mmPeakTime"  ]=200
        kwargs["sTgcPeakTime"]=0
    else: ## peaking times for data
        kwargs["mmPeakTime"  ]=200
        kwargs["sTgcPeakTime"]=0
    return CfgMgr.Muon__NSWCalibTool(name,**kwargs)

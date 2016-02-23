
# if you want calibration hit info in your d3pd and have the right packages checked out you can uncomment the block below
#from MakeCellInfo.MakeCellInfoConf import MakeCalCellInfo
#calCellInfo = MakeCalCellInfo()
#calCellInfo.EflowObjectsName="eflowObjects_FullMode"
#topSequence+=calCellInfo

OutputFile='tmp.d3pd.root'

from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

import D3PDMakerCoreComps
d3pdalg = D3PDMakerCoreComps.MakerAlg( "test", theJob, file = OutputFile,                                       D3PDSvc = "D3PD::RootD3PDSvc" )

from eflowD3PDMaker.eflowD3PD import eflowD3PD
eflowD3PD(d3pdalg)

#eflowRec
from eflowD3PDMaker.eflowD3PDObject import *
from D3PDMakerCoreComps.D3PDObject import D3PDObject
eflowD3PDObject = D3PDObject (makeeflowD3PDObject, 'eflow_')
eflowD3PDObject.defineBlock(0, 'eflow', eflowD3PDMakerConf.D3PD__eflowFillerTool)#,sgkey = "eflowObjects_FullMode")
# you need to change the mode used in mhodgkin/eflowD3PDMaker/python/eflowD3PDObject.py to use eflowObjects_FullMode

d3pdalg += eflowD3PDObject(10)

# if you want calibration hit info in your d3pd and have the right packages checked out you can uncomment the block below
#from calCellInfoD3PDMaker.calCellInfoD3PDObject import *
#calCellInfoD3PDObject = D3PDObject(makecalCellInfoD3PDObject,'CalCellInfo_')
#calCellInfoD3PDObject.defineBlock(0,'CallCellInfo',calCellInfoD3PDMakerConf.D3PD__calCellInfoFillerTool)
#d3pdalg += calCellInfoD3PDObject(10)

from CaloD3PDMaker.ClusterD3PDObject import ClusterD3PDObject
d3pdalg += ClusterD3PDObject( 1 )

import AthenaPoolCnvSvc.ReadAthenaPool
#svcMgr.EventSelector.InputCollections = [ "../../../../../01PoolFile/data12_hip.00210184.physics_MinBias.recon.ESD.r5762_tid01566321_00/ESD.01566321._001465.pool.root.1" ]
svcMgr.EventSelector.InputCollections = [ "/afs/cern.ch/user/s/soumya/public/data11_hi.00194382.physics_HardProbes.recon.ESD.f424/data11_hi.00194382.physics_HardProbes.recon.ESD.f424._lb0216._SFO-12._0002.1" ]
#import glob
#svcMgr.EventSelector.InputCollections = glob.glob("/afs/cern.ch/user/s/soumya/workarea/DATA/pbpb_run11/ESD/data11_hi.00194382.physics_HardProbes.recon.ESD.f424/*.1")

#theApp.EvtMax = -1
theApp.EvtMax = 5




from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource.set_Value_and_Lock('data')
#GlobalFlags.DataSource.set_data()


svcMgr += CfgMgr.AthenaEventLoopMgr(EventPrintoutInterval=5)
#DetDescrVersion="ATLAS-GEO-10-00-00"
#DetDescrVersion="ATLAS-GEO-20-00-01"
DetDescrVersion="ATLAS-GEO-18-01-01"
include ("RecExCommon/AllDet_detDescr.py")
#include ("/afs/cern.ch/user/s/soumya/testarea/19.2.1/Reconstruction/HeavyIonRec/HIRecExample/python/HIRecExampleFlags.py")
#theApp.Dlls += [ "CaloTools", "TileRecAlgs", "LArClusterRec" ]






#the main algorithm sequence
algSeq = CfgMgr.AthSequencer("AthAlgSeq")





#The HIEventShapeFillerTool
HIEventShapeFillerTool_instance=CfgMgr.HIEventShapeFillerTool("HIEventShapeFillerTool")
HIEventShapeFillerTool_instance.OutputLevel=VERBOSE
ToolSvc += HIEventShapeFillerTool_instance


#Algorithm to Call the HIEventShapeFillerTool
HIEventShapeMakerAlg=CfgMgr.HIEventShapeMaker()
HIEventShapeMakerAlg.OutputLevel           =VERBOSE
HIEventShapeMakerAlg.InputTowerKey         ="CombinedTower"
HIEventShapeMakerAlg.InputCellKey          ="AllCalo"
HIEventShapeMakerAlg.UseCaloCell           =False
HIEventShapeMakerAlg.OutputContainerKey    ="HIEventShapeContainer"
HIEventShapeMakerAlg.OrderOfFlowHarmonics  =7
HIEventShapeMakerAlg.HIEventShapeFillerTool=HIEventShapeFillerTool_instance
algSeq += HIEventShapeMakerAlg





#The HIEventShapeAnalysisTool
HIEventShapeAnalysisTool_instance=CfgMgr.HIEventShapeAnalysisTool("HIEventShapeAnalysisTool")
HIEventShapeAnalysisTool_instance.OutputLevel=VERBOSE
ToolSvc += HIEventShapeAnalysisTool_instance

#The GRLTool
MyGRLTool=CfgMgr.GoodRunsListSelectorTool("MyGRLTool")
MyGRLTool.GoodRunsListVec=["GRL_2011hi.xml"]
MyGRLTool.PassThrough    =True
ToolSvc += MyGRLTool

#algorithm to fill the Validation hists
HIEventShapeValidationAlg=CfgMgr.HIEventShapeValidation()
HIEventShapeValidationAlg.OutputLevel             =VERBOSE
HIEventShapeValidationAlg.InputContainerKey       ="HIEventShapeContainer"
HIEventShapeValidationAlg.EventInfoKey            ="ByteStreamEventInfo"
HIEventShapeValidationAlg.VtxContainerKey         ="VxPrimaryCandidate"
HIEventShapeValidationAlg.MaxZvtx                 =300
HIEventShapeValidationAlg.HIEventShapeAnalysisTool=HIEventShapeAnalysisTool_instance
HIEventShapeValidationAlg.GRLTool                 =MyGRLTool
algSeq += HIEventShapeValidationAlg



svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["MYSTREAM DATAFILE='myfile.root' OPT='RECREATE'"]


from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
outStream = MSMgr.NewPoolRootStream( "MyXAODStream", "myXAOD.pool.root");
outStream.AddItem(['xAOD::HIEventShapeContainer_v1#*'])
outStream.AddItem(['xAOD::HIEventShapeAuxContainer_v1#*'])
#outStream.AddItem(['CaloTowerContainer_v1#*'])
#outStream.AddItem(['CaloTowerAuxContainer_v1#*'])


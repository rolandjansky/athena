#Skeleton joboption for a simple analysis job

theApp.EvtMax=10                                              #says how many events to run over. Set to -1 for all events

import AthenaPoolCnvSvc.ReadAthenaPool                        #sets up reading of POOL files (e.g. xAODs)
inputFile = os.environ['ASG_TEST_FILE_MC_AFII']               #test input file  
svcMgr.EventSelector.InputCollections=[ inputFile ]           #insert your list of input files here


ToolSvc += CfgMgr.CP__JetTileCorrectionTool("JTCtool")

ToolSvc.JTCtool.CorrectionFileName = "JetTileCorrection/JetTile_pFile_010216.root" #default anyway
#ToolSvc.JTCtool.UserMaskedRegions = ["1 04","1 05","1 06","1 07","1 08","1 09",
#                                    "2 04","2 05","2 06","2 07","2 08","2 09"  ] #just to get some interesting output. These are NOT the current masked modules in TileCal!

algseq = CfgMgr.AthSequencer("AthAlgSeq")                     #gets the main AthSequencer
algseq += CfgMgr.CP__JetTileCorrectionAlg("JTCtester")        #adds an instance of your alg to it

algseq.JTCtester.JetTileCorrectionTool = ToolSvc.JTCtool

include("AthAnalysisBaseComps/SuppressLogging.py")  


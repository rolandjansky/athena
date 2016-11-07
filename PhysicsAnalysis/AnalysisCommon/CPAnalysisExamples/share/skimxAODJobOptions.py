#Author: Will Buttinger

#This is an example of how you could set up a skimming of an xAOD based on Run and Event Numbers
#It is written to work on both the analysis releases and the full athena releases
#Some parts are 'hidden' from the analysis releases, because those features are not yet available there
#If you wish to use this script, please update the relevent parts, which are marked by the SETME comments



theApp.EvtMax=10 #SETME: use -1 to process all input events

import AthenaPoolCnvSvc.ReadAthenaPool
myfile = os.environ.get("ASG_TEST_FILE_MC", "/afs/cern.ch/user/a/asgbase/patspace/xAODs/r6630/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.AOD.e3698_s2608_s2183_r6630_tid05352803_00/AOD.05352803._000242.pool.root.1")
svcMgr.EventSelector.InputCollections = [myfile] #SETME: put your input files in a list here
algseq = CfgMgr.AthSequencer("AthAlgSeq")



#if we are running in a full athena release we will also set up the calorimeter geometry, to copy the CaloCells
if "AthAnalysisBase" not in os.environ.get("CMTEXTRATAGS",""):
    #use the recexconfig AutoConfiguration to set the necessary tags and flags    
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    athenaCommonFlags.FilesInput = svcMgr.EventSelector.InputCollections
    from RecExConfig import AutoConfiguration
    AutoConfiguration.ConfigureSimulationOrRealData()  #this sets AthenaCommon.GlobalFlags.globalflags.DataSource to 'data' or 'geant4' based on input file
    AutoConfiguration.ConfigureGeo() 
    from AthenaCommon.DetFlags import DetFlags
    DetFlags.detdescr.all_setOff()
    DetFlags.detdescr.Calo_setOn()
    #Now load the description, will only load calorimeter, for calocells
    include("RecExCond/AllDet_detDescr.py")


#use athfile peeking to determine if simulation or data
from PyUtils import AthFile
af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']

#also take the stream name from the input file, to ensure the names don't change
streamName = af.fileinfos['stream_names'][0]

#This is how to configure the PyEvtFilter algorithm, which is designed to 'pick' events
from GaudiSequencer.PyComps import PyEvtFilter
algseq += PyEvtFilter("MyFilter", evt_list=[(222510,187960),(222510,185506)]) #SETME: put the run and event numbers here, as list of (runNum,eventNum)


#setup the output stream
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( streamName , "xAOD.out.root" )
xaodStream.Stream.TakeItemsFromInput = True #take all the event payload that analysis releases can read

#Create the metadata tools for handling the propagation of metadata
ToolSvc += CfgMgr.LumiBlockMetaDataTool("LumiBlockMetaDataTool") #only needed for data
ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool( "TriggerMenuMetaDataTool" )
theApp.CreateSvc += ['CutFlowSvc/CutFlowSvc'] #Not a metadatatool yet :-(
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool  ]
if not isMC: svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool  ]

#Add Metadata items to the output list. Only the ones that are present will actually get added
xaodStream.AddMetaDataItem(["xAOD::LumiBlockRangeContainer#*","xAOD::LumiBlockRangeAuxContainer#*",
                                                   "xAOD::TriggerMenuContainer#*","xAOD::TriggerMenuAuxContainer#*",
                                                   "xAOD::CutBookkeeperContainer#*", "xAOD::CutBookkeeperAuxContainer#*"])



#SETME: uncomment the next line to activate the event skimming. Only events passing the filter will go into the output stream
#xaodStream.AddAcceptAlgs( "MyFilter" ) #SETME: UNCOMMENT THIS LINE FOR THE SKIMMING







#Author: Will Buttinger

#This is an example of how you could set up a skimming of an xAOD based on Run and Event Numbers
#It is written to work on both the analysis releases and the full athena releases
#Some parts are 'hidden' from the analysis releases, because those features are not yet available there

#This joboption produces a file called xAOD.out.root
#This joboption can be used from the command line, like this:
#
#athena CPAnalysisExamples/skimxAODJobOptions.py --filesInput=/path/to/input/*.root [--evtMax=100] [-c "eventPickEvtList='myNumbers.txt'"]
#
#evtMax is optional argument, defaults to -1 (i.e. run on all events)
#specifying a eventPickEvtList will activate event skimming. The file should be a list
#of run eventNumber pairs
#one on each line
#e.g.
#
#222510 187960
#222510 185506
#
#etc
#you can skim on the grid like this
#
#pathena CPAnalysisExamples/skimxAODJobOptions.py -c "eventPickEvtList='eventList.txt'" --eventPickDataType=AOD --eventPickStreamName=physics_Main --eventPickEvtList=eventList.txt --outDS=user.$USER.skim/ --nFilesPerJob=1
#
#Remember to update the outDS

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
#this first bit is a clever hack for pathena submission without input file peeking
if type(theApp).__name__ == "fakeAppMgr":
    print "In Job Submission mode ..."
    #declare the output file, anyway we want!
    xaodStream = MSMgr.NewPoolRootStream( "dummy" , "xAOD.out.root" )
    theApp.initialize()


theApp.EvtMax=-1 #default value, run over all events
myfile = os.environ.get("ASG_TEST_FILE_MC", "/afs/cern.ch/user/a/asgbase/patspace/xAODs/r6630/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.AOD.e3698_s2608_s2183_r6630_tid05352803_00/AOD.05352803._000242.pool.root.1")
jps.AthenaCommonFlags.FilesInput = [myfile]  #list of input files

#use POOL read/write mechanism for this job
import AthenaPoolCnvSvc.ReadAthenaPool




#if we are running in a full athena release we will also set up the calorimeter geometry, to copy the CaloCells
if "AthAnalysisBase" not in os.environ.get("CMTEXTRATAGS","") and "AthAnalysis_DIR" not in os.environ:
    #use the recexconfig AutoConfiguration to set the necessary tags and flags    
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
af = AthFile.fopen(jps.AthenaCommonFlags.FilesInput()[0]) #peek the first file
isMC = 'IS_SIMULATION' in af.fileinfos['evt_type']


#also take the stream name from the input file, to ensure the names don't change
streamName = af.fileinfos['stream_names'][0]
#setup the output stream
xaodStream = MSMgr.NewPoolRootStream( streamName , "xAOD.out.root" )
xaodStream.Stream.TakeItemsFromInput = True #take all the event payload that analysis releases can read

#Add the required metadata tools
theApp.CreateSvc += ['CutFlowSvc/CutFlowSvc'] #Not a metadatatool yet :-(
svcMgr.MetaDataSvc.MetaDataTools += [ "xAODMaker::TriggerMenuMetaDataTool"  ]
if not isMC: svcMgr.MetaDataSvc.MetaDataTools += [ "LumiBlockMetaDataTool"  ]

#Add Metadata items to the output list. Only the ones that are present will actually get added
xaodStream.AddMetaDataItem(["xAOD::LumiBlockRangeContainer#*","xAOD::LumiBlockRangeAuxContainer#*",
                            "xAOD::TriggerMenuContainer#*","xAOD::TriggerMenuAuxContainer#*",
                            "xAOD::CutBookkeeperContainer#*", "xAOD::CutBookkeeperAuxContainer#*",
                            "xAOD::FileMetaData#*","xAOD::FileMetaDataAuxInfo#"])




#if a skimFile has been specified (using the -c "eventPickEvtList='file.txt'" option on command line)
#then activate skimming
if "eventPickEvtList" in locals():
    #open the skim file and read in list of run,event numbers
    evtList=[]
    with open(eventPickEvtList) as f:
        for line in f:
            if "," in line: num = line.split(",") #split by comma
            else: num = line.splt(" ") #otherwise split by space
            if len(num) == 0:
                print "problem with line %s" % (line)
            elif len(num) == 1:
                evtList += [None,long(num[0])] #assumes the number is an event number
            else:
                evtList += [(int(num[0]),long(num[1]))]
    #This is how to configure the PyEvtFilter algorithm, which is designed to 'pick' events
    from GaudiSequencer.PyComps import PyEvtFilter
    athAlgSeq += PyEvtFilter("MyFilter", evt_list=evtList) #add skim alg to main sequence, so that it runs
    xaodStream.AddAcceptAlgs( "MyFilter" )


include("AthAnalysisBaseComps/SuppressLogging.py") #for quieter output




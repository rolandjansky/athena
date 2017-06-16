# =============================================================================
#  Name:        AODMerge.py
#
#  Author:      Karsten Koeneke
#  Created:     September 2015
#
#  Usage: This script is meant to (slow-) merge several AODs into one AOD.
#         It should also correctly merge all available meta data.
#         To run, type:
#              athena POOLRootAccess/AODMerge.py 2>&1 | tee log.txt
# =============================================================================

# Import the error handling
#svcMgr.CoreDumpSvc.FatalHandler = 438
#import traceback

# To change the print format, leave 40 characters before "INFO",...
MessageSvc.Format = "% F%60W%S%7W%R%T %0W%M"

# for messaging
from AthenaCommon.Logging import logging
paodMsg = logging.getLogger('AODMerge')
logLevel = vars().get('AODLogLevel', INFO) # 3=INFO, 2=DEBUG
paodMsg.setLevel(logLevel)


# ==============================================================================
# Load your input file that you want to process.
# Note that the last method gets the highest priority.
# ==============================================================================
from glob import glob

# For the automatic local merging
if vars().has_key('INDIR'):
    inDir = vars().get('INDIR')
    if os.path.exists(inDir): myFileList = glob(inDir+"/*.root*")
    else: paodMsg.warning("Provided directory '%s' does not exist. Using input files: %s" % (inDir, myFileList))
    pass
if vars().has_key('INFILELIST'):
    inFileStringList = vars().get('INFILELIST')
    inFileList = inFileStringList.split(',')
    tmpFileList = []
    for inFile in inFileList:
        if os.path.exists(inFile): tmpFileList.append(inFile)
        else: paodMsg.warning("Provided input file '%s' does not exist.... skipping it!" % inFile)
        pass
    myFileList = tmpFileList
    pass
if vars().has_key('INTEXTFILE'):
    fileName = vars().get('INTEXTFILE')
    tmpFileList = []
    # Test if the file exists
    if not os.path.isfile(fileName): paodMsg.warning("Provided input text file '%s' does not exist...." % fileName)
    # Otherwise, the file exists and we can quickly open it in read-only mode
    else:
        with open(fileName,'r') as fileObject:
            for line in fileObject: tmpFileList.append(inFile.rstrip('\n'))
            pass
        pass
    myFileList = tmpFileList
    pass
if vars().has_key('INFILE'):
    inFile = vars().get('INFILE')
    if os.path.exists(inFile): myFileList = [inFile]
    else: paodMsg.warning("Provided input file '%s' does not exist. Using input files: %s" % (inFile, myFileList))
    pass
outFileName = ""
if vars().has_key('OUTFILE'): outFileName = vars().get('OUTFILE')
#myFileList = [myFileList[0]]



# ==============================================================================
#           ---- NO NEED TO CHANGE ANYTHING BELOW THIS LINE !!! ----
# ==============================================================================

# Set up the Athena file reading and tell it to load the files that you specified above
paodMsg.info("Using %i input files: %s" % (len(myFileList), myFileList))
jps.AthenaCommonFlags.FilesInput = myFileList
import AthenaPoolCnvSvc.ReadAthenaPool


# ==============================================================================
# Perform some commonly needed tasks
# ==============================================================================
svcMgr.StatusCodeSvc.AbortOnError=True

# Import the reading of in-file metadata
from PyUtils import AthFile
af = AthFile.fopen( svcMgr.EventSelector.InputCollections[0] )
eventDataItems = []
for evtItem in af.fileinfos["eventdata_items"]:
    evtItem0 = evtItem[0]
    if evtItem[0] == None :
        paodMsg.info("Got an unknown type from the input file for the item with name: %s" % evtItem[1])
        evtItem0 = "None"
    eventDataItems.append( evtItem0 + "#" + evtItem[1] )
    pass
# Get also the input stream name
from EventBookkeeperTools.CutFlowHelpers import GetInputStreamNameFromMetaDataItemList
inputStreamName = GetInputStreamNameFromMetaDataItemList( af.fileinfos["metadata_items"] )
paodMsg.info("Extracted input stream name: %s" % inputStreamName)
inputStreamName = vars().get('OUTSTREAMNAME', inputStreamName)
paodMsg.info("Using output stream name: %s" % inputStreamName)


# ==============================================================================
# Create the new output stream and copy over all items fron the input file to
# the output file, both event data and meta data
# ==============================================================================

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
outStreamName = inputStreamName # Use the same name also for the output as we are merging
if outFileName == "": outFileName = outStreamName+".merge.pool.root"
outStream = MSMgr.NewPoolRootStream( outStreamName, outFileName )
outStream.GetEventStream().TakeItemsFromInput = True
outStream.GetMetaDataStream().TakeItemsFromInput = True

# Schedule the meta-data tools/services to correctly propagate the meta-data
# from EventBookkeeperTools.CutFlowHelpers import CreateCutFlowSvc
# CreateCutFlowSvc( svcName="CutFlowSvc", athFile=af, seq=topSequence, addMetaDataToAllOutputFiles=True )
# theApp.CreateSvc += ['CutFlowSvc/CutFlowSvc']
# Create the CutFlowSvc instance
# import AthenaCommon.CfgMgr as CfgMgr
if not hasattr(svcMgr,"CutFlowSvc"): svcMgr += CfgMgr.CutFlowSvc()
svcMgr.CutFlowSvc.InputStream = inputStreamName
theApp.CreateSvc += ['CutFlowSvc/CutFlowSvc']
outStream.AddMetaDataItem( ["xAOD::CutBookkeeperContainer#CutBookkeepers",
                            "xAOD::CutBookkeeperAuxContainer#CutBookkeepers*",
                            "xAOD::CutBookkeeperContainer#IncompleteCutBookkeepers",
                            "xAOD::CutBookkeeperAuxContainer#IncompleteCutBookkeepers*"] )
# Also add an instance of the CutFlowSvc for all other CutBookkeeperContainers
for mdItem in af.fileinfos["metadata_items"]:
    if mdItem[0].startswith("xAOD::CutBookkeeperContainer") \
      and not mdItem[1] == "CutBookkeepers" \
      and not mdItem[1].startswith("Incomplete") :
        cbkName = mdItem[1]
        paodMsg.info("Creating now a CutFlowSvc instance for CutBookkeeperContainer with name %s" % cbkName)
        cfsName = "CutFlowSvc"+cbkName
        svcMgr += CfgMgr.CutFlowSvc(cfsName,
                                    #OutputLevel              = WARNING,
                                    InputStream              = inputStreamName,
                                    OutputCollName           = cbkName,
                                    OutputIncompleteCollName = "Incomplete"+cbkName
                                    )
        theApp.CreateSvc += [ "CutFlowSvc/"+cfsName ]
        outStream.AddMetaDataItem( ["xAOD::CutBookkeeperContainer#"+cbkName,
                                    "xAOD::CutBookkeeperAuxContainer#"+cbkName+"*"] )
        pass
    pass


# For the correct merging of the trigger meta-data
ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool("TriggerMenuMetaDataTool")
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]
outStream.AddMetaDataItem( ["xAOD::TriggerMenuContainer#*",
                            "xAOD::TriggerMenuAuxContainer#*"] )

# For the correct merging of the luminosity blocks meta-data
ToolSvc += CfgMgr.LumiBlockMetaDataTool("LumiBlockMetaDataTool")
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]
outStream.AddMetaDataItem( ["xAOD::LumiBlockRangeContainer#*",
                            "xAOD::LumiBlockRangeAuxContainer#*"] )

# For the correct handling of the xAOD::EventFormat (required to read file in ROOT)
ToolSvc += CfgMgr.xAODMaker__EventFormatMetaDataTool( "EventFormatMetaDataTool" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.EventFormatMetaDataTool ]

# Set up the metadata tool:
ToolSvc += CfgMgr.xAODMaker__FileMetaDataCreatorTool( "FileMetaDataCreatorTool" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.FileMetaDataCreatorTool ]
outStream.AddMetaDataItem( ["xAOD::FileMetaData#FileMetaData",
                            "xAOD::FileMetaDataAuxInfo#FileMetaDataAux."] )

# Add some more meta data
outStream.AddMetaDataItem( ["IOVMetaDataContainer#*"] )


# ==============================================================================
# Set the number of events that you want to process (-1 means all events) or skip.
# Shown is a handy way how you can use command-line options.
# If EVTMAX is not given on the command line, the default -1 (process all events) is used.
# If SKIPEVT is not given on the command line, the default 0 (start from the beginning) is used.
# This works for any command line option that you may need; it is a python feature.
# ==============================================================================
svcMgr.EventSelector.SkipEvents = vars().get('SKIPEVT', 0)
theApp.EvtMax = vars().get('EVTMAX', -1)


# Suppress warnings from unknown objects (i.e., non-xAOD objects) in the input file
#svcMgr.PoolSvc.OutputLevel=ERROR

# Change the event printout interval, if you want to
evtPrintoutInterval = vars().get('EVTPRINT', 1000)
svcMgr += CfgMgr.AthenaEventLoopMgr( EventPrintoutInterval=evtPrintoutInterval )


# ==============================================================================
# Remove the long printouts of ItemListSvc at the end of the job
# ==============================================================================
# svcMgr.ItemListSvc.OutputLevel = WARNING


# ==============================================================================
# Add basic code performance monitoring.
# The results will be presented at the end of your job.
# PerfMon profiles what memory was use and how long it took to run.
# Chrono tells you in an ordered table how much time was spend in each algorithm.
# ==============================================================================
if vars().get('doPerfMon',False):
    from PerfMonComps.PerfMonFlags import jobproperties as pmjp
    pmjp.PerfMonFlags.doFastMon = True    # to only enable a lightweight monitoring
if vars().get('doChrono',False):
    theAuditorSvc = svcMgr.AuditorSvc
    theAuditorSvc.Auditors  += [ "ChronoAuditor"]
    svcMgr.ChronoStatSvc.PrintUserTime     = False
    svcMgr.ChronoStatSvc.PrintSystemTime   = False
    svcMgr.ChronoStatSvc.PrintEllapsedTime = False
    svcMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 1
    theApp.AuditAlgorithms = True



include("AthAnalysisBaseComps/SuppressLogging.py")

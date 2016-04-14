#----------------------------------------------------------------
## @file ExampleStreamConfig.py
## @brief
## This file puts together the pieces needed to add 
##   classes in AthenaPoooHlExampleData to an AthenaOutputStream
## <br><br>
## Note: In this case the transform is very simple for testing purposes, 
##       but provides an example of encapsulation. 
## @author Jack Cranshaw (Jack.Cranshaw@cern.ch)
## $Id: ExampleStreamConfig.py,v 1.11 2009-02-19 22:04:48 cranshaw Exp $
#----------------------------------------------------------------
# include needed conversion libraries
#   - hopefully will become unnecessary through CLIDSvc or plugins, 
#     but since it's inside the transform this is easy to change
#include( "AthenaPoolExampleConverter/AthenaPoolExampleConverter_jobOption.py" )
#include( "AthenaPoolExampleConverter/AthenaPoolExampleConverterItemList_jobOption.py" )
#include( "EventAthenaPool/EventAthenaPool_joboptions.py")
#include( "GeneratorObjectsAthenaPoolItemList_joboptions.py")

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

# Must make sure that no OutStream's have been declared
theApp.OutStream    = [];

# define an itemlist
#   - string is available in calling python file
#   - easy to print itemlist
#   - easy to expand list
exampleItemList = []
exampleItemList += [ "EventInfo#*" ]
exampleItemList += [ "xAOD::EventInfo_v1#*", "xAOD::EventAuxInfo_v1#*" ]
exampleItemList += [ "EventStreamInfo#*" ]
exampleItemList += [ "ExampleClass#*" ]
exampleItemList += [ "ExampleHitContainer#*" ]
exampleItemList += [ "ExampleTrackContainer#*" ]
#exampleItemList += [ "MyObjContainer#MyObjContainerWriteData" ]
#print exampleItemList
exampleMetadataList = []
exampleMetadataList += ["xAOD::EventFormat#*"]
exampleMetadataList += ["xAOD::CutBookkeeperContainer#*"]
exampleMetadataList += ["xAOD::CutBookkeeperAuxContainer#*"]
exampleMetadataList += ["LumiBlockCollection#*"]

# Define transform of stream
#   - allows streams to be defined outside, i.e this file adds no output
#   - only defines transform
#   - does not disrupt further stream transforms

def ExampleStreamConfig(streamname):
   streamname.WritingTool = "AthenaPoolOutputStreamTool"
   streamname.ItemList += exampleItemList
   return

def ExampleStreamConfig2(streamname,tag):
   streamname.WritingTool = "AthenaPoolOutputStreamTool"
#   streamname.ItemList += exampleItemList
   streamname.TakeItemsFromInput = True
   streamname.ProcessingTag = tag
   print "Setting tag ", streamname.ProcessingTag
   return

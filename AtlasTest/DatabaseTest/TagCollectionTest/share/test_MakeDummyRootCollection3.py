## @file test_MakeDummyRootCollection3.py
## @brief Test of reading differently produced data files and output partial back nav list
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeDummyRootCollection3.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InColl = [ "EventData2.root", "EventData.root" ]
InCollType = "ImplicitROOT"
OutColl = "EventTagCollection3.root"
OutCollType = "ExplicitROOT"
OutFile = "EventData3.root"
ProvOn = True
ProvIncludes = ['TEST1','TEST2']
MsgLevel = VERBOSE
WriteInputHeader = TRUE
StageOutName = "TEST3"

include("TagCollectionTest/MakeDummyCollection.py")

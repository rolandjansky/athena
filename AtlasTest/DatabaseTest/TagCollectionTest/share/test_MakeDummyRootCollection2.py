## @file test_MakeDummyRootCollection2.py
## @brief Test of read data and output both data and collection
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeDummyRootCollection2.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InColl = [ "EventData.root" ]
InCollType = "ImplicitROOT"
OutColl = "EventTagCollection2.root"
OutCollType = "ExplicitROOT"
OutFile = "EventData2.root"
MsgLevel = VERBOSE
StageOutName = "TEST2"

include("TagCollectionTest/MakeDummyCollection.py")

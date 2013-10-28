## @file test_MakeDummyRootCollection4.py
## @brief Test of MakeDummyCollection.py for ROOT collection creation
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeDummyRootCollection4.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InColl = [ "EventData2.root", "EventData3.root" ]
InCollType = "ImplicitROOT"
OutColl = "EventTagCollection4.root"
OutCollType = "ExplicitROOT"
OutFile = "EventData4.root"
ProvOn = True
MsgLevel = VERBOSE
StageOutName = "TEST4"

include("TagCollectionTest/MakeDummyCollection.py")

## @file test_MakeDummyRootCollection5.py
## @brief Test of MakeDummyCollection.py for ROOT collection creation
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeDummyRootCollection5.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InColl = [ "EventData3.root", "EventData2.root" ]
InCollType = "ImplicitROOT"
OutColl = "EventTagCollection5.root"
OutCollType = "ExplicitROOT"
OutFile = "EventData5.root"
ProvOn = True
ProvIncludes = ['TEST2']
MsgLevel = VERBOSE
StageOutName = "TEST5"

include("TagCollectionTest/MakeDummyCollection.py")

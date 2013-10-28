## @file test_MakeDummyRootCollection6.py
## @brief Test of name collision for primary/backnav refs
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeDummyRootCollection6.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InColl = [ "EventData5.root" ]
InCollType = "ImplicitROOT"
OutColl = "EventTagCollection6.root"
OutCollType = "ExplicitROOT"
OutFile = "EventData6.root"
ProvOn = True
MsgLevel = VERBOSE
StageOutName = "TEST5"

include("TagCollectionTest/MakeDummyCollection.py")

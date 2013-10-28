## @file test_MakeDummyRootCollection.py
## @brief Test of MakeDummyCollection.py for ROOT collection creation
## @brief 
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeDummyRootCollection.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

EvtMax = 5
OutColl = "EventTagCollection"
OutCollType = "ExplicitROOT"
OutCollConnect = ""
OutCatalog = "xmlcatalog_file:PoolFileCatalog.xml"
OutFile = "EventData.root"
OpenMode = "CREATE_AND_OVERWRITE"
MsgLevel = VERBOSE
StageOutName = "TEST1"
nometa = True

include("TagCollectionTest/MakeDummyCollection.py")


## @file test_MakeSimpleRootCollection.py
## @brief Test of MakeSimpleCollection.py for ROOT collection creation
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeSimpleRootCollection.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

EvtMax = 5
InCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
InColl = [ "EventData.root" ]
InCollType = "ImplicitROOT"
InCollConnect = ""
OutColl = "EventTagCollection"
OutCollType = "ExplicitROOT"
OutCollConnect = ""
OpenMode = "CREATE_AND_OVERWRITE"
MsgLevel = DEBUG

include("EventSelection/MakeSimpleCollection.py")


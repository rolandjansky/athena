## @file test_ReadEventTagRootCollection.py
## @brief Test to read back ROOT collection created by MakeEventTagCollection.py
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_ReadEventTagRootCollection.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InCatalog = [ "xmlcatalog_file:PoolFileCatalog1.xml" ]
InColl = [ "EventTagCollection" ]
InCollType = "ExplicitROOT"
InCollConnect = ""
Query = "EventNumber < 4"
MsgLevel = DEBUG

include("TagCollectionTest/ReadCollection.py")

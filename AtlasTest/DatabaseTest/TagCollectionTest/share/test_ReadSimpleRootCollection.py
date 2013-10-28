## @file test_ReadSimpleRootCollection.py
## @brief Test to read back ROOT collection created by MakeSimpleCollection.py
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_ReadSimpleRootCollection.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
InColl = [ "EventTagCollection" ]
InCollType = "ExplicitROOT"
InCollConnect = ""
Query = "EventNumber < 4"
MsgLevel = DEBUG

include("TagCollectionTest/ReadCollection.py")

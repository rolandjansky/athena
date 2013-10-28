## @file test_ReadDummyRootCollection.py
## @brief Test to read back ROOT collection created by MakeDummyCollection.py
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_ReadDummyRootCollection.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
InColl = [ "GlobalEventTagCollection.root" ]
InCollType = "ExplicitROOT"
InCollConnect = ""
MsgLevel = DEBUG

include("TagCollectionTest/ReadCollection.py")

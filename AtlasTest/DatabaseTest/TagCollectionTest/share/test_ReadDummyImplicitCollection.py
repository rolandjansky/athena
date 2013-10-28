## @file test_ReadDummyImplicitCollection.py
## @brief Test to read back, as an implicit collection, a ROOT data file created by MakeDummyCollection.py
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_ReadDummyImplicitCollection.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
InColl = [ "EventDataNoCatalog.root" ]
InCollType = "ImplicitROOT"
MsgLevel = DEBUG

include("TagCollectionTest/ReadCollection.py")


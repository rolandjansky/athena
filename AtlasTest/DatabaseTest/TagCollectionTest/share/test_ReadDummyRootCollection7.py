## @file test_ReadDummyRootCollection.py
## @brief Test to read back ROOT collection created by MakeDummyCollection.py
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_ReadDummyRootCollection.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
InColl = [ "EventTagCollection7.root","PFN:ToolSupremacy.root", "PFN:TagTool.root" ]
#InColl = [ "EventTagCollection7.root","ToolSupremacy.root" ]
InCollType = "ExplicitROOT"
InCollConnect = ""
Query = "NEGamma > 2 && EEGamma_1 > 20000 && EEGamma_2 > 18000"
MsgLevel = DEBUG
EvtMax=-1

include("TagCollectionTest/ReadCollection.py")

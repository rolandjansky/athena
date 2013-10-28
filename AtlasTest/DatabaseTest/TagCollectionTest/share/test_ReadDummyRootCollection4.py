## @file test_ReadDummyRootCollection4.py
## @brief Test to read back ROOT collection created by MakeDummyCollection4.py
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_ReadDummyRootCollection4.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
InColl = [ "PFN:EventTagCollection4.root" ]
InCollType = "ExplicitROOT"
InCollConnect = ""
Query = "NEGamma > 2 && EEGamma_1 > 20000 && EEGamma_2 > 18000"
MsgLevel = DEBUG

include("TagCollectionTest/ReadCollection.py")

## @file test_MakeSimpleRootCollectionFromCollection.py
## @brief Test of MakeSimpleCollection.py for ROOT collection creation from another ROOT collection
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeSimpleRootCollectionFromCollection.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

EvtMax = 5
InCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
InColl = [ "EventTagCollection" ]
InCollType = "ExplicitROOT"
OutColl = "EventTagCollection2"
MsgLevel = DEBUG

include("EventSelection/MakeSimpleCollection.py")


## @file test_MakeSimpleRootCollectionFromMC.py
## @brief Test of MakeSimpleCollection.py for ROOT collection creation from Monte Carlo data
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeSimpleRootCollectionFromMC.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

EvtMax = 5
InCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
InCollType = "MonteCarlo"
OutCatalog = "xmlcatalog_file:PoolFileCatalog.xml"
OutColl = "EventTagCollection"
OutCollType = "ExplicitROOT"
OutCollConnect = ""
OutFile = "EventData.root"
OpenMode = "CREATE_AND_OVERWRITE"
MsgLevel = DEBUG

include("EventSelection/MakeSimpleCollection.py")


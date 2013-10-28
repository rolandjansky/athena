## @file test_MakeGlobalEventRootCollection.py
## @brief Test of MakeGlobalEventCollection.py for ROOT collection creation
## @brief 
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_MakeGlobalEventRootCollection.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

EvtMax = 5
Out = "GlobalEventTagCollection.root"
Type = "ExplicitROOT"
Catalog = "xmlcatalog_file:PoolFileCatalog.xml"
In = ["EventData.root"]
Mode = "CREATE_AND_OVERWRITE"
MsgLevel = VERBOSE

include("TagCollectionTest/MakeGlobalEventCollection.py")


## @file test_EventTagRootCollectionDeepCopy.py
## @brief Test of EventTagCollectionDeepCopy.py for ROOT collections
## @author K. Karr (Kristo.Karr@cern.ch)
## $Id: test_EventTagRootCollectionDeepCopy.py,v 1.2 2005/02/04 01:42:05 kkarr Exp

InCatalog = [ "xmlcatalog_file:PoolFileCatalog.xml" ]
InColl = [ "EventTagCollection" ]
Query = "EventNumber > 1 && EventNumber < 4"
OutCatalog = "xmlcatalog_file:PoolFileCatalog1.xml"
OutFile = "EventDataCopy.root"
StageOutName = "AOD"
MsgLevel = DEBUG

include("EventSelection/EventTagCollectionDeepCopy.py")


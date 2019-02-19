/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**


@page EventInfo_page 

This package contains classes which provide general information about
an event.  

The main information is in the EventInfo class which provides access
to then: 

  - EventID: the unique identification of the event.
  - EventType: the type of the event, e.g. simulation, testbeam, etc
  - TriggerInfo: trigger information 
  .

For simulation pileup, two classes (MergedEventInfo, PileUpEventInfo)
extend this class.

The EventInfo may also contain a set of detector description tags
(name/value pairs) which were used to process the event, e.g. coming
from GeoModel and IOVDb. These are stored in EventType.  As well,
during event processing the tags are stored in a TagInfo object in the
DetectorStore. The TagInfoMgr service manages the creation of the
TagInfo object from information in the incoming event and IOVDb.

*/

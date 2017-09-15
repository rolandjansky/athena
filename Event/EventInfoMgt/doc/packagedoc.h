/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@page EventInfoMgt_page 

This package contains TagInfoMgr which is an Athena service that 
manages detector description tag information. It maintains a TagInfo
object in the Detector Store with current tag values.


The tags to fill the TagInfo object may come from a variety of
sources: i.e. the tags specified by the GeoModelSvc and IOVDbSvc,
or the tags stored in the incoming EventInfo object, or the
TagInfo object itself may be in the IOV DB. The TagInfoMgr itself
only adds in tags from the incoming event. Others are added by
external clients.

The interface provides a 'input' method to allow client to add in
their own tags:
   
virtual StatusCode addTag(const std::string& tagName, 
                          const std::string& tagValue) = 0;

The full set of tags in TagInfo are copied each event into
EventInfo (specifically EventType) to be written out in the event
stream. Currently, the tags are NOT written to the IOV DB via the
TagInfoMgr. 

Some clients need to know when detector description tags change
so that they may update their descriptions, such as the geometry
description. These clients need to set up a callback with the
IOVSvc, as is done below for this class. 

The TagInfoMgr also has a property to add value/tag pairs via
jobOptions. This is currently used in standard configurable
EventInfoMgtInit.py, to set the release version as a tag/value pair:
Release/<version>. The release version is extracted from an
environment variable.  

@section EventInfoMgt_jobopt Default values of properties of TagInfoMgr

AddEventInfoTags        - true
OverrideEventInfoTags   - true
ExtraTagValuePairs      - just add in release version
TagInfoKey              - ProcessingTags





*/

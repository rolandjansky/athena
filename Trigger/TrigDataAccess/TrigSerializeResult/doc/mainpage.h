/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage TrigSerializeResult

@section introduction Introduction 

The package is used for sending the L2 result to the EF in the online
environment where other serialization technologies are too heavy to be
used. It is implemented as a tool and its interface is defined by
ITrigSerializerToolBase. The clients use ITrigSerializer::serialize
and ITrigSerializer::deserialize mathods. The most efficient way is to
use one instance of serializer for sending all data. serialize and
deserialize methods can be called many times on different objects.
Before starting serialization of a new event (or unrelated data in
general) ITrigSerializer::reset should be called.
Serialization/deserialization keeps pointers to other objects valid.

There are two implementations Serializer and TrigTSerializer.
Serializer uses Reflex while TrigTSerializer uses Root streaming.

The package contains also StringSerializer class for
efficient serialization of strings and test classes from Sara
Maguerite Traynor.

The package which is using this tools is HLT::Navigation.


@section bcomp Backward compatibility
Please note that the result of 
changes may affect possibility to read back data already taken.
Please make sure that changes in this package (especially in Serializer)
are well tested in this context.


@htmlinclude used_packages.html

@include requirements

*/

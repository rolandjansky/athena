/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEO_MODEL_XML_TRANSLATE_H
#define GEO_MODEL_XML_TRANSLATE_H
//
//    Just an abbreviation for transcode functions. 
//        translate("a string") returns the u16 type needed internally by Xerces-C
//        translate(Xerces-C string) returns a char * string version
//
//    remember to release a char * with XMLString::release(char *theStringYouGaveMe) when finished with it
//
#include <xercesc/util/XMLString.hpp>

#define Translate xercesc::XMLString::transcode 

#endif // TRANSLATE_H

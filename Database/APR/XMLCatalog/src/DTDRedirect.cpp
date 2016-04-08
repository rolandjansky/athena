/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/XMLUniDefs.hpp"
#include "xercesc/util/XMLUni.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"

#include "DTDRedirect.h"

#include <string>
#include <iostream>

using namespace xercesc;

static const char* inmemdtd = "\
<!ELEMENT POOLFILECATALOG (META*,File*)>\
<!ELEMENT META EMPTY>\
<!ELEMENT File (physical,logical,metadata*)>\
<!ATTLIST META name CDATA #REQUIRED>\
<!ATTLIST META type CDATA #REQUIRED>\
<!ELEMENT physical (pfn)+>\
<!ELEMENT logical (lfn)*>\
<!ELEMENT metadata EMPTY>\
<!ELEMENT lfn EMPTY>\
<!ELEMENT pfn EMPTY>\
<!ATTLIST File ID ID  #REQUIRED>\
<!ATTLIST pfn name ID #REQUIRED>\
<!ATTLIST pfn filetype CDATA #IMPLIED>\
<!ATTLIST lfn name ID #REQUIRED>\
<!ATTLIST metadata att_name  CDATA #REQUIRED>\
<!ATTLIST metadata att_value CDATA #REQUIRED>\
";



const char* inmemdtdId = "redirectinmem.dtd";
        
InputSource* DTDRedirect::resolveEntity (const XMLCh* const  /*  publicId*/,
                                         const XMLCh* const  /*  systemId*/ )
{
    //
    //  Create MemBufferInputSource from the buffer containing the XML
    //  statements.
    //
    //  NOTE: We are using strlen() here, since we know that the chars in
    //  our hard coded buffer are single byte chars!!! The parameter wants
    //  the number of BYTES, not chars, so when you create a memory buffer
    //  give it the byte size (which just happens to be the same here.)
    //

  MemBufInputSource* memBufIS = new MemBufInputSource
    (
     (const XMLByte*)inmemdtd
     , strlen(inmemdtd)
     , inmemdtdId
     , false
     );
  return memBufIS;
}


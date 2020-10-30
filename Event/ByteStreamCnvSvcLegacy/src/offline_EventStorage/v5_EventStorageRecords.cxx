/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//
// functions to convert records to strings
// for debugging 
// 
#include <sstream>

#include "ByteStreamCnvSvcLegacy/offline_EventStorage/v5_EventStorageRecords.h"

std::string offline_EventStorage_v5::string_record(void *ri, const void *pi) {

  std::ostringstream s;
  
  uint32_t *record = reinterpret_cast<uint32_t *>(ri);
  const uint32_t *pattern = reinterpret_cast<const uint32_t *>(pi);
  int size=pattern[1];

  for(int i=0; i<size; i++) {
    if(pattern[i] != 0) {
      s << std::hex << pattern[i] << std::dec << " ";
    } else {
      s << record[i] << " ";
    }
  }
  
  std::string rs = s.str();   
  return rs;	    
}

std::string offline_EventStorage_v5::string_record(offline_EventStorage_v5::file_name_strings nst) {
  
  std::ostringstream s;

  s << std::hex << offline_EventStorage_v5::file_name_strings_marker << std::dec;

  s << " " << nst.appName.size();
  s << " " << nst.appName;

  char ns = nst.appName.size() % 4;
  if(ns) s.write("____",4-ns);

  s << " " << nst.fileNameCore.size();
  s << " " << nst.fileNameCore;

  ns = nst.fileNameCore.size() % 4;
  if(ns) s.write("____",4-ns);

  std::string rs = s.str();   
  return rs;	    

}

void offline_EventStorage_v5::reset_record(void *ri, const void *pi) {

  uint32_t *record = reinterpret_cast<uint32_t *>(ri);
  const uint32_t *pattern = reinterpret_cast<const uint32_t *>(pi);
  int size=pattern[1];

  for(int i=0; i<size; i++) record[i] = pattern[i];
}

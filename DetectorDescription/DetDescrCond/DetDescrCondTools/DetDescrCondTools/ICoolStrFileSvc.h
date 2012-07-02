/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTOOLS_ICOOLSTRFILESVC_H
#define DETDESCRCONDTOOLS_ICOOLSTRFILESVC_H
// ICoolStrFileSvc.h - interact with COOL conditions database for simple
// XML file/string data
// see mainpage.h for full documentation
// Richard Hawkings, started 14/11/05

#include <string>
#include "GaudiKernel/IInterface.h"

class ICoolStrFileSvc : virtual public IInterface {

 public:
  static const InterfaceID& interfaceID();

  // retrieve data from folder and channel as a string
  virtual StatusCode getString(const std::string& folder, const int chan,
    std::string& data) const=0;

  // retrieve data from folder and channel, save in a file
  // blank filename results in data being saved using filename stored with
  // data in COOL, i.e. same as the original input filename
  virtual StatusCode getFile(const std::string& folder, const int chan,
			     const std::string& file="") const=0;

  // put data in file into COOL at specified folder and channel, using
  // technology 0 (string, limited to 4000 characters) 
  // or 1 (CLOB, limited to 16Mbytes)
  virtual StatusCode putFile(const std::string& folder, const std::string& 
			     filename, const int chan, const int tech) const=0;
};

inline const InterfaceID& ICoolStrFileSvc::interfaceID() {
  static const InterfaceID IID_ICoolStrFileSvc("ICoolStrFileSvc",1,0);
  return IID_ICoolStrFileSvc;
}

#endif // DETDESCRCONDTOOLS_ICOOLSTRFILESVC_H

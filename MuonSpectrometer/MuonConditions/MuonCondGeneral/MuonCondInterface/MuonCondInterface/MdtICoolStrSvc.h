/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDSVC_MDTICOOLSTRSVC_H
#define MUONCONDSVC_MDTICOOLSTRSVC_H
// ICoolStrFileSvc.h - interact with COOL conditions database for simple
// XML file/string data


#include <string>
#include "GaudiKernel/IInterface.h"

namespace MuonCalib {
class MdtICoolStrSvc : virtual public IInterface {

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


  virtual StatusCode putFileT0(const std::string& folder, const std::string&
                             filename, const int chan, const int tech) const=0;

 virtual StatusCode putFileRT(const std::string& folder, const std::string&
                             filename, const int chan, const int tech) const=0;
 virtual StatusCode putFileAlignCorr(const std::string& folder, const std::string&
                             filename, const int chan, const int tech) const=0;
 virtual StatusCode putAligFromFile(const std::string& folder, const std::string&
                             filename, const int chan, const int tech) const=0;
 virtual StatusCode putFileTube(const std::string& folder, const std::string&
                             filename, const int chan, const int tech) const=0;

};
inline const InterfaceID& MdtICoolStrSvc::interfaceID() {
  static const InterfaceID IID_MdtICoolStrSvc("MdtICoolStrSvc",1,0);
  return IID_MdtICoolStrSvc;
}
}
#endif // MUONCONDSVC_MDTICOOLSTRSVC_H

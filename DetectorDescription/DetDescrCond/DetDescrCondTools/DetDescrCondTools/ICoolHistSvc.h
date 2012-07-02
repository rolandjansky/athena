/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DETDESCRCONDTOOLS_ICOOLHISTSVC_H
#define DETDESCRCONDTOOLS_ICOOLHISTSVC_H
// ICoolHistSvc.h - interface to manage reference histograms under COOL control
// Richard Hawkings, started 22/1/07

#include "GaudiKernel/IInterface.h"

class TH1;
class TH2;
class TH3;
class TObject;

class ICoolHistSvc : virtual public IInterface {
 public:
  static const InterfaceID& interfaceID();

  // get pointers to COOL-managed histograms, given the COOL folder
  // and channel number for the reference, and the histogram name within
  // the referenecd file
  // versions exist for 1,2 and 3 dimensional histograms
  // and for generic TObject which can be cast to other types
  // also an objectExists function to test if it exists without provoking 
  // ERROR messages or failures
  virtual StatusCode getHist(const std::string& folder, 
			     const unsigned int channel,
			     const std::string& histname,TH1*& hist) =0;
  virtual StatusCode getHist(const std::string& folder, 
			     const unsigned int channel,
			     const std::string& histname,TH2*& hist) =0;
  virtual StatusCode getHist(const std::string& folder, 
			     const unsigned int channel,
			     const std::string& histname,TH3*& hist) =0;
  virtual StatusCode getTObject(const std::string& folder, 
  			        const unsigned int channel,
			        const std::string& histname,TObject*& hist) =0;
  virtual bool objectExists(const std::string& folder, 
			    const unsigned int channel,
			    const std::string& histname) =0;

  // same but using string-named COOL channels rather than integer IDs
  // versions exist for 1,2 and 3 dimensional histograms
  // and for generic TObject which can be cast to other types
  virtual StatusCode getHist(const std::string& folder, 
			     const std::string& channel,
			     const std::string& histname,TH1*& hist) =0;
  virtual StatusCode getHist(const std::string& folder, 
			     const std::string& channel,
			     const std::string& histname,TH2*& hist) =0;
  virtual StatusCode getHist(const std::string& folder, 
			     const std::string& channel,
			     const std::string& histname,TH3*& hist) =0;
  virtual StatusCode getTObject(const std::string& folder, 
			        const std::string& channel,
			        const std::string& histname,TObject*& hist) =0;
  virtual bool objectExists(const std::string& folder, 
			    const std::string& channel,
			    const std::string& histname) =0;

};

inline const InterfaceID& ICoolHistSvc::interfaceID() {
  static const InterfaceID IID_ICoolHistSvc("ICoolHistSvc",1,0);
  return IID_ICoolHistSvc;
}

#endif // DETDESCRCONDTOOLS_ICOOLHISTSVC_H

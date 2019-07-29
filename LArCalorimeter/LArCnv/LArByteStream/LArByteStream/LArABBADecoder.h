//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARBYTESTREAM_LARABBADDECODER_H
#define LARBYTESTREAM_LARABBADDECODER_H

#include <stdint.h>

#include "AthenaBaseComps/AthAlgTool.h"
//#include "GaudiKernel/ToolHandle.h"

#include "LArRawEvent/LArDigitContainer.h"

#include "ByteStreamData/RawEvent.h" 

// #include "eformat/RODHeader.h"
#include "eformat/Version.h"
#include "eformat/Issue.h"
#include <vector>
#include <string>
#include "eformat/index.h"
//#include "eformat/ROBFragment.h"
//class ROBFragment;

using namespace OFFLINE_FRAGMENTS_NAMESPACE ; 

class LArABBADecoder : public AthAlgTool {

public: 

  LArABBADecoder(const std::string& type, const std::string& name,
		 const IInterface* parent ) ;

  static const InterfaceID& interfaceID( ) ;
  /** Destructor
   */ 
  virtual ~LArABBADecoder(); 

  virtual StatusCode initialize();
  virtual StatusCode finalize();


  StatusCode convert(const RawEvent* re, LArDigitContainer* coll);

private:

  void fillCollection(const ROBFragment* pROB, LArDigitContainer* coll);
}; 

#endif


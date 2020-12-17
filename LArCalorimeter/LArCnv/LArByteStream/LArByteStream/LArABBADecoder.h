//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


class LArABBADecoder : public AthAlgTool {

public: 

  LArABBADecoder(const std::string& type, const std::string& name,
		 const IInterface* parent ) ;

  static const InterfaceID& interfaceID( ) ;
  /** Destructor
   */ 
  virtual ~LArABBADecoder(); 

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;


  StatusCode convert(const RawEvent* re, LArDigitContainer* coll) const;

private:

  void fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* pROB,
                      LArDigitContainer* coll) const;
}; 

#endif


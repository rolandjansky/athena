/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECODER_RUNIT_H
#define DECODER_RUNIT_H 1
#include "AFP_ByteStream2RawCnv.h"
#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!



class runit: public ::AthAlgorithm { 
 public: 
  runit( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~runit(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

ToolHandle<AFP_ByteStream2RawCnv> m_decoder;

}; 

#endif //> !DECODER_RUNIT_H

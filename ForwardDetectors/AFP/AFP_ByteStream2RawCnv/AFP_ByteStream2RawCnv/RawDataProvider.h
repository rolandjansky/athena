/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECODER_RAWDATAPROVIDER_H
#define DECODER_RAWDATAPROVIDER_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
//#include "GaudiKernel/ToolHandle.h" //included under assumption you'll want to use some tools! Remove if you don't!
#include "AFP_ByteStream2RawCnv/AFP_ByteStream2RawCnv.h"


class RawDataProvider: public ::AthAlgorithm { 
 public: 
  RawDataProvider( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RawDataProvider(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

 private: 

}; 

#endif //> !DECODER_RAWDATAPROVIDER_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigFake_FakeRoIB_h
#define TrigFake_FakeRoIB_h
#include "TrigT1Result/CTPResult.h"
#include "AthenaBaseComps/AthAlgorithm.h"


class FakeRoIB : public AthAlgorithm {
 public:
  FakeRoIB( const std::string& name, ISvcLocator* pSvcLocator );
  /**
   * @brief this function reads the input file formatted as follows:
   * item:TBP,TAP,TAV item:TBP,TAP,TAV ... e.g. 12:1,0,0 13:1,1,1 ....
   **/
  StatusCode initialize() override;
  StatusCode execute() override;
  
 private:

  
  struct CTPRecord {
    CTPRecord( const std::vector<uint32_t>& tbp, const std::vector<uint32_t>& tav ) 
    :  TBP(tbp), TAV(tav) {}
    std::vector<uint32_t> TBP;
    std::vector<uint32_t> TAV;

  };
  
  std::vector<CTPRecord> m_data;
  size_t m_eventCounter;
  std::string m_inputFile;
};

namespace FakeRoIBHelper {
  ROIB::CTPResult buildCTPFragment( const std::vector<uint32_t>& tbp, const std::vector<uint32_t>& tav, uint32_t index );

}


#endif 

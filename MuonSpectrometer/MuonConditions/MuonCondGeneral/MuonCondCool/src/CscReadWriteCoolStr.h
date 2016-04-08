/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDCOOL_CSCREADWRITECOOLSTR_H
#define MUONCONDCOOL_CSCREADWRITECOOLSTR_H
/** CscReadWriteCoolStrFile - simple algorithm to demonstrate reading/writing
  of string objects stored in COOL via CoolStrFileSvc*/


#include <string>
#include <fstream>
#include <istream>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/DataHandle.h"

#include "MuonCondData/CscCondDataCollection.h"
#include "MuonCondData/CscCondDataContainer.h"

#include "MuonCondInterface/CscICoolStrSvc.h"
#include "AthenaKernel/errorcheck.h"

class CscIdHelper;
class StoreGateSvc;

namespace MuonCalib {
  class CscICoolStrSvc;
  /**	
    @class CscReadWriteCoolStr

    @brief Front end class that manages program

    @author lampen@physics.arizona.edu

    @section Description
    CscReadWriteCoolStr is the main algorithm, and controls the program flow.
   */

  class CscReadWriteCoolStr: public AthAlgorithm
  {
    public:
      CscReadWriteCoolStr(const std::string& name, ISvcLocator* pSvcLocator);
      ~CscReadWriteCoolStr(void);
      /**Initializes the detector store*/
      StatusCode initialize(void);
      /**Either writes to or reads from the detector store (depending on jobOptions). The brunt of the work though is done by CscCoolStrSvc*/
      StatusCode execute(void);
      StatusCode finalize(void);

    private:
      /**Reads in a set of files for entry to cool database*/
      StatusCode readFiles();
      StatusCode procInputStream(std::istream & input); 

      template<typename data_type>
        StatusCode procParameter3(std::istream & input, const std::string & name, const std::string & cat, const int& nBits, const int & shiftBits); 
      template<typename data_type>
        StatusCode procParameter4(std::istream & input, const std::string & name, const std::string & cat, const int& nBits, const int & shiftBits); 

      StatusCode mergeBits(const uint32_t & inputDatum, const uint32_t & refDatum, uint32_t & newDatum, 
          const int & nBits, const int & bitShift);
      StatusCode mergeBits(const int & inputDatum, const int & refDatum, const int & newDatum, 
          const int & nBits, const int & bitShift);
      StatusCode mergeBits(const float & inputDatum, const float & refDatum, const float & newDatum, 
          const int & nBits, const int & bitShift);


      /**Retrieves data from cool database and adds to stream*/
      std::string RetrieveDataAsString(std::string coolKey, int hash, std::string dataType);

      //StatusCode getIndex(const std::string & idString,  const std::string & cat,unsigned int & index ) const; 

      StatusCode writeToCool();

      StatusCode makeFile();

    private: 
      MsgStream m_log;
      StoreGateSvc* p_detstore;
      ServiceHandle<MuonCalib::CscICoolStrSvc> m_cscCoolStrSvc;

      bool m_forceChanCat;

      bool m_done;
      // properties
      bool m_write;
      std::vector<std::string> m_ifiles;
      // methods

      bool m_read;
      std::string m_ofile;
      std::string m_outFileType;
      std::vector<std::string> m_outParameters;	

      CscCondDataContainer  * m_condDataContainer;
      std::map<std::string, CscCondDataCollectionBase *> m_condDataMap;

      /**CscIdHelper is used to convert from identifiers to hash ids. MuonDetector manager is a
        requirement on CscIdHelper*/
      const CscIdHelper* m_cscId;
  };


}

#include "CscReadWriteCoolStr.icc"

#endif // MUONCONDCOOL_CSCREADWRITECOOLSTR_H

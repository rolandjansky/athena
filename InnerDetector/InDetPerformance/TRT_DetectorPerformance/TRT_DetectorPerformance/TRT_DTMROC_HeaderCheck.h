/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DTMROC_HeaderCheck.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRT_DTMROC_HEADERCHECK_H
#define INDETTRT_DTMROC_HEADERCHECK_H


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TRT_ConditionsServices/ITRT_ByteStream_ConditionsSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "InDetRawData/InDetTimeCollection.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <string>
#include <map>

//#include "InDetByteStreamErrors/TRT_BSErrContainer.h"
//#include "InDetByteStreamErrors/TRT_BSIdErrContainer.h"

#include "TTree.h"
#include "TH2.h"

class AtlasDetectorID;
class Identifier;
class StoreGateSvc;


namespace InDet 
{

  /** @class TRT_DTMROC_HeaderCheck

      Algorithm to pull DTMROC Headers from events for analysis
      
      @author  Jonathan Stahlman <stahlman@cern.ch>
  */  

  class TRT_DTMROC_HeaderCheck : public AthAlgorithm
    {
    public:

       /** Standard Athena-Algorithm Constructor */
       TRT_DTMROC_HeaderCheck(const std::string& name, ISvcLocator* pSvcLocator);
       /** Default Destructor */
       ~TRT_DTMROC_HeaderCheck();

       /** standard Athena-Algorithm method */
       StatusCode          initialize();
       /** standard Athena-Algorithm method */
       StatusCode          execute();
       /** standard Athena-Algorithm method */
       StatusCode          finalize();

    private:
       
       bool BCID_Comparison(int rod_bcid, int dtmroc_bcid);

      ServiceHandle<StoreGateSvc>  m_sgSvc;
      ServiceHandle<ITHistSvc> m_histSvc;
      ServiceHandle<ITRT_ByteStream_ConditionsSvc> m_bytestream_conditionsSvc;
      const InDetTimeCollection* TRT_BCIDColl;
      const InDetTimeCollection* TRT_L1IDColl;
      std::map<uint32_t,uint32_t>* dtmroc_map;

      TTree* m_tree;

      int eventCount;

      unsigned int m_event_number;
      unsigned int m_time_stamp;
      unsigned int m_time_stamp_ns_offset;

      unsigned int m_total_num_l1id;
      unsigned int m_total_num_bcid;
      unsigned int m_total_num_missing;
      unsigned int m_total_num_general;
      unsigned int m_total_num_sendid;

      int m_bcid_returned_by_ROD;
      int m_l1id_returned_by_ROD;

      std::vector<uint32_t>* m_l1id_dtmroc_index;
      std::vector<uint32_t>* m_l1id_dtmroc_index_remap;
      std::vector<uint32_t>* m_l1id_rod_index;
      std::vector<uint32_t>* m_l1id_returned_l1id;

      std::vector<uint32_t>* m_bcid_dtmroc_index;
      std::vector<uint32_t>* m_bcid_dtmroc_index_remap; 
      std::vector<uint32_t>* m_bcid_rod_index;
      std::vector<uint32_t>* m_bcid_returned_bcid;

      std::vector<uint32_t>* m_missing_dtmroc_index;
      std::vector<uint32_t>* m_missing_dtmroc_index_remap;
      std::vector<uint32_t>* m_missing_rod_index;
      
      std::vector<uint32_t>* m_general_dtmroc_index;
      std::vector<uint32_t>* m_general_dtmroc_index_remap;
      std::vector<uint32_t>* m_general_rod_index;

      std::vector<uint32_t>* m_sendid_dtmroc_index;
      std::vector<uint32_t>* m_sendid_dtmroc_index_remap;
      std::vector<uint32_t>* m_sendid_rod_index;

    }; 


} // end of namespace

#endif 

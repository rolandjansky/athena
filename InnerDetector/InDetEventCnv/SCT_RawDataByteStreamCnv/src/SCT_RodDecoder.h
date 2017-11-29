/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *      @file header for SCT_RodDecoder Class
 *      AlgTool class to decode ROB bytestream data into RDO
 *      @author: Kondo.Gnanvo@cern.ch, Maria.Jose.Costa@cern.ch
 *      @current developer: Kondo Gnanvo, QMUL (London), august 2005
 */

#ifndef INDETRAWDATABYTESTREAM_SCT_RODDECODER_H 
#define INDETRAWDATABYTESTREAM_SCT_RODDECODER_H
//STL
#include <string>
#include <stdint.h> //puts definitions in global namespace, should use Boost/cstdint.hpp
                    //but what the heck.

#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetByteStreamErrors/InDetBSErrContainer.h"

class ISCT_CablingSvc;
class ISCT_ByteStreamErrorsSvc;
class SCT_ID;

namespace InDetDD{
  class SCT_DetectorManager; 
}

//using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
/** @class SCT_RodDecoder
 *  @brief AlgTool which decodes the SCT binary format to create RDOs
 *  also inserts them to the collection.
 *  This tool is used by the SCTRawCollByteStreamTool
 **/
class SCT_RodDecoder : public AthAlgTool, public ISCT_RodDecoder
{

  struct CacheHelper{//temp object to help with trigger caching
    IdentifierHash skipHash, lastHash;
    std::vector<IdentifierHash>* vecHash;
  };

 public: 
  //@name Usual AlgTool methods
  //@{
  /** constructor*/
  SCT_RodDecoder(const std::string& type, const std::string& name, const IInterface* parent ) ;
  
  /** destructor  */
  virtual ~SCT_RodDecoder(); 

  /** AlgTool initialize */
  virtual StatusCode initialize() override;

  /** AlgTool finalize*/
  virtual StatusCode finalize() override;
  
  /** @brief Decode the rob data fragment and fill the collection SCT_RDO_Collection 
   *  with the RDO built by the makeRDO(..) method
   **/
  virtual StatusCode fillCollection(const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment& robFrag,
                                    ISCT_RDO_Container& rdoIdc,
                                    InDetBSErrContainer* errs,
                                    std::vector<IdentifierHash>* vecHash = 0)
    override;


 private:
  /// method that builds the RawData RDO and add it to the collection 
  int makeRDO(int strip, int groupSize, int tbin, 
              uint32_t onlineId, int ERRORS,
              ISCT_RDO_Container& rdoIdc,
              CacheHelper&,
              const std::vector<int>& errorHit);

  /// add an error for each wafer in a problematic ROD.
  void addRODError(uint32_t rodid, int errorType,
                   InDetBSErrContainer* errs);

  bool addSingleError(const IdentifierHash idHash,
                      const int bsErrorType,
                      InDetBSErrContainer* errs);
  const SCT_ID* m_sct_id;
  const InDetDD::SCT_DetectorManager *m_indet_mgr;
  ServiceHandle<ISCT_CablingSvc> m_cabling;
  ServiceHandle<ISCT_ByteStreamErrorsSvc> m_byteStreamErrSvc;
  bool m_condensedMode ;
  bool m_superCondensedMode ;
  /** Summary of the decoding process */
  //  unsigned int m_hitnumber;             //!< Total number of decoded hits
  unsigned int  m_singleCondHitNumber;  //!< Total number of single hit decoded in condensed mode
  unsigned int  m_pairedCondHitNumber;  //!< Total number of paired hit decoded in condensed mode
  unsigned int  m_firstExpHitNumber;    //!< Total number of first hit decoded in expanded mode
  unsigned int  m_evenExpHitNumber;     //!< Total number of paired hit decoded in expanded mode
  unsigned int  m_lastExpHitNumber;     //!< Total number of last hit decoded in expanded mode
  unsigned int m_headnumber;            //!< Total number of decoded header data
  unsigned int m_trailnumber;           //!< Total number of decoded trailer data
  unsigned int m_head_error_bcid;       //!< Total number of bcid error in the header data
  unsigned int m_head_error_lvl1id;     //!< Total number of lvl1id error in the header data 
  unsigned int m_head_error_timeout;    //!< Total number of timeout error in the header data
  unsigned int m_head_error_formatter;  //!< Total number of formatter error in the header data
  unsigned int m_head_error_preamb;     //!< Total number of preamble error in the header data  
  unsigned int m_trail_error_overflow;  //!< Total number of overflow error in the trailer data
  unsigned int m_trail_error_limit;     //!< Total number of header trailer limit error in the trailer data 
  unsigned int m_trail_error_bit;       //!< Total number of trailer bit error 
  unsigned int m_config_data_bit;       //!< Total number of configuration data
  unsigned int m_flag_error_bit;        //!< Total number of flag error data
  unsigned int m_cond_hit1_error;       //!< Total number of first hit data error
  unsigned int m_cond_hit2_error;       //!< Total number second hit data errors 
  unsigned int m_chip_number_error;     //!< Total number of chip number error  
  unsigned int m_unknown_data_format;   //!< Total number of unknown data format
  unsigned int m_nHits;
  unsigned int m_nRDOs;
  unsigned int m_maskedLinkNumber;
  unsigned int m_maskedRODNumber;
  unsigned int m_RODClockErrorNumber;
  unsigned int m_truncatedRODNumber;
  unsigned int m_numMissingLinkHeader;
  unsigned int m_numUnknownOfflineId;
  
  bool m_triggerMode;

  ServiceHandle<IIncidentSvc> m_incidentSvc;
};

#endif //SCT_RAWDATABYTESTREAM_SCT_RODDECODER_H

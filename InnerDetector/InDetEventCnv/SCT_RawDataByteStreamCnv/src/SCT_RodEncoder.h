/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *      @header file for SCT_RodEncoder Class
 *      This class provides conversion from SCT RDO to ROD format ByteStream.
 *      @date 21/06/07:  SCT_RodEncoder turned into An AlgTool 
 *      @author: Nick Barlow nbarlow@mail.cern.ch, Kondo.Gnanvo@cern.ch
 *  
 */
 
#ifndef SCT_RAWDATABYTESTREAM_SCT_RODENCODER_H
#define SCT_RAWDATABYTESTREAM_SCT_RODENCODER_H

///STL
#include <vector>

///C99 : Not nice, this header puts types in the global namespace
//should use <cstdint> and std:: namespace, since C++11
#include <inttypes.h>

///Gaudi
#include "GaudiKernel/ServiceHandle.h"
/// base classes
#include "AthenaBaseComps/AthAlgTool.h"
#include "SCT_RawDataByteStreamCnv/ISCT_RodEncoder.h"

///InDet
/**needed for typedef, cannot fwd declare */
#include "InDetRawData/SCT_RDO_Container.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"

class ISCT_ByteStreamErrorsSvc;
class SCT_ID;

class SCT_RDORawData;
class Identifier;
class IdentifierHash;

/** Jan 09: make SCT_RodEncoder an AlgTool again, of which there
 * is one instance per job, rather than the lightweight class of 
 * which there was one instance per ROD.  This change is motivated
 * by the changes to write ByteStream errors back into the bytestream,
 * as we don't want to access the ByteStreamErrorsSvc 90 times per event
 * if we can avoid it.. */
 
class SCT_RodEncoder : virtual public ISCT_RodEncoder, virtual public AthAlgTool
{ 
 public:
 
  typedef SCT_RDORawData RDO ;
  typedef std::vector<const RDO*> vRDOs_t;
  typedef SCT_RDO_Container SCTRawContainer ;
  
  /** constructor  */

  SCT_RodEncoder(const std::string& type, const std::string& name, const IInterface* parent ) ;

  /** destructor  */
  virtual ~SCT_RodEncoder(); 

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize*/
  virtual StatusCode finalize();

  /// convert all collections of RDO's in the current  list to vector of 32bit words   
  virtual void fillROD(std::vector<uint32_t>& v, uint32_t robid, vRDOs_t& rdoVec);

  /// Encode rdo into the data: called by fillROD(..) 
  void encodeData(std::vector<int>& vtbin, std::vector<uint16_t>& v16, const RDO *rdo, int gSize, int strip);
  
  /// pack 32 bit word:  called by  encodeData(..) 
  void packFragments(std::vector<uint16_t>& v16, std::vector<uint32_t>& v32) const;

  /// from 16 bits array to 32 bit array   
  uint32_t set32bits(const unsigned short int* v16, const unsigned short int* pos, const unsigned short int n) const;

  /// Get the side info from the RDO 
  int side(const RDO* rdo);
  
  /// Get the time bin info from the RDO
  int tbin(const RDO* rdo);

  /// Get the group size info from the RDO 
  int groupSize(const RDO* rdo) { return rdo->getGroupSize(); }
  
  /// Get the strip number info from the RDO  
  int strip(const RDO* rdo);
 
  /// Get the offline Identifirer from the RDO  
  Identifier offlineId(const RDO* rdo);

  /// Get the online id from the RDO  
  uint32_t onlineId(const RDO* rdo);

  /// Get the ROD link info from the RDO 
  int rodLink(const RDO* rdo);

  /// fill vector with module with inverted phi readout direction between offline and online position 
  void addSwapModuleId (Identifier IdColl) { m_swapModuleId.insert(IdColl); } 

  /// Get the 16-bit word for a header for a hit 
  uint16_t getHeaderUsingRDO(const RDO* rdo);
  
  /// Get the 16-bit word for a header for a link with a ByteStream error 
  uint16_t getHeaderUsingHash(IdentifierHash, int);
  
  /// Get the 16-bit word for a trailer, with or without ByteStream errors 
  uint16_t getTrailer(int);

 private:
  enum ErrorWords{TIMEOUT_ERR=(1<<11),
                  L1_ERR=(1<<10),
                  BCID_ERR=(1<<9),
                  PREAMBLE_ERR=(1<<12),
                  FORMATTER_ERR=12,
                  TRAILER_ERR=(1<<12),
                  NULL_HEADER_ERR=0,
                  HEADER_TRAILER_ERR=(1<<11),
                  TRAILER_OVFLW_ERR=(1<<10),
                  ABCD_ERR=0,
                  RAWDATA_ERR=(3<<13),
                  NULL_TRAILER_ERR=0}; 
  void addHeadersWithErrors(const uint32_t robid, const std::set<IdentifierHash>* errors, 
                            ErrorWords errType, std::vector<uint16_t>& v16data);
  void addTrailersWithErrors(const uint32_t robid, const std::set<IdentifierHash>* errors, 
                             ErrorWords errType, std::vector<uint16_t>& v16data);
  void addSpecificErrors(const uint32_t robid, const std::set<IdentifierHash>* errors, 
                         ErrorWords errType, std::vector<uint16_t>& v16data);
  ServiceHandle<ISCT_CablingSvc> m_cabling;
  ServiceHandle<ISCT_ByteStreamErrorsSvc> m_bsErrs;
  const SCT_ID* m_sct_id;
  bool m_condensed;
  std::set<Identifier> m_swapModuleId;
  unsigned int m_singleCondHitNumber;
  unsigned int m_pairedCondHitNumber;
  unsigned int m_firstExpHitNumber;
  unsigned int m_evenExpHitNumber;
  unsigned int m_lastExpHitNumber;
  unsigned int m_headerNumber;
  unsigned int m_trailerNumber;
};

#endif // SCT_RAWDATABYTESTREAM_SCT_RODENCODER_H

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_CSCCRAWDATACOLLECTION_H
#define MUONBYTESTREAM_CSCCRAWDATACOLLECTION_H



#include "MuonRDO/CscRawData.h"
#include "DataModel/DataVector.h"
#include "SGTools/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"
#include <map>

/**

Collection of CSC Raw Hits, arranged according to CSC Detector Elements
Author: Ketevi A. Assamagan
        BNL December 24, 2003

*/
class CscRawDataCollection : public DataVector<CscRawData>
{

private:

  /** Identifier of this collection */
  uint32_t m_id;
  
  /** Offline IdentifierHash for this collection*/
  IdentifierHash m_idHash;

  /** online ROD Identifier */
  uint16_t m_rodId;
  uint16_t m_subDetectorId;

  /** number of RPU */
  uint16_t m_numRPU;  

  /** sampling phase */
  bool m_samplingPhase;

  /** trigger Type */ 
  bool m_triggerType;

  /** First Bit Summary */
  uint8_t m_firstBitSummary;

  /** event type/data type */
  uint32_t m_eventType;

  /** RPU ID and data type */
  std::vector<uint16_t> m_rpuID;
  std::vector<uint8_t> m_dataType;

  /** cluster counts for 10 SPU: 5 SPU in each chamber or RPU! 
      For Chamber CSS, RPU ID = 5 and for CSL RPU ID = 11
      For CSS, SPU ID goes from 0-4 and 5-9 for CSS
      The non-precision SPU are at index 4 and 9 in the list */
  std::vector<uint16_t> m_spuCount;

  /** SCA Address */
  uint32_t m_scaAddress;

public:

  /** typedef required by container */
  typedef uint16_t ID; 
  typedef CscRawData DIGIT; 

  /** Default constructor */
  CscRawDataCollection (uint32_t id)
    : DataVector<CscRawData>(), m_id(id) {
    m_numRPU          = 0;
    m_samplingPhase   = false;
    m_triggerType     = false;
    m_firstBitSummary = 0x0;
    m_eventType       = 0x0;
    m_rpuID.clear();
    m_dataType.clear();
    m_spuCount.resize(10,0);
    m_scaAddress     = 0x0;
    m_idHash         = m_id;
    m_rodId           = 0;
    m_subDetectorId   = 0;
  }

  CscRawDataCollection ()
    : DataVector<CscRawData>(), m_id(0) {
    m_numRPU          = 0;
    m_samplingPhase   = false;
    m_triggerType     = false;
    m_firstBitSummary = 0x0;
    m_eventType       = 0x0;
    m_rpuID.clear();
    m_dataType.clear();
    m_spuCount.resize(10,0);
    m_scaAddress      = 0x0;
    m_rodId           = 0;
    m_subDetectorId   = 0;
  }

  virtual ~CscRawDataCollection() {}

  /** access methods */
  uint16_t identify() const {return m_id;} 
  
  /** Returns the OFFLINE identifier hash for this collection*/
  IdentifierHash identifyHash() const {return m_idHash;}
  
  uint16_t rodId()     const {return m_rodId;}
  uint16_t subDetectorId() const {return m_subDetectorId;}
  uint16_t numRPU()  const { return m_rpuID.size(); }
  const std::vector<uint16_t> & rpuID() const { return m_rpuID; }
  const std::vector<uint8_t> & dataType() const { return m_dataType; }
  bool samplingPhase() const {return  m_samplingPhase; }
  bool triggerType() const { return m_triggerType; }
  uint8_t firstBitSummary() const { return m_firstBitSummary; }
  uint32_t eventType () const { return m_eventType; }
  uint8_t calAmplitude() const;
  uint8_t calLayer() const;
  uint32_t scaAddress() const { return m_scaAddress; }

  uint16_t numSamples() const;
  uint16_t latency() const;
  uint8_t  rate() const;

  /** is sparsified data? 
      is neutron rejection ON?
      is calibration enabled? */
  bool sparsified () const;
  bool neutron () const;
  bool calEnabled() const;

  /** return the SPU cluster counts 
      For all the non-precision layers,
      */
  uint16_t spuCount(unsigned int i) const 
   { 
     if (i<10) return m_spuCount[i];
     else return 0x0;
   }

  const std::vector<uint16_t>& spuCount() const  { return m_spuCount; }

  /** set methods - the name should be self-descriptive */
  void setRodId (uint16_t rodId) {m_rodId = rodId;}
  void setSubDetectorId (uint16_t subDetectorId) {m_subDetectorId = subDetectorId;}
  void setOnlineId (uint16_t id) {m_id = id; m_idHash = m_id; }
  
  /**sets the OFFLINE IdentifierHash for this collection*/
  void setIdentifyHash(const IdentifierHash& idHash) { m_idHash=idHash;}
  void set_samplingPhase() { m_samplingPhase = true; }
  void set_samplingPhase(bool flag) { m_samplingPhase = flag; }
  void set_triggerType() { m_triggerType = true; }
  void set_triggerType(bool flag) { m_triggerType = flag; }
  void set_firstBitSummary(const uint8_t summary) { m_firstBitSummary=summary; }
  void set_eventType (const uint32_t eventType ) { m_eventType = eventType; }

  void set_scaAddress( const uint32_t sca ) { m_scaAddress = sca; }
  void set_spuCount ( unsigned int i, uint16_t count) 
   { if ( i<10 ) m_spuCount[i]=count; }
  void set_spuCount ( const std::vector<uint16_t>& spuCount)
  { m_spuCount = spuCount; }

  void setSamplingRate (const uint16_t rate);
  void setNumSamples (const uint16_t n);
  void setLatency (const uint16_t latency);
  void setNumDPU  (const uint16_t numDPU);
 
  /** is this sparsified data? */
  void set_sparsified ();

  /** is neutron rejection applied? */
  void set_neutron ();
  
  /** is calibration enabled */
  void set_enableCal ();

  /** set the calibration amplitude */
  void set_calAmplitude ( const uint8_t calAmp );

  /** set the layer number where the calibration is been done */
  void set_calLayer     ( const uint8_t calLay );

  void setRPUID (const std::vector<uint16_t>& rpuID) { m_rpuID = rpuID; }
  void addRPU (const uint16_t rpuID ) { m_rpuID.push_back(rpuID); }
  void addDataType (const uint8_t dataType ) { m_dataType.push_back(dataType); }
  void set_dataType( const unsigned int i, const uint8_t dataType) {
    if ( i < m_dataType.size() ) m_dataType[i] = dataType;
  }
  void set_dataType( const std::vector<uint8_t>& dataType)
  { m_dataType = dataType; }
};

CLASS_DEF(CscRawDataCollection,4184,0)

/* Class_Def needed only for persistency */
typedef DataVector<CscRawDataCollection> CSC_RAWDATACOLLECTION_vector;
CLASS_DEF( CSC_RAWDATACOLLECTION_vector, 4179, 1 )

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const CscRawDataCollection& coll);

#endif


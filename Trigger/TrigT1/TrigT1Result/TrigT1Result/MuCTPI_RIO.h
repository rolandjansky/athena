// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1RESULT_MUCTPI_RIO_H
#define TRIGT1RESULT_MUCTPI_RIO_H

// System include(s):
#include <vector>
#include <cstdint>

// Gaudi/Athena include(s):
#include "AthenaKernel/CLASS_DEF.h"

// Forward declaration(s):
class MsgStream;

/**
 *   $Date: 2007-07-05 13:26:22 $
 *
 *   @short Reconstruction Input Object of the MuCTPI hardware and simulation
 *
 *          This is a slightly smarter version of MuCTPI_RDO. It basically provides
 *          the same features with additional accessor functions.
 *
 *          For a detailed description of the data format of the MuCTPI, see
 *          the MIROD documentation (https://edms.cern.ch/file/248757/1/mirod.pdf).
 *
 *          For a detailed description of the ATLAS raw event format, see
 *          https://edms.cern.ch/file/445840/3.1/ATL-D-ES-0019v31.pdf
 *
 *     @see MuCTPI_RDO
 *
 *  @author Tadashi Maeno <tmaeno@bnl.gov>
 *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *  @author David Berge <David.Berge@cern.ch>
 *  @author Ellie Dobson <eleanor.dobson@cern.ch>
 *  @author Christian Ohm <christian.ohm@cern.ch>
 * @version $Revision: 1.6 $
 */
class MuCTPI_RIO {

public:
  /// number of candidate multiplicity thresholds
  enum { N_SUM = 6 };

  /// Default constructor
  MuCTPI_RIO();
  /// Destructor
  ~MuCTPI_RIO() = default;

  /// Function setting the BCID of the object
  void setBCID( const uint16_t bcId ) { m_bcId = bcId; }
  /// The BCID of the object
  uint16_t getBCID() const { return m_bcId; }

  /// Set the multiplicity sum
  void setSum( const uint16_t * sum );
  /// Get the multiplicity sum for a given p<sub>T</sub> threshold
  uint16_t getSum( const int index ) const;

  /// <strong>Add</strong> a muon RoI to the object
  void setRoI( const uint16_t bcId, const uint16_t pTval,const uint16_t pTnum, const double eta, const double phi,
               const uint16_t secID, const uint16_t sysId, const uint16_t hemisphere,  const uint16_t roiNum, 
	       const bool accepted, const bool first, const bool duplicatedRoI, const bool duplicatedSector);
  /// Get the number of muon RoIs
  int getNRoI() const { return m_roI.size(); }
  /// Get the properties of a given muon RoI
  bool getRoI( const int index, uint16_t &bcId, uint16_t &pTval,uint16_t& pTnum,
               double &eta, double &phi, uint16_t& secID, uint16_t& sysId, uint16_t& hemisphere, uint16_t& roiNum,
               bool &accepted, bool &first, bool &duplicatedRoI, bool &duplicatedSector) const;

  /// Get the ROD ID specified in the header
  uint32_t getHeaderSourceId() const { return m_headerSourceId; }
  /// Get the run number specified in the header
  uint32_t getHeaderRunNumber() const { return m_headerRunNumber; }
  /// Get the LVL1 ID specified in the header
  uint32_t getHeaderLVL1ID() const { return m_headerLVL1ID; }
  /// Get the full BCID specified in the header
  uint32_t getHeaderBCID() const { return m_headerBCID; }
  /// Get the LVL1 trigger type specified in the header
  uint32_t getHeaderLVL1TriggerType() const { return m_headerLVL1TriggerType; }
  /// Get the event type specified in the header
  uint32_t getHeaderDetectorEventType() const { return m_headerDetEventType; }
  /// Get the number of data words as it was specified in the <strong>trailer</strong>
  uint32_t getHeaderNumberDataWords() const { return m_headerNDataWords; }
  /// Get the number of status words as it was specified in the <strong>trailer</strong>
  uint32_t getHeaderNumberStatusWords() const { return m_headerNStatusWords; }
  /// Get the status words from the payload
  const std::vector< uint32_t >& getHeaderStatusWords() const { return m_headerStatusWords; }

  /// Set the ROD ID specidied in the header
  void headerSourceId( uint32_t val ) { m_headerSourceId = val; }
  /// Set the run number specified in the header
  void headerRunNumber( uint32_t val ) { m_headerRunNumber = val; }
  /// Set the LVL1 ID specified in the header
  void headerLVL1ID( uint32_t val ) { m_headerLVL1ID = val; }
  /// Set the BCID specified in the header
  void headerBCID( uint32_t val ) { m_headerBCID = val; }
  /// Set the LVL1 trigger type specified in the header
  void headerLVL1TriggerType( uint32_t val ) { m_headerLVL1TriggerType = val; }
  /// Set the event type specified in the header
  void headerDetectorEventType( uint32_t val ) { m_headerDetEventType = val; }
  /// Set the number of data words as it was specified in the <strong>trailer</strong>
  void headerNumberDataWords( uint32_t val ) { m_headerNDataWords = val; }
  /// Set the number of status words as it was specified in the <strong>trailer</strong>
  void headerNumberStatusWords( uint32_t val ) { m_headerNStatusWords = val; }
  /// Get the status words from the payload
  std::vector< uint32_t >& headerStatusWords() { return m_headerStatusWords; }

  /// Function dumping the stored information to the message stream
  void dumpData() const;
  /// Function dumping the stored information to the message stream
  void dumpData( MsgStream& ) const;

private:
  /// Source ID as specified in the header
  /**
   * @see MuCTPI_RIO::getHeaderSourceId()
   */
  uint32_t m_headerSourceId;
  /// Run number as specified in the header
  /**
   * @see MuCTPI_RIO::getHeaderRunNumber()
   */
  uint32_t m_headerRunNumber;
  /// LVL1 ID as specified in the header
  /**
   * @see MuCTPI_RIO::getHeaderLVL1ID()
   */
  uint32_t m_headerLVL1ID;
  /// The full BCID as specified in the header
  /**
   * @see MuCTPI_RIO::getHeaderBCID()
   */
  uint32_t m_headerBCID;
  /// The LVL1 trigger type as specified in the header
  /**
   * @see MuCTPI_RIO::getHeaderLVL1TriggerType()
   */
  uint32_t m_headerLVL1TriggerType;
  /// The event type as specified in the header
  /**
   * @see MuCTPI_RIO::getHeaderDetectorEventType()
   */
  uint32_t m_headerDetEventType;
  /// The number of data words as specified in the <strong>trailer</strong>
  /**
   * @see MuCTPI_RIO::getHeaderNumberDataWords()
   */
  uint32_t m_headerNDataWords;
  /// The number of status words as specified in the <strong>trailer</strong>
  /**
   * @see MuCTPI_RIO::getHeaderNumberStatusWords()
   */
  uint32_t m_headerNStatusWords;
  /// The status words
  /**
   * @see MuCTPI_RIO::getHeaderStatusWords()
   */
  std::vector< uint32_t > m_headerStatusWords;

  /// What kind of BCID is this???
  uint16_t m_bcId;


  /// Candidate multiplicity in MuCTPI_RIO::N_SUM number of p<sub>T</sub> thresholds
  std::array<uint16_t, N_SUM> m_sum;

public:
  /**
   *   $Date: 2007-07-05 13:26:22 $
   *
   *   @short Custom private object to hold information about the stored reconstructed RoIs
   *
   *          For a little bit clearer code, the properties of the "reconstructed RoIs" in
   *          MuCTPI_RIO are stored in such objects. They store the information encoded in
   *          the 32-bit MuCTPI data words in separate variables for faster access, but this
   *          makes the object waste a bit of space. &rarr; Should not be written to POOL.
   *
   *          For a detailed description of the data format of the MuCTPI, see
   *          the MIROD documentation (https://edms.cern.ch/file/248757/1/mirod.pdf).
   *
   *  @author Tadashi Maeno <tmaeno@bnl.gov>
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author David Berge <David.Berge@cern.ch>
   *  @author Ellie Dobson <Eleanor.Dobson@cern.ch>
   * @version $Revision: 1.6 $
   */
  class MyRoI {

  public:
    /// Constructor
    MyRoI( const uint16_t bcId, const uint16_t pTVal, const uint16_t pTNum, const double eta, const double phi,
           const uint16_t secID, const uint16_t sysId, const uint16_t hemisphere, const uint16_t roiNum, 
	   const bool accepted, const bool first, const bool duplicatedRoI, const bool duplicatedSector);
    /// Destructor
    ~MyRoI() = default;

    /// Function dumping the stored information to the message stream
    void dumpData() const;
    /// Function dumping the stored information to the message stream
    void dumpData( MsgStream& ) const;

    /// 3-bit BCID as stored in the data word
    uint16_t m_bcId;
    /// p<sub>T</sub> threshold value (in GeV) as reconstructed from the data word
    uint16_t m_pTvalue;
    /// 3-bit p<sub>T</sub> threshold number as stored in the data word
    uint16_t m_pTnumber;
    /// &eta; position of the candidate
    double m_eta;
    /// &phi; position of the candidate
    double m_phi;
    /// 8-bit sector address as stored in the data word
    uint16_t m_sectorID;
    /// System in which the candidate was detected
    uint16_t m_sysId;
    /// Hemisphere in which the candidate was detected (false: z < 0, true: z > 0)
    bool m_hemisphere;

    /// RoI number (sub-sector in which the candidate was detected) as stored in the data word
    uint16_t m_roINumber;

    /// Flag showing whether the candidate was sent to the RoIB
    bool m_accepted;
    /// Flag showing whether the candidate had the highest p<sub>T</sub> in its sector
    bool m_first;
    /// Flag showing whether there was more than 1 candidate in this candidate's RoI
    bool m_duplicatedRoI;
    /// Flag showing whether there were more than 2 candidates in this candidate's sector
    bool m_duplicatedSector;


  }; // class MyRoI
private:
  /// Variable holding the custom RoIs
  std::vector< MyRoI > m_roI;

}; // class MuCTPI_RIO

CLASS_DEF( MuCTPI_RIO , 6271 , 0 )

#endif // TRIGT1RESULT_MUCTPI_RIO_H

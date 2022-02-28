/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_TGCL1RDO_H
#define MUONRDO_TGCL1RDO_H

#include <inttypes.h>
#include "MuonRDO/TgcL1RawData.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "Identifier/IdentifierHash.h"

/*
  TGC collection class used for bare RDO ByteStream Conversion.
  This class holds one ROD information

  @author Tadashi Maeno
      based on the previous one by H.Kurashige
*/
class TgcL1Rdo : public DataVector<TgcL1RawData>
{
public:
    // typedef required by container
    typedef uint16_t ID;
    typedef TgcL1RawData DIGIT;
/*
    enum
    {
        MAX_N_SIDE = 2, // A and C side
        MAX_N_ROD  = 12, // max number of ROD in each side
        MAX_ID = MAX_N_SIDE*MAX_N_ROD
    };
*/
    struct Errors
    {
        unsigned badBcID:1;
        unsigned badL1Id:1;
        unsigned timedout:1;
        unsigned badData:1;
        unsigned overflow:1;
    };

    struct SRodStatus
    {
        unsigned EC_RXsend:1; // Error in request to send an event via RXlink
        unsigned EC_FELdown:1; // A Front End link has gone down - abandoned
        unsigned EC_frame:1; // Invalid FE link framing words
        unsigned EC_Glnk:1; // G-link error
        unsigned EC_xor:1; // Invalid XOR event checksum
        unsigned EC_ovfl:1; // Input FE event is too long or FE FIFO overflow
        unsigned EC_timeout:1; // Timeout expired for at least one FE link
        unsigned EC_xormezz:1; // Bad XOR checksum from mezz board
        unsigned EC_wc0:1; // Event has WC=0
        unsigned EC_L1ID:1; // L1ID mismatch (TTC EVID FIFO vs local).
        unsigned EC_nohdr:1; // First word is not header
        unsigned EC_rectype:1; // Unrecognized record type
        unsigned EC_null:1; // Unexpected nulls in FE input
        unsigned EC_order:1; // Word is out of order
        unsigned EC_LDB:1; // Invalid or unexpected Local Data Block ID
        unsigned EC_RXovfl:1; // RXfifo has overflowed
        unsigned EC_SSWerr:1; // SSW reports T1C, NRC, T2C, or GlinkNoLock error
        unsigned EC_sbid:1; // Illegal SB ID
        unsigned EC_unxsbid:1; // Unexpected SB ID received
        unsigned EC_dupsb:1; // SB ID is duplicated in the event
        unsigned EC_ec4:1; // Unexpected SB L1 Event ID(lo 4)
        unsigned EC_bc:1; // Unexpected SB BCID
        unsigned EC_celladr:1; // Invalid cell address
        unsigned EC_hitovfl:1; // Too many hits in event
        unsigned EC_trgbit:1; // Unexpected trigger bits
        unsigned EC_badEoE:1; // Bad End-of-event marker received, not 0xFCA
        unsigned EC_endWCnot0:1; // WC not 0 after EoE marker
        unsigned EC_noEoE:1; // No End-of-event marker received
    };

    struct LocalStatus
    {
        unsigned mergedHitBCs:1;
        unsigned mergedTrackletBCs:1;
        unsigned sortedHits:1;
        unsigned sortedTracklets:1;
        unsigned hasRoI:1;
        unsigned fakeSsw:1;
        unsigned fill1:10;
    };

    /* Constructors
     */
    /**Default constructor*/
    TgcL1Rdo();

    /** Creates an empty container ready for writing */
    TgcL1Rdo(uint16_t id, IdentifierHash idHash);

    // P1
    TgcL1Rdo(uint16_t subDetectorId,
           uint16_t srodId,
           uint16_t bcId,
           uint16_t l1Id);

    // Destructor
    virtual ~TgcL1Rdo()
    {
    }

    // Identifier
    uint16_t identify() const
    {
        return m_id;
    }

        // Identifier
    IdentifierHash identifyHash() const
    {
        return m_idHash;
    }
    
    // set methods
    void setL1Id(uint32_t v)
    {
        m_l1Id = v;
    }
    void setBcId(uint16_t v)
    {
        m_bcId = v;
    }
    void setTriggerType(uint16_t v)
    {
        m_triggerType = v;
    }
    void setOnlineId (uint16_t subDetectorId, uint16_t srodId);

    // get methods
    uint16_t subDetectorId() const
    {
        return m_subDetectorId;
    }
    uint16_t srodId() const
    {
        return m_srodId;
    }
    uint16_t triggerType() const
    {
        return m_triggerType;
    }
    uint16_t bcId() const
    {
        return m_bcId;
    }
    uint16_t l1Id() const
    {
        return m_l1Id;
    }

    // class method for RawData identification
    static uint16_t identifyRawData (const TgcL1RawData &rawData);

    const Errors& errors() const
    {
        return m_errors;
    }
    void setErrors(uint16_t data)
    {
        // FIXME BUG: This is not portable!
        //   The layout of bitfields in a structure is implementation-defined.
        union cnvErrors {
          uint16_t data;
          Errors errors;
        } cnv;
        cnv.data = data;
        m_errors = cnv.errors;
    }

    const SRodStatus& srodStatus() const
    {
        return m_srodStatus;
    }
    void setSRodStatus(uint32_t data)
    {
        // FIXME BUG: This is not portable!
        //   The layout of bitfields in a structure is implementation-defined.
        union cnvSRodStatus {
          uint16_t data;
          SRodStatus srodStatus;
        } cnv;
        cnv.data = data;
        m_srodStatus = cnv.srodStatus;
    }

    const LocalStatus& localStatus() const
    {
        return m_localStatus;
    }
    void setLocalStatus(uint32_t data)
    {
        // FIXME BUG: This is not portable!
        //   The layout of bitfields in a structure is implementation-defined.
        union cnvLocalStatus {
          uint16_t data;
          LocalStatus localStatus;
        } cnv;
        cnv.data = data;
        m_localStatus = cnv.localStatus;
    }

    uint32_t orbit() const
    {
        return m_orbit;
    }
    void setOrbit(uint32_t orbit)
    {
        m_orbit = orbit;
    }

    uint16_t version() const
    {
        return m_version;
    }

    void setVersion(uint16_t version)
    {
        m_version = version;
    }

    void clear();

    // online ID calculator
    static uint16_t calculateOnlineId (uint16_t subDetectorId, uint16_t rodId);

private:
    // Returns offset, MAX_N_SROD
    static std::pair<int, int> initOnlineId();

    uint16_t m_version  = 0U; // starting August 2006 version = 300. Before that, version = 0

    /** ID of this instance*/
    uint16_t m_id = 0U;

    /** OFFLINE hash of this collection*/
    IdentifierHash m_idHash; 
    
    // online IDs
    uint16_t m_subDetectorId  = 0U;
    uint16_t m_srodId  = 0U;

    // Trigger Type
    uint16_t m_triggerType  = 0U;

    // BCID and L1ID on ROD
    uint16_t m_bcId ;
    uint16_t m_l1Id ;

    struct Errors      m_errors{};
    struct SRodStatus   m_srodStatus{};
    struct LocalStatus m_localStatus{};
    uint32_t m_orbit  = 0U;
};

/**Overload of << operator for std::ostream for debug output*/
std::ostream& operator<<(std::ostream& sl, const TgcL1Rdo& coll);

CLASS_DEF(TgcL1Rdo,29142172,0)

// Class needed only for persistency
typedef DataVector<TgcL1Rdo> TGC_L1RDO_vector;
CLASS_DEF(TGC_L1RDO_vector, 147069493, 1)

#endif

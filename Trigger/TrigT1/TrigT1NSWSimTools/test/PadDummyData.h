/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
#ifndef NSWL1_PADDUMMYDATA_H
#define NSWL1_PADDUMMYDATA_H

#include "Identifier/Identifier.h"

#include "TrigT1NSWSimTools/PadData.h"

namespace NSWL1 {

/**
   @short dummy PadData for unit test

   Contains only the values you set by hand (i.e. does not retrieve
   anything from the stgc helper). Everything else is set to
   default dummy values.

   @author davide.gerbaudo@gmail.com
   @date October 2015
*/

class PadDummyData: public PadData {

public:
    PadDummyData():
        m_id(0x0),
        m_time(0.0),
        m_bc_tag(0),
        m_sideId(0),
        m_moduleId(0),
        m_sectorId(0),
        m_multipletId(0),
        m_gasGapId(0),
        m_channelId(0),
        m_padEtaId(0),
        m_padPhiId(0) {}

    /** @defgroup getters
        @{
    */
    uint32_t id()       const { return m_id;          }
    uint16_t BC_Tag()   const { return m_bc_tag;      }
    float time()        const { return m_time;        }
    int sideId()        const { return m_sideId;      }
    int moduleId()      const { return m_moduleId;    }
    int sectorId()      const { return m_sectorId;    }
    int multipletId()   const { return m_multipletId; }
    int gasGapId()      const { return m_gasGapId;    }
    int channelId()     const { return m_channelId;   }
    int padEtaId()      const { return m_padEtaId;    }
    int padPhiId()      const { return m_padPhiId;    }
    /** @} */

    /** @defgroup setters
        @{
    */
    PadDummyData& id(const uint32_t &v)     { m_id = v; return *this;          }
    PadDummyData& BC_Tag(const uint16_t &v) { m_bc_tag = v; return *this;      }
    PadDummyData& time(const float &v)      { m_time = v; return *this;        }
    PadDummyData& sideId(const int &v)      { m_sideId = v; return *this;      }
    PadDummyData& moduleId(const int &v)    { m_moduleId = v; return *this;    }
    PadDummyData& sectorId(const int &v)    { m_sectorId = v; return *this;    }
    PadDummyData& multipletId(const int &v) { m_multipletId = v; return *this; }
    PadDummyData& gasGapId(const int &v)    { m_gasGapId = v; return *this;    }
    PadDummyData& channelId(const int &v)   { m_channelId = v; return *this;   }
    PadDummyData& padEtaId(const int &v)    { m_padEtaId = v; return *this;    }
    PadDummyData& padPhiId(const int &v)    { m_padPhiId = v; return *this;    }
    /** @} */

private:
    uint32_t m_id;
    float    m_time;
    uint16_t m_bc_tag;
    int m_sideId;
    int m_moduleId;
    int m_sectorId;
    int m_multipletId;
    int m_gasGapId;
    int m_channelId;
    int m_padEtaId;
    int m_padPhiId;
  };  // end PadDummyData

} // namespace NSWL1

#endif

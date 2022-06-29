/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_NSWASBUILTDBDATA_H
#define MUONCONDDATA_NSWASBUILTDBDATA_H

// STL includes
#include <string>

// Athena includes
#include "AthenaKernel/CondCont.h" 
#include "AthenaKernel/BaseInfo.h" 


class NswAsBuiltDbData {

  friend class MuonAlignmentCondAlg;

public:

    NswAsBuiltDbData(){};
    virtual ~NswAsBuiltDbData() = default;

    inline void setMmData(const std::string& data){ 
        m_MmData.assign(data); 
        m_MmIsSet = true; 
    };
    inline void setSTgcData(const std::string& data){ 
        m_sTgcData.assign(data); 
        m_sTgcIsSet = true; 
    };
    inline bool getMmData(std::string& data) const {
        data = m_MmData;
        return m_MmIsSet; 
    };
    inline bool getSTgcData(std::string& data) const {
        data = m_sTgcData;
        return m_sTgcIsSet; 
    };

 
private:

	// containers
    std::string m_MmData;
    std::string m_sTgcData;
    bool m_MmIsSet  {false};
    bool m_sTgcIsSet{false};

};

CLASS_DEF( NswAsBuiltDbData , 163462850 , 1 )
CLASS_DEF( CondCont<NswAsBuiltDbData> , 20792446 , 1 )

#endif

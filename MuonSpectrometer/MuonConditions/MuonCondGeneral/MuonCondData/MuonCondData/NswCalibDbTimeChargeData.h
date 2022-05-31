/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_NSWCALIBDBTIMECHARGEDATA_H
#define MUONCONDDATA_NSWCALIBDBTIMECHARGEDATA_H

// STL includes
#include <vector>

// Athena includes
#include "AthenaKernel/CondCont.h" 
#include "AthenaKernel/BaseInfo.h" 
#include "MuonIdHelpers/IMuonIdHelperSvc.h"



class NswCalibDbTimeChargeData {  

public:
    enum class CalibTechType{
        MM,
        STGC        
    };
    enum class CalibDataType{
        TDO,
        PDO        
    };
    /// Helper struct to cache all calibration constants 
    /// in a common place of the memory
    struct CalibConstants{
       double slope{0.};
       double slopeError{0.};
       double intercept{0.};
       double interceptError{0.};
       bool is_valid{false}; /// Flag set by the data indicating that the constants are actually valid 
    };
    
    NswCalibDbTimeChargeData(const MmIdHelper&, const sTgcIdHelper&);
    virtual ~NswCalibDbTimeChargeData() = default;

	// setting functions
	void setData(CalibDataType type, const Identifier& chnlId, CalibConstants constants);
	void setZero(CalibDataType type, CalibTechType tech,  CalibConstants constants);

	// retrieval functions
	std::vector<Identifier> getChannelIds   (CalibDataType type, const std::string tech, const std::string side) const;
    const CalibConstants& getCalibForChannel(CalibDataType type, const Identifier& channelId) const; 
 
private:
    
	// containers
    using ChannelCalibMap = std::map<unsigned long long, CalibConstants>;
    std::map<CalibDataType, ChannelCalibMap> m_data{};

    using ZeroCalibMap = std::map<CalibDataType, CalibConstants>;
    std::map<CalibTechType, ZeroCalibMap> m_zero{};

	// ID helpers
	const MmIdHelper&   m_mmIdHelper;
	const sTgcIdHelper& m_stgcIdHelper;

};

CLASS_DEF( NswCalibDbTimeChargeData , 120842040 , 1 )
CLASS_DEF( CondCont<NswCalibDbTimeChargeData> , 217895024 , 1 )

#endif

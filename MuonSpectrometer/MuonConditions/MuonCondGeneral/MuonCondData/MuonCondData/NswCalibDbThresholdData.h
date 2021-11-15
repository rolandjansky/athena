/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_NSWCALIBDBTHRESHOLDDATA_H
#define MUONCONDDATA_NSWCALIBDBTHRESHOLDDATA_H

// STL includes
#include <vector>

// Athena includes
#include "AthenaKernel/CondCont.h" 
#include "AthenaKernel/BaseInfo.h" 

// Forward declarations
class Identifier;
class MmIdHelper;
class sTgcIdHelper;


class NswCalibDbThresholdData {

  friend class NswCalibDbAlg;

public:

    NswCalibDbThresholdData(const MmIdHelper&, const sTgcIdHelper&);
    virtual ~NswCalibDbThresholdData() = default;

	// setting functions
	void setData(const Identifier*, const double);

	// retrieval functions
	std::vector<Identifier> getChannelIds(const std::string="", const std::string="") const;
	bool                    getThreshold (const Identifier*   , double&             ) const;

 
private:

	// containers
    std::map<unsigned long long, std::vector<double> > m_data{};

	// ID helpers
	const MmIdHelper&   m_mmIdHelper;
	const sTgcIdHelper& m_stgcIdHelper;

};

CLASS_DEF( NswCalibDbThresholdData , 108292495 , 1 )
CLASS_DEF( CondCont<NswCalibDbThresholdData> , 169109811 , 1 )

#endif

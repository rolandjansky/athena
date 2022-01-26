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

// Forward declarations
class Identifier;
class MmIdHelper;
class sTgcIdHelper;


class NswCalibDbTimeChargeData {

  friend class NswCalibDbAlg;

public:

    NswCalibDbTimeChargeData(const MmIdHelper&, const sTgcIdHelper&);
    virtual ~NswCalibDbTimeChargeData() = default;

	// setting functions
	void setData   (const std::string&, const Identifier*, const double, const double, const double, const double);

	// retrieval functions
	std::vector<Identifier> getChannelIds    (const std::string&, const std::string="", const std::string="") const;
	bool                    getSlope         (const std::string&, const Identifier*, double&) const;
	bool                    getSlopeError    (const std::string&, const Identifier*, double&) const;
	bool                    getIntercept     (const std::string&, const Identifier*, double&) const;
	bool                    getInterceptError(const std::string&, const Identifier*, double&) const;

 
private:

	// containers
    std::map<std::string, std::map<unsigned long long, std::vector<double> > > m_data{};

	// ID helpers
	const MmIdHelper&   m_mmIdHelper;
	const sTgcIdHelper& m_stgcIdHelper;

};

CLASS_DEF( NswCalibDbTimeChargeData , 120842040 , 1 )
CLASS_DEF( CondCont<NswCalibDbTimeChargeData> , 217895024 , 1 )

#endif

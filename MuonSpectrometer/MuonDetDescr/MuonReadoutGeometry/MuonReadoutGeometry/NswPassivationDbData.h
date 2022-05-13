/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_NSWPASSIVATIONDBDATA_H
#define MUONCONDDATA_NSWPASSIVATIONDBDATA_H

// STL includes
#include <vector>

// Athena includes
#include "AthenaKernel/CondCont.h" 
#include "AthenaKernel/BaseInfo.h" 

// Forward declarations
class Identifier;
class MmIdHelper;


class NswPassivationDbData {

  friend class NswPassivationDbAlg;

public:

    NswPassivationDbData(const MmIdHelper&);
    virtual ~NswPassivationDbData() = default;

	// setting functions
	void setData(const Identifier, const int, const float, const float, const std::string&);

	// retrieval functions
	std::vector<Identifier> getChannelIds      () const;
	bool                    getPassivatedWidth (const Identifier, float&, float&) const;
	bool                    getPassivatedHeight(const Identifier, float&, float&) const;

 
private:

	unsigned long long buildChannelId(const Identifier) const;

	// containers
	std::map<unsigned long long, std::array<float, 4> > m_data{};

	// ID helpers
	const MmIdHelper& m_mmIdHelper;

};

CLASS_DEF( NswPassivationDbData , 183672311 , 1 )
CLASS_DEF( CondCont<NswPassivationDbData> , 139639445 , 1 )

#endif

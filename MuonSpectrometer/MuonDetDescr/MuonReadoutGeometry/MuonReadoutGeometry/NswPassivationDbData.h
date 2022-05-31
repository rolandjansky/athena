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
    /// Helper struct to save the four passivation values of each PCB
    struct PCBPassivation{        
        double left{0.};
        double right{0.};
        double top{0.};
        double bottom{0.};        
        bool valid{false};
    };
    
    NswPassivationDbData(const MmIdHelper&);
    virtual ~NswPassivationDbData() = default;

	// setting functions
	void setData(const Identifier& chnlId, const int pcb, const float indiv, const float extra, const std::string& position);

	// retrieval functions
	std::vector<Identifier> getChannelIds      () const;
    const PCBPassivation&         getPassivation(const Identifier& id) const;
	

 
private:

	unsigned long long buildChannelId(const Identifier& id) const;

	// containers
	std::map<unsigned long long, PCBPassivation> m_data{};

	// ID helpers
	const MmIdHelper& m_mmIdHelper;

};

CLASS_DEF( NswPassivationDbData , 183672311 , 1 )
CLASS_DEF( CondCont<NswPassivationDbData> , 139639445 , 1 )

#endif

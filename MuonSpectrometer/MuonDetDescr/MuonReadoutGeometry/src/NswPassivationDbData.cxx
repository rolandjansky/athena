/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonReadoutGeometry/NswPassivationDbData.h"
#include "MuonIdHelpers/MmIdHelper.h"
#include "Identifier/Identifier.h"


// general functions ---------------------------------
NswPassivationDbData::NswPassivationDbData(const MmIdHelper& mmIdHelper):
    m_mmIdHelper(mmIdHelper)
{
}


// setting functions ---------------------------------

// setData
void
NswPassivationDbData::setData(const Identifier& chnlId, const int pcb, const float indiv, const float extra, const std::string& position) {
	unsigned long long channelId = chnlId.get_compact();
	PCBPassivation& passiv = m_data[channelId];
    passiv.valid = true;
	if     (position=="left" ) passiv.left = indiv + extra;
	else if(position=="right") passiv.right = indiv + extra;
	else {
		passiv.left = extra;
		passiv.right = extra;
	}
	if(extra==0) return;
	/* in case an extra passivation is given (extra>0), it is applied (1/2) 
	to top or bottom depending on the following cases:
	* if pcb = 1 or 6 => only top
	* if pcb = 5 or 8 => only bottom
	* else (if pcb = 2, 3, 4 or 7) => both top and bottom
	*/
	if(pcb!=5 && pcb!=8) passiv.top = extra/2;
	if(pcb!=1 && pcb!=6) passiv.bottom = extra/2;
}




// retrieval functions -------------------------------

// getChannelIds
std::vector<Identifier>
NswPassivationDbData::getChannelIds() const {
	std::vector<Identifier> keys;
	keys.reserve(m_data.size());
	for (const auto& p : m_data) {
		keys.emplace_back(p.first);
	}
	return keys;
}

const NswPassivationDbData::PCBPassivation&  NswPassivationDbData::getPassivation(const Identifier& id) const {
    unsigned long long channelId = buildChannelId(id);
    std::map<unsigned long long, PCBPassivation>::const_iterator itr = m_data.find(channelId);
    if (itr != m_data.end()) return itr->second;
    static const PCBPassivation dummy{};
    return dummy;    
}
// buildChannelId
unsigned long long
NswPassivationDbData::buildChannelId(const Identifier& chnlId) const {
	int chnl       = m_mmIdHelper.channel(chnlId);
	int pcb        = (chnl-1)/1024+1; // int division should round downwards
	int newChnl    = (pcb -1)*1024+1; // normalizing to the first strip
	Identifier boardId = m_mmIdHelper.channelID(m_mmIdHelper.elementID (chnlId), 
	                                            m_mmIdHelper.multilayer(chnlId), 
	                                            m_mmIdHelper.gasGap    (chnlId), newChnl);
	return boardId.get_compact();
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArCondFEBIdChanMap.h"
#include <iomanip>
void LArCondFEBIdChanMap::dump(std::ostream& out, const LArOnlineID* onlineID) const {

  //FEBIdMap::const_iterator itmap->
  //ChannelMap::const_iterator itmap=m_febIdVecs->begin();
  //ChannelMap::const_iterator itmap_e=m_febIdVecs->end();
  for (unsigned int iCoolChannel=0;iCoolChannel<m_febIdVecs.size();iCoolChannel++) {
    out << "Cool Channel #:" << std::setw(3) << iCoolChannel << " FEB id:"  <<  std::endl;
    const FEBIdVector& febIdVec=m_febIdVecs[iCoolChannel];
    FEBIdVector::const_iterator it=febIdVec.begin();
    FEBIdVector::const_iterator it_e=febIdVec.end();
    for (;it!=it_e;it++) {
      out << "                   " << (*it) << "(0x" << std::hex << *it << std::dec << ")";
      if (onlineID) {
	const HWIdentifier hwid((*it));
	int barrel_ec = onlineID->barrel_ec(hwid);
	std::string bc;
	if (barrel_ec==0)
	  bc="Barrel";
	else if (barrel_ec==1)
	  bc="Endcap";
	else
	  bc="unknown";
	
	int pos_neg   = onlineID->pos_neg(hwid);
	std::string pn;
	if (pos_neg==0) 
	  pn="C-Side";
	else if (pos_neg==1) 
	  pn="A-Side";
	else
	  pn="unknown";
	int FT        = onlineID->feedthrough(hwid);
	int slot      = onlineID->slot(hwid);
	out << " (" << bc << "/" << pn << "/FT=" << FT << "/Slot=" << slot << ")";
      }
      out << std::endl;
    }
    out << std::endl;
  }
  out.flush();
}

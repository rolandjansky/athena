#include "LArRecConditions/LArBadChannelCont.h"

//Constructor with payload
LArBadChannelCont::LArBadChannelCont( const BadChanVec& vec): 
  m_cont(vec) {
  sort();
}

//Add a bad channel to the list of bad channels
void LArBadChannelCont::add(const HWIdentifier hwid, const LArBadChannel stat) {
  m_cont.push_back(std::make_pair(hwid.get_identifier32().get_compact(),stat));
  return;
}


 //Sorting method (call before recording!) 
void LArBadChannelCont::sort() {
  std::sort(m_cont.begin(), m_cont.end(), ChannelLess());

  //Merge duplicate entries is needed ...
  if (m_cont.size()>1) { //got at least 2 entries
    auto it=m_cont.begin();
    while (it+1!=m_cont.end()) {
      auto it2=it+1;
      if (it->first==it2->first) {//Same channel number
	it2->second|=it->second;  //Merge (logical-or) bad channel status ...
	it=m_cont.erase(it);   //... and delete first instance
      }
      else
	++it;
    }
  }//end if got at least 2 entries
}  

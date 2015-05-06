/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ToString.h"
#include <sstream>


namespace {

	template<typename T>
		std::string ToString(const std::vector<T>& vv){
		std::stringstream o;
		std::string delim = "";
		for(auto val: vv){
			o << delim << int(val) << /*"(0x" << std::hex << int(val) << ")" << */  std::dec;
			delim ="|";
		}
		return o.str();
	}
}

std::string LVL1BS::ToString(const xAOD::TriggerTower& tt){
	std::stringstream o;
	if(tt.coolId() == 0) {
		o << "xAOD::TriggerTower, coolId=0";
	} else {
		o << "xAOD::TriggerTower, coolId=" << std::hex << tt.coolId() << std::dec
		  << ", layer=" << int(tt.layer())
                  << ", sampling=" << int(tt.sampling())
		  << ", eta=" << tt.eta()
		  << ", phi=" << tt.phi()
		  << ", lut_cp=" << ::ToString(tt.lut_cp())
		  << ", lut_jep=" << ::ToString(tt.lut_jep())
		  << ", correction=" << ::ToString(tt.correction())
		  << ", correctionEnabled=" << ::ToString(tt.correctionEnabled())
		  << ", bcidVec=" << ::ToString(tt.bcidVec())
		  << ", adc=" << ::ToString(tt.adc())
		  << ", bcidExt=" << ::ToString(tt.bcidExt())
		  << ", errorWord=" << tt.errorWord()
		  << ", peak=" << int(tt.peak())
		  << ", adcPeak=" << int(tt.adcPeak());
		  if (!tt.lut_cp().empty()) {
			o << ", cpET=" << int(tt.cpET());
		  }
		  if (!tt.lut_jep().empty()) {
		  	o << ", jepET=" << int(tt.jepET());
		  }
		  
	}

	return o.str();
}

std::string LVL1BS::ToString(const xAOD::TriggerTowerContainer& tt){
	std::stringstream o;
	for(xAOD::TriggerTowerContainer::const_iterator iter = tt.begin();
			iter != tt.end(); ++iter){
		o << ToString(**iter) << std::endl;
	}
	return o.str();
}

std::string LVL1BS::ToString(const xAOD::CPMTower& cpm){
  std::stringstream o;

  o << "xAOD::CPMTower"
    << ", eta=" << cpm.eta()
    << ", phi=" << cpm.phi()
    << ", em peak energy=" << cpm.emEnergy()
    << ", had peak energy=" << cpm.hadEnergy()
    ;

  return o.str();
}

std::string LVL1BS::ToString(const xAOD::CPMTowerContainer& tt){
  std::stringstream o;
  for(auto iter = tt.begin();
      iter != tt.end(); ++iter){
    o << ToString(**iter) << std::endl;
  }
  return o.str();
}


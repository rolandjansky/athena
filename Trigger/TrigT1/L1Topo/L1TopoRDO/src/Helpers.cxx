/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "L1TopoRDO/Helpers.h"

#include <vector>
#include <bitset>
#include <algorithm> 
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include <boost/io/ios_state.hpp>
#include "L1TopoRDO/L1TopoRDO.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/Fibre.h"
#include "L1TopoRDO/Status.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "L1TopoRDO/BlockTypes.h"

// Print function
std::ostream& operator<<(std::ostream& os, const L1TopoRDO& rdo) {
  // save ostream state; destructor will restore state when it goes out of scope
  boost::io::ios_all_saver ias(os); 
  os << "L1TopoRDO:\n";
  os << "     SourceID: " << L1Topo::formatHex8(rdo.getSourceID());
  if (rdo.isDAQModule()){
    os << " DAQ";
  } 
  else if (rdo.isROIModule()){
    os << " ROI";
  }
  os << "\n";
  os << "     Errors: " << rdo.getErrors() << "\n";
  os << "     Status words:\n";
  std::vector<uint32_t> status = rdo.getStatusWords();
  for(auto & word: status){
    os << "     " << L1Topo::formatHex8(word) << "\n";
  }
  os << "     Data words:\n";
  std::vector<uint32_t> data = rdo.getDataWords();
  for(auto & word: data){
    os << "     " << L1Topo::formatHex8(word) << " ";
    switch (L1Topo::blockType(word)){
    case L1Topo::BlockTypes::HEADER:
      {
	os << L1Topo::Header(word) << "\n";
	break;
      }
    case L1Topo::BlockTypes::FIBRE:
      {
	os << L1Topo::Fibre(word) << "\n";
	break;
      }
    case L1Topo::BlockTypes::STATUS:
      {
	os << L1Topo::Status(word) << "\n";
	break;
      }
    case L1Topo::BlockTypes::EM_TOB:
      {
	os << "     EM TOB\n";
	break;
      }
    case L1Topo::BlockTypes::TAU_TOB:
      {
	os << "     TAU TOB\n";
	break;
      }
    case L1Topo::BlockTypes::MUON_TOB:
      {
	os << "     MUON TOB\n";
	break;
      }
    case L1Topo::BlockTypes::JET1_TOB:
      {
	os << "     JET TOB crate 1\n";
	break;
      }
    case L1Topo::BlockTypes::JET2_TOB:
      {
	os << "     JET TOB crate 2\n";
	break;
      }
    case L1Topo::BlockTypes::ENERGY_TOB:
      {
	os << "     Energy TOB\n";
	break;
      }
    case L1Topo::BlockTypes::L1TOPO_TOB:
      {
	os << L1Topo::L1TopoTOB(word);
	break;
      }
    default:
      os << "\n";
      break;
    }
  }
  return os;
}

namespace L1Topo{
  
  std::string formatHex8(uint32_t word){
    std::ostringstream s;
    s << std::showbase << std::hex << std::internal << std::setfill ('0') << std::setw(10) << word << std::dec << std::noshowbase;
    return s.str();
  }

  std::string formatHex4(uint32_t word){
    std::ostringstream s;
    s << std::showbase << std::hex << std::internal << std::setfill ('0') << std::setw( 6 ) << word << std::dec << std::noshowbase;
    return s.str();
  }


  // this is out of date and should not be used
  unsigned int triggerBitIndex(uint32_t moduleId, L1Topo::L1TopoTOB c){
    uint32_t module = (moduleId >>4) & 0x1;
    uint32_t index = 64*module + 32*c.fpga() + 16*c.clock() + 8*c.index();
    return index;
  }

  // this reflects the actual CTP mapping and the CTP simulation with which L1Topo output is compared
  unsigned int triggerBitIndexNew(uint32_t moduleId, L1Topo::L1TopoTOB c, unsigned int bitIdx){
    uint32_t module = (moduleId >>4) & 0x1;
    uint32_t index = 64*module + 32*c.fpga() + c.clock() + 2*(8*c.index() + bitIdx);
    //std::cout << "L1Topo::triggerBitIndexNew DEBUG index=" << index << " for module=" << module << " fpga=" << c.fpga() << " clock=" << c.clock() << " index=" << c.index() << " bitIdx=" << bitIdx << std::endl;
    return index;
  }

  // convenience wrapper for use with L1TopoResult - can't be included here as is creates dependency loop with TrigT1Result
  /*
  std::pair< std::bitset<128>,std::bitset<128> > getDecisionAndOverflowBits(const std::vector<L1TopoResult>& res){
    L1TopoRDOCollection col;
    for (auto & r : res){
      col.push_back(new L1TopoRDO(r.rdo()));
    }
    return getDecisionAndOverflowBits(col);
  }
  */

  std::pair< std::bitset<128>,std::bitset<128> > getDecisionAndOverflowBits(const L1TopoRDOCollection& col){
    
    // There is no uint128_t so a bit-wise approach is needed.
    std::bitset<128> decision;
    std::bitset<128> overflow;

    //std::cout << "L1Topo::getDecisionAndOverflowBits: number of RDOs " << col.size() << std::endl;
    
    for (L1TopoRDOCollection::const_iterator pRDO = col.begin(); pRDO != col.end(); ++pRDO){
      const std::vector<uint32_t> data = (*pRDO)->getDataWords();
      //std::cout << "L1Topo::getDecisionAndOverflowBits: number of data words " << data.size() << std::endl;
      for(auto & word: data){
	if (L1Topo::blockType(word) == L1Topo::BlockTypes::L1TOPO_TOB){
	  L1Topo::L1TopoTOB c(word);
	  const uint32_t triggerByte = c.trigger_bits();
	  const uint32_t overflowByte = c.overflow_bits();
	  // Take one bit at a time and set it in the bitset
	  //std::cout << "L1Topo::getDecisionAndOverflowBits: " << c;
	  for (unsigned int i=0; i<8; ++i){
            const unsigned int index = L1Topo::triggerBitIndexNew((*pRDO)->getSourceID(),c,i);
	    decision[index]=( triggerByte >>i)&1;
	    overflow[index]=(overflowByte >>i)&1;
	  }
	  //std::cout << " index " << index << " updated decision " << decision << std::endl;
	  //std::cout << " index " << index << " updated overflow " << overflow << std::endl;
	}
	else{
	  //std::cout << "L1Topo::getDecisionAndOverflowBits: skipping block as not L1Topo_TOB " << L1Topo::formatHex8(word) << std::endl;
	}
      }
    }  
    return std::pair< std::bitset<128>,std::bitset<128> >(decision,overflow);
  }

} // namespace L1Topo

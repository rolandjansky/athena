/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCombinedEventTPCnv
 * @Class  : TrigComposite_p1
 *
 * @brief persistent partner for TrigComposite object
 *
 * @author Camille B.-Champagne  <camille.belanger-champagne@cern.ch> 
 **********************************************************************************/
#ifndef TRIGCOMBINEDEVENTTPCNV_TRIGCOMPOSITE_P1_H
#define TRIGCOMBINEDEVENTTPCNV_TRIGCOMPOSITE_P1_H


#include "TrigCombinedEvent/TrigComposite.h"
#include "AthenaPoolUtilities/TPObjRef.h"


//// needed to fix a Gaudi problem in SLC3 (should be removed eventually):
#include <stdint.h>

#include <vector>

class TrigComposite_p1 {
  friend class TrigCompositeCnv_p1;
	   
 public:
	   
 // default constructor
  TrigComposite_p1() { }

 // default destructor
  ~TrigComposite_p1() { }
	
 private:
 
 //Storage for TrigFeatureLinks
  std::vector<uint32_t> m_clids;
  std::vector<uint16_t> m_stypes;
  std::vector<uint32_t> m_indexes; 
  std::vector<std::string> m_labels;
 
 //Storage for details
  std::vector<float> m_floats;
  std::vector<std::string> m_fnames;
  std::vector<int> m_ints;
  std::vector<std::string> m_inames;
  std::vector<std::string> m_strings;
  std::vector<std::string> m_snames;

  std::vector<std::vector<float> > m_v_floats;
  std::vector<std::string> m_v_fnames;
  std::vector<std::vector<int> > m_v_ints;
  std::vector<std::string> m_v_inames;
  std::vector<std::vector<std::string> > m_v_strings;
  std::vector<std::string> m_v_snames;

  std::string m_instance;
  //  bool m_f_locked;
  //  bool m_mustset;
  //  std::vector<bool> m_v_set;
  
};
	
#endif

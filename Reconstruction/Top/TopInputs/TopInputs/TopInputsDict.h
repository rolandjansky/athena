/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOP_INPUTS_DICT_H
#define TOP_INPUTS_DICT_H

#include "DataModel/ElementLinkVector.h"
#include <map>
#include <vector>
#include <utility>

#include "TopInputs/TopInputs.h"
#include "TopInputs/TopMuonMap.h"
#include "TopInputs/TopElectronMap.h"
#include "TopInputs/TopPhotonMap.h"
#include "TopInputs/TopTauJetMap.h"
#include "TopInputs/TopJetMap.h"
#include "TopInputs/TopOverlap.h"
#include "TopInputs/TopOverlapTrigger.h"
#include "TopInputs/TopOverlapVector.h"
#include "TopInputs/TopOverlapTriggerVector.h"
#include "DataModel/DataLink.h"
#include "MissingETEvent/MissingET.h"

// For muons
template class std::_Rb_tree_iterator<std::pair<const TopRec::TopMuonLink,TopRec::TopOverlapTrigger> >;
template class std::_Rb_tree_const_iterator<std::pair<const TopRec::TopMuonLink,TopRec::TopOverlapTrigger> >;
template class std::pair<std::_Rb_tree_iterator<std::pair<const TopRec::TopMuonLink,TopRec::TopOverlapTrigger> >,bool>;
template class std::map<TopRec::TopMuonLink, TopRec::TopOverlapTrigger>;
template class __gnu_cxx::__normal_iterator<TopRec::TopMuonLink*,std::vector<TopRec::TopMuonLink> >;
template class __gnu_cxx::__normal_iterator<const TopRec::TopMuonLink*,std::vector<TopRec::TopMuonLink> >;

// For Electrons
template class std::_Rb_tree_iterator<std::pair<const TopRec::TopElectronLink,TopRec::TopOverlapTrigger> >;
template class std::_Rb_tree_const_iterator<std::pair<const TopRec::TopElectronLink,TopRec::TopOverlapTrigger> >;
template class std::pair<std::_Rb_tree_iterator<std::pair<const TopRec::TopElectronLink,TopRec::TopOverlapTrigger> >,bool>;
template class std::map<TopRec::TopElectronLink, TopRec::TopOverlapTrigger>;
template class __gnu_cxx::__normal_iterator<TopRec::TopElectronLink*,std::vector<TopRec::TopElectronLink> >;
template class __gnu_cxx::__normal_iterator<const TopRec::TopElectronLink*,std::vector<TopRec::TopElectronLink> >;

// For Photons
template class std::_Rb_tree_iterator<std::pair<const TopRec::TopPhotonLink,TopRec::TopOverlap> >;
template class std::_Rb_tree_const_iterator<std::pair<const TopRec::TopPhotonLink,TopRec::TopOverlap> >;
template class std::pair<std::_Rb_tree_iterator<std::pair<const TopRec::TopPhotonLink,TopRec::TopOverlap> >,bool>;
template class std::map<TopRec::TopPhotonLink, TopRec::TopOverlap>;
template class __gnu_cxx::__normal_iterator<TopRec::TopPhotonLink*,std::vector<TopRec::TopPhotonLink> >;
template class __gnu_cxx::__normal_iterator<const TopRec::TopPhotonLink*,std::vector<TopRec::TopPhotonLink> >;

// For TauJets
template class std::_Rb_tree_iterator<std::pair<const TopRec::TopTauJetLink,TopRec::TopOverlap> >;
template class std::_Rb_tree_const_iterator<std::pair<const TopRec::TopTauJetLink,TopRec::TopOverlap> >;
template class std::pair<std::_Rb_tree_iterator<std::pair<const TopRec::TopTauJetLink,TopRec::TopOverlap> >,bool>;
template class std::map<TopRec::TopTauJetLink, TopRec::TopOverlap>;
template class __gnu_cxx::__normal_iterator<TopRec::TopTauJetLink*,std::vector<TopRec::TopTauJetLink> >;
template class __gnu_cxx::__normal_iterator<const TopRec::TopTauJetLink*,std::vector<TopRec::TopTauJetLink> >;

// For Jets
template class std::_Rb_tree_iterator<std::pair<const TopRec::TopJetLink,TopRec::TopOverlap> >;
template class std::_Rb_tree_const_iterator<std::pair<const TopRec::TopJetLink,TopRec::TopOverlap> >;
template class std::pair<std::_Rb_tree_iterator<std::pair<const TopRec::TopJetLink,TopRec::TopOverlap> >,bool>;
template class std::map<TopRec::TopJetLink, TopRec::TopOverlap>;
template class __gnu_cxx::__normal_iterator<TopRec::TopJetLink*,std::vector<TopRec::TopJetLink> >;
template class __gnu_cxx::__normal_iterator<const TopRec::TopJetLink*,std::vector<TopRec::TopJetLink> >;

// For Tracks
template class std::_Rb_tree_iterator<std::pair<const TopRec::TopTrackLink,TopRec::TopOverlap> >;
template class std::_Rb_tree_const_iterator<std::pair<const TopRec::TopTrackLink,TopRec::TopOverlap> >;
template class std::pair<std::_Rb_tree_iterator<std::pair<const TopRec::TopTrackLink,TopRec::TopOverlap> >,bool>;
template class std::map<TopRec::TopTrackLink, TopRec::TopOverlap>;
template class __gnu_cxx::__normal_iterator<TopRec::TopTrackLink*,std::vector<TopRec::TopTrackLink> >;
template class __gnu_cxx::__normal_iterator<const TopRec::TopTrackLink*,std::vector<TopRec::TopTrackLink> >;

namespace TopInputsDict {
  struct dict {
    TopRec::TopInputs              m_ti;
    
    TopRec::TopOverlap             m_to;
    TopRec::TopOverlapTrigger      m_tot;
    
    TopRec::TopMuonMap             m_tmm;
    TopRec::TopElectronMap         m_tem;
    TopRec::TopPhotonMap           m_tpm;
    TopRec::TopTauJetMap           m_ttjm;
    TopRec::TopJetMap              m_tjm;
    TopRec::TopOverlapVector       m_tov;
    TopRec::TopOverlapTriggerVector   m_totv;
  };
}
	
#endif

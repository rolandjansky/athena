/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <string>
#include <iostream>

#include "TTree.h"
#include "TBranch.h"
#include "TFile.h"

#include "EDM_object.h"

EDM_object::EDM_object () { 
	m_stationName = nullptr; 
	m_stationEta = nullptr;
	m_stationPhi = nullptr;
	m_multiplet = nullptr;
	m_gas_gap = nullptr;
	m_channel_type = nullptr;
	m_channel = nullptr;
	m_matchedchannel = nullptr;
	m_mismatches = 0;
	m_total = 0;
}

void EDM_object::clearVars () { 
	if (m_matchedchannel) {
		delete m_matchedchannel; 
		m_matchedchannel = nullptr; 
	}
}

size_t EDM_object::size() {
	if (!m_stationName) { return 0; }
	checksize();
	return m_stationName->size();
}

void EDM_object::checksize() {
   size_t channel_type_size;
   size_t size = m_stationName->size();
   //skip channel_type check if MM, as MM does not have this parameter.
   channel_type_size = (m_channel_type) ? m_channel_type->size() : size;
   if (! (m_stationEta->size() == size ||
       	 m_stationPhi->size() == size ||
       	 m_multiplet->size()  == size ||
       	 m_gas_gap->size() 	 == size ||
       	 channel_type_size    == size ||
       	 m_channel->size() 	 == size) ) 
   	{ printf("NSWMatchingAlg: Sizes of data object %s_%s do not match!\n", m_name.Data(), m_detector.Data()); }
}

void EDM_object::printInfo () {
	// sTGC
   if (m_channel_type) {
     printf("%-15s %-15s %-15s %-15s %-15s %-15s %-15s %-15s\n", "stationname", "stationEta", "stationPhi", "multiplet", "gas_gap", "channel_type", "channel", "matchedchannel");
      for (uint i = 0; i < this->size(); ++i)
         { printf("%-15s %-15d %-15d %-15d %-15d %-15d %-15d %-15d\n", m_stationName->at(i).data(), m_stationEta->at(i), m_stationPhi->at(i), m_multiplet->at(i), m_gas_gap->at(i), m_channel_type->at(i), m_channel->at(i), m_matchedchannel->at(i));}
   // MM
   } else {
      printf("%-15s %-15s %-15s %-15s %-15s %-15s %-15s\n", "stationname", "stationEta", "stationPhi", "multiplet", "gas_gap", "channel", "matchedchannel");
      for (uint i = 0; i < this->size(); ++i)
         { printf("%-15s %-15d %-15d %-15d %-15d %-15d %-15d\n", m_stationName->at(i).data(), m_stationEta->at(i), m_stationPhi->at(i), m_multiplet->at(i), m_gas_gap->at(i), m_channel->at(i), m_matchedchannel->at(i));}
   }
}

bool EDM_object::identifierMatch(EDM_object &data0, EDM_object &data1, uint i, uint j) {
	bool match = true;
	match &= data0.m_channel_type ?  data0.m_channel_type->at(i) == data1.m_channel_type->at(j) : true;
	match &= data0.m_stationName->at(i).compare(data1.m_stationName->at(j)) == 0;
	match &= data0.m_stationEta->at(i) == data1.m_stationEta->at(j);
	match &= data0.m_stationPhi->at(i) == data1.m_stationPhi->at(j);
	match &= data0.m_multiplet->at(i)  == data1.m_multiplet->at(j);
	match &= data0.m_gas_gap->at(i) 	== data1.m_gas_gap->at(j);
	return match;
}

bool EDM_object::update_match(int index, int ch_candidate) {
	// make sure the machedchannel is set to the default value (-10)
	if (!m_matchedchannel) { return false; }
	// If any match found, overwarite default value
	if ( m_matchedchannel->at(index) == -10 ) { m_matchedchannel->at(index) = ch_candidate; }
	// Default channel value in digitization = -1
	if ( ch_candidate == -1 ) { return false; }
	// Check if the match is close enough
	if (abs(ch_candidate - m_channel->at(index)) <  abs(m_channel->at(index) - m_matchedchannel->at(index)) ) { 
		m_matchedchannel->at(index) = ch_candidate;
		return true; 
	}
	return false;
}

void EDM_object::init_matching () {
	if (empty()) { return; }
	m_matchedchannel = new std::vector<int>(this->size());
	for ( uint i = 0; i < this->size(); ++i) { m_matchedchannel->at(i) = -10; }
}

bool EDM_object::empty () {
	if (!m_stationName) { return true; }
	if (m_stationName->empty()) { return true; }
	return false; 
}

void EDM_object::update_efficiency ( int maximum_difference ) {
	uint nMatches = 0;
	size_t n_obj = size();
	m_total += n_obj;
	for (uint i = 0; i < n_obj; ++i) {
		nMatches += abs( m_channel->at(i) - m_matchedchannel->at(i) ) <= maximum_difference;
	}
	m_mismatches += (n_obj - nMatches);
}

void EDM_object::printEfficiency(std::ofstream& file) {
	file << "\nMatching " << m_name << " to " << m_matchedwith << " for " << m_detector << std::endl;
	file << "Total: " << m_total << ", number of mismatches: " << m_mismatches << std::endl;
	file << "Efficiency: " << (m_total - m_mismatches) / (double)m_total * 100. << "%" << std::endl;
}

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EDM_OBJECT_H
#define EDM_OBJECT_H

#include "TString.h"

#include <fstream>
#include <vector>

class EDM_object
{
public:
	// Matching
	void init_matching();
	bool update_match(int index, int ch_candidate);
	bool identifierMatch(EDM_object &data0, EDM_object &data1, uint i, uint j);

	// Efficiency	
	void update_efficiency ( int maximum_difference );
	void printEfficiency (std::ofstream& file);

	// setters
	void setName (TString name) { m_name = name; }
	void setName (TString name, TString detector) { m_name = name; m_detector = detector; }
	void setDetector (TString detector) { m_detector = detector; }
	void setMatchedwith (TString matchedwith) { m_matchedwith = matchedwith; }

	// getters 
	TString getName () { return m_name; }
	TString getDetector () { return m_detector; }
	TString getMatchedwith () { return m_matchedwith; }

	// Helper functions
	size_t size ();
	void checksize ();
	void printInfo();
	void clearVars();
	bool empty ();

	EDM_object ();
	~EDM_object () { clearVars(); }

	std::vector<std::string>  *m_stationName;
	std::vector<int>     *m_stationEta;
	std::vector<int>     *m_stationPhi;
	std::vector<int>     *m_multiplet;
	std::vector<int>     *m_gas_gap;
	std::vector<int>     *m_channel_type;
	std::vector<int>     *m_channel;

	std::vector<int>     *m_matchedchannel;

private:
	TString m_name;
	TString m_matchedwith;
	TString m_detector;
	int m_mismatches;
	int m_total;
};

#endif
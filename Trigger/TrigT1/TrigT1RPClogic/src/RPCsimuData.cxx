/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/RPCsimuData.h"

using namespace std;

RPCsimuData::RPCsimuData() :
    RPCdata("RPCsimuData")
{
    m_muon_tracks.clear();
    m_eta_digits.clear();
    m_phi_digits.clear();
}

RPCsimuData::RPCsimuData(const RPCsimuData& data) :
    RPCdata(data)
{
    m_muon_tracks = data.muon_tracks();
    m_eta_digits = data.eta_digits();
    m_phi_digits = data.phi_digits();
}

RPCsimuData::~RPCsimuData()
{
    m_muon_tracks.clear();
    m_eta_digits.clear();
    m_phi_digits.clear();    
}

RPCsimuData
RPCsimuData::operator=(const RPCsimuData& data)
{
    m_muon_tracks.clear();
    m_eta_digits.clear();
    m_phi_digits.clear();   

    m_muon_tracks = data.muon_tracks();
    m_eta_digits = data.eta_digits();
    m_phi_digits = data.phi_digits(); 

    return *this;
}

void
RPCsimuData::load_data(MuonSimuTrack& muon_track)
{
    muon_track.set_number(m_muon_tracks.size()+1);
    MuonList::const_iterator it = m_muon_tracks.begin();
    while(it != m_muon_tracks.end())
    {
	if((*it).track_number() != muon_track.track_number()) return;
	++it;
    }
    m_muon_tracks.push_back(muon_track);
}

void
RPCsimuData::load_data(RPCsimuDigit& digit)
{
    digit.set_data(*this);
    if(digit.decoding().view() == Eta)
    {
	digit.set_number(m_eta_digits.size()+1);
	m_eta_digits.push_back(digit);
    }
    else if(digit.decoding().view() == Phi)
    {
	digit.set_number(m_phi_digits.size()+1);
	m_phi_digits.push_back(digit);
    }
}

RPCsimuData& 
RPCsimuData::operator<<(MuonSimuTrack& muon_track)
{
    this->load_data(muon_track);
    return *this;
}

RPCsimuData& 
RPCsimuData::operator<<(RPCsimuDigit& rpc_digit)
{
    this->load_data(rpc_digit);
    return *this;
}

RPCdata::digitList
RPCsimuData::eta_digits_list(void) const
{
    digitList list;
    DigitList::const_iterator it = m_eta_digits.begin();
    while(it != m_eta_digits.end())
    {
        list.push_back(&(*it));
	++it;
    }
    return list;
}

RPCdata::digitList
RPCsimuData::phi_digits_list(void) const
{
    digitList list;
    DigitList::const_iterator it = m_phi_digits.begin();
    while(it != m_phi_digits.end())
    {
        list.push_back(&(*it));
	++it;
    }
    return list;
}

int
RPCsimuData::how_many(int type,int sector,int station,int layer,int chamber,
		      int strip) const
{
    int size = 0;

    if(type < 0 || type == 1)
    {
        DigitList::const_iterator it = m_phi_digits.begin();
        while(it != m_phi_digits.end())
        {
	  if( (sector  < 0 || sector  == (*it).decoding().logic_sector()) &&
              (station < 0 || station == (*it).decoding().lvl1_station()) &&
              (layer   < 0 || layer   == (*it).decoding().rpc_layer()   ) &&
	      (chamber < 0 || layer   == (*it).decoding().rpc_z_index() ) &&
	      (strip   < 0 || layer   == (*it).decoding().strip_number()) )
	    {
	        ++size;
	    }
	    ++it;
        }
    }

    if(type < 0 || type == 2)
    {
        DigitList::const_iterator it = m_eta_digits.begin();
        while(it != m_eta_digits.end())
        {
	  if( (sector  < 0 || sector  == (*it).decoding().logic_sector()) &&
              (station < 0 || station == (*it).decoding().lvl1_station()) &&
              (layer   < 0 || layer   == (*it).decoding().rpc_layer()   ) &&
	      (chamber < 0 || layer   == (*it).decoding().rpc_z_index() ) &&
	      (strip   < 0 || layer   == (*it).decoding().strip_number()) )
	    {
	        ++size;
	    }
	    ++it;
        }
    }

    return size;
}

void
RPCsimuData::PrintElement(ostream& stream,std::string element,bool detail) const
{
    bool all  = (element == name() || element == "")? true : false;
    bool nTracks = m_muon_tracks.size();
    bool nEtaDig = m_eta_digits.size();
    bool nPhiDig = m_phi_digits.size();
    bool printed = false;

    if(nTracks && (element == (*m_muon_tracks.begin()).name() || all))
    {
        printed = true;
	MuonList::const_iterator it = m_muon_tracks.begin();
	while(it != m_muon_tracks.end())
        {
            it->Print(stream,detail);
	    ++it;
	}
    }

    if (nEtaDig && (element == (*m_eta_digits.begin()).name() || all))
    {
        printed = true;
        DigitList::const_iterator it = m_eta_digits.begin();
	while(it != m_eta_digits.end())
        {
            it->Print(stream,detail);
	    ++it;
	}
    }

    if (nPhiDig && (element == (*m_phi_digits.begin()).name() || all))
    {
        printed = true;
        DigitList::const_iterator it = m_phi_digits.begin();
	while(it != m_phi_digits.end())
        {
            it->Print(stream,detail);
	    ++it;
	}        
    }

    if(!printed)
    {
        if( element == "") element = "digits";
        stream << name() << " contains no " << element << "!" << endl;
    }
}


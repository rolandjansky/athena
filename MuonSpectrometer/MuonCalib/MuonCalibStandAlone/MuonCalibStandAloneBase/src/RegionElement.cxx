/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibStandAloneBase/RegionElement.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "sstream"
#include <cstdlib>

namespace MuonCalib {	
	
bool RegionElement::Initialize(std::string & region)
	{
	m_region = region;
	std::string inner_region(region, 1, region.size()-2);
//clear all - empty means all chambers
	m_stations.clear();
	m_eta_start.clear();m_eta_end.clear();
	m_phi_start.clear(); m_phi_end.clear();
	m_ml=0;
//loop on the four elements of a region element
	std::istringstream region_stream(inner_region);
	std::string item;
	for(int i=0; i<4; i++)		
		{
		getline(region_stream, item ,',');
		if(region_stream.fail()) break;
	//split in subelements
		std::string item2;
		std::istringstream item_stream(item);
		while(!item_stream.eof())	
			{
			item_stream>>item2;
			if(item2.size() == 0) continue;
			switch(i)
				{
				case 0:
					if(!process_station_name(item2)) return false;
					break;
				case 1:
					if(!process_int_range(item2, m_phi_start, m_phi_end)) return false;
					break;
				case 2: 
					if(!process_int_range(item2, m_eta_start, m_eta_end)) return false;
					break;
				case 3:
					{
					m_ml=atoi(item2.c_str());
					if(m_ml<0 || m_ml>2) return false;
					break;
					}
				}
			}
		}
	return true;
	}

void RegionElement::Print(std::ostream & os) const
	{
	os<<m_region;
	}

bool RegionElement::Result(const MuonFixedId & id) const
	{
//check for station
	if(m_stations.size()!=0 && m_stations.find(id.stationName())==m_stations.end())
		return false;
//check for eta
	if(m_eta_start.size()>0)
		{
		bool match(false);
		for(unsigned int i=0; i<m_eta_start.size(); i++)
			{
			if(m_eta_start[i]<=id.eta() && id.eta()<=m_eta_end[i])
				{
				match=true;
				break;
				}
			}
		if(!match) return false;
		}
//check for phi
	if(m_phi_start.size()>0)
		{
		bool match(false);
		for(unsigned int i=0; i<m_phi_start.size(); i++)
			{
			if(m_phi_start[i]<=id.phi() && id.phi()<=m_phi_end[i])
				{
				match=true;
				break;
				}
			}
		if(!match) return false;
		}
//check for multilayer
	if(m_ml>0)
		{
		if(id.mdtMultilayer()!=m_ml) return false;
		}
	return true;
	}

bool RegionElement::process_station_name(std::string & substr)
	{
	if(substr.size() > 3) {
		MsgStream log(Athena::getMessageSvc(),"RegionElement");
		log<<MSG::WARNING<<"Syntax Error '"<<substr<<"'"<<endmsg;
		return false;
	}
	std::string name_template("???");
	for(unsigned int i=0; i<substr.size(); i++)
		{
		name_template[i]=substr[i];
		}	
//get matching stations
	MuonFixedId  id;
	std::string station_name;
	for(int i=1; true; i++)
		{
		station_name=id.stationNumberToFixedStationString(i);
		if(station_name=="XXX") break;
		bool match(true);
		for(int j=0; j<3; j++)
			{
			if(name_template[j]=='?') continue;
			if(name_template[j]==station_name[j]) continue;
			match=false;
			}
		if(match) 
			{
			m_stations.insert(i);
			}
		}
	return true;
	}

bool RegionElement::process_int_range(std::string &substr, std::vector<int> &target_start, std::vector<int> &target_end)
	{
//check for - in substring
	std::string begin_range[2];
	int n_substrings(0);
	bool begin_number(true);
	for(unsigned int i=0; i<substr.size(); i++)
		{
		switch (substr[i])
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				begin_range[n_substrings]+=substr[i];
				begin_number=false;
				break;
			case '-':
				if(begin_number)
					{
					begin_range[n_substrings]+=substr[i];
					begin_number=false;
					break;
					}
				if(n_substrings>0) {
					MsgStream log(Athena::getMessageSvc(),"RegionElement");
					log<<MSG::WARNING<<"Surplus '-' in "<<substr<<endmsg;
					return false;
				}
				begin_number=true;	
				n_substrings=1;
				break;
			default:
				MsgStream log(Athena::getMessageSvc(),"RegionElement");
				log<<MSG::WARNING<<"Syntax error in "<<substr<<endmsg;
				return false;
			}
		}
	if(begin_range[0].size()==0)
		{
		return true;
		}
	target_start.push_back(atoi(begin_range[0].c_str()));
	if(n_substrings==0)
		target_end.push_back(atoi(begin_range[0].c_str()));
	else
		target_end.push_back(atoi(begin_range[1].c_str()));
	return true;
	}
}

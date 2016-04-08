/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include <string>
#include "RPCcablingInterface/CMAidentity.h"

using namespace std;

const char CMAidentity::CoverageTAG[3][5] = {{"even"},{"odd"},{""}};

bool
CMAidentity::coverage(std::string side,CMAcoverage& cov)
{
    for(int i=EvenSectors ; i<= AllSectors; ++i)
    {
        std::string tag(CoverageTAG[i]);
        if(side == tag) 
        { 
            cov = (CMAcoverage)i;
            return true;
	}
    }
    return false;
}

const std::string
CMAidentity::name(const ViewType view,const CMAcoverage coverage)
{
    std::string cover(CMAidentity::covtag(coverage));
    std::string side  = (view == Eta)? "Eta" : "Phi";
    std::string name = cover + side + "CMA";
    //cout << name << endl;
    return name; 
}

CMAidentity::CMAidentity(ViewType view, CMAcoverage side, int number,
                         int eta_index,int phi_index,
			 int PAD_index, int Ixx_index)
{
    m_type = view;
    m_coverage = side;
    m_number = number;
    m_eta_index = eta_index;
    m_phi_index = phi_index;
    m_PAD_index = PAD_index;
    m_Ixx_index = Ixx_index;
}

CMAidentity::CMAidentity(ViewType view, CMAcoverage side, int number)
{
    m_type = view;
    m_coverage = side;
    m_number = number;
    m_eta_index = -1;
    m_phi_index = -1;
    m_PAD_index = -1;
    m_Ixx_index = -1;
}

CMAidentity::CMAidentity(ViewType view, CMAcoverage side, 
                         Offline_indexes& indexes)
{
    m_type = view;
    m_coverage = side;
    m_number = -1;
    m_eta_index = indexes.first;
    m_phi_index = indexes.second;
    m_PAD_index = -1;
    m_Ixx_index = -1;
}

CMAidentity::CMAidentity(ViewType view, CMAcoverage side, int PAD, int Ixx)
{
    m_type = view;
    m_coverage = side;
    m_number = -1;
    m_eta_index = -1;
    m_phi_index = -1;
    m_PAD_index = PAD;
    m_Ixx_index = Ixx;
}

CMAidentity::CMAidentity(const CMAidentity& id)
{
    m_type = id.type();
    m_coverage = id.coverage();
    m_number = id.number();
    m_eta_index = id.eta_index();
    m_phi_index = id.phi_index();
    m_PAD_index = id.PAD_index();
    m_Ixx_index = id.Ixx_index();
}

CMAidentity&
CMAidentity::operator=(const CMAidentity& id)
{
    if (this!=&id) {
      m_type = id.type();
      m_coverage = id.coverage();
      m_number = id.number();
      m_eta_index = id.eta_index();
      m_phi_index = id.phi_index();
      m_PAD_index = id.PAD_index();
      m_Ixx_index = id.Ixx_index();
    }
    return *this;
}

bool
CMAidentity::operator==(const CMAidentity& id) const
{
    //cout << "I am:    "<< *this << endl;
    //cout << "You are: "<< id << endl;
    if(m_type == id.type() && m_coverage == id.coverage())
    {
        if(m_number >= 0)    if(m_number == id.number())        return true;
	
	//cout << "Check number" << endl;
        if(m_eta_index >= 0) if(m_eta_index==id.eta_index() &&
                                m_phi_index==id.phi_index())    return true;
				
	//cout << "Check Eta Phi" << endl;			
        if(m_PAD_index >= 0) if(m_PAD_index==id.PAD_index() &&
                                m_Ixx_index==id.Ixx_index())    return true;
	//cout << "Check PAD Ixx" <<endl;
    }
    //cout << " ---- IS WRONG ----" << endl;
    return false;
}

bool
CMAidentity::operator!=(const CMAidentity& id) const
{
    return !(*this == id);
}

bool
CMAidentity::operator<(const CMAidentity& id) const
{
    if(*this == id) return false;
    if( m_type < id.type() ) return true;
    if( m_coverage < id.coverage() ) return true;
    int id1 = m_eta_index    * 10 + m_phi_index;
    int id2 = id.eta_index() * 10 + id.phi_index();
    //int id1 = m_PAD_index    * 10 + m_Ixx_index;
    //int id2 = id.PAD_index() * 10 + id.Ixx_index();
    if(id1 < id2) return true;
    return false;
}

CMAidentity&
CMAidentity::operator+=(const CMAidentity& id)
{
    if ( *this != id ) return *this;
    if (m_number == -1 ) m_number = id.number();
    if (m_eta_index == -1 ) m_eta_index = id.eta_index();
    if (m_phi_index == -1 ) m_phi_index = id.phi_index();
    if (m_PAD_index == -1 ) m_PAD_index = id.PAD_index();
    if (m_Ixx_index == -1 ) m_Ixx_index = id.Ixx_index();
    return*this;
}

void
CMAidentity::inversion()
{
    m_Ixx_index = (m_Ixx_index==0)? 1 : 0;
}

ostream& operator<<(ostream& stream, const CMAidentity& id)
{
    char exttag[5] = {' ',' ',' ',' ','\0'};
    const char* tag = CMAidentity::covtag(id.m_coverage);
    for(int i = 0; i<5; ++i) 
    {
        if(tag[i] == '\0') break;
        exttag[i] = tag[i];
    }

    std::string covtag(exttag);    

    std::string type = (id.type() == Phi )?" phi cma " + covtag : " eta cma " + covtag;
    if(covtag != "" ) type = type + " ";
    stream << type << setw(2) << id.number() << ": Eta ";
    stream << setw(2) << id.eta_index();
    stream << "  Phi " << setw(2) << id.phi_index();
    stream << "  PAD " << setw(2) << id.PAD_index();
    stream << "  Ixx " << setw(2) << id.Ixx_index();
    return stream;
}

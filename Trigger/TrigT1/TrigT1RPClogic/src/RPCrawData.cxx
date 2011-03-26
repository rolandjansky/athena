/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1RPClogic/RPCrawData.h"

using namespace std;

RPCrawData::RPCrawData() :
    RPCdata("RPCrawData")
{
    m_eta_digits.clear();
    m_phi_digits.clear();
}

RPCrawData::RPCrawData(const RPCrawData& data) :
    RPCdata(data)
{
    m_eta_digits = data.eta_digits();
    m_phi_digits = data.phi_digits();
}

RPCrawData::~RPCrawData()
{
    m_eta_digits.clear();
    m_phi_digits.clear();    
}

RPCrawData
RPCrawData::operator=(const RPCrawData& data)
{
    m_eta_digits.clear();
    m_phi_digits.clear();   

    m_eta_digits = data.eta_digits();
    m_phi_digits = data.phi_digits(); 

    return *this;
}


const RPCrawDigit*
RPCrawData::load_data(RPCrawDigit& digit)
{
    digit.set_data(*this);
    if(digit.decoding().view() == Eta)
    {
        RPCrawData::DigitList::const_iterator dig = m_eta_digits.begin();
	while(dig != m_eta_digits.end())
	{
	    if(*dig == static_cast<RPCdigit&>(digit)) return &(*dig);
	    ++dig;
	}
	digit.set_number(m_eta_digits.size()+1);
	m_eta_digits.push_back(digit);
	return &m_eta_digits.back();
    }
    else if(digit.decoding().view() == Phi)
    {
        RPCrawData::DigitList::const_iterator dig = m_phi_digits.begin();
	while(dig != m_phi_digits.end())
	{
	    if(*dig == static_cast<RPCdigit&>(digit)) return &(*dig);
	    ++dig;
	}    
	digit.set_number(m_phi_digits.size()+1);
	m_phi_digits.push_back(digit);
	return &m_phi_digits.back();
    }
    return 0;
}


const RPCrawDigit* 
RPCrawData::operator<<(RPCrawDigit& rpc_digit)
{
    return this->load_data(rpc_digit);
}

RPCdata::digitList
RPCrawData::eta_digits_list(void) const
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
RPCrawData::phi_digits_list(void) const
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
RPCrawData::how_many(int type,int sector,int station,int layer,int chamber,
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
	      (chamber < 0 || chamber == (*it).decoding().rpc_z_index() ) &&
	      (strip   < 0 || strip   == (*it).decoding().strip_number()) )
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
	      (chamber < 0 || chamber == (*it).decoding().rpc_z_index() ) &&
	      (strip   < 0 || strip   == (*it).decoding().strip_number()) )
	    {
	        ++size;
	    }
	    ++it;
        }
    }

    return size;
}

void
RPCrawData::PrintElement(ostream& stream,std::string element,bool detail) const
{
    bool all  = (element == name() || element == "")? true : false;
    bool nEtaDig = m_eta_digits.size();
    bool nPhiDig = m_phi_digits.size();
    bool printed = false;

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


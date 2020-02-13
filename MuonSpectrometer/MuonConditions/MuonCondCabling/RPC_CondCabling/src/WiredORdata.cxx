/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "RPC_CondCabling/WiredORdata.h"

using namespace RPC_CondCabling;

WiredORdata::WiredORdata() : BaseObject(Logic,"Wired OR Data")
{
    m_type = -1;
    m_station = -1;
    reset_data();
}

WiredORdata::WiredORdata(DBline& data, int type) :
    BaseObject(Logic,"Wired OR Data")
{
    m_type = type;
    m_station = -1;
    reset_data();
    if(!(data("station") >> m_station)) return;


    m_fail = false;

    (++data)("{");
    do
    {
        if(get_data(data))
	{
	    WiredOR Wor(m_number,m_station,m_type,m_start,m_stop);
            m_wor.push_back(Wor);
	}
        data++;
    }while(!data("}"));
}

WiredORdata::~WiredORdata()
{
    m_wor.clear();
}

void
WiredORdata::reset_data()
{
    m_number = -1;
    m_start  = -1;
    m_stop   = -1;

    m_fail = true;
}

bool
WiredORdata::confirm_boundary() const
{
    if(m_start > m_stop )
    {
        __osstream display;

        display << "WORdata error in configuration for Sector Type " << m_type
                << ", station " <<  m_station << ", WOR number "
                << m_number << std::endl
                << " start RPC chamber (" << m_start << ") is greater than "
                << "stop RPC chamber (" << m_stop << ")" << std::endl;
        display_error(display);
	return false;
    }
    return true;
}

bool
WiredORdata::get_data(DBline& data)
{
    reset_data();
    if(data("hard wired phi chamber") >> m_number >> m_start >> "-" >> m_stop)
    {
	m_fail = false;
        if( !confirm_boundary() ) m_fail = true;
    }

    return !m_fail;
}

std::unique_ptr<WiredOR>
WiredORdata::give_wor(void)
{
    if(m_wor.size())
    {
      std::unique_ptr<WiredOR> Wor(new WiredOR(m_wor.front()));
      m_wor.pop_front();
      return Wor;
    }
    return 0;
}

void
WiredORdata::Print(std::ostream& stream, bool detail) const
{
    stream << "Wired OR data of station n. " << m_station;
    stream << " belonging to sector type " << m_type << std::endl;
    stream << "It contains " << m_wor.size();
    stream << " Wired ORs:" << std::endl;
    std::list < WiredOR >::const_iterator it;
    for(it = m_wor.begin();it!=m_wor.end();++it) 
        stream << ShowRequest<WiredOR>(*it,detail); 
}

std::ostream& operator<< (std::ostream& stream, WiredORdata& data)
{
    data.Print(stream,false);
    return stream;
}

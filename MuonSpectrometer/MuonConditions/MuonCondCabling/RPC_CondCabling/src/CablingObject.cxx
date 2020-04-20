/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "RPC_CondCabling/CablingObject.h"

CablingObject::CablingObject(int num,int stat,int type,std::string name) : 
    BaseObject(Logic,name),m_number(num),m_station(stat),m_sector_type(type) {}

CablingObject::CablingObject(int num,int stat,int type,const char* name) : 
    BaseObject(Logic,name),m_number(num),m_station(stat),m_sector_type(type) {}

CablingObject::CablingObject(const CablingObject& obj) :
    BaseObject(Logic,obj.name())
{
    m_number = obj.number();
    m_station = obj.station();
    m_sector_type = obj.sector_type();
}

CablingObject&
CablingObject::operator=(const CablingObject& obj)
{
    if (this!=&obj) {
      m_number = obj.number();
      m_station = obj.station();
      m_sector_type = obj.sector_type();
    }
    return*this;
}

std::ostream& operator<< (std::ostream& stream,const CablingObject& obj)
{
    stream   << obj.name() << " number " << std::setw(3) << obj.number()
             << "  associated to sector type " << std::setw(3) 
	     << obj.sector_type() << " in station n. " << obj.station()
             << std::endl;
    return stream;
}

void
CablingObject::error_header(void) const
{

    __osstream disp;
    disp << "Error in Sector Type " << m_sector_type;
    if( m_station >0 ) disp << ", station " << m_station << ":" << std::endl;
    else disp << ":" << std::endl;

    display_error(disp);
}

void
CablingObject::no_connection_error(std::string name,int num)
{
    this->error_header();

    DISP << this->name() << " n. " << m_number
         << " is supposed to receive input from " << name << " n. " << num
         << " which doesn't exist!" << std::endl;
    DISP_ERROR;
}


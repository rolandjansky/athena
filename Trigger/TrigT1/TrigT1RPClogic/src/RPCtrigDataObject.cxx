/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include "TrigT1RPClogic/RPCtrigDataObject.h"

using namespace std;

RPCtrigDataObject::RPCtrigDataObject(int num,std::string name) : 
    BaseObject(Data,name),m_number(num) {}

RPCtrigDataObject::RPCtrigDataObject(int num,const char* name) : 
    BaseObject(Data,name),m_number(num) {}

RPCtrigDataObject::RPCtrigDataObject(const RPCtrigDataObject& obj) :
    BaseObject(Data,obj.name())
{
    m_number = obj.number();
}

RPCtrigDataObject
RPCtrigDataObject::operator=(const RPCtrigDataObject& obj)
{
    static_cast<BaseObject&>(*this) = static_cast<const BaseObject&>(obj);
    m_number = obj.number();
    return*this;
}

void 
RPCtrigDataObject::Print(ostream& stream,bool detail) const
{
    detail = true;
    if(detail)
    {
        stream << name() << " number " << setw(3) << number();
    }
    stream << endl;
}

void
RPCtrigDataObject::set_number(int number)
{
    m_number = number;
}

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <sstream>

template <class Type> class ShowData
{
    public:
    const Type* m_object;
    const std::string m_element;
    const bool m_detail;

    public:
    ShowData(const Type*,std::string,bool);
    ShowData(const Type&,std::string,bool);
    ~ShowData() {}
};

template <class Type>
ShowData<Type>::ShowData(const Type& obj,std::string element,bool detail) :
    m_object(&obj),m_element(element),m_detail(detail) {}


template <class Type>
ShowData<Type>::ShowData(const Type* obj,std::string element,bool detail) :
    m_object(obj),m_element(element),m_detail(detail) {}


template <class Type> std::ostream& operator<<
    (std::ostream& stream,const ShowData<Type>& data)
{
    std::ostringstream display;
    data.m_object->PrintElement(display,data.m_element,data.m_detail);
    stream << display.str();
    return stream;
}


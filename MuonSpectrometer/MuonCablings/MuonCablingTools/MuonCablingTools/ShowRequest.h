/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGTOOLS_PRINTREQ_H
#define MUONCABLINGTOOLS_PRINTREQ_H

#include <iostream>

#include <sstream>
typedef std::ostringstream __osstream;

template <class Type>
class ShowRequest
{
    public:
    const Type* m_object;
    bool  m_detail;

    public:
    ShowRequest(const Type&,bool);
    ShowRequest(const Type*,bool);
    ~ShowRequest() {}
    /*
    friend ostream& operator<< <Type> (ostream&,const ShowRequest<Type>&);
#ifndef LVL1_STANDALONE
    friend MsgStream& operator<< <Type> (MsgStream&,const ShowRequest<Type>&);
#endif
    */
};

template <class Type>
ShowRequest<Type>::ShowRequest(const Type& obj,bool detail) : 
    m_object(&obj),m_detail(detail) {}


template <class Type>
ShowRequest<Type>::ShowRequest(const Type* obj,bool detail) : 
    m_object(obj),m_detail(detail) {}



template <class Type> std::ostream& operator<<
    (std::ostream& stream,const ShowRequest<Type>& print)
{

    __osstream display;
    print.m_object->Print(display,print.m_detail);
    stream << display.str();
    return stream;
}

#ifndef LVL1_STANDALONE
#include "GaudiKernel/MsgStream.h"

template <class Type> MsgStream& operator<<
    (MsgStream& stream,const ShowRequest<Type>& print)
{
    __osstream display;
    print.m_object->Print(display,print.m_detail);
    stream << display.str();
    return stream;
}
#endif


#endif

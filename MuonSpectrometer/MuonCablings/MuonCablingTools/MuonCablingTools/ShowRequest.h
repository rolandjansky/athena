/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRINTREQ_H
#define PRINTREQ_H

#include <iostream>

// for compatibility between gcc2.95 and gcc3.2
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    #include <sstream>
    typedef std::ostringstream __osstream;
#else
    // put your gcc 2.95 specific code here
    #include <strstream>
    typedef ostrstream __osstream;
#endif


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

#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[50000];
    for (int i=0;i<50000;++i) buffer[i] = '\0';
    __osstream display(buffer,50000);
#endif

    print.m_object->Print(display,print.m_detail);
    stream << display.str();
    return stream;
}

#ifndef LVL1_STANDALONE
#include "GaudiKernel/MsgStream.h"

template <class Type> MsgStream& operator<<
    (MsgStream& stream,const ShowRequest<Type>& print)
{

#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[50000];
    for (int i=0;i<50000;++i) buffer[i] = '\0';
    __osstream display(buffer,50000);
#endif

    print.m_object->Print(display,print.m_detail);
    stream << display.str();
    return stream;
}
#endif


#endif

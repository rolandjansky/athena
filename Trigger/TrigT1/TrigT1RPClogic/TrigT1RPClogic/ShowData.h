/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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

#ifndef LVL1_STANDALONE
///#include "GaudiKernel/MsgStream.h"
#endif

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
    /*
    friend ostream& operator<< <Type> (ostream&,const ShowData<Type>&);
#ifndef LVL1_STANDALONE
////    friend MsgStream& operator<< <Type> (MsgStream&,const ShowData<Type>&);
#endif
    */ 
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
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    __osstream display;
#else
    // put your gcc 2.95 specific code here
    char buffer[500000];
    for (int i=0;i<500000;++i) buffer[i] = '\0';
    __osstream display(buffer,500000);
#endif

    data.m_object->PrintElement(display,data.m_element,data.m_detail);
    stream << display.str();
    return stream;
}

//// DRQ: This inserter causes problems with Gaudi v19. By commenting it out, the
//// DRQ: ostream-based one is used instead.
////#ifndef LVL1_STANDALONE
////template <class Type> MsgStream& operator<<
////    (MsgStream& stream,const ShowData<Type>& data)
////{
////#if (__GNUC__) && (__GNUC__ > 2) 
////    // put your gcc 3.2 specific code here
////    __osstream display;
////#else
////    // put your gcc 2.95 specific code here
////    char buffer[500000];
////    for (int i=0;i<500000;++i) buffer[i] = '\0';
////    __osstream display(buffer,500000);
////#endif
////
////    data.m_object->PrintElement(display,data.m_element,data.m_detail);
////    stream << display.str();
////    return stream;
////}
////#endif

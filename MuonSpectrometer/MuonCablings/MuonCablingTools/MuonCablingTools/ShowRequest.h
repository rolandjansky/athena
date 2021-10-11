/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGTOOLS_PRINTREQ_H
#define MUONCABLINGTOOLS_PRINTREQ_H

#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <sstream>

template <class Type> class ShowRequest {
public:
    const Type* m_object;
    bool m_detail;

public:
    ShowRequest(const Type&, bool);
    ShowRequest(const Type*, bool);
    ~ShowRequest() {}
};

template <class Type> ShowRequest<Type>::ShowRequest(const Type& obj, bool detail) : m_object(&obj), m_detail(detail) {}

template <class Type> ShowRequest<Type>::ShowRequest(const Type* obj, bool detail) : m_object(obj), m_detail(detail) {}

template <class Type> std::ostream& operator<<(std::ostream& stream, const ShowRequest<Type>& print) {
    std::ostringstream display;
    print.m_object->Print(display, print.m_detail);
    stream << display.str();
    return stream;
}

template <class Type> MsgStream& operator<<(MsgStream& stream, const ShowRequest<Type>& print) {
    std::ostringstream display;
    print.m_object->Print(display, print.m_detail);
    stream << display.str();
    return stream;
}

#endif

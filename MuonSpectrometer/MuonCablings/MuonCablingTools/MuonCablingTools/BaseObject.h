/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGTOOLS_LVL1OBJECT_H
#define MUONCABLINGTOOLS_LVL1OBJECT_H

#include <iostream>
#include <string>

enum ObjectType { Logic, Data, Hardware, Monitoring };

class BaseObject {
private:
    ObjectType m_tag;
    std::string m_name;

public:
    BaseObject(ObjectType, const std::string&);
    virtual ~BaseObject() = default;

    ObjectType tag() const { return m_tag; }
    std::string name() const { return m_name; }

    virtual void Print(std::ostream&, bool) const {}
};

#endif

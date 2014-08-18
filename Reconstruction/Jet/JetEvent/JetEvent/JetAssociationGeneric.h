/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETASSOCIATIONGENERIC_H
#define JETEVENT_JETASSOCIATIONGENERIC_H

#include "JetEvent/JetAssociationBase.h"
#include "Navigation/Navigable.h"

template <class T>
class JetAssociationGeneric : public JetAssociationBase,
                              public Navigable<T> {

    private:
    std::string m_name;
    T* m_obj;

    public:
    JetAssociationGeneric(const std::string& name, T* obj) :
            m_name(name), m_obj(obj) { }

    JetAssociationGeneric<T>* clone() const {
        return new JetAssociationGeneric<T>(m_name, m_obj);
    }

    T* get() const { return m_obj; }
    const std::string& name() const { return m_name; }
};

#endif

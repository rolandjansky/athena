///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VarHandleProperty.h 
// Header file for class VarHandleProperty
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef STOREGATE_VARHANDLEPROPERTY_H
#define STOREGATE_VARHANDLEPROPERTY_H 1

#include "StoreGate/VarHandleKeyProperty.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/UpdateHandle.h"

#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"


// ** Specializations of SimplePropertyRef for the VarHandle classes.

namespace Gaudi {
template<>
  class Property<SG::VarHandleBase&> : public SG::VarHandleKeyProperty {
public:
    Property(const std::string& name, SG::VarHandleBase& value) : 
    SG::VarHandleKeyProperty(name, value.vhKey()) {}
    virtual ~Property() {}
};

template<typename T>
  class Property<SG::ReadHandle<T>&> : public SG::VarHandleKeyProperty {
public:
    Property(const std::string& name, SG::ReadHandle<T>& value) : 
    SG::VarHandleKeyProperty(name, value.vhKey()) {}
    virtual ~Property() {}
};

template<typename T>
  class Property<SG::WriteHandle<T>&> : public SG::VarHandleKeyProperty {
public:
    Property(const std::string& name, SG::WriteHandle<T>& value) : 
    SG::VarHandleKeyProperty(name, value.vhKey()) {}
    virtual ~Property() {}
};

template<typename T>
  class Property<SG::UpdateHandle<T>&> : public SG::VarHandleKeyProperty {
public:
    Property(const std::string& name, SG::UpdateHandle<T>& value) : 
    SG::VarHandleKeyProperty(name, value.vhKey()) {}
    virtual ~Property() {}
};


template<typename T>
  class Property<SG::ReadCondHandle<T>&> : public SG::VarHandleKeyProperty {
public:
    Property(const std::string& name, SG::ReadCondHandle<T>& value) : 
    SG::VarHandleKeyProperty(name, value.vhKey()) {}
    virtual ~Property() {}
};

template<typename T>
  class Property<SG::WriteCondHandle<T>&> : public SG::VarHandleKeyProperty {
public:
    Property(const std::string& name, SG::WriteCondHandle<T>& value) : 
    SG::VarHandleKeyProperty(name, value) {}
    virtual ~Property() {}
};


}

// template<>
// class SimplePropertyRef< SG::VarHandleBase > :
//   public SG::VarHandleKeyProperty
// {
// public:
//   SimplePropertyRef(const std::string& name, SG::VarHandleBase& value) :
//     SG::VarHandleKeyProperty(name, value) {}
// };


// template<typename T>
// class SimplePropertyRef< SG::ReadHandle<T> > :
//   public SG::VarHandleKeyProperty
// {
// public:
//   SimplePropertyRef(const std::string& name, SG::ReadHandle<T>& value) :
//     SG::VarHandleKeyProperty(name, value) {}
// };


// template<typename T>
// class SimplePropertyRef< SG::UpdateHandle<T> > :
//   public SG::VarHandleKeyProperty
// {
// public:
//   SimplePropertyRef( const std::string& name, SG::UpdateHandle<T>& value ) :
//     SG::VarHandleKeyProperty(name, value) {}
// };


// template<typename T>
// class SimplePropertyRef< SG::WriteHandle<T> > :
//   public SG::VarHandleKeyProperty
// {
// public:
//   SimplePropertyRef( const std::string& name, SG::WriteHandle<T>& value ) :
//     SG::VarHandleKeyProperty(name, value) {}
// };

// template<typename T>
// class SimplePropertyRef< SG::ReadCondHandle<T> > :
//   public SG::VarHandleKeyProperty
// {
// public:
//   SimplePropertyRef(const std::string& name, SG::ReadCondHandle<T>& value) :
//     SG::VarHandleKeyProperty(name, value) {}
// };

// template<typename T>
// class SimplePropertyRef< SG::WriteCondHandle<T> > :
//   public SG::VarHandleKeyProperty
// {
// public:
//   SimplePropertyRef(const std::string& name, SG::WriteCondHandle<T>& value) :
//     SG::VarHandleKeyProperty(name, value) {}
// };


#endif /* !STOREGATE_VARHANDLEPROPERTY_H */


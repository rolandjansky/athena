/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEVENTPRIMITIVES_SURFACEHOLDER_H
#define TRKEVENTPRIMITIVES_SURFACEHOLDER_H

#include "TrkEventPrimitives/SurfaceUniquePtrT.h"

/**
Convenience Helper classes
SurfacePtrHolder
SurfaceUniqHolder
also see
SurfaceUniquePtrT.h

In the case of surfaces we do not want to
take onwership when they are owned by the
Trakcing Geometry or an element.

In the other hand we  want to take owneship if
they are free.

As a result when have classes with a m_associatedSurface
we need to include some boiler plate code.

These 2 helpers try to make that easier.

In reality one should prefer the one based
on SurfaceUniquePtr but we get this kind of
warning when creating
dictionaries:
Error in <CloseStreamerInfoROOTFile>: I/O is supported only
for unique_ptrs with a default deleter.
Trk::SurfaceUniqHolder::m_associatedSurface  appears to have a custom one
Which can be problematic for some cases. So we provide also
the plain ptr one

The test : SurfaceHolder_test
has a mock code on how we could use
these for classes where we want to hold
a ptr to a const Surface and manage
based on if its free or owned.

**/

namespace Trk {


template<typename S>
class SurfacePtrHolderImpl
{
public:
  /// default ctor
  SurfacePtrHolderImpl() = default;
  /// ctor from const Surface ref if surface is free
  /// we clone, otherwise we take its address.
  SurfacePtrHolderImpl(const S& s)
    : m_associatedSurface(s.isFree() ? s.clone() : &s)
  {}
  /// copy ctor, if surface is free we clone/copy.
  /// If not we just point to the one owned by detector geometry
  SurfacePtrHolderImpl(const SurfacePtrHolderImpl& other)
    : m_associatedSurface(other.m_associatedSurface &&
                              other.m_associatedSurface->isFree()
                            ? other.m_associatedSurface->clone()
                            : other.m_associatedSurface)
  {}
  /// Move constructor we just steal the resource and leave
  /// other to point to nullptr
  SurfacePtrHolderImpl(SurfacePtrHolderImpl&& other) noexcept
  {
    // tranfer ownership and leave other nullptr
    m_associatedSurface = other.m_associatedSurface;
    other.m_associatedSurface = nullptr;
  }

  /// copy assingmemnt if surface is free we clone/copy.
  /// If not we just point to the one owned by detector geometry
  SurfacePtrHolderImpl& operator=(const SurfacePtrHolderImpl& other)
  {
    if (this != &other) {
      destroySurface(); // clean up ourselves
      m_associatedSurface =
        other.m_associatedSurface && other.m_associatedSurface->isFree()
          ? other.m_associatedSurface->clone()
          : other.m_associatedSurface;
    }
    return *this;
  }

  /// Move assignement we just steal the resource and leave
  /// other to point to nullptr
  SurfacePtrHolderImpl& operator=(SurfacePtrHolderImpl&& other) noexcept
  {
    // destroy current surface
    destroySurface();
    // steal payload of other and leave it to nullptr
    this->m_associatedSurface = other.m_associatedSurface;
    other.m_associatedSurface = nullptr;
    return *this;
  }

  /// return the ptr we hold useful for tests
  const S* surfacePtr() { return m_associatedSurface; }

  /// destroySurface deletes the ptr if not null and is free
  /// usefull also for testing
  void destroySurface() noexcept
  {
    if (m_associatedSurface && m_associatedSurface->isFree()) {
      delete m_associatedSurface;
    }
    m_associatedSurface = nullptr;
  }

protected:
  const S* m_associatedSurface = nullptr;

  // protected we can not create instances of this object
  // is to be used as helper
  ~SurfacePtrHolderImpl() { destroySurface(); };
};

template<typename S>
class SurfaceUniqHolderImpl
{
public:
  /// default ctor
  SurfaceUniqHolderImpl() = default;
  ///
  SurfaceUniqHolderImpl(const S& s)
    : m_associatedSurface(s.isFree() ? s.clone() : &s)
  {}
  /// copy ctor, if surface is free we clone/copy.
  /// If not we just point to the one owned by detector geometry
  SurfaceUniqHolderImpl(const SurfaceUniqHolderImpl& other)
    : m_associatedSurface(other.m_associatedSurface &&
                              other.m_associatedSurface->isFree()
                            ? other.m_associatedSurface->clone()
                            : other.m_associatedSurface.get())
  {}

  /// default move ctor
  SurfaceUniqHolderImpl(SurfaceUniqHolderImpl&&) noexcept = default;

  /// copy assignment, if surface is free we clone/copy.
  /// If not we just point to the one owned by detector geometry
  SurfaceUniqHolderImpl& operator=(const SurfaceUniqHolderImpl& other)
  {
    if (this != &other) {
      m_associatedSurface.reset(
        (other.m_associatedSurface && other.m_associatedSurface->isFree())
          ? other.m_associatedSurface->clone()
          : other.m_associatedSurface.get());
    }
    return *this;
  }

  /// default move assignement
  SurfaceUniqHolderImpl& operator=(SurfaceUniqHolderImpl&&) noexcept = default;

  /// return the ptr we hold
  const S* surfacePtr() { return m_associatedSurface.get(); }

  /// destroySurface deletes the ptr if not null and is free
  /// usefull also for testing
  void destroySurface() noexcept { m_associatedSurface.reset(); }

protected:
  SurfaceUniquePtrT<const S> m_associatedSurface = nullptr;
  // protected we can not create instances of this object
  // is to be used as helper
  ~SurfaceUniqHolderImpl() = default;
};
}

#endif

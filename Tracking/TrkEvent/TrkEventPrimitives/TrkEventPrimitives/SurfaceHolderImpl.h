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
Tracking Geometry or a detector element.

In the other hand we  want to take ownership if
they are free.

As a result when we have classes
with a m_associatedSurface member
we need to repeat some boiler plate code.

These helpers try to make writing such code
a bit easier and more uniform.

In reality one should prefer the code based
on SurfaceUniquePtr
But :
We get this kind of
warning when creating
dictionaries:
Error in <CloseStreamerInfoROOTFile>: I/O is supported only
for unique_ptrs with a default deleter.
Trk::SurfaceUniqHolder::m_associatedSurface  appears to have a custom one
Which can be problematic for some cases. So we provide also
the plain ptr one

Under TrkDetDescr/TrkSurfaces :

The test : SurfaceHolder_test
has a mock code on how we could use
these for classes where we want to hold
a ptr to a const Surface and manage it,
based on if its free or owned.

The header :
SurfaceHolder.h provides typedefs
for most commom cases used
by client code so as to hide
some details of the implementation


**/

namespace Trk {

/* Helper to avoid repeating code
 */
template<typename S>
class SurfacePtrHolderImplBase
{
protected:
  // protected we can not create instances of this object
  // is to be used as helper
  ~SurfacePtrHolderImplBase() = default;
};

/*
 * Implementation based on plain ptr and
 * isFree semantics
 */
template<typename S>
class SurfacePtrHolderImpl
{
public:
  /// default ctor
  SurfacePtrHolderImpl() = default;
  /// ctor from const Surface ref.
  explicit SurfacePtrHolderImpl(const S& s)
    : m_associatedSurface(cloneHelper(&s))
  {}
  /// ctor from const Surface ptr. Takes ownership
  explicit SurfacePtrHolderImpl(const S* s)
    : m_associatedSurface(s)
  {}
  /// copy ctor
  SurfacePtrHolderImpl(const SurfacePtrHolderImpl& other)
    : m_associatedSurface(cloneHelper(other.m_associatedSurface))
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
      m_associatedSurface = cloneHelper(other.m_associatedSurface);
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
  /// destroySurface deletes the ptr if not null and
  /// the surface isFree
  /// Usefull also for testing
  void destroySurface() noexcept
  {
    if (m_associatedSurface && m_associatedSurface->isFree()) {
      delete m_associatedSurface;
    }
    //
    m_associatedSurface = nullptr;
  }
  /// return the ptr we hold useful for tests
  const S* surfacePtr() { return m_associatedSurface; }
  /// release ala unique_ptr release
  const S* release() noexcept
  {
    const S* tmp = m_associatedSurface;
    m_associatedSurface = nullptr;
    return tmp;
  }
  /// Helper for cloning or not when we need depending
  /// on if the surface isFree
  static const S* cloneHelper(const S* input)
  {
    return (input && input->isFree() ? input->clone() : input);
  }

protected:
  const S* m_associatedSurface = nullptr;
  // protected we can not create instances of this object
  // is to be used as helper
  ~SurfacePtrHolderImpl() { destroySurface(); };
};

/*
 * Implementation based on plain ptr and
 * has detector Element Semantics
 */
template<typename S>
class SurfacePtrHolderImplDetEl
{

public:
  /// default ctor
  SurfacePtrHolderImplDetEl() = default;
  /// ctor from const Surface ref.
  explicit SurfacePtrHolderImplDetEl(const S& s)
    : m_associatedSurface(cloneHelper(&s))
  {}
  /// ctor from const Surface ptr. Takes ownership
  explicit SurfacePtrHolderImplDetEl(const S* s)
    : m_associatedSurface(s)
  {}
  /// copy ctor
  SurfacePtrHolderImplDetEl(const SurfacePtrHolderImplDetEl& other)
    : m_associatedSurface(cloneHelper(other.m_associatedSurface))
  {}
  /// Move constructor we just steal the resource and leave
  /// other to point to nullptr
  SurfacePtrHolderImplDetEl(SurfacePtrHolderImplDetEl&& other) noexcept
  {
    // tranfer ownership and leave other nullptr
    m_associatedSurface = other.m_associatedSurface;
    other.m_associatedSurface = nullptr;
  }
  /// copy assingmemnt if surface is free we clone/copy.
  /// If not we just point to the one owned by detector geometry
  SurfacePtrHolderImplDetEl& operator=(const SurfacePtrHolderImplDetEl& other)
  {
    if (this != &other) {
      destroySurface(); // clean up ourselves
      m_associatedSurface = cloneHelper(other.m_associatedSurface);
    }
    return *this;
  }
  /// Move assignement we just steal the resource and leave
  /// other to point to nullptr
  SurfacePtrHolderImplDetEl& operator=(
    SurfacePtrHolderImplDetEl&& other) noexcept
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
  /// release ala unique_ptr release
  const S* release() noexcept
  {
    const S* tmp = m_associatedSurface;
    m_associatedSurface = nullptr;
    return tmp;
  }
  /// destroySurface deletes the ptr if not null and
  /// the surface has not a associatedDetectorElement
  /// Usefull also for testing
  void destroySurface() noexcept
  {
    if (m_associatedSurface &&
        m_associatedSurface->associatedDetectorElement() == nullptr) {
      delete m_associatedSurface;
    }
    //
    m_associatedSurface = nullptr;
  }

  /// Helper for cloning or not when we need depending
  /// on if we have an associatedDetectorElement
  static const S* cloneHelper(const S* input)
  {
    return (input && input->associatedDetectorElement() == nullptr
              ? input->clone()
              : input);
  }

protected:
  const S* m_associatedSurface = nullptr;
  // protected we can not create instances of this object
  // is to be used as helper
  ~SurfacePtrHolderImplDetEl() { destroySurface(); };
};

/*
 * Implementation based on SurfaceUniquePtr
 */
template<typename S>
class SurfaceUniqHolderImpl
{
public:
  /// default ctor
  SurfaceUniqHolderImpl() = default;

  /// ctor from const Surface ref. If surface is free
  /// we clone, otherwise we take its address.

  explicit SurfaceUniqHolderImpl(const S& s)
    : m_associatedSurface(s.isFree() ? s.clone() : &s)
  {}

  /// ctor from const Surface ptr. Takes ownership
  explicit SurfaceUniqHolderImpl(const S* s)
    : m_associatedSurface(s)
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

  /// release , release the unique_ptr we hold
  const S* release() noexcept { return m_associatedSurface.release(); }

protected:
  SurfaceUniquePtrT<const S> m_associatedSurface = nullptr;
  // protected we can not create instances of this object
  // is to be used as helper
  ~SurfaceUniqHolderImpl() = default;
};
}

#endif

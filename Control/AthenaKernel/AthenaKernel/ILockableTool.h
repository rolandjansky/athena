// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaKernel/ILockableTool.h
 * @author scott snyder <snyder@bnl.gov>, from IMetaDataTool.h
 * @date Mar, 2022
 * @brief Provide an interface for locking and unlocking a tool externally.
 */


#ifndef ATHENAKERNEL_ILOCKABLETOOL_H
#define ATHENAKERNEL_ILOCKABLETOOL_H


/**
 * @class ILockableTool
 * @brief Provide an interface for locking and unlocking a tool externally.
 *
 * Used by MetaDataSvc to allow keeping the metadata store locked while
 * external code has references to metadata objects.
 */
class ILockableTool
{
public:
  /// Lock the tool.
  virtual void lock_shared() const = 0;

  /// Unlock the tool.
  virtual void unlock_shared() const = 0;
};


namespace Athena {


/**
 * @class ToolLock
 * @brief RAII helper for acquiring the lock of an @c ILockableTool.
 */
class ToolLock
{
public:
  /// Acquire the lock of @c tool.
  ToolLock (ILockableTool& tool)
    : m_tool (tool)
  {
    m_tool.lock_shared();
  }

  /// Release the lock.
  ~ToolLock()
  {
    m_tool.unlock_shared();
  }


private:
  ILockableTool& m_tool;
};


} // namespace Athena



#endif // not ATHENAKERNEL_ILOCKABLETOOL_H

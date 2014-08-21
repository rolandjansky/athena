// This file's extension implies that it's C, but it is really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ISetCaloCellContainerName.h,v 1.1 2009-04-25 17:47:37 ssnyder Exp $
/* @file  ISetCaloCellContainerName.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2009
 * @brief Allow changing the name of the CaloCellContainer used by a tool.
 */

#ifndef ISETCALOCELLCONTAINERNAME_H
#define ISETCALOCELLCONTAINERNAME_H

/**
 * @brief Allow changing the name of the CaloCellContainer used by a tool.
 *
 * This is used by the trigger.
 */
class ISetCaloCellContainerName
{
public:
  /// Just to give this class a vtable.
  virtual ~ISetCaloCellContainerName() {}

  /**
   * @brief Change the name of the CaloCellContainer used by this tool.
   * @param name The new container name.
   */
  virtual StatusCode setCaloCellContainerName (const std::string& name) = 0;
};


#endif // not ISETCALOCELLCONTAINERNAME_H

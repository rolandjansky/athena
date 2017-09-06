/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1Settings                       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: December 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1Settings.h"
#include "VP1Base/VP1QtUtils.h"

#include <QFileInfo>
#include <QDir>

//____________________________________________________________________
QString VP1Settings::defaultFileSelectDirectory()
{
  static QString thedir;
  if (thedir.isEmpty()) {
    thedir = VP1QtUtils::environmentVariableValue("VP1_FILESELECTDIR");
    if (thedir.isEmpty()||!QFileInfo(thedir).isDir()||!QFileInfo(thedir).exists())
      thedir = QDir::currentPath();
  }
  return thedir;
}

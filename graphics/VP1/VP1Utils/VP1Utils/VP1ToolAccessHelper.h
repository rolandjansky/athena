/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1ToolAccessHelper                 //
//                                                            //
//  Description: For safe retrieval of tools                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1TOOLACCESSHELPER_H
#define VP1TOOLACCESSHELPER_H

#include "VP1Base/VP1HelperClassBase.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ToolHandle.h"
#include <QString>
#include <map>
#include <stdexcept>

class IVP1System;

class VP1ToolAccessHelper : public VP1HelperClassBase {
public:

  VP1ToolAccessHelper( IToolSvc* );//Use this to get error messages output to stdout.
  VP1ToolAccessHelper( IVP1System* );//Use this to also error messages output in VP1 message area.
  ~VP1ToolAccessHelper() {}//Fixme: We should release tools in destructor.

  //NB: toolname should be in the form "ConcreteToolType/ToolInstanceName"
  template <class toolT>
  toolT * getToolPointer( const QString& toolname, bool silent=false,
			  bool createIfNotExists = false );//Returns 0 if tool does not exist or in case of errors.
                                                           //It never attempts to create the tool.
                                                           //If silent is set, problems are only output as verbose messages

private:
  VP1ToolAccessHelper(const VP1ToolAccessHelper & );
  VP1ToolAccessHelper & operator= (const VP1ToolAccessHelper & );
  IToolSvc * m_toolsvc;
  std::map<std::pair<QString,QString>, IAlgTool*> m_toolname2pointer;//key is <toolname, typeinfo>
};

#include "VP1Utils/VP1ToolAccessHelper.icc"

#endif

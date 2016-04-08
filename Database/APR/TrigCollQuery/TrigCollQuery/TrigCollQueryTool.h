/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCOLLQUERY_TRIGCOLLQUERYTOOL_H
#define TRIGCOLLQUERY_TRIGCOLLQUERYTOOL_H


#include "AthenaKernel/IAthenaSelectorTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigCollQuery.h"


class TrigCollQueryTool
   : virtual public IAthenaSelectorTool,
     public AthAlgTool,
     public TrigCollQuery 
{
public:
  TrigCollQueryTool(const std::string& name,
                    const std::string& type,
                    const IInterface* parent);


/// Gaudi boilerplate
  // do I need that ?
  // static const InterfaceID& interfaceID();
 
  /// Initialize AlgTool
  virtual StatusCode initialize();
  
  /// Called at the end of initialize
  virtual StatusCode postInitialize();

  /// Called at the beginning of execute
  virtual StatusCode preNext();

  /// Called at the end of execute
  virtual StatusCode postNext();

  /// Called at the beginning of finalize
  virtual StatusCode preFinalize();

  /// Finalize AlgTool
  virtual StatusCode finalize();

  virtual ~TrigCollQueryTool();

protected:
  /// property - path to Trigger XML files
  std::string    m_XMLDir;
};


#endif



/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AraTool_ARAALGTOOLWRAPPER_H
#define AraTool_ARAALGTOOLWRAPPER_H

#include "AraTool/IAraAlgTool.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"

#include <string>


/////////////////////////////////////////////////////////////////////////////

//namespace ToolSpace {
  
template< class T >
class AraAlgToolWrapper : virtual public IAraAlgTool, virtual public AthAlgTool {
 public:
  AraAlgToolWrapper( const std::string&, const std::string&, const IInterface* );
  
  // to allow access to the IAraAlgTool interface
  StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );
  
  // setup/teardown functions, similar like those for Algorithm/Service
  StatusCode initialize();
  StatusCode finalize();
  
  // the magic method this tool provides
  //   virtual StatusCode performTask( double& result );
  
 public:
  // to resolve possible conflicts with IProperty::interfaceID()
  static const InterfaceID& interfaceID() { return IAraAlgTool::interfaceID(); }
  
  
  T *operator->() const {
    return m_ptool;
  }
  T* getTool() {return m_ptool;}
  
 private:
  T *m_ptool;
  
 private:
  MsgStream m_log;
  
 private:
  double m_factor;
};

//}

#include "AraAlgToolWrapper.icc"

#endif // !AraTool_ARAALGTOOLWRAPPER_H

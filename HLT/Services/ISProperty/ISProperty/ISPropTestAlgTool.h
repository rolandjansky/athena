/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISPROPERTY_ISPROPTESTALGTOOL_H 
#define ISPROPERTY_ISPROPTESTALGTOOL_H 1

// Include files
#include "GaudiKernel/AlgTool.h"
#include "IISPropTestAlgTool.h"

/** @class ISPropTestAlgTool ISPropTestAlgTool.h
 *  This is an interface class for a example tool
 *
 *  @author Sebastian Boeser
 */
class ISPropTestAlgTool : public AlgTool,
               virtual public IISPropTestAlgTool {
public:
  
  /// Standard Constructor
  ISPropTestAlgTool(const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  /// IISPropTestAlgTool interface
  virtual void  doIt();
  /// Overriding initialize and finalize
  virtual StatusCode initialize();
  virtual StatusCode finalize();

private:
  /// Properties
  int          m_Counter;
  /// Message service
  mutable MsgStream   log;
 };
#endif // ISPROPERTY_ISPROPTESTALGTOOL_H


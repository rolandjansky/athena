/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MissingETGoodness_EtmissGoodnessManagerTool_H
#define MissingETGoodness_EtmissGoodnessManagerTool_H

/** @file EtmissGoodnessManagerTool.h
 *  @brief This file contains the class definition for the EtmissGoodnessManagerTool class.
 *  @author Max Baak <mbaak@cern.ch>
 **/

#include "AthenaBaseComps/AthAlgTool.h"

/** @class EtmissGoodnessManagerTool
 *  @brief This file contains the class definition for the EtmissGoodnessManagerTool class.
 **/

static const InterfaceID IID_EtmissGoodnessManagerTool("EtmissGoodnessManagerTool", 1, 0);

class EtmissGoodnessManagerTool : public AthAlgTool
{
 public:    
  EtmissGoodnessManagerTool( const std::string&, const std::string&, const IInterface* );
  virtual ~EtmissGoodnessManagerTool();

  /// AlgTool and IAlgTool interface methods
  static const InterfaceID& interfaceID( ) { return IID_EtmissGoodnessManagerTool; };

  /** Overriding initialize and finalize */
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

 protected:

  int m_version;
  int m_quality;

};

#endif


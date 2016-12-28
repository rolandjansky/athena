/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTRTTRACKEXTENSIONTOOL_TRIGMAGNETICFIELDTOOL_H
#define TRIGTRTTRACKEXTENSIONTOOL_TRIGMAGNETICFIELDTOOL_H

#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

namespace MagField {	
	class IMagFieldSvc;
}

class TrigMagneticFieldTool : public AthAlgTool 
{
 public:

  TrigMagneticFieldTool( const std::string& type, 
                       const std::string& name, 
		       const IInterface* parent );

  ~TrigMagneticFieldTool();

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();

  virtual StatusCode finalize();
 
  void getMagneticField(double[3],double*);

private:
  ServiceHandle<MagField::IMagFieldSvc> m_MagFieldSvc;
};



#endif

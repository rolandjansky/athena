/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArHVPathologyDbTool_H
#define LArHVPathologyDbTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "LArElecCalib/ILArHVPathologyDbTool.h" 

class LArHVPathologyDbTool: public AthAlgTool, virtual public ILArHVPathologyDbTool
{
 public: 
  LArHVPathologyDbTool(const std::string& type
		       ,const std::string& name
		       ,const IInterface* parent);

  virtual ~LArHVPathologyDbTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  AthenaAttributeList* hvPathology2AttrList(const LArHVPathologiesDb&);
  LArHVPathologiesDb* attrList2HvPathology(const AthenaAttributeList&);
};

#endif

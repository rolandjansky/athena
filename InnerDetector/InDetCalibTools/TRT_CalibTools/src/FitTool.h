/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__FITTOOL_H
#define TRT_CALIBTOOLS__FITTOOL_H
/********************************************************************
 
NAME:     FitTool
PACKAGE:  TRT_AlignTOOLS
 
AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005
 
PURPOSE:  Tool to fit accumulated alignment histograms
 
********************************************************************/

// INCLUDES:

#include <string>
#include "TRT_CalibTools/IFitTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

class FitTool : virtual public IFitTool, public AthAlgTool {
public:

  FitTool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual ~FitTool() {}

  virtual StatusCode initialize();

  virtual bool fit(std::vector<IdentifierProfileHistogram*>& histograms, std::vector<FloatArrayStore>& results);

private:
//  MsgStream m_log;
};

#endif //  TRT_CALIBTOOLS__FITTOOL_H

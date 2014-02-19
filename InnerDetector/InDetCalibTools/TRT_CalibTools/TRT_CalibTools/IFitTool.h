/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_CALIBTOOLS__IFITTOOL_H
#define TRT_CALIBTOOLS__IFITTOOL_H
/********************************************************************

NAME:     FitTool
PACKAGE:  TRT_AlignTOOLS

AUTHORS:  Jorgen Beck Hansen
CREATED:  December 2005

PURPOSE:  Alignment Fit Tool interface          

********************************************************************/

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TRT_CalibData/IdentifiedProfileHistogram.h"
#include "Identifier/Identifier.h"

typedef IdentifiedProfileHistogram<Identifier> IdentifierProfileHistogram;

class FloatArrayStore;

static const InterfaceID IID_IFitTool("IFitTool", 1, 0);

class IFitTool : virtual public IAlgTool {
public:

  virtual bool fit(std::vector<IdentifierProfileHistogram*>& histograms, std::vector<FloatArrayStore>& results) = 0;

  static const InterfaceID& interfaceID();
};

inline const InterfaceID& IFitTool::interfaceID()
{ 
        return IID_IFitTool; 
}

#endif // TRT_CALIBTOOLS__IFITTOOL_H


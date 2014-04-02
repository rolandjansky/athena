/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTOOLS_INDETALIGNFILLSICLUSTER_IH
#define INDETALIGNTOOLS_INDETALIGNFILLSICLUSTER_IH
// IInDetAlignFillSiCluster.h
// is in InDetAlignFillSiCluster.h
// Carlos Escobar, started 08/03/2008

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID 
  IID_INDETALIGN_IInDetAlignFillSiCluster("IInDetAlignFillSiCluster",1,0);

class IInDetAlignFillSiCluster: virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID();

  virtual StatusCode FillSiCluster() = 0;

};

inline const InterfaceID& IInDetAlignFillSiCluster::interfaceID()
{ return   IID_INDETALIGN_IInDetAlignFillSiCluster; }


#endif // INDETALIGNTOOLS_INDETALIGNFILLSICLUSTER_IH

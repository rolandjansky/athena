/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNTOOLS_INDETALIGNFILLTRACK_IH
#define INDETALIGNTOOLS_INDETALIGNFILLTRACK_IH
// IInDetAlignFillTrack.h
// is in InDetAlignFillTrack.h
// Carlos Escobar, started 27/12/2007

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID 
  IID_INDETALIGN_IInDetAlignFillTrack("IInDetAlignFillTrack",1,0);

class IInDetAlignFillTrack: virtual public IAlgTool {
 public:
  static const InterfaceID& interfaceID();

  virtual StatusCode FillTrack() = 0;

  virtual int GetTrks() const = 0;
  virtual int GetTrkHits() const = 0;
  virtual int GetTrkPixHits() const = 0;
  virtual int GetTrkSCTHits() const = 0;
  virtual int GetTrkTRTHits() const = 0;
  
};

inline const InterfaceID& IInDetAlignFillTrack::interfaceID()
{ return   IID_INDETALIGN_IInDetAlignFillTrack; }


#endif // INDETALIGNTOOLS_INDETALIGNFILLTRACK_IH

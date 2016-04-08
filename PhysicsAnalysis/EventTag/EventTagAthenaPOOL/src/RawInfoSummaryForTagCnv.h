/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RAWINFOSUMMARYFORTAGCNV_H
#define RAWINFOSUMMARYFORTAGCNV_H

/**
 *  @file RawInfoSummaryForTagCnv.h
 *  @brief AthenaPOOL converter for RawInfoSummaryForTag
 *  @author Marcin.Nowak@cern.ch
 **/

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TagEvent/RawInfoSummaryForTag.h"
#include "EventTagTPCnv/RawInfoSummaryForTagCnv_p1.h"

/** @class RawInfoSummaryForTagCnv
 *  @brief This class provides the converter to customize the saving of RawInfoSummaryForTag
 **/

typedef RawInfoSummaryForTag  TRANS;
typedef RawInfoSummaryForTag_p1 PERS;
typedef T_AthenaPoolCustomCnv<TRANS, PERS>  RawInfoSummaryForTagCnvBase;

class RawInfoSummaryForTagCnv : public RawInfoSummaryForTagCnvBase
{
  friend class CnvFactory<RawInfoSummaryForTagCnv>;
public:
  RawInfoSummaryForTagCnv(ISvcLocator* svcloc);

  virtual PERS* createPersistent(TRANS* transObj);
  virtual TRANS* createTransient();

private:
   RawInfoSummaryForTagCnv_p1 m_TPConverter;
};

#endif

//Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef CALOUTILS_H1WEIGHTTOOLCSC12GENERIC_H
#define CALOUTILS_H1WEIGHTTOOLCSC12GENERIC_H
/***********************************************************************
Filename : H1WeightToolCSC12Generic.h
Author   : Walter Lampl
Created  : Feb 2007

***********************************************************************/

#include "GaudiKernel/AlgTool.h"
#include "CaloInterface/IHadronicCalibrationTool.h"
#include "CaloUtils/ToolWithConstants.h"
//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>

class CaloCell;

class H1WeightToolCSC12Generic :
  public extends<CaloUtils::ToolWithConstants<AthAlgTool>,
                 IHadronicCalibrationTool>
{
 public:
  /// Inherit constructor.
  using base_class::base_class;

  virtual StatusCode initialize() override;

  virtual double etCell(const CaloCell* thisCell, double weight) override;
  virtual double etCryo(double etAccb3, double etTile1)          override;
  virtual double jetScale(double e, double eta)                  override;
  virtual double wtCell(const CaloCell* thisCell) const          override;
  virtual double wtCryo()                                        override;


private:
  Constant<CxxUtils::Array<1> > m_wtEMB0  { this, "wtEMB0" };
  Constant<CxxUtils::Array<1> > m_wtEME0  { this, "wtEME0" };
  Constant<CxxUtils::Array<1> > m_wtEMB1  { this, "wtEMB1" };
  Constant<CxxUtils::Array<1> > m_wtEMB2  { this, "wtEMB2" };
  Constant<CxxUtils::Array<1> > m_wtEME1  { this, "wtEME1" };
  Constant<CxxUtils::Array<1> > m_wtEME2  { this, "wtEME2" };
  Constant<CxxUtils::Array<1> > m_wtTile1 { this, "wtTile1" };
  Constant<CxxUtils::Array<1> > m_wtTile2 { this, "wtTile2" };
  Constant<CxxUtils::Array<1> > m_wtHec1  { this, "wtHec1" };
  Constant<CxxUtils::Array<1> > m_wtHec2  { this, "wtHec2" };
  Constant<CxxUtils::Array<1> > m_wtFCal1 { this, "wrFCal1" };
  Constant<CxxUtils::Array<1> > m_wtFCal2 { this, "wrFCal2" };
  Constant<float>               m_wtGap   { this, "wtGap" };
  Constant<float>               m_wtScint { this, "wtScint" };
  Constant<float>               m_wtCryo  { this, "wtCyo" };
  Constant<CxxUtils::Array<2> > m_etaFit  { this, "etaFit" };
};

#endif

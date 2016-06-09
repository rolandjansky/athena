//Dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "CaloRec/ToolWithConstantsMixin.h"
//#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include <string>

class CaloCell;

class H1WeightToolCSC12Generic : public AthAlgTool, virtual public IHadronicCalibrationTool, 
				 public CaloRec::ToolWithConstantsMixin
{
 public:

  // Algtool constructor
  H1WeightToolCSC12Generic(const std::string& name, const std::string& type,
                   const IInterface* parent);
  virtual ~H1WeightToolCSC12Generic();

  StatusCode initialize();

  virtual double etCell(const CaloCell* thisCell, double weight) ;
  virtual double etCryo(double etAccb3, double etTile1)          ;
  virtual double jetScale(double e, double eta)                  ;
  virtual double wtCell(const CaloCell* thisCell)                ;
  virtual double wtCryo()                                        ;

  using AlgTool::setProperty;
  // using IHadronicCalibrationTool::setProperty;
  /**
   * @brief Method to set a property value.
   * @param propname The name of the property to set.
   * @param value The value to which to set it.
   *
   * Defined here as required by @c ToolWithConstantsMixin.
   */
  virtual StatusCode setProperty (const std::string& propname,
                                  const std::string& value);

  /**
   * @brief Method to set a property value.
   * @param p The property name/value to set.
   *
   * Defined here as required by @c ToolWithConstantsMixin.
   */
  virtual StatusCode setProperty (const Property& p);

 private:
  CaloRec::Array<1> m_wtEMB0;
  CaloRec::Array<1> m_wtEME0;
  CaloRec::Array<1> m_wtEMB1;
  CaloRec::Array<1> m_wtEMB2;
  CaloRec::Array<1> m_wtEME1;
  CaloRec::Array<1> m_wtEME2;
  CaloRec::Array<1> m_wtTile1;
  CaloRec::Array<1> m_wtTile2;
  CaloRec::Array<1> m_wtHec1;
  CaloRec::Array<1> m_wtHec2;
  CaloRec::Array<1> m_wtFCal1;
  CaloRec::Array<1> m_wtFCal2;
  float m_wtGap;
  float  m_wtScint;
  float m_wtCryo;
  CaloRec::Array<2> m_etaFit;
};

#endif

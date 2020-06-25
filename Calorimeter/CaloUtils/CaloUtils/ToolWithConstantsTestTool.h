// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloUtils/ToolWithConstantsTestTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2020
 * @brief Algorithm used for testing ToolWithConstants.
 */


#ifndef CALOUTILS_TOOLWITHCONSTANTSTESTTOOL_H
#define CALOUTILS_TOOLWITHCONSTANTSTESTTOOL_H


#include "CaloUtils/ToolWithConstants.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace CaloUtils {


/**
 * @brief Algorithm used for testing ToolWithConstants.
 */
class ToolWithConstantsTestTool
  : public CaloUtils::ToolWithConstants<AthAlgTool>
{
public:
  /// Inherit constructor.
  using base_class::base_class;


  /// Gaudi initialize method.
  StatusCode initialize () override;


  /// Gaudi execute method.
  StatusCode execute (const EventContext& ctx) const;


  /// A non-default tool version, for testing.
  virtual int toolVersion() const override;


  /// For testing writeConstants().
  void testWriteConstants (const EventContext& ctx) const;

  
private:
  StringProperty m_condKey
  { this, "CondKey", "", "" };

  IntegerProperty m_runNumber
  { this, "RunNumber", 284500, "" };

  Constant<float> m_cf1 { this, "cf1", 1.5, "cf1 doc" };
  Constant<int> m_ci1 { this, "ci1", "ci1 doc" };
  Constant<bool> m_cb1 { this, "cb1", "cb1 doc" };
  Constant<CaloRec::Array<1> > m_ca1 { this, "ca1", "ca1 doc" }; 
  Constant<CaloRec::Array<2> > m_ca2 { this, "ca2", "ca2 doc" };
};


} // namespace CaloUtils


#endif // not CALOUTILS_TOOLWITHCONSTANTSTESTTOOL_H

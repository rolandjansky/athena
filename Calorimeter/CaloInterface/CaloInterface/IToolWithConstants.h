// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CaloInterface/IToolWithConstants.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2020
 * @brief Abstact interface for ToolWithConstants.
 */


#ifndef CALOINTERFACE_ITOOLWITHCONSTANTS_H
#define CALOINTERFACE_ITOOLWITHCONSTANTS_H


#include "CaloConditions/ToolConstants.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/EventContext.h"
#include <string>


/**
 * @brief Abstact interface for ToolWithConstants.
 *
 * Defines operations needed by CaloClusterCorrDBWriter.
 */
class IToolWithConstants
  : virtual public IAlgTool
{
public:
  DeclareInterfaceID (IToolWithConstants, 1, 0);


  /**
   * @brief Merge our constants into @c out with the proper prefix.
   * @param[out] out Object to receive our constants.
   * @param ctx Event context.
   */
  virtual
  StatusCode
  mergeConstants (CaloRec::ToolConstants& out,
                  const EventContext& ctx) const = 0;
};


#endif // not CALOINTERFACE_ITOOLWITHCONSTANTS_H

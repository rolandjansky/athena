// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/Obj4GetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2011
 * @brief Test getter tool for Obj4.
 */


#ifndef D3PDMAKERTEST_OBJ4GETTERTOOL_H
#define D3PDMAKERTEST_OBJ4GETTERTOOL_H


#include "D3PDMakerTest/Obj4.h"
#include "D3PDMakerUtils/SGCollectionGetterTool.h"


namespace D3PDTest {


class Obj4GetterTool
  : public D3PD::SGCollectionGetterTool<Obj4Container>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  Obj4GetterTool (const std::string& type,
                  const std::string& name,
                  const IInterface* parent);
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_OBJ4GETTERTOOL_H

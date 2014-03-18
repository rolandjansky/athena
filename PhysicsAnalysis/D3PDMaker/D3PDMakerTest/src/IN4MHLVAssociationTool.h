// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/IN4MHLVAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2013
 * @brief Test code: IN4M -> HLV
 */


#ifndef D3PDMAKERTEST_IN4MHLVASSOCIATIONTOOL_H
#define D3PDMAKERTEST_IN4MHLVASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "CLHEP/Vector/LorentzVector.h"
class INavigable4Momentum;


namespace D3PDTest {


/**
 * @brief Test code: IN4M -> HLV
 */
class IN4MHLVAssociationTool
  : public D3PD::SingleAssociationTool<INavigable4Momentum,
                                       CLHEP::HepLorentzVector>
{
public:
  typedef D3PD::SingleAssociationTool<INavigable4Momentum,
                                      CLHEP::HepLorentzVector> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  IN4MHLVAssociationTool (const std::string& type,
                          const std::string& name,
                          const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const CLHEP::HepLorentzVector* get (const INavigable4Momentum& p);


private:
  /// Hold the allocated object.
  CLHEP::HepLorentzVector m_p;
};


} // namespace D3PDTest


#endif // not D3PDMAKERTEST_IN4MHLVASSOCIATIONTOOL_H

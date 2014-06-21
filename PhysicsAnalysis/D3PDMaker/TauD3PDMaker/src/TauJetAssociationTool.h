// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TauD3PDMaker/src/TauJetAssociationTool.h
 * @author Dugan O'Neil <doneil@sfu.ca>
 * @date January, 2010
 * @brief Associate from a tau to its jet seed
 */


#ifndef TAUD3PDMAKER_TAUJETASSOCIATIONTOOL_H
#define TAUD3PDMAKER_TAUJETASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "tauEvent/TauJet.h"
#include "JetEvent/Jet.h"
//class Jet;
//class TauJet;


namespace D3PD {


/**
 * @brief Associate from an egamma to its CaloCluster.
 */
class TauJetAssociationTool
  : public SingleAssociationTool<Analysis::TauJet, Jet>
{
public:
  typedef SingleAssociationTool<Analysis::TauJet, Jet> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TauJetAssociationTool (const std::string& type,
                                const std::string& name,
                                const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Jet* get (const Analysis::TauJet& p);
};


} // namespace D3PD



#endif // not TAUD3PDMAKER_TAUJETASSOCIATIONTOOL_H

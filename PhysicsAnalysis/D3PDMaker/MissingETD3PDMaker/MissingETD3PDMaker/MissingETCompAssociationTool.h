// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MissingETD3PDMaker/MissingETD3PDMaker/MissingETCompAssociationTool.h
 * @author Jet Goodson <jgoodson@cern.ch>, based on similar tools by scott snyder <snyder@bnl.gov>
 * @date September, 2010
 * @brief Associate electron to MET Composition object
 */


#ifndef MISSINGETD3PDMAKER_MISSINGETCOMPASSOCIATIONTOOL_H
#define MISSINGETD3PDMAKER_MISSINGETCOMPASSOCIATIONTOOL_H

#include "D3PDMakerInterfaces/IObjGetterTool.h"
#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "GaudiKernel/ToolHandle.h"
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtTruth.h"
#include "MissingETEvent/MissingEtCalo.h"
#include "MissingETEvent/MissingETComposition.h"


namespace D3PD {


/**
 * @brief Associate to particle closest in DR.
 *
 * This is a single association tool.
 * Given an @c INavigable4Momentum object and a container of PAU::gamma objects,
 * return the object in the container that is associated with the original object.
 *
 * Parameters:
 *  Getter - ICollectionGetterTool instance defining the target collection.
 *
 */
class MissingETCompAssociationTool
  : public SingleAssociationTool<INavigable4Momentum, MissingETComposition>
{
public:
  typedef SingleAssociationTool<INavigable4Momentum, MissingETComposition> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MissingETCompAssociationTool (const std::string& type,
			   const std::string& name,
			   const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Create any needed tuple variables.
   */
  virtual StatusCode book();


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const MissingETComposition* get(const INavigable4Momentum& p);

  template<class T>
  void weighter(const T* objecto, const MissingETComposition* compo);

  void weighter(const INavigable4Momentum* objecto, const MissingETComposition* compo);

private:
  /// Property: The getter for the target collection.
  ToolHandle<IObjGetterTool> m_getter;
  bool m_allowMissing;
  std::string m_objectType;

 
  std::vector<float> *m_vec_weight_px;
  std::vector<float> *m_vec_weight_py;
  std::vector<float> *m_vec_weight_ET;
  std::vector<unsigned int> *m_vec_statusWord;

  //float *m_px;

};


} // namespace D3PD



#endif // not MISSINGETD3PDMAKER_MISSINGETCOMPASSOCIATIONTOOL_H

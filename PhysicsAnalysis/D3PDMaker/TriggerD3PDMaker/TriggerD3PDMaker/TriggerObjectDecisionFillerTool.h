// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TriggerD3PDMaker/src/TriggerObjectDecisionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2010
 * @brief Record which triggers are passed by a given trigger object.
 */


#ifndef TRIGGERD3PDMAKER_TRIGGEROBJECTDECISIONFILLERTOOL_H
#define TRIGGERD3PDMAKER_TRIGGEROBJECTDECISIONFILLERTOOL_H

// Trigger analysis include(s):
#include "TrigObjectMatching/DistanceFunctor.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// Local include(s):
#include "TriggerObjectDecisionFillerToolImpl.h"

#include "GaudiKernel/IIncidentListener.h"


namespace D3PD {


/**
 * @brief Record which triggers are passed by a given trigger object.
 */
template <class T>
class TriggerObjectDecisionFillerTool
  : public BlockFillerTool<T>,
    public IIncidentListener
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TriggerObjectDecisionFillerTool (const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent);

   /**
   * @brief Constructor specifying a metric for the matching
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  TriggerObjectDecisionFillerTool (const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent,
                                   DistanceFunctor< T, T >* metric );

   /// Destructor needed to clean up some things
   ~TriggerObjectDecisionFillerTool();


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block.
   * @param p The input object.
   *
   * This is called once per event.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const T& p);


  /// Function called when a relevant incident happens.
  virtual void handle (const Incident& inc);


private:
   /// The implementation object.
   TriggerObjectDecisionFillerToolImpl m_impl;
   /// The metric used for the matching
   DistanceFunctor< T, T >* m_metric;

   /// Don't allow copying.
   TriggerObjectDecisionFillerTool (const TriggerObjectDecisionFillerTool&);
   TriggerObjectDecisionFillerTool&
   operator= (const TriggerObjectDecisionFillerTool&);
};


} // namespace D3PD


#ifndef TRIGGERD3PDMAKER_ENTRIES_FILE
#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.icc"
#endif


#endif // not TRIGGERD3PDMAKER_TRIGGEROBJECTDECISIONFILLERTOOL_H

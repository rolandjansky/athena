// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/LeadingPtAssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2013
 * @brief Choose the leading Pt object from a multiple association.
 */


#ifndef EVENTCOMMOND3PDMAKER_LEADINGPTASSOCIATIONTOOL_H
#define EVENTCOMMOND3PDMAKER_LEADINGPTASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"
#include "D3PDMakerUtils/TypeConverter.h"
#include "D3PDMakerInterfaces/IMultiAssociationTool.h"
#include "EventKernel/INavigable4Momentum.h"
#include "GaudiKernel/ToolHandle.h"


namespace D3PD {


/**
 * @brief Choose the leading Pt object from a multiple association.
 *
 * This single association is defined starting from a multiple association;
 * we look at all the target objects and choose the one with the largest Pt.
 * (Both the source and target of the multiple association must be convertible
 * to INavigable4Momentum.)
 */
class LeadingPtAssociationTool
  : public SingleAssociationTool<INavigable4Momentum>
{
public:
  typedef SingleAssociationTool<INavigable4Momentum> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  LeadingPtAssociationTool (const std::string& type,
                            const std::string& name,
                            const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();


  /**
   * @brief Configure during initialization: type-check.
   * @param tree Our parent for tuple making.
   * @param ti Gives the type of the object being passed to @c resetUntyped.
   *
   * @c configureD3PD should check that the type of the object coming as input
   * (to @c resetUntyped)
   * is compatible with what it expects, and raise an error otherwise.
   */
  virtual StatusCode configureD3PD (IAddVariable* tree,
                                    const std::type_info& ti);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const INavigable4Momentum* get (const INavigable4Momentum& p);


  /**
   * @brief Release an object retrieved from the association.
   * @param p The object to release.
   *
   * Call this when you are done with the object returned by
   * @c get().  The default implementation is a no-op,
   * but if the association dynamically allocated the object which
   * it returned, this gives it a chance to free it.
   */
  virtual void releaseObject (const INavigable4Momentum* p);


private:
  /// Property: The underlying multiple association.
  ToolHandle<IMultiAssociationTool> m_multiAssocTool;

  /// Converter from the multi assoc's target type to IN4M.
  TypeConverter m_cnv;

  /// Converter from IN4M to the multi assoc's target type.
  TypeConverter m_cnvBack;
};


} // namespace D3PD



#endif // not EVENTCOMMOND3PDMAKER_LEADINGPTASSOCIATIONTOOL_H

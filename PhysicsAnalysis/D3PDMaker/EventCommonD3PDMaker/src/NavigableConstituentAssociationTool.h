// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file EventCommonD3PDMaker/src/NavigableConstituentAssociationTool.h
 * @author scott snyder, W. H. Bell < W.Bell AT cern.ch > 
 * @date April, 2011
 * @brief Association between navigables and their constituents.
 */


#ifndef EVENTCOMMOND3PDMAKER_NAVIGABLECONSTITUENTASSOCIATIONTOOL_H
#define EVENTCOMMOND3PDMAKER_NAVIGABLECONSTITUENTASSOCIATIONTOOL_H

#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "D3PDMakerUtils/TypeConverter.h"
#include "EventKernel/INavigable4Momentum.h"
#include "Navigation/NavigationToken.h"
#include "Navigation/INavigationSelector.h"


namespace D3PD {


/// Helper to define ordering of the NavigationToken.
class IN4MHash
{
public:
  size_t operator() (const INavigable4Momentum* p) const
  {
    return p->pt();
  }
};


/**  
 * @brief Association between navigables and their constituents.
 */
class NavigableConstituentAssociationTool 
  : public MultiAssociationTool<INavigable4Momentum, INavigable4Momentum>
{
public:
  /// Base class shorthand.
  typedef  MultiAssociationTool<INavigable4Momentum, INavigable4Momentum> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  NavigableConstituentAssociationTool(const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();
    
    
  /**
   * @brief Create any needed tuple variables.
   *
   * This is called at the start of the first event.
   */
  StatusCode book();


  /**
   * @brief Start the iteration for a new association.
   * @param p The object from which to associate.
   */
  virtual StatusCode reset (const INavigable4Momentum& p);
    
   
  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const INavigable4Momentum* next();
    

private:
  /// NavigationToken selector.
  class Selector
    : public INavigationSelector<INavigable4Momentum>
  {
  public:
    /// Constructor.
    Selector (TypeConverter& converter);

    /// check data acceptance
    virtual bool accept(const INavigable4Momentum* data, double weight) const;

    /// reset condition
    virtual void reset();


  private:
    TypeConverter& m_converter;
  };

  /// Property: Target type name for the association.
  std::string m_typeName;

  /// Property: Should the association weights be saved?
  bool m_writeWeight;

  /// Variable: Association weight.
  float* m_weight;

  /// Converter for the output type.
  TypeConverter m_converter;

  /// NavigationToken selector.
  Selector m_selector;

  /// Hold the state of the iteration.
  NavigationToken<INavigable4Momentum, double, IN4MHash> m_token;
  NavigationToken<INavigable4Momentum, double, IN4MHash>::const_iterator m_it;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_NAVIGABLECONSTITUENTASSOCIATIONTOOL_H

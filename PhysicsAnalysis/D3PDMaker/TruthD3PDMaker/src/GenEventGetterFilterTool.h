// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: GenEventGetterFilterTool.h 639304 2015-01-13 20:08:39Z ssnyder $
/**
 * @file EventCommonD3PDMaker/src/GenEventGetterFilterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2009
 * @brief Collection getter filter to remove empty GenEvent instances.
 */


#include "D3PDMakerUtils/CollectionGetterFilterTool.h"
#include "AtlasHepMC/GenEvent.h"


#ifndef EVENTCOMMOND3PDMAKER_GENEVENTGETTERFILTERTOOL_H
#define EVENTCOMMOND3PDMAKER_GENEVENTGETTERFILTERTOOL_H


namespace D3PD {


/**
 * @brief Collection getter filter to remove empty GenEvent instances.
 *
 * In some samples, @c McEventCollection contains thousands
 * of empty @c GenEvent instances (why???).
 * Use this to filter those out.
 */
class GenEventGetterFilterTool
  : public CollectionGetterFilterTool<HepMC::GenEvent>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  GenEventGetterFilterTool (const std::string& type,
                            const std::string& name,
                            const IInterface* parent);


  /**
   * @brief Apply filtering to one element.
   *
   * Returns true if the filter passes; false otherwise.
   */
  virtual bool filter (const HepMC::GenEvent* p) const;
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_GENEVENTGETTERFILTERTOOL_H

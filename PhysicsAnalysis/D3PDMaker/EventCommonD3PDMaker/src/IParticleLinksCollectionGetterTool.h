// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file IParticleLinksCollectionGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Mar, 2010
 * @brief Getter for ParticleLinks<IParticleContainer>
 */


#ifndef IPARTICLELINKSCOLLECTIONGETTERTOOL_H
#define IPARTICLELINKSCOLLECTIONGETTERTOOL_H


#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "ParticleEvent/IParticleLinks.h"
#include <vector>


namespace D3PD {


class IParticleLinksCollectionGetterTool
  : public SGCollectionGetterTool<IParticleLinks>
{
public:
  typedef SGCollectionGetterTool<IParticleLinks> Base;

  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  IParticleLinksCollectionGetterTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);
};


} // namespace D3PD


#endif // not IPARTICLELINKSCOLLECTIONGETTERTOOL_H

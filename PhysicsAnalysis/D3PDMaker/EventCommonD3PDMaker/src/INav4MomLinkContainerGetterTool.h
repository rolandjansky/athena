// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/INav4MomLinkContainerGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 20, 2012
 * @brief Getter tool to retrieve an @c INav4MomLinkContainer from SG.
 */


#ifndef EVENTCOMMOND3PDMAKER_INAV4MOMLINKCONTAINERGETTERTOOL_H
#define EVENTCOMMOND3PDMAKER_INAV4MOMLINKCONTAINERGETTERTOOL_H


#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "NavFourMom/INav4MomLinkContainer.h"


namespace D3PD {


/**
 * @brief Getter tool to retrieve an @c INav4MomLinkContainer from SG.
 *
 * @c INav4MomLinkContainer is not a @c DataVector, so we can't
 * use the generic tool.
 *
 * Properties:
 *   SGKey      - StoreGate key of the object being retrieved.
 *                Can also be a comma or space-separated list.
 *                In that case, the first key that actually exists
 *                in the data store will be used.
 *   ClassIDSvc - ClassIDSvc instance to use.
 */
class INav4MomLinkContainerGetterTool
  : public SGCollectionGetterTool<INav4MomLinkContainer>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  INav4MomLinkContainerGetterTool (const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent);


  /// Standard Gaudi initialize method.
  virtual StatusCode initialize();
};


} // namespace D3PD


#endif // not EVENTCOMMOND3PDMAKER_INAV4MOMLINKCONTAINERGETTERTOOL_H


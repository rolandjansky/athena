// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file EventCommonD3PDMaker/src/IN4MCollectionGetterTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Getter tool to retrieve an @c INavigable4MomentumCollection from SG.
 */


#ifndef D3PDMAKER_IN4MCOLLECTIONGETTERTOOL_H
#define D3PDMAKER_IN4MCOLLECTIONGETTERTOOL_H


#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "NavFourMom/INavigable4MomentumCollection.h"


namespace D3PD {


/**
 * @brief Getter tool to retrieve an @c INavigable4MomentumCollection from SG.
 *
 * Note: this tool is redundant with @c SGDataVectorGetterTool; that
 * tool should generally be used instead of this.  This tool is retained
 * as an example of using @c SGCollectionGetterTool.
 *
 * Properties:
 *   SGKey      - StoreGate key of the object being retrieved.
 *                Can also be a comma or space-separated list.
 *                In that case, the first key that actually exists
 *                in the data store will be used.
 *   ClassIDSvc - ClassIDSvc instance to use.
 */
class IN4MCollectionGetterTool
  : public SGCollectionGetterTool<INavigable4MomentumCollection>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  IN4MCollectionGetterTool (const std::string& type,
                            const std::string& name,
                            const IInterface* parent);
};


} // namespace D3PD


#endif // not D3PDMAKER_IN4MCOLLECTIONGETTERTOOL_H


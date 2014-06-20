// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMbD3PDMaker/associators/BCM_RDO_CollectionRawDataAssocTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2010
 * @brief Associate from a BCM_RDO_Collection to its constituent BCM_RawData
 *        objects.
 */


#ifndef TRIGMBD3PDMAKER_BCM_RDO_COLLECTIONRAWDATAASSOCTOOL_H
#define TRIGMBD3PDMAKER_BCM_RDO_COLLECTIONRAWDATAASSOCTOOL_H


#include "D3PDMakerUtils/MultiAssociationTool.h"
#include "InDetBCM_RawData/BCM_RDO_Collection.h"
class BCM_RawData;


namespace TrigMbD3PD {


class BCM_RDO_CollectionRawDataAssocTool
  : public D3PD::MultiAssociationTool<BCM_RDO_Collection, BCM_RawData>
{
public:
  typedef D3PD::MultiAssociationTool<BCM_RDO_Collection, BCM_RawData> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  BCM_RDO_CollectionRawDataAssocTool (const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent);


  /**
   * @brief Start the iteration for a new association.
   * @param coll The object from which to associate.
   */
  virtual StatusCode reset (const BCM_RDO_Collection &coll);


  /**
   * @brief Return a pointer to the next element in the association.
   *
   * Return 0 when the association has been exhausted.
   */
  virtual const BCM_RawData* next();


private:
  /// Current collection iterator.
  BCM_RDO_Collection::const_iterator m_it;

  /// Collection end iterator.
  BCM_RDO_Collection::const_iterator m_end;
};


} // namespace TrigMbD3PD


#endif // not TRIGMBD3PDMAKER_BCM_RDO_COLLECTIONRAWDATAASSOCTOOL_H

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_CPBSCOLLECTIONV1_H
#define TRIGT1CALO_CPBSCOLLECTIONV1_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

namespace LVL1 {

class CPMTower;
class CPMHits;
class CMMCPHits;


/** Cluster Processor container for writing bytestream.
 *
 *  Contains all the component collections needed to build the ROD.
 *
 *  @author Peter Faulkner
 */

class CPBSCollectionV1 {

 public:
   CPBSCollectionV1(const DataVector<CPMTower>*  ttCollection,
                  const DataVector<CPMHits>*   hitCollection,
                  const DataVector<CMMCPHits>* cmmHitCollection);

   /// Return pointer to CPM tower collection
   const DataVector<CPMTower>*  towers()  const;
   /// Return pointer to CPM hits collection
   const DataVector<CPMHits>*   hits()    const;
   /// Return pointer to CMM hits collection
   const DataVector<CMMCPHits>* cmmHits() const;

 private:

   /// CPM tower collection
   const DataVector<CPMTower>*  m_ttCollection;
   /// CPM hits collection
   const DataVector<CPMHits>*   m_hitCollection;
   /// CMM hits collection
   const DataVector<CMMCPHits>* m_cmmHitCollection;

};

inline const DataVector<CPMTower>* CPBSCollectionV1::towers() const
{
  return m_ttCollection;
}

inline const DataVector<CPMHits>* CPBSCollectionV1::hits() const
{
  return m_hitCollection;
}

inline const DataVector<CMMCPHits>* CPBSCollectionV1::cmmHits() const
{
  return m_cmmHitCollection;
}

} // end namespace

CLASS_DEF(LVL1::CPBSCollectionV1, 1270847938, 1)

#endif

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_CPBSCOLLECTIONV2_H
#define TRIGT1CALO_CPBSCOLLECTIONV2_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

namespace LVL1 {

class CPMTower;
class CMXCPTob;
class CMXCPHits;


/** Cluster Processor container for writing bytestream.
 *
 *  Contains all the component collections needed to build the ROD.
 *
 *  @author Peter Faulkner
 */

class CPBSCollectionV2 {

 public:
   CPBSCollectionV2(const DataVector<CPMTower>*  ttCollection,
                    const DataVector<CMXCPTob>*  tobCollection,
                    const DataVector<CMXCPHits>* hitCollection);

   /// Return pointer to CPM tower collection
   const DataVector<CPMTower>*  towers()  const;
   /// Return pointer to CMX TOB collection
   const DataVector<CMXCPTob>*  tobs()    const;
   /// Return pointer to CMX hits collection
   const DataVector<CMXCPHits>* hits() const;

 private:

   /// CPM tower collection
   const DataVector<CPMTower>*  m_ttCollection;
   /// CMX TOB collection
   const DataVector<CMXCPTob>*  m_tobCollection;
   /// CMX hits collection
   const DataVector<CMXCPHits>* m_hitCollection;

};

inline const DataVector<CPMTower>* CPBSCollectionV2::towers() const
{
  return m_ttCollection;
}

inline const DataVector<CMXCPTob>* CPBSCollectionV2::tobs() const
{
  return m_tobCollection;
}

inline const DataVector<CMXCPHits>* CPBSCollectionV2::hits() const
{
  return m_hitCollection;
}

} // end namespace

CLASS_DEF(LVL1::CPBSCollectionV2, 1270847937, 1)

#endif

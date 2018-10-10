/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_JEPBSCOLLECTION_H
#define TRIGT1CALO_JEPBSCOLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

namespace LVL1 {

class JetElement;
class JEMHits;
class JEMEtSums;
class CMMJetHits;
class CMMEtSums;
 

/** Jet/Energy Processor container for writing bytestream.
 *
 *  Contains all the component collections needed to build the ROD.
 *
 *  @author Peter Faulkner
 */

class JEPBSCollection {

 public:
   JEPBSCollection(const DataVector<JetElement>* jeCollection,
                   const DataVector<JEMHits>*    hitCollection,
		   const DataVector<JEMEtSums>*  etCollection,
                   const DataVector<CMMJetHits>* cmmHitCollection,
		   const DataVector<CMMEtSums>*  cmmEtCollection);

   /// Return pointer to jet element collection
   const DataVector<JetElement>* JetElements() const;
   /// Return pointer to hit sums collection
   const DataVector<JEMHits>*    JetHits()     const;
   /// Return pointer to energy sums collection
   const DataVector<JEMEtSums>*  EnergySums()  const;
   /// Return pointer to CMM hit sums collection
   const DataVector<CMMJetHits>* CmmHits()     const;
   /// Return pointer to CMM energy sums collection
   const DataVector<CMMEtSums>*  CmmSums()     const;

 private:

   /// Jet element collection
   const DataVector<JetElement>* m_jeCollection;
   /// Hit sums collection
   const DataVector<JEMHits>*    m_hitCollection;
   /// Energy sums collection
   const DataVector<JEMEtSums>*  m_etCollection;
   /// CMM Hit sums collection
   const DataVector<CMMJetHits>* m_cmmHitCollection;
   /// CMM Energy sums collection
   const DataVector<CMMEtSums>*  m_cmmEtCollection;

};

inline const DataVector<JetElement>* JEPBSCollection::JetElements() const
{
  return m_jeCollection;
}

inline const DataVector<JEMHits>* JEPBSCollection::JetHits() const
{
  return m_hitCollection;
}

inline const DataVector<JEMEtSums>* JEPBSCollection::EnergySums() const
{
  return m_etCollection;
}

inline const DataVector<CMMJetHits>* JEPBSCollection::CmmHits() const
{
  return m_cmmHitCollection;
}

inline const DataVector<CMMEtSums>* JEPBSCollection::CmmSums() const
{
  return m_cmmEtCollection;
}

} // end namespace

CLASS_DEF(LVL1::JEPBSCollection, 1255323120, 1)

#endif

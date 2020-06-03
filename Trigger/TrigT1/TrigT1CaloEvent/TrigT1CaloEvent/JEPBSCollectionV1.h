/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_JEPBSCOLLECTIONV1_H
#define TRIGT1CALO_JEPBSCOLLECTIONV1_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

namespace LVL1 {

class JetElement;
class JEMHits;
class JEMEtSums;
class CMMJetHits;
class CMMEtSums;
 

/** Jet/Energy Processor container for writing bytestream pre-LS1.
 *
 *  Contains all the component collections needed to build the ROD.
 *
 *  @author Peter Faulkner
 */

class JEPBSCollectionV1 {

 public:
   JEPBSCollectionV1(const DataVector<JetElement>* jeCollection,
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

inline const DataVector<JetElement>* JEPBSCollectionV1::JetElements() const
{
  return m_jeCollection;
}

inline const DataVector<JEMHits>* JEPBSCollectionV1::JetHits() const
{
  return m_hitCollection;
}

inline const DataVector<JEMEtSums>* JEPBSCollectionV1::EnergySums() const
{
  return m_etCollection;
}

inline const DataVector<CMMJetHits>* JEPBSCollectionV1::CmmHits() const
{
  return m_cmmHitCollection;
}

inline const DataVector<CMMEtSums>* JEPBSCollectionV1::CmmSums() const
{
  return m_cmmEtCollection;
}

} // end namespace

CLASS_DEF(LVL1::JEPBSCollectionV1, 1243139661, 1)

#endif

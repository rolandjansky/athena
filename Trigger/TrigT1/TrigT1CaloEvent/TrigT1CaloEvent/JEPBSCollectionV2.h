/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_JEPBSCOLLECTIONV2_H
#define TRIGT1CALO_JEPBSCOLLECTIONV2_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

namespace LVL1 {

class JetElement;
class JEMEtSums;
class CMXJetTob;
class CMXJetHits;
class CMXEtSums;
 

/** Jet/Energy Processor container for writing bytestream post-LS1.
 *
 *  Contains all the component collections needed to build the ROD.
 *
 *  @author Peter Faulkner
 */

class JEPBSCollectionV2 {

 public:
   JEPBSCollectionV2(const DataVector<JetElement>* jeCollection,
		     const DataVector<JEMEtSums>*  etCollection,
                     const DataVector<CMXJetTob>*  cmxTobCollection,
                     const DataVector<CMXJetHits>* cmxHitCollection,
		     const DataVector<CMXEtSums>*  cmxEtCollection);

   /// Return pointer to jet element collection
   const DataVector<JetElement>* JetElements() const;
   /// Return pointer to energy sums collection
   const DataVector<JEMEtSums>*  EnergySums()  const;
   /// Return pointer to CMX TOBs collection
   const DataVector<CMXJetTob>*  CmxTobs()     const;
   /// Return pointer to CMX hit sums collection
   const DataVector<CMXJetHits>* CmxHits()     const;
   /// Return pointer to CMX energy sums collection
   const DataVector<CMXEtSums>*  CmxSums()     const;

 private:

   /// Jet element collection
   const DataVector<JetElement>* m_jeCollection;
   /// Energy sums collection
   const DataVector<JEMEtSums>*  m_etCollection;
   /// CMX TOBs collection
   const DataVector<CMXJetTob>*  m_cmxTobCollection;
   /// CMX Hit sums collection
   const DataVector<CMXJetHits>* m_cmxHitCollection;
   /// CMX Energy sums collection
   const DataVector<CMXEtSums>*  m_cmxEtCollection;

};

inline const DataVector<JetElement>* JEPBSCollectionV2::JetElements() const
{
  return m_jeCollection;
}

inline const DataVector<JEMEtSums>* JEPBSCollectionV2::EnergySums() const
{
  return m_etCollection;
}

inline const DataVector<CMXJetTob>* JEPBSCollectionV2::CmxTobs() const
{
  return m_cmxTobCollection;
}

inline const DataVector<CMXJetHits>* JEPBSCollectionV2::CmxHits() const
{
  return m_cmxHitCollection;
}

inline const DataVector<CMXEtSums>* JEPBSCollectionV2::CmxSums() const
{
  return m_cmxEtCollection;
}

} // end namespace

CLASS_DEF(LVL1::JEPBSCollectionV2, 1243139662, 1)

#endif

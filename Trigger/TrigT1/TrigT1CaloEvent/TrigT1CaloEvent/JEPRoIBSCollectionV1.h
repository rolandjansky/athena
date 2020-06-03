/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_JEPROIBSCOLLECTIONV1_H
#define TRIGT1CALO_JEPROIBSCOLLECTIONV1_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

namespace LVL1 {

class JEMRoI;
class CMMRoI;
class CMMJetHits;
class CMMEtSums;
 

/** Jet/Energy Processor RoI container for writing bytestream pre-LS1.
 *
 *  Contains all the component collections needed to build the ROD.
 *
 *  @author Peter Faulkner
 */

class JEPRoIBSCollectionV1 {

 public:
   JEPRoIBSCollectionV1(const DataVector<JEMRoI>*     jemRoiCollection,
                        const CMMRoI*                 cmmRoi,
                        const DataVector<CMMJetHits>* cmmHitCollection,
		        const DataVector<CMMEtSums>*  cmmEtCollection);

   /// Return pointer to JEM RoI collection
   const DataVector<JEMRoI>*     JemRoi()  const;
   /// Return pointer to CMM RoIs
   const CMMRoI*                 CmmRoi()  const;
   /// Return pointer to CMM hit sums collection
   const DataVector<CMMJetHits>* CmmHits() const;
   /// Return pointer to CMM energy sums collection
   const DataVector<CMMEtSums>*  CmmSums() const;

 private:

   /// JEM RoI collection
   const DataVector<JEMRoI>*     m_jemRoiCollection;
   /// CMM RoIs
   const CMMRoI*                 m_cmmRoi;
   /// CMM Hit sums collection
   const DataVector<CMMJetHits>* m_cmmHitCollection;
   /// CMM Energy sums collection
   const DataVector<CMMEtSums>*  m_cmmEtCollection;

};

inline const DataVector<JEMRoI>* JEPRoIBSCollectionV1::JemRoi() const
{
  return m_jemRoiCollection;
}

inline const CMMRoI* JEPRoIBSCollectionV1::CmmRoi() const
{
  return m_cmmRoi;
}

inline const DataVector<CMMJetHits>* JEPRoIBSCollectionV1::CmmHits() const
{
  return m_cmmHitCollection;
}

inline const DataVector<CMMEtSums>* JEPRoIBSCollectionV1::CmmSums() const
{
  return m_cmmEtCollection;
}

} // end namespace

CLASS_DEF(LVL1::JEPRoIBSCollectionV1, 1316106214, 1)

#endif

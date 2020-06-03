/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_JEPROIBSCOLLECTION_H
#define TRIGT1CALO_JEPROIBSCOLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

namespace LVL1 {

class JEMRoI;
class CMMRoI;
class CMMJetHits;
class CMMEtSums;
 

/** Jet/Energy Processor RoI container for writing bytestream.
 *
 *  Contains all the component collections needed to build the ROD.
 *
 *  @author Peter Faulkner
 */

class JEPRoIBSCollection {

 public:
   JEPRoIBSCollection(const DataVector<JEMRoI>*     jemRoiCollection,
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

inline const DataVector<JEMRoI>* JEPRoIBSCollection::JemRoi() const
{
  return m_jemRoiCollection;
}

inline const CMMRoI* JEPRoIBSCollection::CmmRoi() const
{
  return m_cmmRoi;
}

inline const DataVector<CMMJetHits>* JEPRoIBSCollection::CmmHits() const
{
  return m_cmmHitCollection;
}

inline const DataVector<CMMEtSums>* JEPRoIBSCollection::CmmSums() const
{
  return m_cmmEtCollection;
}

} // end namespace

CLASS_DEF(LVL1::JEPRoIBSCollection, 1266611723, 1)

#endif

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGT1CALO_JEPROIBSCOLLECTIONV2_H
#define TRIGT1CALO_JEPROIBSCOLLECTIONV2_H

#include "CLIDSvc/CLASS_DEF.h"
#include "DataModel/DataVector.h"

namespace LVL1 {

class JEMTobRoI;
class CMXRoI;
class CMXEtSums;
 

/** Jet/Energy Processor RoI container for writing bytestream post-LS1.
 *
 *  Contains all the component collections needed to build the ROD.
 *
 *  @author Peter Faulkner
 */

class JEPRoIBSCollectionV2 {

 public:
   JEPRoIBSCollectionV2(const DataVector<JEMTobRoI>*  jemRoiCollection,
                        const CMXRoI*                 cmxRoi,
		        const DataVector<CMXEtSums>*  cmxEtCollection);

   /// Return pointer to JEM RoI collection
   const DataVector<JEMTobRoI>*  JemRoi()  const;
   /// Return pointer to CMX RoIs
   const CMXRoI*                 CmxRoi()  const;
   /// Return pointer to CMX energy sums collection
   const DataVector<CMXEtSums>*  CmxSums() const;

 private:

   /// JEM RoI collection
   const DataVector<JEMTobRoI>*  m_jemRoiCollection;
   /// CMX RoIs
   const CMXRoI*                 m_cmxRoi;
   /// CMX Energy sums collection
   const DataVector<CMXEtSums>*  m_cmxEtCollection;

};

inline const DataVector<JEMTobRoI>* JEPRoIBSCollectionV2::JemRoi() const
{
  return m_jemRoiCollection;
}

inline const CMXRoI* JEPRoIBSCollectionV2::CmxRoi() const
{
  return m_cmxRoi;
}

inline const DataVector<CMXEtSums>* JEPRoIBSCollectionV2::CmxSums() const
{
  return m_cmxEtCollection;
}

} // end namespace

CLASS_DEF(LVL1::JEPRoIBSCollectionV2, 1316106213, 1)

#endif

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALO_MERGESLICES_H
#define TRIGT1CALO_MERGESLICES_H

#include <string>
#include <map>

#include "AthContainers/DataVector.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

namespace LVL1 {

class CPMHits;
class CPMTower;
class JEMEtSums;
class JEMHits;
class JetElement;
class TriggerTower;


/** Algorithm to merge individually processed slices into multi-slice
 *  collections for bytestream.
 *
 *  @author Peter Faulkner
 */

class MergeSlices : public AthAlgorithm {

 public:
   MergeSlices(const std::string& name, ISvcLocator* pSvcLocator);
   virtual ~MergeSlices();

   virtual StatusCode initialize();
   virtual StatusCode execute();
   virtual StatusCode finalize();

 private:

   typedef DataVector<TriggerTower>              TriggerTowerCollection;
   typedef DataVector<CPMTower>                  CPMTowerCollection;
   typedef DataVector<CPMHits>                   CPMHitsCollection;
   typedef DataVector<JetElement>                JetElementCollection;
   typedef DataVector<JEMHits>                   JEMHitsCollection;
   typedef DataVector<JEMEtSums>                 JEMEtSumsCollection;
   typedef std::map<unsigned int, TriggerTower*> TriggerTowerMap;
   typedef std::map<unsigned int, CPMTower*>     CPMTowerMap;
   typedef std::map<int, CPMHits*>               CPMHitsMap;
   typedef std::map<unsigned int, JetElement*>   JetElementMap;
   typedef std::map<int, JEMHits*>               JEMHitsMap;
   typedef std::map<int, JEMEtSums*>             JEMEtSumsMap;

   /// Merge PPM containers
   StatusCode mergePpm();
   /// Merge CPM containers
   StatusCode mergeCpm();
   /// Merge JEM containers
   StatusCode mergeJem();

   /// Return number as a string
   std::string numString(int num);

   //  Container StoreGate keys
   std::string m_cpmHitsLocation;
   std::string m_cpmTowerLocation;
   std::string m_jemEtSumsLocation;
   std::string m_jemHitsLocation;
   std::string m_jetElementLocation;
   std::string m_triggerTowerLocation;
   std::string m_fullAdcLocation;
   /// Number of slices to merge
   int m_slicesLut;

};

} // end namespace

#endif

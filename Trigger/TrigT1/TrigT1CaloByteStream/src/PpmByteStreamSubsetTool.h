/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_PPMBYTESTREAMSUBSETTOOL_H
#define TRIGT1CALOBYTESTREAM_PPMBYTESTREAMSUBSETTOOL_H

#include <stdint.h>

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"
#include "DataModel/DataVector.h"
#include "eformat/SourceIdentifier.h"
#include "GaudiKernel/ToolHandle.h"

#include "IPpmByteStreamSubsetTool.h"
#include "PpmSubBlock.h"

class IInterface;
class StatusCode;

namespace LVL1 {
  class IL1CaloMappingTool;
  class TriggerTower;
  class TriggerTowerKey;
}

namespace LVL1BS {

class L1CaloSrcIdMap;

/** Tool to perform ROB fragments to trigger towers conversions.
 *
 *  Based on ROD document version 1_09h.
 *
 *  @author Peter Faulkner
 */

class PpmByteStreamSubsetTool : virtual public IPpmByteStreamSubsetTool,
                                        public AthAlgTool {

 public:
   PpmByteStreamSubsetTool(const std::string& type, const std::string& name,
                           const IInterface* parent);
   virtual ~PpmByteStreamSubsetTool();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Convert ROB fragments to trigger towers
   virtual StatusCode convert(const IROBDataProviderSvc::VROBFRAG& robFrags,
                              DataVector<LVL1::TriggerTower>* ttCollection,
		              const std::vector<unsigned int>& chanIds);
   virtual void eventNumber(const unsigned int eN ) { m_event=eN;};

 private:
   typedef DataVector<LVL1::TriggerTower>                TriggerTowerCollection;
   typedef std::map<unsigned int, LVL1::TriggerTower*>   TriggerTowerMap;
   typedef IROBDataProviderSvc::VROBFRAG::const_iterator ROBIterator;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      ROBPointer;
   typedef OFFLINE_FRAGMENTS_NAMESPACE::PointerType      RODPointer;
   typedef std::pair<std::vector<unsigned int>::const_iterator,
                     std::vector<unsigned int>::const_iterator> IteratorPair;
   typedef std::map<unsigned int, IteratorPair>          ChannelMap;

   /// Find a trigger tower given eta, phi
   LVL1::TriggerTower* findTriggerTower(double eta, double phi);

   /// Print a vector
   void printVec(const std::vector<int>& vec) const;

   /// Tool for mappings
   ToolHandle<LVL1::IL1CaloMappingTool> m_ppmMaps;

   /// Number of channels per module (may not all be used)
   int m_channels;
   /// Number of crates
   int m_crates;
   /// Number of modules per crate (may not all exist)
   int m_modules;
   /// Sub-detector type
   eformat::SubDetector m_subDetector;
   /// Zero suppression on input
   bool m_zeroSuppress;
   /// Source ID converter
   L1CaloSrcIdMap* m_srcIdMap;
   /// Trigger tower key provider
   LVL1::TriggerTowerKey* m_towerKey;
   /// Current error block
   PpmSubBlock* m_errorBlock;
   /// Trigger tower map for conversion from bytestream
   TriggerTowerMap m_ttMap;
   /// Vector for current PPM sub-blocks. Changed type
   std::vector<PpmSubBlock*> m_ppmBlocks;
   /// Pool for current PPM sub-blocks
   PpmSubBlock* m_ppmBlocksPool[10];
   // To simplify life, let's prepare maps beforehand
   // Organized by crate (8), module (16) and channel (64)
   // eta and phi maps could be avoided. Only used in DEBUG
   // building of the code
   double etamap[8192];
   double phimap[8192];
   int layermap [8192];
   unsigned int uniqueness[8192];
   // The same for new tt addresses (no mem allocation)
   LVL1::TriggerTower* ttpool[8192];
   // Cache temporary vector
   std::vector<LVL1::TriggerTower*> ttTemp;
   // Cache channel map
   ChannelMap chanMap;
   ChannelMap chanMapFull;
   // Get channel maps for full calo unpack
   bool m_first;
   // Caching some variables
   PpmSubBlock testBlock;
   // counter of present event
   unsigned int m_event;


};

} // end namespace

#endif

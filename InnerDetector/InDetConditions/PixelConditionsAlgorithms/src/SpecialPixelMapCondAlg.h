/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SPECIAL_PIXEL_MAPCONDALG_H
#define SPECIAL_PIXEL_MAPCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include <vector>
#include <string>
#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#include "PixelConditionsData/SpecialPixelMap.h"
#include "GaudiKernel/ICondSvc.h"

class PixelID;
class IGeoModelSvc;

class SpecialPixelMapCondAlg: public AthReentrantAlgorithm {

   public:
  SpecialPixelMapCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  ~SpecialPixelMapCondAlg();
  
  virtual StatusCode initialize() override;
  virtual StatusCode execute_r (const EventContext&) const override;


private:
  unsigned int getChips(const unsigned int index) const;
  StatusCode createFromDetectorStore(const CondAttrListCollection*, 
    bool fillMissing, DetectorSpecialPixelMap*& ) const;

  StatusCode createMaskingOverlay(DetectorSpecialPixelMap*&) const;
  StatusCode registerCondAttrListCollection(const DetectorSpecialPixelMap* ,
     const DetectorSpecialPixelMap* , const EventContext&, const EventIDRange &r ) const;

  const PixelID* m_pixelID;
  Gaudi::Property<bool> m_forceNewDBContent{this, "ForceNewDBContent", false};
  Gaudi::Property<bool> m_dummy{this, "MakingDummyMaps", false};
  Gaudi::Property<bool> m_isIBL{this, "isIBL", false};
  Gaudi::Property<bool> m_mergePixelMaps{this, "MergePixelMaps", true};
  Gaudi::Property<bool> m_moduleLevelOverlay{this, "ModuleLevelOverlay", false};
  Gaudi::Property<bool> m_maskLayers{this, "MaskLayers",  false, "Mask full layers/disks in overlay"};
  Gaudi::Property<bool> m_differentialUpdates{"DifferentialUpdates", false, "Switch for differential updates"};
  Gaudi::Property<bool> m_useDualFolderStructure{"UseDualFolderStructure", false, 
		  "Use dual folder structure when creating a CondAttrListCollection"};
  Gaudi::Property<bool> m_writeBlobs{"WriteBlobs", false, "Switch between blob and clob writing"};

  Gaudi::Property<std::vector<std::string>> m_condAttrListCollectionKeys{this, "DBFolders", { }, "list of database folders to be accessed" }; 
  Gaudi::Property<std::vector<std::string>> m_specialPixelMapKeys{this, "SpecialPixelMapKeys", {},  "StoreGate keys at which pixel maps are to be stored" };
  SG::ReadCondHandleKey<CondAttrListCollection> m_overlayFolder{this, "OverlayFolder", ""};
  SG::WriteCondHandleKey<DetectorSpecialPixelMap> m_overlayKey{this , "OverlayKey", "PixMapOverlay"};
  SG::WriteCondHandleKey<DetectorSpecialPixelMap> m_maskingOverlayKey{this , "MaskingOverlayKey", "MaskingOverlay"};

  SG::WriteCondHandleKey<CondAttrListCollection> m_outputFolder{this, "OutputFolder", "", "Name of output folder"};
  SG::WriteCondHandleKey<CondAttrListCollection> m_outputLongFolder{this, "OutputLongFolder", "", "Name of output folder for long maps"};

  ServiceHandle< IGeoModelSvc > m_geoModelSvc;
  std::unordered_map<unsigned int, unsigned int> m_chips;
  SG::ReadCondHandleKey<CondAttrListCollection> m_ConAttrList1{this, "ConAttrList1", ""};
  SG::ReadCondHandleKey<CondAttrListCollection> m_ConAttrList2{this, "ConAttrList2", ""};
  SG::ReadCondHandleKey<CondAttrListCollection> m_ConAttrList3{this, "ConAttrList3", ""};
  SG::ReadCondHandleKey<CondAttrListCollection> m_ConAttrList4{this, "ConAttrList4", ""};

  SG::WriteCondHandleKey<DetectorSpecialPixelMap> m_pixmap1{this , "PixelMap1", ""};
  SG::WriteCondHandleKey<DetectorSpecialPixelMap> m_pixmap2{this , "PixelMap2", ""};
  SG::WriteCondHandleKey<DetectorSpecialPixelMap> m_pixmap3{this , "PixelMap3", ""};
  SG::WriteCondHandleKey<DetectorSpecialPixelMap> m_pixmap4{this , "PixelMap4", ""};

  SG::ReadCondHandleKey<CondAttrListCollection> m_overlayLongFolder{this , "OverlayLongFolder", "", "Name of long overlay DB folder"};
  SG::WriteCondHandleKey<DetectorSpecialPixelMap> m_overlayLongKey{this , "OverlayLongKey", "PixMapOverlayLong", "StoreGate key for long overlay map"};

  Gaudi::Property<std::vector<int>> m_layersToMask{this, "LayersToMask", {}, "Which barrel layers to mask out, goes from 0 to N-1"};
  Gaudi::Property<std::vector<int>> m_disksToMask{this, "DisksToMask", {}, "Which endcap disks to mask out, goes from -N+1 to N+1 , skipping zero"};
  ServiceHandle<ICondSvc> m_condSvc;
};


#endif


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELCONDITIONSSERVICES_SPECIALPIXELMAPSVC_H
#define PIXELCONDITIONSSERVICES_SPECIALPIXELMAPSVC_H

#include "AthenaBaseComps/AthService.h"
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h" 

#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IOVSvcDefs.h"

#include <map>

class StoreGateSvc;
class IIOVDbSvc;
class PixelID;
class IBLParameterSvc;

namespace InDetDD{ 
  class PixelDetectorManager; 
}


/** 
 * Creates and manages a set of special pixel map objects in the detector store. If not explicitly switched
 * off in the job options, a callback funtion is registered that creates the special pixel map objects in the
 * detector store and automatically keeps them up to date for the current run/lumi block. By default all
 * special pixel maps are merged into one map which is stored in place of the first map in the list of
 * StoreGate keys.
 *
 * In addition to the callback function the following functions are provided: A special pixel map object can
 * be created explicitly with the create method. This method can be configured to either use the data from
 * the offline COOL database or from text files. The contents of maps for selected modules can be printed to
 * the screen with print(). The number of maps managed can be retrieved with getNPixelMaps(), and the maps
 * can be retrieved with getPixelMap(unsigned int index). 
 *
 * ruwiedel@physik.uni-bonn.de 
 *
 */

class SpecialPixelMapSvc : public AthService, virtual public ISpecialPixelMapSvc{
  
public:
  static const InterfaceID& interfaceID();

  SpecialPixelMapSvc(const std::string& name, ISvcLocator* sl);
  virtual ~SpecialPixelMapSvc ();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvIf); 
  
  /** automatically creates and updates pixel map objects in the detector store for the current run/lumi block */
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS); //!< callback function registered in initialize()

  StatusCode createFromTextfiles(bool fillMissing = true) const; 
  //!< create special pixel map objects using data read from text files (for details see the README file in the doc directory)
  StatusCode createFromDetectorStore(const std::string condAttrListCollectionKey, const std::string pixelMapKey,
				     bool fillMissing = true) const;
  //!< create special pixel map from CondAttrListCollection at key
  StatusCode createFromDetectorStore(bool fillMissing = true) const; 
  //!< create special pixel map objects using data from the offline COOL database
  StatusCode createDeadModuleList() const;
  //!<create PixelMapOverlap by randomly killing some fraction of modules
  StatusCode createMaskingOverlay(const std::string internalKey) const;
  StatusCode create() const; 
  //!< create special pixel map objects using the data source specified in the job options
  StatusCode print() const; 
  //!< print pixel maps for the modules specified in the job options to the screen

  unsigned int getNPixelMaps() const { return m_specialPixelMapKeys.size(); }
  //!< returns the number of special pixel map objects managed by the SpecialPixelMapSvc
  const DetectorSpecialPixelMap *getPixelMap(unsigned int index) const;
  //!< returns a const pointer to the index'th special pixel map

  StatusCode registerCondAttrListCollection(const DetectorSpecialPixelMap* spm) const;
  //!< convert spm to CondAttrListCollection and register in the detector store
  //!< used when preparing database write
 
private:
  
  unsigned int getChips(const unsigned int & index) const;

  ServiceHandle< StoreGateSvc > m_detStore;
  ServiceHandle< IIOVDbSvc > m_iovDbSvc;

  std::vector<std::string> m_condAttrListCollectionKeys; 
  std::vector<std::string> m_specialPixelMapKeys;
  std::string m_overlayFolder;
  std::string m_overlayLongFolder;
  std::string m_overlayKey;
  std::string m_overlayLongKey;
  bool m_moduleLevelOverlay;
  std::string m_dataSource;
  std::vector<std::string> m_moduleIDsForPrinting;
  std::string m_outputFolder;
  std::string m_outputLongFolder;
  ServiceHandle< IBLParameterSvc> m_IBLParameterSvc;
  bool m_registerCallback;
  bool m_verbosePixelID;
  bool m_binaryPixelStatus;
  bool m_markSpecialRegions;
  bool m_mergePixelMaps;
  bool m_differentialUpdates;
  bool m_useDualFolderStructure;
  bool m_maskLayers;

  bool m_writeBlobs;

  bool m_forceNewDBContent;
  
  std::string m_connectionString;
  std::string m_connectivityTag;
  std::string m_aliasTag;
  std::string m_fileListFileName;
  std::string m_fileListFileDir;

  double m_killingModule;
  std::vector<int> m_layersToMask;
  std::vector<int> m_disksToMask;
 
  std::map<unsigned int, unsigned int> m_chips;

  const PixelID* m_pixelID;
  const InDetDD::PixelDetectorManager* m_pixman; 

};

inline const InterfaceID& SpecialPixelMapSvc::interfaceID(){
  static const InterfaceID IID_SpecialPixelMapSvc("SpecialPixelMapSvc", 1, 0); 
  return IID_SpecialPixelMapSvc;
}

#endif 

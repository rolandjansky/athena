/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGL2SPACEPOINTTRUTHTOOL_H__
#define __TRIGL2SPACEPOINTTRUTHTOOL_H__


#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigInDetToolInterfaces/ITrigL2SpacePointTruthTool.h"
#include <vector>

//#include "InDetIdentifier/SCT_ID.h"
//#include "InDetIdentifier/PixelID.h" 
//#include "InDetReadoutGeometry/PixelDetectorManager.h"
//#include "InDetReadoutGeometry/SCT_DetectorManager.h"

// detector geometry and helper classes
#include "InDetSimData/InDetSimDataCollection.h"
class SCT_ID;
class PixelID;
// monte carlo edm
class KineHepMcmap;
class GenParticle;

class TrigL2SpacePointTruthTool : virtual public ITrigL2SpacePointTruthTool, public AthAlgTool {
 public:
      
  // standard AthAlgTool methods
  TrigL2SpacePointTruthTool(const std::string&,const std::string&,const IInterface*);
  virtual ~TrigL2SpacePointTruthTool();
		
  // standard Athena methods
  StatusCode initialize();
  StatusCode finalize();
  virtual void getBarCodes(std::vector<TrigSiSpacePoint*>&, std::vector<int>&);
  virtual void getBarCodes(std::vector<TrigSiSpacePointBase>&, std::vector<int>&);
  //  virtual void getBarCodes(const TrigSiSpacePoint*, std::vector<int>&, std::vector<float>&);
  // virtual int  getBarCode(const TrigSiSpacePoint*);
 private:

  /** SDO maps for */
  std::string m_pixel_SDO_Map; //!< Pixel
  std::string m_SCT_SDO_Map; //!< SCT

  /** Internal properties */
  bool m_havePIXmap; //!< set for each event
  bool m_haveSCTmap;

  /** Pointers to Identifier helper classes for */
  const PixelID* m_PIXid; //!< Pixel detector data
  const SCT_ID*  m_SCTid; //!< SCT detector data

  /** Maps: std::map<Identifier,InDetSimData> */
  const InDetSimDataCollection* m_id2SimDataMapPIX;
  const InDetSimDataCollection* m_id2SimDataMapSCT;

  void SCT_ClusterTruth(const Trk::PrepRawData*,std::vector<HepMcParticleLink>&);
  void PIX_ClusterTruth(const Trk::PrepRawData*,std::vector<HepMcParticleLink>&);
  int analyzeClusterMatchings(std::vector<HepMcParticleLink>& vPL);
  bool GetTruthMaps(); 
  void updatePLvector(std::vector<HepMcParticleLink>&, InDetSimDataCollection::const_iterator& simData);

};

#endif 

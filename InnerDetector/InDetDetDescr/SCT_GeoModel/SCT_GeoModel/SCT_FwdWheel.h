/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDWHEEL_H
#define SCT_GEOMODEL_SCT_FWDWHEEL_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

#include <memory>
#include <vector>


class SCT_FwdRing;
class SCT_FwdDiscFixation;
class SCT_FwdDiscSupport;
class SCT_FwdPatchPanel;
class SCT_FwdPPConnector;
class SCT_FwdPPCooling;
class SCT_FwdModule;
class SCT_FwdFSI;

class FSIDetails;

class SCT_FwdWheel : public SCT_UniqueComponentFactory
{

public:
  SCT_FwdWheel(const std::string & name,
	       int iWheel,
	       const std::vector<SCT_FwdModule *> & modules,
	       int ec,
               InDetDD::SCT_DetectorManager* detectorManager,
               const SCT_GeometryManager* geometryManager,
               SCT_MaterialManager* materials);
  ~SCT_FwdWheel();
  //Explicitly disallow copy and assignment to appease coverity
  SCT_FwdWheel(const SCT_FwdWheel &) = delete;
  SCT_FwdWheel & operator=(const SCT_FwdWheel &) = delete;
  
  int wheelNum() const {return m_iWheel;}

  // Retrieve basic parameters
  //
  double zPosition() const {return m_zPosition;} 
  int    numRings() const {return m_numRings;}
  int    ringType(int i) const {return m_ringType[i];} 
  const  SCT_FwdRing* ring(int i) {return m_rings[i].get();}

  //
  // Retrieve derived parameters
  //
  double thickness() const {return m_thickness;}
  double innerRadius() const {return m_innerRadius;} 
  double outerRadius() const {return m_outerRadius;} 
  int    totalModules() const {return m_totalModules;}

  virtual GeoVPhysVol * build(SCT_Identifier id);
  
 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  int m_iWheel;
  int m_endcap;

  // Basic parameters
  int      m_numRings = 0;
  double   m_zPosition = 0.0;
  std::vector<int> m_ringType;
  std::vector<int> m_patchPanelType;
  std::vector<double> m_patchPanelLocAngle;
  std::vector<bool> m_patchPanelRepeatQuadrant;
  int m_numPatchPanelLocs = 0;
  int m_numPatchPanelTypes = 0;
  int m_numFSITypes = 0;
  int m_numWheels = 0;
  bool m_optoHarnessPresent = false;
  bool m_pPConnectorPresent = false;
  bool m_pPCoolingPresent = false;
  bool m_discFixationPresent = false;
  std::vector<double> m_discFixationLocAngle;

  // Derived parameters
  double m_innerRadius = 0.0;
  double m_outerRadius = 0.0;
  double m_thickness = 0.0;
  double m_thicknessFront = 0.0;
  double m_thicknessBack = 0.0;
  int    m_totalModules = 0;
  int    m_rotateWheel = 0;
  double m_ringMaxRadius = 0.0;

  double m_safety = 0.0;

  // Child detector elements
  std::vector<std::unique_ptr<SCT_FwdRing>> m_rings;
  std::unique_ptr<SCT_FwdDiscSupport> m_discSupport;
  std::vector<std::unique_ptr<SCT_FwdPatchPanel>> m_patchPanel;
  std::unique_ptr<SCT_FwdPPConnector> m_pPConnector;
  std::unique_ptr<SCT_FwdPPCooling> m_pPCooling;
  std::vector<std::unique_ptr<SCT_FwdFSI>> m_fsiType;
  std::unique_ptr<SCT_FwdDiscFixation> m_discFixation;

  const std::vector<SCT_FwdModule*> & m_modules;


  const std::vector<const FSIDetails *> * m_fsiVector = nullptr;


};

#endif // SCT_GEOMODEL_SCT_FWDWHEEL_H


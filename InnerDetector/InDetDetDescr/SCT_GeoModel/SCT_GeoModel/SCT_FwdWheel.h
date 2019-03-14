/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FWDWHEEL_H
#define SCT_GEOMODEL_SCT_FWDWHEEL_H

#include "SCT_GeoModel/SCT_ComponentFactory.h"

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
  //int    ringStereoType(int i) const {return m_ringStereoType[i];} 
  const  SCT_FwdRing* ring(int i) {return m_rings[i];}

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
  int      m_numRings;
  double   m_zPosition;
  std::vector<int> m_ringType;
  std::vector<int> m_patchPanelType;
  std::vector<double> m_patchPanelLocAngle;
  std::vector<bool> m_patchPanelRepeatQuadrant;
  int m_numPatchPanelLocs;
  int m_numPatchPanelTypes;
  int m_numFSITypes;
  int m_numWheels;
  bool m_optoHarnessPresent;
  bool m_pPConnectorPresent;
  bool m_pPCoolingPresent;
  bool m_discFixationPresent;
  std::vector<double> m_discFixationLocAngle;

  // Derived parameters
  double m_innerRadius;
  double m_outerRadius;
  double m_thickness;
  double m_thicknessFront;
  double m_thicknessBack;
  int    m_totalModules;
  int    m_rotateWheel;
  double m_ringMaxRadius;

  double m_safety;

  // Child detector elements
  std::vector<SCT_FwdRing *>      m_rings;
  SCT_FwdDiscSupport *            m_discSupport;
  std::vector<SCT_FwdPatchPanel*> m_patchPanel;
  SCT_FwdPPConnector*             m_pPConnector;
  SCT_FwdPPCooling*               m_pPCooling;
  std::vector<SCT_FwdFSI*>        m_fsiType;
  SCT_FwdDiscFixation*            m_discFixation;

  const std::vector<SCT_FwdModule*> & m_modules;


  const std::vector<const FSIDetails *> * m_fsiVector;


};

#endif // SCT_GEOMODEL_SCT_FWDWHEEL_H


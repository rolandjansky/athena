/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelLayerValidationTool_H
#define PixelLayerValidationTool_H

#include "PixelInterfaces/IPixelLayerValidationTool.h"
#include "PixelGeoModel/PixelGeoBuilder.h"
#include "BarrelInclinedRef/GeoPixelLadderInclRef.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "InDetTrackingGeometryXML/XMLReaderSvc.h"
#include "PixelInterfaces/IPixelServicesTool.h"
#include "GaudiKernel/ITHistSvc.h"
#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"

class GeoPhysVol;
class GeoTransform;

class PixelLayerValidationTool : virtual public IPixelLayerValidationTool, public AthAlgTool {

 public:
  PixelLayerValidationTool(const std::string&,const std::string&,const IInterface*);
  ~PixelLayerValidationTool();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  void printInfo(const GeoVPhysVol* vol, int iLayer) const;
 
 private:

  void printChildren(const PVConstLink pv,int gen, int igen, Amg::Transform3D trIn) const;

  ServiceHandle<InDet::XMLReaderSvc> m_xmlReader;
  ToolHandle<IPixelServicesTool> m_IDserviceTool;

  /// get a handle on the Hist/TTree registration service
  ITHistSvc * m_thistSvc;

  // histograms
  std::vector<TH2F*> m_coverage;
  std::vector<TH2F*> m_coverage15;
  std::vector<TH2F*> m_coverage15n;

  std::vector<TH2F*> m_hitrate;
  std::vector<TH2F*> m_hitrate15;
  std::vector<TH2F*> m_hitrate15n;
 
  mutable TH1F* m_dInX0_mod; 
  mutable TH1F* m_dInX0_svc; 
  mutable TH1F* m_dInX0_stave; 

  TH1F* m_dInX0_mod_flat; 
  TH1F* m_dInX0_svc_flat; 
  TH1F* m_dInX0_stave_flat; 

  mutable float binEta;
  mutable float binPhi;
};

#endif

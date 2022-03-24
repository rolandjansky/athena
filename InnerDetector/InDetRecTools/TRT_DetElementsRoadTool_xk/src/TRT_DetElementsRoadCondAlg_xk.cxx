/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_DetElementsRoadUtils_xk.h"

#include "TRT_DetElementsRoadCondAlg_xk.h"

#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsLayer_xk.h"
#include "TRT_DetElementsRoadTool_xk/TRT_DetElementsComparison.h"
#include "TRT_ReadoutGeometry/TRT_Numerology.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"
#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "TrkSurfaces/CylinderBounds.h"
#include <memory>


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_DetElementsRoadCondAlg_xk::TRT_DetElementsRoadCondAlg_xk(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DetElementsRoadCondAlg_xk::initialize()
{

  ATH_CHECK(m_trtDetEleContKey.initialize());
  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Map of detector elements production
// Taken from InDet::TRT_DetElementsRoadMaker_xk::mapDetectorElementsProduction()
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_DetElementsRoadCondAlg_xk::execute(const EventContext& ctx) const
{

  const double pi2=2.*M_PI, pi=M_PI;

  SG::WriteCondHandle<TRT_DetElementsRoadData_xk> writeHandle{m_writeKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.");
    return StatusCode::SUCCESS;
  }


  EventIDRange rangeTrt;

  SG::ReadCondHandle<InDetDD::TRT_DetElementContainer> trtDetEleHandle(m_trtDetEleContKey, ctx);
  if (not trtDetEleHandle.isValid()) {
    ATH_MSG_FATAL(m_trtDetEleContKey.fullKey() << " is not available.");
    return StatusCode::FAILURE;
  }

  const InDetDD::TRT_Numerology* trtNum = trtDetEleHandle->getTRTNumerology();
  if (trtNum==nullptr){
    ATH_MSG_FATAL("Pointer to TRT_Numerology not found in condition store" << m_trtDetEleContKey.fullKey());
  }

  InDet::TRT_DetElementsLayerVectors_xk layerVectors(3);

  if (not trtDetEleHandle.range(rangeTrt)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << trtDetEleHandle.key());
    return StatusCode::FAILURE;
  }


  double P[33], Wf = 0.,Wz = 0.;
  double mzmin [3] ;  // min Z coordinate 
  double mzmax [3] ;  // max Z coordinate
  double mrmin [3] ;  // min radius
  double mrmax [3] ;  // max radius

  // Barrel
  //
  int N = 1;

  int Rings  = trtNum->getNBarrelRings();
  int NPhi   = trtNum->getNBarrelPhi(); 
  
  mrmin[N] = 100000.; mrmax[N] =-100000.;
  mzmin[N] = 100000.; mzmax[N] =-100000.;

  std::vector<const InDetDD::TRT_BaseElement*> pE;
  pE.reserve(NPhi*2);

  for(int ring = 0; ring!=Rings; ++ring) {
    
    int NSlayers = trtNum->getNBarrelLayers(ring);
    
    for(int nsl=0; nsl!=NSlayers; ++nsl) {
      
      InDet::TRT_DetElementsLayer_xk layer;
      double rmin = 100000., rmax =-100000.;
      double zmin = 100000., zmax =-100000.;
      double dfm  = 0.;
      
      pE.clear(); // RESET PE
      for(int f=0; f!=NPhi; ++f) {
        pE.push_back(trtDetEleHandle->getBarrelDetElement(0,ring,f,nsl));
	pE.push_back(trtDetEleHandle->getBarrelDetElement(1,ring,f,nsl));
      }
      
      std::sort(pE.begin(),pE.end(),InDet::compTRTDetElements_AZ());
      for(unsigned int j=0; j!=pE.size(); ++j) {

	if(pE[j]) {

	  InDet::TRT_DetElementsRoadUtils_xk::detElementInformation(*(pE[j]),P);
	  Wf = sqrt(P[20]*P[20]+P[21]*P[21]);
	  Wz = sqrt(P[22]*P[22]+P[23]*P[23]); 
	  if( P[ 9] < mrmin[N] ) mrmin[N] = P[ 9]; 
	  if( P[10] > mrmax[N] ) mrmax[N] = P[10]; 
	  if( P[11] < mzmin[N] ) mzmin[N] = P[11]; 
	  if( P[12] > mzmax[N] ) mzmax[N] = P[12]; 
	  
	  if( P[ 9] < rmin ) rmin = P[ 9]; 
	  if( P[10] > rmax ) rmax = P[10]; 
	  if( P[11] < zmin ) zmin = P[11]; 
	  if( P[12] > zmax ) zmax = P[12]; 
	  
	  double df1 = std::abs(P[13]-P[2]); if(df1>pi) df1 = std::abs(df1-pi2); 
	  double df2 = std::abs(P[14]-P[2]); if(df2>pi) df2 = std::abs(df2-pi2); 
	  if(df1>dfm) dfm = df1;
	  if(df2>dfm) dfm = df2;
	  InDet::TRT_DetElementLink_xk link(pE[j],P);
	  layer.add(std::move(link));
	}
      }
      double r  =(rmax+rmin)*.5;
      double dr =(rmax-rmin)*.5; 
      double z  =(zmax+zmin)*.5;
      double dz =(zmax-zmin)*.5;
      layer.set(r,dr,z,dz,dfm,Wf,Wz);
      (layerVectors.at(N)).push_back(std::move(layer));

    }
  }

  // Endcaps
  //
  int Wheels = trtNum->getNEndcapWheels();
  NPhi       = trtNum->getNEndcapPhi(); 
  if(Wheels ) {

    for(N=0; N<3; N+=2) {

      mrmin[N] = 100000.; mrmax[N] =-100000.;
      mzmin[N] = 100000.; mzmax[N] =-100000.;
      
      int side = 0; if(N==2) side = 1;
      for(int wh = 0; wh!=Wheels; ++wh) {
	
	int ns = trtNum->getNEndcapLayers(wh);
	for(int s = 0; s!=ns; ++s) {
	  
	  InDet::TRT_DetElementsLayer_xk layer;
	  double rmin = 100000., rmax =-100000.;
	  double zmin = 100000., zmax =-100000.;
	  double dfm  = 0.;
	  pE.clear(); //Reset Pe
	  
	  for(int f=0; f!=NPhi; ++f) {
	    pE.push_back(trtDetEleHandle->getEndcapDetElement(side,wh,s,f));
	  }
	  std::sort(pE.begin(),pE.end(),InDet::compTRTDetElements_A());
	  
	  for(unsigned int j=0; j!=pE.size(); ++j) {
	    
	    if(pE[j]) {

	      InDet::TRT_DetElementsRoadUtils_xk::detElementInformation(*(pE[j]),P);
	      Wf = sqrt(P[20]*P[20]+P[21]*P[21]);
	      Wz = sqrt(P[22]*P[22]+P[23]*P[23]); 
	      
	      if( P[ 9] < mrmin[N] ) mrmin[N] = P[ 9]; 
	      if( P[10] > mrmax[N] ) mrmax[N] = P[10]; 
	      if( P[11] < mzmin[N] ) mzmin[N] = P[11]; 
	      if( P[12] > mzmax[N] ) mzmax[N] = P[12]; 
	      
	      if( P[ 9] < rmin ) rmin = P[ 9]; 
	      if( P[10] > rmax ) rmax = P[10]; 
	      if( P[11] < zmin ) zmin = P[11]; 
	      if( P[12] > zmax ) zmax = P[12]; 
	      
	      double df1 = std::abs(P[13]-P[2]); if(df1>pi) df1 = std::abs(df1-pi2); 
	      double df2 = std::abs(P[14]-P[2]); if(df2>pi) df2 = std::abs(df2-pi2); 
	      if(df1>dfm) dfm = df1;
	      if(df2>dfm) dfm = df2;
	      
	      InDet::TRT_DetElementLink_xk link(pE[j],P);
	      layer.add(std::move(link));
	    }
	  }
	  double r  =(rmax+rmin)*.5;
	  double dr =(rmax-rmin)*.5; 
	  double z  =(zmax+zmin)*.5;
	  double dz =(zmax-zmin)*.5;
	  layer.set(r,dr,z,dz,dfm,Wf,Wz);
	  (layerVectors.at(N)).push_back(std::move(layer));
	}
      }
    }
  }

  double zmi = +100000.;
  double zma = -100000.;
  double rma = -100000.;
  double rmi = +100000.;
  for(int i=0; i!=3; ++i) {
    if(!layerVectors[i].empty()) {
      if(mzmin[i]<zmi) zmi=mzmin[i];
      if(mzmax[i]>zma) zma=mzmax[i];
      if(mrmax[i]>rma) rma=mrmax[i];
      if(mrmin[i]<rmi) rmi=mrmin[i];
    }
  }

  double hz = std::abs(zma); 
  if(hz<std::abs(zmi)) hz = std::abs(zmi);

  const Trk::CylinderBounds CB(rma+20.,hz+20.);
  std::unique_ptr<InDet::TRT_DetElementsRoadData_xk> writeCdo{std::make_unique<InDet::TRT_DetElementsRoadData_xk>()};
  writeCdo->setTRTLayerVectors(std::move(layerVectors));
  writeCdo->setBounds(CB,rmi);

  if (writeHandle.record(rangeTrt, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandle.key()
                  << " with EventRange " << rangeTrt
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeTrt << " into Conditions Store");

  return StatusCode::SUCCESS;
}

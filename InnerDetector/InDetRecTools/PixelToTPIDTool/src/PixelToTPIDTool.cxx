/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelToTPIDTool/PixelToTPIDTool.h"

// StoreGate, Athena, and Database stuff:
#include "Identifier/Identifier.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

// Tracking:
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/TrackInfo.h"

// Pixels:
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
#include "InDetIdentifier/PixelID.h"
#include "PixelGeoModel/IBLParameterSvc.h" 

// CLHEP:
#include "CLHEP/Matrix/Vector.h"

// Particle masses

// Math functions:
#include <cmath>

#include "PathResolver/PathResolver.h"

InDet::PixelToTPIDTool::PixelToTPIDTool(const std::string& t, const std::string& n, const IInterface*  p )
  :AthAlgTool(t,n,p),
  m_IBLParameterSvc("IBLParameterSvc",n),
  m_pixelid(nullptr)
{
  declareInterface<IPixelToTPIDTool>(this);

  float energyPair = 3.68e-6; // Energy in MeV to create an electron-hole pair in silicon
  float sidensity = 2.329; // silicon density in g cm^-3
 
  //conversion Factor
  //{.025,.023,.020}; //{Old Planars,IBL_3Ds,IBL_Planars} the sensors thickness will be take into account in dEdx calculation
  
  m_conversionfactor=energyPair/sidensity;

}

InDet::PixelToTPIDTool::~PixelToTPIDTool() { }

StatusCode InDet::PixelToTPIDTool::initialize() {

  ATH_CHECK(AthAlgTool::initialize());

  ATH_CHECK(detStore()->retrieve(m_pixelid,"PixelID"));

  if (m_IBLParameterSvc.retrieve().isFailure()) { 
    ATH_MSG_FATAL("Could not retrieve IBLParameterSvc"); 
    return StatusCode::FAILURE; 
  } else  
    ATH_MSG_INFO("Retrieved service " << m_IBLParameterSvc); 

  ATH_CHECK(m_moduleDataKey.initialize());

  ATH_CHECK(m_dedxKey.initialize());

  ATH_MSG_INFO ("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::PixelToTPIDTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}


//============================================================================================

float InDet::PixelToTPIDTool::dEdx(const Trk::Track& track, int& nUsedHits, int& nUsedIBLOverflowHits) const {
 
  unsigned int pixelhits = 0;
  nUsedHits=0;
  nUsedIBLOverflowHits=0;
  float Pixel_sensorthickness=.025; //250 microns Pixel Planars
  float IBL_3D_sensorthickness=.023; //230 microns IBL 3D 
  float IBL_PLANAR_sensorthickness=.020;// 200 microns IBL Planars
  float dEdxValue=0;
  
  //std::multimap<float,int> chargesMap; //second value keeps track if the cluster is in IBL and has at least an overflow hit
  std::multimap<float,int> dEdxMap;
  
  // Check for track states:
  const DataVector<const Trk::TrackStateOnSurface>* recoTrackStates = track.trackStateOnSurfaces();
  if (recoTrackStates) {
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIter    = recoTrackStates->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsosIterEnd = recoTrackStates->end();

    // Loop over track states on surfaces (i.e. generalized hits):
    for (; tsosIter != tsosIterEnd; ++tsosIter) {
      const Trk::MeasurementBase *measurement = (*tsosIter)->measurementOnTrack();
      if (measurement && !(*tsosIter)->type(Trk::TrackStateOnSurface::Outlier)) {
        if (!(*tsosIter)->trackParameters()) {
          msg(MSG::WARNING) << "No track parameters available for a state of type measurement, returning -1" << endmsg;
          msg(MSG::WARNING) << "Don't run this tool on slimmed tracks!" << endmsg;
          return -1;
        }

        const InDet::PixelClusterOnTrack* pixclus = nullptr;
        if (measurement->type(Trk::MeasurementBaseType::RIO_OnTrack)) {
          const Trk::RIO_OnTrack* tmpRio = static_cast<const Trk::RIO_OnTrack*>(measurement);
          if (tmpRio->rioType(Trk::RIO_OnTrackType::PixelCluster)) {
            pixclus = static_cast<const InDet::PixelClusterOnTrack*>(tmpRio);
          }
        }
        if (pixclus) {
          //bool isok=false;
          double locx=pixclus->localParameters()[Trk::locX];
          double locy=pixclus->localParameters()[Trk::locY];
          int bec=m_pixelid->barrel_ec(pixclus->identify());
          int layer=m_pixelid->layer_disk(pixclus->identify());
          int eta_module=m_pixelid->eta_module(pixclus->identify());//check eta module to select thickness
   
          float dotProd = (*tsosIter)->trackParameters()->momentum().dot((*tsosIter)->trackParameters()->associatedSurface().normal());
          float cosalpha=fabs(dotProd/(*tsosIter)->trackParameters()->momentum().mag());
          
          if (std::abs(cosalpha)<0.16) { continue; }
   
          float charge=pixclus->prepRawData()->totalCharge()*cosalpha;
   
          //keep track if this is an ibl cluster with overflow
          int iblOverflow=0;
          if ((m_IBLParameterSvc->containsIBL()) and (bec==0) and (layer==0)) { // check if IBL 
   
            //loop over ToT and check if anyone is overflow (ToT==14) check for IBL cluster overflow
            int overflowIBLToT = SG::ReadCondHandle<PixelModuleData>(m_moduleDataKey)->getFEI4OverflowToT(0,0);
            const std::vector<int>& ToTs = pixclus->prepRawData()->totList();
   
            for (int pixToT : ToTs) {
              if (pixToT >= overflowIBLToT) {
                //overflow pixel hit -- flag cluster
                iblOverflow = 1;
                break; //no need to check other hits of this cluster
              }
            }// end

            //this is IBL layer -- @todo: check using proper service (safe against geometries)
            if (((eta_module>=-10 && eta_module<=-7)||(eta_module>=6 && eta_module<=9)) && (fabs(locy)<10. && (locx>-8.33 && locx<8.3))) { //check if IBL 3D and good cluster selection
              dEdxValue=charge*m_conversionfactor/IBL_3D_sensorthickness;
              dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
              pixelhits++;
              if (iblOverflow==1) { nUsedIBLOverflowHits++; }
            }
            else if((eta_module>=-6 && eta_module<=5) && (fabs(locy)<20. &&(locx>-8.33 && locx<8.3))) { //check if IBL planar and good cluster selection
              dEdxValue=charge*m_conversionfactor/IBL_PLANAR_sensorthickness; 
              dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
              pixelhits++;
              if (iblOverflow==1) { nUsedIBLOverflowHits++; }
            }
            else{
              dEdxValue=-1;
            } //end check which IBL Module
     
          }
          //PIXEL layer and ENDCAP
          else if(bec==0 && fabs(locy)<30. &&  ((locx>-8.20 && locx<-0.60) || (locx>0.50 && locx<8.10))) {
            dEdxValue=charge*m_conversionfactor/Pixel_sensorthickness;
            dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
            pixelhits++;
          }
          else if (std::abs(bec)==2 && fabs(locy)<30. && ((locx>-8.15 && locx<-0.55) || (locx>0.55 && locx<8.15))) {
            dEdxValue=charge*m_conversionfactor/Pixel_sensorthickness;
            dEdxMap.insert(std::pair<float,int>(dEdxValue, iblOverflow));
            pixelhits++;
          }
        } //pixclus iterator
      }
    }
  }

  //Now calculate dEdx, multimap is already sorted in ascending order
  float averagedEdx=0.;
  nUsedHits=0;
  int IBLOverflow=0;
  
  for (std::pair<float,int> itdEdx : dEdxMap) {
    if (itdEdx.second==0) {
      averagedEdx += itdEdx.first;
      nUsedHits++;
    }
    if (itdEdx.second>0) { IBLOverflow++; }

    //break, skipping last or the two last elements depending on total measurements
    if (((int)pixelhits>=5) and ((int)nUsedHits>=(int)pixelhits-2)) { break; }
    
    //break, IBL Overflow case pixelhits==3 and 4 
    if ((int)IBLOverflow>0 and ((int)pixelhits==3) and (int)nUsedHits==1) { break; }
    if ((int)IBLOverflow>0 and ((int)pixelhits==4) and (int)nUsedHits==2) { break; }
    
    if (((int)pixelhits > 1) and ((int)nUsedHits >=(int)pixelhits-1)) { break; }
    
    if ((int)IBLOverflow>0 and (int)pixelhits==1) { //only IBL in overflow
      averagedEdx=itdEdx.first;
      break;
    }
  }
  
  if (nUsedHits>0 or (nUsedHits==0 and(int)IBLOverflow>0 and (int)pixelhits==1)) {
    if (nUsedHits>0) { averagedEdx=averagedEdx/nUsedHits; }
    
    ATH_MSG_DEBUG("NEW dEdx = " << averagedEdx);
    ATH_MSG_DEBUG("Used hits: " << nUsedHits << ", IBL overflows: " << IBLOverflow );
    ATH_MSG_DEBUG("Original number of measurements = " << pixelhits << "( map size = " << dEdxMap.size() << ")");
    return averagedEdx;  
  }
  return -1;
}

std::vector<float> InDet::PixelToTPIDTool::getLikelihoods(double dedx2, double p2, int nGoodPixels) const {
  return SG::ReadCondHandle<PixeldEdxData>(m_dedxKey)->getLikelihoods(dedx2,p2,nGoodPixels);
}

float InDet::PixelToTPIDTool::getMass(double dedx, double p, int nGoodPixels) const {
  return SG::ReadCondHandle<PixeldEdxData>(m_dedxKey)->getMass(dedx,p/1000,nGoodPixels); 
}


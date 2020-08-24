/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////
// RobustTrackingGeometryBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// InDet
#include "InDetTrackingGeometry/RobustTrackingGeometryBuilder.h"
// EnvelopeDefinitionService
#include "SubDetectorEnvelopes/IEnvelopeDefSvc.h"
// Trk interfaces
#include "TrkDetDescrInterfaces/ILayerBuilder.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeCreator.h"
#include "TrkDetDescrInterfaces/ILayerArrayCreator.h"
// Trk Geometry stuff
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/Material.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkSurfaces/DiscBounds.h"
// Athena
#include "CxxUtils/checker_macros.h"
//Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/MsgStream.h"

// constructor
InDet::RobustTrackingGeometryBuilder::RobustTrackingGeometryBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  Trk::TrackingVolumeManipulator(),
  m_beamPipeBuilder("InDet::BeamPipeBuilder/AtlasBeamPipeBuilder"),
  m_trackingVolumeCreator("Trk::CylinderVolumeCreator/CylinderVolumeCreator"),  
  m_layerArrayCreator("Trk::LayerArrayCreator/LayerArrayCreator"),
  m_enclosingEnvelopeSvc("AtlasEnvelopeDefSvc", n),
  m_layerEnvelopeCover(2*Gaudi::Units::mm),
  m_buildBoundaryLayers(true),
  m_replaceJointBoundaries(true),
  m_outwardsFraction(0.75),
  m_indexStaticLayers(true),
  m_namespace("InDet::"),
  m_exitVolume("InDet::Containers::InnerDetector")
{
  declareInterface<Trk::IGeometryBuilder>(this);  
  // layer builders and their configurations
  declareProperty("BeamPipeBuilder",                  m_beamPipeBuilder);
  declareProperty("LayerBuilders",                    m_layerBuilders);
  declareProperty("LayerBinningType",                 m_layerBinningType);
  declareProperty("ColorCodes",                       m_colorCodesConfig);  
  // envelope definition service
  declareProperty("EnvelopeDefinitionSvc",            m_enclosingEnvelopeSvc );
  declareProperty("VolumeEnclosureCylinderRadii",     m_enclosingCylinderRadius);
  declareProperty("VolumeEnclosureDiscPositions",     m_enclosingDiscPositionZ);
  // helper tools  
  declareProperty("TrackingVolumeCreator",            m_trackingVolumeCreator);
  declareProperty("LayerArrayCreator",                m_layerArrayCreator);  
  // build the Boundary Layers
  declareProperty("EnvelopeCover",                    m_layerEnvelopeCover);
  declareProperty("BuildBoundaryLayers",              m_buildBoundaryLayers);
  declareProperty("ReplaceAllJointBoundaries",        m_replaceJointBoundaries);
  // outer envelope to
  declareProperty("OutwardsFraction",                 m_outwardsFraction); 
  // force robust layer indexing  
  declareProperty("IndexStaticLayers",                m_indexStaticLayers);
  // volume namespace & contaienr name
  declareProperty("VolumeNamespace",                  m_namespace); 
  declareProperty("ExitVolumeName",                   m_exitVolume);
  declareProperty("isSLHC",                           m_isSLHC=false);
}

// destructor
InDet::RobustTrackingGeometryBuilder::~RobustTrackingGeometryBuilder()
{
}

// Athena standard methods
// initialize
StatusCode InDet::RobustTrackingGeometryBuilder::initialize()
{
  
   // retrieve envelope definition service --------------------------------------------------
   if ( m_enclosingEnvelopeSvc.retrieve().isFailure() ){
     ATH_MSG_FATAL( "Could not retrieve EnvelopeDefinition service. Abort.");
     return StatusCode::FAILURE;
   }
  
   // Retrieve the beampipe builders --------------------------------------------------------  
   if (m_beamPipeBuilder.retrieve().isFailure())
   {
      ATH_MSG_FATAL( "Failed to retrieve tool(s) " << m_beamPipeBuilder );
      return StatusCode::FAILURE;
   } else
      ATH_MSG_DEBUG( "Retrieved tool " << m_beamPipeBuilder );
           
   // Retrieve the layer builders -----------------------------------------------------------
   if (m_layerBuilders.retrieve().isFailure())
   {
      ATH_MSG_FATAL( "Failed to retrieve tool(s) " << m_layerBuilders );
      return StatusCode::FAILURE;
   } else
      ATH_MSG_DEBUG( "Retrieved tool " << m_layerBuilders );

   // Retrieve the tracking volume creator  -------------------------------------------------
   if (m_trackingVolumeCreator.retrieve().isFailure())
   {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackingVolumeCreator );
      return StatusCode::FAILURE;
   } else
      ATH_MSG_DEBUG( "Retrieved tool " << m_trackingVolumeCreator );
      
   // Retrieve the layer array creator  ----------------------------------------------------
   if (m_layerArrayCreator.retrieve().isFailure())
   {
      ATH_MSG_FATAL( "Failed to retrieve tool " << m_layerArrayCreator );
      return StatusCode::FAILURE;
   } else
      ATH_MSG_INFO( "Retrieved tool " << m_layerArrayCreator );      

   // Dummy MaterialProerties
   m_materialProperties.set(std::make_unique<Trk::Material>());

   ATH_MSG_INFO( "initialize() succesful" );
    
   return StatusCode::SUCCESS;
}


const Trk::TrackingGeometry* InDet::RobustTrackingGeometryBuilder::trackingGeometry ATLAS_NOT_THREAD_SAFE // Thread unsafe TrackingGeometry::indexStaticLayers and TrackingVolume::registerColorCode methods are used.
(const Trk::TrackingVolume*) const
{
   // only one assumption: 
   // layer builders are ordered in increasing r
   
   ////////////////////////////////////////////////////////////////////////////////////////////////////////    
   // The Overall Geometry
   Trk::TrackingGeometry* trackingGeometry = 0;   

   // retrieve all the layers and sort them
   unsigned int numLayBuilders = m_layerBuilders.size();

   // central sector extend
   double centralExtendZ  = 0.;
   // endcap extends
   double endcapMaxExtend = 0.;
   double endcapMinExtend = 10e10;  
   // and the overallExtend(s)
   double overallExtendZ  = 0.;   
   double overallRmin     = 10e10;
   double overallRmax     = 0.;
      
   // cylinder layers at growing radii
   std::vector< std::vector< const Trk::Layer*> > providedCylinderLayers;
   providedCylinderLayers.reserve(numLayBuilders);
   std::vector< double > cylinderInnerRadii(numLayBuilders,10e10);
   std::vector< double > cylinderOuterRadii(numLayBuilders,0.);
   std::vector< double > cylinderExtendsInZ(numLayBuilders,0.);
   
   // negative disc layers and positive disc layers
   std::vector< std::vector< const Trk::Layer*> > providedDiscLayersNeg;   
   std::vector< std::vector< const Trk::Layer*> > providedDiscLayersPos;
   providedDiscLayersNeg.reserve(numLayBuilders);
   providedDiscLayersPos.reserve(numLayBuilders);   
   std::vector< double > discInnerRadii(numLayBuilders,10e10);
   std::vector< double > discOuterRadii(numLayBuilders,0.);
   std::vector< double > discMinZ(numLayBuilders,10e10);
   std::vector< double > discMaxZ(numLayBuilders,0.);
   
   // (I) PARSE THE LAYERS FOR OVERALL DIMENSIONS -------------------------------------------------------------
   ATH_MSG_DEBUG( "[ STEP 1 ] : Parse the provided layers for the dimensions." );
   // fill the layers into the vectors
   for ( unsigned int ilb = 0; ilb < m_layerBuilders.size(); ++ilb){
       
       Trk::BinningType binningType = (m_layerBinningType.size() == m_layerBuilders.size()) ? 
                                      (Trk::BinningType)m_layerBinningType[ilb] : Trk::arbitrary; 
       
       // retrieve the cylinder and disc layers
       ATH_MSG_DEBUG( "[ LayerBuilder : '" << m_layerBuilders[ilb]->identification() << "' ] being processed. " );
       // (a) cylinder           
       const std::vector<const Trk::CylinderLayer*>* cylinderLayers = m_layerBuilders[ilb]->cylindricalLayers();
       // (a)
       std::vector<const Trk::Layer*> cylinderVolumeLayers;     
       if (cylinderLayers && cylinderLayers->size()){
           // screen output
           ATH_MSG_DEBUG(  "          Processing CylinderLayers : " );         
           // the ones to be filled into the double-vector
           for (auto& cylIter : *cylinderLayers){
               // get the CylinderBounds
               const Trk::CylinderBounds& cylBounds = (cylIter)->surfaceRepresentation().bounds();               
               double currentR = cylBounds.r();
               // rmin/rmax with thicknes in mind
               double currentRmin = binningType!=Trk::biequidistant ? 
                   currentR - 0.5*(cylIter)->thickness()-m_layerEnvelopeCover : currentR + 0.5*(cylIter)->thickness()-m_layerEnvelopeCover;               
               double currentRmax = currentR + 0.5*(cylIter)->thickness() + m_layerEnvelopeCover;
               // safe  
               double extendZ  = cylIter->surfaceRepresentation().center().z() < 0. ?
                                 fabs(cylIter->surfaceRepresentation().center().z() - cylBounds.halflengthZ())-m_layerEnvelopeCover:
                                 cylIter->surfaceRepresentation().center().z() + cylBounds.halflengthZ() + m_layerEnvelopeCover;
               // note the dimension
               takeSmaller(cylinderInnerRadii[ilb],currentRmin); takeBigger(cylinderOuterRadii[ilb],currentRmax);
               takeBigger(cylinderExtendsInZ[ilb],extendZ);
               // push it into the vector
               cylinderVolumeLayers.push_back(cylIter); 
               // overall dimensions
               takeSmaller(overallRmin,currentRmin); takeBigger(overallRmax,currentRmax);               
               takeBigger(centralExtendZ, extendZ);
               // in case no disc layers are provided
               takeBigger(overallExtendZ,centralExtendZ);
           }
           // special treatment for bi-equidistant binning - a navigation layer will be added before and after the layers
           if (binningType==Trk::biequidistant){
               double rStep = (cylinderOuterRadii[ilb]-cylinderInnerRadii[ilb])/(cylinderLayers->size()-1);
               ATH_MSG_VERBOSE( "           -> bi-equidistant : rStep estimated as " << rStep);
               cylinderInnerRadii[ilb] -= rStep; cylinderOuterRadii[ilb] += rStep;
               takeBigger(overallRmax,cylinderOuterRadii[ilb]);
           }
           ATH_MSG_VERBOSE(  "           -> yield (rMin/rMax/halflengthZ) = " 
                 << cylinderInnerRadii[ilb] << " / " << cylinderOuterRadii[ilb] << " / " << cylinderExtendsInZ[ilb] ); 
             
       } else 
         ATH_MSG_DEBUG(  "          No cylindrical layers processed." );        
 
       providedCylinderLayers.push_back(cylinderVolumeLayers);       
       // if after parsing of cylinder layers the maximal extend of cylinders is bigger than the minmal of discs
       // reset the disc min extend (tiple will be swallowed)!
       endcapMinExtend =  ( centralExtendZ > endcapMinExtend) ? 10e10 : endcapMinExtend; 
       
       // (b) discs       
       const std::vector<const Trk::DiscLayer*>* discLayers = m_layerBuilders[ilb]->discLayers();       
       std::vector<const Trk::Layer*> discVolumeLayersNeg;
       std::vector<const Trk::Layer*> discVolumeLayersPos;                        
       if (discLayers && discLayers->size()){
         // screen output
         ATH_MSG_DEBUG(  "          Processing DiscLayers : " );            
         for (auto& discIter : *discLayers){
               // get the DiscBounds
               const Trk::DiscBounds* discBounds = dynamic_cast<const Trk::DiscBounds*>
                                                               (&(discIter->surfaceRepresentation().bounds()));               
               // get the current Z min/max
               double discZ = discIter->surfaceRepresentation().center().z();
               double currentZmin = binningType!=Trk::biequidistant ? (discZ-0.5*discIter->thickness()) : (discZ+0.5*discIter->thickness());
               double currentZmax = discZ+0.5*discIter->thickness();
               // force it to be symmetrized
               if (currentZmax > 0.){
                 takeSmaller(discMinZ[ilb],currentZmin); takeBigger(discMaxZ[ilb], currentZmax);
                 // push it into the vector
                 discVolumeLayersPos.push_back(discIter);                            
               } else {
                // in symmetrized setup only interested in total numbers
                 double negMinZ = fabs(currentZmax);
                 double negMaxZ = fabs(currentZmin);
                 takeSmaller(discMinZ[ilb],negMinZ); takeBigger(discMaxZ[ilb],negMaxZ);
                 // push it into the vector
                 discVolumeLayersNeg.push_back(discIter);                            
               }
               
               // radial dimensions               
               if (discBounds){
                 double currentRmin = discBounds->rMin();                 
                 double currentRmax = discBounds->rMax();                   
                  // note the dimension
                 takeSmaller(discInnerRadii[ilb], currentRmin); takeBigger(discOuterRadii[ilb],currentRmax);
                 // overall dimensions
                 takeSmaller(overallRmin, currentRmin); takeBigger(overallRmax, currentRmax);
               }
              // min/max extend of the discs
              takeSmaller(endcapMinExtend, discMinZ[ilb]); takeBigger(endcapMaxExtend, discMaxZ[ilb]);
              takeBigger(overallExtendZ, endcapMaxExtend);
         }       
         // special treatment for bi-equidistant binning - a navigation layer will be added before and after the layers
         if (binningType==Trk::biequidistant){
             double zStep = (discMaxZ[ilb]-discMinZ[ilb])/(discLayers->size()/2-1);
             discMinZ[ilb] -= zStep;
             discMaxZ[ilb] += zStep;
             takeBigger(overallExtendZ,discMaxZ[ilb]);
         }
         
         ATH_MSG_VERBOSE(  "           -> yield (rMin/rMax/zMin/zMax) = "
                  << discInnerRadii[ilb] << " / " << discOuterRadii[ilb] << " / " << discMinZ[ilb] << " / " << discMaxZ[ilb] ); 
         
       } else 
         ATH_MSG_DEBUG(  "          No disk-like layers processed." );   
 

       // fill the layers into the double-vector
       providedDiscLayersNeg.push_back(discVolumeLayersNeg);
       providedDiscLayersPos.push_back(discVolumeLayersPos);
                
       // memory cleanup
       delete discLayers;
       delete cylinderLayers;  
       
       if (msgLvl(MSG::VERBOSE)){
         // summary after this step
         ATH_MSG_VERBOSE( "[ Summary STEP 1  ---------------------------------------- ]  " );
         ATH_MSG_VERBOSE( "                      overall minimum r : " << overallRmin      );       
         ATH_MSG_VERBOSE( "                      overall maximum r : " << overallRmax      );       
         ATH_MSG_VERBOSE( "                       overall z extend : " << overallExtendZ   );       
         ATH_MSG_VERBOSE( "                central sector z extend : " << centralExtendZ   );
         ATH_MSG_VERBOSE( "                maximum endcap z extend : " << endcapMaxExtend  );
         ATH_MSG_VERBOSE( "                minimum endcap z extend : " << endcapMinExtend  );
       }
              
   }
  
    
   // (II) update the positions ------------------------------------------------------------
   ATH_MSG_DEBUG( "[ STEP 2 ] : Closing the gaps from the parsed original dimensions." );   
   // i.e. close the gap
   double oldCeZ   = centralExtendZ;
   centralExtendZ  = 0.5*(endcapMinExtend+oldCeZ);
   endcapMinExtend = centralExtendZ;
      
   ATH_MSG_DEBUG( "             central sector extend in z recalculated to be " << centralExtendZ << " (was " << oldCeZ << ")" );
 
   // (III) create the sub volumes -------------------------------------------------------------
   ATH_MSG_DEBUG( "[ STEP 3 ] : Create the sub volumes." );   
   
   std::vector<const Trk::TrackingVolume*> centralSectorVolumes;
   std::vector<const Trk::TrackingVolume*> negativeSectorVolumes;
   std::vector<const Trk::TrackingVolume*> positiveSectorVolumes;
   
   // --------------------------------------------------------------------------------------------------
   // first the beampipe volume
   Trk::TrackingVolume* beamPipeVolume = 0;
   // the Volume Bounds
   Trk::CylinderVolumeBounds* beamPipeBounds = new Trk::CylinderVolumeBounds(overallRmin,overallExtendZ); 
   // BinnedArray needed
   Trk::BinnedArray<Trk::Layer>* beamPipeLayerArray = 0;
   const std::vector<const Trk::CylinderLayer*>* beamPipeVec = m_beamPipeBuilder->cylindricalLayers();
   if (beamPipeVec->size())
       beamPipeLayerArray = m_layerArrayCreator->cylinderLayerArray(*beamPipeVec,0.,beamPipeBounds->outerRadius(),Trk::arbitrary);
   delete beamPipeVec;
   // create the TrackingVolume
   beamPipeVolume = new Trk::TrackingVolume(0,
                                            beamPipeBounds,
                                            *m_materialProperties,
                                            beamPipeLayerArray,
                                            0,
                                            "InDet::BeamPipe");
    // set the color code
    beamPipeVolume->registerColorCode(46);
   
   // beampipe + detectors / prepared                                                                   
   std::vector<const Trk::TrackingVolume*> idVolumes;
   idVolumes.push_back(beamPipeVolume);     
   
   // --------------------------------------------------------------------------------------------------   
   double lastCentralOuterR    = beamPipeBounds->outerRadius();
   double lastNegEndcapOuterR  = beamPipeBounds->outerRadius();
   double lastPosEndcapOuterR  = beamPipeBounds->outerRadius();
   
   // loop over double-vectors, access the dimensions from the parsing
   auto pclIter  = providedCylinderLayers.begin();
   auto pclEnd   = providedCylinderLayers.end();   
   auto pndlIter = providedDiscLayersNeg.begin();
   auto pndlEnd  = providedDiscLayersNeg.end();
   auto ppdlIter = providedDiscLayersPos.begin();
   auto ppdlEnd  = providedDiscLayersPos.end();
   
   // the number of layer builders
   unsigned int numLayerBuilders = m_layerBuilders.size();
   
   // loop over the prepared volumes
   for ( unsigned int ilb=0 ; pclIter != pclEnd && pndlIter != pndlEnd && ppdlIter != ppdlEnd; 
        ++pclIter, ++pndlIter, ++ppdlIter, ++ilb)
   {          
       // color code configuration (from jobOptions/declareProperty interface)
       int colorCode = (m_colorCodesConfig.size() == numLayerBuilders) ? m_colorCodesConfig[ilb] : 21;
       
       Trk::BinningType binningType = (m_layerBinningType.size() == numLayerBuilders) ? 
                                      (Trk::BinningType)m_layerBinningType[ilb] : Trk::arbitrary; 
       
       // volume name base
       std::string baseName = m_layerBuilders[ilb]->identification();
       ATH_MSG_DEBUG( "[ Volume Creation : '" << baseName << "'] carried out." ); 
       ATH_MSG_DEBUG( "       + color code                     : " << colorCode        );
                  
       // maxmmal z extend for volume set & endcap inner z
       double currentSetExtendZ    =  ( (*pndlIter).size() || (*ppdlIter).size() ) ? discMaxZ[ilb] : cylinderExtendsInZ[ilb];
       double currentEndcapInnerZ  =  ( (*pndlIter).size() || (*ppdlIter).size() ) ? discMinZ[ilb] : currentSetExtendZ;

       // radii for the sectors 
       double currentCentralOuterR  = 0.;
       double currentEndcapOuterR   = 0.;
       
       if (m_isSLHC){
	       double NextInnerRadii = ((ilb!=cylinderOuterRadii.size()-1) && cylinderInnerRadii[ilb+1]<discInnerRadii[ilb+1]) ? cylinderInnerRadii[ilb+1] : discInnerRadii[ilb+1]; 
	       currentCentralOuterR = (ilb!=cylinderOuterRadii.size()-1) ? 0.5*(NextInnerRadii+cylinderOuterRadii[ilb]) : overallRmax;
	       currentEndcapOuterR = (ilb!=discOuterRadii.size()-1) ? 0.5*(NextInnerRadii+discOuterRadii[ilb]) : overallRmax;
       } else{
           // we build from inside to outside, don't take middle position, but tend towards outer extend 
	       currentCentralOuterR = (ilb!=cylinderOuterRadii.size()-1) ? (m_outwardsFraction*cylinderInnerRadii[ilb+1]+(1.-m_outwardsFraction)*cylinderOuterRadii[ilb]) : overallRmax;
           currentEndcapOuterR = (ilb!=discOuterRadii.size()-1) ? (m_outwardsFraction*discInnerRadii[ilb+1]+(1.-m_outwardsFraction)*discOuterRadii[ilb]) : overallRmax;
	            
       }

       ATH_MSG_VERBOSE( "       + checking the sector boundaries : " );        
       ATH_MSG_VERBOSE( "         central sector ext. z (boundary)  = "  << currentSetExtendZ
           << " ("  << centralExtendZ << ")" );        
       ATH_MSG_VERBOSE( "       + current volume radii rMax (c/d)   = "  << currentCentralOuterR       
           << " / " << currentEndcapOuterR );
       // estimate the case
       // cases are :
       enum PackCase { contained, radial, central, split };  
       //   0 - both cylinders and disc are contained in the central sector
       //   1 - radial packing, volumes can't be put in in central / ec sectors
       //   2 - only central sector exists
       //   3 - standard ecn / central / ecp sectors split
       bool endcapsExist =  (*pndlIter).size()  && (*ppdlIter).size() ;
       // case flag set 
       PackCase caseFlag = (currentSetExtendZ < centralExtendZ && endcapsExist) ? contained : split;       
       if (currentSetExtendZ > centralExtendZ && currentEndcapInnerZ < centralExtendZ && endcapsExist) caseFlag = radial;
       if (!endcapsExist) caseFlag = central;
       
       // case 0 and 1 can be handled together : 
       if ( caseFlag == contained || caseFlag == radial ){
          // create the volume sizes for the compact volume
          double currentCentralExtendZ = 0.5*(cylinderExtendsInZ[ilb]+discMinZ[ilb]);
          double currentExtendZ = contained ? centralExtendZ : overallExtendZ;
          // in the radial wrapping case : take the smaller radius, assumes that packing is possible               
          double currentOuterR = currentCentralOuterR < currentEndcapOuterR ? currentCentralOuterR : currentEndcapOuterR;
          // create the tiple container
          const Trk::TrackingVolume* tripleContainer = packVolumeTriple((*pndlIter),
                                                                        (*pclIter),
                                                                        (*ppdlIter),
                                                                        lastCentralOuterR, currentOuterR,
                                                                        currentExtendZ,currentCentralExtendZ,
                                                                        baseName,
                                                                        colorCode,
                                                                        binningType);


          // cache the last central / ec outer radius       
          lastCentralOuterR = currentOuterR;
          if (contained){                     
              ATH_MSG_VERBOSE( "       + case I   : provided cylinder and disc layers are entirely contained in central sector." );                                                          
              // and push the centralSectorVolumes
              centralSectorVolumes.push_back(tripleContainer);
           } else {
              ATH_MSG_VERBOSE( "       + case II  : split into negative - central - positive sectors doesn't work, radial wrapping" ); 
              // cache the laster endcap radii  (need to be equal since this is radially wrapped)
              lastNegEndcapOuterR = currentOuterR;
              lastPosEndcapOuterR = currentOuterR;
              // the triple goes into the radial packing volumes
              idVolumes.push_back(tripleContainer);     
           }
       // case 2 and 3        
       } else {       
          // output for case 3 and 4          
          if (!endcapsExist)   
            ATH_MSG_VERBOSE( "       + case III : only central sector." );        
          else 
            ATH_MSG_VERBOSE( "       + case IV  : split into negative - central - positive sectors." ); 
          // build the name
          std::string volumeBase = m_namespace+"Detectors::"+baseName;          
          // cylinder layers exist -------------------------------
          if ((*pclIter).size()){
            
            ATH_MSG_VERBOSE( "       -> central sector is being build." ); 
            // create the cylinder barrel 
            const Trk::TrackingVolume* barrel    = 
                m_trackingVolumeCreator->createTrackingVolume((*pclIter),
                                                              *m_materialProperties,
                                                              lastCentralOuterR,currentCentralOuterR,
                                                              -centralExtendZ,centralExtendZ,
                                                              volumeBase+"::Barrel",
                                                              binningType);
            // register the color code
            barrel->registerColorCode(colorCode);                
            // cache the last ones
            lastCentralOuterR = currentCentralOuterR;
            // and push the centralSectorVolumes
            centralSectorVolumes.push_back(barrel);          
          } 

          // negative disc layers exist ------------------------------
          if ((*pndlIter).size()){
          
            ATH_MSG_VERBOSE( "       -> negative endcap is being build." );       
            // create the cylinder barrel 
            const Trk::TrackingVolume* negEndcap    = 
                m_trackingVolumeCreator->createTrackingVolume((*pndlIter),
                                                              *m_materialProperties,
                                                              lastNegEndcapOuterR,currentEndcapOuterR,
                                                              -overallExtendZ, -endcapMinExtend,
                                                              volumeBase+"::NegativeEndcap",
                                                              binningType);                
            // register the color code
            negEndcap->registerColorCode(colorCode);                
            // cache the last ones
            lastNegEndcapOuterR = currentEndcapOuterR;
            // and push the negativeSectorVolumes
            negativeSectorVolumes.push_back(negEndcap);                    
          }
          // positive disc layers exist --------------------------------
          if ((*ppdlIter).size()){
          
            ATH_MSG_VERBOSE( "       -> positive endcap is being build." );          
            // create the cylinder barrel 
            const Trk::TrackingVolume* posEndcap    = 
                m_trackingVolumeCreator->createTrackingVolume((*ppdlIter),
                                                              *m_materialProperties,
                                                              lastPosEndcapOuterR,currentEndcapOuterR,
                                                              endcapMinExtend,overallExtendZ,
                                                              volumeBase+"::PositiveEndcap",
                                                              binningType);                
            // register the color code
            posEndcap->registerColorCode(colorCode);                
            // cache the last ones
            lastPosEndcapOuterR = currentEndcapOuterR;
            // and push the positiveSectorVolumes
            positiveSectorVolumes.push_back(posEndcap);                    
          }                      
       }   
   }      
   
   // (IV) create the container ---------------------------------------------------------
   ATH_MSG_DEBUG( "[ STEP 4 ] : Create the container volume" );   
   
   bool enclose = (!m_enclosingEnvelopeSvc.empty());
 
   const Trk::TrackingVolume* detectorContainer = packVolumeTriple(negativeSectorVolumes,
                                                                   centralSectorVolumes,
                                                                   positiveSectorVolumes,
                                                                   "Container");
                                                                   
   // get the dimensions from the envelope service 
   RZPairVector& envelopeDefs = m_enclosingEnvelopeSvc->getInDetRZValues();
   ATH_MSG_VERBOSE("       -> retrieved Inner Detector envelope definitions at size " << envelopeDefs.size());
   double enclosingVolumeRadius = envelopeDefs[1].first;
   double enclosingVolumeHalfZ  = fabs(envelopeDefs[1].second);
                                                                   
   // central enclosure volume
   const Trk::TrackingVolume* centralEnclosure =  enclose ?
                           m_trackingVolumeCreator->createGapTrackingVolume(*m_materialProperties,
                                                                           overallRmax, enclosingVolumeRadius,
                                                                           -overallExtendZ, overallExtendZ,
                                                                           1, true,
                                                                           m_namespace+"Gaps::CentralEnclosure") : 0;
   
   // push the detector container into the ID volumes                                                              
   idVolumes.push_back(detectorContainer);
   if (centralEnclosure) idVolumes.push_back(centralEnclosure);
   
   std::string volumeName = enclose ? "InDet::Detectors::BpPixSctTrt" : m_exitVolume;

   ATH_MSG_VERBOSE("       -> inserting beam pipe into detectors." ); 

   const Trk::TrackingVolume* detectorWithBp = 
         m_trackingVolumeCreator->createContainerTrackingVolume(idVolumes,
                                                                *m_materialProperties,
                                                                volumeName,
                                                                m_buildBoundaryLayers,
                                                                m_replaceJointBoundaries);
  
   // if packing is needed ------------------------------------------------------------------
   const Trk::TrackingVolume* highestIdVolume = 0;
   if (enclose){
    
     // negative positions
     std::vector<double> negDiscPositionZ(m_enclosingDiscPositionZ);
     for (std::vector<double>::iterator posIter = negDiscPositionZ.begin();
         posIter != negDiscPositionZ.end(); ++posIter) (*posIter) *= -1;
        
     
     const Trk::TrackingVolume* negativeEnclosure =  negDiscPositionZ.size() ?
                           m_trackingVolumeCreator->createGapTrackingVolume(*m_materialProperties,
                                                                           0., enclosingVolumeRadius,
                                                                           -enclosingVolumeHalfZ, -overallExtendZ,
                                                                           negDiscPositionZ, false,
                                                                           m_namespace+"Gaps::NegativeEnclosure") :      
                           m_trackingVolumeCreator->createGapTrackingVolume(*m_materialProperties,
                                                                            0., enclosingVolumeRadius,
                                                                            -enclosingVolumeHalfZ, -overallExtendZ,
                                                                            1, false,
                                                                            m_namespace+"Gaps::NegativeEnclosure");
  
     const Trk::TrackingVolume* positiveEnclosure = m_enclosingDiscPositionZ.size() ? 
                           m_trackingVolumeCreator->createGapTrackingVolume(*m_materialProperties,
                                                                           0., enclosingVolumeRadius,
                                                                           overallExtendZ, enclosingVolumeHalfZ,
                                                                           m_enclosingDiscPositionZ, false,
                                                                           m_namespace+"Gaps::PositiveEnclosure") :
                           m_trackingVolumeCreator->createGapTrackingVolume(*m_materialProperties,
                                                                            0., enclosingVolumeRadius,
                                                                            overallExtendZ,enclosingVolumeHalfZ,
                                                                            1, false,
                                                                            m_namespace+"Gaps::PositiveEnclosure");
  
      std::vector<const Trk::TrackingVolume*> enclosedVolumes;
        enclosedVolumes.push_back(negativeEnclosure);
        enclosedVolumes.push_back(detectorWithBp);
        enclosedVolumes.push_back(positiveEnclosure);
   
     const Trk::TrackingVolume* enclosedDetector = 
         m_trackingVolumeCreator->createContainerTrackingVolume(enclosedVolumes,
                                                                *m_materialProperties,
                                                                 m_exitVolume,
                                                                 m_buildBoundaryLayers,
                                                                 m_replaceJointBoundaries);
                                                                          
      highestIdVolume = enclosedDetector;                                                                       
    } else  
      highestIdVolume = detectorWithBp;
      
  // (V) create the TrackingGeometry ------------------------------------------------------  
  trackingGeometry = new Trk::TrackingGeometry(highestIdVolume);
 
 if (m_indexStaticLayers) {
      ATH_MSG_VERBOSE("Re-index the static layers ...");
      // ST temporary hack till layer numbering resolved
      //trackingGeometry->indexStaticLayers(geometrySignature());   
      trackingGeometry->indexStaticLayers(Trk::Global);   
   }                       
                                                                     
   return trackingGeometry;
}

// finalize
StatusCode InDet::RobustTrackingGeometryBuilder::finalize()
{
    ATH_MSG_INFO( "finalize() successful" );
    return StatusCode::SUCCESS;
}


const Trk::TrackingVolume* InDet::RobustTrackingGeometryBuilder::packVolumeTriple ATLAS_NOT_THREAD_SAFE ( // Thread unsafe TrackingVolume::registerColorCode method is use.
                                     const std::vector<const Trk::Layer*>& negLayers,
                                     const std::vector<const Trk::Layer*>& centralLayers,
                                     const std::vector<const Trk::Layer*>& posLayers,
                                     double rMin, double rMax,
                                     double zMax, double zPosCentral,
                                     const std::string& baseName,
                                     int colorCode,
                                     Trk::BinningType bintyp) const
{


  ATH_MSG_VERBOSE( '\t' << '\t'<< "Pack provided Layers from '" << baseName << "' triple into a container volume. " );
  
  // create the strings
  std::string volumeBase = m_namespace+"Detectors::"+baseName;
  
  const Trk::TrackingVolume* negativeVolume = 
       m_trackingVolumeCreator->createTrackingVolume(negLayers,
                                                     *m_materialProperties,
                                                     rMin,rMax,
                                                     -zMax,-zPosCentral,
                                                     volumeBase+"::NegativeEndcap",
                                                     bintyp);
                                                                
  const Trk::TrackingVolume* centralVolume = 
         m_trackingVolumeCreator->createTrackingVolume(centralLayers,
                                                       *m_materialProperties,
                                                       rMin,rMax,
                                                       -zPosCentral,zPosCentral,
                                                       volumeBase+"::Barrel",
                                                       bintyp);
                                                       
   const Trk::TrackingVolume* positiveVolume = 
         m_trackingVolumeCreator->createTrackingVolume(posLayers,
                                                       *m_materialProperties,
                                                       rMin,rMax,
                                                       zPosCentral,zMax,
                                                       volumeBase+"::PositiveEndcap",
                                                       bintyp);
   
   // the base volumes have been created
   ATH_MSG_VERBOSE('\t' << '\t'<< "Volumes have been created, now pack them into a triple.");
   // registerColorCode                                                   
   negativeVolume->registerColorCode(colorCode);   
   centralVolume->registerColorCode(colorCode);
   positiveVolume->registerColorCode(colorCode);
                                                         
   // pack them together
   std::vector<const Trk::TrackingVolume*> tripleVolumes;
   tripleVolumes.push_back(negativeVolume);
   tripleVolumes.push_back(centralVolume);
   tripleVolumes.push_back(positiveVolume);
   
   // create the tiple container
   const Trk::TrackingVolume* tripleContainer = 
         m_trackingVolumeCreator->createContainerTrackingVolume(tripleVolumes,
                                                                *m_materialProperties,
                                                                volumeBase,
                                                                m_buildBoundaryLayers,
                                                                m_replaceJointBoundaries);
                                                                
   ATH_MSG_VERBOSE( '\t' << '\t'<< "Created container volume with bounds: " << tripleContainer->volumeBounds() );
                                                                
   return tripleContainer;
}

const Trk::TrackingVolume* InDet::RobustTrackingGeometryBuilder::packVolumeTriple(
                                     const std::vector<const Trk::TrackingVolume*>& negVolumes,
                                     const std::vector<const Trk::TrackingVolume*>& centralVolumes,
                                     const std::vector<const Trk::TrackingVolume*>& posVolumes,
                                     const std::string& baseName) const
{
  ATH_MSG_VERBOSE( '\t' << '\t'<< "Pack provided Volumes from '" << baseName << "' triple into a container volume. " );

  unsigned int negVolSize = negVolumes.size();
  unsigned int cenVolSize = centralVolumes.size();
  unsigned int posVolSize = posVolumes.size();
  
  
  
    // create the strings
  std::string volumeBase = m_namespace+"Containers::"+baseName;
  
  const Trk::TrackingVolume* negativeVolume = (negVolSize > 1) ?
       m_trackingVolumeCreator->createContainerTrackingVolume(negVolumes,
                                                       *m_materialProperties,
                                                       volumeBase+"::NegativeSector",
                                                       m_buildBoundaryLayers,
                                                       m_replaceJointBoundaries) : 
                                             (negVolSize ? negVolumes[0] : 0);
  const Trk::TrackingVolume* centralVolume = (cenVolSize > 1) ?
         m_trackingVolumeCreator->createContainerTrackingVolume(centralVolumes,
                                                       *m_materialProperties,
                                                       volumeBase+"::CentralSector",
                                                       m_buildBoundaryLayers,
                                                       m_replaceJointBoundaries) :
                                              (cenVolSize ? centralVolumes[0] : 0) ;
                                              
   const Trk::TrackingVolume* positiveVolume = ( posVolSize > 1) ?
         m_trackingVolumeCreator->createContainerTrackingVolume(posVolumes,
                                                       *m_materialProperties,
                                                       volumeBase+"::PositiveSector",
                                                       m_buildBoundaryLayers,
                                                       m_replaceJointBoundaries) : 
                                               (posVolSize ? posVolumes[0] : 0);
   
   if (!negativeVolume && !positiveVolume){
       ATH_MSG_DEBUG( "No negative/positive sector given - no packing needed, returning central container!" );
       return centralVolume;
   }
   // pack them together
   std::vector<const Trk::TrackingVolume*> tripleVolumes;
   if (negativeVolume) tripleVolumes.push_back(negativeVolume);
   if (centralVolume) tripleVolumes.push_back(centralVolume);
   if (positiveVolume) tripleVolumes.push_back(positiveVolume);
   // create the tiple container
   const Trk::TrackingVolume* tripleContainer = 
         m_trackingVolumeCreator->createContainerTrackingVolume(tripleVolumes,
                                                                *m_materialProperties,
                                                                volumeBase,
                                                                m_buildBoundaryLayers,
                                                                m_replaceJointBoundaries);
   return tripleContainer;
}

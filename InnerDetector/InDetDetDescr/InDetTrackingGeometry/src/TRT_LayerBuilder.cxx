/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_LayerBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "InDetTrackingGeometry/TRT_LayerBuilder.h"
#include "InDetTrackingGeometry/TRT_OverlapDescriptor.h"
//Trk
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/PlaneLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/BinnedArray2D.h"
#include "TrkDetDescrUtils/BinnedArray1D.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinUtility.h"
// InDetDD
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_Numerology.h"
#include "InDetIdentifier/TRT_ID.h"
// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SystemOfUnits.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"
// STL
#include <map>

// constructor
InDet::TRT_LayerBuilder::TRT_LayerBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_trtMgr(0),
  m_trtMgrLocation("TRT"),
  m_layerThickness(0.1*Gaudi::Units::mm),
  m_modelGeometry(true),
  m_modelBarrelLayers(7),
  m_modelEndcapLayers(14),
  m_barrelLayerBinsZ(25),
  m_barrelLayerBinsPhi(1),
  m_endcapLayerBinsR(25),
  m_endcapLayerBinsPhi(1),
  m_endcapConly(false),
  m_registerStraws(false),
  m_identification("TRT")
{
  declareInterface<Trk::ILayerBuilder>(this);
  // properties from outside
  declareProperty("TRT_DetManagerLocation",       m_trtMgrLocation);
  declareProperty("LayerThickness",               m_layerThickness);
  // material binning
  declareProperty("BarrelLayerBinsZ"            , m_barrelLayerBinsZ);
  declareProperty("BarrelLayerBinsPhi"          , m_barrelLayerBinsPhi);
  declareProperty("EndcapLayerBinsR"            , m_endcapLayerBinsR);
  declareProperty("EndcapLayerBinsPhi"          , m_endcapLayerBinsPhi);
  // steering
  declareProperty("ModelLayersOnly",              m_modelGeometry);
  declareProperty("ModelBarrelLayers",            m_modelBarrelLayers);
  declareProperty("ModelEndcapLayers",            m_modelEndcapLayers);
  declareProperty("EndcapConly",                  m_endcapConly);
  declareProperty("RegisterStraws",               m_registerStraws);
  // identification
  declareProperty("Identification"              , m_identification);

}

// destructor
InDet::TRT_LayerBuilder::~TRT_LayerBuilder()
{}

// Athena standard methods
// initialize
StatusCode InDet::TRT_LayerBuilder::initialize()
{
   ATH_MSG_INFO( "initialize()" );
   // get TRT Detector Description Manager
   if ((detStore()->retrieve(m_trtMgr, m_trtMgrLocation)).isFailure())
     ATH_MSG_ERROR( "Could not get TRT_DetectorManager, no layers for TRT Detector will be built. " );

   return StatusCode::SUCCESS;
}

// finalize
StatusCode InDet::TRT_LayerBuilder::finalize()
{
    ATH_MSG_INFO( "finalize() successful" );

    return StatusCode::SUCCESS;
}


/** LayerBuilder interface method - returning Barrel-like layers */
const std::vector< const Trk::CylinderLayer* >* InDet::TRT_LayerBuilder::cylindricalLayers() const
{

  if (!m_trtMgr) return 0;

  ATH_MSG_DEBUG( "building cylindrical layers for the TRT " );

  std::vector< const Trk::CylinderLayer* >* barrelLayers = new std::vector< const Trk::CylinderLayer* >;

  // get Numerology and Id HElper
  const InDetDD::TRT_Numerology* trtNums = m_trtMgr->getNumerology();

  // get the TRT ID Helper
  const TRT_ID* trtIdHelper = 0;
  if (detStore()->retrieve(trtIdHelper, "TRT_ID").isFailure()) {
     ATH_MSG_ERROR("Could not get TRT ID helper");
     delete barrelLayers;
     return 0;
  }

  int nBarrelRings  = trtNums->getNBarrelRings();
  int nBarrelPhiSectors = trtNums->getNBarrelPhi();
  int nTotalBarrelLayers = 0;

  // register total number of straws per layer (to be corrected by sector borders and cooling straws)
  std::vector<int> strawsPerLayer;
  double rMin = 10e10;
  double rMax = 0.;

  double layerZmax = 0.;
  double layerZmin = 10e10;

  // pre-loop for overall layer numbers ---------------------------------------
  for (int ring=0; ring < nBarrelRings; ring++)
    {
     int nBarrelLayers = trtNums->getNBarrelLayers(ring);
     nTotalBarrelLayers += nBarrelLayers;
     // loop over layers
      for (int layer=0; layer < nBarrelLayers; layer++){
       int strawsOnLayer = 0;
       for (int phisec=0; phisec<nBarrelPhiSectors; ++phisec)
       {
         int straws = 0;
         for (int iposneg=0; iposneg<2; ++iposneg){
           // get the element
           const InDetDD::TRT_BarrelElement* trtbar = m_trtMgr->getBarrelElement(iposneg, ring, phisec, layer);

          // get overall dimensions only one time
          const Trk::PlaneSurface*    elementSurface = dynamic_cast<const Trk::PlaneSurface*>(&(trtbar->surface()));
          if (!elementSurface) {
              ATH_MSG_WARNING( "elementSurface: dynamic_cast to Trk::PlaneSurface failed - skipping ... ring/layer/phisec/iposneg = " << ring << "/" << layer << "/" << phisec << "/" << iposneg );
              continue;
          }
          const Trk::RectangleBounds* elementBounds  = dynamic_cast<const Trk::RectangleBounds*>(&(trtbar->bounds()));
          if (!elementBounds) {
              ATH_MSG_WARNING( "elementBounds: dynamic_cast to Trk::RectangleBounds failed - skipping ... ring/layer/phisec/iposneg = " << ring << "/" << layer << "/" << phisec << "/" << iposneg );
              continue;
          }
          double elementZcenter = (elementSurface->center()).z();
          double elementZmin    = fabs(elementZcenter - elementBounds->halflengthY());
          double elementZmax    = fabs(elementZcenter + elementBounds->halflengthY());

         layerZmax = (elementZmax > layerZmax ) ? elementZmax : layerZmax;
         layerZmin = (elementZmin < layerZmin ) ? elementZmin : layerZmin;

          // get the radial dimensions
          double currentR = trtbar->center().perp();
          rMin = currentR < rMin ? currentR : rMin;
          rMax = currentR > rMax ? currentR : rMax;
          //
          straws = trtbar->nStraws();
         }
         // correct for the sector borders
         strawsOnLayer += 1;
         strawsOnLayer += straws;
         // correct for cooling straws (0,18,19,42,43,72)
         strawsOnLayer += (layer==0) ? 1 : 0;
         strawsOnLayer += (ring==0 && layer==18) ? 1 : 0;
         strawsOnLayer += (ring==1 && layer==23) ? 1 : 0;
         strawsOnLayer += (ring==2 && layer==29) ? 1 : 0;
       }
       // empirical
       strawsPerLayer.push_back(strawsOnLayer);
    }
  }
  if (nTotalBarrelLayers==0) {
      ATH_MSG_WARNING( "nTotalBarrelLayers = 0 ... aborting and returning 0 !" );
      return 0;
  }
  // calculate delta(R) steps and delta(R)
  double rDiff = fabs(rMax-rMin);
  double rStep = rDiff/(m_modelBarrelLayers-1);
  double averageStep = rDiff/nTotalBarrelLayers;
  double layerHalflength = layerZmax;

  // prepare the material
  if ( fabs(rDiff) <= 0.1 ) return 0;

  // ilay - for accessing the straw layers and for material decission
  int ilay = 0;

  //  fix the positions where the layers are
  std::vector<double> layerRadii;
  layerRadii.reserve(m_modelBarrelLayers);
  for (unsigned int ilay = 0; ilay < m_modelBarrelLayers; ++ilay)
       layerRadii.push_back(rMin + ilay*rStep);

  std::vector<double>::const_iterator layerRadiusIter = layerRadii.begin();
  std::vector<double>::const_iterator layerRadiusIterEnd = layerRadii.end();

  // (A) model geometry section
  if (m_modelGeometry && !m_registerStraws){

     // create the layers
     for ( ; layerRadiusIter != layerRadiusIterEnd; ++layerRadiusIter ) {
       // ----- prepare the BinnedLayerMaterial -----------------------------------------------------
       Trk::BinnedLayerMaterial* layerMaterial = 0;
       // -- material with 1D binning
       Trk::BinUtility layerBinUtility1DZ(m_barrelLayerBinsZ,-layerHalflength, layerHalflength, Trk::open, Trk::binZ);
       if (m_barrelLayerBinsPhi==1){
         // no binning in phi  
         layerMaterial =new Trk::BinnedLayerMaterial(layerBinUtility1DZ);
       } else { // -- material with 2D binning : Rphi*Z optimized for cylinder layer
         Trk::BinUtility layerBinUtility2DRPhiZ(m_barrelLayerBinsPhi,
                                                -(*layerRadiusIter)*M_PI,
                                                (*layerRadiusIter)*M_PI,
                                                Trk::closed,
                                                Trk::binRPhi);
         layerBinUtility2DRPhiZ += layerBinUtility1DZ;                                         
         layerMaterial =new Trk::BinnedLayerMaterial(layerBinUtility2DRPhiZ);
       }
       // Barrel layers are centered around (0,0,0) by definition
       barrelLayers->push_back(new Trk::CylinderLayer(new Trk::CylinderBounds(*layerRadiusIter,layerHalflength),
                                                      *layerMaterial,
                                                      m_layerThickness));
       ATH_MSG_VERBOSE( "creating a layer at radius : " << *layerRadiusIter );
       delete layerMaterial;
     }
  } else {
    // (B) complex geometry section
    // loop over rings
    for (int ring=0; ring < nBarrelRings; ring++){
       int nBarrelLayers = trtNums->getNBarrelLayers(ring);

         // loop over layers
         for (int layer=0; layer < nBarrelLayers; layer++){

          // set layer dimensions radius
          double layerRadius      = 0.;

          // order the straws onto layers
          std::vector< Trk::SurfaceOrderPosition > strawPerBarrelLayer;

          // loop over phi sectors
          for (int phisec=0; phisec < nBarrelPhiSectors; phisec++){

             for (int posneg=0; posneg<2; ++posneg){

              // sort the elements
              const InDetDD::TRT_BarrelElement* currentElement = m_trtMgr->getBarrelElement(posneg, ring, phisec, layer);
              // get overall dimensions only one time
              const Trk::PlaneSurface*    elementSurface = dynamic_cast<const Trk::PlaneSurface*>(&(currentElement->surface()));
              if (!elementSurface) {
                  ATH_MSG_WARNING( "elementSurface: dynamic_cast to Trk::PlaneSurface failed - skipping ... ring/layer/phisec/posneg = " << ring << "/" << layer << "/" << phisec << "/" << posneg );
                  continue;
              }
              // layer Radius
              layerRadius += (elementSurface->center()).perp();

              // don't do that for the model layer layers
              int nstraws = currentElement->nStraws();
              // loop over straws and fill them
              for (int istraw=0; istraw<nstraws; ++istraw)
              {
                Identifier strawId = trtIdHelper->straw_id(currentElement->identify(), istraw);
                const Trk::Surface* currentStraw = &(currentElement->surface(strawId));
                Amg::Vector3D strawOrderPos(currentStraw->center());
                Trk::SharedObject<const Trk::Surface> sharedSurface(currentStraw, true);
                strawPerBarrelLayer.push_back(Trk::SurfaceOrderPosition(sharedSurface, strawOrderPos));
              } // loop over straws done
             }  // loop over posneg done
          } // loop over PhiSectors done

          // build the mean of the layer Radius
          layerRadius /= double(nBarrelPhiSectors*2);
          bool assignMaterial = false;
          // correct the layerRadius and create a material layer
          if (layerRadiusIter != layerRadiusIterEnd && fabs(layerRadius - (*layerRadiusIter)) < 0.6*averageStep){
             // set it to the new radius
             layerRadius = (*layerRadiusIter);
             ATH_MSG_VERBOSE( "material   a layer at radius  : " << layerRadius );
             // increase the iterator
             ++layerRadiusIter;
             // set the assignMaterial flag
             assignMaterial = true;
          }
          ATH_MSG_VERBOSE( "creating a layer at radius : " << layerRadius );


          // now order the plane layers to sit on cylindrical layers
          Trk::CylinderBounds* barrelLayerBounds = new Trk::CylinderBounds(layerRadius, layerHalflength);

          // do not build the straw array for the model geometry -------------------------------------------------------------------
          Trk::BinUtility* strawsBinUtility = new Trk::BinUtility(strawsPerLayer[ilay],-layerRadius*M_PI,layerRadius*M_PI,Trk::closed, Trk::binRPhi);
                                 (*strawsBinUtility) += Trk::BinUtility(2.,-layerHalflength,layerHalflength,Trk::open,Trk::binZ);

          Trk::BinnedArray2D<Trk::Surface>* strawArray = new Trk::BinnedArray2D<Trk::Surface>(strawPerBarrelLayer, strawsBinUtility );

         // do not give every layer material properties
         if (assignMaterial) {
            // ----- prepare the BinnedLayerMaterial -----------------------------------------------------
            Trk::BinnedLayerMaterial* layerMaterial = 0;
            // -- material with 1D binning
            Trk::BinUtility layerBinUtilityZ(m_barrelLayerBinsZ, -layerHalflength, layerHalflength, Trk::open, Trk::binZ );
            if (m_barrelLayerBinsPhi==1){
               layerMaterial =new Trk::BinnedLayerMaterial(layerBinUtilityZ);
            } else { // -- material with 2D binning: RPhiZ binning
               Trk::BinUtility layerBinUtilityRPhiZ(m_barrelLayerBinsPhi,
                                                          -layerRadius*M_PI, layerRadius*M_PI,
                                                           Trk::closed,
                                                           Trk::binRPhi);
                                      layerBinUtilityRPhiZ += layerBinUtilityZ;                       
              layerMaterial =new Trk::BinnedLayerMaterial(layerBinUtilityRPhiZ);
            }

             barrelLayers->push_back(new Trk::CylinderLayer(barrelLayerBounds,
                                                            strawArray,
                                                            *layerMaterial,
                                                            m_layerThickness,
                                                            new InDet::TRT_OverlapDescriptor(trtIdHelper)));
              delete layerMaterial;

         } else
            barrelLayers->push_back(new Trk::CylinderLayer(barrelLayerBounds,
                                                           strawArray,
                                                           m_layerThickness,
                                                           new InDet::TRT_OverlapDescriptor(trtIdHelper)));
                 ++ilay;
        } // loop over layers
     } // loop over rings
   }

  // return what you have
  return barrelLayers;
}

const std::vector< const Trk::DiscLayer* >* InDet::TRT_LayerBuilder::discLayers() const
{


  ATH_MSG_DEBUG( "building disc-like layers for the TRT " );

  std::vector< const Trk::DiscLayer* >* endcapLayers = new std::vector<const Trk::DiscLayer*>;

  const InDetDD::TRT_Numerology* trtNums = m_trtMgr->getNumerology();
  // get the TRT ID Helper
  const TRT_ID* trtIdHelper = 0;
  if (detStore()->retrieve(trtIdHelper, "TRT_ID").isFailure()) {
     ATH_MSG_ERROR("Could not get TRT ID helper");
     delete endcapLayers;
     return 0;
  }
  unsigned int nEndcapWheels = trtNums->getNEndcapWheels();
  unsigned int nEndcapPhiSectors = trtNums->getNEndcapPhi();

  // total layer numbers
  int numTotalLayers = 0;

  // zMin / zMax
  double zMin = 10e10;
  double zMax = 0.;

  const Trk::DiscBounds* sectorDiscBounds = 0;

  // preloop for overall numbers
  for (unsigned int iwheel=0; iwheel<nEndcapWheels; ++iwheel)
    {
       unsigned int nEndcapLayers = trtNums->getNEndcapLayers(iwheel);
       numTotalLayers += nEndcapLayers;
       for (unsigned int ilayer = 0; ilayer<nEndcapLayers; ++ilayer){
         const InDetDD::TRT_EndcapElement* sectorDiscElement = m_trtMgr->getEndcapElement(0, iwheel, ilayer, 0);

         // get a reference element for dimensions
         if (!sectorDiscBounds){
           const Trk::SurfaceBounds& sectorSurfaceBounds = sectorDiscElement->bounds();
           sectorDiscBounds = dynamic_cast<const Trk::DiscBounds*>(&sectorSurfaceBounds);
         }

         double currentZ = fabs(sectorDiscElement->center().z());
         zMin = currentZ < zMin ? currentZ : zMin;
         zMax = currentZ > zMax ? currentZ : zMax;
       }
    }
  if (numTotalLayers==0) {
      ATH_MSG_WARNING( "numTotalLayers = 0 ... aborting and returning 0 !" );
      return 0;
  }

  Trk::DiscBounds* fullDiscBounds = sectorDiscBounds ? new Trk::DiscBounds(sectorDiscBounds->rMin(), sectorDiscBounds->rMax()) : 0;
  if (!fullDiscBounds) {
      ATH_MSG_WARNING( "fullDiscBounds do not exist ... aborting and returning 0 !" );
      return 0;
  }
  // the BinUtility for the material
  Trk::BinnedLayerMaterial* layerMaterial = 0;
  // -- material with 1D binning
  Trk::BinUtility layerBinUtilityR(m_endcapLayerBinsR,
                                         fullDiscBounds->rMin(),
                                         fullDiscBounds->rMax(),
                                         Trk::open,
                                         Trk::binR);
  if (m_barrelLayerBinsPhi==1)
     layerMaterial = new Trk::BinnedLayerMaterial(layerBinUtilityR);
   else { // -- material with 2D binning
     Trk::BinUtility layerBinUtilityPhi(m_barrelLayerBinsPhi,
                                               -M_PI, M_PI,
                                               Trk::closed,
                                               Trk::binPhi);
                            // make it rPhi now
                       layerBinUtilityR += layerBinUtilityPhi;                   
     layerMaterial =new Trk::BinnedLayerMaterial(layerBinUtilityR);
  }

  // global geometry statistics
  double zDiff = fabs(zMax-zMin);
  double zStep = zDiff/(m_modelEndcapLayers-1);
  double zAverage = zDiff/numTotalLayers;

  // loop for surface ordering
  int maxendcaps=2;
  if (m_endcapConly) maxendcaps=1;
  for (int iposneg=0; iposneg<maxendcaps; ++iposneg){

    // fill the positions of the disc layers
    std::vector<double> zPositions;
    zPositions.reserve(m_modelEndcapLayers);

    double stepdir = iposneg ? 1. : -1.;
    double zStart = stepdir*zMin;

    // take a different modelling for
    for (unsigned int izpos = 0; izpos < m_modelEndcapLayers; ++izpos){
       ATH_MSG_VERBOSE( "registering layer at z pos   : " << zStart + stepdir * double(izpos) * zStep );
       zPositions.push_back(zStart + stepdir * double(izpos) * zStep);
   }

    std::vector<double>::const_iterator zPosIter    = zPositions.begin();
    std::vector<double>::const_iterator zPosIterEnd = zPositions.end();

    if (m_modelGeometry){
       // build the layers actually
       for ( ; zPosIter != zPosIterEnd; ++zPosIter){
          ATH_MSG_VERBOSE( "creating a layer at z pos    : " << (*zPosIter) );
          Amg::Transform3D* zPosTrans = new Amg::Transform3D;
          (*zPosTrans) = Amg::Translation3D(0.,0.,(*zPosIter));
          endcapLayers->push_back(new Trk::DiscLayer(zPosTrans,
                                                     fullDiscBounds->clone(),
                                                     *layerMaterial,
                                                     m_layerThickness));
         }

    } else {

      int currentLayerCounter = 0;
      for (unsigned int iwheel=0; iwheel<nEndcapWheels; ++iwheel)
      {
        // do the loop per side
        unsigned int nEndcapLayers = trtNums->getNEndcapLayers(iwheel);
        for (unsigned int ilayer = 0; ilayer<nEndcapLayers; ++ilayer){
         // increase the layerCounter for material layer decission
         ++currentLayerCounter;
          // count the straws;
         int numberOfStraws = 0;

         // check if dynamic cast worked
         if (fullDiscBounds){
          // get a reference element for dimensions
           const InDetDD::TRT_EndcapElement* sectorDiscElement = m_trtMgr->getEndcapElement(iposneg, iwheel, ilayer, 0);

           // take the position, but not the rotation (the rotation has to be standard)
           Amg::Vector3D fullDiscPosition(sectorDiscElement->surface().transform().translation());

           double discZ = fullDiscPosition.z();
           // check whether material needs to be assigned or not
           bool assignMaterial(fabs((*zPosIter)-fullDiscPosition.z()) < 0.95*zAverage);
           if (assignMaterial){
               // assign the new z value
               discZ = (*zPosIter);
               ATH_MSG_VERBOSE( "material layer   at z pos    : " << discZ );
               // increase the iterator
               ++zPosIter;
           } else
               ATH_MSG_VERBOSE( "creating a layer at z pos    : " << discZ << " next material layer at/diff = "
			             <<  (*zPosIter) << " / " << fabs((*zPosIter)-fullDiscPosition.z()));

           Amg::Transform3D* fullDiscTransform = new Amg::Transform3D;
           (*fullDiscTransform) = Amg::Translation3D(0.,0.,discZ);

           // order the straws onto layers
           std::vector< Trk::SurfaceOrderPosition > strawPerEndcapLayer;

           for (unsigned int iphisec=0; iphisec<nEndcapPhiSectors; ++iphisec){
                const InDetDD::TRT_EndcapElement* currentElement = m_trtMgr->getEndcapElement(iposneg, iwheel, ilayer, iphisec);
                unsigned int nstraws = currentElement->nStraws();
                for (unsigned int istraw=0; istraw<nstraws; istraw++){
                    Identifier strawId = trtIdHelper->straw_id(currentElement->identify(), istraw);
                    const Trk::Surface* currentStraw = &(currentElement->surface(strawId));
                    Amg::Vector3D strawOrderPos(currentStraw->center());
                    Trk::SharedObject<const Trk::Surface> sharedSurface(currentStraw, true);
                    strawPerEndcapLayer.push_back(Trk::SurfaceOrderPosition(sharedSurface, strawOrderPos));
                    ++numberOfStraws;
                }
           }

           Trk::BinUtility* currentBinUtility = new Trk::BinUtility(numberOfStraws, -M_PI, M_PI, Trk::closed, Trk::binPhi);
           Trk::BinnedArray<Trk::Surface>*  strawArray = new Trk::BinnedArray1D<Trk::Surface>(strawPerEndcapLayer, currentBinUtility);
           Trk::DiscLayer* currentLayer = 0;

           // do not give every layer material properties
           if (assignMaterial)
              currentLayer = new Trk::DiscLayer(fullDiscTransform,
                                                fullDiscBounds->clone(),
                                                strawArray,
                                                *layerMaterial,
                                                m_layerThickness,
                                                new InDet::TRT_OverlapDescriptor(trtIdHelper));
           else if (!m_modelGeometry)
              currentLayer = new Trk::DiscLayer(fullDiscTransform,
                                                fullDiscBounds->clone(),
                                                strawArray,
                                                m_layerThickness,
                                                new InDet::TRT_OverlapDescriptor(trtIdHelper));

          if (currentLayer) endcapLayers->push_back(currentLayer);
       } // end of sectorDiscBounds if
      } // end of layer loop
     } // end of wheel loop
    } // model/real geometry
  } // end of posneg loop

  delete layerMaterial; layerMaterial = 0;
  delete fullDiscBounds; fullDiscBounds = 0;

  return endcapLayers;

}


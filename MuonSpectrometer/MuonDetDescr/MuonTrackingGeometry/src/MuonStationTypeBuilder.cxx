/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonStationTypeBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Muon
#include "MuonTrackingGeometry/MuonStationTypeBuilder.h"
//MuonSpectrometer include
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonIdHelpers/MuonIdHelper.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/ILayerArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrInterfaces/ILayerBuilder.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinningType.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/NavBinnedArray1D.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/TrapezoidVolumeBounds.h"
#include "TrkVolumes/DoubleTrapezoidVolumeBounds.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/CombinedVolumeBounds.h"
#include "TrkVolumes/SubtractedVolumeBounds.h"
#include "TrkVolumes/VolumeExcluder.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/DiamondBounds.h"
#include "TrkGeometrySurfaces/SubtractedPlaneSurface.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/PlaneLayer.h"
#include "TrkGeometry/SubtractedPlaneLayer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkGeometry/OverlapDescriptor.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include<fstream>
// StoreGate
#include "StoreGate/StoreGateSvc.h"

// STD
#include <map>

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTube.h"

static const InterfaceID IID_IMuonStationTypeBuilder("MuonStationTypeBuilder", 1, 0);

const InterfaceID& Muon::MuonStationTypeBuilder::interfaceID()
{
  return IID_IMuonStationTypeBuilder;
}

// constructor
Muon::MuonStationTypeBuilder::MuonStationTypeBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_muonMgrLocation("MuonMgr"),
  m_multilayerRepresentation(true), 
  m_resolveSpacer(false), 
  m_trackingVolumeArrayCreator("Trk::TrackingVolumeArrayCreator/TrackingVolumeArrayCreator"),
  m_mdtTubeMat(),
  m_mdtFoamMat(),
  m_rpc46(),
  m_rpcDed(),
  m_rpcLayer(),
  m_rpcExtPanel(),
  m_rpcMidPanel(),
  m_matCSC01(),
  m_matCSCspacer1(),
  m_matCSC02(),
  m_matCSCspacer2(),
  m_matTGC01(),
  m_matTGC06()
{
  declareInterface<Muon::MuonStationTypeBuilder>(this);
  declareProperty("MuonDetManagerLocation",           m_muonMgrLocation);
  declareProperty("BuildMultilayerRepresentation",    m_multilayerRepresentation);
  declareProperty("ResolveSpacerBeams",               m_resolveSpacer);
}

// destructor
Muon::MuonStationTypeBuilder::~MuonStationTypeBuilder()
{}

// Athena standard methods
// initialize
StatusCode Muon::MuonStationTypeBuilder::initialize()
{
    
    // get Muon Spectrometer Description Manager
    ATH_CHECK( detStore()->retrieve(m_muonMgr) );

    ATH_MSG_DEBUG( m_muonMgr->geometryVersion()); 
    
    // Retrieve the tracking volume array creator   -------------------------------------------    
    ATH_CHECK( m_trackingVolumeArrayCreator.retrieve() );
    ATH_MSG_INFO("Retrieved tool " << m_trackingVolumeArrayCreator);

    // default (trivial) muon material properties 
    m_muonMaterial = new Trk::Material(10e10,10e10,0.,0.,0.);      

    m_materialConverter= new Trk::GeoMaterialConverter();
     
    ATH_MSG_INFO(name() <<" initialize() successful");    
    
    return StatusCode::SUCCESS;
}

const Trk::TrackingVolumeArray* Muon::MuonStationTypeBuilder::processBoxStationComponents(const GeoVPhysVol* mv, Trk::CuboidVolumeBounds* envelope) const
{

   ATH_MSG_DEBUG( name() <<" processing station components for " <<mv->getLogVol()->getName());    
///////////////////////////////////////////////////////////////////////////////////////////////////
   
   double tolerance = 0.001;   

   // loop over children volumes; ( make sure they do not exceed enveloping volume boundaries ?)
   // split into connected subvolumes ( assume ordering along X unless otherwise )
      std::vector<Trk::Volume*> compVol;
      std::vector<std::string> compName;
      std::vector<const GeoVPhysVol*> compGeo;
      std::vector<Amg::Transform3D> compTransf;
      double halfZ=0.;   
      double halfX1=0.;
      double halfX2=0.;
      double halfY1=0.;
      double halfY2=0.;
      for (unsigned int ich =0; ich< mv->getNChildVols(); ++ich) 
      {
        const GeoVPhysVol* cv = &(*(mv->getChildVol(ich))); 
        const GeoLogVol* clv = cv->getLogVol();
        Amg::Transform3D transf = Amg::CLHEPTransformToEigen(mv->getXToChildVol(ich));        
        // TEMPORARY CORRECTION 
        //if ( (mv->getLogVol()->getName()).substr(0,3)=="BMF" && (clv->getName()).substr(0,2)=="LB" ) {
        //	  ATH_MSG_DEBUG( "TEMPORARY MANUAL CORRECTION OF BMF SPACER LONG BEAM POSITION");
        //	  transf = transf * Amg::Translation3D(-37.5,0.,0.);
        //  } 
        // retrieve volumes for components
	Trk::VolumeBounds* volBounds=0; 
	Trk::Volume* vol; 
        if ( clv->getShape()->type()=="Trd")
	{
          const GeoTrd* trd = dynamic_cast<const GeoTrd*> (clv->getShape());
          halfX1 = trd->getXHalfLength1();
          halfX2 = trd->getXHalfLength2();
          halfY1 = trd->getYHalfLength1();
          halfY2 = trd->getYHalfLength2();
          halfZ  = trd->getZHalfLength();
          volBounds = new Trk::CuboidVolumeBounds(fmax(halfX1,halfX2),fmax(halfY1,halfY2),halfZ);
        } else if ( clv->getShape()->type()=="Box")
        {
	  const GeoBox* box = dynamic_cast<const GeoBox*> (clv->getShape());
	  halfX1 = box->getXHalfLength();
          halfX2 = halfX1;
          halfY1 = box->getYHalfLength();  
          halfY2 = halfY1;
	  halfZ  = box->getZHalfLength(); 
          volBounds = new Trk::CuboidVolumeBounds(halfX1,halfY1,halfZ);
        } else {
 	  //std::cout << "WARNING:component shape not Box nor Trapezoid, determining the x size from subcomponents" << std::endl; 
          double xSize = get_x_size(cv);
	  ATH_MSG_VERBOSE("subvolume not box nor trapezoid, estimated x size:" << xSize);
          //printChildren(cv);
          volBounds = new Trk::CuboidVolumeBounds(xSize,envelope->halflengthY(),envelope->halflengthZ());
        }
	vol = new Trk::Volume(new Amg::Transform3D(transf),volBounds);
	ATH_MSG_VERBOSE("subvolume center:"<< vol->center().x()<<","<< vol->center().y()<<","<< vol->center().z());
	std::string cname = clv->getName();
	std::string vname = mv->getLogVol()->getName();
        int nameSize = vname.size()-8;
        if (cname.substr(0,nameSize) == vname.substr(0,nameSize)) cname = cname.substr(nameSize,cname.size()-nameSize);  
        // order in X
        if (compVol.size()==0 || vol->center()[0]>=compVol.back()->center()[0]){
          compVol.push_back(vol);
          compName.push_back(cname);
          compGeo.push_back(cv);
          compTransf.push_back(transf);
        } else {
	  std::vector<Trk::Volume*>::iterator volIter=compVol.begin();
	  std::vector<std::string>::iterator  nameIter=compName.begin();
	  std::vector<const GeoVPhysVol*>::iterator  geoIter=compGeo.begin();
	  std::vector<Amg::Transform3D>::iterator  transfIter=compTransf.begin();
          while ( vol->center()[0]>= (*volIter)->center()[0]) {volIter++;nameIter++;geoIter++;transfIter++;}
          compVol.insert(volIter,vol);
          compName.insert(nameIter,cname);
          compGeo.insert(geoIter,cv);
          compTransf.insert(transfIter,transf);
        } 
      } // loop over components
      
      /* 
      // check components ordering
      for (unsigned i=0; i<compVol.size();i++){
	ATH_MSG_VERBOSE("list compononents:xcoord:name:"<<i<<":"<< compVol[i]->center()[0]<<" "<<compName[i]); 
      } 
      */      

      // define enveloping volumes for each "technology"
      std::vector<const Trk::TrackingVolume*> trkVols;
      double envX = envelope->halflengthX();
      double envY = envelope->halflengthY();
      double envZ = envelope->halflengthZ();
      double currX = -envX;
      double maxX = envX;
      bool openSpacer = false;
      bool openRpc = false;
      std::vector<const GeoVPhysVol*> geoSpacer;
      std::vector<const GeoVPhysVol*> geoRpc;
      std::vector<Amg::Transform3D> transfSpacer;
      std::vector<Amg::Transform3D> transfRpc;
      double spacerlowXsize=0; 
      double spaceruppXsize=0; 
      double rpclowXsize=0; 
      double rpcuppXsize=0; 
      std::vector<float> volSteps;
      volSteps.push_back(-envX);
      for (unsigned i=0; i<compVol.size();i++){
        bool comp_processed = false;
        const Trk::CuboidVolumeBounds* compBounds = dynamic_cast<const Trk::CuboidVolumeBounds*> (&(compVol[i]->volumeBounds()));
        // check return to comply with coverity
        if (!compBounds) {
	  ATH_MSG_ERROR( "box station component does not return cuboid shape" ) ;
          continue; 
	}
        //        
        double lowX = compVol[i]->center()[0]-compBounds->halflengthX();
        double uppX = compVol[i]->center()[0]+compBounds->halflengthX();
                
	if ( lowX < currX ) ATH_MSG_WARNING(" clash between components in volume:"<< compName[i]<<"current:"<< currX
					    <<": low edge of next volume:"<< lowX);
	if ( uppX > maxX ) ATH_MSG_WARNING(" clash between component and envelope:"<< compName[i]<<"upper:"<<uppX<<">"<<maxX);

        // close Rpc if no further components
        if (openRpc  && compName[i].substr(0,3) != "RPC" && compName[i].substr(0,3) != "Ded"){
          // low edge of current volume
          double Xcurr = compVol[i]->center()[0]-compBounds->halflengthX();
          if (Xcurr >= currX+rpclowXsize+rpcuppXsize) {
            Trk::CuboidVolumeBounds* rpcBounds = new Trk::CuboidVolumeBounds(0.5*(Xcurr-currX),envY,envZ);
	    Trk::Volume* rpcVol =new Trk::Volume(new Amg::Transform3D(Amg::Translation3D(currX+rpcBounds->halflengthX(),0.,0.)),rpcBounds);
            const Trk::TrackingVolume* rpcTrkVol = processRpc(rpcVol ,geoRpc,transfRpc);
            delete rpcVol;
            trkVols.push_back(rpcTrkVol);  
            volSteps.push_back(Xcurr); 
            currX = Xcurr;
            openRpc = false;
          } else {
	    ATH_MSG_WARNING("clash in Rpc definition!"); 
          } 
        }   
        // close spacer if no further components
        if (openSpacer &&  compName[i].substr(0,1) != "C" && compName[i].substr(0,2) != "LB"){
          // low edge of current volume
          double Xcurr = compVol[i]->center()[0]-compBounds->halflengthX();
          if (Xcurr-currX-(spacerlowXsize+spaceruppXsize)>= -tolerance ) {
            Trk::CuboidVolumeBounds* spacerBounds = new Trk::CuboidVolumeBounds(0.5*(Xcurr-currX),envY,envZ);
	    Trk::Volume spacerVol(new Amg::Transform3D(Amg::Translation3D(currX+spacerBounds->halflengthX(),0.,0.)),spacerBounds);
            const Trk::TrackingVolume* spacerTrkVol = processSpacer(spacerVol ,geoSpacer, transfSpacer);
            trkVols.push_back(spacerTrkVol);  
            volSteps.push_back(Xcurr); 
            currX = Xcurr;
            openSpacer = false;
          } else {
	    ATH_MSG_WARNING("clash in spacer definition!"); 
          } 
        }   
        if (compName[i].substr(0,3) == "RPC" || compName[i].substr(0,3) == "Ded" ) {
          if (!openRpc) {
            openRpc = true;
            geoRpc.clear();
	    geoRpc.push_back(compGeo[i]);
            transfRpc.clear();
	    transfRpc.push_back(compTransf[i]);
            //establish temporary volume size
            rpclowXsize = compVol[i]->center()[0]-currX;
            rpcuppXsize = compBounds->halflengthX();
            // check clash at low edge
            if (fabs(rpclowXsize) < compBounds->halflengthX()-tolerance) ATH_MSG_WARNING("rpc low edge - not enough space");
          } else {
            geoRpc.push_back(compGeo[i]);
	    transfRpc.push_back(compTransf[i]);
            // check temporary volume size
            if ( fabs(compVol[i]->center()[0]-currX) < compBounds->halflengthX()-tolerance)
              ATH_MSG_WARNING("rpc low edge - not enough space");
            if ( compVol[i]->center()[0]+compBounds->halflengthX() > currX + rpclowXsize+rpcuppXsize) 
	      rpcuppXsize += (compVol[i]->center()[0]+compBounds->halflengthX())-( currX + rpclowXsize+rpcuppXsize);
          }
          comp_processed = true;
        }
        if (compName[i].substr(0,1) == "C" || compName[i].substr(0,2) == "LB" ) {
          if (!openSpacer) {
            openSpacer = true;
            geoSpacer.clear();
	    geoSpacer.push_back(compGeo[i]);
            transfSpacer.clear();
	    transfSpacer.push_back(compTransf[i]);
            //establish temporary volume size
            spacerlowXsize = compVol[i]->center()[0]-currX;
            spaceruppXsize = compBounds->halflengthX();
            // check clash at low edge
            if (fabs(spacerlowXsize) < compBounds->halflengthX()-tolerance) {
	      ATH_MSG_WARNING( "spacer low edge - not enough space:current:center:halfSize:"
			       << currX <<","<<compVol[i]->center()[0]<<","<<compBounds->halflengthX()  );
            }
          } else {
            geoSpacer.push_back(compGeo[i]);
	    transfSpacer.push_back(compTransf[i]);
            // check temporary volume size
            if ( fabs(compVol[i]->center()[0]-currX) < compBounds->halflengthX()-tolerance) {
	      ATH_MSG_WARNING( "spacer low edge - not enough space:current:center:halfSize:"
			       << currX <<","<<compVol[i]->center()[0]<<","<<compBounds->halflengthX()  );
            }  
            if ( compVol[i]->center()[0]+compBounds->halflengthX() > currX + spacerlowXsize+spaceruppXsize) 
	      spaceruppXsize += (compVol[i]->center()[0]+compBounds->halflengthX())-( currX + spacerlowXsize+spaceruppXsize);
          }
          comp_processed = true;
        }
        if (compName[i].substr(0,3) == "MDT") {
          Trk::Volume* mdtVol;
	  Trk::CuboidVolumeBounds* mdtBounds=0; 
          // remove z shift in transform !! bugfix !!
          double zShift = compVol[i]->transform().translation()[2];
	  if ( fabs(zShift)>0 ) {
	    ATH_MSG_DEBUG("unusual z shift for subvolume:" << zShift); 
          }  
          //                                 (HepGeom::TranslateZ3D(-zShift)*(*compTransf[i])).getTranslation() <<std::endl;
          if (lowX == currX) {
	    mdtBounds = new Trk::CuboidVolumeBounds(compBounds->halflengthX(),envY,envZ);
            mdtVol = new Trk::Volume(new Amg::Transform3D(Amg::Translation3D(0.,0.,-zShift)*compVol[i]->transform()),mdtBounds);
	  } else {
            if ( fabs(lowX-currX)>0.002 ) { 
	      ATH_MSG_DEBUG( "Mdt volume size does not match the envelope:lowX,currX:" << lowX <<","<<currX );
	      ATH_MSG_DEBUG( "adjusting Mdt volume ");
	    }
	    mdtBounds = new Trk::CuboidVolumeBounds(compBounds->halflengthX()+0.5*(lowX-currX),envY,envZ);
            mdtVol = new Trk::Volume(new Amg::Transform3D(Amg::Translation3D(0.5*(currX-lowX),0.,0.)
							  *Amg::Translation3D(0.,0.,-zShift)*compVol[i]->transform()),mdtBounds);
          }
          double shiftSign = 1.; 
          if (fabs(zShift) > 0.) {
	    std::string stName = mv->getLogVol()->getName();
	    if ( stName.substr(0,4)=="BIR3" || stName.substr(0,4)=="BIR5" || stName.substr(0,4)=="BIR7" || stName.substr(0,5)=="BIR10" ) shiftSign = -1.;
          }
          const Trk::TrackingVolume* mdtTrkVol = processMdtBox(mdtVol,compGeo[i],
							       new Amg::Transform3D(Amg::Translation3D(0.,0.,-zShift)*compTransf[i]),
							       shiftSign*fabs(zShift));
          trkVols.push_back(mdtTrkVol); 
          currX += 2.*mdtBounds->halflengthX();
          volSteps.push_back(currX); 
          delete mdtVol;
          comp_processed = true;
          zShift = 0.;
        }
        if ( !comp_processed ) std::cout << "unknown technology:" <<compName[i]<<std::endl;
      } // end loop over station children

      // there may be a spacer still open
      if (openSpacer) {
        if (maxX >= currX+spacerlowXsize+spaceruppXsize) {
          Trk::CuboidVolumeBounds* spacerBounds = new Trk::CuboidVolumeBounds(0.5*(maxX-currX),envY,envZ);
	  Trk::Volume spacerVol(new Amg::Transform3D(Amg::Translation3D(currX+spacerBounds->halflengthX(),0.,0.)),spacerBounds);
	  //std::cout << "new Spacer volume:position:" << (spacerVol->transform()).getTranslation() << std::endl; 
	  //std::cout << "new Spacer volume:dimensions:" <<0.5*(maxX-currX)<<","<<envY<<","<<envZ << std::endl; 
          const Trk::TrackingVolume* spacerTrkVol = processSpacer(spacerVol ,geoSpacer, transfSpacer);
          trkVols.push_back(spacerTrkVol);  
          currX = maxX;
          volSteps.push_back(currX); 
          openSpacer = false;
        } else {
	  //std::cout <<"currX,maxX,lowX,uppX:"<< currX<<"," << maxX <<","<<spacerlowXsize<<"," <<spaceruppXsize<< std::endl; 
	  //std::cout << "clash in spacer definition!(last volume)" << std::endl; 
        }          
      }
      // there may be an Rpc still open
      if (openRpc) {
	//std::cout << "maxX, other:"<< maxX <<"," << currX+rpclowXsize+rpcuppXsize << std::endl;
        if (maxX >= currX+rpclowXsize+rpcuppXsize) {
          Trk::CuboidVolumeBounds* rpcBounds = new Trk::CuboidVolumeBounds(0.5*(maxX-currX),envY,envZ);
	  Trk::Volume* rpcVol =new Trk::Volume(new Amg::Transform3D(Amg::Translation3D(currX+rpcBounds->halflengthX(),0.,0.)),rpcBounds);
	  //std::cout << "new Rpc volume:position:" << (rpcVol->transform()).getTranslation() << std::cout; 
	  //std::cout << "new Rpc volume:dimensions:" <<0.5*(maxX-currX)<<","<<envY<<","<<envZ << std::cout; 
          const Trk::TrackingVolume* rpcTrkVol = processRpc(rpcVol ,geoRpc,transfRpc);
          delete rpcVol;
          trkVols.push_back(rpcTrkVol);  
          currX = maxX;
          volSteps.push_back(currX); 
          openRpc = false;
        } else {
	  std::cout << "clash in Rpc definition!(last volume)" << std::endl; 
        }          
      }
      // create VolumeArray (1DX) 
      const Trk::TrackingVolumeArray* components = 0;
      if (m_trackingVolumeArrayCreator) {
	Trk::BinUtility* binUtility = new Trk::BinUtility( volSteps, Trk::BinningOption::open, Trk::BinningValue::binX);
        components = m_trackingVolumeArrayCreator->cuboidVolumesArrayNav( trkVols, binUtility, false);
      }
      // std::cout << "tracking volume array created" << std::endl;

      for (size_t i=0; i<compVol.size(); i++) delete compVol[i];

     
   return components;  
}

const Trk::TrackingVolumeArray* Muon::MuonStationTypeBuilder::processTrdStationComponents(const GeoVPhysVol* mv, Trk::TrapezoidVolumeBounds* envelope ) const
{
  ATH_MSG_DEBUG( name() <<" processing station components for " <<mv->getLogVol()->getName() );    
///////////////////////////////////////////////////////////////////////////////////////////////////

    double tolerance = 0.0001;   
   
   // loop over children volumes; ( make sure they do not exceed enveloping volume boundaries ?)
   // split into connected subvolumes ( assume ordering along X unless otherwise )
      std::vector<Trk::Volume*> compVol;
      std::vector<std::string> compName;
      std::vector<const GeoVPhysVol*> compGeo;
      std::vector<Amg::Transform3D> compTransf;
      double halfZ=0.;   
      double halfX1=0.;
      double halfX2=0.;
      double halfY1=0.;
      double halfY2=0.;
      for (unsigned int ich =0; ich< mv->getNChildVols(); ++ich) 
      {
        //std::cout << "next component:"<< ich << std::endl;
        const GeoVPhysVol* cv = &(*(mv->getChildVol(ich))); 
        const GeoLogVol* clv = cv->getLogVol();
        Amg::Transform3D transf = Amg::CLHEPTransformToEigen(mv->getXToChildVol(ich));        
        //std::cout << "component:"<<ich<<":" << clv->getName() <<", made of "<<clv->getMaterial()->getName()<<","<<clv->getShape()->type()<<","  <<transf.getTranslation()<<std::endl;
        // retrieve volumes for components
	Trk::VolumeBounds* volBounds=0; 
	Trk::Volume* vol; 
        if ( clv->getShape()->type()=="Trd") {
          const GeoTrd* trd = dynamic_cast<const GeoTrd*> (clv->getShape());
          halfX1 = trd->getXHalfLength1();
          halfX2 = trd->getXHalfLength2();
          halfY1 = trd->getYHalfLength1();
          halfY2 = trd->getYHalfLength2();
          halfZ  = trd->getZHalfLength();
          if (halfX1==halfX2 && halfY1==halfY2) volBounds = new Trk::CuboidVolumeBounds(fmax(halfX1,halfX2),fmax(halfY1,halfY2),halfZ);
          if (halfX1==halfX2 && halfY1!=halfY2 ) {
	    transf *= Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.));
             volBounds = new Trk::TrapezoidVolumeBounds(halfY1,halfY2,halfZ,halfX1);
          }
          if (halfX1!=halfX2 && halfY1==halfY2 ) {
             volBounds = new Trk::TrapezoidVolumeBounds(halfX1,halfX2,halfY1,halfZ);
          }
          if (!volBounds) std::cout << "volume shape for component not recognized" << std::endl;
        } else if ( clv->getShape()->type()=="Box") {
	  const GeoBox* box = dynamic_cast<const GeoBox*> (clv->getShape());
	  halfX1 = box->getXHalfLength();
          //halfX2 = halfX1; //neither halfX2 nor halfY2 are ever used after this assignment
          halfY1 = box->getYHalfLength();  
          //halfY2 = halfY1;
	  halfZ  = box->getZHalfLength(); 
          volBounds = new Trk::CuboidVolumeBounds(halfX1,halfY1,halfZ);
        } else {
 	  //std::cout<<"WARNING:component shape not Box nor Trapezoid, determining the x size from subcomponents"<<std::endl; 
	  double xSize = get_x_size(cv);
          // printChildren(cv);
	  if (clv->getName().substr(0,1)!="C" && clv->getName().substr(0,2)!="LB")
	    transf *= Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.));
          volBounds = new Trk::TrapezoidVolumeBounds(envelope->minHalflengthX(),envelope->maxHalflengthX(),envelope->halflengthY(),xSize);
        }
	vol = new Trk::Volume(new Amg::Transform3D(transf),volBounds);
	// std::cout <<"volume center:"<< vol->center() << ","<< ich << std::endl;
	std::string cname = clv->getName();
	std::string vname = mv->getLogVol()->getName();
        int nameSize = vname.size()-8;
        if (cname.substr(0,nameSize) == vname.substr(0,nameSize)) cname = cname.substr(nameSize,cname.size()-nameSize);  
        // order in X
        if (compVol.size()==0 || vol->center()[0]>=compVol.back()->center()[0]){
          compVol.push_back(vol);
          compName.push_back(cname);
          compGeo.push_back(cv);
          compTransf.push_back(transf);
        } else {
	  std::vector<Trk::Volume*>::iterator volIter=compVol.begin();
	  std::vector<std::string>::iterator  nameIter=compName.begin();
	  std::vector<const GeoVPhysVol*>::iterator  geoIter=compGeo.begin();
	  std::vector<Amg::Transform3D>::iterator  transfIter=compTransf.begin();
          while ( vol->center()[0]>= (*volIter)->center()[0]) {volIter++;nameIter++;geoIter++;transfIter++;}
          compVol.insert(volIter,vol);
          compName.insert(nameIter,cname);
          compGeo.insert(geoIter,cv);
          compTransf.insert(transfIter,transf);
        } 
      } // loop over components
      /*
      // check components ordering
      for (unsigned i=0; i<compVol.size();i++){
	std::cout << compVol[i]->center()[0]<<" "<<compName[i]<<std::endl; 
      } 
      */
      // define enveloping volumes for each "technology"
      std::vector<const Trk::TrackingVolume*> trkVols;
      double envX1 = envelope->minHalflengthX();
      double envX2 = envelope->maxHalflengthX();
      double envY = envelope->halflengthY();
      double envZ = envelope->halflengthZ();
      //
      double currX = -envZ;
      double maxX = envZ;
      //
      bool openSpacer = false;
      std::vector<const GeoVPhysVol*> geoSpacer;
      std::vector<const GeoVPhysVol*> geoRpc;
      std::vector<Amg::Transform3D> transfSpacer;
      std::vector<Amg::Transform3D> transfRpc;
      double spacerlowXsize=0; 
      double spaceruppXsize=0; 
      double Xcurr=0;
      double lowX=0.;
      double uppX=0.;
      std::vector<float> volSteps;
      volSteps.push_back( -envelope->halflengthZ());
      for (unsigned i=0; i<compVol.size();i++){
        bool comp_processed = false;
        const Trk::CuboidVolumeBounds* compCubBounds = dynamic_cast<const Trk::CuboidVolumeBounds*> (&(compVol[i]->volumeBounds()));
        const Trk::TrapezoidVolumeBounds* compTrdBounds = dynamic_cast<const Trk::TrapezoidVolumeBounds*> (&(compVol[i]->volumeBounds()));
        if (compCubBounds) {
          lowX = compVol[i]->center()[0]-compCubBounds->halflengthX();
          uppX = compVol[i]->center()[0]+compCubBounds->halflengthX();
	  if ( lowX < currX ) std::cout<<"Warning: we have a clash between components here!"<< std::endl;
	  if ( uppX > maxX ) std::cout<<"Warning: we have a clash between component and envelope!"<< std::endl;
          // low edge of current volume
          Xcurr = compVol[i]->center()[0]-compCubBounds->halflengthX();
        } 
        if (compTrdBounds) {
          lowX = compVol[i]->center()[0]-compTrdBounds->halflengthZ();
          uppX = compVol[i]->center()[0]+compTrdBounds->halflengthZ();
	  if ( lowX < currX ) std::cout<<"Warning: we have a clash between components here!"<< std::endl;
	  if ( uppX > maxX ) std::cout<<"Warning: we have a clash between component and envelope!"<< std::endl;
          // low edge of current volume
          Xcurr = compVol[i]->center()[0]-compTrdBounds->halflengthZ();
        }
        if (!compCubBounds && !compTrdBounds ) {
           std::cout << "unknown volume shape" << std::endl; 
           return 0;
        }
        // close spacer if no further components
        if (openSpacer &&  compName[i].substr(0,1) != "C" && compName[i].substr(0,2) != "LB"){
          if (Xcurr-currX-(spacerlowXsize+spaceruppXsize)>= -tolerance ) {
            Trk::TrapezoidVolumeBounds* spacerBounds = new Trk::TrapezoidVolumeBounds(envX1,envX2,envY,0.5*(Xcurr-currX));
            Amg::Transform3D tr( Amg::Translation3D(currX+spacerBounds->halflengthZ(),0.,0.)*
				 Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.)));

	    Trk::Volume spacerVol(new Amg::Transform3D(tr),spacerBounds);
            const Trk::TrackingVolume* spacerTrkVol = processSpacer(spacerVol ,geoSpacer, transfSpacer);
            trkVols.push_back(spacerTrkVol);  
            currX = Xcurr;
            volSteps.push_back(Xcurr);
            openSpacer = false;
          } else {
	    std::cout << "currX,Xcurr,lowX,uppX" << currX <<"," << Xcurr<<","<<spacerlowXsize <<"," << spaceruppXsize<<std::endl;
	    std::cout << Xcurr-currX << "," << spacerlowXsize+spaceruppXsize << std::endl;  
	    std::cout << "clash in spacer definition!" << std::endl; 
          } 
        }   
        if (compName[i].substr(0,3) == "RPC" || compName[i].substr(0,3) == "Ded" ) {
	  std::cout << " RPC components for endcaps not coded " << std::endl;
        }
        if (compName[i].substr(0,1) == "C" || compName[i].substr(0,2) == "LB" ) {
          if (!openSpacer) {
	    // std::cout << "opening spacer,"<<currX<<std::endl;
            openSpacer = true;
            geoSpacer.clear();
	    geoSpacer.push_back(compGeo[i]);
            transfSpacer.clear();
	    transfSpacer.push_back(compTransf[i]);
            //establish temporary volume size
            spacerlowXsize = compVol[i]->center()[0]-currX;
            if (compCubBounds) {
              spaceruppXsize = compCubBounds->halflengthX();
              // check clash at low edge
              if (spacerlowXsize < compCubBounds->halflengthX()) 
                std::cout << "WARNING at spacer low edge - not enough space" << std::endl;
            }
            if (compTrdBounds) {
              spaceruppXsize = compTrdBounds->halflengthZ();
              // check clash at low edge
              if (spacerlowXsize < compTrdBounds->halflengthZ()) 
              std::cout << "WARNING at spacer low edge - not enough space" << std::endl;
            } 
          } else {
            geoSpacer.push_back(compGeo[i]);
	    transfSpacer.push_back(compTransf[i]);
            // check temporary volume size
            if (compCubBounds) {
               if ( compVol[i]->center()[0]-currX < compCubBounds->halflengthX()) 
                  std::cout << "WARNING at spacer low edge - not enough space" << std::endl;
               if ( compVol[i]->center()[0]+compCubBounds->halflengthX() > currX + spacerlowXsize+spaceruppXsize) 
	      spaceruppXsize += (compVol[i]->center()[0]+compCubBounds->halflengthX())-( currX + spacerlowXsize+spaceruppXsize);
            }
            if (compTrdBounds) {
               if ( compVol[i]->center()[0]-currX < compTrdBounds->halflengthZ()) 
                  std::cout << "WARNING at spacer low edge - not enough space" << std::endl;
               if ( compVol[i]->center()[0]+compTrdBounds->halflengthZ() > currX + spacerlowXsize+spaceruppXsize) 
	      spaceruppXsize += (compVol[i]->center()[0]+compTrdBounds->halflengthZ())-( currX + spacerlowXsize+spaceruppXsize);
            }
          }
          comp_processed = true;
        }
        if (compName[i].substr(0,3) == "MDT") {
          Trk::Volume* mdtVol=0;
	  Trk::TrapezoidVolumeBounds* mdtBounds=0; 
	  //std::cout <<"processMdtTrd:"<<envX1 <<","<<envX2 <<","<<envY<<","<<compTrdBounds->halflengthZ()<<std::endl;
          if (lowX == currX) {
	    mdtBounds = compTrdBounds ? new Trk::TrapezoidVolumeBounds(envX1,envX2,envY,compTrdBounds->halflengthZ()) :
	      new Trk::TrapezoidVolumeBounds(envX1,envX2,envY,compCubBounds->halflengthX());
            mdtVol = new Trk::Volume(new Amg::Transform3D(compVol[i]->transform()),mdtBounds);
	  } else {
            if (fabs(lowX-currX)>0.002 ) {
	      ATH_MSG_DEBUG( "Mdt volume size does not match the envelope:lowX,currX:" << lowX <<","<<currX );
	      ATH_MSG_DEBUG( "adjusting Mdt volume " );
	    }
	    mdtBounds = compTrdBounds ? new Trk::TrapezoidVolumeBounds(envX1,envX2,envY,compTrdBounds->halflengthZ()+0.5*(lowX-currX)):
	      new Trk::TrapezoidVolumeBounds(envX1,envX2,envY,compCubBounds->halflengthX()+0.5*(lowX-currX));
            mdtVol = new Trk::Volume(new Amg::Transform3D(Amg::Translation3D(0.,0.,0.5*(currX-lowX))*compVol[i]->transform()),mdtBounds);
          }
          const Trk::TrackingVolume* mdtTrkVol = processMdtTrd(mdtVol,compGeo[i],&compTransf[i]);
          trkVols.push_back(mdtTrkVol); 
          currX += 2.*mdtBounds->halflengthZ();
          volSteps.push_back(currX);
          delete mdtVol;
          comp_processed = true;
        }
        if ( !comp_processed ) std::cout << "unknown technology:" <<compName[i]<<std::endl;
      } // end loop over station children

      // there may be a spacer still open
      if (openSpacer) {
        if (maxX >= currX+spacerlowXsize+spaceruppXsize) {
          Trk::TrapezoidVolumeBounds* spacerBounds = new Trk::TrapezoidVolumeBounds(envX1,envX2,envY,0.5*(maxX-currX));
          
	  Trk::Volume spacerVol(new Amg::Transform3D( Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						      Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))*
						      Amg::Translation3D(0.,0.,currX+spacerBounds->halflengthZ())),spacerBounds);
          const Trk::TrackingVolume* spacerTrkVol = processSpacer(spacerVol ,geoSpacer, transfSpacer);
          trkVols.push_back(spacerTrkVol);  
          currX = maxX;
          volSteps.push_back(currX);
          openSpacer = false;
        } else {
	  std::cout <<"currX,maxX,lowX,uppX:"<< currX<<"," << maxX <<","<<spacerlowXsize<<"," <<spaceruppXsize<< std::endl; 
	  std::cout << "clash in spacer definition!(last volume)" << std::endl; 
        }          
      }
      // create VolumeArray (1DX) 
      const Trk::TrackingVolumeArray* components = 0;
      //Trk::BinUtility* binUtility = new Trk::BinUtility1DX( -( envelope->halflengthZ() ), volSteps);
      if (m_trackingVolumeArrayCreator) {
	Trk::BinUtility* binUtility = new Trk::BinUtility( volSteps, Trk::BinningOption::open, Trk::BinningValue::binX );
	components = m_trackingVolumeArrayCreator->trapezoidVolumesArrayNav( trkVols, binUtility, false);
        // std::cout << "tracking volume array created" << std::endl;
      }

      for (size_t i=0; i<compVol.size(); i++) delete compVol[i];
     
   return components;  
}

// finalize
StatusCode Muon::MuonStationTypeBuilder::finalize()
{
  
  delete m_matCSCspacer1;
  delete m_matCSCspacer2;
  delete m_materialConverter;
  delete m_matCSC01;
  delete m_matCSC02;
  delete m_matTGC01;
  delete m_matTGC06;
  delete m_mdtTubeMat;
  delete m_rpcLayer;
  delete m_rpcMidPanel;
  delete m_rpcExtPanel;
  delete m_muonMaterial;
  for (unsigned int i=0;i<m_rpcDed.size();i++) delete m_rpcDed[i];
  for (unsigned int i=0;i<m_mdtFoamMat.size();i++) delete m_mdtFoamMat[i];


    ATH_MSG_INFO( name() <<" finalize() successful");
 
    return StatusCode::SUCCESS;
}
//
const Trk::TrackingVolume* Muon::MuonStationTypeBuilder::processMdtBox(Trk::Volume*& vol,const GeoVPhysVol*& gv, Amg::Transform3D* transf, double zShift) const
{
  std::vector<const Trk::PlaneLayer*> layers;
  std::vector<double> x_array;
  std::vector<double> x_ref;
  std::vector<Trk::MaterialProperties*> x_mat;
  std::vector<double> x_thickness;
  std::vector<int> x_active;
  double currX = -100000; 
  // here one could save time by not reading all tubes  
  for (unsigned int ich =0; ich< gv->getNChildVols(); ++ich) {
    const GeoVPhysVol* cv = &(*(gv->getChildVol(ich))); 
    const GeoLogVol* clv = cv->getLogVol();
    Amg::Transform3D transfc = Amg::CLHEPTransformToEigen(gv->getXToChildVol(ich));        
    //std::cout << "MDT component:"<<ich<<":" << clv->getName() <<", made of "<<clv->getMaterial()->getName()<<","<<clv->getShape()->type()<<","<<transfc.getTranslation()<<std::endl;
    // printChildren(cv);
    Trk::MaterialProperties* mdtMat=0;
    double xv = 0.;
    int active = 0;
    if ( (clv->getName()).substr(0,3)=="MDT") {
      xv = 13.0055;  // the half-thickness
      if ( !m_mdtTubeMat ) {
	const GeoTube* tube=dynamic_cast<const GeoTube*> (clv->getShape());
        if (!tube) {
	  ATH_MSG_ERROR( "tube component does not return tube shape" ) ;
	} else {  
	  double volume = 8*(tube->getRMax())*(tube->getZHalfLength())*xv;
	  //std::cout << " part of layer volume assigned to 1 tube:" << volume << std::endl;
	  //std::cout << "tube dimensions:" << tube->getRMax() << "," << tube->getRMin() << "," << tube->getZHalfLength() << std::endl;
	  m_mdtTubeMat = new Trk::MaterialProperties(getAveragedLayerMaterial(cv,volume,2*xv));
	} 
      }        
      mdtMat = m_mdtTubeMat;
      active = 1; 
    }
    if ( (clv->getName())=="MultiLayerFoam") {
      //std::cout << "processing MultiLayerFoam" << std::endl;  
      xv = decodeX(clv->getShape());
      for (unsigned int i=0;i<m_mdtFoamMat.size();i++) {
        if ( fabs(xv-0.5*m_mdtFoamMat[i]->thickness())<0.001 ) {
	  mdtMat = m_mdtFoamMat[i];
          break;
	}
      }
      if (!mdtMat) {
        //std::cout << "adding new box MDT:foam thickness:"<< xv << std::endl;
        const Trk::CuboidVolumeBounds* cub =  dynamic_cast<const Trk::CuboidVolumeBounds*> (&(vol->volumeBounds()));
        if (!cub) {
	  ATH_MSG_ERROR( "box station component does not return cuboid shape" ) ;
	} else {  
	  double volume = 8*(cub->halflengthY())*(cub->halflengthZ())*xv;
	  m_mdtFoamMat.push_back(new Trk::MaterialProperties(getAveragedLayerMaterial(cv,volume,2*xv)));
	} 
        if (m_mdtFoamMat.size()) mdtMat = m_mdtFoamMat.back();
      }
    }
    if (  transfc.translation()[0] != currX ) {
      if (x_array.size() == 0 || transfc.translation()[0] > x_array.back() ) {
	x_array.push_back(  transfc.translation()[0] );
	x_mat.push_back(mdtMat);
	x_thickness.push_back( 2*xv );
        x_active.push_back(active);
	currX = transfc.translation()[0];
	if ( fabs(transfc.translation()[1])>0.001) {
	  // code 2.corrdinate shift
	  double ref =  transfc.translation()[2]+1e5;
	  ref += int(1000*transfc.translation()[1])*10e6; 
	  x_ref.push_back( ref ) ;
	} else {
	  x_ref.push_back( transfc.translation()[2] ) ;
	}
	// std::cout << "layer info included:" << clv->getName()<<"," << 2*xv <<","<< currX<< std::endl; 
      } else {
	std::vector<double>::iterator xIter=x_array.begin();
	std::vector<Trk::MaterialProperties*>::iterator mIter=x_mat.begin();
	std::vector<double>::iterator tIter=x_thickness.begin();
	std::vector<double>::iterator rIter=x_ref.begin();
	std::vector<int>::iterator aIter=x_active.begin();
	while ( transfc.translation()[0] > *xIter ) {xIter++;mIter++;rIter++;}
	x_array.insert(xIter,transfc.translation()[0]);
	x_mat.insert(mIter,mdtMat);
	x_thickness.insert(tIter,2*xv);
        x_active.insert(aIter,active);
	if ( fabs(transfc.translation()[1])>0.001) {
	  // code 2.corrdinate shift
	  double sign = (transfc.translation()[1]>0.) ? 1. : -1.;
	  double ref =  transfc.translation()[2]+sign*1e5;            
	  ref += int(1000*transfc.translation()[1])*10e6; 
	  x_ref.insert( rIter,ref ) ;
	} else {
	  x_ref.insert(rIter,transfc.translation()[2] ) ;
	}
	currX = transfc.translation()[0];
      }
    }         
  }
  // create layers // 
  const Trk::PlaneLayer* layer;
  double thickness=0.;
  Trk::OverlapDescriptor* od=0;
  const Trk::CuboidVolumeBounds* volBounds = dynamic_cast<const Trk::CuboidVolumeBounds*> (&(vol->volumeBounds()));
  if ( volBounds ) {
    double yv = volBounds->halflengthY();
    double zv = volBounds->halflengthZ();
    Trk::RectangleBounds* rbounds = new Trk::RectangleBounds(yv,zv); 
    Trk::SharedObject<const Trk::SurfaceBounds> bounds(rbounds);
    for (unsigned int iloop=0; iloop<x_array.size(); iloop++) {
      // x-y plane -> y-z plane
      thickness = x_thickness[iloop];
      Amg::Transform3D* cTr = new Amg::Transform3D( (*transf) * Amg::Translation3D(x_array[iloop],0.,0.)*
						    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.)));

      if (!x_mat[iloop]) ATH_MSG_WARNING("Undefined MDT layer material");
      Trk::MaterialProperties matLay = x_mat[iloop] ? *(x_mat[iloop]) : Trk::MaterialProperties(*m_muonMaterial,thickness);
      Trk::HomogeneousLayerMaterial mdtMaterial(matLay,0.);
      layer = new Trk::PlaneLayer(cTr,
                                  bounds,
                                  mdtMaterial,
                                  thickness,
                                  od );
      layer->setRef(x_ref[iloop]-zShift);
      //std::cout << " reference value set for layer:"<<iloop<<","<<layer->getRef()<<std::endl;
      //std::cout << transf->translation() << std::endl;
      layers.push_back(layer);
      
      // make preliminary identification of active layers
      layer->setLayerType(x_active[iloop]);
    }
  } 
  // create the BinnedArray
  //std::cout << "number of Mdt layers:"<<layers.size()<<std::endl;
  std::vector<Trk::SharedObject<const Trk::Layer> > layerOrder;
  std::vector<float> binSteps;
  // check if additional (navigation) layers needed
  // fix lower and upper bound of step vector to volume boundary
  // double minX = - volBounds->halflengthX();
  float minX = transf->translation()[0] - volBounds->halflengthX();
  binSteps.push_back(minX);
  if (layers.size()) {
    //minX = layers[0]->transform().translation()[0]-0.5*layers[0]->thickness();
    currX = minX; 
    for (unsigned int i=0;i<layers.size();i++) { 
      const Amg::Transform3D ltransf = layers[i]->transform();
      layerOrder.push_back(Trk::SharedObject<const Trk::Layer>(layers[i]));
      if (i<layers.size()-1) { 
	currX = ltransf.translation()[0]+0.5*layers[i]->thickness();     
	binSteps.push_back(currX);
      }
    }
    binSteps.push_back(transf->translation()[0]+volBounds->halflengthX());
  }
  //Trk::BinUtility* binUtility = new Trk::BinUtility1DX( minX, new std::vector<double>(binSteps));
  Trk::BinUtility* binUtility = new Trk::BinUtility( binSteps, Trk::BinningOption::open, Trk::BinningValue::binX);

  Trk::LayerArray* mdtLayerArray = 0;
  mdtLayerArray = new Trk::NavBinnedArray1D<Trk::Layer>(layerOrder, binUtility, new Amg::Transform3D(Trk::s_idTransform));     
  std::string name="MDT";
  const Trk::TrackingVolume* mdt= new Trk::TrackingVolume(*vol,
                                                          *m_muonMaterial,
                                                          mdtLayerArray,0,
                                                          name);         
  delete transf;
  return mdt;
}
//
const Trk::TrackingVolume* Muon::MuonStationTypeBuilder::processMdtTrd(Trk::Volume*& vol,const GeoVPhysVol*& gv, Amg::Transform3D* transf) const
{
   // std::cout << "processing MDT, number of children volumes:"<< gv->getNChildVols() <<std::endl; 
  std::vector<const Trk::PlaneLayer*> layers;
  std::vector<double> x_array;
  std::vector<Trk::MaterialProperties*> x_mat;
  std::vector<double> x_thickness;
  std::vector<double> x_ref;
  std::vector<int> x_active;
  double currX = -100000; 
  for (unsigned int ich =0; ich< gv->getNChildVols(); ++ich) {
    const GeoVPhysVol* cv = &(*(gv->getChildVol(ich))); 
    const GeoLogVol* clv = cv->getLogVol();
    Amg::Transform3D transfc = Amg::CLHEPTransformToEigen(gv->getXToChildVol(ich));        
    //std::cout << "MDT component:"<<ich<<":" << clv->getName() <<", made of "<<clv->getMaterial()->getName()<<","<<clv->getShape()->type()<<","<<transfc.translation()<<std::endl;
    double xv = 0.;
    int active = 0;
    if (clv->getShape()->type()=="Trd"){
      const GeoTrd* trd = dynamic_cast<const GeoTrd*> (clv->getShape());
      double x1v = trd->getXHalfLength1();
      double x2v = trd->getXHalfLength2();
      if ( x1v==x2v ) xv = x1v;
    }
    Trk::MaterialProperties* mdtMat=0; 
    if ( (clv->getName()).substr(0,3)=="MDT") {
      xv = 13.0055;  // the half-thickness
      if (!m_mdtTubeMat ) {
	const GeoTube* tube=dynamic_cast<const GeoTube*> (clv->getShape());
	double volume = 8*(tube->getRMax())*(tube->getZHalfLength())*xv;
	// std::cout << " part of layer volume assigned to 1 tube:" << vol << std::endl;
	// std::cout << "tube dimensions:" << tube->getRMax() << "," << tube->getRMin() << "," << tube->getZHalfLength() << std::endl;
	m_mdtTubeMat = new Trk::MaterialProperties(getAveragedLayerMaterial(cv,volume,2*xv)); 
      }        
      mdtMat = m_mdtTubeMat;
      active = 1;
    }
    if ( (clv->getName())=="MultiLayerFoam") {
      //std::cout << "processing MultiLayerFoam" << std::endl;  
      xv = decodeX(clv->getShape());
      for (unsigned int i=0;i<m_mdtFoamMat.size();i++) {
        if ( fabs(xv-0.5*m_mdtFoamMat[i]->thickness())<0.001 ) {
	  mdtMat = m_mdtFoamMat[i];
          break;
	}
      }
      if (!mdtMat) {
        //std::cout << "adding new trd MDT:foam thickness:"<< xv << std::endl;
        const Trk::TrapezoidVolumeBounds* trd =  dynamic_cast<const Trk::TrapezoidVolumeBounds*> (&(vol->volumeBounds()));
        // check return to comply with coverity
        if (!trd) {
	  ATH_MSG_ERROR( "trd station component does not return trapezoid shape" ) ;
	} else {  
	  double volume = 4*(trd->minHalflengthX()+trd->maxHalflengthX())*(trd->halflengthY())*xv;
	  m_mdtFoamMat.push_back(new Trk::MaterialProperties(getAveragedLayerMaterial(cv,volume,2*xv))); 
	}
        if (m_mdtFoamMat.size()) mdtMat = m_mdtFoamMat.back();
      }
    }
    
    if (  transfc.translation()[0] != currX ) {
      if (x_array.size() == 0 || transfc.translation()[0] > x_array.back() ) {
	x_array.push_back(  transfc.translation()[0] );
	x_mat.push_back(mdtMat);
	x_thickness.push_back( 2*xv );
	x_ref.push_back(  transfc.translation()[2] );
	currX = transfc.translation()[0];
	x_active.push_back(active);
      } else {
	std::vector<double>::iterator xIter=x_array.begin();
	std::vector<Trk::MaterialProperties*>::iterator mIter=x_mat.begin();
	std::vector<double>::iterator tIter=x_thickness.begin();
	std::vector<double>::iterator rIter=x_ref.begin();
	std::vector<int>::iterator aIter=x_active.begin();
	while ( transfc.translation()[0] > *xIter ) {xIter++;mIter++;rIter++;}
	x_array.insert(xIter,transfc.translation()[0]);
	x_mat.insert(mIter,mdtMat);
	x_thickness.insert(tIter,2*xv);
	x_ref.insert(rIter,transfc.translation()[2] );
	x_active.insert(aIter,active);
	currX = transfc.translation()[0];
      }
    }     
  }
  // create layers // 
  const Trk::PlaneLayer* layer;
  double thickness=0.;
  Trk::OverlapDescriptor* od=0;
  const Trk::TrapezoidVolumeBounds* volBounds = dynamic_cast<const Trk::TrapezoidVolumeBounds*> (&(vol->volumeBounds()));
  if ( volBounds ) {
    double x1v = volBounds->minHalflengthX();
    double x2v = volBounds->maxHalflengthX();
    double yv = volBounds->halflengthY();
    // x-y plane -> y-z plane
    Trk::TrapezoidBounds* tbounds= new Trk::TrapezoidBounds(x1v,x2v,yv);
    Trk::SharedObject<const Trk::SurfaceBounds> bounds(tbounds);
    for (unsigned int iloop=0; iloop<x_array.size(); iloop++) {      
      thickness = x_thickness[iloop];
      if (!x_mat[iloop]) ATH_MSG_WARNING("Undefined MDT layer material");
      Trk::MaterialProperties matLay = x_mat[iloop] ? *(x_mat[iloop]) : Trk::MaterialProperties(*m_muonMaterial,thickness);
      Trk::HomogeneousLayerMaterial mdtMaterial(matLay,0.);
      Amg::Transform3D* cTr = new Amg::Transform3D( (*transf) * Amg::Translation3D(0.,0.,x_array[iloop]) );
      layer = new Trk::PlaneLayer(cTr,
                                  bounds,
                                  mdtMaterial,
                                  thickness,
                                  od );
      layer->setRef(x_ref[iloop]);
      //std::cout << " reference value set for layer:"<<iloop<<","<<layer->getRef()<<std::endl;
      layers.push_back(layer);
      // make preliminary identification of active layers
      layer->setLayerType(x_active[iloop]);
    } 
    
    // create the BinnedArray
    //std::cout << "number of Mdt layers:"<<layers.size()<<std::endl;
    std::vector<Trk::SharedObject<const Trk::Layer> > layerOrder;
    std::vector<float> binSteps;
    // 
    // double minX = - volBounds->halflengthZ();
    double minX = transf->translation()[0] - volBounds->halflengthZ();
    binSteps.push_back(minX);
    if (layers.size()) {
      //minX = layers[0]->transform().translation()[0]-0.5*layers[0]->thickness();
      currX = minX; 
      for (unsigned int i=0;i<layers.size();i++) { 
	const Amg::Transform3D ltransf = layers[i]->transform();
	layerOrder.push_back(Trk::SharedObject<const Trk::Layer>(layers[i]));
	if ( i < layers.size()-1 ) { 
	  currX = ltransf.translation()[0]+0.5*layers[i]->thickness();
	  binSteps.push_back(currX);
	}     
      }
      binSteps.push_back( transf->translation()[0] + volBounds->halflengthZ() );
    }
    /*
      for (unsigned int i=0;i<layers.size();i++) { 
      const Amg::Transform3D* ltransf = new Amg::Transform3D(layers[i]->transform());
      layerOrder.push_back(LayTr(Trk::SharedObject<const Trk::Layer>(layers[i]), ltransf ));
      if (i>0) {
      binSteps.push_back(ltransf->translation()[0] -currX);
      } else {
      minX = ltransf->translation()[0];
      } 
      currX = ltransf->translation()[0];
      }
    */
    
    //Trk::BinUtility* binUtility = new Trk::BinUtility1DX( minX, new std::vector<double>(binSteps));
    Trk::BinUtility* binUtility = new Trk::BinUtility( binSteps, Trk::BinningOption::open, Trk::BinningValue::binX );
    Trk::LayerArray* mdtLayerArray = 0;
    mdtLayerArray = new Trk::NavBinnedArray1D<Trk::Layer>(layerOrder, binUtility, new Amg::Transform3D(Trk::s_idTransform));     
    std::string name="MDT";
    const Trk::TrackingVolume* mdt= new Trk::TrackingVolume(*vol,
							    *m_muonMaterial,
							    mdtLayerArray,0,
							    name);         
    // std::cout << "Mdt processed with" << layers.size() << " layers" << std::endl;
    return mdt;
  }
  return 0;
}
//
const Trk::TrackingVolume* Muon::MuonStationTypeBuilder::processRpc(Trk::Volume*& vol,std::vector<const GeoVPhysVol*> gv, std::vector<Amg::Transform3D> transfc) const
{
  // layers correspond to DedModules and RpcModules; all substructures averaged in material properties
  std::vector<const Trk::Layer*> layers;
  for (unsigned int ic=0; ic<gv.size(); ++ic) {
    //std::cout << "processing Rpc component:"<< gv[ic]->getLogVol()->getName() <<","<< transfc[ic].rotation()<<std::endl;
    const GeoLogVol* glv = gv[ic]->getLogVol();
    const GeoShape* shape = glv->getShape();
    if (shape->type()!="Box" && shape->type()!="Trd") {
      const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction*> (shape);
      const GeoShape* subt = 0;
      while (sub) {
        subt = sub->getOpA();
        sub = dynamic_cast<const GeoShapeSubtraction*> (subt);
      }
      shape = subt; 
    }
    if (shape && shape->type()=="Box") {
      const GeoBox* box = dynamic_cast<const GeoBox*> (shape);
      double xs = box->getXHalfLength();
      double ys = box->getYHalfLength();
      double zs = box->getZHalfLength();
      // std::cout << "dimensions:"<<box->getXHalfLength() << ","<<box->getYHalfLength() << ","<<box->getZHalfLength() << std::endl;
      // translating into layer; x dimension defines thickness
      const Trk::PlaneLayer* layer;
      double thickness=2*xs;
      Trk::OverlapDescriptor* od=0;
      Trk::RectangleBounds* rbounds = new Trk::RectangleBounds(ys,zs);
      Trk::SharedObject<const Trk::SurfaceBounds> bounds(rbounds); 
      Amg::Transform3D* cTr = new Amg::Transform3D( transfc[ic] *
						    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.)));
      Trk::MaterialProperties rpcMat(0.,10.e10,10.e10,13.,26.,0.);               // default
      if ( (glv->getName()).substr(0,3)=="Ded" ) {
        // find if material exists already
        bool found = false;
        for (unsigned int i=0;i<m_rpcDed.size();i++) {
	  if (fabs(thickness-m_rpcDed[i]->thickness())<0.001) { 
	    rpcMat = Trk::MaterialProperties(*m_rpcDed[i]); 
	    found = true;
	    break;
	  }
        } 
	if (!found) {
	  double volc = 8*xs*ys*zs;
	  m_rpcDed.push_back( new Trk::MaterialProperties(getAveragedLayerMaterial(gv[ic],volc,2*xs)));
	  rpcMat = Trk::MaterialProperties(*m_rpcDed.back());
	}
      } else {
        //printChildren(gv[ic]);
        if (fabs(thickness-46.0)<0.001) {
          if (!m_rpc46) {
            double volc = 8*xs*ys*zs;
            m_rpc46 = new Trk::MaterialProperties(getAveragedLayerMaterial(gv[ic],volc,2*xs));
          }
          rpcMat=Trk::MaterialProperties(*m_rpc46);  
        } else { ATH_MSG_WARNING( name() << "RPC module thickness different from 46:" << thickness ); }
      }
          
      Trk::HomogeneousLayerMaterial rpcMaterial(rpcMat,0.);
      layer = new Trk::PlaneLayer(cTr,
				  bounds,
				  rpcMaterial,
				  thickness,
				  od );
      layers.push_back(layer);
      // make preliminary identification of active layers
      if ((glv->getName()).substr(0,3)!="Ded" ) {
        layer->setLayerType(1);
      } else {
        layer->setLayerType(0);
      }
    } else if (shape && shape->type()=="Trd") {
      const GeoTrd* trd = dynamic_cast<const GeoTrd*> (shape);
      double xs1 = trd->getXHalfLength1();
      double xs2 = trd->getXHalfLength2();
      double ys1 = trd->getYHalfLength1();
      double ys2 = trd->getYHalfLength2();
      double zs = trd->getZHalfLength();
      // translating into layer; x dimension defines thickness
      if (xs1==xs2 && ys1==ys2) {
        const Trk::PlaneLayer* layer;
        double thickness=2*xs1;
        Trk::OverlapDescriptor* od=0;
        Trk::RectangleBounds* rbounds = new Trk::RectangleBounds(ys1,zs); 
        Trk::SharedObject<const Trk::SurfaceBounds> bounds(rbounds);
        Amg::Transform3D* cTr = new Amg::Transform3D( transfc[ic] *
						    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.)));
	Trk::MaterialProperties rpcMat(0.,10.e10,10.e10,13.,26.,0.);               // default
        if ( (glv->getName()).substr(0,3)=="Ded" ) {
	  // find if material exists already
	  bool found = false;
	  for (unsigned int i=0;i<m_rpcDed.size();i++) {
	    if (fabs(thickness-m_rpcDed[i]->thickness())<0.001) {
	      rpcMat = Trk::MaterialProperties(*m_rpcDed[i]); 
	      found = true;
	      break;
	    }
	  } 
	  if (!found) {
	    double volc = 8*xs1*ys1*zs;
	    m_rpcDed.push_back(new Trk::MaterialProperties(getAveragedLayerMaterial(gv[ic],volc,2*xs1)));
	    rpcMat = Trk::MaterialProperties(*m_rpcDed.back());
	  }
          // create Ded layer
	  Trk::HomogeneousLayerMaterial rpcMaterial(rpcMat,0.);
	  layer = new Trk::PlaneLayer(cTr, bounds, rpcMaterial, thickness, od );
          layers.push_back(layer);
	  layer->setLayerType(0);
        } else {
          // RPC layer; step one level below to resolve strip planes
          //printChildren(gv[ic]);
          unsigned int ngc = gv[ic]->getNChildVols();
          for (unsigned int igc=0; igc<ngc; igc++) {
	    Amg::Transform3D trgc(Trk::s_idTransform);             
            if (transfc[ic].rotation().isIdentity()) trgc = Amg::CLHEPTransformToEigen(gv[ic]->getXToChildVol(igc));
            else trgc = Amg::AngleAxis3D(M_PI,Amg::Vector3D(0.,0.,1.))*Amg::CLHEPTransformToEigen(gv[ic]->getXToChildVol(igc)); 
	    
	    const GeoVPhysVol* gcv = &(*(gv[ic]->getChildVol(igc)));
	    const GeoLogVol* gclv = gcv->getLogVol();
	    const GeoShape* lshape = gclv->getShape();
	    while (lshape->type()=="Subtraction") {
	      const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction*> (lshape); 
	      lshape = sub->getOpA(); 
	    } 
	    const GeoTrd* gtrd = dynamic_cast<const GeoTrd*> (lshape);
	    double gx = gtrd->getXHalfLength1();
	    double gy = gtrd->getYHalfLength1();
	    double gz = gtrd->getZHalfLength();
            
	    if ( (gclv->getName()).substr(0,6)=="RPC_AL" ) {
	      if (fabs(gx-5.0) < 0.001) {
		if (!m_rpcExtPanel) {
		  double volc = 8*gx*gy*gz;
		  m_rpcExtPanel = new Trk::MaterialProperties(getAveragedLayerMaterial(gcv,volc,2*gx));
		}
		rpcMat=Trk::MaterialProperties(*m_rpcExtPanel);
	      } else if (fabs(gx - 4.3) < 0.001) {
		if (!m_rpcMidPanel) {
		  double volc = 8*gx*gy*gz;
		  m_rpcMidPanel = new Trk::MaterialProperties(getAveragedLayerMaterial(gcv,volc,2*gx));
		}
		rpcMat=Trk::MaterialProperties(*m_rpcMidPanel);
	      } else {
		ATH_MSG_WARNING( name() << "unknown RPC panel:" << gx );               
	      }
	      // create Rpc panel layers 
	      thickness = 2*gx;
	      Trk::HomogeneousLayerMaterial rpcMaterial(rpcMat,0.);
	      layer = new Trk::PlaneLayer(new Amg::Transform3D(Amg::Translation3D(trgc.translation())*(*cTr)),
					  bounds, rpcMaterial, thickness, od );
	      layers.push_back(layer);
	      layer->setLayerType(0);
	    } else if  ( (gclv->getName())=="Rpclayer" ) {
	      if ( fabs(gx-6.85)>0.001 )  ATH_MSG_WARNING( name() << " unusual thickness of RPC layer:" << 2*gx );
	      if (!m_rpcLayer) {                   
		double volc = 8*gx*gy*gz;
		// material allocated to two strip planes ( gas volume suppressed )
		m_rpcLayer = new Trk::MaterialProperties(getAveragedLayerMaterial(gcv,volc,2*gx));
	      }
	      rpcMat=Trk::MaterialProperties(*m_rpcLayer);
	      // define 1 layer for 2 strip planes
	      thickness = 2*gx;
	      Trk::HomogeneousLayerMaterial rpcMaterial(rpcMat,0.);
	      layer = new Trk::PlaneLayer(new Amg::Transform3D(Amg::Translation3D(trgc.translation())*(*cTr)),
					  bounds, rpcMaterial, thickness, od );
	      layers.push_back(layer);
	      layer->setLayerType(1);               
	    } else {
	      ATH_MSG_WARNING( name()  << "unknown RPC component? " << gclv->getName() );
	    }
	  }
          delete cTr;
	}
      } else {
	ATH_MSG_WARNING( name() << "RPC true trapezoid layer, not coded yet" );
      }
    } else {
      ATH_MSG_WARNING( name() << "RPC layer shape not recognized" );
    }
  } // end loop over Modules

  std::vector<const Trk::Layer*>* rpcLayers = new std::vector<const Trk::Layer*>(layers); 
  std::string name="RPC";
  const Trk::TrackingVolume* rpc= new Trk::TrackingVolume(*vol,
                                                          *m_muonMaterial,
                                                          rpcLayers,
                                                          name);         
  ATH_MSG_DEBUG( " Rpc component volume processed with" << layers.size() << " layers");
  return rpc;
}
//

const Trk::TrackingVolume* Muon::MuonStationTypeBuilder::processSpacer(Trk::Volume& vol,std::vector<const GeoVPhysVol*> gv, std::vector<Amg::Transform3D> transf) const
{
  // spacers: one level below, assumed boxes
  std::vector<const Trk::Layer*> layers;
  // resolve child volumes
  std::vector<const GeoVPhysVol*>::iterator vIter = gv.begin();
  std::vector<Amg::Transform3D>::iterator tIter = transf.begin();
  while (vIter != gv.end() ) {
    if ((*vIter)->getNChildVols()) {
      for (unsigned int ich=0; ich<(*vIter)->getNChildVols();++ich) {
	gv.push_back(&(*((*vIter)->getChildVol(ich))));
        transf.push_back(Amg::Transform3D( (*tIter)*Amg::CLHEPTransformToEigen((*vIter)->getXToChildVol(ich))));  
      }
      vIter=gv.erase(vIter); tIter=transf.erase(tIter);
    } else { vIter++; tIter++; }
  } 
  // translate into layers
  for (unsigned int ic=0; ic<gv.size(); ++ic) {
    const GeoLogVol* clv = gv[ic]->getLogVol();        
    //std::cout << "Spacer component:"<<ic<<":" << clv->getName() <<", made of "<<clv->getMaterial()->getName()<<","<<clv->getShape()->type()<<","<<transf[ic].translation()<<std::endl; 
    //Trk::ExtendedMaterialProperties cmat = m_materialConverter->convertExtended(clv->getMaterial());
    Trk::Material cmat = m_materialConverter->convert(clv->getMaterial());
    Trk::OverlapDescriptor* od=0;
    if (clv->getShape()->type()=="Box") {
      const GeoBox* box = dynamic_cast<const GeoBox*> (clv->getShape());
      double xs = box->getXHalfLength();
      double ys = box->getYHalfLength();
      double zs = box->getZHalfLength();
      // translating into layer; find minimal size
      const Trk::PlaneLayer* layer;
      Trk::RectangleBounds* rbounds=0;
      double thickness=0.;
      Amg::Transform3D* cTr = 0;
      if (zs <= xs && zs <= ys ) { // x-y plane
	rbounds = new Trk::RectangleBounds(xs,ys); 
	thickness = 2*zs;
        cTr = new Amg::Transform3D(transf[ic]);
      } else if (xs <= ys && xs <= zs ) { // x-y plane -> y-z plane
	rbounds = new Trk::RectangleBounds(ys,zs); 
	thickness = 2*xs; 
	cTr = new Amg::Transform3D( transf[ic] *
				    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
				    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.)));
      } else {  // x-y plane -> x-z plane
	rbounds = new Trk::RectangleBounds(xs,zs); 
	thickness = 2*ys; 
	cTr = new Amg::Transform3D( transf[ic] * Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(1.,0.,0.)));
      } 
      Trk::MaterialProperties material(thickness,cmat.X0,cmat.L0,cmat.A,cmat.Z,cmat.rho);  
      Trk::HomogeneousLayerMaterial spacerMaterial(material,0.);
      Trk::SharedObject<const Trk::SurfaceBounds> bounds(rbounds);
      
      layer = new Trk::PlaneLayer(cTr,
				  bounds,
				  spacerMaterial,
				  thickness,
				  od,0 );
      layers.push_back(layer);
    } else if ( clv->getShape()->type()=="Subtraction") {
      const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction*> (clv->getShape());
      if (sub && sub->getOpA()->type()=="Box" && sub->getOpB()->type()=="Box") {
        // LB
        const GeoBox* boxA = dynamic_cast<const GeoBox*> (sub->getOpA());
        const GeoBox* boxB = dynamic_cast<const GeoBox*> (sub->getOpB());
	Trk::RectangleBounds* rbounds = new Trk::RectangleBounds(boxA->getYHalfLength(),boxA->getZHalfLength());
        double thickness = (boxA->getXHalfLength()-boxB->getXHalfLength());
        double shift     = 0.5*(boxA->getXHalfLength()+boxB->getXHalfLength());
	Trk::MaterialProperties material(0.,10.e10,10.e10,13.,26.,0.);
	Trk::HomogeneousLayerMaterial spacerMaterial;
	Trk::SharedObject<const Trk::SurfaceBounds> bounds;
        if (thickness>0.) {
	  material = Trk::MaterialProperties(thickness,cmat.X0,cmat.L0,cmat.A,cmat.Z,cmat.rho);  
	  spacerMaterial = Trk::HomogeneousLayerMaterial(material,0.);
	  bounds = Trk::SharedObject<const Trk::SurfaceBounds>(rbounds);
	  Trk::PlaneLayer* layx = new Trk::PlaneLayer(new Amg::Transform3D( transf[ic]*Amg::Translation3D(shift,0.,0.)*
									    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
									    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))),
						      bounds, spacerMaterial, thickness, od, 0 );
	  layers.push_back(layx)  ; 
	  Trk::SharedObject<const Trk::SurfaceBounds> bounds2(bounds);
	  Trk::PlaneLayer* layxx = new Trk::PlaneLayer(new Amg::Transform3D( transf[ic]*Amg::Translation3D(-shift,0.,0.)*
									    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
									    Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))),
						      bounds2, spacerMaterial, thickness, od, 0 );
          // TODO find out why copy with shift crashes in the destructor ( double transform delete )
	  //Trk::PlaneLayer* layxx = new Trk::PlaneLayer(*layx,Amg::Transform3D(Amg::Translation3D(-2*shift,0.,0.)));
	  layers.push_back(layxx) ; 
	}
        thickness = (boxA->getYHalfLength()-boxB->getYHalfLength());
        if (thickness>0.) {
	  material = Trk::MaterialProperties(thickness,cmat.X0,cmat.L0,cmat.A,cmat.Z,cmat.rho); 
	  spacerMaterial = Trk::HomogeneousLayerMaterial(material,0.);
	  shift     = 0.5*(boxA->getYHalfLength()+boxB->getYHalfLength());
	  rbounds = new Trk::RectangleBounds(boxB->getXHalfLength(),boxA->getZHalfLength());
	  bounds = Trk::SharedObject<const Trk::SurfaceBounds>(rbounds);
	  Trk::PlaneLayer* lay = new Trk::PlaneLayer(new Amg::Transform3D( transf[ic]*Amg::Translation3D(0.,shift,0.)*
									   Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(1.,0.,0.))),
						     bounds, spacerMaterial, thickness, od, 0 );
	  layers.push_back(lay)  ; 
	  Trk::SharedObject<const Trk::SurfaceBounds> bounds2(bounds);
	  Trk::PlaneLayer* layy = new Trk::PlaneLayer(new Amg::Transform3D( transf[ic]*Amg::Translation3D(0.,-shift,0.)*
									   Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(1.,0.,0.))),
						     bounds2, spacerMaterial, thickness, od, 0 );
          // TODO find out why copy with shift crashes in the destructor ( double transform delete )
	  //Trk::PlaneLayer* layy = new Trk::PlaneLayer(*lay,Amg::Transform3D(Amg::Translation3D(0.,-2*shift,0.)));
	  layers.push_back(layy) ; 
	}
        thickness = (boxA->getZHalfLength()-boxB->getZHalfLength());
        if (thickness>0.) {
	  material = Trk::MaterialProperties(thickness,cmat.X0,cmat.L0,cmat.A,cmat.Z,cmat.rho);   
	  spacerMaterial = Trk::HomogeneousLayerMaterial(material,0.);
	  shift     = 0.5*(boxA->getZHalfLength()+boxB->getZHalfLength());
	  rbounds = new Trk::RectangleBounds(boxB->getXHalfLength(),boxB->getYHalfLength());
	  bounds = Trk::SharedObject<const Trk::SurfaceBounds>(rbounds);
	  Trk::PlaneLayer* layz = new Trk::PlaneLayer(new Amg::Transform3D( transf[ic]*Amg::Translation3D(0.,0.,shift)), 
						      bounds, spacerMaterial, thickness, od, 0 );
	  layers.push_back(layz)  ;
	  Trk::SharedObject<const Trk::SurfaceBounds> bounds2(bounds);
	  Trk::PlaneLayer* layzz = new Trk::PlaneLayer(new Amg::Transform3D( transf[ic]*Amg::Translation3D(0.,0.,-shift)), 
						       bounds2, spacerMaterial, thickness, od, 0 );
          // TODO find out why copy with shift crashes in the destructor ( double transform delete )
	  //Trk::PlaneLayer* layzz = new Trk::PlaneLayer(*layz,Amg::Transform3D(Amg::Translation3D(0.,0.,-2*shift)));
	  layers.push_back(layzz) ; 
	}
      } else if (sub) {
	std::vector<std::pair<const GeoShape*,Amg::Transform3D> > subVs;
	const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (sub->getOpB());
	if (shift) subVs.push_back(std::pair<const GeoShape*,Amg::Transform3D>(shift->getOp(),Amg::CLHEPTransformToEigen(shift->getX())));
        const GeoShape* shape = sub->getOpA();
        while (shape->type()=="Subtraction") {
          const GeoShapeSubtraction* subtr = dynamic_cast<const GeoShapeSubtraction*> (shape);
          const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (subtr->getOpB());
          if (shift) subVs.push_back(std::pair<const GeoShape*,Amg::Transform3D>(shift->getOp(),Amg::CLHEPTransformToEigen(shift->getX())));
          shape = subtr->getOpA();  
	}
        const GeoBox* box = dynamic_cast<const GeoBox*> (shape);
        if (box && subVs.size()==4) {
	  Trk::Volume* v1 = 0; Trk::Volume* v2 = 0; Trk::VolumeExcluder* volExcl=0; 
          const GeoBox* sb1 = dynamic_cast<const GeoBox*> (subVs[0].first);
          if (sb1) v1 = new Trk::Volume(new Amg::Transform3D(subVs[0].second),
					new Trk::CuboidVolumeBounds(sb1->getXHalfLength(),
								    sb1->getYHalfLength(),
								    sb1->getZHalfLength()));
          const GeoBox* sb2 = dynamic_cast<const GeoBox*> (subVs[1].first);
          if (sb2) v2 = new Trk::Volume(new Amg::Transform3D(subVs[1].second),
					new Trk::CuboidVolumeBounds(sb2->getXHalfLength(),
								    sb2->getYHalfLength(),
								    sb2->getZHalfLength()));
           
          const GeoBox* boxB = dynamic_cast<const GeoBox*> (subVs[2].first);
          if (boxB && v1 && v2) {
	    Trk::RectangleBounds* rbounds = new Trk::RectangleBounds(box->getYHalfLength(),box->getZHalfLength());
	    double thickness = (box->getXHalfLength()-boxB->getXHalfLength());
	    double shift     = 0.5*(box->getXHalfLength()+boxB->getXHalfLength());
	    Trk::SharedObject<const Trk::SurfaceBounds> bounds(rbounds);
	    Trk::Volume* cVol = new Trk::Volume(new Amg::Transform3D(Amg::Translation3D(-shift,0.,0.)),
						new Trk::CombinedVolumeBounds(v1,v2,false));
            volExcl=new Trk::VolumeExcluder(cVol);
	    Trk::SubtractedPlaneSurface* subPlane=new Trk::SubtractedPlaneSurface(Trk::PlaneSurface(new Amg::Transform3D(transf[ic]*
					Amg::Translation3D(shift,0.,0.)*
					Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
					Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))), bounds), 
										  volExcl, false);
	    Trk::MaterialProperties material(thickness,cmat.X0,cmat.L0,cmat.A,cmat.Z,cmat.rho); 
	    Trk::HomogeneousLayerMaterial spacerMaterial(material,0.);
	    Trk::SubtractedPlaneLayer* layx = new Trk::SubtractedPlaneLayer(subPlane,spacerMaterial, thickness, od, 0 );
	    layers.push_back(layx) ; 
	    //Trk::SubtractedPlaneSurface* subPlaneX
	    //        = new Trk::SubtractedPlaneSurface(*subPlane,Amg::Transform3D(Amg::Translation3D(-2*shift,0.,0.)));
	    std::unique_ptr<Trk::SubtractedPlaneSurface> 
	      subPlaneX(new Trk::SubtractedPlaneSurface(*subPlane,Amg::Transform3D(Amg::Translation3D(-2*shift,0.,0.))));
	    Trk::SubtractedPlaneLayer* layxx = new Trk::SubtractedPlaneLayer(subPlaneX.get(),spacerMaterial, thickness, od, 0 );
	    layers.push_back(layxx) ;
            delete subPlane;

	    rbounds = new Trk::RectangleBounds(boxB->getXHalfLength(),box->getZHalfLength());
	    thickness = subVs[2].second.translation().mag();
	    Trk::VolumeExcluder* volEx=new Trk::VolumeExcluder(new Trk::Volume(*cVol, 
									       Amg::Transform3D(Amg::Translation3D(2*shift,0.,0.))));
	    bounds = Trk::SharedObject<const Trk::SurfaceBounds>(rbounds);
	    subPlane = new Trk::SubtractedPlaneSurface(Trk::PlaneSurface(new Amg::Transform3D(transf[ic]*
											      Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(1.,0.,0.))),
									 bounds), volEx, false);
	    material = Trk::MaterialProperties(thickness,cmat.X0,cmat.L0,cmat.A,cmat.Z,cmat.rho); 
	    spacerMaterial = Trk::HomogeneousLayerMaterial(material,0.);
	    Trk::SubtractedPlaneLayer* lay = new Trk::SubtractedPlaneLayer(subPlane,spacerMaterial, thickness, od, 0 );
            delete subPlane;
	    layers.push_back(lay) ; 
	  } else {
            delete v1; delete v2;
	  }          
	}
      } else {
	std::cout << "unresolved spacer component "<< clv->getName() << std::endl; 
      }    
    } else {
      std::cout << "unresolved spacer component "<< clv->getName() << std::endl; 
    }
  }

  std::vector<const Trk::Layer*>::iterator lIt = layers.begin();
  for ( ; lIt!=layers.end();lIt++) if ((*lIt)->thickness()<0.) lIt=layers.erase(lIt); 

  std::vector<const Trk::Layer*>* spacerLayers = new std::vector<const Trk::Layer*>(layers); 
  std::string name="Spacer";
  const Trk::TrackingVolume* spacer= new Trk::TrackingVolume(vol,
							     *m_muonMaterial,
							     spacerLayers,
							     name);         

  if (!m_resolveSpacer) {       // average into a single material layer
    std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>*> laySpacer = createLayerRepresentation(spacer);    
    delete spacer;
    laySpacer.first->setLayerType(0);
    layers.clear();
    layers.push_back(laySpacer.first); 
    std::vector<const Trk::Layer*>* spacerLays = new std::vector<const Trk::Layer*>(layers); 
    spacer= new Trk::TrackingVolume(vol,
				    *m_muonMaterial,
				    spacerLays,
				    name);         
  }

  //std::cout << "spacer processed with" << layers.size() << " layers" << std::endl;
  return spacer;
}


const Trk::TrackingVolume* Muon::MuonStationTypeBuilder::processNSW(std::vector< const Trk::Layer* > layers ) const
{
  ATH_MSG_DEBUG( name() <<" processing NSW station components "<<layers.size() );    
///////////////////////////////////////////////////////////////////////////////////////////////////

  //double tolerance = 0.0001;   

    const Trk::TrackingVolume* trVol=0;

    Amg::Transform3D transf=layers[0]->surfaceRepresentation().transform();

    // loop over layers and retrieve boundaries
    double zMin = 25000.;
    double zMax = -25000.;
    double rMin = 13000.;
    double rMed = 0.;
    double rMax = 0.;
    double hMin = 0.;
    double hMed = 0.;
    double hMax = 0.;


    for (unsigned int il=0; il<layers.size(); il++) {

      zMin = fmin( zMin, (layers[il]->surfaceRepresentation().center().z())-0.5*layers[il]->thickness());      
      zMax = fmax( zMax, (layers[il]->surfaceRepresentation().center().z())+0.5*layers[il]->thickness());      

      const Trk::TrapezoidBounds* trdBounds = dynamic_cast<const Trk::TrapezoidBounds*> (&(layers[il]->surfaceRepresentation().bounds()));

      if (trdBounds) {
        rMin = fmin( rMin, (layers[il]->surfaceRepresentation().center().perp()) - trdBounds->halflengthY() );
        rMax = fmax( rMax, (layers[il]->surfaceRepresentation().center().perp()) + trdBounds->halflengthY() );

        // hMin taken from MM, ring 0
        Identifier id(layers[il]->layerType());
        if (m_muonMgr->mmIdHelper()->is_mm(id)) {
	  //std::cout <<"MM station eta index:"<< m_muonMgr->mmIdHelper()->stationEta(id) << std::endl;
	  if ( abs(m_muonMgr->mmIdHelper()->stationEta(id))==1 ) hMin = trdBounds->minHalflengthX(); 
	  // hMed taken from MM, ring 1
	  if ( abs(m_muonMgr->mmIdHelper()->stationEta(id))==2 ) {
	    hMed = trdBounds->minHalflengthX(); 
	    rMed = layers[il]->surfaceRepresentation().center().perp()  - trdBounds->halflengthY();
	  }
	  // hMax taken from MM, ring 3
	  if (  abs(m_muonMgr->mmIdHelper()->stationEta(id))==4  ) hMax = trdBounds->maxHalflengthX();
	  //if (layId>0) std::cout <<"decode layId?"<<layId<<","<<  int(layId/1.e+05)<<","<< int(layId/1.e+04)<< std::endl; 
	}
      }
      
      //if (trdBounds) std::cout <<"loop over NSW layers:"<<il<<":"<<layers[il]->layerType()<<":"<<layers[il]->surfaceRepresentation().center().perp()<<":"<< trdBounds->minHalflengthX()<<","<<trdBounds->maxHalflengthX()<<","<<trdBounds->halflengthY()<<std::endl;
      
    }
    
    //std::cout <<"zMin,Max:"<< zMin<<","<<zMax<< std::endl;
    //std::cout <<"rMin,rMed,rMax:"<< rMin<<","<<rMed<<","<<rMax<< std::endl;
    //std::cout <<"hMin,hMed,hMax:"<< hMin<<","<<hMed<<","<<hMax<< std::endl;

    double c1 = (hMed-hMin)/(rMed-rMin);
    double c2 = (hMax-hMed)/(rMax-rMed);

    double r=0.5*(rMin+rMax);
    double z=0.5*(zMin+zMax); 
    Amg::Vector3D center(r*cos(transf.translation().phi()),r*sin(transf.translation().phi()),z);
    Amg::Transform3D* cTr = new Amg::Transform3D( Amg::Translation3D(center-transf.translation())*transf);

    if (fabs(c1-c2)>0.1) { //combined volume bounds needed but enlarge Trd instead ( otherwise layer representation not created )

      hMax = (c1>c2 ? c1:c2)*(rMax-rMin)+hMin;

    }

    //  Trk::TrapezoidVolumeBounds* trd1=new Trk::TrapezoidVolumeBounds(hMin,hMed,0.5*(rMed-rMin),0.5*fabs(zMax-zMin));
    //  Trk::TrapezoidVolumeBounds* trd2=new Trk::TrapezoidVolumeBounds(hMed,hMax,0.5*(rMax-rMed),0.5*fabs(zMax-zMin));
    //  Amg::Transform3D* transf1 = new Amg::Transform3D( Amg::Translation3D(0.,0.5*(rMed-rMax),0.));
    //  Amg::Transform3D* transf2 = new Amg::Transform3D( Amg::Translation3D(0.,0.5*(rMed-rMin),0.));
    //  Trk::Volume* ev1=new Trk::Volume(transf1,trd1);
    //  Trk::Volume* ev2=new Trk::Volume(transf2,trd2);
    //  Trk::CombinedVolumeBounds* cBounds=new Trk::CombinedVolumeBounds(ev1,ev2,false);
    //  envelope = new Trk::Volume(cTr,cBounds);
    //  
    //} else {
    
    Trk::TrapezoidVolumeBounds* trdVolBounds=new Trk::TrapezoidVolumeBounds(hMin,hMax,0.5*(rMax-rMin),0.5*fabs(zMax-zMin));
    Trk::Volume envelope(cTr,trdVolBounds);
    //}

    //std::cout <<"envelope:"<<envelope->center()<<std::endl;

    std::vector<const Trk::Layer*>* nswLayers = new std::vector<const Trk::Layer*>(layers); 
    std::string name="NSW";
    trVol= new Trk::TrackingVolume(envelope,
				   *m_muonMaterial,
				   nswLayers,
				   name);         

    ATH_MSG_DEBUG( " NSW component volume processed with" << layers.size() << " layers");
    return trVol;

}

const Trk::TrackingVolume* Muon::MuonStationTypeBuilder::processCscStation(const GeoVPhysVol* mv, std::string name) const
{
  
  // CSC stations have the particularity of displacement in Z between multilayer and the spacer - the envelope
  //   has to be derived from the component volume shape and component displacement
  bool isDiamond = false;
  double xMin=0; double xMed=0; double xMax=0; double y1=0; double y2=0; double z=0;
  //printChildren(mv);
  // find the shape and dimensions for the first component
  const GeoVPhysVol* cv = &(*(mv->getChildVol(0))); 
  const GeoLogVol* clv = cv->getLogVol();
  //Amg::Transform3D transform = Amg::CLHEPTransformToEigen(mv->getXToChildVol(0));        
  if (clv->getShape()->type()=="Shift") {
    const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (clv->getShape());
    if (shift->getOp()->type()=="Union") {
      // that would be the union making the diamond/double trapezoid shape, let's retrieve the parameters 
      isDiamond = true;
      const GeoShapeUnion* uni = dynamic_cast<const GeoShapeUnion*> (shift->getOp());
      if (uni->getOpA()->type()=="Trd") {
	const GeoTrd* trdA = dynamic_cast<const GeoTrd*> (uni->getOpA());                  
	xMin =  trdA->getYHalfLength1();
	xMed =  trdA->getYHalfLength2();
	y1   =  trdA->getZHalfLength();
	z    =  trdA->getXHalfLength1();
      }
      if (uni->getOpB()->type()=="Shift") {
	const GeoShapeShift* sh = dynamic_cast<const GeoShapeShift*> (uni->getOpB());
	const GeoTrd* trdB = dynamic_cast<const GeoTrd*> (sh->getOp());          
	if ( trdB->getYHalfLength1() != xMed ||  trdB->getXHalfLength1() != z )
	  std::cout <<"Something is wrong: dimensions of 2 trapezoids do not match"<<std::endl ;
	xMax =  trdB->getYHalfLength2();
	y2   =  trdB->getZHalfLength();
      }
    } //end Union
    if (shift->getOp()->type()=="Trd") {
      // that would be the trapezoid shape, let's retrieve the parameters 
      const GeoTrd* trd = dynamic_cast<const GeoTrd*> (shift->getOp());
      xMin =  trd->getYHalfLength1();
      xMed =  trd->getYHalfLength2();
      y1   =  trd->getZHalfLength();
      z    =  trd->getXHalfLength1();      
    } //end Trd
  } else {
    if (clv->getShape()->type()=="Trd"){
      // that would be the trapezoid shape, let's retrieve the parameters 
      const GeoTrd* trd = dynamic_cast<const GeoTrd*> (clv->getShape());
      xMin =  trd->getYHalfLength1();
      xMed =  trd->getYHalfLength2();
      y1   =  trd->getZHalfLength();
      z    =  trd->getXHalfLength1();      
    }
  }
  // then loop over all components to get total Xsize & transforms 
  std::vector<Amg::Transform3D> compTransf;
  std::vector<std::string> compName;
  std::vector<const GeoVPhysVol*> compGeoVol;
  std::vector<double> xSizes;
  double xmn = +10000.; double xmx = -10000.;
  for (unsigned int ich =0; ich< mv->getNChildVols(); ++ich) {
    const GeoVPhysVol* cv = &(*(mv->getChildVol(ich))); 
    const GeoLogVol* clv = cv->getLogVol();
    Amg::Transform3D transform = Amg::CLHEPTransformToEigen(mv->getXToChildVol(ich));        
    compTransf.push_back(transform);
    compName.push_back(clv->getName());
    compGeoVol.push_back(cv);
    if (clv->getShape()->type()=="Shift") {
      const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (clv->getShape());
      if (shift->getOp()->type()=="Union") {
	// that would be the union making the diamond/double trapezoid shape, let's retrieve the parameters 
	const GeoShapeUnion* uni = dynamic_cast<const GeoShapeUnion*> (shift->getOp());
	//std::cout << uni->getOpA()->type()<<","<< uni->getOpB()->type() << std::endl;
	if (uni->getOpA()->type()=="Trd") {
	  const GeoTrd* trdA = dynamic_cast<const GeoTrd*> (uni->getOpA());
	  double xSize  =  trdA->getXHalfLength1();
	  if (xSizes.size()>0) xSizes.push_back((fabs(transform.translation()[0]-compTransf[ich-1].translation()[0])
						 -xSizes.back()));
	  else xSizes.push_back(xSize);
	  double xpos = (transform*Amg::CLHEPTransformToEigen(shift->getX())).translation()[0];
	  if ( xpos-xSize < xmn ) xmn = xpos-xSizes.back();
	  if ( xpos+xSize > xmx ) xmx = xpos+xSizes.back();
	}
      } //end Union
    } // end Shift
    if (clv->getShape()->type()=="Trd") {
      const GeoTrd* trd = dynamic_cast<const GeoTrd*> (clv->getShape());
      double xSize  =  trd->getXHalfLength1();
      if (xSizes.size()>0) xSizes.push_back(fabs(transform.translation()[0]-compTransf[ich-1].translation()[0])
					    -xSizes.back());
      else xSizes.push_back(xSize); 
      double xpos = transform.translation()[0];
      if ( xpos-xSize < xmn ) xmn = xpos-xSizes.back();
      if ( xpos+xSize > xmx ) xmx = xpos+xSizes.back();
    } // end Trd
      // printChildren(cv);  
  } 
  // this should be enough to build station envelope
  double xTotal = 0;
  for (unsigned int i=0;i<xSizes.size(); i++) xTotal += xSizes[i];
  double xShift = 0.5*(xmx+xmn);
  double zShift = 0;
  zShift = fabs(((compTransf.front()).translation())[2])+  fabs(((compTransf.back()).translation())[2]);
  //std::cout << "z displacement:" << zShift << std::endl;
  // calculate displacement with respect to GeoModel station volume
  // one way or the other, the station envelope is double trapezoid
  Trk::Volume* envelope;
  double envXMed = xMed;
  double envY1   = y1; 
  double envY2   = y2;
  std::vector<float> volSteps; 
  volSteps.push_back(-xTotal+xShift);
  std::vector<const Trk::TrackingVolume*> components;
  if ( !isDiamond ) {
    Trk::TrapezoidVolumeBounds* cscBounds=0;
    Trk::TrapezoidVolumeBounds* compBounds=0;
    xMax = xMed;
    y2 = 0.5*zShift;
    //std::cout << "envelope dimensions:"<<xMin<<","<<xMax<<","<<y1<<","<<xTotal<<std::endl;
    cscBounds = new Trk::TrapezoidVolumeBounds(xMin,xMax,y1,xTotal); 
    // xy -> yz  rotation
    // the center of Volume is shifted by y1-y2 in y
    Amg::Transform3D* cTr = new Amg::Transform3D(  Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						   Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))*
						   Amg::Translation3D(0.,0.,xShift) );
    envelope = new Trk::Volume(cTr,cscBounds);
    // components
    double xCurr = -xTotal;
    for (unsigned int ic = 0; ic< xSizes.size(); ic++) {
      // component volumes follow the envelope dimension
      xCurr += xSizes[ic];
      Amg::Transform3D* compTr = new Amg::Transform3D(  Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
							Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))*
							Amg::Translation3D(0.,0.,xCurr+xShift));
      compBounds = new Trk::TrapezoidVolumeBounds(xMin,xMax,y1,xSizes[ic]);
      const Trk::LayerArray* cscLayerArray = processCSCTrdComponent(compGeoVol[ic],compBounds,compTr); 
      Trk::Volume* compVol = new Trk::Volume(compTr,compBounds);
      const Trk::TrackingVolume* compTV = new Trk::TrackingVolume( *compVol,
								   *m_muonMaterial,
								   cscLayerArray,0,
								   compName[ic]);                    
      delete compVol;
      components.push_back(compTV);
      xCurr += xSizes[ic];
      volSteps.push_back(xCurr+xShift);  
    } // end components      
  } else {
    Trk::DoubleTrapezoidVolumeBounds* cscBounds=0;
    Trk::DoubleTrapezoidVolumeBounds* compBounds=0;
    if (xMed!=xMin && xMed!=xMax) {
      envXMed += zShift/(y1/(xMed-xMin)+y2/(xMed-xMax));
      envY1 = y1*(envXMed-xMin)/(xMed-xMin);
      envY2 = y2*(envXMed-xMax)/(xMed-xMax);
    } 
    //std::cout << "envelope dimensions:"<<xMin<<","<<envXMed<<","<<xMax<<","<<envY1<<","<<envY2<<","<<z+0.5*zShift<<std::endl;
    cscBounds = new Trk::DoubleTrapezoidVolumeBounds(xMin,envXMed,xMax,envY1,envY2,xTotal); 
    // xy -> yz  rotation
    // the center of DoubleTrapezoidVolume is shifted by (envY1-envY2) in y
    Amg::Transform3D* cTr = new Amg::Transform3D( Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						  Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))*
						  Amg::Translation3D(0.,envY1-envY2,0.)*Amg::Translation3D(0.,0.,xShift));
    envelope = new Trk::Volume(cTr,cscBounds);
    // components
    double xCurr = -xTotal;
    for (unsigned int ic = 0; ic< xSizes.size(); ic++) {
      // component volumes follow the envelope dimension
      xCurr += xSizes[ic];
      Amg::Transform3D* compTr = new Amg::Transform3D( Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						       Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))*
						       Amg::Translation3D(0.,envY1-envY2,0.)*Amg::Translation3D(0.,0.,xCurr+xShift));
      compBounds = new Trk::DoubleTrapezoidVolumeBounds(xMin,envXMed,xMax,envY1,envY2,xSizes[ic]); 
      const Trk::LayerArray* cscLayerArray = processCSCDiamondComponent(compGeoVol[ic],compBounds,compTr); 
      Trk::Volume* compVol = new Trk::Volume(compTr,compBounds);
      const Trk::TrackingVolume* compTV = new Trk::TrackingVolume( *compVol,
								   *m_muonMaterial,
								   cscLayerArray,0,
								   compName[ic]);                    
      delete compVol;
      components.push_back(compTV);
      xCurr += xSizes[ic];
      volSteps.push_back(xCurr+xShift);  
    } // end components
  }
    
  // convert component volumes into array 
  const Trk::BinnedArray<Trk::TrackingVolume>* compArray = 0; 
  //std::cout << "verifying CSC subvolume array:"<< -xTotal<< std::endl;
  //for (unsigned int i=0;i<volSteps->size();i++) std::cout << i<<","<<(*volSteps)[i]<< std::endl;
  if (components.size() && isDiamond) {
    //Trk::BinUtility1DX* binUtil = new Trk::BinUtility1DX(-xTotal+xShift, volSteps);
    if (m_trackingVolumeArrayCreator) {
      Trk::BinUtility* binUtil = new Trk::BinUtility( volSteps, Trk::BinningOption::open, Trk::BinningValue::binX);
      compArray = m_trackingVolumeArrayCreator->doubleTrapezoidVolumesArrayNav( components, binUtil, false);
    }
  }
  if (components.size() && !isDiamond) {
    //Trk::BinUtility1DX* binUtil = new Trk::BinUtility1DX(-xTotal+xShift, volSteps);
    if (m_trackingVolumeArrayCreator) {
      Trk::BinUtility* binUtil = new Trk::BinUtility( volSteps, Trk::BinningOption::open, Trk::BinningValue::binX);
      compArray = m_trackingVolumeArrayCreator->trapezoidVolumesArrayNav( components, binUtil, false);
    }
  }
  // ready to build the station prototype
  const Trk::TrackingVolume* csc_station = new Trk::TrackingVolume( *envelope,
								    *m_muonMaterial,
								    0,compArray,
								    name);                      
  delete envelope;
  return csc_station;    
}

std::vector<const Trk::TrackingVolume*> Muon::MuonStationTypeBuilder::processTgcStation(const GeoVPhysVol* mv) const
{
 // TGC stations 
  std::vector<const Trk::TrackingVolume*> tgc_stations;
 //  printChildren(mv);
  Trk::TrapezoidVolumeBounds* tgcBounds;
  Trk::Volume* envelope;
  for (unsigned int ich =0; ich< mv->getNChildVols(); ++ich) {
     const GeoVPhysVol* cv = &(*(mv->getChildVol(ich))); 
     const GeoLogVol* clv = cv->getLogVol();
     std::string tgc_name = clv->getName();
     //std::cout << "tgc name:" << tgc_name << std::endl; 
     Amg::Transform3D transform = Amg::CLHEPTransformToEigen(mv->getXToChildVol(ich));        
     //std::cout << "TGC component:"<<ich<<":" << clv->getName() <<", made of "<<clv->getMaterial()->getName()<<","<<clv->getShape()->type()<<","<<transform.translation()<<std::endl;
     const GeoShape* baseShape = clv->getShape();
     if (baseShape->type()=="Subtraction") {
       const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction*> (baseShape);
       if (sub) baseShape = sub->getOpA();
     }
     
     if (baseShape->type()=="Trd") {
       const GeoTrd* trd = dynamic_cast<const GeoTrd*> (baseShape);
       double x1 = trd->getXHalfLength1();
       double y1 = trd->getYHalfLength1();
       double y2 = trd->getYHalfLength2();
       double z = trd->getZHalfLength();
       //std::cout << "dimensions:"<<x1<<","<<x2<<","<<y1<<","<< y2 << ","<< z << std::endl;
       // define envelope
       tgcBounds = new Trk::TrapezoidVolumeBounds(y1,y2,z,x1); 
       // xy -> yz  rotation
       Amg::Transform3D* tTr = new Amg::Transform3D( transform *  Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						     Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.)));
       envelope = new Trk::Volume(tTr,tgcBounds);
       const Trk::LayerArray* tgcLayerArray = processTGCComponent(cv,tgcBounds, tTr); 
       // ready to build the station prototype
       const Trk::TrackingVolume* tgc_station = new Trk::TrackingVolume( *envelope,
                                                                        *m_muonMaterial,
									tgcLayerArray,0,
                                                                        tgc_name);                    

       delete envelope;
       if (tgc_station) tgc_stations.push_back(tgc_station);       
      
     } else {
       std::cout << "TGC component not trapezoid ?" << std::endl;
     }
  }
 return tgc_stations;    

}

void Muon::MuonStationTypeBuilder::printChildren(const GeoVPhysVol* pv) const
{
  // subcomponents
  unsigned int nc = pv->getNChildVols();
  for (unsigned int ic=0; ic<nc; ic++) {
    Amg::Transform3D transf = Amg::CLHEPTransformToEigen(pv->getXToChildVol(ic));
 
    //
    /*
    std::cout << " dumping transform to subcomponent" << std::endl;
    std::cout << transf[0][0]<<"," <<transf[0][1]<<"," <<transf[0][2]<<","<<transf[0][3] << std::endl;
    std::cout << transf[1][0]<<"," <<transf[1][1]<<"," <<transf[1][2]<<","<<transf[1][3] << std::endl;
    std::cout << transf[2][0]<<"," <<transf[2][1]<<"," <<transf[2][2]<<","<<transf[2][3] << std::endl;
    */
    //
    const GeoVPhysVol* cv = &(*(pv->getChildVol(ic)));
    const GeoLogVol* clv = cv->getLogVol();
    std::cout << "  ";
    std::cout << "subcomponent:"<<ic<<":"<<clv->getName()<<", made of"<<clv->getMaterial()->getName()<<","<<clv->getShape()->type()<< ","<< transf.translation()<<std::endl;
	 
          if ( clv->getShape()->type()=="Trd") {
	      const GeoTrd* trd = dynamic_cast<const GeoTrd*> (clv->getShape());
	      //
	      std::cout << "dimensions:"<< trd->getXHalfLength1() <<","
                                        << trd->getXHalfLength2() <<","  
                                        << trd->getYHalfLength1() <<","  
                                        << trd->getYHalfLength2() <<","  
			                << trd->getZHalfLength() <<std::endl; 
	      //
          } 
          if ( clv->getShape()->type()=="Box") {
	      const GeoBox* box = dynamic_cast<const GeoBox*> (clv->getShape());
	      //
	        std::cout << "dimensions:"<< box->getXHalfLength() <<","
                                        << box->getYHalfLength() <<","  
			                << box->getZHalfLength() <<std::endl; 
	      //
          } 

	  printChildren(cv);
    }  
   

}

double Muon::MuonStationTypeBuilder::get_x_size(const GeoVPhysVol* pv) const
{
  double xlow = 0;
  double xup  = 0; 
  // subcomponents
  unsigned int nc = pv->getNChildVols();
  if (nc==0) {
    const GeoLogVol* clv = pv->getLogVol();
    double xh=0;
    std::string type =  clv->getShape()->type();
    if (type=="Trd") {
       const GeoTrd* trd = dynamic_cast<const GeoTrd*> (clv->getShape());
       xh = fmax(trd->getXHalfLength1(),trd->getXHalfLength2()); 
    } 
    if (type=="Box") {
       const GeoBox* box = dynamic_cast<const GeoBox*> (clv->getShape());
       xh = box->getXHalfLength();
    } 
    if (type=="Tube") {
       const GeoTube* tube = dynamic_cast<const GeoTube*> (clv->getShape());
       xh = tube->getRMax();
    } 
    if (type=="Subtraction") {
       xh = decodeX(clv->getShape());
    }
    
    return xh; 

  }

  for (unsigned int ic=0; ic<nc; ic++) {
    Amg::Transform3D transf = Amg::CLHEPTransformToEigen(pv->getXToChildVol(ic));
    const GeoVPhysVol* cv = &(*(pv->getChildVol(ic)));
    const GeoLogVol* clv = cv->getLogVol();
    double xh=0;
    std::string type =  clv->getShape()->type();
    if (type=="Trd") {
       const GeoTrd* trd = dynamic_cast<const GeoTrd*> (clv->getShape());
       xh = fmax(trd->getXHalfLength1(),trd->getXHalfLength2()); 
    } 
    if (type=="Box") {
       const GeoBox* box = dynamic_cast<const GeoBox*> (clv->getShape());
       xh = box->getXHalfLength();
    } 
    if (type=="Tube") {
       const GeoTube* tube = dynamic_cast<const GeoTube*> (clv->getShape());
       xh = tube->getRMax();
    } 
    if (type=="Subtraction") {
       xh = decodeX(clv->getShape());
    }
 
    xlow = fmin(xlow,(transf.translation())[0]-xh);
    xup  = fmax(xup ,(transf.translation())[0]+xh);
  }  
  // std::cout << "x size:" << xlow <<","<< xup << std::endl;
   
  return fmax( -xlow, xup ); 
}

Trk::MaterialProperties Muon::MuonStationTypeBuilder::getAveragedLayerMaterial( const GeoVPhysVol* pv, double volume, double thickness) const
{  

  ATH_MSG_DEBUG( name() << "::getAveragedLayerMaterial:processing "); 
  // loop through the whole hierarchy; collect material
  Trk::MaterialProperties sumMat(0.,10.e10,10.e10,13.,26.,0.);
  collectMaterial( pv, sumMat, volume/thickness);
  ATH_MSG_VERBOSE(name() << " combined material thickness: "<< sumMat.thickness() ); 
  ATH_MSG_VERBOSE(name() << " actual layer thickness: "<< thickness ); 

  // scale material properties to the actual layer thickness
  if (sumMat.thickness() != thickness && sumMat.thickness()>0. ) { 
    double sf = thickness/sumMat.thickness();
    sumMat.material().X0 /=sf; 
    sumMat.material().L0 /=sf; 
    sumMat.material().rho *=sf; 
    ATH_MSG_VERBOSE( "averaged material scale :"<< sf <<" sumMat.material().X0() "<<sumMat.material().X0 << " sumMat.material().L0 " << sumMat.material().L0 << " sumMat.material().rho " << sumMat.material().rho  << " sumMat.material().x0() " << sumMat.material().x0() );
    ATH_MSG_VERBOSE( "averaged material:d,x0,dInX0:"<< sumMat.thickness()<<","<<sumMat.material().x0() );
    return sumMat;
  }  
  return sumMat;
}

void Muon::MuonStationTypeBuilder::collectMaterial(const GeoVPhysVol* pv, Trk::MaterialProperties& layMat, double sf) const
{
  // sf is surface of the new layer used to calculate the average 'thickness' of components
  // number of child volumes
  unsigned int nc = pv->getNChildVols();
  // add current volume 
  const GeoLogVol* lv = pv->getLogVol(); 
  //std::cout << "collect material:component:"<<lv->getName()<<", made of"<<lv->getMaterial()->getName()<<","<<lv->getShape()->type()<<std::endl;

  std::string nm = lv->getName();
  if (nm=="") nm="Spacer"; 
     
  if ( lv->getMaterial()->getName() != "Air" && nm.substr(0,1)!="T") {

    //std::cout <<"current material:"<< lv->getMaterial()->getName()<< std::endl;
    // get material properties from GeoModel
    Trk::Material newMP= m_materialConverter->convert( lv->getMaterial() ); 
    // current volume
    double vol = getVolume(lv->getShape());
    // subtract children volumes
    for (unsigned int ic=0; ic<nc; ic++) {
      const GeoVPhysVol* cv = &(*(pv->getChildVol(ic)));
      if ( getVolume(cv->getLogVol()->getShape()) > vol ) {
	//std::cout << "WARNING:collect material : child volume bigger than mother volume" << std::endl; 
      } else {
	vol = vol - getVolume(cv->getLogVol()->getShape()); 
      }
    }
    double d = vol / sf;
    layMat.addMaterial(newMP,d/newMP.x0()); 

  } 
  // subcomponents
  // skip children volume if we deal with G10 ( not correctly described )
  //if ( lv->getName() != "G10" ) { 
  for (unsigned int ic=0; ic<nc; ic++) {
    const GeoVPhysVol* cv = &(*(pv->getChildVol(ic)));
    collectMaterial( cv, layMat, sf);
  }
}

double Muon::MuonStationTypeBuilder::getVolume( const GeoShape* shape) const {
  //
  double volume = 0.;
  
  if (shape->type()=="Shift" ) {
    const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (shape);
    volume = getVolume(shift->getOp());
  } else if (shape->type()=="Subtraction" ) {
    const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction* > (shape);
    double volA = getVolume(sub->getOpA());
    double volB = getVolume(sub->getOpB());
    // protection against subtraction of large volumes 
    if (volA > volB){
      volume = volA - volB;
    } else {
      volume = volA;
    }
  } else if (shape->type()=="Union") {  
    const GeoShapeUnion* uni = dynamic_cast<const GeoShapeUnion* > (shape);
    double volA = getVolume(uni->getOpA());
    double volB = getVolume(uni->getOpB());
    volume = volA+volB; 
  } else {
    volume = shape->volume();
  }
  return volume;
} 

const Trk::LayerArray* Muon::MuonStationTypeBuilder::processCSCTrdComponent(const GeoVPhysVol*& pv, Trk::TrapezoidVolumeBounds*& compBounds, Amg::Transform3D*& transf) const {

  // tolerance
  // double tol = 0.001;
  std::string name = pv->getLogVol()->getName();
  //std::cout << "processing CSC component, number of children volumes:"<< name << "," << pv->getNChildVols() <<std::endl; 
  //printChildren(pv);
  std::vector<const Trk::PlaneLayer*> layers;
  std::vector<double> x_array;
  std::vector<Trk::MaterialProperties> x_mat;
  std::vector<double> x_thickness;
  std::vector<int> x_active;
  double currX = -100000;
  // while waiting for better suggestion, define a single material layer
  Trk::MaterialProperties matCSC(0.,10.e10,10.e10,13.,26.,0.);
  double thickness =2* compBounds->halflengthZ();
  double minX = compBounds->minHalflengthX();
  double maxX = compBounds->maxHalflengthX();
  double halfY = compBounds->halflengthY();
  double halfZ = compBounds->halflengthZ();
  if (name.substr( name.size()-5,5 ) == "CSC01" ) {
    if (!m_matCSC01 ) { 
      double vol = (minX + maxX)*2*halfY*thickness;
      m_matCSC01 = new Trk::MaterialProperties(getAveragedLayerMaterial(pv,vol,thickness)); 
    }
    matCSC = Trk::MaterialProperties(*m_matCSC01); 
    // retrieve number of gas gaps and their position -> turn them into active layers
    // step 1 level below
    const GeoVPhysVol* cv1 = &(*(pv->getChildVol(0)));
    for (unsigned int ic=0; ic < cv1->getNChildVols(); ic++) {
      Amg::Transform3D transfc = Amg::CLHEPTransformToEigen(cv1->getXToChildVol(ic));
      const GeoVPhysVol* cv = &(*(cv1->getChildVol(ic)));
      const GeoLogVol* clv = cv->getLogVol();
      if ( clv->getName() == "CscArCO2" ) {
        double xl = transfc.translation()[0];
        if (x_array.size() == 0 || xl >= x_array.back()) { 
          x_array.push_back(xl);
        } else {
          unsigned int ix = 0; 
          while ( ix < x_array.size() && x_array[ix] < xl  ) {ix++;}
          x_array.insert(x_array.begin()+ix,xl);
        } 
      }
    } 
    if (x_array.size()==0) {
      x_array.push_back(0.);
      x_mat.push_back(matCSC);
      x_thickness.push_back(thickness );
      x_active.push_back(1);
    } else if (x_array.size()==1) {
      double xthick = 2*fmin(x_array[0]+halfZ,halfZ-x_array[0]);
      double scale = xthick/thickness;
      Trk::MaterialProperties xmatCSC(xthick,scale*matCSC.x0(),scale*matCSC.l0(),
				      matCSC.averageA(),matCSC.averageZ(),matCSC.averageRho()/scale); 
      x_mat.push_back(xmatCSC);
      x_thickness.push_back(xthick);
      x_active.push_back(1); 
    } else {
      double currX = -halfZ;
      for (unsigned int il=0; il < x_array.size(); il++) {
        double xthick;
	if (il<x_array.size()-1) {
          xthick = 2*fmin(x_array[il]-currX,0.5*(x_array[il+1]-x_array[il])) ;
	} else {
          xthick = 2*fmin(x_array[il]-currX,halfZ-x_array[il]);
        }
	x_thickness.push_back(xthick);
	Trk::MaterialProperties xmatCSC(matCSC); 
        x_mat.push_back(xmatCSC);
        currX = x_array[il]+0.5*x_thickness.back();
        x_active.push_back(1);
      }
    }
  } 
  if (name == "CSCspacer" ) {
    if (!m_matCSCspacer1 ) { 
      double vol = (minX + maxX)*2*halfY*thickness;
      m_matCSCspacer1 = new Trk::MaterialProperties(getAveragedLayerMaterial(pv,vol,thickness)); 
    }
    x_array.push_back(0.);
    x_mat.push_back(*m_matCSCspacer1);
    x_thickness.push_back(thickness );
    x_active.push_back(0);
  } 
  // create layers
  const Trk::PlaneLayer* layer;
  Trk::OverlapDescriptor* od=0;
  Trk::TrapezoidBounds* tbounds= new Trk::TrapezoidBounds(minX,maxX,halfY); 
  Trk::SharedObject<const Trk::SurfaceBounds> bounds(tbounds);
  for (unsigned int iloop=0; iloop<x_array.size(); iloop++) {
    Amg::Transform3D* cTr = new Amg::Transform3D( (*transf) * Amg::Translation3D(0.,0.,x_array[iloop])  ); // this won't work for multiple layers !!! //
    Trk::HomogeneousLayerMaterial cscMaterial(x_mat[iloop],0.);  
    layer = new Trk::PlaneLayer(cTr,
                                bounds,
                                cscMaterial,
                                x_thickness[iloop],
                                od );
    layers.push_back(layer);
    // make preliminary identification of active layers
    layer->setLayerType(x_active[iloop]);
  }

  // create the BinnedArray
  std::vector<Trk::SharedObject<const Trk::Layer> > layerOrder;
  std::vector<float> binSteps;
  double xShift = transf->translation()[0];
  float lowX = - compBounds->halflengthZ()+xShift ;
  binSteps.push_back(lowX);

  if (layers.size()) {
    currX = lowX-xShift; 
    for (unsigned int i=0;i<layers.size()-1;i++) { 
      const Amg::Transform3D ltransf(Amg::Translation3D(x_array[i],0.,0.));
      layerOrder.push_back(Trk::SharedObject<const Trk::Layer>(layers[i]));
      currX = ltransf.translation()[0]+0.5*layers[i]->thickness();     
      binSteps.push_back(currX+xShift);
    }
    const Amg::Transform3D ltransf(Amg::Translation3D(x_array.back(),0.,0.));
    layerOrder.push_back(Trk::SharedObject<const Trk::Layer>(layers.back()));
    binSteps.push_back(compBounds->halflengthZ()+xShift);
  }
  //Trk::BinUtility* binUtility = new Trk::BinUtility1DX( lowX, new std::vector<double>(binSteps));
  Trk::BinUtility* binUtility = new Trk::BinUtility( binSteps, Trk::BinningOption::open, Trk::BinningValue::binX);
  Trk::LayerArray* cscLayerArray = 0;
  cscLayerArray = new Trk::NavBinnedArray1D<Trk::Layer>(layerOrder, binUtility, new Amg::Transform3D(Trk::s_idTransform));     
  
  return cscLayerArray;

} 

const Trk::LayerArray* Muon::MuonStationTypeBuilder::processCSCDiamondComponent(const GeoVPhysVol*& pv, Trk::DoubleTrapezoidVolumeBounds*& compBounds, Amg::Transform3D*& transf) const {

  // tolerance
  // double tol = 0.001;
  std::string name = pv->getLogVol()->getName();
  //std::cout << "processing CSC component, number of children volumes:"<< name << "," << pv->getNChildVols() <<std::endl; 
  //printChildren(pv);
  std::vector<const Trk::PlaneLayer*> layers;
  std::vector<double> x_array;
  std::vector<Trk::MaterialProperties> x_mat;
  std::vector<double> x_thickness;
  std::vector<int> x_active;
  double currX = -100000;
  // while waiting for better suggestion, define a single material layer
  Trk::MaterialProperties matCSC(0.,10e8,10e8,13.,26.,0.);
  double thickness = 2*compBounds->halflengthZ();
  double minX = compBounds->minHalflengthX();
  double medX = compBounds->medHalflengthX();
  double maxX = compBounds->maxHalflengthX();
  double halfY1 = compBounds->halflengthY1();
  double halfY2 = compBounds->halflengthY2();
  double halfZ = compBounds->halflengthZ();
  if (name.substr( name.size()-5,5 ) == "CSC02" ) {
    if (!m_matCSC02 ) { 
      double vol = ( (minX + medX)*2*halfY1+(medX+maxX)*2*halfY2 ) * thickness;
      m_matCSC02 = new Trk::MaterialProperties(getAveragedLayerMaterial(pv,vol,thickness)); 
    }
    matCSC = Trk::MaterialProperties(*m_matCSC02); 
    // retrieve number of gas gaps and their position -> turn them into active layers
    // step 1 level below
    const GeoVPhysVol* cv1 = &(*(pv->getChildVol(0)));
    for (unsigned int ic=0; ic < cv1->getNChildVols(); ic++) {
      Amg::Transform3D transfc = Amg::CLHEPTransformToEigen(cv1->getXToChildVol(ic));
      const GeoVPhysVol* cv = &(*(cv1->getChildVol(ic)));
      const GeoLogVol* clv = cv->getLogVol();
      if ( clv->getName() == "CscArCO2" ) {
        double xl = transfc.translation()[0];
        if (x_array.size() == 0 || xl >= x_array.back()) { 
          x_array.push_back(xl);
        } else {
          unsigned int ix = 0; 
          while ( ix < x_array.size() && x_array[ix] < xl  ) {ix++;}
          x_array.insert(x_array.begin()+ix,xl);
        } 
      }
    } 
    //
    if (x_array.size()==0) {
      x_array.push_back(0.);
      x_mat.push_back(matCSC);
      x_thickness.push_back(thickness );
      x_active.push_back(1);
    } else if (x_array.size()==1) {
      x_mat.push_back(matCSC);
      x_thickness.push_back(2*fmin(x_array[0]+halfZ,halfZ-x_array[0]));
      x_active.push_back(1);
    } else {
      double currX = -halfZ;
      for (unsigned int il=0; il < x_array.size(); il++) {
        double xthick = 0.;        
	if (il<x_array.size()-1) {
          xthick = 2*fmin(x_array[il]-currX,0.5*(x_array[il+1]-x_array[il])); 
	  x_thickness.push_back(xthick);
	} else {
          xthick = 2*fmin(x_array[il]-currX,halfZ-x_array[il]);
	  x_thickness.push_back(xthick);
        }
        x_mat.push_back(matCSC);
        currX = x_array[il]+0.5*x_thickness.back();
        x_active.push_back(1); 
      }
    }
  } 
  if (name == "CSCspacer" ) {
    if (!m_matCSCspacer2 ) { 
      double vol = ( (minX + medX)*2*halfY1+(medX+maxX)*2*halfY2 ) * thickness;
      m_matCSCspacer2 = new Trk::MaterialProperties(getAveragedLayerMaterial(pv,vol,thickness)); 
    }
    matCSC = Trk::MaterialProperties(*m_matCSCspacer2); 
    x_array.push_back(0.);
    x_mat.push_back(matCSC);
    x_thickness.push_back(thickness );
    x_active.push_back(0);
  } 
  // create layers
  const Trk::PlaneLayer* layer;
  Trk::OverlapDescriptor* od=0;
  Trk::DiamondBounds* dbounds= new Trk::DiamondBounds(minX,medX,maxX,halfY1,halfY2); ;
  Trk::SharedObject<const Trk::SurfaceBounds> bounds(dbounds);
  for (unsigned int iloop=0; iloop<x_array.size(); iloop++) {
    Amg::Transform3D* cTr = new Amg::Transform3D( (*transf)*Amg::Translation3D(0.,0.,x_array[iloop]) ); // this won't work for multiple layers !!! //
    Trk::HomogeneousLayerMaterial cscMaterial(x_mat[iloop],0.);  
    layer = new Trk::PlaneLayer(cTr,
                                bounds,
                                cscMaterial,
                                x_thickness[iloop],
                                od );
    layers.push_back(layer);
    // make preliminary identification of active layers
    layer->setLayerType(x_active[iloop]);
  }
   
  // create the BinnedArray
  std::vector<Trk::SharedObject<const Trk::Layer> > layerOrder;
  std::vector<float> binSteps;
  double xShift = transf->translation()[0];
  double lowX = - compBounds->halflengthZ()+xShift ;
  binSteps.push_back(lowX);
  
  if (layers.size()) {
    currX = lowX; 
    for (unsigned int i=0;i<layers.size()-1;i++) { 
      const Amg::Transform3D ltransf(Amg::Translation3D(x_array[i],0.,0.));
      layerOrder.push_back(Trk::SharedObject<const Trk::Layer>(layers[i]));
      currX = ltransf.translation()[0]+0.5*layers[i]->thickness()+xShift;     
      binSteps.push_back(currX);
    }
    const Amg::Transform3D ltransf(Amg::Translation3D(x_array.back(),0.,0.));
    layerOrder.push_back(Trk::SharedObject<const Trk::Layer>(layers.back()));
    binSteps.push_back(compBounds->halflengthZ()+xShift);
  }
  //Trk::BinUtility* binUtility = new Trk::BinUtility1DX( lowX, new std::vector<double>(binSteps));
  Trk::BinUtility* binUtility = new Trk::BinUtility( binSteps, Trk::BinningOption::open, Trk::BinningValue::binX);
  Trk::LayerArray* cscLayerArray = 0;
  cscLayerArray = new Trk::NavBinnedArray1D<Trk::Layer>(layerOrder, binUtility, new Amg::Transform3D(Trk::s_idTransform));     
  
  return cscLayerArray;
} 

const Trk::LayerArray* Muon::MuonStationTypeBuilder::processTGCComponent(const GeoVPhysVol*& pv, Trk::TrapezoidVolumeBounds*& tgcBounds, Amg::Transform3D*& transf) const {

  // tolerance
  double tol = 0.001;
  std::string name = pv->getLogVol()->getName();
  //std::cout << "processing TGC component, number of children volumes:"<< pv->getLogVol()->getName() << "," << pv->getNChildVols() <<std::endl; 
  //printChildren(pv);
  std::vector<const Trk::PlaneLayer*> layers;
  std::vector<double> x_array;
  std::vector<Trk::MaterialProperties> x_mat;
  std::vector<double> x_thickness;
  double currX = -100000;
  // while waiting for better suggestion, define a single material layer
  Trk::MaterialProperties matTGC(0.,10e8,10e8,13.,26.,0.);
  double minX = tgcBounds->minHalflengthX();
  double maxX = tgcBounds->maxHalflengthX();
  double halfY = tgcBounds->halflengthY();
  double halfZ = tgcBounds->halflengthZ();
  double thickness =2*halfZ;
  //std::cout << "tgc bounds half y:" << tgcBounds->halflengthY() << std::endl; 
  if ( fabs( tgcBounds->halflengthZ() - 35.00) < tol ) {
    if (!m_matTGC01 ) { 
      double vol = (minX + maxX)*2*halfY*thickness;
      m_matTGC01 = new Trk::MaterialProperties(getAveragedLayerMaterial(pv,vol,thickness)); 
    }
    matTGC = Trk::MaterialProperties(*m_matTGC01); 
  } else if ( fabs( tgcBounds->halflengthZ() - 21.85) < tol ) {
    if ( !m_matTGC06 ) { 
      double vol = (minX + maxX)*2*halfY*thickness;
      m_matTGC06 = new Trk::MaterialProperties(getAveragedLayerMaterial(pv,vol,thickness)); 
    }
    matTGC = Trk::MaterialProperties(*m_matTGC06); 
  } else {
    std::cout << "unknown TGC material:" << tgcBounds->halflengthZ()  << std::endl;
  }

  for (unsigned int ic=0; ic < pv->getNChildVols(); ic++) {
    Amg::Transform3D transfc = Amg::CLHEPTransformToEigen(pv->getXToChildVol(ic));
    const GeoVPhysVol* cv = &(*(pv->getChildVol(ic)));
    const GeoLogVol* clv = cv->getLogVol();
    if ( clv->getName() == "muo::TGCGas" ) {
      double xl = transfc.translation()[0];
      if (x_array.size() == 0 || xl >= x_array.back()) { 
	x_array.push_back(xl);
      } else {
	unsigned int ix = 0; 
	while ( ix < x_array.size() && x_array[ix] < xl  ) {ix++;}
	x_array.insert(x_array.begin()+ix,xl);
      } 
    }
  } 
  //
  // std::cout << " TGC multilayer has " << x_array.size() << " active layers" << std::endl; 
  //
  double activeThick=0.;
  if (x_array.size()==0) {
    x_array.push_back(0.);
    x_thickness.push_back(thickness );
    activeThick = thickness;
  } else if (x_array.size()==1) {
    x_thickness.push_back(2*fmin(x_array[0]+halfZ,halfZ-x_array[0]));
    activeThick += x_thickness.back();
  } else {
    double currX = -halfZ;
    for (unsigned int il=0; il < x_array.size(); il++) {
      if (il<x_array.size()-1) {
	x_thickness.push_back(2*fmin(x_array[il]-currX,0.5*(x_array[il+1]-x_array[il])));
      } else {
	x_thickness.push_back(2*fmin(x_array[il]-currX,halfZ-x_array[il]));
      }
      currX = x_array[il]+0.5*x_thickness.back();
      activeThick += x_thickness.back();
    }
  }
  // rescale material to match the combined thickness of active layers
  //std::cout << "thickness:active,env:" << activeThick <<"," << thickness << std::endl;
  double scale = activeThick/thickness;
  matTGC = Trk::MaterialProperties(activeThick,scale*matTGC.x0(),scale*matTGC.l0(),
				   matTGC.averageA(),matTGC.averageZ(),matTGC.averageRho()/scale);
  // create layers
  const Trk::PlaneLayer* layer;
  Trk::OverlapDescriptor* od=0;
  Trk::TrapezoidBounds* tbounds= new Trk::TrapezoidBounds(minX,maxX,halfY); ;
  Trk::SharedObject<const Trk::SurfaceBounds> bounds(tbounds);
  for (unsigned int iloop=0; iloop<x_array.size(); iloop++) {
    Amg::Transform3D* cTr = new Amg::Transform3D( Amg::Translation3D(x_array[iloop],0.,0.)*(*transf) ); // this won't work for multiple layers !!! //
    Trk::HomogeneousLayerMaterial tgcMaterial(matTGC,0.);  
    layer = new Trk::PlaneLayer(cTr,
                                bounds,
                                tgcMaterial,
                                x_thickness[iloop],
                                od );
    layers.push_back(layer);
    // make preliminary identification of active layers
    layer->setLayerType(1);
    //std::cout << "TGC layer built ok"<<std::endl;
  }
  // create the BinnedArray
  //std::cout << "number of Tgc layers:"<<layers.size()<<std::endl;
  std::vector<Trk::SharedObject<const Trk::Layer> > layerOrder;
  std::vector<float> binSteps;
  // 
  float xShift = transf->translation()[0]; 
  float lowX = - halfZ+xShift;
  binSteps.push_back(lowX);
  if (layers.size()) {
     //lowX = layers[0]->transform().translation()[0]-0.5*layers[0]->thickness();
     currX = lowX; 
     for (unsigned int i=0;i<layers.size()-1;i++) { 
       const Amg::Transform3D ltransf(Amg::Translation3D(x_array[i],0.,0.));
       layerOrder.push_back(Trk::SharedObject<const Trk::Layer>(layers[i]));
       currX = ltransf.translation()[0]+0.5*layers[i]->thickness()+xShift;     
       binSteps.push_back(currX);
     }
     const Amg::Transform3D ltransf(Amg::Translation3D(x_array.back(),0.,0.));
     layerOrder.push_back(Trk::SharedObject<const Trk::Layer>(layers.back()));
     binSteps.push_back( halfZ+xShift);
  }
  //Trk::BinUtility* binUtility = new Trk::BinUtility1DX( lowX, new std::vector<double>(binSteps));
  Trk::BinUtility* binUtility = new Trk::BinUtility( binSteps, Trk::BinningOption::open, Trk::BinningValue::binX);
  Trk::LayerArray* tgcLayerArray = 0;
  tgcLayerArray = new Trk::NavBinnedArray1D<Trk::Layer>(layerOrder, binUtility, new Amg::Transform3D(Trk::s_idTransform));     

  return tgcLayerArray;

} 

double Muon::MuonStationTypeBuilder::decodeX(const GeoShape* sh) const 
{
  double xHalf = 0;

  const GeoTrd*  trd = dynamic_cast<const GeoTrd*> (sh);
  const GeoBox*  box = dynamic_cast<const GeoBox*> (sh);
  const GeoTube* tub = dynamic_cast<const GeoTube*> (sh);
  const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (sh);
  const GeoShapeUnion* uni = dynamic_cast<const GeoShapeUnion*> (sh);
  const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction*> (sh);
  const GeoSimplePolygonBrep* spb = dynamic_cast<const GeoSimplePolygonBrep*> (sh);

  if (!trd && !box && !tub && !shift && !uni && !sub && !spb) {
    std::cout << "MuonStationTypeBuilder::decodeX : unknown shape type ?" <<sh->type() << std::endl;  
  }

  if(spb) {
    for (unsigned int i=0;i<spb->getNVertices();i++) {
        ATH_MSG_DEBUG( " XVertex " << spb->getXVertex(i) << " YVertex " << spb->getYVertex(i) );
        if(spb->getXVertex(i)>xHalf) xHalf = spb->getXVertex(i);
    }
    ATH_MSG_DEBUG( " GeoSimplePolygonBrep xHalf " << xHalf );
  }

  //  if (trd ) std::cout << "trapezoid dimensions:" << trd->getXHalfLength1()<<"," << trd->getXHalfLength2()<<
  //	      "," << trd->getYHalfLength1()<<"," << trd->getYHalfLength2()<<"," << trd->getZHalfLength() << std::endl; 
  if (trd) xHalf = fmax( trd->getXHalfLength1(), trd->getXHalfLength2() );
  if (box) xHalf = box->getXHalfLength();
  // if (box ) std::cout << "box dimensions:" << box->getXHalfLength()<<"," << box->getYHalfLength()<<"," << box->getZHalfLength() << std::endl; 
  if (tub) xHalf = tub->getRMax();

  if (sub) {
    // be careful to handle properly GeoModel habit of subtracting large volumes from smaller ones
    // std::cout << " decoding subtraction:" << sub->getOpA()->type() << "," << sub->getOpB()->type() << std::endl;
    double xA = decodeX( sub->getOpA() );
    // double xB = decodeX( sub->getOpB() );
    xHalf = xA;
  }
  if (uni) {
    // std::cout << " decoding union:" << uni->getOpA()->type() << "," << uni->getOpB()->type() << std::endl;
    double xA = decodeX( uni->getOpA() );
    double xB = decodeX( uni->getOpB() );
    xHalf = fmax(xA,xB);
  }
  if (shift) {
    // std::cout << " decoding shift:" << shift->getOp()->type() <<"," << shift->getX().translation() << std::endl;
    double xA = decodeX( shift->getOp() );
    double xB = shift->getX().getTranslation()[0]; 
    xHalf = xA + fabs(xB);
  }

  // std::cout << "MuonStationTypeBuilder::decodeX : returns " << xHalf << std::endl;  
  return xHalf;
}
//

std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>*> Muon::MuonStationTypeBuilder::createLayerRepresentation(const Trk::TrackingVolume* trVol) const
{

  const Trk::Layer* layRepr = 0;   
  if (!trVol) return std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>*> (layRepr,0);

  std::vector<const Trk::Layer*>* multi = new std::vector<const Trk::Layer*>;

  // retrieve volume envelope

  const Trk::CuboidVolumeBounds* cubBounds= dynamic_cast<const Trk::CuboidVolumeBounds*> (&(trVol->volumeBounds()));
  const Trk::TrapezoidVolumeBounds* trdBounds= dynamic_cast<const Trk::TrapezoidVolumeBounds*> (&(trVol->volumeBounds()));
  const Trk::DoubleTrapezoidVolumeBounds* dtrdBounds= dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds*> (&(trVol->volumeBounds()));

  Amg::Transform3D subt= Trk::s_idTransform;

  const Trk::SubtractedVolumeBounds* subBounds= dynamic_cast<const Trk::SubtractedVolumeBounds*> (&(trVol->volumeBounds()));
  if (subBounds) {    
    subt *= Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.));
    while (subBounds) {
      cubBounds= dynamic_cast<const Trk::CuboidVolumeBounds*> (&(subBounds->outer()->volumeBounds()));
      trdBounds= dynamic_cast<const Trk::TrapezoidVolumeBounds*> (&(subBounds->outer()->volumeBounds()));
      dtrdBounds= dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds*> (&(subBounds->outer()->volumeBounds()));
      subBounds= dynamic_cast<const Trk::SubtractedVolumeBounds*> (&(subBounds->outer()->volumeBounds()));
    }
  }

  const Trk::PlaneLayer* layer = 0;

  if (cubBounds) { 
    double thickness = 2*cubBounds->halflengthX();
    double sf        = 4*cubBounds->halflengthZ()*cubBounds->halflengthY();
    //const std::vector<const Trk::Surface*>* surfs = cubBounds->decomposeToSurfaces(Trk::s_idTransform);
    //const Trk::RectangleBounds* rbounds = dynamic_cast<const Trk::RectangleBounds*> (&(*(surfs))[0]->bounds());
    Trk::RectangleBounds* rbounds=new Trk::RectangleBounds(cubBounds->halflengthY(),cubBounds->halflengthZ());
    Trk::SharedObject<const Trk::SurfaceBounds> bounds(rbounds);
    Trk::OverlapDescriptor* od=0;
    Trk::MaterialProperties matProp = collectStationMaterial(trVol,sf);
    if (matProp.thickness() > thickness) {
      ATH_MSG_DEBUG( " thickness of combined station material exceeds station size:" << trVol->volumeName() );
    } else if (matProp.thickness()<thickness && matProp.thickness()>0.) {
      //if (matProp.thickness()> 0.)  matProp *= thickness/matProp.thickness();
      double sf = thickness/matProp.thickness();
      //matProp.scale(sf);
      matProp = Trk::MaterialProperties(thickness,sf*matProp.x0(),sf*matProp.l0(),matProp.averageA(),
					matProp.averageZ(),matProp.averageRho()/sf);
    }
    Trk::HomogeneousLayerMaterial mat(matProp,0.);  
    layer = new Trk::PlaneLayer(new Amg::Transform3D(trVol->transform()*
						     Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						     Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))),
                                bounds, mat, thickness, od, 1 );
    //for (size_t i=0; i<surfs->size(); i++) delete (*surfs)[i];
    //delete surfs;
    // multilayers
    if ( m_multilayerRepresentation && trVol->confinedVolumes()) {
      const std::vector<const Trk::TrackingVolume*> vols = trVol->confinedVolumes()->arrayObjects();
      if (vols.size()>1) {
      for (unsigned int i=0;i<vols.size();i++) {
	Trk::MaterialProperties matMulti = collectStationMaterial(vols[i],sf);
        multi->push_back(new Trk::PlaneLayer(new Amg::Transform3D(vols[i]->transform()*
								  Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
								  Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))),
					     bounds, Trk::HomogeneousLayerMaterial(matMulti,0.), matMulti.thickness(), od, 1 ));
	//std::cout <<"multilayer added:"<< i<<","<<matMulti.thickness()<< std::endl;
      }}      
    }
  }
  else if (trdBounds) {
    double thickness = 2*trdBounds->halflengthZ();
    //std::cout <<"trdBounds:"<<thickness<<std::endl;
    double sf        = 2*(trdBounds->minHalflengthX()+trdBounds->maxHalflengthX())*trdBounds->halflengthY();
    const std::vector<const Trk::Surface*>* surfs = trdBounds->decomposeToSurfaces(Trk::s_idTransform);
    const Trk::TrapezoidBounds* tbounds = dynamic_cast<const Trk::TrapezoidBounds*> (&(*(surfs))[0]->bounds());
    Trk::SharedObject<const Trk::SurfaceBounds> bounds(new Trk::TrapezoidBounds(*tbounds));
    Trk::OverlapDescriptor* od=0;
    Trk::MaterialProperties matProp = collectStationMaterial(trVol,sf);
    if (matProp.thickness() > thickness) {
      ATH_MSG_DEBUG( " thickness of combined station material exceeds station size:" << trVol->volumeName() );
    } else if (matProp.thickness()<thickness && matProp.thickness()>0.) {
      float sf = thickness/matProp.thickness();
      matProp = Trk::MaterialProperties(thickness,sf*matProp.x0(),sf*matProp.l0(),matProp.averageA(),
					matProp.averageZ(),matProp.averageRho()/sf);
    }
    Trk::HomogeneousLayerMaterial mat(matProp,0.);  
    //std::cout <<"trd layer bounds:mat:"<<matProp.thickness()<<","<<matProp.x0()<<","<<thickness<<std::endl;
    layer = new Trk::PlaneLayer(new Amg::Transform3D(subt*trVol->transform()),
				bounds, mat, thickness, od, 1 );
    for (size_t i=0; i<surfs->size(); i++) delete (*surfs)[i];
    delete surfs;
    // multilayers
    if ( m_multilayerRepresentation && trVol->confinedVolumes() ) {
      const std::vector<const Trk::TrackingVolume*> vols = trVol->confinedVolumes()->arrayObjects();
      if (vols.size()>1) {
      for (unsigned int i=0;i<vols.size();i++) {
	Trk::MaterialProperties matMulti = collectStationMaterial(vols[i],sf);
        multi->push_back(new Trk::PlaneLayer(new Amg::Transform3D(vols[i]->transform()),
					     bounds, Trk::HomogeneousLayerMaterial(matMulti,0.), matMulti.thickness(), od, 1 ));
	//std::cout <<"multilayer added:"<< i<<","<<matMulti.thickness<< std::endl;
      }}      
    }
  }
  else if (dtrdBounds) {
    double thickness = 2*dtrdBounds->halflengthZ();
    double sf        = 2*(dtrdBounds->minHalflengthX()+dtrdBounds->medHalflengthX())*dtrdBounds->halflengthY1()
                      +2*(dtrdBounds->medHalflengthX()+dtrdBounds->maxHalflengthX())*dtrdBounds->halflengthY2();
    const std::vector<const Trk::Surface*>* surfs = dtrdBounds->decomposeToSurfaces(Trk::s_idTransform);
    const Trk::DiamondBounds* dbounds = dynamic_cast<const Trk::DiamondBounds*> (& (*(surfs))[0]->bounds());
    Trk::SharedObject<const Trk::SurfaceBounds> bounds(new Trk::DiamondBounds(*dbounds));
    Trk::OverlapDescriptor* od=0;
    Trk::MaterialProperties matProp = collectStationMaterial(trVol,sf);
    if (matProp.thickness() > thickness) {
      ATH_MSG_DEBUG( " thickness of combined station material exceeds station size:" << trVol->volumeName() );
    } else if (matProp.thickness()<thickness && matProp.thickness()>0.) {
      //if (matProp.thickness()> 0.)  matProp *= thickness/matProp.thickness();
      // double scale = dEst;    ????
      float sf = thickness/matProp.thickness();
      matProp = Trk::MaterialProperties(thickness,sf*matProp.x0(),sf*matProp.l0(),matProp.averageA(),
					matProp.averageZ(),matProp.averageRho()/sf);
    }
    Trk::HomogeneousLayerMaterial mat(matProp,0.);  
    layer = new Trk::PlaneLayer(new Amg::Transform3D(trVol->transform()),
                                bounds, mat, thickness, od, 1 );
    for (size_t i=0; i<surfs->size(); i++) delete (*surfs)[i];
    delete surfs;
    // multilayers
    if ( m_multilayerRepresentation && trVol->confinedVolumes()) {
      const std::vector<const Trk::TrackingVolume*> vols = trVol->confinedVolumes()->arrayObjects();
      if (vols.size()>1) {
      for (unsigned int i=0;i<vols.size();i++) {
	Trk::MaterialProperties matMulti = collectStationMaterial(vols[i],sf);
        multi->push_back(new Trk::PlaneLayer(new Amg::Transform3D(vols[i]->transform()),
					     bounds, Trk::HomogeneousLayerMaterial(matMulti,0.), matMulti.thickness(), od, 1 ));
	//std::cout <<"multilayer added:"<< i<<","<<matMulti.thickness<< std::endl;
      }}      
    }
  }

  //std::cout << "station:"<<trVol->volumeName()<<",thickness:"<<layer->thickness()<<std::endl;
  //std::cout << "station:"<<trVol->volumeName()<<",dInX0:"<<layer->layerMaterialProperties()->fullMaterial(layer->center())->thicknessInX0()<<std::endl;
  //if (trVol->confinedVolumes()) std::cout << "subvolumes:"<<trVol->confinedVolumes()->arrayObjects().size()<< std::endl;
 
  layRepr = layer;

  if (!multi->size()) {delete multi; multi=0;} 
  return std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>*> (layRepr,multi);
}


Identifier Muon::MuonStationTypeBuilder::identifyNSW( std::string vName, Amg::Transform3D transf ) const 
{
  Identifier id(0);

  if ((vName[0]=='Q') || (vName[0]=='M')) {     // NSW stations
    // station eta
    std::istringstream istr(&vName[1]);
    int iEta;
    if(vName[0]=='Q') {
      std::istringstream istr2(&vName[2]);
      istr2 >> iEta;
    } else istr >> iEta;
    if (transf.translation().z()<0.) iEta *= -1;
    // station Phi
    unsigned int iPhi = 1;
    //if (trVol->center().z()>0.) iPhi += 8;
    // station multilayer
    std::istringstream istm(&vName[3]);
    int iMult;
    istm >> iMult;
    if(vName[0]=='Q'&&vName[3]=='P') iMult = 1;
    if(vName[0]=='Q'&&vName[3]=='C') iMult = 2;
    // layer
    std::string stl(&vName[vName.size()-1]);
    std::istringstream istl(stl);
    int iLay;
    istl >> iLay;
    iLay += 1;
    if (vName[0]=='Q') {
      std::string stName = (vName[1]=='L') ? "STL" : "STS";
      //int stId = (vName[2]=='L') ? 0 : 1;
      id = m_muonMgr->stgcIdHelper()->channelID(stName,iEta,iPhi,iMult,iLay,1,1);
    } else {
      std::string stName = (vName[2]=='L') ? "MML" : "MMS";
      //int stId = (vName[2]=='L') ? 0 : 1;
      id = m_muonMgr->mmIdHelper()->channelID(stName,iEta,iPhi,iMult,iLay,1);   
    }
  }

  return id;
}

const Trk::Layer* Muon::MuonStationTypeBuilder::createLayer(const Trk::TrackingVolume* trVol, Trk::MaterialProperties* matEx, Amg::Transform3D& transf) const
{
  // identification first

  std::string vName=trVol->volumeName().substr(trVol->volumeName().find("-")+1); 
  //std::cout <<"NSW layer identification:"<< vName << std::endl;

  const Trk::RotatedTrapezoidBounds* rtrd=0;
  Amg::Vector3D mrg_pos=transf.translation();
 
  unsigned int layType = 0;  

  //std::cout <<vName[0]<<","<<vName[1]<<","<<vName[2]<< ","<<vName[vName.size()-1]<<std::endl;
  if ((vName[0]=='Q') || (vName[0]=='M')) {     // NSW stations
    // station eta
    std::istringstream istr(&vName[1]);
    int iEta;
    if(vName[0]=='Q') {
      std::istringstream  istr2(&vName[2]);
      istr2 >> iEta;
    } else istr >> iEta;
    if (transf.translation().z()<0.) iEta *= -1;
    // station Phi
    unsigned int iPhi = 1;
    //if (trVol->center().z()>0.) iPhi += 8;
    // station multilayer
    std::istringstream istm(&vName[3]);
    unsigned int iMult;
    istm >> iMult;
    if(vName[0]=='Q'&&vName[3]=='P') iMult = 1;
    if(vName[0]=='Q'&&vName[3]=='C') iMult = 2;
    // layer
    std::string stl(&vName[vName.size()-1]);
    std::istringstream istl(stl);
    unsigned int iLay;
    istl >> iLay;
    iLay += 1;
    if (vName[0]=='Q') {
      std::string stName = (vName[2]=='L') ? "STL" : "STS";
      int stId = (vName[2]=='L') ? 0 : 1;
      Identifier id = m_muonMgr->stgcIdHelper()->channelID(stName,iEta,iPhi,iMult,iLay,1,1);
      //std::cout <<"identifier:"<< id << std::endl;
      const MuonGM::sTgcReadoutElement* stgc=m_muonMgr->getsTgcRElement_fromIdFields(stId,iEta,iPhi,iMult);
      //std::cout <<"stgcRE:"<< stgc << std::endl;
      //if (stgc) std::cout <<"stgcRE recovered:"<<id<<","<< stgc->center(id) << std::endl;
      //std::cout <<"input sTGC:"<< iEta<<","<<iPhi<<","<<iMult<<","<<iLay<<std::endl;
      //std::cout << m_muonMgr->stgcIdHelper()->stationName(id)<<","<< m_muonMgr->stgcIdHelper()->stationEta(id)<<","<<
      //m_muonMgr->stgcIdHelper()->stationPhi(id)<<","<< m_muonMgr->stgcIdHelper()->multiplet(id)<<","<< m_muonMgr->stgcIdHelper()->gasGap(id)<<std::endl;
      layType = id.get_identifier32().get_compact();
      if (stgc) {
	rtrd = dynamic_cast<const Trk::RotatedTrapezoidBounds*> (&stgc->bounds(id));
	//std::cout <<"stgc bounds rotated trapezoid ? "<< rtrd<<  std::endl;
	mrg_pos = stgc->center(id);
      }
    } else {
      std::string stName = (vName[2]=='L') ? "MML" : "MMS";
      int stId = (vName[2]=='L') ? 0 : 1;
      Identifier id = m_muonMgr->mmIdHelper()->channelID(stName,iEta,iPhi,iMult,iLay,1);   
      const MuonGM::MMReadoutElement* mm=m_muonMgr->getMMRElement_fromIdFields(stId,iEta,iPhi,iMult);
      //if (mm) std::cout <<"mmRE recovered:"<<id<<","<< mm->center(id) << std::endl;
      //std::cout <<"input MM:"<< iEta<<","<<iPhi<<","<<iMult<<","<<iLay<<std::endl;
      //std::cout << mm->getStationName()<<","<< mm->getStationEta()<<","<<
      //	mm->getStationPhi()<<","<< m_muonMgr->mmIdHelper()->multiplet(id)<<","<< m_muonMgr->mmIdHelper()->gasGap(id)<<std::endl;
      layType = id.get_identifier32().get_compact();
      if (mm) {
	rtrd = dynamic_cast<const Trk::RotatedTrapezoidBounds*> (&mm->bounds(id));
	mrg_pos = mm->center(id);
      }
    }
  }

  //if (rtrd) std::cout <<"extract layer size from MRG:"<< rtrd->halflengthX()<<","<<rtrd->minHalflengthY()<<","<<rtrd->maxHalflengthY()<< std::endl; 

  const Trk::Layer* layRepr = 0;   
  if (!trVol) return layRepr;

  // retrieve volume envelope

  const Trk::CuboidVolumeBounds* cubBounds= dynamic_cast<const Trk::CuboidVolumeBounds*> (&(trVol->volumeBounds()));
  const Trk::TrapezoidVolumeBounds* trdBounds= dynamic_cast<const Trk::TrapezoidVolumeBounds*> (&(trVol->volumeBounds()));
  const Trk::DoubleTrapezoidVolumeBounds* dtrdBounds= dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds*> (&(trVol->volumeBounds()));

  Amg::Transform3D subt(Trk::s_idTransform);

  const Trk::SubtractedVolumeBounds* subBounds= dynamic_cast<const Trk::SubtractedVolumeBounds*> (&(trVol->volumeBounds()));
  if (subBounds) {    
    subt *= Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))* Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.));
    while (subBounds) {
      //std::cout << "looping over subtracted volume bounds:outer,inner position:"<< subBounds->outer()->center()<<"," << subBounds->inner()->center() << std::endl;
      cubBounds= dynamic_cast<const Trk::CuboidVolumeBounds*> (&(subBounds->outer()->volumeBounds()));
      trdBounds= dynamic_cast<const Trk::TrapezoidVolumeBounds*> (&(subBounds->outer()->volumeBounds()));
      dtrdBounds= dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds*> (&(subBounds->outer()->volumeBounds()));
      subBounds= dynamic_cast<const Trk::SubtractedVolumeBounds*> (&(subBounds->outer()->volumeBounds()));

      /*
      if (&(subBounds->inner()->volumeBounds()) ) {
	const Trk::CuboidVolumeBounds* icubBounds= dynamic_cast<const Trk::CuboidVolumeBounds*> (&(subBounds->inner()->volumeBounds()));
	//const Trk::TrapezoidVolumeBounds* itrdBounds= dynamic_cast<const Trk::TrapezoidVolumeBounds*> (&(subBounds->inner()->volumeBounds()));
	//const Trk::DoubleTrapezoidVolumeBounds* idtrdBounds= dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds*> (&(subBounds->inner()->volumeBounds()));
	//const Trk::SubtractedVolumeBounds* isubBounds= dynamic_cast<const Trk::SubtractedVolumeBounds*> (&(subBounds->inner()->volumeBounds()));
	//std::cout << "inner volume:box,trd,subtr:" <<icubBounds<<"," << itrdBounds<<"," << isubBounds << std::endl;
      }
      */
    }
  }

  const Trk::PlaneLayer* layer = 0;

  if (cubBounds) { 
    double thickness = 2*cubBounds->halflengthX();
    Trk::OverlapDescriptor* od=0;
    double scale = matEx->thickness()/thickness;
    Trk::MaterialProperties  matProp(thickness,matEx->x0()/scale,matEx->l0()/scale,
					     matEx->averageA(),matEx->averageZ(),scale*matEx->averageRho());
    Trk::HomogeneousLayerMaterial mat(matProp,0.);  

    Trk::RectangleBounds* rbounds=new Trk::RectangleBounds(cubBounds->halflengthY(),cubBounds->halflengthZ());
    Trk::SharedObject<const Trk::SurfaceBounds> bounds(rbounds);
    layer = new Trk::PlaneLayer(new Amg::Transform3D(trVol->transform()*
						     Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						     Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.))),
                                bounds, mat, thickness, od, 1 );
  }
  else if (trdBounds) {
    double thickness = 2*trdBounds->halflengthZ();
    Trk::OverlapDescriptor* od=0;
    double scale = matEx->thickness()/thickness;
    Trk::MaterialProperties  matProp(thickness,matEx->x0()/scale,matEx->l0()/scale,
					     matEx->averageA(),matEx->averageZ(),scale*matEx->averageRho());
    Trk::HomogeneousLayerMaterial mat(matProp,0.);  
    //double sf        = 2*(trdBounds->minHalflengthX()+trdBounds->maxHalflengthX())*trdBounds->halflengthY();
    if (rtrd) {
      Trk::TrapezoidBounds* tbounds = new Trk::TrapezoidBounds(rtrd->halflengthX(),rtrd->minHalflengthY(),rtrd->maxHalflengthY());
      Trk::SharedObject<const Trk::SurfaceBounds> bounds(tbounds);
      layer = new Trk::PlaneLayer(new Amg::Transform3D(subt*trVol->transform()),
				bounds, mat, thickness, od, 1 );
      //std::cout <<"MTG layer shift ? " << (transf*subt*trVol->transform()).translation()<<"->"<<mrg_pos << std::endl;
      Amg::Vector3D mtg_pos=(transf*subt*trVol->transform()).translation();
      transf *= Amg::Translation3D(mrg_pos-mtg_pos);
    } else {
      const std::vector<const Trk::Surface*>* surfs = trdBounds->decomposeToSurfaces(Amg::Transform3D(Trk::s_idTransform));
      const Trk::TrapezoidBounds* tbounds = dynamic_cast<const Trk::TrapezoidBounds*> (&(*(surfs))[0]->bounds());
      Trk::SharedObject<const Trk::SurfaceBounds> bounds(new Trk::TrapezoidBounds(*tbounds));
      layer = new Trk::PlaneLayer(new Amg::Transform3D(subt*trVol->transform()),
				bounds, mat, thickness, od, 1 );
    }
  }
  else if (dtrdBounds) {
    double thickness = 2*dtrdBounds->halflengthZ();
    //double sf        = 2*(dtrdBounds->minHalflengthX()+dtrdBounds->medHalflengthX())*dtrdBounds->halflengthY1()
    //                  +2*(dtrdBounds->medHalflengthX()+dtrdBounds->maxHalflengthX())*dtrdBounds->halflengthY2();
    const std::vector<const Trk::Surface*>* surfs = dtrdBounds->decomposeToSurfaces(Amg::Transform3D(Trk::s_idTransform));
    const Trk::DiamondBounds* dbounds = dynamic_cast<const Trk::DiamondBounds*> (& (*(surfs))[0]->bounds());
    Trk::SharedObject<const Trk::SurfaceBounds> bounds(new Trk::DiamondBounds(*dbounds));
    Trk::OverlapDescriptor* od=0;
    double scale = matEx->thickness()/thickness;
    Trk::MaterialProperties  matProp(thickness,matEx->x0()/scale,matEx->l0()/scale,
					     matEx->averageA(),matEx->averageZ(),scale*matEx->averageRho());
    Trk::HomogeneousLayerMaterial mat(matProp,0.);   
    layer = new Trk::PlaneLayer(new Amg::Transform3D(trVol->transform()),
                                bounds, mat, thickness, od, 1 );
    delete surfs;
  }

  //std::cout << "station:"<<trVol->volumeName()<<",thickness:"<<layer->thickness()<<std::endl;
  //std::cout << "station:"<<trVol->volumeName()<<",dInX0:"<<layer->layerMaterialProperties()->fullMaterial(layer->center())->thicknessInX0()<<std::endl;
  //  if (trVol->confinedVolumes()) std::cout << "subvolumes:"<<trVol->confinedVolumes()->arrayObjects().size()<< std::endl;

  layRepr = layer;
  layRepr->setLayerType(layType);

  //std::cout <<"layer identifier"<< layRepr->layerType()<< std::endl;

  return layRepr;
}


Trk::MaterialProperties Muon::MuonStationTypeBuilder::collectStationMaterial(const Trk::TrackingVolume* vol, double sf) const
{
  Trk::MaterialProperties layMat(0.,10.e10,10.e10,13.,26.,0.);

  // sf is surface of the new layer used to calculate the average 'thickness' of components
  // layers
  if (vol->confinedLayers()){
    const std::vector<const Trk::Layer*> lays = vol->confinedLayers()->arrayObjects();
    for (unsigned il=0; il<lays.size(); il++) {
      const Trk::MaterialProperties* mLay = lays[il]->layerMaterialProperties()->fullMaterial(lays[il]->surfaceRepresentation().center());
      if (mLay) {
        layMat.addMaterial(mLay->material(),lays[il]->thickness()/mLay->material().x0());
      }
    }
  } 
  if (vol->confinedArbitraryLayers()){
    const std::vector<const Trk::Layer*> lays = *(vol->confinedArbitraryLayers());
    for (unsigned il=0; il<lays.size(); il++) {
      const Trk::MaterialProperties* mLay = lays[il]->layerMaterialProperties()->fullMaterial(lays[il]->surfaceRepresentation().center());
      // scaling factor
      const Trk::RectangleBounds* rect = dynamic_cast<const Trk::RectangleBounds*> (&(lays[il]->surfaceRepresentation().bounds()));
      const Trk::TrapezoidBounds* trap = dynamic_cast<const Trk::TrapezoidBounds*> (&(lays[il]->surfaceRepresentation().bounds()));
      if ((rect || trap) && mLay) {
	double scale = rect ? 4*rect->halflengthX()*rect->halflengthY()/sf 
	  : 2*(trap->minHalflengthX()+trap->maxHalflengthX())*trap->halflengthY()/sf;
        layMat.addMaterial(mLay->material(),scale*lays[il]->thickness()/mLay->material().x0());
      }
    }
  } 
  // subvolumes
  if (vol->confinedVolumes()){
    const std::vector<const Trk::TrackingVolume*> subVols = vol->confinedVolumes()->arrayObjects();
    for (unsigned iv=0; iv<subVols.size(); iv++) {
      if (subVols[iv]->confinedLayers()){
	const std::vector<const Trk::Layer*> lays = subVols[iv]->confinedLayers()->arrayObjects();
	for (unsigned il=0; il<lays.size(); il++) {
	  const Trk::MaterialProperties* mLay = lays[il]->layerMaterialProperties()->fullMaterial(lays[il]->surfaceRepresentation().center());
          if (mLay) layMat.addMaterial(mLay->material(),lays[il]->thickness()/mLay->material().x0());
        }
      } 
      if (subVols[iv]->confinedArbitraryLayers()){
	const std::vector<const Trk::Layer*> lays = *(subVols[iv]->confinedArbitraryLayers());
	for (unsigned il=0; il<lays.size(); il++) {
	  const Trk::MaterialProperties* mLay = lays[il]->layerMaterialProperties()->fullMaterial(lays[il]->surfaceRepresentation().center());
	  // scaling factor
	  const Trk::RectangleBounds* rect = dynamic_cast<const Trk::RectangleBounds*> (&(lays[il]->surfaceRepresentation().bounds()));
	  const Trk::TrapezoidBounds* trap = dynamic_cast<const Trk::TrapezoidBounds*> (&(lays[il]->surfaceRepresentation().bounds()));
	  if ((rect || trap) && mLay) {
	    double scale = rect ? 4*rect->halflengthX()*rect->halflengthY()/sf
	      : 2*(trap->minHalflengthX()+trap->maxHalflengthX())*trap->halflengthY()/sf;
	    layMat.addMaterial(mLay->material(),scale*lays[il]->thickness()/mLay->material().x0());
	  }
        }
      }     
    }
  }
  return layMat;
}

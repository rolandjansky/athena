/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// CaloSurfacaBuilder.cxx, (c) ATLAS detector software
//****************************************************************************

#include "CaloTrackingGeometry/CaloSurfaceBuilder.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <vector>

#include "TrkSurfaces/CylinderSurface.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkSurfaces/CylinderBounds.h"

#include "TrkSurfaces/DiscSurface.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkSurfaces/DiscBounds.h"

#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"

// Amg
#include "GeoPrimitives/GeoPrimitives.h"

// Calo specific stuff :
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/ICaloCoordinateTool.h"
#include "CaloDetDescr/CaloDepthTool.h"
#include "CaloDetDescr/ICaloRecoMaterialTool.h"
#include "CaloDetDescr/ICaloRecoSimpleGeomTool.h"

// Tile specific stuff :
#include "TileDetDescr/TileDetDescrManager.h"
#include "TileDetDescr/TileDetDescriptor.h"
#include "CaloIdentifier/TileID.h"

#include "TrkDetDescrInterfaces/ITrackingVolumeBuilder.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometrySurfaces/SlidingCylinderSurface.h"
#include "TrkGeometrySurfaces/SlidingDiscSurface.h"


#include <cmath>

#include <iostream>
#include <iomanip>

CaloSurfaceBuilder::CaloSurfaceBuilder(const std::string& type,
				       const std::string& name,
				       const IInterface* parent) :
  AthAlgTool(type, name, parent),
  //m_calo_tb_coord("TBCaloCoordinate"),
  m_calo_dd(nullptr),
  m_tile_dd(nullptr),
  m_lar_mat("LArRecoMaterialTool"),
  m_lar_simplegeom("LArRecoSimpleGeomTool"),
  m_calodepth("CaloDepthTool"),
  m_tileVolumeBuilder("Tile::TileVolumeBuilder/TileVolumeBuilder")
{
  declareInterface<ICaloSurfaceBuilder>( this );

  //declareProperty ("TBCaloCoordinateTool",      m_calo_tb_coord);
  declareProperty ("LArRecoMaterialTool",       m_lar_mat);
  declareProperty ("LarRecoSimpleGeometryTool", m_lar_simplegeom);
  declareProperty ("CaloDepthTool",             m_calodepth);
  declareProperty ("TileVolumeBuilder",         m_tileVolumeBuilder);

}

CaloSurfaceBuilder::~CaloSurfaceBuilder()
{}

StatusCode
CaloSurfaceBuilder::initialize()
{

  // Retrieve the tools one by one
  //if (m_calo_tb_coord.retrieve().isFailure()){
  //    ATH_MSG_FATAL("Failed tor retrieve "<< m_calo_tb_coord << ". Arborting.");
  //    return StatusCode::FAILURE;
  //} else
  //    ATH_MSG_DEBUG("Sucessfully retrieved "<< m_calodepth << ".");

  if (m_lar_mat.retrieve().isFailure()){
      ATH_MSG_FATAL("Failed tor retrieve "<< m_lar_mat << ". Arborting.");
      return StatusCode::FAILURE;
  } else
      ATH_MSG_DEBUG("Sucessfully retrieved "<< m_lar_mat << ".");

  if (m_lar_simplegeom.retrieve().isFailure()){
      ATH_MSG_FATAL("Failed tor retrieve "<< m_lar_simplegeom << ". Arborting.");
      return StatusCode::FAILURE;
  } else
      ATH_MSG_DEBUG("Sucessfully retrieved "<< m_lar_simplegeom << ".");

  if (m_calodepth.retrieve().isFailure()){
      ATH_MSG_FATAL("Failed tor retrieve "<< m_calodepth << ". Arborting.");
      return StatusCode::FAILURE;
  } else
      ATH_MSG_DEBUG("Sucessfully retrieved "<< m_calodepth << ".");

  /* Doesn't work (yet) in mig5 
  if (m_tileVolumeBuilder.retrieve().isFailure()){
      ATH_MSG_FATAL("Failed tor retrieve "<< m_tileVolumeBuilder << ". Arborting.");
      return StatusCode::FAILURE;
  } else
        ATH_MSG_DEBUG("Sucessfully retrieved "<< m_tileVolumeBuilder << ".");
  */

  if (detStore()->retrieve(m_calo_dd).isFailure()) {
     ATH_MSG_WARNING("Could not find CaloDetDescriptorManager in DetStore, calling CaloDetDescrManager::instance() instead");
     m_calo_dd = CaloDetDescrManager::instance();
     if (!m_calo_dd) return StatusCode::FAILURE;
  }

  if (detStore()->retrieve(m_tile_dd).isFailure()){
    ATH_MSG_FATAL("Could not find TileDetDescrManager in DetStore" );
    return StatusCode::FAILURE;
  }

  fill_tg_surfaces(); 

  return StatusCode::SUCCESS;
}

StatusCode
CaloSurfaceBuilder::finalize()
{
  // clear surface cash
  for (unsigned int i=0; i<m_layerEntries.size(); i++) {
    delete m_layerEntries[i].first;
    delete m_layerEntries[i].second;
  }

  delete m_layerExits[CaloCell_ID::TileBar2].first;
  delete m_layerExits[CaloCell_ID::TileBar2].second;
  delete m_layerExits[CaloCell_ID::TileExt2].first;
  delete m_layerExits[CaloCell_ID::TileExt2].second;
  delete m_layerExits[CaloCell_ID::TileGap2].first;
  delete m_layerExits[CaloCell_ID::TileGap2].second;
  delete m_layerExits[CaloCell_ID::TileGap3].first;
  delete m_layerExits[CaloCell_ID::TileGap3].second;
  delete m_layerExits[CaloCell_ID::HEC3].first;
  delete m_layerExits[CaloCell_ID::HEC3].second;
  
  StatusCode sc = StatusCode::SUCCESS;
  return sc;
}

Trk::Surface*
CaloSurfaceBuilder:: CreateDDSurface (const CaloCell_ID::CaloSample sample,  const int side)
{
  // NB: the Transform3D created here belong to the surface,
  //     and will be deleted by it
  Amg::Transform3D* pos = new Amg::Transform3D(Trk::s_idTransform);
  Trk::Surface* surf;

  bool result = false;
  double rmin = 0.;
  double rmax = 0.;
  double radius = 0.;
  double hphi = 0.;
  double hlen = 0.;
  double z =0.;
  double depth =0.;

  // too bad, I need this ugly if to decide which type of surface should be built
  // cylinder of disk ?
  bool barrel = true;
  if ( sample == CaloCell_ID::PreSamplerE || sample == CaloCell_ID::EME1 ||
       sample == CaloCell_ID::EME2 || sample == CaloCell_ID::EME3 ||
       sample == CaloCell_ID::HEC0 || sample == CaloCell_ID::HEC1 ||
       sample == CaloCell_ID::HEC2 || sample == CaloCell_ID::HEC3 ||
       sample == CaloCell_ID::FCAL0 || sample == CaloCell_ID::FCAL1 || sample == CaloCell_ID::FCAL2 )
    barrel = false;

  if ( barrel ) {
    result = this->get_cylinder_surface (sample,side, pos, radius, hphi, hlen, depth);
    if (!result){
      delete pos;
      return 0;
    }
    ATH_MSG_VERBOSE("got -flat- cylinder for Sample " << (int) sample << " radius, hlen are " << radius
       << " " << hlen );
    //m_calo_tb_coord->read_table_position();
    //*pos = ( *(m_calo_tb_coord->transform_calo_to_ctb()) ) * ( *pos );

    // rounding effects. the phi range was introduced for the testbeam
    if ( hphi < 0.9*M_PI)
      surf = new Trk::CylinderSurface(pos,radius,hphi,hlen);
    else
      surf = new Trk::CylinderSurface(pos,radius,hlen);
  }
  else {

    bool result = this->get_disk_surface (sample,side,pos,z,rmin, rmax, hphi, depth);
    if (!result){
      delete pos;
      return 0;
    }


    ATH_MSG_VERBOSE(" got -flat- disk for Sample " << (int) sample << " rmin rmax = " << rmin
       << " " << rmax  << " z = " << pos->translation().z());

    //m_calo_tb_coord->print_transform ( *pos );

    // for Atlas configuration, avoid to create a hole a phi ~ pi by hphi
    // rounding effects. the phi range was introduced for the testbeam
    if ( hphi < 0.9*M_PI)
      surf = new Trk::DiscSurface(pos,rmin,rmax,hphi);
    else
      surf = new Trk::DiscSurface(pos,rmin,rmax);
  }

  return surf;
}



Trk::Surface*  CaloSurfaceBuilder:: CreateUserSurface (const CaloCell_ID::CaloSample sample,
					                                   const double offset,
					                                   const double etaCaloLocal)
{

 if (sample == CaloCell_ID::Unknown) return 0;
 if (!m_calo_dd) return 0;

  // NB: the Transform3D created here belong to the surface,
  //     and will be deleted by it
 Amg::Transform3D* pos = new Amg::Transform3D(Trk::s_idTransform);
  Trk::Surface* surf =0;

  bool result = false;
  double rmin = 0.;
  double z=0.;
  double rmax = 0.;
  double radius = 0.;
  double hphi = 0.;
  double hlen = 0.;
  double depth =0.;
  int side = 1;

  bool tile = false;
  if ( sample == CaloCell_ID::TileBar0 || sample == CaloCell_ID::TileBar1 || sample == CaloCell_ID::TileBar2 ||
       sample == CaloCell_ID::TileExt0 || sample == CaloCell_ID::TileExt1 || sample == CaloCell_ID::TileExt2 ||
       sample == CaloCell_ID::TileGap1 || sample == CaloCell_ID::TileGap2 || sample == CaloCell_ID::TileGap3 )
    tile = true;

  bool HEC3 = false;
  if ( sample == CaloCell_ID::HEC3)
    HEC3 = true;

  if (etaCaloLocal<0) side = -1;

  // Specific hack for the testbeam : even if tracks have eta<0, ask for the positive
  // side
  if ( m_calo_dd->lar_geometry() == "H8" ) side = 1;

  // too bad, I need this ugly if to decide which type of surface should be built
  // cylinder of disk ?
  //Note: TileGap3 is in the barrel but is a disk!!
  bool barrel = true;
  if ( sample == CaloCell_ID::PreSamplerE || sample == CaloCell_ID::EME1 ||
       sample == CaloCell_ID::EME2 || sample == CaloCell_ID::EME3 ||
       sample == CaloCell_ID::HEC0 || sample == CaloCell_ID::HEC1 ||
       sample == CaloCell_ID::HEC2 ||  sample == CaloCell_ID::HEC3 ||
       sample == CaloCell_ID::FCAL0 ||  sample == CaloCell_ID::FCAL1 ||
       sample == CaloCell_ID::FCAL2 || sample == CaloCell_ID::TileGap3 )
    barrel = false;

  if ( barrel ) {


    /*To avoid the funny numbers given by the CaloDescriptors to  TileBar2, (due to the cell D type)
      we'll take only the radius from them, the rest we copy from TileBar1 */
    if ( sample == CaloCell_ID::TileBar2){
      double radius2=0;
      Amg::Transform3D* pos2 = new Amg::Transform3D(Trk::s_idTransform);
      result = this->get_cylinder_surface (sample,side,pos2, radius, hphi, hlen, depth);
      delete pos2;
      result = this->get_cylinder_surface (CaloCell_ID::TileBar1,side, pos, radius2, hphi, hlen, depth);
    }else{

      result = this->get_cylinder_surface (sample,side, pos, radius, hphi, hlen, depth);
    }
    if (!result){
      delete pos;
      return 0;
    }

    //this correction will only work for LAr
    double betterrad = 0;
    if(!tile){
      betterrad = m_calodepth->radius(sample,etaCaloLocal,0.);
      radius = offset + betterrad;
    }


    if (tile) hlen = hlen/2;

    //m_calo_tb_coord->read_table_position();
    //*pos = ( *(m_calo_tb_coord->transform_calo_to_ctb()) ) * ( *pos );

    // for Atlas configuration, avoid to create a hole at phi ~ pi by hphi
    // rounding effects  (the phi range was introduced for the testbeam)
    if ( hphi < 0.9*M_PI)
      surf = new Trk::CylinderSurface(pos,radius,hphi,hlen);
    else
      surf = new Trk::CylinderSurface(pos,radius,hlen);
  }
  else {

    bool result = this->get_disk_surface (sample,side, pos, z,rmin, rmax, hphi, depth);
    if (!result){
      delete pos;
      return 0;
    }

    double betterz =0;

    if(!tile && !HEC3){
	betterz = m_calodepth->radius(sample,etaCaloLocal,0.);
	z = offset + betterz - (*pos)(2,3);
	//log <<MSG::DEBUG << " shift z by : betterz + offset - old-z = " << betterz << " + " << offset << " - "
	//    <<  pos->getTranslation().z() << " = " << z << endreq;
	Amg::Translation3D vec(0.,0.,z);
	*pos =  (*pos) * vec  ;

    }
    else{
      //log<<MSG::DEBUG<<" Is a tile or HEC3 the z won't be changed  "<<z<<endreq;
    }

    //log<<MSG::DEBUG << " got -real- disk for Sample " << (int) sample << " rmin: "<< rmin <<"  rmax: "
    //   << rmax  << " z = " << pos->getTranslation().z() << " for etaCaloLocal=" << etaCaloLocal << endreq;
    //m_calo_tb_coord->print_transform ( *pos );

    // for Atlas configuration, avoid to create a hole a phi ~ pi by hphi
    // rounding effects. the phi range was introduced for the testbeam
    if ( hphi < 0.9*M_PI)
      surf = new Trk::DiscSurface(pos,rmin,rmax,hphi);
    else
      surf = new Trk::DiscSurface(pos,rmin,rmax);
  }

  return surf;
}


Trk::Surface*
CaloSurfaceBuilder::CreateLastSurface (const CaloCell_ID::CaloSample sample,
					const double offset,
					const double etaCaloLocal)
{
  ATH_MSG_DEBUG( "In CreateLastSurface()" );

  if (!m_calo_dd) return 0;

  Trk::Surface* surf =0;

  bool tile = false;
  if ( sample == CaloCell_ID::TileBar0 || sample == CaloCell_ID::TileBar1 || sample == CaloCell_ID::TileBar2 ||
       sample == CaloCell_ID::TileExt0 || sample == CaloCell_ID::TileExt1 || sample == CaloCell_ID::TileExt2 ||
       sample == CaloCell_ID::TileGap1 || sample == CaloCell_ID::TileGap2 || sample == CaloCell_ID::TileGap3 )
    tile = true;

  // NB: the Transform3D created here belong to the surface,
  //     and will be deleted by it
  Amg::Transform3D* pos = new Amg::Transform3D(Trk::s_idTransform);

  bool result = false;
  double rmin = 0.;
  double z=0.;
  double rmax = 0.;
  double radius = 0.;
  double hphi = 0.;
  double hlen = 0.;
  double depth =0.;
  int side = 1;

  if (etaCaloLocal<0) side = -1;

  if ( sample == CaloCell_ID::TileBar2 ||  sample == CaloCell_ID::TileExt2 || sample == CaloCell_ID::TileGap2){


    /*To avoid the funny numbers given by the CaloDescriptors to  TileBar2, (due to the cell D type)
      we'll take only the radius from them, the rest we copy from TileBar1 */
    if ( sample == CaloCell_ID::TileBar2){
      double radius2=0;
      Amg::Transform3D* pos2 = new Amg::Transform3D(Trk::s_idTransform);
      result = this->get_cylinder_surface (sample,side,pos2, radius, hphi, hlen, depth);
      delete pos2;
      result = this->get_cylinder_surface (CaloCell_ID::TileBar1,side, pos, radius2, hphi, hlen, depth);
    }else{

      result = this->get_cylinder_surface (sample,side, pos, radius, hphi, hlen, depth);
    }
    if (!result){
      delete pos;
      return 0;
    }


    // for Atlas configuration, avoid to create a hole at phi ~ pi by hphi
    // rounding effects  (the phi range was introduced for the testbeam)
    else{
      std::vector <TileDetDescriptor*>::const_iterator  Tile_first = m_tile_dd->tile_descriptors_begin();
      //std::vector <CaloDetDescriptor*>::const_iterator  Tile_first = m_calo_dd->tile_descriptors_begin();
      TileDetDescriptor* TileBar_descr = *(Tile_first+1);
      TileDetDescriptor* TileExt_descr = *(Tile_first+3);
      TileDetDescriptor* TileGap_descr = *(Tile_first+5);
      if (sample == CaloCell_ID::TileBar2 )      radius = TileBar_descr->rcenter(2) + .5*TileBar_descr->dr(2) ;
      else if (sample == CaloCell_ID::TileExt2 ) radius = TileExt_descr->rcenter(2) + .5*TileExt_descr->dr(2) ;
      else if (sample == CaloCell_ID::TileGap2 ) radius = TileGap_descr->rcenter(2) + .5*TileGap_descr->dr(2) ;
      else ATH_MSG_DEBUG("Not a considered cylinder tile");

      if (tile)
	hlen = hlen/2;
      ATH_MSG_DEBUG("got -real- cylinder for Sample " << (int) sample << " eta= "
	 << etaCaloLocal << " offset=" << offset
	 << " ==> radius, hlen are " << radius
		    << " " << hlen);


      if ( hphi < 0.9*M_PI)
	return surf = new Trk::CylinderSurface(pos,radius,hphi,hlen);
      else
	return surf = new Trk::CylinderSurface(pos,radius,hlen);

    }
  }
  else if (  sample == CaloCell_ID::TileGap3 || sample == CaloCell_ID::HEC3   ){

    bool result = this->get_disk_surface (sample,side, pos, z,rmin, rmax, hphi, depth);
    if (!result){
      delete pos;
      return 0;
    }
    double zend=0;
    if (sample == CaloCell_ID::TileGap3){
      //  std::vector <CaloDetDescriptor*>::const_iterator  Tile_first = m_calo_dd->tile_descriptors_begin();
      std::vector <TileDetDescriptor*>::const_iterator  Tile_first = m_tile_dd->tile_descriptors_begin();
      TileDetDescriptor* TileGap_descr = *(Tile_first+5);
      zend =   TileGap_descr->zcenter(2) + .5*TileGap_descr->dz(2) ;
      zend = zend * side;
    }
    if (sample == CaloCell_ID::HEC3){
      std::vector <CaloDetDescriptor*>::const_iterator  HEC_first = m_calo_dd->calo_descriptors_begin();
      std::vector <CaloDetDescriptor*>::const_iterator  HEC_last = m_calo_dd->calo_descriptors_end();
      for (; HEC_first != HEC_last; ++HEC_first) {
	CaloDetDescriptor* descr = *HEC_first;
	if (descr) {
	  if ( descr->getSampling(0) == CaloCell_ID::HEC3){
	    zend = descr->calo_z_max();
	    zend = zend * side;
	  }
	}
      }
    }

    z = offset + zend - (*pos)(2,3);
    ATH_MSG_DEBUG(" LastSurface shift z by : offset - zend + oldz  = " << offset << " - "
		  <<zend << " +  " << (*pos)(2,3) << " = " << z);

    Amg::Translation3D vec(0.,0.,z);
    *pos = (*pos) * vec;

    ATH_MSG_DEBUG(" got -real- disk for Sample " << (int) sample << " rmin: "<< rmin <<"  rmax: "
		  << rmax  << " z = " << (*pos)(2,3) << " for etaCaloLocal=" << etaCaloLocal);


    // for Atlas configuration, avoid to create a hole a phi ~ pi by hphi
    // rounding effects. the phi range was introduced for the testbeam
    if ( hphi < 0.9*M_PI)
      return new Trk::DiscSurface(pos,rmin,rmax,hphi);
    else
      return new Trk::DiscSurface(pos,rmin,rmax);
  }
  else{
    delete pos;
    ATH_MSG_ERROR("This sample is not a 'LAST' surface... returning dummy surface");
    return surf;
  }

  return 0;
}


Trk::Surface*
CaloSurfaceBuilder::CreateGirderSurface(){
  ATH_MSG_FATAL("CaloSufraceBuilder::CreateGirderSurface not implemented in mig5!");
  return NULL;

  /*
  log <<MSG::DEBUG << "In CreateGirderSurface()"<<endreq;
  const std::vector<const Trk::TrackingVolume*>* tileVolumes = m_tileVolumeBuilder->trackingVolumes();
  //Girder part
  const Trk::TrackingVolume* tileBarrelGirder                 = (*tileVolumes)[11];

  const Trk::CylinderVolumeBounds* tileBarrelGirderBounds
    = dynamic_cast<const Trk::CylinderVolumeBounds*>(&(tileBarrelGirder->volumeBounds()));
  if (!tileBarrelGirderBounds) std::abort();

  double radius = tileBarrelGirderBounds->outerRadius();
  delete tileBarrelGirderBounds;
  double hlen = 6600; //big enough so the extrapolation will always find it.
  log<<MSG::DEBUG << "Created Surface right after the Girder with  radius: " << radius << "  hlen: " << hlen << endreq;
  return (new Trk::CylinderSurface(new Amg::Transform3D,radius,hlen) );
  */
}


bool
CaloSurfaceBuilder::CreateDDLayers(CaloSubdetNames::ALIGNVOL alvol, std::vector< Trk::CylinderLayer*>* thelayer)
{
  bool result = false;
  thelayer->clear();

  Amg::Transform3D htrans(Trk::s_idTransform);
  double hphi;
  std::vector<double> radius;
  std::vector<double> depth;
  std::vector<double> hlength;
  double mass, volume, x0, dEdx, aveA, aveZ;

  // the material
  result = m_lar_mat->get_material(alvol, mass, volume, x0, dEdx, aveA, aveZ);

  // the layers
  result = this->get_cylinder_surface(alvol, &htrans, hphi, radius, depth, hlength);
  if (!result){
      return 0;
    }


  for ( unsigned int i = 0; i<radius.size(); i++ ) {
    Trk::MaterialProperties objectMaterial(depth[i], x0, 3*x0/aveZ, aveA, aveZ, mass/volume);    // TODO set l0 properly
    Trk::HomogeneousLayerMaterial objectLayerMaterial(objectMaterial, 1.0);

    Trk::CylinderBounds* objectBounds    = new Trk::CylinderBounds(radius[i], hlength[i]);

    // log << MSG::DEBUG << "DD barrel Layer constructed with material-properties: " << objectMaterial << endreq;
    // CylinderLayer takes ownership of the transformation.
    thelayer->push_back(new Trk::CylinderLayer(new Amg::Transform3D(htrans),
                                               objectBounds, objectLayerMaterial));
  }

  return result;
}

bool
CaloSurfaceBuilder::CreateDDECLayers(CaloSubdetNames::ALIGNVOL alvol, std::vector< Trk::DiscLayer*>* thelayer)
{
  bool result = false;
  thelayer->clear();

  Amg::Transform3D theTrans(Trk::s_idTransform);
  double hphi;
  std::vector<double> depth;
  std::vector<double> z;
  std::vector<double> rmin;
  std::vector<double> rmax;
  double mass, volume, x0, dEdx, aveA, aveZ;

  // the material
  result = m_lar_mat->get_material(alvol, mass, volume, x0, dEdx, aveA, aveZ);

  // the layers
  Amg::Transform3D* ptrans = &theTrans;
  result = this->get_disk_surface(alvol, ptrans, hphi, z, depth, rmin, rmax);
    if (!result){
      return 0;
    }

  for ( unsigned int i = 0; i<z.size(); i++ ) {

    // Fix the z ( convention is different in Calo's and Tracking )
    Amg::Transform3D* eachtrans = new Amg::Transform3D(Trk::s_idTransform);
    Amg::Translation3D vec(0.,0.,z[i] - theTrans(2,3));
    *eachtrans = theTrans * vec;

    Trk::MaterialProperties objectMaterial(depth[i], x0, 3*x0/aveZ, aveA, aveZ, mass/volume);   // TODO: set l0 properly
    Trk::HomogeneousLayerMaterial objectLayerMaterial(objectMaterial, 1.0);

    Trk::DiscBounds* objectBounds    = new Trk::DiscBounds(rmin[i], rmax[i] );

    // log << MSG::DEBUG << "DD endcap Layer constructed with material-properties: " << objectMaterial << endreq;
    thelayer->push_back(new Trk::DiscLayer(eachtrans, objectBounds, objectLayerMaterial));
  }

  return result;
}

void
CaloSurfaceBuilder::setCaloDepth(CaloDepthTool* /*mytool*/ )
{
  // This allows clients who already have a CaloDepthTool to use it
  // and be sure that the properties set via jobOpt are used consistently
  //
  // For clients who do not have any, the default will be retrieved and used

  ATH_MSG_WARNING("The client doesn't need to set CaloDepthTool anymore!!! ");
 //  if(mytool)
//     m_calodepth = mytool ;
//   else {
//     StatusCode s = m_toolsvc->retrieveTool("CaloDepthTool",m_calodepth,this);
//     if (s.isFailure())
//       log<<MSG::ERROR<<"Could not find CaloDepthTool " <<endreq;
//   }
}

CaloDepthTool*
CaloSurfaceBuilder::getCaloDepth()
{
 return &(*m_calodepth); // from ToolHandle to pointer
}

// End of the general user methods :

bool
CaloSurfaceBuilder::get_cylinder_surface (CaloCell_ID::CaloSample sample, int side,
					  Amg::Transform3D* htrans,
					  double& radius, double& hphi,
					  double& hlength, double& depth)
{

  //MsgStream log(m_msgSvc, name());

  bool result = false;

  if (!m_calo_dd) return result;

  // strips are spread on several descriptor, which all have the same
  // htrans, radius, hphi, but not the same hlength
  double hl;
  hlength = 0;

  // these ones are definitely not cylinders :
  if ( sample == CaloCell_ID::PreSamplerE )  return false;
  if ( sample == CaloCell_ID::EME1 )  return false;
  if ( sample == CaloCell_ID::EME2 )  return false;
  if ( sample == CaloCell_ID::EME3 )  return false;
  if ( sample == CaloCell_ID::HEC0 )  return false;
  if ( sample == CaloCell_ID::HEC1 )  return false;
  if ( sample == CaloCell_ID::HEC2 )  return false;
  if ( sample == CaloCell_ID::HEC3 )  return false;
  if ( sample == CaloCell_ID::FCAL0 )  return false;
  if ( sample == CaloCell_ID::FCAL1 )  return false;
  if ( sample == CaloCell_ID::FCAL2 )  return false;
  if ( sample == CaloCell_ID::Unknown )  return false;
  if ( sample == CaloCell_ID::TileGap3 ) return false;

  //std::cout << "  cylinder work ..." << std::endl;

  if ( sample == CaloCell_ID::PreSamplerB ||
       sample == CaloCell_ID::EMB1 ||
       sample == CaloCell_ID::EMB2 ||
       sample == CaloCell_ID::EMB3 ) {

    std::vector <CaloDetDescriptor*>::const_iterator  first = m_calo_dd->calo_descriptors_begin();
    std::vector <CaloDetDescriptor*>::const_iterator  last = m_calo_dd->calo_descriptors_end();

    for (; first != last; ++first) {
      CaloDetDescriptor* reg = *first;
      if (reg) {
        if ( reg->getSampling(0) == sample && reg->calo_sign()*side > 0)
          {
            result = reg->get_cylinder_surface(htrans,radius,hphi,hl,depth);
            if (hl > hlength) hlength = hl;
            //std::cout << " got a cylinder, radius= " << radius << ", length " << hl<<", depth:"<< depth 
	    //      << " side " << reg->calo_sign()<<", transform:"<<htrans->translation()<<","<<htrans->rotation() << std::endl;
          }
      }
    }
  }

  else{
    //log<<MSG::VERBOSE<<" get_cylinder_surface  in TILES sample   "<<sample <<"  side  "<<side<<endreq;
    //this are not cylinders

    switch (sample) {
    case CaloCell_ID::TileBar0:
    case CaloCell_ID::TileBar1:
    case CaloCell_ID::TileBar2:
    case CaloCell_ID::TileExt0:
    case CaloCell_ID::TileExt1:
    case CaloCell_ID::TileExt2:
    case CaloCell_ID::TileGap1:
    case CaloCell_ID::TileGap2:
      break;
    default:
      ATH_MSG_ERROR(" get_cylinder_surface Not Cylindrical Tile found!!!   ");
      return 0;
    }

    std::vector <CaloDetDescriptor*>::const_iterator  first = m_calo_dd->tile_descriptors_begin();
    std::vector <CaloDetDescriptor*>::const_iterator  last = m_calo_dd->tile_descriptors_end();
    for (; first != last; ++first) {
      CaloDetDescriptor* reg = *first;
      if (reg) {
        if ( reg->getSampling(0) == sample && reg->calo_sign()*side > 0){
	  /*result =*/(void) reg->get_cylinder_surface(htrans,radius,hphi,hl,depth);
	  if (hl > hlength) hlength = hl;
	  hphi    = .5*reg->dphi()*reg->n_phi();
	}
      }
    }
    //log<<MSG::DEBUG<<" get_cylinder_surface with hphi: "<<hphi <<"  radius: "<<radius<<"  hlength: "<<hlength<<endreq;
    result = true;
  }
  return result;
}


bool
CaloSurfaceBuilder::get_disk_surface (CaloCell_ID::CaloSample sample, int side,
				      Amg::Transform3D* htrans, double& z,
				      double& rmin,
				      double& rmax, double& hphisec, double& depth)
{
  //MsgStream log(m_msgSvc, name());

  // set defaults :
  bool result = false;
  z = 0.;
  rmin = 999999.;
  rmax = 0.;

  if (!m_calo_dd) return result;

  // strips are spread on several descriptor, which all have the same
  // htrans, hphisec, but not the same rmin and rmax
  // + for EMEC2 and EMEC3, the InnerWeel entrance z is different :-(
  // ==> as CaloCell_ID::CaloSample does not allow to distinguish,
  //     take OW z and fix the HepTransform accordingly

  // these ones are definitely not disks :
  if ( sample == CaloCell_ID::PreSamplerB )  return false;
  if ( sample == CaloCell_ID::EMB1 )  return false;
  if ( sample == CaloCell_ID::EMB2 )  return false;
  if ( sample == CaloCell_ID::EMB3 )  return false;
  if ( sample == CaloCell_ID::TileBar0 )  return false;
  if ( sample == CaloCell_ID::TileBar1 )  return false;
  if ( sample == CaloCell_ID::TileBar2 )  return false;
  if ( sample == CaloCell_ID::TileGap1 )  return false;
  if ( sample == CaloCell_ID::TileGap2 )  return false;
  if ( sample == CaloCell_ID::TileExt0 )  return false;
  if ( sample == CaloCell_ID::TileExt1 )  return false;
  if ( sample == CaloCell_ID::TileExt2 )  return false;

  double ri, ra, zow;
  //log<<MSG::VERBOSE<<" get_disk_surface in sample   "<<sample<<endreq;
  //std::cout << "  disk work ..." << std::endl;

  std::vector <CaloDetDescriptor*>::const_iterator  first = m_calo_dd->calo_descriptors_begin();
  std::vector <CaloDetDescriptor*>::const_iterator  last = m_calo_dd->calo_descriptors_end();

  for (; first != last; ++first) {
    CaloDetDescriptor* reg = *first;
    if (reg) {
      if ( reg->getSampling(0) == sample && reg->calo_sign()*side > 0 )
	{
	  result = reg->get_disk_surface(htrans,zow,ri,ra,hphisec,depth);
	  if ( !reg->is_lar_em_endcap_inner() ) z = zow;
	  if ( ri < rmin ) rmin = ri;
	  if ( ra > rmax ) rmax = ra;
	}
    }
  }

  if ( sample == CaloCell_ID::TileGap3 ){
    //log<<MSG::VERBOSE<<" get_disk_surface in TileGap3 sample   "<<sample<<endreq;

    std::vector <CaloDetDescriptor*>::const_iterator  first = m_calo_dd->tile_descriptors_begin();
    std::vector <CaloDetDescriptor*>::const_iterator  last = m_calo_dd->tile_descriptors_end();

    for (; first != last; ++first) {
      CaloDetDescriptor* reg = *first;
      if (reg) {
	if ( reg->getSampling(0) == sample && reg->calo_sign()*side > 0 )
	  {
	    result = reg->get_disk_surface(htrans,zow,ri,ra,hphisec,depth);
	    if ( !reg->is_lar_em_endcap_inner() ) z = zow;
	    if ( ri < rmin ) rmin = ri;
	    if ( ra > rmax ) rmax = ra;
	  }
      }
    }
    //log<<MSG::VERBOSE<<" get_disk_surface in TileGap3 sample   "<<sample<<endreq;
  }

  if ( rmin > rmax ) rmin = 0.;
  // Fix the z ( convention is different in Calo's and Tracking )
  Amg::Translation3D vec(0.,0.,z - (*htrans)(2,3));
  *htrans = (*htrans) * vec;

  //log<<MSG::DEBUG<<" get_disk_surface  with hphi: "<<hphisec <<"  rmin: "<<rmin<<"  rmax: "<<rmax<<
  //  "   z: "<<z<<"   depth: "<<depth<<endreq;

  return result;
}


bool
CaloSurfaceBuilder::get_cylinder_surface (CaloSubdetNames::ALIGNVOL alvol,
                                          Amg::Transform3D* htrans,double& hphi,
					  std::vector<double>& radius,
					  std::vector<double>& depth,
					  std::vector<double>& hlength )
{
  bool result = m_lar_simplegeom->get_cylinder_surface(alvol, *htrans, hphi, radius, depth, hlength);

  // Shift the POS/NEG sides  ( htrans convention is different in Calo's and Tracking )
  //    FIXME : for this, assume that the 1rst hlength is the good one

  if ( alvol == CaloSubdetNames::EMB_POS ||  alvol == CaloSubdetNames::PRESAMPLER_B_POS ) {
    result = m_lar_simplegeom->get_cylinder_surface(alvol, *htrans, hphi, radius, depth, hlength);
    Amg::Translation3D vec(0.,0.,hlength[0]);
    *htrans = (*htrans) * vec;
    return result;
  }
  else if ( alvol == CaloSubdetNames::EMB_NEG ||  alvol == CaloSubdetNames::PRESAMPLER_B_NEG ) {
    result = m_lar_simplegeom->get_cylinder_surface(alvol, *htrans, hphi, radius, depth, hlength);
    Amg::Translation3D vec(0.,0.,-1.*hlength[0]);
    *htrans = (*htrans) * vec;
    return result;
  }

  return result;
}

bool
CaloSurfaceBuilder::get_disk_surface (CaloSubdetNames::ALIGNVOL alvol,
                                      Amg::Transform3D* htrans,
				      double& hphi,
				      std::vector<double>& z,
				      std::vector<double>& depth,
				      std::vector<double>& rmin, std::vector<double>& rmax)
{
  bool result = m_lar_simplegeom->get_disk_surface(alvol, *htrans, hphi, z, depth, rmin, rmax );

  // Fix the z ( htrans convention is different in Calo's and Tracking ) ?
  //   = left to the CreateDDEClayer method, because z is a vector => 1 fix per disk element

  return result;
}




//Write all the surfaces into a vector for debugging porposes
std::vector<const Trk::Surface*> CaloSurfaceBuilder::allSampleSurfaces()
{
  std::vector<const Trk::Surface*> surfaces;
  Trk::Surface * surf;
  for (CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB; sample< CaloCell_ID::Unknown; sample = CaloCell_ID::CaloSample(sample+1) ){
    surf = CreateUserSurface(sample,0.,-1.);
    if(surf)
      surfaces.push_back(surf);
    surf = CreateUserSurface(sample,0.,1.);
    if(surf)
      surfaces.push_back(surf);

    if ( sample == CaloCell_ID::TileBar2 ||  sample == CaloCell_ID::TileExt2 ||  sample == CaloCell_ID::TileGap3 || sample == CaloCell_ID::HEC3   ){
      surf = CreateLastSurface(sample,0.,-1.);
      if(surf)
	surfaces.push_back(surf);
      surf = CreateLastSurface(sample,0.,1.);
      if(surf)
	surfaces.push_back(surf);
    }
  }
  return (surfaces);
}



//Write all TileBar surfaces into a vector for debugging porposes
std::vector<const Trk::Surface*> CaloSurfaceBuilder::allTileBarSurfaces()
{
  std::vector<const Trk::Surface*> surfaces;
  Trk::Surface * surf;
  for (CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB; sample < CaloCell_ID::Unknown; sample = CaloCell_ID::CaloSample(sample+1) ){
    if ( sample == CaloCell_ID::TileBar0 ||
	 sample == CaloCell_ID::TileBar1 ||
	 sample == CaloCell_ID::TileBar2  ){
      surf = CreateUserSurface(sample,0.,-1.);
      if(surf)
	surfaces.push_back(surf);
      surf = CreateUserSurface(sample,0.,1.);
      if(surf)
	surfaces.push_back(surf);

      if ( sample == CaloCell_ID::TileBar2 ||  sample == CaloCell_ID::TileExt2 ||  sample == CaloCell_ID::TileGap3  ){
	surf = CreateLastSurface(sample,0.,-1.);
	if(surf)
	  surfaces.push_back(surf);
	surf = CreateLastSurface(sample,0.,1.);
	if(surf)
	  surfaces.push_back(surf);
      }
    }
  }
  return (surfaces);
}


//Write all TileBar surfaces into a vector for debugging porposes
std::vector<const Trk::Surface*> CaloSurfaceBuilder::allTileExtSurfaces()
{
  std::vector<const Trk::Surface*> surfaces;
  Trk::Surface * surf;
  for (CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB; sample < CaloCell_ID::Unknown; sample = CaloCell_ID::CaloSample(sample+1) ){
    if (
	 sample == CaloCell_ID::TileExt0 ||
	 sample == CaloCell_ID::TileExt1 ||
	 sample == CaloCell_ID::TileExt2 ){
      surf = CreateUserSurface(sample,0.,-1.);
      if(surf)
	surfaces.push_back(surf);
      surf = CreateUserSurface(sample,0.,1.);
      if(surf)
	surfaces.push_back(surf);

      if ( sample == CaloCell_ID::TileBar2 ||  sample == CaloCell_ID::TileExt2 ||  sample == CaloCell_ID::TileGap3  ){
	surf = CreateLastSurface(sample,0.,-1.);
	if(surf)
	  surfaces.push_back(surf);
	surf = CreateLastSurface(sample,0.,1.);
	if(surf)
	  surfaces.push_back(surf);
      }
    }
  }
  return (surfaces);
}

//Write all TileBar surfaces into a vector for debugging porposes
std::vector<const Trk::Surface*> CaloSurfaceBuilder::allTileGapSurfaces()
{
  std::vector<const Trk::Surface*> surfaces;
  Trk::Surface * surf;
  for (CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB; sample < CaloCell_ID::Unknown; sample = CaloCell_ID::CaloSample(sample+1) ){
    if (
	 sample == CaloCell_ID::TileGap1 ||
	 sample == CaloCell_ID::TileGap2 ||
	 sample == CaloCell_ID::TileGap3 ){
      surf = CreateUserSurface(sample,0.,-1.);
      if(surf)
	surfaces.push_back(surf);
      surf = CreateUserSurface(sample,0.,1.);
      if(surf)
	surfaces.push_back(surf);

      if ( sample == CaloCell_ID::TileBar2 ||  sample == CaloCell_ID::TileExt2 ||  sample == CaloCell_ID::TileGap3  ){
	surf = CreateLastSurface(sample,0.,-1.);
	if(surf)
	  surfaces.push_back(surf);
	surf = CreateLastSurface(sample,0.,1.);
	if(surf)
	  surfaces.push_back(surf);
      }
    }
  }
  return (surfaces);
}


//Write all EMB surfaces into a vector for debugging porposes
std::vector<const Trk::Surface*> CaloSurfaceBuilder::allEMBSurfaces()
{
  std::vector<const Trk::Surface*> surfaces;
  Trk::Surface * surf;
  for (CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB; sample< CaloCell_ID::Unknown; sample = CaloCell_ID::CaloSample(sample+1) ){
    if ( sample == CaloCell_ID::EMB1 ||
	 sample == CaloCell_ID::EMB2 ||
	 sample == CaloCell_ID::EMB3 ){
      surf = CreateUserSurface(sample,0.,-1.);
      if(surf)
	surfaces.push_back(surf);
      surf = CreateUserSurface(sample,0.,1.);
      if(surf)
	surfaces.push_back(surf);
    }
  }
  return (surfaces);
}


//Write all EME surfaces into a vector for debugging porposes
std::vector<const Trk::Surface*> CaloSurfaceBuilder::allEMESurfaces()
{
  std::vector<const Trk::Surface*> surfaces;
  Trk::Surface * surf;
  for (CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB; sample< CaloCell_ID::Unknown; sample = CaloCell_ID::CaloSample(sample+1) ){
    if ( sample == CaloCell_ID::EME1 ||
	 sample == CaloCell_ID::EME2 ||
	 sample == CaloCell_ID::EME3 ){
      surf = CreateUserSurface(sample,0.,-1.);
      if(surf)
	surfaces.push_back(surf);
      surf = CreateUserSurface(sample,0.,1.);
      if(surf)
	surfaces.push_back(surf);
    }
  }
  return (surfaces);
}


//Write all HEC surfaces into a vector for debugging porposes
std::vector<const Trk::Surface*> CaloSurfaceBuilder::allHECSurfaces()
{
  std::vector<const Trk::Surface*> surfaces;
  Trk::Surface * surf;
  for (CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB; sample< CaloCell_ID::Unknown; sample = CaloCell_ID::CaloSample(sample+1) ){
    if ( sample == CaloCell_ID::HEC0 ||
	 sample == CaloCell_ID::HEC1 ||
	 sample == CaloCell_ID::HEC2 ||
	 sample == CaloCell_ID::HEC3 ){
      surf = CreateUserSurface(sample,0.,-1.);
      if(surf)
	surfaces.push_back(surf);
      surf = CreateUserSurface(sample,0.,1.);
      if(surf)
	surfaces.push_back(surf);

      if ( sample == CaloCell_ID::HEC3  ){
	surf = CreateLastSurface(sample,0.,-1.);
	if(surf)
	  surfaces.push_back(surf);
	surf = CreateLastSurface(sample,0.,1.);
	if(surf)
	  surfaces.push_back(surf);
      }

    }
  }
  return (surfaces);
}

//store all the surfaces into a vector 
void CaloSurfaceBuilder::fill_tg_surfaces()
{
  //for (float eta=1.4; eta<3.2; eta+=0.1 ) {
  //  const Trk::Surface* surf = CreateUserSurface(CaloCell_ID::EME2,0.,eta);
  //  const Trk::Surface* sl = CreateUserSurface(CaloCell_ID::EME3,0.,eta);
  //  std::cout <<"surface r (EME2-3):"<< eta<<":"<<surf->center().z()<<","<<( sl? sl->center().z() : 0.) <<std::endl;
  //}

  // entry surfaces ( id<24 to avoid error messages )
  for (CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB; sample< 24; sample = CaloCell_ID::CaloSample(sample+1) ){
    float etaRef = ( sample<4 || std::abs(sample-13)<2 ) ? 1. : 2. ;
    const Trk::Surface* spos = CreateUserSurface(sample,0.,etaRef);
    const Trk::Surface* sneg = CreateUserSurface(sample,0.,-etaRef);
    if (spos) spos->setOwner(Trk::TGOwn);
    if (sneg) sneg->setOwner(Trk::TGOwn);
    m_layerEntries.push_back(std::pair<const Trk::Surface*,const Trk::Surface*>(spos,sneg));

    // if (spos && sneg) std::cout<<"CaloSurf:translation:"<<sample<<","<<spos->transform().translation()<<","<<sneg->transform().translation()<< std::endl; 
    //if (spos && sneg) std::cout<<"CaloSurf:rotation:"<<sample<<","<<spos->transform().rotation()<<","<<sneg->transform().rotation()<< std::endl; 
  }
  // turn into SlidingSurfaces 
  // EMB2 neg
  float etaMin = -1.5; float etaMax = 0.; float deta = 0.05;
  unsigned int neta = (etaMax-etaMin)/deta; std::vector<float> offset(neta);
  float rbase =  m_layerEntries[CaloCell_ID::EMB2].second->bounds().r();
  for (unsigned int ie=0; ie<neta; ie++ ) {
    float eta = etaMin+(ie+0.5)*deta;
    const Trk::Surface* surf = CreateUserSurface(CaloCell_ID::EMB2,0.,eta);
    offset[ie] = surf->bounds().r()- rbase;
    delete surf;  
  }
  const Trk::CylinderSurface* cyl = dynamic_cast<const Trk::CylinderSurface*> (m_layerEntries[CaloCell_ID::EMB2].second);
  m_layerEntries[CaloCell_ID::EMB2].second = new Trk::SlidingCylinderSurface(*cyl,
							                   new Trk::BinUtility(neta,etaMin,etaMax,Trk::open,Trk::binEta),
									   new std::vector<float> (offset));
  m_layerEntries[CaloCell_ID::EMB2].second->setOwner(Trk::TGOwn);
  delete cyl;
  // EMB2 pos
  etaMin = 0.; etaMax = 1.5; deta = 0.05;
  neta = (etaMax-etaMin)/deta; offset.resize(neta);
  rbase =  m_layerEntries[CaloCell_ID::EMB2].first->bounds().r();
  for (unsigned int ie=0; ie<neta; ie++ ) {
    float eta = etaMin+(ie+0.5)*deta;
    const Trk::Surface* surf = CreateUserSurface(CaloCell_ID::EMB2,0.,eta);
    offset[ie] = surf->bounds().r()- rbase;
    delete surf;  
  }
  cyl = dynamic_cast<const Trk::CylinderSurface*> (m_layerEntries[CaloCell_ID::EMB2].first);
  m_layerEntries[CaloCell_ID::EMB2].first = new Trk::SlidingCylinderSurface(*cyl,
									    new Trk::BinUtility(neta,etaMin,etaMax,Trk::open,Trk::binEta),
									    new std::vector<float> (offset));
  m_layerEntries[CaloCell_ID::EMB2].first->setOwner(Trk::TGOwn);
  delete cyl;
  // EMB3 neg
  etaMin = -1.5; etaMax = 0.; deta = 0.05;
  neta = (etaMax-etaMin)/deta; offset.resize(neta);
  rbase =  m_layerEntries[CaloCell_ID::EMB3].second->bounds().r();
  for (unsigned int ie=0; ie<neta; ie++ ) {
    float eta = etaMin+(ie+0.5)*deta;
    const Trk::Surface* surf = CreateUserSurface(CaloCell_ID::EMB3,0.,eta);
    offset[ie] = surf->bounds().r()- rbase;
    delete surf;  
  }
  cyl = dynamic_cast<const Trk::CylinderSurface*> (m_layerEntries[CaloCell_ID::EMB3].second);
  m_layerEntries[CaloCell_ID::EMB3].second = new Trk::SlidingCylinderSurface(*cyl,
						      new Trk::BinUtility(neta,etaMin,etaMax,Trk::open,Trk::binEta),
						      new std::vector<float> (offset));
  m_layerEntries[CaloCell_ID::EMB3].second->setOwner(Trk::TGOwn);
  delete cyl;

  // EMB3 pos
  etaMin = 0.; etaMax = 1.5; deta = 0.05;
  neta = (etaMax-etaMin)/deta; offset.resize(neta);
  rbase =  m_layerEntries[CaloCell_ID::EMB3].first->bounds().r();
  for (unsigned int ie=0; ie<neta; ie++ ) {
    float eta = etaMin+(ie+0.5)*deta;
    const Trk::Surface* surf = CreateUserSurface(CaloCell_ID::EMB3,0.,eta);
    offset[ie] = surf->bounds().r()- rbase;
    delete surf;  
  }
  cyl = dynamic_cast<const Trk::CylinderSurface*> (m_layerEntries[CaloCell_ID::EMB3].first);
  m_layerEntries[CaloCell_ID::EMB3].first = new Trk::SlidingCylinderSurface(*cyl,
						      new Trk::BinUtility(neta,etaMin,etaMax,Trk::open,Trk::binEta),
						     new std::vector<float> (offset));
  m_layerEntries[CaloCell_ID::EMB3].first->setOwner(Trk::TGOwn);
  delete cyl;

  // EME2 neg
  etaMin = -3.2; etaMax = -1.35; deta = 0.05;
  neta = (etaMax-etaMin)/deta; offset.resize(neta);
  float zbase =  m_layerEntries[CaloCell_ID::EME2].second->center().z();
  for (unsigned int ie=0; ie<neta; ie++ ) {
    float eta = etaMin+(ie+0.5)*deta;
    const Trk::Surface* surf = CreateUserSurface(CaloCell_ID::EME2,0.,eta);
    offset[ie] = surf->center().z()- zbase;
    delete surf;  
  }
  const Trk::DiscSurface* disc = dynamic_cast<const Trk::DiscSurface*> (m_layerEntries[CaloCell_ID::EME2].second);
  m_layerEntries[CaloCell_ID::EME2].second = new Trk::SlidingDiscSurface(*disc,
									 new Trk::BinUtility(neta,etaMin,etaMax,Trk::open,Trk::binEta),
									 new std::vector<float> (offset));
  m_layerEntries[CaloCell_ID::EME2].second->setOwner(Trk::TGOwn);
  delete disc;

  // EME2 pos
  etaMin = 1.35; etaMax = 3.2; deta = 0.05;
  neta = (etaMax-etaMin)/deta; offset.resize(neta);
  zbase =  m_layerEntries[CaloCell_ID::EME2].first->center().z();
  for (unsigned int ie=0; ie<neta; ie++ ) {
    float eta = etaMin+(ie+0.5)*deta;
    const Trk::Surface* surf = CreateUserSurface(CaloCell_ID::EME2,0.,eta);
    offset[ie] = surf->center().z()- zbase;
    delete surf;  
  }
  disc = dynamic_cast<const Trk::DiscSurface*> (m_layerEntries[CaloCell_ID::EME2].first);
  m_layerEntries[CaloCell_ID::EME2].first = new Trk::SlidingDiscSurface(*disc,
									new Trk::BinUtility(neta,etaMin,etaMax,Trk::open,Trk::binEta),
									new std::vector<float> (offset));
  m_layerEntries[CaloCell_ID::EME2].first->setOwner(Trk::TGOwn);
  delete disc;

  // EME3 neg
  etaMin = -3.2; etaMax = -1.35; deta = 0.05;
  neta = (etaMax-etaMin)/deta; offset.resize(neta);
  zbase =  m_layerEntries[CaloCell_ID::EME3].second->center().z();
  for (unsigned int ie=0; ie<neta; ie++ ) {
    float eta = etaMin+(ie+0.5)*deta;
    const Trk::Surface* surf = CreateUserSurface(CaloCell_ID::EME3,0.,eta);
    offset[ie] = surf->center().z()- zbase;
    delete surf;  
  }
  disc = dynamic_cast<const Trk::DiscSurface*> (m_layerEntries[CaloCell_ID::EME3].second);
  m_layerEntries[CaloCell_ID::EME3].second = new Trk::SlidingDiscSurface(*disc,
									 new Trk::BinUtility(neta,etaMin,etaMax,Trk::open,Trk::binEta),
									 new std::vector<float> (offset));
  m_layerEntries[CaloCell_ID::EME3].second->setOwner(Trk::TGOwn);
  delete disc;

  // EME3 pos
  etaMin = 1.35; etaMax = 3.2; deta = 0.05;
  neta = (etaMax-etaMin)/deta; offset.resize(neta);
  zbase =  m_layerEntries[CaloCell_ID::EME3].first->center().z();
  for (unsigned int ie=0; ie<neta; ie++ ) {
    float eta = etaMin+(ie+0.5)*deta;
    const Trk::Surface* surf = CreateUserSurface(CaloCell_ID::EME3,0.,eta);
    offset[ie] = surf->center().z()- zbase;
    delete surf;  
  }
  disc = dynamic_cast<const Trk::DiscSurface*> (m_layerEntries[CaloCell_ID::EME3].first);
  m_layerEntries[CaloCell_ID::EME3].first = new Trk::SlidingDiscSurface(*disc,
									new Trk::BinUtility(neta,etaMin,etaMax,Trk::open,Trk::binEta),
									new std::vector<float> (offset));
  m_layerEntries[CaloCell_ID::EME3].first->setOwner(Trk::TGOwn);
  delete disc;

  // exit surfaces
  for (CaloCell_ID::CaloSample sample = CaloCell_ID::PreSamplerB; sample+1< CaloCell_ID::Unknown; sample = CaloCell_ID::CaloSample(sample+1) ){
    m_layerExits.push_back(m_layerEntries[sample+1]);
  }

  // fix exit for outer layers
  const Trk::Surface* lpos =  CreateLastSurface(CaloCell_ID::TileBar2,0.,1.);
  const Trk::Surface* lneg =  CreateLastSurface(CaloCell_ID::TileBar2,0.,-1.);
  if (lpos) lpos->setOwner(Trk::TGOwn);
  if (lneg) lneg->setOwner(Trk::TGOwn);
  m_layerExits[CaloCell_ID::TileBar2]=std::pair<const Trk::Surface*,const Trk::Surface*>(lpos,lneg);

  lpos =  CreateLastSurface(CaloCell_ID::TileExt2,0.,1.);
  lneg =  CreateLastSurface(CaloCell_ID::TileExt2,0.,-1.);
  if (lpos) lpos->setOwner(Trk::TGOwn);
  lneg->setOwner(Trk::TGOwn);
  m_layerExits[CaloCell_ID::TileExt2]=std::pair<const Trk::Surface*,const Trk::Surface*>(lpos,lneg);

  lpos =  CreateLastSurface(CaloCell_ID::TileGap2,0.,1.);
  lneg =  CreateLastSurface(CaloCell_ID::TileGap2,0.,-1.);
  if (lpos) lpos->setOwner(Trk::TGOwn);
  if (lneg) lneg->setOwner(Trk::TGOwn);
  m_layerExits[CaloCell_ID::TileGap2]=std::pair<const Trk::Surface*,const Trk::Surface*>(lpos,lneg);

  lpos =  CreateLastSurface(CaloCell_ID::TileGap3,0.,1.);
  lneg =  CreateLastSurface(CaloCell_ID::TileGap3,0.,-1.);
  if (lpos) lpos->setOwner(Trk::TGOwn);
  if (lneg) lneg->setOwner(Trk::TGOwn);
  m_layerExits[CaloCell_ID::TileGap3]=std::pair<const Trk::Surface*,const Trk::Surface*>(lpos,lneg);

  lpos =  CreateLastSurface(CaloCell_ID::HEC3,0.,1.);
  lneg =  CreateLastSurface(CaloCell_ID::HEC3,0.,-1.);
  if (lpos) lpos->setOwner(Trk::TGOwn);
  if (lneg) lneg->setOwner(Trk::TGOwn);
  m_layerExits[CaloCell_ID::HEC3]=std::pair<const Trk::Surface*,const Trk::Surface*>(lpos,lneg);

}


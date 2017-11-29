/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonStationBuilder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// Muon
#include "MuonTrackingGeometry/MuonStationBuilder.h"
#include "MuonTrackingGeometry/MuonStationTypeBuilder.h"
//MuonSpectrometer include
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/ITrackingVolumeArrayCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkDetDescrInterfaces/IDetachedTrackingVolumeBuilder.h"
#include "TrkDetDescrUtils/BinUtility.h"
#include "TrkDetDescrUtils/BinnedArray.h"
#include "TrkDetDescrUtils/GeometryStatics.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/TrapezoidVolumeBounds.h"
#include "TrkVolumes/DoubleTrapezoidVolumeBounds.h"
#include "TrkVolumes/SimplePolygonBrepVolumeBounds.h"
#include "TrkVolumes/BoundarySurface.h"
#include "TrkVolumes/BoundarySurfaceFace.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include<fstream>
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoCons.h"
//mw
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoTrap.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoPara.h"
#include "GeoModelKernel/GeoVolumeCursor.h"
// StoreGate
#include "StoreGate/StoreGateSvc.h"

// STD
#include <map>

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoVolumeCursor.h"

// constructor
Muon::MuonStationBuilder::MuonStationBuilder(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p),
  m_muonMgr(0),
  m_mdtIdHelper(0),
  m_rpcIdHelper(0),
  m_cscIdHelper(0),
  m_tgcIdHelper(0),
  m_stgcIdHelper(0),
  m_mmIdHelper(0),
  m_muonMgrLocation("MuonMgr"),
  m_muonStationTypeBuilder("Muon::MuonStationTypeBuilder/MuonStationTypeBuilder"),
  m_trackingVolumeHelper("Trk::TrackingVolumeHelper/TrackingVolumeHelper"),
  m_geoShapeConverter(0),
  m_materialConverter(0),
  m_buildBarrel(true),
  m_buildEndcap(true),
  m_buildCsc(true),
  m_buildTgc(true),
  m_resolveActiveLayers(true)
{
  declareInterface<Trk::IDetachedTrackingVolumeBuilder>(this);
  declareProperty("StationTypeBuilder",               m_muonStationTypeBuilder);
  declareProperty("MuonDetManagerLocation",           m_muonMgrLocation);
  declareProperty("BuildBarrelStations",              m_buildBarrel);
  declareProperty("BuildEndcapStations",              m_buildEndcap);
  declareProperty("BuildCSCStations",                 m_buildCsc);
  declareProperty("BuildTGCStations",                 m_buildTgc);
  declareProperty("ResolveActiveLayers",              m_resolveActiveLayers);
}

// destructor
Muon::MuonStationBuilder::~MuonStationBuilder()
{}

// Athena standard methods
// initialize
StatusCode Muon::MuonStationBuilder::initialize()
{

    // get Muon Spectrometer Description Manager
    ATH_CHECK( detStore()->retrieve(m_muonMgr) );
    
    m_mdtIdHelper = m_muonMgr-> mdtIdHelper(); 
    m_rpcIdHelper = m_muonMgr-> rpcIdHelper();
    m_cscIdHelper = m_muonMgr-> cscIdHelper();
    m_tgcIdHelper = m_muonMgr-> tgcIdHelper();
    m_stgcIdHelper = m_muonMgr-> stgcIdHelper();
    m_mmIdHelper  = m_muonMgr-> mmIdHelper();

    ATH_MSG_INFO( m_muonMgr->geometryVersion() ); 
    
    // Retrieve the tracking volume helper   -------------------------------------------------    
    ATH_CHECK( m_trackingVolumeHelper.retrieve() );
    ATH_MSG_INFO( "Retrieved tool " << m_trackingVolumeHelper );

    // Retrieve muon station builder tool   -------------------------------------------------    
    ATH_CHECK( m_muonStationTypeBuilder.retrieve() );
    ATH_MSG_INFO( "Retrieved tool " << m_muonStationTypeBuilder );

    // if no muon materials are declared, take default ones
  
    m_muonMaterial = Trk::Material(10e10,10e10,0.,0.,0.);      // default material properties

    m_materialConverter= new Trk::GeoMaterialConverter();
    m_geoShapeConverter= new Trk::GeoShapeConverter();

    ATH_MSG_INFO( name() <<" initialize() successful" );    
    
  return StatusCode::SUCCESS;
}

const std::vector<const Trk::DetachedTrackingVolume*>* Muon::MuonStationBuilder::buildDetachedTrackingVolumes(bool blend)
 const
{

  std::vector<const Trk::DetachedTrackingVolume*> mStations;

  if (m_muonMgr) { 
    // retrieve muon station prototypes from GeoModel
    const std::vector<const Trk::DetachedTrackingVolume*>* msTypes = buildDetachedTrackingVolumeTypes(blend);
    //const std::vector<const Trk::DetachedTrackingVolume*>* msTypes=new std::vector<const Trk::DetachedTrackingVolume*>;
    std::vector<const Trk::DetachedTrackingVolume*>::const_iterator msTypeIter = msTypes->begin();
    

    // position MDT chambers by repeating loop over muon tree
    // link to top tree
    const GeoVPhysVol* top = &(*(m_muonMgr->getTreeTop(0)));
    GeoVolumeCursor vol(top);
    while (!vol.atEnd())
    {
      const GeoVPhysVol* cv = &(*(vol.getVolume()));
      const GeoLogVol* clv = cv->getLogVol();
      std::string vname = clv->getName();
      
      // special treatment for NSW
      if (vname.substr(0,3) =="NSW" || vname.substr(0,8) =="NewSmall") {
	ATH_MSG_INFO( vname <<" processing NSW " );
	std::vector<std::pair<const Trk::DetachedTrackingVolume*,std::vector<Amg::Transform3D> > > objs;
	
	std::vector<const GeoShape*> input_shapes;
	std::vector<std::pair<std::pair<const GeoLogVol*,Trk::MaterialProperties*>,std::vector<Amg::Transform3D> > > vols;
	std::vector<std::string> volNames;
	
	bool simpleTree = false;
	if ( !cv->getNChildVols() ) {
	  std::vector<Amg::Transform3D > volTr;
	  volTr.push_back(Amg::CLHEPTransformToEigen(vol.getTransform())); 
	  std::pair<const GeoLogVol*,Trk::MaterialProperties*> cpair(clv,0);
	  vols.push_back(std::pair<std::pair<const GeoLogVol*,Trk::MaterialProperties*>,std::vector<Amg::Transform3D> > (cpair,volTr) );
          volNames.push_back(vname);
	  simpleTree = true;
	} else {
	  getObjsForTranslation(cv,"",Amg::Transform3D(Trk::s_idTransform),vols,volNames );
        }
	input_shapes.resize(vols.size());             
	for (unsigned int i=0;i<vols.size();i++) input_shapes[i]=vols[i].first.first->getShape();

        // initial solution 
        // Large/Small sector envelope englobing (4+4+1+4+4)x 4(rings) =  68 layers identified with simId (station type,ring,phi,sector,multi,layer)
        //
        // advanced solution:
        // Large/Small sector envelope englobing (4+4+1+4+4) 17 layers (or: 5 volumes with 4/4/1/4/4 layers )
        // each layer carrying 4 surfaces linked to readout geometry (rings)   

        // collect layers corresponding to sensitive planes/spacers
	std::vector<const Trk::Layer*> sectorL;
	std::vector<const Trk::Layer*> sectorS;

        int nClones =  vols.size() ? vols[0].second.size() : 0;

	for (unsigned int ish=0; ish < vols.size(); ish++) { 

          bool isLargeSector =  fabs(((vols[ish]).second)[0].translation().phi())<0.01 ? true : false;
	  std::string protoName = vname;
	  if (!simpleTree) protoName = vname+"_"+volNames[ish];

	  std::string oName = protoName.substr(protoName.find("-")+1);	  
          Identifier nswId = m_muonStationTypeBuilder->identifyNSW(oName, vols[ish].second[0]);

          // get bounds and transform from readout geometry
	  const Trk::RotatedTrapezoidBounds* rtrd=0;
	  Amg::Transform3D layTransf(Trk::s_idTransform);
	  if (m_muonMgr->stgcIdHelper()->is_stgc(nswId)) {
	    const MuonGM::sTgcReadoutElement* stgc=m_muonMgr->getsTgcReadoutElement(nswId);
	    if (stgc) rtrd = dynamic_cast<const Trk::RotatedTrapezoidBounds*> (&stgc->bounds(nswId));
            if (stgc) layTransf = stgc->transform(nswId);
            if(stgc) ATH_MSG_DEBUG( " STGC readout element " );
            if(!stgc) ATH_MSG_DEBUG( " STGC and NO readout element " );
	  } else if ( m_muonMgr->mmIdHelper()->is_mm(nswId)) {
	    const MuonGM::MMReadoutElement* mm=m_muonMgr->getMMReadoutElement(nswId);
	    if (mm) rtrd = dynamic_cast<const Trk::RotatedTrapezoidBounds*> (&mm->bounds(nswId));
            if (mm) layTransf = mm->transform(nswId);
            if(mm) ATH_MSG_DEBUG( " MM readout element " );
            if(!mm) ATH_MSG_DEBUG( " MM and NO readout element " );
	  }
          if(isLargeSector) ATH_MSG_DEBUG( " Large Sector " );
          if(!isLargeSector) ATH_MSG_DEBUG( " Small Sector " );

          ATH_MSG_DEBUG( " ish " << ish << " "<< protoName << " input_shapes[ish] " << input_shapes[ish]->type() );

          const Trk::HomogeneousLayerMaterial mat(*(vols[ish].first.second),0.);  

	  const Trk::Layer* layer=0;

          if (!rtrd ) {    // translate from GeoModel ( spacer & non-identified stuff )
	    ATH_MSG_DEBUG( " translate from GeoModel " << protoName );

	    Amg::Transform3D ident(Trk::s_idTransform);
	    //m_geoShapeConverter->decodeShape(input_shapes[ish]);
	    const Trk::Volume* trObject = m_geoShapeConverter->translateGeoShape(input_shapes[ish],&ident);
            if (trObject) {
              const Trk::TrackingVolume* newType=new Trk::TrackingVolume(*trObject,vols[ish].first.second->material(),0,0,protoName);
              layer = m_muonStationTypeBuilder->createLayer(newType,vols[ish].first.second,vols[ish].second[0]);
              if (layer) layer->moveLayer(vols[ish].second[0]);
              delete trObject;
	    }
	  } else {
	    // create active layer
	    // change of boundary type ( VP1 problems with rotated trapezoid )   
	    //Trk::RotatedTrapezoidBounds* tbounds = new Trk::RotatedTrapezoidBounds(*rtrd);
	    Trk::TrapezoidBounds* tbounds = new Trk::TrapezoidBounds(rtrd->minHalflengthY(),rtrd->maxHalflengthY(),rtrd->halflengthX());
	    Trk::SharedObject<const Trk::SurfaceBounds> bounds(tbounds);
	    Trk::OverlapDescriptor* od=0;
	    double thickness=(mat.fullMaterial(layTransf.translation()))->thickness();
	    layer = new Trk::PlaneLayer(new Amg::Transform3D(layTransf*Amg::AngleAxis3D(-0.5*M_PI,Amg::Vector3D(0.,0.,1.))),
					bounds, mat,thickness, od, 1 );
	  }

          if (layer) {
	    unsigned int layType = nswId.get_identifier32().get_compact();
	    layer->setLayerType(layType);
         
	    if (isLargeSector) sectorL.push_back(layer);
	    else sectorS.push_back(layer);
	    if(isLargeSector) ATH_MSG_INFO( "new prototype build for Large sector: " << protoName <<","<<vols[ish].second.size() );
	    if(!isLargeSector) ATH_MSG_INFO( "new prototype build for Large sector: " << protoName <<","<<vols[ish].second.size() );
	  }
	} // end new object

        // create station prototypes
        const Trk::TrackingVolume* newTypeL=m_muonStationTypeBuilder->processNSW(sectorL); 
	// create layer representation
        std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>*> layerReprL =
	  m_muonStationTypeBuilder->createLayerRepresentation(newTypeL);
	// create prototype as detached tracking volume
	const Trk::DetachedTrackingVolume* typeL = newTypeL ? new Trk::DetachedTrackingVolume("NSWL",newTypeL,layerReprL.first,layerReprL.second) : 0;
	//objs.push_back(std::pair<const Trk::DetachedTrackingVolume*,std::vector<Amg::Transform3D> >(typeStat,vols[ish].second));
        const Trk::TrackingVolume* newTypeS=m_muonStationTypeBuilder->processNSW(sectorS); 
	// create layer representation
        std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>*> layerReprS =
	  m_muonStationTypeBuilder->createLayerRepresentation(newTypeS);
	const Trk::DetachedTrackingVolume* typeS = newTypeS ? new Trk::DetachedTrackingVolume("NSWS",newTypeS,layerReprS.first,layerReprS.second) : 0;

        if (typeL) {
	  mStations.push_back(typeL); 
	  
	  for (int it=1 ;it<8; it++)  { 
	    // clone station from prototype :: CHECK z<0 side, probably turns in wrong direction
	    Amg::Transform3D ntransf( Amg::AngleAxis3D(it*0.25*M_PI, Amg::Vector3D(0.,0.,1.)));
	    const Trk::DetachedTrackingVolume* newStat = typeL->clone("NSWL",ntransf);
            // no detailed identification of NSW layer representation
            const std::vector<const Trk::Layer*>* lays=newStat->trackingVolume()->confinedArbitraryLayers();
            for (unsigned int il=0; il<lays->size(); il++) {
	      int iType = (*lays)[il]->layerType();
	      if (iType!=0) {
                Identifier id(iType);
		//std::cout <<"cloned layer position:"<<(*lays)[il]->surfaceRepresentation().center();
		//std::cout <<"recalculating:"<<id<<","<<m_muonMgr->mmIdHelper()->is_stgc(id)<<","<<m_muonMgr->mmIdHelper()->stationName(id)<<  std::endl;
		//std::cout <<"recalculating:"<< m_muonMgr->mmIdHelper()->stationName(id)<<","<< m_muonMgr->mmIdHelper()->stationEta(id)<<","<<
		//  m_muonMgr->mmIdHelper()->stationPhi(id)<<","<< m_muonMgr->mmIdHelper()->multilayer(id)<<","<< m_muonMgr->mmIdHelper()->gasGap(id)<<std::endl;
		Identifier nid(0);
		if (m_muonMgr->mmIdHelper()->is_stgc(id)) {
		  nid = m_muonMgr->stgcIdHelper()->channelID(m_muonMgr->stgcIdHelper()->stationName(id),
							     m_muonMgr->stgcIdHelper()->stationEta(id),
							     m_muonMgr->stgcIdHelper()->stationPhi(id)+it,
							     m_muonMgr->stgcIdHelper()->multilayer(id),
							     m_muonMgr->stgcIdHelper()->gasGap(id),1,1);
		  //const MuonGM::sTgcReadoutElement* stgc=m_muonMgr->getsTgcReadoutElement(nid);
		  //if (stgc) std::cout <<"stgcRE recovered:"<<nid<<","<< stgc->center(nid) << std::endl;
		}  else {
		  nid = m_muonMgr->mmIdHelper()->channelID(m_muonMgr->mmIdHelper()->stationName(id),
							   m_muonMgr->mmIdHelper()->stationEta(id),
							   m_muonMgr->mmIdHelper()->stationPhi(id)+it,
							   m_muonMgr->mmIdHelper()->multilayer(id),
							   m_muonMgr->mmIdHelper()->gasGap(id),1);
		  //const MuonGM::MMReadoutElement* mm=m_muonMgr->getMMReadoutElement(nid);
		  //if (mm) std::cout <<"mmRE recovered:"<<nid<<","<< mm->center(nid) << std::endl;
		}
		
		unsigned int nType = nid.get_identifier32().get_compact();
		(*lays)[il]->setLayerType(nType);
	      }
	    }
	    mStations.push_back(newStat);
	  }
	  
          if (nClones==16) { // have to mirror stations as well
	    Amg::Transform3D ntransf(Amg::AngleAxis3D(M_PI,Amg::Vector3D(1.,0.,0.)));
	    const Trk::DetachedTrackingVolume* mtypeL = typeL->clone("NSWL",ntransf);
	    //std::cout <<"cloned NSW station:"<<mtypeL->trackingVolume()->center()<<std::endl;   
            // recalculate identifier
            const std::vector<const Trk::Layer*>* lays=mtypeL->trackingVolume()->confinedArbitraryLayers();
            for (unsigned int il=0; il<lays->size(); il++) {
	      int iType = (*lays)[il]->layerType();
	      if (iType!=0) {
		Identifier id(iType);
		//std::cout <<"cloned layer position:"<<(*lays)[il]->surfaceRepresentation().center();
		//std::cout <<"recalculating:"<<id<<","<<m_muonMgr->mmIdHelper()->is_stgc(id)<<","<<m_muonMgr->mmIdHelper()->stationName(id)<<  std::endl;
		//std::cout <<"recalculating:"<< m_muonMgr->mmIdHelper()->stationName(id)<<","<< m_muonMgr->mmIdHelper()->stationEta(id)<<","<<
		//  m_muonMgr->mmIdHelper()->stationPhi(id)<<","<< m_muonMgr->mmIdHelper()->multilayer(id)<<","<< m_muonMgr->mmIdHelper()->gasGap(id)<<std::endl;
		Identifier nid(0);
		if (m_muonMgr->mmIdHelper()->is_stgc(id)) {
		  nid = m_muonMgr->stgcIdHelper()->channelID(m_muonMgr->stgcIdHelper()->stationName(id),
							     -m_muonMgr->stgcIdHelper()->stationEta(id),
							     m_muonMgr->stgcIdHelper()->stationPhi(id),
							     m_muonMgr->stgcIdHelper()->multilayer(id),
							     m_muonMgr->stgcIdHelper()->gasGap(id),1,1);
		  //const MuonGM::sTgcReadoutElement* stgc=m_muonMgr->getsTgcReadoutElement(nid);
		  //if (stgc) std::cout <<"stgcRE recovered:"<<nid<<","<< stgc->center(nid) << std::endl;
		}  else {
		  nid = m_muonMgr->mmIdHelper()->channelID(m_muonMgr->mmIdHelper()->stationName(id),
							   -m_muonMgr->mmIdHelper()->stationEta(id),
							   m_muonMgr->mmIdHelper()->stationPhi(id),
							   m_muonMgr->mmIdHelper()->multilayer(id),
							   m_muonMgr->mmIdHelper()->gasGap(id),1);
		  //const MuonGM::MMReadoutElement* mm=m_muonMgr->getMMReadoutElement(nid);
		  //if (mm) std::cout <<"mmRE recovered:"<<nid<<","<< mm->center(nid) << std::endl;
		}
		
		unsigned int nType = nid.get_identifier32().get_compact();
		(*lays)[il]->setLayerType(nType);
	      }
	    }
	    mStations.push_back(mtypeL);
	    for (unsigned int it=1 ;it<8; it++)  { 
	      // clone station from prototype :: CHECK z<0 side, probably turns in wrong direction
	      Amg::Transform3D ntransf(Amg::AngleAxis3D(it*0.25*M_PI,Amg::Vector3D(0.,0.,1.)));
	      const Trk::DetachedTrackingVolume* newStat = mtypeL->clone("NSWL",ntransf);
	      ATH_MSG_DEBUG( "cloned NSW station:"<<newStat->trackingVolume()->center() );
	      // recalculate identifiers
	      const std::vector<const Trk::Layer*>* lays=newStat->trackingVolume()->confinedArbitraryLayers();
	      for (unsigned int il=0; il<lays->size(); il++) {
		int iType = (*lays)[il]->layerType();
		if (iType!=0) {
		  Identifier id(iType);
		  //std::cout <<"cloned layer position:"<<(*lays)[il]->surfaceRepresentation().center();
		  //std::cout <<"recalculating:"<<id<<","<<m_muonMgr->mmIdHelper()->is_stgc(id)<<","<<m_muonMgr->mmIdHelper()->stationName(id)<<  std::endl;
		  //std::cout <<"recalculating:"<< m_muonMgr->mmIdHelper()->stationName(id)<<","<< m_muonMgr->mmIdHelper()->stationEta(id)<<","<<
		  //  m_muonMgr->mmIdHelper()->stationPhi(id)<<","<< m_muonMgr->mmIdHelper()->multilayer(id)<<","<< m_muonMgr->mmIdHelper()->gasGap(id)<<std::endl;
		  Identifier nid(0);
		  if (m_muonMgr->mmIdHelper()->is_stgc(id)) {
		    nid = m_muonMgr->stgcIdHelper()->channelID(m_muonMgr->stgcIdHelper()->stationName(id),
							       m_muonMgr->stgcIdHelper()->stationEta(id),
							       m_muonMgr->stgcIdHelper()->stationPhi(id)+it,
							       m_muonMgr->stgcIdHelper()->multilayer(id),
							       m_muonMgr->stgcIdHelper()->gasGap(id),1,1);
		    //const MuonGM::sTgcReadoutElement* stgc=m_muonMgr->getsTgcReadoutElement(nid);
		    //if (stgc) std::cout <<"stgcRE recovered:"<<nid<<","<< stgc->center(nid) << std::endl;
		  }  else {
		    nid = m_muonMgr->mmIdHelper()->channelID(m_muonMgr->mmIdHelper()->stationName(id),
							     m_muonMgr->mmIdHelper()->stationEta(id),
							     m_muonMgr->mmIdHelper()->stationPhi(id)+it,
							     m_muonMgr->mmIdHelper()->multilayer(id),
							     m_muonMgr->mmIdHelper()->gasGap(id),1);
		    //const MuonGM::MMReadoutElement* mm=m_muonMgr->getMMReadoutElement(nid);
		    //if (mm) std::cout <<"mmRE recovered:"<<nid<<","<< mm->center(nid) << std::endl;
		  }

		  unsigned int nType = nid.get_identifier32().get_compact();
		  (*lays)[il]->setLayerType(nType);
		}
	      }
	      mStations.push_back(newStat);
	    }
	  }          
	}	  

        if (typeS) {
	  mStations.push_back(typeS); 
      
	  for (unsigned int it=1 ;it<8; it++)  { 
	    // clone station from prototype
	    Amg::Transform3D ntransf(Amg::AngleAxis3D(it*0.25*M_PI,Amg::Vector3D(0.,0.,1.)));
	    const Trk::DetachedTrackingVolume* newStat = typeS->clone("NSWS",ntransf);
	    ATH_MSG_DEBUG( "cloned NSWS station:"<<newStat->trackingVolume()->center() );
            // recalculate identifiers
            const std::vector<const Trk::Layer*>* lays=newStat->trackingVolume()->confinedArbitraryLayers();
            for (unsigned int il=0; il<lays->size(); il++) {
	      int iType = (*lays)[il]->layerType();
	      if (iType!=0) {
                Identifier id(iType);
		//std::cout <<"cloned layer position:"<<(*lays)[il]->surfaceRepresentation().center();
		//std::cout <<"recalculating:"<<id<<","<<m_muonMgr->mmIdHelper()->is_stgc(id)<<","<<m_muonMgr->mmIdHelper()->stationName(id)<<  std::endl;
		//std::cout <<"recalculating:"<< m_muonMgr->mmIdHelper()->stationName(id)<<","<< m_muonMgr->mmIdHelper()->stationEta(id)<<","<<
		//  m_muonMgr->mmIdHelper()->stationPhi(id)<<","<< m_muonMgr->mmIdHelper()->multilayer(id)<<","<< m_muonMgr->mmIdHelper()->gasGap(id)<<std::endl;
		Identifier nid(0);
		if (m_muonMgr->mmIdHelper()->is_stgc(id)) {
		  nid = m_muonMgr->stgcIdHelper()->channelID(m_muonMgr->stgcIdHelper()->stationName(id),
							     m_muonMgr->stgcIdHelper()->stationEta(id),
							     m_muonMgr->stgcIdHelper()->stationPhi(id)+it,
							     m_muonMgr->stgcIdHelper()->multilayer(id),
							     m_muonMgr->stgcIdHelper()->gasGap(id),1,1);
		  //const MuonGM::sTgcReadoutElement* stgc=m_muonMgr->getsTgcReadoutElement(nid);
		  //if (stgc) std::cout <<"stgcRE recovered:"<<nid<<","<< stgc->center(nid) << std::endl;
		}  else {
		  nid = m_muonMgr->mmIdHelper()->channelID(m_muonMgr->mmIdHelper()->stationName(id),
							   m_muonMgr->mmIdHelper()->stationEta(id),
							   m_muonMgr->mmIdHelper()->stationPhi(id)+it,
							   m_muonMgr->mmIdHelper()->multilayer(id),
							   m_muonMgr->mmIdHelper()->gasGap(id),1);
		  //const MuonGM::MMReadoutElement* mm=m_muonMgr->getMMReadoutElement(nid);
		  //if (mm) std::cout <<"mmRE recovered:"<<nid<<","<< mm->center(nid) << std::endl;
		}

		unsigned int nType = nid.get_identifier32().get_compact();
		(*lays)[il]->setLayerType(nType);
	      }
	    }
	    mStations.push_back(newStat);
	  }

          if (nClones==16) { // have to mirror stations as well
	    //HepGeom::Transform3D ntransf= HepGeom::RotateZ3D(-22.5*CLHEP::deg)*HepGeom::RotateX3D(180*CLHEP::deg)* HepGeom::RotateZ3D(22.5*CLHEP::deg);
            double phiS=typeS->trackingVolume()->center().phi();
	    Amg::Transform3D ntransf(Amg::AngleAxis3D(+phiS,Amg::Vector3D(0.,0.,1.))*
				     Amg::AngleAxis3D(+M_PI,Amg::Vector3D(1.,0.,0.))*
				     Amg::AngleAxis3D(-phiS,Amg::Vector3D(0.,0.,1.)));
	    const Trk::DetachedTrackingVolume* mtypeS = typeS->clone("NSWL",ntransf);
	    ATH_MSG_DEBUG( "cloned NSWS station:"<<mtypeS->trackingVolume()->center() );
            // recalculate identifiers
            const std::vector<const Trk::Layer*>* lays=mtypeS->trackingVolume()->confinedArbitraryLayers();
            for (unsigned int il=0; il<lays->size(); il++) {
	      int iType = (*lays)[il]->layerType();
	      if (iType!=0) {
		Identifier id(iType);
		//std::cout <<"cloned layer position:"<<(*lays)[il]->surfaceRepresentation().center();
		//std::cout <<"recalculating:"<<id<<","<<m_muonMgr->mmIdHelper()->is_stgc(id)<<","<<m_muonMgr->mmIdHelper()->stationName(id)<<  std::endl;
		//std::cout <<"recalculating:"<< m_muonMgr->mmIdHelper()->stationName(id)<<","<< m_muonMgr->mmIdHelper()->stationEta(id)<<","<<
		//  m_muonMgr->mmIdHelper()->stationPhi(id)<<","<< m_muonMgr->mmIdHelper()->multilayer(id)<<","<< m_muonMgr->mmIdHelper()->gasGap(id)<<std::endl;
		Identifier nid(0);
		if (m_muonMgr->mmIdHelper()->is_stgc(id)) {
		  nid = m_muonMgr->stgcIdHelper()->channelID(m_muonMgr->stgcIdHelper()->stationName(id),
							     -m_muonMgr->stgcIdHelper()->stationEta(id),
							     m_muonMgr->stgcIdHelper()->stationPhi(id),
							     m_muonMgr->stgcIdHelper()->multilayer(id),
							     m_muonMgr->stgcIdHelper()->gasGap(id),1,1);
		  //const MuonGM::sTgcReadoutElement* stgc=m_muonMgr->getsTgcReadoutElement(nid);
		  //if (stgc) std::cout <<"stgcRE recovered:"<<nid<<","<< stgc->center(nid) << std::endl;
		}  else {
		  nid = m_muonMgr->mmIdHelper()->channelID(m_muonMgr->mmIdHelper()->stationName(id),
							   -m_muonMgr->mmIdHelper()->stationEta(id),
							   m_muonMgr->mmIdHelper()->stationPhi(id),
							   m_muonMgr->mmIdHelper()->multilayer(id),
							   m_muonMgr->mmIdHelper()->gasGap(id),1);
		  //const MuonGM::MMReadoutElement* mm=m_muonMgr->getMMReadoutElement(nid);
		  //if (mm) std::cout <<"mmRE recovered:"<<nid<<","<< mm->center(nid) << std::endl;
		}
		
		unsigned int nType = nid.get_identifier32().get_compact();
		(*lays)[il]->setLayerType(nType);
	      }
	    }
	    mStations.push_back(mtypeS);
	    for (unsigned int it=1 ;it<8; it++)  { 
	      // clone station from prototype :: CHECK z<0 side, probably turns in wrong direction
	      Amg::Transform3D ntransf( Amg::AngleAxis3D(it*0.25*M_PI, Amg::Vector3D(0.,0.,1.)));
	      const Trk::DetachedTrackingVolume* newStat = mtypeS->clone("NSWL",ntransf);
	      ATH_MSG_DEBUG( "cloned NSWS station:"<<newStat->trackingVolume()->center() );
	      // recalculate identifiers
	      const std::vector<const Trk::Layer*>* lays=newStat->trackingVolume()->confinedArbitraryLayers();
	      for (unsigned int il=0; il<lays->size(); il++) {
		int iType = (*lays)[il]->layerType();
		if (iType!=0) {
		  Identifier id(iType);
		  //std::cout <<"cloned layer position:"<<(*lays)[il]->surfaceRepresentation().center();
		  //std::cout <<"recalculating:"<<id<<","<<m_muonMgr->mmIdHelper()->is_stgc(id)<<","<<m_muonMgr->mmIdHelper()->stationName(id)<<  std::endl;
		  //std::cout <<"recalculating:"<< m_muonMgr->mmIdHelper()->stationName(id)<<","<< m_muonMgr->mmIdHelper()->stationEta(id)<<","<<
		  //  m_muonMgr->mmIdHelper()->stationPhi(id)<<","<< m_muonMgr->mmIdHelper()->multilayer(id)<<","<< m_muonMgr->mmIdHelper()->gasGap(id)<<std::endl;
		  Identifier nid(0);
		  if (m_muonMgr->mmIdHelper()->is_stgc(id)) {
		    nid = m_muonMgr->stgcIdHelper()->channelID(m_muonMgr->stgcIdHelper()->stationName(id),
							       m_muonMgr->stgcIdHelper()->stationEta(id),
							       m_muonMgr->stgcIdHelper()->stationPhi(id)+it,
							       m_muonMgr->stgcIdHelper()->multilayer(id),
							       m_muonMgr->stgcIdHelper()->gasGap(id),1,1);
		    //const MuonGM::sTgcReadoutElement* stgc=m_muonMgr->getsTgcReadoutElement(nid);
		    //if (stgc) std::cout <<"stgcRE recovered:"<<nid<<","<< stgc->center(nid) << std::endl;
		  }  else {
		    nid = m_muonMgr->mmIdHelper()->channelID(m_muonMgr->mmIdHelper()->stationName(id),
							     m_muonMgr->mmIdHelper()->stationEta(id),
							     m_muonMgr->mmIdHelper()->stationPhi(id)+it,
							     m_muonMgr->mmIdHelper()->multilayer(id),
							     m_muonMgr->mmIdHelper()->gasGap(id),1);
		    //const MuonGM::MMReadoutElement* mm=m_muonMgr->getMMReadoutElement(nid);
		    //if (mm) std::cout <<"mmRE recovered:"<<nid<<","<< mm->center(nid) << std::endl;
		  }

		  unsigned int nType = nid.get_identifier32().get_compact();
		  (*lays)[il]->setLayerType(nType);
		  //std::cout <<"new layer id:"<<it<<","<<iType<<","<<(*lays)[il]->layerType()<<std::endl;
		}
	      }
	      mStations.push_back(newStat);
	    }
	  }
	}

	// clean up prototypes
	//for (unsigned int it = 0; it < objs.size(); it++)  delete objs[it].first; 
	
      } // end NSW!

      if (vname.size()>7 && vname.substr(vname.size()-7,7) =="Station" && 
            ( (m_buildBarrel && vname.substr(0,1) =="B")
            ||(m_buildEndcap && vname.substr(0,1) =="E")
	    ||(m_buildCsc    && vname.substr(0,1) =="C")
	    ||(m_buildTgc    && vname.substr(0,1) =="T") ) ) {

        int etaphi = vol.getId();        // retrive eta/phi indexes
        int sign =( etaphi < 0 ) ? -1 : 1 ;
        etaphi = sign*etaphi;
        int is_mirr = etaphi/1000;
        etaphi = etaphi - is_mirr*1000;
        int eta = etaphi/100;
        int phi = etaphi - eta*100;
        eta = eta*sign;
	MuonGM::MuonStation* gmStation = m_muonMgr->getMuonStation(vname.substr(0,3),eta,phi);
        // try to retrieve 
	if ( !gmStation) {
          gmStation = m_muonMgr->getMuonStation(vname.substr(0,4),eta,phi);
        }
        // assembly ?
	if ( !gmStation) {
	  int etaphi = vol.getId();        // retrieve eta/phi indexes
          int a_etaphi = static_cast<int> (etaphi/100000);
          int sideC = static_cast<int> (a_etaphi/10000);
          a_etaphi -= sideC*10000; 
          is_mirr = static_cast<int> (a_etaphi/1000);
          a_etaphi -= is_mirr*1000; 
          eta = static_cast<int> (a_etaphi/100);
          phi = a_etaphi - eta*100;
          if (sideC) eta *=-1;
          gmStation = m_muonMgr->getMuonStation(vname.substr(0,3),eta,phi);
        }
        //
        if (!gmStation) ATH_MSG_WARNING( "Muon station not found! "<<vname<<","<<eta<<","<<phi ); 
        std::string stName = (clv->getName()).substr(0,vname.size()-8);
        if (stName.substr(0,1)=="B" && eta < 0 ) {
          stName = (clv->getName()).substr(0,vname.size()-8) + "-";
        }
        if (stName.substr(0,1)=="T" || stName.substr(0,1)=="C") {
          //std::string tgc_name = cv->getChildVol(0)->getLogVol()->getName();
          stName = vname.substr(0,4);
        }
        // loop over prototypes
        const Trk::DetachedTrackingVolume* msTV = 0;
        for (msTypeIter = msTypes->begin(); msTypeIter != msTypes->end(); ++msTypeIter) { 
          std::string msTypeName = (*msTypeIter)->name();
          if (  (stName.substr(0,1)=="T" && stName == msTypeName.substr(0,stName.size()) )
              ||(stName.substr(0,1)!="T" && stName == msTypeName) ) {
            msTV = *msTypeIter;
	    if (msTV && gmStation) {
	      Amg::Transform3D transf = Amg::CLHEPTransformToEigen(gmStation->getTransform()); 
              Identifier stId(0);
              if (stName.substr(0,1)=="C") {
		stId = m_cscIdHelper->elementID(vname.substr(0,3),eta,phi);
              }
              // adjust eta,phi
              if (msTypeName.substr(0,1)=="C") {
                eta = 1;
		if (transf.translation().z() < 0 ) eta = 0;
		double phic = transf.translation().phi() + 0.1 ;  
                phi = static_cast<int> (phic<0 ? 4*phic/M_PI+8 : 4*phic/M_PI);
              } 
	      if (msTypeName.substr(0,1)=="T") {
		bool az = true;
		std::string msName = msTV->trackingVolume()->volumeName();
		if (transf.translation().z() < 0 ) az = false;
		if (msName.substr(7,2)=="01") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="02") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="03") eta = az ? 6 : 3;
		if (msName.substr(7,2)=="04") eta = az ? 7 : 2;
		if (msName.substr(7,2)=="05") eta = az ? 8 : 1;
		if (msName.substr(7,2)=="06") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="07") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="08") eta = az ? 6 : 3;
		if (msName.substr(7,2)=="09") eta = az ? 7 : 2;
		if (msName.substr(7,2)=="10") eta = az ? 8 : 1;
		if (msName.substr(7,2)=="11") eta = az ? 9 : 0;
		if (msName.substr(7,2)=="12") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="13") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="14") eta = az ? 6 : 3;
		if (msName.substr(7,2)=="15") eta = az ? 7 : 2;
		if (msName.substr(7,2)=="16") eta = az ? 8 : 1;
		if (msName.substr(7,2)=="17") eta = az ? 9 : 0;
		if (msName.substr(7,2)=="18") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="19") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="20") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="21") eta = az ? 5 : 4;
		if (msName.substr(7,2)=="22") eta = az ? 5 : 4;
	      }     
              if (stName.substr(0,1)=="T") {
		int etaSt = eta - 4;
		if (eta < 5) etaSt = eta - 5; 
		double phic = transf.translation().phi(); 
		if (msTypeName.substr(2,1)=="E" && msTypeName.substr(0,3)!="T4E")
		  phi = static_cast<int> (phic<0 ? 24*phic/M_PI+48 : 24*phic/M_PI);
		else
                  phi = static_cast<int> (phic<0 ? 12*phic/M_PI+24 : 12*phic/M_PI);
                phi++;
                stId = m_tgcIdHelper->elementID(vname.substr(0,3),etaSt,phi);
              } else if (stName.substr(0,3)=="BML") {
		stId = m_rpcIdHelper->elementID(vname.substr(0,3),eta,phi,1);
              } else if (stName.substr(0,1)!="C" ) {
		stId = m_mdtIdHelper->elementID(vname.substr(0,3),eta,phi);
              }
              if (!(stId.get_compact())) ATH_MSG_WARNING( "identifier of the station not found:"<<vname <<","<<eta<<","<<phi );
              unsigned int iD = stId.get_identifier32().get_compact();
	      // clone station from prototype
	      const Trk::DetachedTrackingVolume* newStat = msTV->clone(vname,transf);
              // identify layer representation
              newStat->layerRepresentation()->setLayerType(iD);
              // resolved stations only:
              if (m_resolveActiveLayers) {
		// glue components
		glueComponents(newStat);
		// identify layers
		identifyLayers(newStat,eta,phi); 
	      } 
	      mStations.push_back(newStat);
            }
	  }
	}  
	if (!msTV)  ATH_MSG_DEBUG( name() <<" this station has no prototype: " << vname );    
      }
      vol.next();      
    }
    // clean up prototypes
    for (unsigned int it = 0; it < msTypes->size(); it++) delete (*msTypes)[it];
    delete msTypes;   
  }
  const std::vector<const Trk::DetachedTrackingVolume*>* muonStations=new std::vector<const Trk::DetachedTrackingVolume*>(mStations);

  //
  ATH_MSG_INFO( name() << "returns " << (*muonStations).size() << " stations" );	 
  return muonStations; 
}

const std::vector<const Trk::DetachedTrackingVolume*>* Muon::MuonStationBuilder::buildDetachedTrackingVolumeTypes(bool /*blend*/) const 
{

  ATH_MSG_INFO( name() <<" building station types" );    
///////////////////////////////////////////////////////////////////////////////////////////////////
      std::vector<const Trk::DetachedTrackingVolume*> stations;

   if (m_muonMgr){

      // link to top tree
      const GeoVPhysVol* top = &(*(m_muonMgr->getTreeTop(0)));
      GeoVolumeCursor vol (top);
      while (!vol.atEnd())
      {
        const GeoVPhysVol* cv = &(*(vol.getVolume()));
        const GeoLogVol* clv = cv->getLogVol();
        std::string vname = clv->getName();
	if (vname.size()>7 && vname.substr(vname.size()-7,7) =="Station" && 
            ( (m_buildBarrel && vname.substr(0,1) =="B")
            ||(m_buildEndcap && vname.substr(0,1) =="E")
            ||(m_buildCsc && vname.substr(0,1) =="C")
	    ||(m_buildTgc && vname.substr(0,1) =="T") ) )
	{
          int etaphi = vol.getId();        // retrieve eta/phi indexes
          int sign  = ( etaphi < 0 ) ? -1 : 1;
          etaphi = sign*etaphi;
          int is_mirr = etaphi/1000;
          etaphi = etaphi - is_mirr*1000;
          int eta = etaphi/100;
          int phi = etaphi - eta*100;
          eta = eta*sign;
	  // std::cout << vname.substr(0,3) <<","<<is_mirr<<","<<eta<<","<<phi<<std::endl;
	  MuonGM::MuonStation* gmStation = m_muonMgr->getMuonStation(vname.substr(0,3),eta,phi);
	  if ( !gmStation) {
            gmStation = m_muonMgr->getMuonStation(vname.substr(0,4),eta,phi);
          }
	  // assembly ?
	  if ( !gmStation) {
	    int etaphi = vol.getId();        // retrieve eta/phi indexes
	    int a_etaphi = static_cast<int> (etaphi/100000);
	    int sideC = static_cast<int> (a_etaphi/10000);
	    a_etaphi -= sideC*10000; 
	    is_mirr = static_cast<int> (a_etaphi/1000);
	    a_etaphi -= is_mirr*1000; 
	    eta = static_cast<int> (a_etaphi/100);
	    phi = a_etaphi - eta*100;
	    if (sideC) eta *=-1;
	    gmStation = m_muonMgr->getMuonStation(vname.substr(0,3),eta,phi);
	  }
          //
          if (!gmStation ) continue;
          //
          std::string name = (clv->getName()).substr(0,vname.size()-8);
          // is this station known ?        
          // if TGC station, look for 1 component instead
          if (name.substr(0,1)=="T") {
            std::string tgc_name = cv->getChildVol(0)->getLogVol()->getName();
            name = tgc_name;
          }
          if (name.substr(0,1)=="B" && eta < 0 ) {
            name = (clv->getName()).substr(0,vname.size()-8) + "-";
          }
          unsigned is=0; 
          for (unsigned in=0; in< stations.size(); in++) 
          {
            if (stations[in]!=0 && name == stations[in]->name()) is++;
          }
          // if (is!=0 ) std::cout << "prototype exists" << std::endl;
          // if (is==0 ) std::cout << "station shape type:"<< name<< ","<<clv->getShape()->type()<<std::endl; 
          if (is==0 )          
          {
            ATH_MSG_VERBOSE(" new station type " << name << "," << clv->getShape()->type() );    
            ATH_MSG_VERBOSE(" prototype built from eta, phi:" << eta << "," << phi );    
             
            if (name.substr(0,2)=="CS" || name.substr(0,1)=="T") {
              if (m_muonStationTypeBuilder) {
                if (name.substr(0,2)=="CS") { 
                  const Trk::TrackingVolume* csc_station = m_muonStationTypeBuilder->processCscStation(cv, name);   
		  // create layer representation
		  std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>*> layerRepr =
		    m_muonStationTypeBuilder->createLayerRepresentation(csc_station);
		  // create prototype as detached tracking volume
		  const Trk::DetachedTrackingVolume* typeStat = new Trk::DetachedTrackingVolume(name,csc_station,layerRepr.first,layerRepr.second);
                  if (!m_resolveActiveLayers) typeStat->trackingVolume()->clear();
		  stations.push_back(typeStat); 
                } else {
                  std::vector<const Trk::TrackingVolume*> tgc_stations = m_muonStationTypeBuilder->processTgcStation(cv);   
                  for (unsigned int i=0;i<tgc_stations.size();i++) {
		    // create layer representation
		    std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>*> layerRepr =
		      m_muonStationTypeBuilder->createLayerRepresentation(tgc_stations[i]);
		    // create prototype as detached tracking volume
		    const Trk::DetachedTrackingVolume* typeStat = new Trk::DetachedTrackingVolume(name,tgc_stations[i],layerRepr.first,layerRepr.second);
		    if (!m_resolveActiveLayers) typeStat->trackingVolume()->clear();
                    stations.push_back(typeStat); 
                  }
                }
              }
            } else {    
              
              const GeoShape* shapeS = clv->getShape();
              while ( shapeS->type() != "Trd" ){
		if (shapeS->type()=="Shift") {
		  const GeoShapeShift* shift = dynamic_cast<const GeoShapeShift*> (shapeS); 
                  shapeS = shift->getOp();
		} else if (shapeS->type()=="Subtraction") {
		  const GeoShapeSubtraction* sub = dynamic_cast<const GeoShapeSubtraction*> (shapeS); 
                  shapeS = sub->getOpA();
		} else if (shapeS->type()=="Union") {
		  const GeoShapeUnion* uni = dynamic_cast<const GeoShapeUnion*> (shapeS); 
                  shapeS = uni->getOpA();
		} else {
		  ATH_MSG_WARNING( "unexpected station shape ? "<< shapeS->type() << ", station not built" );
		  break; 
		}
	      }
	      const GeoTrd* trd=dynamic_cast<const GeoTrd*> ( shapeS );

	      double halfX1=0.;
	      double halfX2=0.;
	      double halfY1=0.;
	      double halfY2=0.;
	      double halfZ=0.;   
	      if (trd) {
		//
		halfX1 = trd->getXHalfLength1();
		halfX2 = trd->getXHalfLength2();
		halfY1 = trd->getYHalfLength1();
		halfY2 = trd->getYHalfLength2();
		halfZ  = trd->getZHalfLength();              
	      
		// define enveloping volume
		const Trk::TrackingVolumeArray* confinedVolumes = 0; 
		Trk::Volume* envelope = 0;
		std::string shape = "Trd";
		if (halfX1==halfX2 && halfY1==halfY2) shape = "Box";
		if (shape=="Box") { 
		  Trk::CuboidVolumeBounds* envBounds = new Trk::CuboidVolumeBounds(halfX1,halfY1,halfZ);
		  // station components
		  if (m_muonStationTypeBuilder) confinedVolumes = 
						  m_muonStationTypeBuilder->processBoxStationComponents(cv,envBounds); 
		  // enveloping volume
		  envelope= new Trk::Volume(0,envBounds);
		} else if (shape=="Trd") {
		  Trk::TrapezoidVolumeBounds* envBounds = 0;
		  Amg::Transform3D* transf=new Amg::Transform3D(Trk::s_idTransform); 
		  if (halfY1==halfY2) {
		    envBounds = new Trk::TrapezoidVolumeBounds(halfX1,halfX2,halfY1,halfZ);
		    ATH_MSG_VERBOSE( "CAUTION!!!: this trapezoid volume does not require XY -> YZ switch" );
		  }
		  if (halfY1!=halfY2 && halfX1 == halfX2 ) {
		    delete transf;
	            transf = new Amg::Transform3D( Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,1.,0.))*
						   Amg::AngleAxis3D(0.5*M_PI,Amg::Vector3D(0.,0.,1.)));
		    envBounds = new Trk::TrapezoidVolumeBounds(halfY1,halfY2,halfZ,halfX1); 
		  }
		  if (halfX1!=halfX2 && halfY1!=halfY2 )  ATH_MSG_WARNING( "station envelope arbitrary trapezoid?"<< name );

                  if (envBounds) {
		    // station components
		    if (m_muonStationTypeBuilder) confinedVolumes = 
						    m_muonStationTypeBuilder->processTrdStationComponents(cv,envBounds); 
		    // enveloping volume
		    envelope= new Trk::Volume(transf,envBounds);
		  } else {
                    delete transf; 
		  }
		}
		// hack to verify BI/BM stations
		//  if (name.substr(0,2)=="BI") name = gmStation->getKey();
		//  if (name.substr(0,2)=="BM") name = gmStation->getKey();
	
                if (envelope) {
	
		  // ready to build the station prototype
		  const Trk::TrackingVolume* newType= new Trk::TrackingVolume( *envelope,
									       m_muonMaterial,
									       0,confinedVolumes,
									       name);         
		  delete envelope; 
		
		  // identify prototype
		  if (m_resolveActiveLayers && ( name.substr(0,1)=="B" || name.substr(0,1)=="E") ) 
		    identifyPrototype(newType,eta,phi,Amg::CLHEPTransformToEigen(gmStation->getTransform()));
		  
		  // create layer representation
		  std::pair<const Trk::Layer*,const std::vector<const Trk::Layer*>*> layerRepr 
		    = m_muonStationTypeBuilder->createLayerRepresentation(newType);
		  
		  // create prototype as detached tracking volume
		  const Trk::DetachedTrackingVolume* typeStat = new Trk::DetachedTrackingVolume(name,newType,layerRepr.first,layerRepr.second);
		
		  if (!m_resolveActiveLayers) typeStat->trackingVolume()->clear();
		  stations.push_back(typeStat);
		} 
	      }
	    }
	  } // end new station type 
	} // end if "Shift" (station)
	vol.next();      
      }      
      ATH_MSG_INFO( name() << stations.size() <<" station prototypes built " );    
   }
   
///////////////////////////////////////////////////////////////////////////////////////
   const std::vector<const Trk::DetachedTrackingVolume*>* mStations = new std::vector<const Trk::DetachedTrackingVolume*>(stations); 
   return mStations;  
}

// finalize
StatusCode Muon::MuonStationBuilder::finalize()
{
  ATH_MSG_INFO(  name() <<" finalize() successful" );
  return StatusCode::SUCCESS;
}
//
void Muon::MuonStationBuilder::glueComponents(const Trk::DetachedTrackingVolume* stat) const
{
   const Trk::TrackingVolumeArray* volArray = stat->trackingVolume()->confinedVolumes();
   if (volArray) {
     if (volArray->arrayObjectsNumber() > 1) {
       const std::vector< const Trk::TrackingVolume* > components = volArray->arrayObjects();
       const Trk::BinUtility* binUtilityX = volArray->binUtility(); 
       const Trk::CuboidVolumeBounds* cubVolBounds = 
	 dynamic_cast<const Trk::CuboidVolumeBounds* >( &(components[0]->volumeBounds())); 
       /*
       const Trk::TrapezoidVolumeBounds* trdVolBounds = 
	 dynamic_cast<const Trk::TrapezoidVolumeBounds* >( &(components[0]->volumeBounds())); 
       const Trk::DoubleTrapezoidVolumeBounds* dtrdVolBounds = 
	 dynamic_cast<const Trk::DoubleTrapezoidVolumeBounds* >( &(components[0]->volumeBounds())); 
       */
       // identify 'lower' and 'upper' boundary surface
       Trk::BoundarySurfaceFace low = Trk::negativeFaceXY;
       Trk::BoundarySurfaceFace up = Trk::positiveFaceXY;

       // rectangular station in x ordering (MDT barrel)
       if ( cubVolBounds && binUtilityX ) {
	 low = Trk::negativeFaceYZ;
	 up  = Trk::positiveFaceYZ;
       }
       
       /*
       // rotated trapezoid station in x ordering (MDT endcap)
       if ( trdVolBounds && binUtilityX ) {
	 low = Trk::negativeFaceXY;
	 up  = Trk::positiveFaceXY;
       }
       // rotated diamond station in x ordering (MDT endcap)
       if ( dtrdVolBounds && binUtilityX ) {
	 low = Trk::negativeFaceXY;
	 up  = Trk::positiveFaceXY;
       }
       */

       if (low>=0 && up >=0) {
         // glue volumes 
         for (unsigned int i = 0; i< components.size()-1; i++) {
           m_trackingVolumeHelper -> glueTrackingVolumes(*(components[i]),up,
                                                         *(components[i+1]),low );
         }
       }
     }
   }
}


void Muon::MuonStationBuilder::identifyLayers(const Trk::DetachedTrackingVolume* station, int eta, int phi ) const
{
  ATH_MSG_VERBOSE( name() <<" identifying layers " );    

  std::string stationName = station->trackingVolume()->volumeName();
  ATH_MSG_VERBOSE( " in station " << station->name() );    

  
  if (stationName.substr(0,1)=="C") { 
    int st = stationName.substr(0,3)=="CSS" ? 0 : 1;
    const MuonGM::CscReadoutElement* cscRE = m_muonMgr->getCscReadoutElement(st,eta,phi,0);    
    int cLay = cscRE ? 0 : 1;
    if (!cscRE) cscRE = m_muonMgr->getCscReadoutElement(st,eta,phi,cLay);
    if (cscRE) {
      for (int gasgap = 0; gasgap < cscRE->Ngasgaps(); gasgap++) {
	Identifier idi = m_cscIdHelper->channelID(cscRE->identify(),cscRE->ChamberLayer(),gasgap+1,0,1);   
        const Trk::PlaneSurface* stripSurf = dynamic_cast<const Trk::PlaneSurface*> (&(cscRE->surface(idi)));
        const Amg::Vector3D gpi = stripSurf->center();
        const Trk::TrackingVolume* assocVol = station->trackingVolume()->associatedSubVolume(gpi);
        const Trk::Layer* assocLay = 0;
        if (assocVol) assocLay = assocVol->associatedLayer(gpi);
        unsigned int iD = idi.get_identifier32().get_compact();
        if (assocVol && assocLay) assocLay->setLayerType(iD);        
	if (assocLay) assocLay->setRef((assocLay->surfaceRepresentation().transform().inverse()*gpi)[1]);
      }
    } else {
      ATH_MSG_DEBUG( "cscRE not found:"<<st<<","<<eta<<","<<phi );
    }
  }
  

  if (stationName.substr(0,1)=="T") {
    int st = 7;
    if (stationName.substr(0,3)=="T1F") {
      st = 0;
    } else if (stationName.substr(0,3)=="T1E") {
      st = 1;
    } else if (stationName.substr(0,3)=="T2F") {
      st = 2;
    } else if (stationName.substr(0,3)=="T2E") {
      st = 3;
    } else if (stationName.substr(0,3)=="T3F") {
      st = 4;
    } else if (stationName.substr(0,3)=="T3E") {
      st = 5;
    } else if (stationName.substr(0,3)=="T4F") {
      st = 6;
    }

    const MuonGM::TgcReadoutElement* tgc = m_muonMgr->getTgcReadoutElement(st,eta,phi-1);

    if (!tgc || !(station->trackingVolume()->inside(tgc->center(),0.))  ) {
      unsigned int phit=0;
      while ( phit<48 ) {
	const MuonGM::TgcReadoutElement* tgct = m_muonMgr->getTgcReadoutElement(st,eta,phit);
        if (tgct && station->trackingVolume()->inside(tgct->center(),0.)) {
          tgc = tgct;
          phi = phit;
          // update station identity
          Identifier oldId(station->layerRepresentation()->layerType());
          int stationName = m_tgcIdHelper->stationName(oldId);
          int stationEta  = m_tgcIdHelper->stationEta(oldId);
          Identifier stId = m_tgcIdHelper->elementID(stationName,stationEta,phi);
          station->layerRepresentation()->setLayerType(stId.get_identifier32().get_compact());
          break;
        }
	phit++;  
      }
    }
    
    if (tgc) {

      int etaSt = tgc->getStationEta();
      int phiSt = tgc->getStationPhi();

      bool* validId=new bool(false);
      Identifier wireId  = m_tgcIdHelper->channelID(stationName.substr(0,3),etaSt,phiSt,1,0,1,true,validId);
      if (!(*validId)) ATH_MSG_ERROR( "invalid TGC channel:" << wireId );
      const Amg::Vector3D gp = tgc->channelPos(wireId);
      //std::cout << "wire position:"<< gp << std::endl;
      const Trk::TrackingVolume* assocVol = station->trackingVolume()->associatedSubVolume(gp);
      if (!assocVol) ATH_MSG_DEBUG( "wrong tgcROE?" << stationName <<"," << etaSt <<"," << phiSt );
      if (assocVol && assocVol->confinedLayers()) {
	const std::vector<const Trk::Layer*> layers = assocVol->confinedLayers()->arrayObjects();           
	for (unsigned int il=0;il<layers.size();il++) {
	  wireId  = m_tgcIdHelper->channelID(stationName.substr(0,3),etaSt,phiSt,il+1,0,1,true,validId);            
	  if (!(*validId)) layers[il]->setLayerType(1);
	  else {
	    if (!(*validId)) ATH_MSG_ERROR( "invalid TGC channel:" << wireId );
	    unsigned int id = wireId.get_identifier32().get_compact();
	    layers[il]->setLayerType(id);
            // validation
	    Identifier checkId(layers[il]->layerType());
	    //std::cout <<"is recognized as tgc? " << m_tgcIdHelper->is_tgc(checkId)<< std::endl; 
	    const Trk::PlaneSurface* stripSurf = dynamic_cast<const Trk::PlaneSurface*> (&(tgc->surface(checkId)));
	    //std::cout << "layer, strip surface position:"<< layers[il]->surfaceRepresentation().center()<<","<< stripSurf->center() <<
	    //  ":check:"<< tgc->surface(checkId).center()<<","<<tgc->transform(checkId).translation()<<","<<tgc->channelPos(checkId) << std::endl;
	    if ( (layers[il]->surfaceRepresentation().transform().inverse()*stripSurf->center()).mag()>0.001)   
	      ATH_MSG_DEBUG( "TGC strip plane shifted:"<<st<<","<<eta<<","<<phi<<":" <<
			     layers[il]->surfaceRepresentation().transform().inverse()*stripSurf->center());
	  }
	}
      }
      delete validId; 
      validId = 0;
    } else {
      ATH_MSG_WARNING( name() << "tgcROE not found for :" << stationName <<","<<eta<<","<<phi );         
    }
  }

  if (stationName.substr(0,1)=="B" || stationName.substr(0,1)=="E" ) { 
    // recalculate id
    Identifier stId(station->layerRepresentation()->layerType());
   
    int nameIndex = m_mdtIdHelper->stationNameIndex( stationName.substr(0,3) ); 
    if (station->trackingVolume()->confinedVolumes()) {
      const std::vector<const Trk::TrackingVolume*> cVols = station->trackingVolume()->confinedVolumes()->arrayObjects();
      for (unsigned int i=0; i<cVols.size() ; i++) {
	if (cVols[i]->confinedLayers()) {
	  const std::vector<const Trk::Layer*> cLays = cVols[i]->confinedLayers()->arrayObjects();
          const MuonGM::MdtReadoutElement* mdtROE = 0;
	  for (unsigned int il=0; il<cLays.size() ; il++) {
	    Identifier id(cLays[il]->layerType());
	    if (id.get_compact() > 0 && m_mdtIdHelper->is_mdt(id)) {
              Identifier newId = m_mdtIdHelper->channelID(nameIndex,eta,phi,
							  m_mdtIdHelper->multilayer(id),m_mdtIdHelper->tubeLayer(id),m_mdtIdHelper->tube(id));
              if (!mdtROE) mdtROE=m_muonMgr->getMdtReadoutElement(newId);
              unsigned int newid = newId.get_identifier32().get_compact();
              cLays[il]->setLayerType(newid); 
              // check reference position
              if (mdtROE) {
		double ref = cLays[il]->getRef();
		//double loc = mdtROE->localROPos(newId)[2];     // this does not take into account ROE shift wrt TG station/layer
		double loc = (cLays[il]->surfaceRepresentation().transform().inverse()*mdtROE->tubePos(newId))[1];
		if (fabs(ref)>10e6) {
		  double sign = (ref>0.) ? 1.:-1.;
		  int dec = int(ref/1e5);
		  ref = ref - dec*1e5 -0.5*(sign+1)*1e5;                  
                  if (fabs(ref-loc)>0.001) {    // re-pack
                    cLays[il]->setRef(loc+dec*1e5+0.5*(sign+1)*1e5);
		  }
		} else if ( fabs(ref-loc) > 0.001 )  cLays[il]->setRef(loc);
	      }
            }
	  }
        }
	if (cVols[i]->confinedArbitraryLayers()) {
	  const std::vector<const Trk::Layer*>* cLays = cVols[i]->confinedArbitraryLayers();
	  for (unsigned int il=0; il<cLays->size() ; il++) {
	    Identifier id((*cLays)[il]->layerType());
	    if (id.get_compact() > 0 && m_rpcIdHelper->is_rpc(id)) {
              Identifier newId = m_rpcIdHelper->channelID(nameIndex,eta,phi,m_rpcIdHelper->doubletR(id),
							  m_rpcIdHelper->doubletZ(id),m_rpcIdHelper->doubletPhi(id),m_rpcIdHelper->gasGap(id),
							  m_rpcIdHelper->measuresPhi(id),m_rpcIdHelper->strip(id));
              int newid = newId.get_identifier32().get_compact();
              (*cLays)[il]->setLayerType(newid);  
            }
	  }
	}
      }
    }
  }

  /*
  if (stationName.substr(0,1)=="B" || stationName.substr(0,1)=="E" ) { 

    // MDT
    int nameIndex = m_mdtIdHelper->stationNameIndex( stationName.substr(0,3) ); 
    int nameIndexC = nameIndex;
    if (stationName.substr(0,3)=="EIS") nameIndexC = 22; 
    if (stationName.substr(0,3)=="BIM") nameIndexC = 23; 
    for (int multi = 0; multi < 2; multi++ ) {
      const MuonGM::MdtReadoutElement* multilayer = m_muonMgr->getMdtReadoutElement(nameIndexC,eta+8,phi-1,multi);
      if (multilayer) {
        const Trk::TrackingVolume* assocVol = station->trackingVolume()->associatedSubVolume(multilayer->center());
        if (!assocVol ) ATH_MSG_WARNING( "valid multilayer outside station:" << stationName );
        if (assocVol) {
	  int nLayers = multilayer->getNLayers();
	  for (int layer =1; layer <= nLayers ; layer++) {
	    Identifier id = m_mdtIdHelper->channelID(nameIndex,eta,phi,multi+1,layer,1);           
	    if (id>0) {
	      // retrieve associated layer
	      HepGeom::Point3D<double> gp = multilayer->tubePos(id);
	      const Trk::Layer* assocLay = assocVol->associatedLayer(gp);
	      unsigned int iD = id;
	      if (assocLay && assocLay->layerType()!= iD) {
		std::cout <<"ERROR IDENTIFICATION:"<<stationName<<","<<assocLay->layerType()<<","<<id<<std::endl;
                Identifier newId(assocLay->layerType());  
		std::cout << m_mdtIdHelper->elementID(newId) <<","<< m_mdtIdHelper->elementID(id)<<std::endl;
              }
	      if (assocLay) assocLay->setLayerType(iD); 
	      if (assocLay) {
		//const Trk::LocalPosition* locPos = (assocLay->surfaceRepresentation()).globalToLocal(gp,0.001);
                Amg::Vector3D locPos = assocLay->surfaceRepresentation().transform().inverse()*gp;
		if (fabs(assocLay->getRef()-locPos[1])>0.01) 
		  std::cout << "ERROR REFERENCE:"<< stationName<<","<<assocLay->getRef()<<","<<locPos[1]<<std::endl;
		if (fabs(locPos[2])>0.01) 
		  std::cout << "ERROR Alignment:"<< stationName<<","<<assocLay->getRef()<<","<<locPos[2]<<std::endl;
		assocLay->setRef(locPos[1]);
	      }
	    } 
          }
        }
      }     
    }
    
    // RPC ?
    const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = station->trackingVolume()->confinedVolumes();
    if (confinedVolumes){
      const std::vector<const Trk::TrackingVolume*>& vols = confinedVolumes->arrayObjects();
      for (unsigned int iv=0;iv<vols.size();iv++) if (vols[iv]->volumeName() == "RPC") {
        // for active layers do a search of associated ROE
        const std::vector<const Trk::Layer*>* layers = vols[iv]->confinedArbitraryLayers();
        int nameIndex = m_rpcIdHelper->stationNameIndex( stationName.substr(0,3) ); 
        // loop over doubletR, doubletZ 
	for (int doubletR = 0; doubletR < 2; doubletR++ ) {
	for (int doubletZ = 0; doubletZ < 3; doubletZ++ ) {
	for (int doubletPhi = 0; doubletPhi < 1; doubletPhi++ ) {
	  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(nameIndex-2,eta+8,phi-1,doubletR,doubletZ);
	  if (rpc) {
            if (doubletZ < rpc->getDoubletZ() ) {  
	      for (int gasGap=0; gasGap<2; gasGap++) {
		Identifier etaId = m_rpcIdHelper->channelID(nameIndex,eta,phi,
							    doubletR+1,doubletZ+1,doubletPhi+1,gasGap+1,0,1); 
		Identifier phiId = m_rpcIdHelper->channelID(nameIndex,eta,phi,
							    doubletR+1,doubletZ+1,doubletPhi+1,gasGap+1,1,1); 
		if (m_rpcIdHelper->valid(etaId)){
		  for (unsigned int il=0;il<layers->size();il++) {
		    if ((*layers)[il]->layerType() != 0 && (*layers)[il]->isOnLayer(rpc->stripPos(etaId)) ) {
		      unsigned int id = etaId;
		      if ((*layers)[il]->layerType()!= id) {
                        std::cout <<"ERROR IDENTIFICATION:RPC:"<<stationName<<","<<(*layers)[il]->layerType()<<","<<id<<std::endl;
                        std::cout <<"ERROR IDENTIFICATION:RPC:"<<il<<","<<(*layers)[il]->surfaceRepresentation().center() <<std::endl;
			Identifier newId((*layers)[il]->layerType());  
			std::cout << m_rpcIdHelper->elementID(newId) <<","<< m_rpcIdHelper->elementID(etaId)<<std::endl;
			std::cout << m_rpcIdHelper->doubletR(newId) <<","<< m_rpcIdHelper->doubletR(etaId)<<std::endl;
                        std::cout << m_rpcIdHelper->doubletZ(newId)<<","<<m_rpcIdHelper->doubletPhi(newId)<<","<<m_rpcIdHelper->gasGap(newId)
				  <<","<<m_rpcIdHelper->measuresPhi(newId)<<","<<m_rpcIdHelper->strip(newId)<<std::endl;
                        std::cout << m_rpcIdHelper->doubletZ(etaId)<<","<<m_rpcIdHelper->doubletPhi(etaId)<<","<<m_rpcIdHelper->gasGap(etaId)
				  <<","<<m_rpcIdHelper->measuresPhi(etaId)<<","<<m_rpcIdHelper->strip(etaId)<<std::endl;
                      } else {
                        std::cout <<"RPC IDENTIFICATION OK:"<<stationName<<","<<(*layers)[il]->layerType()<<","<<id<<std::endl;
                        std::cout <<"RPC IDENTIFICATION OK:"<<il<<","<<(*layers)[il]->surfaceRepresentation().center() <<std::endl;
                      }
		      (*layers)[il]->setLayerType(id);
                      //turn eta position into integer to truncate
		      HepGeom::Point3D<double> locPosEta = ((*layers)[il]->surfaceRepresentation().transform().inverse()) * (rpc->stripPos(etaId));
		      HepGeom::Point3D<double> locPosPhi = ((*layers)[il]->surfaceRepresentation().transform().inverse()) * (rpc->stripPos(phiId));
                      int etaRefi = int(1000*locPosEta[Trk::locY]);                       
		      (*layers)[il]->setRef( 10e4+locPosPhi[Trk::locX] + 10e5*(etaRefi+10e6));
		    } 
		  }
		}
	 }}}}}}                  
      }
    }
  } 
  */ 

  // by now, all the layers should be identified - verify
  if (station->trackingVolume()->confinedVolumes()) {
    const std::vector<const Trk::TrackingVolume*> cVols = station->trackingVolume()->confinedVolumes()->arrayObjects();
    for (unsigned int i=0; i<cVols.size() ; i++) {
      if (cVols[i]->confinedLayers()) {
        const std::vector<const Trk::Layer*> cLays = cVols[i]->confinedLayers()->arrayObjects();
        for (unsigned int il=0; il<cLays.size() ; il++) {
          Identifier id(cLays[il]->layerType());
          if (id==1) ATH_MSG_DEBUG(station->name()<<","<< cVols[i]->volumeName()<<", unidentified active layer:"<<il );
        }
      }
      if (cVols[i]->confinedArbitraryLayers()) {
        const std::vector<const Trk::Layer*>* cLays = cVols[i]->confinedArbitraryLayers();
        for (unsigned int il=0; il<cLays->size() ; il++) {
          Identifier id((*cLays)[il]->layerType());
          if (id==1) ATH_MSG_DEBUG( station->name()<<","<< cVols[i]->volumeName()<<", unidentified active layer:"<<il );
        }
      }
    }
  } 
  if (station->trackingVolume()->confinedLayers()) {
    const std::vector<const Trk::Layer*> cLays = station->trackingVolume()->confinedLayers()->arrayObjects();
    for (unsigned int il=0; il<cLays.size() ; il++) {
      Identifier id(cLays[il]->layerType());
      if (id==1) ATH_MSG_DEBUG( station->name()<<","<< station->name()<<", unidentified active layer:"<<il );
    }
  }
  // end identification check

}

void Muon::MuonStationBuilder::identifyPrototype(const Trk::TrackingVolume* station, int eta, int phi, Amg::Transform3D transf ) const
{
  ATH_MSG_VERBOSE( name() <<" identifying prototype " );    

  std::string stationName = station->volumeName();
  ATH_MSG_VERBOSE( " for station " << stationName );    

  if (stationName.substr(0,1)=="B" || stationName.substr(0,1)=="E" ) { 
    // MDT
    int nameIndex = m_mdtIdHelper->stationNameIndex( stationName.substr(0,3) ); 
    int nameIndexC = nameIndex;
    if (stationName.substr(0,3)=="EIS") nameIndexC = 22; 
    if (stationName.substr(0,3)=="BIM") nameIndexC = 23; 
    if (stationName.substr(0,3)=="BME") nameIndexC = 24; 
    if (stationName.substr(0,3)=="BMG") nameIndexC = 25; 

    for (int multi = 0; multi < 2; multi++ ) {
      const MuonGM::MdtReadoutElement* multilayer = m_muonMgr->getMdtReadoutElement(nameIndexC,eta+8,phi-1,multi);
      if (multilayer) {
        const Trk::TrackingVolume* assocVol = station->associatedSubVolume(transf.inverse()*multilayer->center());
        if (!assocVol ) {
	  ATH_MSG_WARNING( "valid multilayer outside station:" << stationName );
	  //ATH_MSG_WARNING( "multilayer position:" << multilayer->center()<<", local: "<< transf.inverse()*multilayer->center() );
	}
        if (assocVol) {
	  int nLayers = multilayer->getNLayers();
	  for (int layer =1; layer <= nLayers ; layer++) {
	    Identifier id = m_mdtIdHelper->channelID(nameIndex,eta,phi,multi+1,layer,1);           
	    if (id.get_compact() > 0) {
	      // retrieve associated layer
	      Amg::Vector3D gp = multilayer->tubePos(id);
	      const Trk::Layer* assocLay = assocVol->associatedLayer(transf.inverse()*gp);
	      unsigned int iD = id.get_identifier32().get_compact();
	      if (assocLay) assocLay->setLayerType(iD); 
	      //if (assocLay) {
	      //  const Amg::Vector2D* locPos = (assocLay->surfaceRepresentation()).globalToLocal(gp,0.001);
              //  if (locPos) {
	      //    if (fabs(assocLay->getRef()-(*locPos)[Trk::locY])>0.01) { 
	      //    std::cout << "ERROR REFERENCE:"<< stationName<<","<<assocLay->getRef()<<","<<(*locPos)[Trk::locY]<<std::endl;
	      //  } else {
	      //    std::cout << "station identified:"<< stationName<<","<<assocLay->getRef()<<","<<(*locPos)[Trk::locY]<<std::endl;
	      //  }
	      //  assocLay->setRef((*locPos)[Trk::locY]);
	      //}
	      //}
	    } 
          }
        }
      }     
    }
    
    // RPC ?
    const Trk::BinnedArray< Trk::TrackingVolume >* confinedVolumes = station->confinedVolumes();
    if (confinedVolumes){
      const std::vector<const Trk::TrackingVolume*>& vols = confinedVolumes->arrayObjects();
      for (unsigned int iv=0;iv<vols.size();iv++) if (vols[iv]->volumeName() == "RPC") {
        // for active layers do a search of associated ROE
        const std::vector<const Trk::Layer*>* layers = vols[iv]->confinedArbitraryLayers();
        int nameIndex = m_rpcIdHelper->stationNameIndex( stationName.substr(0,3) ); 
        if (stationName.substr(0,3)=="BME") nameIndex=12;              // hack for BME
        if (stationName.substr(0,3)=="BMG") nameIndex=13;              // hack for BMG (even though BMG don't have RPC)
        // loop over doubletR, doubletZ 
	for (int doubletR = 0; doubletR < 2; doubletR++ ) {
	for (int doubletZ = 0; doubletZ < 3; doubletZ++ ) {
	for (int doubletPhi = 0; doubletPhi < 1; doubletPhi++ ) {
	  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(nameIndex-2,eta+8,phi-1,doubletR,doubletZ);
	  if (rpc) {
            if (doubletZ < rpc->getDoubletZ() ) {  
	      for (int gasGap=0; gasGap<2; gasGap++) {
		Identifier etaId = m_rpcIdHelper->channelID(nameIndex,eta,phi,
							    doubletR+1,doubletZ+1,doubletPhi+1,gasGap+1,0,1); 
		//Identifier phiId = m_rpcIdHelper->channelID(nameIndex,eta,phi,
		//					    doubletR+1,doubletZ+1,doubletPhi+1,gasGap+1,1,1); 
		if (1/*m_rpcIdHelper->valid(etaId)*/){
		  for (unsigned int il=0;il<layers->size();il++) {
		    if ((*layers)[il]->layerType() != 0 && (*layers)[il]->surfaceRepresentation().isOnSurface(transf.inverse()*rpc->stripPos(etaId),false,0.5*(*layers)[il]->thickness() ) ) {
                      const Amg::Vector3D locPos1 = (*layers)[il]->surfaceRepresentation().transform().inverse()*transf.inverse()*rpc->stripPos(etaId);
                      const Amg::Vector3D locPos2 = rpc->surface(etaId).transform().inverse()*rpc->stripPos(etaId);
                      double swap = ( fabs( locPos1[1] - locPos2[0] ) > 0.001 ) ? 20000. : 0. ;
		      unsigned int id = etaId.get_identifier32().get_compact();
		      (*layers)[il]->setLayerType(id);
                      const Amg::Vector3D locPos = (*layers)[il]->surfaceRepresentation().transform().inverse()
			*transf.inverse()*rpc->surface(etaId).center(); 
		      (*layers)[il]->setRef(swap + locPos[0]);
                      //turn eta position into integer to truncate
		      //HepGeom::Point3D<double> locPosEta = ((*layers)[il]->surfaceRepresentation().transform().inverse()) * (rpc->stripPos(etaId));
		      //HepGeom::Point3D<double> locPosPhi = ((*layers)[il]->surfaceRepresentation().transform().inverse()) * (rpc->stripPos(phiId));
                      //int etaRefi = int(1000*locPosEta[Trk::locY]);                       
		      //(*layers)[il]->setRef( 10e4+locPosPhi[Trk::locX] + 10e5*(etaRefi+10e6));
		    } 
		  }
		}
	 }}}}}}                  
      }
    }
  } 

  // by now, all the layers should be identified - verify
  if (station->confinedVolumes()) {
    const std::vector<const Trk::TrackingVolume*> cVols = station->confinedVolumes()->arrayObjects();
    for (unsigned int i=0; i<cVols.size() ; i++) {
      if (cVols[i]->confinedLayers()) {
        const std::vector<const Trk::Layer*> cLays = cVols[i]->confinedLayers()->arrayObjects();
        for (unsigned int il=0; il<cLays.size() ; il++) {
          Identifier id(cLays[il]->layerType());
          if (id==1) ATH_MSG_DEBUG(station->volumeName()<<","<< cVols[i]->volumeName()<<", unidentified active layer:"<<il );
        }
      }
      if (cVols[i]->confinedArbitraryLayers()) {
        const std::vector<const Trk::Layer*>* cLays = cVols[i]->confinedArbitraryLayers();
        for (unsigned int il=0; il<cLays->size() ; il++) {
          Identifier id((*cLays)[il]->layerType());
          if (id==1) ATH_MSG_DEBUG(station->volumeName()<<","<< cVols[i]->volumeName()<<", unidentified active layer:"<<il );
        }
      }
    }
  } 
  if (station->confinedLayers()) {
    const std::vector<const Trk::Layer*> cLays = station->confinedLayers()->arrayObjects();
    for (unsigned int il=0; il<cLays.size() ; il++) {
      Identifier id(cLays[il]->layerType());
      if (id==1) ATH_MSG_DEBUG( station->volumeName()<<","<< station->volumeName()<<", unidentified active layer:"<<il );
    }
  }
  // end identification check
}


void Muon::MuonStationBuilder::getObjsForTranslation(const GeoVPhysVol* pv, std::string name, Amg::Transform3D transform, std::vector<std::pair<std::pair<const GeoLogVol*,Trk::MaterialProperties*> , std::vector<Amg::Transform3D> > >& vols, std::vector< std::string >& volNames ) const
{
  // subcomponents 
  unsigned int nc = pv->getNChildVols();
  //std::cout << "getObjsForTranslation from:"<< pv->getLogVol()->getName()<<","<<pv->getLogVol()->getMaterial()->getName()<<", looping over "<< nc << " children" << std::endl;
  double thick = 2*m_muonStationTypeBuilder->get_x_size(pv);
  double vol = m_muonStationTypeBuilder->getVolume(pv->getLogVol()->getShape()); 
  Trk::MaterialProperties matComb = m_muonStationTypeBuilder->getAveragedLayerMaterial(pv,vol,thick);
  //if (matComb) std::cout << "thickness, averaged x0:"<< matComb->thickness()<<","<< matComb->x0()<< std::endl;
  //double dInX0 = matComb->thickness()/matComb->x0(); 
  for (unsigned int ic=0; ic<nc; ic++) {
    Amg::Transform3D transf = Amg::CLHEPTransformToEigen( pv->getXToChildVol(ic));
    const GeoVPhysVol* cv = &(*(pv->getChildVol(ic)));
    const GeoLogVol* clv = cv->getLogVol();
    std::string childName = clv->getName();
    bool matMode = false;
    if (childName=="") childName="Spacer";
    if (childName.size()>9 && childName.substr(childName.size()-9,9)=="Sensitive") {
      std::stringstream st;
      st << ic; 
      childName=childName+st.str();
      matMode = true;
    }
    if (!matMode )  {  //don't take material from mother volume
      thick = 2*m_muonStationTypeBuilder->get_x_size(cv);
      vol = m_muonStationTypeBuilder->getVolume(cv->getLogVol()->getShape()); 
      //std::cout <<"collecting material for daughter volume:"<<thick<<","<<vol<<std::endl;
      matComb = m_muonStationTypeBuilder->getAveragedLayerMaterial(cv,vol,thick);
      //std::cout << "thickness, averaged x0:"<< matComb->thickness()<<","<< matComb->x0()<< std::endl;
      //dInX0 = matComb->thickness()/matComb->x0();
    }

    std::string cName = childName.substr(0,3)=="NSW" || childName.substr(0,8)=="NewSmall"? name : name+childName;
    //std::cout << "child number,name,position:"<< ic<<":"<<clv->getName() <<":"<< (transform*transf).translation().perp() <<","<<(transform*transf).translation().z()<<","<<(transform*transf).translation().phi() << std::endl;
    if (!cv->getNChildVols()) {
      bool found = false;
      for (unsigned int is = 0; is < vols.size(); is++) {
	if (cName == volNames[is]) {
	   if ( fabs((transform*transf).translation().perp()- vols[is].second.front().translation().perp())< 1. ) {
	    found = true; 
            // order transforms to position prototype at phi=0/ 0.125 pi
            double phiTr = (transform*transf).translation().phi();
            if ( phiTr>-0.001 && phiTr<0.4 ) {
              vols[is].second.insert(vols[is].second.begin(),transform*transf);
            } else vols[is].second.push_back(transform*transf);
	    //std::cout << "clone?" << clv->getName() <<","<<(transform*transf).translation().perp()<<","<<(transform*transf).translation().z()<<","<<phiTr << std::endl;
	    break;
	   }
	}
      }
      if (!found) {
	std::vector<Amg::Transform3D > volTr;
	volTr.push_back(transform*transf);
        // divide mother material ? seems strange - check !
        //double scale =  1.;     // 1./nc;
	Trk::MaterialProperties* nMat=new Trk::MaterialProperties(matComb);

	std::pair<const GeoLogVol*,Trk::MaterialProperties*> cpair(clv,nMat);
	vols.push_back(std::pair<std::pair<const GeoLogVol*,Trk::MaterialProperties*>,std::vector<Amg::Transform3D> > (cpair,volTr) );
        volNames.push_back(cName);
	//std::cout << "new volume added:"<< cName <<","<<clv->getMaterial()->getName()<<","<< volTr.back().translation().z()<<","<<volTr.back().translation().phi()<<":mat:"<<matComb->thicknessInX0()<<std::endl;
	//printInfo(cv);
      }
    } else {
      getObjsForTranslation(cv, cName, transform*transf, vols, volNames);
    }
  }
}

/*
void Muon::MuonStationBuilder::getObjsForTranslation(const GeoVPhysVol* pv, std::string name, HepGeom::Transform3D transform, std::vector<std::pair<const GeoLogVol*, std::vector<HepGeom::Transform3D> > >& vols, std::vector< std::string >& volNames ) const
{
  // subcomponents 
  unsigned int nc = pv->getNChildVols();
  std::cout << "getObjsForTranslation from:"<< pv->getLogVol()->getName()<<","<<pv->getLogVol()->getMaterial()->getName()<<", looping over "<< nc << " children" << std::endl;
  for (unsigned int ic=0; ic<nc; ic++) {
    HepGeom::Transform3D transf = pv->getXToChildVol(ic);
    const GeoVPhysVol* cv = &(*(pv->getChildVol(ic)));
    const GeoLogVol* clv = cv->getLogVol();
    std::string childName = clv->getName();
    if (childName=="") childName="Spacer";
    if (childName.size()>9 && childName.substr(childName.size()-9,9)=="Sensitive") {
      std::stringstream st;
      st << ic; 
      childName=childName+st.str();
    }
    std::string cName = childName.substr(0,3)=="NSW" ? name : name+childName;
    std::cout << "child number,name,position:"<< ic<<":"<<clv->getName() <<":"<< (transform*transf).translation().perp() <<","<<(transform*transf).translation().z()<<","<<(transform*transf).translation().phi() << std::endl;
    //if (!cv->getNChildVols()) {
      bool found = false;
      for (unsigned int is = 0; is < vols.size(); is++) {
	if (cName == volNames[is]) {
	   if ( fabs((transform*transf).translation().perp()- vols[is].second.front().translation().perp())< 1. ) {
	    found = true; 
            // order transforms to position prototype at phi=0/ 0.125 pi
            double phiTr = (transform*transf).translation().phi();
            if ( phiTr>-0.001 && phiTr<0.4 ) {
              vols[is].second.insert(vols[is].second.begin(),transform*transf);
            } else vols[is].second.push_back(transform*transf);
	    std::cout << "clone?" << clv->getName() <<","<<(transform*transf).translation().perp()<<","<<(transform*transf).translation().z()<<","<<phiTr << std::endl;
	    break;
	   }
	}
      }
      if (!found) {
	std::vector<HepGeom::Transform3D > volTr;
	volTr.push_back(transform*transf); 
	vols.push_back(std::pair<const GeoLogVol*,std::vector<HepGeom::Transform3D> > (clv,volTr) );
        volNames.push_back(cName);
	std::cout << "new volume added:"<< cName <<","<<clv->getMaterial()->getName()<<","<< volTr.back().translation().z()<<","<<volTr.back().translation().phi()<<std::endl;
	//printInfo(cv);
      }
   //} else {
   // getObjsForTranslation(cv, cName, transform*transf, vols, volNames);
   // }
  }
}
*/

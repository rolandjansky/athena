/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialLayerHelper.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkDetDescrGeoModelCnv/MaterialLayerHelper.h"
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/CylinderLayer.h"
#include "TrkGeometry/DiscLayer.h"
#include "TrkGeometry/BinnedLayerMaterial.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
// Amg
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// #define DEBUG
#ifdef DEBUG
#  define DEBUG_TRACE(a) do { a } while (0)
#else
#  define DEBUG_TRACE(a) do {  } while (0)
#endif

namespace Trk{
  typedef std::pair< SharedObject<const Surface>, Amg::Vector3D > SurfaceOrderPosition;
}

void Trk::MaterialLayerHelper::processMaterial(const Trk::TrackingVolume* inputVol, std::vector<MaterialElement>& inputMat, float beamPipeRadius, bool printInfo)
{
  // if no input tracking volume, bail out
  if ( !inputVol ) return;  

  // extract summary information about material in GeoModel
  if (printInfo) materialBudget(inputMat); 
 
  // loop over input volume and collect material layers
  std::vector< const Trk::Layer* >  matLayers;
  getMaterialLayers( inputVol, matLayers ); 

  // resolve volume boundaries
  const Trk::CylinderVolumeBounds* cylBounds = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(inputVol->volumeBounds()));
  if (!cylBounds) {
    DEBUG_TRACE( std::cout<<"Trk::MaterialLayerHelped failed to retrieve cylindrical bounds for input volume, material mapping not done"<< std::endl; );
    return;
  }
  m_rExt = cylBounds->outerRadius() ;
  m_zExt = cylBounds->halflengthZ() ;
  m_rBP = beamPipeRadius; 

  // prepare the material collection for merge
  m_mat4merge.clear();
  for (unsigned int j=0; j< matLayers.size(); j++) m_mat4merge.push_back(std::vector< MaterialElement>());

  // assign material objects to layers
  findClosestLayer( inputMat, matLayers);

  // merge material into binned material arrays
  buildMaterial( matLayers );

}


void Trk::MaterialLayerHelper::getMaterialLayers(const Trk::TrackingVolume* inputVol, std::vector< const Trk::Layer* >& matLayers) {

  if (inputVol->confinedVolumes() ) {
    const std::vector<const Trk::TrackingVolume*>& volumes = inputVol->confinedVolumes()->arrayObjects();
    for (auto vol : volumes) getMaterialLayers( vol, matLayers ); 
  }

  if (inputVol->confinedLayers() ) {
    const std::vector<const Trk::Layer*>& layers = inputVol->confinedLayers()->arrayObjects();
    for (auto lay : layers) {
      if (lay->layerMaterialProperties()) matLayers.push_back(lay); 
    }
  }
  
  // Process the boundary surface layers 
  auto& bSurfaces = inputVol->boundarySurfaces();
  for (auto& bsIter : bSurfaces ) 
    if (bsIter->surfaceRepresentation().materialLayer()) {
      // boundaries shared : check if not added already
      bool notCounted = true;
      for (auto mlay : matLayers) if (bsIter->surfaceRepresentation().materialLayer() == mlay ) { notCounted=false; break;}
      if (notCounted) matLayers.push_back(bsIter->surfaceRepresentation().materialLayer());
    }
}


void Trk::MaterialLayerHelper::findClosestLayer(std::vector<MaterialElement>& matInput, std::vector< const Trk::Layer* >&  layers) {

  // convert into MaterialObject & assign to closest layer
  const Trk::GeoMaterialConverter matCnvTool;
  
  double massTot = 0.;
  double massInp = 0.;
  m_unassignedMass=0.;
  m_unassignedITk =0.;
 
  double precision = 10.; 

  std::vector<std::pair<double,double> > layThRange;
  std::vector<std::pair<const Trk::CylinderBounds*,const Trk::DiscBounds*> > layBounds;
  for (unsigned int i=0; i<layers.size(); i++) {
    const Trk::Layer* lay = layers[i];
    const Trk::DiscBounds*  db = dynamic_cast<const Trk::DiscBounds*> (&lay->surfaceRepresentation().bounds()); 
    const Trk::CylinderBounds*  cyl = db ? 0 : dynamic_cast<const Trk::CylinderBounds*> (&lay->surfaceRepresentation().bounds()); 
    float zLay = lay->surfaceRepresentation().center().z(); 
    layBounds.push_back(std::pair<const Trk::CylinderBounds*,const Trk::DiscBounds*>(cyl,db)); 
    if (cyl) {
      double zLayMin = zLay-cyl->halflengthZ(); double zLayMax =  zLay+cyl->halflengthZ();
      double layThMax = zLayMin==0 ? acos(-1.)/2. : atan(cyl->r()/zLayMin); if (zLayMin<0) layThMax+=acos(-1.);
      double layThMin = zLayMax==0 ? acos(-1.)/2. : atan(cyl->r()/zLayMax); if (zLayMax<0) layThMin+=acos(-1.);
      layThRange.push_back(std::pair<double,double>(layThMin,layThMax));
    } else if (db) {
      double layThMax = zLay==0 ? acos(-1.)/2. : ( zLay>0 ? atan(db->rMax()/zLay) : atan(db->rMin()/zLay)+acos(-1.) );
      double layThMin = zLay==0 ? acos(-1.)/2. : ( zLay>0 ? atan(db->rMin()/zLay) : atan(db->rMax()/zLay)+acos(-1.) );
      layThRange.push_back(std::pair<double,double>(layThMin,layThMax));
    }
  }

  const Trk::Layer* assocLayer=0;
  int index=-1; 
  float dist = 5.e5;
  
  std::vector< std::pair<int, std::pair<float,float> > > closeLays;

  float mLimitShuffle = 500.;   // this is the limit of material reshuffling (in g)
  float mLimitScale = 0.1;  // this is the limit beyond which we cut objects 

  float massContained = 0;  unsigned int nAssocContained = 0; 

  for ( unsigned int ie=0; ie<matInput.size(); ie++ ) {
    MaterialElement mat = matInput[ie];      // updates done on local copy of the material object ( except for the splitted volumes )
 
    // ignore objects beyond the volume boundaries !TODO extend to deal with volumes shifted in z!
    if (mat.rmin>=m_rExt) continue; 
    if (mat.zmin>=m_zExt) continue;  
    if (mat.zmax<=-m_zExt) continue;  

    // remove material beyond volume boundaries
    if (mat.rmax>m_rExt)  {
      float inscale = ( m_rExt*m_rExt -mat.rmin*mat.rmin)/(mat.rmax*mat.rmax-mat.rmin*mat.rmin); 
      mat.rmax=m_rExt; mat.geoVolume = inscale*mat.geoVolume;
    }
    if (mat.zmax>m_zExt)  {
      float inscale = ( m_zExt-mat.zmin )/( mat.zmax - mat.zmin ); 
      mat.zmax=m_zExt; mat.geoVolume = inscale*mat.geoVolume;
    }
    if (mat.zmin<-m_zExt) {
      float inscale = ( mat.zmax+m_zExt )/( mat.zmax - mat.zmin ); 
      mat.zmin=-m_zExt; mat.geoVolume = inscale*mat.geoVolume;
    }

    massContained += mat.mass(); 
    nAssocContained++; 
    //
    double dr = mat.rmax-mat.rmin;
    double dz = mat.zmax-mat.zmin;
    if (dr==0 && mat.mass()>0) {
      float dphi = mat.phimax-mat.phimin;
      if (dphi<=0 || dphi>2*acos(-1.)) dphi=2*acos(-1.);
      dr = mat.geoVolume/dz/dphi/mat.rmin;
      mat.rmin = mat.rmin-0.5*dr;
      mat.rmax =mat.rmax+0.5*dr;
      } 

    if (mat.volume()<=0.) {
       m_unassignedMass += mat.mass();
      continue; 
    }    

    Trk::Material  material = matCnvTool.convert(mat.material);
    double scale =   (mat.geoVolume>0.) ? mat.geoVolume/mat.volume() : 1. ;
    mat.setMaterial(Trk::Material( material.X0/scale, material.L0/scale, scale*material.A, scale*material.Z, scale*material.rho));
  
    assocLayer=0;
    dist = 5.e5;
    index=-1; 
    float thOverlap = 0.;
    double rRef = 0.5*(mat.rmax+mat.rmin);
    double zRef = 0.5*(mat.zmax+mat.zmin);
    closeLays.clear();

    // loop over layers to find best match
    float d=0.;float overlap=0.; float zpos=0.;
 
    for (unsigned int i=0; i<layers.size(); i++) {
      
      if ( mat.thmin< layThRange[i].second && mat.thmax > layThRange[i].first ) {
	
	const Trk::Layer* lay = layers[i];      
	const Trk::DiscBounds*  db = layBounds[i].second; 
	const Trk::CylinderBounds*  cyl = layBounds[i].first; 
	
	if (db && mat.rmin<m_rBP ) continue;    // TODO : check if needed
	if (cyl && cyl->r()<m_rBP && mat.rmax > m_rBP) continue;  //TODO check if needed
	
	zpos = lay->surfaceRepresentation().center().z();
        // evaluate the overlap in theta 
	overlap =  (fmin(mat.thmax,layThRange[i].second) -fmax(mat.thmin,layThRange[i].first))/(mat.thmax-mat.thmin);
        // evaluate the relative scaling associated with the projection
	if (cyl) d=fabs(cyl->r()/rRef-1.);
	else if (db) d=  zRef!=0 ? fabs(zpos/zRef-1.) : fabs(zpos);   // absolute distance for disks at z=0.
	
	if (d<dist) {dist=d; assocLayer = lay; index = i; thOverlap = overlap; }
	if (overlap>0 ) { // order in distance weighted by overlap
	  if (!closeLays.size()) closeLays.push_back(std::pair<int,std::pair<float,float> > (i,std::pair<float,float>(d,overlap)));
          else {
	    std::vector< std::pair<int, std::pair<float,float> > >::iterator cIt = closeLays.begin();
            if (!db || zRef!=0) {
	      if ( d<mLimitScale) {   //  among close layers, order in decreasing overlap
		while (cIt!=closeLays.end() && (*cIt).second.second>overlap && (*cIt).second.first<mLimitScale ) cIt++;   	
              } else {
		while (cIt!=closeLays.end() && (*cIt).second.first<mLimitScale ) cIt++;   	
                while (cIt!=closeLays.end() && (*cIt).second.first*((*cIt).second.second>0.99 ? 1. : 1.-(*cIt).second.second)<d*(overlap>0.99? 1. : 1.-overlap) ) cIt++;   	
	      }
	    }  else 
	      while (cIt!=closeLays.end() && (*cIt).second.first*((*cIt).second.second>0.99 ? 1. : 1.-(*cIt).second.second)<d*(overlap>0.99? 1. : 1.-overlap) ) cIt++;   	
            //  insert layer 
	    closeLays.insert(cIt,std::pair<int,std::pair<float,float> > (i,std::pair<float,float>(d,overlap)));
	  }
	} 
      }
      
      if (dist==0 && assocLayer && overlap==1.) break; 
    }

   // first, closest layer chosen, check if overlap acceptable : change if another suitable layer found, or split volume
   if (thOverlap<1. ) {  //  partial overlap
      float mDiff = (1.-thOverlap)*mat.mass();
      if (mDiff<mLimitScale*mLimitShuffle)  {     //  not worth the optimization 
      } else if (mDiff<mLimitShuffle) {  // associated layer acceptable, just check if no better solution
 	if (closeLays.size() && closeLays[0].first!=index) {
          if ( closeLays[0].second.first<mLimitScale && (1-closeLays[0].second.second)*mat.mass()<mDiff) {   // update
	    index = closeLays[0].first; assocLayer = layers[index];
            dist = closeLays[0].second.first;
            thOverlap =  closeLays[0].second.second;
          }
	}
      } else {     // action required
         if (closeLays.size() && closeLays[0].first!=index) {
	   index = closeLays[0].first; assocLayer = layers[index];
	   dist = closeLays[0].second.first;
	   thOverlap =  closeLays[0].second.second;
	 }
	 // the best alternative accepted if not too far and the bias acceptable 
	 if ( closeLays[0].second.first<mLimitScale && (1-closeLays[0].second.second)*mat.mass()<2*mLimitShuffle) {   // update
	 } else if ( closeLays[0].second.first<mLimitScale && closeLays[0].second.second>1.-mLimitScale){
	 } else {
	   float dth = layThRange[index].first - mat.thmin;
	   if (dth>mLimitScale*(mat.thmax-mat.thmin)) {
	     if (mat.rmax-mat.rmin<mat.zmax-mat.zmin) {
	       float zcut = 0.5*(mat.rmin+mat.rmax)/tan(layThRange[index].first);
	       if (zcut>mat.zmin && zcut<mat.zmax) {
		 matInput.push_back(MaterialElement(mat.name+"_lowsplit",mat.rmin,mat.rmax,mat.zmin,zcut,mat.phimin,mat.phimax,
						    mat.material,(zcut-mat.zmin)/(mat.zmax-mat.zmin)*mat.geoVolume));
		 matInput[ie].zmin = zcut; matInput[ie].geoVolume = (mat.zmax-zcut)/(mat.zmax-mat.zmin)*mat.geoVolume;
		 mat.zmin = zcut; mat.geoVolume = matInput[ie].geoVolume;
	       }
	     } else {
	       float rcut = 0.5*(mat.zmin+mat.zmax)*tan(layThRange[index].first);
	       if (rcut>mat.rmin && rcut<mat.rmax) {
		 if (zRef<0)  {
		   matInput.push_back(MaterialElement(mat.name+"_lowsplit",mat.rmin,rcut,mat.zmin,mat.zmax,mat.phimin,mat.phimax,
						      mat.material,(rcut*rcut-mat.rmin*mat.rmin)/(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.geoVolume));
		   matInput[ie].rmin = rcut; matInput[ie].geoVolume = (mat.rmax*mat.rmax-rcut*rcut)/(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.geoVolume;
		 } else {
		   matInput.push_back(MaterialElement(mat.name+"_lowsplit",rcut,mat.rmax,mat.zmin,mat.zmax,mat.phimin,mat.phimax,
						      mat.material,(mat.rmax*mat.rmax-rcut*rcut)/(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.geoVolume));
		   matInput[ie].rmax = rcut; matInput[ie].geoVolume = (rcut*rcut-mat.rmin*mat.rmin)/(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.geoVolume;
		 }
	       }
	     } 
	   }
	   dth = mat.thmax -layThRange[index].second ;
	   if (dth>mLimitScale*(mat.thmax-mat.thmin)) {
	     if (mat.rmax-mat.rmin<mat.zmax-mat.zmin) {
	       float zcut = 0.5*(mat.rmin+mat.rmax)/tan(layThRange[index].second);
	       if (zcut>mat.zmin && zcut<mat.zmax) {
		 matInput.push_back(MaterialElement(mat.name+"_upsplit",mat.rmin,mat.rmax,zcut,mat.zmax,mat.phimin,mat.phimax,
						    mat.material,(mat.zmax-zcut)/(mat.zmax-mat.zmin)*mat.geoVolume));
		 matInput[ie].zmax = zcut; matInput[ie].geoVolume = (zcut-mat.zmin)/(mat.zmax-mat.zmin)*mat.geoVolume;
		 mat.zmax = zcut; mat.geoVolume = matInput[ie].geoVolume;
	       }
	     } else {
	       float rcut = 0.5*(mat.zmin+mat.zmax)*tan(layThRange[index].second);
	       if (rcut>mat.rmin && rcut<mat.rmax) {
		 if (zRef>0)  {
			       matInput.push_back(MaterialElement(mat.name+"_upsplit",mat.rmin,rcut,mat.zmin,mat.zmax,mat.phimin,mat.phimax,
								  mat.material,(rcut*rcut-mat.rmin*mat.rmin)/(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.geoVolume));
			       matInput[ie].rmin = rcut; matInput[ie].geoVolume = (mat.rmax*mat.rmax-rcut*rcut)/(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.geoVolume;
		 } else {
		   matInput.push_back(MaterialElement(mat.name+"_lowsplit",rcut,mat.rmax,mat.zmin,mat.zmax,mat.phimin,mat.phimax,
						      mat.material,(mat.rmax*mat.rmax-rcut*rcut)/(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.geoVolume));
		   matInput[ie].rmax = rcut; matInput[ie].geoVolume = (rcut*rcut-mat.rmin*mat.rmin)/(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.geoVolume;
		 }
	       }
	     } 
	   }
	 }
      }  // end of action item
   }   // end treatment of partial overlap
   
   if (assocLayer) {
     // resolve projections
     if (m_mat4merge[index].size() && mat.name ==  m_mat4merge[index].back().name) { 
       bool mmatch =  fabs(mat.mass()- m_mat4merge[index].back().mass())<0.01*mat.mass();
       bool zmatch = fabs(mat.zmin- m_mat4merge[index].back().zmin)<precision
								    &&  fabs(mat.zmax- m_mat4merge[index].back().zmax)<precision;
       bool rmatch = fabs(mat.rmin- m_mat4merge[index].back().rmin)<precision
								    &&  fabs(mat.rmax- m_mat4merge[index].back().rmax)<precision;
       if ( mmatch && zmatch && rmatch ) {
	 m_mat4merge[index].back().multi =  m_mat4merge[index].back().multi+1;
       } else {
         unsigned int iord = 0;
         while (iord<m_mat4merge[index].size() && mat.mass()<m_mat4merge[index][iord].mass()) iord++;
	 m_mat4merge[index].push_back(mat);	  
       }
     } else  m_mat4merge[index].push_back(mat);
     
   } else {
     DEBUG_TRACE( std::cout <<"no suitable layer found for material element:"<<mat.rmin<<","<<mat.rmax<<":"<<mat.zmin<<","<<mat.zmax<< std::endl; );
     m_unassignedMass += mat.mass();
     if (mat.rmin>m_rBP && mat.zmax<m_zExt && mat.zmin>-m_zExt) m_unassignedITk += mat.mass();
   }
   
   double dphi = mat.phimax-mat.phimin;
   if (dphi<0 || dphi>2*acos(-1.)) dphi=2*acos(-1.);
   // sanity check
   double massObj= 0.5*dphi*(mat.zmax-mat.zmin)*(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.matProp.rho;    
   massTot+=massObj;
   massInp+=mat.mass();

   }
  DEBUG_TRACE( std::cout<<"total mass:contained,read:"<< massContained<<","<<massInp<<std::endl; );
  DEBUG_TRACE( std::cout<<"unassigned:"<< m_unassignedMass<<":"<<m_unassignedITk<< std::endl; );

}

void Trk::MaterialLayerHelper::buildMaterial(std::vector< const Trk::Layer* >&  matLayers)
{

  int icounter = 0;
  float tol = 10.;    // smallest array step
  float heavyLimit = 500.;  // set a limit for objects worth additional effort (splits)

  static FILE* flog = 0;
  flog = fopen("layer_material.dump","w+");
  float sbin =0.; float dInXbin = 0.;

  float layMatTot=0.;

  for (auto lay : matLayers) {

    const Trk::DiscBounds*  db = dynamic_cast<const Trk::DiscBounds*> (&lay->surfaceRepresentation().bounds()); 
    const Trk::CylinderBounds*  cyl = db ? 0 : dynamic_cast<const Trk::CylinderBounds*> (&lay->surfaceRepresentation().bounds()); 

    float matLayInput = 0.;
    float matLayWeight = 0.;

    //============ cylinder layers ===============
 
    if (cyl && m_mat4merge[icounter].size() ) {
      double layR = cyl->r();
     const Trk::LayerMaterialProperties* layMatProp=lay->layerMaterialProperties();
      if (layMatProp) {
	Amg::Vector3D gp(cyl->r(),0.,lay->surfaceRepresentation().center().z());
	const Trk::MaterialProperties* mp=layMatProp->fullMaterial(gp);
        if (mp)  DEBUG_TRACE(std::cout << "layer material present before merge, overwritten:"<<mp->thicknessInX0()<<std::endl;);
      }
      // check if heavy objects associated 
      bool hasheavy = false;
      for (auto mat :  m_mat4merge[icounter])  if (mat.mass()>heavyLimit) hasheavy=true; 
      std::vector<std::pair<float,Trk::MaterialProperties> > zmat;

      double layZmax = lay->surfaceRepresentation().center().z()+cyl->halflengthZ();
      zmat.push_back(std::pair<float,Trk::MaterialProperties>(lay->surfaceRepresentation().center().z()-cyl->halflengthZ(),Trk::MaterialProperties()));
      if (hasheavy) {
         double zdiff = 50.; double znext = zmat.back().first+zdiff;
         while (znext<layZmax-0.5*zdiff) {
	  zmat.push_back(std::pair<float,Trk::MaterialProperties>(znext,Trk::MaterialProperties())); 
	  znext = zmat.back().first+zdiff;  
	} 
      }
      zmat.push_back(std::pair<float,Trk::MaterialProperties>(layZmax,Trk::MaterialProperties()));
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator iz=zmat.begin();
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator iztmp=zmat.begin();
      // eta projection
      std::vector<std::pair<float,Trk::MaterialProperties> > etamat;
      etamat.push_back(std::pair<float,Trk::MaterialProperties>(eta(zmat.front().first,layR),Trk::MaterialProperties()));
      etamat.push_back(std::pair<float,Trk::MaterialProperties>(eta(zmat.back().first,layR),Trk::MaterialProperties()));
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator ieta=etamat.begin();
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator ietmp=etamat.begin();
 
      for (unsigned int im=0; im<m_mat4merge[icounter].size(); im++) {
        MaterialElement mat = m_mat4merge[icounter][im];
        matLayInput +=mat.mass()*mat.multi; 
        // project object dimensions
        double mzmin = mat.zmin*layR/(mat.zmin<0? mat.rmin : mat.rmax); 
	double mzmax = mat.zmax*layR/(mat.zmax<0?mat.rmax : mat.rmin);
        // contained part
        if ( mat.etamin>=etamat.back().first || mat.etamax<etamat.front().first)  continue; 
        if ( mzmin>=zmat.back().first || mzmax<zmat.front().first) continue; 
        double excess = fmax( fmin(zmat.front().first,mzmax) - mzmin , 0.)
	  +fmax( mzmax-fmax(zmat.back().first,mzmin), 0.);
        double exscale = fmin((mzmax-mzmin)/(mzmax-mzmin-excess),2.);    //  TODO : object with low overlap should be split or re-assigned
        // thickness of object calculated at the reference radius, corrected for longitudinal excess
	std::pair<double,std::pair<double,double> > thicknessEta = averageThickness(mat,true);
        double thickness = thicknessEta.first*exscale;
        //
	iz = zmat.begin(); 
         float matMassValid = 0.;
	while (iz!=(zmat.end()-1)) { 
          if ((*(iz+1)).first <= mzmin) iz++; 
	  else if ((*iz).first >= mzmax) break; // 
	  else{
	    if ((*iz).first<mzmin-tol && (*(iz+1)).first > mzmin+tol ) {     // resolve lower bin
	      iztmp=zmat.insert(iz+1,std::pair<float,Trk::MaterialProperties>(mzmin,(*iz).second));              
              iz = iztmp;
	    }	  			  
	    if ((*iz).first<mzmax-tol && (*(iz+1)).first > mzmax+tol) {  // resolve upper bin
	      iztmp=zmat.insert(iz+1,std::pair<float,Trk::MaterialProperties>(mzmax,(*iz).second));
	      iz=iztmp-1;
	    }
            double overlap = fmin(mzmax,(*(iz+1)).first)-fmax(mzmin,(*iz).first);
            double binsize = (*(iz+1)).first-(*iz).first;
            double zscale = binsize>0? overlap/binsize : 0.; 
            if (zscale>0.) {
	      double binLow = eta((*iz).first,layR);
	      double binUp = eta((*(iz+1)).first,layR);	      
	      double thAv = binThickness(binLow,binUp,thickness,
					   mat.etamin,thicknessEta.second.first,thicknessEta.second.second,mat.etamax);
	      (*iz).second.addMaterial(mat.matProp,thAv*zscale*mat.multi/mat.matProp.x0());
 	      matMassValid +=zscale*mat.matProp.rho*binsize*2*acos(-1.)*layR*thAv;
	    }
	    iz++; 
	  }
	}
      }
      if (flog) fprintf(flog,"lay cyl R zMin zMax: %e %e %e \n",layR,zmat.front().first,zmat.back().first); 
      if (flog) {int nbins=zmat.size(); fprintf(flog,"nbins %d \n",nbins); }
      std::vector<float> steps; std::vector<const Trk::MaterialProperties*> matMerged;
      iz = zmat.begin();
      while (iz!=(zmat.end()-1)) {
        fprintf(flog," %e %e \n", (*iz).first, (*iz).second.thicknessInX0()); 
        steps.push_back((*iz).first); matMerged.push_back(new Trk::MaterialProperties((*iz).second));
        matLayWeight +=2*acos(-1.)*cyl->r()*((*(iz+1)).first-(*iz).first)
	  *(*iz).second.thicknessInX0()*(*iz).second.x0()*(*iz).second.averageRho();
	iz++;
      }
      steps.push_back((*iz).first);
      fprintf(flog," %e %e \n", (*iz).first, (*iz).second.thicknessInX0()); 
      fprintf(flog,"   \n"); 
      Trk::BinUtility buZ(steps,Trk::open,Trk::binZ);      
      Trk::BinnedLayerMaterial material(buZ,matMerged);
      lay->assignMaterialProperties(material);
      if( cyl->r()>m_rBP) layMatTot += matLayInput;
    }

    //=========== disk layers =============

    if (db && m_mat4merge[icounter].size() ) {

      // check if heavy objects associated 
      bool hasheavy = false;
      for (auto mat :  m_mat4merge[icounter])  if (mat.mass()>heavyLimit) hasheavy=true; 
   
      std::vector<std::pair<float,Trk::MaterialProperties> > rmat;
      rmat.push_back(std::pair<float,Trk::MaterialProperties>(fmax(db->rMin(),m_rBP),Trk::MaterialProperties()));   // do not accept disks within the beam pipe volume     
      if (hasheavy) {
        double rdiff = 20.; double rnext = rmat.back().first+rdiff;
        while (rnext<db->rMax()-0.5*rdiff) {
	  rmat.push_back(std::pair<float,Trk::MaterialProperties>(rnext,Trk::MaterialProperties())); 
	  rnext = rmat.back().first+rdiff;  
	} 
     }
      rmat.push_back(std::pair<float,Trk::MaterialProperties>(db->rMax(),Trk::MaterialProperties()));
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator ir=rmat.begin();
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator irtmp=rmat.begin();
      double zLay = lay->surfaceRepresentation().center().z();
 
      for (unsigned int im=0; im<m_mat4merge[icounter].size(); im++) {
        MaterialElement mat = m_mat4merge[icounter][im];
        matLayInput +=mat.mass()*mat.multi;
        double zProjn = zLay*mat.zmin >0 ? zLay/mat.zmin : 1.;
        double zProjx = zLay*mat.zmax >0 ? zLay/mat.zmax : 1.;
        // project object dimensions
        double mrmin = mat.rmin*(mat.zmin<0? zProjn : zProjx);
        double mrmax = mat.rmax*(mat.zmax<0?zProjx : zProjn);
        // contained part
        if ( mat.etamin>= eta(zLay,zLay>0? rmat.front().first : rmat.back().first)  || mat.etamax<eta(zLay,zLay>0? rmat.back().first : rmat.front().first)) {
         continue; 
	}
        if ( mrmin>=rmat.back().first || mrmax<rmat.front().first)  continue; 
	
        double excess=0;
        if (mrmin<rmat.front().first) { 
	  double rup=fmin(rmat.front().first,mrmax);
          excess +=(rup*rup -mrmin*mrmin);
	} 
        if (mrmax>rmat.back().first) {
	  double rlow=fmax(rmat.back().first,mrmin);
          excess +=(mrmax*mrmax - rlow*rlow);
	}
        double rSf=mrmax*mrmax-mrmin*mrmin;
        double rscale = fmin(rSf/(rSf-excess),2.);  // TODO: objects with small overlap should be split or re-assigned
        // thickness of object calculated at the reference radius, corrected for longitudinal excess
	std::pair<double,std::pair<double,double> > thicknessEta = averageThickness(mat,false);
       double thickness = thicknessEta.first*rscale;

        ir = rmat.begin(); 
        float matMassValid = 0.;
	while (ir!=(rmat.end()-1)) { 
          if ((*(ir+1)).first < mrmin) ir++; 
	  else if ((*ir).first >= mrmax) break;  // TODO dr>0 should be added
          else {
	    if ((*ir).first<mrmin && (*(ir+1)).first > mrmin ) {     // resolve lower bin
              if (mrmin-(*ir).first>tol && (*(ir+1)).first-mrmin>tol)
		ir=rmat.insert(ir+1,std::pair<float,Trk::MaterialProperties>(mrmin,(*ir).second));
	    }	  			
	    if ((*ir).first<mrmax && (*(ir+1)).first > mrmax) {  // resolve upper bin
              if (mrmax-(*ir).first>tol && (*(ir+1)).first-mrmax>tol) {
		irtmp=rmat.insert(ir+1,std::pair<float,Trk::MaterialProperties>(mrmax,(*ir).second));
		ir=irtmp-1;
	      }
	    }
            double overlap = pow(fmin(mrmax,(*(ir+1)).first),2)-pow(fmax(mrmin,(*ir).first),2);
            double binsize = ((*(ir+1)).first-(*ir).first)*((*(ir+1)).first+(*ir).first) ;
           double zscale = binsize>0? overlap/binsize : 0.; 
            if (zscale>0.) {
               double binLow = fmin(eta(zLay,(*ir).first),eta(zLay,(*(ir+1)).first));
	       double binUp = fmax(eta(zLay,(*ir).first),eta(zLay,(*(ir+1)).first));	      
	       double thAv = binThickness(binLow,binUp,thickness,
					  mat.etamin,thicknessEta.second.first,thicknessEta.second.second,mat.etamax);
	       (*ir).second.addMaterial(mat.matProp,thAv*zscale*mat.multi/mat.matProp.X0); 
	       matMassValid += acos(-1.)*binsize*zscale*thAv*mat.matProp.rho;
	    }
	    ir++; 
	  }
	}
      }
      if (flog) fprintf(flog,"lay disc zpos rMin rMax: %e %e %e \n",zLay,rmat.front().first,rmat.back().first); 
      if (flog) {int nbins=rmat.size(); fprintf(flog,"nbins %d \n",nbins); }
      std::vector<float> steps; std::vector<const Trk::MaterialProperties*> matMerged;
      ir = rmat.begin();
      while (ir!=(rmat.end()-1)) {
	float sbin = (*ir).first; float dInXbin = (*ir).second.thicknessInX0();
	fprintf(flog," %e %e \n", sbin, dInXbin); 
        steps.push_back((*ir).first); matMerged.push_back(new Trk::MaterialProperties((*ir).second));
        matLayWeight += acos(-1.)*(pow((*(ir+1)).first,2)-pow((*ir).first,2))
	  *(*ir).second.thicknessInX0()*(*ir).second.x0()*(*ir).second.averageRho();
	ir++;
      }
      steps.push_back((*ir).first);
      sbin = (*ir).first; dInXbin = (*ir).second.thicknessInX0();
      fprintf(flog," %e %e \n", sbin, dInXbin); 
      fprintf(flog," \n"); 
      Trk::BinUtility buR(steps,Trk::open,Trk::binR);      
      Trk::BinnedLayerMaterial material(buR,matMerged);
      lay->assignMaterialProperties(material);
       layMatTot += matLayInput;
    }
    
    icounter++;
  }

  fclose(flog);
}


double Trk::MaterialLayerHelper::eta(double z, double R) const 
{
  if (z==0)  return 0;
  double th = atan(R/z);
  if (z<0.) th+=acos(-1.);
  double eta = -log(tan(th/2.)); 

  return eta;
}

double Trk::MaterialLayerHelper::etaToZ(double eta, double R) const 
{
  double th = 2*atan(exp(-eta));
  double z = R / tan(th);
  return z;
}

double Trk::MaterialLayerHelper::etaToR(double eta, double z) const 
{
  double th = 2*atan(exp(-eta));
  double R = z * tan(th);
  return R;
}

// best thickness approximation (input in eta)
double Trk::MaterialLayerHelper::binThickness( double binLow, double binUp, double thick, double e0, double e1, double e2, double e3) const 
{
 if (binLow>=e3 || binUp<=e0) return 0.;
  double xtot = 0.; double xlow = 0.; double xup = 0.;  
  // collect increasing part 
  if (binLow<e1) {
    if (e1>e0) {
      xlow = thick*(fmax(binLow,e0)-e0) /(e1-e0);
      xup = (binUp<e1) ?  thick*(binUp-e0) /(e1-e0) : thick;
      double de = fmin(binUp,e1) -  fmax(binLow,e0);
      xtot +=  de*0.5*(xlow+xup);
    }
  } 
  // collect flat part
  if (binUp>e1 && binLow<e2) {  
    double de = fmin(binUp,e2) -fmax(binLow,e1);
    xtot += de*thick;
 }
 // collect decreasing part 
  if (binUp>e2) {
    if (e3>e2) {
      xlow = thick*(e3-fmax(binLow,e2)) /(e3-e2);
      xup =  thick*(e3-fmin(binUp,e3))/(e3-e2);
      double de = fmin(binUp,e3) -  fmax(binLow,e2);
      xtot +=  de*0.5*(xlow+xup);
    }
  } ;
  return xtot/(fmin(binUp,e3)-fmax(binLow,e0));
}

std::pair<double,std::pair<double,double> > Trk::MaterialLayerHelper::averageThickness(MaterialElement mat,bool cyl) const 
{
  double e0 = mat.etamin;
  double e1 = fmin( eta(mat.zmin,mat.zmin<0? mat.rmax : mat.rmin), eta(mat.zmax,mat.zmin<0? mat.rmin : mat.rmax));
  double e2 = fmax( eta(mat.zmin,mat.zmin<0? mat.rmax : mat.rmin), eta(mat.zmax,mat.zmin<0? mat.rmin : mat.rmax));
  double e3 = mat.etamax;
  // thickness in inner points
  double th1 = 2*atan(exp(-e1));
  double th2 = 2*atan(exp(-e2));
  // true thickness
  double d1 = fmin( (mat.rmax-mat.rmin)/sin(th1) , (mat.zmax-mat.zmin)/fabs(cos(th1)));
  double d2 = fmin( (mat.rmax-mat.rmin)/sin(th2) , (mat.zmax-mat.zmin)/fabs(cos(th2)));
  // corrected for impact angle
  double d1c = cyl? d1*sin(th1) : d1*fabs(cos(th1));
  double d2c = cyl? d2*sin(th2) : d2*fabs(cos(th2));
  // average corrected for phi dilution
  double dphi = mat.phimax-mat.phimin;
  if (dphi<0 || dphi>2*acos(-1.)) dphi = 2*acos(-1.);
  double dav = 0.5*dphi/2/acos(-1.)/(e3-e0)*(     (e1<=e2? d1c*(e1-e0) : d2c*(e2-e0))
					      +(d1c+d2c)*fabs(e2-e1) + (e1<=e2 ? d2c*(e3-e2) : d1c*(e3-e1))    );

  std::pair<double,double> etamid(fmin(e1,e2),fmax(e1,e2));
  return std::pair<double,std::pair<double,double> > (dav,etamid); 

}


void Trk::MaterialLayerHelper::materialBudget(std::vector<MaterialElement>& inputMat) const 
{

  // extract summary information about material in GeoModel
  // using geometrical association while developing a consistent naming scheme
  float radiusInOut = 145. ; 
  float radiusPixSCT = 342.;
  float zlowOuterEC =  1100.;
  int im=0;
  float sct = 0.;
  float bp = 0.;
  float siInner = 0;  
  //  float siOuter = 0;  
  float siOB = 0.;
  float siEC = 0.;
  float hybridInner = 0;  
  float hybridOB = 0.;
  float hybridEC = 0.;
  float chipInner = 0;  
  float chipOB = 0.;
  float chipEC = 0.;
  float modules = 0.;
  float ladders = 0.;
  float svcInner = 0;  
  float svcOB = 0.;
  float svcEC = 0.;
  float structInner = 0.;
  float structOB = 0.;
  float structEC = 0.;
  //  float ipt = 0.;
  float ist = 0.;
  float pst = 0.;
  float interlink = 0.;
  float foam = 0.;
  float longeron = 0.;
  float shell = 0.;
  std::vector<float> shellLayer=std::vector<float>(20,0.);
  int nModOB = 0; int layIndex=-1; size_t labelLay;
  float materialAll = 0.;
  for ( auto mat : inputMat ) {
    materialAll +=mat.mass();
    float rad =  0.5*(mat.rmin+mat.rmax);
    labelLay = mat.name.find("_L");
    if (labelLay!=std::string::npos) {
      if ( sscanf(mat.name.substr(labelLay+2,1).c_str(),"%d",&layIndex)!=1 ) layIndex=-1;
    }   
    labelLay = mat.name.find("_L1");
    if (labelLay!=std::string::npos) {
      if ( sscanf(mat.name.substr(labelLay+2,2).c_str(),"%d",&layIndex)!=1 ) layIndex=1;
    }   
    if (mat.rmin > radiusPixSCT) sct += mat.mass();
    else if (mat.name.find("siLog")!=std::string::npos) {
      if (rad < radiusInOut) siInner += mat.mass();
      else if ( mat.zmin<-zlowOuterEC || mat.zmax>zlowOuterEC) siEC += mat.mass();
      else siOB += mat.mass(); 
    } else if (mat.name.find("Hybrid")!=std::string::npos) {
      if (rad < radiusInOut) hybridInner += mat.mass();
      else if ( mat.zmin<-zlowOuterEC || mat.zmax>zlowOuterEC) hybridEC += mat.mass();
      else hybridOB += mat.mass();
    } else if (mat.name.find("Chip")!=std::string::npos) {
      if (rad < radiusInOut) chipInner += mat.mass();
      else if ( mat.zmin<-zlowOuterEC || mat.zmax>zlowOuterEC) chipEC += mat.mass();
      else chipOB += mat.mass(); 
    } else if (mat.name=="Module") { modules += mat.mass();
      if (rad>radiusInOut && mat.zmin>-zlowOuterEC && mat.zmax<1100) nModOB++; 
    } else if (mat.name=="Ladder") { ladders += mat.mass(); 
    } else if (mat.name=="Pigtail") { svcInner += mat.mass(); 
    } else if (mat.name=="FoamSupport") { structOB += mat.mass(); foam += mat.mass();
    } else if (mat.name.find("Section")!=std::string::npos) {  bp += mat.mass();
    } else if (mat.name.find("StaveSupport")!=std::string::npos) { structInner += mat.mass();
    } else if (mat.name.find("IST")!=std::string::npos) { ist += mat.mass();
    } else if (mat.name.find("PST")!=std::string::npos) { pst += mat.mass();
    } else if (mat.name.find("QuarterShell")!=std::string::npos) { structInner += mat.mass();
    } else if (mat.name.find("Interlink")!=std::string::npos) { interlink += mat.mass();
    } else if (mat.name.find("Longeron")!=std::string::npos) { structOB += mat.mass(); longeron += mat.mass();
    } else if (mat.name=="ModuleSvc") {
      if (rad < radiusInOut) svcInner += mat.mass();
      else if ( mat.zmin<-zlowOuterEC || mat.zmax>zlowOuterEC) svcEC += mat.mass();
      else svcOB += mat.mass(); 
    } else if (mat.name.find("shellWallLog")!=std::string::npos) {
      if (rad < radiusInOut) structInner += mat.mass();
      else if ( mat.zmin<-zlowOuterEC || mat.zmax>zlowOuterEC) structEC += mat.mass();
      else { structOB += mat.mass(); shell += mat.mass(); }
      if (layIndex>=0) shellLayer[layIndex] += mat.mass();  
    } else if (mat.name.find("shellBotLog")!=std::string::npos) {
      if (rad < radiusInOut) structInner += mat.mass();
      else if ( mat.zmin<-zlowOuterEC || mat.zmax>zlowOuterEC) structEC += mat.mass();
      else { structOB += mat.mass(); shell += mat.mass();}  
      if (layIndex>=0) shellLayer[layIndex] += mat.mass(); 
    } else if (mat.name.find("shellSupLog")!=std::string::npos) {
      if (rad < radiusInOut) structInner += mat.mass();
      else if ( mat.zmin<-zlowOuterEC || mat.zmax>zlowOuterEC) structEC += mat.mass();
      else { structOB += mat.mass(); shell += mat.mass();} 
      if (layIndex>=0) shellLayer[layIndex] += mat.mass(); 
    } else if (mat.name.find("supLog")!=std::string::npos) {
      if (rad<radiusInOut) structInner += mat.mass();
      else if ( mat.zmin<-zlowOuterEC || mat.zmax>zlowOuterEC) structEC += mat.mass();
      else structOB += mat.mass(); 
    } else if (mat.name.find("ringLog")!=std::string::npos) {
      if (rad<radiusInOut) structInner += mat.mass();
      else if ( mat.zmin<-zlowOuterEC || mat.zmax>zlowOuterEC) structEC += mat.mass();
      else structOB += mat.mass(); 
    } else if (mat.name.find("SvcEc")!=std::string::npos) {
      if (rad<radiusInOut) svcInner += mat.mass();
      else svcEC += mat.mass();
    } else if (mat.name.find("SvcBrl")!=std::string::npos) {
      if (rad<radiusInOut) svcInner += mat.mass();
      else svcOB += mat.mass();
    } else if (mat.name.find("Svc")!=std::string::npos) {
      if (rad<radiusInOut) svcInner += mat.mass();
      else svcOB += mat.mass();
    } else if (mat.name.find("Wall")!=std::string::npos) {  structOB += mat.mass(); longeron += mat.mass();
    } else if (mat.name.find("Corner")!=std::string::npos) {  structOB += mat.mass(); longeron += mat.mass(); 
    }
    im++;

  }

  DEBUG_TRACE(std::cout << "GM material budget:"<< materialAll<< std::endl;);
  DEBUG_TRACE(std::cout << "SCT total:"<< sct<< std::endl;);
  DEBUG_TRACE(std::cout << "interlinks:"<< interlink<< std::endl;);
  
  DEBUG_TRACE(std::cout << "modules inner:Si:hybrid:chip:"<<siInner<<":"<<hybridInner<<":"<<chipInner<<":total:"<<siInner+hybridInner+chipInner<< std::endl;);
  DEBUG_TRACE(std::cout << "modules OB:Si:hybrid:chip:"<<siOB<<":"<<hybridOB<<":"<<chipOB<<":total:"<<siOB+hybridOB+chipOB<< std::endl;);
  DEBUG_TRACE(std::cout << "modules EC:Si:hybrid:chip:"<<siEC<<":"<<hybridEC<<":"<<chipEC<<":total:"<<siEC+hybridEC+chipEC<< std::endl;);

  DEBUG_TRACE(std::cout <<"( x-check total modules ... should be empty)"<< modules<< std::endl;);
  DEBUG_TRACE(std::cout <<"( x-check total ladders ... should be empty)"<< ladders<< std::endl;);

  DEBUG_TRACE(std::cout << "services inner : OB : EC (position):"<<svcInner<<":"<<svcOB<<":"<<svcEC<<":total:"<<svcInner+svcOB+svcEC<< std::endl;);
  DEBUG_TRACE(std::cout << "structure inner : OB : EC (position):"<<structInner<<":"<<structOB<<":"<<structEC<<":total:"<<structInner+structOB+structEC<< std::endl;);

  DEBUG_TRACE(std::cout << " OB substructure: longeron: shell:"<< longeron<<":"<<shell<< std::endl; );

  DEBUG_TRACE(std::cout << "number of OB modules:"<< nModOB << std::endl; ); 

  DEBUG_TRACE(std::cout <<"IPT:IST:PST"<<ipt<<":"<<ist<<":"<<pst<< std::endl;);
  for (unsigned int is=0; is<shellLayer.size(); is++) if (shellLayer[is]>0.) DEBUG_TRACE(std::cout << "shell layer material:"<< is<<":"<<shellLayer[is]<< std::endl;);
}

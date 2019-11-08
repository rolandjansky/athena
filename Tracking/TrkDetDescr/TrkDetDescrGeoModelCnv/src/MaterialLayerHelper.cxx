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
//#include "TrkGeometry/LayerMaterialBinned.h"
#include "TrkGeometry/HomogeneousLayerMaterial.h"
#include "TrkVolumes/CylinderVolumeBounds.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkDetDescrUtils/BinnedArray1D.h"
#include "TrkDetDescrUtils/BinnedArray1D1D.h"
#include "TrkDetDescrUtils/BinningData.h"
#include "TrkDetDescrUtils/BinUtility.h"
// Amg
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

namespace Trk{
  typedef std::pair< SharedObject<const Surface>, Amg::Vector3D > SurfaceOrderPosition;
}

void Trk::MaterialLayerHelper::processMaterial(const Trk::TrackingVolume* inputVol, std::vector<MaterialElement>& inputMat, bool printInfo)
{
  // loop over input volume and collect material layers

  std::vector< const Trk::Layer* >  matLayers;
  
  getMaterialLayers( inputVol, matLayers ); 

  //std::cout << "Number of material layers in the tracking volume:"<< inputVol->volumeName()<<":"<<matLayers.size()<< std::endl; 

  const Trk::CylinderVolumeBounds* cylBounds = dynamic_cast<const Trk::CylinderVolumeBounds*> (&(inputVol->volumeBounds()));

  m_rExt = cylBounds ? cylBounds->outerRadius() : 1148.;
  m_zExt = cylBounds ? cylBounds->halflengthZ() : 3450.;

  // prepare the material collection for merge
  m_mat4merge.clear();
  for (auto l : matLayers) m_mat4merge.push_back(std::vector< MaterialElement>());

  findClosestLayer( inputMat, matLayers);

  buildMaterial( matLayers );
  //buildMaterialInEtaProjection( matLayers );

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
      //std::cout <<"confined layer in "<<inputVol->volumeName()<<":has material layer? "<< lay->layerMaterialProperties()<< std::endl;
    }
  }
  
  // Process the boundary surface layers 
  auto& bSurfaces = inputVol->boundarySurfaces();
  for (auto& bsIter : bSurfaces ) 
    if (bsIter->surfaceRepresentation().materialLayer()) {
      //std::cout<<"material boundary found in volume:"<<inputVol->volumeName()<<std::endl; 
      // boundaries shared : check if not added already
      bool notCounted = true;
      for (auto mlay : matLayers) if (bsIter->surfaceRepresentation().materialLayer() == mlay ) { notCounted=false; break;}
      if (notCounted) matLayers.push_back(bsIter->surfaceRepresentation().materialLayer());
      //if (notCounted) std::cout<<"material boundary found in:"<<inputVol->volumeName()<<std::endl; 
    }
}


void Trk::MaterialLayerHelper::findClosestLayer(std::vector<MaterialElement>& matInput, std::vector< const Trk::Layer* >&  layers) {

  // convert into MaterialObject & assign to closest layer
  const Trk::GeoMaterialConverter matCnvTool;
  
  double massTot = 0.;
  double massInp = 0.;
  m_unassignedMass=0.;
  m_unassignedITk =0.;

  std::vector<std::pair<double,double> > layThRange;
  std::vector<std::pair<const Trk::CylinderBounds*,const Trk::DiscBounds*> > layBounds;
  for (unsigned int i=0; i<layers.size(); i++) {
    const Trk::Layer* lay = layers[i];
    const Trk::DiscBounds*  db = dynamic_cast<const Trk::DiscBounds*> (&lay->surfaceRepresentation().bounds()); 
    const Trk::CylinderBounds*  cyl = dynamic_cast<const Trk::CylinderBounds*> (&lay->surfaceRepresentation().bounds()); 
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

    //if (cyl)  std::cout <<"layerInfo:"<<i<<":cyl:"<<zLay<<"("<<zLay-cyl->halflengthZ()<<","<<zLay+cyl->halflengthZ()<<"),eta range:"
    //			<<-log(tan(0.5*layThRange.back().first))<<","<<-log(tan(0.5*layThRange.back().second))<<std::endl;
    //else  std::cout <<"layerInfo:"<<i<<":disk:"<<zLay<<"("<<db->rMin()<<","<<db->rMax()<<"),eta range:"
    //		    <<-log(tan(0.5*layThRange.back().first))<<","<<-log(tan(0.5*layThRange.back().second))<<std::endl;
  }

  const Trk::Layer* assocLayer=0;
  int index=-1; 
  float dist = 5.e5;
  float matOK = 0;
  std::vector< std::pair<int, std::pair<float,float> > > closeLays;
 
  for ( auto mat : matInput ) {
    
    // ignore objects beyond the volume boundaries !TODO extend to deal with volumes shifted in z!
    if (mat.rmin>=m_rExt) continue; 
    if (mat.zmin>=m_zExt) continue;  
    if (mat.zmax<=-m_zExt) continue;  

    // remove material beyond volume boundaries
    if (mat.rmax>m_rExt)  mat.rmax=m_rExt;
    if (mat.zmax>m_zExt)  mat.zmax=m_zExt;
    if (mat.zmin<-m_zExt) mat.zmin=-m_zExt;

    double dr = mat.rmax-mat.rmin;
    double dz = mat.zmax-mat.zmin;

    if (mat.volume()<=0.) {
      m_unassignedMass += mat.mass();
      continue; 
    }    

    //std::cout<<"mat.elm:"<<mat.name<<":"<<mat.mass()<<":"<<mat.geoVolume<<":"<<mat.volume()<<std::endl;   
    Trk::Material  material = matCnvTool.convert(mat.material);
    double scale = 1.;
    if (mat.geoVolume>0.) scale = mat.geoVolume/mat.volume();
    mat.setMaterial(Trk::Material( material.X0/scale, material.L0/scale, scale*material.A, scale*material.Z, scale*material.rho));
    //if ( fabs(mat.mass()-mat.volume()*mat.matProp.rho)>0.001*mat.mass()) {
    //  std::cout<<"mat.elm conv:"<<mat.name<<":"<<mat.mass()<<":"<<mat.volume()*mat.matProp.rho<<std::endl;   
    //}  
 
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
	
	if (db && mat.rmin<30 ) continue;    // TODO : check if needed
	if (cyl && cyl->r()<32.2 && mat.rmax > 30.) continue;  //TODO check if needed

	zpos = lay->surfaceRepresentation().center().z(); 
	overlap =  (fmin(mat.thmax,layThRange[i].second) -fmax(mat.thmin,layThRange[i].first))/(mat.thmax-mat.thmin);
	if (cyl) d=fabs(cyl->r()-rRef);
	else if (db) d=fabs(zpos-zRef);

	if (d<dist) {dist=d; assocLayer = lay; index = i; thOverlap = overlap; }
	if (overlap>0 && d<500.) { // order in overlap
	  if (!closeLays.size()) closeLays.push_back(std::pair<int,std::pair<float,float> > (i,std::pair<float,float>(d,overlap)));
          else {
	    std::vector< std::pair<int, std::pair<float,float> > >::iterator clIt = closeLays.begin();
	    while (clIt!=closeLays.end() && (*clIt).second.second > overlap ) clIt++;   	
	    while (clIt!=closeLays.end() && (*clIt).second.second == overlap && (*clIt).second.first < d ) clIt++;   	
	    closeLays.insert(clIt,std::pair<int,std::pair<float,float> > (i,std::pair<float,float>(d,overlap)));
	  }
	} 
      }
      
      if (dist==0 && assocLayer && overlap==1.) break; 
    }

    if ( thOverlap < 0.9) {
      //std::cout<<"mat.elm:"<<mat.name<<":"<<mat.mass()<<":"<<mat.rmin<<","<<mat.rmax<<":"<<mat.zmin<<","<<mat.zmax<<std::endl;
      //std::cout <<"low overlap:"<<thOverlap<< std::endl;
      for (auto cl : closeLays) {
	//std::cout << "compatible layers found :"<<cl.second.first<<":"<<cl.second.second<< std::endl;
        if (cl.second.first<200 && cl.second.second>thOverlap) {
          dist=cl.second.first; thOverlap =cl.second.second; index=cl.first; assocLayer=layers[cl.first];
          if (thOverlap>0.9) break;
	} 
      }
      //std::cout <<"association resolved to :"<<index<<":"<<dist<<":"<<thOverlap<<std::endl;
    }

    if (assocLayer) {
      // resolve projections

      //if (thOverlap< 0.9 ) std::cout <<"candidate for split object:"<<mat.name<<":"<<mat.mass()<<":thOverlap:"<<thOverlap<<":eta range:"<<mat.etamin<<":"<<mat.etamax<< std::endl;

      // if (thOverlap<0.9 && mat.mass()>20.) {
      //	if (mat.thmin< layThRange[index].first) {
      //    double zn = zRef? rRef/tan(layThRange[index].first) :           
      //	}
      //}

      if (m_mat4merge[index].size() && mat.name ==  m_mat4merge[index].back().name) { 
        bool mmatch =  fabs(mat.mass()- m_mat4merge[index].back().mass())<0.01*mat.mass();
        float dzm= m_mat4merge[index].back().zmax-m_mat4merge[index].back().zmin;
        float dzn= mat.zmax-mat.zmin;
        float drm= m_mat4merge[index].back().rmax-m_mat4merge[index].back().rmin;
        float drn= mat.rmax-mat.rmin;
        bool zmatch = fabs(mat.zmin- m_mat4merge[index].back().zmin)<10.
	  &&  fabs(mat.zmax- m_mat4merge[index].back().zmax)<10.;
        bool rmatch = fabs(mat.rmin- m_mat4merge[index].back().rmin)<10.
	  &&  fabs(mat.rmax- m_mat4merge[index].back().rmax)<10.;
        if ( mmatch && zmatch && rmatch ) {
	  m_mat4merge[index].back().multi =  m_mat4merge[index].back().multi+1;
	  //std::cout<<"merged objects:"<< mat.name<<":"<< m_mat4merge[index].back().multi<<std::endl;         
	} else {

          // split object if it significantly exceeds the layer dimension
	  //if (layBounds[index].first ) {
          //  double cylR = layBounds[index].first->r();
          //  double lemin =  
 	  /*
	  float zn =  fmin(mat.zmin, m_mat4merge[index].back().zmin);
          float zx =  fmax(mat.zmax, m_mat4merge[index].back().zmax);
          float rn =  fmin(mat.rmin, m_mat4merge[index].back().rmin);
          float rx =  fmax(mat.rmax, m_mat4merge[index].back().rmax);
          bool zmerge = zx-zn < dzm+dzn+20.;
          bool rmerge = rx-rn < drm+drn+20.;

	  if (layBounds[index].first && rmatch && zmerge) {
            double cylR = layBounds[index].first->r(); 
            double vol = mat.volume()+m_mat4merge[index].back().volume();            
            m_mat4merge[index].back().zmin = zn;  m_mat4merge[index].back().zmax = zx;
            double thick = 0.5*vol/acos(-1.)/cylR/(zx-zn);
            m_mat4merge[index].back().rmin = cylR-0.5*thick;  m_mat4merge[index].back().rmax = cylR+0.5*thick;
	    Trk::MaterialProperties matProp(material,thick);
	    m_mat4merge[index].back().setMaterialProperties(matProp);   
	  } else if (layBounds[index].second && zmatch && rmerge) {
            //double diskRmin = layBounds[index].second->rMin(); 
            //double diskRmax = layBounds[index].second->rMax(); 
            double vol = mat.volume()+m_mat4merge[index].back().volume();             
            m_mat4merge[index].back().rmin = rn;  m_mat4merge[index].back().rmax = rx;
            double thick = vol/acos(-1.)/(rx*rx-rn*rn); double zdisk = 0.5*(zn+zx);
            m_mat4merge[index].back().zmin = zdisk-0.5*thick;  m_mat4merge[index].back().rmax = zdisk+0.5*thick;
	    Trk::MaterialProperties matProp(material,thick);
	    m_mat4merge[index].back().setMaterialProperties(matProp);   
	  } else {
	    std::cout <<mat.name<<":lay:"<<index<<":m/r/z/match:"<< mmatch<<":"<<rmatch<<":"<<zmatch<<":zmerge:"<<zmerge<<";rmerge:"<<rmerge<<std::endl;
	  */
	  m_mat4merge[index].push_back(mat);	  
	}
      } else  m_mat4merge[index].push_back(mat);
	
   } else {
      std::cout <<"no suitable layer found for material element:"<<mat.rmin<<","<<mat.rmax<<":"<<mat.zmin<<","<<mat.zmax<< std::endl;
      
      //double dphi = mat.phimax-mat.phimin;
      //if (dphi<0 || dphi>2*acos(-1.)) dphi=2*acos(-1.);
      //double mass= 0.5*dphi*(mat.zmax-mat.zmin)*(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.matProp.rho;
      m_unassignedMass += mat.mass();
      if (mat.rmin>30. && mat.zmax<3500. && mat.zmin>-3500) m_unassignedITk += mat.mass();
    }
    
    double dphi = mat.phimax-mat.phimin;
    if (dphi<0 || dphi>2*acos(-1.)) dphi=2*acos(-1.);
    // sanity check
    double massObj= 0.5*dphi*(mat.zmax-mat.zmin)*(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.matProp.rho;    
    massTot+=massObj;
    massInp+=mat.mass();
  }
  //std::cout<<"total mass:"<< massTot<<":"<<massInp<<std::endl;
  //std::cout<<"unassigned:"<< m_unassignedMass<<":"<<m_unassignedITk<< std::endl;
  //std::cout<<"contained:"<< matOK<< std::endl;

  // print out layer mass
  index=-1;  
  for ( auto matVector : m_mat4merge  ) {
    index++;
    float layerMass = 0.;
    for ( auto m : matVector) layerMass+=m.mass();

    const Trk::DiscBounds*  db = dynamic_cast<const Trk::DiscBounds*> (&(layers[index]->surfaceRepresentation().bounds())); 
    const Trk::CylinderBounds*  cyl = dynamic_cast<const Trk::CylinderBounds*> (&(layers[index]->surfaceRepresentation().bounds())); 

    float zpos =  layers[index]->surfaceRepresentation().center().z() ;
  
    //if (db) std::cout <<"disc:"<< db->rMin()<<","<< db->rMax() <<":z:"<< zpos <<":mass:"<<layerMass <<std::endl;
    //if (cyl) std::cout << "cyl:"<<cyl->r()<<":"<< zpos-cyl->halflengthZ() <<","<< zpos+cyl->halflengthZ()<<":mass:"<<layerMass<< std::endl;

  }

}

void Trk::MaterialLayerHelper::closestLayer(MaterialElement mat ,std::vector< const Trk::Layer* >&  layers ) {

  const Trk::Layer* assocLayer=0;
  float dist = 500000.;
  int index=-1; 

  for (unsigned int i=0; i<layers.size(); i++) {
    const Trk::Layer* lay = layers[i];

    const Trk::DiscBounds*  db = dynamic_cast<const Trk::DiscBounds*> (&lay->surfaceRepresentation().bounds()); 
    const Trk::CylinderBounds*  cyl = dynamic_cast<const Trk::CylinderBounds*> (&lay->surfaceRepresentation().bounds()); 

    float zpos = lay->surfaceRepresentation().center().z(); 

    if (db && mat.rmin<30 ) continue;    // TODO : check if needed
    if (cyl && cyl->r()<30. && mat.rmax > 30.) continue;  //TODO check if needed
  
    if (cyl) {
      double zLayMin = zpos-cyl->halflengthZ(); double zLayMax =  zpos+cyl->halflengthZ();
      double layThMin = zLayMin==0 ? acos(-1.)/2. : atan(cyl->r()/zLayMin); if (zLayMin<0) layThMin+=acos(-1.);
      double layThMax = zLayMax==0 ? acos(-1.)/2. : atan(cyl->r()/zLayMax); if (zLayMax<0) layThMax+=acos(-1.);
      //if (layThMin>layThMax) std::cout << "layThMin > layThMax!!"<< std::endl;
      //if ( mat.zmin< zpos+cyl->halflengthZ() && mat.zmax > zpos-cyl->halflengthZ() ) {
      if ( mat.thmin< layThMax && mat.thmax > layThMin ) {
	float d=fabs(cyl->r()-0.5*(mat.rmin+mat.rmax));
	if (d<dist) {dist=d; assocLayer = lay; index = i; }
      }
    }

    else if (db) {
      double layThMin = zpos==0 ? acos(-1.)/2. : ( zpos>0 ? atan(db->rMax()/zpos) : atan(db->rMin()/zpos)+acos(-1.) );
      double layThMax = zpos==0 ? acos(-1.)/2. : ( zpos>0 ? atan(db->rMin()/zpos) : atan(db->rMax()/zpos)+acos(-1.) );
      //if (layThMin>layThMax) std::cout << "db:layThMin > layThMax!!"<< std::endl;
      //if ( mat.rmin < db->rMax() && mat.rmax > db->rMin() ) {
      if ( mat.thmin< layThMax && mat.thmax > layThMin ) {
	float d=fabs(zpos-0.5*(mat.zmin+mat.zmax));
        if (d==dist && fabs(zpos) < fabs( assocLayer->surfaceRepresentation().center().z()) ) { assocLayer = lay; index = i; }
	else if (d<dist) {dist=d; assocLayer = lay; index=i;}
      }
    }
    
    if (dist==0 && assocLayer) break; 
  }

  if (assocLayer) {

    m_mat4merge[index].push_back(mat);

    /*
    const Trk::DiscBounds*  db = dynamic_cast<const Trk::DiscBounds*> (&assocLayer->surfaceRepresentation().bounds()); 
    const Trk::CylinderBounds*  cyl = dynamic_cast<const Trk::CylinderBounds*> (&assocLayer->surfaceRepresentation().bounds()); 

    std::cout <<"material element:"<<mat.rmin<<","<<mat.rmax<<":"<<mat.zmin<<","<<mat.zmax<< std::endl;
    std::cout <<"associated with layer:";
    float zpos =  assocLayer->surfaceRepresentation().center().z() ;
    //{ 
      if (db) std::cout <<"disc:"<< db->rMin()<<","<< db->rMax() <<":"<< zpos <<std::endl;
      if (cyl) std::cout << "cyl:"<<cyl->r()<<":"<< zpos-cyl->halflengthZ() <<","<< zpos+cyl->halflengthZ()<< std::endl;
    //}
    std::cout <<"at distance:"<< dist<< ":material:"<< mat.materialName<<":"<<mat.layer<<std::endl;
    // project material object into MaterialProperties, collect material input for each layer
    */
  } else {
    std::cout <<"no suitable layer found for material element:"<<mat.rmin<<","<<mat.rmax<<":"<<mat.zmin<<","<<mat.zmax<< std::endl;
    double dphi = mat.phimax-mat.phimin;
    if (dphi<0 || dphi>2*acos(-1.)) dphi=2*acos(-1.);
    double mass= 0.5*dphi*(mat.zmax-mat.zmin)*(mat.rmax*mat.rmax-mat.rmin*mat.rmin)*mat.matProp.rho;
    m_unassignedMass += mass;
    if (mat.rmin>30. && mat.zmax<3500. && mat.zmin>-3500) m_unassignedITk += mass;
  }

}


void Trk::MaterialLayerHelper::buildMaterial(std::vector< const Trk::Layer* >&  matLayers)
{

  int icounter = 0;
  float tol = 10.;    // smallest array step

  static FILE* flog = 0;
  flog = fopen("layer_material.dump","w+");
  float sbin =0.; float dInXbin = 0.;

  float layMatTot=0.;

  for (auto lay : matLayers) {

    //std::cout <<"loop over material layers, number of associated objects:"<<icounter<<":"<<m_mat4merge[icounter].size()<<std::endl;

    const Trk::DiscBounds*  db = dynamic_cast<const Trk::DiscBounds*> (&lay->surfaceRepresentation().bounds()); 
    const Trk::CylinderBounds*  cyl = dynamic_cast<const Trk::CylinderBounds*> (&lay->surfaceRepresentation().bounds()); 

    float matLayInput = 0.;
    float matLayWeight = 0.;
 
    if (cyl && m_mat4merge[icounter].size() ) {
      double layR = cyl->r();
     const Trk::LayerMaterialProperties* layMatProp=lay->layerMaterialProperties();
      if (layMatProp) {
	Amg::Vector3D gp(cyl->r(),0.,lay->surfaceRepresentation().center().z());
	const Trk::MaterialProperties* mp=layMatProp->fullMaterial(gp);
        if (mp)  std::cout << "layer material present before merge, overwritten:"<<mp->thicknessInX0()<<std::endl;
      }
      std::vector<std::pair<float,Trk::MaterialProperties> > zmat;
      zmat.push_back(std::pair<float,Trk::MaterialProperties>(lay->surfaceRepresentation().center().z()-cyl->halflengthZ(),Trk::MaterialProperties()));
      zmat.push_back(std::pair<float,Trk::MaterialProperties>(lay->surfaceRepresentation().center().z()+cyl->halflengthZ(),Trk::MaterialProperties()));
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator iz=zmat.begin();
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator iztmp=zmat.begin();

      for (unsigned int im=0; im<m_mat4merge[icounter].size(); im++) {
        MaterialElement mat = m_mat4merge[icounter][im];
        matLayInput +=mat.mass()*mat.multi; 
        double dphi = mat.phimax-mat.phimin;
	float phi_dilution = dphi<2*acos( -1.) ? dphi/2/acos(-1.) : 1.;
	double dz=mat.zmax-mat.zmin; double dr=mat.rmax-mat.rmin;
	double dmax=sqrt(dz*dz+dr*dr*phi_dilution*phi_dilution);
        double rRef = 0.5*(mat.rmin+mat.rmax); 
        double zRef = 0.5*(mat.zmin+mat.zmax); 
        double rProj = layR/rRef;
        // project object dimensions
        double mzmin = mat.zmin*rProj; double mzmax = mat.zmax*rProj;
        // contained part
        if ( mzmin>=zmat.back().first || mzmax<zmat.front().first) {
	  std::cout << "skipping non-overlapping object:"<<mat.name<<":"<<mat.mass()<<"closest layer:"<< zmat.front().first<<":"<<zmat.back().first<<std::endl;
          continue; 
	}
        double excess = fmax( fmin(zmat.front().first,mzmax) - mzmin , 0.)
	  +fmax( mzmax-fmax(zmat.back().first,mzmin), 0.);
        double rscale = (mzmax-mzmin)/(mzmax-mzmin-excess);
	double thickness=0.5*mat.volume()/acos(-1.)/layR/(mzmax-mzmin)*rscale;    // correction of eta ragne excess
        //
	iz = zmat.begin(); 
        //float dz = (*iz).first > mat.zmin ? fmin(mat.zmax,(*iz).first)-mat.zmin : 0.;
	//float dz = 0.;
        float matMassValid = 0.;
	while (iz!=(zmat.end()-1)) { 
          if ((*(iz+1)).first <= mzmin) iz++; 
	  else if ((*iz).first >= mzmax) break; // 
	  else{
	    if ((*iz).first<mzmin-tol && (*(iz+1)).first > mzmin+tol ) {     // resolve lower bin
	      iztmp=zmat.insert(iz+1,std::pair<float,Trk::MaterialProperties>(mzmin,(*iz).second));              
              iz = iztmp;
	      //std::cout << "inserting zstep low:"<<(*(iz-1)).first<<":"<<(*iz).first<<":"<<(*(iz+1)).first<< std::endl;
	    }	  			  
	    if ((*iz).first<mzmax-tol && (*(iz+1)).first > mzmax+tol) {  // resolve upper bin
	      iztmp=zmat.insert(iz+1,std::pair<float,Trk::MaterialProperties>(mzmax,(*iz).second));
	      iz=iztmp-1;
	      //std::cout << "inserting zstep up:"<<(*(iz-1)).first<<":"<<(*iz).first<<":"<<(*(iz+1)).first<< std::endl;
	    }
            //if ((*iz).first<mzmin) dz+=(*iz).first-mzmin; 
            //if ((*(iz+1)).first>mzmax) dz+=mzmax-(*(iz+1)).first;
            //if (iz+2 == zmat.end() && mat.zmax>(*(iz+1)).first) dz+=mat.zmax-fmax((*(iz+1)).first,mat.zmin); 
            double overlap = fmin(mzmax,(*(iz+1)).first)-fmax(mzmin,(*iz).first);
            double binsize = (*(iz+1)).first-(*iz).first;
	    //std::cout <<"dz:binsize:"<<dz<<":"<<binsize<<std::endl;
            double zscale = binsize>0? overlap/binsize : 0.; 
           
            // for keeping d/x0 in the projection ( for weight conservation, apply another factor 1/rProj ) 
            if (zscale>0.) {
	      // matMassValid += overlap*2*acos(-1.)*layR*rscale/rProj*phi_dilution*(mat.rmax-mat.rmin)*mat.matProp.averageRho();
	      //double massOld = binsize*2*acos(-1.)*layR*(*iz).second.thicknessInX0()*(*iz).second.x0()*(*iz).second.averageRho();
              //double dz=mat.zmax-mat.zmin; double dr=mat.rmax-mat.rmin;
              //double th = atan(rRef/fabs(zRef));
              //double thickness = dz<dr ? fmin(dz/cos(th),dr)*sin(th) : dr;
              double zLoc =  0.5*((*(iz+1)).first+(*iz).first) ;
	      double thRef = zLoc!=0 ? atan(layR/fabs(zLoc)) : acos(-1.)/2.;
              double thickness_corr = rProj*rProj*thickness;     // correction to re-establish projection
              if ( thickness_corr > dmax*sin(thRef) ) thickness_corr=dmax*sin(thRef); 
	      (*iz).second.addMaterial(mat.matProp,thickness_corr*zscale*mat.multi/mat.matProp.x0());
 	      matMassValid +=zscale*mat.matProp.rho*binsize*2*acos(-1.)*layR*thickness;
	      //double massNew      = binsize*2*acos(-1.)*layR*(*iz).second.thicknessInX0()*(*iz).second.x0()*(*iz).second.averageRho();
	      //std::cout << "check merged material:"<< massNew-massOld<<":"<<matMassValid<<std::endl;
	    }
	    iz++; //dz=0.;
	  }
	}
        //if (fabs(mat.mass()-matMassValid)>0.1*mat.mass()) {
	//  std::cout << "wrong projection cyl:"<<mat.name<<":rscale:"<<rscale
	//							    <<":mat_true/mat_projected"<<mat.mass()/matMassValid<< std::endl;	   
	//  std::cout << "volume check:"<<mat.volume()<<":"<<2*acos(-1.)*layR*thickness*(mzmax-mzmin-excess)*phi_dilution<< std::endl; 
	//}
	//std::cout << "translated mass :"<<mat.mass()<<":"<<matMassValid<<":"<<rscale<<":"<<rProj<<";"<<phi_dilution<<std::endl;
      }
      //if (flog) fprintf(flog,"lay cyl R zMin zMax: %e %e %e \n",layR,zmat.front().first,zmat.back().first); 
      //if (flog) {int nbins=zmat.size(); fprintf(flog,"nbins %d \n",nbins); }
      std::vector<float> steps; std::vector<const Trk::MaterialProperties*> matMerged;
      iz = zmat.begin();
      while (iz!=(zmat.end()-1)) {
	//std::cout<<"material array in z:"<<(*iz).first<<":"<<(*iz).second.thicknessInX0()<<std::endl;
        //fprintf(flog," %e %e \n", (*iz).first, (*iz).second.thicknessInX0()); 
	//std::cout<<"checking:"<<(*iz).second.x0()<<":"<<(*iz).second.averageRho()<<std::endl;
        steps.push_back((*iz).first); matMerged.push_back(new Trk::MaterialProperties((*iz).second));
        matLayWeight +=2*acos(-1.)*cyl->r()*((*(iz+1)).first-(*iz).first)
	  *(*iz).second.thicknessInX0()*(*iz).second.x0()*(*iz).second.averageRho();
	iz++;
      }
      steps.push_back((*iz).first);
      //fprintf(flog," %e %e \n", (*iz).first, (*iz).second.thicknessInX0()); 
      //fprintf(flog,"   \n"); 
      Trk::BinUtility buZ(steps,Trk::open,Trk::binZ);      
      Trk::BinnedLayerMaterial material(buZ,matMerged);
      lay->assignMaterialProperties(material);
      //std::cout <<"material comparison cylindrical layer, input/output:"<<matLayInput<<":"<<matLayWeight<< std::endl;
      if( cyl->r()>30.) layMatTot += matLayInput;
    }

    if (db && m_mat4merge[icounter].size() ) {
      const Trk::LayerMaterialProperties* layMatProp=lay->layerMaterialProperties();
 
      if (layMatProp) {
	Amg::Vector3D gp(0.5*(db->rMin()+db->rMax()),0.,lay->surfaceRepresentation().center().z());
	const Trk::MaterialProperties* mp=layMatProp->fullMaterial(gp);
        if (mp)  std::cout<<"layer material present before merging, overwritten:"<<mp->thicknessInX0()<<std::endl;
      } 
    
      std::vector<std::pair<float,Trk::MaterialProperties> > rmat;
      rmat.push_back(std::pair<float,Trk::MaterialProperties>(db->rMin(),Trk::MaterialProperties()));
      rmat.push_back(std::pair<float,Trk::MaterialProperties>(db->rMax(),Trk::MaterialProperties()));
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator ir=rmat.begin();
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator irtmp=rmat.begin();
      double zLay = lay->surfaceRepresentation().center().z();

      for (unsigned int im=0; im<m_mat4merge[icounter].size(); im++) {
        MaterialElement mat = m_mat4merge[icounter][im];
        matLayInput +=mat.mass()*mat.multi;
        double dphi = mat.phimax-mat.phimin;
	float phi_dilution = dphi<2*acos(-1.) ? dphi/2/acos(-1.) : 1.;
	//std::cout<<"adding mat element:"<<mat.zmin<<":"<<mat.zmax<<":"<<mat.matProp.thicknessInX0()<<":phi_dilution:"<<phi_dilution<<std::endl;
	//std::cout<<"phimin,max:"<<mat.phimin<<":"<<mat.phimax<<std::endl;
	//std::cout<<"zPos->dbZ:"<<0.5*(mat.zmin+mat.zmax)<<"->"<<lay->surfaceRepresentation().center().z() <<std::endl;
	//std::cout<<"adding material object:"<<mat.name<<"with material thickness:"<<mat.matProp.thicknessInX0()<<":x0:"<<
	//  mat.matProp.x0()<<std::endl;
	double dz=mat.zmax-mat.zmin; double dr=mat.rmax-mat.rmin;
	double dmax=sqrt(dr*dr+dz*dz*phi_dilution*phi_dilution);
    
        double zRef = 0.5*(mat.zmin+mat.zmax);
        double rRef = 0.5*(mat.rmin+mat.rmax);
        double zProj = zLay*zRef >0 ? zLay/zRef : 1.;
        // project object dimensions
        double mrmin = mat.rmin*zProj; double mrmax = mat.rmax*zProj;
	//std::cout<<"adding mat element in projection:"<<mrmin<<":"<<mrmax<<":"<<mat.matProp.thicknessInX0()<<":phi_dilution:"<<phi_dilution<<std::endl;
	//std::cout<<"phimin,max:"<<mat.phimin<<":"<<mat.phimax<<std::endl;
	//std::cout<<"zRef->zDisc:"<<zRef<<"->"<<zLay<<std::endl;
        // contained part
        if ( mrmin>=rmat.back().first || mrmax<rmat.front().first) {
	  std::cout << "skipping non-overlapping object:"<<mat.name<<":"<<mat.mass()<<"closest layer:"<< rmat.front().first<<":"<<rmat.back().first<<std::endl;
          continue; 
	}
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
        double rscale = rSf/(rSf-excess);
	double thickness = mat.volume()/acos(-1.)/rSf*rscale;
	//if (excess>0.) std::cout << "scaling for dB layer excess in R:"<<rscale<< std::endl;
        //

        ir = rmat.begin(); 
        //float dr = (*ir).first > mat.rmin ? fmin(mat.rmax,(*ir).first)-mat.rmin : 0.;
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
	    //std::cout <<"dz:binsize:"<<dz<<":"<<binsize<<std::endl;
            double zscale = binsize>0? overlap/binsize : 0.; 
            if (zscale>0.) {
	      //std::cout<<"adding material thickness:"<< zscale*rscale/zProj/zProj*phi_dilution*(mat.zmax-mat.zmin)/mat.matProp.x0()<<std::endl;
	      //std::cout <<"zscale,rscale,zProj,phi_dilution:"<<zscale<<","<<rscale<<","<<zProj<<":"<<phi_dilution<<std::endl;
              //double dz=mat.zmax-mat.zmin; double dr=mat.rmax-mat.rmin;
              //double th = atan(rRef/fabs(zRef));
              //double thickness = dr<dz ? fmin(dr/sin(th),dz)*cos(th) : dz;
              double rLoc =  0.5*((*(ir+1)).first+(*ir).first) ;
	      double thRef = zLay!=0 ? atan(rLoc/fabs(zLay)) : acos(-1.)/2.;
              double thickness_corr = zProj*zProj*thickness;     // correction to re-establish projection
              if ( thickness_corr > dmax*cos(thRef) ) thickness_corr=dmax*cos(thRef); 
	      (*ir).second.addMaterial(mat.matProp,thickness_corr*zscale*mat.multi/mat.matProp.X0); 
	      matMassValid += acos(-1.)*binsize*zscale*thickness*mat.matProp.rho;
	    }
	    ir++; 
	  }
	}
        //if (fabs(mat.mass()-matMassValid)>0.1*mat.mass()) {
        // std::cout << "wrong projection db:"<<mat.name<<":rscale:"<<rscale
	//							    <<":mat_true/mat_projected"<<mat.mass()/matMassValid<< std::endl;	   
	//  std::cout << "volume check:"<<mat.volume()<<":"<<acos(-1.)*thickness*(rSf-excess)<< std::endl; 
	//}
	//std::cout << "translated mass :"<<mat.mass()<<":"<<matMassValid<<":"<<rscale<<":"<<rProj<<";"<<phi_dilution<<std::endl;
	   
      }
      //if (flog) fprintf(flog,"lay disc zpos rMin rMax: %e %e %e \n",zLay,rmat.front().first,rmat.back().first); 
      //if (flog) {int nbins=rmat.size(); fprintf(flog,"nbins %d \n",nbins); }
      std::vector<float> steps; std::vector<const Trk::MaterialProperties*> matMerged;
      ir = rmat.begin();
      while (ir!=(rmat.end()-1)) {
	//std::cout<<"material array in R:"<<(*ir).first<<":"<<(*ir).second.thicknessInX0()<<std::endl;
	float sbin = (*ir).first; float dInXbin = (*ir).second.thicknessInX0();
	//fprintf(flog," %e %e \n", sbin, dInXbin); 
        steps.push_back((*ir).first); matMerged.push_back(new Trk::MaterialProperties((*ir).second));
        matLayWeight += acos(-1.)*(pow((*(ir+1)).first,2)-pow((*ir).first,2))
	  *(*ir).second.thicknessInX0()*(*ir).second.x0()*(*ir).second.averageRho();
	//std::cout<<"disk add:"<< (pow((*(ir+1)).first,2)-pow((*ir).first,2)) <<":"<<
	//  (*ir).second.thicknessInX0()<<":"<<(*ir).second.x0()<<":"<<(*ir).second.averageRho() <<std::endl;
	ir++;
      }
      steps.push_back((*ir).first);
      sbin = (*ir).first; dInXbin = (*ir).second.thicknessInX0();
      //fprintf(flog," %e %e \n", sbin, dInXbin); 
      //fprintf(flog," \n"); 
      Trk::BinUtility buR(steps,Trk::open,Trk::binR);      
      Trk::BinnedLayerMaterial material(buR,matMerged);
      lay->assignMaterialProperties(material);
      //std::cout <<"material comparison disk layer, input/output:"<<matLayInput<<":"<<matLayWeight<< std::endl; 
      layMatTot += matLayInput;
    }
    
    icounter++;
  }

  //std::cout <<"total weight input layers :"<<layMatTot<< std::endl;


  fclose(flog);
}

void Trk::MaterialLayerHelper::buildMaterialInEtaProjection(std::vector< const Trk::Layer* >&  matLayers)
{

  int icounter = 0;
  float tol = 0.1;    // smallest array step
  for (auto lay : matLayers) {

    //std::cout <<"loop over material layers, number of associated objects:"<<icounter<<":"<<m_mat4merge[icounter].size()<<std::endl;

    const Trk::DiscBounds*  db = dynamic_cast<const Trk::DiscBounds*> (&lay->surfaceRepresentation().bounds()); 
    const Trk::CylinderBounds*  cyl = dynamic_cast<const Trk::CylinderBounds*> (&lay->surfaceRepresentation().bounds()); 

    float matLayInput = 0.;
    float matLayWeight = 0.;
 
    if (cyl && m_mat4merge[icounter].size() ) {
      //std::cout<<"cyl:R/hZ:"<< cyl->r()<<","<< lay->surfaceRepresentation().center().z()<<"+-"<<cyl->halflengthZ() <<std::endl; 
      const Trk::LayerMaterialProperties* layMatProp=lay->layerMaterialProperties();
      if (layMatProp) {
	Amg::Vector3D gp(cyl->r(),0.,lay->surfaceRepresentation().center().z());
	const Trk::MaterialProperties* mp=layMatProp->fullMaterial(gp);
        if (mp)  std::cout << "layer material present before merge, overwritten:"<<mp->thicknessInX0()<<std::endl;
      }
      std::vector<std::pair<float,Trk::MaterialProperties> > emat;
      emat.push_back(std::pair<float,Trk::MaterialProperties> (eta(lay->surfaceRepresentation().center().z()-cyl->halflengthZ(),cyl->r()),
							       Trk::MaterialProperties()));  
      emat.push_back(std::pair<float,Trk::MaterialProperties> (eta(lay->surfaceRepresentation().center().z()+cyl->halflengthZ(),cyl->r()),
							       Trk::MaterialProperties()));  
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator ie=emat.begin();
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator ietmp=emat.begin();

      for (unsigned int im=0; im<m_mat4merge[icounter].size(); im++) {
        MaterialElement mat = m_mat4merge[icounter][im];
        matLayInput +=mat.mass();
        double dphi = mat.phimax-mat.phimin;
	float scale = dphi<2*acos(-1.) ? dphi/2/acos(-1.) : 1.;
        float etasize = z(mat.etamax,cyl->r())-z(mat.etamin,cyl->r());
	double thickness = mat.volume()/(2*acos(-1.)*cyl->r())/etasize;
	ie = emat.begin(); 
        float de = (*ie).first > mat.etamin ? fmin(mat.etamax,(*ie).first)-mat.etamin : 0.;
	while (ie!=(emat.end()-1)) { 
          if ((*(ie+1)).first <= mat.etamin) ie++; 
	  else if ((*ie).first >= mat.etamax) break; // TODO dz>0 should be added
	  else{
	    if ((*ie).first<mat.etamin && (*(ie+1)).first > mat.etamin ) {     // resolve lower bin
              if (mat.etamin-(*ie).first>tol && (*(ie+1)).first-mat.etamin>tol)
		ie=emat.insert(ie+1,std::pair<float,Trk::MaterialProperties>(mat.etamin,(*ie).second));              
	    }	  			
	    if ((*ie).first<mat.etamax && (*(ie+1)).first > mat.etamax) {  // resolve upper bin
              if (mat.etamax-(*ie).first>tol && (*(ie+1)).first-mat.etamax>tol) {
		ietmp=emat.insert(ie+1,std::pair<float,Trk::MaterialProperties>(mat.etamax,(*ie).second));
		ie=ietmp-1;
	      }
	    }
            if ((*ie).first<mat.etamin) de+=(*ie).first-mat.etamin; 
            if ((*(ie+1)).first>mat.etamax) de+=mat.etamax-(*(ie+1)).first;
            if (ie+2 == emat.end() && mat.etamax>(*(ie+1)).first) de+=mat.etamax-fmax((*(ie+1)).first,mat.etamin); 
            float binsize = (*(ie+1)).first-(*ie).first;
            float fscale = binsize>0? de/binsize+1 : 0.;  
	    (*ie).second.addMaterial(mat.matProp,fscale*scale*thickness/mat.matProp.X0);
	    ie++; de=0.;
	  }
	}	   
      }
      std::vector<float> steps; std::vector<const Trk::MaterialProperties*> matMerged;
      ie = emat.begin();
      while (ie!=(emat.end()-1)) {
	//std::cout<<"material array in z:"<<z((*ie).first,cyl->r())<<":"<<(*ie).second.thicknessInX0()<<std::endl;
        steps.push_back(z((*ie).first,cyl->r())); matMerged.push_back(new Trk::MaterialProperties((*ie).second));
        matLayWeight +=2*acos(-1.)*cyl->r()*(z((*(ie+1)).first,cyl->r())-z((*ie).first,cyl->r()))
	  *(*ie).second.thicknessInX0()*(*ie).second.x0()*(*ie).second.averageRho();
	ie++;
      }
      steps.push_back(z((*ie).first,cyl->r()));
      Trk::BinUtility buZ(steps,Trk::open,Trk::binZ);      
      Trk::BinnedLayerMaterial material(buZ,matMerged);
      lay->assignMaterialProperties(material);
      //std::cout <<"material comparison cylindrical layer, input/output:"<<matLayInput<<":"<<matLayWeight<< std::endl;
    }

    if (db && m_mat4merge[icounter].size() ) {
      const Trk::LayerMaterialProperties* layMatProp=lay->layerMaterialProperties();
 
      if (layMatProp) {
	Amg::Vector3D gp(0.5*(db->rMin()+db->rMax()),0.,lay->surfaceRepresentation().center().z());
	const Trk::MaterialProperties* mp=layMatProp->fullMaterial(gp);
        if (mp)  std::cout<<"layer material present before merging, overwritten:"<<mp->thicknessInX0()<<std::endl;
      } 
    
      std::vector<std::pair<float,Trk::MaterialProperties> > rmat;
      rmat.push_back(std::pair<float,Trk::MaterialProperties>(db->rMin(),Trk::MaterialProperties()));
      rmat.push_back(std::pair<float,Trk::MaterialProperties>(db->rMax(),Trk::MaterialProperties()));
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator ir=rmat.begin();
      std::vector<std::pair<float,Trk::MaterialProperties> >::iterator irtmp=rmat.begin();

      for (unsigned int im=0; im<m_mat4merge[icounter].size(); im++) {
        MaterialElement mat = m_mat4merge[icounter][im];
        matLayInput +=mat.mass();
        double dphi = mat.phimax-mat.phimin;
	float scale = dphi<2*acos(-1.) ? dphi/2/acos(-1.) : 1.;
        ir = rmat.begin(); 
        float dr = (*ir).first > mat.rmin ? fmin(mat.rmax,(*ir).first)-mat.rmin : 0.;
	while (ir!=(rmat.end()-1)) { 
          if ((*(ir+1)).first < mat.rmin) ir++; 
	  else if ((*ir).first >= mat.rmax) break;  // TODO dr>0 should be added
          else {
	    if ((*ir).first<mat.rmin && (*(ir+1)).first > mat.rmin ) {     // resolve lower bin
              if (mat.rmin-(*ir).first>tol && (*(ir+1)).first-mat.rmin>tol)
		ir=rmat.insert(ir+1,std::pair<float,Trk::MaterialProperties>(mat.rmin,(*ir).second));
	    }	  			
	    if ((*ir).first<mat.rmax && (*(ir+1)).first > mat.rmax) {  // resolve upper bin
              if (mat.rmax-(*ir).first>tol && (*(ir+1)).first-mat.rmax>tol) {
		irtmp=rmat.insert(ir+1,std::pair<float,Trk::MaterialProperties>(mat.rmax,(*ir).second));
		ir=irtmp-1;
	      }
	    }
            if ((*ir).first<mat.rmin) dr+=(*ir).first-mat.rmin; 
            if ((*(ir+1)).first>mat.rmax) dr+=mat.rmax-(*(ir+1)).first;
            if (ir+2 == rmat.end() && mat.rmax>(*(ir+1)).first) dr+=mat.rmax-fmax((*(ir+1)).first,mat.rmin); 
            float binsize = (*(ir+1)).first-(*ir).first;
            float rscale = binsize>0? dr/binsize+1 : 0.; 
            if (rscale*scale*(mat.zmax-mat.zmin)/mat.matProp.X0>1.e-6) {
	      (*ir).second.addMaterial(mat.matProp,rscale*scale*(mat.zmax-mat.zmin)/mat.matProp.X0);
	    }
	    ir++; dr=0.;
	  }
	}	   
      }
      std::vector<float> steps; std::vector<const Trk::MaterialProperties*> matMerged;
      ir = rmat.begin();
      while (ir!=(rmat.end()-1)) {
	//std::cout<<"material array in R:"<<(*ir).first<<":"<<(*ir).second.thicknessInX0()<<std::endl;
        steps.push_back((*ir).first); matMerged.push_back(new Trk::MaterialProperties((*ir).second));
        matLayWeight += acos(-1.)*(pow((*(ir+1)).first,2)-pow((*ir).first,2))
	  *(*ir).second.thicknessInX0()*(*ir).second.x0()*(*ir).second.averageRho();
	ir++;
      }
      steps.push_back((*ir).first);
      Trk::BinUtility buR(steps,Trk::open,Trk::binR);      
      Trk::BinnedLayerMaterial material(buR,matMerged);
      lay->assignMaterialProperties(material);
      //std::cout <<"material comparison disk layer, input/output:"<<matLayInput<<":"<<matLayWeight<< std::endl; 
    }
    
    icounter++;
  }


}

double Trk::MaterialLayerHelper::eta(double z, double R) const 
{
  double th = atan(R/z);
  if (z<0.) th+=acos(-1.);
  double eta = -log(tan(th/2.)); 

  return eta;
}

double Trk::MaterialLayerHelper::z(double eta, double R) const 
{
  double th = 2*atan(exp(-eta));
  double z = R / tan(th);
  return z;
}

void Trk::MaterialLayerHelper::printInfo() const {}

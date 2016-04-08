/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/AGDD2GeoModelBuilder.h"
#include "AGDDModel/AGDDMaterialStore.h"
#include "AGDDModel/AGDDMaterial.h"
#include "AGDDModel/AGDDMolecule.h"
#include "AGDDModel/AGDDMixture.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDKernel/AGDDDetector.h"
#include "AGDDKernel/AGDDDetectorPositioner.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDModel/AGDDBox.h"
#include "AGDDModel/AGDDTubs.h"
#include "AGDDModel/AGDDElcyl.h"
#include "AGDDModel/AGDDTrd.h"
#include "AGDDModel/AGDDCons.h"
#include "AGDDModel/AGDDPcon.h"
#include "AGDDModel/AGDDPgon.h"
#include "AGDDModel/AGDDColorStore.h"
#include "AGDDModel/AGDDColor.h"
#include "AGDDModel/AGDDGvxy.h"
#include "AGDDModel/AGDDSnake.h"
#include "AGDDModel/AGDDUnion.h"
#include "AGDDModel/AGDDSubtraction.h"
#include "AGDDModel/AGDDIntersection.h"
#include "AGDDModel/AGDDComposition.h"
#include "AGDDKernel/TwoPoint.h"
#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDSectionStore.h"
#include "AGDDKernel/AGDDSection.h"

#include "AGDDModel/AGDDBolt.h"
#include "AGDDModel/AGDDIbeam.h"
#include "AGDDModel/AGDDUbeam.h"

#include "AGDDKernel/AliasStore.h"



#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoEllipticalTube.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoPgon.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeIntersection.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoTransform.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GeoModelInterfaces/StoredMaterialManager.h" 

#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Point3D.h"

#include <iostream>
#include <sstream>
#include <vector>


AGDD2GeoModelBuilder::AGDD2GeoModelBuilder():AGDDBuilder(),m_mother(0)
{
//	std::cout << "Creating AGDD2GeoModel Builder"<<std::endl;
//  m_detectors = new std::map<std::string, GeoFullPhysVol*>;
//  m_detectors->clear();
}
GeoElement* AGDD2GeoModelBuilder::CreateElement(std::string name)
{
	AGDDMaterialStore *ms=AGDDMaterialStore::GetMaterialStore();
	AGDDElement *el=ms->GetElement(name);
	if (el)
		if (el->Extant())
			return (GeoElement *)(el->GetElement());
		else
		{
			el->Created(true);
			GeoElement *g4el;
			g4el=new GeoElement(el->GetName(),el->GetSymbol(),
					    double(el->GetZ()),el->GetA()*(CLHEP::g/CLHEP::mole));
			el->SetElement(g4el);
			return g4el;
		}
	else
		return 0;
}
GeoMaterial* AGDD2GeoModelBuilder::CreateMaterial(std::string name)
{

//  give priority to GeoModel's Material Manager in retrieving materials
	GeoMaterial* mmMaterial=GetMMMaterial(name);
	if (mmMaterial)
	{
//		std::cout<<"material "<<name<<" found in Material Manager "<<std::endl;
		return mmMaterial;
	}
	
//  oh well... too bad....

	AGDDMaterialStore *ms=AGDDMaterialStore::GetMaterialStore();
	AGDDSimpleMaterial *mat=ms->GetMaterial(name);
	if (mat)
		if (mat->Extant())
			return (GeoMaterial*)(mat->GetMaterial());
		else
		{
			mat->Created(true);
			material_type mtype=mat->GetMaterialType();
			GeoMaterial *g4mat;
			if (mtype==Material)
			{
				AGDDMaterial* nmat=dynamic_cast<AGDDMaterial*>(mat);
				if (!nmat) {
                                  std::cout<<"something is wrong, nmat=0!!!"<<std::endl;
                                  return 0;
                                }
				g4mat=new GeoMaterial(nmat->GetName(),nmat->GetDensity()*(CLHEP::g/CLHEP::cm3));
				AGDDElement *el=ms->GetElement(nmat->GetName());
				if (el) 
				{
					if (!el->Extant()) CreateElement(el->GetName());
					g4mat->add((GeoElement*)(el->GetElement()));	
				}
				mat->SetMaterial(g4mat);
				g4mat->lock();
				return g4mat;
			}
			else if (mtype==Molecule)
			{
				AGDDMolecule* nmat=dynamic_cast<AGDDMolecule*>(mat);
				if (!nmat) {
                                  std::cout<<"something is wrong, nmat=0!!!"<<std::endl;
                                  return 0;
                                }
				g4mat=new GeoMaterial(nmat->GetName(),nmat->GetDensity()*(CLHEP::g/CLHEP::cm3));
				for (int i=0;i<nmat->NComponents();i++)
				{
					AGDDElement *el=nmat->Element(i);
					GeoElement* g4el=CreateElement(el->GetName());
					g4mat->add(g4el,nmat->Composition(i));
				}
				mat->SetMaterial(g4mat);
				g4mat->lock();
				return g4mat;
			}
			else if (mtype==Mixture)
			{
//				std::cout<<"Mixture "<<mat->GetName()<<std::endl;
				AGDDMixture* nmat=dynamic_cast<AGDDMixture*>(mat);
				if (!nmat)
				{
					std::cout<<"can't get the mixture, returning!"<<std::endl;
					return 0;
				}
				g4mat=new GeoMaterial(nmat->GetName(),nmat->GetDensity()*(CLHEP::g/CLHEP::cm3));
				for (int i=0;i<nmat->NComponents();i++)
				{
					AGDDSimpleMaterial *el=nmat->Material(i);
					GeoMaterial* g4el=CreateMaterial(el->GetName());
					g4mat->add(g4el,nmat->Composition(i));
				}
				mat->SetMaterial(g4mat);
				g4mat->lock();
				return g4mat;
			}
			else
			{
				std::cout<<" Help! unknown material type!!!"<<std::endl;
				return 0;
			}
		}
	else
		return 0;
}
void AGDD2GeoModelBuilder::CreateElements()
{
	AGDDMaterialStore *ms=AGDDMaterialStore::GetMaterialStore();
	ElementIterator it;
	for (it=ms->ElementBegin();it!=ms->ElementEnd();it++)
	{
		CreateElement((*it).second->GetName());
	}
}
void AGDD2GeoModelBuilder::CreateMaterial()
{
	AGDDMaterialStore *ms=AGDDMaterialStore::GetMaterialStore();
	MaterialIterator it;
	for (it=ms->MaterialBegin();it!=ms->MaterialEnd();it++)
	{
		CreateMaterial((*it).second->GetName());
	}
}
void AGDD2GeoModelBuilder::CreateBox(AGDDBox* v)
{
//	std::cout<<"this is CreateBox"<<std::endl;
	void *p=v->GetSolid();
	if (!p)
	{
		GeoShape* solid=new GeoBox(v->x()/2.,v->y()/2.,std::abs(v->z())/2.);
		v->SetSolid(solid);
	}
}
void AGDD2GeoModelBuilder::CreateTrd(AGDDTrd* v)
{
	void *p=v->GetSolid();
	if (!p)
	{
		GeoShape* solid=new GeoTrd(v->x1()/2.,v->x2()/2,v->y1()/2.,v->y2()/2.,v->z()/2.);
		v->SetSolid(solid);
	}
}

void AGDD2GeoModelBuilder::CreateSnake(AGDDSnake* v)
{
// here begins a nasty piece of code
	static GeoBox *box1=new GeoBox(1.*CLHEP::km,1*CLHEP::km,1*CLHEP::km);
	CLHEP::Hep3Vector v1(0.,0.,-1*CLHEP::km);
	CLHEP::Hep3Vector v2(0.,0.,+1*CLHEP::km);
	CLHEP::HepRotation h;
	HepGeom::Transform3D ttt1(h,v1);
	HepGeom::Transform3D ttt2(h,v2);
	static GeoShape *s1=new GeoShapeShift(box1,ttt1);
	static GeoShape *s2=new GeoShapeShift(box1,ttt2);
	
//	std::cout<<" Snake "<<v->GetName()<<" nr. of Points "<<v->NrOfPoints()<<std::endl;
	double radius=v->Radius();
//	std::cout<<" Snake radius "<<radius<<std::endl;
	CLHEP::Hep3Vector axis0=(v->GetPoint(0)-v->GetPoint(1));
	CLHEP::Hep3Vector axis(v->GetPoint(1)-v->GetPoint(0));
	CLHEP::Hep3Vector axis1;
	CLHEP::Hep3Vector axis2(v->GetPoint(2)-v->GetPoint(1));
	double length=axis.mag();
	double angle1=0;
	double angle2=std::abs(axis.angle(axis2))/2;
	double delta_l1=0;
	double delta_l2=radius*tan(angle2);
	double lengthnew=length+delta_l2;
	GeoShape* solid=new GeoTubs(0.,radius,lengthnew/2.,0.,4*asin(1.));
	
	const CLHEP::Hep3Vector vt(0.,0.,-lengthnew/2.+delta_l2+2.);
	CLHEP::HepRotation rrr;
	rrr.rotateZ(axis2.phi());
	rrr.rotateY(angle2);
	HepGeom::Transform3D ttt(rrr,vt);
	GeoShape *ssnew=new GeoShapeShift(s1,ttt);
	
	solid = new GeoShapeSubtraction(solid,ssnew);
//	std::cout<<" angles theta, phi "<<axis2.theta()<<" "<<axis2.phi()<<std::endl;
	
	CLHEP::Hep3Vector vref(0.,0.,-lengthnew/2.);
	CLHEP::HepRotation r;
	HepGeom::Transform3D tref(r,vref);
	solid=new GeoShapeShift(solid,tref);
	CLHEP::HepRotation r1;
	r1.rotateY(axis0.theta());
	r1.rotateZ(axis0.phi());
	CLHEP::Hep3Vector vtt(v->GetPoint(0).x(),v->GetPoint(0).y(),v->GetPoint(0).z());
	HepGeom::Transform3D t(r1,vtt);
	solid=new GeoShapeShift(solid,t);
	
 	for (int i=1;i<v->NrOfPoints()-1;i++)
	{
		axis0=v->GetPoint(i)-v->GetPoint(i+1);
		axis=v->GetPoint(i+1)-v->GetPoint(i);
		axis1=v->GetPoint(i)-v->GetPoint(i-1);

		length=axis.mag();
		angle1=std::abs(axis.angle(axis1))/2;
		delta_l1=radius*tan(angle1);
		delta_l2=0;
		if (i<(v->NrOfPoints()-2)) 
		{
			axis2=v->GetPoint(i+2)-v->GetPoint(i+1);
			angle2=std::abs(axis.angle(axis2))/2;
			delta_l2=radius*tan(angle2);
		}
		length=axis.mag();
		lengthnew=length+delta_l1+delta_l2;

		CLHEP::HepRotation rr;
		CLHEP::Hep3Vector vvref(0.,0.,-lengthnew/2+delta_l1);
		HepGeom::Transform3D ttref(rr,vvref);
		
		GeoShape* ss=new GeoTubs(0.,radius,lengthnew/2.,0.,4*asin(1.));

	    const CLHEP::Hep3Vector vt1(0.,0.,+lengthnew/2.-delta_l1-2.);
		const CLHEP::Hep3Vector vt2(0.,0.,-lengthnew/2.+delta_l2+2.);
	    CLHEP::HepRotation rrr1,rrr2;
		
	    rrr1.rotateY(angle1);
		rrr1.rotateZ(-axis1.phi());
		rrr2.rotateY(-angle2);
		rrr2.rotateZ(axis2.phi());
	    HepGeom::Transform3D ttt1(rrr1,vt1);
		HepGeom::Transform3D ttt2(rrr2,vt2);
	    GeoShape *ssnew1=new GeoShapeShift(s2,ttt1);
		ss = new GeoShapeSubtraction(ss,ssnew1);
		if (i<(v->NrOfPoints()-2)) 
		{
			GeoShape *ssnew2=new GeoShapeShift(s1,ttt2);
	    	ss = new GeoShapeSubtraction(ss,ssnew2);
		}

		ss=new GeoShapeShift(ss,ttref);
		
	//	std::cout<<" angles: theta "<<axis.theta()<<" phi "<<axis.phi()<<std::endl;
		CLHEP::HepRotation rr1;

		rr1.rotateY(axis0.theta());
		rr1.rotateZ(axis0.phi());

		const CLHEP::Hep3Vector vv(v->GetPoint(i).x(),v->GetPoint(i).y(),v->GetPoint(i).z());
		HepGeom::Transform3D tt(rr1,vv);

		ss=new GeoShapeShift(ss,tt);
		solid=new GeoShapeUnion(solid,ss);
	}
	v->SetSolid(solid);
}

void AGDD2GeoModelBuilder::CreateCons(AGDDCons* v)
{
	const double mdeg=asin(1)/90.;
	void *p=v->GetSolid();
	if (!p)
	{
		GeoShape* solid=new GeoCons(v->rin1(),v->rin2(),v->rou1(),v->rou2(),v->z()/2.,v->phi0()*mdeg,v->dphi()*mdeg);
		v->SetSolid(solid);
	}
}
void AGDD2GeoModelBuilder::CreateTubs(AGDDTubs* v)
{
	const double mdeg=asin(1.)/90.;
	void *p=v->GetSolid();
	if (!p)
	{
//		std::cout<<"++++++++ Tubs "<<v->phi0()<<" "<<v->dphi()<<std::endl;
		GeoShape* solid=new GeoTubs(v->rin(),v->rou(),v->z()/2.,v->phi0()*mdeg,v->dphi()*mdeg);
		v->SetSolid(solid);
	}
}

void AGDD2GeoModelBuilder::CreateElcyl(AGDDElcyl* v)
{
	void *p=v->GetSolid();
	if (!p)
	{
		GeoShape* solid=new GeoEllipticalTube(v->dx(),v->dy(),v->z()/2.);
		v->SetSolid(solid);
	}
}

void AGDD2GeoModelBuilder::CreateGvxy(AGDDGvxy* v)
{
	void *p=v->GetSolid();
//	std::cout<<"now creating BREP"<<v->GetName()<<std::endl;
	if (!p)
	{
		std::vector<CLHEP::Hep2Vector> points;
		int nPoint=v->NrOfPoints();
		GeoSimplePolygonBrep* solid;
		solid = new GeoSimplePolygonBrep(v->GetDz()/2.);
		double area=0;
		v->SetPoint(v->GetPoint(0));
		for (int i=0;i<nPoint;i++)
		{
//			std::cout << " \ttwo point "<<v->GetPoint(i).x()<<" "<<v->GetPoint(i).y()<<std::endl;
			int iplus=i+1;
			area+= v->GetPoint(i).x()*v->GetPoint(iplus).y()-v->GetPoint(iplus).x()*v->GetPoint(i).y();
			
		}
//		std::cout<< " area "<<area<<std::endl;
		bool clockwise=area<0?true:false;
//		std::cout<< " clockwise? "<<clockwise<<std::endl;

		TwoPoint pV;
		for (int i=0;i<nPoint;i++)
		{
			if (clockwise) pV=v->GetPoint(nPoint-1-i);
			else pV=v->GetPoint(i);
//			std::cout<<"\t\t Point "<<pV.x()<<" "<<pV.y()<<std::endl;
			solid->addVertex(pV.x(),pV.y());
		}
		v->SetSolid(solid);
	}
}

void AGDD2GeoModelBuilder::CreateUnion(AGDDUnion* v)
{
	int nPos=v->NrOfDaughter();
	AGDDPositioner* pos=v->GetDaughter(0);
	AGDDVolume *vol=pos->GetVolume();
//	std::cout<<" volume name "<<vol->GetName()<<std::endl;
	vol->CreateSolid();
	GeoShape *sV=(GeoShape*)(vol->GetSolid());
	sV=new GeoShapeShift(sV,pos->Transform());
 	for (int i=1;i<nPos;i++)
 	{
 		AGDDPositioner* pp=v->GetDaughter(i);
 		AGDDVolume *vv=pp->GetVolume();
//		std::cout<<" \t secondary volume name "<<vv->GetName()<<std::endl;
 		vv->CreateSolid();
 		GeoShape *nsV=(GeoShape*)(vv->GetSolid());
		nsV=new GeoShapeShift(nsV,pp->Transform());
 		sV=new GeoShapeUnion(sV,nsV);
 	}
	v->SetMaterial(vol->GetMaterial());
	v->SetColor(vol->GetColor());
	v->SetSolid(sV);
}
void AGDD2GeoModelBuilder::CreateIntersection(AGDDIntersection* v)
{
	int nPos=v->NrOfDaughter();
	AGDDPositioner* pos=v->GetDaughter(0);
	AGDDVolume *vol=pos->GetVolume();
	vol->CreateSolid();
	GeoShape *sV=(GeoShape*)(vol->GetSolid());
	sV=new GeoShapeShift(sV,pos->Transform());
 	for (int i=1;i<nPos;i++)
 	{
 		AGDDPositioner* pp=v->GetDaughter(i);
 		AGDDVolume *vv=pp->GetVolume();
 		vv->CreateSolid();
 		GeoShape *nsV=(GeoShape*)(vv->GetSolid());
		nsV=new GeoShapeShift(nsV,pp->Transform());
 		sV=new GeoShapeIntersection(sV,nsV);
 	}
	v->SetMaterial(vol->GetMaterial());
	v->SetSolid(sV);
}
void AGDD2GeoModelBuilder::CreateSubtraction(AGDDSubtraction* v)
{
	int nPos=v->NrOfDaughter();
	AGDDPositioner* pos=v->GetDaughter(0);
	AGDDVolume *vol=pos->GetVolume();
	vol->CreateSolid();
	GeoShape *sV=(GeoShape*)(vol->GetSolid());
	sV=new GeoShapeShift(sV,pos->Transform());
 	for (int i=1;i<nPos;i++)
 	{
 		AGDDPositioner* pp=v->GetDaughter(i);
 		AGDDVolume *vv=pp->GetVolume();
 		vv->CreateSolid();
 		GeoShape *nsV=(GeoShape*)(vv->GetSolid());
		nsV=new GeoShapeShift(nsV,pp->Transform());
 		sV=new GeoShapeSubtraction(sV,nsV);
 	}
	v->SetMaterial(vol->GetMaterial());
	v->SetSolid(sV);
}

void AGDD2GeoModelBuilder::CreatePcon(AGDDPcon* v)
{
	void *p=v->GetSolid();
	if (!p)
	{
		int nPlanes=v->NrOfPlanes();
		GeoPcon* solid=new GeoPcon(v->Phi0()*CLHEP::deg,v->Dphi()*CLHEP::deg);
		for (int i=0;i<nPlanes;i++)
		{
			double ri=v->Rin(i);
			double ro=v->Rout(i);
			double z=v->Z(i);
			solid->addPlane(z,ri,ro);
		}
		v->SetSolid(solid);
	}
}

void AGDD2GeoModelBuilder::CreatePgon(AGDDPgon* v)
{
	void *p=v->GetSolid();
	if (!p)
	{
		int nPlanes=v->NrOfPlanes();
		GeoPgon* solid=new GeoPgon(v->Phi0()*CLHEP::deg,v->Dphi()*CLHEP::deg,v->_nbPhi);
		for (int i=0;i<nPlanes;i++)
		{
			double ri=v->Rin(i);
			double ro=v->Rout(i);
			double z=v->Z(i);
			solid->addPlane(z,ri,ro);
		}
		v->SetSolid(solid);
	}
}


void AGDD2GeoModelBuilder::CreateComposition(AGDDComposition *v)
{
	static int ifirst=1;
	static GeoMaterial *ether=0;
	static GeoShape* fakeVol=0;
	if (ifirst)
	{
		ifirst=0;
//		ether=new GeoMaterial("special::Ether",0.);
        ether = GetMMMaterial("special::Ether");
	 	fakeVol=new GeoTubs(0.,500.,1000.,0.,4*asin(1.));
	}

	if (!v->GetVolume())
	{
		// std::cout<<"CreateComposition: Logical Volume "<<v->GetName()<<std::endl;
		GeoLogVol *a=new GeoLogVol(v->GetName(),fakeVol,ether);
		GeoPhysVol *a_phys=new GeoPhysVol(a);
		v->SetVolume(a_phys);

		for (int i=0;i<v->NrOfDaughter();i++)
		{
			AGDDPositioner* pos=v->GetDaughter(i);
			AGDDVolume *vol=pos->GetVolume();
			const std::string volName = vol->GetName();
			
			bool isDetElement=vol->IsSensitiveVolume();
			AGDDDetector *d=0;
			AGDDDetectorPositioner *p=0;
			std::string detFullTag="";
			if (isDetElement) 
			{ 
				AGDDDetectorStore* ds=AGDDDetectorStore::GetDetectorStore();
				d=ds->GetDetector(volName);
				if (!d) std::cout<<" Help! can't retrieve Detector element for "<<volName<<std::endl;
				p=dynamic_cast<AGDDDetectorPositioner *>(pos);
				if (p) detFullTag=p->ID.detectorAddress;
				else std::cout<<" something is very wrong!!!!"<<std::endl;
				// if (isDetElement) std::cout<<"\t\t Detector: "<<volName<<" "<<detFullTag<<std::endl;
			}
			HepGeom::Transform3D trf=pos->Transform();
			GeoTransform *geotrf=new GeoTransform(trf);
			void *temp=vol->GetVolume();
			
			// GeoFullPhysVol are needed for detectors (corresponding to ReadoutElements)
			GeoFullPhysVol* detVol=NULL;
			if (!temp) 
			{
			  // if it's the first occurrence of this Volume, build it 
			  vol->CreateVolume();
			  if (isDetElement) {
			    detVol=(GeoFullPhysVol*)(vol->GetVolume());
				if (p) p->theVolume=detVol;
			    // (*m_detectors)[detFullTag]=detVol;
			  }
			}
			else {
			  // if this Volume was already built, you can re-use it; 
			  // however if it is a GeoFullPhysVol you need to clone it 
			  // (in order to allow for a new and indipendent cached transform)
			  if (isDetElement) 
			    {
			      detVol=(GeoFullPhysVol*)temp;
			      detVol=detVol->clone();
			      //(*m_detectors)[detFullTag]=detVol;
				  if (p) p->theVolume=detVol;
			    }
			}
			
			a_phys->add(geotrf);
			if (isDetElement) a_phys->add(detVol);
			else a_phys->add((GeoPhysVol*)(vol->GetVolume()));
		}
	}
}

void AGDD2GeoModelBuilder::CreateVolume(AGDDVolume* v)
{
//	std::cout<<" this is CreateVolume"<<std::endl;

	GeoMaterial *mat=CreateMaterial(ALIAS(v->GetMaterial()));
	
	void* p=v->GetVolume();
	if (!p)
	{
//		std::cout<<" creating Volume "<<v->GetName()<<std::endl;
		GeoShape* sol=(GeoShape*)v->GetSolid();
		if (!sol) std::cout<<" Something wrong!!! solid is NULL "<<std::endl;
        else
		{
			GeoLogVol* lv=new GeoLogVol(v->GetName(),sol,mat);
			GeoPhysVol *lv_phys=new GeoPhysVol(lv);

			v->SetVolume(lv_phys);
		}
	}
}

void AGDD2GeoModelBuilder::BuildAllVolumes()
{
  AGDDVolumeStore *vs=AGDDVolumeStore::GetVolumeStore();
  AGDDVolumeMap::const_iterator it;
  CLHEP::HepRotation rot;
  CLHEP::Hep3Vector transl(0.,0.,0.);
  HepGeom::Transform3D trf(rot,transl);
  
  for (it=vs->begin();it!=vs->end();it++)
  {
  	AGDDVolume* vol=(*it).second;
  	// std::cout<<" Volume in the list "<<vol->GetName();
	if (!vol->HasParent())
	{
//		std::cout<<" start building with "<<vol->GetName()<<std::endl;
		vol->CreateVolume();
		AGDDComposition *vv=dynamic_cast<AGDDComposition *>(vol);
		
		if (vv)
		{
			GeoPhysVol *vvv=(GeoPhysVol*)(vol->GetVolume());
			if (vvv)
			{
				if (!m_mother) 
				{
					std::cout<<"AGDDController: mother not set!!"<<std::endl;
					return;
				}
                                GeoTransform *gtrf=new GeoTransform(trf);
				m_mother->add(gtrf);
				m_mother->add(vvv);
			}
		}
	}
  }
}

void AGDD2GeoModelBuilder::BuildFromSection(std::string s)
{
  CLHEP::HepRotation rot;
  CLHEP::Hep3Vector transl(0.,0.,0.);
  HepGeom::Transform3D trf(rot,transl);
  
  
  AGDDSectionStore* ss=AGDDSectionStore::GetSectionStore();
  AGDDSection* sect=ss->GetSection(s);

  bool bFlag=sect->IsToBeBuilt();
  if (bFlag)
  {
    volumeIterator it;
    std::string topVolumeName=sect->TopVolume();
    if (topVolumeName!="useless" && !topVolumeName.empty())
    {
       for (it=sect->VolumeBegin();it!=sect->VolumeEnd();it++)
       {
          AGDDVolume* vol=(*it).second;
          if (vol->GetName()==topVolumeName)
          {
             vol->CreateVolume();

             AGDDComposition *vv=dynamic_cast<AGDDComposition *>(vol);

             if (vv)
             {
                GeoPhysVol *vvv=(GeoPhysVol*)(vol->GetVolume());
                if (vvv)
                {
		   if (!m_mother) 
		   {
		 	std::cout<<"AGDDController: mother not set!!"<<std::endl;
			return;
		   }
		   GeoTransform *gtrf=new GeoTransform(trf);
                   m_mother->add(gtrf);
                   m_mother->add(vvv);
                }
             }
          }
       }
    }
    else 
      for (it=sect->VolumeBegin();it!=sect->VolumeEnd();it++)
      {
  	    AGDDVolume* vol=(*it).second;
  	    // std::cout<<" Volume in the list "<<vol->GetName();
	    if (!vol->HasParent())
	    {
//		     std::cout<<" start building with "<<vol->GetName()<<std::endl;
		  vol->CreateVolume();
		  AGDDComposition *vv=dynamic_cast<AGDDComposition *>(vol);
		
		  if (vv)
		  {
			GeoPhysVol *vvv=(GeoPhysVol*)(vol->GetVolume());
			if (vvv)
			{
				if (!m_mother) 
				{
					std::cout<<"AGDDController: mother not set!!"<<std::endl;
					return;
				}
				GeoTransform *gtrf=new GeoTransform(trf);
				m_mother->add(gtrf);
				m_mother->add(vvv);
			}
		  }
	    }
      }
  } 
  else
    std::cout<<" This section is flagged as not to be built!!"<<std::endl;


}
void AGDD2GeoModelBuilder::BuildFromVolume(std::string s)
{
  CLHEP::HepRotation rot;
  CLHEP::Hep3Vector transl(0.,0.,0.);
  HepGeom::Transform3D trf(rot,transl);
  
    AGDDVolumeStore *vs=AGDDVolumeStore::GetVolumeStore();
	AGDDVolume* vol=vs->GetVolume(s);
	if (!vol)
	{
		std::cout<<"============>>>>>>> Warning!!! Volume "<<s<<" not found in the store!!!!!! quitting...."<<std::endl;
		return;
	}
	vol->CreateVolume();
		GeoPhysVol *vvv=(GeoPhysVol*)(vol->GetVolume());
		if (vvv)
		{
			if (!m_mother) 
			{
				std::cout<<"AGDDController: mother not set!!"<<std::endl;
				return;
			}
			GeoTransform *gtrf=new GeoTransform(trf);
			m_mother->add(gtrf);
			m_mother->add(vvv);
		}
}

void AGDD2GeoModelBuilder::CreateBolt(AGDDBolt *b)
{
	const double mdeg=asin(1.)/90.;
	void *p=b->GetSolid();
	if (!p)
	{
		std::cout<<"creating bolt"<<std::endl;
		GeoShape* solid=new GeoTubs(0,b->_diameter/2.,b->_length/2.,0.,360.*mdeg);
		
		GeoPgon* s=new GeoPgon(0.,360*mdeg,6);
		s->addPlane(-b->_length/2.,0,b->_headDiameter/2.);
		s->addPlane(-b->_length/2.+b->_headLength,0,b->_headDiameter/2.);
		solid=new GeoShapeUnion(solid,s);		
		b->SetSolid(solid);
	}
}

void AGDD2GeoModelBuilder::CreateIbeam(AGDDIbeam *b)
{
	void *p=b->GetSolid();
	if (!p)
	{
		GeoSimplePolygonBrep* solid;
		solid = new GeoSimplePolygonBrep(b->GetLength()/2.);
		solid->addVertex(b->_width/2.,b->_height/2);
		solid->addVertex(-b->_width/2.,b->_height/2);
		solid->addVertex(-b->_width/2.,b->_smallHeight/2);
		solid->addVertex(-b->_smallWidth/2.,b->_smallHeight/2);
		solid->addVertex(-b->_smallWidth/2.,-b->_smallHeight/2);
		solid->addVertex(-b->_width/2.,-b->_smallHeight/2);
		solid->addVertex(-b->_width/2.,-b->_height/2);
		solid->addVertex(b->_width/2.,-b->_height/2);
		solid->addVertex(b->_width/2.,-b->_smallHeight/2);
		solid->addVertex(b->_smallWidth/2.,-b->_smallHeight/2);
		solid->addVertex(b->_smallWidth/2.,b->_smallHeight/2);
		solid->addVertex(b->_width/2.,b->_smallHeight/2);
		b->SetSolid(solid);
	}
}

void AGDD2GeoModelBuilder::CreateUbeam(AGDDUbeam *b)
{
	void *p=b->GetSolid();
	if (!p)
	{
		GeoSimplePolygonBrep* solid;
		solid = new GeoSimplePolygonBrep(b->GetLength()/2.);
		solid->addVertex(b->_width/2.,b->_smallHeight/2.);
		solid->addVertex(-b->_width/2.,b->_smallHeight/2.);
		solid->addVertex(-b->_width/2.,-b->_height+b->_smallHeight/2.);
		solid->addVertex(-b->_width/2.+b->_smallWidth,-b->_height+b->_smallHeight/2.);
		solid->addVertex(-b->_width/2.+b->_smallWidth,-b->_smallHeight/2.);
		solid->addVertex(b->_width/2.-b->_smallWidth,-b->_smallHeight/2.);
		solid->addVertex(b->_width/2.-b->_smallWidth,-b->_height+b->_smallHeight/2.);
		solid->addVertex(b->_width/2.,-b->_height+b->_smallHeight/2.);
		b->SetSolid(solid);
	}
}

GeoMaterial* AGDD2GeoModelBuilder::GetMMMaterial(std::string name)
{
	StoreGateSvc* pDetStore=0;
	ISvcLocator* svcLocator = Gaudi::svcLocator();
	StatusCode sc=svcLocator->service("DetectorStore",pDetStore);
	if(sc.isSuccess())
	{
		DataHandle<StoredMaterialManager> theMaterialManager;
		sc = pDetStore->retrieve(theMaterialManager, "MATERIALS");
		if(sc.isSuccess())
        {
			return theMaterialManager->getMaterial(name);
        }
	}
	return 0;
}

//std::map<std::string, GeoFullPhysVol*>* AGDD2GeoModelBuilder::GetMSdetectors() const
//{
//  return m_detectors;
//}

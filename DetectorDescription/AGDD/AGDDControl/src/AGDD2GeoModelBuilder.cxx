/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/AGDD2GeoModelBuilder.h"

#include "AGDDKernel/AGDDDetector.h"
#include "AGDDKernel/AGDDDetectorPositioner.h"
#include "AGDDKernel/AGDDDetectorStore.h"
#include "AGDDKernel/TwoPoint.h"
#include "AGDDKernel/AGDDVolumeStore.h"
#include "AGDDKernel/AGDDSectionStore.h"
#include "AGDDKernel/AGDDSection.h"
#include "AGDDKernel/AliasStore.h"

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
#include "AGDDModel/AGDDMaterialStore.h"
#include "AGDDModel/AGDDMaterial.h"
#include "AGDDModel/AGDDMolecule.h"
#include "AGDDModel/AGDDMixture.h"
#include "AGDDModel/AGDDElement.h"
#include "AGDDModel/AGDDBolt.h"
#include "AGDDModel/AGDDIbeam.h"
#include "AGDDModel/AGDDUbeam.h"

#include "GeoModelKernel/Units.h"
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
#include "GeoModelKernel/GeoTransform.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GeoModelInterfaces/StoredMaterialManager.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

AGDD2GeoModelBuilder::AGDD2GeoModelBuilder() :
  AGDDBuilder(),
  m_mother(nullptr) {
}

GeoElement* AGDD2GeoModelBuilder::CreateElement(const std::string& name)
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
					    double(el->GetZ()),el->GetA()*(GeoModelKernelUnits::gram/GeoModelKernelUnits::mole));
			el->SetElement(g4el);
			return g4el;
		}
	else
		return nullptr;
}
const GeoMaterial* AGDD2GeoModelBuilder::CreateMaterial(const std::string& name)
{

//  give priority to GeoModel's Material Manager in retrieving materials
        const GeoMaterial* mmMaterial=GetMMMaterial(name);
	if (mmMaterial)
	{
		return mmMaterial;
	}

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
					MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
					log<<MSG::WARNING<<"CreateMaterial() - AGDDMaterial is nullptr"<<endmsg;
					return nullptr;
				}
				g4mat=new GeoMaterial(nmat->GetName(),nmat->GetDensity()*(GeoModelKernelUnits::gram/GeoModelKernelUnits::cm3));
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
					MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
					log<<MSG::WARNING<<"CreateMaterial() - AGDDMolecule is nullptr"<<endmsg;
					return nullptr;
				}
				g4mat=new GeoMaterial(nmat->GetName(),nmat->GetDensity()*(GeoModelKernelUnits::gram/GeoModelKernelUnits::cm3));
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
				AGDDMixture* nmat=dynamic_cast<AGDDMixture*>(mat);
				if (!nmat)
				{
					MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
					log<<MSG::WARNING<<"CreateMaterial() - AGDDMixture is nullptr"<<endmsg;
					return nullptr;
				}
				g4mat=new GeoMaterial(nmat->GetName(),nmat->GetDensity()*(GeoModelKernelUnits::gram/GeoModelKernelUnits::cm3));
				for (int i=0;i<nmat->NComponents();i++)
				{
					AGDDSimpleMaterial *el=nmat->Material(i);
					const GeoMaterial* g4el=CreateMaterial(el->GetName());
					g4mat->add(g4el,nmat->Composition(i));
				}
				mat->SetMaterial(g4mat);
				g4mat->lock();
				return g4mat;
			}
			else
			{
				MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
				log<<MSG::WARNING<<"CreateMaterial() - Unknown material type "<<mtype<<endmsg;
				return nullptr;
			}
		}
	else
		return nullptr;
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
	static GeoBox *box1=new GeoBox(1.*GeoModelKernelUnits::km,1*GeoModelKernelUnits::km,1*GeoModelKernelUnits::km);
	GeoTrf::Vector3D v1(0,0,-1*GeoModelKernelUnits::km);
	GeoTrf::Vector3D v2(0,0,+1*GeoModelKernelUnits::km);
	GeoTrf::Transform3D ttt1 = GeoTrf::Transform3D::Identity()*GeoTrf::Translation3D(v1);
	GeoTrf::Transform3D ttt2 = GeoTrf::Transform3D::Identity()*GeoTrf::Translation3D(v2);
	static GeoShape *s1=new GeoShapeShift(box1,ttt1);
	static GeoShape *s2=new GeoShapeShift(box1,ttt2);
	
	double radius=v->Radius();
	GeoTrf::Vector3D axis0(v->GetPoint(0)-v->GetPoint(1));
	GeoTrf::Vector3D axis(v->GetPoint(1)-v->GetPoint(0));
	GeoTrf::Vector3D axis1 = GeoTrf::Vector3D::Identity();
	GeoTrf::Vector3D axis2(v->GetPoint(2)-v->GetPoint(1));
	double length=axis.norm();
	double angle1=0;
	double angle2=std::abs(std::atan2(axis.cross(axis2).norm(), axis.dot(axis2)))/2;
	double delta_l1=0;
	double delta_l2=radius*std::tan(angle2);
	double lengthnew=length+delta_l2;
	GeoShape* solid=new GeoTubs(0.,radius,lengthnew/2.,0.,4*std::asin(1.));
	
	const GeoTrf::Vector3D vt(0.,0.,-lengthnew/2.+delta_l2+2.);
	GeoTrf::Transform3D rrr = GeoTrf::RotateY3D(angle2)*GeoTrf::RotateZ3D(phi(axis2));
	GeoShape *ssnew=new GeoShapeShift(s1,GeoTrf::Translation3D(vt)*rrr);
	
	solid = new GeoShapeSubtraction(solid,ssnew);
	
	GeoTrf::Vector3D vref(0.,0.,-lengthnew/2.);
	GeoTrf::Transform3D tref = GeoTrf::Transform3D::Identity()*GeoTrf::Translation3D(vref);
	solid=new GeoShapeShift(solid,tref);
	GeoTrf::Transform3D r1 = GeoTrf::RotateZ3D(phi(axis0))*GeoTrf::RotateY3D(theta(axis0));
	GeoTrf::Vector3D vtt(v->GetPoint(0).x(),v->GetPoint(0).y(),v->GetPoint(0).z());
	solid=new GeoShapeShift(solid,GeoTrf::Translation3D(vtt)*r1);
	
 	for (int i=1;i<v->NrOfPoints()-1;i++)
	{
		axis0=v->GetPoint(i)-v->GetPoint(i+1);
		axis=v->GetPoint(i+1)-v->GetPoint(i);
		axis1=v->GetPoint(i)-v->GetPoint(i-1);

		length=axis.norm();
		angle1=std::abs(std::atan2(axis.cross(axis1).norm(), axis.dot(axis1)))/2;
		delta_l1=radius*std::tan(angle1);
		delta_l2=0;
		if (i<(v->NrOfPoints()-2)) 
		{
			axis2=v->GetPoint(i+2)-v->GetPoint(i+1);
			angle2=std::abs(std::atan2(axis.cross(axis2).norm(), axis.dot(axis2)))/2;
			delta_l2=radius*std::tan(angle2);
		}
		length=axis.norm();
		lengthnew=length+delta_l1+delta_l2;

		GeoTrf::Vector3D vvref(0.,0.,-lengthnew/2+delta_l1);
		GeoTrf::Transform3D ttref = GeoTrf::Transform3D::Identity()*GeoTrf::Translation3D(vvref);
		
		GeoShape* ss=new GeoTubs(0.,radius,lengthnew/2.,0.,4*std::asin(1.));

		const GeoTrf::Vector3D vt1(0.,0.,+lengthnew/2.-delta_l1-2.);
		const GeoTrf::Vector3D vt2(0.,0.,-lengthnew/2.+delta_l2+2.);
		GeoTrf::Transform3D rrr1 = GeoTrf::RotateZ3D(-phi(axis1))*GeoTrf::RotateY3D(angle1);
		GeoTrf::Transform3D rrr2 = GeoTrf::RotateZ3D(phi(axis2))*GeoTrf::RotateY3D(-angle2);
		GeoTrf::Transform3D ttt1 = rrr1*GeoTrf::Translation3D(vt1);
		GeoTrf::Transform3D ttt2 = rrr2*GeoTrf::Translation3D(vt2);
		GeoShape *ssnew1=new GeoShapeShift(s2,ttt1);
		ss = new GeoShapeSubtraction(ss,ssnew1);
		if (i<(v->NrOfPoints()-2)) 
		{
		  GeoShape *ssnew2=new GeoShapeShift(s1,ttt2);
		  ss = new GeoShapeSubtraction(ss,ssnew2);
		}

		ss=new GeoShapeShift(ss,ttref);
		
		GeoTrf::Transform3D rr1 = GeoTrf::RotateZ3D(phi(axis0))*GeoTrf::RotateY3D(theta(axis0));
		const GeoTrf::Vector3D vv(v->GetPoint(i).x(),v->GetPoint(i).y(),v->GetPoint(i).z());
		ss=new GeoShapeShift(ss,GeoTrf::Translation3D(vv)*rr1);
		solid=new GeoShapeUnion(solid,ss);
	}
	v->SetSolid(solid);
}

void AGDD2GeoModelBuilder::CreateCons(AGDDCons* v)
{
	void *p=v->GetSolid();
	if (!p)
	{
		GeoShape* solid=new GeoCons(v->rin1(),v->rin2(),v->rou1(),v->rou2(),v->z()/2.,v->phi0()*GeoModelKernelUnits::degree,v->dphi()*GeoModelKernelUnits::degree);
		v->SetSolid(solid);
	}
}
void AGDD2GeoModelBuilder::CreateTubs(AGDDTubs* v)
{
	void *p=v->GetSolid();
	if (!p)
	{
		GeoShape* solid=new GeoTubs(v->rin(),v->rou(),v->z()/2.,v->phi0()*GeoModelKernelUnits::degree,v->dphi()*GeoModelKernelUnits::degree);
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
	if (!p)
	{
		int nPoint=v->NrOfPoints();
		GeoSimplePolygonBrep* solid;
		solid = new GeoSimplePolygonBrep(v->GetDz()/2.);
		double area=0;
		v->SetPoint(v->GetPoint(0));
		for (int i=0;i<nPoint;i++)
		{
			int iplus=i+1;
			area+= v->GetPoint(i).x()*v->GetPoint(iplus).y()-v->GetPoint(iplus).x()*v->GetPoint(i).y();
			
		}
		bool clockwise=area<0?true:false;

		TwoPoint pV;
		for (int i=0;i<nPoint;i++)
		{
			if (clockwise) pV=v->GetPoint(nPoint-1-i);
			else pV=v->GetPoint(i);
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
		GeoPcon* solid=new GeoPcon(v->Phi0()*GeoModelKernelUnits::degree,v->Dphi()*GeoModelKernelUnits::degree);
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
		GeoPgon* solid=new GeoPgon(v->Phi0()*GeoModelKernelUnits::degree,v->Dphi()*GeoModelKernelUnits::degree,v->_nbPhi);
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
	static const GeoMaterial *ether=0;
	static GeoShape* fakeVol=0;
	if (ifirst)
	{
		ifirst=0;
        ether = GetMMMaterial("special::Ether");
	 	fakeVol=new GeoTubs(0.,500.,1000.,0.,4*std::asin(1.));
	}

	if (!v->GetVolume())
	{
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
				if (!d) {
					MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
					log<<MSG::WARNING<<"CreateComposition() - Cannot retrieve Detector element for "<<volName<<endmsg;
				}
				p=dynamic_cast<AGDDDetectorPositioner *>(pos);
				if (p) detFullTag=p->ID.detectorAddress;
				else {
					MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
					log<<MSG::WARNING<<"CreateComposition() - AGDDDetectorPositioner is nullptr"<<endmsg;
				}
			}
			GeoTrf::Transform3D trf=pos->Transform();
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
	const GeoMaterial *mat=CreateMaterial(ALIAS(v->GetMaterial()));
	
	void* p=v->GetVolume();
	if (!p)
	{
		GeoShape* sol=(GeoShape*)v->GetSolid();
		if (!sol) {
			MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
			log<<MSG::WARNING<<"CreateVolume() - solid is nullptr!"<<endmsg;
		}
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
  GeoTrf::Transform3D trf = GeoTrf::Transform3D::Identity();
  
  for (it=vs->begin();it!=vs->end();it++)
  {
  	AGDDVolume* vol=(*it).second;
	if (!vol->HasParent())
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
					MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
					log<<MSG::WARNING<<"BuildAllVolumes() - mother not set!"<<endmsg;
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
  GeoTrf::Transform3D trf = GeoTrf::Transform3D::Identity();

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
		   if (!m_mother) {
		       MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
		       log<<MSG::WARNING<<"BuildFromSection() - mother not set!"<<endmsg;
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
	    if (!vol->HasParent())
	    {
		  vol->CreateVolume();
		  AGDDComposition *vv=dynamic_cast<AGDDComposition *>(vol);
		
		  if (vv)
		  {
			GeoPhysVol *vvv=(GeoPhysVol*)(vol->GetVolume());
			if (vvv)
			{
				if (!m_mother) {
					MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
					log<<MSG::WARNING<<"BuildFromSection() - mother not set (no top volume)!"<<endmsg;
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
  else {
    MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
    log<<MSG::WARNING<<"BuildFromSection() - This section is flagged as not to be built!"<<endmsg;
  }
}
void AGDD2GeoModelBuilder::BuildFromVolume(std::string s)
{
    GeoTrf::Transform3D trf = GeoTrf::Transform3D::Identity();
  
    AGDDVolumeStore *vs=AGDDVolumeStore::GetVolumeStore();
	AGDDVolume* vol=vs->GetVolume(s);
	if (!vol) {
		MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
		log<<MSG::WARNING<<"BuildFromVolume() - Volume "<<s<<" not found in the store! Exiting..."<<endmsg;
		return;
	}
	vol->CreateVolume();
		GeoPhysVol *vvv=(GeoPhysVol*)(vol->GetVolume());
		if (vvv)
		{
			if (!m_mother) {
				MsgStream log(Athena::getMessageSvc(),"AGDD2GeoModelBuilder");
				log<<MSG::WARNING<<"BuildFromVolume() - mother not set!"<<endmsg;
				return;
			}
			GeoTransform *gtrf=new GeoTransform(trf);
			m_mother->add(gtrf);
			m_mother->add(vvv);
		}
}

void AGDD2GeoModelBuilder::CreateBolt(AGDDBolt *b)
{
	void *p=b->GetSolid();
	if (!p)
	{
		GeoShape* solid=new GeoTubs(0,b->_diameter/2.,b->_length/2.,0.,360.*GeoModelKernelUnits::degree);
		
		GeoPgon* s=new GeoPgon(0.,360*GeoModelKernelUnits::degree,6);
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

const GeoMaterial* AGDD2GeoModelBuilder::GetMMMaterial(const std::string& name)
{
    StoreGateSvc* pDetStore=nullptr;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StatusCode sc=svcLocator->service("DetectorStore",pDetStore);
    if(sc.isSuccess()) {
        const StoredMaterialManager* theMaterialManager = nullptr;
        sc = pDetStore->retrieve(theMaterialManager, "MATERIALS");
        if(sc.isSuccess()) {
            return theMaterialManager->getMaterial(name);
        }
    }
    return nullptr;
}

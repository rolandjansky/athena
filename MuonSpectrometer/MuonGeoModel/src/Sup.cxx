/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonGeoModel/Sup.h"
#include "MuonGeoModel/SUP_Technology.h"
#include "MuonGeoModel/Station.h"
#include "MuonGeoModel/SupComponent.h"
#include "MuonGeoModel/MYSQL.h"
#include "MuonGeoModel/Cutout.h"
#include "MuonGeoModel/SimpleVolAction.h"
#include "GeoModelKernel/GeoTrd.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"

namespace MuonGM {


Sup::Sup(Component* ss): DetectorElement(ss->name)
{
  SupComponent* s = (SupComponent*)ss;
  width = s->dx1;
  length = s->dy;
  MYSQL* mysql = MYSQL::GetPointer();
  SUP* sup = (SUP*)mysql->GetTechnology(s->name);
  thickness = sup->thickness;
  alThickness = sup->alThickness;
  m_component = s;  // questo 'component' serve!
}


GeoVPhysVol* Sup::build()
{
  std::vector<const GeoShape*> vcutouts;
  int cutoutson = 0;
  return build(cutoutson, vcutouts);
}

GeoVPhysVol* Sup::build(int cutoutson,
			std::vector<const GeoShape*> vcutouts)
{
    MYSQL *mysql=MYSQL::GetPointer();
    SUP *sup=(SUP *)mysql->GetTechnology(name);
    std::cout<<" SUP build method for "<<name<<std::endl;    

    double totxgm = sup->thickness;
    double totygm = width;
    double totzgm = 0.;
    double eps = 0.01*CLHEP::cm;

    double x0=-sup->largeVerClearance+sup->alHorProfileThickness/2.;
    std::cout<<"SUP::x0 for everybody "<<x0<<std::endl;
    
    //    double y0=0.;
    double z0=0.;

    const GeoShape* ssup=NULL;
    GeoPhysVol     *psup=NULL;
    
        
    const GeoMaterial *msup   = matManager->getMaterial("std::Aluminium");
    if(name.substr(3,1)=="3")
    { //SUP3
        std::cout<<" recognised as type 3"<<std::endl;
        totzgm = 2.*sup->alHorFlangeLength+sup->alVerProfileThickness+sup->HorClearance;
        z0     = -sup->alVerProfileThickness-(sup->HorClearance-sup->alVerProfileThickness)/2.;
        const GeoTrd      *suptrd   = new GeoTrd(totxgm/2, totxgm/2, totygm/2, 
                                               totygm/2, totzgm/2);
        std::cout<<"SUP3::z0 "<<z0<<std::endl;
        std::cout<<"SUP3::tot_x_y_z_gm, "<<totxgm<<" "<<totygm<<" "<<totzgm<<std::endl;

        // subtract shapes to describe the most external flangeprofiles
        double xhsiz=sup->alVerFlangeLength/2+eps;
        double yhsiz=width/2+eps;
        double zhsiz=(totzgm
                     - sup->alHorFlangeLength
                     - sup->alVerProfileThickness
                     - sup->alFlangeThickness )/2.+eps;
        const GeoTrd        *cuttop = new GeoTrd(xhsiz, xhsiz, yhsiz, yhsiz, zhsiz);
        std::cout<<"SUP3:: subtracting a box of size "<<xhsiz<<" "<<yhsiz<<" "<<zhsiz<<std::endl;
        double xc = sup->largeVerClearance+sup->smallVerClearance+sup->alFlangeThickness + xhsiz + x0;
        double yc = 0.;
        double zc = sup->alVerProfileThickness+sup->HorClearance+sup->alFlangeThickness + zhsiz + z0;
        // top rigth
        ssup=suptrd;
        ssup = & ( ssup->subtract (  (*cuttop) << HepGeom::Translate3D( xc, yc, zc) ) );
        std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;
        // bottom left
        xc = -xc;
        zc = -zc;
        ssup = & ( ssup->subtract (  (*cuttop)<<HepGeom::Translate3D( xc, yc, zc) ) );
        std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;
        // top left
        zc = zc + sup->HorClearance;
        xc = -xc;
        ssup = & ( ssup->subtract (  (*cuttop)<<HepGeom::Translate3D( xc, yc, zc) ) );
        std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;
        // bottom rigth
        xc = -xc;
        zc = -zc;
        ssup = & ( ssup->subtract (  (*cuttop)<<HepGeom::Translate3D( xc, yc, zc) ) );
        std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;
        //////
        //////
        //////
        //////
         zc = zc + sup->alHorFlangeLength - sup->alFlangeThickness;
         xc = xc + xhsiz + 0.5*sup->alFlangeThickness;
         ssup = & ( ssup->subtract (  (*cuttop)<<HepGeom::Translate3D( xc, yc, zc) ) );
         std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;
         xc = -xc;
         zc = -zc;
         ssup = & ( ssup->subtract (  (*cuttop)<<HepGeom::Translate3D( xc, yc, zc) ) );
         std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;

        
        // subtract a box to get the large internal clearance (DED location)
        xhsiz=sup->largeVerClearance/2+10*eps;
        yhsiz=width/2+10.*eps;
        zhsiz=sup->alHorFlangeLength/2+10.*eps;
        std::cout<<"SUP3:: subtracting a box of size "<<xhsiz<<" "<<yhsiz<<" "<<zhsiz<<std::endl;
        const GeoTrd  *cutbig = new GeoTrd(xhsiz, xhsiz, yhsiz, yhsiz, zhsiz);
        xc = -sup->alHorProfileThickness/2. -2.*eps + xhsiz;
        yc = 0.;
        zc = sup->alVerProfileThickness + (sup->HorClearance-sup->alVerProfileThickness)/2. + zhsiz;
        // big rigth
        ssup = & ( ssup->subtract (  (*cutbig) << HepGeom::Translate3D( xc, yc, zc) ) );
        std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;
        // big left
        xc = -xc;
        zc = -zc;
        ssup = & ( ssup->subtract (  (*cutbig) << HepGeom::Translate3D( -xc, yc, -zc) ) );
        std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;

        // subtract a box to get the small internal clearance (RPC location)
        xhsiz=sup->smallVerClearance/2.;
        yhsiz=width/2+eps;
        zhsiz=sup->alHorFlangeLength/2+sup->HorClearance/2.+eps;
        std::cout<<"SUP3:: subtracting a box of size "<<xhsiz<<" "<<yhsiz<<" "<<zhsiz<<std::endl;
        const GeoTrd  *cutsmall = new GeoTrd(xhsiz, xhsiz, yhsiz, yhsiz, zhsiz);
        // small rigth
        xc = -sup->alHorProfileThickness/2. - xhsiz;
        zc = -(sup->HorClearance-sup->alVerProfileThickness)/2. + zhsiz;
        ssup = & ( ssup->subtract (  (*cutsmall) << HepGeom::Translate3D( -xc, yc, -zc) ) );
        std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;
        // small left
        xc = -xc;
        zc = -zc;
        ssup = & ( ssup->subtract (  (*cutsmall) << HepGeom::Translate3D( -xc, yc, -zc) ) );
        std::cout<<"SUP3:: with centre at "<<xc<<" "<<yc<<" "<<zc<<std::endl;
	if (cutoutson && vcutouts.size()>0)
	  {
	    for (unsigned int i=0; i<vcutouts.size(); i++)
	      {
		const GeoShape* sTemp = &(ssup->subtract(*vcutouts[i]));
		ssup = sTemp;
	      }
	  }

        GeoLogVol         *lsup   = new GeoLogVol("Sup", ssup, msup);
        psup   = new GeoPhysVol(lsup);
    }
    else
    { //SUP1 and SUP2
        std::cout<<" recognised as type 1 or 2"<<std::endl;
        totzgm = sup->alHorFlangeLength+sup->alVerProfileThickness;
        GeoTrd            *suptrd   = new GeoTrd(totxgm/2, totxgm/2, totygm/2, 
                                                 totygm/2, totzgm/2);
        std::cout<<"SUP12::z0 "<<z0<<std::endl;
        std::cout<<"SUP12::tot_x_y_z_gm, "<<totxgm<<" "<<totygm<<" "<<totzgm<<std::endl;
        std::cout<<" big box done"<<std::endl;        
        const GeoTrd*   emptybox =  new GeoTrd(totxgm/2.-sup->alFlangeThickness,
                                               totxgm/2.-sup->alFlangeThickness,
                                               totygm/2+eps, totygm/2+eps,
                                               totzgm/2);
        std::cout<<" empty box done"<<std::endl;
        ssup=suptrd;
        ssup = & ( ssup->subtract (  (*emptybox) << HepGeom::Translate3D( 0., 0., sup->alVerProfileThickness) ) );
        std::cout<<" empty subtracted from big box"<<std::endl;
	if (cutoutson && vcutouts.size()>0)
	  {
	    for (unsigned int i=0; i<vcutouts.size(); i++)
	      {
		const GeoShape* sTemp = &(ssup->subtract(*vcutouts[i]));
		ssup = sTemp;
	      }
	  }

        GeoLogVol         *lsup   = new GeoLogVol("SupModule", ssup, msup);
        psup   = new GeoPhysVol(lsup);
        if(name.substr(3,1)=="2") // SUP2
        {
            std::cout<<" recognised as type 2"<<std::endl;
            // never used (coverity 12157): ssup = & ((*ssup) << HepGeom::RotateY3D( 180.*CLHEP::deg ));
            std::cout<<" in this case:: rotating around y by 180.CLHEP::deg"<<std::endl;
        }
        else std::cout<<" recognised as type 1"<<std::endl;
    }
    return psup;        
}

void Sup::print()
{
	std::cout<<"Sup "<<name<<" :"<<std::endl;
}
} // namespace MuonGM

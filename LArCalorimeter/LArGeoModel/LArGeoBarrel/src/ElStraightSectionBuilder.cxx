/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArGeoBarrel/ElStraightSectionBuilder.h"
#include "BarrelAuxFunctions.h"

#include "LArReadoutGeometry/GeoStraightAccSection.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GeoModelKernel/GeoTrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/MsgStream.h"

#include "GeoGenericFunctions/Sqrt.h"
#include "GeoGenericFunctions/ATan.h"
#include "GeoGenericFunctions/Variable.h"
#include "GeoGenericFunctions/FixedConstant.h"

#define SYSTEM_OF_UNITS Gaudi::Units

StatusCode LArGeo::buildElStraightSections(StoreGateSvc* detStore
					   , IRDBAccessSvc* paramSvc
					   , IMessageSvc* msgSvc
					   , bool sagging)
{
  MsgStream log(msgSvc, "buildElStraightSections"); 
  
  GeoGenfun::Sqrt Sqrt;
  GeoGenfun::ATan ATan;

  IRDBRecordset_ptr barrelGeometryRec  = paramSvc->getRecordsetPtr("BarrelGeometry","");
  IRDBRecordset_ptr barrelSaggingRec   = paramSvc->getRecordsetPtr("BarrelSagging","");
  IRDBRecordset_ptr barrelEtaTransRec  = paramSvc->getRecordsetPtr("BarrelEtaTrans","");
  IRDBRecordset_ptr coldContractionRec = paramSvc->getRecordsetPtr("ColdContraction","");

  double Moth_Z_min = (*barrelGeometryRec)[0]->getDouble("ZMIN")*SYSTEM_OF_UNITS::cm;
  double Moth_Z_max = (*barrelGeometryRec)[0]->getDouble("ZMAX")*SYSTEM_OF_UNITS::cm;
  double Moth_inner_radius = (*barrelGeometryRec)[0]->getDouble("RMIN")*SYSTEM_OF_UNITS::cm;

  // number of zigs for accordion
  int    Nbrt = (*barrelGeometryRec)[0]->getInt("NBRT");  // =14

  // Z coordinates for half barrel in 
  double Bar_Z_min = Moth_Z_min;
  double Bar_Z_max = Moth_Z_max;

  double Bar_Rcmx  = (*barrelGeometryRec)[0]->getDouble("RMINHIGHZ")*SYSTEM_OF_UNITS::cm;

  // Accordion shape parameters:
  // rho phi of curvature centers and delta's
  // IN 2D_transverse LOCAL framework of a layer and symetry axis as X_axis
  // delta's are GEOMETRICAL angles of straight parts w.r. the Y_axis )

  double Rhocen[15], Phicen[15], Delta[15], deltay[15], deltax[15], TetaTrans[15];
  for (int idat=0; idat<15; idat++) {
    std::string strIdat = std::to_string(idat);
    Rhocen[idat] = (*barrelGeometryRec)[0]->getDouble("RHOCEN_"+strIdat)*SYSTEM_OF_UNITS::cm;
    Phicen[idat] = (*barrelGeometryRec)[0]->getDouble("PHICEN_"+strIdat)*SYSTEM_OF_UNITS::deg;
    Delta[idat]  = (*barrelGeometryRec)[0]->getDouble("DELTA_"+strIdat)*SYSTEM_OF_UNITS::deg;
    if(idat == 14) {
      Delta[idat]  = (90.0) * SYSTEM_OF_UNITS::deg;
    }

    // Maximum SAGGING displacement for each of the fifteen folds in Gaudi::Units::mm
    // (should be 0.0, 0.17, 0.30, 0.63, 0.78, 1.06, 1.09, 1.21, 1.07, 1.03, 0.74, 0.61, 0.27, 0.20, 0.0)
    //GUtmp sagging amplied by 10
    if (sagging)  {
      deltay[idat] = (*barrelSaggingRec)[0]->getDouble("SAG_"+strIdat)*SYSTEM_OF_UNITS::cm;
      deltax[idat] = (*barrelSaggingRec)[0]->getDouble("SAG_"+strIdat+"_X")*SYSTEM_OF_UNITS::cm;
    }
    else {
      deltay[idat]=0.;
      deltax[idat]=0.;
    }
  
    // eta of lead transition
    double etaTrans = (*barrelEtaTransRec)[idat]->getDouble("ETATRANS");
    TetaTrans[idat] = 2.*atan(exp(-etaTrans));
  }

  // parity of accordion waves
  int checkParity = (Phicen[0]<0. ? 1 : 0);   // 0 for case where first absorber wave goes up
                                              // 1 for case where first absorber wave goes down

  int Ncell      = (*barrelGeometryRec)[0]->getInt("NCELMX");
  int Nabsorber  = (Ncell==64)  ? Ncell + 1 : Ncell;
  int Nelectrode = Ncell;

  // z of end of G10 ring
  double z1   = (*barrelGeometryRec)[0]->getDouble("G10FRONTDELTAZ")*SYSTEM_OF_UNITS::cm + Moth_Z_min;
  // radius of end of G10 ring
  const double r1    = Moth_inner_radius
    + (*barrelGeometryRec)[0]->getDouble("XEL1")*SYSTEM_OF_UNITS::cm
    + (*barrelGeometryRec)[0]->getDouble("XG10")*SYSTEM_OF_UNITS::cm;
  // minimum radius at end of volume
  const double r2    =  (*barrelGeometryRec)[0]->getDouble("RMINHIGHZ")*SYSTEM_OF_UNITS::cm;
  const double inv_r2_r1 = 1. / (r2-r1);
  
  {
    // Construct the straight and the corner parts of the Accordion plates
    double Thgl       = (*barrelGeometryRec)[0]->getDouble("THGL")*SYSTEM_OF_UNITS::cm;
    double Thfe       = (*barrelGeometryRec)[0]->getDouble("THFE")*SYSTEM_OF_UNITS::cm;
    double Thpb       = (*barrelGeometryRec)[0]->getDouble("THPB")*SYSTEM_OF_UNITS::cm;
    double Thcu       = (*barrelGeometryRec)[0]->getDouble("THCU")*SYSTEM_OF_UNITS::cm;
    double Thfg       = (*barrelGeometryRec)[0]->getDouble("THFG")*SYSTEM_OF_UNITS::cm;

    double Contract   = (*coldContractionRec)[0]->getDouble("ABSORBERCONTRACTION");

    double Thce = (Thpb+Thgl+Thfe)*Contract;
    double Thel = Thcu+Thfg;
   
    double Zmin = Bar_Z_min;
    double Zmax = Bar_Z_max;
    double /*Zcp2[15],*/Along[14];

    double Zcp1l[14],Zcp1h[14],Zcp2l[14],Zcp2h[14];
    double Rhol[14],Rhoh[14];

    double safety_along = 0.007*SYSTEM_OF_UNITS::mm;

    // Compute centers of curvature coordinates in a local frame.
    double Cenx[15]={0}, Ceny[15]={0} ;
    for (int jf=0; jf<(Nbrt+1); jf++) {
	Cenx[jf] = Rhocen[jf]*cos(Phicen[jf]); // Phicen[] already in radians
	Ceny[jf] = Rhocen[jf]*sin(Phicen[jf]);
    }

    // Compute staight lengths of folds
    double Rint = (*barrelGeometryRec)[0]->getDouble("RINT")*SYSTEM_OF_UNITS::cm;
    for (int jl=0; jl<Nbrt; jl++) {
      double Adisc2 = (Cenx[jl+1]-Cenx[jl])*(Cenx[jl+1]-Cenx[jl])+
                      (Ceny[jl+1]-Ceny[jl])*(Ceny[jl+1]-Ceny[jl]);
      double Along2 = Adisc2 - 4.*Rint*Rint; 
      Along[jl] = sqrt(Along2);

      double ddelta = M_PI/2.-Delta[jl];
      // different parity depending on direction of first wave
      if (checkParity==0) {
        if (jl%2==1) {
	  ddelta=-1.*ddelta;
	}
      } else {
        if (jl%2==0) {
	  ddelta=-1.*ddelta;
	}
      }
      double xlong=Along[jl]-2.*safety_along;
      double x2=0.5*(Cenx[jl+1]+Cenx[jl])+0.5*xlong*cos(ddelta);
      double y2=0.5*(Ceny[jl+1]+Ceny[jl])+0.5*xlong*sin(ddelta);
      double x1=0.5*(Cenx[jl+1]+Cenx[jl])-0.5*xlong*cos(ddelta);
      double y1=0.5*(Ceny[jl+1]+Ceny[jl])-0.5*xlong*sin(ddelta);
      Rhol[jl] = sqrt(x1*x1+y1*y1);
      Rhoh[jl] = sqrt(x2*x2+y2*y2);
      Zcp1l[jl] = Rhol[jl]/tan(TetaTrans[jl]);
      Zcp1h[jl] = Rhoh[jl]/tan(TetaTrans[jl+1]);
      if (Rhol[jl] < r2) {
	Zcp2l[jl] = z1 + (Rhol[jl]-r1)*inv_r2_r1*(Moth_Z_max-z1);
      }
      else {
	Zcp2l[jl]=Moth_Z_max;
      }
      if (Rhoh[jl] < r2) {
	Zcp2h[jl] = z1 + (Rhoh[jl]-r1)*inv_r2_r1*(Moth_Z_max-z1);
      }
      else {    
	Zcp2h[jl]=Moth_Z_max;
      }
    }
   
    double Psi        = (*barrelGeometryRec)[0]->getDouble("ALFA")*SYSTEM_OF_UNITS::deg;   // 360.*Gaudi::Units::deg/1024
    double Gama0      = (*barrelGeometryRec)[0]->getDouble("PHIFIRST");
    double Alfa = Psi;                        // size of one phi cell   

    GeoGenfun::Variable icopy;
    GeoGenfun::GENFUNCTION Game = Gama0 + Psi/2 + Alfa*icopy;
    GeoGenfun::GENFUNCTION Gama = Gama0 + Alfa*icopy;

    GeoStraightAccSection* gStraightElectrodes{nullptr};
    GeoStraightAccSection* gStraightAbsorbers{nullptr};
    //
    // Loop through the straight and corner(Fold) parts of the Accordion plates
    // Repeat each part around Phi, then move to the next, towards outer radii
    //

    // GU 09/06/2004 add some safety in z size
    double safety_zlen=0.050*SYSTEM_OF_UNITS::mm;
   
    for(int irl=0; irl<Nbrt; irl++) {  // loop over zig-zag in radius
      int iparit;
      // different parity depending on direction of first wave
      if (checkParity==0) {       
	iparit= (1-2*(irl%2));  // +1 for irl=0,2,4,..   -1 for irl=1,3,5,..
      } 
      else {
	iparit=(2*(irl%2)-1);  // -1 for irl=0,2,3...  +1 for irl=1,3,5
      }
      
      // special case if Rhocen[il] < Bar_Rcmx < Rhocen[il+1]
      // we had to divide the fold in two different pieces to deal with the shape
      int ndivi=1;
      if (Rhocen[irl] < Bar_Rcmx && Rhocen[irl+1] > Bar_Rcmx) {
	ndivi=2;
      } 

      for (int idivi=0;idivi<ndivi;idivi++) {

	// lenght in z at beginning and end of straigth part
	double bl1,tl1;
	double frac;
	if (ndivi==1) {
	  bl1 = (std::min(Zcp2l[irl],Zmax)-Zmin)/2.;
	  tl1 = (std::min(Zcp2h[irl],Zmax)-Zmin)/2.;
	  frac=1.;
	}
	else {
	  if (idivi==0) {
	    bl1 = (std::min(Zcp2l[irl],Zmax)-Zmin)/2.;
	    tl1 = (Zmax-Zmin)/2.;
	    frac=(Bar_Rcmx-Rhol[irl])/(Rhoh[irl]-Rhol[irl]);
	  }
	  else {
	    bl1 = (Zmax-Zmin)/2.;
	    tl1 = (Zmax-Zmin)/2.;
	    frac=(Rhoh[irl]-Bar_Rcmx)/(Rhoh[irl]-Rhol[irl]);
	  }
	}
	//GU 09/06/2004 add small safety in size tl1 and bl1
	tl1=tl1-safety_zlen;
	bl1=bl1-safety_zlen;

	//    =================================== Absorbers
	{
	  // thickness of absorber
	  double Dz = Thce/2.;
	   
	  // For absorbers
	  GeoGenfun::GENFUNCTION x1a = LArGeo::Fx(irl+0., Gama, Cenx, Ceny)
	    +deltay[irl]*LArGeo::Del1(Gama)
	    +deltax[irl]*LArGeo::Del2(Gama);
	  GeoGenfun::GENFUNCTION x2a = LArGeo::Fx(irl+1., Gama, Cenx, Ceny)
	    +deltay[irl+1]*LArGeo::Del1(Gama)
	    +deltax[irl+1]*LArGeo::Del2(Gama);
	  GeoGenfun::GENFUNCTION y1a = LArGeo::Fy(irl+0., Gama, Cenx, Ceny)
	    -deltay[irl]*LArGeo::Del2(Gama)
	    +deltax[irl]*LArGeo::Del1(Gama);
	  GeoGenfun::GENFUNCTION y2a = LArGeo::Fy(irl+1., Gama, Cenx, Ceny)
	    -deltay[irl+1]*LArGeo::Del2(Gama)
	    +deltax[irl+1]*LArGeo::Del1(Gama);
	  GeoGenfun::GENFUNCTION dx = x2a - x1a;
	  GeoGenfun::GENFUNCTION dy = y2a - y1a;
	   
	  // Da the two fold centers distance, da straight part length
	   
	  GeoGenfun::GENFUNCTION Da = Sqrt ( dx*dx + dy*dy );
	  GeoGenfun::GENFUNCTION da = Sqrt ( (Da - 2.*Rint)*(Da + 2.*Rint) );
	   
	  // newalpha (slant angle) value of the rotation angle around Z_axis
	  GeoGenfun::GENFUNCTION cosalfa = (da*dx -iparit*2.*Rint*dy)/Da/Da;
	  GeoGenfun::GENFUNCTION sinalfa = (da*dy +iparit*2.*Rint*dx)/Da/Da;
	  GeoGenfun::GENFUNCTION newalpha = LArGeo::ATan2(sinalfa,cosalfa);       
	  
	  GeoGenfun::GENFUNCTION h1 = da/2. * frac  - .007*SYSTEM_OF_UNITS::mm;
	   
	  // thick absorber pieces
	  // more correct computation with z lenght computed exactly at the
	  // radius of the end and the beginning of the straight sections
	  double Xb1,Xt1;
	  if (ndivi==1) {
	    Xb1 = (Zcp1l[irl]-Zmin)/2.;
	    Xt1 = (Zcp1h[irl]-Zmin)/2.;
	  } 
	  else {
	    double xfrac=(Bar_Rcmx-Rhol[irl])/(Rhoh[irl]-Rhol[irl]);
	    if (idivi==0) {
	      Xb1 = (Zcp1l[irl]-Zmin)/2.;
	      Xt1 = (xfrac*Zcp1h[irl]+(1.-xfrac)*Zcp1l[irl]-Zmin)/2.;
	    }
	    else {
	      Xb1 = (xfrac*Zcp1h[irl]+(1.-xfrac)*Zcp1l[irl]-Zmin)/2.;
	      Xt1 = (Zcp1h[irl]-Zmin)/2.;
	    } 
	  }

	  // lengths that remain to be covered with the thin absorber
	  double Xt2 = tl1-Xt1;
	  double Xb2 = bl1-Xb1;
        
	  Xt2 = Xt2 -0.007*SYSTEM_OF_UNITS::mm;
	  Xb2 = Xb2 -0.007*SYSTEM_OF_UNITS::mm;
           
	   
	  GeoGenfun::GENFUNCTION alpha = ATan(0.5*(bl1-tl1)/h1);

	  // angle that would be needed for trap do describe only thin absorber
	  //     ------------------|---------X---------|
	  //     <------------------------------------->  2*tl1
	  //                       <--------->   Xt2
	  //
	  //     <--------------------------------->  2*bl1
	  //                    <-------->  Xb2 
	  //     ---------------|--------X---------|
	  // alpha = (-) angle between X's
	  //   tan(alpha) = delta X size / width,   deltaX size = 2*tl1-Xt2-(2*bl1-Xb2),  width = 2.*h1  
	   
	  // .newalpha is already computed angle wrt z axis
	  // P/2 rotation is to get absorber aligned along local x axis
	  // instead of y, then rotate with angle newalpha
	  GeoGenfun::GENFUNCTION alfrot =  -M_PI/2. - newalpha;
	  
	  GeoGenfun::GENFUNCTION Xcd    = (x1a + x2a)/2. + (2.*idivi-1.)*(1.-frac)*da/2.*cosalfa;
	  GeoGenfun::GENFUNCTION Ycd    = (y1a + y2a)/2. + (2.*idivi-1.)*(1.-frac)*da/2.*sinalfa;
	  GeoGenfun::GENFUNCTION Zcd    = GeoGenfun::FixedConstant(Zmin+(tl1+bl1)/2.+safety_zlen);
	  
	  GeoXF::TRANSFUNCTION TX = 
	    GeoXF::Pow(GeoTrf::TranslateX3D(1.0),Xcd) *
	    GeoXF::Pow(GeoTrf::TranslateY3D(1.0),Ycd) *
	    GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),Zcd) * 
	    GeoXF::Pow(GeoTrf::RotateZ3D(1.0),-alfrot)*
	    GeoTrf::RotateY3D(-90*SYSTEM_OF_UNITS::deg);                    
	  
	  for (int instance = 0; instance < Nabsorber; instance++) {
		
	    GeoTrap* thinTrap = new GeoTrap(Dz,0.,0.,h1(instance),tl1,bl1,alpha(instance),
					    h1(instance),tl1,bl1,alpha(instance));
	    thinTrap->ref();	    
	    if (sagging) {
	      if (!gStraightAbsorbers) {
		gStraightAbsorbers = new GeoStraightAccSection();
	      }
	      gStraightAbsorbers->XCent(instance,irl)=TX(instance)(0,3); //dx
	      gStraightAbsorbers->YCent(instance,irl)=TX(instance)(1,3); //dy
	      gStraightAbsorbers->Cosu(instance,irl)    =-(TX(instance)(0,1)); //xy
	      gStraightAbsorbers->Sinu(instance,irl)    = (TX(instance)(0,2)); //xz
	      gStraightAbsorbers->HalfLength(instance,irl) = thinTrap->getDydzn();
	      thinTrap->unref();	    
	    }
	    else {
	      if (!gStraightAbsorbers) {
		gStraightAbsorbers = new GeoStraightAccSection();
	      }
	      gStraightAbsorbers->setTransform (irl,TX);
	      gStraightAbsorbers->setHalfLength(irl,thinTrap->getDydzn());
	      thinTrap->unref();
	      break;
	    }
	  }    // loop over instances
	}   // end absorbers

	// ========================  Electrodes:
	{
	  // thickness of electrode
	  double Dze = Thel/2.;
	  
	  // For electrodes
	  GeoGenfun::GENFUNCTION x1e = LArGeo::Fx(irl+0., Game, Cenx, Ceny)
	    +deltay[irl]*LArGeo::Del1(Game)
	    +deltax[irl]*LArGeo::Del2(Game);
	  GeoGenfun::GENFUNCTION x2e = LArGeo::Fx(irl+1., Game, Cenx, Ceny)
	    +deltay[irl+1]*LArGeo::Del1(Game)
	    +deltax[irl+1]*LArGeo::Del2(Game);
	  GeoGenfun::GENFUNCTION y1e = LArGeo::Fy(irl+0., Game, Cenx, Ceny) 
	    -deltay[irl]*LArGeo::Del2(Game)
	    +deltax[irl]*LArGeo::Del1(Game);
	  GeoGenfun::GENFUNCTION y2e = LArGeo::Fy(irl+1., Game, Cenx, Ceny)
	    -deltay[irl+1]*LArGeo::Del2(Game)
	    +deltax[irl+1]*LArGeo::Del1(Game);
	  GeoGenfun::GENFUNCTION dxe = x2e - x1e;
	  GeoGenfun::GENFUNCTION dye = y2e - y1e;
	  // De the two fold centers distance, de straight part length
	  GeoGenfun::GENFUNCTION De = Sqrt ( dxe*dxe + dye*dye );
	  GeoGenfun::GENFUNCTION de = Sqrt ( (De - 2.*Rint)*(De + 2.*Rint) );
	  
	  //newalphe (slant angle) value of the rotation angle around Z_axis
	  GeoGenfun::GENFUNCTION cosalfae = (de*dxe -iparit*2.*Rint*dye)/De/De;
	  GeoGenfun::GENFUNCTION sinalfae = (de*dye +iparit*2.*Rint*dxe)/De/De;
	  GeoGenfun::GENFUNCTION newalphe = LArGeo::ATan2(sinalfae,cosalfae);
	  
	  // newalphae is already computed angle wrt z axis
	  // P/2 rotation is to get absorber aligned along local x axis
	  // instead of y, then rotate with angle newalpha
	  GeoGenfun::GENFUNCTION alfrote = -M_PI/2. - newalphe;
	   
	  GeoGenfun::GENFUNCTION Xcde    = (x1e + x2e)/2.+ (2.*idivi-1.)*(1.-frac)*de/2.*cosalfae;
	  GeoGenfun::GENFUNCTION Ycde    = (y1e + y2e)/2.+ (2.*idivi-1.)*(1.-frac)*de/2.*sinalfae;
	  GeoGenfun::GENFUNCTION Zcde       = GeoGenfun::FixedConstant(Zmin+(tl1+bl1)/2.+safety_zlen);
	  
	  GeoGenfun::GENFUNCTION h1e      = de/2.*frac - .007*SYSTEM_OF_UNITS::mm;
	  GeoGenfun::GENFUNCTION alpha_e  = ATan(0.5*(bl1-tl1)/h1e); 
	  
	  GeoXF::TRANSFUNCTION TXE = 
	    GeoXF::Pow(GeoTrf::TranslateX3D(1.0),Xcde) *
	    GeoXF::Pow(GeoTrf::TranslateY3D(1.0),Ycde) *
	    GeoXF::Pow(GeoTrf::TranslateZ3D(1.0),Zcde) * 
	    GeoXF::Pow(GeoTrf::RotateZ3D(1.0),-alfrote)*
	    GeoTrf::RotateY3D(-90*SYSTEM_OF_UNITS::deg);                    
	  
	  for (int instance = 0; instance < Nelectrode; instance++) {

	    GeoTrap* trap = new GeoTrap(Dze,0.,0.,h1e(instance),tl1,bl1,alpha_e(instance),
					h1e(instance),tl1,bl1,alpha_e(instance));
	    trap->ref();
	    if (sagging) {
	      if (!gStraightElectrodes) {
		gStraightElectrodes = new GeoStraightAccSection();
	      }
	      gStraightElectrodes->XCent(instance,irl)=TXE(instance)(0,3); //dx
	      gStraightElectrodes->YCent(instance,irl)=TXE(instance)(1,3); //dy
	      gStraightElectrodes->Cosu(instance,irl)    =-(TXE(instance)(0,1)); //xy
	      gStraightElectrodes->Sinu(instance,irl)    = (TXE(instance)(0,2)); //xz
	      gStraightElectrodes->HalfLength(instance,irl) = trap->getDydzn();
	      trap->unref();
	    }
	    else {
	      if (!gStraightElectrodes) {
		gStraightElectrodes = new GeoStraightAccSection();
	      }
	      gStraightElectrodes->setTransform (irl,TXE);
	      gStraightElectrodes->setHalfLength(irl,trap->getDydzn());
	      trap->unref();
	      break;
	    }
	  }   // loop over instances
         }        // end loop over ndivi
      }         //  end loop over irl (zig-zag)
    }

    if(!detStore->record(gStraightElectrodes,"STRAIGHTELECTRODES").isSuccess()) {
      log << MSG::FATAL << "Cannot store STRAIGHTELECTRODES structure" << endmsg;
      return StatusCode::FAILURE;
    }

    if(!detStore->record(gStraightAbsorbers,"STRAIGHTABSORBERS").isSuccess()) {
      log << MSG::FATAL << "Cannot store STRAIGHTABSORBERS structure" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

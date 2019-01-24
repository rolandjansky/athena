/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArReadoutGeometry/EMBAccordionDetails.h"
#include "LArReadoutGeometry/GeoStraightAccSection.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "StoreGate/StoreGateSvc.h"
#include <cmath>
class EMBAccordionDetails::Clockwork {

public:
  /**
   * @brief  compute number (0 to 1023) of closest electrode according to nominal accordion geometry
   **/
  int phiGap(double radius, double xhit, double yhit);

  /**
   * @brief initialization routine
   **/
  void getRPhi();

  /**
   * @brief phi of first absorber as function of radius for nominal accordion geometry (before sagging).
   **/
  double phi0(double radius) const;

  /**
   * @brief Compute the distance to the electrode
   **/
  double Distance_Ele(const double &x, const double &y,
                      const int &PhiC, int &Num_Straight, const int &Num_Coude,
                      double &xl);
  /**
   * @brief Compute the distance to the absorber
   **/
  double Distance_Abs(const double &x, const double &y, 
                     const int &nabs, const int &Num_Straight, const int &Num_Coude);

  double gam0;
  double Rmin;
  double Rmax;
  double dR;
  double Rphi[5000];
  int    NRphi;
  int    Nbrt;
  int    Nbrt1;
  double rint_eleFib ; //2.78
  double delta[15];    // zig-zag angles
  double rc[15] ;      // R and
  double phic[15] ;    // phi positions of center of fold for first absorber 
  double xc[15];     // corresponding x,y values
  double yc[15];
  double rMinAccordion;
  double rMaxAccordion;
  double zMinBarrel;
  double zMaxBarrel;
  double etaMaxBarrel;
  int    NCellTot;    // either 64 or 1024 for TestBeam or Atlas
  int    NCellMax;    // 1024 

  const GeoStraightAccSection *electrodeStraightSection;
  const GeoStraightAccSection *absorberStraightSection;

};

void EMBAccordionDetails::Clockwork::getRPhi()
{
  const double dl=0.001;
  const double inv_dl = 1 / dl;
  double cenx[15],ceny[15];
  double xl,xl2;
  double sum1[5000],sumx[5000];
  xl=0;
  xl2=0.;
  NRphi=5000;
  Rmin=1500.;
  dR=0.10;
  Rmax=0.;

  const double rint= rint_eleFib;
  const double inv_rint = 1. / rint;
  const double dt=dl * inv_rint;
  const double inv_dt = 1. / dt;

  for (int i=0;i<NRphi;i++) {
     sum1[i]=0.;
     sumx[i]=0.;
   }
  for (int i=0;i<15;i++) {
     cenx[i]=rc[i]*cos(phic[i]);
     ceny[i]=rc[i]*sin(phic[i]);
  }

  for (int i=0; i<15; i++) {
     
// fold
     double phi0,phi1;
     if (i==0) {
        phi0=-M_PI/2.;
        phi1=-M_PI/2.+delta[0];
     }
     else if (i==14) {
        phi0=-M_PI+delta[13];
        phi1=-M_PI/2.;
     }
     else {
        if (i%2==1) {
           phi0=delta[i];
           phi1=M_PI-delta[i-1];
        }
        else {
           phi0=-M_PI+delta[i-1];
           phi1=-delta[i];
        }
     }
     xl2+=rint*fabs(phi1-phi0);
     int nstep=int((phi1-phi0) * inv_dt)+1;
//     std::cout << "fold " << phi0 << " " << phi1 << " " << nstep << std::endl;
     for (int ii=0;ii<nstep;ii++) {
        xl+=dl;
        double phi=phi0+dt*((double)ii);
        double x=cenx[i]+rint*cos(phi);
        double y=ceny[i]+rint*sin(phi);
        double radius=sqrt(x*x+y*y);
        if (radius>Rmax) Rmax=radius;
        double phid=atan(y/x);
        int ir=((int) ((radius-Rmin)/dR) );
        if (ir>=0 && ir < NRphi) {
           sum1[ir]+=1.;
           sumx[ir]+=phid;
        }
     }

// straight section
     if (i<14) {
        double dx=cenx[i+1]-cenx[i];
        double dy=ceny[i+1]-ceny[i];
        double along=dx*dx+dy*dy-4.*rint*rint;
        along=sqrt(along);
        double x0=0.5*(cenx[i+1]+cenx[i]);
        double y0=0.5*(ceny[i+1]+ceny[i]);
        double phi;
        if (i%2==0) phi=M_PI/2-delta[i];
        else        phi=-M_PI/2.+delta[i];
        double x1=x0-0.5*along*cos(phi);
        double y1=y0-0.5*along*sin(phi);
        xl2+=along;
        int nstep=int(along * inv_dl)+1;
//        std::cout << "straight" << x0 << " " << y0 << along << " " << nstep << std::endl;
        for (int ii=0;ii<nstep;ii++) {
           xl+=dl;
           double x=x1+dl*((double)ii)*cos(phi);
           double y=y1+dl*((double)ii)*sin(phi);
           double radius=sqrt(x*x+y*y);
           if (radius>Rmax) Rmax=radius;
           double phid=atan(y/x);
           int ir=((int) ((radius-Rmin)/dR) );
           if (ir>=0 && ir < NRphi) {
              sum1[ir]+=1.;
              sumx[ir]+=phid;
           }
        }
     }
  }
//  std::cout << "total electrode lenght " << xl << " " << xl2 << std::endl;
//  std::cout << "rmax in accordion " << Rmax << std::endl;
  for (int i=0; i<NRphi; i++) {
    if (sum1[i]>0) {
     Rphi[i]=sumx[i]/sum1[i];
     // Not used: double radius = Rmin + ((double(i))+0.5)*dR;
//     std::cout << " r,phi0 " << radius << " " << Rphi[i] << std::endl;
    } 
    else Rphi[i]=0.;
  }
}


// ======================================================================================
// compute number (0 to 1023) of closest electrode according to nominal
// accordion geometry
int EMBAccordionDetails::Clockwork::phiGap(double radius, double xhit, const double yhit)
{
  const double m2pi = 2.0*Gaudi::Units::pi;
  double phi_0=phi0(radius)+gam0;   // from -M_PI to M_PI
  double phi_hit=atan2(yhit,xhit);  // from -M_PI to M_PI
  double dphi=phi_hit-phi_0;
// bring back to 0-2pi
  if (dphi<0) dphi=dphi+m2pi;
  if (dphi>=m2pi) dphi=dphi-m2pi;
  dphi=dphi*(1024/m2pi);
  int ngap=((int) dphi);
  return ngap;
}



EMBAccordionDetails::EMBAccordionDetails():m_c(new Clockwork()) {
  // Access the GeoModelSvc:
  IGeoModelSvc *geoModel;
  StatusCode status;

  ISvcLocator* svcLocator = Gaudi::svcLocator(); 
  status = svcLocator->service ("GeoModelSvc",geoModel);
  if (status != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate GeoModelSvc!!");
  }
  
  // Access the geometry database:
  IRDBAccessSvc *pAccessSvc;
  status=svcLocator->service("RDBAccessSvc",pAccessSvc);
  if (status != StatusCode::SUCCESS) {
    throw std::runtime_error ("Cannot locate RDBAccessSvc!!");
  }
  
  // Obtain the geometry version information:
  
  std::string AtlasVersion = geoModel->atlasVersion();
  std::string LArVersion = geoModel->LAr_VersionOverride();
  
  std::string detectorKey  = LArVersion.empty() ? AtlasVersion : LArVersion;
  std::string detectorNode = LArVersion.empty() ? "ATLAS" : "LAr";
  
  IRDBRecordset_ptr barrelGeometry = pAccessSvc->getRecordsetPtr("BarrelGeometry",detectorKey,detectorNode);
  if (barrelGeometry->size()==0) {
    throw std::runtime_error("Cannot find the BarrelGeometry Table");
  }
  
  IRDBRecordset_ptr barrelLongDiv = pAccessSvc->getRecordsetPtr("BarrelLongDiv",detectorKey,detectorNode);
  if (barrelLongDiv->size()==0) {
    throw std::runtime_error("Cannot find the BarrelLongDiv Table");
  }
  
  // number of straight sections (should be 14)
  m_c->Nbrt = (*barrelGeometry)[0]->getInt("NBRT");
  // Number of ZIGs + 1  i.e.  15 = number of folds
  m_c->Nbrt1 = m_c->Nbrt + 1;
  // phi of first absorber
  m_c->gam0 = (*barrelGeometry)[0]->getDouble("PHIFIRST");
  // radius of curvature of neutral fiber in the folds
  m_c->rint_eleFib = (*barrelGeometry)[0]->getDouble("RINT")*Gaudi::Units::cm;
  
  // r,phi positions of the centre of the folds (nominal geometry)
  for (int idat = 0; idat < m_c->Nbrt1 ; idat++) 
    {
      m_c->rc[idat]   = (*barrelGeometry)[0]->getDouble("RHOCEN",idat)*Gaudi::Units::cm; 
      m_c->phic[idat] = (*barrelGeometry)[0]->getDouble("PHICEN",idat)*Gaudi::Units::deg; 
      m_c->delta[idat] = (*barrelGeometry)[0]->getDouble("DELTA",idat)*Gaudi::Units::deg; 
      m_c->xc[idat] = m_c->rc[idat]*cos(m_c->phic[idat]);
      m_c->yc[idat] = m_c->rc[idat]*sin(m_c->phic[idat]);
    }
  //
  m_c->rMinAccordion  =   (*barrelGeometry)[0]->getDouble("RIN_AC")*Gaudi::Units::cm;  
  m_c->rMaxAccordion  =   (*barrelGeometry)[0]->getDouble("ROUT_AC")*Gaudi::Units::cm;  
  m_c->etaMaxBarrel   =   (*barrelGeometry)[0]->getDouble("ETACUT");      
  m_c->zMinBarrel     =   (*barrelLongDiv)[0]->getDouble("ZMAXACT")*Gaudi::Units::cm;    
  m_c->zMaxBarrel     =   (*barrelLongDiv)[0]->getDouble("ZMINACT")*Gaudi::Units::cm;    
  // === GU 11/06/2003   total number of cells in phi
  // to distinguish 1 module (testbeam case) from full Atlas
  m_c->NCellTot =         (*barrelGeometry)[0]->getInt("NCELMX"); 
  m_c->NCellMax=1024;

  //====================================================

  m_c->getRPhi();

}

EMBAccordionDetails::~EMBAccordionDetails() {
  delete m_c;
}

double EMBAccordionDetails::phiFirstAbsorber() const {
  return m_c->gam0;
}


double EMBAccordionDetails::Clockwork::phi0(double radius) const 
{
 int ir;
 if (radius < Rmin) ir=0;
 else {
   if (radius > Rmax) radius=Rmax-0.0001;
   ir=((int) ((radius-Rmin)/dR) );
 }
 return Rphi[ir];
}


const GeoStraightAccSection *EMBAccordionDetails::getAbsorberSections() const {
  if (m_c->absorberStraightSection==NULL) {
    StatusCode status;
    ISvcLocator* svcLocator = Gaudi::svcLocator(); 
    StoreGateSvc *detStore;

    const DataHandle<GeoStraightAccSection> sa(NULL);

    status = svcLocator->service ("DetectorStore",detStore);
    if (status != StatusCode::SUCCESS) throw std::runtime_error ("Cannot locate Storegate");

    
    status = detStore->retrieve(sa,"STRAIGHTABSORBERS");
    if (status != StatusCode::SUCCESS) throw std::runtime_error ("Cannot locate Straight Absorbers");
    
    m_c->absorberStraightSection = sa;

  }
  return m_c->absorberStraightSection;
  
}
  

const GeoStraightAccSection *EMBAccordionDetails::getElectrodeSections() const {
  if (m_c->electrodeStraightSection==NULL) {
    StatusCode status;
    ISvcLocator* svcLocator = Gaudi::svcLocator(); 
    StoreGateSvc *detStore;

    const DataHandle<GeoStraightAccSection> sa(NULL);

    status = svcLocator->service ("DetectorStore",detStore);
    if (status != StatusCode::SUCCESS) throw std::runtime_error ("Cannot locate Storegate");

    status = detStore->retrieve(sa,"STRAIGHTELECTRODES");
    if (status != StatusCode::SUCCESS) throw std::runtime_error ("Cannot locate Straight Electrodes");
    
    m_c->electrodeStraightSection=sa;

  }
  return m_c->electrodeStraightSection;
  
}



//======================================================================================
//
// Here INTRINSIC Distance_to_electrode determination (G.P.)
//
// This retuns an ALGEBRICDistEle value, the distance from electrode
//neutral fiber TOWARDS the Sub_Step in LAr (measured on a local perpendicular 
//vector unit oriented upwards i.e. following increasing Phi values).  
//
// This is done in THE INTRINSIC LOCAL Z > 0. half_barrel part ("stac_phys1") 
//
// inputs: xhit,yhit = x,y positions in local half barrel
//         PhiCell      = electrode number in phi (0 to 1023 for Atlas case)
//         Num_Straight = number (0 to 13) of the straight section
//         Num_Coude    = number (0 to 14) of closest fold
//
// output:  Function value = algebric distance to electrode
//          xl = normalized lenght along electrode straight section (between +-1)

#ifdef READY
double EMBAccordionDetails::Clockwork::Distance_Ele(const double & xhit, 
                                                    const double &yhit, const int &PhiCell, int &Num_Straight,
                                                    const int &Num_Coude, double &xl)
{
  double dx, dy, dr;
  double DistEle = 0.;
//
// FrameWork is consistent with the one used to PhiCell determination
// e.g. it assumes HERE to be the LOCAL one of "stac_phys1",
// (mother of ACCordion volumes) from which Z> 0. and Z < 0. half_barrel 
// parts are then defined.
//
// One needs POINTERS to Electrode neutral fibers
// either for straight parts or for folds
//
// Fold Center ccoordinates
    double Xc[2];
    Xc[0] = m_coudeelec->XCentCoude(Num_Coude, PhiCell);
    Xc[1] = m_coudeelec->YCentCoude(Num_Coude, PhiCell);
    double radfold = sqrt(Xc[0]*Xc[0]+Xc[1]*Xc[1]);
    double radhit = sqrt(xhit*xhit+yhit*yhit);

// check if the assumed straight number is the correct one
//   (this can be wrong when we are close to a fold and there is sagging)
  if (Num_Coude == Num_Straight && radhit <radfold) {
     if (Num_Straight>0) Num_Straight = Num_Straight-1;
//     std::cout << "radhit,radfold " << radhit << " " << radfold << " change straight by +1" << std::endl;
  }
  if (Num_Coude == (Num_Straight+1) && radhit > radfold) {
     if (Num_Straight<12) Num_Straight = Num_Straight+1;
//     std::cout << "radhit,radfold " << radhit << " " << radfold << " change straight by -1" << std::endl;
  }

// u unit 2D_Vector along straight part of the electrode neutral fiber 
  double u[2];
  u[0] = m_electrode->Cosu(Num_Straight, PhiCell);
  u[1] = m_electrode->Sinu(Num_Straight, PhiCell);
// Middle m_coordinates of this straight part of the electrode neutral fiber
  double Xm[2];
  Xm[0] = m_electrode->XCentEle(Num_Straight, PhiCell);
  Xm[1] = m_electrode->YCentEle(Num_Straight, PhiCell);
// m_Hit Vector components
  dx = xhit - Xm[0];  dy = yhit - Xm[1];

// First compute algebric distance m_hit (2D) the 2D_projection of the
// m_Hit Vector on this electrode neutral fiber.
  double m_hit = dx*u[0] + dy*u[1];
  
//
// Flat of Fold Region ?
//
  double Half_Elec;
  Half_Elec = m_electrode->HalfLength(Num_Straight,PhiCell); 

  if(fabs(m_hit) < Half_Elec) {
// Flat Region
    DistEle = u[0]*dy - u[1]*dx;
    xl=m_hit/Half_Elec;
  }
  else {
// Fold region
// c_Hit Vector components and its length
    dx = xhit - Xc[0];  dy = yhit - Xc[1];  dr = sqrt( dx*dx + dy*dy);
    DistEle = (Num_Coude%2 == 0) ? (rint_eleFib-dr) : (dr - rint_eleFib);
    if (Num_Coude==Num_Straight) xl=-1.;
    else xl=+1;
  }      // end of Fold Regions

  return DistEle;

} // end of the function Distance_Ele
 
#endif

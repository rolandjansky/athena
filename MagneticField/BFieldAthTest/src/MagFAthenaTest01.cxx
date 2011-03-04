/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Author :  Saclay Muon Software Group SaMuSoG
//  Date   :  30/03/00
//
// The MagFAthenaTest01 class
// is an Algorithm class to test MagFieldAthenaSvc
//
#include "BFieldAth/IMagFieldAthenaSvc.h"
#include "BFieldAthTest/MagFAthenaTest01.h"
 
MagFAthenaTest01::MagFAthenaTest01(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  p_MagFieldAthenaSvc("MagFieldAthenaSvc", name)
{

  DoneDumpy01 = 0 ;

  declareProperty("ca"     , m_ca     = 0 ) ;
  declareProperty("nr"     , m_nr     = 0 ) ;
  declareProperty("nphi"   , m_nphi   = 0 ) ;
  declareProperty("nstx"   , m_nstx   = 0 ) ;
  declareProperty("nsty"   , m_nsty   = 0 ) ;
  declareProperty("nstz"   , m_nstz   = 0 ) ;
  declareProperty("nstr"   , m_nstr   = 0 ) ;
  declareProperty("nx"     , m_nx     = 0 ) ;
  declareProperty("ny"     , m_ny     = 0 ) ;
  declareProperty("nz"     , m_nz     = 0 ) ;
  declareProperty("rmin"   , m_rmin   = 0 ) ;
  declareProperty("xmin"   , m_xmin   = 0 ) ;
  declareProperty("ymin"   , m_ymin   = 0 ) ;
  declareProperty("zmin"   , m_zmin   = 0 ) ;
  declareProperty("xmax"   , m_xmax   = 0 ) ;
  declareProperty("ymax"   , m_ymax   = 0 ) ;
  declareProperty("zmax"   , m_zmax   = 0 ) ;
  declareProperty("rmax"   , m_rmax   = 0 ) ;
  declareProperty("phimax" , m_phimax = 0 ) ;
  declareProperty("phimin" , m_phimin = 0 ) ;
  declareProperty("MagFieldSvc", p_MagFieldAthenaSvc, "Magnet Field used by this algorithm") ;

}
 
// Initialize
StatusCode MagFAthenaTest01::initialize(){

  MsgStream mlog(messageService(), name());
  mlog << MSG::INFO << "================================" << endreq;
  mlog << MSG::INFO << "=Proprieties are     " << endreq;
  mlog << MSG::INFO << "= ca       " << m_ca       << endreq;
  mlog << MSG::INFO << "= nr       " << m_nr       << endreq;
  mlog << MSG::INFO << "= nphi     " << m_nphi     << endreq;
  mlog << MSG::INFO << "= nstx     " << m_nstx     << endreq;
  mlog << MSG::INFO << "= nsty     " << m_nsty     << endreq;
  mlog << MSG::INFO << "= nstz     " << m_nstz     << endreq;
  mlog << MSG::INFO << "= nstr     " << m_nstr     << endreq;
  mlog << MSG::INFO << "= nx       " << m_nx       << endreq;
  mlog << MSG::INFO << "= ny       " << m_ny       << endreq;
  mlog << MSG::INFO << "= nz       " << m_nz       << endreq;
  mlog << MSG::INFO << "= rmin     " << m_rmin     << endreq;
  mlog << MSG::INFO << "= xmin     " << m_xmin     << endreq;
  mlog << MSG::INFO << "= ymin     " << m_ymin     << endreq;
  mlog << MSG::INFO << "= zmin     " << m_zmin     << endreq;
  mlog << MSG::INFO << "= xmax     " << m_xmax     << endreq;
  mlog << MSG::INFO << "= ymax     " << m_ymax     << endreq;
  mlog << MSG::INFO << "= zmax     " << m_zmax     << endreq;
  mlog << MSG::INFO << "= rmax     " << m_rmax     << endreq;
  mlog << MSG::INFO << "= phimax   " << m_phimax   << endreq;
  mlog << MSG::INFO << "= phimin   " << m_phimin   << endreq;
  mlog << MSG::INFO << "================================" << endreq;


//Set pointer on MagFieldAthenaSvc
 if (p_MagFieldAthenaSvc.retrieve().isFailure()){
    mlog << MSG::WARNING   << "Could not find MagFieldAthenaSvc" << endreq;
    return( StatusCode::FAILURE );
  }

  mlog << MSG::INFO << "Initialisation ended     " << endreq;
  return StatusCode::SUCCESS;
  
}
 
// Execute
StatusCode MagFAthenaTest01::execute(){

  MsgStream mlog(messageService(), name());
  mlog << MSG::INFO << "in execute()" << endreq;

  if ( DoneDumpy01==0 ) {
    DoneDumpy01 = 1 ;
    Dumpy01();
  }
  
  return StatusCode::SUCCESS;
  
}				
 
// Finalize
StatusCode MagFAthenaTest01::finalize(){

  MsgStream mlog(messageService(), name());
  mlog << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;

}
 
// Do Dumpy01
void MagFAthenaTest01::Dumpy01(){

  //*Access MagFieldAthena
  MagFieldAthena* pMagField= p_MagFieldAthenaSvc->GetUpdatedMagFieldAthena();

  //*Set the file to dump
  std::ofstream  OutTest;
  OutTest.open("Out.BFieldAthTest");
  OutTest << setiosflags(std::ios::fixed);

  std::ofstream  OutTest1;
  OutTest1.open("Out.BFieldAthTest01");
  OutTest1 << setiosflags(std::ios::fixed);

  //*Dump
  OutTest << " Dumpy01: " << std::endl;
  OutTest << "m_ca    : " << m_ca     <<  std::endl;
  OutTest << "m_nx    : " << m_nx     <<  std::endl;
  OutTest << "m_ny    : " << m_ny     <<  std::endl;
  OutTest << "m_nz    : " << m_nz     <<  std::endl;
  OutTest << "m_nr    : " << m_nr     <<  std::endl;
  OutTest << "m_nphi  : " << m_nphi   <<  std::endl;
  OutTest << "m_nstx  : " << m_nstx   <<  std::endl;
  OutTest << "m_nsty  : " << m_nsty   <<  std::endl;
  OutTest << "m_nstz  : " << m_nstz   <<  std::endl;
  OutTest << "m_nstr  : " << m_nstr   <<  std::endl;
  OutTest << "m_xmax  : " << m_xmax   <<  std::endl;
  OutTest << "m_xmin  : " << m_xmin   <<  std::endl;
  OutTest << "m_ymax  : " << m_ymax   <<  std::endl;
  OutTest << "m_ymin  : " << m_ymin   <<  std::endl;
  OutTest << "m_zmax  : " << m_zmax   <<  std::endl;
  OutTest << "m_zmin  : " << m_zmin   <<  std::endl;
  OutTest << "m_rmax  : " << m_rmax   <<  std::endl;
  OutTest << "m_rmin  : " << m_rmin   <<  std::endl;
  OutTest << "m_phimax: " << m_phimax <<  std::endl;
  OutTest << "m_phimin: " << m_phimin <<  std::endl;

  if(m_ca==10){
  
    OutTest << "cylindrical coordinate: " << m_ca     <<  std::endl;
    int    nz     = m_nz ;
    double zmax   = m_zmax;
    double zmin   = m_zmin;
    //   
    int    nr     = m_nr;
    double rmax   = m_rmax;
    double rmin   = m_rmin;
    //   
    int    nphi   = m_nphi;
    double phimax = m_phimax;
    double phimin = m_phimin; 
    // 
    int nstep     = m_nstr;
    //   
    double zstep   = (zmax  -zmin  )/(nz  -1);
    double rstep   = (rmax  -rmin  )/(nr  -1);
    double phistep = (phimax-phimin)/(nphi-1);
    float precision = 0.0001;
    //   
    for (int iz=1; iz<=nz; iz=iz+nstep){
      double The_z = zmin + zstep * (iz-1);
      for (int ir=1; ir<=nr; ir=ir+nstep){
	double The_r = rmin + rstep * (ir-1);
	for (int iphi=1; iphi<=nphi; iphi=iphi+nstep){
	  double The_phi = (phimin + phistep * (iphi-1))*M_PI/180.;	
	  double The_x   = cos(The_phi)*The_r;
	  double The_y   = sin(The_phi)*The_r;
	
	  float XYZ[3]   ;
	  float BGXYZ[12] ;
	  float BGXYZ2[12] ;
	  XYZ[0] = The_x ;
	  XYZ[1] = The_y ;
	  XYZ[2] = The_z ;
	  pMagField->fieldGradient(XYZ,BGXYZ);
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[3]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[4]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[5]
		  << std::endl;
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[6]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[7]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[8]
		  << std::endl;
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[9]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[10]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[11]
		  << std::endl;
	  XYZ[0] = The_x*10. ;
	  XYZ[1] = The_y*10. ;
	  XYZ[2] = The_z*10. ;
	  pMagField->field_XYZ_in_mm(XYZ,BGXYZ2);
	  for (int i=0; i<3; i++){
	    if(fabs(BGXYZ[i]-BGXYZ2[i]) >= precision){ OutTest << " ca merde "<< BGXYZ[i] <<" " <<BGXYZ2[i] << XYZ[i] << std::endl;}
	  }
	  pMagField->fieldGradient_XYZ_in_mm(XYZ,BGXYZ2);
	  for (int i=0; i<12; i++){
	    if(i<3  && fabs(BGXYZ[i]- BGXYZ2[i]) >= precision){     OutTest << " ca merde2 "<< BGXYZ[i] <<" " <<BGXYZ2[i] << XYZ[i] << std::endl;}
	    //	    if(i>=3 && fabs(BGXYZ[i]-BGXYZ2[i]*10.) >= precision){  OutTest << " ca merde2 "<< BGXYZ[i] <<" " <<BGXYZ2[i] << XYZ[i] <<std::endl;}
	  }
	}
      }
    }
  }else if(m_ca==9){
    std::ofstream  OutTest2;
    OutTest2.open("Out.BFieldAthTest02");
    OutTest2 << setiosflags(std::ios::fixed);
    
    std::ofstream  OutTest3;
    OutTest3.open("Out.BFieldAthTest03");
    OutTest3 << setiosflags(std::ios::fixed);
    
    OutTest3<< "Should be Empty   "<< std::endl;
    OutTest << "carthesian coordinate: " << m_ca     <<  std::endl;
    OutTest << "ToroidBarrelOn()  " <<  pMagField->getToroidBarrelOn() <<  std::endl;
    OutTest << "ToroidECTAOn()    " <<  pMagField->getToroidECTAOn()   <<  std::endl;
    OutTest << "ToroidECTCOn()    " <<  pMagField->getToroidECTCOn()   <<  std::endl;
    OutTest << "AllToroidOn()     " <<  pMagField->getAllToroidOn()    <<  std::endl;
    OutTest << "SolenoidOn()      " <<  pMagField->getSolenoidOn()     <<  std::endl;
    OutTest << "SolenoidOn()      " <<  pMagField->getSolenoidOn()     <<  std::endl;
    OutTest << "FieldStatusOn()   " <<  pMagField->getFieldStatusOn()  <<  std::endl;
    int    nx     = m_nx ;
    int    xmax   = (int) m_xmax;
    int    xmin   = (int) m_xmin;
    int    ny     = m_ny ;
    int    ymax   = (int) m_ymax;
    int    ymin   = (int) m_ymin;
    int    nz     = m_nz ;
    int    zmax   = (int) m_zmax;
    int    zmin   = (int) m_zmin;
    int    nstx   = m_nstx;
    int    nsty   = m_nsty;
    int    nstz   = m_nstz;
    float precision = 0.0001;
    OutTest 
    << " m_nx   " <<   nx 
    << " m_xmax " <<   xmax
    << " m_xmin " <<   xmin
    << std::endl;
    OutTest 
    << " m_ny   " <<   ny  
    << " m_ymax " <<   ymax
    << " m_ymin " <<   ymin
    << std::endl;
    OutTest 
    << " m_nz   " <<   nz  
    << " m_zmax " <<   zmax
    << " m_zmin " <<   zmin
    << std::endl;
    OutTest 
    << " m_nstx " <<   nstx
    << " m_nsty " <<   nsty
    << " m_nstz " <<   nstz		  
    << std::endl;
/*
    for (int iz=zmin; iz<=zmax; iz=iz+nstz){
      for (int iy=ymin; iy<=ymax; iy=iy+nsty){
	for (int ix=xmin; ix<=xmax; ix=ix+nstx){
*/
    float dx = (float) (xmax - xmin) / (float) nx;
    float dy = (float) (ymax - ymin) / (float) ny;
    float dz = (float) (zmax - zmin) / (float) nz;
    for (int iz=0; iz<nz; ++iz) {
      for (int iy=0; iy<ny; ++iy) {
        for (int ix=0; ix<nx; ++ix) {
	  float XYZ[3]   ;
	  float BGXYZ[12] ;
	  float BGXYZ2[12] ;
/*
	  XYZ[0] = ix*1.;
	  XYZ[1] = iy*1.;
	  XYZ[2] = iz*1.;
*/
	  XYZ[0] = (float) xmin + dx * (float) ix;
	  XYZ[1] = (float) ymin + dy * (float) iy;
	  XYZ[2] = (float) zmin + dz * (float) iz;
	  pMagField->field(XYZ,BGXYZ);
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]/10.
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]/10.
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]/10.
		  << std::endl;
	  pMagField->fieldGradient(XYZ,BGXYZ);
	  OutTest1<< " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]/10.
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]/10.
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]/10.
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[3]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[4]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[5]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[6]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[7]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[8]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[9]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[10]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[11]
		  << std::endl;
	  pMagField->field_tesla_cm(XYZ,BGXYZ2);
	  for (int i=0; i<3; i++){
	    if(fabs(BGXYZ[i]/10.-BGXYZ2[i]) >= precision && fabs(XYZ[i]) > precision){ 
	      OutTest3 << " field - field_tesla_cm "<< std::endl;
	      OutTest3 << XYZ[i]                 <<"  " 
		       << BGXYZ[i]/10.           <<"  " 
		       << BGXYZ2[i]              <<"  "
		       << BGXYZ[i]/10.-BGXYZ2[i] << std::endl;
	    }
	  }
	  pMagField->fieldGradient_tesla_cm(XYZ,BGXYZ2);
	  for (int i=0; i<12; i++){
	    if(fabs(BGXYZ[i]/10.- BGXYZ2[i]) >= precision && fabs(XYZ[i]) > precision ){     
	      OutTest3 << " fieldGradient - fieldGradient_tesla_cm "<< std::endl;
	      OutTest3 << XYZ[i]                 <<"  " 
		       << BGXYZ[i]/10.           <<"  " 
		       << BGXYZ2[i]              <<"  "
		       << BGXYZ[i]/10.-BGXYZ2[i] << std::endl;
	    }
	  }
	  OutTest2<< " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[2]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[3]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[4]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[5]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[6]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[7]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[8]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[9]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[10]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ2[11]
		  << std::endl;
	}
      }
    }
  }else if(m_ca==19){
    OutTest1<< " Should be Empty        "<< std::endl;
    OutTest << " carthesian coordinate: " << m_ca     <<  std::endl;
    OutTest << " ToroidBarrelOn()  " <<  pMagField->getToroidBarrelOn() <<  std::endl;
    OutTest << " ToroidECTAOn()    " <<  pMagField->getToroidECTAOn()   <<  std::endl;
    OutTest << " ToroidECTCOn()    " <<  pMagField->getToroidECTCOn()   <<  std::endl;
    OutTest << " AllToroidOn()     " <<  pMagField->getAllToroidOn()    <<  std::endl;
    OutTest << " SolenoidOn()      " <<  pMagField->getSolenoidOn()     <<  std::endl;
    OutTest << " SolenoidOn()      " <<  pMagField->getSolenoidOn()     <<  std::endl;
    OutTest << " FieldStatusOn()   " <<  pMagField->getFieldStatusOn()  <<  std::endl;
    int    nx     = m_nx ;
    int    xmax   = (int) m_xmax;
    int    xmin   = (int) m_xmin;
    int    ny     = m_ny ;
    int    ymax   = (int) m_ymax;
    int    ymin   = (int) m_ymin;
    int    nz     = m_nz ;
    int    zmax   = (int) m_zmax;
    int    zmin   = (int) m_zmin;
    int    nstx   = m_nstx;
    int    nsty   = m_nsty;
    int    nstz   = m_nstz;
    float precision = 0.0001;
    OutTest 
    << " m_nx   " <<   nx 
    << " m_xmax " <<   xmax
    << " m_xmin " <<   xmin
    << std::endl;
    OutTest 
    << " m_ny   " <<   ny  
    << " m_ymax " <<   ymax
    << " m_ymin " <<   ymin
    << std::endl;
    OutTest 
    << " m_nz   " <<   nz  
    << " m_zmax " <<   zmax
    << " m_zmin " <<   zmin
    << std::endl;
    OutTest 
    << " m_nstx " <<   nstx
    << " m_nsty " <<   nsty
    << " m_nstz " <<   nstz		  
    << std::endl;
    float dx = (float) (xmax - xmin) / (float) nx;
    float dy = (float) (ymax - ymin) / (float) ny;
    float dz = (float) (zmax - zmin) / (float) nz;
    for (int iz=0; iz<nz; ++iz) {
      for (int iy=0; iy<ny; ++iy) {
        for (int ix=0; ix<nx; ++ix) {
	  float XYZ[3]   ;
	  float BGXYZ[12] ;
	  float BGXYZ2[12] ;
	  XYZ[0] = (float) xmin + dx * (float) ix;
	  XYZ[1] = (float) ymin + dy * (float) iy;
	  XYZ[2] = (float) zmin + dz * (float) iz;
	  pMagField->field(XYZ,BGXYZ);
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]/10.
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]/10.
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]/10.
		  << std::endl;
	  pMagField->field_tesla_cm(XYZ,BGXYZ2);
	  for (int i=0; i<3; i++){
	    if(fabs(BGXYZ[i]/10.-BGXYZ2[i]) >= precision && fabs(XYZ[i]) > precision){ 
	      OutTest1 << " field - field_tesla_cm " << std::endl;
	      OutTest1 << XYZ[i]                 <<"  " 
		       << BGXYZ[i]/10.           <<"  " 
		       << BGXYZ2[i]              <<"  "
		       << BGXYZ[i]/10.-BGXYZ2[i] << std::endl;
	    }
	  }
	  pMagField->fieldGradient_tesla_cm(XYZ,BGXYZ2);
	  OutTest1 << " fieldGradient_tesla_cm " << std::endl;
	  for (int i=0; i<12; i++){
	      OutTest1 << " i                     = " << i                      << std::endl;
	      if( i < 3 ) OutTest1 << " XYZ[i]                = " << XYZ[i]                 << std::endl;
	      OutTest1 << " BGXYZ2[i]             = " << BGXYZ2[i]              << std::endl;
	  }
	  OutTest1 << "  " << std::endl;
	  OutTest1 << "  " << std::endl;
	  OutTest1 << "  " << std::endl;
	}
      }
    }
  }else if(m_ca==1){
    OutTest << "carthesian coordinate: " << m_ca     <<  std::endl;
    int    nx     = m_nx ;
    double xmax   = m_xmax;
    double xmin   = m_xmin;
    int    ny     = m_ny ;
    double ymax   = m_ymax;
    double ymin   = m_ymin;
    int    nz     = m_nz ;
    double zmax   = m_zmax;
    double zmin   = m_zmin;
    int nstepx    = m_nstx;
    int nstepy    = m_nsty;
    int nstepz    = m_nstz;
    double zstep  = (zmax - zmin  )/(nz  - 1);
    double ystep  = (ymax - ymin  )/(ny  - 1);
    double xstep  = (xmax - xmin  )/(nx  - 1);
    for (int iz=1; iz<=nz; iz=iz+nstepz){
      double The_z = zmin + zstep * (iz-1);
      for (int iy=1; iy<=ny; iy=iy+nstepy){
	double The_y = ymin + ystep * (iy-1);
	for (int ix=1; ix<=nx; ix=ix+nstepx){
	  double The_x = xmin + xstep * (ix-1);
	  float XYZ[3]   ;
	  float BGXYZ[12] ;
	  XYZ[0] = The_x ;
	  XYZ[1] = The_y ;
	  XYZ[2] = The_z ;
//	  pMagField->fieldGradient(XYZ,BGXYZ);
	  pMagField->field(XYZ,BGXYZ);
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	}
      }
    }
  }else if(m_ca==11){
    std::ofstream  OutTest2;
    OutTest2.open("Out.BFieldAthTest02");
    OutTest2 << setiosflags(std::ios::fixed);
    
    std::ofstream  OutTest3;
    OutTest3.open("Out.BFieldAthTest03");
    OutTest3 << setiosflags(std::ios::fixed);

    OutTest << "carthesian coordinate: " << m_ca     <<  std::endl;
    OutTest << "ToroidBarrelOn()  " <<  pMagField->getToroidBarrelOn() <<  std::endl;
    OutTest << "ToroidECTAOn()    " <<  pMagField->getToroidECTAOn()   <<  std::endl;
    OutTest << "ToroidECTCOn()    " <<  pMagField->getToroidECTCOn()   <<  std::endl;
    OutTest << "AllToroidOn()     " <<  pMagField->getAllToroidOn()    <<  std::endl;
    OutTest << "SolenoidOn()      " <<  pMagField->getSolenoidOn()     <<  std::endl;
    OutTest << "SolenoidOn()      " <<  pMagField->getSolenoidOn()     <<  std::endl;
    OutTest << "FieldStatusOn()   " <<  pMagField->getFieldStatusOn()  <<  std::endl;
    int    nx     = m_nx ;
    int    xmax   = (int) m_xmax;
    int    xmin   = (int) m_xmin;
    int    ny     = m_ny ;
    int    ymax   = (int) m_ymax;
    int    ymin   = (int) m_ymin;
    int    nz     = m_nz ;
    int    zmax   = (int) m_zmax;
    int    zmin   = (int) m_zmin;
    int    nstx   = m_nstx;
    int    nsty   = m_nsty;
    int    nstz   = m_nstz;
    OutTest 
    << " m_nx   " <<   nx 
    << " m_xmax " <<   xmax
    << " m_xmin " <<   xmin
    << std::endl;
    OutTest 
    << " m_ny   " <<   ny  
    << " m_ymax " <<   ymax
    << " m_ymin " <<   ymin
    << std::endl;
    OutTest 
    << " m_nz   " <<   nz  
    << " m_zmax " <<   zmax
    << " m_zmin " <<   zmin
    << std::endl;
    OutTest 
    << " m_nstx " <<   nstx
    << " m_nsty " <<   nsty
    << " m_nstz " <<   nstz		  
    << std::endl;
    float dx = (float) (xmax - xmin) / (float) nx;
    float dy = (float) (ymax - ymin) / (float) ny;
    float dz = (float) (zmax - zmin) / (float) nz;
    float XYZ[3],XYZ2[3];
    float BGXYZ[12] ;
    for (int iz=0; iz<nz; ++iz) {
      for (int iy=0; iy<ny; ++iy) {
        for (int ix=0; ix<nx; ++ix) {
	  XYZ[0]  = (float) xmin + dx * (float) ix;
	  XYZ[1]  = (float) ymin + dy * (float) iy;
	  XYZ[2]  = (float) zmin + dz * (float) iz;
	  XYZ2[0] = XYZ[0]*10. ; 
	  XYZ2[1] = XYZ[1]*10. ;
	  XYZ2[2] = XYZ[2]*10. ;
	  pMagField->field(XYZ,BGXYZ);
	  OutTest << "field " <<  std::endl;
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  pMagField->fieldGradient(XYZ,BGXYZ);
	  OutTest << "fieldGradient " <<  std::endl;
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  pMagField->field_tesla_mm(XYZ2,BGXYZ);
	  OutTest1<< "field_tesla_mm " <<  std::endl;
	  OutTest1<< " " <<std::setw(10)<<std::setprecision(4) <<  XYZ2[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ2[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ2[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  pMagField->field_tesla_cm(XYZ,BGXYZ);
	  OutTest2<< "field_tesla_cm " <<  std::endl;
	  OutTest2<< " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  pMagField->fieldGradient_tesla_cm(XYZ,BGXYZ);
	  OutTest2<< "fieldGradient_tesla_cm " <<  std::endl;
	  OutTest2<< " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  pMagField->field_XYZ_in_mm(XYZ2,BGXYZ);
	  OutTest3<< "field_XYZ_in_mm " <<  std::endl;
	  OutTest3<< " " <<std::setw(10)<<std::setprecision(4) <<  XYZ2[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ2[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ2[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  pMagField->fieldGradient_XYZ_in_mm(XYZ2,BGXYZ);
	  OutTest3<< "fieldGradient_XYZ_in_mm " <<  std::endl;
	  OutTest3<< " " <<std::setw(10)<<std::setprecision(4) <<  XYZ2[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ2[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ2[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  }
	}
      }
  }else{
    OutTest << "cylindrical coordinate Perso: " << m_ca     <<  std::endl;
    int    nz     = 2300 ;
    double zmax   = 2300.;
    double zmin   =    0.;
    int    nr     = 1500 ;
    double rmax   = 1500.;
    double rmin   =    0.;
    int    nphi   = 23  ;
    double phimax = 22.5;
    double phimin =  0.5; 
    int nstep     = 100;
    double zstep   = (zmax  -zmin  )/(nz  -1);
    double rstep   = (rmax  -rmin  )/(nr  -1);
    double phistep = (phimax-phimin)/(nphi-1);
    float precision = 0.0001;
    //   
    for (int iz=1; iz<=nz; iz=iz+nstep){
      double The_z = zmin + zstep * (iz-1);
      for (int ir=1; ir<=nr; ir=ir+nstep){
	double The_r = rmin + rstep * (ir-1);
	for (int iphi=1; iphi<=nphi; iphi=iphi+nstep){
	  double The_phi = (phimin + phistep * (iphi-1))*M_PI/180.;
	
	  double The_x   = cos(The_phi)*The_r;
	  double The_y   = sin(The_phi)*The_r;
	
	  float XYZ[3]   ;
	  float BGXYZ[12] ;
	  float BGXYZ2[12] ;
	  int iz;
	  XYZ[0] = The_x ;
	  XYZ[1] = The_y ;
	  XYZ[2] = The_z ;
	  pMagField->field(XYZ,BGXYZ);
	  OutTest1<< " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  XYZ[2]
	          << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  pMagField->whereAmI( XYZ, iz);
	  OutTest << "----- iz  ---- " << iz << "  " << pMagField->whereAmI( XYZ) << std::endl;
	  //	  pMagField->whereAmI( XYZ);
	  pMagField->fieldGradient(XYZ,BGXYZ);
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[0]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[1]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[2]
		  << std::endl;
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[3]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[4]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[5]
		  << std::endl;
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[6]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[7]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[8]
		  << std::endl;
	  OutTest << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[9]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[10]
		  << " " <<std::setw(10)<<std::setprecision(4) <<  BGXYZ[11]
		  << std::endl;
	  XYZ[0] = The_x*10. ;
	  XYZ[1] = The_y*10. ;
	  XYZ[2] = The_z*10. ;
	  pMagField->field_XYZ_in_mm(XYZ,BGXYZ2);
	  for (int i=0; i<3; i++){
	    if(fabs(BGXYZ[i]-BGXYZ2[i]) >= precision){ OutTest << " ca merde "<< BGXYZ[i] <<" " <<BGXYZ2[i] << XYZ[i] << std::endl;}
	  }
	  pMagField->fieldGradient_XYZ_in_mm(XYZ,BGXYZ2);
	  for (int i=0; i<12; i++){
	    if(i<3  && fabs(BGXYZ[i]- BGXYZ2[i]) >= precision){     OutTest << " ca merde2 "<< BGXYZ[i] <<" " <<BGXYZ2[i] << XYZ[i] << std::endl;}
	    //	    if(i>=3 && fabs(BGXYZ[i]-BGXYZ2[i]*10.) >= precision){  OutTest << " ca merde2 "<< BGXYZ[i] <<" " <<BGXYZ2[i] << XYZ[i] <<std::endl;}
	  }
	}
      }
    }
  }
  //*Close the file to dump
  OutTest.close();
  
}

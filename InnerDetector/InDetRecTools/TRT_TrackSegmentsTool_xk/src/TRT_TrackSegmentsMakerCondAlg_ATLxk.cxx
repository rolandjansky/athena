/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TRT_TrackSegmentsMakerCondAlg_ATLxk.h"

#include "TRT_ReadoutGeometry/TRT_Numerology.h"
#include "TRT_ReadoutGeometry/TRT_BarrelElement.h"
#include "TRT_ReadoutGeometry/TRT_EndcapElement.h"
#include "TRT_ReadoutGeometry/TRT_BaseElement.h"

#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/RectangleBounds.h"

#include "TrkExInterfaces/IPropagator.h"
#include "TrkGeometry/MagneticFieldProperties.h"


#include <memory>
#include <utility>
#include <ostream>
#include <fstream>
#include <cmath>


///////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////

InDet::TRT_TrackSegmentsMakerCondAlg_ATLxk::TRT_TrackSegmentsMakerCondAlg_ATLxk(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator),
    m_propTool("Trk::RungeKuttaPropagator")

{
  m_fieldmode   =      "MapSolenoid" ;
  m_pTmin       =                500.;
  m_nMom        =                 70 ;
  declareProperty("PropagatorTool"         ,m_propTool     );
  declareProperty("pTmin"                  ,m_pTmin        );
  declareProperty("NumberMomentumChannel"  ,m_nMom         ); 

}

///////////////////////////////////////////////////////////////////
// Initialisation
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsMakerCondAlg_ATLxk::initialize()
{

  ATH_CHECK(m_trtDetEleContKey.initialize());
  ATH_CHECK(m_writeKey.initialize());

  // Get propagator tool
  //
  if( m_propTool.retrieve().isFailure()) {
    msg(MSG::FATAL)<<"Failed to retrieve tool "<< m_propTool <<endmsg;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) << "Retrieved tool " << m_propTool << endmsg;
  }

  magneticFieldInit();

  // Get TRT ID                                                                                             
  if (detStore()->retrieve(m_trtid, "TRT_ID").isFailure()) {
    msg(MSG::FATAL) << "Could not get TRT ID helper" << endmsg;
    return StatusCode::FAILURE;
  }

  m_nPhi        =                500                  ;
  m_Psi         = 2./float(m_nMom-1)                  ;
  m_Psi128      = m_Psi*128.                          ;
  m_A           = float(m_nPhi)/(2.*M_PI)             ;

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////
// Map of straws production
///////////////////////////////////////////////////////////////////
// Taken from the old InDet::TRT_TrackSegmentsMaker_ATLxk::mapStrawsProduction()
///////////////////////////////////////////////////////////////////

StatusCode InDet::TRT_TrackSegmentsMakerCondAlg_ATLxk::execute(const EventContext& ctx) const
{

  SG::WriteCondHandle<InDet::TRT_TrackSegmentsToolCondData_xk> writeHandle{m_writeKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.");
    return StatusCode::SUCCESS;
  }

  InDet::TRT_TrackSegmentsToolCondData_xk* writeCdo=new InDet::TRT_TrackSegmentsToolCondData_xk();

  EventIDRange rangeTrt;

  SG::ReadCondHandle<InDetDD::TRT_DetElementContainer> trtDetEleHandle(m_trtDetEleContKey, ctx);
  if (not trtDetEleHandle.isValid()) {
    ATH_MSG_FATAL(m_trtDetEleContKey.fullKey() << " is not available.");
    return StatusCode::FAILURE;
  }

  const InDetDD::TRT_Numerology* trtNum = trtDetEleHandle->getTRTNumerology();
  if (trtNum==nullptr){
    ATH_MSG_FATAL("Pointer to TRT_Numerology not found in condition store" << m_trtDetEleContKey.fullKey());
  }

  if (not trtDetEleHandle.range(rangeTrt)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << trtDetEleHandle.key());
    return StatusCode::FAILURE;
  }

  if(writeCdo->m_ndzdr  ) delete [] writeCdo->m_ndzdr  ;
  if(writeCdo->m_slope  ) delete [] writeCdo->m_slope  ;
  if(writeCdo->m_islope ) delete [] writeCdo->m_islope ;
  
  float RZ   [4][200];
  float Tmin [4][200];
  float Tmax [4][200];

  // Map straws production for barrel geometry
  //
  int Rings  = trtNum->getNBarrelRings();
  int NPhi   = trtNum->getNBarrelPhi(); 
  int n      = 0;
 
  for(int ring = 0; ring!=Rings; ++ring) {
    
    int NSlayers = trtNum->getNBarrelLayers(ring);

    writeCdo->m_flayers[1][ring] = writeCdo->m_nlayers[1]; writeCdo->m_flayers[2][ring] = writeCdo->m_nlayers[2]; 
    writeCdo->m_nlayers[1]      += NSlayers    ; writeCdo->m_nlayers[2]      += NSlayers    ;

    for(int nsl=0; nsl!=NSlayers; ++nsl) {
      

      for(int f=0; f!=NPhi; ++f) {

	const InDetDD::TRT_BaseElement*   base1 = trtDetEleHandle->getBarrelDetElement(0,ring,f,nsl);
	const InDetDD::TRT_BaseElement*   base2 = trtDetEleHandle->getBarrelDetElement(1,ring,f,nsl);
	if(!base1 || !base2) continue;
	const InDetDD::TRT_BarrelElement* bael1 =  
	  dynamic_cast<const InDetDD::TRT_BarrelElement*>(base1);
	const Trk::RectangleBounds* rb1 = 
	  dynamic_cast<const Trk::RectangleBounds*>(&base1->bounds());
	const Trk::RectangleBounds* rb2 = 
	  dynamic_cast<const Trk::RectangleBounds*>(&base2->bounds());
	if(!bael1 || !rb1 || !rb2) continue;
	float rmean = 0;
	if(f==0) {

	  Amg::Vector3D  C1  = base1->center(); 
	  Amg::Vector3D  C2  = base2->center();
 	  RZ   [1][n] = std::sqrt(C1.x()*C1.x()+C1.y()*C1.y());

	  Tmin [1][n] = (C1.z()-rb1->halflengthY())/RZ[1][n];
	  Tmax [1][n] = -.001;

	  RZ   [2][n] = std::sqrt(C2.x()*C2.x()+C2.y()*C2.y());

	  Tmin [2][n] = +.001;
	  Tmax [2][n] = (C2.z()+rb2->halflengthY())/RZ[2][n];
	  ++n;
	}
	int ns = bael1->nStraws();
	for(int s=0; s!=ns; ++s) {
	  
	  const Identifier        id1 = m_trtid->straw_id(-1,f,ring,nsl,s );
	  const Amg::Vector3D   * sc1 = &(base1->strawCenter           (s));
	  const Amg::Transform3D* st1 = &(base1->strawTransform        (s)); 
	  const Amg::Transform3D* tr1 = &(base1->surface(id1).transform() );
	  if(f==0) rmean+=std::sqrt(sc1->x()*sc1->x()+sc1->y()*sc1->y());

	  if(!sc1 || !st1 || !tr1 ) {ATH_MSG_ERROR("problem with TRT geometry");}
	  ++writeCdo->m_nstraws[1];
	  const Identifier        id2 = m_trtid->straw_id(+1,f,ring,nsl,s );
	  const Amg::Vector3D   * sc2 = &(base2->strawCenter           (s));
	  const Amg::Transform3D* st2 = &(base2->strawTransform        (s)); 
	  const Amg::Transform3D* tr2 = &(base2->surface(id2).transform() );
	  if(!sc2 || !st2 || !tr2)  {ATH_MSG_ERROR("problem with TRT geometry");}
	  ++writeCdo->m_nstraws[2];
	}
	if(f==0) { RZ[1][n-1] = RZ[2][n-1] = rmean/float(ns);}
      }
    }
  }

  // Endcap
  //
  int Wheels = trtNum->getNEndcapWheels(); if(!Wheels) return StatusCode::SUCCESS;
  NPhi       = trtNum->getNEndcapPhi   (); 
  n          = 0; 
  for(int wh = 0; wh!=Wheels; ++wh) {

    int NSlayers = trtNum->getNEndcapLayers(wh);
    writeCdo->m_flayers[0][wh] = writeCdo->m_nlayers[0]; writeCdo->m_flayers[3][wh] = writeCdo->m_nlayers[3];
    writeCdo->m_nlayers[0]    += NSlayers    ; writeCdo->m_nlayers[3]     += NSlayers   ;

    for(int nsl = 0; nsl!=NSlayers; ++nsl) {

      for(int f=0; f!=NPhi; ++f) {

	const InDetDD::TRT_BaseElement* base1 = trtDetEleHandle->getEndcapDetElement(0,wh,nsl,f);
	const InDetDD::TRT_BaseElement* base2 = trtDetEleHandle->getEndcapDetElement(1,wh,nsl,f);
	if(!base1 || !base2) continue;

	const InDetDD::TRT_EndcapElement* enel1 =  
	  dynamic_cast<const InDetDD::TRT_EndcapElement*>(base1);
	const Trk::DiscBounds* db1 = 
	  dynamic_cast<const Trk::DiscBounds*>(&base1->bounds());
	const Trk::DiscBounds* db2 = 
	  dynamic_cast<const Trk::DiscBounds*>(&base2->bounds());
	if(!enel1 || !db1 || !db2) continue;
	if(f==0) {

	  Amg::Vector3D  C1  = base1->center(); 
	  Amg::Vector3D  C2  = base2->center(); 
	  RZ   [0][n] = C1.z();
	  Tmin [0][n] = RZ[0][n]/db1->rMin(); 
	  Tmax [0][n] = RZ[0][n]/db1->rMax(); 

	  RZ   [3][n] = C2.z();
	  Tmin [3][n] = RZ[3][n]/db2->rMax();
	  Tmax [3][n] = RZ[3][n]/db2->rMin();

	  ++n;
	}

	int ns = enel1->nStraws();

	for(int s=0; s!=ns; ++s) {

	  const Identifier        id1 = m_trtid->straw_id(-2,f,wh,nsl,s  );
	  const Amg::Vector3D   * sc1 = &(base1->strawCenter         (s) );
	  const Amg::Transform3D* st1 = &(base1->strawTransform      (s) ); 
	  const Amg::Transform3D* tr1 = &(base1->surface(id1).transform());

	  if(!sc1 || !st1 || !tr1) {ATH_MSG_ERROR("problem with TRT geometry");}
	  ++writeCdo->m_nstraws[0];

	  const Identifier        id2 = m_trtid->straw_id(+2,f,wh,nsl,s  );
	  const Amg::Vector3D   * sc2 = &(base2->strawCenter         (s) );
	  const Amg::Transform3D* st2 = &(base2->strawTransform      (s) ); 
	  const Amg::Transform3D* tr2 = &(base2->surface(id2).transform());

	  if(!sc2 || !st2 || !tr2) {ATH_MSG_ERROR("problem with TRT geometry");}
	  ++writeCdo->m_nstraws[3];
	}
      }
    }
  }

  double zmax = RZ[3][writeCdo->m_nlayers[3]-1]+10.;
  double rmax = RZ[2][writeCdo->m_nlayers[2]-1]+10.;
  const Trk::CylinderBounds CB(rmax,zmax);

  float rapidity[26]=
    {-2.05,-1.95,-1.84,-1.72,-1.62,-1.53,-1.43,-1.33,-1.21,-1.00,-.94, -.85,-.32,
       .32,  .85,  .94, 1.00, 1.21, 1.33, 1.43, 1.53, 1.62, 1.72,1.84, 1.95,2.05}; 

  std::list<Amg::Vector3D> G [26]; 
  Amg::Vector3D psv(0.,0.,0.); 
  Trk::PerigeeSurface ps(psv);

  for(int r=0; r!=26; ++r) {
    writeCdo->m_dzdr[r]   = 1./std::tan(2.*std::atan(std::exp(-rapidity[r])));
    double pinv =-1./(m_pTmin*std::sqrt(1.+writeCdo->m_dzdr[r]*writeCdo->m_dzdr[r]));
    auto trackPar = ps.createUniqueTrackParameters(
      0.,
      0.,
      0.,
      std::atan2(1., double(writeCdo->m_dzdr[r])),
      pinv,
      std::nullopt);
    m_propTool->globalPositions(ctx,
      G[r], *trackPar, m_fieldprop, CB, 5., Trk::pion);
  } 
  
  n = 0;
  for(int b=0; b!=4; ++b) {
    for(unsigned int i=0; i!=writeCdo->m_nlayers[b]; ++i) {
      writeCdo->m_begin[b][i] = n;
      for(int r=0; r!=26; ++r) {
	if( writeCdo->m_dzdr[r] >= Tmin[b][i] && writeCdo->m_dzdr[r] <= Tmax[b][i] ) {
	  writeCdo->m_end[b][i] = n++;
	}
      }
    }
  }

  writeCdo->m_ndzdr    = new unsigned int[n];
  writeCdo->m_islope   = new          int[n];
  writeCdo->m_slope    = new float       [n];
  writeCdo->m_cirsize  = writeCdo->m_nstraws[0]+writeCdo->m_nstraws[1];

  n          = 0;
  for(int b=0; b!=4; ++b) {

    for(unsigned int i=0; i!=writeCdo->m_nlayers[b]; ++i) {

      for(int r=0; r!=26; ++r) {
	if( writeCdo->m_dzdr[r] >= Tmin[b][i] && writeCdo->m_dzdr[r] <= Tmax[b][i] ) {
	  writeCdo->m_ndzdr[n] = r;
	  std::list<Amg::Vector3D>::iterator gp0,gp1, gp=G[r].begin(),gpe=G[r].end();
	  if   (b==0 || b==3) {

	    gp0 = gp;
	    for(++gp; gp!=gpe; ++gp) {
	      if(b==3 && (*gp).z() >= RZ[b][i]) break; 
	      if(b==0 && (*gp).z() <= RZ[b][i]) break;
	      gp1 = gp0;
	      gp0 = gp; 
	    }
	  }
	  else {
	    gp0 = gp;
	    for(++gp; gp!=gpe; ++gp) {
	      if(std::sqrt((*gp).x()*(*gp).x()+(*gp).y()*(*gp).y()) > RZ[b][i]) break;
	      gp1 = gp0;
	      gp0 = gp; 
	    }
	  }
	  double x,y,z,ax,ay,az;
	  if(gp!=gpe) {
	    x = (*gp0).x(); ax = (*gp ).x()-x;
	    y = (*gp0).y(); ay = (*gp ).y()-y;
	    z = (*gp0).z(); az = (*gp ).z()-z;
	    double as = 1./std::sqrt(ax*ax+ay*ay+az*az); ax*=as; ay*=as; az*=as;
	  }
	  else       {
	    x = (*gp1).x(); ax = (*gp0).x()-x;
	    y = (*gp1).y(); ay = (*gp0).y()-y;
	    z = (*gp1).z(); az = (*gp0).z()-z;
	    double as = 1./std::sqrt(ax*ax+ay*ay+az*az); ax*=as; ay*=as; az*=as;
	  }
	  double S  = 0;

	  if (b==0 || b==3) {
	    S = (RZ[b][i]-z)/az;
	  }
	  else              {
	    double A  = (ax*x+ay*y)*2.;
	    double D  = (RZ[b][i]-x-y)*(RZ[b][i]+x+y)+2.*x*y;
	    S         = D/A;
	    double B  = 2.*(ax*ax+ay*ay);
	    double Sq = A*A+2.*D*B;  Sq>0. ? Sq=std::sqrt(Sq) : Sq=0.;
	    double S1 =-(A+Sq)/B;
	    double S2 =-(A-Sq)/B;
            if (S > S2)
              S = S2;
            else if (S < S1)
              S = S1;
	  }
	  writeCdo->m_slope [n] = std::atan2(y+S*ay,x+S*ax)*m_A; 
	  writeCdo->m_islope[n] = int(writeCdo->m_slope[n]*m_Psi128);
	  ++n;
	}
      }
    }
  }

  if (writeHandle.record(rangeTrt, writeCdo).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandle.key()
                  << " with EventRange " << rangeTrt
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("recorded new CDO " << writeHandle.key() << " with range " << rangeTrt << " into Conditions Store");

  return StatusCode::SUCCESS;
  
}

void InDet::TRT_TrackSegmentsMakerCondAlg_ATLxk::magneticFieldInit()
{
  // Build MagneticFieldProperties 
  //
  if     (m_fieldmode == "NoField"    ) m_fieldprop = Trk::MagneticFieldProperties(Trk::NoField  );
  else if(m_fieldmode == "MapSolenoid") m_fieldprop = Trk::MagneticFieldProperties(Trk::FastField);
  else                                  m_fieldprop = Trk::MagneticFieldProperties(Trk::FullField);
}

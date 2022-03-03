/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentMomentum.h"
#include "MuonSegment/MuonSegment.h"
#include <sstream>
#include <iostream>
#include <vector>

MuonSegmentMomentum::MuonSegmentMomentum(const std::string& type,const std::string& name,const IInterface* parent)
  :
  AthAlgTool(type,name,parent)
{
  declareInterface<IMuonSegmentMomentumEstimator>(this);
}

void MuonSegmentMomentum::fitMomentumVectorSegments( const EventContext& ctx, const std::vector <const Muon::MuonSegment*> segments, double & signedMomentum ) const
{

  /** Estimate signed momentum from vector of MDT/CSC segments
      using fit to pairs of segments */

//  MsgStream log(messageService(),name());
  ATH_MSG_VERBOSE(" Executing MuonSegmentMomentumTool  fitMomentumVectorSegments ");
  ATH_MSG_DEBUG(" fitMomentumVectorSegments " << segments.size() << " segments ");

  std::vector<const Muon::MuonSegment*>::const_iterator it = segments.begin();
  std::vector<const Muon::MuonSegment*>::const_iterator it2 = segments.begin();
  std::vector<const Muon::MuonSegment*>::const_iterator it_end = segments.end();

  double pmin = 10000000000.;
  for(; it < it_end ; ++it )  {
    for(it2 = it; it2 < it_end ; ++it2 )  {
      if (it == it2) continue;
      double smom;
      fitMomentum2Segments(ctx, *it, *it2, smom);
      ATH_MSG_DEBUG(" Fit pair of segments signed momentum " << smom);
      if (fabs(smom) < fabs(pmin)) pmin = smom; 
    }
  }

  signedMomentum = pmin; 
  if (signedMomentum < 100. && signedMomentum > 0 ) signedMomentum = 100.; 
  if (signedMomentum > -100. && signedMomentum <= 0 ) signedMomentum = -100.; 

  ATH_MSG_DEBUG( " Estimated signed momentum " << signedMomentum);
}



void MuonSegmentMomentum::fitMomentum2Segments( const EventContext&, const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2, double & signedMomentum ) const 
{
  
  /** Estimate signed momentum for two segments
      by fitting 2 segments to one approximate track model */

//  MsgStream log(messageService(),name());
  ATH_MSG_VERBOSE(" Executing MuonSegmentMomentumTool  fitMomentum2Segments ");

  ATH_MSG_DEBUG(" fitMomentum2Segments");



  // Tracking matrices
  Amg::MatrixX model(5,3);// H projection matrix PDG
  model.setIdentity();
  Amg::MatrixX covT(3,3); // covariance or Track parameters here 3: z beam; r/z segment; tan/cot theta segment;
  covT.setIdentity();
  Amg::MatrixX v(3,1);// V = H^T cov^-1 r   cov-1 = covariance of measurements
  v.setIdentity();
  Amg::MatrixX t(3,1);// Track parameters
  t.setIdentity();
  Amg::MatrixX ym(5,1); // Vector of measurememns
  ym.setIdentity();
  // Extrapolation Matrices
  Amg::MatrixX resi(5,1); // residuals
  resi.setIdentity();

  // Geometry conventions
  double radius_cylinder = 4000.;
  double z_cylinder = 6000.;
  double z_end = 15000.;
  double cos_barrel = std::cos(std::atan2(radius_cylinder,z_cylinder));
  int imeth;

  // First Segment
  double thetas = segment1->globalDirection().theta();
  double rs = segment1->globalPosition().perp();
  double zs = segment1->globalPosition().z();
//   double phi  = segment1->globalDirection().phi();
//   double phisp  = atan2( segment1->globalPosition().y(), segment1->globalPosition().x());
  double thesp  = atan2( rs, zs);
  // angular resolution
  double era1 = 0.002;
  // Second Segment
  double thetase = segment2->globalDirection().theta();
  double rse = segment2->globalPosition().perp();
  double zse = segment2->globalPosition().z();
//   double phie  = segment2->globalDirection().phi();
//   double phispe  = atan2( segment2->globalPosition().y(), segment2->globalPosition().x());
  double thespe  = atan2( rse,zse);
  double era2 = 0.002;
  ATH_MSG_DEBUG(" radius1 " << rs << " radius2 " <<  rse << " z1 " << zs << " z2 " << zse << " theta1 " << thetas << " theta2 " << thetase << " thetasp1 " << thesp << " thetasp2 " << thespe);

  bool barrel = false;
  if (fabs(cos(thesp)) < cos_barrel || fabs(cos(thespe)) < cos_barrel ) barrel = true;
  bool forward = false;
  if (fabs(cos(thesp)) > cos_barrel || fabs(cos(thespe)) > cos_barrel ) forward = true;

  double scf = 20.;
  ATH_MSG_DEBUG(" error scaling in Curved fit " << scf);
  if (forward) scf = 2*scf;
  double scfn = 20.0;
  if (forward) scfn = 2*scfn;


// Error definitions

  //  double scf = 1; // pt = 100 GeV scf = 1;  pt 5 GeV scf = 20*20
  double ers2 = 0.1*0.1+scf*scf+scfn*scfn; //error squared position
  double ebs2 = 50*50*fabs(sin(thetas)*sin(thetase))+ers2; //error squared beam position
  double ets21 = era1*era1 + 0.002*0.002*scf*scf; // error squared angle
  double ets22 = era2*era2 + 0.002*0.002*scf*scf; // error squared angle

// No beam constraint for cosmics

  if(m_cosmics) ebs2 = 100000.*100000.;

  double sign = 1;
  if ( zs < 0 ) sign = -1.;

  if (barrel) {
    imeth = 0;
    // Barrel Track Model Matrix
    model(0,0) = 1.;
    model(1,0) = 1.;
    model(1,1) = rs;
    model(1,2) = (rs-radius_cylinder)*(rs-radius_cylinder);
    model(2,1) = 1.;
    model(2,2) = 2*(rs-radius_cylinder);
    model(3,0) = 1.;
    model(3,1) = rse;
    model(3,2) = (rse-radius_cylinder)*(rse-radius_cylinder);
    model(4,1) = 1.;
    model(4,2) = 2*(rse-radius_cylinder);
    // Measurements ym
    // correspondig squared errors: zs -> ers2  cot(thetas) ->ets2
    ym(0,0) = 0.;
    ym(1,0) = zs;
    ym(2,0)= cos(thetas)/sin(thetas);
    ym(3,0) = zse;
    ym(4,0)= cos(thetase)/sin(thetase);
  } else {
    imeth = 1;
    ATH_MSG_DEBUG(" forward fit " << cos(thetas));
    // Forward Track Model Matrix
    model(0,0) = 1.;
    model(1,0) = 1.;
    model(1,1) = zs;
    model(1,2) = sign*(zs-sign*z_cylinder)*(zs-sign*z_cylinder);
    model(2,1) = 1;
    model(2,2) = sign*2*(zs-sign*z_cylinder);
    model(3,0) = 1.;
    model(3,1) = zse;
    model(3,2) = sign*(zse-sign*z_cylinder)*(zse-sign*z_cylinder);
    model(4,1) = 1;
    model(4,2) = sign*2*(zse-sign*z_cylinder);

    if (fabs(zs) > z_end+2000) {
      model(1,2) = sign*(-z_end*z_end + z_cylinder*z_cylinder + 2*zs*sign*(z_end-z_cylinder)
                          +      (zs-sign*z_end)*(zs-sign*z_end)/5.);
      model(2,2) = sign*(2*(sign*z_end-sign*z_cylinder)
                          +     (zs-sign*z_end)/5.);
    }
    if (fabs(zse) > z_end+2000) {
      model(3,2) = sign*(-z_end*z_end + z_cylinder*z_cylinder + 2*zse*sign*(z_end-z_cylinder)
                          +      (zse-sign*z_end)*(zse-sign*z_end)/5.);
      model(4,2) = sign*(2*(sign*z_end-sign*z_cylinder)
                          +     (zse-sign*z_end)/5.);
    }
    // Measurements ym
    // correspondig squared errors: rs -> ers2  tan(thetas) ->ets2
    ym(0,0) = 0.;
    ym(1,0) = rs;
    ym(2,0)= tan(thetas);
    ym(3,0) = rse;
    ym(4,0)= tan(thetase);
    //   std::cout << " Forward zs " << zs << std::endl;
  }
  ATH_MSG_DEBUG(" distance segments " << sqrt((zs-zse)*(zs-zse)+(rs-rse)*(rs-rse)) );
  
  for(int i = 0; i <3 ; ++i )  {
    v(i,0)= model(0,i)*ym(0,0)/ebs2 + model(1,i)*ym(1,0)/ers2 + model(2,i)*ym(2,0)/ets21
      + model(3,i)*ym(3,0)/ers2 + model(4,i)*ym(4,0)/ets22;
  }

  Amg::MatrixX covTI(3,3); // covariance Inverse of Track parameters
  covTI.setIdentity();
  for(int i = 0; i <3 ; ++i )  {
    for(int j = 0; j <3 ; ++j )  {
      covTI(i,j) += model(0,i)*model(0,j)/ebs2;
      covTI(i,j) += model(1,i)*model(1,j)/ers2;
      covTI(i,j) += model(2,i)*model(2,j)/ets21;
      covTI(i,j) += model(3,i)*model(3,j)/ers2;
      covTI(i,j) += model(4,i)*model(4,j)/ets22;
    }
  }
  // Solution for Track parameters
  covT = covTI.inverse();
  t = covT*v;
  //   std::cout << " covariance at track 00 " << covT(0,0) << " 11 " << covT(1,1) << " 12 " << covT(1,2) << " 22 " << covT(2,2) << std::endl;

  double theta=0.; double invcurvature=0.;
  if (imeth == 0) {
    theta = atan2(1.,t(1,0));
    invcurvature = t(2,0)*sin(theta);
  } else if (imeth == 1) {
    theta = atan2(1.,1./t(1,0));
    invcurvature = -t(2,0)*fabs(cos(theta));
  }

  signedMomentum = (1./invcurvature)/10.;

  ATH_MSG_DEBUG(" MuonSegmentMomentum in MeV " << (1./invcurvature)/10. << " theta fit " << theta << " cos theta " << cos(theta) );

  // calculate residuals and chi2
  for(int i = 0; i <5 ; ++i )  {
    resi(i,0)= model(i,0)*t(0,0) + model(i,1)*t(1,0) + model(i,2)*t(2,0) - ym(i,0);
  }
  double chi2 = resi(0,0)*resi(0,0)/ebs2 + resi(1,0)*resi(1,0)/ers2 + resi(2,0)*resi(2,0)/ets21
    + resi(3,0)*resi(3,0)/ers2 + resi(4,0)*resi(4,0)/ets22;
  ATH_MSG_DEBUG("resi 00 " << resi(0,0) << " chi2 " << chi2);
  ATH_MSG_DEBUG(" Track parameters Matrix T00 " << t(0,0) << " T10 " << t(1,0) << " T20 " << t(2,0) );

  // Reshuffle residuals  res(0) -> segment position in model (1,x)
  //                      res(1) -> segment angle in model (2,x)

  Amg::VectorX res(4);
  for(int i = 0; i<4 ; ++i )  {
    res[i]= resi(i,0);
  }

  std::vector <double> pull(4);

  bool toobig = false;
  for(int i = 0; i <4 ; ++i )  {
    if ( i==0 ) pull[i] = res[i]/sqrt(ers2);
    if ( i==1 ) pull[i] = res[i]/sqrt(ets21);
    if ( i==2 ) pull[i] = res[i]/sqrt(ers2);
    if ( i==3 ) pull[i] = res[i]/sqrt(ets22);
    if ( fabs( pull[i] ) > 5 ) toobig = true;
  }

  if (toobig) {
    ATH_MSG_DEBUG(" Pull too BIGFIT " << " rad pos1 " << rs << " rad pos2 " << rse << " ang1 " << thetas << " ang2 " << thetase );
    double phisp  = atan2( segment1->globalPosition().y(), segment1->globalPosition().x());
    double phispe  = atan2( segment2->globalPosition().y(), segment2->globalPosition().x());
    ATH_MSG_DEBUG(" z pos1 " << zs << " z pos2 " << zse << " phi pos 1 " << phisp << " phi pos 2 " << phispe );

    Amg::Vector3D d1 = segment1->globalDirection();
    Amg::Vector3D d2 = segment2->globalDirection();
    ATH_MSG_DEBUG(" diff phi " << d1.x()*d2.y() - d1.y()*d2.x() );
  }
  ATH_MSG_DEBUG( " Fit2MomentumSegments: residual 0 " << res[0] << " pull 0 " << pull[0] << " residual 1 " << res[1] << " pull 1 " << pull[1] );
  ATH_MSG_DEBUG(" Fit2MomentumSegments: residual 2 " << res[2] << " pull 2 " << pull[2] << " residual 3 " << res[3] << " pull 3 " << pull[3] );
  ATH_MSG_DEBUG(" radius 1  " << ym(1,0) << " cottan theta 1 " << ym(2,0) << " radius 2  " << ym(3,0)
                       << " cottan theta 2 " << ym(4,0) );
  ATH_MSG_DEBUG(" radius fit 1  " << -(res[0]+ym(1,0)) << " cottan theta fit 1 " << -(res[1]+ym(2,0)) << " radius 2 fit  " << -(res[2]+ym(3,0))
                       << " cottan theta 2 fit " << -(res[3]+ym(4,0)) );
}





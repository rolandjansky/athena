/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentMomentum/MuonSegmentMomentumFromField.h"
#include "MuonSegment/MuonSegment.h"
#include <sstream>
#include <iostream>
#include <vector>
#include "TrkGeometry/MagneticFieldProperties.h"

#include "TrkExInterfaces/IPropagator.h"
#include "TrkExInterfaces/INavigator.h"
#include "TrkExUtils/TransportJacobian.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"

#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

MuonSegmentMomentumFromField::MuonSegmentMomentumFromField(const std::string& type,const std::string& name,const IInterface* 
							   parent):AthAlgTool(type,name,parent),
								   m_magFieldSvc("AtlasFieldSvc",name),
								   m_propagator("Trk::StraightLinePropagator/CosmicsPropagator"),
								   m_navigator("Trk::Navigator/CosmicsNavigator")
{
  declareInterface<IMuonSegmentMomentumEstimator>(this);


  m_debug = false;
  declareProperty("DoDebug",m_debug);

  m_summary = false;
  declareProperty("DoSummary",m_summary);

  m_cosmics = false;
  declareProperty("DoCosmics",m_cosmics);
  declareProperty("MagFieldSvc",    m_magFieldSvc );
  declareProperty("PropagatorTool",m_propagator);
  declareProperty("NavigatorTool",m_navigator);
  declareProperty("DoOld",m_doOld=false);
}

MuonSegmentMomentumFromField::~MuonSegmentMomentumFromField()
{
}

StatusCode MuonSegmentMomentumFromField::initialize()
{

  ATH_MSG_VERBOSE(" MuonSegmentMomentumFromField::Initializing ");

  ATH_CHECK( m_magFieldSvc.retrieve() );

  ATH_CHECK( m_propagator.retrieve() );

  ATH_CHECK( m_navigator.retrieve() );

  ATH_CHECK( detStore()->retrieve( m_cscid ) );

  ATH_CHECK( detStore()->retrieve( m_rpcid ) );

  ATH_CHECK( detStore()->retrieve( m_tgcid ) );

  ATH_MSG_VERBOSE("End of Initializing");  

  return StatusCode::SUCCESS; 
}

StatusCode MuonSegmentMomentumFromField::finalize()
{
  return StatusCode::SUCCESS;
}

void MuonSegmentMomentumFromField::fitMomentumVectorSegments( const std::vector <const Muon::MuonSegment*> segments, double & signedMomentum ) const
{

  /** Estimate signed momentum from vector of MDT/CSC segments
      using fit to pairs of segments */

  ATH_MSG_VERBOSE(" Executing MuonSegmentMomentumTool  fitMomentumVectorSegments ");
  if (m_debug||m_summary) std::cout << " fitMomentumVectorSegments " << segments.size() << " segments " << std::endl;

  std::vector<const Muon::MuonSegment*>::const_iterator it = segments.begin();
  std::vector<const Muon::MuonSegment*>::const_iterator it2 = segments.begin();
  std::vector<const Muon::MuonSegment*>::const_iterator it_end = segments.end();

  it2++;
  double maxintegral=0;  
  while (it2!=it_end){
    double integral= m_doOld ? fieldIntegralEstimate_old(*it,*it2) : fieldIntegralEstimate(*it,*it2);
    if (std::abs(integral)>maxintegral){
      maxintegral=std::abs(integral);
      if( m_doOld ) fitMomentum2Segments_old(*it, *it2, signedMomentum);
      else          fitMomentum2Segments(*it, *it2, signedMomentum);
    }
    it++;
    it2++;
  }


  if (m_debug||m_summary) std::cout << " Estimated signed momentum " << signedMomentum << std::endl;

}



double MuonSegmentMomentumFromField::fieldIntegralEstimate( const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2) const 
{

  Amg::Vector3D pos1=segment1->globalPosition();
  Amg::Vector3D pos2=segment2->globalPosition();
  Amg::Vector3D posdiff=pos2-pos1;
  if (posdiff.dot(segment1->globalDirection())<0){
    posdiff=-posdiff;
    pos1=segment2->globalPosition();
    pos2=segment1->globalPosition();
  }
  Amg::Vector3D point1=pos1+.25*posdiff;
  Amg::Vector3D point2=pos1+.5*posdiff;
  Amg::Vector3D point3=pos1+.75*posdiff;
  Amg::Vector3D field1,field2,field3;
  m_magFieldSvc->getField(point1.data(),field1.data()); // field in kiloTesla
  m_magFieldSvc->getField(point2.data(),field2.data());
  m_magFieldSvc->getField(point3.data(),field3.data());
  ATH_MSG_DEBUG("Mid Point " << Amg::toString(point2) << " field " << Amg::toString(field2) );
  field1[2]=field2[2]=field3[2]=0;
  Amg::Vector3D fieldsum=field1+field2+field3;
  //double averageBcrossl=(field1.cross(posdiff.unit()).mag()+field2.cross(posdiff.unit()).mag()+field3.cross(posdiff.unit()).mag())/3;
  Amg::Vector3D crossvec=posdiff.unit().cross(field1+field2+field3);
  Amg::Vector2D rphidir(-posdiff.y(),posdiff.x());
  double averagelcrossB=crossvec.mag()/3;
  if (rphidir.x()*fieldsum.x()+rphidir.y()*fieldsum.y()<0) averagelcrossB=-averagelcrossB;
  ATH_MSG_DEBUG("field integral " << averagelcrossB << "dist " << posdiff.mag() << " tot " << averagelcrossB*posdiff.mag());
  return averagelcrossB*posdiff.mag();
}

void MuonSegmentMomentumFromField::fitMomentum2Segments( const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2, double & signedMomentum ) const 
{
  
  /** Estimate signed momentum for two segments
      by fitting 2 segments to one approximate track model */

  ATH_MSG_VERBOSE(" Executing MuonSegmentMomentumTool  fitMomentum2Segments ");

  const Muon::MuonSegment* myseg1=segment1,*myseg2=segment2;
  if (myseg1->globalDirection().dot(myseg2->globalPosition()-myseg1->globalPosition())<0) {
    myseg1=segment2;
    myseg2=segment1;
  }
  double fieldintegral=fieldIntegralEstimate(myseg1,myseg2);
  double theta1=myseg1->globalDirection().theta();
  double theta2=myseg2->globalDirection().theta();
  double phi1=myseg1->globalDirection().phi();
  double phi2=myseg2->globalDirection().phi();
  double deltaphi=std::abs(phi2-phi1);
  double deltatheta=99999999;
  if (std::abs(deltaphi-2*M_PI)<deltaphi) deltaphi=std::abs(deltaphi-2*M_PI);
  if (deltaphi<M_PI/2) deltatheta=theta2-theta1;
  else {
    if (theta1>M_PI/2) deltatheta=2*M_PI-theta2-theta1;
    else deltatheta=-theta1-theta2;
  }
  double dist1=-1,dist2=-1;
  const Trk::RIO_OnTrack *firstphi1=0,*lastphi1=0,*firstphi2=0,*lastphi2=0;

  const Muon::MuonSegment *bestseg=myseg1,*worstseg=myseg2;
  for (int i=0;i<(int)myseg1->numberOfMeasurementBases();i++){
    const Trk::RIO_OnTrack *rot=dynamic_cast<const Trk::RIO_OnTrack *>(myseg1->measurement(i));
    if (!rot){
      const Trk::CompetingRIOsOnTrack *crot=dynamic_cast<const Trk::CompetingRIOsOnTrack *>(myseg1->measurement(i));
      if (crot) rot=&crot->rioOnTrack(0);
    }
    if (!rot) continue;
    Identifier id=rot->identify();

    if ((m_rpcid->is_rpc(id) && m_rpcid->measuresPhi(id)) || (m_cscid->is_csc(id) && m_cscid->measuresPhi(id)) || (m_tgcid->is_tgc(id) && m_tgcid->isStrip(id))){    
      if (!firstphi1) firstphi1=rot;
      lastphi1=rot;
    }
  }
  for (int i=0;i<(int)myseg2->numberOfMeasurementBases();i++){
    const Trk::RIO_OnTrack *rot=dynamic_cast<const Trk::RIO_OnTrack *>(myseg2->measurement(i));
    if (!rot){
      const Trk::CompetingRIOsOnTrack *crot=dynamic_cast<const Trk::CompetingRIOsOnTrack *>(myseg2->measurement(i));
      if (crot) rot=&crot->rioOnTrack(0);
    }
    if (!rot) continue;
    Identifier id=rot->identify();
    if ((m_rpcid->is_rpc(id) && m_rpcid->measuresPhi(id)) || (m_cscid->is_csc(id) && m_cscid->measuresPhi(id)) || (m_tgcid->is_tgc(id) && m_tgcid->isStrip(id))){
      if (!firstphi2) firstphi2=rot;
      lastphi2=rot;
    }
  }

  if (firstphi1) dist1=fabs((firstphi1->globalPosition()-lastphi1->globalPosition()).dot(myseg1->globalDirection()));
  if (firstphi2) dist2=fabs((firstphi2->globalPosition()-lastphi2->globalPosition()).dot(myseg2->globalDirection()));
  if (dist2>dist1) {
    bestseg=myseg2;
    worstseg=myseg1;
  }
  signedMomentum =-.3e3*fieldintegral/deltatheta;
  ATH_MSG_DEBUG("integral: " << fieldintegral << " deltatheta: " << deltatheta << " signedmomentum : " << signedMomentum);
  for (int i=0;i<3;i++){
    Trk::TransportJacobian *jac=0;
    Trk::AtaPlane startpar(bestseg->globalPosition(),bestseg->globalDirection().phi(),
			   bestseg->globalDirection().theta(),1/signedMomentum,bestseg->associatedSurface());
    const Trk::TrackParameters *par=m_propagator->propagateParameters(startpar,worstseg->associatedSurface(),
								      (bestseg==myseg1) ? Trk::alongMomentum : Trk::oppositeMomentum,false,
								      Trk::MagneticFieldProperties(Trk::FullField),jac,Trk::nonInteracting);
    if (m_debug) std::cout << "par: " << par << " jac: " << jac << std::endl;
    if (par && jac && (*jac)(1,4)!=0){
      double residual=worstseg->localParameters()[Trk::locY] - par->parameters()[Trk::locY];
      double delta_qoverp=residual/(*jac)(1,4);
      signedMomentum=1/(1/signedMomentum+delta_qoverp);
      ATH_MSG_DEBUG("residual: " << residual << " jac " << (*jac)(1,4) << " dp " << delta_qoverp << " signedmomentum: " << signedMomentum);
      delete par;
      delete jac;
      if (std::abs(residual)<1) break;
    }
  }
}


double MuonSegmentMomentumFromField::fieldIntegralEstimate_old( const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2) const 
{

  Amg::Vector3D pos1=segment1->globalPosition();
  Amg::Vector3D pos2=segment2->globalPosition();
  Amg::Vector3D posdiff=pos2-pos1;
  if (posdiff.dot(segment1->globalDirection())<0){
    posdiff=-posdiff;
    pos1=segment2->globalPosition();
    pos2=segment1->globalPosition();
  }
  Amg::Vector3D point1=pos1+.25*posdiff;
  Amg::Vector3D point2=pos1+.5*posdiff;
  Amg::Vector3D point3=pos1+.75*posdiff;
  Amg::Vector3D field1,field2,field3;
  m_magFieldSvc->getField((double*)&point1,(double*)&field1); // field in kiloTesla
  m_magFieldSvc->getField((double*)&point2,(double*)&field2);
  m_magFieldSvc->getField((double*)&point3,(double*)&field3);
  ATH_MSG_DEBUG("Mid Point " << Amg::toString(point2) << " field " << Amg::toString(field2) );
  double averageBcrossl=(field1.cross(posdiff.unit()).mag()+field2.cross(posdiff.unit()).mag()+field3.cross(posdiff.unit()).mag())/3;
  ATH_MSG_DEBUG("field integral " << averageBcrossl << "dist " << posdiff.mag() << " tot " << averageBcrossl*posdiff.mag());
  return averageBcrossl*posdiff.mag();
}

void MuonSegmentMomentumFromField::fitMomentum2Segments_old( const Muon::MuonSegment* segment1, const Muon::MuonSegment* segment2, double & signedMomentum ) const 
{
  
  /** Estimate signed momentum for two segments
      by fitting 2 segments to one approximate track model */

  ATH_MSG_VERBOSE(" Executing MuonSegmentMomentumTool  fitMomentum2Segments ");

  const Muon::MuonSegment* myseg1=segment1,*myseg2=segment2;
  if (myseg1->globalDirection().dot(myseg2->globalPosition()-myseg1->globalPosition())<0) {
    myseg1=segment2;
    myseg2=segment1;
  }
  double fieldintegral=fieldIntegralEstimate(myseg1,myseg2);
  double theta1=myseg1->globalDirection().theta();
  double theta2=myseg2->globalDirection().theta();
  double phi1=myseg1->globalDirection().phi();
  double phi2=myseg2->globalDirection().phi();
  double deltaphi=std::abs(phi2-phi1);
  double deltatheta=99999999;
  if (std::abs(deltaphi-2*M_PI)<deltaphi) deltaphi=std::abs(deltaphi-2*M_PI);
  if (deltaphi<M_PI/2) deltatheta=theta2-theta1;
  else {
    if (theta1>M_PI/2) deltatheta=2*M_PI-theta2-theta1;
    else deltatheta=-theta1-theta2;
  }
  double dist1=-1,dist2=-1;
  const Trk::RIO_OnTrack *firstphi1=0,*lastphi1=0,*firstphi2=0,*lastphi2=0;

  const Muon::MuonSegment *bestseg=myseg1,*worstseg=myseg2;
  for (int i=0;i<(int)myseg1->numberOfMeasurementBases();i++){
    const Trk::RIO_OnTrack *rot=dynamic_cast<const Trk::RIO_OnTrack *>(myseg1->measurement(i));
    if (!rot){
      const Trk::CompetingRIOsOnTrack *crot=dynamic_cast<const Trk::CompetingRIOsOnTrack *>(myseg1->measurement(i));
      if (crot) rot=&crot->rioOnTrack(0);
    }
    if (!rot) continue;
    Identifier id=rot->identify();

    if ((m_rpcid->is_rpc(id) && m_rpcid->measuresPhi(id)) || (m_cscid->is_csc(id) && m_cscid->measuresPhi(id)) || 
	(m_tgcid->is_tgc(id) && m_tgcid->isStrip(id))){    
      if (!firstphi1) firstphi1=rot;
      lastphi1=rot;
    }
  }
  for (int i=0;i<(int)myseg2->numberOfMeasurementBases();i++){
    const Trk::RIO_OnTrack *rot=dynamic_cast<const Trk::RIO_OnTrack *>(myseg2->measurement(i));
    if (!rot){
      const Trk::CompetingRIOsOnTrack *crot=dynamic_cast<const Trk::CompetingRIOsOnTrack *>(myseg2->measurement(i));
      if (crot) rot=&crot->rioOnTrack(0);
    }
    if (!rot) continue;
    Identifier id=rot->identify();
    if ((m_rpcid->is_rpc(id) && m_rpcid->measuresPhi(id)) || (m_cscid->is_csc(id) && m_cscid->measuresPhi(id)) || 
	(m_tgcid->is_tgc(id) && m_tgcid->isStrip(id))){
      if (!firstphi2) firstphi2=rot;
      lastphi2=rot;
    }
  }

  if (firstphi1) dist1=fabs((firstphi1->globalPosition()-lastphi1->globalPosition()).dot(myseg1->globalDirection()));
  if (firstphi2) dist2=fabs((firstphi2->globalPosition()-lastphi2->globalPosition()).dot(myseg2->globalDirection()));
  if (dist2>dist1) {
    bestseg=myseg2;
    worstseg=myseg1;
  }
  signedMomentum =-.3e3*fieldintegral/deltatheta;
  //std::cout << "integral: " << fieldintegral << " deltatheta: " << deltatheta << " signedmomentum : " << signedMomentum << std::endl;
  ATH_MSG_DEBUG("integral: " << fieldintegral << " deltatheta: " << deltatheta << " signedmomentum : " << signedMomentum);
  for (int i=0;i<3;i++){
    Trk::AtaPlane startpar(bestseg->globalPosition(),bestseg->globalDirection().phi(),bestseg->globalDirection().theta(),
			   1/signedMomentum,bestseg->associatedSurface());
    Trk::TransportJacobian *jac=0;
    const Trk::TrackParameters *par=m_propagator->propagateParameters(startpar,worstseg->associatedSurface(),
								      (bestseg==myseg1) ? Trk::alongMomentum : Trk::oppositeMomentum,false,
								      Trk::MagneticFieldProperties(Trk::FullField),jac,Trk::nonInteracting);
    if (par && jac && (*jac)(1,4)!=0){
      double residual=worstseg->localParameters()[Trk::locY] - par->parameters()[Trk::locY];
      double delta_qoverp=residual/(*jac)(1,4);
      signedMomentum=1/(1/signedMomentum+delta_qoverp);
      ATH_MSG_DEBUG("residual: " << residual << " jac " << (*jac)(1,4) << " dp " << delta_qoverp << " signedmomentum: " << signedMomentum);
      //std::cout << "residual: " << residual << " signedmomentum: " << signedMomentum << std::endl;
      delete par;
      delete jac;
      if (std::abs(residual)<1) break;
    }
  }
}

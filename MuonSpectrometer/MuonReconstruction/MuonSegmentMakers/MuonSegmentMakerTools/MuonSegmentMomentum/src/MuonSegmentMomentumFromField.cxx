/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentMomentumFromField.h"
#include "MuonSegment/MuonSegment.h"
#include <sstream>
#include <iostream>
#include <vector>
#include "TrkGeometry/MagneticFieldProperties.h"

#include "TrkExUtils/TransportJacobian.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"
#include <TString.h> // for Form

MuonSegmentMomentumFromField::MuonSegmentMomentumFromField(const std::string& type,const std::string& name,const IInterface* 
							   parent):AthAlgTool(type,name,parent)
{
  declareInterface<IMuonSegmentMomentumEstimator>(this);
}

StatusCode MuonSegmentMomentumFromField::initialize()
{
  ATH_MSG_VERBOSE(" MuonSegmentMomentumFromField::Initializing ");
  ATH_CHECK(AthAlgTool::initialize());
  ATH_CHECK(m_propagator.retrieve());
  ATH_CHECK(m_navigator.retrieve());
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_fieldCondObjInputKey.initialize());
  ATH_MSG_VERBOSE("End of Initializing");  
  return StatusCode::SUCCESS; 
}

void MuonSegmentMomentumFromField::fitMomentumVectorSegments( const std::vector <const Muon::MuonSegment*> segments, double & signedMomentum ) const
{

  /** Estimate signed momentum from vector of MDT/CSC segments
      using fit to pairs of segments */

  ATH_MSG_VERBOSE(" Executing MuonSegmentMomentumTool  fitMomentumVectorSegments ");
  ATH_MSG_DEBUG(" fitMomentumVectorSegments " << segments.size() << " segments " );

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
  ATH_MSG_DEBUG( " Estimated signed momentum " << signedMomentum );
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

  MagField::AtlasFieldCache fieldCache;
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, Gaudi::Hive::currentContext()};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (!fieldCondObj) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonSegmentMomentumFromField::fieldIntegralEstimate() - Failed to retrieve AtlasFieldCacheCondObj with key %s", __FILE__, __LINE__, (m_fieldCondObjInputKey.key()).c_str()));
  }
  fieldCondObj->getInitializedCache(fieldCache);

  fieldCache.getField(point1.data(),field1.data()); // field in kiloTesla
  fieldCache.getField(point2.data(),field2.data());
  fieldCache.getField(point3.data(),field3.data());
  ATH_MSG_DEBUG("Mid Point " << Amg::toString(point2) << " field " << Amg::toString(field2) );
  field1[2]=field2[2]=field3[2]=0;
  Amg::Vector3D fieldsum=field1+field2+field3;
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

    if ((m_idHelperSvc->isRpc(id) && m_idHelperSvc->rpcIdHelper().measuresPhi(id)) || (m_idHelperSvc->isCsc(id) && m_idHelperSvc->cscIdHelper().measuresPhi(id)) || (m_idHelperSvc->isTgc(id) && m_idHelperSvc->tgcIdHelper().isStrip(id))
        || (m_idHelperSvc->issTgc(id) && m_idHelperSvc->stgcIdHelper().measuresPhi(id) ) ){
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
    if ((m_idHelperSvc->isRpc(id) && m_idHelperSvc->rpcIdHelper().measuresPhi(id)) || (m_idHelperSvc->isCsc(id) && m_idHelperSvc->cscIdHelper().measuresPhi(id)) || (m_idHelperSvc->isTgc(id) && m_idHelperSvc->tgcIdHelper().isStrip(id))
        || (m_idHelperSvc->issTgc(id) && m_idHelperSvc->stgcIdHelper().measuresPhi(id) ) ){
      if (!firstphi2) firstphi2=rot;
      lastphi2=rot;
    }
  }
  bool flip = false;
  if (firstphi1) dist1=std::abs((firstphi1->globalPosition()-lastphi1->globalPosition()).dot(myseg1->globalDirection()));
  if (firstphi2) dist2=std::abs((firstphi2->globalPosition()-lastphi2->globalPosition()).dot(myseg2->globalDirection()));
  if (dist2>dist1) {
    flip = true;
    bestseg=myseg2;
    worstseg=myseg1;
  }
  
  for (int itry=0;itry<2;itry++){

    if(itry==1) {
// retry with other as best segmt segment
      if(!flip) {
        bestseg = myseg2;  
        worstseg=myseg1;
      } else {
        bestseg = myseg1;  
        worstseg=myseg2;
      }
    }

    signedMomentum =-.3e3*fieldintegral/deltatheta;
    if(std::abs(signedMomentum)<1000.) signedMomentum = 1e6;
    ATH_MSG_DEBUG("integral: " << fieldintegral << " deltatheta: " << deltatheta << " signedmomentum : " << signedMomentum);
    double residual = 9999.;
    double resi[4],qoverp[4];
    for (int i=0;i<4;i++){
      Trk::TransportJacobian *jac=0;
      Trk::AtaPlane startpar(bestseg->globalPosition(),bestseg->globalDirection().phi(),
 	 		     bestseg->globalDirection().theta(),1/signedMomentum,bestseg->associatedSurface());
      const Trk::TrackParameters *par=m_propagator->propagateParameters(startpar,worstseg->associatedSurface(),
								      (bestseg==myseg1) ? Trk::alongMomentum : Trk::oppositeMomentum,false,
								      Trk::MagneticFieldProperties(Trk::FullField),jac,Trk::nonInteracting);
      ATH_MSG_DEBUG("par: " << par << " jac: " << jac );
      if (par && jac && (*jac)(1,4)!=0){
        residual = worstseg->localParameters()[Trk::locY] - par->parameters()[Trk::locY];
        resi[i]   = residual; 
        qoverp[i] = 1/signedMomentum; 
// update qoverp
        double delta_qoverp=residual/(*jac)(1,4);
        double der_simple = -10.*(bestseg->globalPosition()-worstseg->globalPosition()).mag()/(.3*fieldintegral);
        if(bestseg->globalDirection().z()<0) der_simple = - der_simple;
        if(i>0) {
          if(qoverp[i]!=qoverp[i-1]) {
            double derivative = -(resi[i]-resi[i-1])/(qoverp[i]-qoverp[i-1]);
            ATH_MSG_DEBUG(" numerical derivative " << derivative << " derivative from track " << (*jac)(1,4) << " der_simple " << der_simple);
            if(std::abs(derivative)>std::abs((*jac)(1,4))) {
              ATH_MSG_DEBUG(" use numerical derivative " << derivative << " derivative from track " << (*jac)(1,4));
              delta_qoverp = residual/derivative;
            }
          }
        } else {
          if(std::abs(der_simple)>std::abs((*jac)(1,4))) {
            ATH_MSG_DEBUG(" use simple numerical derivative " << der_simple << " derivative from track " << (*jac)(1,4));
            delta_qoverp = residual/der_simple;
          }
        }
        ATH_MSG_DEBUG("residual: " << residual << " jac " << (*jac)(1,4) << " signedmomentum: " << signedMomentum << " delta_qoverp " << delta_qoverp);
        double signedMomentum_updated = signedMomentum/(1+signedMomentum*delta_qoverp);
        if(std::abs(signedMomentum_updated)<1000.) {
          ATH_MSG_DEBUG("Too low signed momentum " << signedMomentum_updated );
//      protect against too low momenta as propagation will fail
          signedMomentum = signedMomentum_updated>0? 1000.:-1000;
        } else {  
// do not change momentum beyond last iteration
          if(i<3) signedMomentum = signedMomentum_updated;
        }
        delete par;
        delete jac;
        if (std::abs(residual)<1) break;
      }
    }
    if(std::abs(residual)>10.) {
       ATH_MSG_DEBUG("NOT converged residual: " << residual << " itry " << itry);
       if(itry==1) ATH_MSG_DEBUG("NOT converged residual after two trials ");
    } else break;
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

  MagField::AtlasFieldCache fieldCache;
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, Gaudi::Hive::currentContext()};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};
  if (!fieldCondObj) {
    throw std::runtime_error(Form("File: %s, Line: %d\nMuonSegmentMomentumFromField::fieldIntegralEstimate_old() - Failed to retrieve AtlasFieldCacheCondObj with key %s", __FILE__, __LINE__, (m_fieldCondObjInputKey.key()).c_str()));
  }
  fieldCondObj->getInitializedCache(fieldCache);

  fieldCache.getField(point1.data(),field1.data()); // field in kiloTesla
  fieldCache.getField(point2.data(),field2.data());
  fieldCache.getField(point3.data(),field3.data());
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

    if ((m_idHelperSvc->isRpc(id) && m_idHelperSvc->rpcIdHelper().measuresPhi(id)) || (m_idHelperSvc->isCsc(id) && m_idHelperSvc->cscIdHelper().measuresPhi(id)) ||
	(m_idHelperSvc->isTgc(id) && m_idHelperSvc->tgcIdHelper().isStrip(id))){    
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
    if ((m_idHelperSvc->isRpc(id) && m_idHelperSvc->rpcIdHelper().measuresPhi(id)) || (m_idHelperSvc->isCsc(id) && m_idHelperSvc->cscIdHelper().measuresPhi(id)) ||
	(m_idHelperSvc->isTgc(id) && m_idHelperSvc->tgcIdHelper().isStrip(id))){
      if (!firstphi2) firstphi2=rot;
      lastphi2=rot;
    }
  }

  if (firstphi1) dist1=std::abs((firstphi1->globalPosition()-lastphi1->globalPosition()).dot(myseg1->globalDirection()));
  if (firstphi2) dist2=std::abs((firstphi2->globalPosition()-lastphi2->globalPosition()).dot(myseg2->globalDirection()));
  if (dist2>dist1) {
    bestseg=myseg2;
    worstseg=myseg1;
  }
  signedMomentum =-.3e3*fieldintegral/deltatheta;
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
      delete par;
      delete jac;
      if (std::abs(residual)<1) break;
    }
  }
}

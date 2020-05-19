/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentLocationFillerTool.h"
#include "TrkSegment/Segment.h"
#include "MuonSegment/MuonSegment.h"
#include "AthenaKernel/errorcheck.h"
#include "TrkEventPrimitives/LocalDirection.h"

namespace D3PD {

MuonSegmentLocationFillerTool::MuonSegmentLocationFillerTool (const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent)
  : BlockFillerTool<Trk::Segment> (type, name, parent),
    m_idToFixedIdTool("MuonCalib::IdToFixedIdTool"),
    m_slPropagator("Trk::StraightLinePropagator/MuonStraightLinePropagator"),
    m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator")
{
  book().ignore(); // Avoid coverity warnings.
}


StatusCode MuonSegmentLocationFillerTool::initialize()
{
  CHECK( BlockFillerTool<Trk::Segment>::initialize() );
  CHECK( m_edmHelperSvc.retrieve() );
  CHECK( m_idHelperSvc.retrieve() );
  CHECK( m_idToFixedIdTool.retrieve() );
  CHECK( m_slPropagator.retrieve() );
  CHECK( m_pullCalculator.retrieve() );
  m_magFieldProperties = new Trk::MagneticFieldProperties(Trk::FullField);
  return StatusCode::SUCCESS;
}

StatusCode MuonSegmentLocationFillerTool::book()
{
  CHECK( addVariable ("x",             m_x)  );
  CHECK( addVariable ("y",             m_y)  );
  CHECK( addVariable ("z",             m_z)  );
  CHECK( addVariable ("phi",           m_phi)  );
  CHECK( addVariable ("theta",         m_theta)  );
  CHECK( addVariable ("locX",          m_locX)  );
  CHECK( addVariable ("locY",          m_locY)  );
  CHECK( addVariable ("locAngleXZ",    m_thetaXZ)  );
  CHECK( addVariable ("locAngleYZ",    m_thetaYZ)  );
  CHECK( addVariable ("locAngleXZ_IP", m_thetaXZ_IP)  );
  CHECK( addVariable ("locAngleYZ_IP", m_thetaYZ_IP)  );
  CHECK( addVariable ("sector",        m_sector)  );
  CHECK( addVariable ("stationEta",    m_stationEta)  );
  CHECK( addVariable ("isEndcap",      m_isEndcap)  );
  CHECK( addVariable ("stationName",   m_stationName)  );
  CHECK( addVariable ("nphiHits",      m_nphiHits)  );
  CHECK( addVariable ("netaHits",      m_netaHits)  );
  CHECK( addVariable ("netaTrigHits",  m_netaTrigHits)  );
  CHECK( addVariable ("npadHits",      m_npadHits)  );
  CHECK( addVariable ("npseudoHits",   m_npseudoHits)  );
  CHECK( addVariable ("id",            m_id)  );
  CHECK( addVariable ("type",          m_type)  );
  CHECK( addVariable ("residual",      m_residual)  );
  CHECK( addVariable ("error",         m_error)  );
  CHECK( addVariable ("biasedpull",    m_biasedPull)  );
  CHECK( addVariable ("unbiasedpull",  m_unbiasedPull)  );

  return StatusCode::SUCCESS;
}


StatusCode MuonSegmentLocationFillerTool::fill (const Trk::Segment& ts) {
  const Muon::MuonSegment& mSeg = dynamic_cast<const Muon::MuonSegment&> (ts);

  // global position
  const Amg::Vector3D&  globalPos         = mSeg.globalPosition();
  *m_x = (float) globalPos.x();
  *m_y = (float) globalPos.y();
  *m_z = (float) globalPos.z();

  // global direction
  const Amg::Vector3D& globalDir         = mSeg.globalDirection();
  *m_phi = (float) globalDir.phi();
  *m_theta = (float) globalDir.theta();

  //local position
  Amg::Vector2D localPos;
  Amg::Vector3D momdum (0, 0, 0);
  
  if( mSeg.associatedSurface().globalToLocal(globalPos, momdum, localPos) )
  {
    *m_locX = (float) localPos.x();
    *m_locY = (float) localPos.y();
  }
  else{
    *m_locX = -99999.;
    *m_locY = -99999.;
  }

  //local direction
  const Trk::LocalDirection& localDir  = mSeg.localDirection();
  *m_thetaXZ = (float) localDir.angleXZ();
  *m_thetaYZ = (float) localDir.angleYZ();

  Amg::Vector3D pointingDir(globalPos.x(),globalPos.y(),globalPos.z());
  Trk::LocalDirection localIPDir;
  mSeg.associatedSurface().globalToLocalDirection(pointingDir, localIPDir);
  *m_thetaXZ_IP = (float) localIPDir.angleXZ();
  *m_thetaYZ_IP = (float) localIPDir.angleYZ();

  Identifier chid = m_edmHelperSvc->chamberId(mSeg);
  if( !(m_idHelperSvc->isMuon(chid)) ) { // bad chid
    *m_stationName = -1;
    *m_sector = -1;
    *m_stationEta = -1;
    *m_isEndcap = -1;
  } else {
    MuonCalib::MuonFixedId fid = m_idToFixedIdTool->idToFixedId( chid ) ;
    *m_stationName = fid.stationName();
    // chamber location information
    // should not happen...chamberId is supposed to return id only for MDT or CSC hits.
    if(!fid.is_tgc() ) *m_sector = m_idHelperSvc->sector(chid);
    else *m_sector = 0;
    *m_stationEta = m_idHelperSvc->stationEta(chid);
    *m_isEndcap = m_idHelperSvc->isEndcap(chid);
  }

  const Trk::AtaPlane* pars = m_edmHelperSvc->createTrackParameters( mSeg );
  if( !pars ) return StatusCode::SUCCESS;
  
  int nphiHits = 0;
  int netaHits = 0;
  int netaTrigHits = 0;
  int npadHits = 0;
  int npseudoHits = 0;

  const std::vector<const Trk::MeasurementBase*>& measurements = mSeg.containedMeasurements(); 
  for( std::vector<const Trk::MeasurementBase*>::const_iterator it = measurements.begin();it!=measurements.end();++it ){
    const Trk::MeasurementBase& meas = **it;
    Identifier id = m_edmHelperSvc->getIdentifier(meas);
    m_id->push_back(m_idToFixedIdTool->idToFixedId(id).getIdInt());
    int type = 6;
    if( id.is_valid() ){
      if( m_idHelperSvc->isMdt(id) )       type = 0;
      else if( m_idHelperSvc->isRpc(id) )  type = 1;
      else if( m_idHelperSvc->isTgc(id) )  type = 2;
      else if( m_idHelperSvc->isCsc(id) )  type = 3;
      else if( m_idHelperSvc->isMM(id)   ) type = 4;
      else if( m_idHelperSvc->issTgc(id) ) type = 5;
      
      if( m_idHelperSvc->issTgc(id) ){
	int chtype = m_idHelperSvc->stgcIdHelper().channelType(id);
	type += 1000*chtype;
	if( chtype == 0 )      ++npadHits;
	else if( chtype == 1 ) ++netaTrigHits;
	else                   ++nphiHits;
      }else if( !m_idHelperSvc->measuresPhi(id) ) {
	if( type == 1 || type ==3 ) ++netaTrigHits;
	else                        ++netaHits;
	type += 1000;
      }else{
	++nphiHits;
      }
    }else ++npseudoHits;

    m_type->push_back(type);

    *m_nphiHits = nphiHits;
    *m_netaHits = netaHits;
    *m_netaTrigHits = netaTrigHits;
    *m_npadHits = npadHits;
    *m_npseudoHits = npseudoHits;

    const Trk::TrackParameters* exPars = m_slPropagator->propagate(*pars,meas.associatedSurface(),Trk::anyDirection,false,*m_magFieldProperties);
    if( !exPars && meas.associatedSurface().isOnSurface(pars->position()) ){
      exPars = pars->clone();
    }
    float res = -99999;
    float pullub = -99999;
    float pullb  = -99999;
    if( exPars ){
      const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( &meas, exPars, Trk::ResidualPull::Biased );
      if( resPull ) {
	res = resPull->residual().front();
	pullub = resPull->pull().front();
	delete resPull;
      }else ATH_MSG_WARNING("Failed to calculate biased residual for " << m_idHelperSvc->toString(id) );

      resPull = m_pullCalculator->residualPull( &meas, exPars, Trk::ResidualPull::Unbiased );
      if( resPull ) {
	pullb = resPull->pull().front();
	delete resPull;
      }else ATH_MSG_WARNING("Failed to calculate biased residual for " << m_idHelperSvc->toString(id) );
      delete exPars;
    }else{
      ATH_MSG_WARNING("Failed to obtain track parameters for " << m_idHelperSvc->toString(id) );
    }
    m_residual->push_back(res);
    m_biasedPull->push_back(pullb);
    m_unbiasedPull->push_back(pullub);
  }
  delete pars;
  return StatusCode::SUCCESS;
}


} // namespace D3PD

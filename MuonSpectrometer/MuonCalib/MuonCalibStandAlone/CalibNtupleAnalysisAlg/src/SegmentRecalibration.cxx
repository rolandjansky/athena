/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//this
#include "CalibNtupleAnalysisAlg/SegmentRecalibration.h"

//MdtCalibData
#include "MdtCalibData/BFieldCorFunc.h"
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/MdtSlewCorFuncHardcoded.h"
#include "MdtCalibData/RtScaleFunction.h"


//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MuonCalibEvent.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "MuonCalibEventBase/MuonCalibTruthCollection.h"
#include "MuonCalibEventBase/MuonCalibMdtTruthHit.h"

//MdtCalibIOSvc
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"

namespace MuonCalib {

SegmentRecalibration::SegmentRecalibration(const std::string &t, const std::string &n, const IInterface *p): AthAlgTool(t, n, p), 
m_calib_input_svc("MdtCalibInputSvc", n) {
  m_t0_switch="LEAVE";
  declareProperty("T0Switch", m_t0_switch);
  m_B_corr_switch="LEAVE";
  declareProperty("BSwitch", m_B_corr_switch);
  m_recalc_r_switch=false;
  declareProperty("RecalcR", m_recalc_r_switch);
  m_use_true_r=false;
  declareProperty("UseTrueR", m_use_true_r);
  m_undo_refitted_t0 = -1;
  declareProperty("UndoRefittedT0", m_undo_refitted_t0);
  m_time_slwing_corr_switch="FILE";
  declareProperty("TimeSlewingSwitch", m_time_slwing_corr_switch);
  m_rt_scaling_switch="LEAVE";
  declareProperty("RtMultilayerScaling", m_rt_scaling_switch);
  p_ts_corr_func=NULL;
  m_prop_factor=1.0;
  declareProperty("PrpagationFactor", m_prop_factor);
  declareProperty("MdtCalibInputSvc", m_calib_input_svc);
  declareInterface< CalibSegmentPreparationTool >(this);
}  //end SegmentRecalibration::SegmentRecalibration
	
StatusCode SegmentRecalibration::initialize(void) {
  //get calibration input service
  ATH_CHECK( m_calib_input_svc.retrieve() );
//convert threefold switch settings
  if(!convert_switch(m_t0_switch, m_t0_numswitch, "t0")) {
    return StatusCode::FAILURE;
  }
  if(!convert_switch(m_B_corr_switch, m_B_corr_numswitch, "B-field")) {
    return StatusCode::FAILURE;
  }
  if(!convert_switch(m_time_slwing_corr_switch, m_time_slwing_numswitch, "time-slewing")) {
    return StatusCode::FAILURE;
  }
  if(!convert_switch(m_rt_scaling_switch, m_rt_scaling_numswitch, "multilayer-scaling")) {
    return StatusCode::FAILURE;
  }
  if(m_time_slwing_numswitch==2) {
    CalibFunc::ParVec pv;
    p_ts_corr_func = static_cast<IMdtSlewCorFunc *>(new MdtSlewCorFuncHardcoded(pv));
  }
  return StatusCode::SUCCESS;
}  //end SegmentRecalibration::SegmentRecalibration

void SegmentRecalibration::prepareSegments(const MuonCalibEvent *&event, std::map<NtupleStationId, MuonCalibSegment *> &segments) {
  const MdtStationT0Container *t0(NULL);
  const BFieldCorFunc *B_corr(NULL);
  const IRtResolution *spat_res(NULL);
  const IRtRelation *rt_relation(NULL);
  for(std::map<NtupleStationId, MuonCalibSegment *>::const_iterator it= segments.begin(); it!= segments.end(); it++) {
    for(unsigned int j=0; j<it->second->mdtHitsOnTrack(); j++) {
      //pointer and references
      MdtCalibHitBase *hit = it->second->mdtHOT()[j];
      if(m_time_slwing_numswitch>0) {
	hit->setDriftTime(hit->driftTime() + hit->slewingTime());
	hit->setSlewingTime(0);
      }
      if(m_prop_factor!=1.0) {
	float old_prop_time(hit->propagationTime());
	float new_prop_time(old_prop_time/m_prop_factor);
	hit->setPropagationTime(new_prop_time);
	hit->setDriftTime(hit->driftTime() + old_prop_time - new_prop_time);
      }
      if(m_time_slwing_numswitch==2) {
	hit->setSlewingTime(p_ts_corr_func->correction( hit->driftTime(), hit->adcCount()));
	hit->setDriftTime(hit->driftTime() - hit->slewingTime());
	//				std::cout<<hit->slewingTime()<<std::endl;
      }
      if(m_rt_scaling_numswitch>0 && hit->TemperatureTime() != MdtCalibHitBase::kNoValue) {
	hit->setDriftTime(hit->driftTime() + hit->TemperatureTime());
	hit->setTemperatureTime(0);
      }
      const MuonFixedId & id(hit->identify());
      //undo refitted t0
      if((m_undo_refitted_t0<0 && hit->segmentT0Applied()) || m_undo_refitted_t0>0) {
	hit->setDriftTime(hit->driftTime() + it->second->fittedT0());
      }
      //overwrite t0 correction
      if(m_t0_numswitch > 0 && hit->tubeT0() != MdtCalibHitBase::kNoValue) {
	hit->setDriftTime(hit->driftTime() + hit->tubeT0());
	hit->setTubeT0(0.0);
      }
      if(m_t0_numswitch ==2) {
	//get t0 if it was not already selected for this station
	if((t0=m_calib_input_svc->GetT0(id)) == NULL) {
	  ATH_MSG_FATAL( "No t0 correction loaded for this chamber!" );
	  event = NULL;
	  return;
	}
	hit->setDriftTime(hit->driftTime() - t0->t0(id.mdtMultilayer(), id.mdtTubeLayer(), id.mdtTube()));
	hit->setTubeT0(t0-> t0(id.mdtMultilayer(), id.mdtTubeLayer(), id.mdtTube()));
      }
      //overwrite magnetic field correction of selected
      if(m_B_corr_numswitch > 0 && hit->bFieldTime() != MdtCalibHitBase::kNoValue) {
	hit->setDriftTime(hit->driftTime() + hit->bFieldTime());
      }
      if(m_B_corr_numswitch == 2) {
	if((B_corr = m_calib_input_svc->GetBCorr(id)) == NULL) {
	  ATH_MSG_FATAL( "No rt relation loaded for this chamber!" );
	  event=NULL;
	  return;
	}
	hit->setBFieldTime(B_corr-> correction(hit->driftTime(), hit->bFieldPara(), hit->bFieldPerp()));
	hit->setDriftTime(hit->driftTime() - hit->bFieldTime());
      }
      if(m_rt_scaling_numswitch==2) {
	if((rt_relation = m_calib_input_svc->GetRtRelation(id)) == NULL) {
	  ATH_MSG_FATAL( "No rt relation loaded for this chamber! Needed for scaling!" );
	  event=NULL;
	  return;
	}
	double corr_val=RtScaleFunction(hit->driftTime(), id.mdtMultilayer()==2, *rt_relation);
	hit->setTemperatureTime(corr_val);
	hit->setDriftTime(hit->driftTime() - corr_val);
      }
      
      //overwrite drift radius if selected
      if(m_recalc_r_switch) {
	if((rt_relation = m_calib_input_svc->GetRtRelation(id)) == NULL) {
	  ATH_MSG_FATAL( "No rt relation loaded for this chamber!" );
	  event=NULL;
	  return;
	}
	if((spat_res = m_calib_input_svc->GetResolution(id)) == NULL) {
	  ATH_MSG_FATAL( "No rt relation loaded for this chamber!" );
	  event=NULL;
	  return;
	}
	double dt=hit->driftTime();
	hit->setDriftRadius( rt_relation->radius(dt), spat_res->resolution(dt));
      } else {
	//if drift radius is not recalculated re,ove sign from radius
	hit-> setDriftRadius(std::fabs(hit-> driftRadius()), hit->sigmaDriftRadius());
      }
      if (m_use_true_r)	{
	double r=9e9;
	const MuonCalibTruthCollection* tk= event->calibTruthCollection();
	for(MuonCalibTruthCollection::MdtTruthVecCit t_it=tk->mdtTruthCollectionBegin(); t_it!=tk->mdtTruthCollectionEnd(); t_it++) {
	  const MuonCalibMdtTruthHit *thit=*t_it;
	  if(hit->identify() == thit->identify()) {
	    r=thit->driftRadius();
	    break;
	  }
	}
	if(r<8e8) {
	  hit->setDriftRadius(std::fabs(r), hit->sigmaDriftRadius());
	} else {
	  hit->setDriftRadius(9e9, 9e9);
	}
      }
    }
    if(m_undo_refitted_t0) {
      it->second->setFittedT0(0.0);
    }
  }
}  //end SegmentRecalibration::prepareSegments

inline bool SegmentRecalibration::convert_switch(const std::string &s_switch, int &num_switch, const char *switchname) const	{
  if(s_switch=="LEAVE")	{
    num_switch=0;
    return true;
  }
  if(s_switch=="UNAPPLY") {
    num_switch=1;
    return true;
  }
  if(s_switch=="FILE") {
    num_switch=2;
    return true;
  }
  ATH_MSG_FATAL( "Invalid switch setting "<<s_switch<< " for "<< switchname<<"!" );
  return false;
}  //end SegmentRecalibration::convert_switch

} //namespace MuonCalib

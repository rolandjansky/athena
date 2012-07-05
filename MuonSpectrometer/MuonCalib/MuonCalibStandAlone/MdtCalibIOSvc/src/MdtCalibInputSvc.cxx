/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//c - c++
#include "fstream"
#include <sys/types.h>
#include <dirent.h>
#include "iostream"
#include "list"

// Gaudi //
#include "GaudiKernel/MsgStream.h"

//MdtCalibData
#include "MdtCalibData/IRtRelation.h"
#include "MdtCalibData/IRtResolution.h"
#include "MdtCalibData/BFieldCorFunc.h"
#include "MdtCalibData/RtFromPoints.h"

//MuonCalibMath
#include "MuonCalibMath/SamplePoint.h"
#include "MdtCalibData/RtResolutionFromPoints.h"

//this
#include "MdtCalibIOSvc/MdtCalibInputSvc.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/MdtStationT0Container.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include "MuonCalibStandAloneBase/RegionSelectionSvc.h"

#include "MuonCalibIdentifier/MuonFixedId.h"

MdtCalibInputSvc ::MdtCalibInputSvc(const std::string & name, ISvcLocator *svc_locator) : Service(name, svc_locator), m_calib_input_tool("MuonCalib::CalibrationDummyIOTool")
	{
	declareProperty("CalibrationInputTool", m_calib_input_tool);
	p_sel_region_res=NULL;
	p_sel_region_rt=NULL;
	p_sel_region_b=NULL;
	p_reg_sel_svc=NULL;
	}


MdtCalibInputSvc ::~MdtCalibInputSvc()
	{
	std::map<MuonCalib::NtupleStationId, MuonCalib::MdtStationT0Container *> :: iterator it1;
	for(it1=m_t0.begin(); it1!=m_t0.end(); it1++)
		delete it1->second;
	std::map<MuonCalib::NtupleStationId, MuonCalib::IRtRelation *> :: iterator it2;
	for(it2=m_rt_relation.begin(); it2!=m_rt_relation.end(); it2++)
		delete it2->second;
	std::map<MuonCalib::NtupleStationId, MuonCalib::BFieldCorFunc *> :: iterator it3;
	for(it3=m_B_corr.begin(); it3!=m_B_corr.end(); it3++)
		delete it3->second;
	std::map<MuonCalib::NtupleStationId, MuonCalib::IRtResolution *> :: iterator it4;
	for(it4=m_spat_res.begin(); it4!=m_spat_res.end(); it4++)
		delete it4->second;
	}


StatusCode MdtCalibInputSvc :: initialize()
	{
 	MsgStream log(messageService(), name());
  	if(!Service::initialize().isSuccess())
		{
		log << MSG::FATAL <<"Failed to initialize service"<<endreq;
		return StatusCode::FAILURE;
		}
	log<< MSG::INFO << "initialize()" << endreq;
//get region selection service
	StatusCode sc=service("RegionSelectionSvc", p_reg_sel_svc);
	if(!sc.isSuccess() || p_reg_sel_svc==NULL)
		{
		log << MSG::ERROR <<"Cannot retrieve RegionSelectionSvc!" <<endreq;
		return sc;
		}	
	sc=m_calib_input_tool.retrieve();
	if(!sc.isSuccess())
		{
		log << MSG::ERROR <<"Cannot retrieve inpput tool!" <<endreq;
		return sc;
		}
	sc=read_calib_input();
	return sc;
	}

StatusCode MdtCalibInputSvc :: finalize(void) 
	{
	return StatusCode::SUCCESS;
	}

StatusCode MdtCalibInputSvc::queryInterface(const InterfaceID& riid, 
							void** ppvUnknown) {
	std::cout<<"StatusCode MdtCalibInputSvc::queryInterface"<<std::endl;

	if (IID_IMdtCalibInputSvc.versionMatch(riid)) { 
		*ppvUnknown = (MdtCalibInputSvc *)this; 
	} else { 
		return Service::queryInterface(riid, ppvUnknown); 
	}

	return StatusCode::SUCCESS;
}


const MuonCalib::MdtStationT0Container * MdtCalibInputSvc :: GetT0(const MuonCalib::NtupleStationId & id) const
	{
	std::map<MuonCalib::NtupleStationId, MuonCalib::MdtStationT0Container *> :: const_iterator it;
	if((it=m_t0.find(id)) == m_t0.end())
		{
		MuonCalib::NtupleStationId chamber_id=id;
		chamber_id.SetMultilayer(0);
		if((it=m_t0.find(chamber_id)) == m_t0.end())
			{
			if(m_t0_warned.find(chamber_id) == m_t0_warned.end())
				{
				MsgStream log(messageService(), name());
				log<< MSG::WARNING << "T0 not loaded for station " << id.regionId() <<endreq;
				m_t0_warned.insert(chamber_id);
				}
			return NULL;
			}
		}
	return it->second;
	}


const MuonCalib::IRtRelation * MdtCalibInputSvc :: GetRtRelation(const MuonCalib::NtupleStationId & id) const
	{
	std::map<MuonCalib::NtupleStationId, MuonCalib::IRtRelation *> :: const_iterator it;
	if((it=m_rt_relation.find(id)) == m_rt_relation.end())
		{
		MuonCalib::NtupleStationId chamber_id=id;
		chamber_id.SetMultilayer(0);
		if((it=m_rt_relation.find(chamber_id)) == m_rt_relation.end())
			{
			if(m_rt_warned.find(chamber_id) == m_rt_warned.end())
				{
				MsgStream log(messageService(), name());
				log<< MSG::WARNING << "Rt relation not loaded for station" << chamber_id.regionId() <<endreq;
				m_rt_warned.insert(chamber_id);
				}
			return NULL;
			}
		}
	return it->second;
	}


const MuonCalib::BFieldCorFunc * MdtCalibInputSvc :: GetBCorr(const MuonCalib::NtupleStationId & id) const
	{
	const MuonCalib::BFieldCorFunc * fun;
	if((fun=findbfieldfun(id))!=NULL)
		return fun;
	MuonCalib::NtupleStationId chamber_id(id);
	chamber_id.SetMultilayer(0);
	return findbfieldfun(chamber_id);
	}

const MuonCalib::IRtResolution * MdtCalibInputSvc :: GetResolution(const MuonCalib::NtupleStationId & id) const
	{
	std::map<MuonCalib::NtupleStationId, MuonCalib::IRtResolution *>:: const_iterator it;
	if((it=m_spat_res.find(id)) == m_spat_res.end())
		{
		MuonCalib::NtupleStationId chamber_id=id;
		chamber_id.SetMultilayer(0);
		if((it=m_spat_res.find(chamber_id)) == m_spat_res.end())
			{
			if(m_rt_warned.find(chamber_id) == m_rt_warned.end())
				{
				MsgStream log(messageService(), name());
				log<< MSG::FATAL << "Rt relation not loaded for station" <<endreq;
				m_rt_warned.insert(chamber_id);
				}
			return NULL;
			}
		}
	return it->second;
	}



inline StatusCode MdtCalibInputSvc :: read_calib_input()
	{
	MsgStream log(messageService(), name());
	StatusCode sc=m_calib_input_tool->LoadT0(m_t0, -1);
	if(!sc.isSuccess())
		{
		log << MSG::ERROR << "Cannot read t0s" <<endreq;
		return sc;
		}
	sc=m_calib_input_tool->LoadRt(m_rt_relation, m_spat_res, -1);
	if(!sc.isSuccess())
		{
		log << MSG::ERROR << "Cannot read rts" <<endreq;
		return sc;
		}
	create_mean_rts();
	return sc;
	}


inline bool MdtCalibInputSvc::create_b_field_correction(const MuonCalib::NtupleStationId &id) const
	{
	std::map<MuonCalib::NtupleStationId, MuonCalib::IRtRelation *> :: const_iterator it(m_rt_relation.find(id));
	if (it==m_rt_relation.end()) return false;
	MsgStream log(messageService(), name());
	log<< MSG::INFO << "Initiailizing B-Field correction for " << id.regionId() <<endreq;	
	const MuonCalib::IRtRelation *rt_rel(it->second);
// create magnetic field correction
	std::vector<double> corr_params(2);
	corr_params[0] = 3080.0; // high voltage
	corr_params[1] = 0.11; // epsilon parameter
	m_B_corr[id] = new MuonCalib::BFieldCorFunc(corr_params, rt_rel);	
	return true;
	}

inline void MdtCalibInputSvc::create_mean_rts()
	{
	std::cout<<"MdtCalibInputSvc::create_mean_rts()"<<std::endl;
	MsgStream log(messageService(), name());
	std::list<const MuonCalib::IRtRelation *> matching_relations;
	std::list<MuonCalib::NtupleStationId> matching_ids;
//loop over all rts
	for(std::map<MuonCalib::NtupleStationId, MuonCalib::IRtRelation *>::const_iterator it = m_rt_relation.begin(); it!=m_rt_relation.end(); it++)
		{
		MuonCalib::MuonFixedId id;
	//if the rt relation is stored per chamber, check both multilayers
		if(it->first.GetMl()==0)
			{
			id.set_mdt();
			for(int i=1; i<=2; i++)
				{
				id.setStationName(it->first.GetStation());
				id.setStationEta(it->first.GetEta());
				id.setStationPhi(it->first.GetPhi());
				id.setMdtMultilayer(i);
				if(p_reg_sel_svc->isInRegion(id))
					{
					matching_relations.push_back( it->second);
					matching_ids.push_back(it->first);
					break;
					}
				}
			}
		else
			{
			id.setStationName(it->first.GetStation());
			id.setStationEta(it->first.GetEta());
			id.setStationPhi(it->first.GetPhi());
			id.setMdtMultilayer(it->first.GetMl());
			if(p_reg_sel_svc->isInRegion(id))
				{
				matching_relations.push_back( it->second);
				matching_ids.push_back(it->first);
				}
			}
		}
	log << MSG::INFO <<"Found "<<matching_relations.size()<< " rt-relations for calibration region"<<endreq;
	if(matching_relations.size()==0) return;
//averageing over rt relations is not yet implemented - take the first found
	if(matching_relations.size()>1)
		{
		log << MSG::WARNING <<"More than one rt relation for this region loaded! Taking first!" << endreq;
		}
	p_sel_region_rt = *(matching_relations.begin());
	m_mean_station_id = *(matching_ids.begin());
//	p_sel_region_b = GetBCorr(*(matching_ids.begin()));
	p_sel_region_b = NULL;
	p_sel_region_res = GetResolution(*(matching_ids.begin()));
	}
	
inline const MuonCalib::BFieldCorFunc * MdtCalibInputSvc :: findbfieldfun(const MuonCalib::NtupleStationId & id) const
	{
	std::map<MuonCalib::NtupleStationId, MuonCalib::BFieldCorFunc *> :: const_iterator it(m_B_corr.find(id));
	if(it==m_B_corr.end())
		{
		if(!create_b_field_correction(id))
			return NULL;
		return findbfieldfun(id);
		}
	return it->second;
	}

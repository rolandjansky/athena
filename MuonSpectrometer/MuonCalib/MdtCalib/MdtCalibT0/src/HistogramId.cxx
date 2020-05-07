/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MdtCalibT0/HistogramId.h"
#include "MuonCalibIdentifier/MuonFixedId.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <algorithm>
#include "iostream"
#include "sstream"
#include "iomanip"

namespace MuonCalib {

std::map<std::pair<int, int>, std::string> HistogramId :: s_histogram_names;


void HistogramId::Initialize(const MuonFixedId &id, int sort_by)
	{
   m_id.first=sort_by;
   static const unsigned int kStationNameShift     = 24;

   static const unsigned int kStationEtaShift         = 19;

   static const unsigned int kStationPhiShift         = 13;


//   Mdt specific code 

   static const unsigned int kMdtMultilayerShift   = 9;

   static const unsigned int kMdtTubeLayerShift    = 7;

//   static const unsigned int kMdtTubeShift         = 0;
	switch(sort_by)
		{
		case TUBE:
			{
			m_id.second = id.getIdInt();
			break;
			}
		case LAYER:
			{
			m_id.second  = (id.stationNameIndex()<<kStationNameShift) | (id. etaIndex()<<kStationEtaShift) | (id.phiIndex()<<kStationPhiShift) | (id.mdtMultilayerIndex()<<kMdtMultilayerShift) | (id.mdtTubeLayerIndex()<<kMdtTubeLayerShift);
			break;
			}
		case MULTILAYER:
			{
			m_id.second  = (id.stationNameIndex()<<kStationNameShift) | (id. etaIndex()<<kStationEtaShift) | (id.phiIndex()<<kStationPhiShift) | (id.mdtMultilayerIndex()<<kMdtMultilayerShift);
			break;
			}
		case CHAMBER:
			{
			m_id.second  = (id.stationNameIndex()<<kStationNameShift) | (id. etaIndex()<<kStationEtaShift) | (id.phiIndex()<<kStationPhiShift);
			break;
			}
		case MEZZ_CARD:
			{
		//first part is like multilayer
			m_id.second  = id.mdtMezzanine();
			break;
			}
		default:
			{
				MsgStream log(Athena::getMessageSvc(),"HistogramId");
				log<<MSG::WARNING<<"HistogramId :: Initialize: sort_by arguemnt is invalid!"<<endmsg;
			}
		}
//create histogram name 
	if(s_histogram_names[m_id] == "")
		{
		std::ostringstream os;
		if(m_id.second==-999999)
			{
			os<<"Summary";
			}
		else
			{
	//chamber name
			os << id.stationNumberToFixedStationString(id.stationName()) <<"_eta";
			if(id.eta()<0) os<<"C";
			else os<<"A";
			os<<std::abs(id.eta())<<"_phi"<<id.phi();
		//multilayer name
			if(sort_by != CHAMBER)
				os<<"_ml"<<id.mdtMultilayer();
		//layer name
			if(sort_by != CHAMBER && sort_by != MULTILAYER && sort_by!=MEZZ_CARD)
				os<<"_ly"<<id.mdtTubeLayer();
		//tube name
			if(sort_by == TUBE)
				os<<"_tb"<<id.mdtTube();
		//mezz-id
			if(sort_by == MEZZ_CARD)
				{
			
				os<<"_mez"<<(id.mdtMezzanine() %100);
				}
		//numeric_id
			os<<"_num"<<m_id.second<<"_"<<sort_by;
			
			}
		//store
		s_histogram_names[m_id] = os.str();
		MsgStream log(Athena::getMessageSvc(),"HistogramId");
		log<<MSG::INFO<<sort_by<<" "<<s_histogram_names[m_id]<<endmsg;
		}
	}





}

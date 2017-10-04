/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//c - c++
#include "iostream"

//root
#include "TDirectory.h"
#include "TH1.h"

//MuonCalibEventBase
#include "MuonCalibEventBase/MuonCalibSegment.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"

//this
#include "MuonCalibStandAloneTools/HitCounter.h"

namespace MuonCalib {

bool HitCounter :: Initialize(const NtupleStationId &id)
	{
	m_id=id;
	if(id.NMultilayers() == -1)
		{
		std::cerr<<"HitCounter :: Initialize:Geometry for idenifier not initialized!"<<std::endl;
		return false;
		}
	m_per_chamber=0;
//	std::cout<<"NMultilayers = "<<id.NMultilayers()<<std::endl;
	for(int i=0; i<id.NMultilayers(); i++)
		{
		m_per_ml[i]=0;
		m_mezz_width[i]=8;
		if(id.NLayers(i)==4) m_mezz_width[i]=6;
		TubeId tid;
		tid.ml=i;
//		std::cout<<"NLayers = "<<id.NLayers(i)<<std::endl;
//		std::cout<<"NTubes = "<<id.NTubes(i)<<std::endl;
		for(tid.ly=0; tid.ly<id.NLayers(i); tid.ly++)
			for(tid.tb=0; tid.tb<id.NTubes(i); tid.tb++)
				{
				m_per_tube[tid]=0;
				}
		MezzId mid;
		mid.ml=i;
		for(mid.mezz=0; mid.mezz<id.NTubes(i)/m_mezz_width[i]; mid.mezz++)
			{
			m_per_mezz[mid]=0;
			}
		}
	m_n_dead_tubes=0;
	m_n_dead_mezz=0;
	m_n_dead_ml=0;	
	m_name = id.regionId();	
	m_is_initialized=true;
	m_wd = gDirectory->mkdir(m_name.c_str(), m_name.c_str());
//	m_wd->Write();	
	m_wd->cd();
	m_hits_per_segments = new TH1F("hits_per_segments", "", 433, -0.5, 432.5);
	return true;
	}


void HitCounter :: ProcessSegment(const MuonCalibSegment & segment)
	{
	m_hits_per_segments->Fill(static_cast<Axis_t>(segment.hitsOnTrack()));
	MuonCalibSegment :: MdtHitCit it(segment.mdtHOTBegin());
	for(; it!=segment.mdtHOTEnd(); it++)
		{
		m_per_chamber++;
		TubeId tid;
		if (!(m_id==(*it)->identify())) continue;
		NtupleStationId id((*it)->identify());
//		std::cout<<id.regionId()<<std::endl;
		tid.ml = (*it)->identify().mdtMultilayer()-1;
//		std::cout<<"tid.ml="<<tid.ml<<std::endl;
		tid.ly = (*it)->identify().mdtTubeLayer()-1;
//		std::cout<<"tid.ly="<<tid.ly<<std::endl;
		tid.tb = (*it)->identify().mdtTube()-1;
//		std::cout<<"tid.tb="<<tid.tb<<std::endl;
		m_per_tube[tid]=m_per_tube[tid]+1;
		MezzId mid;
		mid.ml=tid.ml;
//		std::cout<<m_mezz_width[tid.ml]<<std::endl;
		mid.mezz=tid.tb/m_mezz_width[tid.ml];
//		std::cout<<"mid.mezz="<<mid.mezz<<std::endl;
		m_per_mezz[mid]=m_per_mezz[mid]+1;
		m_per_ml[tid.ml]=m_per_ml[tid.ml]+1;
//		std::cout<<"."<<std::endl;
		}
	}

const std::string & HitCounter :: FittingBy(int min_hits, double bad_fit_rate)
	{
	m_wd->cd();
//try per tube
	double n_bad(0.0), n_total(0.0);
	Axis_t index(0.0);
	TH1F *tubes = new TH1F("Tubes", "Tubes", m_per_tube.size(), -0.5, m_per_tube.size() - 0.5);
	TH1F *tubes_index = new TH1F("tubes_index", "", 576, -0.5,575.5);
	for (int i=1; i<=576; i++)
		tubes_index->SetBinContent(i, -1.0);
	m_fit_by_int = -1;
	for(std::map<TubeId, int> :: const_iterator it=m_per_tube.begin(); it!=m_per_tube.end(); it++)
		{
		if(it->second>0)
			{
			n_total++;
			if(it->second<min_hits) n_bad++;
			}
		else
			{
			m_n_dead_tubes++;
			}
		tubes->Fill(index, it->second);
		index++;
		int fix_index = it->first.tb + 72*(it->first.ly) + 288*(it->first.ml);
//		std::cout<<it->first.tb<<" "<<it->first.ly<<" "<<it->first.ml<<std::endl;
		tubes_index->SetBinContent(fix_index + 1, it->second);
		}
	tubes->SetEntries(m_per_chamber);
	if(n_bad/n_total <= bad_fit_rate)
		{
		m_fit_by="TUBE";
		m_fit_by_int = 1;
		}
//try per mezzanine
	n_bad=0; n_total=0;
	index=0.0;
	TH1F *mezzanaines = new TH1F("Mezzanine", "Mezzanine", m_per_mezz.size(), -0.5, m_per_mezz.size() - 0.5);
	for(std::map<MezzId, int> :: const_iterator it=m_per_mezz.begin(); it!=m_per_mezz.end(); it++)
		{
		if(it->second > 0)
			{
			n_total++;
			if(it->second<min_hits) n_bad++;
			}
		else
			{
			m_n_dead_mezz++;
			}
		mezzanaines->Fill(index, it->second);
		index++;
		}
	mezzanaines->SetEntries(m_per_chamber);
	if(n_bad/n_total <= bad_fit_rate && m_fit_by_int == -1)
		{
		m_fit_by="MEZZ_CARD";
		m_fit_by_int=2;
		}
//try per multilayer
	n_bad=0; n_total=0;
	index=0.0;
	TH1F *mls = new TH1F("Multilayers", "Mulitlayers", m_per_ml.size(), -0.5, m_per_ml.size() - 0.5);
	for(std::map<int, int> :: const_iterator it=m_per_ml.begin(); it!=m_per_ml.end(); it++)
		{
		if(it->second > 0)
			{
			n_total++;
			if(it->second<min_hits) n_bad++;
			}
		else
			{
			m_n_dead_ml++;
			}
		mls->Fill(index, it->second);
		index++;
		}
	mls->SetEntries(m_per_chamber);
	if(n_bad/n_total <= bad_fit_rate && m_fit_by_int == -1)
		{
		m_fit_by="MULTILAYER";
		m_fit_by_int = 3;
		}
//	m_wd->Write();	
//try total
	if(m_per_chamber>=min_hits &&m_fit_by_int == -1 )
		{
		m_fit_by="CHAMBER";
		m_fit_by_int = 4;
		return m_fit_by;
		}
	if(m_fit_by_int == -1)
		{
		m_fit_by="NOFIT";
		}
	return m_fit_by;
	}

std::map<HitCounter::TubeId, HitCounter::DQInfo> HitCounter :: InitialDQ(int noisy_tube_factor) const
	{
	double mean_nhits(0.0);
	double n(0.0);
	for(std::map<TubeId, int> :: const_iterator it=m_per_tube.begin(); it!=m_per_tube.end(); it++)
		{
		if(it->second>0)
			{
			mean_nhits+=1.0/static_cast<double>(it->second);
			n++;
			}
		}
	mean_nhits=n/mean_nhits;
//	std::cout<<mean_nhits<<std::endl;
	std::map<TubeId, DQInfo> ret;
	for(std::map<TubeId, int> :: const_iterator it=m_per_tube.begin(); it!=m_per_tube.end(); it++)
		{
		if(it->second == 0) 
			ret[it->first].SetDead();
		if(it->second > n * noisy_tube_factor * mean_nhits)
			{
			 ret[it->first].SetNoisy();
//			 std::cout<<mean_nhits<<" "<<it->second<<std::endl;
//			std::cout<<it->first.ml<<" "<<it->first.ly<<" "<<it->first.tb<<std::endl;
			}
		}
	return ret;
	}

}

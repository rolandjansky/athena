/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/HIEventShapeSummaryTool.h"
#include "HIEventUtils/HIEventDefs.h"
#include "HIEventUtils/HICaloRange.h"

#include <sstream>
#include <iostream>
#include <iomanip>

const std::vector<std::string> HIEventShapeSummaryTool::s_SubCaloNames {"EMBarrel", "EMEC", "HEC", "TileBarrel", "TileGap", "TileExt", "FCal", "EMCal", "Tile","HCal", "PreSampler","ALL"};
const std::vector<std::initializer_list<int> > HIEventShapeSummaryTool::s_SubCaloLists{HI::SubCalo::Lists::EMBarrel,
    HI::SubCalo::Lists::EMEC,
    HI::SubCalo::Lists::HEC,
    HI::SubCalo::Lists::TileBarrel,
    HI::SubCalo::Lists::TileGap,
    HI::SubCalo::Lists::TileExt,
    HI::SubCalo::Lists::FCal,
    HI::SubCalo::Lists::EMCal,
    HI::SubCalo::Lists::Tile,
    HI::SubCalo::Lists::HCal,
    HI::SubCalo::Lists::PreSampler,
    HI::SubCalo::Lists::ALL};

HIEventShapeSummaryTool::HIEventShapeSummaryTool(const std::string& n) : asg::AsgTool(n)
{
  declareProperty("Samplings",m_samp_names );
  declareProperty("SubCalos",m_subcalo_names);
  declareProperty("DoPositiveNegativeSides",m_do_sides=false);
  if(initialize().isFailure()) ATH_MSG_WARNING("Initial configuration of tool failed");

}

StatusCode HIEventShapeSummaryTool::summarize(const xAOD::HIEventShapeContainer* in, xAOD::HIEventShapeContainer* out) const
{
  out->reserve(m_summary_list.size());
  for(const auto& itr : m_summary_list)
  {
    xAOD::HIEventShape* es=new xAOD::HIEventShape();
    out->push_back(es);
    HI::fillSummary(in,es,itr.second.func);
    es->setEtaMin(roundToTenth(itr.second.eta_min));
    es->setEtaMax(roundToTenth(itr.second.eta_max));
    es->setLayer(itr.second.layer);
    es->auxdata<std::string>("Summary")=itr.second.name;
  }
  return StatusCode::SUCCESS;
}
  
StatusCode HIEventShapeSummaryTool::initialize()
{
  //not optimal, but minimal code re-use, gets called only at initialization
  ATH_MSG_INFO("Configuring for summary");
  for(const auto& s : m_samp_names)
  {
    auto mItr=m_summary_list.find(s);
    if(mItr!=m_summary_list.end()) continue;
    int samp=CaloSampling::getNumberOfSamplings();
    for(unsigned int j=0; j<CaloSampling::getNumberOfSamplings(); j++)
    {
      if(s.compare(CaloSampling::getSamplingName(j))==0)
      {
	samp=j;
	break;
      }
    }
    if(samp == static_cast<int>(CaloSampling::getNumberOfSamplings()))
    {
      ATH_MSG_WARNING("Sampling with name " << s << " not defined in standard list, skipping.");
      break;
    }
    float emin=HICaloRange::getRange().getRangeMin(samp);
    float emax=HICaloRange::getRange().getRangeMax(samp);
    m_summary_list.emplace(s,summary_info_t(s,emin,emax,samp,HI::ByLayer(samp)));
    if(m_do_sides)
    {
      m_summary_list.emplace(s+"_pos",summary_info_t(s+"_pos",emin,emax,samp,HI::ByLayer(samp)));
      m_summary_list.emplace(s+"_neg",summary_info_t(s+"_neg",-emax,-emin,samp,HI::ByLayer(samp)));
    }
  }

  for(const auto& s : m_subcalo_names)
  {
    auto mItr=m_summary_list.find(s);
    if(mItr!=m_summary_list.end()) continue;

    unsigned int isub=s_SubCaloNames.size();
    for(unsigned int i=0; i < s_SubCaloNames.size(); i++)
    {
      if(s_SubCaloNames.at(i)==s) 
      {
	isub=i;
	break;
      }
    }

    if(isub==s_SubCaloNames.size()) 
    {
      ATH_MSG_WARNING("Subcalo with name " << s << " not defined in standard list, skipping." );
      break;
    }


    const std::vector<int> layer_list=s_SubCaloLists.at(isub);
    int samp=getSubCaloLayer(layer_list);
    float emin=getSubCaloEtaMin(layer_list);
    float emax=getSubCaloEtaMax(layer_list);

    m_summary_list.emplace(s,summary_info_t(s,emin,emax,samp,HI::ByLayers(s_SubCaloLists.at(isub))));
    if(m_do_sides)
    {
      m_summary_list.emplace(s+"_pos",summary_info_t(s+"_pos",emin,emax,samp,HI::ByLayersP(s_SubCaloLists.at(isub))));
      m_summary_list.emplace(s+"_neg",summary_info_t(s+"_neg",-emax,-emin,samp,HI::ByLayersN(s_SubCaloLists.at(isub))));
    }

  }

  ATH_MSG_DEBUG(dumpList());
  return StatusCode::SUCCESS;
}


float HIEventShapeSummaryTool::getSubCaloEtaMin(const std::vector<int>& samps) const
{
  float min=10;
  for(const auto i : samps)
  {
    float min_c=HICaloRange::getRange().getRangeMin(i);
    if(min_c < min) min=min_c;
  }
  return min;
}

float HIEventShapeSummaryTool::getSubCaloEtaMax(const std::vector<int>& samps) const
{
  float max=0.;
  for(const auto i : samps)
  {
    float max_c=HICaloRange::getRange().getRangeMax(i);
    if(max_c > max) max=max_c;
  }
  return max;
}

int HIEventShapeSummaryTool::getSubCaloLayer(const std::vector<int>& samps) const
{
  int outlayer=0;
  for(const auto i : samps) outlayer |= 0x1 << i;
  return outlayer;
}

std::string HIEventShapeSummaryTool::dumpList() const
{
  std::stringstream ss;
  ss << "========= Dumping HIEventShapeSummaryTool ========" << std::endl;
  ss << std::setw(15) << "Name"
     << std::setw(15) << "eta min"
     << std::setw(15) << "eta max"
     << std::setw(15) << "layer"
     << std::endl;
  for(const auto& itr : m_summary_list)
  {
    ss << std::setw(15) << itr.second.name
       << std::setw(15) << itr.second.eta_min
       << std::setw(15) << itr.second.eta_max
       << std::setw(15) << itr.second.layer
       << std::endl;
  }
  return ss.str();
}

float HIEventShapeSummaryTool::roundToTenth(float v) const
{
  return std::floor(v*10.)/10.;
}
  

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/HIEventShapeIndex.h"
#include "HIEventUtils/HICaloRange.h"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iterator>

HIEventShapeIndex::HIEventShapeIndex() : m_shape_container(nullptr),
					 m_size(0)
{
}

HIEventShapeIndex::~HIEventShapeIndex() 
{
}
unsigned int HIEventShapeIndex::setBinning(const xAOD::HIEventShapeContainer* shape)
{
  if(shape!=nullptr && shape!=m_shape_container)
  {
    //ATH_MSG_WARNING("Resetting binning on different HIEventShapeContainer");
  } 
  else if(shape==nullptr) 
  {
    //ATH_MSG_ERROR("Bad HIEventShapePointer.");
    return 0;
  }
  else if(getNumBins()==shape->size())
  {
    //ATH_MSG_INFO("Binning already synched this with HIEventShapeContainer.");
    return getNumBins();
  }
								      
  //assume contiguous non-overlapping eta bins
  unsigned int index=0;
  for(; index<shape->size(); index++)
  {
    const xAOD::HIEventShape* s=shape->at(index);
    int layer=s->layer();
    float eta_min=s->etaMin();
    float eta_max=s->etaMax();

    auto mItr=m_edges.emplace_hint(m_edges.end(),layer,std::vector<range_index_t>());
    mItr->second.push_back(range_index_t(eta_min,eta_max,index));
  }
  for(auto pp : m_edges) std::sort( pp.second.begin(),pp.second.end() );
  m_shape_container=shape;
  m_size=index;
  return index;

}

unsigned int HIEventShapeIndex::setBinning(const TH2* h2, bool asMask)
{
  if(h2==nullptr) 
  {
    //ATH_MSG_ERROR("Bad histogram pointer");
    return 0;
  }

  unsigned int count=0;
  std::set<unsigned int> used_indices;
  for(int ybin=1; ybin<h2->GetNbinsY(); ybin++)
  {
    auto mItr=m_edges.emplace_hint(m_edges.end(),ybin-1,std::vector<range_index_t>());
    for(int xbin=1; xbin<h2->GetNbinsX(); xbin++)
    {
      if(h2->GetBinContent(xbin,ybin) < 0) continue;
      unsigned int v=h2->GetBinContent(xbin,ybin);

      if(asMask) v=count;
      else
      {
	auto sItr=used_indices.find(v);
	if(sItr==used_indices.end()) used_indices.insert(v);
	else
	{
	  //ATH_MSG_ERROR("Could not set binning. initialization histogram has duplicate indices " << v);
	  //m_edges.clear();
	  //return 0;
	  continue;
	}
      }
      mItr->second.push_back(range_index_t(h2->GetXaxis()->GetBinLowEdge(xbin),h2->GetXaxis()->GetBinUpEdge(xbin),v));
      count++;
    }
  } 
  m_size=count;
  return count;
}

unsigned int HIEventShapeIndex::setBinning(HIEventShapeIndex::BinningScheme scheme)
{
  unsigned int index=0;
  if(scheme==TOWER)
  {
    for(unsigned int layer=0; layer < static_cast<int>(HI::TowerBins::numLayers()); layer++)
    {
      auto mItr=m_edges.emplace_hint(m_edges.end(),layer,std::vector<range_index_t>(HI::TowerBins::numEtaBins(),range_index_t()));
      for(unsigned int eb=0; eb<HI::TowerBins::numEtaBins(); eb++,index++) 
      {
	
	mItr->second[eb].eta_min=HI::TowerBins::getBinLowEdgeEta(eb);
	mItr->second[eb].eta_max=HI::TowerBins::getBinUpEdgeEta(eb);
	mItr->second[eb].index=index;
      }
    }
  }
  else if(scheme==COMPACT)
  {
    float deta=HI::TowerBins::getBinSizeEta();
    for(int layer=0; layer < static_cast<int>(HI::TowerBins::numLayers()); layer++)
    {
      float emax=HICaloRange::getRange().getRangeMax(layer);
      float emin=HICaloRange::getRange().getRangeMin(layer);
      unsigned int num_eta_bins= static_cast<unsigned int>(std::round(std::abs((emax-emin)/deta)));
      auto mItr=m_edges.emplace_hint(m_edges.end(),layer,std::vector<range_index_t>(2*num_eta_bins,range_index_t()));
      for(unsigned int k=0; k<num_eta_bins; k++)
      {
	mItr->second[k].eta_min=roundToTenth(-emax+k*deta);
	mItr->second[k].eta_max=roundToTenth(-emax+k*deta+deta);
	mItr->second[k].index=roundToTenth(k+index);
	mItr->second[k+num_eta_bins].eta_min=roundToTenth(emin+k*deta);
	mItr->second[k+num_eta_bins].eta_max=roundToTenth(emin+k*deta+deta);
	mItr->second[k+num_eta_bins].index=roundToTenth(k+num_eta_bins+index);
      }
      index+=2*num_eta_bins;
    }
  }
  m_size=index;
  return index;

}

unsigned int HIEventShapeIndex::getIndexFromBin(unsigned int ebin, int layer) const
{
  return getLayer(layer)->second.at(ebin).index;
}

unsigned int HIEventShapeIndex::getIndex_Internal(float eta, int layer, bool etaIndex) const
{
  auto mItr=getLayer(layer);
  const std::vector<range_index_t>& vec=mItr->second;
  unsigned int vsize=vec.size();

  if(vsize==0)
  {
    std::cerr << "Layer " << layer << " has no range in calorimeter" << std::endl;
    throw std::range_error("Bad binning request");
    return 0;
  }

  float eta_c=eta;
  if(m_edges.size()>1)
  {
    float abs_eta=std::abs(eta);
    float rmin=HICaloRange::getRange().getRangeMin(layer);
    float rmax=HICaloRange::getRange().getRangeMax(layer);
    if(abs_eta < rmin) eta_c=rmin-std::copysign(1e-2,eta);
    else if(abs_eta > rmax) eta_c=rmax-std::copysign(1e-2,eta);
  }

  unsigned int pIndex=0;
  for(; pIndex < vsize-1; pIndex++)
  {
    const range_index_t& p=vec.at(pIndex);
    if(p(eta_c)) break;
  }
  return (etaIndex) ? pIndex : vec.at(pIndex).index;

  
  // std::vector<range_index_t>::iterator r=std::lower_bound(vec.begin(),vec.end(),range_index_t(eta_c,eta_c+1,0));
  // return (etaIndex) ? std::distance(vec.begin(),r) : r->index();
  
  
}


unsigned int HIEventShapeIndex::getIndex(float eta, int layer) const
{
  return getIndex_Internal(eta,layer,false);
}

unsigned int HIEventShapeIndex::getEtaBin(float eta, int layer) const
{
  return getIndex_Internal(eta,layer,true);
}

xAOD::HIEventShape* HIEventShapeIndex::getShape(float eta, int layer, xAOD::HIEventShapeContainer* shape_container) const
{

  if(shape_container==nullptr) 
  {
    //ATH_MSG_WARNING("No default HIEventShapeContainer to query. Returning nullptr_t.");
    return nullptr;
  }
  unsigned int bin=getIndex(eta,layer);
  //correct for round-off errors in cell eta values, just need to make sure they end up in the right bin
  if( bin >= shape_container->size() )  bin=getIndex(eta-std::copysign(1e-2,eta),layer);

  return shape_container->at(bin);
}

const xAOD::HIEventShape* HIEventShapeIndex::getShape(float eta, int layer, const xAOD::HIEventShapeContainer* shape_container) const
{

  if(shape_container==nullptr) 
  {
    //ATH_MSG_WARNING("No default HIEventShapeContainer to query. Returning nullptr_t.");
    return nullptr;
  }
  unsigned int bin=getIndex(eta,layer);
  //correct for round-off errors in cell eta values, just need to make sure they end up in the right bin
  if( bin >= shape_container->size() )  bin=getIndex(eta-std::copysign(1e-2,eta),layer);

  return shape_container->at(bin);
}

std::map<int,std::vector<HIEventShapeIndex::range_index_t> >::const_iterator HIEventShapeIndex::getLayer(int layer) const
{
  auto mItr=m_edges.find(layer);

  if(mItr==m_edges.end())
  {
    //ATH_MSG_ERROR("Layer " << layer << " has no binning specification");
    std::cerr << "Layer " << layer << " has no binning specification" << std::endl;
    throw std::range_error("Bad binning request");
  }
  return mItr;
}

void HIEventShapeIndex::initializeEventShapeContainer(xAOD::HIEventShapeContainer* shape_container, unsigned int num_harmonics) const
{
  if(shape_container==nullptr)
  {
    //ATH_MSG_WARNING("Bad pointer to HIEventShapeContainer. Cannot inititalize its elements.");
    return;
  }
  if(getNumBins()==0)
  {
    //ATH_MSG_WARNING("Cannot initialize HIEventShapeContainer. Index has not been initialized.");
    return;

  }
  bool isEmpty=(shape_container->size()==0);
  if(isEmpty) shape_container->resize(getNumBins());
  else if(shape_container->size()!=getNumBins())
  {
    //ATH_MSG_WARNING("Cannot initialize HIEventShapeContainer. Number of elements incompatible w/ requested binning.");
    return;
    
  }
  
  for(auto pp : m_edges)
  {
    for(auto ri : pp.second)
    {
      xAOD::HIEventShape* slice;
      if(isEmpty) 
      {
	slice=new xAOD::HIEventShape();
	//shape_container->push_back(slice);
	shape_container->operator[](ri.index)=slice;
      }
      else slice=shape_container->at(ri.index);

      slice->setEtaMin(ri.eta_min);
      slice->setEtaMax(ri.eta_max);
      slice->setLayer(pp.first);
      slice->etCos().assign(num_harmonics,0);
      slice->etSin().assign(num_harmonics,0);
    }
  }
}

// HIEventShapeIndex& HIEventShapeIndex::operator=(const HIEventShapeIndex& in)
// {
//   m_edges=in.m_edges;
//   m_shape_container=in.m_shape_container;
//   m_size=in.m_size;
//   return (*this);
// }

std::string HIEventShapeIndex::print() const
{
  std::stringstream ss;
  ss.precision(3);
  for(auto pp : m_edges)
  {
    for(auto ri : pp.second)
    {
      ss << std::setw(10) << pp.first
	 << std::setw(10) << ri.eta_min
	 << std::setw(10) << ri.eta_max
	 << std::setw(10) << ri.index
	 << std::endl;
    }
  }
  return ss.str();
}

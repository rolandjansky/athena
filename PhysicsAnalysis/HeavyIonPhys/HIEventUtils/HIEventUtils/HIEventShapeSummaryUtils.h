/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HIEVENTSHAPESUMMARYUTILS_H
#define HIEVENTUTILS_HIEVENTSHAPESUMMARYUTILS_H

#include "xAODHIEvent/HIEventShapeContainer.h"
#include "xAODHIEvent/HIEventShape.h"
#include <functional>
#include <vector>
#include <set>

namespace HI
{

  void fillSummary(const xAOD::HIEventShapeContainer* in, xAOD::HIEventShape* out,
		   std::function<bool (const xAOD::HIEventShape*)> incFunction,
		   std::function<void (xAOD::HIEventShape*,const xAOD::HIEventShape*)> addFunction);

  void fillSummary(const xAOD::HIEventShapeContainer* in, xAOD::HIEventShape* out,
		   std::function<bool (const xAOD::HIEventShape*)> incFunction);


  void fillSummary(const xAOD::HIEventShapeContainer* in, xAOD::HIEventShape* out,
		   const std::set<unsigned int> indices,
		   std::function<void (xAOD::HIEventShape*,const xAOD::HIEventShape*)> addFunction);

  struct AddEventShape
  {

    bool match_num_harmonics;

    AddEventShape(float ww) :  match_num_harmonics(true), m_weight(ww){};
    constexpr AddEventShape() : match_num_harmonics(true), m_weight(1){};
    void operator()(xAOD::HIEventShape* slice, const xAOD::HIEventShape* in_slice) const;

  private:
    float m_weight;
  };

  constexpr AddEventShape AddES=AddEventShape();


  // Predicates

  struct ByLayer
  {
    int layer;
    ByLayer(int ll) : layer(ll){};
    bool operator()(const xAOD::HIEventShape* in_slice) {return (in_slice->layer()==layer);};
  };

  struct ByLayers
  {
    std::initializer_list<int> layers;
    constexpr ByLayers(std::initializer_list<int> ll) : layers(ll) {};
    bool operator()(const xAOD::HIEventShape* in_slice)
    {
      for(int i : layers)
      {
	if(i==in_slice->layer()) return true;
      }
      return false;
    };
  };

  struct ByEtaRange
  {
    float eta_min;
    float eta_max;
    constexpr ByEtaRange(float eta_min_i, float eta_max_i) : eta_min(eta_min_i), eta_max(eta_max_i){};
    bool operator()(const xAOD::HIEventShape* in_slice) {return (in_slice->etaMin() > eta_min && in_slice->etaMax() < eta_max);};
  };

  struct ByLayersWithEtaRange
  {
    ByLayers layers_sel;
    ByEtaRange range_sel;

    constexpr ByLayersWithEtaRange(ByLayers& ll,float eta_min_i, float eta_max_i) : layers_sel(ll), range_sel(eta_min_i,eta_max_i){};
    bool operator()(const xAOD::HIEventShape* in_slice) {return (layers_sel(in_slice) && range_sel(in_slice));};
  };

  struct ByLayersP
  {
    ByLayers layers_sel;
    ByEtaRange range_sel;
    constexpr ByLayersP(std::initializer_list<int> ll) : layers_sel(ll), range_sel(-1e-4,1000){};
    bool operator()(const xAOD::HIEventShape* in_slice) {return (layers_sel(in_slice) && range_sel(in_slice));};
  };

  struct ByLayersN
  {
    ByLayers layers_sel;
    ByEtaRange range_sel;
    constexpr ByLayersN(std::initializer_list<int> ll) : layers_sel(ll), range_sel(-1000,1e-4){};
    bool operator()(const xAOD::HIEventShape* in_slice) {return (layers_sel(in_slice) && range_sel(in_slice));};
  };

  float getModulation(const xAOD::HIEventShape* es, const std::vector<unsigned int> harmonics, float phi);
  int setHarmonics(std::vector<unsigned int>& in);
}

#endif

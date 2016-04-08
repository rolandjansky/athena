/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIEventUtils/HIEventShapeSummaryUtils.h"
#include "CxxUtils/sincos.h"

namespace HI
{
  void AddEventShape::operator()(xAOD::HIEventShape* slice, const xAOD::HIEventShape* in_slice) const
  {
    //update members
    slice->setNCells(slice->nCells()+weight*in_slice->nCells());
    slice->setEt(slice->et()+weight*in_slice->et());
    slice->setArea(slice->area() + weight*in_slice->area());
    slice->setRho(slice->rho() + weight*in_slice->rho());

    unsigned int nord=slice->etCos().size();
    if(nord!=slice->etSin().size()) throw std::domain_error("Input HIEventShape has unequal n-harmonics for Q_x and Q_y");

    //if output shape is empty, copy directly from input
    if(nord==0)
    {
      slice->etCos().assign(in_slice->etCos().begin(),in_slice->etCos().end());
      slice->etSin().assign(in_slice->etSin().begin(),in_slice->etSin().end());
      return;
    }

    unsigned int in_nord=in_slice->etCos().size();
    //can only do addition for harmonics in input slice, print warning?
    if(in_nord < nord) nord=in_nord;
    else if(in_nord > nord && match_num_harmonics)
    {
      //print a warning?
      auto itr=in_slice->etCos().begin();
      std::advance(itr,nord);
      slice->etCos().insert(slice->etCos().end(),itr,in_slice->etCos().end());
      itr=in_slice->etSin().begin();
      std::advance(itr,nord);
      slice->etSin().insert(slice->etSin().end(),itr,in_slice->etSin().end());
    }
    //only need to sum to nord, if prev condition was met i>nord set explicitly by insert
    for(unsigned int i=0; i<nord; i++)
    {
      float tmp_cos = slice->etCos().at(i);
      slice->etCos()[i] = tmp_cos + weight*in_slice->etCos().at(i);
      float tmp_sin = slice->etSin().at(i);
      slice->etSin()[i] = tmp_sin + weight*in_slice->etSin().at(i);
    }
  }


  void fillSummary(const xAOD::HIEventShapeContainer* in, xAOD::HIEventShape* out,
		   std::function<bool (const xAOD::HIEventShape*)> incFunction,
		   std::function<void (xAOD::HIEventShape*,const xAOD::HIEventShape*)> addFunction)
  {
    for(const auto sItr : *in )
    {
      if(incFunction(sItr)) addFunction(out,sItr);
    }
  }

  void fillSummary(const xAOD::HIEventShapeContainer* in, xAOD::HIEventShape* out,
		   const std::set<unsigned int> indices,
		   std::function<void (xAOD::HIEventShape*,const xAOD::HIEventShape*)> addFunction)
  {
    for(const auto i : indices ) addFunction(out,in->at(i));
  } 


  void fillSummary(const xAOD::HIEventShapeContainer* in, xAOD::HIEventShape* out,
		   std::function<bool (const xAOD::HIEventShape*)> incFunction)
  {
    for(const auto sItr : *in )
    {
      if(incFunction(sItr)) HI::AddES(out,sItr);
    }
  }

  float getModulation(const xAOD::HIEventShape* es, const std::vector<unsigned int> harmonics, float phi)
  {
    float mod=1;
    for(const auto itr : harmonics)
    {
      CxxUtils::sincos sc (itr*phi);
      mod+=sc.apply(es->etSin().at(itr-1),es->etCos().at(itr-1));
    }
    return mod;
  }
  int setHarmonics(std::vector<unsigned int>& in)
  {
    for(unsigned int i=0; i<in.size(); i++)
    {
      int harmonic=in[i]-1; //v_0=1 by construction
      if(harmonic < 0) return i;
      in[i]=harmonic;
    }
    return -1;
  }
}

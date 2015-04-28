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
    slice->setNcells(slice->nCells()+weight*in_slice->nCells());
    slice->setEt(slice->Et()+weight*in_slice->Et());
    slice->setArea(slice->area() + weight*in_slice->area());
    slice->setRho(slice->rho() + weight*in_slice->rho());

    unsigned int nord=slice->Et_cos().size();
    if(nord!=slice->Et_sin().size()) throw std::domain_error("Input HIEventShape has unequal n-harmonics for Q_x and Q_y");

    //if output shape is empty, copy directly from input
    if(nord==0)
    {
      slice->Et_cos().assign(in_slice->Et_cos().begin(),in_slice->Et_cos().end());
      slice->Et_sin().assign(in_slice->Et_sin().begin(),in_slice->Et_sin().end());
      return;
    }

    unsigned int in_nord=in_slice->Et_cos().size();
    //can only do addition for harmonics in input slice, print warning?
    if(in_nord < nord) nord=in_nord;
    else if(in_nord > nord && match_num_harmonics)
    {
      //print a warning?
      auto itr=in_slice->Et_cos().begin();
      std::advance(itr,nord);
      slice->Et_cos().insert(slice->Et_cos().end(),itr,in_slice->Et_cos().end());
      itr=in_slice->Et_sin().begin();
      std::advance(itr,nord);
      slice->Et_sin().insert(slice->Et_sin().end(),itr,in_slice->Et_sin().end());
    }
    //only need to sum to nord, if prev condition was met i>nord set explicitly by insert
    for(unsigned int i=0; i<nord; i++)
    {
      float tmp_cos = slice->Et_cos().at(i);
      slice->Et_cos()[i] = tmp_cos + weight*in_slice->Et_cos().at(i);
      float tmp_sin = slice->Et_sin().at(i);
      slice->Et_sin()[i] = tmp_sin + weight*in_slice->Et_sin().at(i);
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
      mod+=2.*sc.apply(es->Et_cos().at(itr),es->Et_sin().at(itr));
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

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "EventShapeUtils/EventShapeCalculators.h"

#include "TMath.h"

#include <cstdlib>
#include <iostream>

size_t EventShapeCalculators::ProfileShape::m_plateauIdx = 0;
size_t EventShapeCalculators::ProfileShape::m_rangeIdx   = 1;
size_t EventShapeCalculators::ProfileShape::m_baseIdx    = 2;
size_t EventShapeCalculators::ProfileShape::m_shapeIdx   = 3;

EventShapeCalculators::ProfileShape::ProfileShape()
  : m_parameters(4,0.)
  , m_errors(4,0.)
{ }

EventShapeCalculators::ProfileShape::ProfileShape(const std::vector<double>& 
						  pars)
  : m_parameters(pars)
  , m_errors(4,0.)
{ }

EventShapeCalculators::ProfileShape::ProfileShape(const std::vector<double>& 
						  pars,
						  const std::vector<double>& 
						  errs)
  : m_parameters(pars)
  , m_errors(errs)
{ }

EventShapeCalculators::ProfileShape::ProfileShape(const ProfileShape& shape)
  : m_parameters(shape.m_parameters)
  , m_errors(shape.m_errors)
{ }
  
EventShapeCalculators::ProfileShape::~ProfileShape()
{ }

// interface to ROOT
double EventShapeCalculators::ProfileShape::operator()(double* xArr,
						       double* pArr)
{
  // save parameters
  for ( size_t i(0); i<m_parameters.size(); ++i ) 
    { m_parameters[i] = pArr[i]; }
  return this->evaluate(xArr[0]);
}

double EventShapeCalculators::ProfileShape::evaluate(double x) const
{
  // central part is flat
  if ( fabs(x) < m_parameters[m_rangeIdx] ) 
    { return m_parameters[m_plateauIdx]; }

  // near Gaussian part`
  return 
    x > 0. 
    ? (m_parameters[m_plateauIdx]-m_parameters[m_baseIdx]) 
    * TMath::Gaus(x,m_parameters[m_rangeIdx],m_parameters[m_shapeIdx]) + 
    m_parameters[m_baseIdx] 
    : (m_parameters[m_plateauIdx]-m_parameters[m_baseIdx]) 
    * TMath::Gaus(x,-m_parameters[m_rangeIdx],m_parameters[m_shapeIdx]) + 
    m_parameters[m_baseIdx];
}

bool EventShapeCalculators::ProfileShape::setParameter(size_t idx,
						       double value,
						       double error)
{
  if ( idx >= m_parameters.size() ) return false;
  m_parameters[idx] = value;
  m_errors[idx]     = error;
  return true;
}

double EventShapeCalculators::ProfileShape::parameter(size_t idx) const
{
  if ( idx >= m_parameters.size() ) return 0.;
  return m_parameters.at(idx);
}

double EventShapeCalculators::ProfileShape::error(size_t idx) const
{
  if ( idx >= m_errors.size() ) return 0.;
  return m_errors.at(idx); 
}

size_t EventShapeCalculators::ProfileShape::np() 
{ return m_parameters.size(); }

bool EventShapeCalculators::ProfileShape::setPlateau(double value,double error)
{ return this->setParameter(m_plateauIdx,value,error); }

bool EventShapeCalculators::ProfileShape::setRange(double value,double error)
{ return this->setParameter(m_rangeIdx,value,error); }

bool EventShapeCalculators::ProfileShape::setShape(double value,double error)
{ return this->setParameter(m_shapeIdx,value,error); }

bool EventShapeCalculators::ProfileShape::setBase(double value,double error)
{ return this->setParameter(m_baseIdx,value,error); }

double EventShapeCalculators::ProfileShape::plateau() const 
{ return this->parameter(m_plateauIdx); }

double EventShapeCalculators::ProfileShape::range() const
{ return this->parameter(m_rangeIdx); }

double EventShapeCalculators::ProfileShape::shape() const
{ return this->parameter(m_shapeIdx); }

double EventShapeCalculators::ProfileShape::base() const
{ return this->parameter(m_baseIdx); }

double EventShapeCalculators::ProfileShape::plateauError() const 
{ return this->error(m_plateauIdx); }

double EventShapeCalculators::ProfileShape::rangeError() const
{ return this->error(m_rangeIdx); }

double EventShapeCalculators::ProfileShape::shapeError() const
{ return this->error(m_shapeIdx); }

double EventShapeCalculators::ProfileShape::baseError() const
{ return this->error(m_baseIdx); }

//////
// The following function parameters are from 2011 MB data. They need to be 
// moved to the conditions DB at some point in time.
//////

#ifndef _P_RANGE_WINDOW_P0 
#define _P_RANGE_WINDOW_P0 2.21153e+00
#endif

#ifndef _P_RANGE_WINDOW_P1
#define _P_RANGE_WINDOW_P1 -6.43196e-02
#endif

#ifndef _P_RANGE_WINDOW_P2
#define _P_RANGE_WINDOW_P2 -1.07936e-01
#endif

#ifndef _P_SHAPE_WINDOW_P0
#define _P_SHAPE_WINDOW_P0 3.43037e-01
#endif

#ifndef _P_SHAPE_WINDOW_P1
#define _P_SHAPE_WINDOW_P1 3.81656e-02
#endif

#ifndef _P_SHAPE_WINDOW_P2
#define _P_SHAPE_WINDOW_P2 1.43255e-01
#endif

#ifndef _P_BASELINE_MU_OFFSET_P0
#define _P_BASELINE_MU_OFFSET_P0 2.5694e-02
#endif

#ifndef _P_BASELINE_MU_OFFSET_P1
#define _P_BASELINE_MU_OFFSET_P1 6.92623e-03
#endif

#ifndef _P_BASELINE_MU_SLOPE_P0
#define _P_BASELINE_MU_SLOPE_P0 3.63131e-02
#endif

#ifndef _P_BASELINE_MU_SLOPE_P1
#define _P_BASELINE_MU_SLOPE_P1 1.80655e-03
#endif

double EventShapeCalculators::ProfileFunctions::p_range_window_p0    
= _P_RANGE_WINDOW_P0 ;
double EventShapeCalculators::ProfileFunctions::p_range_window_p1    
= _P_RANGE_WINDOW_P1 ;
double EventShapeCalculators::ProfileFunctions::p_range_window_p2    
= _P_RANGE_WINDOW_P2 ;

double EventShapeCalculators::ProfileFunctions::p_shape_window_p0    
= _P_SHAPE_WINDOW_P0 ;
double EventShapeCalculators::ProfileFunctions::p_shape_window_p1    
= _P_SHAPE_WINDOW_P1 ;
double EventShapeCalculators::ProfileFunctions::p_shape_window_p2    
= _P_SHAPE_WINDOW_P2 ;

double EventShapeCalculators::ProfileFunctions::p_baseline_mu_offset_p0 
= _P_BASELINE_MU_OFFSET_P0 ;
double EventShapeCalculators::ProfileFunctions::p_baseline_mu_offset_p1 
= _P_BASELINE_MU_OFFSET_P1 ;

double EventShapeCalculators::ProfileFunctions::p_baseline_mu_slope_p0  
= _P_BASELINE_MU_SLOPE_P0;
double EventShapeCalculators::ProfileFunctions::p_baseline_mu_slope_p1  
= _P_BASELINE_MU_SLOPE_P1;

EventShapeCalculators::ProfileFunctions::ProfileFunctions() 
  : f_shape((ProfileShape*)0)
  , m_window(1.6)
  , m_npv(6.)
  , m_mu(8.)
  , m_etaMin(-5.)
  , m_etaMax(5.)
{
  std::vector<double> preLoad(4,0.);
  f_shape = new EventShapeCalculators::ProfileShape(preLoad);
  f_shape->setPlateau(1.);
  this->shape(m_npv,m_mu,m_window);
}

EventShapeCalculators::ProfileFunctions::ProfileFunctions(double etaMin,
							  double etaMax)
  : f_shape((ProfileShape*)0)
  , m_window(1.6)
  , m_npv(6.)
  , m_mu(8.)
  , m_etaMin(etaMin)
  , m_etaMax(etaMax)
{
  std::vector<double> preLoad(4,0.);
  f_shape = new EventShapeCalculators::ProfileShape(preLoad);
  f_shape->setPlateau(1.);
  this->shape(m_npv,m_mu,m_window);
}

EventShapeCalculators::ProfileFunctions::ProfileFunctions(const ProfileShape* 
							  shape)
  : f_shape(new ProfileShape(*shape))
  , m_window(1.6)
  , m_npv(6.)
  , m_mu(8.)
  , m_etaMin(-5.)
  , m_etaMax(5.)
{
  this->shape(m_npv,m_mu,m_window);
}

EventShapeCalculators::ProfileFunctions::ProfileFunctions(const 
							  ProfileFunctions& 
							  profile)
  : f_shape(new ProfileShape(*(profile.f_shape)))
  , m_window(profile.m_window)
  , m_npv(profile.m_npv)
  , m_mu(profile.m_mu)
  , m_etaMin(profile.m_etaMin)
  , m_etaMax(profile.m_etaMax)
{ }

EventShapeCalculators::ProfileFunctions::~ProfileFunctions()
{ if ( f_shape != 0 ) delete f_shape; }


double EventShapeCalculators::ProfileFunctions::rho(double npv,double mu,
						    double eta,
						    double rhoRef,double window)
{ 
  if ( f_shape == 0 ) return 0.;
  return rhoRef*(this->shape(npv,mu,window)->operator())(eta);
}

double EventShapeCalculators::ProfileFunctions::rho(double eta,double rhoRef)
{
  if ( f_shape == 0 ) return 0.;
  return rhoRef*(f_shape->operator())(eta);
}

const EventShapeCalculators::ProfileShape* 
EventShapeCalculators::ProfileFunctions::shape(double npv,double mu,
					       double window)
{ 
  // calculate parameterized function parameters
  double range(this->f_poly3(p_range_window_p0,p_range_window_p1,
			     p_range_window_p2,window));
  double shape(this->f_poly3(p_shape_window_p0,p_shape_window_p1,
			     p_shape_window_p2,window));
  double baseOS(this->f_poly2(p_baseline_mu_offset_p0,
			      p_baseline_mu_offset_p1,mu));
  double baseSP(this->f_poly2(p_baseline_mu_slope_p0,
			      p_baseline_mu_slope_p1,mu));
  double base(this->f_poly2(baseOS,baseSP,npv));
  
  // set parameters
  if ( !f_shape->setRange(range) ) 
    printf("[ProfileShape::shape(...)] - WARNING problem setting new range value %6.3f\n",range);
  if ( !f_shape->setShape(shape) ) 
    printf("[ProfileShape::shape(...)] - WARNING problem setting new shape value %6.3f\n",shape);
  if ( !f_shape->setBase(base) )   
    printf("[ProfileShape::shape(...)] - WARNING problem setting new base value %6.3f\n",base);
  return f_shape; 
}

TF1* EventShapeCalculators::ProfileFunctions::profile(double npv,double mu,
						      double window,
						      const std::string& name)
{
  this->shape(npv,mu,window);
  TF1* f = new TF1(name.c_str(),f_shape,m_etaMin,m_etaMax,f_shape->np()); 
  if ( f != 0 ) f->SetNpx(2000);
  return f;
}

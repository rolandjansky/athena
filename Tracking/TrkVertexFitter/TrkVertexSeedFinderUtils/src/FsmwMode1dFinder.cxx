/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************
          FsmwMode1dFinder.cxx - Description in header file
*********************************************************************/

//#define FSMWMODE1DFINDER_DEBUG

#include "TrkVertexSeedFinderUtils/FsmwMode1dFinder.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace Trk
{

  FsmwMode1dFinder::FsmwMode1dFinder(const std::string& t, const std::string& n, const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_fraction(0.5),
    m_firstfraction(0.5)
  {   
    declareProperty("Fraction",     m_fraction);
    declareProperty("FirstFraction", m_firstfraction);
    declareInterface<IMode1dFinder>(this);
  }

  FsmwMode1dFinder::~FsmwMode1dFinder() = default;

  StatusCode FsmwMode1dFinder::initialize() 
  { 
    msg(MSG::INFO) << "Initialize successfull" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode FsmwMode1dFinder::finalize() 
  {
    msg(MSG::INFO)  << "Finalize successfull" << endmsg;
    return StatusCode::SUCCESS;
  }

#ifdef FSMWMODE1DFINDER_DEBUG
  namespace {
    int debug_counter=200;
  }
#endif
  
  double FsmwMode1dFinder::getMode(std::vector<DoubleAndWeight> DoubleAndWeights) const {
    
    if(DoubleAndWeights.empty()) return 0.;
    if(DoubleAndWeights.size() == 1) return DoubleAndWeights.begin()->first;
    
#ifdef FSMWMODE1DFINDER_DEBUG
    msg(MSG::DEBUG) << "entered FsmwMode1dFinder::GetMode " << endmsg;
#endif
    
  //first of all order the vector according to the double value
    std::sort(DoubleAndWeights.begin(),DoubleAndWeights.end(),CompareTheTwoDoubleAndWeights() );
  
    //ok now begin to consider a certain number of elements according to the fraction
    std::vector<DoubleAndWeight>::const_iterator begin=DoubleAndWeights.begin();
    std::vector<DoubleAndWeight>::const_iterator end=DoubleAndWeights.end();

    double overallweight(0.);
    std::vector<DoubleAndWeight>::const_iterator best_begin=begin;
    std::vector<DoubleAndWeight>::const_iterator best_end=end;

#ifdef FSMWMODE1DFINDER_DEBUG
    msg(MSG::DEBUG) << "Size of incoming vector is" << DoubleAndWeights.size() << endmsg;
#endif
    

    double last_value(1e100);
    
    bool isthelast=false;

    int counter=0;
    double fraction=m_firstfraction;
    while (!isthelast) {

      counter+=1;
      if (counter==2) {
	fraction = m_fraction;
      }

#ifdef FSMWMODE1DFINDER_DEBUG
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Before \"int step = (int)std::floor(fraction*(end-begin+1))\" " << endmsg;
#endif
    int step = (int)std::floor(fraction*(end-begin+1));
    overallweight=0.;
    {
      std::vector<DoubleAndWeight>::const_iterator i=begin;
      if (step>0) {
        std::vector<DoubleAndWeight>::const_iterator j_end=i+step-1;
      for (std::vector<DoubleAndWeight>::const_iterator j=i;j!=j_end;j++) {
#ifdef FSMWMODE1DFINDER_DEBUG
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In the single interval, the component number unknown, value: " << j->first << " weight " 
	<< j->second << endmsg;
#endif		  
	overallweight+=j->second;
      }
      }
    }

    std::vector<DoubleAndWeight>::const_iterator i_last = begin+step-1;

#ifdef FSMWMODE1DFINDER_DEBUG
    if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "After \"int step = (int)std::floor(fraction*(end-begin+1))\" " << endmsg;
#endif
    for (std::vector<DoubleAndWeight>::const_iterator i=begin;i!=(end-step+1);++i, ++i_last) {
#ifdef FSMWMODE1DFINDER_DEBUG
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Interval number unknown, value: " << i->first << endmsg;
#endif	

      //calculate the weight the interval should be divided into
      overallweight+= i_last->second;

      #ifdef FSMWMODE1DFINDER_DEBUG
      if (debug_counter>0)
      {
        --debug_counter;
      double alt_overallweight=0.;
      for (std::vector<DoubleAndWeight>::const_iterator j=i;j!=i+step;j++) {
#ifdef FSMWMODE1DFINDER_DEBUG
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In the single interval, the component number unknown, value: " << j->first << " weight " 
	<< j->second << endmsg;
#endif		  
	alt_overallweight+=j->second;
      }
      if (std::abs(alt_overallweight - overallweight) > alt_overallweight * std::numeric_limits<double>::epsilon() * 16 ) {
        ATH_MSG_ERROR("Weights differ: " << alt_overallweight  << " != " << overallweight);
      }
      }
      #endif


      double new_value = ((i+step-1)->first-i->first)/overallweight;
      if (new_value<last_value) {
#ifdef FSMWMODE1DFINDER_DEBUG
	msg(MSG::DEBUG) << "New value: " << ((i+step-1)->first-i->first)/overallweight << " while the previous one was " << 
		last_value << endmsg;
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Best case since ever now" << endmsg;
#endif		  
	last_value=((i+step-1)->first-i->first)/overallweight;
	best_begin=i;
	best_end=i+step-1;
      }
      overallweight-= i->second;

    }
    
#ifdef FSMWMODE1DFINDER_DEBUG
    msg(MSG::DEBUG) << "Ended a cycle with success" << endmsg;
    if (begin==best_begin && end ==best_end) {
      ATH_MSG_ERROR( "No change at step=" << step );
    }
#endif
    //assign the new begin and end...
    begin=best_begin;
    end=best_end;
    last_value=1e100;
    
    //Now it should have returned the value with smallest (x2-x1)/weight
    if (best_end-best_begin<=2) isthelast=true;
  }

    //return the weighted mean of the best_begin and best_end

    //you are working only on first and last
    //(but what about taking all three ???)

    if (best_end-best_begin==2)
    {
      std::vector<DoubleAndWeight>::const_iterator medium=begin;
      medium++;
      return (begin->first*begin->second+medium->first*medium->second+end->first*end->second)/(begin->second+medium->second+end->second);
    }
    

    return (begin->first*begin->second+end->first*end->second)/(begin->second+end->second);
    
  }
  

  double FsmwMode1dFinder::getMode(std::vector<double> Doubles) const {
    


#ifdef FSMWMODE1DFINDER_DEBUG
    msg(MSG::DEBUG) << "entered FsmwMode1dFinder::GetMode " << endmsg;
#endif
  
    //first of all order the vector according to the double value
    std::sort(Doubles.begin(),Doubles.end(),CompareTheTwoDoubles() );
  
    //ok now begin to consider a certain number of elements according to the fraction
    std::vector<double>::const_iterator begin=Doubles.begin();
    std::vector<double>::const_iterator end=Doubles.end();

    //    double overallweight(0.);
    std::vector<double>::const_iterator best_begin=begin;
    std::vector<double>::const_iterator best_end=end;

#ifdef FSMWMODE1DFINDER_DEBUG
    msg(MSG::DEBUG) << "Size of incoming vector is" << Doubles.size() << endmsg;
#endif
    

    double last_value(1e100);
    
    bool isthelast=false;
    while (!isthelast) {
#ifdef FSMWMODE1DFINDER_DEBUG
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Before \"int step = (int)std::floor(m_fraction*(end-begin+1))\" " << endmsg;
#endif
    int step = (int)std::floor(m_fraction*(end-begin+1));
#ifdef FSMWMODE1DFINDER_DEBUG
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "After \"int step = (int)std::floor(m_fraction*(end-begin+1))\" " << endmsg;
#endif
    for (std::vector<double>::const_iterator i=begin;i!=(end-step+1);++i) {
#ifdef FSMWMODE1DFINDER_DEBUG
      if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Interval number unknown, value: " << i->first << endmsg;
#endif	
      //calculate the weight the interval should be divided into
      //      overallweight=0.;
      //for (std::vector<double>::const_iterator j=i;j!=i+step;j++) {
      //#ifdef FSMWMODE1DFINDER_DEBUG
      //if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "In the single interval, the component number unknown, value: " << j->first << " weight " 
      //<< j->second << endmsg;
      //#endif		  
      //overallweight+=j->second;
      //}
      if ((*(i+step-1)-*i)<last_value) {
#ifdef FSMWMODE1DFINDER_DEBUG
	msg(MSG::DEBUG) << "New value: " << *(i+step-1)-*i << " while the previous one was " << 
		last_value << endmsg;
	if(msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Best case since ever now" << endmsg;
#endif		  
	last_value=*(i+step-1)-*i;
	best_begin=i;
	best_end=i+step-1;
      }
    }
    
#ifdef FSMWMODE1DFINDER_DEBUG
    if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Ended a cycle with success" << endmsg;
#endif

    //assign the new begin and end...
    begin=best_begin;
    end=best_end;
    last_value=1e100;
    
    //Now it should have returned the value with smallest (x2-x1)/weight
    if (best_end-best_begin<=2) isthelast=true;
  }

    if (best_end-best_begin==2)
    {
      std::vector<double>::const_iterator medium=begin;
      medium++;
      return (*begin+*medium+*end)/3.;
    }

    //return the weighted mean of the best_begin and best_end
    return (*begin+*end)/2.;
    
    
  }
}



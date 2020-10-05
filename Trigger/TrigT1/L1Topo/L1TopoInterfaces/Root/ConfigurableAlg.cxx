/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//  BaseTOBoAlg.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/IL1TopoHistSvc.h"

#include "TH1.h"
#include "TH2.h"
#include <iostream>

using namespace std;
using namespace TCS;


/**
 * The implementation class
 *
 */
class ConfigurableAlg::ConfigurableAlgImpl {
public:
   ConfigurableAlgImpl(const std::string & name) :
      m_name(name)
   {}

   ~ConfigurableAlgImpl() {
      for( auto h : m_localHistStore )
         delete h;
   }
   
   void setL1TopoHistSvc(shared_ptr<IL1TopoHistSvc> histSvc) {
      m_histSvc = histSvc;
   }

   void registerHist(TH1 * h) {
      // histograms in the L1Topo framework are put in a algorithm specific folder
      string newHistName = m_name + "/" + h->GetName();
      h->SetName(newHistName.c_str());

      if( m_histSvc ) {
         m_histSvc->registerHist(h);
      } else {
         m_localHistStore.push_back(h);
      }
   }
  
   void registerHist(TH2 * h) {
      // histograms in the L1Topo framework are put in a algorithm specific folder
      string newHistName = m_name + "/" + h->GetName();
      h->SetName(newHistName.c_str());

      if( m_histSvc ) {
         m_histSvc->registerHist(h);
      } else {
         m_localHistStore.push_back(h);
      }
   }
  
   void fillHist1D(const std::string & histName, double x) {
     if ( m_histSvc ) {
       m_histSvc->fillHist1D(histName,x);
     } else {
       //Implement here to fill local histograms or something.
       return;
     }
   }
  
  
   void fillHist2D(const std::string & histName, double x, double y) {
     if ( m_histSvc ) {
       m_histSvc->fillHist2D(histName,x,y);
     } else {
       //Implement here to fill local histograms or something.
       return;
     }
   }
  
   
private:

   std::string m_name;

   // histogram service
   shared_ptr<IL1TopoHistSvc> m_histSvc;

   // store histograms locally if no hist service is available
   vector<TH1 *> m_localHistStore;

};






ConfigurableAlg::ConfigurableAlg(const  std::string & name, AlgType algType) :
   TrigConfMessaging(name),
   m_impl(new ConfigurableAlgImpl(name)),
   m_name(name),
   m_parameters(name),
   m_algType(algType)
{}


ConfigurableAlg::~ConfigurableAlg()
{}

// define parameters to be called by developer
void
ConfigurableAlg::defineParameter(const std::string & name, TCS::parType_t value) {
   defineParameter( Parameter(name, value) );
}


void
ConfigurableAlg::defineParameter(const std::string & name, TCS::parType_t value, unsigned int selection) {
   defineParameter( Parameter(name, value, selection) );
}


void
ConfigurableAlg::defineParameter(const Parameter & parameter) {
   if( parameters().contains( parameter.name(), parameter.selection() ) ) {
      TCS_EXCEPTION("Duplicate parameter definition '" << parameter.name() << "' for algorithm '" << name() << "'");
   }
   m_parameters.addParameter(parameter);
}


void
ConfigurableAlg::setParameters(const ParameterSpace & parameters) {
   
   // check if parameterspace is for this algorithm
   if(name() != parameters.algName()) {
      TCS_EXCEPTION("Name of parameter set (" << parameters.algName() << ") do not match configurable name (" << name() << ")" );
   }

   // copy parameters
   for(const Parameter & p : parameters) {
      try {
         m_parameters.setParameter(p);
      }
      catch(const TCS::Exception& e) {
         TCS_EXCEPTION("Algorithm configuration failed for " << name() << "! Caught exception: " << e.what());
      }
   }
}



const TCS::Parameter &
ConfigurableAlg::parameter(const std::string & parameterName) const {
   return parameters().parameter(parameterName);
}


const TCS::Parameter &
ConfigurableAlg::parameter(const std::string & parameterName, unsigned int selection) const {
   return parameters().parameter(parameterName, selection);
}


/**
 * forwarding public interface
 **/

void ConfigurableAlg::setL1TopoHistSvc(std::shared_ptr<IL1TopoHistSvc> histSvc) {
   m_impl->setL1TopoHistSvc(histSvc);
}

void ConfigurableAlg::registerHist(TH1 * h) {
   m_impl->registerHist(h);
}

void ConfigurableAlg::registerHist(TH2 * h) {
   m_impl->registerHist(h);
}

void ConfigurableAlg::bookHist(std::vector<std::string> &regName, const std::string name,const std::string title, const int binx, const float xmin, const float xmax) {
  string newTitle = to_string((int)xmin)+title+to_string((int)xmax);
  std::string newname=name+"_"+to_string((int)xmin)+title+to_string((int)xmax);
  std::replace( newname.begin(), newname.end(), '-', 'n');
  std::replace( newname.begin(), newname.end(), ' ', '_');
  regName.push_back(m_name+"/"+newname);

  float xmin_new,xmax_new;
  if ( xmin > 0.0)
    { xmin_new=0.0; }
  else
    { xmin_new=1.5*xmin; }
  // if minimum mass requirement less than twice of bin length,
  // adjust to range by changing maximum mass with the 10 time of bin length.
  // This is necessary when range is too wide and minimum cut unvisible.
  // Later will be changed with more automated way.
  if ( 2*(xmax-xmin)/binx > xmin && xmin > 0.0 )
    { xmax_new=10*(xmax-xmin)/binx; }
  else
    { xmax_new=1.5*xmax; }
  
  // if angular kinematics, use fixed range
  if ( title.find("ETA") != std::string::npos || title.find("PHI") != std::string::npos ){
    xmin_new=-70;
    xmax_new=70;
  }
  if ( title.find("DETA") != std::string::npos || title.find("DPHI") != std::string::npos || title.find("DR") != std::string::npos ){
    xmin_new=0;
    xmax_new=70;
  }

  TH1 *h = new TH1F(newname.c_str(),newTitle.c_str(),binx,xmin_new,xmax_new);
  h->GetXaxis()->SetTitle(title.c_str());
  m_impl->registerHist(h);
}

void ConfigurableAlg::bookHist(std::vector<std::string> &regName, const std::string name,const std::string title, const int binx, const float xmin, const float xmax, const int biny, const float ymin, const float ymax) {
  auto usPos = title.find(" vs ");
  string xName = title.substr(0,usPos);
  string yName = title.substr(usPos+4);
  string newTitle = to_string((int)xmin)+xName+to_string((int)xmax)+" vs "+to_string((int)ymin)+yName+to_string((int)ymax);
  std::string newname=name+"_"+to_string((int)xmin)+xName+to_string((int)xmax)+"_"+to_string((int)ymin)+yName+to_string((int)ymax);
  std::replace( newname.begin(), newname.end(), '-', 'n');
  std::replace( newname.begin(), newname.end(), ' ', '_');
  regName.push_back(m_name+"/"+newname);

  float xmin_new,xmax_new;
  if ( xmin > 0.0)
    { xmin_new=0.0; }
  else
    { xmin_new=1.5*xmin; }
  // if minimum x requirement less than twice of bin length,
  // adjust to range by changing maximum mass with the 10 time of bin length.
  // This is necessary when range is too wide and minimum cut unvisible.
  // Later will be changed with more automated way.
  if ( 2*(xmax-xmin)/binx > xmin && xmin > 0.0 )
    { xmax_new=10*(xmax-xmin)/binx; }
  else
    { xmax_new=1.5*xmax; }

  float ymin_new,ymax_new;
  if ( ymin > 0.0)
    { ymin_new=0.0; }
  else
    { ymin_new=1.5*ymin; }
  // if minimum y requirement less than twice of bin length,
  // adjust to range by changing maximum mass with the 10 time of bin length.
  // This is necessary when range is too wide and minimum cut unvisible.
  // Later will be changed with more automated way.
  if ( 2*(ymax-ymin)/biny > ymin && ymin > 0.0 )
    { ymax_new=10*(ymax-ymin)/biny; }
  else
    { ymax_new=1.5*ymax; }

  // if angular kinematics, use fixed range
  if ( xName.find("ETA") != std::string::npos || xName.find("PHI") != std::string::npos ){
    xmin_new=-70;
    xmax_new=70;
  }
  if ( yName.find("ETA") != std::string::npos || yName.find("PHI") != std::string::npos ){
    ymin_new=-70;
    ymax_new=70;
  }
  if ( xName.find("DETA") != std::string::npos || xName.find("DPHI") != std::string::npos || xName.find("DR") != std::string::npos ){
    xmin_new=0;
    xmax_new=70;
  }
  if ( yName.find("DETA") != std::string::npos || yName.find("DPHI") != std::string::npos || yName.find("DR") != std::string::npos ){
    ymin_new=0;
    ymax_new=70;
  }

  TH2 *h = new TH2F(newname.c_str(),newTitle.c_str(),binx,xmin_new,xmax_new,biny,ymin_new,ymax_new);
  h->GetXaxis()->SetTitle(xName.c_str());
  h->GetYaxis()->SetTitle(yName.c_str());
  m_impl->registerHist(h);
}
  
void ConfigurableAlg::fillHist1D(const std::string & histName, double x) {
  m_impl->fillHist1D(histName,x);
}

void ConfigurableAlg::fillHist2D(const std::string & histName, double x, double y) {
  m_impl->fillHist2D(histName,x,y);
}


namespace TCS {

   std::ostream &
   operator<<(std::ostream & o, const TCS::ConfigurableAlg & alg) {

      o << "algorithm '" << alg.fullname() << "'" << endl;
      o << alg.parameters();
      return o;

   }

}

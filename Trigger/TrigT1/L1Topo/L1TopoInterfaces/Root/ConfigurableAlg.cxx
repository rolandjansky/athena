//  BaseTOBoAlg.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.
//  Copyright (c) 2012 Joerg Stelzer. All rights reserved.

#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/IL1TopoHistSvc.h"

#include "TH1.h"
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
      // histograms in the L1Topo framework are put in a algorith specific folder
      string newHistName = m_name + "/" + h->GetName();
      h->SetName(newHistName.c_str());

      if( m_histSvc ) {
         m_histSvc->registerHist(h);
      } else {
         m_localHistStore.push_back(h);
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


namespace TCS {

   std::ostream &
   operator<<(std::ostream & o, const TCS::ConfigurableAlg & alg) {

      o << "algorithm '" << alg.fullname() << "'" << endl;
      o << alg.parameters();
      return o;

   }

}

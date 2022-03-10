/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//  BaseTOBoAlg.cpp
//  TopoCore
//  Created by Joerg Stelzer on 11/10/12.

#include "L1TopoInterfaces/ConfigurableAlg.h"
#include "L1TopoCommon/Exception.h"
#include "L1TopoInterfaces/IL1TopoHistSvc.h"
// Bitwise implementation utils
#include "L1TopoSimulationUtils/L1TopoDataTypes.h"
#include "L1TopoSimulationUtils/Trigo.h"
#include "L1TopoSimulationUtils/Hyperbolic.h"
#include "L1TopoSimulationUtils/Kinematics.h"

#include "TH1.h"
#include "TH2.h"
#include <iostream>
#include <sstream>

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
   
   void setL1TopoHistSvc(std::shared_ptr<IL1TopoHistSvc> histSvc) {
      m_histSvc = histSvc;
   }

   void registerHist(TH1 * h) {
      // histograms in the L1Topo framework are put in a algorithm specific folder
      std::string newHistName = m_name + "/" + h->GetName();
      h->SetName(newHistName.c_str());

      if( m_histSvc ) {
         m_histSvc->registerHist(h);
      } else {
         m_localHistStore.push_back(h);
      }
   }
  
   void registerHist(TH2 * h) {
      // histograms in the L1Topo framework are put in a algorithm specific folder
      std::string newHistName = m_name + "/" + h->GetName();
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
   std::shared_ptr<IL1TopoHistSvc> m_histSvc;

   // store histograms locally if no hist service is available
   std::vector<TH1 *> m_localHistStore;

};






ConfigurableAlg::ConfigurableAlg(const  std::string & name, AlgType algType) :
   TrigConfMessaging(name),
   m_impl(new ConfigurableAlgImpl(name)),
   m_name(name),
   m_parameters(name),
   m_algType(algType),
   m_isLegacyTopo(false)
{}


ConfigurableAlg::~ConfigurableAlg()
{}

// Kinematic Calculation
unsigned int
ConfigurableAlg::calcDeltaPhiBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  if (m_isLegacyTopo)
    {return TSU::Kinematics::calcDeltaPhiBWLegacy(tob1,tob2);}
  else
    {return TSU::Kinematics::calcDeltaPhiBW(tob1,tob2);}
}

unsigned int
ConfigurableAlg::calcDeltaEtaBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  if (m_isLegacyTopo)
    {return TSU::Kinematics::calcDeltaEtaBWLegacy(tob1,tob2);}
  else
    {return TSU::Kinematics::calcDeltaEtaBW(tob1,tob2);}
}

unsigned int
ConfigurableAlg::calcInvMassBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  if (m_isLegacyTopo)
    {return TSU::Kinematics::calcInvMassBWLegacy(tob1,tob2);}
  else
    {return TSU::Kinematics::calcInvMassBW(tob1,tob2);}
}

unsigned int
ConfigurableAlg::calcTMassBW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  if (m_isLegacyTopo)
    {return TSU::Kinematics::calcTMassBWLegacy(tob1,tob2);}
  else
    {return TSU::Kinematics::calcTMassBW(tob1,tob2);}
}

unsigned int
ConfigurableAlg::calcDeltaR2BW(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  if (m_isLegacyTopo)
    {return TSU::Kinematics::calcDeltaR2BWLegacy(tob1,tob2);}
  else
    {return TSU::Kinematics::calcDeltaR2BW(tob1,tob2);}
}

unsigned long
ConfigurableAlg::quadraticSumBW(int i1, int i2) {
  return TSU::Kinematics::quadraticSumBW(i1, i2);
}

unsigned int
ConfigurableAlg::calcDeltaPhi(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  if (m_isLegacyTopo)
    {return TSU::Kinematics::calcDeltaPhiLegacy(tob1,tob2);}
  else
    {return TSU::Kinematics::calcDeltaPhi(tob1,tob2);}
}

unsigned int
ConfigurableAlg::calcDeltaEta(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  if (m_isLegacyTopo)
    {return TSU::Kinematics::calcDeltaEtaLegacy(tob1,tob2);}
  else
    {return TSU::Kinematics::calcDeltaEta(tob1,tob2);}
}

unsigned int
ConfigurableAlg::calcInvMass(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  return TSU::Kinematics::calcInvMass(tob1,tob2);
}

unsigned int
ConfigurableAlg::calcTMass(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  return TSU::Kinematics::calcTMass(tob1,tob2);
}

unsigned int
ConfigurableAlg::calcDeltaR2(const TCS::GenericTOB* tob1, const TCS::GenericTOB* tob2) {
  if (m_isLegacyTopo)
    {return TSU::Kinematics::calcDeltaR2Legacy(tob1,tob2);}
  else
    {return TSU::Kinematics::calcDeltaR2(tob1,tob2);}
}


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

void ConfigurableAlg::bookHist(std::vector<std::string> &regName, const std::string& name,const std::string& title, const int binx, const int xmin, const int xmax, const bool isMult) {
  std::string xmin_str = ToString(xmin);
  std::string xmax_str = ToString(xmax);
  std::string newTitle = title;
  std::string newName = name;
  if (not isMult) {
    newTitle = xmin_str+title+xmax_str;
    newName = name+"_"+xmin_str+title+xmax_str;
    std::replace( newName.begin(), newName.end(), '-', 'n');
    std::replace( newName.begin(), newName.end(), ' ', '_');
  }
  regName.push_back(m_name+"/"+newName);

  // Add units to axis labels
  std::string xTitle = title;
  if (m_isLegacyTopo) {
    if (title == "ETA" || title == "DETA" || title == "PHI" || title == "DPHI" || title == "DR") { xTitle = title+"#times10"; }
    if (title == "PT" || title == "ET" || title == "HT" || title == "INVM" || title == "MT")     { xTitle = title+" [GeV]"; } 
  } 
  else {
    if (title == "ETA" || title == "DETA" || title == "DR")                                  { xTitle = title+"#times40"; }
    if (title == "PHI" || title == "DPHI")                                                   { xTitle = title+"#times20"; }
    if (title == "PT" || title == "ET" || title == "HT" || title == "INVM" || title == "MT") { xTitle = isMult ? title+" [GeV]" : title+" [100 MeV]"; } 
  }
 
  int xmin_new,xmax_new;
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
  int eta_max = 50;
  int phi_max = 64;
  int dr_max = 30;
  if (not m_isLegacyTopo) {
    eta_max *= 4;
    phi_max *= 2;
    dr_max *= 4;
  }

  if ( title.find("ETA") != std::string::npos ){
    xmin_new=-eta_max;
    xmax_new=eta_max;
  }
  if ( title.find("PHI") != std::string::npos || title.find("DPHI") != std::string::npos ){
    xmin_new=0;
    xmax_new=phi_max;
  }
  if ( title.find("DETA") != std::string::npos || title.find("DR") != std::string::npos ){
    xmin_new=0;
    xmax_new=dr_max;
  }

  TH1 *h = new TH1F(newName.c_str(),newTitle.c_str(),binx,xmin_new,xmax_new);
  h->GetXaxis()->SetTitle(xTitle.c_str());
  m_impl->registerHist(h);
}

void ConfigurableAlg::bookHist(std::vector<std::string> &regName, const std::string& name,const std::string& title, const int binx, const int xmin, const int xmax, const int biny, const int ymin, const int ymax, const bool isMult) {
  auto usPos = title.find(" vs ");
  std::string xName = title.substr(0,usPos);
  std::string yName = title.substr(usPos+4);
  std::string xmin_str = ToString(xmin);
  std::string xmax_str = ToString(xmax);
  std::string ymin_str = ToString(ymin);
  std::string ymax_str = ToString(ymax);
  std::string newTitle = title;
  std::string newName = name;
  if (not isMult) {
    newTitle = xmin_str+xName+xmax_str+" vs "+ymin_str+yName+ymax_str;
    newName = name+"_"+xmin_str+xName+xmax_str+"_"+ymin_str+yName+ymax_str;
    std::replace( newName.begin(), newName.end(), '-', 'n');
    std::replace( newName.begin(), newName.end(), ' ', '_');
  }
  regName.push_back(m_name+"/"+newName);

  // Add units to axis labels
  std::string xTitle = xName;
  std::string yTitle = yName;
  if (m_isLegacyTopo) {
    if (xName == "ETA" || xName == "DETA" || xName == "PHI" || xName == "DPHI" || xName == "DR") { xTitle = xName+"#times10"; }
    if (xName == "PT" || xName == "ET" || xName == "HT" || xName == "INVM" || xName == "MT")     { xTitle = xName+" [GeV]"; } 

    if (yName == "ETA" || yName == "DETA" || yName == "PHI" || yName == "DPHI" || yName == "DR") { yTitle = yName+"#times10"; }
    if (yName == "PT" || yName == "ET" || yName == "HT" || yName == "INVM" || yName == "MT")     { yTitle = yName+" [GeV]"; } 
  } 
  else {
    if (xName == "ETA" || xName == "DETA" || xName == "DR")                                  { xTitle = xName+"#times40"; }
    if (xName == "PHI" || xName == "DPHI")                                                   { xTitle = xName+"#times20"; }
    if (xName == "PT" || xName == "ET" || xName == "HT" || xName == "INVM" || xName == "MT") { xTitle = isMult ? xName+" [GeV]" : xName+" [100 MeV]"; } 

    if (yName == "ETA" || yName == "DETA" || yName == "DR")                                  { yTitle = yName+"#times40"; }
    if (yName == "PHI" || yName == "DPHI")                                                   { yTitle = yName+"#times20"; }
    if (yName == "PT" || yName == "ET" || yName == "HT" || yName == "INVM" || yName == "MT") { yTitle = isMult ? yName+" [GeV]" : yName+" [100 MeV]"; } 
  }

  int xmin_new,xmax_new;
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

  int ymin_new,ymax_new;
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
  int eta_max = 50;
  int phi_max = 64;
  int dr_max = 30;
  if (not m_isLegacyTopo) {
    eta_max *= 4;
    phi_max *= 2;
    dr_max *= 4;
  }

  if ( xName.find("ETA") != std::string::npos ){
    xmin_new=-eta_max;
    xmax_new=eta_max;
  }
  if ( yName.find("ETA") != std::string::npos ){
    ymin_new=-eta_max;
    ymax_new=eta_max;
  }
  if ( xName.find("PHI") != std::string::npos || xName.find("DPHI") != std::string::npos ){
    xmin_new=0;
    xmax_new=phi_max;
  }
  if ( yName.find("PHI") != std::string::npos || yName.find("DPHI") != std::string::npos ){
    ymin_new=0;
    ymax_new=phi_max;
  }
  if ( xName.find("DETA") != std::string::npos || xName.find("DR") != std::string::npos ){
    xmin_new=0;
    xmax_new=dr_max;
  }
  if ( yName.find("DETA") != std::string::npos || yName.find("DR") != std::string::npos ){
    ymin_new=0;
    ymax_new=dr_max;
  }

  TH2 *h = new TH2F(newName.c_str(),newTitle.c_str(),binx,xmin_new,xmax_new,biny,ymin_new,ymax_new);
  h->GetXaxis()->SetTitle(xTitle.c_str());
  h->GetYaxis()->SetTitle(yTitle.c_str());
  m_impl->registerHist(h);
}
  
void ConfigurableAlg::fillHist1D(const std::string & histName, double x) {
  m_impl->fillHist1D(histName,x);
}

void ConfigurableAlg::fillHist2D(const std::string & histName, double x, double y) {
  m_impl->fillHist2D(histName,x,y);
}

std::string ConfigurableAlg::ToString(const int val)
{
  const int val_int = static_cast<int>(val);
  std::ostringstream temp;
  temp << val_int;
  return temp.str();
}

bool
ConfigurableAlg::isocut(const std::string& threshold, const unsigned int bit) {
  unsigned int value = 0;
  if (threshold == "None") {value = 0;}
  else if (threshold == "Loose") {value = 1;}
  else if (threshold == "Medium") {value = 2;}
  else if (threshold == "HadMedium") {value = 2;}
  else if (threshold == "Tight") {value = 3;}
  else {
    TRG_MSG_WARNING("No isolation defined as " << threshold);
  }
  
  if (bit >= value) {return true;}
  else {return false;}
}

bool
ConfigurableAlg::isocut(const unsigned int threshold, const unsigned int bit) {
  if (bit >= threshold) {return true;}
  else {return false;}
}

namespace TCS {

   std::ostream &
   operator<<(std::ostream & o, const TCS::ConfigurableAlg & alg) {

      o << "algorithm '" << alg.fullname() << "'" << std::endl;
      o << alg.parameters();
      return o;

   }

}

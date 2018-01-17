/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EMShower.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 5, 2000

PURPOSE:  object containing information useful for e/g identification such
          as shower shapes in the em calorimeter and leakage into hadronic
          calorimeter. Information about the different variables can be
	  found in EMShower.h

Updated:  Jan 13, 2001   (MW)
          add some more comments
          Oct 15, 2002   (FD)
          add new variables for soft electrons
          Mar 2, 2003    (MW)
          declare some more variables
          May 5, 2003    (MW)
          make code compliant for gcc-3.2
          add  leakage into had calo (EThad)
          Feb 02, 2004 (FD)
          Remove unnecessary variables
          Jun 20, 2004 (FD) add etcone R=0.2,0.3,0.4
          Oct. 10, 2008 (FD) : change names of e131, e1153, iso
          Feb. 12, 2009 (FD) add etringnoisedR03SigN variables
          Feb. 23, 2009 (FD) change etconoise20,30 into 
	                     etconoisedR04Sig2 etconoisedR04Sig2
          May 06, 2009 (FD) add track isolation variable
          Mar 03, 2010 (FD) add etcone15, etcone25, etcone35
          Jul 08, 2010 (FD) add ptcone20, ptcone40
          Jul 09, 2010 (FD) add nucone20, nucone30, nucone40
          Nov 21, 2010 (TCD/JM) add etcone(15,20,25,30,35,40)_ptcorrected
          May 13, 2011 (JM) add interfaces to pointing variables
          May 13, 2011 (JM) move accessors to inline
          Jan 26, 2012 (JM/TCD) add more iso variables
          May 10, 2012 (JM) add more ptcone variables
********************************************************************/

// INCLUDE HEADER FILES:

#include "egammaEvent/EMShower.h"
#include "GaudiKernel/GaudiException.h"
#include "SGTools/ClassName.h"
#include <iomanip>
#include <iostream>

//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructor
// ----------------------------
EMShower::EMShower()
  : egDetail()
{
}

// --------------------------
// Destructor
// --------------------------
EMShower::~EMShower()
{ }

/// interface methods
std::string EMShower::s_className = ClassName<EMShower>::name();
const std::string& EMShower::className() const
{
  return s_className;
}

// ===============================================================
double EMShower::parameter(egammaParameters::ParamDef key) const {


  typedef std::pair<egammaParameters::ParamDef,double> elParams;

  double result = egammaParameters::EgParamUndefined;
  std::vector<elParams>::const_iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == key ) {
      result = (*p).second;
      break;
    }
  }

  return result;
}

// ==================================================================
void EMShower::set_parameter(egammaParameters::ParamDef key, double value, bool overwrite) {

  typedef std::pair<egammaParameters::ParamDef,double> elParams;

  std::vector<elParams>::iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_parameters.end() ) {
    m_parameters.push_back( elParams(key,value) );
  }
  else {
    if ( overwrite ) {
      (*p).second = value;
    }
    else {      
      throw GaudiException("parameter not saved", "egamma::set_parameter(...)", StatusCode::FAILURE);
    }
  }

}


// ----------------------------
// print EMShower quantities
// ----------------------------
void EMShower::print() const
{

  std::cout << " EMShower variables "        << std::endl;
  std::cout << " e011        =  " << parameter(egammaParameters::e011)     << std::endl; 
  std::cout << " e033        =  " << parameter(egammaParameters::e033)     << std::endl; 
  std::cout << " e132        =  " << parameter(egammaParameters::e132)     << std::endl; 
  std::cout << " e1152       =  " << parameter(egammaParameters::e1152)    << std::endl; 
  std::cout << " ethad1      =  " << parameter(egammaParameters::ethad1)   << std::endl;
  std::cout << " ethad       =  " << parameter(egammaParameters::ethad)    << std::endl;
  std::cout << " etcone45    =  " << parameter(egammaParameters::etcone)   << std::endl; 
  std::cout << " etcone15    =  " << parameter(egammaParameters::etcone15) << std::endl; 
  std::cout << " etcone20    =  " << parameter(egammaParameters::etcone20) << std::endl; 
  std::cout << " etcone25    =  " << parameter(egammaParameters::etcone25) << std::endl; 
  std::cout << " etcone30    =  " << parameter(egammaParameters::etcone30) << std::endl; 
  std::cout << " etcone35    =  " << parameter(egammaParameters::etcone35) << std::endl; 
  std::cout << " etcone40    =  " << parameter(egammaParameters::etcone40) << std::endl; 
  std::cout << " etconoisedR04Sig2 =  " << parameter(egammaParameters::etconoisedR04Sig2) << std::endl; 
  std::cout << " etconoisedR04Sig3 =  " << parameter(egammaParameters::etconoisedR04Sig3) << std::endl; 
  std::cout << " etconoisedR02SigAbs2 =  " << parameter(egammaParameters::etconoisedR02SigAbs2) << std::endl; 
  std::cout << " etconoisedR02SigAbs3 =  " << parameter(egammaParameters::etconoisedR02SigAbs3) << std::endl; 
  std::cout << " etconoisedR03SigAbs2 =  " << parameter(egammaParameters::etconoisedR03SigAbs2) << std::endl; 
  std::cout << " etconoisedR03SigAbs3 =  " << parameter(egammaParameters::etconoisedR03SigAbs3) << std::endl; 
  std::cout << " etconoisedR04SigAbs2 =  " << parameter(egammaParameters::etconoisedR04SigAbs2) << std::endl; 
  std::cout << " etconoisedR04SigAbs3 =  " << parameter(egammaParameters::etconoisedR04SigAbs3) << std::endl; 
  std::cout << " ehad1       =  " << parameter(egammaParameters::ehad1)    << std::endl; 
  std::cout << " f1          =  " << parameter(egammaParameters::f1)       << std::endl; 
  std::cout << " f3          =  " << parameter(egammaParameters::f3)       << std::endl; 
  std::cout << " f1core      =  " << parameter(egammaParameters::f1core)   << std::endl; 
  std::cout << " f3core      =  " << parameter(egammaParameters::f3core)   << std::endl; 
  std::cout << " e233        =  " << parameter(egammaParameters::e233)     << std::endl; 
  std::cout << " e235        =  " << parameter(egammaParameters::e235)     << std::endl; 
  std::cout << " e237        =  " << parameter(egammaParameters::e237)     << std::endl; 
  std::cout << " e255        =  " << parameter(egammaParameters::e255)     << std::endl; 
  std::cout << " e277        =  " << parameter(egammaParameters::e277)     << std::endl; 
  std::cout << " e333        =  " << parameter(egammaParameters::e333)     << std::endl; 
  std::cout << " e335        =  " << parameter(egammaParameters::e335)     << std::endl; 
  std::cout << " e337        =  " << parameter(egammaParameters::e337)     << std::endl; 
  std::cout << " e377        =  " << parameter(egammaParameters::e377)     << std::endl; 
  std::cout << " weta1       =  " << parameter(egammaParameters::weta1)    << std::endl; 
  std::cout << " weta2       =  " << parameter(egammaParameters::weta2)    << std::endl; 
  std::cout << " e2ts1       =  " << parameter(egammaParameters::e2ts1)    << std::endl; 
  std::cout << " e2tsts1     =  " << parameter(egammaParameters::e2tsts1)  << std::endl; 
  std::cout << " fracs1      =  " << parameter(egammaParameters::fracs1)   << std::endl; 
  std::cout << " widths1     =  " << parameter(egammaParameters::widths1)  << std::endl; 
  std::cout << " widths2     =  " << parameter(egammaParameters::widths2)  << std::endl; 
  std::cout << " poscs1      =  " << parameter(egammaParameters::poscs1)   << std::endl; 
  std::cout << " poscs2      =  " << parameter(egammaParameters::poscs2)   << std::endl; 
  std::cout << " asy1        =  " << parameter(egammaParameters::asy1)     << std::endl; 
  std::cout << " barys1      =  " << parameter(egammaParameters::barys1)   << std::endl; 
  std::cout << " wtots1      =  " << parameter(egammaParameters::wtots1)   << std::endl; 
  std::cout << " emins1      =  " << parameter(egammaParameters::emins1)   << std::endl; 
  std::cout << " emaxs1      =  " << parameter(egammaParameters::emaxs1)   << std::endl; 
  std::cout << " r33over37allcalo  =  " << parameter(egammaParameters::r33over37allcalo)      << std::endl; 
  std::cout << " pos         =  " << parameter(egammaParameters::pos)      << std::endl; 
  std::cout << " pos7        =  " << parameter(egammaParameters::pos7)     << std::endl; 
  std::cout << " ecore       =  " << parameter(egammaParameters::ecore)    << std::endl; 
  std::cout << " zvertex     =  " << parameter(egammaParameters::zvertex)  << std::endl; 
  std::cout << " errz        =  " << parameter(egammaParameters::errz)     << std::endl; 
  std::cout << " etap        =  " << parameter(egammaParameters::etap)     << std::endl; 
  std::cout << " depth       =  " << parameter(egammaParameters::depth)    << std::endl; 
  std::cout << " etringnoisedR03Sig2  =  " << parameter(egammaParameters::etringnoisedR03Sig2)    << std::endl; 
  std::cout << " etringnoisedR03Sig3  =  " << parameter(egammaParameters::etringnoisedR03Sig3)    << std::endl; 
  std::cout << " etringnoisedR03Sig4  =  " << parameter(egammaParameters::etringnoisedR03Sig4)    << std::endl; 
  std::cout << " etringnoisedR04Sig2  =  " << parameter(egammaParameters::etringnoisedR04Sig2)    << std::endl; 
  std::cout << " etringnoisedR04Sig3  =  " << parameter(egammaParameters::etringnoisedR04Sig3)    << std::endl; 
  std::cout << " etringnoisedR02SigAbs2  =  " << parameter(egammaParameters::etringnoisedR02SigAbs2)    << std::endl; 
  std::cout << " etringnoisedR02SigAbs3  =  " << parameter(egammaParameters::etringnoisedR02SigAbs3)    << std::endl; 
  std::cout << " etringnoisedR03SigAbs2  =  " << parameter(egammaParameters::etringnoisedR03SigAbs2)    << std::endl; 
  std::cout << " etringnoisedR03SigAbs3  =  " << parameter(egammaParameters::etringnoisedR03SigAbs3)    << std::endl; 
  std::cout << " etringnoisedR04SigAbs2  =  " << parameter(egammaParameters::etringnoisedR04SigAbs2)    << std::endl; 
  std::cout << " etringnoisedR04SigAbs3  =  " << parameter(egammaParameters::etringnoisedR04SigAbs3)    << std::endl; 
  std::cout << " linkIndex    =  " << parameter(egammaParameters::linkIndex)    << std::endl;
  std::cout << " ptcone20    =  " << parameter(egammaParameters::ptcone20)    << std::endl;
  std::cout << " ptcone30    =  " << parameter(egammaParameters::ptcone30)    << std::endl;
  std::cout << " ptcone40    =  " << parameter(egammaParameters::ptcone40)    << std::endl;
  std::cout << " nucone20    =  " << parameter(egammaParameters::nucone20)    << std::endl;
  std::cout << " nucone30    =  " << parameter(egammaParameters::nucone30)    << std::endl;
  std::cout << " nucone40    =  " << parameter(egammaParameters::nucone40)    << std::endl;
  std::cout << " ptcone20_zpv05    =  " << parameter(egammaParameters::ptcone20_zpv05)    << std::endl;
  std::cout << " ptcone30_zpv05    =  " << parameter(egammaParameters::ptcone30_zpv05)    << std::endl;
  std::cout << " ptcone40_zpv05    =  " << parameter(egammaParameters::ptcone40_zpv05)    << std::endl;
  std::cout << " nucone20_zpv05    =  " << parameter(egammaParameters::nucone20_zpv05)    << std::endl;
  std::cout << " nucone30_zpv05    =  " << parameter(egammaParameters::nucone30_zpv05)    << std::endl;
  std::cout << " nucone40_zpv05    =  " << parameter(egammaParameters::nucone40_zpv05)    << std::endl;
  std::cout << " etcone15_ptcorrected    =  " << parameter(egammaParameters::etcone15_ptcorrected) << std::endl;
  std::cout << " etcone20_ptcorrected    =  " << parameter(egammaParameters::etcone20_ptcorrected) << std::endl;
  std::cout << " etcone25_ptcorrected    =  " << parameter(egammaParameters::etcone25_ptcorrected) << std::endl;
  std::cout << " etcone30_ptcorrected    =  " << parameter(egammaParameters::etcone30_ptcorrected) << std::endl;
  std::cout << " etcone35_ptcorrected    =  " << parameter(egammaParameters::etcone35_ptcorrected) << std::endl;
  std::cout << " etcone40_ptcorrected    =  " << parameter(egammaParameters::etcone40_ptcorrected) << std::endl;
  std::cout << " etcone20_corrected    =  " << parameter(egammaParameters::etcone20_corrected) << std::endl;
  std::cout << " etcone30_corrected    =  " << parameter(egammaParameters::etcone30_corrected) << std::endl;
  std::cout << " etcone40_corrected    =  " << parameter(egammaParameters::etcone40_corrected) << std::endl;
  std::cout << " topoetcone20             =  " << parameter(egammaParameters::topoetcone20)               << std::endl;
  std::cout << " topoetcone30             =  " << parameter(egammaParameters::topoetcone30)               << std::endl;
  std::cout << " topoetcone40             =  " << parameter(egammaParameters::topoetcone40)               << std::endl;
  std::cout << " topoetcone40_ptcorrected =  " << parameter(egammaParameters::topoetcone40_ptcorrected)   << std::endl;
  std::cout << " topoetcone40_corrected   =  " << parameter(egammaParameters::topoetcone40_corrected)     << std::endl;


}

/************************************************************/
//
// return values for the different variables in the EMShower object
//


bool EMShower::hasParameter(egammaParameters::ParamDef key) const {
  if (key == egammaParameters::e011 ) return true; 
  if (key == egammaParameters::e033 ) return true;     
  if (key == egammaParameters::e132 ) return true;     
  if (key == egammaParameters::e1152 ) return true;    
  if (key == egammaParameters::ethad1 ) return true;   
  if (key == egammaParameters::ethad ) return true;    
  if (key == egammaParameters::ehad1 ) return true;    
  if (key == egammaParameters::f1 ) return true;       
  if (key == egammaParameters::f3 ) return true;       
  if (key == egammaParameters::f1core ) return true;   
  if (key == egammaParameters::f3core ) return true;   
  if (key == egammaParameters::e233 ) return true;     
  if (key == egammaParameters::e235 ) return true;     
  if (key == egammaParameters::e255 ) return true;     
  if (key == egammaParameters::e237 ) return true;     
  if (key == egammaParameters::e277 ) return true;     
  if (key == egammaParameters::e333 ) return true;     
  if (key == egammaParameters::e335 ) return true;     
  if (key == egammaParameters::e337 ) return true;     
  if (key == egammaParameters::e377 ) return true;     
  if (key == egammaParameters::weta1 ) return true;    
  if (key == egammaParameters::weta2 ) return true;    
  if (key == egammaParameters::e2ts1 ) return true;    
  if (key == egammaParameters::e2tsts1 ) return true;  
  if (key == egammaParameters::fracs1 ) return true;   
  if (key == egammaParameters::widths1 ) return true;  
  if (key == egammaParameters::widths2 ) return true;  
  if (key == egammaParameters::poscs1 ) return true;   
  if (key == egammaParameters::poscs2 ) return true;   
  if (key == egammaParameters::asy1 ) return true;     
  if (key == egammaParameters::pos ) return true;      
  if (key == egammaParameters::pos7 ) return true;     
  if (key == egammaParameters::barys1 ) return true;   
  if (key == egammaParameters::wtots1 ) return true;   
  if (key == egammaParameters::emins1 ) return true;   
  if (key == egammaParameters::emaxs1 ) return true;   
  if (key == egammaParameters::etcone ) return true;   
  if (key == egammaParameters::etcone15 ) return true; 
  if (key == egammaParameters::etcone20 ) return true; 
  if (key == egammaParameters::etcone25 ) return true; 
  if (key == egammaParameters::etcone30 ) return true; 
  if (key == egammaParameters::etcone35 ) return true; 
  if (key == egammaParameters::etcone40 ) return true; 
  if (key == egammaParameters::etconoisedR04Sig2 ) return true; 
  if (key == egammaParameters::etconoisedR04Sig3 ) return true; 
  if (key == egammaParameters::etconoisedR02SigAbs2 ) return true; 
  if (key == egammaParameters::etconoisedR02SigAbs3 ) return true; 
  if (key == egammaParameters::etconoisedR03SigAbs2 ) return true; 
  if (key == egammaParameters::etconoisedR03SigAbs3 ) return true; 
  if (key == egammaParameters::etconoisedR04SigAbs2 ) return true; 
  if (key == egammaParameters::etconoisedR04SigAbs3 ) return true; 
  if (key == egammaParameters::r33over37allcalo ) return true;      
  if (key == egammaParameters::ecore ) return true;
  if (key == egammaParameters::zvertex) return true;
  if (key == egammaParameters::errz) return true;
  if (key == egammaParameters::etap) return true;
  if (key == egammaParameters::depth) return true;
  if (key == egammaParameters::etringnoisedR03Sig2) return true;
  if (key == egammaParameters::etringnoisedR03Sig3) return true;
  if (key == egammaParameters::etringnoisedR03Sig4) return true;
  if (key == egammaParameters::etringnoisedR04Sig2) return true;
  if (key == egammaParameters::etringnoisedR04Sig3) return true;
  if (key == egammaParameters::etringnoisedR02SigAbs2) return true;
  if (key == egammaParameters::etringnoisedR02SigAbs3) return true;
  if (key == egammaParameters::etringnoisedR03SigAbs2) return true;
  if (key == egammaParameters::etringnoisedR03SigAbs3) return true;
  if (key == egammaParameters::etringnoisedR04SigAbs2) return true;
  if (key == egammaParameters::etringnoisedR04SigAbs3) return true;
  if (key == egammaParameters::linkIndex ) return true;     
  if (key == egammaParameters::ptcone20 ) return true;    
  if (key == egammaParameters::ptcone30 ) return true;    
  if (key == egammaParameters::ptcone40 ) return true;    
  if (key == egammaParameters::nucone20 ) return true;    
  if (key == egammaParameters::nucone30 ) return true;    
  if (key == egammaParameters::nucone40 ) return true;    
  if (key == egammaParameters::ptcone20_zpv05 ) return true;    
  if (key == egammaParameters::ptcone30_zpv05 ) return true;    
  if (key == egammaParameters::ptcone40_zpv05 ) return true;    
  if (key == egammaParameters::nucone20_zpv05 ) return true;    
  if (key == egammaParameters::nucone30_zpv05 ) return true;    
  if (key == egammaParameters::nucone40_zpv05 ) return true;    
  if (key == egammaParameters::etcone15_ptcorrected ) return true;
  if (key == egammaParameters::etcone20_ptcorrected ) return true;
  if (key == egammaParameters::etcone25_ptcorrected ) return true;
  if (key == egammaParameters::etcone30_ptcorrected ) return true;
  if (key == egammaParameters::etcone35_ptcorrected ) return true;
  if (key == egammaParameters::etcone40_ptcorrected ) return true;
  if (key == egammaParameters::etcone20_corrected ) return true;
  if (key == egammaParameters::etcone30_corrected ) return true;
  if (key == egammaParameters::etcone40_corrected ) return true;
  if (key == egammaParameters::topoetcone20)              return true;
  if (key == egammaParameters::topoetcone30)              return true;
  if (key == egammaParameters::topoetcone40)              return true;
  if (key == egammaParameters::topoetcone40_ptcorrected ) return true;
  if (key == egammaParameters::topoetcone40_corrected )   return true;

  return false;
}    



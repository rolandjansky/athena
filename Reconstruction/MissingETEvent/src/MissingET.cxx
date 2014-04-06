/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEt.cxx
PACKAGE:  offline/Reconstruction/MissingETEvent

AUTHORS:  P. Loch, S. Resconi
CREATED:  Oct 2004

PURPOSE:  Base Class for MissingETEvent package, it owns the region object
********************************************************************/
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtRegions.h"

//fix by Andrei Gaponenko
MissingET::MissingET(const MissingET& b)
: m_regions(b.m_regions ? new MissingEtRegions(*b.m_regions) : 0)
, m_source(b.m_source)
, m_ex(b.m_ex)
, m_ey(b.m_ey)
, m_etSum(b.m_etSum)
{}

//fix by Andrei Gaponenko
MissingET& MissingET::operator=(const MissingET& b) {
  if(this != &b) {
    delete m_regions;   // added this line to avoid memory leaks !
    m_regions = (b.m_regions ? new MissingEtRegions(*b.m_regions) : 0);
    m_source = b.m_source;
    m_ex = b.m_ex;
    m_ey = b.m_ey;
    m_etSum = b.m_etSum;
  }
  return *this;
} 


// default contructor
MissingET::MissingET() :
  m_regions(0),
  m_source(MissingET::Unknown),
  m_ex(0.),
  m_ey(0.),
  m_etSum(0.)
{ }

//contructor with source specifier
MissingET::MissingET(MissingET::Source aSource) :
  m_regions(0),
  m_source(aSource),
  m_ex(0.),
  m_ey(0.),
  m_etSum(0.)
{ }


//contructor with regions
MissingET::MissingET(MissingEtRegions* theRegions) :
  m_regions(theRegions),
  m_source(MissingET::Unknown),
  m_ex(0.),
  m_ey(0.),
  m_etSum(0.)
{ }

//contructor with regions and source
MissingET::MissingET(MissingET::Source aSource, MissingEtRegions* theRegions) :
  m_regions(theRegions),
  m_source(aSource),
  m_ex(0.),
  m_ey(0.),
  m_etSum(0.)
{ }


// destructor
MissingET::~MissingET()
{
  // it owns the regions object 
 delete m_regions;
}

// set methods
void MissingET::setEx(double theEx)
{
  m_ex = theEx;
}
void MissingET::setEy(double theEy)
{
  m_ey = theEy;
}
void MissingET::setEtSum(double theEtSum)
{
  m_etSum = theEtSum;
}

// add method
/*void MissingET::add(const CaloCell* aCell, double &weight)
{
  double exc=0.;
  double eyc=0.;
  double eta = aCell->eta();
     
  Calc_Ecellxy(aCell, exc, eyc);
  
  // Note the minus sign! We want to calculate missing ET 
  m_ex -= exc * weight;
  m_ey -= eyc * weight;
  m_etSum += aCell->et() * weight;
  
  // regions
  if ( m_regions != 0 ) m_regions->addReg(aCell, weight);
} */

void MissingET::add(double theEx,  double theEy, double theEt, 
                   double  theEta, double weight)
{        
  // Note the minus sign! We want to calculate missing ET 
  m_ex -= theEx * weight;
  m_ey -= theEy * weight;
  m_etSum += theEt * weight;
    
  // regions
  if ( m_regions != 0 ) m_regions->addReg(theEx, theEy, theEt, theEta, weight);
} 


// retrieve kinematics
double MissingET::etx() const
{ 
  return m_ex; 
}
double MissingET::ety() const
{ 
  return m_ey; 
}
double MissingET::sumet() const
{ 
  return m_etSum; 
}
double MissingET::et() const
{
  return sqrt(m_ex*m_ex + m_ey*m_ey);
}
double MissingET::phi() const
{

  return atan2(m_ey,m_ex);
}

// retrieve the regions
MissingEtRegions* MissingET::getRegions() const
{
  return m_regions;
}
  
// return the source information

MissingET::Source MissingET::getSource() const
{
  return m_source;
}

/*
void MissingET::Calc_Ecellxy(const CaloCell* cObj, double &ex, double &ey)
{
  //function to calculate x and y component of cell energy 

  // get object kinematics
  double et_cell   = cObj->et();
  double phi_cell  = cObj->phi();

  // directly get et (optimised)
  ex = et_cell * cos(phi_cell);
  ey = et_cell * sin(phi_cell);

}; */

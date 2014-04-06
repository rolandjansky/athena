/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     MissingEtCalo.cxx
PACKAGE:  offline/Reconstruction/MissingETEvent

AUTHORS:  P. Loch, S. Resconi
CREATED:  Oct 2004

PURPOSE: Calculate ExMiss,EyMiss and Sumet in Calorimeter Regions 
********************************************************************/
#include "MissingETEvent/MissingET.h"
#include "MissingETEvent/MissingEtCalo.h"
#include "MissingETEvent/MissingEtRegions.h"
#include "CxxUtils/sincos.h"

MissingEtCalo::MissingEtCalo() : MissingET(MissingET::Calo)
{
  this->setup();
}

MissingEtCalo::MissingEtCalo(MissingEtRegions* aRegion)
  : MissingET(MissingET::Calo,aRegion)
{
  this->setup();
}

MissingEtCalo::MissingEtCalo(MissingET::Source aSource, 
			     MissingEtRegions* aRegion)
  : MissingET(aSource,aRegion)
{ 
  this->setup();
}

MissingEtCalo::~MissingEtCalo()
{ 
  this->setup();
}

void MissingEtCalo::addCalo(const CaloCell* aCell, double weight)
{
  //For efficiency don't call Calc_Ecellxy directly here but put the
  //calculations directly:
  double etc  = aCell->et();
  CxxUtils::sincos sc(aCell->phi());
  double exc = etc * sc.cs;
  double eyc = etc * sc.sn;
  double etac = aCell->eta();

  // update the global sums and eta regions
  this->add(exc, eyc, etc, etac, weight);
  
  // get the calo sample from each cell 
  CaloSampling::CaloSample aSample = aCell->caloDDE()->getSampling();
  CaloIndex theIndex = this->getCaloIndex(aSample);
  
  // calorimeter sums
  if ( theIndex != NotValid )
    { 
      m_exCalo[(size_t)theIndex] -= weight * exc;
      m_eyCalo[(size_t)theIndex] -= weight * eyc;
      m_etSumCalo[(size_t)theIndex] += weight * etc;
      ++(m_nCellsCalo[(size_t)theIndex]);
    }
}

MissingEtCalo::CaloIndex MissingEtCalo::getCaloIndex(CaloSampling::CaloSample aSample) 
{
   
  // The CaloIndex identifies the calorimeters used in Atrecon and as such 
  // users will be able to seemlessly use the Athena values in their analysis.
  // 7 calos : PS Barrel, EM Barrel, PS EndCap, EM EndCap, Tile, HEC, FCAL

  CaloIndex theIndex = NotValid;
  switch ( aSample )
    {
    
    // PS Barrel
    case CaloSampling::PreSamplerB:
      theIndex = PEMB;
      break;
   
    // EM Barrel
    case CaloSampling::EMB1:
    case CaloSampling::EMB2:
    case CaloSampling::EMB3:
      theIndex = EMB;
      break;
   
    // PS EndCap
    case CaloSampling::PreSamplerE:
      theIndex = PEMEC;
      break;
      
    // EM EndCap
    case CaloSampling::EME1:
    case CaloSampling::EME2:
    case CaloSampling::EME3:
      theIndex = EME;
      break;  
  
    // Tile   
    case CaloSampling::TileBar0:
    case CaloSampling::TileBar1:
    case CaloSampling::TileBar2:
    case CaloSampling::TileExt0:
    case CaloSampling::TileExt1:
    case CaloSampling::TileExt2:
    case CaloSampling::TileGap1:
    case CaloSampling::TileGap2:
    case CaloSampling::TileGap3:
      theIndex = TILE;
      break;  
      
    // HEC  
    case CaloSampling::HEC0:
    case CaloSampling::HEC1:
    case CaloSampling::HEC2:
    case CaloSampling::HEC3:
      theIndex = HEC;
      break;
      
    // FCAL  
    case CaloSampling::FCAL0:
    case CaloSampling::FCAL1:
    case CaloSampling::FCAL2:
      theIndex = FCAL;
      break;
  
    default:
      break;
    }
  return theIndex;
}

/*
double MissingEtCalo::getExEta(MissingEtRegions::EtaRegion aRegion) const
{
  const theRegionSums = this->getRegion();
  return ( theRegionSums != 0 )
    ? theRegionSums(aRegion)
    : 0.;
}

*/

//set methods

void MissingEtCalo::setExCalo(MissingEtCalo::CaloIndex theCalo, 
			      double theEx)
{
  if ( theCalo < Size ) m_exCalo[(size_t)theCalo] = theEx;
}


void MissingEtCalo::setEyCalo(MissingEtCalo::CaloIndex theCalo, 
			      double theEy)
{
  if ( theCalo < Size ) m_eyCalo[(size_t)theCalo] = theEy;
}


void MissingEtCalo::setEtSumCalo(MissingEtCalo::CaloIndex theCalo, 
			         double theEtSum)
{
  if ( theCalo < Size ) m_etSumCalo[(size_t)theCalo] = theEtSum;
  
}


void MissingEtCalo::setNCellCalo(MissingEtCalo::CaloIndex theCalo, 
			         unsigned int theNCell)
{
  if ( theCalo < Size ) m_nCellsCalo[(size_t)theCalo] = theNCell;
  
}

void MissingEtCalo::setCalibType(std::string theCalibType)
{
  
  m_calibType=theCalibType;
}

// get methods

double MissingEtCalo::exCalo(MissingEtCalo::CaloIndex theCalo) const
{
  return m_exCalo[theCalo] ;
}

double MissingEtCalo::eyCalo(MissingEtCalo::CaloIndex theCalo) const
{
  return m_eyCalo[theCalo] ;
}

double MissingEtCalo::etSumCalo(MissingEtCalo::CaloIndex theCalo) const
{
  return m_etSumCalo[theCalo] ;
}

unsigned int MissingEtCalo::ncellCalo(MissingEtCalo::CaloIndex theCalo) const
{
  return m_nCellsCalo[theCalo] ;
}

std::string MissingEtCalo::calibType() const
{

  return m_calibType ;
}


void MissingEtCalo::setup()
{
  // initialize data members
  
  m_exCalo.resize(MissingEtCalo::Size,0.);
  m_eyCalo.resize(MissingEtCalo::Size,0.);
  m_etSumCalo.resize(MissingEtCalo::Size,0.);
  m_nCellsCalo.resize(MissingEtCalo::Size,0);
}

void MissingEtCalo::Calc_Ecellxy(const CaloCell* cObj, double &ex, double &ey)
{
  //function to calculate x and y component of cell energy 

  CxxUtils::sincos sc(cObj->phi());
  //Done the following way to avoid a temporary et_cell variable:
  ex = cObj->et();
  ey = ex * sc.sn;
  ex *= sc.cs;
}

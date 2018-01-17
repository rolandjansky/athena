/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// Name: FilterTool.h
//
/**
   @class FilterTool

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Define all needed fitler methods here

*/
//=============================================================================


// Preprocessor magic for debugging
#define XXX std::cout << "I am here: " << __FILE__ << ":" << __LINE__ << std::endl;

// This classes header
#include "D2PDMaker/FilterTool.h"

// STL includes
#include <string>
#include <vector>
#include <iostream>

// DataModel
#include "GaudiKernel/ToolFactory.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/CLASS_DEF.h"

// particles includes
#include "CLHEP/Vector/LorentzVector.h"
#include "EventKernel/IParticle.h"

// System of Units
#include "CLHEP/Units/SystemOfUnits.h"

// Needed for deltaR calculation (really?)
//#include "FourMom/P4EEtaPhiMBase.h"
//#include "CLHEP/Vector/LorentzVector.h"


using CLHEP::HepLorentzVector;


//=============================================================================
// Constructor
//=============================================================================
FilterTool::FilterTool( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent ) :
  AthAlgTool( type, name, parent )
{
  // declare interface
  declareInterface<FilterTool>( this );

  // Properties go here
  //declareProperty("filterTool", m_outCollKey="MyCompositeCollection" );

}



//=============================================================================
// Destructor
//=============================================================================
FilterTool::~FilterTool()
{
}




//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode FilterTool::initialize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;
  
  return sc ;
}




//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode FilterTool::finalize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;
  
  return sc ;
}





//=============================================================================
// double filter
//=============================================================================
bool FilterTool::doubleFilter( double num, double min, double max )
{
  if ( num >= min && num <= max )
    {
      return true;
    }
  else
    {
      return false;
    }
}
 




//=============================================================================
// absDeltaEta filter
//=============================================================================
bool FilterTool::absDeltaEtaFilter( const INavigable4Momentum* part1, const INavigable4Momentum* part2,
                                    double deltaEtaMin, double deltaEtaMax )
{
  return absDeltaEtaFilter( part1->hlv(), part2->hlv(), deltaEtaMin, deltaEtaMax );
}



//=============================================================================
// absDeltaEta filter
//=============================================================================
bool FilterTool::absDeltaEtaFilter( const HepLorentzVector& part1,
                                    const HepLorentzVector& part2,
                                    double deltaEtaMin, double deltaEtaMax )
{
  double absDeltaEta = std::fabs( part1.eta() - part2.eta() );
  if ( absDeltaEta > deltaEtaMin && absDeltaEta < deltaEtaMax )
    {
      return true;
    }
  else
    {
      return false;
    }
}





//=============================================================================
// deltaPhi filter
//=============================================================================
bool FilterTool::deltaPhiFilter( const INavigable4Momentum* part1, const INavigable4Momentum* part2,
                                 double deltaRMin, double deltaRMax )
{
  return deltaPhiFilter( part1->hlv(), part2->hlv(), deltaRMin, deltaRMax );
}



//=============================================================================
// deltaPhi filter
//=============================================================================
bool FilterTool::deltaPhiFilter( const IParticle* part1, const IParticle* part2,
                                 double deltaPhiMin, double deltaPhiMax )
{
  // determine deltaPhi, taking the charge into account
  double deltaPhi = part1->hlv().getV().deltaPhi( part2->hlv().getV() );
  int qpart1(0);
  int qpart2(0);
  if ( part1->hasCharge() )
    {
      if ( part1->charge() < -0.1 ) 
        {
          qpart1 = -1;
        }
      if ( part1->charge() > 0.1 ) 
        {
          qpart1 = 1;
        }
    }
  if ( part2->hasCharge() )
    {
      if ( part2->charge() < -0.1 ) 
        {
          qpart2 = -1;
        }
      if ( part2->charge() > 0.1 ) 
        {
          qpart2 = 1;
        }
    }
  if ( qpart1 != 0 && qpart2 == 0 )
    {
      deltaPhi = qpart1 * deltaPhi;
    }
  if ( qpart1 == 0 && qpart2 != 0 )
    {
      deltaPhi = -1.0 * qpart2 * deltaPhi;
    }

  // See if this deltaPhi passes the cuts
  if ( deltaPhi > deltaPhiMin && deltaPhi < deltaPhiMax )
    {
      return true;
    }
  else
    {
      return false;
    }
}



//=============================================================================
// deltaPhi filter
//=============================================================================
bool FilterTool::deltaPhiFilter( const HepLorentzVector& part1,
                                 const HepLorentzVector& part2,
                                 double deltaPhiMin, double deltaPhiMax )
{
  double deltaPhi = part1.getV().deltaPhi( part2.getV() );
  if ( deltaPhi > deltaPhiMin && deltaPhi < deltaPhiMax )
    {
      return true;
    }
  else
    {
      return false;
    }
}





//=============================================================================
// deltaR filter
//=============================================================================
bool FilterTool::deltaRFilter( const INavigable4Momentum* part1, const INavigable4Momentum* part2,
                               double deltaRMin, double deltaRMax )
{
  return deltaRFilter( part1->hlv(), part2->hlv(), deltaRMin, deltaRMax );
}



//=============================================================================
// deltaR filter
//=============================================================================
bool FilterTool::deltaRFilter( const HepLorentzVector& part1,
                               const HepLorentzVector& part2,
                               double deltaRMin, double deltaRMax )
{
  double deltaR = part1.deltaR( part2 );
  if ( deltaR > deltaRMin && deltaR < deltaRMax )
    {
      return true;
    }
  else
    {
      return false;
    }
}





//=============================================================================
// Charge filter
//=============================================================================
bool FilterTool::chargeFilter( const IParticle* part,
                               int chargeMin,
                               int chargeMax,
                               bool allowChargeConj )
{
  // Check if the particle actually has the charge defined
  if ( !(part->hasCharge()) )
    {
      return true;
    }
  else
    {
      int charge = (int)part->charge();
      if ( allowChargeConj )
        {
          if ( std::abs(charge) >= chargeMin && std::abs(charge) <= chargeMax )
            {
              return true;
            }
          else
            {
              return false;
            }
        }
      else
        {
          if ( charge >= chargeMin && charge <= chargeMax )
            {
              return true;
            }
          else
            {
              return false;
            }
        } // End: if/else allowChargeConjugate


    }

}


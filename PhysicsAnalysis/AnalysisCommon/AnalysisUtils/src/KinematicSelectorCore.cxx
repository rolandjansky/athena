/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 * @Project: AnalysisUtils
 *
 * @class KinematicSelectorCore
 *
 * @author Amir Farbin <amir.farbin@cernSPAMNOT.ch>
 * @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
 *
 * @date June 2009
 *
 * @brief Dual use tool (athena/ARA) for kinematic cuts.
 *
 *****************************************************************************/

// This class's header
#include "AnalysisUtils/KinematicSelectorCore.h"

// STL includes
#include <string>
#include <cfloat>

// Atlas includes
#include "EventKernel/INavigable4Momentum.h"


//=============================================================================
// The contructor
//=============================================================================
KinematicSelectorCore::KinematicSelectorCore( PropertyMgr *pmgr ): 
  AraToolBase(pmgr),
  ISelectorCore(pmgr)
{

  //Need a mechanism to turn on/off a cut
  // The default values are such that the cut is effectefly turned off.

  declareProperty( "minPt", m_minPt=-DBL_MAX, "Min Pt");
  declareProperty( "maxPt", m_maxPt=DBL_MAX,  "Max Pt");

  declareProperty( "minP", m_minP=-DBL_MAX, "Min P");
  declareProperty( "maxP", m_maxP=DBL_MAX,  "Max P");

  declareProperty( "minEt", m_minEt=-DBL_MAX, "Min Et");
  declareProperty( "maxEt", m_maxEt=DBL_MAX,  "Max Et");
 
  declareProperty( "minE", m_minE=-DBL_MAX, "Min E");
  declareProperty( "maxE", m_maxE=DBL_MAX,  "Max E");

  declareProperty( "minEta", m_minEta=-DBL_MAX, "Min Eta");
  declareProperty( "maxEta", m_maxEta=DBL_MAX,  "Max Eta");

  declareProperty( "minAbsEta", m_minAbsEta=-DBL_MAX, "Min AbsEta");
  declareProperty( "maxAbsEta", m_maxAbsEta=DBL_MAX,  "Max AbsEta");

  declareProperty( "etaVetoRanges", m_etaVetoRanges="", 
                   "Eta ranges in which the object should NOT be. Example: [[-1.52, -1.37], [1.37, 1.52]]");

  declareProperty( "minPhi", m_minPhi=-DBL_MAX, "Min Phi");
  declareProperty( "maxPhi", m_maxPhi=DBL_MAX,  "Max Phi");

  declareProperty( "phiVetoRanges", m_phiVetoRanges="", 
                   "Phi ranges in which the object should NOT be. Example: [[-1.4, -1.2], [1.1, 1.5]]");

  declareProperty( "minMass", m_minMass=-DBL_MAX, "Min Mass");
  declareProperty( "maxMass", m_maxMass=DBL_MAX,  "Max Mass");

}



//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode KinematicSelectorCore::initialize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;
  
  //-----------------------------------------------------------------
  // Process the strings of veto ranges 
  // and translate it into the double boundaries

  // Make sure the eta string is not empty
  if ( !(m_etaVetoRanges.empty()) )
    {
      convertStringRange( m_etaVetoRanges, m_minEtaVeto, m_maxEtaVeto );
    } // End: if m_etaVetoRanges is not empty

  // Make sure the phi string is not empty
  if ( !(m_phiVetoRanges.empty()) )
    {
      convertStringRange( m_phiVetoRanges, m_minPhiVeto, m_maxPhiVeto );
    } // End: if m_phiVetoRanges is not empty

  // End processing of the veto ranges string
  //-----------------------------------------------------------------


  return sc ;
}



//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode KinematicSelectorCore::finalize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;
  
  return sc ;
}



//=============================================================================
// The main accept method for IParticle
//=============================================================================
bool KinematicSelectorCore::accept( const IParticle *part ) const
{
  // Default return
  //bool pass(true);

  // Try to cast the IParticle to INavigable4Momentum
  // and then use the accept(INavigable4Momentum*) methods.
  // This should always work since the IParticle class inherits 
  // from INavigable4Momentum.
  const INavigable4Momentum* inav(NULL);
  inav = dynamic_cast< const INavigable4Momentum* >(part);
  if ( inav != NULL )
    {
      return accept(inav);
    }
  else
    {
      std::cerr << "ERROR! Couldn't cast to INavigable4Momentum!"
                << " But this should always work from an IParticle!"
                << std::endl;
    }

  return false;
}


//=============================================================================
// The main accept method for INavigable4Momentum
//=============================================================================
bool KinematicSelectorCore::accept( const INavigable4Momentum *part ) const
{
  // This can be a lot smarter.
  bool pass(true);
  
  pass = pass && passPt(part) ;
  pass = pass && passP(part) ;
  pass = pass && passEt(part) ;
  pass = pass && passE(part) ;
  pass = pass && passEta(part) ;
  pass = pass && passAbsEta(part) ;
  pass = pass && passEtaVetoRanges(part) ;
  pass = pass && passPhi(part) ;
  pass = pass && passPhiVetoRanges(part) ;
  pass = pass && passMass(part) ;

  return pass;
}



// The check if the object (part) is in an eta veto region
bool KinematicSelectorCore::passEtaVetoRanges( const INavigable4Momentum* part ) const
{
  // Default return
  bool isInVetoRange = false;

  double eta = part->eta();
  if ( (std::isinf(eta) || std::isnan(eta)) ) return true;

  // Check consistency of upper and lower range boundaries
  // and a non-empty string
  if ( !(m_etaVetoRanges.empty())
       && m_minEtaVeto.size() == m_maxEtaVeto.size() )
    {
      // Loop over all ranges
      for ( unsigned int i=0; i<m_minEtaVeto.size(); ++i )
        {
          if ( eta > m_minEtaVeto[i] && eta < m_maxEtaVeto[i] )
            {
              isInVetoRange = true;
            }
        }
    } // End: if ( m_minEtaVeto.size() == m_maxEtaVeto.size() )

  return !isInVetoRange;
}



// The check if the object (part) is in an phi veto region
bool KinematicSelectorCore::passPhiVetoRanges( const INavigable4Momentum* part ) const
{
  // Default return
  bool isInVetoRange = false;

  double phi = part->phi();
  if ( (std::isinf(phi) || std::isnan(phi)) ) return true;

  // Check consistency of upper and lower range boundaries
  // and a non-empty string
  if ( !(m_phiVetoRanges.empty())
       && m_minPhiVeto.size() == m_maxPhiVeto.size() )
    {
      // Loop over all ranges
      for ( unsigned int i=0; i<m_minPhiVeto.size(); ++i )
        {
          if ( phi > m_minPhiVeto[i] && phi < m_maxPhiVeto[i] )
            {
              isInVetoRange = true;
            }
        }
    } // End: if ( m_minPhiVeto.size() == m_maxPhiVeto.size() )

  return !isInVetoRange;
}



// Convert the string ranges in range into double min and max vectors
void KinematicSelectorCore::convertStringRange( std::string &range,
                                                std::vector<double> &minVals, 
                                                std::vector<double> &maxVals )
{
  // Copy the string for further prcessing
  std::string rangeCopy(range);
  
  // Remove all spaces
  for ( unsigned int i=0; i<rangeCopy.length(); i++ ) 
    {
      if ( rangeCopy[i]==' ' )
        {
          rangeCopy.erase(i,1);
          i--;
        }
    }

  // Find the position of all open and closed brackets
  std::vector<size_t> openBracketPositions;
  std::vector<size_t> closedBracketPositions;
  for ( size_t i=0; i < rangeCopy.size(); ++i )
    {
      if ( rangeCopy.compare( i, 1, "[") == 0 ) 
        {
          openBracketPositions.push_back( i );
        }
      if ( rangeCopy.compare( i, 1, "]") == 0 ) 
        {
          closedBracketPositions.push_back( i );
        }
    }

  // Consistency checks: number([) == number(])
  if ( openBracketPositions.size() != closedBracketPositions.size() )
    {
//       if (msgLvl(MSG::WARNING))
//         {
//           msg(MSG::WARNING) 
      std::cerr 
        << "Inconsistent number of open and closed brackets! " 
        << " numberOpen=" << openBracketPositions.size()
        << " numberClosed=" << closedBracketPositions.size()
        << " and cut string=" << rangeCopy
        << std::endl;
//             << endreq ;
//         }
    }
  
  // Now, get the actual boundaries
  for ( unsigned int i=0; i<openBracketPositions.size(); ++i )
    {
      std::string newRange;
      newRange = rangeCopy.substr( openBracketPositions[i]+1,
                                   closedBracketPositions[i] );
      size_t comma = newRange.find(",");
      minVals.push_back( (double)strtod( (newRange.substr( 0, comma )).c_str(), 
                                        NULL ) );
      maxVals.push_back( (double)strtod( (newRange.substr( comma+1 )).c_str(),
                                        NULL ) );
    }

}




// Convert the string ranges in range into double min and max vectors
void KinematicSelectorCore::setEtaVetoRanges(std::string &range )
{
  // Set the member variable to the given input
  m_etaVetoRanges = range;

  // Decode the other member variables again
 
  // Make sure the eta string is not empty
  if ( !(m_etaVetoRanges.empty()) )
    {
      convertStringRange( m_etaVetoRanges, m_minEtaVeto, m_maxEtaVeto );
    } // End: if m_etaVetoRanges is not empty

}




// Convert the string ranges in range into double min and max vectors
void KinematicSelectorCore::setPhiVetoRanges(std::string &range )
{
  // Set the member variable to the given input
  m_phiVetoRanges = range;

  // Decode the other member variables again

  // Make sure the phi string is not empty
  if ( !(m_phiVetoRanges.empty()) )
    {
      convertStringRange( m_phiVetoRanges, m_minPhiVeto, m_maxPhiVeto );
    } // End: if m_phiVetoRanges is not empty

}

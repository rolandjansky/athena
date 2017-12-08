/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EMTrackMatch.cxx
PACKAGE:  offline/Reconstruction/egammaEvent

AUTHORS:  H. Ma, S. Rajagopalan
CREATED:  Dec. 5, 2000

PURPOSE:  object containing information about the match of a calo cluster 
          with the best matched track in front of the shower. Information 
	  about the different variables can be found in EMTrackMatch.h

Updated:  Mar 2003  (AP)
          change data member definitions
          Jan 13, 2001   (MW)
          add some more comments
          Nov 24, 2003   (MW)
          add parameter for the track refittingoption
          Dec 10, 2003  (DR)
          add SimpleTrack pointer
Updated:  Jan, 2004 (FD+AK)
          add deltaEta, deltaPhi for all samplings
          Jan 23, 2004 (FD) add initialisation of all deltaEta, deltaPhi
          Apr 28, 2004 (FD) add eta of track corrected for magnetic field
          May 06, 2009 (FD) add track isolation variable
          May 06, 2009 (FD) remove track isolation variable !
          May 31, 2009 (FD/RD) add three variables for track extrapolation
          Feb 01, 2010 (FD) add transverse impact parameter values wrt beam spot
          Mar 03, 2010 (FD) add transverse impact parameter values 
                            wrt primary vertex and deltaPhiRot
          Mar 05, 2010 (FD) add EtaExtrap, PhiExtrap and methods
          Mar 16, 2010 (FD) add PhiRescaled in each sampling
********************************************************************/

// INCLUDE HEADER FILES:
#include <math.h>
#include "egammaEvent/EMTrackMatch.h"
#include "SGTools/BaseInfo.h"
#include "SGTools/ClassName.h"
#include "GaudiKernel/GaudiException.h"

//  END OF HEADER FILES INCLUDE

// ----------------------------
//  Constructor
// ----------------------------

// give default values
EMTrackMatch::EMTrackMatch()
  :	egDetail() 
{    
}

// =================================================================
// constructor with data
EMTrackMatch::EMTrackMatch(const std::vector<double>& EtaExtrap,
                           const std::vector<double>& PhiExtrap,
			   const std::vector<double>& deltaEta,
                           const std::vector<double>& deltaPhi,
                           bool                       isPrimary,
                           int                        trackNumber,
                           bool                       trackRefit,
			   int                        linkIndex)
  :     egDetail()
{
  set_boolParameter(egammaParameters::isPrimary, isPrimary,true);
  set_boolParameter(egammaParameters::trackRefit, trackRefit,true);
  set_parameter    (egammaParameters::trackNumber, (double) trackNumber, true);
  set_parameter    (egammaParameters::tracksInBroadWindow, (double) 0., true);
  set_parameter    (egammaParameters::linkIndex, (double) linkIndex, true);
  for (unsigned int i = 0; i < EtaExtrap.size(); ++i) 
    setEtaExtrap (i, EtaExtrap[i], true);
  for (unsigned int i = 0; i < PhiExtrap.size(); ++i) 
    setPhiExtrap (i, PhiExtrap[i], true);
  for (unsigned int i = 0; i < deltaEta.size(); ++i) 
    setDeltaEta (i, deltaEta[i], true);
  for (unsigned int i = 0; i < deltaPhi.size(); ++i) 
    setDeltaPhi (i, deltaPhi[i], true);

}

// =================================================================
// constructor with data
EMTrackMatch::EMTrackMatch(const std::vector<double>& deltaEta,
                           const std::vector<double>& deltaPhi,
                           bool                       isPrimary,
                           int                        trackNumber,
                           bool                       trackRefit,
			   int                        linkIndex)
  :     egDetail()
{
  set_boolParameter(egammaParameters::isPrimary, isPrimary,true);
  set_boolParameter(egammaParameters::trackRefit, trackRefit,true);
  set_parameter    (egammaParameters::trackNumber, (double) trackNumber, true);
  set_parameter    (egammaParameters::tracksInBroadWindow, (double) 0., true);
  set_parameter    (egammaParameters::linkIndex, (double) linkIndex, true);
  for (unsigned int i = 0; i < deltaEta.size(); ++i) setDeltaEta (i, deltaEta[i], true);
  for (unsigned int i = 0; i < deltaPhi.size(); ++i) setDeltaPhi (i, deltaPhi[i], true);

}

// =================================================================
EMTrackMatch::~EMTrackMatch()
{ 
  //
  // destructor
  //
}

/// interface methods
const std::string& EMTrackMatch::className() const
{
  static std::string className = ClassName<EMTrackMatch>::name();
  return className;
}

// ===============================================================
double EMTrackMatch::parameter(egammaParameters::ParamDef key) const 
{
  if ( hasBoolParameter(key) ) {
    if ( boolParameter(key) ) return 1.;
    return 0;
  }

  typedef std::pair<egammaParameters::ParamDef,double> elParams;

  double result = egammaParameters::EgParamUndefined;
  std::vector<elParams>::const_iterator p = m_parameters.begin();
 
  for (;p !=m_parameters.end(); p++) {
    if ( (*p).first == key ){
      result = (*p).second;
      break;
    }
  }

  return result;
}

// ==============================================================
void EMTrackMatch::set_parameter(egammaParameters::ParamDef key, double value, bool overwrite) 
{
  if ( hasBoolParameter(key) ) {
    if (value != 0.)
      set_boolParameter(key, true, overwrite);
    else
      set_boolParameter(key, false, overwrite);

    return;
  }

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
      throw GaudiException("parameter not saved", "EMTrackMatch::set_parameter(...)", false);
    }
  }

}

// ====================================================================
void EMTrackMatch::set_boolParameter(egammaParameters::ParamDef key, bool value, bool overwrite) 
{
  typedef std::pair<egammaParameters::ParamDef,bool> elParams;

  std::vector<elParams>::iterator p = m_parametersBool.begin();
 
  for (;p !=m_parametersBool.end(); p++) {
    if ( (*p).first == key ) break;
  }

  if ( p == m_parametersBool.end() ) {
    m_parametersBool.push_back( elParams(key,value) );
  }
  else {
    if ( overwrite ) {
      (*p).second = value;
    }
    else {      
      throw GaudiException("parameter not saved", "EMTrackMatch::set_boolparameter(...)", false);
    }
  }
}

// ====================================================================
bool EMTrackMatch::boolParameter(egammaParameters::ParamDef key) const 
{
  typedef std::pair<egammaParameters::ParamDef,bool> elParams;

  double result = false;
  std::vector<elParams>::const_iterator p = m_parametersBool.begin();
 
  for (;p !=m_parametersBool.end(); p++) {
    if ( (*p).first == key ) {
      result = (*p).second;
      break;
    }
  }

  return result;
}

// ===================================================================
void EMTrackMatch::setEtaExtrap (int sampl, double value, bool overwrite) 
{
  //
  // Eta extrapolated at presampler(0) -> 3rd sampling (3)
  // 
  egammaParameters::ParamDef key = egammaParameters::EtaExtraps1;
  if ( sampl == 0) key = egammaParameters::EtaExtraps0;
  else if ( sampl == 1) key = egammaParameters::EtaExtraps1;
  else if ( sampl == 2) key = egammaParameters::EtaExtraps2;
  else if ( sampl == 3) key = egammaParameters::EtaExtraps3;
  return set_parameter(key, value, overwrite); 
}

// ===================================================================    
void EMTrackMatch::setPhiExtrap (int sampl, double value, bool overwrite) 
{
  //
  // Phi extrapolated at presampler (0) -> 3rd sampling (3)
  //  
  egammaParameters::ParamDef key = egammaParameters::PhiExtraps2;
  if ( sampl == 0) key = egammaParameters::PhiExtraps0;
  else if ( sampl == 1) key = egammaParameters::PhiExtraps1;
  else if ( sampl == 2) key = egammaParameters::PhiExtraps2;
  else if ( sampl == 3) key = egammaParameters::PhiExtraps3;
  return set_parameter(key, value, overwrite); 
}

// ===================================================================    
void EMTrackMatch::setPhiRescaled (int sampl, double value, bool overwrite) 
{
  //
  // Phi rescaled at presampler (0) -> 3rd sampling (3)
  //  
  egammaParameters::ParamDef key = egammaParameters::PhiRescaled2;
  if ( sampl == 0) key = egammaParameters::PhiRescaled0;
  else if ( sampl == 1) key = egammaParameters::PhiRescaled1;
  else if ( sampl == 2) key = egammaParameters::PhiRescaled2;
  else if ( sampl == 3) key = egammaParameters::PhiRescaled3;
  return set_parameter(key, value, overwrite); 
}

// ===================================================================
void EMTrackMatch::setDeltaEta (int sampl, double value, bool overwrite) 
{
  //
  // deltaEta at presampler(0) -> 3rd sampling (3)
  // 
  egammaParameters::ParamDef key = egammaParameters::deltaEta1;
  if ( sampl == 0) key = egammaParameters::deltaEta0;
  else if ( sampl == 1) key = egammaParameters::deltaEta1;
  else if ( sampl == 2) key = egammaParameters::deltaEta2;
  else if ( sampl == 3) key = egammaParameters::deltaEta3;
  return set_parameter(key, value, overwrite); 
}

// ===================================================================    
void EMTrackMatch::setDeltaPhi (int sampl, double value, bool overwrite) 
{
  //
  // deltaPhi at presampler (0) -> 3rd sampling (3)
  //  
  egammaParameters::ParamDef key = egammaParameters::deltaPhi2;
  if ( sampl == 0) key = egammaParameters::deltaPhi0;
  else if ( sampl == 1) key = egammaParameters::deltaPhi1;
  else if ( sampl == 2) key = egammaParameters::deltaPhi2;
  else if ( sampl == 3) key = egammaParameters::deltaPhi3;
  return set_parameter(key, value, overwrite); 
}

// =============================================================
void EMTrackMatch::setEtaSampling1 (double value, bool overwrite) 
{
  //
  //  eta at sampling 1 for extrapolation from the last hit -
  //  allows recalcultion of deltaEta
  //
  return set_parameter(egammaParameters::etaSampling1, value, overwrite); 
}

// ===========================================================
void EMTrackMatch::setDeltaPhiRescaled (double value, bool overwrite) 
{
  //
  // deltaPhi at sampling 2 for rescaled momentum extrapolation 
  // from the perigee 
  // Only available if not in deltaPhi, i.e. default
  //
  return set_parameter(egammaParameters::deltaPhiRescaled, value, overwrite); 
}

// ===========================================================
void EMTrackMatch::setextrapMethod (TrkExtrapDef value, bool overwrite)
{
  //
  // extrapolation method used for track match
  //
    return set_parameter(egammaParameters::extrapMethod, (double) value, overwrite); 
}

// ===========================================================
void EMTrackMatch::setDeltaPhiRot (double value, bool overwrite) 
{
  //
  // deltaPhi at sampling 2 for rescaled momentum extrapolation 
  // from the perigee 
  // Only available if not in deltaPhi, i.e. default
  //
  return set_parameter(egammaParameters::deltaPhiRot, value, overwrite); 
}

// ===========================================================
void EMTrackMatch::setDeltaPhiLast (double value, bool overwrite) 
{
  //
  // deltaPhi at sampling 2 from the last measurement
  //
  return set_parameter(egammaParameters::deltaPhiLast, value, overwrite); 
}


/*// ===========================================================
void EMTrackMatch::setexpectHitInBLayer (bool value, bool overwrite) 
{
  //
  // boolean to see if hit was expected in b-layer
  //
  return set_boolParameter(egammaParameters::expectHitInBLayer, value, overwrite); 
}
*/

// =================================================================
// general reset:
void EMTrackMatch::fill(const std::vector<double>& EtaExtrap,
			const std::vector<double>& PhiExtrap,
			const std::vector<double>& deltaEta,
			const std::vector<double>& deltaPhi,
			bool                       isPrimary,
			int                        trackNumber,
			bool                       trackRefit,
			int                        linkIndex)
{
  set_parameter(egammaParameters::trackNumber, (double) trackNumber, true);
  set_parameter(egammaParameters::tracksInBroadWindow, (double) 0., true);
  set_parameter(egammaParameters::linkIndex, (double) linkIndex, true);
  for (unsigned int i = 0; i < EtaExtrap.size(); ++i) 
    setDeltaEta (i, EtaExtrap[i], true);
  for (unsigned int i = 0; i < PhiExtrap.size(); ++i) 
    setDeltaPhi (i, PhiExtrap[i], true);
  for (unsigned int i = 0; i < deltaEta.size(); ++i) 
    setDeltaEta (i, deltaEta[i], true);
  for (unsigned int i = 0; i < deltaPhi.size(); ++i) 
    setDeltaPhi (i, deltaPhi[i], true);
  set_boolParameter(egammaParameters::isPrimary, isPrimary, true);
  set_boolParameter(egammaParameters::trackRefit, trackRefit, true);
}

// =================================================================
// general reset:
void EMTrackMatch::fill(const std::vector<double>& deltaEta,
			const std::vector<double>& deltaPhi,
			bool                       isPrimary,
			int                        trackNumber,
			bool                       trackRefit,
			int                        linkIndex)
{
  set_parameter(egammaParameters::trackNumber, (double) trackNumber, true);
  set_parameter(egammaParameters::tracksInBroadWindow, (double) 0., true);
  set_parameter(egammaParameters::linkIndex, (double) linkIndex, true);
  for (unsigned int i = 0; i < deltaEta.size(); ++i) setDeltaEta (i, deltaEta[i], true);
  for (unsigned int i = 0; i < deltaPhi.size(); ++i) setDeltaPhi (i, deltaPhi[i], true);
  set_boolParameter(egammaParameters::isPrimary, isPrimary, true);
  set_boolParameter(egammaParameters::trackRefit, trackRefit, true);
}

// ========================================================================
// only E/p
void
EMTrackMatch::fill(int			trackNumber)
{
  set_parameter(egammaParameters::trackNumber, (double) trackNumber, true);
}

// ======================================================================
// only EtaExtrap, PhiExtrap
void
EMTrackMatch::fillEtaPhiExtrap(const std::vector<double>& EtaExtrap,
			       const std::vector<double>& PhiExtrap)
{
  for (unsigned int i = 0; i < EtaExtrap.size(); ++i) 
    setEtaExtrap (i, EtaExtrap[i], true);
  for (unsigned int i = 0; i < PhiExtrap.size(); ++i) 
    setPhiExtrap (i, PhiExtrap[i], true);
}

// ======================================================================
// only deltaEta, deltaPhi
void
EMTrackMatch::fill(const std::vector<double>& deltaEta,
                   const std::vector<double>& deltaPhi)
{
  for (unsigned int i = 0; i < deltaEta.size(); ++i) setDeltaEta (i, deltaEta[i], true);
  for (unsigned int i = 0; i < deltaPhi.size(); ++i) setDeltaPhi (i, deltaPhi[i], true);
}

// ========================================================================
// print track match quantities
void EMTrackMatch::print() const
{

//     std::cout << "Params: " << std::endl;
    
//     typedef std::vector < std::pair<egammaParameters::ParamDef , double > >::const_iterator it_type;
//     it_type it    = m_parameters.begin();
//     it_type itend = m_parameters.end();
//     for (; it != itend; ++it) {
//         std::cout << "n,value " << it->first << " " << it->second << std::endl;
//     }

//     std::cout <<

//     std::cout << "egammaParameters::deltaEta1            " << egammaParameters::deltaEta1             << std::endl;
//     std::cout << "egammaParameters::deltaPhi2            " << egammaParameters::deltaPhi2             << std::endl;
//     std::cout << "egammaParameters::trackNumber          " << egammaParameters::trackNumber           << std::endl;
//     std::cout << "egammaParameters::linkIndex            " << egammaParameters::linkIndex             << std::endl;
//     std::cout << "egammaParameters::tracksInBroadWindow  " << egammaParameters::tracksInBroadWindow   << std::endl;
//     std::cout << "egammaParameters::etaSampling1         " << egammaParameters::etaSampling1          << std::endl;
//     std::cout << "egammaParameters::deltaPhiRescaled     " << egammaParameters::deltaPhiRescaled      << std::endl;
//     std::cout << "egammaParameters::extrapMethod         " << egammaParameters::extrapMethod          << std::endl;
//     std::cout << "egammaParameters::deltaEta0            " << egammaParameters::deltaEta0             << std::endl;
//     std::cout << "egammaParameters::deltaEta2            " << egammaParameters::deltaEta2             << std::endl;
//     std::cout << "egammaParameters::deltaEta3            " << egammaParameters::deltaEta3             << std::endl;
//     std::cout << "egammaParameters::deltaPhi0            " << egammaParameters::deltaPhi0             << std::endl;
//     std::cout << "egammaParameters::deltaPhi1            " << egammaParameters::deltaPhi1             << std::endl;
//     std::cout << "egammaParameters::deltaPhi3            " << egammaParameters::deltaPhi3             << std::endl;
}

// ==============================================================
bool EMTrackMatch::hasParameter(egammaParameters::ParamDef key) const {
  if ( hasBoolParameter(key) ) return true;
  if (key == egammaParameters::deltaEta1) return true;    
  if (key == egammaParameters::deltaPhi2) return true;
  if (key == egammaParameters::trackNumber) return true;
  if (key == egammaParameters::linkIndex) return true;
  if (key == egammaParameters::tracksInBroadWindow) return true;    
  if (key == egammaParameters::etaSampling1) return true;    
  if (key == egammaParameters::deltaPhiRescaled) return true;    
  if (key == egammaParameters::extrapMethod) return true;    

  // The following are less often used, so we leave them at the end - RDS
  if (key == egammaParameters::EtaExtraps0) return true;
  if (key == egammaParameters::EtaExtraps1) return true;
  if (key == egammaParameters::EtaExtraps2) return true;
  if (key == egammaParameters::EtaExtraps3) return true;

  if (key == egammaParameters::PhiExtraps0) return true;
  if (key == egammaParameters::PhiExtraps1) return true;
  if (key == egammaParameters::PhiExtraps2) return true;
  if (key == egammaParameters::PhiExtraps3) return true;

  if (key == egammaParameters::PhiRescaled0) return true;
  if (key == egammaParameters::PhiRescaled1) return true;
  if (key == egammaParameters::PhiRescaled2) return true;
  if (key == egammaParameters::PhiRescaled3) return true;

  if (key == egammaParameters::deltaEta0) return true;
  if (key == egammaParameters::deltaEta2) return true;
  if (key == egammaParameters::deltaEta3) return true;
  if (key == egammaParameters::deltaPhi0) return true;
  if (key == egammaParameters::deltaPhi1) return true;
  if (key == egammaParameters::deltaPhi3) return true;

  if (key == egammaParameters::deltaPhiRot) return true;
  if (key == egammaParameters::deltaPhiLast) return true;    

  if (key == egammaParameters::trackd0_physics) return true;
  if (key == egammaParameters::trackd0_pv) return true;
  if (key == egammaParameters::trackd0sigma_physics) return true;
  if (key == egammaParameters::trackd0sigma_pv) return true;

  return false;
}    

// =================================================================
bool EMTrackMatch::hasBoolParameter(egammaParameters::ParamDef key) const 
{
  if (key == egammaParameters::isPrimary) return true;
  if (key == egammaParameters::trackRefit) return true;
  if (key == egammaParameters::expectHitInBLayer) return true;
  return false;
}    

// ===============================================================
double EMTrackMatch::EtaExtrap (int sampl) const 
{ 
  //
  // Eta extrapolated at presampler(0) -> 3rd sampling (3)
  // 

  egammaParameters::ParamDef key = egammaParameters::EtaExtraps1;
  if ( sampl == 0) key = egammaParameters::EtaExtraps0;
  else if ( sampl == 1) key = egammaParameters::EtaExtraps1;
  else if ( sampl == 2) key = egammaParameters::EtaExtraps2;
  else if ( sampl == 3) key = egammaParameters::EtaExtraps3;
  return parameter(key); 
}

// ===============================================================    
double EMTrackMatch::PhiExtrap (int sampl) const 
{
  //
  // Phi extrapolated at presampler (0) -> 3rd sampling (3)
  //  
  egammaParameters::ParamDef key = egammaParameters::PhiExtraps2;
  if ( sampl == 0) key = egammaParameters::PhiExtraps0;
  else if ( sampl == 1) key = egammaParameters::PhiExtraps1;
  else if ( sampl == 2) key = egammaParameters::PhiExtraps2;
  else if ( sampl == 3) key = egammaParameters::PhiExtraps3;
  return parameter(key); 
}

// ===============================================================    
double EMTrackMatch::PhiRescaled (int sampl) const 
{
  //
  // Phi rescaled at presampler (0) -> 3rd sampling (3)
  //  
  egammaParameters::ParamDef key = egammaParameters::PhiRescaled2;
  if ( sampl == 0) key = egammaParameters::PhiRescaled0;
  else if ( sampl == 1) key = egammaParameters::PhiRescaled1;
  else if ( sampl == 2) key = egammaParameters::PhiRescaled2;
  else if ( sampl == 3) key = egammaParameters::PhiRescaled3;
  return parameter(key); 
}

// ===============================================================
double EMTrackMatch::deltaEta (int sampl) const 
{ 
  //
  // deltaEta at presampler(0) -> 3rd sampling (3)
  // 

  egammaParameters::ParamDef key = egammaParameters::deltaEta1;
  if ( sampl == 0) key = egammaParameters::deltaEta0;
  else if ( sampl == 1) key = egammaParameters::deltaEta1;
  else if ( sampl == 2) key = egammaParameters::deltaEta2;
  else if ( sampl == 3) key = egammaParameters::deltaEta3;
  return parameter(key); 
}

// ===============================================================    
double EMTrackMatch::deltaPhi (int sampl) const 
{
  //
  // deltaPhi at presampler (0) -> 3rd sampling (3)
  //  
  egammaParameters::ParamDef key = egammaParameters::deltaPhi2;
  if ( sampl == 0) key = egammaParameters::deltaPhi0;
  else if ( sampl == 1) key = egammaParameters::deltaPhi1;
  else if ( sampl == 2) key = egammaParameters::deltaPhi2;
  else if ( sampl == 3) key = egammaParameters::deltaPhi3;
  return parameter(key); 
}

// =============================================================
double EMTrackMatch::etaSampling1 () const 
{ 
  //
  // eta at sampling 1 for extrapolation from the last hit - 
  // allows recalcultion of deltaEta
  // 
  return parameter(egammaParameters::etaSampling1); 
}

// ==============================================================
EMTrackMatch::TrkExtrapDef EMTrackMatch::extrapMethod () const
{
  //
  //  enum to indicate which track extrapolation method was used for
  //  track match
  //
    return ((EMTrackMatch::TrkExtrapDef) parameter(egammaParameters::extrapMethod)); 
}

// ==============================================================
double EMTrackMatch::deltaPhiRescaled () const 
{
  //
  //deltaPhi at sampling 2 for rescaled momentum extrapolation 
  // from the perigee 
  // Only available if not in deltaPhi, i.e. default.
  //  
  return parameter(egammaParameters::deltaPhiRescaled); 
}


// ==============================================================
double EMTrackMatch::deltaPhiRot () const 
{
  //
  //deltaPhi at sampling 2 for rescaled momentum extrapolation 
  // from the perigee 
  // Only available if not in deltaPhi, i.e. default.
  //  
  return parameter(egammaParameters::deltaPhiRot); 
}

// ==============================================================
double EMTrackMatch::deltaPhiLast () const 
{
  //
  //deltaPhi from the last measurement
  //  
  return parameter(egammaParameters::deltaPhiLast); 
}

// =================================================================
bool EMTrackMatch::isPrimary () const
{ 
  //
  // true if matched track has a hit in first or second pixel layer
  //
  return boolParameter(egammaParameters::isPrimary); 
}
     
// ================================================================
int EMTrackMatch::trackNumber () const
{ 
  //
  // index number in TrackContainer (counting starts at 1 for CBNT)
  // 
  return (int) parameter(egammaParameters::trackNumber) ;
}

// ================================================================
int EMTrackMatch::linkIndex () const
{
  //
  // index number among TrackParticle links of egamma object 
  // (counting starts at 0)
  //  
  return (int) parameter(egammaParameters::linkIndex) ;
}

// ===============================================================
void EMTrackMatch::set_linkIndex (int value)
{ 
  set_parameter(egammaParameters::linkIndex, value, true); 
}

// ===============================================================
bool EMTrackMatch::trackRefit () const
{ 
  return boolParameter(egammaParameters::trackRefit) ; 
}
   
// ===============================================================
int EMTrackMatch::tracksInBroadWindow () const
{ 
  //
  // number of tracks in isolation window (taken at sampling 2):   
  // (# primary tracks if matched track is primary; otherwise all tracks)  
  // 
  return (int) parameter(egammaParameters::tracksInBroadWindow) ;
}
    
// ==================================================================
void EMTrackMatch::tracksInBroadWindow (int value, bool overwrite)
{ 
  set_parameter(egammaParameters::tracksInBroadWindow, value, overwrite); 
}

// ==============================================================
double EMTrackMatch::trackd0_physics () const 
{
  //
  // return track transverse impact parameter wrt beam spot 
  //  
  return parameter(egammaParameters::trackd0_physics); 
}

// ==============================================================
double EMTrackMatch::trackd0_pv () const 
{
  //
  // return track transverse impact parameter wrt primary vertex
  //  
  return parameter(egammaParameters::trackd0_pv); 
}

// ==============================================================
double EMTrackMatch::trackd0sigma_physics () const 
{
  //
  // return track transverse impact parameter wrt beam spot 
  //  
  return parameter(egammaParameters::trackd0sigma_physics); 
}

// ==============================================================
double EMTrackMatch::trackd0sigma_pv () const 
{
  //
  // return track transverse impact parameter wrt primary vertex
  //  
  return parameter(egammaParameters::trackd0sigma_pv); 
}

// =================================================================
bool EMTrackMatch::expectHitInBLayer () const
{ 
  //
  // true if expected hit in b-layer
  //
  return boolParameter(egammaParameters::expectHitInBLayer); 
}


SG_ADD_BASE (EMTrackMatch,  egDetail);

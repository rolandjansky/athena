/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/******************************************************************************
Name:        JetCleaningTool

Author:      Zach Marshall
Created:     Feb 2014

Description: Class for selecting jets that pass some cleaning cuts
******************************************************************************/

// This class header
#include "JetSelectorTools/JetCleaningTool.h"

// STL includes
#include <iostream>
#include <cmath>

//=============================================================================
// Constructors
//=============================================================================
JetCleaningTool::JetCleaningTool(const std::string& name)
  : asg::AsgTool(name)
  , m_cutName("")
  , cutLevel(VeryLooseBad)
{
  declareProperty( "CutLevel" , m_cutName = "" );
}

/** Cut-based constructor */
JetCleaningTool::JetCleaningTool(const CleaningLevel alevel)
  : asg::AsgTool( "JetCleaningTool_"+getCutName(alevel) )
  , m_cutName("")
  , cutLevel(alevel)
{
  declareProperty( "CutLevel" , m_cutName = getCutName(alevel) );
}

/** Cut and string based constructor */
JetCleaningTool::JetCleaningTool(const std::string& name , const CleaningLevel alevel)
  : asg::AsgTool(name)
  , m_cutName("")
  , cutLevel(alevel)
{
  declareProperty( "CutLevel" , m_cutName = getCutName(alevel) );
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode JetCleaningTool::initialize()
{
  if (m_cutName!="") cutLevel = getCutLevel( m_cutName );
  ATH_MSG_INFO( "Configured with cut level " << getCutName( cutLevel ) );

  m_accept.addCut( "Cleaning", "Cleaning of the jet" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Calculate the actual accept of each cut individually.
//=============================================================================
const Root::TAccept& JetCleaningTool::accept( const double emf,
                              const double hecf,
                              const double larq,
                              const double hecq,
                              const double time,     //in ns
                              const double sumpttrk, //in MeV, same as sumpttrk
                              const double eta,      //emscale Eta  
                              const double pt,       //in MeV, same as sumpttrk
                              const double fmax,
                              const double negE ,     //in MeV
                              const double AverageLArQF
                              ) const
{
  m_accept.clear();
  m_accept.setCutResult( "Cleaning", false );

  // -----------------------------------------------------------
  // Do the actual selection
  double chf=sumpttrk/pt;

  //=============================================================
  //VeryLoose cuts
  //=============================================================
  //Non-collision background & cosmics
  if(emf<0.05 && chf<0.05 && std::fabs(eta)<2)            return m_accept;
  if(emf<0.05 && std::fabs(eta)>=2)                       return m_accept;
  if(fmax>0.99 && std::fabs(eta)<2)                       return m_accept;
  //HEC spike
  if(std::fabs(negE/1000.)>60)                            return m_accept;
  if(hecf>0.5 && std::fabs(hecq)>0.5 && AverageLArQF/65535>0.8)                     return m_accept;
  //EM calo noise
  if(emf>0.95 && std::fabs(larq)>0.8 && std::fabs(eta)<2.8 && AverageLArQF/65535>0.8)    return m_accept;
  if (VeryLooseBad==cutLevel){
    m_accept.setCutResult( "Cleaning", true );
    return m_accept;
  }

 //=============================================================
  //Loose cuts
  //=============================================================
  //Non-collision background & cosmics
  if(std::fabs(time)>25)                                  return m_accept;
  //HEC spike
  if(hecf>0.5 && std::fabs(hecq)>0.5)                     return m_accept;
  //EM calo noise
  if(emf>0.95 && std::fabs(larq)>0.8 && std::fabs(eta)<2.8)    return m_accept;
  if (LooseBad==cutLevel){
    m_accept.setCutResult( "Cleaning", true );
    return m_accept; 
  } 

  //=============================================================
  //Additionnal medium cuts
  //=============================================================
  //Non-collision background & cosmics
  if(std::fabs(time)>10)                                return m_accept;
  if(emf<0.05 && chf<0.1  && std::fabs(eta)<2)          return m_accept;
  if(emf>0.95 && chf<0.05 && std::fabs(eta)<2)          return m_accept;
  //HEC spike
  if(hecf>1-std::fabs(hecq))                            return m_accept;
  //EM calo noise
  if(emf>0.9 && std::fabs(larq)>0.8 && std::fabs(eta)<2.8)   return m_accept;
  if (MediumBad==cutLevel){
    m_accept.setCutResult( "Cleaning", true );
    return m_accept; 
  } 

  //=============================================================
  //Additionnal tight cuts
  //=============================================================
  //Non-collision background & cosmics
  if(emf<0.1 && chf<0.2 && std::fabs(eta)<2.5)          return m_accept;
  if(emf<0.1 && std::fabs(eta)>2.5 )                    return m_accept;
  if(emf>0.9 && chf<0.1 && std::fabs(eta)<2.5)          return m_accept;
  //EM calo noise
  if(std::fabs(larq)>0.95)                              return m_accept;
  if(emf>0.98 && std::fabs(larq)>0.05)                  return m_accept;
  if(chf<0.01 && std::fabs(eta)<2.5 )                   return m_accept;
  if (TightBad==cutLevel){
    m_accept.setCutResult( "Cleaning", true );
    return m_accept; 
  } 

  // We should never arrive here!
  ATH_MSG_ERROR( "Unknown cut name: " << getCutName( cutLevel ) << " in JetCleaningTool" );
  return m_accept;
}

const Root::TAccept& JetCleaningTool::accept( const xAOD::Jet& jet) const
{
  std::vector<float> sumPtTrkvec;
  jet.getAttribute( xAOD::JetAttribute::SumPtTrkPt500, sumPtTrkvec );

  return accept (jet.getAttribute<float>("EMFrac"),
                 jet.getAttribute<float>("HECFrac"),
                 jet.getAttribute<float>("LArQuality"),
                 jet.getAttribute<float>("HECQuality"),
                 jet.getAttribute<float>("Timing"),
                 sumPtTrkvec[0],
                 jet.eta(),
                 jet.pt(),
                 jet.getAttribute<float>("FracSamplingMax"),
                 jet.getAttribute<float>("NegativeE"),
                 jet.getAttribute<float>("AverageLArQF"));
}

/** Helpers for cut names */
JetCleaningTool::CleaningLevel JetCleaningTool::getCutLevel( const std::string s ) const
{
  if (s=="VeryLooseBad") return VeryLooseBad;
  if (s=="LooseBad")     return LooseBad;
  if (s=="MediumBad")    return MediumBad;
  if (s=="TightBad")     return TightBad;
  ATH_MSG_ERROR( "Unknown cut level requested: " << s );
  return UnknownCut;  
}

std::string JetCleaningTool::getCutName( const CleaningLevel c) const
{
  if (c==VeryLooseBad) return "VeryLooseBad";
  if (c==LooseBad)     return "LooseBad";
  if (c==MediumBad)    return "MediumBad";
  if (c==TightBad)     return "TightBad";
  return "UnknownCut";
}


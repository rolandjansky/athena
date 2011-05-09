// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LVL1_ROI.h,v 1.4 2008-05-08 15:00:11 krasznaa Exp $
#ifndef ANALYSISTRIGGEREVENT_LVL1_ROI_H
#define ANALYSISTRIGGEREVENT_LVL1_ROI_H

// STL include(s):
#include <vector>

// Gaudi/Athena include(s):
#include "CLIDSvc/CLASS_DEF.h"

// Local include(s):
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "AnalysisTriggerEvent/Jet_ROI.h"
#include "AnalysisTriggerEvent/JetET_ROI.h"
#include "AnalysisTriggerEvent/EnergySum_ROI.h"

/**
 *   @short Top level AOD object storing LVL1 RoIs
 *
 *          This class is meant to be a container for all the Regions
 *          of Interest found by the LVL1 trigger system. The actual
 *          trigger information is stored in the RoI objects.
 *
 *  @author Tadashi Maeno
 *  @author Alan Watson
 *  @author Attila Krasznahorkay
 * @version $Revision: 1.4 $
 *    @date $Date: 2008-05-08 15:00:11 $
 *
 *     @see Muon_ROI
 *     @see EmTau_ROI
 *     @see Jet_ROI
 *     @see JetET_ROI
 *     @see EnergySum_ROI
 */
class LVL1_ROI {

public:
   /// Constructor
   LVL1_ROI();
   /// Destructor
   ~LVL1_ROI();

   /// Type storing muon RoIs
   typedef std::vector< Muon_ROI > muons_type;
   /// Type storing em/tau RoIs
   typedef std::vector< EmTau_ROI > emtaus_type;
   /// Type storing jet RoIs
   typedef std::vector< Jet_ROI > jets_type;
   /// Type storing jet-ET RoIs
   typedef std::vector< JetET_ROI > jetets_type;
   /// Type storing energy sum RoIs
   typedef std::vector< EnergySum_ROI > energysums_type;

   /// Get all the muon RoIs in the event
   const muons_type      & getMuonROIs      () const { return m_muonROIs; }
   /// Get all the em/tau RoIs in the event
   const emtaus_type     & getEmTauROIs     () const { return m_emtauROIs; }
   /// Get all the jet RoIs in the event
   const jets_type       & getJetROIs       () const { return m_jetROIs; }
   /// Get all the jet-ET RoIs in the event
   const jetets_type     & getJetEtROIs     () const { return m_jetetROIs; }
   /// Get all the energy sum RoIs in the event
   const energysums_type & getEnergySumROIs () const { return m_energysumROIs; }

   /// Add a muon RoI to the object
   void addMuonROI      ( const muons_type::value_type roi )      { m_muonROIs.push_back( roi ); }
   /// Add an em/tau RoI to the object
   void addEmTauROI     ( const emtaus_type::value_type roi )     { m_emtauROIs.push_back( roi ); }
   /// Add a jet RoI to the object
   void addJetROI       ( const jets_type::value_type roi )       { m_jetROIs.push_back( roi ); }
   /// Add a jet-ET RoI to the object
   void addJetETROI     ( const jetets_type::value_type roi )     { m_jetetROIs.push_back( roi ); }
   /// Add an energy sum RoI to the object
   void addEnergySumROI ( const energysums_type::value_type roi ) { m_energysumROIs.push_back( roi ); }

   /// Clear all the RoIs from the object
   void clear();

private:
   muons_type      m_muonROIs;      //!< Vector of muon RoIs
   emtaus_type     m_emtauROIs;     //!< Vector of em/tau RoIs
   jets_type       m_jetROIs;       //!< Vector of jet RoIs
   jetets_type     m_jetetROIs;     //!< Vector of jet-ET RoIs
   energysums_type m_energysumROIs; //!< Vector of energy sum RoIs

}; // class LVL1_ROI

CLASS_DEF( LVL1_ROI, 6771448, 1 )

#endif // ANALYSISTRIGGEREVENT_LVL1_ROI_H

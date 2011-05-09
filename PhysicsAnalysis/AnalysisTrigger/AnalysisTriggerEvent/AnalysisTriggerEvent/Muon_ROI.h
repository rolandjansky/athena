// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Muon_ROI.h,v 1.6 2008-05-08 15:00:11 krasznaa Exp $
#ifndef ANALYSISTRIGGEREVENT_MUON_ROI_H
#define ANALYSISTRIGGEREVENT_MUON_ROI_H

// STL/System include(s):
#include <string>
#include <inttypes.h>

// Athena include(s):
#include "EventKernel/INavigable4Momentum.h"
#include "FourMom/P4PtEtaPhiMBase.h"
#include "Navigation/NavigableTerminalNode.h"

/**
 *   @short Muon RoI class for analysis
 *
 *          The RoI now inherits from INavigable4Momentum to make it easier to use
 *          in an analysis (especially EventView).
 *
 *  @author Tadashi Maeno
 *  @author Attila Krasznahorkay
 * @version $Revision: 1.6 $
 *    @date $Date: 2008-05-08 15:00:11 $
 *
 *     @see INavigable4Momentum
 */
class Muon_ROI : public P4PtEtaPhiMBase,
                 public NavigableTerminalNode,
                 virtual public INavigable4Momentum {

public:
   /// Constructor used when creating the object from RoIBResult data
   Muon_ROI( uint32_t roiWord, double eta, double phi, const std::string &thrName,
             double thrValue );
   /// Default constructor (for persistency purposes)
   Muon_ROI();
   /// Destructor
   virtual ~Muon_ROI();

   /// RoI source enumeration
   enum RoISource {
      Barrel, //!< The muon candidate was detected in the barrel region
      Endcap, //!< The muon candidate was detected in the endcap region
      Forward //!< The muon candidate was detected in the forward region
   };
   /// RoI hemisphere enumeration
   enum Hemisphere {
      Positive, //!< The candidate came from the +z side (side A)
      Negative  //!< The candidate came from the -z side (side C)
   };
   /// Charge sign
   enum Charge {
      Neg = 0, //!< Candidate is a mu- from a TGC sector
      Pos = 1, //!< Candidate is a mu+ from a TGC sector
      Undef = 100 //!< This is an RPC candidate
   };

   /// Return the original 32-bit RoI word given by the LVL1 muon trigger
   uint32_t    getROIWord()   const { return m_roiWord; }
   /// Legacy function returning the eta coordinate of the RoI
   float       getEta()       const { return m_eta; }
   /// Legacy function returning the phi coordinate of the RoI
   float       getPhi()       const { return m_phi; }
   /// Return the name of the highest threshold this RoI passed
   std::string getThrName()   const { return m_thrName; }
   /// Return the value of the highest threshold this RoI passed
   float       getThrValue()  const { return m_thrValue; }

   /// Get the logic number of the highest threshold this RoI passed
   int getThrNumber() const;
   /// Get the "RoI number" (position inside the sector)
   int getRoI() const;
   /// Get the full sector address
   int getSectorAddress() const;
   /// Returns if the candidate had the highest p<sub>T</sub> in the sector
   bool isFirstCandidate() const;
   /// Returns if there were other muons detected in the same RoI
   bool isMoreCandInRoI() const;
   /// Returns if there were other muons detected in the same sector
   bool isMoreCandInSector() const;
   /// Returns the system that detected the muon candidate
   RoISource getSource() const;
   /// Returns the hemisphere that detected the muon candidate
   Hemisphere getHemisphere() const;
   /// Returns the charge sign of the muon candidate
   Charge getCharge() const;
   /// Returns the veto flag for the candidate
   bool isVetoed() const;

   /// Return the value of the highest threshold this RoI passed
   virtual double pt()  const { return m_thrValue; }
   /// Return the eta coordinate of the center of the RoI
   virtual double eta() const { return m_eta; }
   /// Return the phi coordinate of the center of the RoI
   virtual double phi() const { return m_phi; }
   /// Return the mass of the object, always 0.0 for RoIs
   virtual double m()   const { return 0.0; }

private:
   uint32_t m_roiWord; //!< 32bit encoded ROI word

   float m_eta; //!< eta coordinate of RoI
   float m_phi; //!< phi coordinate of RoI

   std::string m_thrName; //!< Name of the highest threshold this RoI passed
   float m_thrValue;      //!< Value of the highest threshold this RoI passed

}; // class Muon_ROI

#endif // ANALYSISTRIGGEREVENT_MUON_ROI_H

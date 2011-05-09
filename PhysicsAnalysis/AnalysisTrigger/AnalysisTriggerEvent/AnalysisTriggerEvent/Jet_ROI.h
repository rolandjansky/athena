// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Jet_ROI.h,v 1.8 2008-05-16 08:34:32 watsona Exp $
#ifndef ANALYSISTRIGGEREVENT_JET_ROI_H
#define ANALYSISTRIGGEREVENT_JET_ROI_H

// STL/System include(s):
#include <string>
#include <vector>
#include <inttypes.h>

// Athena include(s):
#include "EventKernel/INavigable4Momentum.h"
#include "FourMom/P4PtEtaPhiMBase.h"
#include "Navigation/NavigableTerminalNode.h"

/**
 *   @short Jet RoI class for analysis
 *
 *          As the others, this RoI class also inherits from INavigable4Momentum to make
 *          handling it a bit easier in analyses.
 *
 *  @author Tadashi Maeno, Alan Watson, Attila Krasznahorkay
 * @version $Revision: 1.8 $
 *    @date $Date: 2008-05-16 08:34:32 $
 */
class Jet_ROI : public P4PtEtaPhiMBase,
                public NavigableTerminalNode,
                virtual public INavigable4Momentum {

public:
   /// Constructor used when creating the object from RoIBResult data
   Jet_ROI( uint32_t roiWord, float eta, float phi, uint32_t thrPattern );
   /// Default constructor (for persistency purposes)
   Jet_ROI();
   /// Destructor
   virtual ~Jet_ROI();

   /// Type storing the names of the passed thresholds
   typedef std::vector< std::string > thresholds_type;
   /// Type storing the values of the passed thresholds
   typedef std::vector< float >       thr_value_type;

   /// Return the RoI word produced by the L1Calo hardware
   uint32_t    getROIWord()    const { return m_roiWord; }
   /// Legacy function returning the eta coordinate of the RoI
   float       getEta()        const { return m_eta; }
   /// Legacy function returning the phi coordinate of the RoI
   float       getPhi()        const { return m_phi; }
   /// Return the bit-pattern describing the passed thresholds
   uint32_t    getThrPattern() const { return m_thrPattern; }
   /// Return the ET deposited in a 4x4 cluster
   float       getET4x4()      const { return m_ET4x4; }
   /// Return the ET deposited in a 6x6 cluster
   float       getET6x6()      const { return m_ET6x6; }
   /// Return the ET deposited in an 8x8 cluster
   float       getET8x8()      const { return m_ET8x8; }

   /// Return the names of the passed threshodlds
   const thresholds_type& getThresholdNames()  const { return m_thresholdNames; }
   /// Return the values of the passed thresholds
   const thr_value_type&  getThresholdValues() const { return m_thresholdValues; }

   /// Add the name of a threshold that this RoI passed
   void addThresholdName( thresholds_type::value_type thr ) { m_thresholdNames.push_back( thr ); }
   /// Add the value of a threshold that this RoI passed
   void addThresholdValue( thr_value_type::value_type thr ) { m_thresholdValues.push_back( thr ); }

   /// Set the ET deposited in a 4x4 cluster
   void setET4x4( float value ) { m_ET4x4 = value; }
   /// Set the ET deposited in a 6x6 cluster
   void setET6x6( float value ) { m_ET6x6 = value; }
   /// Set the ET deposited in an 8x8 cluster
   void setET8x8( float value ) { m_ET8x8 = value; }

   /// Return the p<sub>T</sub> of the highest threshold passed
   virtual double pt()  const;
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

   uint32_t m_thrPattern; //!< threshold bit pattern

   thresholds_type m_thresholdNames;  //!< Names of the passed thresholds
   thr_value_type  m_thresholdValues; //!< Values of the passed thresholds

   float m_ET4x4; //!< ET sum in a 4x4 cluster
   float m_ET6x6; //!< ET sum in a 6x6 cluster
   float m_ET8x8; //!< ET sum in an 8x8 cluster

   mutable float m_highestThreshold; //!< return value for inherited pt() accessor

}; // class Jet_ROI

#endif

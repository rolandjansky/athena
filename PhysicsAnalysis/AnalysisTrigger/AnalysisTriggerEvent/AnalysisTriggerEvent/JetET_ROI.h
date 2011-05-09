// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JetET_ROI.h,v 1.4 2008-05-08 15:00:11 krasznaa Exp $
#ifndef ANALYSISTRIGGEREVENT_JETET_ROI_H
#define ANALYSISTRIGGEREVENT_JETET_ROI_H

// STL/System include(s):
#include <string>
#include <vector>
#include <inttypes.h>

/**
 *   @short Energy-Sum RoI class for analysis
 *
 *          Class giving information about LVL1 JetET threshold names and the RoI word.
 *
 *  @author Tadashi Maeno, Alan Watson, Attila Krasznahorkay
 * @version $Revision: 1.4 $
 *    @date $Date: 2008-05-08 15:00:11 $
 */
class JetET_ROI {

public:
   /// Constructor used when creating the object from RoIBResult data
   JetET_ROI( uint32_t roiWord, uint32_t thrPattern )
      : m_roiWord( roiWord ), m_thrPattern( thrPattern ), m_thresholds( 0 ) {}
   /// Default constructor (for persistency purposes)
   JetET_ROI()
      : m_roiWord( 0 ), m_thrPattern( 0 ), m_thresholds( 0 ) {}
   /// Destructor
   virtual ~JetET_ROI() {}

   /// Type storing the names of the passed thresholds
   typedef std::vector< std::string > thresholds_type;

   /// Return the RoI word produced by the L1Calo hardware
   uint32_t getROIWord()    const { return m_roiWord; }
   /// Return the bit-pattern describing the passed thresholds
   uint32_t getThrPattern() const { return m_thrPattern; }

   /// Return the names of the passed threshodlds
   const thresholds_type& getThresholds() const { return m_thresholds; }

   /// Add the name of a threshold that this RoI passed
   void addThreshold( thresholds_type::value_type thr ) { m_thresholds.push_back( thr ); }

private:
   uint32_t m_roiWord;           //!< 32bit encoded ROI word
   uint32_t m_thrPattern;        //!< threshold bit pattern
   thresholds_type m_thresholds; //!< Names of the passed thresholds

}; // class JetET_ROI

#endif // ANALYSISTRIGGEREVENT_JETET_ROI_H

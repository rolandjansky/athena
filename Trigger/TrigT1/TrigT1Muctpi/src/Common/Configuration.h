// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Configuration.h 515239 2012-08-28 11:39:45Z krasznaa $
#ifndef TRIGT1MUCTPI_CONFIGURATION_H
#define TRIGT1MUCTPI_CONFIGURATION_H

namespace LVL1MUCTPI {

   /**
    *  @short Header files holding configuration items for the MUCTPI
    *
    *         This header file groups all information describing
    *         the configuration of the MUCTPI. Eventually this should
    *         come out of a configuration data base.
    *
    * @author Thorsten Wengler
    *
    * $Revision: 515239 $
    * $Date: 2012-08-28 13:39:45 +0200 (Tue, 28 Aug 2012) $
    */
   class Configuration {

   public:
      Configuration()
         : m_doZeroSuppression( true ),
           m_threshold1Candidate( 1 ),
           m_threshold2Candidate( 1 ),
           m_suppressionMode( 0 ),
           m_maxCandPerPtvalue( 64 ),
           m_maxCandSendToRoib( 14 ),
           m_candBcidOffset( 0 )
      {}

      // Configuration items for the MIOCTS
      void setDoZeroSuppression( bool flag )  { m_doZeroSuppression = flag; }
      bool getDoZeroSuppression() const { return m_doZeroSuppression; }

      // Configuration Items for the MIROD Extractor
      // pt-thresholds for the first and second candidate
      void setThreshold1Candidate( unsigned int thr ) { m_threshold1Candidate = thr; }
      unsigned int getThreshold1Candidate() const { return m_threshold1Candidate; }
      void setThreshold2Candidate( unsigned int thr ) { m_threshold2Candidate = thr; }
      unsigned int getThreshold2Candidate() const { return m_threshold2Candidate; }

      // Extractor suppression mode flag
      void setSuppressionMode( int flag ) { m_suppressionMode = flag; }
      int  getSuppressionMode() const { return m_suppressionMode; }

      // Lvl2 processor configuration
      unsigned int getMaxCandPerPtvalue() const { return m_maxCandPerPtvalue; } //nonprogrammable
      void setMaxCandSendToRoib( unsigned int count ) { m_maxCandSendToRoib = count; }
      unsigned int getMaxCandSendToRoib() const { return m_maxCandSendToRoib; }
      void setCandBcidOffset( unsigned int count ) { m_candBcidOffset = count; }
      unsigned int getCandBcidOffset() const { return m_candBcidOffset; }

   private:
      bool m_doZeroSuppression;
      unsigned int m_threshold1Candidate;
      unsigned int m_threshold2Candidate;
      int m_suppressionMode;
      unsigned int m_maxCandPerPtvalue;
      unsigned int m_maxCandSendToRoib;
      unsigned int m_candBcidOffset;

   }; // class Configuration

} // namespace LVL1MUCTPI

#endif // TRIGT1MUCTPI_CONFIGURATION_H

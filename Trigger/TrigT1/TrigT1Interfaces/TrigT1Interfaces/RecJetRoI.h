// Dear emacs, this is -*- c++ -*-
// Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
/***************************************************************************
                         RecJetRoI.h  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/

#ifndef TRIGT1INTERFACES_RECJETTROI_H
#define TRIGT1INTERFACES_RECJETTROI_H

// STL include(s):
#include <map>

// Gaudi/Athena include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/iRecCoordRoI.h"

// Forward declaration(s):
namespace TrigConf {
   class TriggerThreshold;
   class L1Menu;
}

namespace LVL1 {

   class JEPRoIDecoder;

   /** This class defines the reconstructed em/tau hadron ROI. It is generated
       from the Slink output of TrigT1Calo.

       @author Edward Moyse
   */
   class RecJetRoI: public iRecCoordRoI {

   public:
      // constructor
      RecJetRoI() {}
      // constructor using Run 1+2 configuration
      RecJetRoI( unsigned int RoIWord,
                 const std::vector< TrigConf::TriggerThreshold* >* caloThresholds );
      /// constructor using Run 3 configuration
      RecJetRoI( unsigned int RoIWord,
                 const TrigConf::L1Menu * const l1menu );

      //copy constructor
      RecJetRoI( const RecJetRoI &obj );

      //assignment operator
      RecJetRoI& operator=(RecJetRoI const &obj);

      // destructor
      ~RecJetRoI();

      /** returns roi word*/
      virtual unsigned int roiWord() const;

      /** returns phi coord of ROI */
      virtual double phi() const;

      /** returns eta coord of ROI */
      virtual double eta() const;
      
      /** returns JEP crate number */
      virtual unsigned int crate() const;
      
      /** returns JEM number */
      virtual unsigned int module() const;
      
      /** returns RoI frame number  */
      virtual unsigned int frame() const;
      
      /** returns local coordinate within frame */
      virtual unsigned int localcoord() const;

      /** returns roi ET in larger cluster. Only valid for Run 2 data.
          Will return highest threshold passed if called for Run 1 data. */
      virtual unsigned int etLarge() const;

      /** returns roi ET in smaller cluster. Only valid for Run 2 data.
       *  Will return highest threshold passed if called for Run 1 data.
       **/
      virtual unsigned int etSmall() const;

      /** returns TRUE if threshold number <em>threshold_number</em> has been passed by this ROI. */
      bool passedThreshold( int thresholdNumber ) const;

      /** Same for forward jets. Deprecated in Run 2, will always return false. */
      bool passedFwdThreshold( int thresholdNumber ) const;

      /** @brief returns the CoordinateRange.
       * This is worked out from the RoIWord's hardware coords
       * (i.e. crate number, CPM number etc.) by the RoIDecoder class.
       **/
      CoordinateRange coord() const;

      /** returns a vector of thresholds passed. */
      std::vector<unsigned int> thresholdsPassed() const;

      /** @brief returns the value of the trigger threshold for the threshold passed.
       * Thresholds can vary by coordinate. Return value which matches coord of RecRoI
       **/
      unsigned int triggerThreshold( unsigned int thresh ) const;

      /** @brief returns the size of the window.
       * For Run 1 this should be 4, 6 or 8, for 4x4, 6x6, 8x8 jets.
       * For Run 2 this should be 0 or 1, denoting large or small
       **/
      unsigned int windowSize( unsigned int thresh ) const;
      
      /** returns bitmask of passed thresholds */
      unsigned int thresholdPattern() const;

      /** returns true if thresh is a valid threshold number */
      bool isValidThreshold( unsigned int thresh ) const;

      /** returns true if the RoI is a forward jet RoI. A forward jet is defined as one that
       * passes ANY forward jet threshold.
       * For legacy compatibility with Run 1 only. Deprecated in Run 2
       **/
      bool isForwardJet() const;

   private:
      /** this is the actual format of the data sent from
          the LVL1 hardware. */
      unsigned long int m_roiWord { 0 };
      
      /** Used for decoding RoI word */
      JEPRoIDecoder* m_decoder { 0 };

      /** Store the RoI format version */
      int m_version { 0 };

      /** this contains the coordinate range worked out from the RoIWord hardware coord
       * (i.e. crate number, CPM number etc.)
       **/
      CoordinateRange m_coordRange;

      /** Information on trigger thresholds passed by RoI */ 
      unsigned long int m_thresholdMask { 0 };

      std::map< int, unsigned int > m_triggerThresholdValue;
      std::map< int, unsigned int > m_windowSize;
      
   private:
      // Constructor for Run 1 RoI format
      void constructRun1( const std::vector< TrigConf::TriggerThreshold* >* caloThresholds );

      // Constructor for Run 2 RoI format
      void constructRun2( const std::vector< TrigConf::TriggerThreshold* >* caloThresholds );

      // Constructor for Run 3 Legacy RoI format from new config
      void constructRun3(const TrigConf::L1Menu * const l1menu);

   }; // class RecJetRoI

} // namespace LVL1

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( DataVector< LVL1::RecJetRoI >, 6258, 1 )
CLASS_DEF( LVL1::RecJetRoI, 6259, 1 )

#endif // TRIGT1INTERFACES_RECJETTROI_H

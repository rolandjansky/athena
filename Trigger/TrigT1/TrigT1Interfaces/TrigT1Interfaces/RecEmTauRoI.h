// Dear emacs, this is -*- c++ -*-
// $Id: RecEmTauRoI.h 782811 2016-11-07 17:20:40Z smh $
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/***************************************************************************
                         RecEmTauRoI.h  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/


#ifndef TRIGT1INTERFACES_RECEMTAUROI_H
#define TRIGT1INTERFACES_RECEMTAUROI_H

// STL include(s):
#include <map>
#include <vector>

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
   
   class CPRoIDecoder;

   /** This class defines the reconstructed em/tau hadron ROI. It is generated from the
       Slink output of TrigT1Calo

       @author Edward Moyse
   */
   class RecEmTauRoI : public iRecCoordRoI {

   public:
      // constructor
      RecEmTauRoI() {}
      /// constructor using Run 1+2 configuration
      RecEmTauRoI( unsigned int RoIWord,
                   const std::vector< TrigConf::TriggerThreshold* >* caloThresholds );
      /// constructor using Run 3 configuration
      RecEmTauRoI( unsigned int RoIWord,
                   const TrigConf::L1Menu * const l1menu );

      // copy constructor
      RecEmTauRoI(const RecEmTauRoI &obj);

      // assignment operator
      RecEmTauRoI& operator=(const RecEmTauRoI &obj);

      // destructor
      ~RecEmTauRoI();

      /** returns roi word*/
      virtual unsigned int roiWord() const;

      /** returns phi coord of ROI */
      virtual double phi() const;

      /** returns eta coord of ROI */
      virtual double eta() const;
      
      /** returns CP crate number */
      virtual unsigned int crate() const;
      
      /** returns CPM number */
      virtual unsigned int module() const;
      
      /** returns FPGA number */
      virtual unsigned int fpga() const;
      
      /** returns local coordinate within FPGA */
      virtual unsigned int localcoord() const;

      /** returns roi ET (Run 2 only) */
      virtual unsigned int et() const;

      /** returns roi isolation results (Run 2 only) */
      virtual unsigned int isolation() const;

      /** returns roi type (EM = TrigT1CaloDefs::EMRoIWordType,
         Tau = TrigT1CaloDefs::TauRoIWordType, Run 1 = TrigT1CaloDefs::CpRoIWordType) */
      virtual TrigT1CaloDefs::RoIType roiType() const;

      /** returns TRUE if threshold number <em>threshold_number</em> has been
          passed by this ROI. */
      bool passedThreshold( int threshold_number ) const;

      /** returns the CoordinateRange. This is worked out from the RoIWord's hardware coords
          (i.e. crate number, CPM number etc.) by the RoIDecoder class.  */
      CoordinateRange coord() const;

      /** returns a vector of thresholds passed. */
      std::vector< unsigned int >* thresholdsPassed() const;
      
      /** returns bitmask of passed thresholds */
      unsigned int thresholdPattern() const;

      /** returns the value of the trigger threshold for the threshold passed.
          The trigger threshold is also sometimes called the cluster threshold.
          \todo Thresholds can vary by coordinate. Return value which matches coord of RecRoI*/
      unsigned int triggerThreshold( unsigned int thresh ) const;

      /** returns the value of the em isolation ring for the threshold passed. (Run 1)*/
      unsigned int emIsolation( unsigned int thresh ) const;

      /** returns the value of the hadronic layer isolation ring for the threshold passed. (Run 1)*/
      unsigned int hadIsolation( unsigned int thresh ) const ;

      /** returns the value of the hadronic layer core isolation for the threshold passed.(Run 1) */
      unsigned int hadCoreIsolation( unsigned int thresh ) const;

      /** returns the isolation bits required for the threshold passed (Run 2) */
      unsigned int isolationMask( unsigned int thresh ) const;

      /** returns true if thresh is a valid threshold number */
      bool isValidThreshold( unsigned int thresh ) const;

      /** returns the type of the threshold, which is either EMAlg or TauAlg.
          If there is a problem, ClustAlgError is returned */
      TrigT1CaloDefs::ClusterAlgorithm thresholdType( unsigned int thresh ) const;

   private:
      /** this is the actual format of the data sent from
          the LVL1 hardware. See  ATL-DAQ-99-015 for
          further explanation. */
      unsigned long int m_roiWord { 0 };
      
      /** Used for decoding RoI word */
      CPRoIDecoder* m_decoder {nullptr};

      /// Stored properties of the RoI:
      TrigT1CaloDefs::RoIType m_type {TrigT1CaloDefs::RoIWordTypeError};
      
      /** this contains the coordinate range worked out from the RoIWord hardware coord
          (i.e. crate number, CPM number etc.)*/
      CoordinateRange m_coordRange;
      
      unsigned long int m_thresholdMask { 0 };
      std::map< int, unsigned int > m_triggerThresholdValue;
      std::map< int, unsigned int > m_isolationMask;
      std::map< int, unsigned int > m_emIsolation;
      std::map< int, unsigned int > m_hadIsolation;
      std::map< int, unsigned int > m_hadCoreIsolation;
      std::map< int, TrigT1CaloDefs::ClusterAlgorithm > m_thresholdType;
      

   private:
      // constructors for Run 1 and Run 2 data
      void constructRun1( const std::vector< TrigConf::TriggerThreshold* >* caloThresholds );
      
      void constructRun2( const std::vector< TrigConf::TriggerThreshold* >* caloThresholds );

      void constructRun3( const TrigConf::L1Menu * const l1menu );

   }; // class RecEmTauRoI

} // namespace LVL1

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( DataVector< LVL1::RecEmTauRoI >, 6256, 1 )
CLASS_DEF( LVL1::RecEmTauRoI, 6257, 1 )

#endif // TRIGT1INTERFACES_RECEMTAUROI_H

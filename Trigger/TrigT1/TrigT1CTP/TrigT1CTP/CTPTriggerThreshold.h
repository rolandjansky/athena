/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_CTPTRIGGERTHRESHOLD_H
#define TRIGT1CTP_CTPTRIGGERTHRESHOLD_H

// Forward include(s):
namespace TrigConf {
  class TriggerThreshold;
}

namespace LVL1CTP {

   /**
    *
    *   @short Helper class holding trigger threshold multiplicity
    *
    *          For each TrigConf::TriggerThreshold object, I create one of these objects.
    *          I use it to determine the multiplicity for the referenced threshold in the given event.
    *          For convinience the object knows which TriggerThreshold object it belongs to. However,
    *          as most of the time one wants to find the CTPTriggerThreshold object belonging
    *          to a given TriggerThreshold (when making the CTP decision), the DecisionThresholdMap
    *          can be used to do that kind of association.
    *
    *     @see TrigConf::TriggerThreshold
    *     @see ThresholdMap
    *
    *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
    *
    * @version \$Id: CTPTriggerThreshold.h,v 1.3 2007-06-15 16:15:59 efeld Exp $
    *
    */

   class CTPTriggerThreshold {

   public:
      /// constructor setting the TrigConf::TriggerThreshold
      CTPTriggerThreshold( const TrigConf::TriggerThreshold* parent );
      /// default destructor
      ~CTPTriggerThreshold();

      /// Function returning a pointer to the referenced TriggerThreshold object
      const TrigConf::TriggerThreshold* threshold() const;

      /// Get multiplicity of the threshold
      int value() const;
      /// Set the multiplicity of the threshold
      void setValue( int value );

      /// Get the start position of the threshold 
      unsigned int startBit() const;
      /// Set the start position of the threshold 
      void setStartBit( unsigned int start );

      /// Get the end position of the threshold 
      unsigned int endBit() const;
      /// Set the end position of the threshold 
      void setEndBit( unsigned int end );
    
    
      //
      //    /// Get the start position of the threshold on the PIT bus
      //    int pitStart() const;
      //    /// Set the start position of the threshold on the PIT bus
      //    void setPitStart( int pit_start );
      //    
      //    /// Get the end position of the threshold on the PIT bus
      //    int pitEnd() const;
      //    /// Set the end position of the threshold on the PIT bus
      //    void setPitEnd( int pit_end );

   private:
      const TrigConf::TriggerThreshold* m_parent {nullptr};   //!< associated TrigConf::TriggerThreshold
      int m_value {0};                                        //!< multiplicity of threshold
      unsigned int m_start_bit {0};                           //!< start position of threshold 
      unsigned int m_end_bit {0};                             //!< end position of threshold 
      // int m_pit_start;                              //!< start position of threshold on PIT bus
      // int m_pit_end;                                //!< end position of threshold on PIT bus

   }; // class CTPTriggerThreshold

} // namespace LVL1CTP

#endif // TRIGT1CTP_CTPTRIGGERTHRESHOLD_H

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_CTPTRIGGERTHRESHOLD_H
#define TRIGT1CTP_CTPTRIGGERTHRESHOLD_H

#include <string>

namespace LVL1CTP {

   /**
    *   @short Helper class holding trigger threshold multiplicity
    */

   class CTPTriggerThreshold {

   public:

      CTPTriggerThreshold(const std::string & name);
      ~CTPTriggerThreshold();

      const std::string & name() const
      { return m_name; }

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
    
   private:
      std::string m_name {""};
      int m_value {-1};                                        //!< multiplicity of threshold
      unsigned int m_start_bit {0};                           //!< start position of threshold 
      unsigned int m_end_bit {0};                             //!< end position of threshold 

   };

} 

#endif

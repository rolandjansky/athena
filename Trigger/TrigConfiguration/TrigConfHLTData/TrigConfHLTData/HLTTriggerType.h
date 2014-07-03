/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTTriggerType
#define TrigConf_HLTTriggerType

#include <iosfwd>
#include <string>

namespace TrigConf {

   class HLTTriggerType;
   std::ostream & operator<<(std::ostream &, const HLTTriggerType &);
   
   /**@brief HLT trigger type configuration information
    * 
    * A trigger type is associated with a trigger chain and might be
    * used to raise a bit flag in the event header in case the chain
    * has fired
    */
   class HLTTriggerType {

   public:

      /**@brief constructor with configuration data
       *
       * @param bit bit index in the event header
       */
      HLTTriggerType(int bit = 0);

      /**@brief destructor*/
      ~HLTTriggerType();

      // accessors
      int  bit() const { return m_bit; } ///< accessor to the trigger type bit
      void set_bit(int bit) { m_bit = bit; }   ///< setter of the trigger type bit

      /**@brief print method*/
      void print(const std::string& indent="", unsigned int detail=1) const;

   private:

      int    m_bit; ///< the trigger type bit

      friend std::ostream & operator<<(std::ostream &, const TrigConf::HLTTriggerType &);
   };

}

#endif

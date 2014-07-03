/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTStreamTag
#define TrigConf_HLTStreamTag

#include "TrigConfL1Data/TrigConfData.h"

#include <iosfwd>
#include <fstream>
#include <string>

namespace TrigConf {

   class HLTStreamTag;
   std::ostream & operator<<(std::ostream &, const TrigConf::HLTStreamTag &);

   /**@brief HLT stream configuration information
    * 
    * Defines streams that are associated with the chains
    */
   class HLTStreamTag : public TrigConfData {

   public:
    
      /**@brief default constructor*/
      HLTStreamTag();

      /**@brief constructor with configuration data
       *
       * @param stream stream name
       * @param type stream type
       * @param obeyLB flag that the stream obey the boundaries of the lumi-blocks
       * @param prescale stream prescale value (in conjunction with a chain)
       */
      HLTStreamTag(const std::string& stream, const std::string& type, bool obeyLB, float prescale);

      /**@brief destructor*/
      ~HLTStreamTag( void ){};

      bool operator==(const HLTStreamTag &);

      // getters
      const std::string& stream() const { return m_stream; }   ///< accessor to the stream name
      const std::string& type() const { return m_type; }     ///< accessor to the stream type
      bool               obeyLB() const { return m_obeyLB; }   ///< accessor to the obey LB flag
      float              prescale() const { return m_prescale; } ///< accessor to the stream prescale value

      // setters
      void set_stream  ( const std::string& stream  ) { m_stream = stream; setName(m_type+"."+m_stream); }
      void set_type    ( const std::string& type ) { m_type = type; setName(m_type+"."+m_stream); }
      void set_obeyLB  ( bool val ) { m_obeyLB = val; }
      void set_prescale( float val ) { m_prescale = val; }

      // print the chain
      void print(const std::string& indent="", unsigned int detail=1) const;

      void writeXML(std::ofstream & xmlfile) const;

   private:

      std::string m_stream;   ///< the stream name          
      std::string m_type;     ///< the stream type          
      bool        m_obeyLB;   ///< the obey LB flag, if @c true the stream obeys the boundaries of the lumi-blocks
      float       m_prescale; ///< the stream prescale value (in conjunction with a chain)

      friend std::ostream & operator<<(std::ostream &, const TrigConf::HLTStreamTag &);
   };

}

#endif

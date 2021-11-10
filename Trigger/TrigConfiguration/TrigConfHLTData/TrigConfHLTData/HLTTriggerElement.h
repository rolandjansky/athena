/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTTriggerElement
#define TrigConf_HLTTriggerElement

#include "TrigConfL1Data/TrigConfData.h"

#include <iosfwd>
#include <fstream>
#include <string>
#include <map>

namespace TrigConf {

   class HLTTriggerElement;
   std::ostream & operator<<(std::ostream &, const HLTTriggerElement &);

   /**@brief HLT trigger element configuration information
    * 
    * A trigger element is an artificial trigger object (a refined
    * electron, a Z-boson candidate etc.) More detail on the HTL
    * Steering TWIKI.
    */
   class HLTTriggerElement : public TrigConfData {

   public:

      /**@brief default constructor*/
      HLTTriggerElement();

      /**@brief constructor with configuration data
       *
       * @param i ID of the trigger element
       *
       * @param n name of the trigger element
       */
      HLTTriggerElement( const std::string& name );

      /**@brief constructor with configuration data
       *
       * @param i ID of the trigger element
       *
       * @param n name of the trigger element
       */
      HLTTriggerElement( unsigned int i, const std::string& name );

      /**@brief constructor with configuration data
       *
       * @param p pair (ID,name) of the trigger element
       */
      HLTTriggerElement( const std::pair<unsigned int, std::string>& p);

      /**@brief destructor*/
      virtual ~HLTTriggerElement();

      // getters
      unsigned int hashId() const { return m_hashId; }
      unsigned int id() const { return m_hashId; }


      // setters
      void setHashId(unsigned int hashid) { m_hashId = hashid; }


      /**@brief print method*/
      virtual void print(const std::string& indent="", unsigned int detail=1) const;
      virtual void writeXML(std::ofstream & xmlfile);

      void setL2(bool on=true) { set(); if(on) m_level |= 0x1; else m_level &= 0x2; }
      void setEF(bool on=true) { set(); if(on) m_level |= 0x2; else m_level &= 0x1; }
      void reset() { m_level = 0x4; }
      bool inL2() const { return (m_level&0x1) != 0; }
      bool inEF() const { return (m_level&0x2) != 0; }
      bool isSet() const { return (m_level&0x4) == 0; }

   private:

      unsigned int m_hashId;   ///< trigger element id  

      unsigned int m_level;      //!< level where TE is produced for a given configuration (to be determined in light of the menu, this
      //!< is not a property of the TE per se.) In rare cases a sequence could be running explicitely in both levels
      void set() { m_level &= 0x3; }

      friend std::ostream & operator<<(std::ostream &, const TrigConf::HLTTriggerElement &);

   public:
      /**@brief find name from ID in the map m_IdToLabel
       * @param id trigger element ID
       * @param label reference to be set with the trigger element name that has been found
       * @returns @c true if ID has been found
       */
      static bool getLabel(unsigned int id, std::string & label);
   };

}

#endif

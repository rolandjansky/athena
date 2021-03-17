/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTSequence
#define TrigConf_HLTSequence

#include "TrigConfL1Data/TrigConfData.h"

#include <iosfwd>
#include <fstream>
#include <string>
#include <vector>

namespace TrigConf {

   class HLTSequence;
   class HLTTriggerElement;
   class DiffStruct;
   std::ostream & operator<<(std::ostream &, const HLTSequence &);

   /**@brief HLT sequence configuration information
    *
    * A sequence in the HLT defines how the algorithms are linked to
    * the trigger elements. The trigger elements are logical trigger
    * objects which indicate if certain trigger patterns were found.
    */
   class HLTSequence : public TrigConfData {
   public:

      /**@brief default constructor*/
      HLTSequence();

      /**@brief constructor with configuration data
       *
       * @param inputTEs list of input trigger elements for this sequence
       * @param outputTEs output trigger element for this sequence (list must have lenght 1)
       * @param algorithms list of algorithms that run in this sequence
       */
      HLTSequence( const std::vector<HLTTriggerElement*>& inputTEs,
                   HLTTriggerElement* outputTE,
                   const std::vector<std::string>& algorithms);

      /**@brief destructor*/
      ~HLTSequence();

      // accessors
      std::vector<HLTTriggerElement*>& inputTEs() { return m_inputTEs;   } //!< accessor to the list of input trigger elements
      HLTTriggerElement*               outputTE() { return m_outputTE;  } //!< accessor to the list of output trigger elements (just 1)
      std::vector<std::string>&        algorithms() { return m_algorithms; } //!< accessor to the list of algorithms
      HLTTriggerElement*               topoStartTE() { return m_topoStartTE;} //!< accessor to the topological start TE

      const std::vector<HLTTriggerElement*>& inputTEs() const { return m_inputTEs;   } //!< const accessor to the list of input trigger elements
      const HLTTriggerElement*               outputTE()  const { return m_outputTE;   } //!< const accessor to the list of output trigger elements (just 1)
      const std::vector<std::string>&        algorithms() const { return m_algorithms; } //!< const accessor to the list of algorithms
      const HLTTriggerElement*               topoStartTE() const { return m_topoStartTE; } //!< const accessor to the topological start TE
      unsigned int                           hashId() const;

      void set_inputTEs   ( std::vector<HLTTriggerElement*>& inputTEs ) { m_inputTEs = inputTEs;   } //!< setter of the list of input trigger elements
      void set_outputTE   ( HLTTriggerElement* outputTE );
      void set_algorithms ( std::vector<std::string>& algorithms ) { m_algorithms = algorithms; } //!< setter of the list of algorithms
      void set_topoStartTE( HLTTriggerElement* te ) { m_topoStartTE = te; }         //!< setter of the topological starting TE

      /**@brief print method*/
      void print(const std::string& indent="", unsigned int detail=1) const;
      void writeXML(std::ofstream & xmlfile);

      std::string concise() const;

      void setL2(bool on=true) { set(); if(on) m_level |= 0x1; else m_level &= 0x6; }
      void setEF(bool on=true) { set(); if(on) m_level |= 0x2; else m_level &= 0x5; }
      void setHLT(bool on=true) { set(); if(on) m_level = 0x4; else m_level &= 0x3; }

      void reset() { m_level = 0x8; }
      bool inL2() const { return (m_level&0x1) != 0; }
      bool inEF() const { return (m_level&0x2) != 0; }
      bool inHLT() const { return (m_level&0x4) != 0; }
      bool isSet() const { return (m_level&0x8) == 0; }

      bool operator<(const HLTSequence& s) const {
         return name() < s.name();
      }

      DiffStruct* compareTo(const HLTSequence* o) const;

      // python
      std::string __str__() const;

   private:

      std::vector<HLTTriggerElement*> m_inputTEs;   //!< list of input trigger elements
      HLTTriggerElement*              m_outputTE { nullptr };   //!< list of output trigger elements (just 1)
      std::vector<std::string>        m_algorithms; //!< list of algorithms
      HLTTriggerElement*              m_topoStartTE { nullptr }; //!< only for topolical algorithms: specify output TE from previous topological Algo, for combinations

      // bit 0 - L2 / bit 1 - EF / bit 2 - HLT / bit 3 - unset
      unsigned int            m_level { 0 };      //!< level where sequence is running for a given configuration (to be determined in light of the menu, this
                                                    //!< is not a property of the chain per se.) In rare cases a sequence could be running explicitely in both levels
      void set() { m_level &= 0x7; }
                                             
      friend std::ostream & operator<<(std::ostream &, const HLTSequence &);

   };

}

#endif

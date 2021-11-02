/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_HLTFrame
#define TrigConf_HLTFrame

#include <fstream>
#include <map>
#include <string>
#include <iosfwd>
#include <vector>

#include "TrigConfL1Data/TrigConfData.h"

#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTSequenceList.h"
#include "TrigConfHLTData/HLTPrescaleSetCollection.h"

namespace TrigConf {

   class HLTFrame;
   class HLTChain;
   class HLTPrescaleSet;
   class DiffStruct;

   std::ostream & operator<<(std::ostream &, const HLTFrame &);

   /**@brief The HLT trigger menu, 
    * 
    * owns HLTChainList, HLTSequenceList, and PrescaleSet
    */
   class HLTFrame : public TrigConfData {
   public:
      /**@brief default constructor*/
      HLTFrame();

      /**@brief destructor*/
      ~HLTFrame();

      /**@brief clear chains, sequences and prescales*/
      void clear();

      HLTChainList&                   theHLTChainList() { return m_HLTChainList; }    ///< accessor to the list of HLT chains
      HLTSequenceList&                theHLTSequenceList() { return m_HLTSequenceList; } ///< accessor to the list of HLT sequences
      HLTPrescaleSetCollection&       thePrescaleSetCollection() { return m_hltPrescaleSets; }
      HLTPrescaleSet*                 thePrescaleSet(unsigned int lbn=0) { return m_hltPrescaleSets.thePrescaleSet(lbn); }

      const HLTChainList&             getHLTChainList() const { return m_HLTChainList; }    ///< const accessor to the list of HLT chains
      const HLTSequenceList&          getHLTSequenceList() const { return m_HLTSequenceList; } ///< const accessor to the list of HLT sequences
      const HLTPrescaleSetCollection& getPrescaleSetCollection() const { return m_hltPrescaleSets; }
      const HLTPrescaleSet*           getPrescaleSet(unsigned int lbn) const { return m_hltPrescaleSets.prescaleSet(lbn); }
      bool                            mergedHLT() const { return m_setMergedHLT; }
      const std::string&              configSource() const { return m_ConfigSrc; }


      /**@brief pass the HLT merged status */
      void setMergedHLT(bool merged=true) { m_setMergedHLT = merged; }
      void setConfigSource(const std::string& src) { m_ConfigSrc = src; }


      /**@brief print method*/
      void print(const std::string& indent="", unsigned int detail=1) const;
  
      void writeXML(const std::string& filename) const;
      void writeXML(std::ofstream & xmlfile) const;

      bool equals(const HLTFrame* other, const std::string& filename) const;
      DiffStruct* compareTo(const HLTFrame* o) const;

   private:

      HLTChainList              m_HLTChainList;    ///< list of HLT trigger chains (owned)
      HLTSequenceList           m_HLTSequenceList; ///< list of HLT trigger sequences (owned)
      HLTPrescaleSetCollection  m_hltPrescaleSets;  ///< the hlt prescale set (owned)
      bool                      m_setMergedHLT {false};
      std::string               m_ConfigSrc {""};

      /**@brief TE labels <-> id conversion map*/
      std::map<std::string, unsigned int> m_str2idMap; 

      friend std::ostream & operator<<(std::ostream &, const TrigConf::HLTFrame &);
   };
 
}

#endif

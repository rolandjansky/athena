/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @class  : ConfigThreshold, ConfigJetEThreshold,
 *           HLTRoI, MuonRoI, EMTauRoI, JetEnergyRoI
 *
 * @brief  LVL1 RoI and Item data classes which are filled by the Lvl1ResultAccessTool.
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>
 **********************************************************************************/

#ifndef TRIGDECISIONMAKER_LVL1ITEMSANDROIS_H
#define TRIGDECISIONMAKER_LVL1ITEMSANDROIS_H

#include "TrigT1Result/MuCTPIRoI.h"
#include "TrigT1Result/EMTauRoI.h"
#include "TrigT1Result/JetEnergyRoI.h"

#include <vector>
#include <string>


namespace HLT {

   enum JetRoIType { Unknown, JetRoI, ForwardJetRoI, JetEtRoI, TotalEtRoI, MissingEtRoI, METSignificanceRoI, MissingOrTotalEtRoI }; //!< JetEnergy RoI types

   /** @struct
       Generic structure representing one LVL1 threshold which is used in all LVL1 RoIs
   */
   struct ConfigThreshold {
      ConfigThreshold(const std::string& n = "", unsigned int h=0, bool a = false, uint32_t m = 0)
         : name(n), hashId(h), activeHLT(a), mask(m) { }
      std::string name;    //!< threshold name, e.g. EM25i
      unsigned int hashId; //!< hash number of name, to speed up internal usage
      bool activeHLT;      //!< is this LVL1 threshold used in HLT ?
      uint32_t mask;       //!< bit position of this LVL1 threshold in the RoI word
   };

   /** @struct ConfigJetEThreshold
       modifed ConfigThreshold to incorporate the JetEnergy type
   */
   struct ConfigJetEThreshold : public ConfigThreshold {
      ConfigJetEThreshold(const std::string& n = "", unsigned int h=0, bool a = false, uint32_t m = 0, JetRoIType t = Unknown)
         : ConfigThreshold(n, h, a, m), type(t) { }
      JetRoIType type; //!< JetEnergy type, see JetRoIType enumeration
   };

   /** @class HLTRoI
       Generic (templated) class for the RoI classes,
       it contains a vector of the configured ConfigThresholds.
   */
   template<class R> class HLTRoI {
   public :
      HLTRoI(R lvl1RoI) : m_lvl1RoI(lvl1RoI), m_thresholdMask(0) { } //!< constructor from templated LVL1 RoI object

      const R& lvl1RoI() const { return m_lvl1RoI; } //!< const reference getter for underlying LVL1 RoI object
      const std::vector<const ConfigThreshold*>& thresholds() const { return m_thresholds; } //!< const reference getter for configured thresholds
      std::vector<const ConfigThreshold*>::const_iterator begin() const { return m_thresholds.begin(); } //!< begin() method - for thresholds
      std::vector<const ConfigThreshold*>::const_iterator end()   const { return m_thresholds.end(); }   //!< end() method   - for thresholds
      unsigned int size() const  { return m_thresholds.size(); }                                         //!< size() method - for thresholds
      unsigned int thresholdMask() const { return m_thresholdMask; }

   protected:
      void addThreshold(const ConfigThreshold* t) { m_thresholds.push_back(t); } //!< add threshold to this RoI object
      void setThresholdMask(unsigned int mask) { m_thresholdMask = mask; } // Update threshold mask word 

      R m_lvl1RoI; //!< underlying LVL1 RoI object
      std::vector<const ConfigThreshold*> m_thresholds; //!< vector holding all configured thresholds
      unsigned int m_thresholdMask;
   };


   /** @class MuonRoI
       Concrete MuonRoI class (from templated HLRoI class).
   */
   class MuonRoI : public HLTRoI<ROIB::MuCTPIRoI> {
   public :
      MuonRoI(ROIB::MuCTPIRoI lvl1RoI) : HLTRoI<ROIB::MuCTPIRoI>(lvl1RoI) {
         m_thresholdMask = 0;
      } //!< constructor from MuCTPIRoI object
      friend class Lvl1ResultAccessTool; //!< Lvl1ResultAccessTool is a friend to this class -> cann call addThreshold(..)
   };

   /** @class EMTauRoI
       Concrete EMTau class (from templated HLRoI class).
   */
   class EMTauRoI : public HLTRoI<ROIB::EMTauRoI> {
   public :
      EMTauRoI(ROIB::EMTauRoI lvl1RoI) : HLTRoI<ROIB::EMTauRoI>(lvl1RoI) {
         if ( m_lvl1RoI.roIType() == 0 ) m_thresholdMask = (m_lvl1RoI.roIWord() & 0xffff);  // Valid for Run 1 RoIs
         else                            m_thresholdMask = 0;
      } //!< constructor from EMTauRoI object
      friend class Lvl1ResultAccessTool; //!< Lvl1ResultAccessTool is a friend to this class -> cann call addThreshold(..)
   };

   /** @class JetEnergyRoI
       Concrete JetEnergy class (from templated HLRoI class).
   */
   class JetEnergyRoI : public HLTRoI<ROIB::JetEnergyRoI> {
   public :
      JetEnergyRoI(ROIB::JetEnergyRoI lvl1RoI)
         : HLTRoI<ROIB::JetEnergyRoI>(lvl1RoI), m_type(Unknown), m_word0(0), m_word1(0), m_word2(0) {
	
         if ( m_lvl1RoI.jetRoIVersion() == 1 ) m_thresholdMask = (m_lvl1RoI.roIWord() & 0xfff);
         else                                  m_thresholdMask = 0;
	
      }  //!< constructor from JetEnergyRoI object

      friend class Lvl1ResultAccessTool; //!< Lvl1ResultAccessTool is a friend to this class -> cann call addThreshold(..)

      JetRoIType type()  const { return m_type; }  //!< extra getter: JetRoI type, see JetRoIType enumeratio
      uint32_t   word0() const { return m_word0; } //!< extra getter: extra RoI word 1
      uint32_t   word1() const { return m_word1; } //!< extra getter: extra RoI word 1
      uint32_t   word2() const { return m_word2; } //!< extra getter: extra RoI word 2

   private:

      bool setType(JetRoIType type) { 
         if ( m_type == Unknown ) {
            m_type = type; 
            return true; 
         }
         if ( m_type == MissingOrTotalEtRoI && ( type == MissingEtRoI || type == TotalEtRoI || type == METSignificanceRoI ) )
            return true;
         if ( m_type != type) return false;
         return true; 
      } //!< setter for JetRoI type

      bool setExtraWords(uint32_t word0, uint32_t word1, uint32_t word2) {  //!< setter for extra RoI words 1 & 2
         m_word0= word0; m_word1= word1; m_word2 = word2;
         return true; 
      } 
      JetRoIType m_type; //!< additional member: JetRoI type, see JetRoIType enumeration
      uint32_t m_word0;  //!< additional member: 0th word
      uint32_t m_word1;  //!< additional member: extra RoI word 1
      uint32_t m_word2;  //!< additional member: extra RoI word 2

   };

} // end namespace


#endif

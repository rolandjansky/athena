/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1THRESHOLDBASE_H
#define TRIGCONFDATA_L1THRESHOLDBASE_H

#include "TrigConfData/DataStructure.h"

#include <map>
#include <vector>

namespace TrigConf {

   /** class to implement a L1 threshold cut that varies with eta
    *
    * A vector of RangeValue objects describes this eta
    * dependence. Each entry has an eta-range and a corresponding cut
    * value. Overlapping ranges are resolved by looking at the
    * priority (smaller wins)
    *
    * Use case: for instance the L1 EM thresholds change as a function of eta.
    */
   template<class T>
   class ValueWithEtaDependence {
      struct RangeValue {
         T value {}; // the value (energy, set of isolation variables) 
         int etaMin { -49 }; // range boundaries, always inclusive
         int etaMax {  49 }; // etaMin is always smaller than etaMax, e.g. on the negative side: etaMin=-49, etaMax=-31
         unsigned int priority {0}; // higher number has higher priority when resolving overlapping regions 
         bool symmetric { true }; // if true, also applies to the opposity side ( requires etaMax to be positive )
      };
   public:
      typedef typename std::vector<RangeValue>::const_iterator const_iterator;
      ValueWithEtaDependence(const std::string name) : m_name(name) {};
      const std::string & name() const { return m_name; }
      bool empty() const;
      size_t size() const;
      const T & at(int eta) const;
      const_iterator begin() const noexcept;
      const_iterator end() const noexcept;
      void addRangeValue(const T & value, int etaMin, int etaMax, unsigned int priority, bool symmetric = true);
   private:
      const std::string m_name {""};
      std::vector<RangeValue> m_rangeValues{};
   };



   /** @brief L1 extra information for certain threshold types */
   class L1ThrExtraInfoBase : public DataStructure {
   public:

      static std::unique_ptr<L1ThrExtraInfoBase> createExtraInfo(const std::string & thrTypeName, const ptree & data);

      /** Constructors */
      L1ThrExtraInfoBase() = delete;
      L1ThrExtraInfoBase(const L1ThrExtraInfoBase &) = delete;
      L1ThrExtraInfoBase& operator=(const L1ThrExtraInfoBase&) = delete;
      L1ThrExtraInfoBase(L1ThrExtraInfoBase&&) = default;
      L1ThrExtraInfoBase& operator=(L1ThrExtraInfoBase&&) = default;

      /** Constructor initialized with configuration data 
       * @param data The data containing the L1 menu 
       */
      L1ThrExtraInfoBase(const std::string & thrTypeName, const ptree & data);

      /** Destructor */
      virtual ~L1ThrExtraInfoBase();

      virtual std::string className() const;

      const std::string & thresholdTypeName() const;

      bool hasExtraInfo() const;

   protected:

      std::map<std::string, DataStructure> m_extraInfo{};

   private:
      /** Update the internal members */
      virtual void update();
   };



   /** 
    * @brief Standard L1 threshold configuration
    *
    * Provides access to the selection parameters
    */
   class L1Threshold : public DataStructure {
   public:
      
      static std::shared_ptr<L1Threshold> createThreshold( const std::string & name, const std::string & type, 
                                                           std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data );

      /** Constructor */
      L1Threshold() = default;
      
      /** Constructor initialized with configuration data 
       * @param name threshold name
       * @param type threshold type name
       * @param extraInfo The information that is specific for this threshold type 
       * @param data The data containing the L1 threshold 
       */
      L1Threshold( const std::string & name, const std::string & type,
                   std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data);

      /** Destructor */
      virtual ~L1Threshold() = default;

      virtual std::string className() const
      { return "L1Threshold"; }

      /** Accessor to the threshold type */
      const std::string & type() const
      { return m_type; }

      /** Accessor to the mapping number
       * The mapping is unique within a type
       */
      unsigned int mapping() const
      { return m_mapping; }

      /** Accessor to the threshold value for eta-dependent threholds
       * @param eta the eta value should be given for potentially eta-dependent thresholds
       */
      unsigned int thrValue(int eta = 0) const;

      virtual unsigned int thrValueCounts(int eta = 0) const;

      /** access to the list of ThresholdValues */
      const ValueWithEtaDependence<unsigned int> & thrValues() const;

   protected:

      /** Update the internal data after modification of the data object */
      virtual void update();

      std::string m_input{""};
      unsigned int m_thrValue {0}; ///< threshold value in GeV
      std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo;

      ValueWithEtaDependence<unsigned int> m_etaDepThrValue{""};
      
   private:
      std::string m_type{""}; ///< threshold type
      unsigned int m_mapping{0}; ///< unique identifier amongst thresholds of the same type
   };


   /******************************************
    * Isolation for legacy L1Calo thresholds
    ******************************************/
   class IsolationLegacy {
   public:
      IsolationLegacy() = default;
      IsolationLegacy( const boost::property_tree::ptree & );
      bool isDefined()   const { return m_isDefined; } 
      int isobit()       const { return m_isobit; } 
      int offset()       const { return m_offset; }
      int slope()        const { return m_slope; }
      int mincut()       const { return m_mincut; }
      int upperlimit()   const { return m_upperlimit; }
      int etamin()       const { return m_etamin; }
      int etamax()       const { return m_etamax; }
      int priority()     const { return m_priority; }
   private:
      bool m_isDefined { false };
      int m_isobit { 0 };
      int m_offset { 0 };
      int m_slope { 0 };
      int m_mincut { 0 };
      int m_upperlimit { 0 };
      int m_etamin { 0 };
      int m_etamax { 0 };
      int m_priority { 0 };
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::IsolationLegacy & iso);


   /**************************************
    * Isolation for new L1Calo thresholds
    **************************************/
   class Isolation {
   public:
      enum class WP { NONE = 0, LOOSE = 1, MEDIUM = 2, TIGHT = 3 };
      Isolation() = default;
      Isolation( const boost::property_tree::ptree & );
      bool isDefined() const { return m_isDefined; } 
      int reta()       const { return m_reta; } 
      int wstot()      const { return m_wstot; }
      int had()        const { return m_had; }
   private:
      bool m_isDefined { false };
      int m_reta { 0 };
      int m_wstot { 0 };
      int m_had { 0 };
   };
   std::ostream & operator<<(std::ostream & os, const TrigConf::Isolation & iso);

}

#include "TrigConfData/L1ThresholdBase.icc"

#endif

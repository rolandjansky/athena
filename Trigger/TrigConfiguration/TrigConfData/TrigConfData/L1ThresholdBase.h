/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGCONFDATA_L1THRESHOLDBASE_H
#define TRIGCONFDATA_L1THRESHOLDBASE_H

#include "TrigConfData/DataStructure.h"

#include <map>
#include <vector>

namespace TrigConf {

   /** @brief helper funtion to translate energies into counts
    * @param energyMeV [in] energy value in MeV
    * @param energyResolutionMeV [in] energy resolution in MeV (energy that corresponds to one count
    * @return energy counts
    *
    * The funtion throws a runtime error if the energy value is not a multiple of the resolution
    */
   unsigned int energyInCounts(unsigned int energyMeV, unsigned int energyResolutionMeV);

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
      class RangeValue {
      public:
         RangeValue(const T & value, int etaMin, int etaMax, unsigned int priority, bool symmetric);
         const T & value() const { return m_value; }
         int etaMin() const { return m_etaMin; };
         int etaMax() const { return m_etaMax; };
         unsigned int priority() const { return m_priority; }
         bool symmetric() const { return m_symmetric; }
      private:
         T m_value {}; // the value (energy, set of isolation variables) 
         int m_etaMin { -49 }; // range boundaries, always inclusive
         int m_etaMax {  49 }; // etaMin is always smaller than etaMax, e.g. on the negative side: etaMin=-49, etaMax=-31
         unsigned int m_priority {0}; // higher number has higher priority when resolving overlapping regions 
         bool m_symmetric { true }; // if true, also applies to the opposity side ( requires etaMax to be positive )
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

      // Constructors
      L1ThrExtraInfoBase() = delete;
      L1ThrExtraInfoBase(const L1ThrExtraInfoBase &) = delete;
      L1ThrExtraInfoBase& operator=(const L1ThrExtraInfoBase&) = delete;
      L1ThrExtraInfoBase(L1ThrExtraInfoBase&&) = default;
      L1ThrExtraInfoBase& operator=(L1ThrExtraInfoBase&&) = default;
      virtual ~L1ThrExtraInfoBase() = default;

      // Constructor initialized with configuration data 
      // @param data The data containing the L1 menu 
      //
      L1ThrExtraInfoBase(const std::string & thrTypeName, const ptree & data);

      virtual std::string className() const override {
         return "L1ThrExtraInfoBase";
      }

      const std::string & thresholdTypeName() const;

      bool hasExtraInfo() const;

      unsigned int resolutionMeV() const { 
         return m_resolutionMeV;
      }

   protected:
      virtual void upload() {
         load();
      } 
      std::map<std::string, DataStructure> m_extraInfo{};

   private:
      // load the internal members
      void load();

      unsigned int m_resolutionMeV { 1000 }; // default resolution is 1 GeV
   };



   /** 
    * @brief Standard L1 threshold configuration
    *
    * Provides access to the selection parameters
    */
   class L1Threshold : public DataStructure {
   public:
      
      /** @brief static method to create type-specific L1Thresholds
       * @param name [in] name of the threshold
       * @param type [in] type of the threshold (e.g. EM, TAU, JET, XE, TE, eEM, jJ, gXE, ...)
       * @param extraInfo [in] link to the extra info for the given type
       * @param data [in] the threshold definition (json wrapped into a ptree)
       * @return shared ptr of the created threshold (returns ownership)
       */
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

      virtual std::string className() const override
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
       * must be overwritten by L1Threshold_Calo and L1Threnshold_MU
       */
      virtual float thrValue(int eta = 0) const;

   protected:

      /** Update the internal data after modification of the data object */
      virtual void update() override;

      std::weak_ptr<L1ThrExtraInfoBase> m_extraInfo;

   private:

      void load();

      std::string m_type{""}; ///< threshold type
      unsigned int m_mapping{0}; ///< unique identifier amongst thresholds of the same type
   };


   class L1Threshold_Calo : public L1Threshold {
   public:

      /** Constructor */
      L1Threshold_Calo() = delete;
      
      /** Constructor initialized with configuration data 
       * @param name threshold name
       * @param type threshold type name
       * @param extraInfo The information that is specific for this threshold type 
       * @param data The data containing the L1 threshold 
       */
      L1Threshold_Calo( const std::string & name, const std::string & type,
                        std::weak_ptr<L1ThrExtraInfoBase> extraInfo, const ptree & data);

      /** Destructor */
      virtual ~L1Threshold_Calo() = default;

      // Accessors to the threshold value for eta-dependent threholds

      /* @brief Accessor to the threshold value in GeV
       * @param eta the eta value should be given for potentially eta-dependent thresholds
       * @returns threshold in GeV
       */
      virtual float thrValue(int eta = 0) const;

      /* @brief Accessor to the threshold value in energy units
       * @param eta the eta value should be given for potentially eta-dependent thresholds
       * @returns threshold in energy units
       */
      virtual unsigned int thrValueCounts(int eta = 0) const;

      /* @brief Accessor to the threshold value in MeV
       * @param eta the eta value should be given for potentially eta-dependent thresholds
       * @returns threshold in MeV
       */
      virtual unsigned int thrValueMeV(int eta = 0) const;

      /** access to the list of ThresholdValues in GeV */
      virtual ValueWithEtaDependence<float> thrValues() const;

      /** access to the list of ThresholdValues in MeV */
      virtual const ValueWithEtaDependence<unsigned int> & thrValuesMeV() const;      

      /** access to the list of ThresholdValues in energy units */
      virtual ValueWithEtaDependence<unsigned int> thrValuesCounts() const;      

   protected:

      /** Update the internal data after modification of the data object */
      virtual void update();

      std::string m_input{""};

      // internally the threshold cuts are represented in MeV
      // - it is human readible
      // - integer are preferred over float by the L1Calo/L1Calo firmware experts
      // - GeV is not possible as we would like to support cuts with finer granularity than GeV

      unsigned int m_thrValue {0}; ///< threshold value in MeV

      ValueWithEtaDependence<unsigned int> m_etaDepThrValue{""}; ///< eta-dependent threshold value in MeV

   private:
      void load();
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

// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMISSINGET_VERSIONS_MISSINGETBASE_H
#define XAODMISSINGET_VERSIONS_MISSINGETBASE_H


#define EXTRACT_PX( _T_, _V_, _P_ )		\
  _T_ _V_(_P_.p4().Px())

#define EXTRACT_PY( _T_, _V_, _P_ )		\
  _T_ _V_(_P_.p4().Py())

#define EXTRACT_SCALED_PX( _T_, _V_, _P_, _S_ )	\
  _T_ _V_(_P_.p4().Px()*_S_)

#define EXTRACT_SCALED_PY( _T_, _V_, _P_, _S_ )	\
  _T_ _V_(_P_.p4().Py()*_S_)

#include <cstddef>
#include <cmath>
#include <vector>
#include <string>

#include <boost/tuple/tuple.hpp>

/*! @brief General namespace for MET EDM software */
namespace MissingETBase
  {
    /*! @brief Namespace for generally used type definitions */
    namespace Types
    {
      typedef unsigned long long                   bitmask_t;         /*!< @brief Type for status word bit mask */
      typedef std::vector<bitmask_t>               bitmask_vector_t;  /*!< @brief Type for vector of status word bit masks */
      typedef boost::tuples::tuple<size_t,size_t>  indexedlink_t;     /*!< @brief Type for links defined as index pairs */ 
    } // namespace MissingETBase::Types
    
    namespace Constants
    {
      /*! @name Numerical constants */
      /*!@{*/
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double precision  = 0.00001;      /*!< @brief Maximum numerical distance up to which two double precision variables are considered to be identical. */

      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      size_t invalidIndex = size_t(-1); /*!< @brief Invalid index indicator */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      size_t unknownIndex = size_t(-1); /*!< @brief Unknown index indicator */

      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double wpxDefault = 1.0;          /*!< @brief Default kinematic weight @f$ w_{x} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double wpyDefault = 1.0;          /*!< @brief Default kinematic weight @f$ w_{y} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double wetDefault = 1.0;          /*!< @brief Default kinematic weight @f$ w_{\rm T} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double wpxError   = 0.0;          /*!< @brief Error return value for kinematic weight @f$ w_{x} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double wpyError   = 0.0;          /*!< @brief Error return value for kinematic weight @f$ w_{y} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double wetError   = 0.0;          /*!< @brief Error return value for kinematic weight @f$ w_{\rm T} @f$ */

      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double cpxDefault = 0.0;          /*!< @brief Default constituent vector @f$ P_{x} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double cpyDefault = 0.0;          /*!< @brief Default constituent vector @f$ P_{y} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double cpzDefault = 0.0;          /*!< @brief Default constituent vector @f$ P_{z} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double ceDefault = 0.0;           /*!< @brief Default constituent vector @f$ E @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double sumptDefault = 0.0;           /*!< @brief Default constituent sumpt */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double cpxError = -9999999;          /*!< @brief Error constituent vector @f$ P_{x} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double cpyError = -9999999;          /*!< @brief Error constituent vector @f$ P_{y} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double cpzError = -9999999;          /*!< @brief Error constituent vector @f$ P_{z} @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double ceError = -9999999;           /*!< @brief Error constituent vector @f$ E @f$ */
      static
#if __cplusplus < 201100
      const
#else
      constexpr
#endif // C++11
      double sumptError = -9999999;           /*!< @brief Error constituent vector @f$ E @f$ */

      /*!@}*/
      /*! @name String constants */
      /*!@{*/
      static const std::string invalidString = "INVALID"; /*!< @brief Invalid string indicator */
      static const std::string unknownString = "UNKNOWN"; /*!< @brief Unknown string indicator */
      static const std::string emptyString = "EMPTY";     /*!< @brief Empty string indicator */
      static const std::string nullString = "NULL";       /*!< @brief NULL string indicator */
      /*!@}*/
    } // namespace MissingETBase::Constants

    /*! @brief Numerical constants and operations */
    struct Numerical
    {
      /*! @name Access to constants */
      /*!@{*/
      static double               precision()    { return Constants::precision; }     /*!< @brief Access precision of numerical distance measure for equality */
      static double               wpxDefault()   { return Constants::wpxDefault; }    /*!< @brief Access default kinematic weight @f$ w_{x} @f$ */
      static double               wpyDefault()   { return Constants::wpyDefault; }    /*!< @brief Access default kinematic weight @f$ w_{y} @f$ */
      static double               wetDefault()   { return Constants::wetDefault; }    /*!< @brief Access default kinematic weight @f$ w_{\rm T} @f$ */
      static double               wpxError()     { return Constants::wpxError; }      /*!< @brief Access error return value for kinematic weight @f$ w_{x} @f$ */
      static double               wpyError()     { return Constants::wpyError; }      /*!< @brief Access error return value for kinematic weight @f$ w_{y} @f$ */
      static double               wetError()     { return Constants::wetError; }      /*!< @brief Access error return value for kinematic weight @f$ w_{\rm T} @f$ */
      static double               cpxDefault()   { return Constants::cpxDefault; }    /*!< @brief Access default constituent vector @f$ P_{x} @f$ */
      static double               cpyDefault()   { return Constants::cpyDefault; }    /*!< @brief Access default constituent vector @f$ P_{y} @f$ */
      static double               cpzDefault()   { return Constants::cpzDefault; }    /*!< @brief Access default constituent vector @f$ P_{z} @f$ */
      static double               ceDefault()    { return Constants::ceDefault; }     /*!< @brief Access default constituent vector @f$ E @f$ */
      static double               sumptDefault() { return Constants::sumptDefault; }  /*!< @brief Access default constituent vector sumpt */
      static double               cpxError()     { return Constants::cpxError; }      /*!< @brief Access error return value for constituent vector @f$ P_{x} @f$ */
      static double               cpyError()     { return Constants::cpyError; }      /*!< @brief Access error return value for constituent vector @f$ P_{y} @f$ */
      static double               cpzError()     { return Constants::cpzError; }      /*!< @brief Access error return value for constituent vector @f$ P_{z} @f$ */
      static double               ceError()      { return Constants::ceError; }       /*!< @brief Access error return value for constituent vector @f$ E @f$ */
      static double               sumptError()   { return Constants::sumptError; }       /*!< @brief Access error return value for constituent vector sumpt */
      static size_t               invalidIndex() { return Constants::invalidIndex; }  /*!< @brief Access invalid index indicator */
      static size_t               unknownIndex() { return Constants::unknownIndex; }  /*!< @brief Access unknown index indicator */
      static Types::indexedlink_t invalidLink()  { return Types::indexedlink_t(Constants::invalidIndex,Constants::invalidIndex); } /*!< @brief Access invaalid link tag */
      /*!@}*/
      /*! @name Helpers */
      /*@{*/
      /*! @brief Test of equality
       *
       *  Compares the absolute difference @f$ \Delta = | x - y | @f$ between two numbers @f$ ( x, y ) @f$ with a precision indicator @f$ \Delta_{\rm min} @f$, 
       *  and declares those two numbers equal if @f$ \Delta < \Delta_{\rm min} @f$.
       *
       *  @return @c true if the two numbers are identical within the distance @f$ \Delta_{\rm min} @f$, else @c false.
       *
       *  @param[in] x first number for distance measure
       *  @param[in] y second number for distance measure   
       */
      static bool   isEqual(double x,double y) { return std::abs(x-y) < precision(); }
      /*! @brief Failsafe division
       *
       *  This function executes the division of two numbers @f$ x @f$ and @f$ y @f$ such that @f$ f = x/y @f$ is 
       *  @f{eqnarray*}{ f & = & \left\{ \begin{array}{2} x/y & y \neq 0 \\ 0 & y = 0 \end{array} \right. @f}
       */
      static double divide(double x,double y) { return y != 0. ? x/y : 0.; }
      /*!@}*/
    }; // struct Numerical

    /*! @brief String constants, tags, and handling */
    struct Strings
    {
      /*! @name Access to constants */
      /*!@{*/
      static const std::string& invalidString()  { return Constants::invalidString; } /*!< @brief Access invalid string indicator */
      static const std::string& unknownString()  { return Constants::unknownString; } /*!< @brief Access unknown string indicator */
      static const std::string& emptyString()    { return Constants::emptyString; }   /*!< @brief Access empty string indicator */
      static const std::string& nullString()     { return Constants::nullString; }    /*!< @brief Access NULL string indicator */
    }; // struct String
    
    /*! @brief MET source indicators
     *
     *  The source indicators are collected into exclusive categories "Refined", "Truth", "Calo" and "ID". In addition, the
     *  (exclusive) type of the MET term (the signal source) can specified. Further indicators concern regional contributions 
     *  (central, endcap, and forward), and dedicated qualifiers for the type of truth in MET from Monte Carlos. 
     *
     *  <b>MET Categories</b>
     * 
     *  Categories are exclusively tagged, no MET term can be in two categories at the same time.  
     *  
     *  The "Refined" category includes reconstructed MET terms from data and detector simulations which represent the highest level of 
     *  corrections and calibrations applied to given physics and signal object contributions. The particular meaning of this depends
     *  on the context. For example, in the a long standing default in MET reconstruction the fully calibrated and corrected contributions
     *  from hard objects are tagged "Refined", as well as the SoftTerm with or without pile-up correction applied. 
     *
     *  The "Truth" category is exclusivly used for Monte Carlo particle level MET terms. 
     *
     *  The "Diagnostics" category is meant for MET contributions based on simplified reconstruction schemes, which are not appropriate for
     *  physics analysis but can help to evaluate the MET reconstruction performance.  
     */
    struct Source
    {
      /*! @name Bitmasks for MET source indicators */
      /*!@{*/
      /*! @brief Source category tag */
      enum Category  { Refined         = 0x10000,   /*!< @brief Indicator for genuine reconstructed MET terms */ 
		       Truth           = 0x20000,   /*!< @brief Indicator for MET terms from the particle level (MC truth) event */
		       Calo            = 0x40000,   /*!< @brief Indicator for MET terms reconstructed from calorimeter signals alone */
		       ID              = 0x80000,   /*!< @brief Indicator for MET terms reconstructed from inner detector (ID) tracks alone */
		       UnknownCategory = 0x00000    /*!< @brief Indicator for an unknown category */ };
      /*! @brief Physics contribution types category */
      enum Type      { Electron    = 0x00001,   /*!< @brief Indicator for the MET term from reconstructed or MC truth electrons */ 
		       Photon      = 0x00002,   /*!< @brief Indicator for the MET term from reconstructed or MC truth electrons */
		       Tau         = 0x00004,   /*!< @brief Indicator for the MET term from reconstructed or MC truth tau leptons */
		       Muon        = 0x00008,   /*!< @brief Indicator for the MET term from reconstructed or MC truth muons */
		       Jet         = 0x00010,   /*!< @brief Indicator for the MET term from reconstructed or MC truth particle jets */ 
		       SoftEvent   = 0x00020,   /*!< @brief Indicator for the MET term from reconstructed soft event signals (tracks, clusters) or MC truth particles not contributing
						 *         to any other (hard) final state object. */
		       Total       = 0x00040,   /*!< @brief Indicator for the total reconstructed MET (sum of MET terms from reconstruction and MC truth particles) */
		       UnknownType = 0x00000    /*!< @brief Indicator for an unknown MET term source */ };
      /*! @brief Regional tags
       *
       *  These regional tags indicate MET contributions from three regions in ATLAS (central, endcap, and forward). The particular ranges in pseudorapidity (@f$ \eta @f$)
       *  defining these regions are set in the tools calculating the respective MET terms.
       */
      enum Region    { Central        = 0x00100,  /*!< @brief Indicator for MET contribution from the central region */
		       Endcap         = 0x00200,  /*!< @brief Indicator for MET contribution from the endcap region */
		       Forward        = 0x00400,  /*!< @brief Indicator for MET contribution from the forward region */
		       FullAcceptance = 0x00000,  /*!< @brief Indicator for full detector acceptance contribution */
		       UnknownRegion  = 0x00000   /*!< @brief Indicator for unknown region (same as full acceptance contribution) */ };
      /*! @brief Truth type indicators 
       *
       *  These indicators are only applicable for MC generated events. The types have the Truth category bit set. 
       */
      enum TruthType { NonInt       = 0x21000, /*!< @brief Indicator for MET from non-interacting particles generated in the collision */
		       Int          = 0x22000, /*!< @brief Indicator for MET from interacting particles within the nominal @f$ \eta @f$ acceptance of the detector */
		       IntOut       = 0x24000, /*!< @brief Indicator for MET contributions from particles outside of the @f$ \eta @f$ acceptance of the detector (typically between
						*          edge of the detector and the maximum @f$ \eta @f$ for "stable" particle production used in the MC generation) */
		       TruthMuons   = 0x28000, /*!< @brief Indicator for muons from MC truth (muons from the interaction) */
		       UnknownTruth = 0x00000  /*!< @brief Unknown truth type indicator */ };
      /*! @brief Signal contribution types */
      enum Signal    { LCTopo        = 0x01000, /*!< @brief Indicator for MET contribution from TopoClusters with LCW calibration applied */ 
		       EMTopo        = 0x02000, /*!< @brief Indicator for MET contribution from TopoClusters with EM (basic signal) calibration applied */ 
		       Track         = 0x04000, /*!< @brief Indicator for MET contribution from reconstructed charged particle tracks */
		       UnknownSignal = 0x00000  /*!< @brief Unknown signal contribution */ };

      /*! @name Bit mask constructors for pre-defined MET physics object contributions */
      /*!@{*/
      /*! @brief Standard MET term from reconstructed electrons
       *
       *  @return Default bit pattern for the electron MET term indicator. It can optional be OR'ed with a regional tag.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t electron(Region reg=FullAcceptance)                      { return ( Refined | Electron  ) | reg; }
      /*! @brief Standard MET term from reconstructed photons
       *
       *  @return Default bit pattern for the photon MET term indicator. It can optional be OR'ed with a regional tag.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t photon(Region reg=FullAcceptance)                        { return ( Refined | Photon    ) | reg; }
      /*! @brief Standard MET term from reconstructed tau leptons
       *
       *  @return Default bit pattern for the tau MET term indicator. It can optional be OR'ed with a regional tag.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t tau(Region reg=FullAcceptance)                           { return ( Refined | Tau       ) | reg; }
      /*! @brief Standard MET term from reconstructed muons
       *
       *  @return Default bit pattern for the muon MET term indicator. It can optional be OR'ed with a regional tag.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t muon(Region reg=FullAcceptance)                          { return ( Refined | Muon      ) | reg; }
      /*! @brief Standard MET term from reconstructed jets
       *
       *  @return Default bit pattern for the jet MET term indicator. It can optional be OR'ed with a regional tag.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t jet(Region reg=FullAcceptance)                           { return ( Refined | Jet       ) | reg; }
      /*! @brief Standard MET term from reconstructed soft event
       *
       *  @return Default bit pattern for the soft event MET term indicator. It can optional be OR'ed with a regional tag.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t softEvent(Region reg=FullAcceptance)                     { return ( Refined | SoftEvent ) | reg; }
      /*! @brief Standard full reconstructed MET
       *
       *  @return Default bit pattern for a fully reconstructed MET. It can optional be OR'ed with a regional tag.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t total(Region reg=FullAcceptance)                         { return ( Refined | Total     ) | reg; }
      /*!@}*/
      /*! @name Bit mask constructors for pre-defined MET signal object contributions */
      /*!@{*/
      /*! @brief Bit mask for MET term from any (or mixed) signal objects
       *
       *  @return Default bit pattern for a MET term exclusively constructed from LCTopo or Track signal objects, or a mix of LCTopo and Track objects (default behaviour). 
       *
       *  @param[in] sig signal source indicator (optional, default is a mix of LCTopo and Track objects)
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t signal(Types::bitmask_t sig=(LCTopo|Track),Region reg=FullAcceptance)  { return ( Refined | sig       ) | reg; }
      /*! @name Bit mask constructors for pre-defined MET signal object contributions */
      /*!@{*/
      /*! @brief Bit mask for MET term from Track signal objects
       *
       *  @return Default bit pattern for a MET term exclusively constructed from Track signal objects.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t track(Region reg=FullAcceptance)                         { return signal(Track,reg); }
      /*! @brief Bit mask for MET term from LCTopo (locally calibrated calorimeter cell clusters) signal objects
       *
       *  @return Default bit pattern for a MET term exclusively constructed from LCTopo signal objects.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t clusterLC(Region reg=FullAcceptance)                     { return signal(LCTopo,reg); }
      /*! @brief Bit mask for MET term from EMTopo signal objects
       *
       *  @return Default bit pattern for a MET term exclusively constructed from EMTopo signal objects.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t clusterEM(Region reg=FullAcceptance)                     { return signal(EMTopo,reg); }
      /*! @brief Bit mask for MET term from any cluster signal objects
       *
       *  @return Default bit pattern for a MET term constructed from EMTopo and/or LCTopo signal objects.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */		      
      static Types::bitmask_t cluster(Region reg=FullAcceptance)                       { return signal(LCTopo|EMTopo,reg); }
      /*!@}*/

      /*! @name Truth indicator bit patterns */
      /*!@{*/
      /*! @brief Standard bit pattern indicating genuine true MET
       *
       *  @return Default bit pattern for a MET term constructed from non-interacting final state particles in the MC truth event.  
       */
      static Types::bitmask_t truthNonInt()                         { return NonInt; }
      /*! @brief Standard bit pattern indicating effective true MET
       *
       *  @return Default bit pattern for a MET term constructed from all (stable and interacting) particles emitted into the detector @f$ \eta @f$ acceptance. 
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */
      static Types::bitmask_t truthInt(Region reg=FullAcceptance)   { return Int | reg; }
      /*! @brief Standard bit pattern indicating a true MET contribution from particles outside of the detector @f$ \eta @f$ acceptance.
       *
       *  @return Default bit pattern for a MET term constructed from all (stable and interacting) final state particles in the MC truth event generated outside the
       *          detector @f$ \eta @f$ acceptance.  
       */
      static Types::bitmask_t truthIntOut()                         { return IntOut; }
      /*! @brief Standard bit pattern indicating the true muon contribution to MET truth
       *
       *  @return Default bit pattern for a MET term constructed from muons generated in the interaction.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */
      static Types::bitmask_t truthMuons(Region reg=FullAcceptance) { return TruthMuons | reg; }
      /*!@}*/
     
      /*! @name Indicators for MET terms from detector signals */
      /*!@{*/
      /*! @brief Bit pattern indicating a LCTopo MET term
       *
       *  @return Bit pattern indicating a MET term exclusively constructed from (all) LCTopo clusters in the event. 
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */
      static Types::bitmask_t caloLCTopo(Region reg=FullAcceptance) { return ( Calo | LCTopo ) | reg; } 
      /*! @brief Bit pattern indicating a EMTopo MET term
       *
       *  @return Bit pattern indicating a MET term exclusively constructed from (all) EMTopo clusters in the event. 
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */
      static Types::bitmask_t caloEMTopo(Region reg=FullAcceptance) { return ( Calo | EMTopo ) | reg; }
      /*! @brief Bit pattern indicating a ID Track MET term
       *
       *  @return Bit pattern indicating a MET term exclusively constructed from ID tracks.
       *
       *  @param[in] reg regional indicator bit pattern (optional, default is FullAcceptance)
       */
      static Types::bitmask_t idTrack(Region reg=FullAcceptance)    { return ( ID | Track )    | reg; } 
      /*!@}*/

      /*! @name Pattern tests for categories, types, truth types, and signal tags */
      /*!@{*/
      /*! @brief Generic check for given pattern
       *
       *  @return @c true if given bit pattern is set in data word, else @c false.
       *
       *  @param[in] bits bit pattern to be tested
       *  @param[in] mask test pattern
       *
       *  @note This is not a test on equality of two bit patterns. The only requirement is that the tested word has all bits of a given pattern set. Additional
       *        set bits are ignored.
       */
      static bool hasPattern(Types::bitmask_t bits,Types::bitmask_t mask)   { return ( bits & mask)  == mask; }
      /*! @brief Check if bit pattern includes a given category
       *
       *  @return @c true if bits representing a given catergory are set, else @c false.
       *
       *  @param[in] bits bit pattern to be tested
       *  @param[in] cat  category tag  
       */
      static bool hasCategory(Types::bitmask_t bits,Category cat)           { return hasPattern(bits,cat); }
      static bool hasType(Types::bitmask_t bits,Type type)                  { return hasPattern(bits,type); }
      static bool hasTruthType(Types::bitmask_t bits,TruthType type)        { return hasPattern(bits,type); }
      static bool hasSignal(Types::bitmask_t bits,Signal sig)               { return hasPattern(bits,sig);}
      /*!@}*/

      static bool isElectronTerm(Types::bitmask_t bits,Region reg=FullAcceptance) { return hasPattern(bits,electron(reg)); }
      static bool isPhotonTerm(Types::bitmask_t bits,Region reg=FullAcceptance)   { return hasPattern(bits,photon(reg)); }
      static bool isTauTerm(Types::bitmask_t bits,Region reg=FullAcceptance)      { return hasPattern(bits,tau(reg)); }
      static bool isMuonTerm(Types::bitmask_t bits,Region reg=FullAcceptance)     { return hasPattern(bits,muon(reg)); }
      static bool isJetTerm(Types::bitmask_t bits,Region reg=FullAcceptance)      { return hasPattern(bits,jet(reg)); }
      static bool isSoftTerm(Types::bitmask_t bits,Region reg=FullAcceptance)     { return hasPattern(bits,softEvent(reg)); }
      static bool isTrackTerm(Types::bitmask_t bits,Region reg=FullAcceptance)    { return hasPattern(bits,track(reg)); }
      static bool isTotalTerm(Types::bitmask_t bits,Region reg=FullAcceptance)    { return hasPattern(bits,total(reg)); }

      static bool isTruthNonInt(Types::bitmask_t bits)                          { return hasPattern(bits,truthNonInt()); }
      static bool isTruthInt(Types::bitmask_t bits,Region reg=FullAcceptance)    { return hasPattern(bits,truthInt(reg)); }
      static bool isTruthIntOut(Types::bitmask_t bits)                          { return hasPattern(bits,truthIntOut()); }
      static bool isTruthMuons(Types::bitmask_t bits,Region reg=FullAcceptance)  { return hasPattern(bits,truthMuons(reg)); }

      static bool unknown() { return UnknownCategory; }
    };

} // namespace MissingETBase

/* @file MissingETBase.h
 *
 *  @name Globalnamespace operators
 */
/*!@{*/
bool operator==(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1);
bool operator!=(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1);
bool operator>=(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1);
bool operator<=(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1);
bool operator>(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1);
bool operator<(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1);
/*!@}*/

inline bool operator==(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1)
{ return l0.get<0>() == l1.get<0>() && l0.get<1>() == l1.get<1>(); }

inline bool operator!=(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1)
{ return !operator==(l0,l1); }

inline bool operator>=(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1)
{ return l0.get<0>() == l1.get<0>() ? l0.get<1>() >= l1.get<1>() : l0.get<0>() > l1.get<0>(); }

inline bool operator<=(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1)
{ return l0.get<0>() == l1.get<0>() ? l0.get<1>() <= l1.get<1>() : l0.get<0>() < l1.get<0>(); }

inline bool operator>(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1)
{ return l0.get<0>() == l1.get<0>() ? l0.get<1>() > l1.get<1>() : l0.get<0>() > l1.get<0>(); }

inline bool operator<(MissingETBase::Types::indexedlink_t l0,MissingETBase::Types::indexedlink_t l1)
{ return l0.get<0>() == l1.get<0>() ? l0.get<1>() < l1.get<1>() : l0.get<0>() < l1.get<0>(); }
 
#endif

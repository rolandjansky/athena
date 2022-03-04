/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGTOOLS_TRIG_VSI_COORD
#define TRIGTOOLS_TRIG_VSI_COORD

/**
* @file   Coordinate.h
* @brief  Coordinate policies.
* @author Takane Sano <takane.sano@cern.ch>
* @date   July 15th, 2021
* @details  Coordinate policies must have these static member functions.
*           - TVector3 X123toXYZ( const KDPoint<double,3>& )
*           - KDPoint<double,3> XYZtoX123( const TVector3& )
*           - KDPoint<double,3> Proj( const KDPoint<double,3>& )
*           - KDPoint<int,3> ProjBin( const KDPoint<int,3>&, const std::unique_ptr<TH3D>& )
*/
#include "TrigVrtSecInclusive/KDPoint.h"

#include "TMath.h"
#include "TH3D.h"
#include "TVector3.h"

#include <cmath>

namespace TrigVSI{

//! Namespace for Coordinate policy used in TrigVSI::VtxMap
namespace Coordinate{

/**
 *  @brief  Cordinate policy for x, y, z.
 */
class Cartesian {
   public :
      //! @name Converter functions
      //! @{
      static inline TVector3 X123toXYZ( double x1, double x2, double x3) {
         return TVector3(x1, x2, x3);
      };

      static inline TVector3 X123toXYZ( const KDPoint<double,3>& p ) {
         return TVector3(p.at(0),p.at(1),p.at(2));;
      };

      static inline KDPoint<double,3> XYZtoX123( const TVector3& v ) {
         return KDPoint<double,3>({v.x(), v.y(), v.z()});
      };
      //! @}

      //! @name Projection functions
      //! @{
      static inline KDPoint<double,3> Proj(const KDPoint<double,3>& p) {
         return p;
      };
      //! @}

      //! @name Projection functions with ibin
      //! @{
      static inline KDPoint<int,3> ProjBin(const KDPoint<int,3>& ibin, const std::unique_ptr<TH3D>&) {
         return ibin;
      };
      //! @}
};



/**
 *  @brief  Cordinate policy for perp, eta, phi.
 */
class Pseudo {
   public :
      //! @name Converter functions
      //! @{
      static inline TVector3 X123toXYZ( double x1, double x2, double x3) {
         TVector3 tmp;
         tmp.SetPtEtaPhi(x1,x2,x3);
         return tmp;
      };

      static inline TVector3 X123toXYZ( const KDPoint<double,3>& p ) {
         TVector3 tmp;
         tmp.SetPtEtaPhi(p.at(0),p.at(1),p.at(2));
         return tmp;
      };

      static inline KDPoint<double,3> XYZtoX123( const TVector3& v ) {
         return KDPoint<double,3>({v.Perp(), v.Eta(), v.Phi()});
      };
      //! @}

      //! @name Projection functions
      //! @{
      static inline KDPoint<double,3> Proj(const KDPoint<double,3>& p) {
         KDPoint<double,3> tmp = p;
         tmp[2] = std::fmod( p[2] + TMath::Pi(), 2. * TMath::Pi() ) - TMath::Pi();
         return tmp;
      };
      //! @}

      //! @name Projection functions with ibin
      //! @{
      static inline KDPoint<int,3> ProjBin(const KDPoint<int,3>& ibin, const std::unique_ptr<TH3D>& map) {
         KDPoint<int,3> tmp = ibin;
         tmp[2] = (ibin[2] - 1) % (map->GetNbinsZ() - 1 + 1) + 1;
         return tmp;
      };
      //! @}
};



///===========================================================================
/// @brief  struct for meta-function to judge the policy class is coordinate.
///
template<typename T>
struct is_coord {
   static auto check(...) -> std::false_type;

   template<typename _Coord>
   static auto check(_Coord*) ->
               decltype(   static_cast<TVector3(*)(const KDPoint<double,3>&)>(&_Coord::X123toXYZ),
                           static_cast<KDPoint<double,3>(*)(const TVector3&)>(&_Coord::XYZtoX123),
                           static_cast<KDPoint<double,3>(*)(const KDPoint<double,3>&)>(&_Coord::Proj),
                           static_cast<KDPoint<int,3>(*)(const KDPoint<int,3>&, const std::unique_ptr<TH3D>&)>(&_Coord::ProjBin),
                           std::true_type() );

   static constexpr T*   insPolicy = nullptr;
   static constexpr bool value = decltype(check(insPolicy))::value;
};

} // end of namespace Coordinate

} // end of namespace TrigVSI

#endif

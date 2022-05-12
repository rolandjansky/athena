/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ROIPHIRZCOLLECTION_H
#define ROIPHIRZCOLLECTION_H

#include <vector>
#include <array>
#include <algorithm>
#include <cmath>
#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaKernel/CLASS_DEF.h"

#include <iostream>

// class to hold ROIs defined by Phi, r and z
class ROIPhiRZ : public std::array<float,3>
{
public:
   enum ERoiParameters {kPhi,kR, kZ};
   inline double eta() const {
      const double R   = r();
      const double Z   = z();
      return std::atanh( Z / std::sqrt( R*R + Z*Z ) );
   }
   inline double theta() const {
      double Z = z();
      double R = r();
      return std::atan2(1., Z / R);
   }
   inline float phi() const {
      return (*this)[kPhi];
   }
   inline float r() const {
      return (*this)[kR];
   }
   inline float z() const {
      return (*this)[kZ];
   }

};

/** @brief container for phi sorted ROIs defined by phi, r and z.

    Collection to hold a phi sorted array containing ROIs defined by phi, r and z, entries with close to 2 pi will also be stored with negaitve phi
    Thus the lower bound of ROIs with |phi - ROI_phi| < ROI_width, is can be obtained by a binary search for phi-ROI_width
*/
class ROIPhiRZContainer : public std::vector<ROIPhiRZ>
{
public:

   /** Test whether there is a matching ROI for the given phi and r,z corrected eta
    */
   inline bool hasMatchingROI( float phi, double eta, double r, double z, float roi_phi_width, double roi_eta_width) const {
      return hasMatchingROI(*this,phi,eta,r,z,roi_phi_width,roi_eta_width);
   }

   inline const_iterator lowerPhiBound( float phi, float roi_phi_width) const {
      return lowerPhiBound( *this, phi, roi_phi_width);
   }

   void addROI(const Amg::Vector3D &global_position, float roi_phi_width) {

      float phi = global_position.phi();
      assert (std::abs(phi)<=M_PI );
      float z = global_position.z();
      float r = global_position.perp();
      if ( std::abs(phi) > M_PI - roi_phi_width) {
         constexpr float pi_2 = 2*M_PI;
         float sign_phi_times_2_pi = std::copysign(pi_2,phi);
         // wrap ROIs close to -pi and pi around. Thus when searching for the lower bound ROI for phi-phi_width
         // ROIs close to -pi and ROIs close to +pi will be found.
         this->emplace_back( ROIPhiRZ{phi - sign_phi_times_2_pi, r, z} );
     }
     this->emplace_back( ROIPhiRZ{phi, r, z});
   }

   void sort() {
      // sort output ROIs by phi
      std::sort( this->begin(), this->end(), order );
   }

   static inline double eta(const ROIPhiRZ &roi) {
      const double R   = roi.r();
      const double Z   = roi.z();
      return std::atanh( Z / std::sqrt( R*R + Z*Z ) );
   }
   static inline double theta(const ROIPhiRZ &roi) {
      return std::atan2(1., roi.z() / roi.r());
   }
   static inline float phi(const ROIPhiRZ &roi) {
      return roi.phi();
   }

protected:
   static inline double sqr(double a) { return a * a; }

   /** @brief Helper function to compute a z position corrected delta eta.
    */
   static inline double deltaEta(const ROIPhiRZ &roi, double other_r, double other_z, double other_eta) {
      //Correct eta of ROI to take into account the z postion of the reference
      double newR   = roi.r() - other_r;
      double newZ   = roi.z() - other_z;
      double newEta =  std::atanh( newZ / std::sqrt( newR*newR + newZ*newZ ) );
      double delta_eta = std::abs(newEta - other_eta);
      return delta_eta;
   }

   /** Helper function to order ROIs defined by phi,r,z by phi.
    */
   static inline bool order(const ROIPhiRZ &a, const ROIPhiRZ &b) { return a.phi() < b.phi(); }

   /** Helper function to find the lower bound of ROIs which match |phi - ROI_phi| < ROI_width;
    * @param rois the phi sorted roi container
    * @param phi the phi value for which the lower bound of ROIs is desired.
    * @param ROI width the width of the ROI in phi (must be smaller or equal the width that was used during the creation of the ROI container.
    * @param the lower phi bound of ROIs matching |phi - ROI_phi| < ROI_width or rois.end() if there are no matching rois.
    */
   static inline ROIPhiRZContainer::const_iterator lowerPhiBound( const ROIPhiRZContainer &rois, float phi, float roi_phi_width) {
      if (phi>M_PI) {
         phi -= 2*M_PI;
      }
      //search first ROI which is greater than phi - row_phi-width i.e. the first ROI which could be within the vicinity of phi
      return std::upper_bound( rois.begin(), rois.end(), phi-roi_phi_width, [](float value, const ROIPhiRZ &element) { return element.phi() >= value; } );
   }


   static inline bool hasMatchingROI( const ROIPhiRZContainer &rois, float phi, double eta, double r, double z, float roi_phi_width, double roi_eta_width) {
      ROIPhiRZContainer::const_iterator start_roi_iter = lowerPhiBound( rois, phi, roi_phi_width);
      //  by construction if the iterator is valid the cluster it is pointing to has a phi larger than phi-roi_phi_width.
      //  So now just check for all clusters which are not too far away in phi whether there is one with a matching eta.
      for (ROIPhiRZContainer::const_iterator roi_iter = start_roi_iter; roi_iter != rois.end() && roi_iter->phi()  < phi + roi_phi_width; ++roi_iter) {
         if ( deltaEta(*roi_iter, r, z , eta) < roi_eta_width) { return true; }
      }
      return false;
   }

};

CLASS_DEF( ROIPhiRZContainer , 1290867916 , 1 )

#endif

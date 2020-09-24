/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file:   BPhysHypoHelper.h
 *  @author: Daniel Scheirich <daniel.scheirich@cern.ch>
 *
 *  @brief:  B-physcis xAOD helpers.
 *
 *  This class provides interface to the basic B-physics augmemtation which
 *  depends on the invariant mass hypothesis, i.e.:
 *    - invariant mass and its error
 *    - proper decay time
 *    - etc.
 *    
 *  It is derived from the BPhysHelper which means that it also provides
 *  access to all basic hypothesis-independent decorations (reffited tracks, lxy, etc). 
 * 
 *  The basic idea is that the analysis code augments the vertex with 
 *  hypothesis-independent variables (e.g. lxy, ptError, etc) and 
 *  also variables dependent on the given mass hypothesis (e.g. mass). Since
 *  one vertex can be treated as candidate for more than one decay hypothesis
 *  (e.g. in case of Bd->JpsiK* and Bs->JpsiPhi, or LamB->JPsiLam and Bd->Jpsi Ks),
 *  the hypothesis-dependent augmentation variables have prefix to distinguish between
 *  different hypotheses. These prefixes are defined by user when creating the BPhysHypoHelper:
 * 
 *  @code
 *    xAOD::BPhysHypoHelper jpsi("jpsi", vtx); // "jpsi" is the name of the hypothesis
 *  @endcode
 * 
 *  To check if the vertex has been augmented with variables for a given hypothesis, call method
 *  xAOD::BPhysHypoHelper::isValidHypo()  
 *
 *  Usage example:
 *
 *  @code
 *    void myFunction(xAOD::Vertex* vtx) {
 *      // Let "vtx" be some xAOD::Vertex created by the b-physics software
 *      // We gain access to its augmentations through the helper class:
 *      xAOD::BPhysHypoHelper jpsi("jpsi", vtx); // "jpsi" is the name of the hypothesis
 *
 *      // access to mehods inherited from the BPhysHelper:
 *      std::cout << "Refitted pT of the 1st track: ";
 *      if(jpsi.nRefTrks()>0) {
 *        std::cout << jpsi.refTrk(0).Pt();
 *      }
 *      std::cout << std::endl;
 * 
 *      // access to the hypothesis-specific variables
 *      if( !jpsi.isValidHypo() ) {
 *        std::cout <<  "The vertex is not jpsi" << std::endl;
 *        return;
 *      } else {
 *        std::cout << "Jpsi invariant mass" << jpsi.m() << std::endl;
 *      }
 *
 *    }
 *  @endcode
 *
 */  

#ifndef XAOD_BPHYSHYPOHELPER_H
#define XAOD_BPHYSHYPOHELPER_H

#include "BPhysHelper.h"

#include <assert.h>


/** Base class for the B-physcis xAOD helpers */
namespace xAOD {

  class BPhysHypoHelper : public BPhysHelper {

    public:
      
      /************************************************************************/
      /** @brief: Main constructor 
       *
       *  The Main constructor.
       *  BPhysHypoHelper does NOT take ownership of the class "b"
       *
       *  @param[in] hypo  Name of the hypothesis
       *  @param[in] b     Pointer to the xAOD::Vertex
       */
      BPhysHypoHelper(const std::string &hypo, const xAOD::Vertex* b) :
        BPhysHelper(b),
        m_hypo(hypo)
      {
        /* nothing here */
      }
      
      /************************************************************************/
      /** @{
       *  Methods for access to the invariant mass hypothesis and its error
       */
      
      /** Get invariant mass and its error
       * 
       *  @returns: mass or error, -9999999. in case the augmentation doesn't exist
       */
      
      float mass() const;     //!< invariant mass
      float massErr() const;  //!< invariant mass error

      /** Set given invariant mass and its error
       * 
       *  @param[in] val   invariant mass or error calculated externally
       *  @returns: true on success
       */
      
      bool setMass   (const float val);  //!< invariant mass
      bool setMassErr(const float val);  //!< invariant mass error
      
      /** Set invariant mass for given track mass hypotheses. The number of mass hyotheses
       *  must be equal to the number of refitted tracks!
       * 
       *  @param[in] trkMasses   masses of the tracks.    
       *  @returns: true on success
       */
      
      bool setMass(const std::vector<double> &trkMasses);
      
      /** @} */
      /************************************************************************/

      /** @{
       *  Methods for access to the proper decay time and its error
       */
      
      /** @brief: Enum type the proper decay time calculation
       *  \enum tau_type
       *  @c TAU_CONST_MASS  the proper decay time calculated with the constant mass hypothesis
       *  @c TAU_INV_MASS    the proper decay time calculated with the measured invariant mass
       */
      
      enum tau_type { TAU_CONST_MASS, TAU_INV_MASS };
                    
      /** @brief: Get the proper decay time and error. 
       * 
       *  The proper decay time is calculated using formula c*tau = Lxy * M / pT. The Lxy can be calculated
       *  wrt different choice of the PV. Use parameter vertexType to specify which PV was used.
       *  The particle's mass M can be either taken as a constant or as measured invariant mass. Use
       *  parameter tauType to specify which value should be used.
       * 
       *  @param[in] vertexType  type (selection) of the PV (@see: BPhysHelper::pv_type)
       *  @param[in] tauType     specify which value of particle's mass was used (@see: tau_type)
       *  @returns: proper decay time, -9999999. on error       
       */
      
      float tau(const pv_type vertexType = BPhysHelper::PV_MIN_A0,
                const tau_type tauType = BPhysHypoHelper::TAU_CONST_MASS);  //!< proper decay time
                    
      float tauErr(const pv_type vertexType = BPhysHelper::PV_MIN_A0,
                   const tau_type tauType = BPhysHypoHelper::TAU_CONST_MASS);  //!< proper decay time error

      /** @brief: Set the proper decay time and error. 
       *  @param[in] val         input value
       *  @param[in] vertexType  type (selection) of the PV (@see: BPhysHelper::pv_type)
       *  @param[in] tauType     specify which value of particle's mass was used (@see: tau_type)
       *  @returns: true on success
       */
      
      bool setTau(const float val,
                  const pv_type vertexType = BPhysHelper::PV_MIN_A0,
                  const tau_type tauType = BPhysHypoHelper::TAU_CONST_MASS);  //!< proper decay time
                    
      bool setTauErr(const float val, 
                     const pv_type vertexType = BPhysHelper::PV_MIN_A0,
                     const tau_type tauType = BPhysHypoHelper::TAU_CONST_MASS);  //!< proper decay time error
//3D values
      float tau3d(const pv_type vertexType = BPhysHelper::PV_MIN_A0,
                const tau_type tauType = BPhysHypoHelper::TAU_CONST_MASS);  //!< proper decay time
                    
      float tau3dErr(const pv_type vertexType = BPhysHelper::PV_MIN_A0,
                   const tau_type tauType = BPhysHypoHelper::TAU_CONST_MASS);  //!< proper decay time error

      bool setTau3d(const float val,
                  const pv_type vertexType = BPhysHelper::PV_MIN_A0,
                  const tau_type tauType = BPhysHypoHelper::TAU_CONST_MASS);  //!< proper decay time
                    
      bool setTau3dErr(const float val, 
                     const pv_type vertexType = BPhysHelper::PV_MIN_A0,
                     const tau_type tauType = BPhysHypoHelper::TAU_CONST_MASS);  //!< proper decay time error

      /** @} */
      /************************************************************************/

      /** @{
       *  Methods for access to the pass flags
       */
      
      /** get the pass flag for this hypothesis
       *  @return: true, if the candidate passed selection for this hypothesis
       */
      bool pass() const;
      
      /** get the pass flag for this hypothesis
       *  @param[in] input value. Will be converted to Char_t
       *  @return: true on success
       */
      bool setPass(bool passVal);
      


      /** @} */
      /************************************************************************/
      
    protected:
      
      /** @{
       * Private attributes:
       */
      
      std::string m_hypo;   //!< name of the mass hypothesis
      
      /** @} */
      
      
  };
}

#endif
  
  

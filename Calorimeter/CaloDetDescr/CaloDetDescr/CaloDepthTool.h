/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloDepthTool.h
 *
 * @brief Declaration of CaloDepthTool. Created by Claire Bourdarios, 25.10.2004
 *
 * $Id: CaloDepthTool.h,v 1.9 2009-04-22 19:54:16 ssnyder Exp $
 */

#ifndef CALODETDESCR_CALODEPTHTOOL_H
#define CALODETDESCR_CALODEPTHTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CxxUtils/CachedPointer.h"

class CaloDetDescrManager;
class IMessageSvc;

#include "GeoPrimitives/GeoPrimitives.h"

/**
 * @class CaloDepthTool

   @brief Implementation of the ICaloDepthTool interface.Given a Track
 direction, checks if it is in the Calorimeter acceptance and provides various
 radii to be compared/used in the extrapolation.

   This tool extends the functionality of the old egammaUtils/egammaqdepth class

   In all the methods, radius stands for r in the (cylindric) barrel, for z in
 the (disk) endcap.

   The generic method radius() uses the default choosen by joboption, and if the
 result is 0 because the eta/phi direction is outside the Sample acceptance,
 returns the egparametrised depth as backup solution.

 */

class CaloDepthTool : public AthAlgTool

{
public:
  /** @brief Standard constructor
   */
  CaloDepthTool(const std::string& type, const std::string& name, const IInterface* parent);
  /** @brief Destructor
   */
  virtual ~CaloDepthTool();

  /** @brief Query interface
   */
  static const InterfaceID& interfaceID();

  /** @brief Tool Initialize
   */
  virtual StatusCode initialize() override;

  /** @brief Tool Finalize
   */
  virtual StatusCode finalize() override;

public:
  /** @brief get radius

      Generic method to be used in client code :
      the default choosen by joboption, and if the result is 0 because
      the eta/phi direction is outside the Sample acceptance, returns the
      egparametrised depth as backup solution.

   */
  double radius(const CaloCell_ID::SUBCALO subcalo,
                const int sampling_or_module,
                const bool barrel,
                const double eta,
                const double phi,
                const CaloDetDescrManager* caloDD) const;
  /** @brief get radius

      Generic method to be used in client code :
      the default choosen by joboption, and if the result is 0 because
      the eta/phi direction is outside the Sample acceptance, returns the
      egparametrised depth as backup solution.

   */
  double radius(const CaloCell_ID::CaloSample sample,
                const double eta,
                const double phi,
                const CaloDetDescrManager* caloDD) const;

  /** @brief How far are you (in eta) from a given sample or - if you are in -
     how far are you from the edge ?

      This method will return the distance to the closest edge.
      If you are outside the sample volume, it will always be a positive number.
      If you are inside the sample volume, the distance will be multiplied by -1
     : i.e.always a negative number.
  */
  static double deta(const CaloCell_ID::SUBCALO subcalo,
              const int sampling_or_module,
              const bool barrel,
              const double eta,
              const CaloDetDescrManager* caloDD) ;
  /** @brief How far are you (in eta) from a given sample or - if you are in -
     how far are you from the edge ?

      This method will return the distance to the closest edge.
      If you are outside the sample volume, it will always be a positive number.
      If you are inside the sample volume, the distance will be multiplied by -1
     : i.e.always a negative number.
  */
  static double deta(const CaloCell_ID::CaloSample sample,
              const double eta,
              const CaloDetDescrManager* caloDD) ;

  /** @brief For a given direction, return the entrance of the given
     CaloCell_ID::CaloSample. If the direction does not cross any CaloCell, the
     returned bool will be set to False.
   */
  static double entrance(const CaloCell_ID::SUBCALO subcalo,
                  const int sampling_or_module,
                  const bool barrel,
                  const double eta,
                  const double phi,
                  const CaloDetDescrManager* caloDD ) ;
  /** @brief For a given direction, return the entrance of the given
     CaloCell_ID::CaloSample. If the direction does not cross any CaloCell, the
     returned bool will be set to False.
   */
  static double entrance(const CaloCell_ID::CaloSample sample,
                  const double eta,
                  const double phi,
                  const CaloDetDescrManager* caloDD) ;

  /** @brief For a given direction, return the middle of the given
     CaloCell_ID::CaloSample. If the direction does not cross any CaloCell, the
     returned bool will be set to False.
   */
  static double middle(const CaloCell_ID::SUBCALO subcalo,
                const int sampling_or_module,
                const bool barrel,
                const double eta,
                const double phi,
                const CaloDetDescrManager* caloDD) ;
  /** @brief For a given direction, return the middle of the given
     CaloCell_ID::CaloSample. If the direction does not cross any CaloCell, the
     returned bool will be set to False.
   */
  static double middle(const CaloCell_ID::CaloSample sample,
                const double eta,
                const double phi,
                const CaloDetDescrManager* caloDD ) ;

  /** @brief For a given direction, return the exit of the given
     CaloCell_ID::CaloSample. If the direction does not cross any CaloCell, the
     returned bool will be set to False.
   */
  static double exit(const CaloCell_ID::SUBCALO subcalo,
              const int sampling_or_module,
              const bool barrel,
              const double eta,
              const double phi,
              const CaloDetDescrManager* caloDD) ;
  /** @brief For a given direction, return the exit of the given
     CaloCell_ID::CaloSample. If the direction does not cross any CaloCell, the
     returned bool will be set to False.
   */
  static double exit(const CaloCell_ID::CaloSample sample,
              const double eta,
              const double phi,
              const CaloDetDescrManager* caloDD) ;

  /**
      @brief If the direction is not specified, returns a "flat" aproximation
     (truely valid for strips only).
   */
  static double flat(const CaloCell_ID::SUBCALO subcalo,
              const int sampling_or_module,
              const bool barrel,
              const int side,
              const CaloDetDescrManager* caloDD) ;
  /**
      @brief If the direction is not specified, returns a "flat" aproximation
     (truely valid for strips only).
   */
  static double flat(const CaloCell_ID::CaloSample sample,
              const int side,
              const CaloDetDescrManager* caloDD) ;

  /**
      @brief If the direction is not specified, returns a "flat" aproximation
     (truely valid for strips only).
    */
  static double depth(const CaloCell_ID::SUBCALO subcalo,
               const int sampling_or_module,
               const bool barrel,
               const int side,
               const CaloDetDescrManager* caloDD) ;
  /**
      @brief If the direction is not specified, returns a "flat" aproximation
     (truely valid for strips only).
    */
  static double depth(const CaloCell_ID::CaloSample sample,
               const int side,
               const CaloDetDescrManager* caloDD ) ;

  /**
      @brief will look for the corresponding CaloDetDescrElement and return its
     entrance radius
    */
  static double get_entrance_radius(CaloCell_ID::CaloSample sample,
                             double eta,
                             double phi,
                             const CaloDetDescrManager* caloDD) ;

  /** @brief will look for the corresponding CaloDetDescrElement and return its
   * entrance z
   */
  static double get_entrance_z(CaloCell_ID::CaloSample sample,
                        double eta,
                        double phi,
                        const CaloDetDescrManager* caloDD ) ;

  /** @brief Same as cscopt with the outermost EMB2 point excluded.
   This improves the fit.
   From CSC12 E=100 GeV photons.
   */
  static double cscopt2_parametrized(const CaloCell_ID::CaloSample sample,
                              const double eta,
                              const double phi,
                              const CaloDetDescrManager* caloDD) ;

  enum class DepthChoice
  {
    egparam,
    cscopt,
    cscopt2,
    TBparam,
    entrance,
    middle,
    flat
  };

private:
  /** @brief Methods taken from the old egammaqdepth class :
      return the parametrised depth used in DC1 and DC2. Only LAREM is foreseen
   */
  static double egparametrized(const CaloCell_ID::SUBCALO subcalo,
                        const int sampling_or_module,
                        const bool barrel,
                        const double eta,
                        const double phi,
                        const CaloDetDescrManager* caloDD) ;
  /** @brief Methods taken from the old egammaqdepth class :
      return the parametrised depth used in DC1 and DC2. Only LAREM is foreseen
   */
  static double egparametrized(const CaloCell_ID::CaloSample sample,
                        const double eta,
                        const double phi,
                        const CaloDetDescrManager* caloDD) ;

  /** @brief Parametrisation taken from the old egammaqdepth class, but adapted
     for TestBeam. Only LAREM is foreseen
   */
  static double TBparametrized(const CaloCell_ID::SUBCALO subcalo,
                        const int sampling_or_module,
                        const bool barrel,
                        const double eta,
                        const double phi,
                        const CaloDetDescrManager* caloDD) ;
  /** @brief Parametrisation taken from the old egammaqdepth class, but adapted
     for TestBeam. Only LAREM is foreseen
   */
  static double TBparametrized(const CaloCell_ID::CaloSample sample,
                        const double eta,
                        const double phi,
                        const CaloDetDescrManager* caloDD) ;

  /** @brief Layers 1 and 2 depths set to give the best eta resolution.
   From CSC12 E=100 GeV photons.
   */
  static double cscopt_parametrized(const CaloCell_ID::CaloSample sample,
                             const double eta,
                             const double phi,
                             const CaloDetDescrManager* caloDD) ;

  /** @brief will look for the corresponding CaloDetDescrElement and return its
   * middle radius
   */
  static double get_middle_radius(CaloCell_ID::CaloSample sample,
                           double eta,
                           double phi,
                           const CaloDetDescrManager* caloDD) ;

  /** @brief will look for the corresponding CaloDetDescrElement and return its
   * middle z
   */
  static double get_middle_z(CaloCell_ID::CaloSample sample,
                      double eta,
                      double phi,
                      const CaloDetDescrManager* caloDD) ;

  /** @brief will look for the corresponding CaloDetDescrElement and return its
   * middle radius
   */
  static double get_exit_radius(CaloCell_ID::CaloSample sample,
                         double eta,
                         double phi,
                         const CaloDetDescrManager* caloDD) ;

  /** @brief will look for the corresponding CaloDetDescrElement and return its
   * middle z
   */
  static double get_exit_z(CaloCell_ID::CaloSample sample,
                    double eta,
                    double phi,
                    const CaloDetDescrManager* caloDD) ;

protected:
  CaloDepthTool();                                //> not implemented
  CaloDepthTool(const CaloDepthTool&);            //> not implemented
  CaloDepthTool& operator=(const CaloDepthTool&); //> not implemented

private:
  /**
   * Depth choice related code
   * User passes a string but we convert to an enum
   * at init to avoid string comparisons
   */

  Gaudi::Property<std::string> m_depth_choice{ this, "DepthChoice", "", "depth choice" };
  
  DepthChoice m_depthChoice = DepthChoice::entrance;
  /** ID Helper
   */
  const CaloCell_ID* m_calo_id;
  /** Phi range helper object
   */
  CaloPhiRange m_range;
};

#endif // CALODETDESCR_CALODEPTHTOOL_H

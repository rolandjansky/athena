// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloFillRectangularCluster.h,v 1.7 2009-04-25 17:57:00 ssnyder Exp $
/**
 * @file  CaloFillRectangularCluster.h
 * @author scott snyder <snyder@bnl.gov>, D. Lelas, H. Ma, S. Rajagopalan
 * @date March, 2006
 * @brief Calculates the per-layer position, size, etc. of a cluster.
 *        Optionally, fills the cluster with cells from StoreGate.
 *
 * Given a cluster with a specified @f$\eta, \phi@f$ seed location,
 * we draw rectangular windows in each layer, and select cells
 * within those windows.  The cells can either be already associated
 * with the cluster or taken from a container in StoreGate, depending
 * on the setting of the fill_cluster option.  In the latter case,
 * the selected cells will be added to the cluster.  The widths
 * of the windows in each layer are set via variable assignments
 * in the constructor.
 *
 * For each layer, the position, size, etc. are calculated
 * (using CaloUtils/CaloLayerCalculator) and stored in the cluster.
 *
 * The behavior may also be changed via subclassing.  Derived classes
 * may change the window sizes set up in the constructor, and may also
 * change the definition of the seed location via the @c get_seed
 * virtual function.
 *
 * This derives from the earlier version @c CaloComputeEMlayers.
 */


#ifndef CALOCLUSTERCORRECTION_CALOFILLRECTANGULARCLUSTER_H
#define CALOCLUSTERCORRECTION_CALOFILLRECTANGULARCLUSTER_H


#include "CaloClusterCorrection/CaloClusterCorrection.h"
#include "CaloInterface/ISetCaloCellContainerName.h"
#include <string>
#include "AthenaKernel/IOVSvcDefs.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ThreadLocalContext.h"
class CaloDetDescrManager;

// Helper object used for the per-sampling calculations.
namespace CaloClusterCorr {
class SamplingHelper;
}
//class CaloCluster;
class CaloCell;
class CaloCellContainer;


class CaloFillRectangularCluster
  : public CaloClusterCorrection,
    public ISetCaloCellContainerName
{
public:
  /**
   * @brief Standard Gaudi constructor.
   * @param type The type of the tool.
   * @param name The name of the tool.
   * @param parent The parent algorithm of the tool.
   */
  CaloFillRectangularCluster(const std::string& type,
                             const std::string& name,
                             const IInterface* parent);

  /**
   * @brief Standard Gaudi initialize method.
   *
   * Derived classes can extend this to change the sampling window sizes.
   */
  virtual StatusCode initialize() override;

  /**
   * @brief CaloClusterCorrection virtual method
   * @param myctx   ToolWithConstants context.
   * @param cluster The cluster on which to operate.
   */
  virtual void makeCorrection (const Context& myctx,
                               xAOD::CaloCluster* cluster) const override;


  // Alternate version that takes an EventContext.
  void makeCorrection (const EventContext& ctx,
                       xAOD::CaloCluster* cluster) const
  {
    return makeCorrection (context(ctx), cluster);
  }


  /*
   * @brief Return the seed position of a cluster.
   * @param helper Sampling calculation helper object.
   * @param cluster The cluster on which to operate.
   * @param[out] eta The @f$\eta@f$ location of the cluster seed.
   * @param[out] phi The @f$\phi@f$ location of the cluster seed.
   *
   * The cluster seed is the center of rectangular cluster windows.
   * This may be overridden by derived classes to change the seed definition.
   */
  virtual void get_seed (CaloClusterCorr::SamplingHelper& helper,
                         const xAOD::CaloCluster* cluster,
                         double& eta, double& phi) const;


  /**
   * @brief Change the name of the CaloCellContainer used by this tool.
   * @param name The new container name.
   */
  virtual StatusCode setCaloCellContainerName (const std::string& name) override;


  /// Holds the per-layer window sizes.
  typedef std::array<std::pair<double, double>, 4> WindowArray_t;


  /** 
   * @brief Set up layer-by-layer cluster window sizes.
   * @param neta Cluster eta size.
   * @param nphi Cluster phi size.
   * @param detas2 Middle layer cell eta size.
   * @param detas2 Middle layer cell phi size.
   *
   * Returns per-layer array of deta,dphi pairs.
   */
  virtual
  WindowArray_t initWindows (const int neta,
                             const int nphi,
                             const double detas2,
                             const double dphis2) const;



private:

  /// This isn't allowed.
  CaloFillRectangularCluster() = delete;


  /*
   * @brief Actually make the correction for one region (barrel or endcap).
   * @param ctx     The event context.
   * @param helper Sampling calculation helper object.
   * @param eta The @f$\eta@f$ seed of the cluster.
   * @param phi The @f$\phi@f$ seed of the cluster.
   * @param samplings List of samplings for this region.
   */
  void makeCorrection1 (const EventContext& ctx,
                        const CaloDetDescrManager& dd_man,
                        CaloClusterCorr::SamplingHelper& helper,
                        double eta,
                        double phi,
                        const CaloSampling::CaloSample samplings[4]) const;


  /*
   * @brief Execute the correction, given a helper object.
   * @param ctx     The event context.
   * @param helper Sampling calculation helper object.
   */
  void makeCorrection2 (const EventContext& ctx,
                        const CaloDetDescrManager& dd_man,
                        CaloClusterCorr::SamplingHelper& helper) const;


protected:
  /// Cell window sizes in each sampling.
  double m_deta0, m_deta1, m_deta2, m_deta3;
  double m_dphi0, m_dphi1, m_dphi2, m_dphi3;

  /// @f$\eta\times\phi@f$ cluster size.  These are properties.
  int m_neta, m_nphi;

  /// Fill-cluster flag.  This is a property.
  /// If true, then we fill the cluster with cells from StoreGate.
  /// Otherwise, we use the cells already in the cluster.
  bool m_fill_cluster;


  /// Property to tell if the raw energy, eta0 and phi0 should 
  /// be saved as uncalibrated signal state 
  bool m_setRawState;

private:
  /// The StoreGate key for the container of our input cells.
  /// This is a property.
  SG::ReadHandleKey<CaloCellContainer> m_cellsName;
};

#endif // not CALOCLUSTERCORRECTION_CALOFILLRECTANGULARCLUSTER_H

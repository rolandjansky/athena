/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT2CALOCALIBRATION_EGAMMASSHAPECALIBRATION
#define TRIGT2CALOCALIBRATION_EGAMMASSHAPECALIBRATION

#include "TrigT2CaloCalibration/IEgammaCalibration.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/MsgStream.h"
#include "CaloUtils/ToolWithConstants.h"
#include "CaloConditions/Array.h"
//#include "TrigT2CaloCommon/Calo_Def.h"
#include "CaloClusterCorrection/CaloClusterCorrectionCommon.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include <string>

//class TrigEMCluster;

//--------------------------------------------
// class EgammaSshapeCalibration
//--------------------------------------------
class EgammaSshapeCalibration :
  public extends<CaloUtils::ToolWithConstants<AthAlgTool>,
                 IEgammaCalibration>          
{  
 public:

  /// Inherit constructor.
  using base_class::base_class;
  
  /** Initialization of the tool */
  virtual StatusCode initialize() override;
  
  /** Finalization of the tool */
  virtual StatusCode finalize() override;
  
  /** Virtual function from IEgammaCalibration */
  virtual void makeCorrection(xAOD::TrigEMCluster*, const void*) override;

 private:
  void docalc(int,
	      const CaloClusterCorrectionCommon::TableBuilder&,
	      const CxxUtils::Array<1>&, 
	      CxxUtils::WritableArray<2>&,
	      int&);
  
 private:
  
  class Builder : public CaloClusterCorrectionCommon::TableBuilder {
  public:
    /// Constructor.  Gets the parent correction object,
    /// the abs(eta) at which the correction is being
    /// evaluated (in cal-local coordinates), the fractional
    /// cell offset u, and the index of the region for this
    /// correction.
    Builder(const EgammaSshapeCalibration&,double, double, int);
    
    /// Calculate the correction for tabulated energy ENERGY_NDX.
    virtual float calculate (int energy_ndx, bool& good) const;
    
  private:
    float calc0 (float, float, const CaloRec::Array<2>&) const;
    float calc3 (float, float, const CaloRec::Array<2>&) const;
    float calc4 (float, float, const CaloRec::Array<2>&) const;
    float calc5 (float, float, const CaloRec::Array<2>&) const;
    float calc10(float, float, const CaloRec::Array<2>&) const;
    float calc11(float, float, const CaloRec::Array<2>&) const;
    
  private:
    const  EgammaSshapeCalibration& m_corr;
    double m_aeta;
    double m_u;
    int m_region_ndx;
    int m_form;
  };
  friend class Builder;

 private:
  MsgStream* m_log;
  
  /// Calibration constant: tabulated arrays of function parameters.
  /// Index 0: energy
  /// Index 1: region
  /// Index 2: parameter
  /// Index 3: polynomial coefficient
  ///
  /// Note: If [e][r][0][-1] (where -1 means the last in the array)
  /// is exactly 0, for any e and r, then this energy/region is skipped.
  /// (This is usually because insufficient statistics were available
  /// to get a good fit.)
  Constant<CxxUtils::Array<4> > m_correction { this, "correction" };
  
  /// Calibration constant: table of regions.  For each region, we have:
  ///  - Lower @f$|\eta|@f$ for the region.
  ///  - Upper @f$|\eta|@f$ for the region.
  ///  - Index of the functional form for this region.
  ///  - Cell size for this region.
  ///  - Offset of cell low edge wrt a multiple of the cell size
  ///    (not currently used).
  ///
  /// If the forms variable is provided, then the functional form is
  /// taken from there instead of from here.
  Constant<CxxUtils::Array<2> > m_regions { this, "regions" };
  enum {
    REG_LO = 0,
    REG_HI = 1,
    REG_FORM = 2,
    REG_CELLSIZE = 3,
    REG_PHASE = 4
  };
  
  /** Table of energies at which the correction was tabulated. */
  Constant<CxxUtils::Array<1> >  m_energies { this, "energies" };
  
  /** Degree of the polynomial interpolation in energy. */
  Constant<int>  m_energy_degree { this, "energy_degree", 3 };
  
  Constant<bool> m_isRange_barrel { this, "rangeBarrel" };

  /// Calibration constant: Functional form to use per region per energy.
  /// If this is empty, the form is taken instead from the region table
  /// (and is thus same for  all energies).
  Constant<CxxUtils::Array<2> > m_forms { this, "forms" };
};

#endif

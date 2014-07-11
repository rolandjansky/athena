/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file ParabolaCscClusterFitter.h
Header file for the Parabola method of positoin reconstruction.
Tool to fit a CSC cluster using adjacent charge ratios.
@author Michael Schernau
May 2009
*/

#ifndef ParabolaCscClusterFitter_H
#define ParabolaCscClusterFitter_H


#include "AthenaBaseComps/AthAlgTool.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "MuonPrepRawData/CscClusterStatus.h"

namespace Muon {
  class CscPrepData;
}
namespace MuonGM {
  class MuonDetectorManager;
}

class CscIdHelper;

enum CscStation { UNKNOWN_STATION, CSS, CSL };
enum CscPlane { CSS_ETA, CSL_ETA, CSS_PHI, CSL_PHI, UNKNOWN_PLANE };




class ParabolaCscClusterFitter : virtual public ICscClusterFitter, public AthAlgTool {
  
public:

  /** Constructor.
      This class is derived from AthAlgTool(type, aname, parent).
      The 3 args are used to initialize the base class.
      declareProperty(...) is used to declare job options.
   */
  ParabolaCscClusterFitter(std::string type, std::string aname, const IInterface* parent);
        
  /** Destructor. */
  ~ParabolaCscClusterFitter();
        
  /** Initialization. */
  StatusCode initialize();

  /** Finalization. */
  StatusCode finalize();

  /** Correction of raw parabola positions.
      @param plane The csc plane enum for small or large chamber, X or Y plane
      @param raw The raw value of the parabola peak positon
      @return The corrected value
   */
  double ParabolaCorrection(CscPlane &plane, double &raw) const;


  // Inherited methods.
  const DataNames& dataNames() const;
  using ICscClusterFitter::fit;
  Results fit(const StripFitList& sfits) const;
  Results fit(const StripFitList& sfits, double dposdz) const;
  double getCorrectedError(const Muon::CscPrepData* pclu, double slope) const;


private:

  // Properties
  /** Max. width of a cluster to be unspoiled. Layer type in array[4] */ 
  std::vector<unsigned int> m_max_width;

  /** error contribution in mm for the tan(theta) track angle correction */
  double m_error_tantheta;
  /** constant to Calculate the angle of incidence.*/
  double m_xtan_css_eta_offset;
  /** constant to Calculate the angle of incidence.*/
  double m_xtan_css_eta_slope;
  /** constant to Calculate the angle of incidence.*/
  double m_xtan_csl_eta_offset;
  /** constant to Calculate the angle of incidence.*/
  double m_xtan_csl_eta_slope;

  /** threshold multiplier  for cluster peak finding */
  double m_multi;

  const MuonGM::MuonDetectorManager* m_detMgr;
  const CscIdHelper* m_cscIdHelper;
};
#endif

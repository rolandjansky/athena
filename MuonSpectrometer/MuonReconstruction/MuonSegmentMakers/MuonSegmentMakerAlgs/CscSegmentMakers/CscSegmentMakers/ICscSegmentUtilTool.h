/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ICscSegmentUtilTool.h

#ifndef ICscSegmentUtilTool_H
#define ICscSegmentUtilTool_H

#include "GaudiKernel/IAlgTool.h"
#include "CscSegmentMakers/ICscSegmentFinder.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Muon {
  class CscClusterOnTrack;
  class MuonSegment;
  class MuonSegmentCombination;
}
  
namespace Trk {
  class PlaneSurface;
  class RIO_OnTrack;
}

class Identifier;

/** Must declare this, with name of interface*/
static const InterfaceID IID_ICscSegmentUtilTool("ICscSegmentUtilTool", 1, 0);

class ICscSegmentUtilTool : virtual public IAlgTool {

public:  // Static methods

  // Return the interface ID.
  static const InterfaceID& interfaceID() { return IID_ICscSegmentUtilTool; }
  
public:  // Interface methods

  // Return the counts of spoiled and unspoiled measurements from a list
  // of RIOS's.
  virtual void spoiled_count(const std::vector<const Trk::RIO_OnTrack*>& rios, double threshold,
                     int& nspoil, int& nunspoil) =0;
  virtual void spoiled_count(const std::vector<const Trk::RIO_OnTrack*>& rios, 
                     int& nspoil, int& nunspoil) =0;
  virtual void spoiled_count(const std::vector<const Trk::RIO_OnTrack*>& rios, 
                     int& nspoil, int& nunspoil, int& spoilmap) =0;
  
  // Fit a RIO residual.
  virtual void fit_rio_residual(const Trk::PlaneSurface& ssrf, bool dump,
                        const std::vector<const Trk::RIO_OnTrack*>& clus, unsigned int irclu,
                        double& res, double& dres,
                        double& rs, double& drs // for 3pt-method
                        ) const=0;

  // Get 2d segments eta/phi seperately
  virtual Muon::MuonSegmentCombination*
  get2dMuonSegmentCombination( Identifier eta_id, Identifier phi_id,
                               ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                               ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                               const Amg::Vector3D& lpos000, int etaStat=0, int phiStat=0 ) const=0;

  virtual Muon::MuonSegmentCombination*
  get4dMuonSegmentCombination(Identifier eta_id, Identifier phi_id,
                              ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                              ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                              const Amg::Vector3D& lpos000 ) const =0;
  
  // Get 2d segments eta/phi seperately
  virtual Muon::MuonSegmentCombination*
  get4dMuonSegmentCombination(const Muon::MuonSegmentCombination* Muon2dSegComb) const=0;  
  
  virtual std::unique_ptr<std::vector<std::unique_ptr<Muon::MuonSegment> > >
  getMuonSegments(Identifier eta_id, Identifier phi_id,
                  ICscSegmentFinder::ChamberTrkClusters& eta_clus,
                  ICscSegmentFinder::ChamberTrkClusters& phi_clus,
                  const Amg::Vector3D& lpos000) const=0;

  virtual bool isGood(uint32_t stripHashId) const=0;


};
#endif

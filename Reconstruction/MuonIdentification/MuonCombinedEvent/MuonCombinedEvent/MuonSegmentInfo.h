/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUONSEGMENTINFO_H
#define MUONCOMBINEDEVENT_MUONSEGMENTINFO_H

#include "TrkParameters/TrackParameters.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODMuon/MuonSegment.h"
#include "AthLinks/ElementLink.h"

namespace Muon{
  class MuonSegment;
}

namespace Trk{
  class Track;
}



namespace MuonCombined {

    struct MuonSegmentInfo {

    MuonSegmentInfo() = default;
      /**pointer to the track object*/
      const Trk::Track* track{nullptr};

      /** pointer to the muon object*/
      const Muon::MuonSegment* segment{nullptr};

      /** track extrapolated to the segment */
      std::shared_ptr<const Trk::TrackParameters> trackAtSegment{nullptr};

      /** element link to xAOD segment */
      ElementLink<xAOD::MuonSegmentContainer> link;

      /** global theta direction track - segment in bending plane */
      double dtheta{-9999.};

      /** global phi direction track - segment in bending plane */
      double dphi{-9999.};

      /** global theta position track - segment in bending plane */
      double dthetaPos{-9999.};

      /** global phi position track - segment in bending plane */
      double dphiPos{-9999.};

      /** residual track - segment in Local coordinate non-bending plane */
      double resX{-9999.};

      /** error from extrapolation on residual in non-bending plane */
      double exErrorX{-9999.};

      /** error from segment on residual in non-bending plane */
      double segErrorX{-9999.};

      /** pull on residual in non-bending plane */
      double pullX{-9999.};

      /** residual track - segment in Local coordinate in bending plane */
      double resY{-9999.};

      /** error from extrapolation on residual in bending plane */
      double exErrorY{-9999.};
  
      /** error from segment on residual in bending plane */
      double segErrorY{-9999.};

      /** pull on residual in bending plane */
      double pullY{-9999.};

      /** covariance  loc Y theta from track in bending plane */
      double exCovYTheta{-9999.};

      /** covariance  loc Y angleYZ from track in bending plane */
      double exCovYZY{-9999.};

      /** angular residual in the Local coordinate non-bending plane */
      double dangleXZ{-9999.};

     /** error from extrapolation on angle in non-bending plane */
      double exErrorXZ{-9999.};

      /** error from segment on angle in non-bending plane */
      double segErrorXZ{-9999.};

      /** pull on angle in non-bending plane */
      double pullXZ{-9999.};

      /** angular residual in the Local coordinate bending plane */
      double dangleYZ{-9999.};

     /** error from extrapolation on angle in bending plane */
      double exErrorYZ{-9999.};

      /** error from segment on angle in bending plane */
      double segErrorYZ{-9999.};

      /** pull on angle in bending plane */
      double pullYZ{-9999.};

      /** residual on combined local position Y and angle YZ  */
      double resCY{-9999.};

      /** pull on combined local position Y and angle YZ  */
      double pullCY{-9999.};

      /** chi2 in the Y position and YZ angle bending and precision plane */
      double chi2Y{-9999.};

     /** Expect phi hits in EI or EM or BM or BO */
      int hasPhi{-1};

      /** hit count summary */
      //Muon::IMuonSegmentHitSummaryTool::HitCounts hitCounts;

      /** segment quality */
      int quality{-1};

      /** station layer */
      int stationLayer{-1};

      /** number of holes */
      int nholes{-1};

      /** number of segments in track */
      int nsegments{-1};

      /** 1 if single ML */
      int singleML{-1};

      /** maximum residual with tube endplugs (positive means outside of the tube */
      double maximumResidualAlongTube{-99999.};

      /** pullChamber =  maximumResidualAlongTube / extrapolationError  < 5 means within chamber at 5.s.d from edges */

      double pullChamber{-99999.};

      /** maximum residual with the phi hits on the segment */
      double maximumResidualPhi{-99999.};

      /** maximum pull with the phi hits on the segment */
      double maximumPullPhi{-99999.};

      /** minimum residual with the phi hits on the segment */
      double minimumResidualPhi{-99999.};

      /** minimum pull with the phi hits on the segment */
      double minimumPullPhi{-99999.};

      /** fitted t0 */
      double t0{-99999.};

     /** RLocY parametrized rejection as a function of locY
 *          parameter is used for selecting best segment */

      double RLocY{-99999.};

    /** RAYZ parametrized rejection as a function of Angle YZ
 *          parameter is/could be used for selecting best segment */

      double RAYZ{-99999.};

    /** selected == 1 if the segment accepted for MuonTagIMO */

      int selected{-1};

  };     
                 
}


#endif


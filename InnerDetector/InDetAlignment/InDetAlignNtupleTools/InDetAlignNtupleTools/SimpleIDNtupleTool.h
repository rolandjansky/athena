/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNNTUPLETOOLS_SIMPLEIDNTUPLETOOL_H
#define INDETALIGNNTUPLETOOLS_SIMPLEIDNTUPLETOOL_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "TrkAlignInterfaces/IFillNtupleTool.h"

/**
	@file SimpleIDNtupleTool.h
	@class SimpleIDNtupleTool

	@brief Tool for writing information to ntuple for debugging.

	@author Daniel Kollar <daniel.kollar@cern.ck>
	@date dd/CLHEP::mm/yyy
*/

class TFile;
class TTree;

namespace Trk
{
	class AlignTrack;
	class ITrackSummaryTool;
	class IAlignModuleTool;
}

namespace InDetDD
{
	class TRT_DetectorManager;
}

class TRT_ID;

namespace InDet
{
	class SimpleIDNtupleTool : virtual public Trk::IFillNtupleTool, public AthAlgTool
	{

		public:
			SimpleIDNtupleTool(const std::string & type, const std::string & name, const IInterface * parent);
			virtual ~SimpleIDNtupleTool();

			StatusCode initialize();
			StatusCode finalize();

			/** fills track information to ntuple */
			void dumpTrack(int itrk, const Trk::AlignTrack * alignTrack);

			/** fills hit information to ntuple */
			void fillHits(const Trk::AlignTrack * alignTrack);

			void storeHitmap();
			void fillHitmap();
			void fillSummary();
			void showStatistics();

			void setNtuple(TFile* ntuple) { m_file=ntuple; }
			StatusCode fillNtuple();

		private:

			void initializeNtuple();

			ToolHandle <Trk::ITrackSummaryTool>         m_trackSumTool;    //!< Pointer to track summary tool
			ToolHandle <Trk::IAlignModuleTool>          m_alignModuleTool; //!< Pointer to AlignmModuleTool

			const InDetDD::TRT_DetectorManager * m_trtDetManager; //!< Pointer to trt detector manager

			const TRT_ID * m_idHelper;

			TFile * m_file;
			TTree * m_tree;

			//std::string m_filename; //!< name of ntuple file
			//std::string m_filepath;  //!< path to ntuple file

			// Reconstructed variables
			int      m_runNumber;
			int      m_evtNumber;

			// Track information

			/** x coord of track perigee (xvtx in ntuple)  */
			double m_xvtx;

			/** y coord of track perigee (yvtx in ntuple)  */
			double m_yvtx;

			/** z coord of track perigee (zvtx in ntuple)  */
			double m_zvtx;

			/** d0 perigee parameter of reconstructed track (d0 in ntuple)  */
			double m_d0;

			/** z0 perigee parameter of reconstructed track (z0 in ntuple)  */
			double m_z0;

			/** phi perigee parameter of reconstructed track (phi0 in ntuple)  */
			double m_phi0;

			/** theta perigee parameter of reconstructed track (theta in ntuple)  */
			double m_theta;

			/** Q over p perigee parameter of reconstructed track (qoverp in ntuple)  */
			double m_qoverp;

			/** pt perigee parameter of reconstructed track (pt in ntuple)  */
			double m_pt;

			/** eta of perigee of reconstructed track (eta in ntuple)  */
			double m_eta;

			/** Chi2 of the track fit (chi2 in ntuple)  */
			double m_chi2;

			/** number of degrees of freedom of the track fit (ndof in ntuple)  */
			int m_ndof;

			/** Chi2 probability of the track fit (chi2prob in ntuple)  */
			double m_chi2prob;

			// Hit information

			/** number of hits on track [including scattering centers] (nhits in ntuple) */
			int m_nhits;

			/** number of TRT hits on track (ntrthits in ntuple) */
			int m_nhitstrt;

			/** number of SCT hits on track (nscthits in ntuple) */
			int m_nhitssct;

			/** number of Pixel hits on track (ntrthits in ntuple) */
			int m_nhitspix;

			/** number of shared hits on track (nshared in ntuple) */
			int m_nshared;

			/** number of shared SCT hits on track (nshsct in ntuple) */
			int m_nshsct;

			/** number of shared Pixel hits on track (nshpix in ntuple) */
			int m_nshpix;

			/** number of holes on track (nholes in ntuple) */
			int m_nholes;

			/** number of SCT holes on track (nhsct in ntuple) */
			int m_nhsct;

			/** number of Pixel holes on track (nhpix in ntuple) */
			int m_nhpix;

			/** radius of hit (hit_rho in ntuple) */
			double * m_hit_rho;

			/** phi angle of hit (hit_phi in ntuple) */
			double * m_hit_phi;

			/** z coord of hit (hit_z in ntuple) */
			double * m_hit_z;

			/** cot(theta) of hit (hit_cotth in ntuple) */
			double * m_hit_cotth;

			/** eta angle of hit (hit_eta in ntuple) */
			double * m_hit_eta;

			/** local x coord of hit (hit_xloc in ntuple) */
			double * m_hit_xloc;

			/** local y coord of hit (hit_yloc in ntuple) */
			double * m_hit_yloc;

			/** local z coord of hit (hit_zloc in ntuple) */
			double * m_hit_zloc;

			/** drift distance to wire for hit (hit_drd in ntuple) */
			double * m_hit_drd;

			/** hit error (hit_sig in ntuple) */
			double * m_hit_sig;

			double * m_hit_resx;     //!< X-residual of the hit
			double * m_hit_resx_err; //!< X-residual error of the hit
			double * m_hit_resy;     //!< Y-residual of the hit
			double * m_hit_resy_err; //!< Y-residual error of the hit

			double * m_hit_tpar_x;   //!< track parameter locX
			double * m_hit_tpar_y;   //!< track parameter locY

			int * m_hit_det;         //!< detector type for the hit
			int * m_hit_type;        //!< hit type

			double * m_hit_derivx_transx;  //!< derivatives of X-residual wrt. translation X
			double * m_hit_derivx_transy;  //!< derivatives of X-residual wrt. translation Y
			double * m_hit_derivx_transz;  //!< derivatives of X-residual wrt. translation Y
			double * m_hit_derivx_rotx;    //!< derivatives of X-residual wrt. rotation X
			double * m_hit_derivx_roty;    //!< derivatives of X-residual wrt. rotation Y
			double * m_hit_derivx_rotz;    //!< derivatives of X-residual wrt. rotation Y
			double * m_hit_derivx_bowx;    //!< derivatives of X-residual wrt. rotation X
			double * m_hit_derivx_bowy;    //!< derivatives of X-residual wrt. rotation Y
			double * m_hit_derivx_bowz;    //!< derivatives of X-residual wrt. rotation Y
			double * m_hit_derivy_transx;  //!< derivatives of Y-residual wrt. translation X
			double * m_hit_derivy_transy;  //!< derivatives of Y-residual wrt. translation Y
			double * m_hit_derivy_transz;  //!< derivatives of Y-residual wrt. translation Y
			double * m_hit_derivy_rotx;    //!< derivatives of Y-residual wrt. rotation X
			double * m_hit_derivy_roty;    //!< derivatives of Y-residual wrt. rotation Y
			double * m_hit_derivy_rotz;    //!< derivatives of Y-residual wrt. rotation Y
			double * m_hit_derivy_bowx;    //!< derivatives of Y-residual wrt. rotation X
			double * m_hit_derivy_bowy;    //!< derivatives of Y-residual wrt. rotation Y
			double * m_hit_derivy_bowz;    //!< derivatives of Y-residual wrt. rotation Y

			int m_totaltrks;
			int m_totalhits;
			int m_max_hits; //!< max number of hits on track stored in ntuple

			bool m_storeDerivatives;
	}; // end class
  
} // end namespace

#endif // INDETALIGNNTUPLETOOLS_SIMPLEIDNTUPLETOOL_H

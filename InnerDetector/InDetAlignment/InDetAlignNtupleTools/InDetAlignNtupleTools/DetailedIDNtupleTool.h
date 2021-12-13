/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNNTUPLETOOLS_DETAILEDIDNTUPLETOOL_H
#define INDETALIGNNTUPLETOOLS_DETAILEDIDNTUPLETOOL_H

#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/ITruthToTrack.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrkAlignInterfaces/IFillNtupleTool.h"
//#include "TrkParameters/MeasuredPerigee.h"
#include  "TrkParameters/TrackParameters.h"


/**
	@Tool for writing information to ntuple for debugging.
	@author Jike Wang <jike.wang@cern.ck>
	@date dd/CLHEP::mm/yyy
*/

class TFile;
class TTree;

namespace Trk
{
	class AlignTrack;
	class ITrackSummaryTool;
	class IAlignModuleTool;
  class Track; 
  //class TrackParameters; <- typedef, cant fw declare
}

namespace InDetDD
{
	class TRT_DetectorManager;
}



class TRT_ID;

namespace InDet
{
	class DetailedIDNtupleTool : virtual public Trk::IFillNtupleTool, public AthAlgTool
	{

		public:
			DetailedIDNtupleTool(const std::string & type, const std::string & name, const IInterface * parent);
			virtual ~DetailedIDNtupleTool();

			StatusCode initialize();
			StatusCode finalize();

			/** fills track information to ntuple */
			void dumpTrack(int itrk, const Trk::AlignTrack * alignTrack);

			void storeHitmap();
			void fillHitmap();

			void fillSummary();
			void showStatistics();

			void setNtuple(TFile* ntuple) { m_file=ntuple; }
			StatusCode fillNtuple();

		private:

			bool retrieveTruthInfo(const Trk::AlignTrack* trk);
			const Trk::TrackParameters* perigeeParameter(const Trk::AlignTrack* track) const;
			void initializeNtuple();

			ToolHandle <Trk::ITrackSummaryTool>         m_trackSumTool;    //!< Pointer to track summary tool
			ToolHandle <Trk::IAlignModuleTool>          m_alignModuleTool; //!< Pointer to AlignmModuleTool
			PublicToolHandle <Trk::ITruthToTrack>             m_truthToTrack
			   {this,"TruthToTrack","Trk::TruthToTrack/InDetTruthToTrack",""};    //!< the truth to track Tool
			PublicToolHandle <Trk::IExtrapolator>             m_extrapolator
			   {this,"Extrapolator","Trk::Extrapolator/AtlasExtrapolator",""};    //!< track extrapolator

			bool m_storeNormalRefittedOnly;     //!< only store the AlignTrack which is normally refitted
			bool m_storeConstrainedOnly;       //!< onlystore the AlignTrack which is normally refitted
			bool m_storeTruth;                  //!< retrieve the truth information
			double m_matchProbability;          //!< the probabililty cut in the truth matching

			TFile * m_file;
			TTree * m_tree;

			std::string m_filename;        //!< name of ntuple file
			std::string m_filepath;        //!< path to ntuple file

			std::string m_trackCollection; //!
			std::string m_tracksTruthName; //!


			// Reconstructed variables
			int      m_runNumber = 0;
			int      m_evtNumber = 0;


			// original track information
			double m_original_xvtx = 0.0;
			double m_original_yvtx = 0.0;
			double m_original_zvtx = 0.0;

			double m_original_d0 = 0.0;
			double m_original_z0 = 0.0;
			double m_original_phi0 = 0.0;
			double m_original_theta = 0.0;
			double m_original_qoverp = 0.0;
			double m_original_pt = 0.0;

			double m_original_eta = 0.0;
			double m_original_chi2 = 0.0;
			int    m_original_ndof = 0;
			double m_original_chi2prob = 0.0;

			double m_original_err_d0 = 0.0;
			double m_original_err_z0 = 0.0;
			double m_original_err_phi0 = 0.0;
			double m_original_err_theta = 0.0;
			double m_original_err_qoverp = 0.0;

			// truth informaiton
			double m_truth_d0 = 0.0;
			double m_truth_z0 = 0.0;
			double m_truth_phi0 = 0.0;
			double m_truth_theta = 0.0;
			double m_truth_eta = 0.0;
			double m_truth_pt = 0.0;
			double m_truth_qoverp = 0.0;
			double m_truth_qoverpt = 0.0;
			double m_truth_charge = 0.0;
			double m_truth_prod_x = 0.0;
			double m_truth_prod_y = 0.0;
			double m_truth_prod_z = 0.0;

			// oroginal track wrt reference points
			double m_original_toRef_d0 = 0.0;
			double m_original_toRef_z0 = 0.0;
			double m_original_toRef_phi0 = 0.0;
			double m_original_toRef_theta = 0.0;
			double m_original_toRef_qoverp = 0.0;

			// refitted track wrt reference points
			double m_toRef_d0 = 0.0;
			double m_toRef_z0 = 0.0;
			double m_toRef_phi0 = 0.0;
			double m_toRef_theta = 0.0;
			double m_toRef_qoverp = 0.0;

			// the AlignTrack information
			double m_err_d0 = 0.0;
			double m_err_z0 = 0.0;
			double m_err_phi0 = 0.0;
			double m_err_theta = 0.0;
			double m_err_qoverp = 0.0;

			double m_xvtx = 0.0;
			double m_yvtx = 0.0;
			double m_zvtx = 0.0;
			double m_d0 = 0.0;
			double m_z0 = 0.0;
			double m_phi0 = 0.0;
			double m_theta = 0.0;
			double m_qoverp = 0.0;
			double m_pt = 0.0;
			double m_eta = 0.0;
			double m_chi2 = 0.0;
			int    m_ndof = 0;
			double m_chi2prob = 0.0;

	}; // end class
  
} // end namespace

#endif // INDETALIGNNTUPLETOOLS_DETAILEDIDNTUPLETOOL_H

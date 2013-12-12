/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNNTUPLETOOLS_DETAILEDIDNTUPLETOOL_H
#define INDETALIGNNTUPLETOOLS_DETAILEDIDNTUPLETOOL_H

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
        class IExtrapolator;
	class IAlignModuleTool;
        class Track; 
        class ITruthToTrack;
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
			ToolHandle <Trk::ITruthToTrack>             m_truthToTrack;    //!< the truth to track Tool
			ToolHandle <Trk::IExtrapolator>             m_extrapolator;    //!< track extrapolator

			const TRT_ID * m_idHelper;

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
			int      m_runNumber;
			int      m_evtNumber;


			// original track information
			double m_original_xvtx;
			double m_original_yvtx;
			double m_original_zvtx;

			double m_original_d0;
			double m_original_z0;
			double m_original_phi0;
			double m_original_theta;
			double m_original_qoverp;
			double m_original_pt;

			double m_original_eta;
			double m_original_chi2;
			int    m_original_ndof;
			double m_original_chi2prob;

			double m_original_err_d0;
			double m_original_err_z0;
			double m_original_err_phi0;
			double m_original_err_theta;
			double m_original_err_qoverp;

			// truth informaiton
			double m_truth_d0;
			double m_truth_z0;
			double m_truth_phi0;
			double m_truth_theta;
			double m_truth_eta;
			double m_truth_pt;
			double m_truth_qoverp;
			double m_truth_qoverpt;
			double m_truth_charge;
			double m_truth_prod_x;
			double m_truth_prod_y;
			double m_truth_prod_z;

			// oroginal track wrt reference points
			double m_original_toRef_d0;
			double m_original_toRef_z0;
			double m_original_toRef_phi0;
			double m_original_toRef_theta;
			double m_original_toRef_qoverp;

			// refitted track wrt reference points
			double m_toRef_d0;
			double m_toRef_z0;
			double m_toRef_phi0;
			double m_toRef_theta;
			double m_toRef_qoverp;

			// the AlignTrack information
			double m_err_d0;
			double m_err_z0;
			double m_err_phi0;
			double m_err_theta;
			double m_err_qoverp;

			double m_xvtx;
			double m_yvtx;
			double m_zvtx;
			double m_d0;
			double m_z0;
			double m_phi0;
			double m_theta;
			double m_qoverp;
			double m_pt;
			double m_eta;
			double m_chi2;
			int    m_ndof;
			double m_chi2prob;

	}; // end class
  
} // end namespace

#endif // INDETALIGNNTUPLETOOLS_DETAILEDIDNTUPLETOOL_H

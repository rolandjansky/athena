/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _NtupleResidualTool_H
#define _NtupleResidualTool_H

//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

//root 
class TH2F;
class TH1F;
class TFile;
#include "TTree.h"

namespace MuonCalib {


class QuasianalyticLineReconstruction;

/** @class NtupleResidualTool
	Interface tool for the MT T0-Fitter
	@author rauscher@cern.ch
	*/
	
class NtupleResidualTool: public AthAlgTool, virtual public NtupleCalibrationTool
	{
	public:
//==============================================================================
	/** Tool Constructor*/
		NtupleResidualTool(const std::string& t, const std::string& n, const IInterface* p);
	/** tool initialization */
		StatusCode initialize();
	/** tool finalization */
		StatusCode finalize();
	/**analyse event - fill hits into specra*/
		StatusCode handleEvent(const MuonCalibEvent &/*event*/, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
	/** we dont need to keep the segments*/
		inline bool KeepSegments() const
			{
			return false;
			}
//==============================================================================
	private:
		TH2F *m_residual_distribution;
		TH1F *m_r_spec;
		TFile *m_outfile;
		TH1F *m_chi2;
		TH1F *m_nhits;
	//quasianalytic fitter
		QuasianalyticLineReconstruction *m_fitter;
		
	};

}


#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _NtupleResidualVsTimeTool_H
#define _NtupleResidualVsTimeTool_H

//this
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"


//root 
class TH2F;
class TFile;
#include <map>

namespace MuonCalib {


class IMdtPatRecFitter;

/** @class NtupleResidualVsTimeTool
	Interface tool for the MT T0-Fitter
	@author rauscher@cern.ch
	*/
	
class NtupleResidualVsTimeTool: public AthAlgTool, virtual public NtupleCalibrationTool
	{
	public:
//==============================================================================
	/** Tool Constructor*/
		NtupleResidualVsTimeTool(const std::string& t, const std::string& n, const IInterface* p);
	/** tool initialization */
		StatusCode initialize();
	/** tool finalization */
		StatusCode finalize();
	/**analyse event - fill hits into specra*/
		StatusCode handleEvent(const MuonCalibEvent &event, int /*evnt_nr*/, const std::vector<MuonCalibSegment *> &segments, unsigned int position);
	/** we dont need to keep the segments*/
		inline bool KeepSegments() const
			{
			return false;
			}
//==============================================================================
	private:
		bool m_curved_segments;
		int m_min_hits;
		std::map<std::pair<unsigned int, unsigned int> ,TH2F *> m_residual_distribution;
		TH2F *m_last_residual_distribution;
		TFile *m_outfile;
	//quasianalytic fitter
		IMdtPatRecFitter *m_fitter;
		std::pair<unsigned int, unsigned int> m_last_lumiblock;
		
	};

}


#endif

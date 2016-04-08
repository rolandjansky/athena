/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NtupleDbCheckTool_h
#define NtupleDbCheckTool_h

//root
#include "TTree.h"
#include "TFile.h"

//MuonCalibStandAloneBase
#include "MuonCalibStandAloneBase/NtupleCalibrationTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
//MdtCalibIoSvc
class MdtCalibInputSvc;

namespace MuonCalib {


class NtupleDbCheckTool : public AthAlgTool, virtual public NtupleCalibrationTool
	{
	public:
	//contructor
		NtupleDbCheckTool(const std::string & t,
const std::string & n, const IInterface * p);
		~NtupleDbCheckTool();
		
	//initialize - finalize
		StatusCode initialize(void);
		StatusCode finalize(void);
	//handle event
		StatusCode handleEvent(const MuonCalibEvent & /*event*/, int /*evnt_nb*/, const std::vector<MuonCalibSegment *> & segments, unsigned int position);
	//keep seg
		inline bool KeepSegments() const
			{
			return false;
			}
	
	private:
		TFile * m_outfile;
		TTree * m_outtree;
		Int_t m_station, m_eta, m_phi;
		Double_t m_ntuple_t0, m_calib_t0;
		Double_t m_ntuple_r, m_calib_r;
		
	//calibration input service
		ServiceHandle<MdtCalibInputSvc> m_input_service;
	}; //class


} //namespace MuonCalib

#endif

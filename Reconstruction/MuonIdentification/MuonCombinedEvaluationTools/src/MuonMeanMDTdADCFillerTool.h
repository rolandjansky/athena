///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonMeanMDTdADCFillerTool.h,  Header file for class MuonMeanMDTdADCFillerTool
/////////////////////////////////////////////////////////////////// 

#ifndef MUONCOMBINEDEVALUATIONTOOLS_MUONMEANMDTDADCFILLERTOOL_H
#define MUONCOMBINEDEVALUATIONTOOLS_MUONMEANMDTDADCFILLERTOOL_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonMeanMDTdADCFiller.h"

#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"

// class IIncidentSvc;
namespace Trk {}

namespace Rec
{

   /** @class MuonMeanMDTdADCFillerTool
      @brief return mean Number of ADC counts for MDT tubes on the track
    */

	class MuonMeanMDTdADCFillerTool : public AthAlgTool,
			   virtual public IMuonMeanMDTdADCFiller
    {

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  MuonMeanMDTdADCFillerTool(const std::string& type, 
                            const std::string& name,
                            const IInterface* parent);
  virtual ~MuonMeanMDTdADCFillerTool(void); 

    StatusCode		initialize();
    StatusCode		finalize();

    /** return mean Number of ADC counts for MDT tubes on the track of muon (method will simply step down to the relevant track)*/
    double     meanMDTdADCFiller (const xAOD::Muon& muon) const;

    /** return mean Number of ADC counts for MDT tubes on the track */
    double     meanMDTdADCFiller (const Trk::Track& track) const;
 
 private: 
  
    // tools and services 
	ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
    ToolHandle<Muon::MuonIdHelperTool>  m_idHelperTool;
    ToolHandle<MuonCalib::IIdToFixedIdTool> m_idToFixedIdTool;


  }; 

}	// end of namespace
#endif //> !MUONCOMBINEDEVALUATIONTOOLS_MUONMEANMDTDADCFILLERTOOL_H

///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonMeanMDTdADCFillerTool.h,  Header file for class MuonMeanMDTdADCFillerTool
/////////////////////////////////////////////////////////////////// 

#ifndef MUONCOMBINEDEVALUATIONTOOLS_MUONMEANMDTDADCFILLERTOOL_H
#define MUONCOMBINEDEVALUATIONTOOLS_MUONMEANMDTDADCFILLERTOOL_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonCombinedToolInterfaces/IMuonMeanMDTdADCFiller.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "xAODEventInfo/EventInfo.h"

#include "StoreGate/ReadHandleKey.h"

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
  
    ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper",
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc",
      "Handle to the service providing the IMuonEDMHelperSvc interface" };
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};

  }; 

}	// end of namespace
#endif //> !MUONCOMBINEDEVALUATIONTOOLS_MUONMEANMDTDADCFILLERTOOL_H

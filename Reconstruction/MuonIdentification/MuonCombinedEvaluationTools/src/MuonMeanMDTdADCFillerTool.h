/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// MuonMeanMDTdADCFillerTool.h,  Header file for class MuonMeanMDTdADCFillerTool
/////////////////////////////////////////////////////////////////// 

#ifndef MUONCOMBINEDEVALUATIONTOOLS_MUONMEANMDTDADCFILLERTOOL_H
#define MUONCOMBINEDEVALUATIONTOOLS_MUONMEANMDTDADCFILLERTOOL_H 

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonMeanMDTdADCFiller.h"

namespace Muon {
  class MuonEDMHelperTool;
  class MuonIdHelperTool;
}

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
  ToolHandle<Muon::MuonEDMHelperTool> m_helperTool;
  ToolHandle<Muon::MuonIdHelperTool>  m_idHelperTool;

  }; 

}	// end of namespace
#endif //> !MUONCOMBINEDEVALUATIONTOOLS_MUONMEANMDTDADCFILLERTOOL_H

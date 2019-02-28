/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_MV1TAG_H
#define JETTAGTOOLS_MV1TAG_H

/******************************************************
    @class MV1Tag
    Neural-net tagger combining weights of IP3D, SV1 and 
    JetFitterCombNN
    @ author L. Vacavant
********************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"
#include <string>
#include <map>
#include <list>
#include "TMVA/MethodBase.h"
#include "TMVA/IMethod.h"

namespace TMVA { class Reader; }
namespace Analysis { class CalibrationBroker; }

namespace Analysis { 

  class MV1Tag : public AthAlgTool , virtual public ITagTool {
   
  public:
    MV1Tag(const std::string&,const std::string&,const IInterface*);
      
    /**
       Implementations of the methods defined in the abstract base class
    */
    virtual ~MV1Tag();
    StatusCode initialize();
    StatusCode finalize();
      
    /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
	be part of the JetTag IParticle interface implementation. The trouble with 
	ElementLink and persistency has to be solved for that. Revisit ... */
    void setOrigin(const xAOD::Vertex* priVtx);
      
    StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag, const std::string jetName);

    void finalizeHistos() {};

    /** helper functions to define jet category: */
    int findPtBin(double pt) const;
    int findEtaBin(double eta) const;
    int jetCategory(double pt, double eta) const;
    
  private:      

    std::string m_taggerName;
    std::string m_taggerNameBase; // unique name for regular and flip versions

    /** pointer to calibration in COOL: */
    ToolHandle<CalibrationBroker> m_calibrationTool;

    /** This switch is needed to indicate what to do. The algorithm can be run to produce
	reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
	(m_runModus=1) where already made reference histograms are read.*/ 
    std::string    m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)
      
    /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
    // this pointer does not need to be deleted in the destructor (because it
    // points to something in storegate)
    const xAOD::Vertex* m_priVtx = 0;

    /** reader to define the TMVA algorithms */
    float m_ip3;
    float m_sv1;
    float m_jfc;
    float m_cat;
    std::map<std::string, TMVA::Reader*> m_tmvaReaders;
    std::map<std::string, TMVA::MethodBase*> m_tmvaMethod; 
    std::list<std::string> m_undefinedReaders; // keep track of undefined readers to prevent too many warnings.

    /** to define the input weights */
    std::string m_inputIP3DWeightName;
    std::string m_inputSV1WeightName;
    std::string m_inputJetFitterWeightName;
    std::string m_xAODBaseName;

  }; // End class

  inline void MV1Tag::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx = priVtx; }

} // End namespace 

#endif

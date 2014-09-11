/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_MV2TAG_H
#define JETTAGTOOLS_MV2TAG_H

/******************************************************
    @class MV2Tag
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

namespace TMVA { class Reader; }
namespace Analysis { class CalibrationBroker; }

namespace Analysis { 

  class MV2Tag : public AthAlgTool , virtual public ITagTool {
   
  public:
    MV2Tag(const std::string&,const std::string&,const IInterface*);
      
    /**
       Implementations of the methods defined in the abstract base class
    */
    virtual ~MV2Tag();
    StatusCode initialize();
    StatusCode finalize();
      
    /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
	be part of the JetTag IParticle interface implementation. The trouble with 
	ElementLink and persistency has to be solved for that. Revisit ... */
    void setOrigin(const xAOD::Vertex* priVtx);

    StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag);

    void finalizeHistos() {};

  private:      

    std::string m_taggerName;
    std::string m_taggerNameBase; // unique name for regular and flip versions

    /** pointer to calibration in COOL: */
    ToolHandle<CalibrationBroker> m_calibrationTool;
    bool m_forceMV2CalibrationAlias;
    std::string m_MV2CalibAlias;
    std::string m_MV2cXX;
    std::string m_xAODBaseName;

    /** This switch is needed to indicate what to do. The algorithm can be run to produce
	reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
	(m_runModus=1) where already made reference histograms are read.*/ 
    std::string    m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)

    /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
    // this pointer does not need to be deleted in the destructor (because it
    // points to something in storegate)
    const xAOD::Vertex* m_priVtx;

    /** reader to define the TMVA algorithms */
    
    float m_ip2_pu; 
    float m_ip2_pb; 
    float m_ip2_pc; 
    // IP3D posteriors
    float m_ip3_pu; 
    float m_ip3_pb; 
    float m_ip3_pc; 
    //SV1 posteriors
    float m_sv1_pu; 
    float m_sv1_pb; 
    float m_sv1_pc; 
    //JetFitterCombNN posteriors
    float m_jfc_pu; 
    float m_jfc_pb; 
    float m_jfc_pc; 
    //SV0 informations
    float m_sv0; 
    float m_sv0_ntkv; 
    float m_sv0mass; 
    float m_sv0_efrc; 
    float m_sv0_n2t; 
    float m_sv0_radius; 
    //JetFitter informations
    float m_jf_mass; 
    float m_jf_efrc; 
    float m_jf_n2tv; 
    float m_jf_ntrkv; 
    float m_jf_nvtx; 
    float m_jf_nvtx1t; 
    float m_jf_dphi; 
    float m_jf_deta; 
    //float m_chi2Ondof;
    float m_jf_sig3;

    std::map<std::string, TMVA::Reader*> m_tmvaReaders;//    std::map<std::string, TMVA::Reader*> m_tmvaReaders[10];
    std::list<std::string> m_undefinedReaders; // keep track of undefined readers to prevent too many warnings.


    std::string m_ip2d_infosource;
    std::string m_ip3d_infosource;
    std::string m_sv1_infosource;
    std::string m_sv0_infosource;
    std::string m_jftNN_infosource;
    std::string m_jfprob_infosource;

  }; // End class

  inline void MV2Tag::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx=priVtx; }

} // End namespace 

#endif

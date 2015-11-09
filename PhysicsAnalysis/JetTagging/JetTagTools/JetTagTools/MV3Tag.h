/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_MV3TAG_H
#define JETTAGTOOLS_MV3TAG_H

/******************************************************
    @class MV3Tag
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

namespace Trk  { class VxCandidate; class ITrackToVertexIPEstimator; }
//class Jet;
namespace TMVA { class Reader; }
namespace Analysis { class CalibrationBroker; }

namespace Analysis { 

  class MV3Tag : public AthAlgTool , virtual public ITagTool {
   
  public:
    MV3Tag(const std::string&,const std::string&,const IInterface*);
      
    /**
       Implementations of the methods defined in the abstract base class
    */
    virtual ~MV3Tag();
    StatusCode initialize();
    StatusCode finalize();
      
    /** Set the primary vertex. TODO: This is temporary ! The primary vertex should
	be part of the JetTag IParticle interface implementation. The trouble with 
	ElementLink and persistency has to be solved for that. Revisit ... */
    void setOrigin(const Trk::VxCandidate* priVtx);
      
    void tagJet(xAOD::Jet& jetToTag);    

    void finalizeHistos() {};

  private:      

    std::string m_taggerName;
    std::string m_taggerNameBase; // unique name for regular and flip versions

    /** pointer to calibration in COOL: */
    ToolHandle<CalibrationBroker> m_calibrationTool;
    bool m_forceMV3CalibrationAlias;
    std::string m_MV3CalibAlias;
    std::string m_MV3Flavor;

    /** This switch is needed to indicate what to do. The algorithm can be run to produce
	reference histograms from the given MC files (m_runModus=0) or to work in analysis mode
	(m_runModus=1) where already made reference histograms are read.*/ 
    std::string    m_runModus;          //!< 0=Do not read histos, 1=Read referece histos (analysis mode)

    /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
    // this pointer does not need to be deleted in the destructor (because it
    // points to something in storegate)
    const Trk::VxCandidate* m_priVtx;

    bool m_writeInfoBase; // writes a basic info for each tagger with Pb, Pu (IPInfoBase)

    /** reader to define the TMVA algorithms */
    
    float m_ip2_pu; 
    float m_ip2_pb; 
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
    float m_chi2Ondof;
    float m_jf_sig3;

    // Now the logs
    //float m_logip2_pu; 
    //float m_logip2_pb; 
    // IP3D posteriors
    //float m_logip3_pu; 
    //float m_logip3_pb; 
    //float m_logip3_pc; 
    //SV1 posteriors
    //float m_logsv1_pu; 
    //float m_logsv1_pb; 
    //float m_logsv1_pc; 
    //JetFitterCombNN posteriors
    //float m_logjfc_pu; 
    //float m_logjfc_pb; 
    //float m_logjfc_pc; 
    //SV0 informations
    //float m_logsv0; 
    //float m_logsv0_ntkv; 
    //float m_logsv0mass; 
    //float m_logsv0_efrc; 
    //float m_logsv0_n2t; 
    //float m_logsv0_radius; 
    //JetFitter informations
    //float m_logjf_mass; 
    //float m_logjf_efrc; 
    //float m_logjf_n2tv; 
    //float m_logjf_ntrkv; 
    //float m_logjf_nvtx; 
    //float m_logjf_nvtx1t; 
    //float m_logjf_dphi; 
    //float m_logjf_deta; 
    //float m_logchi2Ondof; 
    //float m_logjf_sig3;

    std::map<std::string, TMVA::Reader*> m_tmvaReaders[10];
    std::list<std::string> m_undefinedReaders; // keep track of undefined readers to prevent too many warnings.

    /** to define the input taggers */
    bool m_flipMV3;

    std::string m_inputSV0WeightName;
    std::string m_ip2d_infosource;
    std::string m_ip3d_infosource;
    std::string m_sv1_infosource;
    std::string m_sv0p_infosource;
    std::string m_jftNN_infosource;
    std::string m_jfcNN_infosource;

  }; // End class

  inline void MV3Tag::setOrigin(const Trk::VxCandidate* priVtx) { m_priVtx=priVtx; }

} // End namespace 

#endif

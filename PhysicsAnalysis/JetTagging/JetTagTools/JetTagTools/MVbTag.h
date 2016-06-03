/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_MVbTAG_H
#define JETTAGTOOLS_MVbTAG_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "JetTagTools/ITagTool.h"
#include "TMVA/MethodBase.h" 
#include "TMVA/IMethod.h"
#include <vector>
#include <string>
#include <map>
#include <list>
#include "TString.h"

//class Jet;
namespace TMVA { class Reader; }
namespace Analysis { class CalibrationBroker; }

namespace Analysis { 

    
class MVbTag : public AthAlgTool , virtual public ITagTool {   
  public:
    MVbTag(const std::string&,const std::string&,const IInterface*);
      
    virtual ~MVbTag();
    StatusCode initialize();
    StatusCode finalize();

    void setOrigin(const xAOD::Vertex* priVtx);
    StatusCode tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag);
      

    void finalizeHistos() {};
    
  private:

    std::string m_taggerName;
    std::string m_taggerNameBase; // unique name for regular and flip versions

    /** pointer to calibration in COOL: */
    ToolHandle<CalibrationBroker> m_calibrationTool;
    bool m_forceMVbCalibrationAlias;
    std::string m_MVbCalibAlias;
    std::string m_MVbFlavor;
    std::string m_xAODBaseName;

    std::string    m_runModus;

    /** Storage for the primary vertex. Can be removed when JetTag provides origin(). */
    // this pointer does not need to be deleted in the destructor (because it
    // points to something in storegate)
    const xAOD::Vertex* m_priVtx;

    //bool m_writeInfoBase; // writes a basic info for each tagger with Pb, Pu (IPInfoBase)

    /// IP based 
    float m_ip2d; 
    float m_ip3d; 
    float m_ngood_trks;
    ///sV based

    float m_sv0; 
    float m_sv0_mass; 
    float m_sv0_efrc; 
    float m_sv0_efrc2; 
    float m_sv0_n2t; 
    float m_sv0_radius; 
    ///JetFitter informations
    float m_jf_mass; 
    float m_jf_efrc; 
    float m_jf_efrc2; 
    float m_jf_ntrkv; 
    float m_jf_deltaR; 
    float m_jf_sig3;
    float m_pt;
    float m_eta;

    std::map<std::string, TMVA::Reader*>  m_tmvaReaders[4];
    std::map<std::string, TMVA::MethodBase*> m_tmvaMethod[4];
    std::list<std::string> m_undefinedReaders;
    // keep track of undefined readers to prevent too many warnings.

    /** to define the input taggers */
    //bool m_flipMVb;

    std::string m_ip2d_infosource;
    std::string m_ip3d_infosource;
    std::string m_sv1_infosource;
    std::string m_sv0_infosource;
    std::string m_jftNN_infosource;
    std::string m_jfprob_infosource;
    
  }; // End class

  inline void MVbTag::setOrigin(const xAOD::Vertex* priVtx) { m_priVtx=priVtx; }

} // End namespace 

#endif

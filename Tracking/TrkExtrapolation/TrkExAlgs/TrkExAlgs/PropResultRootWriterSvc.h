 
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKEXALGS_PROPRESULTROOTWRITERSVC_H
#define TRKEXALGS_PROPRESULTROOTWRITERSVC_H

#include "GaudiKernel/IInterface.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthService.h"

#include "TTree.h"

#include <mutex>

class TFile;

namespace Trk {
  
class PropResultRootWriterSvc : public AthService {
  public:
    /// constructor
    PropResultRootWriterSvc(const std::string& name, ISvcLocator* svcloc);
    /// destructor
    virtual ~PropResultRootWriterSvc();
    
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() {
      /// Declaration of the interface ID ( interface id, major version, minor version)
      static const InterfaceID IID_PropResultRootWriterSvc("PropResultRootWriterSvc", 1, 0);
      return IID_PropResultRootWriterSvc;    
    }
    
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface) override {
      ATH_MSG_DEBUG("in queryInterface()");    
      if (PropResultRootWriterSvc::interfaceID().versionMatch(riid)) {
        ATH_MSG_DEBUG("matched PropResultRootWriterSvc");
        *ppvInterface=(PropResultRootWriterSvc*)this;
      } else {
        ATH_MSG_DEBUG("Default to Service interface");
        return AthService::queryInterface(riid,ppvInterface);
      }
      return StatusCode::SUCCESS;
    }
      
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    
    template <typename T>
    void write(const T* initialPerigee, 
               const T* fwdParameters=nullptr, double fwdtime=std::numeric_limits<float>::quiet_NaN(), 
               const T* bkwParameters=nullptr, double bkwtime=std::numeric_limits<float>::quiet_NaN() );  
      
private:
  std::mutex m_writeMutex;
  
  // jobOptions properties
  ServiceHandle<ITHistSvc> m_thistSvc;
  TTree* m_tree;
  Gaudi::Property<std::string> m_ntupleDirName{this, "DirName", "/ExtrapolationStudies/", ""};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "ATLAS", ""};

  int m_eventNum;
  
  float m_start_d0              ;
  float m_start_z0              ;
  float m_start_phi             ;
  float m_start_theta           ;
  float m_start_qop             ;
                                
  float m_fwd_success           ;
  float m_fwd_time              ;
  float m_fwd_final_l0          ;
  float m_fwd_final_l1          ;
  float m_fwd_final_x           ;
  float m_fwd_final_y           ;
  float m_fwd_final_z           ;
  float m_fwd_final_phi         ;
  float m_fwd_final_theta       ;
  float m_fwd_final_qop         ;
  float m_fwd_final_sigma_l0    ;
  float m_fwd_final_sigma_l1    ;
  float m_fwd_final_sigma_phi   ;
  float m_fwd_final_sigma_theta ;
  float m_fwd_final_sigma_qop   ;
                                
  float m_bkw_success           ;
  float m_bkw_time              ;
  float m_bkw_final_d0          ;
  float m_bkw_final_z0          ;
  float m_bkw_final_phi         ;
  float m_bkw_final_theta       ;
  float m_bkw_final_qop         ;
  float m_bkw_final_sigma_d0    ;
  float m_bkw_final_sigma_z0    ;
  float m_bkw_final_sigma_phi   ;
  float m_bkw_final_sigma_theta ;
  float m_bkw_final_sigma_qop   ;
  
  };
  
  template <typename  T>
  void PropResultRootWriterSvc::write(const T* initialPerigee, 
                                      const T* fwdParameters, double fwdtime, 
                                      const T* bkwParameters, double bkwtime ) {
  
    auto ctx = Gaudi::Hive::currentContext();
  
    m_eventNum = ctx.eventID().event_number();
    
    if (initialPerigee) {
      m_start_d0   = initialPerigee->parameters()[0];
      m_start_z0   = initialPerigee->parameters()[1];
      m_start_phi  = initialPerigee->parameters()[2];
      m_start_theta= initialPerigee->parameters()[3];
      m_start_qop  = initialPerigee->parameters()[4];
    } else {
      m_start_d0   = std::numeric_limits<float>::quiet_NaN();
      m_start_z0   = std::numeric_limits<float>::quiet_NaN();
      m_start_phi  = std::numeric_limits<float>::quiet_NaN();
      m_start_theta= std::numeric_limits<float>::quiet_NaN();
      m_start_qop  = std::numeric_limits<float>::quiet_NaN();
    }

    if (fwdParameters) {
      m_fwd_success     = 1;   
      m_fwd_time        = fwdtime;
      m_fwd_final_l0    = fwdParameters->parameters()[0];
      m_fwd_final_l1    = fwdParameters->parameters()[1];
      m_fwd_final_phi   = fwdParameters->parameters()[2];
      m_fwd_final_theta = fwdParameters->parameters()[3];
      m_fwd_final_qop   = fwdParameters->parameters()[4];
      m_fwd_final_x     = fwdParameters->position()[0];
      m_fwd_final_y     = fwdParameters->position()[1];
      m_fwd_final_z     = fwdParameters->position()[2];
      if (fwdParameters->covariance()) {
        m_fwd_final_sigma_l0    = (*fwdParameters->covariance())(0,0);
        m_fwd_final_sigma_l1    = (*fwdParameters->covariance())(1,1);
        m_fwd_final_sigma_phi   = (*fwdParameters->covariance())(2,2);
        m_fwd_final_sigma_theta = (*fwdParameters->covariance())(3,3);
        m_fwd_final_sigma_qop   = (*fwdParameters->covariance())(4,4);
      } else {        
        m_fwd_final_sigma_l0    = std::numeric_limits<float>::quiet_NaN();
        m_fwd_final_sigma_l1    = std::numeric_limits<float>::quiet_NaN();
        m_fwd_final_sigma_phi   = std::numeric_limits<float>::quiet_NaN();
        m_fwd_final_sigma_theta = std::numeric_limits<float>::quiet_NaN();
        m_fwd_final_sigma_qop   = std::numeric_limits<float>::quiet_NaN();
      }
    } else {
      m_fwd_success           = std::numeric_limits<float>::quiet_NaN();
      m_fwd_time              = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_l0          = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_l1          = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_phi         = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_theta       = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_qop         = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_x           = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_y           = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_z           = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_sigma_l0    = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_sigma_l1    = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_sigma_phi   = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_sigma_theta = std::numeric_limits<float>::quiet_NaN();
      m_fwd_final_sigma_qop   = std::numeric_limits<float>::quiet_NaN();
    }
    
    if (bkwParameters) {
      m_bkw_success     = 1;   
      m_bkw_time        = bkwtime;
      m_bkw_final_d0    = bkwParameters->parameters()[0];
      m_bkw_final_z0    = bkwParameters->parameters()[1];
      m_bkw_final_phi   = bkwParameters->parameters()[2];
      m_bkw_final_theta = bkwParameters->parameters()[3];
      m_bkw_final_qop   = bkwParameters->parameters()[4];
      if (bkwParameters->covariance()) {
        m_bkw_final_sigma_d0    = (*bkwParameters->covariance())(0,0);
        m_bkw_final_sigma_z0    = (*bkwParameters->covariance())(1,1);
        m_bkw_final_sigma_phi   = (*bkwParameters->covariance())(2,2);
        m_bkw_final_sigma_theta = (*bkwParameters->covariance())(3,3);
        m_bkw_final_sigma_qop   = (*bkwParameters->covariance())(4,4);
      } else {        
        m_bkw_final_sigma_d0    = std::numeric_limits<float>::quiet_NaN();
        m_bkw_final_sigma_z0    = std::numeric_limits<float>::quiet_NaN();
        m_bkw_final_sigma_phi   = std::numeric_limits<float>::quiet_NaN();
        m_bkw_final_sigma_theta = std::numeric_limits<float>::quiet_NaN();
        m_bkw_final_sigma_qop   = std::numeric_limits<float>::quiet_NaN();
      }
    } else {
      m_bkw_success            = std::numeric_limits<float>::quiet_NaN();
      m_bkw_time               = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_d0           = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_z0           = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_phi          = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_theta        = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_qop          = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_sigma_d0     = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_sigma_z0     = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_sigma_phi    = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_sigma_theta  = std::numeric_limits<float>::quiet_NaN();
      m_bkw_final_sigma_qop    = std::numeric_limits<float>::quiet_NaN();
    }
    m_tree->Fill();
  }
  
  

}

#endif 

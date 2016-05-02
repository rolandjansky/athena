// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODEGAMMACNV_ELECTRONCNVTOOL_H
#define XAODEGAMMACNV_ELECTRONCNVTOOL_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaParamDefs.h"

#include "xAODEgammaCnv/IElectronCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the tool creating ElectronContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         egamma (Electron) Container into an xAOD::ElectronContainer.
    *
    * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
    *
    * $Revision$
    * $Date$
    */
  class ElectronCnvTool : public AthAlgTool,
			  public virtual IElectronCnvTool {

   public:
    /// Regular AlgTool constructor
    ElectronCnvTool( const std::string& type, const std::string& name,
		     const IInterface* parent );

    /// Function initializing the tool
    virtual StatusCode initialize();

    /// Function that fills an existing xAOD::ElectronContainer (xaod or xaodForward can be NULL)
    virtual StatusCode convert( const egammaContainer* aod,
				xAOD::ElectronContainer* xaod ) const;

    /// Function that fills an existing xAOD::ElectronContainer (xaod or xaodForward can be NULL)
    virtual StatusCode convert( const egammaContainer* aod,
				xAOD::ElectronContainer* xaod,
				xAOD::ElectronContainer* xaodFrwd) const;
  private:
    
    ///@brief  Fills in the shower shape variables 
    void setParameters(const egamma&, xAOD::Electron& ) const;
    void checkAndSetParameter(egammaParameters::ParamDef aodParameter,xAOD::EgammaParameters::ShowerShapeType xaodParameter, const egamma& , xAOD::Electron& ) const;
    
    ///@brief  Fills in the isolation variables 
    void setIsolations(const egamma& aodmuon, xAOD::Electron& xaodmuon) const;
    void checkAndSetIsolation(egammaParameters::ParamDef aodParameter,xAOD::Iso::IsolationType xaodParameter, const egamma&, xAOD::Electron& ) const;
    
    ///@brief  Fills in the track match variables       
    void setTrackMatch(const egamma& aodmuon, xAOD::Electron& xaodmuon) const;
    void checkAndSetTrackMatch(egammaParameters::ParamDef aodParameter,xAOD::EgammaParameters::TrackCaloMatchType xaodParameter, const egamma&, xAOD::Electron& ) const;
    
    ///@brief  Set the element links between the various particles
    void setLinks(const egamma&, xAOD::Electron& ) const;
    ElementLink<xAOD::TrackParticleContainer> getNewLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, const std::string& name) const;

    ///@brief  Fills in some extra derived shower shape variables
    void setDerivedParameters(xAOD::Electron& ) const;    

    ///@brief Other Keys
    std::string   m_inDetTrackParticlesGSF; //!< Location/Key for xAOD::TrackParticles from ID
    std::string   m_inDetTrackParticles   ; //!< Location/Key for xAOD::TrackParticles from ID
    std::string   m_caloClusters;           //!< Location/Key for xAOD::CaloClusters for electrons and photons    
    std::string   m_caloClustersSofte;      //!< Location/Key for xAOD::CaloClusters for softe electrons 
    std::string   m_caloClustersFrwd;       //!< Location/Key for xAOD::CaloClusters for forward electrons
    std::string   m_caloClustersOther;      //!< Location/Key for xAOD::CaloClusters for other (trigger?) electrons
    
  }; // class ElectronCnvTool


} // namespace xAODMaker

#endif // XAODEGAMMACNV_ELECTRONCNVTOOL_H

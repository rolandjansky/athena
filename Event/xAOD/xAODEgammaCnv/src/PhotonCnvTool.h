// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODEGAMMACNV_PHOTONCNVTOOL_H
#define XAODEGAMMACNV_PHOTONCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// EDM include(s):
#include "egammaEvent/egamma.h"
#include "egammaEvent/egammaParamDefs.h"

#include "xAODEgammaCnv/IPhotonCnvTool.h"


namespace xAODMaker {

   /**
    *  @short Implementation for the tool creating PhotonContainer from an AOD
    *
    *         This interface is implemented by the tool that converts the
    *         egamma (Photon) Container into an xAOD::PhotonContainer.
    *
    * @author Jovan Mitrevski <Jovan.Mitrevski@cern.ch>
    *
    * $Revision$
    * $Date$
    */
  class PhotonCnvTool : public AthAlgTool,
			public virtual IPhotonCnvTool {
    
  public:
    /// Regular AlgTool constructor
    PhotonCnvTool( const std::string& type, const std::string& name,
		   const IInterface* parent );
    
    /// Function initializing the tool
    virtual StatusCode initialize();
    
    /// Function that fills an existing xAOD::PhotonContainer
    virtual StatusCode convert( const egammaContainer* aod,
				xAOD::PhotonContainer* xaod) const;
    
  private:
    /// Fills in the shower shape variables 
    void setParameters(const egamma&, xAOD::Photon& ) const ;
    void checkAndSetParameter(egammaParameters::ParamDef aodParameter,xAOD::EgammaParameters::ShowerShapeType xaodParameter, const egamma& , xAOD::Photon& ) const;
      
    /// Fills in the isolation variables 
    void setIsolations(const egamma& aodmuon, xAOD::Photon& xaodmuon) const;
    void checkAndSetIsolation(egammaParameters::ParamDef aodParameter,xAOD::Iso::IsolationType xaodParameter, const egamma&, xAOD::Photon& ) const;
    
    void setLinks(const egamma& aodph, xAOD::Photon& xaodph) const;
    ElementLink<xAOD::VertexContainer> getNewLink(const ElementLink<VxContainer>& oldLink, const std::string& name) const;
    
    
    std::string   m_vertexContainer;     //!< Location/Key for xAOD::Vertex for photons    
    std::string   m_caloClusters;        //!< Location/Key for xAOD::CaloClusters for electrons and photons    
    std::string   m_caloClustersTopo;    //!< Location/Key for xAOD::CaloClusters for topo seeded photons
    std::string   m_caloClustersOther;      //!< Location/Key for xAOD::CaloClusters for other (trigger?) electrons
    
    
  }; // class PhotonCnvTool
  
} // namespace xAODMaker

#endif // XAODEGAMMACNV_PHOTONCNVTOOL_H

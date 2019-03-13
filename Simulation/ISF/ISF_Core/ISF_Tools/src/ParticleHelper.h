/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ParticleHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_TOOLS_PARTICLEHELPER_H
#define ISF_TOOLS_PARTICLEHELPER_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
// ISF includes
#include "ISF_Interfaces/IParticleHelper.h"
// Barcode includes
#include "BarcodeEvent/Barcode.h"
#include "BarcodeInterfaces/IBarcodeSvc.h"

// // forward declarations
// namespace Barcode {
//   class IBarcodeSvc;
// }

namespace ISF {

    class ISFParticle;
    class TruthBinding;
    
  /** @class ParticleHelper
  
      A helper tool for the creation or update of an ISF::ISFParticle objects
  
      @author Andreas.Salzburger -at- cern.ch
     */
  class ParticleHelper : public extends<AthAlgTool, IParticleHelper> {
      
    public: 
      //** Constructor with parameters */
      ParticleHelper( const std::string& t, const std::string& n, const IInterface* p );

      // Athena algtool's Hooks
      StatusCode  initialize();
      StatusCode  finalize();

      /** Create a new particle */
      virtual ISFParticle* createParticle( double x, double y, double z,
                                   double px, double py, double pz,
                                   double pMass,
                                   double pCharge,
                                   int pPdgId,
                                   double pTime,
                                   const ISFParticle &parent,
                                   Barcode::ParticleBarcode bc,
                                   TruthBinding* tBinding = nullptr,
				   const HepMcParticleLink * partLink = nullptr) const override final;

      /** Create a new particle */
      virtual ISFParticle* createParticle( const Amg::Vector3D& x,
                                   const Amg::Vector3D& p,
                                   double pMass,
                                   double pCharge,
                                   int pPdgId,
                                   double pTime,
                                   const ISFParticle &parent,
                                   Barcode::ParticleBarcode bc,
                                   TruthBinding* tBinding = nullptr,
				   const HepMcParticleLink * partLink = nullptr) const override final;

      /** Create a new particle */
      virtual ISFParticle* createParticle( const HepGeom::Point3D<double>& x,
                                   const HepGeom::Vector3D<double>& p,
                                   double pMass,
                                   double pCharge,
                                   int pPdgId,
                                   double pTime,
                                   const ISFParticle &parent,
                                   Barcode::ParticleBarcode bc,
                                   TruthBinding* tBinding = nullptr,
				   const HepMcParticleLink * partLink = nullptr) const override final;

      /** An updated particle (e.g. after transport) */
      virtual ISFParticle* updatedParticle( const ISFParticle& origIsp,
                                    const Amg::Vector3D& updatedPos,
                                    const Amg::Vector3D& updatedMom,
                                    double deltaTime = 0.) const override final;
      
      /** An updated particle (e.g. after transport) */
      virtual ISFParticle* updatedParticle( const ISFParticle& origIsp,
                                    const HepGeom::Point3D<double>&  updatedPos,
                                    const HepGeom::Vector3D<double>& updatedMom,
                                    double deltaTime = 0.) const override final;

	private:
	    
      ServiceHandle<Barcode::IBarcodeSvc>  m_barcodeSvc;             //!< The Barcode service
      
  }; 
  
}


#endif //> !ISF_TOOLS_PARTICLEHELPER_H

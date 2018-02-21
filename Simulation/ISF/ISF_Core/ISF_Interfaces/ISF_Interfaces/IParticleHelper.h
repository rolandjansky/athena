/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IParticleHelper.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_INTERFACES_IPARTICLEHELPER_H
#define ISF_INTERFACES_IPARTICLEHELPER_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"
 
// Simulation includes
#include "ISF_Event/SimSvcID.h" 

// Barcode includes
#include "BarcodeEvent/Barcode.h"

// Amg includes
#include "GeoPrimitives/GeoPrimitives.h"
 
// CLHEP includes
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"

namespace ISF {

  class ISFParticle;
  class TruthBinding;
 
  /**
   @class IParticleHelper
       
   Interface to create a particle from primary information,
   allows to reduce dependency on actual StackParticle implementation
       
   Could make use of a Data POOL eventually       
       
   @author Andreas.Salzburger -at- cern.ch
   */
     
  class IParticleHelper : virtual public IAlgTool {
     public:
     
       /** Virtual destructor */
       virtual ~IParticleHelper(){}

       /// Creates the InterfaceID and interfaceID() method
       DeclareInterfaceID(IParticleHelper, 1, 0);

       /** Create a new particle */
       virtual ISFParticle* createParticle(double x, double y, double z,
                                              double px, double py, double pz,
                                              double pMass,
                                              double pCharge,
                                              int pPdgId,
                                              double pTime,
                                              const ISFParticle &parent,
                                              Barcode::ParticleBarcode bc = Barcode::fUndefinedBarcode,
                                              TruthBinding* tBinding = nullptr) const = 0;

       /** Create a new particle */
       virtual ISFParticle* createParticle( const Amg::Vector3D& x,
                                            const Amg::Vector3D& p,
                                            double pMass,
                                            double pCharge,
                                            int pPdgId,
                                            double pTime,
                                            const ISFParticle &parent,
                                            Barcode::ParticleBarcode bc = Barcode::fUndefinedBarcode,
                                            TruthBinding* tBinding = nullptr) const = 0;

       /** Create a new particle */
       virtual ISFParticle* createParticle( const HepGeom::Point3D<double>& x,
                                            const HepGeom::Vector3D<double>& p,
                                            double pMass,
                                            double pCharge,
                                            int pPdgId,
                                            double pTime,
                                            const ISFParticle &parent,
                                            Barcode::ParticleBarcode bc = Barcode::fUndefinedBarcode,
                                            TruthBinding* tBinding = nullptr) const = 0;

       /** An updated particle (e.g. after transport) */
       virtual ISFParticle* updatedParticle( const ISFParticle& origIsp,
                                             const Amg::Vector3D& updatedPos,
                                             const Amg::Vector3D& updatedMom,
                                             double deltaTime = 0.) const = 0;

       /** An updated particle (e.g. after transport) */
       virtual ISFParticle* updatedParticle( const ISFParticle& origIsp,
                                             const HepGeom::Point3D<double>&  updatedPos,
                                             const HepGeom::Vector3D<double>& updatedMom,
                                             double deltaTime = 0.) const = 0;

  };

} // end of namespace

#endif // ISF_INTERFACES_IParticleHelper_H

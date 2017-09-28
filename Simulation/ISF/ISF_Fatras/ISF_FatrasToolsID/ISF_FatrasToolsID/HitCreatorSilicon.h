/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HitCreatorSilicon.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLSID_HITCREATORSILICON_H
#define ISF_FATRASTOOLSID_HITCREATORSILICON_H


// Fatras
#include "ISF_FatrasInterfaces/IHitCreator.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IIncidentListener.h"
// Trk
#include "TrkParameters/TrackParameters.h"
// CLHEP
#include "CLHEP/Random/RandomEngine.h"
// Identifier
#include "Identifier/Identifier.h"
//InDet
#include "InDetSimEvent/SiHitCollection.h"

class PixelID;
class SCT_ID;
class IInDetConditionsSvc;
class StoreGateSvc;
class TF1;
namespace InDet {
    class ClusterMakerTool;
}

namespace InDetDD  {
    class SiDetectorElement;
}

namespace Trk {
    class IRIO_OnTrackCreator; class LocalPosition;
    class Surface;
}

namespace ISF {
    class ISFParticle;
}

namespace iFatras {
      
  /** 
   @class HitCreatorSilicon

   RIOOnTrack creation, starting from intersection on an active surface

   @author Andreas.Salzburger -at- cern.ch 
   */
      
  class HitCreatorSilicon : public extends<AthAlgTool, IHitCreator, IIncidentListener>
  {
    public:

      /**Constructor */
      HitCreatorSilicon(const std::string&,const std::string&,const IInterface*);
      
      /**Destructor*/
      ~HitCreatorSilicon();
      
      /** AlgTool initailize method.*/
      StatusCode initialize();
      
      /** AlgTool finalize method */
      StatusCode finalize();
    
      /** handle for incident service */
      void handle(const Incident& inc);    
    
      /** Return nothing - store the HIT in hit collection */
      void createSimHit(const ISF::ISFParticle& isp, const Trk::TrackParameters&, double  ) const;
      
      /** templated function Return nothing - store the HIT in hit collection */
      template<typename ELEMENT>
      void createSimHit(const ISF::ISFParticle& isp, const Trk::TrackParameters& pars, double time, ELEMENT hitSiDetElement, bool isSiDetElement) const;
      
      /** Return the cluster on Track -- the PrepRawData is contained in this one */       
      const ParametersROT* createHit(const ISF::ISFParticle&, const Trk::TrackParameters&  ) const { return 0; }
      
      /** Return the cluster on Track -- the PrepRawData is contained in this one */       
      const std::vector< ParametersROT >* createHits(const ISF::ISFParticle&, const ParametersLayer& ) const { return 0; }

    protected:
      
      /* Incident Service */  
      ServiceHandle<IIncidentSvc>          m_incidentSvc; 
            
      /*  SiHit collection and collection name */
      SiHitCollection                     *m_hitColl;                  //!< the SiHit collection
      std::string                          m_collectionName;           //!< name of the collection on storegate

      /** Pointer to the random number generator service */
      ServiceHandle<IAtRndmGenSvc>         m_randomSvc;                //!< Random Svc  
      std::string                          m_randomEngineName;         //!< Name of the random number stream
      CLHEP::HepRandomEngine*              m_randomEngine;             //!< Random Engine 

      std::string                          m_siIdHelperName;          //!< where to find the Si helper
      const PixelID*                       m_pixIdHelper;             //!< the Pixel ID helper
      const SCT_ID*                        m_sctIdHelper;             //!< the SCT ID helper

      /** ToolHandle to ClusterMaker */
      ServiceHandle<IInDetConditionsSvc>   m_condSummarySvc;          //!< Handle to Pixel/SCT conditions service
      bool                                 m_useConditionsSvc;
      TF1*                                 m_dEdX_function;            //!< function to evaluate dEdx

      double                               m_siPathToCharge;           //!< convert path in silicon to charge
      bool                                 m_fastEnergyDepositionModel; //!< use fast energy deposition model (landau approximation ) 

     /** Calculate Energyloss with simple Landau approximation */
      double energyDeposit_fast(const ISF::ISFParticle& isp, bool& isPix, bool& isSCT ) const;

   /** Calculate Energyloss with exact Landau*Gauss */
      double energyDeposit_exact(const ISF::ISFParticle& isp, bool& isPix, bool& isSCT ) const;
  };

} // end of namespace

#endif 


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PhysicsValidationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_PHYSICSVALIDATIONTOOL_H
#define ISF_FATRASTOOLS_PHYSICSVALIDATIONTOOL_H

// Athena Base
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// iFatras
#include "ISF_FatrasInterfaces/IPhysicsValidationTool.h"

// Tracking
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#ifndef MAXCHILDREN
#define MAXCHILDREN 40
#endif

// ROOT forward declarations
class TTree;

class IIncidentSvc;
//class AtlasDetectorID;

namespace ISF {
    class ISFParticle;
}

namespace iFatras 
{
 
  class IParticleDecayHelper;
  class ITrackCreator;    
  class ISimHitCreator;    
    
  /** @class PhysicsValidationTool 
      
      Fatras AlgTool to validate the physics content 
      
      @author Sharka Todorova Sarka.Todorova -at- cern.ch
  */  
  
  class PhysicsValidationTool : public extends<AthAlgTool, IPhysicsValidationTool>
  {
  public:
    /** Constructor */
    PhysicsValidationTool(const std::string&,const std::string&,const IInterface*);
    
    /** Destructor */
    virtual ~PhysicsValidationTool ();
    
    /** AlgTool initialize method */
    virtual StatusCode initialize();
    
    /** AlgTool finalize method */

    virtual StatusCode finalize();

    /** ISFParticle info: old transport tool */
    void saveISFParticleInfo(const ISF::ISFParticle& isp, int endProcess, const Trk::TrackParameters* ePar, double time, double dX0 );
    
    /** ISFParticle info: new transport tool */
    void saveISFParticleInfo(const ISF::ISFParticle& isp, const Trk::ExtrapolationCell<Trk::TrackParameters>& ec,
				     Trk::ExtrapolationCode ecode );

    /** ISFParticle info: new transport tool */
    void saveISFParticleInfo(const ISF::ISFParticle& isp, const Trk::ExtrapolationCell<Trk::NeutralParameters>& ec,
				     Trk::ExtrapolationCode ecode );


    void saveISFVertexInfo(int process,Amg::Vector3D vertex,const ISF::ISFParticle& isp,Amg::Vector3D primIn,
			   Amg::Vector3D* primOut, const ISF::ISFParticleVector children);

    
  private:
     /** templated Tool retrieval - gives unique handling & look and feel */
     template <class T> StatusCode retrieveTool(ToolHandle<T>& thandle){
        if (!thandle.empty() && thandle.retrieve().isFailure()){
              ATH_MSG_FATAL( "[ fatras setup ] Cannot retrieve " << thandle << ". Abort.");
              return StatusCode::FAILURE;
          } else
              ATH_MSG_DEBUG("[ fatras setup ] Successfully retrieved " << thandle);
              return StatusCode::SUCCESS;
     }
     
     void saveInfo(const ISF::ISFParticle& isp); 
      
    /*---------------------------------------------------------------------
     *  Private members
     *---------------------------------------------------------------------*/

    ServiceHandle<ITHistSvc>                                     m_thistSvc;         //!< the histogram service
    std::string                                                  m_validationStream; //!< validation THist stream name
    /** Validation output TTree (+variables) */

    TTree                                                                *m_particles;    //!< ROOT tree containing track info
    mutable int                                                           m_pdg;
    mutable int                                                           m_scIn;  
    mutable int                                                           m_scEnd;
    mutable int                                                           m_gen;
    mutable int                                                           m_geoID;  
    mutable float                                                         m_theta;
    mutable float                                                         m_pth;
    mutable float                                                         m_pph;
    mutable float                                                         m_p;
    mutable float                                                         m_eloss;
    mutable float                                                         m_ionloss;
    mutable float                                                         m_radloss;
    mutable float                                                         m_zOaTr;
    mutable float                                                         m_wZ;
    mutable float                                                         m_thIn;
    mutable float                                                         m_phIn;
    mutable float                                                         m_dIn;
    mutable float                                                         m_thEnd;
    mutable float                                                         m_phEnd;
    mutable float                                                         m_dEnd;
    mutable float                                                         m_X0;
    mutable float                                                         m_dt;

    TTree                                                        *m_interactions;   //!< ROOT tree containing vertex info
    mutable int                                                   m_process;
    mutable int                                                   m_pdg_mother;
    mutable int                                                   m_gen_mother;
    mutable int                                                   m_nChild;
    mutable  float                                                m_vtx_dist;
    mutable float                                                 m_vtx_theta;
    mutable float                                                 m_vtx_phi;
    mutable float                                                 m_vtx_e_diff;
    mutable float                                                 m_vtx_p_diff;
    mutable float                                                 m_vtx_plong_diff;
    mutable float                                                 m_vtx_pperp_diff;
    mutable float                                                 m_p_mother;
    mutable int                                                   m_pdg_child[MAXCHILDREN];     // decay product pdg code
    mutable float                                                 m_fp_child[MAXCHILDREN];      // fraction of incoming momentum 
    mutable float                                                 m_oa_child[MAXCHILDREN];      // opening angle wrt the mother
        
  }; 
}

#endif // FATRASTOOLS_TRANSPORTTOOL_H

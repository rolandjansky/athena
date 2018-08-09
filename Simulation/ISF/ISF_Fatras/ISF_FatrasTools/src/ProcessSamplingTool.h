/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TransportTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_PROCESSSAMPLINGTOOL_H
#define ISF_FATRASTOOLS_PROCESSSAMPLINGTOOL_H

// Athena Base
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"

// iFatras
#include "ISF_FatrasInterfaces/IProcessSamplingTool.h"

// Tracking
#include "TrkEventPrimitives/PdgToParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

namespace ISF {
  class ITruthSvc;
}

namespace iFatras 
{
  class IHadronicInteractionProcessor;
  class IPhotonConversionTool; 
  class IParticleDecayHelper;
  class IPhysicsValidationTool;
    
  /** @class ProcessSamplingTool 
      
      Fatras AlgTool to sample process and free path
      
      @author Sharka Todorova Sarka.Todorova -at- cern.ch
  */  
  
  class ProcessSamplingTool : public extends<AthAlgTool, IProcessSamplingTool>
  {
  public:
    /** Constructor */
    ProcessSamplingTool(const std::string&,const std::string&,const IInterface*);
    
    /** Destructor */
    virtual ~ProcessSamplingTool ();
    
    /** AlgTool initialize method */
    virtual StatusCode initialize();
    
    /** AlgTool finalize method */

    virtual StatusCode finalize();

    /** Process pre-sampling : to be moved into material updators eventually */
    Trk::PathLimit sampleProcess(double mom, double charge, Trk::ParticleHypothesis pHypothesis) const;
    
    /** Process simulation */
    ISF::ISFParticleVector  interact(const ISF::ISFParticle* parent,
				     Trk::ExCellCharged& eCell,
				     const Trk::Material* mat) const;
    ISF::ISFParticleVector  interact(const ISF::ISFParticle* parent,
				     Trk::ExCellNeutral& eCell,
				     const Trk::Material* mat) const;
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
      
    /*---------------------------------------------------------------------
     *  Private members
     *---------------------------------------------------------------------*/

     /** Random Generator service  */
     ServiceHandle<IAtRndmGenSvc>                 m_rndGenSvc;
     CLHEP::HepRandomEngine*                      m_randomEngine;
     std::string                                  m_randomEngineName;         //!< Name of the random number stream
     
     /** Truth record */
     ServiceHandle<ISF::ITruthSvc>                m_truthRecordSvc;

     /** hadronic interaction */
     bool                                         m_hadInt;
     ToolHandle<IHadronicInteractionProcessor>    m_hadIntProcessor;
     
     /** decay */
     ToolHandle<IParticleDecayHelper>                 m_particleDecayer;

      /** IPhotonConversionTool */
     ToolHandle<iFatras::IPhotonConversionTool>   m_conversionTool;

     Trk::ParticleMasses                          m_particleMasses;    //!< Struct of Particle masses

     /** validation */
     bool                                         m_validationMode;
     ToolHandle<IPhysicsValidationTool>           m_validationTool;

  }; 
}

#endif // FATRASTOOLS_PROCESSSSAMPLINGTOOL_H

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_IDCALIBHYPOTOOL_H
#define TRIGLONGLIVEDPARTICLESHYPO_IDCALIBHYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <string>

/**
 * @class IDCalibHypoTool
 * @brief Generate the decision for IDCalib stream trigger
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch>
 **/

class IDCalibHypoTool : virtual public ::AthAlgTool
{ 
public: 
   IDCalibHypoTool( const std::string& type, const std::string& name, const IInterface* parent );
   
   virtual ~IDCalibHypoTool() = default;
   virtual StatusCode initialize() override;
   
   struct IDCalibHypoInfo {
      TrigCompositeUtils::Decision*  decision;
      const xAOD::TrackParticle*     track; 
      const xAOD::TrackParticleContainer* trackContainer;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionsIDs;
   };
   
   /**
    * @brief decides upon a collection of tracks
    **/
   StatusCode decide( std::vector<IDCalibHypoInfo>& ) const;

private:

   HLT::Identifier m_decisionId;
   Gaudi::Property< std::vector<float> >  m_cutTrackPtGeV   { this, "cutTrackPtGeV",  { float(10.0) }, "Track pT requirement in GeV" };

   //
   StatusCode inclusiveSelection( std::vector<IDCalibHypoInfo>& ) const;
   StatusCode multiplicitySelection( std::vector<IDCalibHypoInfo>& ) const;
   bool decideOnSingleObject( IDCalibHypoInfo&, size_t ) const;
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOTOOL_H

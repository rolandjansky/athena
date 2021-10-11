/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOTOOL_H
#define TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOTOOL_H

#include "Gaudi/Property.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <string>

/**
 * @class TrigDisappearingTrackHypoTool
 * @brief Generated the decision for hypo thats triggers disappearing track
 * @author Kunihiro Nagano <kunihiro.nagano@cern.ch>
 **/

class TrigDisappearingTrackHypoTool : virtual public ::AthAlgTool
{ 
public: 
   TrigDisappearingTrackHypoTool( const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent );
   
   virtual ~TrigDisappearingTrackHypoTool();
   virtual StatusCode initialize() override;
   
   struct DisTrkHypoInfo {
      TrigCompositeUtils::Decision* decision;
      const xAOD::TrigComposite*    disTrk;
      const TrigCompositeUtils::DecisionIDContainer previousDecisionsIDs;
   };
   
   /**
    * @brief decides upon a collection of tracks
    **/
   StatusCode decide( std::vector<DisTrkHypoInfo>& ) const;

private:

   HLT::Identifier m_decisionId;
   Gaudi::Property< std::vector<float> >  m_cutTrackPtGeV   { this, "cutTrackPtGeV",  { float(10.0) }, "Track pT requirement in GeV" };
   Gaudi::Property< std::vector<float> >  m_cutBDTPix4Sct0  { this, "cutBDTPix4Sct0", { float(0.05) }, "BDT score thershold for Pix4l_Sct0 category" };
   Gaudi::Property< std::vector<float> >  m_cutBDTPix4Sct1p { this, "cutBDTPix4Sct1p",{ float(0.04) }, "BDT score thershold for Pix4l_Sct1p category" };
   Gaudi::Property< std::vector<float> >  m_cutBDTPix3Sct0  { this, "cutBDTPix3Sct0", { float(0.05) }, "BDT score thershold for Pix3l_Sct0 category" };
   Gaudi::Property< std::vector<float> >  m_cutBDTPix3Sct1p { this, "cutBDTPix3Sct1p",{ float(0.04) }, "BDT score thershold for Pix3l_Sct1p category" };

   //
   StatusCode inclusiveSelection( std::vector<DisTrkHypoInfo>& ) const;
   StatusCode multiplicitySelection( std::vector<DisTrkHypoInfo>& ) const;
   bool decideOnSingleObject( DisTrkHypoInfo&, size_t ) const;
}; 

#endif //> !TRIGLONGLIVEDPARTICLESHYPO_TRIGDISTRACKHYPOTOOL_H

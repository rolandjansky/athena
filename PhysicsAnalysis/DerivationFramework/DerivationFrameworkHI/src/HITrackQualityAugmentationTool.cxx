/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//// HITrackQualityAugmentationTool
/////////////////////////////////////////////////////////////////////
//// Author: Soumya Mohapatra (soumya@cern.ch)

 

#include "DerivationFrameworkHI/HITrackQualityAugmentationTool.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include <vector>
#include <string>

namespace DerivationFramework {

HITrackQualityAugmentationTool::HITrackQualityAugmentationTool(const std::string& t,
      const std::string& n,
      const IInterface* p) :
    AthAlgTool(t,n,p)
  {
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    declareProperty("TrackSelectionTool_pp"      ,m_trkSelTool_pp      );
    declareProperty("TrackSelectionTool_hi_loose",m_trkSelTool_hi_loose);
    declareProperty("TrackSelectionTool_hi_tight",m_trkSelTool_hi_tight);
  }


 
StatusCode HITrackQualityAugmentationTool::addBranches() const{
      // Set up the decorators
      SG::AuxElement::Decorator<unsigned short> decorator("TrackQuality");
 
      // Get Primary vertex
      const xAOD::VertexContainer* vertices =  evtStore()->retrieve< const xAOD::VertexContainer >("PrimaryVertices");
      if(!vertices) {
        ATH_MSG_ERROR ("Couldn't retrieve VertexContainer with key PrimaryVertices");
        return StatusCode::FAILURE;
      }
      const xAOD::Vertex* pv(0);
      for (const xAOD::Vertex* vx : *vertices) {
        if (vx->vertexType() == xAOD::VxType::PriVtx) {
          pv = vx;
          break;
        }
      }

      //commented out, not used and generates compiler warning
      //float z_vtx=0;
      //if(pv) z_vtx=pv->z();

 
      // Get the track container
      const xAOD::TrackParticleContainer* tracks = evtStore()->retrieve< const xAOD::TrackParticleContainer >("InDetTrackParticles");
      if(!tracks) {
        ATH_MSG_ERROR ("Couldn't retrieve TrackParticleContainer with key InDetTrackParticles");
        return StatusCode::FAILURE;
      }

 
      // Get track quality this is what we're adding
      for(const auto* track:*tracks) {
      //for (xAOD::TrackParticleContainer::const_iterator trackIt=tracks->begin(); trackIt!=tracks->end(); ++trackIt)
        //std::cout<<GetTrackQuality(*trackIt,z_vtx)<<std::endl;
        if(pv) decorator(*track) =GetTrackQualityNew(track,pv);
        else   decorator(*track) = 0;
      }
 
      return StatusCode::SUCCESS;
}


unsigned short HITrackQualityAugmentationTool::GetTrackQualityNew(const xAOD::TrackParticle* track,const xAOD::Vertex* pv) const {
  int   n_sct_hits      =track->auxdata<unsigned char>("numberOfSCTHits");
  float d0      = track->d0();
  float z0_wrtPV= track->z0()+track->vz()-pv->z();
  float theta   = track->theta();

  //-------------------------------------------------------------------------------------------------
  bool pass_min_bias=false;
  if (m_trkSelTool_pp->accept(*track, pv)) pass_min_bias=true;
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  bool pass_hi_loose=false;
  if (m_trkSelTool_hi_loose->accept(*track, pv)) pass_hi_loose=true;
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  bool pass_hi_loose_additional_SCT_hit=true;
  if(!pass_hi_loose || n_sct_hits<7) pass_hi_loose_additional_SCT_hit=false;
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  bool pass_hi_loose_tight_d0_z0=true;
  if(!pass_hi_loose || fabs(d0)>1.0 || fabs(z0_wrtPV*sin(theta))>1.0) pass_hi_loose_tight_d0_z0=false;
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  bool pass_hi_loose_tighter_d0_z0=true;
  if(!pass_hi_loose || fabs(d0)>0.5 || fabs(z0_wrtPV*sin(theta))>0.5) pass_hi_loose_tighter_d0_z0=false;
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  bool pass_hi_tight=false;
  if (m_trkSelTool_hi_tight->accept(*track, pv)) pass_hi_tight=true;
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  bool pass_hi_tight_loose_d0_z0=true;
  if(pass_hi_tight==false){
    const auto& taccept = m_trkSelTool_hi_tight->getTAccept();
    static const auto d0Index = taccept.getCutPosition("D0");
    static const auto z0Index = taccept.getCutPosition("Z0SinTheta");
    static const auto nCuts = taccept.getNCuts();
    auto cutBitset = taccept.getCutResultBitSet();
    cutBitset |= (1 << d0Index) | (1 << z0Index);
    if(cutBitset.count() != nCuts                   ) pass_hi_tight_loose_d0_z0=false;
    if(fabs(d0)>1.5 || fabs(z0_wrtPV*sin(theta))>1.5) pass_hi_tight_loose_d0_z0=false;
  }
  //-------------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------------
  bool pass_hi_tight_tighter_d0_z0=true;
  if(!pass_hi_tight || fabs(d0)>0.5 || fabs(z0_wrtPV*sin(theta))>0.5) pass_hi_tight_tighter_d0_z0=false;
  //-------------------------------------------------------------------------------------------------

  unsigned short    quality =0;
  if(pass_min_bias                   ) quality+=PP_MIN_BIAS;
  if(pass_hi_loose                   ) quality+=HI_LOOSE;
  if(pass_hi_loose_additional_SCT_hit) quality+=HI_LOOSE_7SCT_HITS;
  if(pass_hi_loose_tight_d0_z0       ) quality+=HI_LOOSE_TIGHT_D0_Z0;
  if(pass_hi_loose_tighter_d0_z0     ) quality+=HI_LOOSE_TIGHTER_D0_Z0;
  if(pass_hi_tight_loose_d0_z0       ) quality+=HI_TIGHT_LOOSE_D0_Z0;
  if(pass_hi_tight                   ) quality+=HI_TIGHT;
  if(pass_hi_tight_tighter_d0_z0     ) quality+=HI_TIGHT_TIGHTER_D0_Z0;
  return quality;
}



unsigned short HITrackQualityAugmentationTool::GetTrackQuality(const xAOD::TrackParticle* track,float z_vtx) const {
          //-------------------------------------------------------------------------------------------------
          float pt      = track->pt();
          float eta     = track->eta();
          //float phi     = track->phi();
    
          int   n_Ipix_hits     =track->auxdata<unsigned char>("numberOfInnermostPixelLayerHits"      );
          int   n_Ipix_expected =track->auxdata<unsigned char>("expectInnermostPixelLayerHit"         );
          int   n_NIpix_hits    =track->auxdata<unsigned char>("numberOfNextToInnermostPixelLayerHits");
          int   n_NIpix_expected=track->auxdata<unsigned char>("expectNextToInnermostPixelLayerHit"   );
          int   n_sct_hits      =track->auxdata<unsigned char>("numberOfSCTHits");
          int   n_pix_hits      =track->auxdata<unsigned char>("numberOfPixelHits");
          int   n_sct_holes     =track->auxdata<unsigned char>("numberOfSCTHoles");
          //int   n_pix_holes     =track->auxdata<unsigned char>("numberOfPixelHoles");
          int   n_sct_dead      =track->auxdata<unsigned char>("numberOfSCTDeadSensors");
          int   n_pix_dead      =track->auxdata<unsigned char>("numberOfPixelDeadSensors");
    
          float chi2=track->chiSquared();
          float ndof=track->numberDoF();
    
          float d0      = track->d0();
          float z0_wrtPV= track->z0()+track->vz()-z_vtx;
          float theta   = track->theta();
          //-------------------------------------------------------------------------------------------------


          //-------------------------------------------------------------------------------------------------
          bool pass_min_bias=true;
          {
            if(fabs(eta)>2.5) pass_min_bias=false;
            if(n_Ipix_expected>0){
              if (n_Ipix_hits==0) pass_min_bias=false;
            }
            else{
              if(n_NIpix_expected>0 && n_NIpix_hits==0) pass_min_bias=false;
            }
      
            int n_sct=n_sct_hits+n_sct_dead;
            if     (pt<=300) {if (n_sct <2)  pass_min_bias=false;}
            else if(pt<=400) {if (n_sct <4)  pass_min_bias=false;}
            else if(pt> 400) {if (n_sct <6)  pass_min_bias=false;}
      
            int n_pix=n_pix_hits+n_pix_dead;
            if(n_pix<=0) pass_min_bias=false;
      
            if(fabs(d0)>1.5) pass_min_bias=false;
            if(fabs(z0_wrtPV*sin(theta))>1.5) pass_min_bias=false;
      
            if(pt>10000 && TMath::Prob(chi2,ndof)<=0.01) pass_min_bias=false;
            //if(n_sct_holes>1 || n_pix_holes>0) continue;
            //if(n_pix_hits<3 || n_sct_hits<8) continue;
          }
          //-------------------------------------------------------------------------------------------------


 
          //-------------------------------------------------------------------------------------------------
          bool pass_hi_loose=true;
          {
            if(fabs(eta)>2.5) pass_hi_loose=false;
            if(n_Ipix_expected>0){
              if (n_Ipix_hits==0) pass_hi_loose=false;
            }
            else{
              if(n_NIpix_expected>0 && n_NIpix_hits==0) pass_hi_loose=false;
            }
      
            if(n_pix_hits==0) pass_hi_loose=false;
            if(n_sct_hits< 6) pass_hi_loose=false;
            if(pt>10000 && TMath::Prob(chi2,ndof)<=0.01) pass_hi_loose=false;
            if(fabs(d0) >1.5) pass_hi_loose=false;
            if(fabs(z0_wrtPV*sin(theta))>1.5) pass_hi_loose=false;
          }
          //-------------------------------------------------------------------------------------------------



          //-------------------------------------------------------------------------------------------------
          bool pass_hi_loose_additional_SCT_hit=true;
          if(!pass_hi_loose) pass_hi_loose_additional_SCT_hit=false;
          else{
            if(n_sct_hits<7) pass_hi_loose_additional_SCT_hit=false;
          }
          //-------------------------------------------------------------------------------------------------



          //-------------------------------------------------------------------------------------------------
          bool pass_hi_loose_tight_d0_z0=true;
          if(!pass_hi_loose || fabs(d0)>1.0 || fabs(z0_wrtPV*sin(theta))>1.0) pass_hi_loose_tight_d0_z0=false;
          //-------------------------------------------------------------------------------------------------



          //-------------------------------------------------------------------------------------------------
          bool pass_hi_loose_tighter_d0_z0=true;
          if(!pass_hi_loose || fabs(d0)>0.5 || fabs(z0_wrtPV*sin(theta))>0.5) pass_hi_loose_tighter_d0_z0=false;
          //-------------------------------------------------------------------------------------------------



         //-------------------------------------------------------------------------------------------------
         bool pass_hi_tight_loose_d0_z0=true;
         if(!pass_hi_loose) pass_hi_tight_loose_d0_z0=false;
         else{
           if(n_pix_hits <2  ) pass_hi_tight_loose_d0_z0=false;
           if(n_sct_hits <8  ) pass_hi_tight_loose_d0_z0=false;
           if(n_sct_holes>1  ) pass_hi_tight_loose_d0_z0=false;
           if(ndof==0) pass_hi_tight_loose_d0_z0=false;
           else if(chi2/ndof>6) pass_hi_tight_loose_d0_z0=false;
         }
         //-------------------------------------------------------------------------------------------------



          //-------------------------------------------------------------------------------------------------
          bool pass_hi_tight=true;
          if(!pass_hi_loose) pass_hi_tight=false;
          else{
            if(n_pix_hits <2  ) pass_hi_tight=false;
            if(n_sct_hits <8  ) pass_hi_tight=false;
            if(n_sct_holes>1  ) pass_hi_tight=false;
            if(fabs(d0)   >1.0) pass_hi_tight=false;
            if(fabs(z0_wrtPV*sin(theta))>1.0) pass_hi_tight=false;
            if(ndof==0) pass_hi_tight=false;
            else if(chi2/ndof>6) pass_hi_tight=false;
          }
          //-------------------------------------------------------------------------------------------------



          //-------------------------------------------------------------------------------------------------
          bool pass_hi_tight_tighter_d0_z0=true;
          if(!pass_hi_tight) pass_hi_tight_tighter_d0_z0=false;
          else{
            if(fabs(d0)>0.5 || fabs(z0_wrtPV*sin(theta))>0.5) pass_hi_tight_tighter_d0_z0=false;
          }
          //-------------------------------------------------------------------------------------------------





          unsigned short    quality =0;
          if(pass_min_bias                   ) quality+=PP_MIN_BIAS;
          if(pass_hi_loose                   ) quality+=HI_LOOSE;
          if(pass_hi_loose_additional_SCT_hit) quality+=HI_LOOSE_7SCT_HITS;
          if(pass_hi_loose_tight_d0_z0       ) quality+=HI_LOOSE_TIGHT_D0_Z0;
          if(pass_hi_loose_tighter_d0_z0     ) quality+=HI_LOOSE_TIGHTER_D0_Z0;
          if(pass_hi_tight_loose_d0_z0       ) quality+=HI_TIGHT_LOOSE_D0_Z0;
          if(pass_hi_tight                   ) quality+=HI_TIGHT;
          if(pass_hi_tight_tighter_d0_z0     ) quality+=HI_TIGHT_TIGHTER_D0_Z0;
          return quality;
}


}

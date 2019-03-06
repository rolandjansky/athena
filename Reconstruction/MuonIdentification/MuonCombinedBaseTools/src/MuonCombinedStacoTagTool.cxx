/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCombinedStacoTagTool
//  AlgTool performing statistical combination of ID and MS tracks (STACO)
//  A StacoTag is added to the InDetCandidate object.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/StacoTag.h"
#include "MuonCombinedStacoTagTool.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuonCombinedStacoTagTool::MuonCombinedStacoTagTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
	m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool",this),
	m_tagTool("MuonCombined::MuonTrackTagTestTool/MuonTrackTagTestTool",this),
        m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
	m_caloExtTool("Trk::ParticleCaloExtensionTool/ParticleCaloExtensionTool",this)
  {
    declareInterface<IMuonCombinedTagTool>(this);
    declareProperty("Printer",m_printer );
    declareProperty("TagTool",m_tagTool );
    declareProperty("Extrapolator",m_extrapolator );
    declareProperty("ParticleCaloExtensionTool", m_caloExtTool);
  }

  MuonCombinedStacoTagTool::~MuonCombinedStacoTagTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuonCombinedStacoTagTool::initialize() {
    ATH_MSG_INFO( "Initializing MuonCombinedStacoTagTool - package version " << PACKAGE_VERSION );

    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_tagTool.retrieve());
    ATH_CHECK(m_extrapolator.retrieve());
    ATH_CHECK(m_caloExtTool.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode MuonCombinedStacoTagTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonCombinedStacoTagTool::combine( const MuonCandidate& muonCandidate, const std::vector<const InDetCandidate*>& indetCandidates, InDetCandidateToTagMap& tagMap,
					  TrackCollection* combTracks, TrackCollection* METracks) const {  

    if(!combTracks || !METracks) ATH_MSG_WARNING("No TrackCollection passed");

    // only combine if the back extrapolation was successfull
    if( !muonCandidate.extrapolatedTrack() ||
	!muonCandidate.extrapolatedTrack()->perigeeParameters() ||
	!muonCandidate.extrapolatedTrack()->perigeeParameters()->covariance() ) return;

    std::unique_ptr<const Trk::Perigee> bestPerigee;
    const InDetCandidate*     bestCandidate = 0;
    double              bestChi2=2e20;

    // loop over ID candidates
    for( const auto idTP : indetCandidates ){
      
      //skip tracklets
      if(idTP->isSiliconAssociated()) continue;

      // ensure that also the id has a perigee with covariance
      if( !idTP->indetTrackParticle().perigeeParameters().covariance() ) continue;

      //ensure that id tp can be extrapolated to something
      std::unique_ptr<Trk::CaloExtension> caloExtension = m_caloExtTool->caloExtension(idTP->indetTrackParticle());
      if(!caloExtension) continue;
      if(caloExtension->caloLayerIntersections().empty()) continue;

      const Trk::Perigee* idPer = &idTP->indetTrackParticle().perigeeParameters();
      const Trk::Perigee* msPer = muonCandidate.extrapolatedTrack()->perigeeParameters();

      // check that the to perigee surfaces are the same
      if( idPer->associatedSurface() != msPer->associatedSurface() ){
        
        // extrapolate to id surface
        const Trk::TrackParameters* exPars = m_extrapolator->extrapolate(*muonCandidate.extrapolatedTrack(),idPer->associatedSurface());
        if( !exPars ) {
          ATH_MSG_WARNING("The ID and MS candidates are not expressed at the same surface: id r " 
                          << idTP->indetTrackParticle().perigeeParameters().associatedSurface().center().perp() 
                          << " z " << idTP->indetTrackParticle().perigeeParameters().associatedSurface().center().z() 
                          << " ms r " << muonCandidate.extrapolatedTrack()->perigeeParameters()->associatedSurface().center().perp() 
                          << " z " << muonCandidate.extrapolatedTrack()->perigeeParameters()->associatedSurface().center().z()
                          << " and extrapolation failed");
          continue;
        }else{ 
          msPer = dynamic_cast<const Trk::Perigee*>(exPars);
          if( !msPer ) {
            ATH_MSG_WARNING("Extrapolation did not return a perigee!");
            delete exPars;
            continue;
          }
        }
      }
      double chi2 = 0;
      std::unique_ptr<const Trk::Perigee> perigee = theCombIdMu(idTP->indetTrackParticle().perigeeParameters(),*muonCandidate.extrapolatedTrack()->perigeeParameters(),chi2);
      if( msPer != muonCandidate.extrapolatedTrack()->perigeeParameters() ) delete msPer;
      if( !perigee ) {
	ATH_MSG_DEBUG("Combination failed");
	continue;
      }
      if( chi2 < bestChi2 ){
	bestChi2 = chi2;
	bestPerigee = std::move(perigee);
	bestCandidate = idTP;
      }
    }
    if( bestCandidate ){
      double outerMatchChi2 = 1e19;
      if( bestCandidate->indetTrackParticle().trackLink().isValid() ){
	outerMatchChi2 = m_tagTool->chi2(*bestCandidate->indetTrackParticle().track(),*muonCandidate.extrapolatedTrack());
      }
      ATH_MSG_DEBUG("Combined Muon with ID " << m_printer->print(*bestPerigee) << " match chi2 " << bestChi2 << " outer match " << outerMatchChi2 );
      StacoTag* tag = new StacoTag(muonCandidate,bestPerigee,bestChi2);
      tagMap.addEntry(bestCandidate,tag);
    }    
  }

  std::unique_ptr<const Trk::Perigee> MuonCombinedStacoTagTool::theCombIdMu( const Trk::Perigee& indetPerigee, const Trk::Perigee& extrPerigee, 
							     double& chi2 ) const {


    chi2   = 1e20;
    if( !indetPerigee.covariance() || !extrPerigee.covariance() ){
      ATH_MSG_WARNING("Perigee parameters without covariance");
      return nullptr;
    }
    
    const AmgSymMatrix(5)& covID = *indetPerigee.covariance();
    const AmgSymMatrix(5)  weightID = covID.inverse();
    if  ( weightID.determinant() == 0 ){
      ATH_MSG_WARNING( " ID weight matrix computation failed     " ) ;
      return nullptr;
    }

    const AmgSymMatrix(5)& covMS = *extrPerigee.covariance();
    const AmgSymMatrix(5)  weightMS = covMS.inverse();
    if  ( weightMS.determinant() == 0 ){
      ATH_MSG_WARNING( "weightMS computation failed      " ) ;
      return nullptr;
    }

    AmgSymMatrix(5) weightCB = weightID + weightMS ;
    AmgSymMatrix(5)* covCB = new AmgSymMatrix(5)(weightCB.inverse());
    if (covCB->determinant() == 0){
      ATH_MSG_WARNING( " Inversion of weightCB failed " ) ;
      delete covCB;
      return nullptr;
    }

    AmgSymMatrix(5) covSum = covID + covMS ;
    AmgSymMatrix(5) invCovSum = covSum.inverse();

    if (invCovSum.determinant() == 0){
      ATH_MSG_WARNING( " Inversion of covSum failed " ) ;
      return nullptr;
    }

    AmgVector(5) parsMS(extrPerigee.parameters());
    double  diffPhi = parsMS[Trk::phi] - indetPerigee.parameters()[Trk::phi] ;
    if(diffPhi>M_PI)       parsMS[Trk::phi] -= 2.*M_PI; 
    else if(diffPhi<-M_PI) parsMS[Trk::phi] += 2.*M_PI;

    AmgVector(5) diffPars = indetPerigee.parameters() - parsMS;
    chi2 = diffPars.transpose() * invCovSum * diffPars; 
    chi2 = chi2/5. ; 

    AmgVector(5) parsCB = (*covCB) * ( weightID * indetPerigee.parameters() + weightMS * parsMS ) ;

    if(parsCB[Trk::phi]>M_PI)       parsCB[Trk::phi] -= 2.*M_PI; 
    else if(parsCB[Trk::phi]<-M_PI) parsCB[Trk::phi] += 2.*M_PI;

    return std::unique_ptr<const Trk::Perigee>
      (indetPerigee.associatedSurface().createParameters<5,Trk::Charged>( parsCB[Trk::locX], parsCB[Trk::locY],
                                                                          parsCB[Trk::phi],  parsCB[Trk::theta], parsCB[Trk::qOverP], 
                                                                          covCB ) );
  }

}	// end of namespace

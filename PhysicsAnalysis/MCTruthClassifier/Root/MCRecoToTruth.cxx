/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GENERATIONBASE
#include "MCTruthClassifier/MCTruthClassifier.h"
using namespace MCTruthPartClassifier;
using std::abs;

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::TrackParticle *trkPtr){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing trackClassifier" );

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_isPrompt            = false;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  m_thePart=getGenPart(trkPtr);

  if(!m_thePart)   return std::make_pair(parttype,partorig);

  ATH_MSG_DEBUG( "trackClassifier  succeeded " ); 
  return particleTruthClassifier(m_thePart);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Electron* elec){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing egamma electron Classifier" );

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  m_isPrompt            = false;

  m_egPartPtr.clear();
  m_egPartdR.clear();
  m_egPartClas.clear();

  if(elec->author()!=xAOD::EgammaParameters::AuthorFwdElectron){
    const xAOD::TrackParticle* trkPtr=elec->trackParticle();
    if(!trkPtr)     {return std::make_pair(parttype,partorig);}
    m_thePart=getGenPart(trkPtr);
  } 
  else {  
#ifndef XAOD_ANALYSIS //Fwd electron available only in Athena 
    const xAOD::CaloCluster* clus=elec->caloCluster();
    m_thePart  = egammaClusMatch(clus,true);
#else
    ATH_MSG_WARNING( "Forward Electron classification using extrapolation to Calo is available only in Athena , check your enviroment. " );
#endif
  }
  if(!m_thePart)  {return std::make_pair(parttype,partorig);}
  ATH_MSG_DEBUG( "egamma electron Classifier  succeeded " );
  return particleTruthClassifier(m_thePart);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Photon* phot){
  //-----------------------------------------------------------------------------------------


  ATH_MSG_DEBUG( "Executing egamma photon Classifier" ); 

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  m_isPrompt            = false;

  m_cnvPhtTrkPtr.clear();
  m_cnvPhtTrPart.clear();
  m_cnvPhtPartType.clear();
  m_cnvPhtPartOrig.clear();

  m_egPartPtr.clear();
  m_egPartdR.clear();
  m_egPartClas.clear();

  const xAOD::CaloCluster* clus=phot->caloCluster();
  if(!clus) return std::make_pair(parttype,partorig);  
  if( std::fabs(clus->eta())> 10.0 ||std::fabs(clus->phi())> 6.28||(clus->et())<=0.) return std::make_pair(parttype,partorig);

  const xAOD::Vertex* VxCvPtr=phot->vertex();

  if(VxCvPtr!=0) {
    for(int itrk=0; itrk<(int) VxCvPtr->nTrackParticles(); itrk++){
      if(itrk>1) continue;
      const xAOD::TrackParticle* trkPtr  = VxCvPtr->trackParticle(itrk);
      if(!trkPtr)  continue;
      m_cnvPhtTrkPtr.push_back(trkPtr); 
      const xAOD::TruthParticle* thePart=getGenPart(trkPtr);

      std::pair<ParticleType,ParticleOrigin>  classif=particleTruthClassifier(thePart);
      m_cnvPhtTrPart.push_back(thePart);

      m_cnvPhtPartType.push_back(classif.first);
      m_cnvPhtPartOrig.push_back(classif.second);
    } // end cycle itrk 

  } // VxCvPtr!=0

#ifndef XAOD_ANALYSIS
  m_thePart = egammaClusMatch(clus,false);
#else
  ATH_MSG_WARNING( "Photon  Classification using extrapolation to Calo is  available only in Athena , check your enviroment. " );
#endif
  if(!m_thePart)  {return std::make_pair(parttype,partorig);}
  ATH_MSG_DEBUG( "egamma photon  Classifier  succeeded " );

  return particleTruthClassifier(m_thePart);

}
//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Muon* mu ){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing muon  Classifier" );

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_isPrompt            = false;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;

  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;

  const xAOD::TrackParticle* trkPtr=0;

  if( mu->primaryTrackParticleLink().isValid())                 trkPtr = *mu->primaryTrackParticleLink();
  else if( mu->combinedTrackParticleLink().isValid() )          trkPtr = *mu->combinedTrackParticleLink();
  else if (mu->inDetTrackParticleLink().isValid())              trkPtr = *mu->combinedTrackParticleLink();
  else if( mu->muonSpectrometerTrackParticleLink().isValid() )  trkPtr = *mu->muonSpectrometerTrackParticleLink();

  if(!trkPtr)   return std::make_pair(parttype,partorig);

  m_thePart=getGenPart(trkPtr);
  if(!m_thePart)  return std::make_pair(parttype,partorig);

  ATH_MSG_DEBUG( "muon Classifier  succeeded " );
  return particleTruthClassifier(m_thePart);
}

//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::CaloCluster* clus){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing egamma photon Classifier with cluster Input" );

  m_thePart             = 0;
  m_Mother              = 0;
  m_MotherBarcode       = 0;
  m_MotherPDG           = 0;
  m_MotherStatus        = 0;
  m_MothOriVert         = 0;
  m_NumOfParents        = 0;
  m_NumOfDaug           = 0;
  m_PhotonMother        = 0; 
  m_PhotonMotherPDG     = 0; 
  m_PhotonMotherStatus  = 0; 
  m_PhotonMotherBarcode = 0;
  m_BkgElecMother       = 0;
  m_ParticleOutCome     = UnknownOutCome;

  m_deltaRMatch         =-999.;
  m_deltaPhi            =-999.;
  m_NumOfSiHits         = 0; 
  m_probability         = 0;
  ParticleType   parttype = Unknown;
  ParticleOrigin partorig = NonDefined;
  m_isPrompt            = false;
  m_egPartPtr.clear();
  m_egPartdR.clear();
  m_egPartClas.clear();

  if(!clus) return std::make_pair(parttype,partorig);  
  if( std::fabs(clus->eta())> 10.0 ||std::fabs(clus->phi())> M_PI||(clus->et())<=0.) return std::make_pair(parttype,partorig);

#ifndef XAOD_ANALYSIS
  m_thePart = egammaClusMatch(clus,false);
#else
  ATH_MSG_WARNING( "Cluster  Classification using extrapolation to Calo is available only in Athena , check your enviroment. " );
#endif

  if(!m_thePart)  {return std::make_pair(parttype,partorig);}
  ATH_MSG_DEBUG( "Calo Cluster  Classifier  succeeded " );

  return particleTruthClassifier(m_thePart);
}
//-----------------------------------------------------------------------------------------
std::pair<ParticleType,ParticleOrigin>
MCTruthClassifier::particleTruthClassifier(const xAOD::Jet* jet, bool DR = true){
  //-----------------------------------------------------------------------------------------

  ATH_MSG_DEBUG( "Executing Classifier with jet Input" );

  m_thePart             = 0;
  m_deltaRMatch         =-999.;
  m_ParticleOutCome           = UnknownOutCome;
  ParticleType   parttype     = UnknownJet;
  ParticleOrigin partorig     = NonDefined;
  ParticleType   tempparttype = UnknownJet;
  std::set<const xAOD::TruthParticle*>      allJetMothers;
  std::pair<ParticleType,ParticleOrigin>   res;

  if(!jet) return std::make_pair(parttype,partorig); 

  allJetMothers.clear();

  if (!DR) { 

  } else { 
    // use a DR matching scheme (default)
    // retrieve collection and get a pointer 
    const xAOD::TruthParticleContainer  * xTruthParticleContainer;
    StatusCode sc = evtStore()->retrieve(xTruthParticleContainer, m_xaodTruthParticleContainerName);
    if (sc.isFailure()||!xTruthParticleContainer){
      ATH_MSG_WARNING( "No  xAODTruthParticleContainer "<<m_xaodTruthParticleContainerName<<" found" ); 
      return std::make_pair(parttype,partorig);
    }

    ATH_MSG_DEBUG( "xAODTruthParticleContainer  " << m_xaodTruthParticleContainerName<<" successfully retrieved " );

    // find the matching truth particles
    for(const auto thePart : *xTruthParticleContainer){
      // do not look at intermediate particles
      if(thePart->status()!=2&&thePart->status()!=3&&thePart->status()!=10902) continue;
      // match truth particles to the jet
      if (deltaR((*thePart),(*jet)) > m_jetPartDRMatch) continue;
      // determine jet origin
      findAllJetMothers(thePart,allJetMothers);
      // determine jet type
      if(thePart->status()==3) continue;
      // determine jet type
      tempparttype = particleTruthClassifier(thePart).first;
      if(tempparttype==Hadron) tempparttype=defTypeOfHadron(thePart->pdgId());
      // classify the jet
      if (tempparttype==BBbarMesonPart || tempparttype==BottomMesonPart || tempparttype==BottomBaryonPart) 
      {parttype = BJet;}
      else if (tempparttype==CCbarMesonPart || tempparttype==CharmedMesonPart || tempparttype==CharmedBaryonPart ) 
      {if (parttype==BJet) {} else { parttype = CJet;}}
      else if (tempparttype==StrangeBaryonPart||tempparttype==LightBaryonPart||tempparttype==StrangeMesonPart||tempparttype==LightMesonPart) 
      {if (parttype==BJet||parttype==CJet) {} else { parttype = LJet;}}
      else {if (parttype==BJet||parttype==CJet||parttype==LJet) {} else { parttype = UnknownJet;}}

    } // end loop over truth particles  

  }// end DR scheme

  // clasify the jet origin
  partorig = defJetOrig(allJetMothers);

  allJetMothers.clear();

  ATH_MSG_DEBUG( " jet Classifier succeeded" );  
  return std::make_pair(parttype,partorig);
}

double MCTruthClassifier::deltaR(const xAOD::TruthParticle& v1, const xAOD::Jet & v2) {
  double dphi = std::fabs(v1.phi()-v2.phi()) ;
  dphi = (dphi<=M_PI)? dphi : 2*M_PI-dphi;
  double deta = std::fabs(v1.eta()-v2.eta()) ;
  return std::sqrt(dphi*dphi+deta*deta) ;
}


//-----------------------------------------------------------------------------------------
const xAOD::TruthParticle* MCTruthClassifier::getGenPart(const xAOD::TrackParticle *trk){
  //-----------------------------------------------------------------------------------------
  //return GenParticle corresponding to given TrackParticle

  ATH_MSG_DEBUG( "Executing getGenPart " ); 

  if (!trk) return 0;

  m_deltaRMatch =-999.;
  m_deltaPhi    =-999.;
  m_NumOfSiHits = 0; 
  m_probability = 0;

  uint8_t NumOfPixHits = 0;
  uint8_t NumOfSCTHits = 0;
  typedef ElementLink<xAOD::TruthParticleContainer> TruthLink_t;

  static SG::AuxElement::Accessor<TruthLink_t>  tPL ("truthParticleLink");
  if(!tPL.isAvailable(*trk)){
    ATH_MSG_DEBUG("Track particle is not associated to truth particle");
    return 0;
  }  

  auto truthLink = tPL(*trk);
  if (!truthLink.isValid()){
    ATH_MSG_DEBUG("Invalid link to truth particle");
    return 0;
  }

  const xAOD::TruthParticle* theGenParticle=(*truthLink);
  if (!theGenParticle){
    ATH_MSG_DEBUG("Could not find truth matching for track"); 
    return 0;
  }

  static SG::AuxElement::Accessor<float >  tMP ("truthMatchProbability");
  if(tMP.isAvailable(*trk)){
    m_probability = tMP(*trk);
  }
  else {
    ATH_MSG_DEBUG("Truth match probability not available");
  }

  if(theGenParticle->status()==2||theGenParticle->status()==3){   
    ATH_MSG_WARNING("track matched to the truth with status "<<theGenParticle->status());
  }

  if(theGenParticle->status()==2 && 
     (abs(theGenParticle->pdgId())==11||abs(theGenParticle->pdgId())==13)){
    const xAOD::TruthVertex* EndVrtx=theGenParticle->decayVtx();
    const xAOD::TruthParticle* theGenPartTmp(0);

    if(EndVrtx!=0){
      int itr=0;
      do { 
        theGenPartTmp=0;
        for(unsigned int ipOut=0; ipOut<EndVrtx->nOutgoingParticles();ipOut++){
          const xAOD::TruthParticle* theDaugt=EndVrtx->outgoingParticle(ipOut);
          if(!theDaugt) {
            continue;
          }
          if(theDaugt->pdgId()==theGenParticle->pdgId()) {
            theGenPartTmp=theDaugt;
          }
          if(theDaugt->pdgId()!=theGenParticle->pdgId()&&theDaugt->pdgId()!=22){
            theGenPartTmp=0;
          }
        }

        itr++;
        if(itr>100) { 
          ATH_MSG_WARNING ("getGenPart infinite while");  break;
        }

        if(theGenPartTmp!=0) {
          EndVrtx = theGenPartTmp->decayVtx();
        } 
        else  {
          EndVrtx = 0;
        }
      } while (theGenPartTmp && theGenPartTmp->pdgId()==theGenParticle->pdgId()
               &&theGenPartTmp->status()==2&&EndVrtx!=0);

      if(theGenPartTmp && theGenPartTmp->pdgId()==theGenParticle->pdgId()) {
        theGenParticle=theGenPartTmp;
      }	   
    }
  }

  if (!trk->summaryValue(NumOfSCTHits, xAOD::numberOfSCTHits) )
    ATH_MSG_DEBUG("Could not retrieve number of SCT hits");
  if (!trk->summaryValue(NumOfPixHits, xAOD::numberOfPixelHits) )
    ATH_MSG_DEBUG("Could not retrieve number of Pixel hits");

  m_NumOfSiHits=NumOfSCTHits+NumOfPixHits;

  m_deltaPhi  = detPhi(theGenParticle->phi(),trk->phi());
  float  deteta  = detEta(theGenParticle->eta(),trk->eta());
  m_deltaRMatch   = rCone(m_deltaPhi, deteta);
  if(m_NumOfSiHits>m_NumOfSiHitsCut&&m_deltaRMatch >m_deltaRMatchCut) theGenParticle=NULL; 
  else if(m_NumOfSiHits<=m_NumOfSiHitsCut && m_deltaPhi > m_deltaPhiMatchCut)  theGenParticle=NULL; 

  ATH_MSG_DEBUG( "getGenPart  succeeded " );
  return(theGenParticle);
}

#endif

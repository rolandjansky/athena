/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETPerformance/JetVariablesTool.h"
//#include "JetUtils/JetCaloHelper.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloGeoHelpers/CaloSampling.h"
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"


namespace {


double getEnergy (const Jet* jet, CaloSampling::CaloSample samp)
{
  return jet->getShape ("energy_" + CaloSampling::getSamplingName (samp));
}


double jetEMFraction(const Jet * jet)
{
  std::string base="energy_";
  unsigned int nsamp = CaloSampling::getNumberOfSamplings();
  double e_HAD = 0;
  double e_EM = 0;

  for (unsigned int i=0; i < nsamp; i++) {
    double e = jet->getShape(base + CaloSampling::getSamplingName(i));
    if (CaloSamplingHelper::isEMSampling ((CaloSampling::CaloSample)(i)))
      e_EM += e;
    else if (CaloSamplingHelper::isHADSampling ((CaloSampling::CaloSample)(i)))
      e_HAD += e;
  }
  
  if( (e_EM==0) || ((e_EM+e_HAD)==0) ) return 0.;
  return (e_EM / (e_EM+e_HAD));
}


int compute_nLeading(std::vector<double> &cell_energies,  const float& e, const float& frac)
{  
  std::sort(cell_energies.rbegin(),cell_energies.rend());
  int counter =0;
  float sum = 0;
  for(unsigned int i=0;i<cell_energies.size();i++)
    {
      sum += cell_energies[i];
      counter++;
      if(sum>frac*e) break;
    }
  return counter;
}


// In JetUtils for xAOD...
int nLeadingCells(const Jet* jet,double threshold)
{
  double sumE_cells=0;
  std::vector<double> cell_energies;

  NavigationToken<CaloCell,double> cellToken;
  jet->fillToken(cellToken,double(1.));
  
  if ( cellToken.size() > 0 )
    {
      NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
      NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
      for(; firstCell != lastCell; ++firstCell)
	{
	  const CaloCell* thisCell = *firstCell;
	  cell_energies.push_back(thisCell->e());
	  sumE_cells+=thisCell->e();
	}//cells
    }
  
  return compute_nLeading(cell_energies,sumE_cells,threshold);
}


} // anonymous namespace


JetVariablesTool::JetVariablesTool( const std::string& type,
			      const std::string& name,
			      const IInterface* parent ) : AthAlgTool( type, name, parent ) {
  declareInterface<JetVariablesTool>( this );

  declareProperty("JetCollectionKey",           m_jetCollectionKey          = "Cone4H1TopoJets" );
  declareProperty("JetTruthCollectionKey",      m_jetTruthCollectionKey     = "Cone4TruthJets" );
  declareProperty("TrackParticleContainerKey",  m_trackParticleKey          = "TrackParticleCandidate" );

  m_dummyJetColl = 0;
  m_dummyTracks = 0;
}

JetVariablesTool::~JetVariablesTool() {
}

StatusCode JetVariablesTool::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode JetVariablesTool::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode JetVariablesTool::retrieveContainers() {

  StatusCode sc = StatusCode::SUCCESS;

  sc = retrieveJetContainer(m_jetCollectionKey);
  if (sc.isFailure()) {
    msg() << MSG::WARNING << "JetVariablesTool: failed retrieving " << m_jetCollectionKey << endmsg; 
    return sc;
  }

  sc = retrieveTrackContainer(m_trackParticleKey);
  if (sc.isFailure()) {
    msg() << MSG::WARNING << "JetVariablesTool: failed retrieving " << m_trackParticleKey << endmsg; 
    return sc;
  }

  sc = retrieveTruthJetContainer(m_jetTruthCollectionKey);
  if (sc.isFailure()) {
    msg() << MSG::WARNING << "JetVariablesTool: failed retrieving " << m_jetTruthCollectionKey << endmsg; 
    return sc;
  }

  return sc;
}//end retrieveContainers()

StatusCode JetVariablesTool::retrieveJetContainer(std::string jetContainerKey) {
  const JetCollection*  jet = 0;
  StatusCode sc = StatusCode::SUCCESS;

  if (evtStore()->contains<JetCollection>(jetContainerKey)) {
    sc=evtStore()->retrieve( jet, jetContainerKey );
    if( sc.isFailure()  ||  !jet ) {
      msg() << MSG::WARNING << "JetVariablesTool: No JetCollection found in StoreGate, key:" << jetContainerKey << endmsg; 
      return sc;
    }
  } else {jet = m_dummyJetColl;}
  setJetCollection(jet);

  return sc;

}//end of retrieveJetContainer

StatusCode JetVariablesTool::retrieveTrackContainer(std::string trackContainerKey) {
  const Rec::TrackParticleContainer *tracks = 0;
  StatusCode sc = StatusCode::SUCCESS;

  if (evtStore()->contains<Rec::TrackParticleContainer>(trackContainerKey)) {
    sc=evtStore()->retrieve( tracks, trackContainerKey );
    if( sc.isFailure()  ||  !tracks ) {
      msg() << MSG::DEBUG << "JetVariablesTool: No TrackParticleContainer found in StoreGate, key:" << trackContainerKey << endmsg;
      return sc;
    }
  } else { tracks = m_dummyTracks;}
  setTrackParticleCollection(tracks);

  return sc;

}//end of retrieveTrackContainer

StatusCode JetVariablesTool::retrieveTruthJetContainer(std::string truthJetContainerKey) {
  const JetCollection*  jetTruth = 0;
  StatusCode sc = StatusCode::SUCCESS;

  if (evtStore()->contains<JetCollection>(truthJetContainerKey)) {
    sc=evtStore()->retrieve( jetTruth, truthJetContainerKey );
    if( sc.isFailure()  ||  !jetTruth ) {
      msg() << MSG::DEBUG << "JetVariablesTool: No Truth JetCollection found in StoreGate, key:" << truthJetContainerKey << endmsg;
      return sc;
    }
  } else { jetTruth = m_dummyJetColl;}
  setJetTruthCollection(jetTruth);

  return sc;
}

float JetVariablesTool::leastJetEMfraction(const JetCollection *jetcoll) {
  float leastJetEMfraction = 9999.;

  for (JetCollection::const_iterator jetItr = jetcoll->begin(); jetItr != jetcoll->end(); ++jetItr) {
    float jetEMF = jetEMFraction(*jetItr);
    if (jetEMF < leastJetEMfraction) {
      leastJetEMfraction = jetEMF;
    }
  }//loop over jets
  return leastJetEMfraction;
}//end leastJetEMfraction()

float JetVariablesTool::leastJetEMfraction() {
  return leastJetEMfraction(m_jetColl);
}

float JetVariablesTool::EventEMFraction()
{
   //
   // implementation from Reyhaneh Rezvani, Hugo Beauchemin
   //
   const CaloClusterContainer* clusters;   
   if ( evtStore()->retrieve(clusters,"CaloCalTopoCluster").isFailure() ) {
      msg() << MSG::WARNING << " Could not get pointer to CaloClusterContainer " << endmsg;
      return 0.;
   }
   
   float EM = 0.;
   float HAD = 0.;
   CaloClusterContainer::const_iterator first = clusters->begin();
   CaloClusterContainer::const_iterator last = clusters->end();
   for(; first != last; ++first) {
      const CaloCluster* pClus = *first;      
      if (pClus) {	 	 
	 //NOTE: The eSampling variables are filled before LC calibrations are applied
	 //      on the topoclusters. The energy obtained this way is therefore only
	 //      calibrated at the EM scale

	 // LAr barrel
	 double presb = pClus->eSample(CaloSampling::PreSamplerB);
	 double emb1  = pClus->eSample(CaloSampling::EMB1);
	 double emb2  = pClus->eSample(CaloSampling::EMB2);
	 double emb3  = pClus->eSample(CaloSampling::EMB3);

	 // LAr EM endcap
	 // .............	 
	 double prese = pClus->eSample(CaloSampling::PreSamplerE);
	 double eme1  = pClus->eSample(CaloSampling::EME1);
	 double eme2  = pClus->eSample(CaloSampling::EME2);
	 double eme3  = pClus->eSample(CaloSampling::EME3);	 
	 
	 // Hadronic end cap calo
	 // .....................	 
	 double hec0  = pClus->eSample(CaloSampling::HEC0);
	 double hec1  = pClus->eSample(CaloSampling::HEC1);
	 double hec2  = pClus->eSample(CaloSampling::HEC2);
	 double hec3  = pClus->eSample(CaloSampling::HEC3);	 
	 
	 // Tile barrel
	 // ...........	 
	 double tbar0 = pClus->eSample(CaloSampling::TileBar0);
	 double tbar1 = pClus->eSample(CaloSampling::TileBar1);
	 double tbar2 = pClus->eSample(CaloSampling::TileBar2);
	 	 
	 // Tile gap (ITC & scint)
	 // ......................	 
	 double tgap1 = pClus->eSample(CaloSampling::TileGap1);
	 double tgap2 = pClus->eSample(CaloSampling::TileGap2);
	 double tgap3 = pClus->eSample(CaloSampling::TileGap3);	 
	 
	 // Tile extended barrel
	 // ....................	 
	 double text0 = pClus->eSample(CaloSampling::TileExt0);
	 double text1 = pClus->eSample(CaloSampling::TileExt1);
	 double text2 = pClus->eSample(CaloSampling::TileExt2);	 
	 
	 // Forward Calo
	 // ............	 
	 double facl0 = pClus->eSample(CaloSampling::FCAL0);
	 double fcal1 = pClus->eSample(CaloSampling::FCAL1);
	 double fcal2 = pClus->eSample(CaloSampling::FCAL2);
	 	 	 
         EM += presb + emb1 + emb2 + emb3 + prese + eme1 + eme2 + eme3 + facl0;	 
         HAD += hec0 + hec1 + hec2 + hec3 + tbar0 + tbar1 + tbar2 + tgap1 + tgap2 + tgap3 + text0 + text1 + text2 + fcal1 + fcal2;	 	 
      }
   }//End of Loop over TopoClusters.
   float EventEMFrac = 0.;
   if ( (EM+HAD) != 0 ) EventEMFrac = EM / (EM+HAD);
   return EventEMFrac;
}

float JetVariablesTool::JetCaloSamplingPS(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::PreSamplerB) +
    getEnergy (jet, CaloSampling::PreSamplerE);
}

float JetVariablesTool::JetCaloSamplingEM(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::EMB1) +
    getEnergy (jet, CaloSampling::EMB2) +
    getEnergy (jet, CaloSampling::EMB3) +
    getEnergy (jet, CaloSampling::EME1) +
    getEnergy (jet, CaloSampling::EME2) +
    getEnergy (jet, CaloSampling::EME3);
}

float JetVariablesTool::JetCaloSamplingHEC(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::HEC0) +
    getEnergy (jet, CaloSampling::HEC1) +
    getEnergy (jet, CaloSampling::HEC2) +
    getEnergy (jet, CaloSampling::HEC3);
}

float JetVariablesTool::JetCaloSamplingHEC3(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::HEC3);
}

float JetVariablesTool::JetCaloSamplingFCAL(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::FCAL0) +
    getEnergy (jet, CaloSampling::FCAL1) +
    getEnergy (jet, CaloSampling::FCAL2);
}

float JetVariablesTool::JetCaloSamplingTile(const Jet *jet){
  return JetCaloSamplingTile10(jet) + JetCaloSamplingTile2(jet);
}

float JetVariablesTool::JetCaloSamplingTile10(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::TileBar0) +
    getEnergy (jet, CaloSampling::TileBar1) +
    getEnergy (jet, CaloSampling::TileExt0) +
    getEnergy (jet, CaloSampling::TileExt1);
}

float JetVariablesTool::JetCaloSamplingTile2(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::TileBar2) +
    getEnergy (jet, CaloSampling::TileExt2);
}

float JetVariablesTool::JetCaloSamplingGap(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::TileGap1) +
    getEnergy (jet, CaloSampling::TileGap2) +
    getEnergy (jet, CaloSampling::TileGap3);
}

// ignore additional weighting factor that is applied in the MET cryo
// term for now
float JetVariablesTool::JetCaloSamplingCryo(const Jet *jet){
  return sqrt( std::fabs(getEnergy(jet,CaloSampling::EMB3) * getEnergy(jet,CaloSampling::TileBar0) ) );
}

float JetVariablesTool::JetCaloSamplingTotal(const Jet *jet){

   return (JetCaloSamplingPS(jet) +
	   JetCaloSamplingEM(jet) +
	   JetCaloSamplingHEC(jet) +
	   JetCaloSamplingFCAL(jet) +
	   JetCaloSamplingTile(jet) +
	   JetCaloSamplingGap(jet) );
}

float JetVariablesTool::JetPtWeightedEventEMfraction(const JetCollection *jetcoll) {

  float ev_emf = 0.;
  float weight = 0.;
  for (JetCollection::const_iterator jetItr = jetcoll->begin(); jetItr != jetcoll->end(); ++jetItr) {
    float jetEMF = jetEMFraction(*jetItr);
    ev_emf = ev_emf + jetEMF * (*jetItr)->et();
    weight = weight + (*jetItr)->et();
  }
  if (weight !=0.0) {
    ev_emf = ev_emf/weight;
  }
  else {//if weight == 0. there are no jets, return nonsense value to signify
    ev_emf = -9999.;
  }
  return ev_emf;
}

float JetVariablesTool::JetPtWeightedEventEMfraction() {
  return JetPtWeightedEventEMfraction(m_jetColl);
}

float JetVariablesTool::JetChargeFraction(const Jet *jet) {
   return JetChargeFraction(jet, m_trackParticles);
}

float JetVariablesTool::JetChargeFraction(const Jet *jet, const Rec::TrackParticleContainer *trackParticles) {

  float trkpt_jet = 0.;
  if( fabs(jet->eta()) < 2.5 ){

    for(Rec::TrackParticleContainer::const_iterator trkPItr = trackParticles->begin(); trkPItr != trackParticles->end(); ++trkPItr) {  
      double trk_jet_deta = fabs(jet->eta()-(*trkPItr)->eta());
      double trk_jet_dphi = fabs(jet->phi()-(*trkPItr)->phi());
      //watch for wraparound
      if (trk_jet_dphi > M_PI) {trk_jet_dphi = fabs(trk_jet_dphi - 2*M_PI);}

      double trk_jet_dR = sqrt( pow(trk_jet_deta,2) + pow(trk_jet_dphi,2) );
  
      if(trk_jet_dR < 0.4){
	 trkpt_jet += (*trkPItr)->et();
      }
    }//loop over tracks
  }//eta < 2.5
  if(fabs(jet->et()) > 1e-11) return trkpt_jet / jet->et();
  else return -1.;
}

int JetVariablesTool::JetNumAssociatedTracks(const Jet *jet) {
   return JetNumAssociatedTracks(jet, m_trackParticles);
}

int JetVariablesTool::JetNumAssociatedTracks(const Jet *jet, const Rec::TrackParticleContainer *trackParticles) {

  int trknum_jet = 0;
  if( fabs(jet->eta()) < 2.5 ){

    for(Rec::TrackParticleContainer::const_iterator trkPItr = trackParticles->begin(); trkPItr != trackParticles->end(); ++trkPItr) {
      double trk_jet_deta = fabs(jet->eta()-(*trkPItr)->eta());
      double trk_jet_dphi = fabs(jet->phi()-(*trkPItr)->phi());
      //watch for wraparound
      if (trk_jet_dphi > M_PI) {trk_jet_dphi = fabs(trk_jet_dphi - 2*M_PI);}

      double trk_jet_dR = sqrt( pow(trk_jet_deta,2) + pow(trk_jet_dphi,2) );
  
      if(trk_jet_dR < 0.4){
	trknum_jet++;
      }
    }//loop over tracks
  }//eta < 2.5
  return trknum_jet;
}

float JetVariablesTool::JetPtWeightedNumAssociatedTracks(const JetCollection *jetcoll, const Rec::TrackParticleContainer *trackParticles) {

  float ev_numtrks = 0.;
  float weight = 0.;
  for (JetCollection::const_iterator jetItr = jetcoll->begin(); jetItr != jetcoll->end(); ++jetItr) {
    float jetnumtrks = (float)(JetNumAssociatedTracks(*jetItr,trackParticles));
    ev_numtrks = ev_numtrks + jetnumtrks * (*jetItr)->et();
    weight = weight + (*jetItr)->et();
  }
  if (weight !=0.0) {
    ev_numtrks = ev_numtrks/weight;
  }
  else {//if weight == 0. there are no jets, return nonsense value to signify
    ev_numtrks = -9999.;
  }
  return ev_numtrks;
}

float JetVariablesTool::JetPtWeightedNumAssociatedTracks() {
  return JetPtWeightedNumAssociatedTracks(m_jetColl, m_trackParticles);
}

float JetVariablesTool::JetPtWeightedSize(const JetCollection *jetcoll) {
  float ev_jetsize = 0.;
  float weight = 0.;
  for (JetCollection::const_iterator jetItr = jetcoll->begin(); jetItr != jetcoll->end(); ++jetItr) {
    float jetsize = (float)((*jetItr)->size());
    ev_jetsize = ev_jetsize + jetsize * (*jetItr)->et();
    weight = weight + (*jetItr)->et();
  }
  if (weight !=0.0) {
    ev_jetsize = ev_jetsize/weight;
  }
  else {//if weight == 0. there are no jets, return nonsense value to signify
    ev_jetsize = -9999.;
  }
  return ev_jetsize;
}

int JetVariablesTool::lowestN90CellsJet() {
  return lowestN90CellsJet(m_jetColl);
}

int JetVariablesTool::lowestN90CellsJet(const JetCollection *jetcoll) {
  int lowestN90 = 9999;

  for (JetCollection::const_iterator jetItr = jetcoll->begin();
       jetItr != jetcoll->end(); ++jetItr) {

    if (nLeadingCells(*jetItr,0.9) < lowestN90) {
      lowestN90 = nLeadingCells(*jetItr,0.9);
    }
  }
  return lowestN90;
}

float JetVariablesTool::JetPtWeightedSize() {
  return JetPtWeightedSize(m_jetColl);
}

float JetVariablesTool::leadingJetEt(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (leadingJet(jet ,jetcoll)) {return jet->et();}
  else {return -9999;}
}
float JetVariablesTool::leadingJetEta(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (leadingJet(jet ,jetcoll)) {return jet->eta();}
  else {return -9999;}
}

float JetVariablesTool::leadingJetPhi(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (leadingJet(jet ,jetcoll)) {return jet->phi();}
  else {return -9999;}
}

float JetVariablesTool::secondJetEt(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (secondJet(jet ,jetcoll)) {return jet->et();}
  else {return -9999;}
}

float JetVariablesTool::secondJetEta(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (secondJet(jet ,jetcoll)) {return jet->eta();}
  else {return -9999;}
}

float JetVariablesTool::secondJetPhi(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (secondJet(jet ,jetcoll)) {return jet->phi();}
  else {return -9999;}
}

bool JetVariablesTool::leadingJet() const {
  const Jet *leadJet = 0; 
  return leadingJet(leadJet ,m_jetColl);
}

bool JetVariablesTool::leadingJet(const Jet * & leadJet ) const {
   return leadingJet(leadJet ,m_jetColl);
}

bool JetVariablesTool::leadingJet(const Jet * & leadJet ,const JetCollection *jetcoll) const {

  float leadingJetEt = -9999.;

  leadJet=0;
  for (JetCollection::const_iterator jetItr = jetcoll->begin(); jetItr != jetcoll->end(); ++jetItr) {
    if ((*jetItr)->et() > leadingJetEt) {
      leadingJetEt = (*jetItr)->et();
      leadJet=(*jetItr);
    }//new highest et jet
  }//loop over jets

  
  return (leadingJetEt>0);
}

Jet JetVariablesTool::GetLeadingJet() const {
  float leadingJetEt = 0;
  const Jet *leadJet=0;
  for (JetCollection::const_iterator jetItr = m_jetColl->begin(); jetItr != m_jetColl->end(); ++jetItr) {
    if ((*jetItr)->et() > leadingJetEt) {
      leadingJetEt = (*jetItr)->et();
      leadJet=(*jetItr);
    }//new highest et jet
  }//loop over jets
  return (leadJet);
}


bool JetVariablesTool::secondJet() const{
  const Jet *secjet=0;
  return secondJet(secjet, m_jetColl);
}

bool JetVariablesTool::secondJet(const Jet * & secJet) const {
   return secondJet(secJet ,m_jetColl) ;
}

bool JetVariablesTool::secondJet(const Jet * & secJet ,const JetCollection *jetcoll) const {

  // first find leading jet.
  secJet=0;
  const Jet * leadJet=0;
  if (!leadingJet(leadJet ,jetcoll)){
     return false;
  }
  if (leadJet==0){
     return false;
  }
  
  float JetEt = -9999.;
  // find leading jet among the rest.
  for (JetCollection::const_iterator jetItr = jetcoll->begin(); jetItr != jetcoll->end(); ++jetItr) {
     if ((*jetItr) == leadJet ){
        continue;
     }
     if ((*jetItr)->et() > JetEt) {
      JetEt = (*jetItr)->et();
      secJet=(*jetItr);
    }//new highest et jet
  }//loop over jets

  return (JetEt>0);
}

float JetVariablesTool::leadingJetEt() {
  return leadingJetEt(m_jetColl);
}

float JetVariablesTool::leadingJetEta() {
  return leadingJetEta(m_jetColl);
}

float JetVariablesTool::leadingJetPhi() {
  return leadingJetPhi(m_jetColl);
}

float JetVariablesTool::secondJetEt() {
  return secondJetEt(m_jetColl);
}

float JetVariablesTool::secondJetEta() {
  return secondJetEta(m_jetColl);
}

float JetVariablesTool::secondJetPhi() {
  return secondJetPhi(m_jetColl);
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETGoodness/JetVarTool.h"
#include "MissingETGoodness/Goodies.h"
//#include "JetUtils/JetCaloHelper.h"
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "TileEvent/TileCell.h"


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


JetVarTool::JetVarTool( const std::string& type,
			      const std::string& name,
			      const IInterface* parent ) :
  AthAlgTool( type, name, parent ),
  m_jetColl(nullptr),
  m_jetTruthColl(nullptr),
  m_trackParticles(nullptr)
{
  declareInterface<JetVarTool>( this );

  declareProperty("JetCollectionKey",           m_JetCollectionKey          = "Cone4H1TopoJets" );
  declareProperty("JetTruthCollectionKey",      m_JetTruthCollectionKey     = "Cone4TruthJets" );
  declareProperty("TrackParticleContainerKey",  m_TrackParticleKey          = "TrackParticleCandidate" );
}

JetVarTool::~JetVarTool() {
}

StatusCode JetVarTool::initialize() {
  ATH_MSG_DEBUG ( "JetVarTool Tool initialize() has been called" ) ;

  IClassIDSvc* clidsvc = 0;
  ATH_CHECK( service("ClassIDSvc", clidsvc) );
  if (!clidsvc->isIDInUse (1118613496)) {
    ATH_CHECK( clidsvc->setTypePackageForID (1118613496,
                                             "ParticleJetContainer",
                                             Athena::PackageInfo("dummy-00-00-00")) );
  }

  return StatusCode::SUCCESS;
}

StatusCode JetVarTool::finalize() {
  ATH_MSG_DEBUG ( "JetVarTool Tool finalize() has been called" ) ;
  return StatusCode::SUCCESS;
}

StatusCode JetVarTool::retrieveContainers() {

  ATH_MSG_DEBUG ( "in JetVarTool::retrieveContainers() " ) ;

  ATH_CHECK( retrieveJetContainer(m_JetCollectionKey) );
  ATH_CHECK( retrieveTrackContainer(m_TrackParticleKey) );
  ATH_CHECK( retrieveTruthJetContainer(m_JetTruthCollectionKey) );

  return StatusCode::SUCCESS;
}//end retrieveContainers()

StatusCode JetVarTool::retrieveJetContainer(std::string jetContainerKey) {
  const JetCollection*  jet = 0;
  if (evtStore()->contains<JetCollection>(jetContainerKey)) {
    ATH_CHECK( evtStore()->retrieve( jet, jetContainerKey ) );
  } else {jet = &m_dummyJetColl;}
  setJetCollection(jet);

  return StatusCode::SUCCESS;

}//end of retrieveJetContainer

StatusCode JetVarTool::retrieveTrackContainer(std::string trackContainerKey) {
  const Rec::TrackParticleContainer *tracks = 0;
  if (evtStore()->contains<Rec::TrackParticleContainer>(trackContainerKey)) {
    ATH_CHECK( evtStore()->retrieve( tracks, trackContainerKey ) );
  } else { tracks = &m_dummyTracks;}
  setTrackParticleCollection(tracks);

  return StatusCode::SUCCESS;

}//end of retrieveTrackContainer

StatusCode JetVarTool::retrieveTruthJetContainer(std::string truthJetContainerKey) {
  const JetCollection*  jetTruth = 0;
  if (evtStore()->contains<JetCollection>(truthJetContainerKey)) {
    ATH_CHECK( evtStore()->retrieve( jetTruth, truthJetContainerKey ) );
  } else { jetTruth = &m_dummyJetColl;}
  setJetTruthCollection(jetTruth);

  return StatusCode::SUCCESS;
}

float JetVarTool::leastJetEMfraction(const JetCollection *jetcoll) {
  float leastJetEMfraction = 9999.;

  for (JetCollection::const_iterator jetItr = jetcoll->begin(); jetItr != jetcoll->end(); ++jetItr) {
    float jetEMF = jetEMFraction(*jetItr);
    if (jetEMF < leastJetEMfraction) {
      leastJetEMfraction = jetEMF;
    }
  }//loop over jets
  if(std::fabs(leastJetEMfraction-9999.)<1e-5) return MET::MagicNumber;
  return leastJetEMfraction;
}//end leastJetEMfraction()

float JetVarTool::leastJetEMfraction() {
  return leastJetEMfraction(m_jetColl);
}

float JetVarTool::EventEMFraction()
{
   //
   // implementation from Reyhaneh Rezvani, Hugo Beauchemin
   //
   const CaloClusterContainer* clusters;   
   if ( evtStore()->retrieve(clusters,"CaloCalTopoCluster").isFailure() ) {
     ATH_MSG_WARNING ( " Could not get pointer to CaloClusterContainer " ) ;
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

float JetVarTool::JetCaloSamplingPS(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::PreSamplerB) +
    getEnergy (jet, CaloSampling::PreSamplerE);
}

float JetVarTool::JetCaloSamplingEM(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::EMB1) +
    getEnergy (jet, CaloSampling::EMB2) +
    getEnergy (jet, CaloSampling::EMB3) +
    getEnergy (jet, CaloSampling::EME1) +
    getEnergy (jet, CaloSampling::EME2) +
    getEnergy (jet, CaloSampling::EME3);
}

float JetVarTool::JetCaloSamplingHEC(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::HEC0) +
    getEnergy (jet, CaloSampling::HEC1) +
    getEnergy (jet, CaloSampling::HEC2) +
    getEnergy (jet, CaloSampling::HEC3);
}

float JetVarTool::JetCaloSamplingHEC3(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::HEC3);
}

float JetVarTool::JetCaloSamplingFCAL(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::FCAL0) +
    getEnergy (jet, CaloSampling::FCAL1) +
    getEnergy (jet, CaloSampling::FCAL2);
}

float JetVarTool::JetCaloSamplingTile(const Jet *jet){
  return JetCaloSamplingTile10(jet) + JetCaloSamplingTile2(jet);
}

float JetVarTool::JetCaloSamplingTile10(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::TileBar0) +
    getEnergy (jet, CaloSampling::TileBar1) +
    getEnergy (jet, CaloSampling::TileExt0) +
    getEnergy (jet, CaloSampling::TileExt1);
}

float JetVarTool::JetCaloSamplingTile2(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::TileBar2) +
    getEnergy (jet, CaloSampling::TileExt2);
}

float JetVarTool::JetCaloSamplingGap(const Jet *jet){
  return
    getEnergy (jet, CaloSampling::TileGap1) +
    getEnergy (jet, CaloSampling::TileGap2) +
    getEnergy (jet, CaloSampling::TileGap3);
}

// ignore additional weighting factor that is applied in the MET cryo
// term for now
float JetVarTool::JetCaloSamplingCryo(const Jet *jet){
  return sqrt( std::fabs(getEnergy(jet,CaloSampling::EMB3) * getEnergy(jet,CaloSampling::TileBar0) ) );
}

float JetVarTool::JetCaloSamplingTotal(const Jet *jet){

   return (JetCaloSamplingPS(jet) +
	   JetCaloSamplingEM(jet) +
	   JetCaloSamplingHEC(jet) +
	   JetCaloSamplingFCAL(jet) +
	   JetCaloSamplingTile(jet) +
	   JetCaloSamplingGap(jet) );
}

float JetVarTool::JetPtWeightedEventEMfraction(const JetCollection *jetcoll) {

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
    ev_emf = MET::MagicNumber;
  }
  return ev_emf;
}

float JetVarTool::JetPtWeightedEventEMfraction() {
  return JetPtWeightedEventEMfraction(m_jetColl);
}

float JetVarTool::JetChargeFraction(const Jet *jet) {
   return JetChargeFraction(jet, m_trackParticles);
}

float JetVarTool::JetChargeFraction(const Jet *jet, const Rec::TrackParticleContainer *trackParticles) {

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

int JetVarTool::JetNumAssociatedTracks(const Jet *jet) {
   return JetNumAssociatedTracks(jet, m_trackParticles);
}

int JetVarTool::JetNumAssociatedTracks(const Jet *jet, const Rec::TrackParticleContainer *trackParticles) {

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

float JetVarTool::JetPtWeightedNumAssociatedTracks(const JetCollection *jetcoll, const Rec::TrackParticleContainer *trackParticles) {

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
    ev_numtrks = MET::MagicNumber;
  }
  return ev_numtrks;
}

float JetVarTool::JetPtWeightedNumAssociatedTracks() {
  return JetPtWeightedNumAssociatedTracks(m_jetColl, m_trackParticles);
}

float JetVarTool::JetPtWeightedSize(const JetCollection *jetcoll) {
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
    ev_jetsize = MET::MagicNumber;
  }
  return ev_jetsize;
}

int JetVarTool::lowestN90CellsJet() {
  return lowestN90CellsJet(m_jetColl);
}

int JetVarTool::lowestN90CellsJet(const JetCollection *jetcoll) {
  int lowestN90 = 9999;

  for (JetCollection::const_iterator jetItr = jetcoll->begin();
       jetItr != jetcoll->end(); ++jetItr) {

    int nlead = nLeadingCells(*jetItr,0.9);
    if (nlead < lowestN90) {
      lowestN90 = nlead;
    }
  }
  if(lowestN90==9999) return MET::MagicNumber;
  return lowestN90;
}

float JetVarTool::JetPtWeightedSize() {
  return JetPtWeightedSize(m_jetColl);
}

float JetVarTool::leadingJetEt(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (leadingJet(jet ,jetcoll)) {return jet->et();}
  else {return MET::MagicNumber;}
}
float JetVarTool::leadingJetEta(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (leadingJet(jet ,jetcoll)) {return jet->eta();}
  else {return MET::MagicNumber;}
}

float JetVarTool::leadingJetPhi(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (leadingJet(jet ,jetcoll)) {return jet->phi();}
  else {return MET::MagicNumber;}
}

float JetVarTool::secondJetEt(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (secondJet(jet ,jetcoll)) {return jet->et();}
  else {return MET::MagicNumber;}
}

float JetVarTool::secondJetEta(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (secondJet(jet ,jetcoll)) {return jet->eta();}
  else {return MET::MagicNumber;}
}

float JetVarTool::secondJetPhi(const JetCollection *jetcoll) {
  const Jet *jet = 0;
  if (secondJet(jet ,jetcoll)) {return jet->phi();}
  else {return MET::MagicNumber;}
}

bool JetVarTool::leadingJet() const {
  const Jet *leadJet = 0; 
  return leadingJet(leadJet ,m_jetColl);
}

bool JetVarTool::leadingJet(const Jet * & leadJet ) const {
   return leadingJet(leadJet ,m_jetColl);
}

bool JetVarTool::leadingJet(const Jet * & leadJet ,const JetCollection *jetcoll) const {

  float leadingJetEt = MET::MagicNumber;

  leadJet=0;
  for (JetCollection::const_iterator jetItr = jetcoll->begin(); jetItr != jetcoll->end(); ++jetItr) {
    if ((*jetItr)->et() > leadingJetEt) {
      leadingJetEt = (*jetItr)->et();
      leadJet=(*jetItr);
    }//new highest et jet
  }//loop over jets

  
  return (leadingJetEt>0);
}

Jet JetVarTool::GetLeadingJet() const {
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


bool JetVarTool::secondJet() const{
  const Jet *secjet=0;
  return secondJet(secjet, m_jetColl);
}

bool JetVarTool::secondJet(const Jet * & secJet) const {
   return secondJet(secJet ,m_jetColl) ;
}

bool JetVarTool::secondJet(const Jet * & secJet ,const JetCollection *jetcoll) const {

  // first find leading jet.
  secJet=0;
  const Jet * leadJet=0;
  if (!leadingJet(leadJet ,jetcoll)){
     return false;
  }
  if (leadJet==0){
     return false;
  }
  
  float JetEt = MET::MagicNumber;
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

float JetVarTool::leadingJetEt() {
  return leadingJetEt(m_jetColl);
}

float JetVarTool::leadingJetEta() {
  return leadingJetEta(m_jetColl);
}

float JetVarTool::leadingJetPhi() {
  return leadingJetPhi(m_jetColl);
}

float JetVarTool::secondJetEt() {
  return secondJetEt(m_jetColl);
}

float JetVarTool::secondJetEta() {
  return secondJetEta(m_jetColl);
}

float JetVarTool::secondJetPhi() {
  return secondJetPhi(m_jetColl);
}


double 
JetVarTool::jetWLArQuality(const Jet* jet)
{
  double totE(0);
  double badE(0);
  
  // We have to navigate right back to cells in this case
  NavigationToken<CaloCell,double> cellToken;
  jet->fillToken(cellToken,double(1.));
  
  if ( cellToken.size() > 0 )
    {
      NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
      NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();

      for(; firstCell != lastCell; ++firstCell)
        {
	  const CaloCell* theCell = (*firstCell); 
          /*
	  if(theCell->caloDDE()->is_tile())
	    {
	      const TileCell* theTileCell = dynamic_cast<const TileCell*>(theCell);
	      if(theTileCell)
		{
		  totE += theCell->e();
		  int tileQuality = std::max(theTileCell->qual1(), theTileCell->qual2());
		  if(tileQuality > TileQualityCut)
		    badE += theCell->e();
		}
	    }
          */
	  if ( !theCell->caloDDE()->is_tile() )
	    {
	      totE += theCell->e();
              badE += theCell->quality() * theCell->e();
	    }
	}
    }

  if(totE > 0)
    return (badE/totE);
  else
    return 0;
}


double 
JetVarTool::jetWTileQuality(const Jet* jet)
{
  // Calculate the fraction of energy in cells flagged with a 'bad' quality
  // The cuts used are fairly approximative, and are used to flag obviously problematic cells
  // The quality factor might have an energy/gain dependence, so choosing a single cut
  // is surely a coarse approximation...
  
  double totE(0);
  double badE(0);
  
  // We have to navigate right back to cells in this case
  NavigationToken<CaloCell,double> cellToken;
  jet->fillToken(cellToken,double(1.));
  
  if ( cellToken.size() > 0 )
    {
      NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
      NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
      for(; firstCell != lastCell; ++firstCell)
        {
	  const CaloCell* theCell = (*firstCell); 

	  if(theCell->caloDDE()->is_tile())
	    {
	      const TileCell* theTileCell = dynamic_cast<const TileCell*>(theCell);
	      if(theTileCell)
		{
		  totE += theCell->e();
		  int tileQuality = std::max(theTileCell->qual1(), theTileCell->qual2());
		  badE += double(tileQuality) * theCell->e();
		}
	    }
	}
    }
  if(totE > 0)
    return (badE/totE);
  else
    return 0;
}


/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloCondPhysAlgs/CaloCellNoiseAlg.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IToolSvc.h"
#include "CaloIdentifier/CaloGain.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "xAODEventInfo/EventInfo.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
//=== AttributeList
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"


using CLHEP::HepMatrix;
using CLHEP::HepVector;


//Constructor
CaloCellNoiseAlg::CaloCellNoiseAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_thistSvc(NULL),
  m_calo_id(NULL),
  m_adc2mevTool("LArADC2MeVTool"),
  m_ncell(0),
  m_lumiblock(0),
  m_lumiblockOld(0),
  m_first(false),
  m_luminosity(0),
  m_nevt(),
  m_nevt_good(),
  m_layer(),
  m_identifier(),
  m_eta(),
  m_phi(),
  m_average(),
  m_rms(),
  m_reference(),
  m_tree(NULL),
  m_doMC(false),
  m_readNtuple(false),
  m_doFit(true),
  m_doLumiFit(true),
  m_nmin(10),
  m_trigDecTool(""),
  m_triggerChainProp(""),
  m_noiseToolDB(""),
  m_addlumiblock(5),
  m_deltaLumi(0.05)
{
  std::vector<float> data(CaloSampling::Unknown);
  data[CaloSampling::PreSamplerB] = 300.;
  data[CaloSampling::EMB1] = 100.;
  data[CaloSampling::EMB2] = 400.;
  data[CaloSampling::EMB3] = 200.;
  data[CaloSampling::PreSamplerE] = 500.;
  data[CaloSampling::EME1] = 200.;
  data[CaloSampling::EME2] = 800.;
  data[CaloSampling::EME3] = 300.;
  data[CaloSampling::HEC0] = 2000.;
  data[CaloSampling::HEC1] = 2000.;
  data[CaloSampling::HEC2] = 2000.;
  data[CaloSampling::HEC3] = 2000.;
  data[CaloSampling::TileBar0] = 9999.;
  data[CaloSampling::TileBar1] = 9999.;
  data[CaloSampling::TileBar2] = 9999.;
  data[CaloSampling::TileExt0] = 9999.;
  data[CaloSampling::TileExt1] = 9999.;
  data[CaloSampling::TileExt2] = 9999.;
  data[CaloSampling::TileGap1] = 9999.;
  data[CaloSampling::TileGap2] = 9999.;
  data[CaloSampling::TileGap3] = 9999.;
  data[CaloSampling::FCAL0]=10000.;
  data[CaloSampling::FCAL1]=10000.;
  data[CaloSampling::FCAL2]=10000.;
                                      
  m_cuts.setValue(data);
  declareProperty("doMC",m_doMC);
  declareProperty("readNtuple",m_readNtuple);
  declareProperty("doFit",m_doFit);
  declareProperty("nevtMin",m_nmin);
  declareProperty("ADC2MeVTool",m_adc2mevTool);
  declareProperty("doLumiFit",m_doLumiFit);
  declareProperty("TrigDecisionTool", m_trigDecTool );
  declareProperty("TriggerChain", m_triggerChainProp );
  declareProperty("EnergyCuts",m_cuts);
  declareProperty("noiseTool",m_noiseToolDB,"noise tool from DB");
  declareProperty("LumiFolderName",m_lumiFolderName="/TRIGGER/LUMI/LBLESTONL");
  declareProperty("NAddLumiBlock",m_addlumiblock,"Number of consecutive lumiblocks to add together ");
  declareProperty("DeltaLumi",m_deltaLumi);
}

//__________________________________________________________________________
//Destructor
CaloCellNoiseAlg::~CaloCellNoiseAlg()
{
  ATH_MSG_DEBUG ( "CaloCellNoiseAlg destructor called" );
}
//__________________________________________________________________________
StatusCode CaloCellNoiseAlg::initialize()
{
  ATH_MSG_DEBUG ("CaloCellNoiseAlg initialize()" );

  ATH_CHECK( detStore()->retrieve( m_caloIdMgr ) );
  m_calo_id      = m_caloIdMgr->getCaloCell_ID();

  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );

  if (m_doMC) {
    ATH_CHECK( detStore()->regHandle(m_dd_noise,"") );
  }
  else {
    ATH_CHECK( detStore()->regHandle(m_dd_pedestal,"") );
  }

  ATH_CHECK( m_adc2mevTool.retrieve() );

  m_first = true;
  m_lumiblock = 0;
  m_luminosity = 0.;
  m_lumiblockOld = 0;

  ATH_CHECK( service("THistSvc",m_thistSvc) );

  if( !m_trigDecTool.empty() ) {
    ATH_CHECK( m_trigDecTool.retrieve() );
    ATH_MSG_INFO ( "  --> Found AlgTool TrigDecisionTool" );
  }

  ATH_CHECK( m_noiseToolDB.retrieve() );

  ATH_MSG_INFO ( " end of CaloCellNoiseAlg::initialize " );
  return StatusCode::SUCCESS; 
}
//__________________________________________________________________________
StatusCode CaloCellNoiseAlg::stop()
{
  ATH_MSG_INFO ("CaloCellNoiseAlg stop()" );

  if (!m_readNtuple) {
    if (this->fillNtuple().isFailure()) {
      ATH_MSG_WARNING ( " failed to fill ntuple " );
      return StatusCode::SUCCESS;
    }
  }
  else {
    if (this->readNtuple().isFailure()) {
      ATH_MSG_WARNING ( " failed to read ntuple " );
      return StatusCode::SUCCESS;
    }
  }

  if (m_doFit) {
    if (this->fitNoise().isFailure()) {
      ATH_MSG_WARNING ( " failed to perform fit " );
       return StatusCode::SUCCESS;
    }
  }

  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode CaloCellNoiseAlg::execute()
{
  if (m_readNtuple) return StatusCode::SUCCESS;

// check trigger
  if ( (m_triggerChainProp != "")  && (!m_trigDecTool.empty()) ) {

    if (m_first) {
      ATH_MSG_INFO ( " L1 items : " << m_trigDecTool->getChainGroup("L1_.*")->getListOfTriggers() );
    }
    bool passTrig = false;
    if (m_triggerChainProp != "") passTrig = m_trigDecTool->isPassed(m_triggerChainProp);

    if (!passTrig) {
      ATH_MSG_DEBUG ( " Failed trigger selection " );
     return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_DEBUG ( " Pass trigger selection " );
    }

  }

  const xAOD::EventInfo* eventInfo = nullptr;
  if (evtStore()->retrieve(eventInfo).isFailure()) {
    ATH_MSG_WARNING ( " Cannot access to event info " );
    return StatusCode::SUCCESS;
  }
  unsigned int lumiblock = eventInfo->lumiBlock();

  ATH_MSG_DEBUG ( " lumiblock " << lumiblock );

  if ( lumiblock != m_lumiblockOld) {
    float luminosity =  this->getLuminosity();
    ATH_MSG_INFO ( " New lumiblock seen " << lumiblock << " " << m_lumiblock << " " << luminosity << " " << m_luminosity <<  " m_first " << m_first );
    // lumiblock, m_lumiblock are unsigned.
    if ( ( lumiblock - m_lumiblock >= static_cast<unsigned int>(m_addlumiblock) ||
           std::fabs(luminosity-m_luminosity)>(m_luminosity*m_deltaLumi) ||
           m_first )
         && m_doLumiFit)
    {
      if (!m_first) {
        ATH_MSG_INFO ( " filling ntuple for lumiblock " << m_lumiblock << " until  " << lumiblock << " (excluded) " );
         if (this->fillNtuple().isFailure()) {
           ATH_MSG_WARNING ( " failed to fill ntuple " );
           return StatusCode::SUCCESS;
         }
      }
      m_lumiblock = lumiblock;
      m_luminosity = luminosity;
    }
    m_lumiblockOld = lumiblock;
  }

  if (m_first) {
    m_ncell = m_calo_id->calo_cell_hash_max();
    ATH_MSG_DEBUG ( " number of cells " << m_ncell );
    if (m_ncell>200000) {
      ATH_MSG_WARNING ( " too many celly " << m_ncell );
      return StatusCode::SUCCESS;
    }
    m_CellList.reserve(m_ncell);

    for (int i=0;i<m_ncell;i++) {
     IdentifierHash idHash=i;
     Identifier id=m_calo_id->cell_id(idHash);
     const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);
     CellInfo cell0;
     cell0.nevt = 0;
     cell0.average = 0.;
     cell0.rms = 0.; 
     cell0.identifier = id.get_identifier32().get_compact();
     cell0.sampling = m_calo_id->calo_sample(id);
     cell0.eta = calodde->eta();
     cell0.phi = calodde->phi();
     cell0.nevt_good=0;
     if (!m_noiseToolDB.empty()) {
          CaloGain::CaloGain gain;
          if (m_calo_id->is_tile(id)) {
              if (m_calo_id->calo_sample(id) == CaloSampling::TileGap3) gain=CaloGain::TILEONEHIGH;
              else gain=CaloGain::TILEHIGHHIGH;
          }
          else {
              if(m_calo_id->is_hec(id)) gain=CaloGain::LARMEDIUMGAIN;
              else gain=CaloGain::LARHIGHGAIN;
          }
          cell0.reference = m_noiseToolDB->totalNoiseRMS(calodde,gain,-1);
     }
     else{
       cell0.reference=0.;
     }
     m_CellList.push_back(cell0);
    }

    m_tree = new TTree("mytree","Calo Noise ntuple");
    m_tree->Branch("luminosity",&m_luminosity,"luminosity/F");
    m_tree->Branch("ncell",&m_ncell,"ncell/I");
    m_tree->Branch("identifier",m_identifier,"identifier[ncell]/I");
    m_tree->Branch("layer",m_layer,"layer[ncell]/I");
    m_tree->Branch("eta",m_eta,"eta[ncell]/F");
    m_tree->Branch("phi",m_phi,"phi[ncell]/F");
    m_tree->Branch("nevt",m_nevt,"nevt[ncell]/I");
    m_tree->Branch("nevt_good",m_nevt_good,"nevt[ncell]/I");
    m_tree->Branch("average",m_average,"average[ncell]/F");
    m_tree->Branch("rms",m_rms,"rms[ncell]/F");
    m_tree->Branch("reference",m_reference,"reference[ncell]/F");

    if( m_thistSvc->regTree("/file1/calonoise/mytree",m_tree).isFailure()) {
      ATH_MSG_WARNING ( " cannot register ntuple " );
      return StatusCode::SUCCESS;
    }

    m_first = false;
  } 


  const CaloCellContainer* cell_container;
  if(evtStore()->retrieve(cell_container,"AllCalo").isFailure()) {
    ATH_MSG_WARNING( " Could not get pointer to Cell Container " );
    return StatusCode::SUCCESS;
  }

   CaloCellContainer::const_iterator first_cell = cell_container->begin();
   CaloCellContainer::const_iterator end_cell   = cell_container->end();
   ATH_MSG_DEBUG ( "*** Start loop over CaloCells " );
   for (; first_cell != end_cell; ++first_cell)
   {

        Identifier cellID = (*first_cell)->ID();
        CaloGain::CaloGain gain =  (*first_cell)->gain();

        if (gain==CaloGain::LARHIGHGAIN || gain ==CaloGain::TILEHIGHHIGH ||
             (gain==CaloGain::TILEONEHIGH && m_calo_id->calo_sample(cellID) == CaloSampling::TileGap3) ||
             (gain==CaloGain::LARMEDIUMGAIN && m_calo_id->is_hec(cellID)) ) {

          double energy=  (*first_cell)->energy();

          int isample = m_calo_id->calo_sample(cellID);
          if (std::fabs(energy) < m_cuts.value()[isample] ) {

            IdentifierHash idhash = m_calo_id->calo_cell_hash(cellID);
            int index = (int) (idhash);

            double oldN = (double) (m_CellList[index].nevt);
            double oldAverage = m_CellList[index].average;
            double oldRMS     = m_CellList[index].rms;

            double frac = oldN/(1.+oldN);
            double Anew = 1.+oldN;
            double newAverage = frac*oldAverage + energy/Anew;
            double deltaE = (energy-oldAverage);
            double newRMS     = frac*(oldRMS + deltaE*deltaE/Anew);

            m_CellList[index].nevt ++;
            m_CellList[index].average = newAverage;
            m_CellList[index].rms = newRMS;

            if (!((*first_cell)->badcell())) m_CellList[index].nevt_good++;
          }
      }
   }

  return StatusCode::SUCCESS;
}

//---------------------------------------------
float  CaloCellNoiseAlg::getLuminosity()
{
  ATH_MSG_INFO ( " in getLuminosity() " );

  float luminosity = 0.;
  const CondAttrListCollection* attrListColl = 0;
  StatusCode sc = evtStore()->retrieve(attrListColl, m_lumiFolderName);
  if (sc.isFailure() || !attrListColl) {
    ATH_MSG_WARNING  ( "attrrListColl not found for " << m_lumiFolderName );
    return luminosity;
  }
  // Loop over collection
  CondAttrListCollection::const_iterator first = attrListColl->begin();
  CondAttrListCollection::const_iterator last  = attrListColl->end();
  for (; first != last; ++first) {
   if ((*first).first == 0) {
      std::ostringstream attrStr1;
      (*first).second.toOutputStream( attrStr1 );
      ATH_MSG_DEBUG ( "ChanNum " << (*first).first <<
                      " Attribute list " << attrStr1.str() );
      const coral::AttributeList& attrList = (*first).second;
      luminosity = attrList["LBAvInstLumi"].data<float>() *1e-3;  // luminosity (from 10**30 units in db to 10*33 units)
      break;
   }
  }
  ATH_MSG_INFO ( " Luminosity (10**33 units) " << luminosity <<  "   (10**27 units) " << 1e+6*luminosity );
  return luminosity;

}

//____________________________________________
StatusCode CaloCellNoiseAlg::fillNtuple()
{
  ATH_MSG_INFO ( "  in fillNtuple " );

 for (int i=0;i<m_ncell;i++) {
   m_identifier[i] = m_CellList[i].identifier;
   m_layer[i] = m_CellList[i].sampling;
   m_eta[i] = m_CellList[i].eta;
   m_phi[i] = m_CellList[i].phi;
   m_nevt[i] = m_CellList[i].nevt;
   m_nevt_good[i] = m_CellList[i].nevt_good;
   m_average[i] = (float) (m_CellList[i].average);
   m_rms[i] = (float) (sqrt(m_CellList[i].rms));
   m_reference[i] = (float) (m_CellList[i].reference);
   ATH_MSG_DEBUG ( " hash,Nevt,Average,RMS " << i << " " << m_nevt[i] << " " << m_average[i] << " " << m_rms[i] );
 }
 m_tree->Fill();

 for (int i=0;i<m_ncell;i++) {
  m_CellList[i].nevt=0;
  m_CellList[i].nevt_good=0;
  m_CellList[i].average=0;
  m_CellList[i].rms=0;
 }


 return StatusCode::SUCCESS;
}

//______________________________________________
StatusCode CaloCellNoiseAlg::readNtuple() 
{
 return StatusCode::SUCCESS;
}

//_________________________________________________
StatusCode CaloCellNoiseAlg::fitNoise()
{
 ATH_MSG_INFO ( " in  CaloCellNoiseAlg::fitNoise() " );

 FILE* fp = fopen("calonoise.txt","w");

 TBranch* b1 = m_tree->GetBranch("luminosity");
 TBranch* b2 = m_tree->GetBranch("nevt");
 TBranch* b3 = m_tree->GetBranch("average");
 TBranch* b4 = m_tree->GetBranch("rms");
 TBranch* b5 = m_tree->GetBranch("nevt_good");
 b1->SetAddress(&m_luminosity);
 b2->SetAddress(&m_nevt);
 b3->SetAddress(&m_average);
 b4->SetAddress(&m_rms);
 b5->SetAddress(&m_nevt_good);
 int nentries = m_tree->GetEntries();
 ATH_MSG_DEBUG ( " Number of entries in ntuple " << nentries );

 std::vector<float> anoise;
 std::vector<float> bnoise;
 anoise.resize(m_ncell,0.);
 bnoise.resize(m_ncell,0.);

 for (int icell=0;icell<m_ncell;icell++) {
   std::vector<float> x;
   std::vector<float> y;
   std::vector<float> ey;

   for (int i=0;i<nentries;i++) {
     b1->GetEntry(i);
     b2->GetEntry(i);
     b4->GetEntry(i);

     if (m_nevt[icell]>m_nmin) {
        x.push_back(sqrt(m_luminosity));
        y.push_back(m_rms[icell]);
        ey.push_back(m_rms[icell]/sqrt(2.*m_nevt[icell]));
     }
   }

   if (x.size()<2) {
     if (x.size()==1) anoise[icell]=y[0];
   }
   else {
     HepMatrix alpha(2,2);
     HepVector beta(2);

     for (unsigned int i=0;i<2;i++) {
       for (unsigned int j=0;j<2;j++) {
         alpha[i][j]=0.;
         for (unsigned int k=0;k<x.size();k++) {
            alpha[i][j] += ((pow(x[k],i))*(pow(x[k],j))/(pow(ey[k],2)));
         }
       }
     }
     for (unsigned int i=0;i<2;i++) {
         beta[i]=0.;
         for (unsigned int k=0;k<x.size();k++) {
           beta[i] += (y[k]*(pow(x[k],i))/(pow(ey[k],2)));
         }
     }
     HepVector comp=solve(alpha,beta);
     anoise[icell] = comp[0];
     bnoise[icell] = comp[1];

   }
   if (msgLvl(MSG::DEBUG)) {
     msg() << MSG::DEBUG  << " cell " << icell << "  lumi/noise ";
     for (unsigned int i=0;i<x.size();i++) {
       msg() << MSG::DEBUG  << x[i] << " " << y[i] << " / ";
     }
     msg() << MSG::DEBUG  << " " << endmsg;
     ATH_MSG_DEBUG  ( "     fitted a,b  " << anoise[icell] << " " << bnoise[icell] );
   }   
 }   // end first loop over cells to store anoise and bnoise

// for LAR try phi patching for missing cells, just to be sure that DB is filled with reasonnable entries in case
//   the cell come back to life
 for (int icell=0;icell<m_ncell;icell++) {
    if (anoise[icell]<3. || m_nevt_good[icell]==0) {
       IdentifierHash idHash = icell;
       Identifier id=m_calo_id->cell_id(idHash);
       if (m_calo_id->is_lar(id)) {
          ATH_MSG_DEBUG ( " cell with no noise found in LAr " << m_calo_id->show_to_string(id) );
          Identifier regionId = m_calo_id->region_id(id);
          int eta=m_calo_id->eta(id); 
          int phimin = m_calo_id->phi_min(regionId);
          int phimax = m_calo_id->phi_max(regionId);
          int nring=0;
          float sum=0.;
          ATH_MSG_DEBUG ( " regionId,eta,phimin,phimax " << regionId << " " << eta << " " << phimin << " " << phimax );
          for (int phi=phimin;phi<=phimax;phi++) {
            Identifier id2 = m_calo_id->cell_id(regionId,eta,phi);
            if (id2.is_valid()) {
               if(msgLvl(MSG::DEBUG))
                 msg() << MSG::DEBUG << "     cell in ring " << m_calo_id->show_to_string(id2) ;
               IdentifierHash idHash2 = m_calo_id->calo_cell_hash(id2);
               int index = (int)(idHash2);
               if (index>=0 && index<m_ncell) {
                 ATH_MSG_DEBUG( " noise " << anoise[index] );
                 if (anoise[index]>3. && m_nevt_good[index]>0) {
                    nring+=1;
                    sum+=anoise[index];
                 }
               }
            }
          }
          if (nring>0) {
             float patched_noise = sum/((float)(nring));
             if (patched_noise>anoise[icell]) anoise[icell] = patched_noise;
          }
          ATH_MSG_DEBUG( " corrected noise nring, anoise[icell] " << nring << " " << anoise[icell] );
       }
    }
 }


// extrapolate to other gains and write result
 for (int icell=0;icell<m_ncell;icell++) {
   IdentifierHash idHash = icell;
   Identifier id=m_calo_id->cell_id(idHash);
   int subCalo;
   IdentifierHash idSubHash = m_calo_id->subcalo_cell_hash (idHash, subCalo);
   const CaloDetDescrElement* calodde = m_calodetdescrmgr->get_element(id);

   int iCool=-1;
   if (m_calo_id->is_em(id)) {    // EM calo
        if (m_calo_id->is_em_barrel(id)) {
            if (m_calo_id->pos_neg(id) > 0 )
               iCool=2;
            else
               iCool=1;
        }
        if (m_calo_id->is_em_endcap(id)) {
            if (m_calo_id->pos_neg(id) > 0 )
               iCool=3;
            else
               iCool=0;
        }

   }
   if (m_calo_id->is_hec(id)) {   // HEC
       iCool=16;
   }
   if (m_calo_id->is_fcal(id)) {   // Fcal
        iCool=32;
   }
   if (m_calo_id->is_tile(id)) {   // Tile
        iCool=48;
   }
   int ii = (int) (idSubHash);


// if LAr
   if (m_calo_id->is_lar(id)) {
      for (int igain=0;igain<3;igain++) {
        CaloGain::CaloGain gain=CaloGain::UNKNOWNGAIN;
        if (igain==0) gain=CaloGain::LARHIGHGAIN;
        if (igain==1) gain=CaloGain::LARMEDIUMGAIN;
        if (igain==2) gain=CaloGain::LARLOWGAIN;

// extrapolate electronic noise part (a term) using ratio of electronic noise values
//  for HEC start from medium gain as reference
        CaloGain::CaloGain gainref = CaloGain::LARHIGHGAIN;
        if (m_calo_id->is_hec(id)) gainref=CaloGain::LARMEDIUMGAIN;

        float anoise_corr=anoise[icell];

        if (gain != gainref) {

// noise and ADC2MeV in gain ref
          float noise0=-1.;
          if (m_doMC) noise0 = m_dd_noise->noise(id,gainref);
          else {
           float noise = m_dd_pedestal->pedestalRMS(id,gainref);
           if (noise>= (1.0+LArElecCalib::ERRORCODE)) noise0 = noise;
          }
          const std::vector<float> *
            polynom_adc2mev0 = &(m_adc2mevTool->ADC2MEV(id,gainref));
          float adc2mev0=-1;
          if (polynom_adc2mev0->size()>1) adc2mev0=(*polynom_adc2mev0)[1];

// noise and ADC2MeV in gain

          float noise1=-1;
          if (m_doMC) noise1 = m_dd_noise->noise(id,gain);
          else {
             float noise = m_dd_pedestal->pedestalRMS(id,gain);
             if (noise>= (1.0+LArElecCalib::ERRORCODE)) noise1 = noise;
          }
          const std::vector<float> *
            polynom_adc2mev1 = &(m_adc2mevTool->ADC2MEV(id,gain));
          float adc2mev1=-1;
          if (polynom_adc2mev1->size()>1) adc2mev1=(*polynom_adc2mev1)[1];

          //if (m_calo_id->is_hec(id))
          //  std::cout << " Hec noise1,noise0,adc2mev1,adc2mev0 " << noise1 << " " << noise0 << " " << adc2mev1 << " " << adc2mev0 << std::endl;

          if (noise0>0 && noise1>0 && adc2mev0>0 && adc2mev1>0.) {
            anoise_corr = anoise[icell]*noise1/noise0 * adc2mev1/adc2mev0;
          }
        }

         //if (m_calo_id->is_hec(id)) 
         //  std::cout << " Hec cell  gain,noise " << igain << " " << anoise_corr << std::endl;

        if (!m_noiseToolDB.empty()) {
          float adb = m_noiseToolDB->elecNoiseRMS(calodde,gain,-1);

          // if no correct noise, use reference instead
          if (anoise_corr<1. && adb>1e-6) { 
            anoise_corr = adb;
            ATH_MSG_WARNING ( " No noise found for cell: " <<m_calo_id->show_to_string(id) << " gain: " << gain << " Use reference value: " << adb );
          }
          if (adb>1e-6) {
            float delta = std::fabs((anoise_corr-adb)/adb);
            if (delta>0.30 && gain==CaloGain::LARHIGHGAIN)
              ATH_MSG_WARNING ( " Noise difference cell " << m_calo_id->show_to_string(id) << " gain: " << gain
                                << " computed " << anoise_corr  << " reference " << adb );
          }
        }
         

        fprintf(fp,"%5d %5d %5d %8.3f %8.3f\n",iCool,ii,gain,anoise_corr,bnoise[icell]);
      }
   }
// if Tile
   else {
      for (int igain=0;igain<4;igain++) {
          CaloGain::CaloGain gain=CaloGain::UNKNOWNGAIN;
          if (igain==0) gain=CaloGain::TILELOWLOW;
          if (igain==1) gain=CaloGain::TILELOWHIGH;
          if (igain==2) gain=CaloGain::TILEHIGHLOW;
          if (igain==3) gain=CaloGain::TILEHIGHHIGH;
          float anoise_corr = anoise[icell];
          if (igain>0) {
          }
          fprintf(fp,"%5d %5d %5d %8.3f %8.3f\n",iCool,ii,gain,anoise_corr,bnoise[icell]);
      }
   }

 }    // loop over cells

 fclose(fp);
 return StatusCode::SUCCESS;
}

StatusCode CaloCellNoiseAlg::finalize()
{
 return StatusCode::SUCCESS;
}

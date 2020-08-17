/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 #include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"
#include "Gaudi/Property.h"

#include "MissingETGoodness/JetGoodiesFiller.h"
#include "MissingETGoodness/MissingETUtils.h"
#include "MissingETGoodness/EtmissGoodness.h"
#include "MissingETEvent/MissingET.h"

//#include "JetUtils/JetCaloHelper.h"
#include "JetUtils/JetCaloQualityUtils.h"
//#include "JetUtils/JetSorters.h"
#include "FourMomUtils/P4Sorters.h"
#include "LArRecEvent/LArNoisyROSummary.h"
#include "JetEvent/JetMoment.h"
#include "JetEvent/JetConstituentIterator.h"
#include "CaloEvent/CaloSamplingHelper.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloEvent/CaloCellPrefetchIterator.h"
#include "AthenaKernel/Units.h"

using Athena::Units::GeV;

namespace {


// When we go to xAOD, see if we can get these from JetCaloQualityUtils


double getEnergy (const Jet* jet, CaloSampling::CaloSample samp)
{
  return jet->getShape ("energy_" + CaloSampling::getSamplingName (samp));
}


double hecF(const Jet* jet)
{
  double e_hec =
    getEnergy (jet, CaloSampling::HEC0) +
    getEnergy (jet, CaloSampling::HEC1) +
    getEnergy (jet, CaloSampling::HEC2) +
    getEnergy (jet, CaloSampling::HEC3);
  
  double e_jet = jet->getRawE();
  
  if(e_jet!=0) return  e_hec/e_jet;
  else return -999;
}

double fracSamplingMax(const Jet* jet, int& SamplingMax)
{
  double fracSamplingMax=-999999999.;    
  double sumE_samplings=0.;
  int nsamp = CaloSampling::getNumberOfSamplings();
  for ( int i(0); i < nsamp; ++i )
    {
      double e = getEnergy (jet, (CaloSampling::CaloSample)i);
      sumE_samplings+=e;
      if (e>fracSamplingMax)
	{
	  fracSamplingMax=e;
	  SamplingMax=i;
	}
    }

  if(sumE_samplings!=0)
    fracSamplingMax/=sumE_samplings;
  else fracSamplingMax=0;
  
  return fracSamplingMax;
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


double jetOutOfTimeEnergyFraction(const Jet* jet, const double timecut) {
  double sumE=0;
  double sumE_OOT=0;
  
  NavigationToken<CaloCell,double> cellToken;
  jet->fillToken(cellToken,double(1.));
  
  if ( cellToken.size() > 0 ) {
    NavigationToken<CaloCell,double>::const_iterator firstCell =  cellToken.begin();
    NavigationToken<CaloCell,double>::const_iterator lastCell  =  cellToken.end();
    for(; firstCell != lastCell; ++firstCell)	{
      // weight is not used
      // double weight = firstCell.getParameter();
      const CaloCell* thisCell = *firstCell;

      double  aCellE = thisCell->e();

      sumE += aCellE;
      
      // ensure timing information is defined
      bool timeIsDefined(false);
      
      if(thisCell->caloDDE()->is_tile()) {
        if ( (thisCell->provenance()) & 0x8080)  timeIsDefined = true;
      } else { //is_lar	    
        if ( (thisCell->provenance()) & 0x2000)  timeIsDefined = true;
      }

      if ( timeIsDefined ) {
        double time = thisCell->time();
        if(fabs(time)>timecut)  sumE_OOT+=thisCell->e();        
      }
    }
  }    

  return ( (sumE!=0) ? (sumE_OOT/sumE) : 0 );
}


double tileGap3F(const Jet* jet)
{
  double e_tileGap3 =getEnergy (jet, CaloSampling::TileGap3);
  double e_jet = jet->getRawE();

  if(e_jet!=0) return  e_tileGap3/e_jet;
  else return -999;
}


bool isBad(const Jet* jet)
{


  double chf = jet->getMoment("sumPtTrk")/jet->pt(P4SignalState::JETFINAL);
  
  double emf       = jetEMFraction(jet); 
  double hecf      = hecF(jet);
  int SamplingMax=CaloSampling::Unknown;
  double fmax = fracSamplingMax(jet,SamplingMax); 


  double jetQuality= -999.;
  double jetTime   = -999.;
  double jetHECQuality = -999.;
  double jetNegativeE = -999.;

  jetQuality= jet->getMoment("LArQuality"); 
  jetTime   = jet->getMoment("Timing"); 
  jetHECQuality = jet->getMoment("HECQuality");
  jetNegativeE = jet->getMoment("NegativeE");

  if( hecf>0.5 && fabs(jetHECQuality)>0.5 ) return true;
  if( fabs(jetNegativeE)/GeV > 60.) return true;

  if( emf>0.95 && fabs(jetQuality)>0.8 && fabs(jet->eta(P4SignalState::JETEMSCALE)) < 2.8) return true;

  if( fabs(jetTime)>10 ) return true;
  if( fmax > 0.99 && fabs(jet->eta(P4SignalState::JETEMSCALE)) < 2.0) return true;
  if( emf<0.05 && chf < 0.05 && fabs(jet->eta(P4SignalState::JETEMSCALE)) < 2.0) return true;  
  if( emf<0.05 && fabs(jet->eta(P4SignalState::JETEMSCALE)) >= 2.0) return true;  

  return false;

}


bool isUgly(const Jet* jet)
{

  double fcor = jet->getMoment("BCH_CORR_CELL");
  double tileGap3f = tileGap3F(jet);

  if (fcor>0.5) return true;
  if (tileGap3f >0.5) return true;

  return false;
}


bool isGood(const Jet* jet)
{

  return  !(isBad(jet)||isUgly(jet));
}


double jetTimeClusters(const Jet* jet)
{
  // Calculate the energy weighted event time for the jet

  // time and normalization
  double time(0);
  double norm(0);
  
  // If we find clusters, we use an approximation:
  // We weigh each cluster by its energy^2
  
  NavigationToken<CaloCluster,double> clusToken;
  jet->fillToken(clusToken,double(1.));
  
  if ( clusToken.size() > 0 )
    {
      // Use the constituent iterator
      JetConstituentIterator firstClus = JetConstituentIterator::first(jet);
      JetConstituentIterator lastClus = JetConstituentIterator::last(jet);
      
      for(; firstClus != lastClus; ++firstClus)
        {
	  // We perform a naive e^2 weighting over the cluster times and energies
	  const CaloCluster* theClus = dynamic_cast<const CaloCluster*>(firstClus.get_real_constit());
          if (theClus) {

            // For the moment, we have to force the weight to be 1, because of navigation
            // difficulties with the JetConstituentIterator
            //double weight(firstClus.get_real_constit().getParameter());
            double weight(1);

            double thisNorm = weight * fabs(weight) * firstClus.e() * firstClus.e();
            double thisTime = thisNorm * theClus->getTime();
          
            time += thisTime;
            norm += thisNorm;
          }
	  
	}
    }
  if(norm>0)
    return (time/norm);
  else
    return 0;
}


bool timeAndQualityDefined(const CaloCell* theCell){
  // ensure timing information is defined
  bool timeIsDefined(false);
  if(theCell->caloDDE()->is_tile())	  
    {
      if ( (theCell->provenance()) & 0x8080)
        timeIsDefined = true;
    }
  else //is_lar
    {
      if ( (theCell->provenance() & 0x2000) &&
           !(theCell->provenance()  & 0x0800 ) // do not take masked-patched cells int
           )         timeIsDefined = true;
    }
  return timeIsDefined;
}

double jetTimeCells(const Jet* jet)
{
  // Calculate the energy weighted event time for the jet

  // time and normalization
  double time(0);
  double norm(0);

  NavigationToken<CaloCell,double> cellToken;
  jet->fillToken(cellToken,double(1.));
  
  if ( cellToken.size() > 0 )
    {
      // Improve performance with a PrefetchIterator
      CaloCellPrefetchIterator<NavigationToken<CaloCell,double>::const_iterator> it (cellToken.begin(), cellToken.end());
      while (it.next())
	{
	  // Get weight and pointer to Cell
	  double weight = it.getParameter();
	  const CaloCell* theCell = (*it); 
	  
	  // ensure timing information is defined
	  bool timeIsDefined = timeAndQualityDefined(theCell);
          
	  if ( timeIsDefined )
	    {
	      double thisNorm = weight * fabs(weight) * theCell->e() * theCell->e();
	      double thisTime = thisNorm * theCell->time();
	      
	      time += thisTime;
	      norm += thisNorm;
	    }
	}
    }
  if(norm > 0)
    return (time/norm);
  else
    return 0;
}


double jetTime(const Jet* jet)
{

  // Provide a default not to break packages already written to depend
  // on this function.  We just check for existence of clusters/cells...

  NavigationToken<CaloCell,double> cellToken;
  jet->fillToken(cellToken,double(1.));

  NavigationToken<CaloCluster,double> clusToken;
  jet->fillToken(clusToken,double(1.));
  
  if(cellToken.size() != 0)
    return jetTimeCells(jet);
  else if(clusToken.size() != 0)
    return jetTimeClusters(jet);
  else return 0;
  
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


int nLeadingConstituents(const Jet* jet,double threshold)
{
  std::vector<double> constituent_energies;
  double sumE_constituents=0;
  JetConstituentIterator iterConst   = JetConstituentIterator::first(jet);
  JetConstituentIterator lConst      = JetConstituentIterator::last(jet);
  for ( ; iterConst != lConst; ++iterConst )
    {
      constituent_energies.push_back(iterConst.e());
      sumE_constituents+=iterConst.e();
    }
  return compute_nLeading(constituent_energies,sumE_constituents,threshold);
}


} // anonymous namespace

MET::Goodies& JetGoodiesFiller::s_goodies(MET::Goodies::instance());

JetGoodiesFiller::JetGoodiesFiller(const std::string& name, ISvcLocator* pSvcLocator)
   : AthAlgorithm(name, pSvcLocator) ,
     m_JetVariables ("JetVarTool")
{  
  declareProperty( "NamePrimaryJetAlg",   m_PrimaryJetAlg = "AntiKt4TopoJets");
  declareProperty( "NameSecondaryJetAlg", m_SecondaryJetAlg = "AntiKt4TopoJets" ); 
  declareProperty( "NumberOfJetsToLookAt", m_NumberOfJets = 2);
  declareProperty( "LArNoisyROSummary", m_LArNoisyROSummary="LArNoisyROSummary");
}

JetGoodiesFiller::~JetGoodiesFiller(){ 
} 


// merge in my stuff
// merge in MuonSp stuff
void JetGoodiesFiller::initGoodiesMap()
{
   ATH_MSG_DEBUG ("execute initGoodiesMap()");

   s_goodies.setValue("LArNoisyROSummary",(float)MET::MagicNumber);
   s_goodies.setValue("EventEMFraction",(float)MET::MagicNumber);

   s_goodies.gInt()["N_Jets"] = (int)MET::MagicNumber;
   s_goodies.gVectorDouble()["Phi_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["PtCal_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["DeltaPhi_MET_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["Eta_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["DeltaEt_JetAlgs_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["DeltaPhi_JetAlgs_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["DeltaEta_JetAlgs_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracPS_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracEM_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracFCAL_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracHEC_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracHEC3_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracTile_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracTile2_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracTile10_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracGap_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracCryo_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["SamplingFracMax_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["HECf_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["EMFraction_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["ChargeFraction_Jet"]= std::vector<double>();
   s_goodies.gVectorDouble()["LArQuality_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["TileQuality_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["QualityFrac_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["Timing_Jet"]= std::vector<double>();
   s_goodies.gVectorDouble()["ootE25_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["ootE50_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["ootE75_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["E_BadCells_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["E_BadCellsCorr_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["BCH_CORR_CELL_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["BCH_CORR_JET_FORCELL_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["BCH_CORR_JET"] = std::vector<double>();
   s_goodies.gVectorDouble()["ECal_Jet"] = std::vector<double>();
   s_goodies.gVectorDouble()["tileGap3F_Jet"] = std::vector<double>();

   s_goodies.gVectorInt()["isGood_Jet"] = std::vector<int>();
   s_goodies.gVectorInt()["isBad_Jet"]  = std::vector<int>();
   s_goodies.gVectorInt()["isUgly_Jet"] = std::vector<int>();
   
   s_goodies.gVectorInt()["NumTracks_Jet"]= std::vector<int>();
   s_goodies.gVectorInt()["N90Cells_Jet"]= std::vector<int>();
   s_goodies.gVectorInt()["N90Constituents_Jet"] = std::vector<int>();
   s_goodies.gVectorInt()["NConstituents_Jet"]= std::vector<int>();
   s_goodies.gVectorInt()["N_BadCells_Jet"] = std::vector<int>();
   s_goodies.gVectorInt()["N_BadCellsCorr_Jet"] = std::vector<int>();
   s_goodies.gVectorInt()["SamplingMaxID_Jet"] = std::vector<int>();
   
}

StatusCode JetGoodiesFiller::initialize() {
   ATH_MSG_DEBUG ("initialize()");
   return StatusCode::SUCCESS;
}


StatusCode JetGoodiesFiller::execute() {
   StatusCode sc = StatusCode::SUCCESS;
   ATH_MSG_DEBUG ("execute JetGoodiesFiller()");
   initGoodiesMap();

   // todo: should be moved to misc goodies filler
   const LArNoisyROSummary* noisyRO(0); // = new LArNoisyROSummary();
   if ( evtStore()->contains<LArNoisyROSummary>(m_LArNoisyROSummary) ) {
     sc = evtStore()->retrieve(noisyRO, m_LArNoisyROSummary);
     if ( !sc.isSuccess() ){
       ATH_MSG_WARNING("JetGoodiesFiller retrieve LArNoisyROSummary container() Failed");
       return sc;
     }
   }

   s_goodies.setValue("LArNoisyROSummary", (noisyRO!=0 ? (double)noisyRO->get_noisy_febs().size() : (double)MET::MagicNumber) );
   
   sc = m_JetVariables->retrieveContainers();
   if ( !sc.isSuccess() ) {
      ATH_MSG_WARNING("JetGoodiesFiller retrieve containers() Failed");
      return sc;
   }
   s_goodies.setValue("EventEMFraction", m_JetVariables->EventEMFraction());
   
   //MET::EtmissGoodness mGood;   
   //MET::EtmissGoodnessManager& manager = MET::EtmissGoodnessManager::instance();
   std::string metName("MET_Topo");
   //std::string metName(manager.GetMETKey()); //crashing here
   const MissingET* MET = 0;
   sc = evtStore()->retrieve( MET, metName );
   if ( !sc.isSuccess() ) {
     ATH_MSG_WARNING("Couldn't retrieve MET collection "<<metName);
     return sc;
   }
   
   //std::string jetName(manager.GetJetKey()); //crashing here
   std::string jetName(m_PrimaryJetAlg); //REMOVE HACK
   sc = m_JetVariables->retrieveJetContainer(jetName);
   if ( !sc.isSuccess() ) {
      ATH_MSG_ERROR("JetGoodiesFiller retrieve jet containers() Failed");
      return sc;
   }
   const JetCollection*  otherJets = 0;
   //this was store gate
   if ( evtStore()->contains<JetCollection>( m_SecondaryJetAlg )) {
     sc = evtStore()->retrieve( otherJets, m_SecondaryJetAlg );
     if( !sc.isSuccess() ) {
       ATH_MSG_WARNING("Other JetCollection not found in StoreGate, key:" << m_SecondaryJetAlg);
       return sc;
     }
   }

   // now fill s_goodies map per jet
   const JetCollection* constJets = m_JetVariables->jetCollection();
   JetCollection jets;
   for (JetCollection::const_iterator it = constJets->begin(); it != constJets->end(); ++it) {
      jets.push_back(new Jet(*it));
   }//loop over reco jets   
   //sort reco jets
   jets.sort(P4Sorters::Descending::Et());

   JetCollection::iterator first = jets.begin();
   JetCollection::iterator last = jets.end();
 
   int njets = 0;

   for(int jetCount = 0; first != last; ++first, ++jetCount){
     //if(jetCount >= m_NumberOfJets) break;
     const Jet* jet = *first;
     
     double pxcal=jet->getCalPx(); 
     double pycal=jet->getCalPy();
     double ptcal=sqrt((pxcal*pxcal)+(pycal*pycal));
     double ecal=jet->getCalE();

     if (ptcal<20000) continue;

     njets++;
     
     s_goodies.setVectorValue(TString("ECal_Jet"), ecal );
     s_goodies.setVectorValue(TString("PtCal_Jet"), ptcal );

     s_goodies.setVectorValue(TString("Phi_Jet"), jet->phi());
     s_goodies.setVectorValue(TString("DeltaPhi_MET_Jet"), MissingETUtils::DiffPhi(jet->phi() , MET->phi()) );
     s_goodies.setVectorValue(TString("Eta_Jet"), jet->eta());
     s_goodies.setVectorValue(TString("NConstituents_Jet"), (int)jet->constituentsN());
     s_goodies.setVectorValue(TString("NumTracks_Jet"),       m_JetVariables->JetNumAssociatedTracks(jet) );
     s_goodies.setVectorValue(TString("ChargeFraction_Jet"),  m_JetVariables->JetChargeFraction(jet) );
     s_goodies.setVectorValue(TString("ootE25_Jet"),  jetOutOfTimeEnergyFraction(jet,25) );
     s_goodies.setVectorValue(TString("ootE50_Jet"),  jetOutOfTimeEnergyFraction(jet,50) );
     s_goodies.setVectorValue(TString("ootE75_Jet"),  jetOutOfTimeEnergyFraction(jet,75) );
     s_goodies.setVectorValue(TString("LArQuality_Jet")  , m_JetVariables->jetWLArQuality(jet) );
     s_goodies.setVectorValue(TString("TileQuality_Jet") , m_JetVariables->jetWTileQuality(jet) );
     s_goodies.setVectorValue(TString("SamplingFracPS_Jet"), m_JetVariables->SamplingFracPS(jet));
     s_goodies.setVectorValue(TString("SamplingFracEM_Jet"), m_JetVariables->SamplingFracEM(jet));
     s_goodies.setVectorValue(TString("SamplingFracFCAL_Jet"), m_JetVariables->SamplingFracFCAL(jet));
     s_goodies.setVectorValue(TString("SamplingFracHEC_Jet"), m_JetVariables->SamplingFracHEC(jet));
     s_goodies.setVectorValue(TString("SamplingFracHEC3_Jet"), m_JetVariables->SamplingFracHEC3(jet));
     s_goodies.setVectorValue(TString("SamplingFracTile_Jet"), m_JetVariables->SamplingFracTile(jet));
     s_goodies.setVectorValue(TString("SamplingFracTile2_Jet"), m_JetVariables->SamplingFracTile2(jet));
     s_goodies.setVectorValue(TString("SamplingFracTile10_Jet"), m_JetVariables->SamplingFracTile10(jet));
     s_goodies.setVectorValue(TString("SamplingFracGap_Jet"), m_JetVariables->SamplingFracGap(jet));
     s_goodies.setVectorValue(TString("SamplingFracCryo_Jet"), m_JetVariables->SamplingFracCryo(jet));
     s_goodies.setVectorValue(TString("tileGap3F_Jet"), tileGap3F(jet));

     s_goodies.setVectorValue(TString("isGood_Jet"), isGood(jet));
     s_goodies.setVectorValue(TString("isBad_Jet"),  isBad(jet));
     s_goodies.setVectorValue(TString("isUgly_Jet"), isUgly(jet));
     
     int SamplingMax = CaloSampling::Unknown; 
     s_goodies.setVectorValue(TString("SamplingFracMax_Jet"), fracSamplingMax(jet,SamplingMax));
     s_goodies.setVectorValue(TString("SamplingMaxID_Jet"),  SamplingMax );

     s_goodies.setVectorValue(TString("N90Cells_Jet"), (int)jet->getMoment("n90")); 
     s_goodies.setVectorValue(TString("N90Constituents_Jet"), (int)nLeadingConstituents(jet,0.9) );
     s_goodies.setVectorValue(TString("EMFraction_Jet"),      jetEMFraction(jet));
     
     s_goodies.setVectorValue( TString("QualityFrac_Jet"), jet->getMoment("LArQuality"));
     s_goodies.setVectorValue( TString("Timing_Jet"), jetTime(jet)); 
     s_goodies.setVectorValue(TString("HECf_Jet"), hecF(jet));

     
     double j1 = static_cast<double>(jet->getMoment("BCH_CORR_CELL"));
     double j2 = static_cast<double>(jet->getMoment("BCH_CORR_JET"));
     double j3 = static_cast<double>(jet->getMoment("BCH_CORR_JET_FORCELL")); 
     double j4 = static_cast<double>(jet->getMoment("ENG_BAD_CELLS"));
     int j5 = static_cast<int>(jet->getMoment("N_BAD_CELLS"));
     int j6 = static_cast<int>(jet->getMoment("N_BAD_CELLS_CORR"));
     double j7 = static_cast<double>(jet->getMoment("BAD_CELLS_CORR_E"));
     
     s_goodies.setVectorValue(TString("N_BadCells_Jet"),           j5 );
     s_goodies.setVectorValue(TString("N_BadCellsCorr_Jet"),       j6 );
     s_goodies.setVectorValue(TString("E_BadCells_Jet"),           j4 );
     s_goodies.setVectorValue(TString("E_BadCellsCorr_Jet"),       j7 );
     s_goodies.setVectorValue(TString("BCH_CORR_CELL_Jet"),        j1 );
     s_goodies.setVectorValue(TString("BCH_CORR_JET_FORCELL_Jet"), j3 );
     s_goodies.setVectorValue(TString("BCH_CORR_JET"),             j2 );

     // todo: here we need to do a matching, cannot simply assume lead jet
     // collection 1 matches lead jet collection 2; instead, implement
     // a DeltaR matching (there might already be helper functions to
     // do such things).

     bool DeltaFilled = false;
     if( otherJets ) {
       if ( (size_t)jetCount < otherJets->size()) {
         const Jet* theOtherJet = (*otherJets)[jetCount];
         if(theOtherJet) {
      DeltaFilled = true;
	   s_goodies.setVectorValue(TString("DeltaEt_JetAlgs_Jet"), jet->et() - theOtherJet->et());
	   s_goodies.setVectorValue(TString("DeltaPhi_JetAlgs_Jet"), MissingETUtils::DiffPhi(jet->phi(), theOtherJet->phi()));
	   s_goodies.setVectorValue(TString("DeltaEta_JetAlgs_Jet"), jet->eta() - theOtherJet->eta());
        }
       } 
     }
     if (!DeltaFilled) {
	   s_goodies.setVectorValue(TString("DeltaEt_JetAlgs_Jet"), (double)MET::MagicNumber);
	   s_goodies.setVectorValue(TString("DeltaPhi_JetAlgs_Jet"), (double)MET::MagicNumber);
	   s_goodies.setVectorValue(TString("DeltaEta_JetAlgs_Jet"), (double)MET::MagicNumber );
     }
   } 
   s_goodies.setValue(TString("N_Jets"), njets);
   ATH_MSG_DEBUG ("execute() successful");
   return sc;
}


StatusCode JetGoodiesFiller::finalize() 
{
   ATH_MSG_INFO ("finalize()");
   return StatusCode::SUCCESS;
}

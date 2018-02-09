/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonResonanceTool.cxx
#include "MuonResonanceTools/MuonResonanceTool.h"
#include "MuonResonanceTools/IMuonResonanceSelectionTool.h"
#include "MuonResonanceTools/IMuonResonancePairingTool.h"

//**********************************************************************

MuonResonanceTool::MuonResonanceTool(std::string myname)
  : AsgTool(myname),
    m_muonResonanceSelectionTool("MuonResonanceSelectionTool/MuonResonanceSelectionTool"),
    m_muonResonancePairingTool("MuonResonancePairingTool/MuonResonancePairingTool"){

  declareProperty("MuonResonanceSelectionTool",  m_muonResonanceSelectionTool);  
  declareProperty("MuonResonancePairingTool",    m_muonResonancePairingTool);  
  
  declareProperty("Binning",         m_binning);  
  declareProperty("HistoPrefix",     m_histoprefix);
  declareProperty("DoSystematics",   m_doSys );
  declareProperty("SysNames",        m_sysnames);  

}

MuonResonanceTool::~MuonResonanceTool(){

  if(m_PlotsInit){
    for(int j=0;j<MURESETABINS;++j){
      for(int i=0;i<TRACKBINS;++i){
	for(int g=0;g<NSys;++g){
	  delete m_resonancePlots[j][i][g];
	} 
      }
    }
  }

}


//**********************************************************************


StatusCode MuonResonanceTool::initialize()
{

  ATH_CHECK(m_muonResonanceSelectionTool.retrieve());
  ATH_CHECK(m_muonResonancePairingTool.retrieve());
  ATH_MSG_INFO( "DoSystematics    = " << m_doSys );

  //Determine number of enabled systematics
  std::vector< CP::SystematicSet > systematics = getSystematics();
  NSys = systematics.size();

  for(int j=0;j<MURESETABINS;++j){
    for(int i=0; i<TRACKBINS;++i){
      for(int g=0;g<NSys;++g){
	TString sys = systematics.at(g).name();
	TString suffix = Form("_eta_%i_%s_%s", j, tp_types[i].Data(), sys.Data());
	std::string suf = suffix.Data();
	m_resonancePlots[j][i][g] = new MuonResonancePlots(0,name());
	m_resonancePlots[j][i][g]->setSuffix(suf);
	m_resonancePlots[j][i][g]->setPrefix(m_histoprefix);
	m_resonancePlots[j][i][g]->setBinning(m_binning);
	m_resonancePlots[j][i][g]->initialize();
      }
    }
  }
  m_PlotsInit = true;

  return StatusCode::SUCCESS;
}


//**********************************************************************

void MuonResonanceTool::findResonance(const xAOD::MuonContainer* tags, bool isMC) const{

  ATH_MSG_DEBUG("Number of candidate tags    : " << tags->size() );
  ATH_MSG_DEBUG("calling selection" );

  //initialize systematics
  std::vector< CP::SystematicSet > Systematics = getSystematics();
  ATH_MSG_DEBUG("systematics size " << Systematics.size() );

  //Loop over systematics
  int iSys = 0;
  std::vector< CP::SystematicSet >::const_iterator sysItr;
  for( sysItr =  Systematics.begin(); sysItr !=  Systematics.end(); ++sysItr ) {
    ATH_MSG_DEBUG("applying sys no " << iSys  << " :: "<< (*sysItr).name() );

    ATH_MSG_DEBUG("calling selection" );
    std::pair<std::vector<const xAOD::Muon*>,std::vector<const xAOD::Muon*> >  goodMuons = m_muonResonanceSelectionTool->selectMuons(tags, isMC, *sysItr);
 
    ATH_MSG_DEBUG("calling pairing" );
    auto resonances = m_muonResonancePairingTool->buildPairs(goodMuons);

    ATH_MSG_DEBUG("filling plots" );
    if(resonances.size()>0){    
      if( !hasTriggerMatch(* resonances[0].first, * resonances[0].second) ) return; 

      int etabin= EtaBin(* resonances[0].first, * resonances[0].second);
      int authbin= AuthorBin(* resonances[0].first, * resonances[0].second);
      ATH_MSG_DEBUG("Eta | Author Bin:  "<< etabin <<" | "<< authbin );

      for(int i=0; i<TRACKBINS;++i){
	//fill inclusive histograms 
	m_resonancePlots[0][i][iSys]->fill(* resonances[0].first, * resonances[0].second, i);
      
	if(etabin>0) //exclusive in eta
	  m_resonancePlots[etabin][i][iSys]->fill(* resonances[0].first, * resonances[0].second, i);	 
      }
    } 
    ATH_MSG_DEBUG("filled plots" );

    //cleanup muon vectors
    for(unsigned int i=0; i<goodMuons.first.size(); i++) delete goodMuons.first.at(i);
    for(unsigned int i=0; i<goodMuons.second.size(); i++) delete goodMuons.second.at(i);
    goodMuons.first.clear();
    goodMuons.second.clear();

    iSys++;
  } //end loop on systematics

  return;
}
//**********************************************************************

std::vector<HistData> MuonResonanceTool::retrieveBookedHistograms(){

  std::vector<HistData> histData0;

  for(int j=0;j<MURESETABINS;++j){
    for(int i=0;i<TRACKBINS;++i){
      for(int g=0;g<NSys;++g){
	std::vector<HistData> histData1 =m_resonancePlots[j][i][g]->retrieveBookedHistograms();
	histData0.insert(histData0.end(), histData1.begin(), histData1.end());
      }
    }
  }
  return histData0;
}


//returns author bin muons 
int MuonResonanceTool::AuthorBin(const xAOD::Muon& mu1st, const xAOD::Muon& mu2nd) const{

  ATH_MSG_DEBUG("Author of muons    : " << int(mu1st.author()) << " | " << int(mu2nd.author()) );

  if( int(mu1st.author()) != int(mu2nd.author()) ) return -1;
  return static_cast<int>(mu1st.author());
}

// returns eta region of muons
int MuonResonanceTool::EtaBin(const xAOD::Muon& mu1st, const xAOD::Muon& mu2nd) const{

//  float eta1 = TMath::Abs(mu1st.eta());
//  float eta2 = TMath::Abs(mu2nd.eta());

  float eta1 = mu1st.eta();
  float eta2 = mu2nd.eta();

  ATH_MSG_DEBUG("Eta of muons    : " << eta1 << " | " << eta2 );

//  if(eta1<1. && eta2<1.) return 1;
//  else if((eta1>=1. && eta2>=1.) && (eta1<=2. && eta2<=2.)) return 2;
//  else if(eta1>2. && eta2>2.) return 3; 
//  else return -1;

  if(eta1<-2. && eta2<-2.) return 1;
  else if((eta1>=-2. && eta2>=-2.) && (eta1<-1. && eta2<-1.)) return 2;
  else if((eta1>=-1. && eta2>=-1.) && (eta1<0. && eta2<0.)) return 3;
  else if((eta1>=0. && eta2>=0.) && (eta1<=1. && eta2<=1.)) return 4;
  else if((eta1>1. && eta2>1.) && (eta1<=2. && eta2<=2.)) return 5;
  else if(eta1>2. && eta2>2.) return 6;
  else return -1;
}

std::vector< CP::SystematicSet > MuonResonanceTool:: getSystematics() const{

  std::vector< CP::SystematicSet > Systematics;
  const CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
  const CP::SystematicSet& recommendedSystematics = registry.recommendedSystematics();
  Systematics.push_back( CP::SystematicSet() );

  if( m_doSys ){ //enables systematics
    for( CP::SystematicSet::const_iterator SysIter = recommendedSystematics.begin();  SysIter != recommendedSystematics.end(); ++SysIter ) {

      // for each sys, check if we selected it
      TString name = (*SysIter).name();
      for(unsigned int i=0;i<m_sysnames.size();++i){
	std::string joname=m_sysnames[i];

	if(name.Contains(joname)){ 
	  Systematics.push_back( CP::SystematicSet() );
	  Systematics.back().insert(*SysIter);
	  ATH_MSG_DEBUG( "Enable systematic " << name );
	}
      }
    } 
  }
  return Systematics;
}

bool MuonResonanceTool::hasTriggerMatch(const xAOD::Muon& mu1st, const xAOD::Muon& mu2nd) const{

  bool trig1st = false, trig2nd = false;
  if(mu1st.isAvailable< bool >("isTriggered"))  trig1st = mu1st.auxdata< bool >("isTriggered");
  if(mu2nd.isAvailable< bool >("isTriggered"))  trig2nd = mu2nd.auxdata< bool >("isTriggered");
  ATH_MSG_DEBUG("MuonTriggerMatch : (" << trig1st << "|" << trig2nd << ")" );  

  return (trig1st || trig2nd);
}

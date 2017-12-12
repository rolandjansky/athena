/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkSUSY/SUSYSignalTagger.h"

#include "xAODEventInfo/EventInfo.h"

namespace DerivationFramework {

  static SG::AuxElement::Decorator<int> dec_procID("SUSY_procID");
  static SG::AuxElement::Decorator<int> dec_pdgId1("SUSY_pid1");
  static SG::AuxElement::Decorator<int> dec_pdgId2("SUSY_pid2");
    
  SUSYSignalTagger::SUSYSignalTagger(const std::string& t, const std::string& n, const IInterface* p):
    AthAlgTool(t,n,p)
  {
    
    declareInterface<DerivationFramework::IAugmentationTool>(this);
    
    declareProperty("EventInfoName",m_eventInfoName="EventInfo");
    declareProperty("MCCollectionName",m_mcName="TruthParticles");
  }
  
  
  
  SUSYSignalTagger::~SUSYSignalTagger(){}
  
  
  
  StatusCode SUSYSignalTagger::initialize(){
    
    ATH_MSG_INFO("Initialize " );
    
    return StatusCode::SUCCESS;
    
  }
  
  
  
  StatusCode SUSYSignalTagger::finalize(){
    
    return StatusCode::SUCCESS;
    
  }
  
  
  
  StatusCode SUSYSignalTagger::addBranches() const{
    
    const xAOD::EventInfo* eventInfo;
    if (evtStore()->retrieve(eventInfo,m_eventInfoName).isFailure()) {
      ATH_MSG_ERROR("could not retrieve event info " <<m_eventInfoName);
      return StatusCode::FAILURE;
    }
    
    const xAOD::TruthParticleContainer* truthPC = 0;
    if (evtStore()->retrieve(truthPC,m_mcName).isFailure()) {
      ATH_MSG_DEBUG("WARNING could not retrieve TruthParticleContainer " <<m_mcName);
      return StatusCode::FAILURE;
    }
    
    //Identify SUSY hard proc
    int pdgId1(0);
    int pdgId2(0);
    
    bool found = FindSusyHardProc( truthPC, pdgId1, pdgId2);
    if (!found) {
      ATH_MSG_WARNING("could not identify SUSY process! ");

      dec_procID(*eventInfo) = 0;
      dec_pdgId1(*eventInfo) = -99;
      dec_pdgId2(*eventInfo) = -99;

      return StatusCode::SUCCESS;
    }


    //Get SUSY proc ID 
    unsigned int procID = finalStateID(pdgId1, pdgId2);
 
    dec_procID(*eventInfo) = procID;
    dec_pdgId1(*eventInfo) = pdgId1;
    dec_pdgId2(*eventInfo) = pdgId2;

    return StatusCode::SUCCESS;
  }


  bool SUSYSignalTagger::FindSusyHardProc(const xAOD::TruthParticleContainer* truthP, int& pdgid1, int& pdgid2) const
  {
    pdgid1 = 0;
    pdgid2 = 0;

    const xAOD::TruthParticle* firstsp(0);
    const xAOD::TruthParticle* secondsp(0);

    if (!truthP || truthP->empty()) {
      return false;
    }
    for (const auto& tp : *truthP) {

      //check ifSUSY particle
      if ((abs(tp->pdgId()) > 1000000 && abs(tp->pdgId()) < 1000007) || // squarkL
	  (abs(tp->pdgId()) > 1000010 && abs(tp->pdgId()) < 1000017) || // sleptonL
	  (abs(tp->pdgId()) > 2000000 && abs(tp->pdgId()) < 2000007) || // squarkR
	  (abs(tp->pdgId()) > 2000010 && abs(tp->pdgId()) < 2000017) || // sleptonR
	  (abs(tp->pdgId()) > 1000020 && abs(tp->pdgId()) < 1000040)) { // gauginos

	if (tp->nParents() != 0) {
	  if ( tp->parent(0)->absPdgId()  < 1000000) {
	    if (!firstsp) {
	      firstsp = tp;
	    } else if (!secondsp) {
	      secondsp = tp;
	    } else {
	      if (firstsp->nChildren() != 0 && tp->barcode() == firstsp->child(0)->barcode()) {
		firstsp = tp;
	      }
	      else if (secondsp->nChildren() != 0 && tp->barcode() == secondsp->child(0)->barcode()) {
		secondsp = tp;
	      }
	      else if (firstsp->nChildren() != 0 && firstsp->child(0)->barcode() == secondsp->barcode()) {
		firstsp = secondsp;
		secondsp = tp;
	      }
	      else if (secondsp->nChildren() != 0 && secondsp->child(0)->barcode() == firstsp->barcode()) {
		secondsp = firstsp;
		firstsp = tp;
	      }
	    }
	  }
	}
      }
    }

    // quit if no sparticles found
    if (!firstsp && !secondsp) return false; // should find none or two

    if (firstsp->nChildren() == 1) {
      for (const auto& tp : *truthP) {
	if (tp->barcode() == firstsp->child(0)->barcode() && tp->pdgId() != firstsp->pdgId()) {
	  firstsp = tp;
	  break;
	}
      }
    }

    if (secondsp->nChildren() == 1) {
      for (const auto& tp : *truthP) {
	if (tp->barcode() == secondsp->child(0)->barcode() && tp->pdgId() != secondsp->pdgId()) {
	  secondsp = tp;
	  break;
	}
      }
    }

    if (abs(firstsp->pdgId()) > 1000000) pdgid1 = firstsp->pdgId();
    if (abs(secondsp->pdgId()) > 1000000) pdgid2 = secondsp->pdgId();

    // Return gracefully:
    return true;
  }


  unsigned int SUSYSignalTagger::finalStateID(const int SUSY_Spart1_pdgId, const int SUSY_Spart2_pdgId) const
  {
    int ngluino=0;
    int nsquark=0; // (up and down type without bottom/top)
    int nantisquark=0; // (up and down type without bottom/top)

    int nsbottom=0;
    int nstop=0;
    int nsbottom2=0;
    int nstop2=0;
    int nantisbottom=0;
    int nantistop=0;
    int nantisbottom2=0;
    int nantistop2=0;

    int nchi01=0;
    int nchi02=0;
    int nchi03=0;
    int nchi04=0;
    int nch1plus=0;
    int nch2plus=0;
    int nch1minus=0;
    int nch2minus=0;

    // sleptons
    int nsmuonRplus=0;
    int nsmuonRminus=0;
    int nselecRplus=0;
    int nselecRminus=0;

    int nsmuonLplus=0;
    int nsmuonLminus=0;
    int nselecLplus=0;
    int nselecLminus=0;

    int nstau1plus=0;
    int nstau1minus=0;
    int nstau2plus=0;
    int nstau2minus=0;

    // snutrinos
    int nselnuL=0;
    int nsmunuL=0;
    int nstaunuL=0;

    int nother=0;

    //Classification of the event follows (gg, sq...):

    if      (fabs(SUSY_Spart1_pdgId)== 1000022) nchi01++;
    else if (fabs(SUSY_Spart1_pdgId)== 1000023) nchi02++;
    else if (fabs(SUSY_Spart1_pdgId)== 1000025) nchi03++;
    else if (fabs(SUSY_Spart1_pdgId)== 1000035) nchi04++;
    else if (     SUSY_Spart1_pdgId == 1000024) nch1plus++;
    else if (     SUSY_Spart1_pdgId ==-1000024) nch1minus++;
    else if (     SUSY_Spart1_pdgId == 1000037) nch2plus++;
    else if (     SUSY_Spart1_pdgId ==-1000037) nch2minus++;
    else if (     SUSY_Spart1_pdgId == 1000021) ngluino++;
    else if ((fabs(SUSY_Spart1_pdgId)>1000000 && fabs(SUSY_Spart1_pdgId)<= 1000004) || (fabs(SUSY_Spart1_pdgId)>2000000 && fabs(SUSY_Spart1_pdgId)<=2000004)) {
      if (SUSY_Spart1_pdgId>0) nsquark++;
      else nantisquark++;
    }
    else if (SUSY_Spart1_pdgId==1000005) nsbottom++;
    else if (SUSY_Spart1_pdgId==1000006) nstop++;
    else if (SUSY_Spart1_pdgId==2000005) nsbottom2++;
    else if (SUSY_Spart1_pdgId==2000006) nstop2++;
    else if (SUSY_Spart1_pdgId==-1000005) nantisbottom++;
    else if (SUSY_Spart1_pdgId==-1000006) nantistop++;
    else if (SUSY_Spart1_pdgId==-2000005) nantisbottom2++;
    else if (SUSY_Spart1_pdgId==-2000006) nantistop2++;
    else if (SUSY_Spart1_pdgId==2000011) nselecRminus++;
    else if (SUSY_Spart1_pdgId==-2000011) nselecRplus++;
    else if (SUSY_Spart1_pdgId==1000011) nselecLminus++;
    else if (SUSY_Spart1_pdgId==-1000011) nselecLplus++;
    else if (fabs(SUSY_Spart1_pdgId)==1000012) nselnuL++;
    else if (SUSY_Spart1_pdgId==2000013) nsmuonRminus++;
    else if (SUSY_Spart1_pdgId==-2000013) nsmuonRplus++;
    else if (SUSY_Spart1_pdgId==1000013) nsmuonLminus++;
    else if (SUSY_Spart1_pdgId==-1000013) nsmuonLplus++;
    else if (fabs(SUSY_Spart1_pdgId)==1000014) nsmunuL++;
    else if (SUSY_Spart1_pdgId==1000015) nstau1minus++;
    else if (SUSY_Spart1_pdgId==-1000015) nstau1plus++;
    else if (SUSY_Spart1_pdgId==2000015) nstau2minus++;
    else if (SUSY_Spart1_pdgId==-2000015) nstau2plus++;
    else if (fabs(SUSY_Spart1_pdgId)==1000016) nstaunuL++;
    else nother++;




    if (fabs(SUSY_Spart2_pdgId)==1000022) nchi01++;
    else if (fabs(SUSY_Spart2_pdgId)==1000023) nchi02++;
    else if (fabs(SUSY_Spart2_pdgId)==1000025) nchi03++;
    else if (fabs(SUSY_Spart2_pdgId)==1000035) nchi04++;
    else if (SUSY_Spart2_pdgId==1000024) nch1plus++;
    else if (SUSY_Spart2_pdgId==-1000024) nch1minus++;
    else if (SUSY_Spart2_pdgId==1000037) nch2plus++;
    else if (SUSY_Spart2_pdgId==-1000037) nch2minus++;

    else if (SUSY_Spart2_pdgId==1000021) ngluino++;
    else if ((fabs(SUSY_Spart2_pdgId)>1000000 && fabs(SUSY_Spart2_pdgId)<=1000004) || (fabs(SUSY_Spart2_pdgId)>2000000 && fabs(SUSY_Spart2_pdgId)<=2000004)) {
      if (SUSY_Spart2_pdgId>0) nsquark++;
      else nantisquark++;
    }
    else if (SUSY_Spart2_pdgId==1000005) nsbottom++;
    else if (SUSY_Spart2_pdgId==1000006) nstop++;
    else if (SUSY_Spart2_pdgId==2000005) nsbottom2++;
    else if (SUSY_Spart2_pdgId==2000006) nstop2++;
    else if (SUSY_Spart2_pdgId==-1000005) nantisbottom++;
    else if (SUSY_Spart2_pdgId==-1000006) nantistop++;
    else if (SUSY_Spart2_pdgId==-2000005) nantisbottom2++;
    else if (SUSY_Spart2_pdgId==-2000006) nantistop2++;

    else if (SUSY_Spart2_pdgId==2000011) nselecRminus++;
    else if (SUSY_Spart2_pdgId==-2000011) nselecRplus++;
    else if (SUSY_Spart2_pdgId==1000011) nselecLminus++;
    else if (SUSY_Spart2_pdgId==-1000011) nselecLplus++;
    else if (fabs(SUSY_Spart2_pdgId)==1000012) nselnuL++;
    else if (SUSY_Spart2_pdgId==2000013) nsmuonRminus++;
    else if (SUSY_Spart2_pdgId==-2000013) nsmuonRplus++;
    else if (SUSY_Spart2_pdgId==1000013) nsmuonLminus++;
    else if (SUSY_Spart2_pdgId==-1000013) nsmuonLplus++;
    else if (fabs(SUSY_Spart2_pdgId)==1000014) nsmunuL++;
    else if (SUSY_Spart2_pdgId==1000015) nstau1minus++;
    else if (SUSY_Spart2_pdgId==-1000015) nstau1plus++;
    else if (SUSY_Spart2_pdgId==2000015) nstau2minus++;
    else if (SUSY_Spart2_pdgId==-2000015) nstau2plus++;
    else if (fabs(SUSY_Spart2_pdgId)==1000016) nstaunuL++;
    else nother++;


    ///Final classification
    // gluino/squark + X
    if (ngluino==1 && (nsquark==1 || nantisquark ==1)) return 1;
    else if (ngluino==2) return 2;
    else if (nsquark==2 || nantisquark==2) return 3;
    else if (nsquark==1 && nantisquark==1) return 4;

    else if (nsbottom==1 && nantisbottom==1) return 51;
    else if (nsbottom2==1 && nantisbottom2==1) return 52;
    else if (nstop==1 && nantistop==1) return 61;
    else if (nstop2==1 && nantistop2==1) return 62;

    else if (ngluino==1 && nchi01==1) return 71;
    else if (ngluino==1 && nchi02==1) return 72;
    else if (ngluino==1 && nchi03==1) return 73;
    else if (ngluino==1 && nchi04==1) return 74;

    else if (ngluino==1 && nch1plus==1) return 75;
    else if (ngluino==1 && nch2plus==1) return 76;
    else if (ngluino==1 && nch1minus==1) return 77;
    else if (ngluino==1 && nch2minus==1) return 78;

    else if ((nsquark==1 || nantisquark ==1) && nchi01==1) return 81;
    else if ((nsquark==1 || nantisquark ==1) && nchi02==1) return 82;
    else if ((nsquark==1 || nantisquark ==1) && nchi03==1) return 83;
    else if ((nsquark==1 || nantisquark ==1) && nchi04==1) return 84;

    else if ((nsquark==1 || nantisquark ==1) && nch1plus==1) return 85;
    else if ((nsquark==1 || nantisquark ==1) && nch2plus==1) return 86;
    else if ((nsquark==1 || nantisquark ==1) && nch1minus==1) return 87;
    else if ((nsquark==1 || nantisquark ==1) && nch2minus==1) return 88;


    // Gaugino pair-production
    // chi^{0}_1 + X
    else if (nchi01==2) return 111;
    else if (nchi01==1 && nchi02==1) return 112;
    else if (nchi01==1 && nchi03==1) return 113;
    else if (nchi01==1 && nchi04==1) return 114;
    else if (nchi01==1 && nch1plus==1) return 115;
    else if (nchi01==1 && nch2plus==1) return 116;
    else if (nchi01==1 && nch1minus==1) return 117;
    else if (nchi01==1 && nch2minus==1) return 118;

    // chi^{0}_2 + X
    else if (nchi02==2) return 122;
    else if (nchi02==1 && nchi03==1) return 123;
    else if (nchi02==1 && nchi04==1) return 124;
    else if (nchi02==1 && nch1plus==1) return 125;
    else if (nchi02==1 && nch2plus==1) return 126;
    else if (nchi02==1 && nch1minus==1) return 127;
    else if (nchi02==1 && nch2minus==1) return 128;

    // chi^{0}_3 + X
    else if (nchi03==2) return 133;
    else if (nchi03==1 && nchi04==1) return 134;
    else if (nchi03==1 && nch1plus==1) return 135;
    else if (nchi03==1 && nch2plus==1) return 136;
    else if (nchi03==1 && nch1minus==1) return 137;
    else if (nchi03==1 && nch2minus==1) return 138;

    // chi^{0}_4 + X
    else if (nchi04==2) return 144;
    else if (nchi04==1 && nch1plus==1) return 145;
    else if (nchi04==1 && nch2plus==1) return 146;
    else if (nchi04==1 && nch1minus==1) return 147;
    else if (nchi04==1 && nch2minus==1) return 148;

    // chi^{+}_1/2 + chi^{-}_1/2
    else if (nch1plus==1 && nch1minus==1) return 157;
    else if (nch1plus==1 && nch2minus==1) return 158;

    else if (nch2plus==1 && nch1minus==1) return 167;
    else if (nch2plus==1 && nch2minus==1) return 168;

    // slepton
    else if (nselecLplus==1 && nselecLminus==1) return 201; // sElectronLPair
    else if (nselecRplus==1 && nselecRminus==1) return 202; // sElectronRPair
    else if (nselnuL==2) return 203; // sElectron neutrino pair
    else if (nselecLplus==1 && nselnuL==1) return 204;  // sElectron+ sNutrino
    else if (nselecLminus==1 && nselnuL==1) return 205; // sElectron- sNutrino
    else if (nstau1plus==1 && nstau1minus==1) return 206;
    else if (nstau2plus==1 && nstau2minus==1) return 207;
    else if ((nstau1plus==1 || nstau1minus==1) && (nstau2plus==1 || nstau2minus==1)) return 208;
    else if (nstaunuL==2) return 209; // sTau neutrino pair
    else if (nstau1plus==1 && nstaunuL==1) return 210;
    else if (nstau1minus==1 && nstaunuL==1) return 211;
    else if (nstau2plus==1 && nstaunuL==1) return 212;
    else if (nstau2minus==1 && nstaunuL==1) return 213;

    else if (nsmuonLplus==1 && nsmuonLminus==1) return 216; // sMuonPair
    else if (nsmuonRplus==1 && nsmuonRminus==1) return 217; // sMuonPair
    else if (nsmunuL==2) return 218; // sMuon neutrino pair
    else if (nsmuonLplus==1 && nsmunuL==1) return 219;  // sMuon+ sNutrino
    else if (nsmuonLminus==1 && nsmunuL==1) return 220; // sMuon- sNutrino

    ATH_MSG_WARNING("could not determine finalState for : (" << SUSY_Spart1_pdgId << " , " << SUSY_Spart2_pdgId << ").  Returning 0.");

    return 0;
  }


} /// namespace

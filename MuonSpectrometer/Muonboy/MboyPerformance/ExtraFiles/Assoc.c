/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "Assoc.h"

#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include "TPostScript.h"

//==============================================================================================
// Association des traces
// Assoc("mboy","IP") = association muonboy extrapolee au IP avec traces simulees
// Assoc("mboy","MS") = association muonboy entree du spectro avec traces simulees entree spectro
// Assoc("mboy","ID") = association muonboy extrapolee au IP avec traces Inner
// Assoc("idet","IP") = association trace ID extrapolee au IP avec traces simulees
// Assoc("stac")      = association staco avec traces simulees (le tout etant au IP)
// Assoc("mtag")      = association staco+mutag avec traces simulees (le tout etant au IP)
//============================================================================================== 


Assoc::Assoc(char * s, char * v):HasEvt()  {
//
  m_PI = 3.14159265;
  m_NMAX = 4000;
//
  m_type = s;
  m_lieu = v;
  m_AngCut = 5.;
  m_MomCut = 5.;
//  m_AngCut = 20.;
//  m_MomCut = 20.;
}


Assoc::~Assoc(){}


bool Assoc::isFake(int irec) { return (m_assoRec[irec] < 0) ; }

bool Assoc::isMuon(int irec) { return (m_assoRec[irec] > -1) ; }

bool Assoc::isAdditional(int irec) { return (m_assoRec[irec] == -2) ; }
 
bool Assoc::isDuplicated(int irec) { return (m_assoRec[irec] == -1) ; }

int Assoc::getNsim() { return m_Nsim; };

int Assoc::getNrec() { return m_Nrec; };
 
int Assoc::getSim(int irec) { return m_assoRec[irec]; }

int Assoc::getConeSim(int irec) { return m_coneRec[irec]; }

int Assoc::getRec(int isim) {
  int i = -1;
  for (int irec=0; irec<m_Nrec; irec++) { if(getSim(irec) == isim)  i = irec; }
  return i;
}

int Assoc::getNCone(int isim) { return m_NassoCone[isim]; }

void Assoc::setCuts(int nsigAng, int nsigMom) {
  m_AngCut = nsigAng;
  m_MomCut = nsigMom;
}

void Assoc::associe() {    // association mu simules - reconstruits

  if(strcmp(m_lieu, "ID") == 0)        m_Nsim = p_evt->Trk_totalNumTracks;
  else if(strcmp(m_lieu, "MS") == 0)   m_Nsim = p_evt->MuonEntRec_Num;
  else                                 m_Nsim = p_evt->NPar;
  if(m_Nsim > m_NMAX) {
    cout << " Warning Assoc " << m_type << " " << m_lieu << " Nsim = " << m_Nsim << endl;
    m_Nsim = m_NMAX;
  }  

  if(strcmp(m_type, "mtag") == 0)      m_Nrec = p_evt->staco_nmuon + p_evt->MuTag_nmuon;
  else if(strcmp(m_type, "stac") == 0) m_Nrec = p_evt->staco_nmuon;
  else if(strcmp(m_type, "idet") == 0) m_Nrec = p_evt->Trk_totalNumTracks;
  else                                 m_Nrec = p_evt->MboyESD_nmuon;
  if(m_Nrec > m_NMAX) {
    cout << " Warning Assoc " << m_type << " " << m_lieu << " Nrec = " << m_Nrec << endl;
    m_Nrec = m_NMAX;
  }
/* temporaire ?? bug dans ID ?? */
//  if( strcmp(m_type, "idet") == 0 && m_Nrec > 500 ) {
//    cout << " Container ID avec plus de 500 traces !!! fait planter ?? on limite a 500 " << endl;
//    m_Nrec = 500;
//  }

  if(strcmp(m_type, "mboy") == 0 && p_evt->MboyTrkMS_NTrk != p_evt->MboyESD_nmuon) {
    cout << " Warning : MboyTrkMS_NTrk = " << p_evt->MboyTrkMS_NTrk << " while MboyESD_nmuon = " << p_evt->MboyESD_nmuon;
  }

//================== temporaire : ne marche que sur single mu ============
//================== pour eviter problemes de MS pendant l'extrapolation =
//================== on prend l'association avec les TrackRecord
//  if(strcmp(m_type, "mboy") == 0 && strcmp(m_lieu, "IP") == 0) {
//    Assoc* asso = new Assoc("mboy", "MS");
//    asso->associe();
//    if(asso->getNsim() != m_Nsim || asso->getNrec() != m_Nrec) {
//      cout << " Warning Probleme in Assoc" << endl;
//      cout << m_Nsim << " " << asso->getNsim() << " " << m_Nrec << " " << asso->getNrec() << endl;
//    }
//    for (int isim=0; isim<m_Nsim; isim++) {
//      m_NassoCone[isim] = asso->getNCone(isim);
//    }
//    for (int irec=0; irec<m_Nrec; irec++) {
//      m_assoRec[irec] = asso->getSim(irec);
//      m_coneRec[irec] = asso->getConeSim(irec);
//    }
//    delete asso;
//    return;
//  }
//=========================================================================

  
  for (int irec=0; irec<m_Nrec; irec++) {
    m_assoRec[irec] = -3;
    m_coneRec[irec] = -1;
  }
//
// On commence par regarder les traces reconstruites dans un cone
// autour de la trace simulee
  for (int isim=0; isim<m_Nsim; isim++) {
    int ncon = 0;
    if( isMuonSimul(isim) ) {
      for (int irec=0; irec<m_Nrec; irec++) {
        if( isInsideCone(irec, isim) && ncon < 49 ) {
          m_listAssoCone[isim][ncon] = irec;
          ncon++;
        }
      }
    }
    m_NassoCone[isim] = ncon;
  }
//
// a faire pour les multitraces
// si une meme trace reconstruite est dans deux cones => en choisir un
//                   ...
//
//
// Puis on choisit la trace reconstruite dans cone la plus proche
// les autres traces reconstruites dans cone (avec chi2P<chicut) sont classees -1
//             "                       "     (avec chi2P>chicut)               -2
// celles etant en dehors de tout cone sont classees -3
//
  int irec;
  float ecar;
  for (int isim=0; isim<m_Nsim; isim++) {
    int ncon = m_NassoCone[isim];
    if( ncon > 0 ) {
      int irecmin = -1;
      float ecarmin = 9999.;
      for (int j=0; j<ncon; j++) {
        irec = m_listAssoCone[isim][j];
        ecar = sqrt(getDeltaPhi(irec,isim)*getDeltaPhi(irec,isim)+getDeltaEta(irec,isim)*getDeltaEta(irec,isim));
        m_coneRec[irec] = isim;
        if( fabs(getPullUnSurP(irec,isim)) > m_MomCut) {
//         if( fabs(getPullUnSurP(irec,isim)) > m_MomCut ||
//           ( strcmp(m_lieu, "MS") != 0 && ( fabs(getPullD0(irec,isim)) > m_MomCut || fabs(getPullZ0(irec,isim)) > m_MomCut ) ) ) {
          if( m_assoRec[irec] < -2 )  m_assoRec[irec] = -2;
        } else {
          if( m_assoRec[irec] < -1 )  m_assoRec[irec] = -1;
          if( ecar < ecarmin && m_assoRec[irec] < 0 ) {
            ecarmin = ecar;
            irecmin = irec;
          }
        }
      }
      if(irecmin >= 0)  m_assoRec[irecmin] = isim;
    }     
  }
}


void Assoc::close() { }


void Assoc::InfoAssociation(){
  //
  cout << "*Assoc::InfoAssociation  " << m_type << " " << m_lieu << endl;
  //
  cout << "* Nber of Sim  " << m_Nsim << endl;
  for (int i=0; i<m_Nsim; i++) {
    if(isMuonSimul(i)) {
      cout << "* " << i << "||" << getEtaSim(i) << "  " << getPhiSim(i) << "  " << getChargeSurPSim(i)
                                                << "  " << getD0Sim(i)  << "  " << getZ0Sim(i) << endl;
    }
  }
  cout << "* Nber of Rec  " << m_Nrec << endl;
  for (int i=0; i<m_Nrec; i++) {
    cout << "* " << i << "||" << getEtaRec(i) << "  " << getPhiRec(i)  << "  " << getChargeSurPRec(i)
                                              << "  " << getD0Rec(i)   << "  " << getZ0Rec(i) << endl;
  }
  //
  cout << "*"  << " irec igen" << endl;
  int Nmuon = 0, Nfake = 0;
  for (int i=0; i<m_Nrec; i++) {
    if(isFake(i))  Nfake = Nfake + 1;
    else 	   Nmuon = Nmuon + 1;
    int isim = m_assoRec[i];
    cout << "*"  << "  " << i << "    " << isim;
    if(isim >= 0)  cout << "  " << getDeltaTheta(i, isim) << "  " << getDeltaPhi(i, isim) << "  " << getPullUnSurP(i, isim);
    cout << endl;
  }
  cout << "* Nber of Fake  " << Nfake << endl;
  cout << "* Nber of Muons " << Nmuon<< endl;
  cout << "*" << endl;
  //
}



bool Assoc::isMuonSimul(int isim) {
  if( strcmp(m_lieu, "ID" ) == 0 )                                                          return true;
  else if( strcmp(m_type, "idet" ) == 0 )                                                   return true;
  else if( strcmp(m_lieu, "MS") == 0 ) {
    if( isim < 0 || isim >= p_evt->MuonEntRec_Num )                                         return false;
    if( (p_evt->MuonEntRec_ID)->at(isim) == 13 || (p_evt->MuonEntRec_ID)->at(isim) == -13 ) return true;
  } else if( strcmp(m_lieu, "IP") == 0 ) {
    if( isim < 0 || isim >= p_evt->NPar )                                                   return false;
    if( (p_evt->Type)->at(isim) != 13 && (p_evt->Type)->at(isim) != -13 )                   return false;
    if( (p_evt->KOriVNt)->at(isim) == 0 )                                                   return true;  /* single muon */
    int mother = (p_evt->KMothNt)->at(isim);
    if( mother < 0 )                                                                        return false;
    if( (p_evt->KOriVNt)->at(isim) == 6 && (p_evt->Type)->at(mother)  == 23 )               return true;  /* pythia zmumu */
    if( fabs((p_evt->Type)->at(mother)) == 24 )                                             return true;  /* t->W->mu     */
    if( fabs((p_evt->Type)->at(mother)) > 100 )                                             return true;
  }
  return false;
}

bool Assoc::isMuonDirect(int isim) {
  if( strcmp(m_lieu, "ID" ) == 0 )                                                          return true;
  else if( strcmp(m_type, "idet" ) == 0 )                                                   return true;
  else if( strcmp(m_lieu, "MS") == 0 ) {
    if( isim < 0 || isim >= p_evt->MuonEntRec_Num )                                         return false;
    if( (p_evt->MuonEntRec_ID)->at(isim) == 13 || (p_evt->MuonEntRec_ID)->at(isim) == -13 ) return true;
  } else if( strcmp(m_lieu, "IP") == 0 ) {
    if( isim < 0 || isim >= p_evt->NPar )                                                   return false;
    if( (p_evt->Type)->at(isim) != 13 && (p_evt->Type)->at(isim) != -13 )                   return false;
    if( (p_evt->KOriVNt)->at(isim) == 0 )                                                   return true;  /* single muon */
    int mother = (p_evt->KMothNt)->at(isim);
    if( mother < 0 )                                                                        return false;
    if( (p_evt->KOriVNt)->at(isim) == 6 && (p_evt->Type)->at(mother)  == 23 )               return true;  /* pythia zmumu */
    if( fabs((p_evt->Type)->at(mother)) == 24 )                                             return true;  /* t->W->mu     */
  }
  return false;
}


bool Assoc::isInsideCone(int irec, int isim) {
  if( fabs(getPullPhi(irec,isim)) < m_AngCut && fabs(getPullTheta(irec,isim)) < m_AngCut )  return true;
  else                                                                                      return false;
}



float Assoc::getEtaSim(int isim) {

  if(strcmp(m_lieu, "ID") == 0)       return TMath::ASinH(1./tan(getThetaSim(isim)));
  else if(strcmp(m_lieu, "MS") == 0)  return (p_evt->MuonEntRec_Eta)->at(isim);
  else                                return (p_evt->EtaGen)->at(isim);
}


float Assoc::getD0Sim(int isim) {
  if(strcmp(m_lieu, "ID") == 0)       return (p_evt->Trk_d0)->at(isim);
  else {
//    float alfa = - (p_evt->RVGen)->at(isim) * ( cos((p_evt->PhiGen)->at(isim))*cos((p_evt->PhiVGen)->at(isim))
//					      + sin((p_evt->PhiGen)->at(isim))*sin((p_evt->PhiVGen)->at(isim)) );
//    float xv = (p_evt->RVGen)->at(isim)*cos((p_evt->PhiVGen)->at(isim)) + alfa*cos((p_evt->PhiGen)->at(isim));
//    float yv = (p_evt->RVGen)->at(isim)*sin((p_evt->PhiVGen)->at(isim)) + alfa*sin((p_evt->PhiGen)->at(isim));
//    return sqrt( xv*xv + yv*yv);
    return ( (p_evt->RVGen)->at(isim) * sin( (p_evt->PhiVGen)->at(isim) - (p_evt->PhiGen)->at(isim) ) ); 
  }
}


float Assoc::getZ0Sim(int isim) {
  if(strcmp(m_lieu, "ID") == 0)       return (p_evt->Trk_z0)->at(isim);
  else {
    float alfa = - (p_evt->RVGen)->at(isim) * cos( (p_evt->PhiVGen)->at(isim) + (p_evt->PhiGen)->at(isim) );
    return ( (p_evt->ZVGen)->at(isim) + alfa / tan(getThetaSim(isim)) );
  }
}


float Assoc::getPhiSim(int isim) {
  if(strcmp(m_lieu, "ID") == 0)       return (p_evt->Trk_phi)->at(isim);
  else if(strcmp(m_lieu, "MS") == 0)  return (p_evt->MuonEntRec_Phi)->at(isim);
  else                                return (p_evt->PhiGen)->at(isim);
}


float Assoc::getThetaSim(int isim) {
  float theta;
  if(strcmp(m_lieu, "ID") == 0)       theta = (p_evt->Trk_theta)->at(isim);
  else                                theta = atan(1./TMath::SinH(getEtaSim(isim)));
  if(theta < 0.)  theta = theta + m_PI;
  return theta;
}


float Assoc::getChargeSurPSim(int isim) {
  if(strcmp(m_lieu, "ID") == 0)      return  (p_evt->Trk_qOverP)->at(isim);
  else if(strcmp(m_lieu, "MS") == 0) return ( - ((p_evt->MuonEntRec_ID)->at(isim)/13) / (p_evt->MuonEntRec_Ene)->at(isim)  );
  else                               return ( (p_evt->Charge)->at(isim) * sin(getThetaSim(isim)) / (p_evt->PtGen)->at(isim) );
}


float Assoc::getEtaRec(int irec) { return TMath::ASinH(1./tan(getThetaRec(irec))); }


float Assoc::getD0Rec(int irec) {
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    return (p_evt->staco_A0)->at(irec);
    else                               return (p_evt->MuTag_A0)->at(irec-(p_evt->staco_nmuon));
  }
  else if(strcmp(m_type, "stac") == 0) return (p_evt->staco_A0)->at(irec);
  else if(strcmp(m_type, "idet") == 0) return (p_evt->Trk_d0)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   return 0.;
  else                                 return (p_evt->MboyESD_A0)->at(irec);
}


float Assoc::getZ0Rec(int irec) {
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    return (p_evt->staco_Z)->at(irec);
    else                               return (p_evt->MuTag_Z)->at(irec-(p_evt->staco_nmuon));
  }
  else if(strcmp(m_type, "stac") == 0) return (p_evt->staco_Z)->at(irec);
  else if(strcmp(m_type, "idet") == 0) return (p_evt->Trk_z0)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   return 0.;
  else                                 return (p_evt->MboyESD_Z)->at(irec);
}


float Assoc::getPhiRec(int irec) {
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    return (p_evt->staco_Phi)->at(irec);
    else                               return (p_evt->MuTag_Phi)->at(irec-(p_evt->staco_nmuon));
  }
  else if(strcmp(m_type, "stac") == 0) return (p_evt->staco_Phi)->at(irec);
  else if(strcmp(m_type, "idet") == 0) return (p_evt->Trk_phi)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   return (p_evt->MboyTrkMS_Phivc)->at(irec);
  else                                 return (p_evt->MboyESD_Phi)->at(irec);
}


float Assoc::getThetaRec(int irec) {
  float theta;
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    theta = (p_evt->staco_Theta)->at(irec);
    else                               theta = (p_evt->MuTag_Theta)->at(irec-(p_evt->staco_nmuon));
  }
  else if(strcmp(m_type, "stac") == 0) theta = (p_evt->staco_Theta)->at(irec);
  else if(strcmp(m_type, "idet") == 0) theta = (p_evt->Trk_theta)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   theta = (p_evt->MboyTrkMS_Thevc)->at(irec);
  else                                 theta = (p_evt->MboyESD_Theta)->at(irec);
  if(theta < 0.)  theta = theta + m_PI;
  return theta;
}


float Assoc::getChargeSurPRec(int irec) {
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    return (p_evt->staco_qOverP)->at(irec);
    else                               return (p_evt->MuTag_qOverP)->at(irec-(p_evt->staco_nmuon));
  }
  else if(strcmp(m_type, "stac") == 0) return (p_evt->staco_qOverP)->at(irec);
  else if(strcmp(m_type, "idet") == 0) return (p_evt->Trk_qOverP)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   return (p_evt->MboyTrkMS_PInv)->at(irec);
  else                                 return (p_evt->MboyESD_qOverP)->at(irec);
}

float Assoc::getPtRec(int irec) {
  return fabs( sin(getThetaRec(irec)) / getChargeSurPRec(irec) );
}

float Assoc::getDeltaD0(int irec, int isim) {
  return (getD0Rec(irec) - getD0Sim(isim));
}


float Assoc::getDeltaZ0(int irec, int isim) {
  return (getZ0Rec(irec) - getZ0Sim(isim));
}


float Assoc::getDeltaPhi(int irec, int isim) {
  float deltaPhi = getPhiRec(irec) - getPhiSim(isim);
  if(deltaPhi >  m_PI)  deltaPhi = deltaPhi - 2.*m_PI;
  if(deltaPhi < -m_PI)  deltaPhi = deltaPhi + 2.*m_PI;
  return deltaPhi;
}


float Assoc::getDeltaEta(int irec, int isim) {
  return (getEtaRec(irec) - getEtaSim(isim));
}

float Assoc::getDeltaTheta(int irec, int isim) {
  return (getThetaRec(irec) - getThetaSim(isim));
}


float Assoc::getDeltaUnSurP(int irec, int isim) {
  return (getChargeSurPRec(irec)-getChargeSurPSim(isim));
}

float Assoc::getDeltaPSurPnoSign(int irec, int isim) {
  return (getDeltaUnSurP(irec,isim)/getChargeSurPRec(irec));
}


float Assoc::getDeltaPSurP(int irec, int isim) {
  return (getDeltaUnSurP(irec,isim)/fabs(getChargeSurPRec(irec)));
}

float Assoc::getDeltaP(int irec, int isim) {
  return fabs(1./getChargeSurPSim(isim))-fabs(1./getChargeSurPRec(irec));
}


float Assoc::getPullD0(int irec, int isim) {
  float err;
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    err = sqrt( (p_evt->staco_covr11)->at(irec) );
    else                               err = sqrt( (p_evt->MuTag_covr11)->at(irec-(p_evt->staco_nmuon)) );
  }
  else if(strcmp(m_type, "stac") == 0) err = sqrt( (p_evt->staco_covr11)->at(irec) );
  else if(strcmp(m_lieu, "ID") == 0)   err = sqrt( (p_evt->MboyESD_covr11)->at(irec) + (p_evt->Trk_sigd0)->at(isim)*(p_evt->Trk_sigd0)->at(isim) );  
  else if(strcmp(m_type, "idet") == 0) err = (p_evt->Trk_sigd0)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   err = 1.;
  else                                 err = sqrt( (p_evt->MboyESD_covr11)->at(irec) );
  return getDeltaD0(irec,isim)/err;
}



float Assoc::getPullZ0(int irec, int isim) {
  float err;
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    err = sqrt( (p_evt->staco_covr22)->at(irec) );
    else                               err = sqrt( (p_evt->MuTag_covr22)->at(irec-(p_evt->staco_nmuon)) );
  }
  else if(strcmp(m_type, "stac") == 0) err = sqrt( (p_evt->staco_covr22)->at(irec) );
  else if(strcmp(m_lieu, "ID") == 0)   err = sqrt( (p_evt->MboyESD_covr22)->at(irec) + (p_evt->Trk_sigz0)->at(isim)*(p_evt->Trk_sigz0)->at(isim) );  
  else if(strcmp(m_type, "idet") == 0) err = (p_evt->Trk_sigz0)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   err = 1.;
  else                                 err = sqrt( (p_evt->MboyESD_covr22)->at(irec) );
  return getDeltaZ0(irec,isim)/err;
}



float Assoc::getPullPhi(int irec, int isim) {
  float err;
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    err = sqrt( (p_evt->staco_covr33)->at(irec) );
    else                               err = sqrt( (p_evt->MuTag_covr33)->at(irec-(p_evt->staco_nmuon)) );
  }
  else if(strcmp(m_type, "stac") == 0) err = sqrt( (p_evt->staco_covr33)->at(irec) );
  else if(strcmp(m_lieu, "ID") == 0)   err = sqrt( (p_evt->MboyESD_covr33)->at(irec) + (p_evt->Trk_sigphi)->at(isim)*(p_evt->Trk_sigphi)->at(isim) );  
  else if(strcmp(m_type, "idet") == 0) err = (p_evt->Trk_sigphi)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   err = sqrt( (p_evt->MboyTrkMS_Cov44)->at(irec) );
  else                                 err = sqrt( (p_evt->MboyESD_covr33)->at(irec) );
  return getDeltaPhi(irec,isim)/err;
}


float Assoc::getPullTheta(int irec, int isim) {
  float err;
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    err = sqrt( (p_evt->staco_covr44)->at(irec) );
    else                               err = sqrt( (p_evt->MuTag_covr44)->at(irec-(p_evt->staco_nmuon)) );
  }
  else if(strcmp(m_type, "stac") == 0) err = sqrt( (p_evt->staco_covr44)->at(irec) );
  else if(strcmp(m_lieu, "ID") == 0)   err = sqrt( (p_evt->MboyESD_covr44)->at(irec) + (p_evt->Trk_sigtheta)->at(isim)*(p_evt->Trk_sigtheta)->at(isim) );
  else if(strcmp(m_type, "idet") == 0) err = (p_evt->Trk_sigtheta)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   err = sqrt( (p_evt->MboyTrkMS_Cov33)->at(irec) );
  else                                 err = sqrt( (p_evt->MboyESD_covr44)->at(irec) );
  return getDeltaTheta(irec,isim)/err;
}


float Assoc::getPullUnSurP(int irec, int isim) {
  float err;
  if(strcmp(m_type, "mtag") == 0) { 
    if( irec < p_evt->staco_nmuon )    err = sqrt( (p_evt->staco_covr55)->at(irec) );
    else                               err = sqrt( (p_evt->MuTag_covr55)->at(irec-(p_evt->staco_nmuon)) );
  }
  else if(strcmp(m_type, "stac") == 0) err = sqrt( (p_evt->staco_covr55)->at(irec) );
  else if(strcmp(m_lieu, "ID") == 0)   err = sqrt( (p_evt->MboyESD_covr55)->at(irec) + (p_evt->Trk_sigqOverP)->at(isim)*(p_evt->Trk_sigqOverP)->at(isim) );
  else if(strcmp(m_type, "idet") == 0) err = (p_evt->Trk_sigqOverP)->at(irec);
  else if(strcmp(m_lieu, "MS") == 0)   err = sqrt( (p_evt->MboyTrkMS_Cov55)->at(irec) );
  else                                 err = sqrt( (p_evt->MboyESD_covr55)->at(irec) );
  return getDeltaUnSurP(irec,isim)/err;
}



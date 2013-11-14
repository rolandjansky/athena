/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: $
#ifndef Assoc_h
#define Assoc_h
#include "HasEvt.h"

// Include files
#include "AANTMisc.h"

/** @class Assoc Assoc.h source/Assoc.h
 *  
 *
 *  @author SaMuSoG
 *  @date   2005-06-03
 */
class Assoc: public HasEvt {
public: 
  Assoc(char * s, char * v = "IP");
  ~Assoc( );

public:
///////////////////////////////////
// Perform Association
   void associe();
   void setCuts(int nsigAng, int nsigMom);

// Is the simulated track a muon
   bool isMuonSimul(int i);
   bool isMuonDirect(int i);
// Is the track NOT a muon candidate
    bool isFake(int i);
// Is the track within Angular cut but fails Chi2 cut
    bool isDuplicated(int i);
// Is the track outside angular cut
    bool isAdditional(int i);
// Is the track a muon (within anglura and chi2 cuts)
    bool isMuon(int i);
// Is the track irec inside cone of isim
    bool isInsideCone(int irec, int isim);

    int getNsim();
    int getNrec();
// Get associated gen Muon
    int getSim(int i);
    int getConeSim(int i);
// get associated rec Muon
    int getRec(int i);
    int getNCone(int i);

// Close
    void close();

    // Book Histos
    void BookHistos();

// Dump info on Association
    void InfoAssociation();

    float getEtaSim(int isim);
    float getD0Sim(int isim);
    float getZ0Sim(int isim);
    float getPhiSim(int isim);
    float getThetaSim(int isim);
    float getChargeSurPSim(int isim);
    float getEtaRec(int irec);
    float getD0Rec(int irec);
    float getZ0Rec(int irec);
    float getPhiRec(int irec);
    float getThetaRec(int irec);
    float getChargeSurPRec(int irec);
    float getPtRec(int irec);

    float getDeltaD0(int irec, int isim);
    float getDeltaZ0(int irec, int isim);
    float getDeltaPhi(int irec, int isim);
    float getDeltaEta(int irec, int isim);
    float getDeltaTheta(int irec, int isim);
    float getDeltaUnSurP(int irec, int isim);
    float getDeltaPSurPnoSign(int irec, int isim);
    float getDeltaPSurP(int irec, int isim);
    float getDeltaP(int irec, int isim);
    float getPullD0(int irec, int isim);
    float getPullZ0(int irec, int isim);
    float getPullPhi(int irec, int isim);
    float getPullTheta(int irec, int isim);
    float getPullUnSurP(int irec, int isim);

    char *  m_type;
    char *  m_lieu;

private:
///////////////////////////////////

// Association type definition
//    char *  m_type;
//    char *  m_lieu;
//
// Cuts
    float m_AngCut;
    float m_MomCut;

// Sim stuff
    int   m_Nsim ;
    int   m_NassoCone[4000];        // Nb of associated muons (angular cut)
    int   m_listAssoCone[4000][50]; // list of associated muons (angular cut)

// Rec stuff
    int   m_Nrec ;
    int   m_coneRec[4000];
    int   m_assoRec[4000];

    float m_PI ;
    int   m_NMAX ;

};
#endif // 

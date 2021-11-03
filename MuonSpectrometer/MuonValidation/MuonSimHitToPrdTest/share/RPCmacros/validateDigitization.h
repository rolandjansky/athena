//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Oct 19 13:35:20 2015 by ROOT version 6.02/08
// from TTree 1/RPC
// found on file: /afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_test.root
//////////////////////////////////////////////////////////

#ifndef validateDigitization_h
#define validateDigitization_h

#include <TChain.h>
#include <TFile.h>
#include <TROOT.h>

#include <iostream>
#include <string>

class TVector3;

// Header file for the classes stored in the TTree if any.

class validateDigitization {
public:
    TTree *fChain;   //! pointer to the analyzed TTree or TChain
    Int_t fCurrent;  //! current Tree number in a TChain

    // Fixed size dimensions of array or collections stored in the TTree if any.

    // Declaration of leaf types
    Int_t Run;
    Int_t Event;
    Int_t NpartStored;
    Float_t partVertX[100];  //[NpartStored]
    Float_t partVertY[100];  //[NpartStored]
    Float_t partVertZ[100];  //[NpartStored]
    Float_t partVertT[100];  //[NpartStored]
    Float_t partPdgId[100];  //[NpartStored]
    Float_t partMomX[100];   //[NpartStored]
    Float_t partMomY[100];   //[NpartStored]
    Float_t partMomZ[100];   //[NpartStored]
    Int_t partStatus[100];   //[NpartStored]
    Int_t partBarcode[100];  //[NpartStored]
    Float_t partMass[100];   //[NpartStored]
    Int_t rpc_Nsimhit;
    Int_t rpc_simhit_station[10000];        //[rpc_Nsimhit]
    Int_t rpc_simhit_eta[10000];            //[rpc_Nsimhit]
    Int_t rpc_simhit_phi[10000];            //[rpc_Nsimhit]
    Int_t rpc_simhit_doublr[10000];         //[rpc_Nsimhit]
    Int_t rpc_simhit_doublz[10000];         //[rpc_Nsimhit]
    Int_t rpc_simhit_doublphi[10000];       //[rpc_Nsimhit]
    Int_t rpc_simhit_gasgap[10000];         //[rpc_Nsimhit]
    Int_t rpc_simhit_measphi[10000];        //[rpc_Nsimhit]
    Float_t rpc_simhit_time[10000];         //[rpc_Nsimhit]
    Float_t rpc_simhit_localx[10000];       //[rpc_Nsimhit]
    Float_t rpc_simhit_localy[10000];       //[rpc_Nsimhit]
    Float_t rpc_simhit_localz[10000];       //[rpc_Nsimhit]
    Float_t rpc_simhit_globalx[10000];      //[rpc_Nsimhit]
    Float_t rpc_simhit_globaly[10000];      //[rpc_Nsimhit]
    Float_t rpc_simhit_globalz[10000];      //[rpc_Nsimhit]
    Float_t rpc_simhit_trackNumber[10000];  //[rpc_Nsimhit]
    Float_t rpc_simhit_stepLen[10000];      //[rpc_Nsimhit]
    Float_t rpc_simhit_energyDep[10000];    //[rpc_Nsimhit]
    Float_t rpc_simhit_kinEnergy[10000];    //[rpc_Nsimhit]
    Int_t rpc_simhit_pdgId[10000];          //[rpc_Nsimhit]
    Int_t rpc_NdigitColl;
    Int_t rpc_Ndigit;
    Int_t rpc_digit_station[10000];   //[rpc_Ndigit]
    Int_t rpc_digit_eta[10000];       //[rpc_Ndigit]
    Int_t rpc_digit_phi[10000];       //[rpc_Ndigit]
    Int_t rpc_digit_doublr[10000];    //[rpc_Ndigit]
    Int_t rpc_digit_doublz[10000];    //[rpc_Ndigit]
    Int_t rpc_digit_doublphi[10000];  //[rpc_Ndigit]
    Int_t rpc_digit_gasgap[10000];    //[rpc_Ndigit]
    Int_t rpc_digit_measphi[10000];   //[rpc_Ndigit]
    Int_t rpc_digit_strip[10000];     //[rpc_Ndigit]
    Float_t rpc_digit_time[10000];    //[rpc_Ndigit]
    Float_t rpc_digit_stripx[10000];  //[rpc_Ndigit]
    Float_t rpc_digit_stripy[10000];  //[rpc_Ndigit]
    Float_t rpc_digit_stripz[10000];  //[rpc_Ndigit]
    Int_t rpc_Npad;
    Int_t rpc_pad_id[100];        //[rpc_Npad]
    Int_t rpc_pad_bcid[100];      //[rpc_Npad]
    Int_t rpc_pad_lvl1id[100];    //[rpc_Npad]
    Int_t rpc_pad_sectorid[100];  //[rpc_Npad]
    Int_t rpc_pad_error[100];     //[rpc_Npad]
    Int_t rpc_pad_status[100];    //[rpc_Npad]
    Int_t rpc_Ncm;
    Int_t rpc_Nrdo;
    Int_t rpc_rdo_sector[10000];     //[rpc_Nrdo]
    Int_t rpc_rdo_padId[10000];      //[rpc_Nrdo]
    Int_t rpc_rdo_status[10000];     //[rpc_Nrdo]
    Int_t rpc_rdo_errcode[10000];    //[rpc_Nrdo]
    Int_t rpc_rdo_cmaId[10000];      //[rpc_Nrdo]
    Int_t rpc_rdo_fel1Id[10000];     //[rpc_Nrdo]
    Int_t rpc_rdo_febcId[10000];     //[rpc_Nrdo]
    Int_t rpc_rdo_crc[10000];        //[rpc_Nrdo]
    Int_t rpc_rdo_bcId[10000];       //[rpc_Nrdo]
    Int_t rpc_rdo_ticks[10000];      //[rpc_Nrdo]
    Int_t rpc_rdo_ijk[10000];        //[rpc_Nrdo]
    Int_t rpc_rdo_cmachan[10000];    //[rpc_Nrdo]
    Int_t rpc_rdo_overlap[10000];    //[rpc_Nrdo]
    Int_t rpc_rdo_threshold[10000];  //[rpc_Nrdo]
    Int_t rpc_NprdColl;
    Int_t rpc_Nprd;
    Int_t rpc_prd_station[10000];        //[rpc_Nprd]
    Int_t rpc_prd_eta[10000];            //[rpc_Nprd]
    Int_t rpc_prd_phi[10000];            //[rpc_Nprd]
    Int_t rpc_prd_doublr[10000];         //[rpc_Nprd]
    Int_t rpc_prd_doublz[10000];         //[rpc_Nprd]
    Int_t rpc_prd_doublphi[10000];       //[rpc_Nprd]
    Int_t rpc_prd_gasgap[10000];         //[rpc_Nprd]
    Int_t rpc_prd_measphi[10000];        //[rpc_Nprd]
    Int_t rpc_prd_strip[10000];          //[rpc_Nprd]
    Float_t rpc_prd_time[10000];         //[rpc_Nprd]
    Float_t rpc_prd_stripx[10000];       //[rpc_Nprd]
    Float_t rpc_prd_stripy[10000];       //[rpc_Nprd]
    Float_t rpc_prd_stripz[10000];       //[rpc_Nprd]
    Float_t rpc_prd_triggerInfo[10000];  //[rpc_Nprd]
    Float_t rpc_prd_ambigFlag[10000];    //[rpc_Nprd]

    // List of branches
    TBranch *b_Run;                     //!
    TBranch *b_Event;                   //!
    TBranch *b_NpartStored;             //!
    TBranch *b_partVertX;               //!
    TBranch *b_partVertY;               //!
    TBranch *b_partVertZ;               //!
    TBranch *b_partVertT;               //!
    TBranch *b_partPdgId;               //!
    TBranch *b_partMomX;                //!
    TBranch *b_partMomY;                //!
    TBranch *b_partMomZ;                //!
    TBranch *b_partStatus;              //!
    TBranch *b_partBarcode;             //!
    TBranch *b_partMass;                //!
    TBranch *b_rpc_Nsimhit;             //!
    TBranch *b_rpc_simhit_station;      //!
    TBranch *b_rpc_simhit_eta;          //!
    TBranch *b_rpc_simhit_phi;          //!
    TBranch *b_rpc_simhit_doublr;       //!
    TBranch *b_rpc_simhit_doublz;       //!
    TBranch *b_rpc_simhit_doublphi;     //!
    TBranch *b_rpc_simhit_gasgap;       //!
    TBranch *b_rpc_simhit_measphi;      //!
    TBranch *b_rpc_simhit_time;         //!
    TBranch *b_rpc_simhit_localx;       //!
    TBranch *b_rpc_simhit_localy;       //!
    TBranch *b_rpc_simhit_localz;       //!
    TBranch *b_rpc_simhit_globalx;      //!
    TBranch *b_rpc_simhit_globaly;      //!
    TBranch *b_rpc_simhit_globalz;      //!
    TBranch *b_rpc_simhit_trackNumber;  //!
    TBranch *b_rpc_simhit_stepLen;      //!
    TBranch *b_rpc_simhit_energyDep;    //!
    TBranch *b_rpc_simhit_kinEnergy;    //!
    TBranch *b_rpc_simhit_pdgId;        //!
    TBranch *b_rpc_NdigitColl;          //!
    TBranch *b_rpc_Ndigit;              //!
    TBranch *b_rpc_digit_station;       //!
    TBranch *b_rpc_digit_eta;           //!
    TBranch *b_rpc_digit_phi;           //!
    TBranch *b_rpc_digit_doublr;        //!
    TBranch *b_rpc_digit_doublz;        //!
    TBranch *b_rpc_digit_doublphi;      //!
    TBranch *b_rpc_digit_gasgap;        //!
    TBranch *b_rpc_digit_measphi;       //!
    TBranch *b_rpc_digit_strip;         //!
    TBranch *b_rpc_digit_time;          //!
    TBranch *b_rpc_digit_stripx;        //!
    TBranch *b_rpc_digit_stripy;        //!
    TBranch *b_rpc_digit_stripz;        //!
    TBranch *b_rpc_Npad;                //!
    TBranch *b_rpc_pad_id;              //!
    TBranch *b_rpc_pad_bcid;            //!
    TBranch *b_rpc_pad_lvl1id;          //!
    TBranch *b_rpc_pad_sectorid;        //!
    TBranch *b_rpc_pad_error;           //!
    TBranch *b_rpc_pad_status;          //!
    TBranch *b_rpc_Ncm;                 //!
    TBranch *b_rpc_Nrdo;                //!
    TBranch *b_rpc_rdo_sector;          //!
    TBranch *b_rpc_rdo_padId;           //!
    TBranch *b_rpc_rdo_status;          //!
    TBranch *b_rpc_rdo_errcode;         //!
    TBranch *b_rpc_rdo_cmaId;           //!
    TBranch *b_rpc_rdo_fel1Id;          //!
    TBranch *b_rpc_rdo_febcId;          //!
    TBranch *b_rpc_rdo_crc;             //!
    TBranch *b_rpc_rdo_bcId;            //!
    TBranch *b_rpc_rdo_ticks;           //!
    TBranch *b_rpc_rdo_ijk;             //!
    TBranch *b_rpc_rdo_cmachan;         //!
    TBranch *b_rpc_rdo_overlap;         //!
    TBranch *b_rpc_rdo_threshold;       //!
    TBranch *b_rpc_NprdColl;            //!
    TBranch *b_rpc_Nprd;                //!
    TBranch *b_rpc_prd_station;         //!
    TBranch *b_rpc_prd_eta;             //!
    TBranch *b_rpc_prd_phi;             //!
    TBranch *b_rpc_prd_doublr;          //!
    TBranch *b_rpc_prd_doublz;          //!
    TBranch *b_rpc_prd_doublphi;        //!
    TBranch *b_rpc_prd_gasgap;          //!
    TBranch *b_rpc_prd_measphi;         //!
    TBranch *b_rpc_prd_strip;           //!
    TBranch *b_rpc_prd_time;            //!
    TBranch *b_rpc_prd_stripx;          //!
    TBranch *b_rpc_prd_stripy;          //!
    TBranch *b_rpc_prd_stripz;          //!
    TBranch *b_rpc_prd_triggerInfo;     //!
    TBranch *b_rpc_prd_ambigFlag;       //!

    validateDigitization(TTree *tree = 0);
    virtual ~validateDigitization();
    virtual Int_t Cut(Long64_t entry);
    virtual Int_t GetEntry(Long64_t entry);
    virtual Long64_t LoadTree(Long64_t entry);
    virtual void Init(TTree *tree);
    virtual void Loop(Long64_t maxEvent);
    virtual Bool_t Notify();
    virtual void Show(Long64_t entry = -1);

    // PRIVATE METHODS
    void computeDistFromPart(TVector3 &myVect, Double_t &distXY, Double_t &distRZ, Double_t &distZ);
    void computeDistFromHit(TVector3 &myVect, TVector3 &myVectRef, Double_t &distXY, Double_t &distRZ, Double_t &distZ);
};

#endif

#ifdef validateDigitization_cxx
validateDigitization::validateDigitization(TTree *tree) : fChain(0) {
    // if parameter tree is not specified (or zero), connect the file
    // used to generate this class and read the Tree.
    if (tree == 0) {
        std::cout << "::validateDigitization init pointer to tree - now still NULL" << std::endl;
        std::string inputFileName = "/afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_test125k_condRun00281411.root";
        // std::string inputFileName = "/afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_testFixed.root";
        // std::string inputFileName = "/afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_test100kFixed_avEffStep5.root";
        // std::string inputFileName = "/afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_testFixed_avEffStep1.root";
        // std::string inputFileName = "/afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_testFixed_avEffStep2.root";
        // std::string inputFileName = "/afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_test100kFixed_avEffStep3.root";
        // std::string inputFileName = "/afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_testFixed_avEffStep4.root";
        // std::string inputFileName = "/afs/cern.ch/work/s/stefspa/RPCntuple_GeantinoHits_testFixed_avEffStep5.root";
        std::string histoPathName = inputFileName + ":/RPCvalidNT";
        std::cout << "::validateDigitization - using file: " << inputFileName << std::endl;
        TFile *f = (TFile *)gROOT->GetListOfFiles()->FindObject(inputFileName.c_str());
        if (!f || !f->IsOpen()) { f = new TFile(inputFileName.c_str()); }
        TDirectory *dir = (TDirectory *)f->Get(histoPathName.c_str());
        if (dir != NULL) {
            dir->GetObject("1", tree);
            std::cout << "::validateDigitization init pointer to tree - now is " << (Long64_t)tree << std::endl;
        }
    }
    Init(tree);
}

validateDigitization::~validateDigitization() {
    if (!fChain) return;
    delete fChain->GetCurrentFile();
}

Int_t validateDigitization::GetEntry(Long64_t entry) {
    // Read contents of entry.
    if (!fChain) return 0;
    return fChain->GetEntry(entry);
}
Long64_t validateDigitization::LoadTree(Long64_t entry) {
    // Set the environment to read one entry
    if (!fChain) return -5;
    Long64_t centry = fChain->LoadTree(entry);
    if (centry < 0) return centry;
    if (fChain->GetTreeNumber() != fCurrent) {
        fCurrent = fChain->GetTreeNumber();
        Notify();
    }
    return centry;
}

void validateDigitization::Init(TTree *tree) {
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the branch addresses and branch
    // pointers of the tree will be set.
    // It is normally not necessary to make changes to the generated
    // code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running on PROOF
    // (once per file to be processed).

    std::cout << "::Init " << std::endl;

    // Set branch addresses and branch pointers
    if (!tree) {
        std::cout << " trying to init chain with the pointer to the tree ... NULL" << std::endl;
        return;
    } else {
        std::cout << "init chain with the pointer to the tree ... " << std::endl;
    }

    fChain = tree;
    fCurrent = -1;
    fChain->SetMakeClass(1);

    fChain->SetBranchAddress("Run", &Run, &b_Run);
    fChain->SetBranchAddress("Event", &Event, &b_Event);
    fChain->SetBranchAddress("NpartStored", &NpartStored, &b_NpartStored);
    fChain->SetBranchAddress("partVertX", partVertX, &b_partVertX);
    fChain->SetBranchAddress("partVertY", partVertY, &b_partVertY);
    fChain->SetBranchAddress("partVertZ", partVertZ, &b_partVertZ);
    fChain->SetBranchAddress("partVertT", partVertT, &b_partVertT);
    fChain->SetBranchAddress("partPdgId", partPdgId, &b_partPdgId);
    fChain->SetBranchAddress("partMomX", partMomX, &b_partMomX);
    fChain->SetBranchAddress("partMomY", partMomY, &b_partMomY);
    fChain->SetBranchAddress("partMomZ", partMomZ, &b_partMomZ);
    fChain->SetBranchAddress("partStatus", partStatus, &b_partStatus);
    fChain->SetBranchAddress("partBarcode", partBarcode, &b_partBarcode);
    fChain->SetBranchAddress("partMass", partMass, &b_partMass);
    fChain->SetBranchAddress("rpc_Nsimhit", &rpc_Nsimhit, &b_rpc_Nsimhit);
    fChain->SetBranchAddress("rpc_simhit_station", rpc_simhit_station, &b_rpc_simhit_station);
    fChain->SetBranchAddress("rpc_simhit_eta", rpc_simhit_eta, &b_rpc_simhit_eta);
    fChain->SetBranchAddress("rpc_simhit_phi", rpc_simhit_phi, &b_rpc_simhit_phi);
    fChain->SetBranchAddress("rpc_simhit_doublr", rpc_simhit_doublr, &b_rpc_simhit_doublr);
    fChain->SetBranchAddress("rpc_simhit_doublz", rpc_simhit_doublz, &b_rpc_simhit_doublz);
    fChain->SetBranchAddress("rpc_simhit_doublphi", rpc_simhit_doublphi, &b_rpc_simhit_doublphi);
    fChain->SetBranchAddress("rpc_simhit_gasgap", rpc_simhit_gasgap, &b_rpc_simhit_gasgap);
    fChain->SetBranchAddress("rpc_simhit_measphi", rpc_simhit_measphi, &b_rpc_simhit_measphi);
    fChain->SetBranchAddress("rpc_simhit_time", rpc_simhit_time, &b_rpc_simhit_time);
    fChain->SetBranchAddress("rpc_simhit_localx", rpc_simhit_localx, &b_rpc_simhit_localx);
    fChain->SetBranchAddress("rpc_simhit_localy", rpc_simhit_localy, &b_rpc_simhit_localy);
    fChain->SetBranchAddress("rpc_simhit_localz", rpc_simhit_localz, &b_rpc_simhit_localz);
    fChain->SetBranchAddress("rpc_simhit_globalx", rpc_simhit_globalx, &b_rpc_simhit_globalx);
    fChain->SetBranchAddress("rpc_simhit_globaly", rpc_simhit_globaly, &b_rpc_simhit_globaly);
    fChain->SetBranchAddress("rpc_simhit_globalz", rpc_simhit_globalz, &b_rpc_simhit_globalz);
    fChain->SetBranchAddress("rpc_simhit_trackNumber", rpc_simhit_trackNumber, &b_rpc_simhit_trackNumber);
    fChain->SetBranchAddress("rpc_simhit_stepLen", rpc_simhit_stepLen, &b_rpc_simhit_stepLen);
    fChain->SetBranchAddress("rpc_simhit_energyDep", rpc_simhit_energyDep, &b_rpc_simhit_energyDep);
    fChain->SetBranchAddress("rpc_simhit_kinEnergy", rpc_simhit_kinEnergy, &b_rpc_simhit_kinEnergy);
    fChain->SetBranchAddress("rpc_simhit_pdgId", rpc_simhit_pdgId, &b_rpc_simhit_pdgId);
    fChain->SetBranchAddress("rpc_NdigitColl", &rpc_NdigitColl, &b_rpc_NdigitColl);
    fChain->SetBranchAddress("rpc_Ndigit", &rpc_Ndigit, &b_rpc_Ndigit);
    fChain->SetBranchAddress("rpc_digit_station", rpc_digit_station, &b_rpc_digit_station);
    fChain->SetBranchAddress("rpc_digit_eta", rpc_digit_eta, &b_rpc_digit_eta);
    fChain->SetBranchAddress("rpc_digit_phi", rpc_digit_phi, &b_rpc_digit_phi);
    fChain->SetBranchAddress("rpc_digit_doublr", rpc_digit_doublr, &b_rpc_digit_doublr);
    fChain->SetBranchAddress("rpc_digit_doublz", rpc_digit_doublz, &b_rpc_digit_doublz);
    fChain->SetBranchAddress("rpc_digit_doublphi", rpc_digit_doublphi, &b_rpc_digit_doublphi);
    fChain->SetBranchAddress("rpc_digit_gasgap", rpc_digit_gasgap, &b_rpc_digit_gasgap);
    fChain->SetBranchAddress("rpc_digit_measphi", rpc_digit_measphi, &b_rpc_digit_measphi);
    fChain->SetBranchAddress("rpc_digit_strip", rpc_digit_strip, &b_rpc_digit_strip);
    fChain->SetBranchAddress("rpc_digit_time", rpc_digit_time, &b_rpc_digit_time);
    fChain->SetBranchAddress("rpc_digit_stripx", rpc_digit_stripx, &b_rpc_digit_stripx);
    fChain->SetBranchAddress("rpc_digit_stripy", rpc_digit_stripy, &b_rpc_digit_stripy);
    fChain->SetBranchAddress("rpc_digit_stripz", rpc_digit_stripz, &b_rpc_digit_stripz);
    fChain->SetBranchAddress("rpc_Npad", &rpc_Npad, &b_rpc_Npad);
    fChain->SetBranchAddress("rpc_pad_id", rpc_pad_id, &b_rpc_pad_id);
    fChain->SetBranchAddress("rpc_pad_bcid", rpc_pad_bcid, &b_rpc_pad_bcid);
    fChain->SetBranchAddress("rpc_pad_lvl1id", rpc_pad_lvl1id, &b_rpc_pad_lvl1id);
    fChain->SetBranchAddress("rpc_pad_sectorid", rpc_pad_sectorid, &b_rpc_pad_sectorid);
    fChain->SetBranchAddress("rpc_pad_error", rpc_pad_error, &b_rpc_pad_error);
    fChain->SetBranchAddress("rpc_pad_status", rpc_pad_status, &b_rpc_pad_status);
    fChain->SetBranchAddress("rpc_Ncm", &rpc_Ncm, &b_rpc_Ncm);
    fChain->SetBranchAddress("rpc_Nrdo", &rpc_Nrdo, &b_rpc_Nrdo);
    fChain->SetBranchAddress("rpc_rdo_sector", rpc_rdo_sector, &b_rpc_rdo_sector);
    fChain->SetBranchAddress("rpc_rdo_padId", rpc_rdo_padId, &b_rpc_rdo_padId);
    fChain->SetBranchAddress("rpc_rdo_status", rpc_rdo_status, &b_rpc_rdo_status);
    fChain->SetBranchAddress("rpc_rdo_errcode", rpc_rdo_errcode, &b_rpc_rdo_errcode);
    fChain->SetBranchAddress("rpc_rdo_cmaId", rpc_rdo_cmaId, &b_rpc_rdo_cmaId);
    fChain->SetBranchAddress("rpc_rdo_fel1Id", rpc_rdo_fel1Id, &b_rpc_rdo_fel1Id);
    fChain->SetBranchAddress("rpc_rdo_febcId", rpc_rdo_febcId, &b_rpc_rdo_febcId);
    fChain->SetBranchAddress("rpc_rdo_crc", rpc_rdo_crc, &b_rpc_rdo_crc);
    fChain->SetBranchAddress("rpc_rdo_bcId", rpc_rdo_bcId, &b_rpc_rdo_bcId);
    fChain->SetBranchAddress("rpc_rdo_ticks", rpc_rdo_ticks, &b_rpc_rdo_ticks);
    fChain->SetBranchAddress("rpc_rdo_ijk", rpc_rdo_ijk, &b_rpc_rdo_ijk);
    fChain->SetBranchAddress("rpc_rdo_cmachan", rpc_rdo_cmachan, &b_rpc_rdo_cmachan);
    fChain->SetBranchAddress("rpc_rdo_overlap", rpc_rdo_overlap, &b_rpc_rdo_overlap);
    fChain->SetBranchAddress("rpc_rdo_threshold", rpc_rdo_threshold, &b_rpc_rdo_threshold);
    fChain->SetBranchAddress("rpc_NprdColl", &rpc_NprdColl, &b_rpc_NprdColl);
    fChain->SetBranchAddress("rpc_Nprd", &rpc_Nprd, &b_rpc_Nprd);
    fChain->SetBranchAddress("rpc_prd_station", rpc_prd_station, &b_rpc_prd_station);
    fChain->SetBranchAddress("rpc_prd_eta", rpc_prd_eta, &b_rpc_prd_eta);
    fChain->SetBranchAddress("rpc_prd_phi", rpc_prd_phi, &b_rpc_prd_phi);
    fChain->SetBranchAddress("rpc_prd_doublr", rpc_prd_doublr, &b_rpc_prd_doublr);
    fChain->SetBranchAddress("rpc_prd_doublz", rpc_prd_doublz, &b_rpc_prd_doublz);
    fChain->SetBranchAddress("rpc_prd_doublphi", rpc_prd_doublphi, &b_rpc_prd_doublphi);
    fChain->SetBranchAddress("rpc_prd_gasgap", rpc_prd_gasgap, &b_rpc_prd_gasgap);
    fChain->SetBranchAddress("rpc_prd_measphi", rpc_prd_measphi, &b_rpc_prd_measphi);
    fChain->SetBranchAddress("rpc_prd_strip", rpc_prd_strip, &b_rpc_prd_strip);
    fChain->SetBranchAddress("rpc_prd_time", rpc_prd_time, &b_rpc_prd_time);
    fChain->SetBranchAddress("rpc_prd_stripx", rpc_prd_stripx, &b_rpc_prd_stripx);
    fChain->SetBranchAddress("rpc_prd_stripy", rpc_prd_stripy, &b_rpc_prd_stripy);
    fChain->SetBranchAddress("rpc_prd_stripz", rpc_prd_stripz, &b_rpc_prd_stripz);
    fChain->SetBranchAddress("rpc_prd_triggerInfo", rpc_prd_triggerInfo, &b_rpc_prd_triggerInfo);
    fChain->SetBranchAddress("rpc_prd_ambigFlag", rpc_prd_ambigFlag, &b_rpc_prd_ambigFlag);
    Notify();
}

Bool_t validateDigitization::Notify() {
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}

void validateDigitization::Show(Long64_t entry) {
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!fChain) return;
    fChain->Show(entry);
}
Int_t validateDigitization::Cut(Long64_t entry) {
    // This function may be called from Loop.
    // returns  1 if entry is accepted.
    // returns -1 otherwise.
    return 1;
}
#endif  // #ifdef validateDigitization_cxx

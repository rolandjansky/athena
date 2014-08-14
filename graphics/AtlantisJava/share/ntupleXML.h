//ntupleXML v1.0 - Tim Martin - Aug 07 - tamartin@cern.ch
//Classes TopViewXML and EventViewXML to output ntuple
//events to a Atlantis readable xml file

#include "TXMLEngine.h"
#include <TChain.h>
#include <TFriendElement.h>
#include <TFile.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

//Main class, TopView and EventView classes inherit from this
class ntupleXML  {
	protected:
		std::string 	dataTypeCon, etaCon, labelCon, pdgIdCon, phiCon, ptCon, typeEVCon, mainTreeName;
		Bool_t 		FR_Active, TR_Active, FRA_Active, TRA_Active, TA_Active, isTopView;
		Int_t		nParticles, Warn, truthPref, Append;
		TChain* 	activeFile;
		vector<TChain*>	fileStore;
		TTree*		activeTree;
		TFriendElement*	friendTree;
		TXMLEngine* 	TVXMLEng;
		XMLNodePointer_t	event;
   		Int_t           FR_eventNumber;
   		Int_t           FR_runNumber;
		Double_t	FR_MET_Final_ex;
		Double_t	FR_MET_Final_ey;
		Double_t	FR_MET_Final_et;
		Double_t	TR_MET_Truth_ex;
		Double_t	TR_MET_Truth_ey;
		Double_t	TR_MET_Truth_et;
   		Int_t           FR_El_N;
   		vector<double>*  FR_El_pdgId;
  		vector<double>*  FR_El_p_T;
  		vector<double>*  FR_El_phi;
   		vector<double>*  FR_El_eta;
   		Int_t            FR_Mu_N;
   		vector<double>*  FR_Mu_pdgId;
  		vector<double>*  FR_Mu_p_T;
  		vector<double>*  FR_Mu_phi;
   		vector<double>*  FR_Mu_eta;
   		Int_t            FR_Tau_N;
   		vector<double>*  FR_Tau_pdgId;
  		vector<double>*  FR_Tau_p_T;
  		vector<double>*  FR_Tau_phi;
   		vector<double>*  FR_Tau_eta;
   		Int_t            FR_Ph_N;
   		vector<double>*  FR_Ph_pdgId;
  		vector<double>*  FR_Ph_p_T;
  		vector<double>*  FR_Ph_phi;
   		vector<double>*  FR_Ph_eta;
   		Int_t            FR_PJet_N;
   		vector<double>*  FR_PJet_pdgId;
   		vector<double>*  FR_PJet_p_T;
   		vector<double>*  FR_PJet_phi;
   		vector<double>*  FR_PJet_eta;
   		vector<int>*	 FR_PJet_BTagged;
		Int_t		 FRA_El_N;
   		vector<double>*  FRA_El_pdgId;
  		vector<double>*  FRA_El_p_T;
  		vector<double>*  FRA_El_phi;
   		vector<double>*  FRA_El_eta;
   		Int_t            FRA_Mu_N;
   		vector<double>*  FRA_Mu_pdgId;
  		vector<double>*  FRA_Mu_p_T;
  		vector<double>*  FRA_Mu_phi;
   		vector<double>*  FRA_Mu_eta;
   		Int_t            FRA_Nu_N;
   		vector<double>*  FRA_Nu_pdgId;
  		vector<double>*  FRA_Nu_p_T;
  		vector<double>*  FRA_Nu_phi;
   		vector<double>*  FRA_Nu_eta;
   		Int_t            FRA_HadW_N;
   		vector<double>*  FRA_HadW_pdgId;
  		vector<double>*  FRA_HadW_p_T;
  		vector<double>*  FRA_HadW_phi;
   		vector<double>*  FRA_HadW_eta;
   		Int_t            FRA_LepW_N;
   		vector<double>*  FRA_LepW_pdgId;
  		vector<double>*  FRA_LepW_p_T;
  		vector<double>*  FRA_LepW_phi;
   		vector<double>*  FRA_LepW_eta;
   		Int_t            FRA_HadTop_N;
   		vector<double>*  FRA_HadTop_pdgId;
  		vector<double>*  FRA_HadTop_p_T;
  		vector<double>*  FRA_HadTop_phi;
   		vector<double>*  FRA_HadTop_eta;
   		Int_t            FRA_LepTop_N;
   		vector<double>*  FRA_LepTop_pdgId;
  		vector<double>*  FRA_LepTop_p_T;
  		vector<double>*  FRA_LepTop_phi;
   		vector<double>*  FRA_LepTop_eta;
   		Int_t            FRA_Lepb_N;
   		vector<double>*  FRA_Lepb_pdgId;
  		vector<double>*  FRA_Lepb_p_T;
  		vector<double>*  FRA_Lepb_phi;
   		vector<double>*  FRA_Lepb_eta;
   		Int_t            FRA_Hadb_N;
   		vector<double>*  FRA_Hadb_pdgId;
  		vector<double>*  FRA_Hadb_p_T;
  		vector<double>*  FRA_Hadb_phi;
   		vector<double>*  FRA_Hadb_eta;
   		Int_t            FRA_Hadj_N;
   		vector<double>*  FRA_Hadj_pdgId;
  		vector<double>*  FRA_Hadj_p_T;
  		vector<double>*  FRA_Hadj_phi;
   		vector<double>*  FRA_Hadj_eta;
		Int_t		 TRA_El_N;
   		vector<double>*  TRA_El_pdgId;
  		vector<double>*  TRA_El_p_T;
  		vector<double>*  TRA_El_phi;
   		vector<double>*  TRA_El_eta;
   		Int_t            TRA_Mu_N;
   		vector<double>*  TRA_Mu_pdgId;
  		vector<double>*  TRA_Mu_p_T;
  		vector<double>*  TRA_Mu_phi;
   		vector<double>*  TRA_Mu_eta;
   		Int_t            TRA_Tau_N;
   		vector<double>*  TRA_Tau_pdgId;
  		vector<double>*  TRA_Tau_p_T;
  		vector<double>*  TRA_Tau_phi;
   		vector<double>*  TRA_Tau_eta;
   		Int_t            TRA_HadW_N;
   		vector<double>*  TRA_HadW_pdgId;
  		vector<double>*  TRA_HadW_p_T;
  		vector<double>*  TRA_HadW_phi;
   		vector<double>*  TRA_HadW_eta;
   		Int_t            TRA_LepW_N;
   		vector<double>*  TRA_LepW_pdgId;
  		vector<double>*  TRA_LepW_p_T;
  		vector<double>*  TRA_LepW_phi;
   		vector<double>*  TRA_LepW_eta;
   		Int_t            TRA_HadTop_N;
   		vector<double>*  TRA_HadTop_pdgId;
  		vector<double>*  TRA_HadTop_p_T;
  		vector<double>*  TRA_HadTop_phi;
   		vector<double>*  TRA_HadTop_eta;
   		Int_t            TRA_LepTop_N;
   		vector<double>*  TRA_LepTop_pdgId;
  		vector<double>*  TRA_LepTop_p_T;
  		vector<double>*  TRA_LepTop_phi;
   		vector<double>*  TRA_LepTop_eta;
   		Int_t            TRA_Bot_N;
   		vector<double>*  TRA_Bot_pdgId;
  		vector<double>*  TRA_Bot_p_T;
  		vector<double>*  TRA_Bot_phi;
   		vector<double>*  TRA_Bot_eta;
   		Int_t            TRA_LQ_N;
   		vector<double>*  TRA_LQ_pdgId;
  		vector<double>*  TRA_LQ_p_T;
  		vector<double>*  TRA_LQ_phi;
   		vector<double>*  TRA_LQ_eta;
		vector<double>*	 TA_Tru_p_T;
		vector<double>*	 TA_Tru_phi;
		vector<double>*	 TA_Tru_eta;
		vector<double>*	 TA_Tru_pdgId;
		vector<int>*	 TA_Tru_barcode;
   		TBranch*	b_FR_eventNumber;
   		TBranch*	b_FR_runNumber;
   		TBranch*	b_FR_MET_Final_ex;
   		TBranch*	b_FR_MET_Final_ey;
   		TBranch*	b_FR_MET_Final_et;
   		TBranch*	b_TR_MET_Truth_ex;
   		TBranch*	b_TR_MET_Truth_ey;
   		TBranch*	b_TR_MET_Truth_et;
   		TBranch*        b_FR_El_N;
   		TBranch*        b_FR_El_pdgId;
   		TBranch*        b_FR_El_p_T;
   		TBranch*        b_FR_El_phi;
   		TBranch*        b_FR_El_eta;
   		TBranch*        b_FR_Mu_N;
   		TBranch*        b_FR_Mu_pdgId;
   		TBranch*        b_FR_Mu_p_T;
   		TBranch*        b_FR_Mu_phi;
   		TBranch*    	b_FR_Mu_eta;
		TBranch*        b_FR_Tau_N;
   		TBranch*        b_FR_Tau_pdgId;
   		TBranch*        b_FR_Tau_p_T;
   		TBranch*        b_FR_Tau_phi;
   		TBranch*        b_FR_Tau_eta;
   		TBranch*        b_FR_Ph_N;
   		TBranch*        b_FR_Ph_pdgId;
   		TBranch*        b_FR_Ph_p_T;
   		TBranch*        b_FR_Ph_phi;
   		TBranch*        b_FR_Ph_eta;
   		TBranch*        b_FR_PJet_N;
   		TBranch*        b_FR_PJet_pdgId;
   		TBranch*        b_FR_PJet_p_T;
   		TBranch*        b_FR_PJet_phi;
   		TBranch*        b_FR_PJet_eta;
   		TBranch* 	b_FR_PJet_BTagged;
		TBranch*	b_FRA_El_N;
   		TBranch*	b_FRA_El_pdgId;
  		TBranch*	b_FRA_El_p_T;
  		TBranch*	b_FRA_El_phi;
   		TBranch*  	b_FRA_El_eta;
   		TBranch*	b_FRA_Mu_N;
   		TBranch*  	b_FRA_Mu_pdgId;
  		TBranch*	b_FRA_Mu_p_T;
  		TBranch*	b_FRA_Mu_phi;
 		TBranch*	b_FRA_Mu_eta;
   		TBranch*	b_FRA_Nu_N;
   		TBranch*  	b_FRA_Nu_pdgId;
  		TBranch*	b_FRA_Nu_p_T;
  		TBranch*	b_FRA_Nu_phi;
 		TBranch*	b_FRA_Nu_eta;
   		TBranch*	b_FRA_HadW_N;
   		TBranch*	b_FRA_HadW_pdgId;
  		TBranch*	b_FRA_HadW_p_T;
  		TBranch*	b_FRA_HadW_phi;
   		TBranch*	b_FRA_HadW_eta;
   		TBranch*	b_FRA_LepW_N;
   		TBranch*	b_FRA_LepW_pdgId;
  		TBranch*	b_FRA_LepW_p_T;
  		TBranch*	b_FRA_LepW_phi;
   		TBranch*	b_FRA_LepW_eta;
   		TBranch*	b_FRA_HadTop_N;
   		TBranch*	b_FRA_HadTop_pdgId;
  		TBranch*	b_FRA_HadTop_p_T;
  		TBranch*	b_FRA_HadTop_phi;
   		TBranch*	b_FRA_HadTop_eta;
   		TBranch*	b_FRA_LepTop_N;
   		TBranch*	b_FRA_LepTop_pdgId;
  		TBranch*	b_FRA_LepTop_p_T;
  		TBranch*	b_FRA_LepTop_phi;
   		TBranch*	b_FRA_LepTop_eta;
   		TBranch*	b_FRA_Lepb_N;
   		TBranch*	b_FRA_Lepb_pdgId;
  		TBranch*	b_FRA_Lepb_p_T;
  		TBranch*	b_FRA_Lepb_phi;
   		TBranch*	b_FRA_Lepb_eta;
   		TBranch*	b_FRA_Hadb_N;
   		TBranch*	b_FRA_Hadb_pdgId;
  		TBranch*	b_FRA_Hadb_p_T;
  		TBranch*	b_FRA_Hadb_phi;
   		TBranch*	b_FRA_Hadb_eta;
   		TBranch*	b_FRA_Hadj_N;
   		TBranch*	b_FRA_Hadj_pdgId;
  		TBranch*	b_FRA_Hadj_p_T;
  		TBranch*	b_FRA_Hadj_phi;
   		TBranch*	b_FRA_Hadj_eta;
		TBranch*	b_TRA_El_N;
   		TBranch*	b_TRA_El_pdgId;
  		TBranch*	b_TRA_El_p_T;
  		TBranch*	b_TRA_El_phi;
   		TBranch*	b_TRA_El_eta;
   		TBranch*        b_TRA_Mu_N;
   		TBranch*	b_TRA_Mu_pdgId;
  		TBranch*	b_TRA_Mu_p_T;
  		TBranch*	b_TRA_Mu_phi;
   		TBranch*	b_TRA_Mu_eta;
   		TBranch*        b_TRA_Tau_N;
   		TBranch*	b_TRA_Tau_pdgId;
  		TBranch*	b_TRA_Tau_p_T;
  		TBranch*	b_TRA_Tau_phi;
   		TBranch*	b_TRA_Tau_eta;
   		TBranch*        b_TRA_HadW_N;
   		TBranch*	b_TRA_HadW_pdgId;
  		TBranch*	b_TRA_HadW_p_T;
  		TBranch*	b_TRA_HadW_phi;
   		TBranch*	b_TRA_HadW_eta;
   		TBranch*        b_TRA_LepW_N;
   		TBranch*	b_TRA_LepW_pdgId;
  		TBranch*	b_TRA_LepW_p_T;
  		TBranch*	b_TRA_LepW_phi;
   		TBranch*	b_TRA_LepW_eta;
   		TBranch*        b_TRA_HadTop_N;
   		TBranch*	b_TRA_HadTop_pdgId;
  		TBranch*	b_TRA_HadTop_p_T;
  		TBranch*	b_TRA_HadTop_phi;
   		TBranch*	b_TRA_HadTop_eta;
   		TBranch*        b_TRA_LepTop_N;
   		TBranch*	b_TRA_LepTop_pdgId;
  		TBranch*	b_TRA_LepTop_p_T;
  		TBranch*	b_TRA_LepTop_phi;
   		TBranch*	b_TRA_LepTop_eta;
   		TBranch*        b_TRA_Bot_N;
   		TBranch*	b_TRA_Bot_pdgId;
  		TBranch*	b_TRA_Bot_p_T;
  		TBranch*	b_TRA_Bot_phi;
   		TBranch*	b_TRA_Bot_eta;
   		TBranch*        b_TRA_LQ_N;
   		TBranch*	b_TRA_LQ_pdgId;
  		TBranch*	b_TRA_LQ_p_T;
  		TBranch*	b_TRA_LQ_phi;
   		TBranch*	b_TRA_LQ_eta;
		TBranch*	b_TA_Tru_p_T;
		TBranch*	b_TA_Tru_phi;
		TBranch*	b_TA_Tru_eta;
		TBranch*	b_TA_Tru_pdgId;
		TBranch*	b_TA_Tru_barcode;
		//Protected memeber functions
		template<typename TYPE>
		std::string	asString(const TYPE&);
		void 	SetBranches(void);
		void	FillBranches(Int_t entryNumber);
		void	SetActiveFile(Int_t fileNumber);
		void	GetFullReco(void);
		void	GetFullRecoAna(void);
		void	GetTruthAll(Bool_t onlyMuons);
		void	GetTruthAna(Bool_t onlyMuons);
		void	GetMET(Bool_t getTruth);
		void 	StartXML(void);
		void 	FillXML_ParticleDump(Char_t* nodeName, Char_t* storeGateKey);
		void 	FillXML_TruthDump(Char_t* nodeName, Char_t* storeGateKey);
		void 	FillXML_MET(Char_t* storeGateKey);
		void 	EndXML(Char_t* outputFilename);
	public:
		Bool_t 	ntuple2XML(Char_t* outputFilename, UInt_t entryNumber, UInt_t fileNumber);
		void	Add(Char_t* fileLocation);
		void	Clear(void);
		Int_t	GetNfiles(void) { return fileStore.size(); }
		Int_t	GetNentries(UInt_t fileNumber);
		Bool_t	SetWarnLevel(UInt_t warnLevel);
		Bool_t 	SetTruthPreference(UInt_t truthPreference);
		Bool_t	SetFilenameAppend(UInt_t filenameAppend);
		
};

struct TopViewXML: public ntupleXML {
	TopViewXML(Char_t* fileLocation);
	TopViewXML(void);
	~TopViewXML(void) {}
};

struct EventViewXML: public ntupleXML {
	EventViewXML(Char_t* fileLocation);
	EventViewXML(void);
	~EventViewXML(void) {}
};

//Constructors//

TopViewXML::TopViewXML(Char_t* fileLocation) {
	isTopView=1;
	Append=1; Warn=1; truthPref=2;
	mainTreeName = "FullReco0";
	ntupleXML::Add(fileLocation);
}

TopViewXML::TopViewXML(void) {
	isTopView=1;
	Append=1; Warn=1; truthPref=2;
	mainTreeName = "FullReco0";
}

EventViewXML::EventViewXML(Char_t* fileLocation) {
	isTopView=0;
	Append=1; Warn=1; truthPref=2;
	mainTreeName = "CollectionTree";
	ntupleXML::Add(fileLocation);
}

EventViewXML::EventViewXML(void) {
	isTopView=0;
	Append=1; Warn=1; truthPref=2;
	mainTreeName = "CollectionTree";
}		

//Inherited public methods//

//ntuple2XML takes an entry from a file loaded in the internal File Chain and writes an
//atlantis xml file for the event.
Bool_t ntupleXML::ntuple2XML(Char_t* outputFilename, UInt_t entryNumber, UInt_t fileNumber) {

	if (fileNumber >= fileStore.size()) {
		if (Warn>0) {	std::cout << "\nntupleXML: Error cannot access file #" << fileNumber
			  		  << ", " << fileStore.size() << " file(s) currently loaded. ";
		}
		return 0;
	}

	//Set the required file as activeFile, main tree as activeTree & associate branches
	ntupleXML::SetActiveFile(fileNumber);

	if (entryNumber >= activeTree->GetEntries()) {
		if (Warn>0) {	std::cout << "\nntupleXML: Error cannot access entry #" << entryNumber
			  	<< ", file #" << fileNumber << " contains " << activeTree->GetEntries() << " entries. ";
		}
		return 0;
	}

	//Cannot proceed without FullReco/CollectionTree branches set
	if (!FR_Active) { 
		if (Warn>0) { std::cout << "\nntupleXML: Error unable to access " << mainTreeName << " Tree. "; }
		return 0;
	}

	//Retrieve event
	ntupleXML::FillBranches(entryNumber);
	nParticles=0;

	//Prepare XML document
	ntupleXML::StartXML();

	ntupleXML::GetFullReco();
	if (nParticles) { ntupleXML::FillXML_ParticleDump("CompositeParticle","RecoView"); }

	ntupleXML::GetMET(0);
	ntupleXML::FillXML_MET("MET_Final");
	if (TR_Active) {
		ntupleXML::GetMET(1);
		ntupleXML::FillXML_MET("MET_Truth");
	} else if (Warn>1) { std::cout << "\nntupleXML: Warning unable to output MET_Truth. "; }

	if (FRA_Active) {
		ntupleXML::GetFullRecoAna(); 
		if (nParticles) { ntupleXML::FillXML_ParticleDump("CompositeParticle","RecoAnaView"); }
	} else if (Warn>1 && isTopView) { std::cout << "\nntupleXML: Warning unable to output FullRecoAna Tree. "; }
	
	if (TA_Active && truthPref == 1) {
		ntupleXML::GetTruthAll(0); 
		if (nParticles) { ntupleXML::FillXML_TruthDump("STr",""); } 
		ntupleXML::GetTruthAll(1); //Get only muons
		if (nParticles) { ntupleXML::FillXML_TruthDump("SMTr",""); }
	} else if (TRA_Active && truthPref) { 
		ntupleXML::GetTruthAna(0);
		if (nParticles) { ntupleXML::FillXML_TruthDump("STr",""); }	
		ntupleXML::GetTruthAna(1);
		if (nParticles) { ntupleXML::FillXML_TruthDump("SMTr",""); }
	} else if (Warn>1 && truthPref && isTopView) { 
		std::cout << "\nntupleXML: Warning unable to output TruthAna or TruthAll Tree. ";
	}

	//Construct filename
	std::string filenameStr(outputFilename), ext;
	ext = filenameStr.substr(filenameStr.length() - 4, 4);
	if (Append) { 
			if (ext == ".xml" || ext == ".Xml" || ext == ".XML") {
				filenameStr = filenameStr.substr(0, filenameStr.length() - 4) ;
				if (Append==1) { filenameStr += "_" + asString(FR_eventNumber) + ext; }
				else { filenameStr += "_" + asString(entryNumber) + ext; }
			} else { 
				if (Append==1) { filenameStr += "_" + asString(FR_eventNumber) + ".xml"; }
				else { filenameStr += "_" + asString(entryNumber) + ".xml"; }
			 }
	} else {
			if (!(ext == ".xml" || ext == ".Xml" || ext == ".XML")) {
				filenameStr += ".xml";
			}
	}

	//Write .xml file
	ntupleXML::EndXML((Char_t*) filenameStr.c_str());
	if (Warn>1) { std::cout << "\nntupleXML: Output to " << filenameStr; }

	//Return success
	return 1;
}

Bool_t ntupleXML::SetWarnLevel(UInt_t warnLevel) {
	if (warnLevel <= 2) {
		Warn=warnLevel;
		return 1;
	} else { return 0; }
}

Bool_t ntupleXML::SetTruthPreference(UInt_t truthPreference) {
	if (truthPreference <= 2) {
		truthPref=truthPreference;
		return 1;
	} else { return 0; }
}

Bool_t ntupleXML::SetFilenameAppend(UInt_t filenameAppend) {
	if (filenameAppend <= 2) {
		Append=filenameAppend;
		return 1;
	} else { return 0; }
}
		
Int_t ntupleXML::GetNentries(UInt_t fileNumber) {
	if (fileNumber >= fileStore.size()) {
		if (Warn>0) { std::cout << "\nntupleXML: Error cannot access file #" << fileNumber
			                << ", " << fileStore.size() << " file(s) currently loaded. ";
		}
		return 0;
	} else {
		ntupleXML::SetActiveFile(fileNumber);
		return activeTree->GetEntries();
	}
}

void ntupleXML::Add(Char_t* fileLocation) {
	TChain* tf;
	if (isTopView) {
		tf = new TChain("FullReco0");
		tf->Add(fileLocation);
		tf->AddFriend("FullRecoAna0");
		tf->AddFriend("TruthAna0");
		tf->AddFriend("TruthAll0");
		tf->AddFriend("Truth0");
		if ( tf->LoadTree(0) < 0 ) {
			if (Warn>0) { std::cout << "\nntupleXML: Error, file was not added. "; }
			return; 
		}
	} else {
		tf = new TChain("CollectionTree");
		tf->Add(fileLocation);
		if ( tf->LoadTree(0) < 0 ) {			
			if (Warn>0) { std::cout << "\nntupleXML: Error, file was not added. "; }
			return; 
		}

	}
	fileStore.push_back(tf);
}

void ntupleXML::Clear(void) {
	for (UInt_t i=0; i<fileStore.size(); i++) {
		delete fileStore[i];
	}
	fileStore.clear();
	activeFile = NULL;
}

//XML File Construction Members//

void ntupleXML::StartXML(void) {
	//Create XML engine
   	TVXMLEng = new TXMLEngine();
   	event = TVXMLEng->NewChild(0, 0, "Event");
   	TVXMLEng->NewAttr(event, 0, "version", "ntuple");
   	TVXMLEng->NewIntAttr(event, "runNumber", FR_runNumber);
   	TVXMLEng->NewIntAttr(event, "eventNumber", FR_eventNumber);
	TDatime* makeTime = new TDatime();
	Char_t makeTimeStr[26];
	makeTime->AsString(makeTimeStr);	//This 'AsString' is member of TDatime
	delete makeTime;
   	TVXMLEng->NewAttr(event, 0, "dateTime", makeTimeStr);
}

void ntupleXML::FillXML_ParticleDump(Char_t* nodeName, Char_t* storeGateKey) {
	//Create Composite particle subnode
      	XMLNodePointer_t compPart = TVXMLEng->NewChild(event, 0, nodeName);
      	TVXMLEng->NewIntAttr(compPart, "count", nParticles);
     	TVXMLEng->NewAttr(compPart, 0, "storeGateKey", storeGateKey);
		TVXMLEng->NewChild(compPart, 0, "dataType", dataTypeCon.c_str());
		TVXMLEng->NewChild(compPart, 0, "eta", etaCon.c_str());
		TVXMLEng->NewChild(compPart, 0, "label", labelCon.c_str());
		TVXMLEng->NewChild(compPart, 0, "pdgId", pdgIdCon.c_str());
		TVXMLEng->NewChild(compPart, 0, "phi", phiCon.c_str());
		TVXMLEng->NewChild(compPart, 0, "pt", ptCon.c_str());
		TVXMLEng->NewChild(compPart, 0, "typeEV", typeEVCon.c_str());
	dataTypeCon=""; etaCon=""; phiCon=""; labelCon=""; pdgIdCon=""; ptCon=""; typeEVCon="";
	nParticles=0;
}

void ntupleXML::FillXML_TruthDump(Char_t* nodeName, Char_t* storeGateKey) {
	//Create truth subnode
      	XMLNodePointer_t truBlock = TVXMLEng->NewChild(event, 0, nodeName);
      	TVXMLEng->NewIntAttr(truBlock, "count", nParticles);
     	TVXMLEng->NewAttr(truBlock, 0, "storeGateKey", storeGateKey);
		TVXMLEng->NewChild(truBlock, 0, "code", pdgIdCon.c_str());
		TVXMLEng->NewChild(truBlock, 0, "eta", etaCon.c_str());
		TVXMLEng->NewChild(truBlock, 0, "id", labelCon.c_str());
		TVXMLEng->NewChild(truBlock, 0, "phi", phiCon.c_str());
		TVXMLEng->NewChild(truBlock, 0, "phiVertex", dataTypeCon.c_str());
		TVXMLEng->NewChild(truBlock, 0, "pt", ptCon.c_str());
		TVXMLEng->NewChild(truBlock, 0, "rhoVertex", dataTypeCon.c_str());
		TVXMLEng->NewChild(truBlock, 0, "zVertex", dataTypeCon.c_str());
	dataTypeCon=""; etaCon=""; phiCon=""; labelCon=""; pdgIdCon=""; ptCon="";
	nParticles=0;
}

void ntupleXML::FillXML_MET(Char_t* storeGateKey) {
	//Create MET subnode
      	XMLNodePointer_t MET = TVXMLEng->NewChild(event, 0, "ETMis");
      	TVXMLEng->NewIntAttr(MET, "count", 1);
     	TVXMLEng->NewAttr(MET, 0, "storeGateKey", storeGateKey);
		TVXMLEng->NewChild(MET, 0, "et", etaCon.c_str());
		TVXMLEng->NewChild(MET, 0, "etx", phiCon.c_str());
		TVXMLEng->NewChild(MET, 0, "ety", ptCon.c_str());
	etaCon=""; phiCon=""; ptCon="";
}

void ntupleXML::EndXML(Char_t* outputFilename) {
	//Create document 
   	XMLDocPointer_t xmldoc = TVXMLEng->NewDoc();
   	TVXMLEng->DocSetRootElement(xmldoc, event);
   	TVXMLEng->AddDocRawLine(xmldoc, "<?ATLAS Release: \"RELEASE#\"?>");
   	TVXMLEng->AddDocRawLine(xmldoc, "<!DOCTYPE Event SYSTEM \"event.dtd\">");
   	// Save document to file
   	TVXMLEng->SaveDoc(xmldoc, outputFilename);
   	// Release memory before exit
   	TVXMLEng->FreeDoc(xmldoc);
   	delete TVXMLEng;
}

//String Filling Members//

void ntupleXML::GetFullReco(void) {
	std::string id;
	if (isTopView) { id = "FR_"; }
	else { id = "EV_"; }
	for (Int_t i=0; i<FR_El_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVElectron ";
		labelCon    += id + "Electron ";
		etaCon      += asString(FR_El_eta->at(i)) + " ";
		phiCon      += asString(FR_El_phi->at(i)) + " ";
		ptCon       += asString(FR_El_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FR_El_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FR_Mu_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVMuon ";
		labelCon    += id + "Muon ";
		etaCon      += asString(FR_Mu_eta->at(i)) + " ";
		phiCon      += asString(FR_Mu_phi->at(i)) + " ";
		ptCon       += asString(FR_Mu_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FR_Mu_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FR_Tau_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVTauJet ";
		labelCon    += id + "Tau ";
		etaCon      += asString(FR_Tau_eta->at(i)) + " ";
		phiCon      += asString(FR_Tau_phi->at(i)) + " ";
		ptCon       += asString(FR_Tau_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FR_Tau_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FR_Ph_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVPhoton ";
		labelCon    += id + "Photon ";
		etaCon      += asString(FR_Ph_eta->at(i)) + " ";
		phiCon      += asString(FR_Ph_phi->at(i)) + " ";
		ptCon       += asString(FR_Ph_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FR_Ph_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FR_PJet_N; i++) {
		if (isTopView) {
			if (FR_PJet_BTagged->at(i)) { 
				typeEVCon   += "EVBJet ";
				labelCon    += id + "BTaggedJet ";
				pdgIdCon    += "5 ";		
			} else {
				typeEVCon   += "EVParticleJet ";
				labelCon    += id + "ParticleJet ";
				pdgIdCon    += "0 ";	
			}
		} else {
			typeEVCon   += "EVParticleJet ";
			labelCon    += id + "ParticleJet ";
			pdgIdCon    += "5 ";	
		}
		nParticles++;
		dataTypeCon += "1 ";
		etaCon      += asString(FR_PJet_eta->at(i)) + " ";
		phiCon      += asString(FR_PJet_phi->at(i)) + " ";
		ptCon       += asString(FR_PJet_p_T->at(i)/1000.)  + " ";
	}
}

void ntupleXML::GetMET(Bool_t getTruth) {
	if (!getTruth) {
		etaCon	+= asString(FR_MET_Final_et/1000.) + " ";
		phiCon	+= asString(FR_MET_Final_ex/1000.) + " ";
		ptCon	+= asString(FR_MET_Final_ey/1000.) + " ";
	} else {
		etaCon	+= asString(TR_MET_Truth_et/1000.) + " ";
		phiCon	+= asString(TR_MET_Truth_ex/1000.) + " ";
		ptCon	+= asString(TR_MET_Truth_ey/1000.) + " ";
	}
}

void ntupleXML::GetFullRecoAna(void) {
	for (Int_t i=0; i<FRA_El_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVElectron ";
		labelCon    += "FRA_Electron ";
		etaCon      += asString(FRA_El_eta->at(i)) + " ";
		phiCon      += asString(FRA_El_phi->at(i)) + " ";
		ptCon       += asString(FRA_El_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FRA_El_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FRA_Mu_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVMuon ";
		labelCon    += "FRA_Muon ";
		etaCon      += asString(FRA_Mu_eta->at(i)) + " ";
		phiCon      += asString(FRA_Mu_phi->at(i)) + " ";
		ptCon       += asString(FRA_Mu_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FRA_Mu_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FRA_Nu_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVNeutrino ";
		labelCon    += "FRA_Neutrino ";
		etaCon      += asString(FRA_Nu_eta->at(i)) + " ";
		phiCon      += asString(FRA_Nu_phi->at(i)) + " ";
		ptCon       += asString(FRA_Nu_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FRA_Nu_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FRA_HadW_N; i++) {
		nParticles++;
		dataTypeCon += "0 ";
		typeEVCon   += "EVCompositeParticle ";
		labelCon    += "FRA_HadronicW ";
		etaCon      += asString(FRA_HadW_eta->at(i)) + " ";
		phiCon      += asString(FRA_HadW_phi->at(i)) + " ";
		ptCon       += asString(FRA_HadW_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FRA_HadW_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FRA_LepW_N; i++) {
		nParticles++;
		dataTypeCon += "0 ";
		typeEVCon   += "EVCompositeParticle ";
		labelCon    += "FRA_LeptonicW ";
		etaCon      += asString(FRA_LepW_eta->at(i)) + " ";
		phiCon      += asString(FRA_LepW_phi->at(i)) + " ";
		ptCon       += asString(FRA_LepW_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FRA_LepW_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FRA_HadTop_N; i++) {
		nParticles++;
		dataTypeCon += "0 ";
		typeEVCon   += "EVCompositeParticle ";
		labelCon    += "FRA_HadronicTop ";
		etaCon      += asString(FRA_HadTop_eta->at(i)) + " ";
		phiCon      += asString(FRA_HadTop_phi->at(i)) + " ";
		ptCon       += asString(FRA_HadTop_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FRA_HadTop_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FRA_LepTop_N; i++) {
		nParticles++;
		dataTypeCon += "0 ";
		typeEVCon   += "EVCompositeParticle ";
		labelCon    += "FRA_LeptonicTop ";
		etaCon      += asString(FRA_LepTop_eta->at(i)) + " ";
		phiCon      += asString(FRA_LepTop_phi->at(i)) + " ";
		ptCon       += asString(FRA_LepTop_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)FRA_LepTop_pdgId->at(i))  + " ";
	}
	for (Int_t i=0; i<FRA_Lepb_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVBJet ";
		labelCon    += "FRA_LeptonicBJet ";
		etaCon      += asString(FRA_Lepb_eta->at(i)) + " ";
		phiCon      += asString(FRA_Lepb_phi->at(i)) + " ";
		ptCon       += asString(FRA_Lepb_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += "5 ";
	}
	for (Int_t i=0; i<FRA_Hadb_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVBJet ";
		labelCon    += "FRA_HadronicBJet ";
		etaCon      += asString(FRA_Hadb_eta->at(i)) + " ";
		phiCon      += asString(FRA_Hadb_phi->at(i)) + " ";
		ptCon       += asString(FRA_Hadb_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += "5 ";
	}
	for (Int_t i=0; i<FRA_Hadj_N; i++) {
		nParticles++;
		dataTypeCon += "1 ";
		typeEVCon   += "EVParticleJet ";
		labelCon    += "FRA_HadronicParticleJet ";
		etaCon      += asString(FRA_Hadj_eta->at(i)) + " ";
		phiCon      += asString(FRA_Hadj_phi->at(i)) + " ";
		ptCon       += asString(FRA_Hadj_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += "0 ";
	}
}

//onlyMouns is set true when filling SMTr
void ntupleXML::GetTruthAna(Bool_t onlyMuons) {
	Int_t sign;
	for (Int_t i=0; i<TRA_Mu_N; i++) {
		nParticles++;
		etaCon      += asString(TRA_Mu_eta->at(i)) + " ";
		phiCon      += asString(TRA_Mu_phi->at(i)) + " ";
		ptCon       += asString(TRA_Mu_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)TRA_Mu_pdgId->at(i))  + " ";
		labelCon    += "0 ";		//Barcode
		dataTypeCon += "0.000001 ";	//Used to fill vertexes
	}

	if (onlyMuons) { return; }

	for (Int_t i=0; i<TRA_El_N; i++) {
		nParticles++;
		etaCon      += asString(TRA_El_eta->at(i)) + " ";
		phiCon      += asString(TRA_El_phi->at(i)) + " ";
		ptCon       += asString(TRA_El_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)TRA_El_pdgId->at(i))  + " ";
		labelCon    += "0 ";
		dataTypeCon += " 0.000001 ";
	}
	for (Int_t i=0; i<TRA_Tau_N; i++) {
		nParticles++;
		etaCon      += asString(TRA_Tau_eta->at(i)) + " ";
		phiCon      += asString(TRA_Tau_phi->at(i)) + " ";
		ptCon       += asString(TRA_Tau_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)TRA_Tau_pdgId->at(i))  + " ";
		labelCon    += "0 ";
		dataTypeCon += "0.000001 ";
	}
	for (Int_t i=0; i<TRA_HadW_N; i++) {
		nParticles++;
		etaCon      += asString(TRA_HadW_eta->at(i)) + " ";
		phiCon      += asString(TRA_HadW_phi->at(i)) + " ";
		ptCon       += asString(TRA_HadW_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)TRA_HadW_pdgId->at(i))  + " ";
		labelCon    += "0 ";
		dataTypeCon += "0.000001 ";
	}
	for (Int_t i=0; i<TRA_LepW_N; i++) {
		nParticles++;
		etaCon      += asString(TRA_LepW_eta->at(i)) + " ";
		phiCon      += asString(TRA_LepW_phi->at(i)) + " ";
		ptCon       += asString(TRA_LepW_p_T->at(i)/1000.)  + " ";
		pdgIdCon    += asString((Int_t)TRA_LepW_pdgId->at(i))  + " ";
		labelCon    += "0 ";
		dataTypeCon += "0.000001 ";
	}
	for (Int_t i=0; i<TRA_HadTop_N; i++) {
		nParticles++;
		etaCon      += asString(TRA_HadTop_eta->at(i)) + " ";
		phiCon      += asString(TRA_HadTop_phi->at(i)) + " ";
		ptCon       += asString(TRA_HadTop_p_T->at(i)/1000.)  + " ";
		sign 	     = (Int_t) (fabs(TRA_HadTop_pdgId->at(i))/TRA_HadTop_pdgId->at(i));
		pdgIdCon    += asString((Int_t)(TRA_HadTop_pdgId->at(i)+(600*sign))) + " ";
		labelCon    += "0 ";
		dataTypeCon += "0.000001 ";
	}
	for (Int_t i=0; i<TRA_LepTop_N; i++) {
		nParticles++;
		etaCon      += asString(TRA_LepTop_eta->at(i)) + " ";
		phiCon      += asString(TRA_LepTop_phi->at(i)) + " ";
		ptCon       += asString(TRA_LepTop_p_T->at(i)/1000.)  + " ";
		sign 	     = (Int_t) (fabs(TRA_LepTop_pdgId->at(i))/TRA_LepTop_pdgId->at(i));
		pdgIdCon    += asString((Int_t)(TRA_LepTop_pdgId->at(i)+(600*sign))) + " ";
		labelCon    += "0 ";
		dataTypeCon += "0.000001 ";
	}
	for (Int_t i=0; i<TRA_Bot_N; i++) {
		nParticles++;
		etaCon      += asString(TRA_Bot_eta->at(i)) + " ";
		phiCon      += asString(TRA_Bot_phi->at(i)) + " ";
		ptCon       += asString(TRA_Bot_p_T->at(i)/1000.)  + " ";
		sign 	     = (Int_t) (fabs(TRA_Bot_pdgId->at(i))/TRA_Bot_pdgId->at(i));
		pdgIdCon    += asString((Int_t)(TRA_Bot_pdgId->at(i)+(600*sign))) + " ";
		labelCon    += "0 ";
		dataTypeCon += "0.000001 ";
	}
	for (Int_t i=0; i<TRA_LQ_N; i++) {	//LQ - Light Quarks
		nParticles++;
		etaCon      += asString(TRA_LQ_eta->at(i)) + " ";
		phiCon      += asString(TRA_LQ_phi->at(i)) + " ";
		ptCon       += asString(TRA_LQ_p_T->at(i)/1000.)  + " ";
		sign 	     = (Int_t) (fabs(TRA_LQ_pdgId->at(i))/TRA_LQ_pdgId->at(i));
		pdgIdCon    += asString((Int_t)(TRA_LQ_pdgId->at(i)+(600*sign))) + " ";
		labelCon    += "0 ";
		dataTypeCon += "0.000001 ";
	}
}

void ntupleXML::GetTruthAll(Bool_t onlyMuons) {
	//Loop over entire truth block (if present)
	Int_t id, sign;
	for (UInt_t i=0; i<TA_Tru_p_T->size(); i++) {
		if (!onlyMuons || (onlyMuons && fabs(TA_Tru_pdgId->at(i)) == 13)) {
			nParticles++;
			etaCon      += asString(TA_Tru_eta->at(i)) + " ";
			phiCon      += asString(TA_Tru_phi->at(i)) + " ";
			ptCon       += asString(TA_Tru_p_T->at(i)/1000.)  + " ";
			id 	     = (Int_t) TA_Tru_pdgId->at(i);
			sign 	     = (Int_t) (fabs(TA_Tru_pdgId->at(i))/TA_Tru_pdgId->at(i));
			//Atlantis' True Particle Simulation ignores quarks, however with TopView
			//we are interested in t and b quarks, 6 special pdgIds have therefore
			//been set up in Atlantis (601-606) so as to display quarks from ntuples
			if (abs(id) <= 6) { id += (600 * sign); }	
			pdgIdCon    += asString(id)  + " ";
			labelCon    += asString(TA_Tru_barcode->at(i))  + " ";
			dataTypeCon += "0.000001 ";	//If zero, Atlantis crashes on eta-phi plot due to rhoVertex
		}
	}
}

//Branch Association//

void ntupleXML::SetBranches(void) {
	FR_El_pdgId = 0;
	FR_El_p_T = 0;
	FR_El_phi = 0;
	FR_El_eta = 0;
	FR_Mu_pdgId = 0;
	FR_Mu_p_T = 0;
	FR_Mu_phi = 0;
	FR_Mu_eta = 0;
	FR_Tau_pdgId = 0;
	FR_Tau_p_T = 0;
	FR_Tau_phi = 0;
	FR_Tau_eta = 0;
	FR_Ph_pdgId = 0;
	FR_Ph_p_T = 0;
	FR_Ph_phi = 0;
	FR_Ph_eta = 0;
	FR_PJet_pdgId = 0;
	FR_PJet_p_T = 0;
	FR_PJet_phi = 0;
	FR_PJet_eta = 0;
	FR_PJet_BTagged = 0;
   	FRA_El_pdgId = 0;
  	FRA_El_p_T = 0;
  	FRA_El_phi = 0;
   	FRA_El_eta = 0;
   	FRA_Mu_pdgId = 0;
  	FRA_Mu_p_T = 0;
  	FRA_Mu_phi = 0;
   	FRA_Mu_eta = 0;
   	FRA_Nu_pdgId = 0;
  	FRA_Nu_p_T = 0;
  	FRA_Nu_phi = 0;
   	FRA_Nu_eta = 0;
   	FRA_HadW_pdgId = 0;
  	FRA_HadW_p_T = 0;
  	FRA_HadW_phi = 0;
   	FRA_HadW_eta = 0;
   	FRA_LepW_pdgId = 0;
  	FRA_LepW_p_T = 0;
  	FRA_LepW_phi = 0;
   	FRA_LepW_eta = 0;
   	FRA_HadTop_pdgId = 0;
  	FRA_HadTop_p_T = 0;
  	FRA_HadTop_phi = 0;
   	FRA_HadTop_eta = 0;
   	FRA_LepTop_pdgId = 0;
  	FRA_LepTop_p_T = 0;
  	FRA_LepTop_phi = 0;
   	FRA_LepTop_eta = 0;
   	FRA_Lepb_pdgId = 0;
  	FRA_Lepb_p_T = 0;
  	FRA_Lepb_phi = 0;
   	FRA_Lepb_eta = 0;
   	FRA_Hadb_pdgId = 0;
  	FRA_Hadb_p_T = 0;
  	FRA_Hadb_phi = 0;
   	FRA_Hadb_eta = 0;
   	FRA_Hadj_pdgId = 0;
  	FRA_Hadj_p_T = 0;
  	FRA_Hadj_phi = 0;
   	FRA_Hadj_eta = 0;
   	TRA_El_pdgId = 0;
  	TRA_El_p_T = 0;
  	TRA_El_phi = 0;
   	TRA_El_eta = 0;
   	TRA_Mu_pdgId = 0;
  	TRA_Mu_p_T = 0;
  	TRA_Mu_phi = 0;
   	TRA_Mu_eta = 0;
   	TRA_Tau_pdgId = 0;
  	TRA_Tau_p_T = 0;
  	TRA_Tau_phi = 0;
   	TRA_Tau_eta = 0;
   	TRA_HadW_pdgId = 0;
  	TRA_HadW_p_T = 0;
  	TRA_HadW_phi = 0;
   	TRA_HadW_eta = 0;
   	TRA_LepW_pdgId = 0;
  	TRA_LepW_p_T = 0;
  	TRA_LepW_phi = 0;
   	TRA_LepW_eta = 0;
   	TRA_HadTop_pdgId = 0;
  	TRA_HadTop_p_T = 0;
  	TRA_HadTop_phi = 0;
   	TRA_HadTop_eta = 0;
   	TRA_LepTop_pdgId = 0;
  	TRA_LepTop_p_T = 0;
  	TRA_LepTop_phi = 0;
   	TRA_LepTop_eta = 0;
   	TRA_Bot_pdgId = 0;
  	TRA_Bot_p_T = 0;
  	TRA_Bot_phi = 0;
   	TRA_Bot_eta = 0;
   	TRA_LQ_pdgId = 0;
  	TRA_LQ_p_T = 0;
  	TRA_LQ_phi = 0;
   	TRA_LQ_eta = 0;
	TA_Tru_p_T = 0;
	TA_Tru_phi = 0;
	TA_Tru_eta = 0;
	TA_Tru_pdgId = 0;
	TA_Tru_barcode = 0;
	FR_Active=0;
	TR_Active=0;
	FRA_Active=0;
	TRA_Active=0;
	TA_Active=0;

	activeTree = activeFile->GetTree();

	if (activeTree) { 
		FR_Active = 1;
   		activeTree->SetBranchAddress("eventNumber", &FR_eventNumber, &b_FR_eventNumber);
   		activeTree->SetBranchAddress("runNumber", &FR_runNumber, &b_FR_runNumber);
		activeTree->SetBranchAddress("MET_Final_ex", &FR_MET_Final_ex, &b_FR_MET_Final_ex);
   		activeTree->SetBranchAddress("MET_Final_ey", &FR_MET_Final_ey, &b_FR_MET_Final_ey);
   		activeTree->SetBranchAddress("MET_Final_et", &FR_MET_Final_et, &b_FR_MET_Final_et);
		if (!isTopView) {
			//With EventView ntuples, MET Truth is in the CollectionTree
			activeTree->SetBranchAddress("MET_Truth_ex", &TR_MET_Truth_ex, &b_TR_MET_Truth_ex);
   			activeTree->SetBranchAddress("MET_Truth_ey", &TR_MET_Truth_ey, &b_TR_MET_Truth_ey);
   			activeTree->SetBranchAddress("MET_Truth_et", &TR_MET_Truth_et, &b_TR_MET_Truth_et);
			TR_Active=1;
		}	
  		activeTree->SetBranchAddress("El_N", &FR_El_N, &b_FR_El_N);
   		activeTree->SetBranchAddress("El_p_T", &FR_El_p_T, &b_FR_El_p_T);
   		activeTree->SetBranchAddress("El_pdgId", &FR_El_pdgId, &b_FR_El_pdgId);
   		activeTree->SetBranchAddress("El_phi", &FR_El_phi, &b_FR_El_phi);
   		activeTree->SetBranchAddress("El_eta", &FR_El_eta, &b_FR_El_eta);	
  		activeTree->SetBranchAddress("Mu_N", &FR_Mu_N, &b_FR_Mu_N);
   		activeTree->SetBranchAddress("Mu_p_T", &FR_Mu_p_T, &b_FR_Mu_p_T);
   		activeTree->SetBranchAddress("Mu_pdgId", &FR_Mu_pdgId, &b_FR_Mu_pdgId);
   		activeTree->SetBranchAddress("Mu_phi", &FR_Mu_phi, &b_FR_Mu_phi);
   		activeTree->SetBranchAddress("Mu_eta", &FR_Mu_eta, &b_FR_Mu_eta);
  		activeTree->SetBranchAddress("Tau_N", &FR_Tau_N, &b_FR_Tau_N);
   		activeTree->SetBranchAddress("Tau_p_T", &FR_Tau_p_T, &b_FR_Tau_p_T);
   		activeTree->SetBranchAddress("Tau_pdgId", &FR_Tau_pdgId, &b_FR_Tau_pdgId);
   		activeTree->SetBranchAddress("Tau_phi", &FR_Tau_phi, &b_FR_Tau_phi);
   		activeTree->SetBranchAddress("Tau_eta", &FR_Tau_eta, &b_FR_Tau_eta);
  		activeTree->SetBranchAddress("Ph_N", &FR_Ph_N, &b_FR_Ph_N);
   		activeTree->SetBranchAddress("Ph_p_T", &FR_Ph_p_T, &b_FR_Ph_p_T);
   		activeTree->SetBranchAddress("Ph_pdgId", &FR_Ph_pdgId, &b_FR_Ph_pdgId);
   		activeTree->SetBranchAddress("Ph_phi", &FR_Ph_phi, &b_FR_Ph_phi);
   		activeTree->SetBranchAddress("Ph_eta", &FR_Ph_eta, &b_FR_Ph_eta);
   		activeTree->SetBranchAddress("PJet_N", &FR_PJet_N, &b_FR_PJet_N);
   		activeTree->SetBranchAddress("PJet_p_T", &FR_PJet_p_T, &b_FR_PJet_p_T);
   		activeTree->SetBranchAddress("PJet_pdgId", &FR_PJet_pdgId, &b_FR_PJet_pdgId);
   		activeTree->SetBranchAddress("PJet_phi", &FR_PJet_phi, &b_FR_PJet_phi);
   		activeTree->SetBranchAddress("PJet_eta", &FR_PJet_eta, &b_FR_PJet_eta);
		if (isTopView) { activeTree->SetBranchAddress("PJet_BTagged", &FR_PJet_BTagged, &b_FR_PJet_BTagged); }
	} else if (Warn>1) { std::cout << "\nntupleXML: Warning cannot find " << mainTreeName << " tree. "; }

	//Stop here with EventView
	if (!isTopView) { return; }

	friendTree = (TFriendElement*) (activeFile->GetListOfFriends()->FindObject("FullRecoAna0"));
	activeTree = friendTree->GetTree();
   	if (activeTree) {
		FRA_Active=1;
  		activeTree->SetBranchAddress("El_N", &FRA_El_N, &b_FRA_El_N);
   		activeTree->SetBranchAddress("El_p_T", &FRA_El_p_T, &b_FRA_El_p_T);
   		activeTree->SetBranchAddress("El_pdgId", &FRA_El_pdgId, &b_FRA_El_pdgId);
   		activeTree->SetBranchAddress("El_phi", &FRA_El_phi, &b_FRA_El_phi);
   		activeTree->SetBranchAddress("El_eta", &FRA_El_eta, &b_FRA_El_eta);	
  		activeTree->SetBranchAddress("Mu_N", &FRA_Mu_N, &b_FRA_Mu_N);
   		activeTree->SetBranchAddress("Mu_p_T", &FRA_Mu_p_T, &b_FRA_Mu_p_T);
   		activeTree->SetBranchAddress("Mu_pdgId", &FRA_Mu_pdgId, &b_FRA_Mu_pdgId);
   		activeTree->SetBranchAddress("Mu_phi", &FRA_Mu_phi, &b_FRA_Mu_phi);
   		activeTree->SetBranchAddress("Mu_eta", &FRA_Mu_eta, &b_FRA_Mu_eta);
  		activeTree->SetBranchAddress("Nu_N", &FRA_Nu_N, &b_FRA_Nu_N);
   		activeTree->SetBranchAddress("Nu_p_T", &FRA_Nu_p_T, &b_FRA_Nu_p_T);
   		activeTree->SetBranchAddress("Nu_pdgId", &FRA_Nu_pdgId, &b_FRA_Nu_pdgId);
   		activeTree->SetBranchAddress("Nu_phi", &FRA_Nu_phi, &b_FRA_Nu_phi);
   		activeTree->SetBranchAddress("Nu_eta", &FRA_Nu_eta, &b_FRA_Nu_eta);
   		activeTree->SetBranchAddress("HadW_N", &FRA_HadW_N, &b_FRA_HadW_N);
   		activeTree->SetBranchAddress("HadW_p_T", &FRA_HadW_p_T, &b_FRA_HadW_p_T);
   		activeTree->SetBranchAddress("HadW_pdgId", &FRA_HadW_pdgId, &b_FRA_HadW_pdgId);
   		activeTree->SetBranchAddress("HadW_phi", &FRA_HadW_phi, &b_FRA_HadW_phi);
   		activeTree->SetBranchAddress("HadW_eta", &FRA_HadW_eta, &b_FRA_HadW_eta);
   		activeTree->SetBranchAddress("LepW_N", &FRA_LepW_N, &b_FRA_LepW_N);
   		activeTree->SetBranchAddress("LepW_p_T", &FRA_LepW_p_T, &b_FRA_LepW_p_T);
   		activeTree->SetBranchAddress("LepW_pdgId", &FRA_LepW_pdgId, &b_FRA_LepW_pdgId);
   		activeTree->SetBranchAddress("LepW_phi", &FRA_LepW_phi, &b_FRA_LepW_phi);
   		activeTree->SetBranchAddress("LepW_eta", &FRA_LepW_eta, &b_FRA_LepW_eta);
   		activeTree->SetBranchAddress("HadTop_N", &FRA_HadTop_N, &b_FRA_HadTop_N);
   		activeTree->SetBranchAddress("HadTop_p_T", &FRA_HadTop_p_T, &b_FRA_HadTop_p_T);
   		activeTree->SetBranchAddress("HadTop_pdgId", &FRA_HadTop_pdgId, &b_FRA_HadTop_pdgId);
   		activeTree->SetBranchAddress("HadTop_phi", &FRA_HadTop_phi, &b_FRA_HadTop_phi);
   		activeTree->SetBranchAddress("HadTop_eta", &FRA_HadTop_eta, &b_FRA_HadTop_eta);
   		activeTree->SetBranchAddress("LepTop_N", &FRA_LepTop_N, &b_FRA_LepTop_N);
   		activeTree->SetBranchAddress("LepTop_p_T", &FRA_LepTop_p_T, &b_FRA_LepTop_p_T);
   		activeTree->SetBranchAddress("LepTop_pdgId", &FRA_LepTop_pdgId, &b_FRA_LepTop_pdgId);
   		activeTree->SetBranchAddress("LepTop_phi", &FRA_LepTop_phi, &b_FRA_LepTop_phi);
   		activeTree->SetBranchAddress("LepTop_eta", &FRA_LepTop_eta, &b_FRA_LepTop_eta);
   		activeTree->SetBranchAddress("Lepb_N", &FRA_Lepb_N, &b_FRA_Lepb_N);
   		activeTree->SetBranchAddress("Lepb_p_T", &FRA_Lepb_p_T, &b_FRA_Lepb_p_T);
   		activeTree->SetBranchAddress("Lepb_pdgId", &FRA_Lepb_pdgId, &b_FRA_Lepb_pdgId);
   		activeTree->SetBranchAddress("Lepb_phi", &FRA_Lepb_phi, &b_FRA_Lepb_phi);
   		activeTree->SetBranchAddress("Lepb_eta", &FRA_Lepb_eta, &b_FRA_Lepb_eta);
   		activeTree->SetBranchAddress("Hadb_N", &FRA_Hadb_N, &b_FRA_Hadb_N);
   		activeTree->SetBranchAddress("Hadb_p_T", &FRA_Hadb_p_T, &b_FRA_Hadb_p_T);
   		activeTree->SetBranchAddress("Hadb_pdgId", &FRA_Hadb_pdgId, &b_FRA_Hadb_pdgId);
   		activeTree->SetBranchAddress("Hadb_phi", &FRA_Hadb_phi, &b_FRA_Hadb_phi);
   		activeTree->SetBranchAddress("Hadb_eta", &FRA_Hadb_eta, &b_FRA_Hadb_eta);
   		activeTree->SetBranchAddress("Hadj_N", &FRA_Hadj_N, &b_FRA_Hadj_N);
   		activeTree->SetBranchAddress("Hadj_p_T", &FRA_Hadj_p_T, &b_FRA_Hadj_p_T);
   		activeTree->SetBranchAddress("Hadj_pdgId", &FRA_Hadj_pdgId, &b_FRA_Hadj_pdgId);
   		activeTree->SetBranchAddress("Hadj_phi", &FRA_Hadj_phi, &b_FRA_Hadj_phi);
   		activeTree->SetBranchAddress("Hadj_eta", &FRA_Hadj_eta, &b_FRA_Hadj_eta);
   	} else if (Warn>1) { std::cout << "\nntupleXML: Warning, cannot find FullRecoAna tree. "; }

	friendTree = (TFriendElement*) (activeFile->GetListOfFriends()->FindObject("TruthAna0"));
	activeTree = friendTree->GetTree();
   	if (activeTree) {
		TRA_Active=1;
  		activeTree->SetBranchAddress("El_N", &TRA_El_N, &b_TRA_El_N);
   		activeTree->SetBranchAddress("El_p_T", &TRA_El_p_T, &b_TRA_El_p_T);
   		activeTree->SetBranchAddress("El_pdgId", &TRA_El_pdgId, &b_TRA_El_pdgId);
   		activeTree->SetBranchAddress("El_phi", &TRA_El_phi, &b_TRA_El_phi);
   		activeTree->SetBranchAddress("El_eta", &TRA_El_eta, &b_TRA_El_eta);	
  		activeTree->SetBranchAddress("Mu_N", &TRA_Mu_N, &b_TRA_Mu_N);
   		activeTree->SetBranchAddress("Mu_p_T", &TRA_Mu_p_T, &b_TRA_Mu_p_T);
   		activeTree->SetBranchAddress("Mu_pdgId", &TRA_Mu_pdgId, &b_TRA_Mu_pdgId);
   		activeTree->SetBranchAddress("Mu_phi", &TRA_Mu_phi, &b_TRA_Mu_phi);
   		activeTree->SetBranchAddress("Mu_eta", &TRA_Mu_eta, &b_TRA_Mu_eta);
  		activeTree->SetBranchAddress("Tau_N", &TRA_Tau_N, &b_TRA_Tau_N);
   		activeTree->SetBranchAddress("Tau_p_T", &TRA_Tau_p_T, &b_TRA_Tau_p_T);
   		activeTree->SetBranchAddress("Tau_pdgId", &TRA_Tau_pdgId, &b_TRA_Tau_pdgId);
   		activeTree->SetBranchAddress("Tau_phi", &TRA_Tau_phi, &b_TRA_Tau_phi);
   		activeTree->SetBranchAddress("Tau_eta", &TRA_Tau_eta, &b_TRA_Tau_eta);
   		activeTree->SetBranchAddress("HadW_N", &TRA_HadW_N, &b_TRA_HadW_N);
   		activeTree->SetBranchAddress("HadW_p_T", &TRA_HadW_p_T, &b_TRA_HadW_p_T);
   		activeTree->SetBranchAddress("HadW_pdgId", &TRA_HadW_pdgId, &b_TRA_HadW_pdgId);
   		activeTree->SetBranchAddress("HadW_phi", &TRA_HadW_phi, &b_TRA_HadW_phi);
   		activeTree->SetBranchAddress("HadW_eta", &TRA_HadW_eta, &b_TRA_HadW_eta);
   		activeTree->SetBranchAddress("LepW_N", &TRA_LepW_N, &b_TRA_LepW_N);
   		activeTree->SetBranchAddress("LepW_p_T", &TRA_LepW_p_T, &b_TRA_LepW_p_T);
   		activeTree->SetBranchAddress("LepW_pdgId", &TRA_LepW_pdgId, &b_TRA_LepW_pdgId);
   		activeTree->SetBranchAddress("LepW_phi", &TRA_LepW_phi, &b_TRA_LepW_phi);
   		activeTree->SetBranchAddress("LepW_eta", &TRA_LepW_eta, &b_TRA_LepW_eta);
   		activeTree->SetBranchAddress("HadTop_N", &TRA_HadTop_N, &b_TRA_HadTop_N);
   		activeTree->SetBranchAddress("HadTop_p_T", &TRA_HadTop_p_T, &b_TRA_HadTop_p_T);
   		activeTree->SetBranchAddress("HadTop_pdgId", &TRA_HadTop_pdgId, &b_TRA_HadTop_pdgId);
   		activeTree->SetBranchAddress("HadTop_phi", &TRA_HadTop_phi, &b_TRA_HadTop_phi);
   		activeTree->SetBranchAddress("HadTop_eta", &TRA_HadTop_eta, &b_TRA_HadTop_eta);
   		activeTree->SetBranchAddress("LepTop_N", &TRA_LepTop_N, &b_TRA_LepTop_N);
   		activeTree->SetBranchAddress("LepTop_p_T", &TRA_LepTop_p_T, &b_TRA_LepTop_p_T);
   		activeTree->SetBranchAddress("LepTop_pdgId", &TRA_LepTop_pdgId, &b_TRA_LepTop_pdgId);
   		activeTree->SetBranchAddress("LepTop_phi", &TRA_LepTop_phi, &b_TRA_LepTop_phi);
   		activeTree->SetBranchAddress("LepTop_eta", &TRA_LepTop_eta, &b_TRA_LepTop_eta);
   		activeTree->SetBranchAddress("Bot_N", &TRA_Bot_N, &b_TRA_Bot_N);
   		activeTree->SetBranchAddress("Bot_p_T", &TRA_Bot_p_T, &b_TRA_Bot_p_T);
   		activeTree->SetBranchAddress("Bot_pdgId", &TRA_Bot_pdgId, &b_TRA_Bot_pdgId);
   		activeTree->SetBranchAddress("Bot_phi", &TRA_Bot_phi, &b_TRA_Bot_phi);
   		activeTree->SetBranchAddress("Bot_eta", &TRA_Bot_eta, &b_TRA_Bot_eta);
   		activeTree->SetBranchAddress("LQ_N", &TRA_LQ_N, &b_TRA_LQ_N);
   		activeTree->SetBranchAddress("LQ_p_T", &TRA_LQ_p_T, &b_TRA_LQ_p_T);
   		activeTree->SetBranchAddress("LQ_pdgId", &TRA_LQ_pdgId, &b_TRA_LQ_pdgId);
   		activeTree->SetBranchAddress("LQ_phi", &TRA_LQ_phi, &b_TRA_LQ_phi);
   		activeTree->SetBranchAddress("LQ_eta", &TRA_LQ_eta, &b_TRA_LQ_eta);
   	} else if (Warn>1) { std::cout << "\nntupleXML: Warning, cannot find TruthAna tree. "; }

	friendTree = (TFriendElement*) (activeFile->GetListOfFriends()->FindObject("TruthAll0"));
	activeTree = friendTree->GetTree();
   	if (activeTree) {
		TA_Active=1;
  		activeTree->SetBranchAddress("Tru_p_T", &TA_Tru_p_T, &b_TA_Tru_p_T);
   		activeTree->SetBranchAddress("Tru_phi", &TA_Tru_phi, &b_TA_Tru_phi);
   		activeTree->SetBranchAddress("Tru_eta", &TA_Tru_eta, &b_TA_Tru_eta);
   		activeTree->SetBranchAddress("Tru_pdgId", &TA_Tru_pdgId, &b_TA_Tru_pdgId);
   		activeTree->SetBranchAddress("Tru_barcode", &TA_Tru_barcode, &b_TA_Tru_barcode);	
   	} else if (Warn>1) { std::cout << "\nntupleXML: Warning, cannot find TruthAll tree. "; }

	//If using TopView ntuples, MET_Truth must be fetched from Truth0
	friendTree = (TFriendElement*) (activeFile->GetListOfFriends()->FindObject("Truth0"));
	activeTree = friendTree->GetTree();
   	if (activeTree) {
		TR_Active=1;
  		activeTree->SetBranchAddress("MET_Truth_ex", &TR_MET_Truth_ex, &b_TR_MET_Truth_ex);
   		activeTree->SetBranchAddress("MET_Truth_ey", &TR_MET_Truth_ey, &b_TR_MET_Truth_ey);
   		activeTree->SetBranchAddress("MET_Truth_et", &TR_MET_Truth_et, &b_TR_MET_Truth_et);
   	} else if (Warn>1) { std::cout << "\nntupleXML: Warning, cannot find Truth tree. "; }
}

void ntupleXML::FillBranches(Int_t entryNumber) {
	if (FR_Active) {
   		b_FR_eventNumber->GetEntry(entryNumber);
   		b_FR_runNumber->GetEntry(entryNumber);
   		b_FR_MET_Final_ex->GetEntry(entryNumber);
   		b_FR_MET_Final_ey->GetEntry(entryNumber);
   		b_FR_MET_Final_et->GetEntry(entryNumber);
   		if (TR_Active) {
			b_TR_MET_Truth_ex->GetEntry(entryNumber);
   			b_TR_MET_Truth_ey->GetEntry(entryNumber);
   			b_TR_MET_Truth_et->GetEntry(entryNumber);
		}
   		b_FR_El_N->GetEntry(entryNumber);
   		b_FR_El_pdgId->GetEntry(entryNumber);
	   	b_FR_El_p_T->GetEntry(entryNumber);
   		b_FR_El_phi->GetEntry(entryNumber);
   		b_FR_El_eta->GetEntry(entryNumber);
   		b_FR_Mu_N->GetEntry(entryNumber);
   		b_FR_Mu_pdgId->GetEntry(entryNumber);
   		b_FR_Mu_p_T->GetEntry(entryNumber);
   		b_FR_Mu_phi->GetEntry(entryNumber);
   		b_FR_Mu_eta->GetEntry(entryNumber);
		b_FR_Tau_N->GetEntry(entryNumber);
   		b_FR_Tau_pdgId->GetEntry(entryNumber);
   		b_FR_Tau_p_T->GetEntry(entryNumber);
   		b_FR_Tau_phi->GetEntry(entryNumber);
   		b_FR_Tau_eta->GetEntry(entryNumber);
   		b_FR_Ph_N->GetEntry(entryNumber);
   		b_FR_Ph_pdgId->GetEntry(entryNumber);
   		b_FR_Ph_p_T->GetEntry(entryNumber);
   		b_FR_Ph_phi->GetEntry(entryNumber);
   		b_FR_Ph_eta->GetEntry(entryNumber);
   		b_FR_PJet_N->GetEntry(entryNumber);
   		b_FR_PJet_pdgId->GetEntry(entryNumber);
  	 	b_FR_PJet_p_T->GetEntry(entryNumber);
  	 	b_FR_PJet_phi->GetEntry(entryNumber);
 	  	b_FR_PJet_eta->GetEntry(entryNumber);
	}

	if (!isTopView) { return; }

   	if (FR_Active) { b_FR_PJet_BTagged->GetEntry(entryNumber); }

	if (FRA_Active) {
		b_FRA_El_N->GetEntry(entryNumber);
	   	b_FRA_El_pdgId->GetEntry(entryNumber);	
  		b_FRA_El_p_T->GetEntry(entryNumber);
    		b_FRA_El_phi->GetEntry(entryNumber);
   		b_FRA_El_eta->GetEntry(entryNumber);
   		b_FRA_Mu_N->GetEntry(entryNumber);
   		b_FRA_Mu_pdgId->GetEntry(entryNumber);
  		b_FRA_Mu_p_T->GetEntry(entryNumber);
  		b_FRA_Mu_phi->GetEntry(entryNumber);
 		b_FRA_Mu_eta->GetEntry(entryNumber);
   		b_FRA_Nu_N->GetEntry(entryNumber);
   		b_FRA_Nu_pdgId->GetEntry(entryNumber);
  		b_FRA_Nu_p_T->GetEntry(entryNumber);
  		b_FRA_Nu_phi->GetEntry(entryNumber);
 		b_FRA_Nu_eta->GetEntry(entryNumber);
   		b_FRA_HadW_N->GetEntry(entryNumber);
   		b_FRA_HadW_pdgId->GetEntry(entryNumber);
  		b_FRA_HadW_p_T->GetEntry(entryNumber);
  		b_FRA_HadW_phi->GetEntry(entryNumber);
   		b_FRA_HadW_eta->GetEntry(entryNumber);
   		b_FRA_LepW_N->GetEntry(entryNumber);
   		b_FRA_LepW_pdgId->GetEntry(entryNumber);
  		b_FRA_LepW_p_T->GetEntry(entryNumber);
  		b_FRA_LepW_phi->GetEntry(entryNumber);
   		b_FRA_LepW_eta->GetEntry(entryNumber);
   		b_FRA_HadTop_N->GetEntry(entryNumber);
   		b_FRA_HadTop_pdgId->GetEntry(entryNumber);
  		b_FRA_HadTop_p_T->GetEntry(entryNumber);
  		b_FRA_HadTop_phi->GetEntry(entryNumber);
   		b_FRA_HadTop_eta->GetEntry(entryNumber);
   		b_FRA_LepTop_N->GetEntry(entryNumber);
   		b_FRA_LepTop_pdgId->GetEntry(entryNumber);
  		b_FRA_LepTop_p_T->GetEntry(entryNumber);
  		b_FRA_LepTop_phi->GetEntry(entryNumber);
   		b_FRA_LepTop_eta->GetEntry(entryNumber);
   		b_FRA_Lepb_N->GetEntry(entryNumber);
   		b_FRA_Lepb_pdgId->GetEntry(entryNumber);
  		b_FRA_Lepb_p_T->GetEntry(entryNumber);
  		b_FRA_Lepb_phi->GetEntry(entryNumber);
   		b_FRA_Lepb_eta->GetEntry(entryNumber);
   		b_FRA_Hadb_N->GetEntry(entryNumber);
   		b_FRA_Hadb_pdgId->GetEntry(entryNumber);
  		b_FRA_Hadb_p_T->GetEntry(entryNumber);
  		b_FRA_Hadb_phi->GetEntry(entryNumber);
   		b_FRA_Hadb_eta->GetEntry(entryNumber);
   		b_FRA_Hadj_N->GetEntry(entryNumber);
   		b_FRA_Hadj_pdgId->GetEntry(entryNumber);
  		b_FRA_Hadj_p_T->GetEntry(entryNumber);
  		b_FRA_Hadj_phi->GetEntry(entryNumber);
   		b_FRA_Hadj_eta->GetEntry(entryNumber);
	}
	if (TRA_Active) {
		b_TRA_El_N->GetEntry(entryNumber);
   		b_TRA_El_pdgId->GetEntry(entryNumber);
  		b_TRA_El_p_T->GetEntry(entryNumber);
  		b_TRA_El_phi->GetEntry(entryNumber);
   		b_TRA_El_eta->GetEntry(entryNumber);
   		b_TRA_Mu_N->GetEntry(entryNumber);
   		b_TRA_Mu_pdgId->GetEntry(entryNumber);
  		b_TRA_Mu_p_T->GetEntry(entryNumber);
  		b_TRA_Mu_phi->GetEntry(entryNumber);
   		b_TRA_Mu_eta->GetEntry(entryNumber);
   		b_TRA_Tau_N->GetEntry(entryNumber);
   		b_TRA_Tau_pdgId->GetEntry(entryNumber);
  		b_TRA_Tau_p_T->GetEntry(entryNumber);
  		b_TRA_Tau_phi->GetEntry(entryNumber);
   		b_TRA_Tau_eta->GetEntry(entryNumber);
   		b_TRA_HadW_N->GetEntry(entryNumber);
   		b_TRA_HadW_pdgId->GetEntry(entryNumber);
  		b_TRA_HadW_p_T->GetEntry(entryNumber);
  		b_TRA_HadW_phi->GetEntry(entryNumber);
   		b_TRA_HadW_eta->GetEntry(entryNumber);
   		b_TRA_LepW_N->GetEntry(entryNumber);
   		b_TRA_LepW_pdgId->GetEntry(entryNumber);
  		b_TRA_LepW_p_T->GetEntry(entryNumber);
  		b_TRA_LepW_phi->GetEntry(entryNumber);
   		b_TRA_LepW_eta->GetEntry(entryNumber);
   		b_TRA_HadTop_N->GetEntry(entryNumber);
   		b_TRA_HadTop_pdgId->GetEntry(entryNumber);
  		b_TRA_HadTop_p_T->GetEntry(entryNumber);
  		b_TRA_HadTop_phi->GetEntry(entryNumber);
   		b_TRA_HadTop_eta->GetEntry(entryNumber);
   		b_TRA_LepTop_N->GetEntry(entryNumber);
   		b_TRA_LepTop_pdgId->GetEntry(entryNumber);
  		b_TRA_LepTop_p_T->GetEntry(entryNumber);
  		b_TRA_LepTop_phi->GetEntry(entryNumber);
   		b_TRA_LepTop_eta->GetEntry(entryNumber);
   		b_TRA_Bot_N->GetEntry(entryNumber);
   		b_TRA_Bot_pdgId->GetEntry(entryNumber);
  		b_TRA_Bot_p_T->GetEntry(entryNumber);
  		b_TRA_Bot_phi->GetEntry(entryNumber);
   		b_TRA_Bot_eta->GetEntry(entryNumber);
   		b_TRA_LQ_N->GetEntry(entryNumber);
   		b_TRA_LQ_pdgId->GetEntry(entryNumber);
  		b_TRA_LQ_p_T->GetEntry(entryNumber);
  		b_TRA_LQ_phi->GetEntry(entryNumber);
   		b_TRA_LQ_eta->GetEntry(entryNumber);
	}
	if (TA_Active) {
		b_TA_Tru_p_T->GetEntry(entryNumber);
		b_TA_Tru_phi->GetEntry(entryNumber);
		b_TA_Tru_eta->GetEntry(entryNumber);
		b_TA_Tru_pdgId->GetEntry(entryNumber);
		b_TA_Tru_barcode->GetEntry(entryNumber);
	}
}

void ntupleXML::SetActiveFile(Int_t fileNumber) {
	if (activeFile != fileStore[fileNumber]) {
		activeFile = fileStore[fileNumber];
		if (Warn>1) { std::cout << "\nntupleXML: Setting New Active Tree. "; }
		//Associates branches with their variables
		ntupleXML::SetBranches();
	}
}

template<typename TYPE>
std::string ntupleXML::asString(const TYPE& x) {
	std::ostringstream o;
	o << std::setiosflags(ios::fixed);
	if (!(o << x)) {
		if (Warn>0) { std::cout << "\nntupleXML: Error in toString Conversion Function. "; }
		return "ERROR";
 	}
	return o.str();
}


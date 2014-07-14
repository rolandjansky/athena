/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TTree.h"
#include "TDataMember.h"

#include "HistoPile.h"

class DumpAll
{
  public:

	DumpAll( 
	const char *input,
    const char *tree_filter="CollectionTree", 
    const char *branch_filter=0, 
    const char *output=0,   
    const char *reference=0 );

	virtual ~DumpAll();
				 
    void DumpTrees();
	void ShowBranch(TBranch * branch);
	void DumpBranches(TTree* t, Long64_t entry); 
	void DumpObject(TClass* cl, void *obj);   
	void Inspect(TClass *cl, const char *mname, void *add);
	void FollowPointer(char*pointer, TString mname);   
	const char *AsString(void *buf, EDataType datatype);
	Float_t AsFloat(void *buf, EDataType datatype);

    //________________________________________________________________________
	void SetTreeFilter(char *tree_filter) {
	  fTreeFilter = tree_filter; 
	}
    //________________________________________________________________________
	void SetBranchFilter(char *branch_filter) {
	  fBranchFilter = branch_filter;
	}
    //________________________________________________________________________
	void SetVerbose(Int_t verbose=1) {
	  fVerbose = verbose; 
	}
    //________________________________________________________________________
	void SetMaxEntries(UInt_t maxentries) {
	  fMaxEntries = maxentries; 
	}
    //________________________________________________________________________
	void SetMaxArrayObj(UInt_t max) {
	  fMaxarrayObj = max; 
	}	
    //________________________________________________________________________
	void SetMaxArraySdt(UInt_t max) {
	  fMaxarraySdt = max; 
	}	
    //________________________________________________________________________
	void SetShowContainers(Bool_t show) {
	  fShowContainers = show;
	}
    //________________________________________________________________________
	//void SetTypeFormat(char *type_format) {
	//  fTypeFormat = type_format;
	//}	
    //________________________________________________________________________
	void SetBranchFilter(const char * branch_filter) {
	  fBranchFilter = branch_filter;
	}	
    //________________________________________________________________________
	void SetInspect(Bool_t inspect) {
	  fInspect = inspect;
	}	
    //________________________________________________________________________
	void SetFollowPointers(Bool_t follow_pointers) {
	  fFollowPointers = follow_pointers;
	}

  private:

//////////	
	// 0 =   print only errors/warnings
	// 1 = + print paths (names)
	// 2 = + print type information
	// 3 = + print current method
	// 4 = + give me everything you've got...
	Int_t  fVerbose;

	// also show nodes ie. elements not directly 
	// containing data (allows eg. to collect size information)
	Bool_t fShowContainers;

	Bool_t fDumpObject;
	Bool_t fInspect;
	Bool_t fFollowPointers;

	UInt_t fMaxEntries;			// max number of entries
	UInt_t fMaxarrayObj;    	// max collection elements to print (object)
	UInt_t fMaxarraySdt;  		// max collection elements to print (simple types)

	const char * fTreeFilter;
	const char * fBranchFilter;

    HistoPile* fHp;            	// hash list to pile up histograms

	const char * fTypeFormat;
///////////

	TFile* fInputFile;


};



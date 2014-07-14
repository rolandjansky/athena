/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  Dump complete ROOT file to ascii and/or histograms
//
//  Traverse object tree of every TTree entry in memory using reflection information    
//  in the TClass'es and print/plot values for every node
//
//  Author: Gerhard Brandt, 2009-09-26
//
//

#include <iostream>
 
#include "TVirtualCollectionProxy.h"
#include "TFile.h"
#include "TBranchElement.h"
#include "TRealData.h"
#include "TROOT.h"
#include "TVirtualStreamerInfo.h"
#include "TStreamerElement.h"

#include "TLeaf.h"
#include "TLeafElement.h"
#include "TLeafF.h"
#include "TLeafD.h"
#include "TLeafC.h"

#include "DumpAll.h"

using namespace std;

//____________________________________________________________________________
DumpAll::DumpAll( 
    const char *input,
	const char *tree_filter, 
	const char *branch_filter, 
	const char *output,   
	const char *reference ) :	
    fVerbose(2)
  , fShowContainers(1) 
  , fDumpObject(1) 
  , fInspect(1) 
  , fFollowPointers(1) 
  , fMaxEntries(0)
  , fMaxarrayObj(1) 
  , fMaxarraySdt(8) 
  , fTreeFilter(tree_filter) 
  , fBranchFilter(branch_filter)
//  fTypeFormat("%-30s ")    
{  
	if (!input) return;

    fHp = new HistoPile(output, reference);
 	fTypeFormat = "%-30s ";
 	// 2nd file is reference file to get histo definitions from  

  	fInputFile = TFile::Open(input);
	
	if (!fInputFile) return;
  
  	cout << "FILE " << fInputFile->GetName() << endl;
}
//____________________________________________________________________________
DumpAll::~DumpAll() 
{
 
  printf( "Read %lld bytes in %d transactions\n",
    fInputFile->GetBytesRead(), fInputFile->GetReadCalls() );

  fHp->Save();
  
  delete fInputFile;
  
  cout << endl;
}
//____________________________________________________________________________
void DumpAll::DumpTrees() 
{
  // Dump (tree) in input file
 
  // TODO: if treeFilter is zero loop on all trees  
  TTree* T = (TTree*)fInputFile->Get(fTreeFilter);
  
  if ( !T ) return;

  cout << Form("======= TREE %s =======",T->GetName()) << endl;
  if (fVerbose>3) T->Print();
  
  UInt_t nentries = T->GetEntries();
  if (fMaxEntries && fMaxEntries < nentries) nentries = fMaxEntries;  
  
  for (Long64_t i=0; i<nentries; ++i) {
    cout << "===== ENTRY " << i << endl;  	
	DumpBranches(T,i); 
  }
}
//______________________________________________________________________________
void DumpAll::DumpBranches(TTree* t, Long64_t entry) 
{      
  TObjArray*branches = t->GetListOfBranches();  
  Int_t nbranches = branches->GetEntries();    

  for (Int_t i=0; i<nbranches; ++i) {

    TBranch* b = (TBranch*)branches->At(i);

    const char* pname  = b->GetName();     
	const char *clname = b->GetClassName();

	if (strstr(pname,"Tau")) continue;  
	

	if ( fBranchFilter && !strstr(pname, fBranchFilter) ) {
	   continue;
	}  
	
  //if (fVerbose>2) cout << Form("\nBRANCH %d: %d %d %s %s", i, b->GetTotalSize(), b->GetTotBytes("*"), b->IsA()->GetName(), b->GetName()) << endl;
    if      (fVerbose==1) cout << Form("=== BRANCH %s ===", pname) << endl;
    else if (fVerbose>1) cout << Form("=== BRANCH (%s) %s ===", b->IsA()->GetName(), pname) << endl;      

    // strip type class name for improved human readability 
    // (special for ATLAS CollectioTree)
    fHp->Push(pname+strlen(clname)+1);	  
  //fHp->Push(pname);

    //Int_t nbytes = 
	b->GetEntry(entry);
	  
    if (b->IsA()==TBranchElement::Class()) {

      TBranchElement* be = (TBranchElement*)b;

      TClass* cl = TClass::GetClass(clname);
      void *obj  = be->GetObject();
	
	  if (fVerbose>2) cout << "DumpBranches " << endl;
        
	  DumpObject(cl,obj);
      
    } else if (b->IsA()==TBranch::Class()) {
	  ShowBranch(b);
	} else {
      cout << "<WARNING> Type "<< b->Class()->GetName() <<" of this TBranch not handled: " << b->GetName() << endl;
    }
	
    fHp->Pop();
  }  	      
}
//______________________________________________________________________________
void DumpAll::ShowBranch(TBranch * branch) 
{
  	// Case of a basic TBranch (not any inherited class)
	// This is similar to TTree::Show()
	//
    // Print values of all leaves for entry.

	if (fVerbose>2) cout << "ShowBranch" << endl;

    if (branch->GetListOfBranches()->GetEntriesFast() > 0) {
      cout << "<WARNING> subbranches of basic TBranch not yet handled!" << endl;
    }

   Int_t lenmax = fMaxarraySdt;

   TObjArray* leaves  = branch->GetListOfLeaves();
   Int_t nleaves = leaves->GetEntriesFast();
   Int_t ltype;
   
   for (Int_t i = 0; i < nleaves; i++) {
   
      TLeaf* leaf = (TLeaf*) leaves->UncheckedAt(i);

    //if (fVerbose  ) cout << fHp->Path();		
	  if (fVerbose>1) cout <<" ("<< leaf->IsA()->GetName() <<") ";

      Int_t len = leaf->GetLen();
      cout << leaf->GetName(); 
	  cout << "[" << leaf->GetLen() << "] = ";
      if (len <= 0) {
         continue;
      }
      len = TMath::Min(len, lenmax);
      if (leaf->IsA() == TLeafElement::Class()) {
         leaf->PrintValue(lenmax);
         continue; 
      }
	  ltype = 10;
	  if (leaf->IsA() == TLeafF::Class()) {
	   if (fVerbose) cout << "(float) ";
		 ltype = 5;
	  }
	  if (leaf->IsA() == TLeafD::Class()) {
	   if (fVerbose) cout << "(double) ";
		 ltype = 5;
	  }
      if (leaf->IsA() == TLeafC::Class()) {
	   //if (fVerbose) cout << "(char) ";
		 len = 1;
		 ltype = 5;
		 char *value = (char*)leaf->GetValuePointer();	     
		 printf("\"%s\"\n",value);
      } 
	  else	  
      for (Int_t l = 0; l < len; l++) {
         leaf->PrintValue(l);
		 if (l == (len - 1)) {
		   printf("\n");
		   continue;
		 }
		 printf(", ");
 		 if ((l % ltype) == 0) {
	 	    printf("\n				   ");
 		 }
      }
   }
	
}     
//______________________________________________________________________________
void DumpAll::DumpObject(TClass* cl, void *obj) 
{      
    // Loop over object members
	
	if (!fDumpObject) return;
	
	if (fVerbose>2) cout << "DumpObject " << endl;
	 
	if (fShowContainers) { 
      if (fVerbose>1) cout <<"OBJ "<< Form(fTypeFormat, cl->GetName());
      if (fVerbose  ) cout << fHp->Path() << endl;  	  
      //fHp->Fill(nbytes); 
    }	   

	// our favourite special case: le string
    if (!(strcmp(cl->GetName(),"string"))) {

      std::string* str = (std::string*)obj;         
      //cout << "std::string " << str->size() << endl;
      if (fVerbose>1) cout << "STR " << Form(fTypeFormat,"string");
	  if (fVerbose  ) cout << fHp->Path() <<" = \""<< (*str)<<"\""<< endl;

	  // fill strings as hash
	  TString rstr(str->c_str());
	  fHp->Fill(rstr.Hash());
	  
 	  return;
    } 

    TList* members = cl->GetListOfDataMembers();	
	for (Int_t j=0; j<members->GetEntries(); ++j) {	

	  TDataMember* rd     = (TDataMember*)(members->At(j));
	  Int_t offset        = rd->GetOffset();
      char *pointer       = (char*)obj + offset;		

      fHp->Push(rd->GetName()); 
      if (fVerbose>2) 
	    cout << "MEMBER "<<rd->GetName() 
		     << ": "<< Form(fTypeFormat,rd->GetTypeName()) 
			 << "\n";
      if (!fInspect) {
	    cout << fHp->Path()<<endl;        
	  }	 else {
		Inspect(cl,rd->GetName(),(void*)pointer); 
	  }	  	 
      fHp->Pop();
    }


/*
	cl->BuildRealData();
//	if (!cl->IsLoaded()) { cl->BuildEmulatedRealData(); }				 
    TList* members = cl->GetListOfRealData();	
	for (Int_t j=0; j<members->GetEntries(); ++j) {	
	  TRealData* rd  = (TRealData*)(members->At(j));
	  Int_t offset = rd->GetThisOffset();
      char *pointer = (char*)obj + offset;		
	  TDataMember *member = rd->GetDataMember();	  
      //if ( strstr(rd->GetName(),".") ) continue;
	  //cout << rd->GetName()<<endl;				
      fHp->Push(rd->GetName()); 
      if (fVerbose>2) 
	    cout << "MEMBER "<<rd->GetName() << " " <<member->GetName() 
		     << ": "<< Form(fTypeFormat,rd->GetDataMember()->GetTypeName()) 
			 << "\n";
      if (!fInspect) {
	    cout << fHp->Path()<<endl;        
	  }	 else {
		Inspect(cl,member->GetName(),(void*)pointer); 
	  }	  	 
      fHp->Pop();
    }
*/
}      
//______________________________________________________________________________
void DumpAll::Inspect(TClass *cl, const char *mname, void *add)
{
   // Print value of member mname.
   //
   //    cl    is the pointer to the current class
   //    pname is the parent name (in case of composed objects)
   //    mname is the data member name
   //    add   is the data member address
   //
   // (this method "inspired" by TDumpMembers - can probably be simplified)
   

   if (fVerbose>2) cout << "<Inspect " << mname << "> ";

   const Int_t kvalue = 0;
   const Int_t kline  = 1024;

   Int_t cdate = 0;
   Int_t ctime = 0;
   UInt_t *cdatime = 0;
   char line[kline];
   
   TDataType *membertype;
   TString memberTypeName;
   const char *memberName;
   const char *memberFullTypeName;
   //const char *memberTitle;
   Bool_t isapointer;
   Bool_t isbasic;   
   
   if (TDataMember *member = cl->GetDataMember(mname)) {
   
      if (fVerbose>2) cout << " (DataMember) >" << endl;
	  
      memberTypeName 	 = member->GetTypeName();
      memberName 		 = member->GetName();
      memberFullTypeName = member->GetFullTypeName();
      //memberTitle 		 = member->GetTitle();
      isapointer 		 = member->IsaPointer();
      isbasic 			 = member->IsBasic();
      membertype 		 = member->GetDataType();
   
   } else if (!cl->IsLoaded()) {
   	   
      if (fVerbose>2) cout << " (Emulated) >";
	  // The class is not loaded, hence it is 'emulated' and the main source of
      // information is the StreamerInfo.
      TVirtualStreamerInfo *info = cl->GetStreamerInfo();
      if (!info) return;
      
	  const char *cursor = mname;
      while ( (*cursor)=='*' ) ++cursor;
      TString elname( cursor );
      Ssiz_t pos = elname.Index("[");
      if ( pos != kNPOS ) {
         elname.Remove( pos );
      }	  
      TStreamerElement *element = (TStreamerElement*)info->GetElements()->FindObject(elname.Data());
      
	  if (!element) {
	    //printf("WARNING Can't find element %s\n",elname.Data());
		return;
	  }
	  
	  memberFullTypeName = element->GetTypeName();
      
      memberTypeName = memberFullTypeName;
      memberTypeName = memberTypeName.Strip(TString::kTrailing, '*');
      if (memberTypeName.Index("const ")==0) memberTypeName.Remove(0,6);

      memberName = element->GetName();
      //memberTitle = element->GetTitle();
      isapointer = element->IsaPointer() || element->GetType() == TVirtualStreamerInfo::kCharStar;
      membertype = gROOT->GetType(memberFullTypeName);
      
      isbasic = membertype !=0;
	   
   } else {
     printf("<WARNING> Can't get data member %s on %s>\n",mname,cl->GetName());
	 return;
   }      

   //cout <<memberFullTypeName<<"|" <<memberTypeName << "|" << memberName << endl;
   
   TClass *clp = TClass::GetClass( memberTypeName );
   if (clp) {
   //TVirtualCollectionProxy *proxy = clp->GetCollectionProxy();
   //if (proxy) {   
      if ( fFollowPointers ) {
		FollowPointer((char*)add,memberTypeName);
      } else { 
        if (fVerbose>1) cout << Form(fTypeFormat, memberFullTypeName);	        
        if (fVerbose  ) cout << fHp->Path() <<"->"<< endl;
      }
     return;
   //}
   }
	
   Bool_t isdate = kFALSE;
   if (strcmp(memberName,"fDatime") == 0 && strcmp(memberTypeName,"UInt_t") == 0) {
      isdate = kTRUE;
   }
   Bool_t isbits = kFALSE;
   if (strcmp(memberName,"fBits") == 0 && strcmp(memberTypeName,"UInt_t") == 0) {
      isbits = kTRUE;
   }

   Int_t i;
   for (i = 0;i < kline; i++) line[i] = ' ';
   line[kline-1] = 0;

   i = strlen(line); line[i] = ' ';

   // Encode data value or pointer value
   char *pointer = (char*)add;
   char **ppointer = (char**)(pointer);

   if (isapointer) {
      char **p3pointer = (char**)(*ppointer);
      if (!p3pointer)
         sprintf(&line[kvalue],"->0");
      else if (!isbasic)
         sprintf(&line[kvalue],"->%lx ", (Long_t)p3pointer);
      else if (membertype) {
         if (!strcmp(membertype->GetTypeName(), "char")) {
            i = strlen(*ppointer);
            if (kvalue+i >= kline) i=kline-kvalue;
            Bool_t isPrintable = kTRUE;
            for (Int_t j = 0; j < i; j++) {
               if (!std::isprint((*ppointer)[j])) {
                  isPrintable = kFALSE;
                  break;
               }
            }
            if (isPrintable) {
               strncpy(line + kvalue, *ppointer, i);
               line[kvalue+i] = 0;
            } else {
               line[kvalue] = 0;
            }
         } else {
            strcpy(&line[kvalue], membertype->AsString(p3pointer));
         }
      } else if (!strcmp(memberFullTypeName, "char*") ||
                 !strcmp(memberFullTypeName, "const char*")) {
         i = strlen(*ppointer);
         if (kvalue+i >= kline) i=kline-kvalue;
         Bool_t isPrintable = kTRUE;
         for (Int_t j = 0; j < i; j++) {
            if (!std::isprint((*ppointer)[j])) {
               isPrintable = kFALSE;
               break;
            }
         }
         if (isPrintable) {
            strncpy(line + kvalue, *ppointer, i);
            line[kvalue+i] = 0;
         } else {
            line[kvalue] = 0;
         }
      } else {
         sprintf(&line[kvalue],"->%lx ", (Long_t)p3pointer);
      }
      
   } else if (membertype) {
      if (isdate) {
         cdatime = (UInt_t*)pointer;
         TDatime::GetDateTime(cdatime[0],cdate,ctime);
         sprintf(&line[kvalue],"%d/%d",cdate,ctime);
      } else if (isbits) {
         sprintf(&line[kvalue],"0x%08x", *(UInt_t*)pointer);
      } else {   
        if (fVerbose>1) cout << "    " << Form(fTypeFormat, memberFullTypeName);	        
        if (fVerbose  ) cout << fHp->Path() <<" = "<< membertype->AsString(pointer) << endl;
        fHp->Fill( AsFloat(pointer,(EDataType)membertype->GetType()) );
      }
   } else {
     printf("<WARNING> cannot inspect %s", mname);
   }
}
//______________________________________________________________________________
void DumpAll::FollowPointer(char*pointer, TString memberTypeName)
{ 
	if (fVerbose>2) { cout << "FollowPointer "; }
    
	TClass *cl = TClass::GetClass( memberTypeName );
	TVirtualCollectionProxy *proxy = cl->GetCollectionProxy();

	// Single object   
	if ( !proxy ) {
       DumpObject(cl,pointer);
       return;
	}

	// We have an STL collection.      
    TClass*   pcl    = proxy->GetValueClass();
	EDataType type   = proxy->GetType();
	//if (proxy->HasPointers()) {		
	//  TClass* pccl = proxy->GetCollectionClass();
	//}
    // Set the collection proxy to look at our object           
	TVirtualCollectionProxy::TPushPop helper(proxy, pointer);

    UInt_t    nelems = proxy->Size();        

    if (fShowContainers) { 
      if (fVerbose>1) cout <<"STL "<<Form(fTypeFormat,cl->GetName());
      if (fVerbose  ) cout << fHp->Path() <<" @"<<nelems<< endl;
	  // histogram containing array elements
	  fHp->Fill(nelems);		
	}

    if ( nelems==0 ) return;

    if (!pcl) { // SIMPLE TYPES

      if (fVerbose>1) cout <<"    "<< Form(fTypeFormat, TDataType::GetTypeName(type));
      if (fVerbose  ) cout << fHp->Path() << " ["<<proxy->Size()<<"] = ";
	  
      for ( UInt_t i = 0; i < nelems; ++i ) {
        void *content = proxy->At( i );
		if (fVerbose && i < fMaxarraySdt ) {
		  cout << AsString(content,type);
 	      if (i < nelems-1) cout << ",";						
 	    }  
   		// fill all elements of collection into same histogram
        fHp->Fill(AsFloat(content,type));
      }
      if (proxy->Size() > fMaxarraySdt ) {
        if (fVerbose) {
		  cout << " ... (" << proxy->Size()-fMaxarraySdt << " more "
		       << TDataType::GetTypeName(type) <<")";
		} 	   
      }
      if (fVerbose) cout << endl;      

    } else { // OBJECTS
      
	  for ( UInt_t i = 0; i < nelems; ++i ) {
        void *content = proxy->At( i ); 
	    if (i < fMaxarrayObj) DumpObject(pcl,content);		  			
      }   
      if (proxy->Size() > fMaxarrayObj) {
  	  	//if (fShowContainers) {
        //  if (fVerbose>1) cout <<"STL "<< Form(fTypeFormat, cl->GetName());
        //  if (fVerbose)   cout << fHp->Path();
		//}  
        if (fVerbose>1) cout <<"    "<< Form(fTypeFormat, "");		
		if (fVerbose) {
		  cout << "... (" << proxy->Size()-fMaxarrayObj << " more "
		       << pcl->GetName() <<")"<<endl;
		}	 
      }
    }
}
//______________________________________________________________________________
const char *DumpAll::AsString(void *buf, EDataType datatype)
{
   // Return string containing value in buffer formatted according to
   // the basic data type. The result needs to be used or copied immediately.

	if (fVerbose>3) cout << "\nAsString" << endl;
   
   static const char* unknown = "(UNKNOWN)";
   static const char* null = "(NULL)";
   static char line[81];

   if (!buf) {
     return null;
   } 

   switch(datatype) {
   case kChar_t:     
   case kUChar_t:    
   case kchar:		 //strncpy(line, "(not shown)", 11);//strncpy(line, (char *)buf,80);
					 sprintf(line, "'%c'", ((char*)buf)[0] & 0x7F); //((char*)buf)); 				
					 return line;
   case kBool_t:     sprintf(line, "%s", *(bool *)buf ? "true" : "false"); return line;    
   case kShort_t:    sprintf(line, "%u", *(short *)buf); 			return line;
   case kUShort_t:   sprintf(line, "%hu", *(unsigned short *)buf); 	return line;
   case kCounter:    
   case kInt_t:      sprintf(line, "%d", *(int *)buf); 				return line;
   case kUInt_t:     sprintf(line, "%u", *(unsigned *)buf); 		return line;
   case kDouble_t:
   case kDouble32_t: sprintf(line, "%8.4g", *(double *)buf); 		return line;
   case kFloat_t: 
   case kFloat16_t:  sprintf(line, "%8.4f", *(float *)buf); 		return line;
   case kLong_t:     sprintf(line, "%ld", *(long *)buf);  			return line;
   case kULong_t:    sprintf(line, "%lu", *(unsigned long *)buf); 	return line;
   case kLong64_t:   sprintf(line, "%llu", *(Long64_t *)buf); 		return line;
   case kULong64_t:  sprintf(line, "%llu", *(ULong64_t *)buf); 		return line;
   case kCharStar:   sprintf(line, "%s", *(char**)buf); 			return line;
   case kBits:             
   case kOther_t:
   case kNoType_t: 
   default: 
      return unknown;
   }             

// still missing?
//      sprintf(line, "%hu", *(unsigned short *)buf);
//      sprintf(line, "%hd", *(short *)buf);      

   return line;
}
//______________________________________________________________________________
Float_t DumpAll::AsFloat(void *buf, EDataType datatype)
{
   // Return string containing value in buffer converted to float

   if (fVerbose>3) cout << "AsFloat " << endl;
   
   if (!buf) { return 0.; } 

   switch(datatype) {
   case kChar_t:     
   case kUChar_t:    return (Float_t)(*(char *)buf); 
   case kBool_t:     return (Float_t)(*(bool *)buf);     
   case kShort_t:    return (Float_t)(*(short *)buf);
   case kUShort_t:   return (Float_t)(*(unsigned short *)buf); 
   case kCounter:    
   case kInt_t:      return (Float_t)(*(int *)buf); 
   case kUInt_t:     return (Float_t)(*(unsigned *)buf); 
   case kDouble_t:
   case kDouble32_t: return (Float_t)(*(double *)buf); 
   case kFloat_t: 
   case kFloat16_t: return (Float_t)(*(float *)buf); 
   case kLong_t:    return (Float_t)(*(long *)buf); 
   case kULong_t:   return (Float_t)(*(unsigned long *)buf);
   case kchar:      return (Float_t)(*(char *)buf);
   case kLong64_t:  return (Float_t)(*(Long64_t *)buf);
   case kULong64_t: return (Float_t)(*(ULong64_t *)buf);
   case kCharStar:  return 0.; //(Float_t)((char**)buf);

   case kBits:             
   case kOther_t:
   case kNoType_t: 
   default: 
      return 0.;
   }             
}
 

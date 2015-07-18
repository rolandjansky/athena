/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file cobs.cxx
 *  @brief Rewrite root files in various ways to speed up reading.
 *  @author: Ilija Vukotic, Gerhard Brandt
 *  $Id: $
 *
 * Current defaults are: 
 *
 *      compression level = 6
 *      split level = 99
 *      dedicated memory = 30MB (per tree)
 *      minimal compression factor = 1.0 (compression is never off)
 *       
 * There are three main transformations that
 * should be applied in the following precedence:
 *
 *      1. change tree split level (should be set on write already)
 *      2. change tree basket sizes
 *      3. change tree basket order
 *      
 **/
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "cobs.h"
#include <vector>
#include <string>
#include <iostream>

#include <Riostream.h>
#include <TTree.h>
#include <TFile.h>
#include <TSystem.h>
#include <TROOT.h>
#include <TKey.h>
#include <TLeaf.h>

using namespace std;
 

//__________________________________________________________________________
cobs::cobs(string inputFile) : m_inputFilename(inputFile) {
    m_compressionLevel     = 6;
    m_splitLevel           = 99;
    m_dedicatedMemory      = 30720; //30Mb
    m_minCompressionFactor = 1.0;
    m_libraryName          = "RFT";
    m_libraryDirName       = "RFT/RFT.so";
    m_useCINTlibrary       = false;
    m_debug                = false;
    m_showBranches         = false;
}
//__________________________________________________________________________
cobs::cobs(string inputFile, string outputFile) :
        m_inputFilename(inputFile),
        m_outputFilename(outputFile)
{
    m_compressionLevel     = 6;
    m_splitLevel           = 99;
    m_dedicatedMemory      = 30720; //30Mb
    m_minCompressionFactor = 1.0;
    m_libraryName          = "RFT";
    m_libraryDirName       = "RFT/RFT.so";
    m_useCINTlibrary       = false;
    m_debug                = false;
    m_showBranches         = false;
}

//__________________________________________________________________________
void cobs::setDebug(bool debug)
{
    m_debug = debug;
}

//__________________________________________________________________________
void cobs::setOutputFile(string outputFile){
    m_outputFilename    = outputFile;
}
//__________________________________________________________________________
void cobs::setCompressionLevel(short CL){
    // from (0-9) ROOT default is 1. Outside HEP people use 6. 
    if (CL>9) CL=9;
    if (CL<0) CL=0;
    m_compressionLevel = CL;
}
//__________________________________________________________________________
void cobs::setSplitLevel(short SL){
    m_splitLevel        = SL;
}
//__________________________________________________________________________
void cobs::setDedicatedMemory(int DM){
    // this call sets default amount of memory to be used.
    // this is not split on all the trees.
    // to give a memory per tree use setTreeMemory  
    // value is given in kb
    m_dedicatedMemory   = DM;
}       
//__________________________________________________________________________
void cobs::setDebugLevel(short DL){
    m_debug   = DL;
}
//__________________________________________________________________________
void cobs::setTreeToSkip(string treeName){
    // tree not to be used in transform
    // when rewriting into new file it will be simply cloned.
    // still compression level will be the one of the output file.
    m_treesToSkip.push_back(treeName);
}
//__________________________________________________________________________
void cobs::setTreeMemory(int mem, string treeName){
    // used to set dedicated memory for a specific tree
    std::pair<int, string> p(mem, treeName);
    m_memoryForATree.push_back(p);
}
//__________________________________________________________________________
void cobs::setMinCompressionFactor(float minComp){
    // if compression of a branch in input file is smaller than value given
    // the branch will not be compressed. Default value is 1.0 (never)
    m_minCompressionFactor = minComp;
}
//__________________________________________________________________________
void cobs::setLibraryName(std::string libN){
    m_libraryName=libN;
    m_libraryDirName=libN+"/"+libN+".so";
}
//__________________________________________________________________________
void cobs::useCINTLibrary(bool use){
    m_useCINTlibrary = use; 
}
//__________________________________________________________________________
void cobs::mkProject(){
    // this function should be called just once before the first transform.
    // it does not work in root versions before 5.25 
    if (!m_useCINTlibrary) return;
    std::cout<<"Making library corresponding to the file: "<< m_inputFilename<<std::endl;
    TFile* f = TFile::Open(m_inputFilename.c_str());
    if (f->IsZombie()) {
        cout << "Error! Can't opend the file to makeProject. Returning." << endl;
        return;
    }
    f->MakeProject(m_libraryName.c_str(),"*","recreate++");
    f->Close();
}
//__________________________________________________________________________
bool cobs::order(short orderBy){
    // orders the branches in one of 3 ways: 
    // 1- byOffset (root default so normaly not to be used)
    // 2- by Entry (optimal one when reading all the events)
    // 3- byBranch (can be optimal in certain quite special cases)
    // output file compression level is root default (=1)
    
    pid_t p = getpid();
    char com [50];
    sprintf (com, "ls -l /proc/%d/fd > openedfiles", p);
    system(com);
    
    if (m_debug) {  
        cout<<"<cobs::order> "<<m_inputFilename;
        if (orderBy==1) cout<<" (SortBasketsByOffset)"<<endl;
        if (orderBy==2) cout<<" (SortBasketsByEntry)"<<endl;
        if (orderBy==3) cout<<" (SortBasketsByBranch)"<<endl;
    }
        
    if (m_useCINTlibrary) {
        gSystem->Load(m_libraryDirName.c_str());
    }
        
    TFile* f  = TFile::Open( m_inputFilename.c_str()); 
    if (f->IsZombie()) {
        cout << "Error with file to be optimized. File is maybe not closed ?" << endl;
        return false;
    }     
    TFile* f2 = TFile::Open( m_outputFilename.c_str(),"RECREATE");
 
    sprintf (com, "ls -l /proc/%d/fd > openedfiles1", p);
    system(com);
            
    TTree* T2 = 0;
    TList* keys = f->GetListOfKeys();
    TKey*  key;
     
    TIter  next( keys );
    while (( key = static_cast< TKey * > (next()) )) {

        TString  name(key->GetName());    
        TObject* obj = f->Get( name );

        TClass *classPtr = gROOT->GetClass((const char *) key->GetClassName());
        if (m_debug) {
            cout << "<cobs::order> (" << classPtr->GetName() <<") "<<name;
        }
                
        if ( !(classPtr->GetBaseClass("TTree")) ) {
            cout << "<cobs::order> WARNING ! object " << name << " not cloned!" << endl;
            continue; 
        }
                
        TTree* T = static_cast<TTree*>(obj);

//      if (m_debug) cout<<"<cobs::order> Ordering tree "<<T->GetName()<<endl;
        if (m_debug) cout<<" --- ordering "<<endl;

        if (orderBy==1)      T2 = T->CloneTree(-1,"SortBasketsByOffset fast");
        else if (orderBy==2) T2 = T->CloneTree(-1,"SortBasketsByEntry fast");
        else if (orderBy==3) T2 = T->CloneTree(-1,"SortBasketsByBranch fast");

        if (!T2) {
          cout << "Did't get tree; bad orderBy? " << orderBy << "\n";
          return false;
        }
        int s=T2->Write("",TObject::kOverwrite);
        if (!s) {
            cout<<" WARNING FILE IS NOT OPTIMIZED. MAYBE NOT ENOUGH DISK SPACE? "<<endl;
            delete f2;
            delete f;
            return false;
            }
        f2->WriteStreamerInfo();
    }
        
    //if (m_debug) cout << "------------ copy"<<endl;
    //f2->ls();
    delete f2;
        
    //if (m_debug) cout << "------------ original"<<endl;
    //f->ls();
    delete f;
    return true;
}
//__________________________________________________________________________
bool cobs::resizeBaskets(){
    // optimizes basket sizes 
    // newly produced file has compression level requested.
    // by default uses function which minimizes total number of baskets in the file
    
    if (m_debug) cout<<"<cobs::resizeBaskets> Resizing baskets ... START"<<endl;
        
    // // TVirtualStreamerInfo::SetStreamMemberWise(1);
    
    if (m_useCINTlibrary) {
        gSystem->Load(m_libraryDirName.c_str());
    }

    
    TFile* f  = TFile::Open( m_inputFilename.c_str());
    if (f->IsZombie()) {
        cout << "Error file to be optimized. File is maybe not closed ?" << endl;
        return false;
    }  
    TFile* f2 = TFile::Open(m_outputFilename.c_str(),"RECREATE","",m_compressionLevel);
    // f2->Write();
    

    TTree* T2 = 0;
    TList* keys = f->GetListOfKeys();
    TKey*  key;
    
    TIter  next( keys );
    while (( key = static_cast< TKey * > (next()) )) {
        
        TString  name(key->GetName());      
        TObject* obj = f->Get( name );

        TClass *classPtr = gROOT->GetClass((const char *) key->GetClassName());
        
        if (m_debug) {
            cout << "<cobs::resize> (" << classPtr->GetName() <<") "<<name;
        }

        if ( !(classPtr->GetBaseClass("TTree")) ) {
            cout << "<cobs::resize> WARNING ! object " << name << " not cloned!" << endl;
            continue; 
        }

        TTree* T = static_cast<TTree*>(obj);
        
        // if no explicit request which trees *not* to optimize is made
        // only optimize the main payload tree
        int skip=0;
        if ( m_treesToSkip.size() == 0) {
            if (strcmp("CollectionTree",T->GetName())) skip=1;
        }
        else{
            // optimize all except those in skip-list
            for (vector<string>::iterator i = m_treesToSkip.begin(); i != m_treesToSkip.end(); i++)
                if (T->GetName() == (*i)) skip=1;
        }
        
        if (m_debug){
            TObjArray *ls = T->GetListOfLeaves();
            Int_t nls = ls->GetEntries();
            cout<<"<cobs::resizeBaskets> "<<name <<" entries: "<<T->GetEntries()<<" size: "<<T->GetTotBytes() <<" zipped: "<<T->GetZipBytes()<<" leaves: "<<nls<<endl;
        }

        int mem = m_dedicatedMemory; // default 
        for (vector<pair<int, string> >::iterator i=m_memoryForATree.begin(); i != m_memoryForATree.end(); i++)
            if ( name==(*i).second ) mem = (*i).first;
  
        if (skip) {
            cout<<"<cobs::resizeBaskets> skipping tree "<<T->GetName()<<endl;
            T2 = T->CloneTree(-1,"SortBasketsByEntry fast");
        }
        else { 
            SimpleOptimizeBaskets(T, mem, m_minCompressionFactor);
            T2 = T->CloneTree(-1,"SortBasketsByEntry");
        }
        
        int s=T2->Write("",TObject::kOverwrite);
        if (!s) {
            cout<<" WARNING FILE IS NOT OPTIMIZED. MAYBE NOT ENOUGH DISK SPACE? "<<endl;
            delete f2;
            delete f;
            return false;
        }
        f2->WriteStreamerInfo();
        
        if (m_debug) cout<<"<cobs::resizeBaskets> new tree zip size: "<<T2->GetZipBytes()<<endl;
    }


    // if (m_debug) {
    //     cout << "------------ copy"<<endl;
    //     f2->ls();
    // }
    delete f2;

    // if (m_debug) {
    //     cout << "------------ original"<<endl;
    //     f->ls();
    // }
    delete f;

    cout<<"Optimizing baskets sizes FINISHED"<<endl;
    return true;
}
//__________________________________________________________________________
void cobs::changeSplitLevel(){
    // this function completely rewrites trees from one file to another
    // it can change compression level
        
    if (m_debug) cout<<"Changing split level ... START"<<endl;
    // TVirtualStreamerInfo::SetStreamMemberWise(1);        
    if (m_useCINTlibrary) gSystem->Load(m_libraryDirName.c_str());

    TFile* f  = TFile::Open( m_inputFilename.c_str());  
    if (f->IsZombie()) {
        cout << "Error! Can't open input file to change split level. returning. " << endl;
        return;
    }
    TFile* f2 = TFile::Open(m_outputFilename.c_str(),"recreate","",m_compressionLevel);     

    TList* keys = f->GetListOfKeys();
    TKey*  key;

    TIter  next( keys );
    while (( key = static_cast< TKey * > (next()) )) {

        TString  name(key->GetName());      
        TObject* obj = f->Get( name );

        TClass* classPtr = gROOT->GetClass((const char *) key->GetClassName());

        if ( !(classPtr->GetBaseClass("TTree") )) continue;

        TTree *  T                        = static_cast<TTree*>(obj);                 
        int      nls          = T->GetListOfLeaves()->GetEntries();               
        // Double_t treeSize     = (Double_t)T->GetTotBytes();
                
        cout<<name<<"\t entries: "<<T->GetEntries()<<"\t Size: "<<T->GetTotBytes()<<"\t zipped:"<<T->GetZipBytes()<<"\t leaves: "<<nls<<endl;

        int skip=0;
        for (vector<string>::iterator i = m_treesToSkip.begin(); i != m_treesToSkip.end(); i++)
            if (name==(*i)) skip=1;
        if (skip) {
            if (m_debug) cout<<"skipping this tree"<<endl;
            TTree *T2 = T->CloneTree(-1,"fast");
            T2->Write("", TObject::kOverwrite);
            cout<<"---------------------------------------------------"<<endl;
            continue;
        }
               
        RecreateTheTree( T ); 
    }

    delete f2; 
        
    if (m_debug) cout<<"Changing split level ... STOP"<<endl;
}
//__________________________________________________________________________
void cobs::SimpleOptimizeBaskets(TTree* tree, unsigned long memory, float minComp){
    // function which minimizes total number of baskets in the file
    // it will not take more memory than a total tree size

    if (tree->GetDirectory()->IsWritable()) tree->FlushBaskets();

    memory *= 1024; // change to bytes
    TObjArray *leaves      = tree->GetListOfLeaves();
    const int nleaves      = leaves->GetEntries();

    int       * totSize    = new int[nleaves];
    int       * basketSize = new int[nleaves];
    int       * baskets    = new int[nleaves];
    float     *CF          = new float[nleaves];


    int Nnb=0; int NBnb=0;
       
    for (int i=0;i<nleaves;i++) { // collects data needed
        TLeaf   *leaf    = (TLeaf*)leaves->At(i);
        TBranch *branch  = leaf->GetBranch();
        int   nb       = branch->GetListOfBranches()->GetEntries();

        totSize[i]    = branch->GetTotBytes();
        basketSize[i] = totSize[i];
        baskets[i]    = 1;
        CF[i]         = 1.0;
        if (branch->GetTotBytes()) CF[i]= (float)branch->GetZipBytes()/branch->GetTotBytes();

        if (nb > 0) {
            Nnb++;
            NBnb += 1 + int(branch->GetTotBytes() /2048);
            // cout << "no B: " 
            // << "\t bs: "<<branch->GetBasketSize()
            // << "\t tot byte: "<< branch->GetTotBytes() 
            // << "\t tot size: " << branch->GetTotalSize() 
            // << "\t zip size: " << branch->GetZipBytes()
            // <<"\t name: "<<branch->GetName() <<endl;
        }

    }
    // cout<<"Nnb: "<<Nnb<<"\tNbasketsNB:"<<NBnb<<endl;

    while(true){ // recursive optimization

        double totMem=0;
        for (int i=0;i<nleaves;i++)
            totMem += basketSize[i];

        if ( totMem < memory) break;

        int maxSpare=-1; int mi=-1;
        for (int i=0;i<nleaves;i++){
            int spare=totSize[i]/baskets[i]-totSize[i]/(baskets[i]+1);
            if (maxSpare<spare){
                maxSpare=spare; 
                mi=i;
            }
        }
        baskets[mi]++;
        basketSize[mi]=totSize[mi]/baskets[mi];
        // cout<<"Spare: "<<totSize[mi]/(baskets[mi]-1)-totSize[mi]/baskets[mi]<<"\t baskets: "<<baskets[mi]<<"\t";
    }
 
    double totMem = 0;
    int    totBas = 0;
    int    maxBs  = -1;
    int    minBs  = 1024*1024*1024;
    for (int i=0;i<nleaves;i++) {           
        TLeaf *leaf = (TLeaf*)leaves->At(i);
        TBranch *branch = leaf->GetBranch();
        if (basketSize[i]==0) basketSize[i]=16;
        basketSize[i] = basketSize[i] - basketSize[i] % 8;
        branch->SetBasketSize(basketSize[i]);
        if (CF[i]<minComp) branch->SetCompressionLevel(0);

        totMem += basketSize[i];
        totBas += baskets[i];
        if (basketSize[i]<minBs) minBs=basketSize[i];
        if (basketSize[i]>maxBs) maxBs=basketSize[i];
        // cout<<basketSize[i]<<"\t"<<baskets[i]<<endl;
    }
    if (m_debug) cout<<"<cobs::SimpleOptimizeBaskets> Total memory buffer: "
                     <<totMem * 1e-3 <<" kb total baskets: "<<totBas<<"\tmin BS: "<<minBs<<"\tmax BS: "<<maxBs<<endl;
    //cout<<"-------------------------------------------------------------------------------------------------"<<endl;
    delete [] totSize;      
    delete [] basketSize;
    delete [] baskets;
    delete [] CF;
}


//__________________________________________________________________________
bool cobs::RecreateTheTree(TTree* T){
    // deep tree copy
    if (m_debug) cout << "<cobs::RecreateTheTree> Recreating tree "<< T->GetName();
        
    TTree *T2 = new TTree(T->GetName(),T->GetTitle(),m_splitLevel);

    Int_t nbranches = T->GetListOfBranches()->GetEntriesFast();
    TBranch *branch;
        
    for (Int_t i=0; i<nbranches; i++) {

        branch = (TBranch*)T->GetListOfBranches()->UncheckedAt(i);
        branch->SetAddress(0);

        if (m_debug) {
            cout <<"<cobs::RecreateTheTree> "<< branch->GetName()
                //<<"\t class: "<< branch->GetClassName()
                 <<"\t BS: "<< branch->GetBasketSize()<<"\t size: "<< branch->GetTotalSize() << endl;
        }       

        if (strlen(branch->GetClassName())) {   // branch with object
            T2->Branch(branch->GetName(), branch->GetClassName(), branch->GetAddress(), branch->GetBasketSize(), m_splitLevel);
        } else {                // branch with simple type
            T2->Branch(branch->GetName(), branch->GetAddress(), branch->GetBasketSize(), m_splitLevel);
        }
    }

    unsigned long nentries = T->GetEntries();
    for ( unsigned long e = 0; e < nentries; ++e) {
        T->GetEntry(e);
        T2->Fill();
        if (!(e%100))  cout << "\tevent: "<< e << endl;
    }

    int s = T2->Write("", TObject::kOverwrite);       
    if (!s) {
        cout<<" WARNING FILE IS NOT OPTIMIZED. MAYBE NOT ENOUGH DISK SPACE? "<<endl;
        return false;
    }              
    //cout << "<cobs::RecreateTheTree> Recreated tree. Now "<< T2->GetTotBytes()/1000000<<" MB "<<endl;
    return true;
}
//__________________________________________________________________________
void cobs::printTrees(std::string fileName){

    //gDebug=1;

    TFile* f  = TFile::Open( fileName.c_str()); 
    if (f->IsZombie()) {
        cout << "Error! Can't open file to print trees. returning..." << endl;
        return;
    }
    cout << Form("%-45s %12s[Mb]%9s[Mb]%8s %8s %8s\n",
                 "TREE","TotBytes","ZipBytes","Entries","Branches","Leaves") << endl;

    // Loop on keys and find all trees
    TList* keys = f->GetListOfKeys();
    TKey*  key;
    TIter  next( keys );
    while ( (key = (TKey *)(next())) ) {
        TString  name(key->GetName());      
        TClass *classPtr = gROOT->GetClass((const char *) key->GetClassName());
        if ( classPtr->GetBaseClass("TTree") ) {
            TTree* T = (TTree*)(f->Get(name));                          

			Int_t nentries  = T->GetEntries();
			Int_t nbranches = T->GetListOfBranches()->GetEntries();
			Int_t nleaves   = T->GetListOfLeaves()->GetEntries();

            printf("TREE   %-45s %12.6f %12.6f %8d %8d %8d\n",
                         T->GetName(),
                         1.e-6*(Float_t)(T->GetTotBytes()),
                         1.e-6*(Float_t)(T->GetZipBytes()),
                         nentries, nbranches, nleaves
                         );
						 
		    if (m_showBranches == true) printBranches(T);
						 
        } else {
		  // also show other objects on file
		  cout << "("<<classPtr->GetName()<<") "<<name<<endl;
		}
    }

    delete f;
}
//____________________________________________________________________________
Int_t cobs::printBranches(TTree *T) {

  Int_t nbranches = T->GetListOfBranches()->GetEntries();
  Int_t nb=0;
  Int_t entry=0;
  Int_t getall=1;

  //hBranchSizes = new TH1F("hbranchsizes","Branch Sizes (unzipped)",nbranches,0,nbranches);
  //hBranchSizes->SetYTitle("Mb");
  //hBranchSizes->SetBit(TH1::kCanRebin);

  printf("%112s\n","basketsize[kb]");

  for (Int_t i=0;i<nbranches;i++)  {
     TBranch* branch = (TBranch*)T->GetListOfBranches()->At(i);

     branch->SetAddress(0);

 	 const char* pname  = branch->GetName();     
     const char* clname = branch->GetClassName();

     // only show transient part of name
	 const char * tname = pname;
	 if (strstr(pname,clname)) {
       tname = pname+strlen(clname)+1;	  
     }
	 
     nb = branch->GetEntry(entry, getall);
     if (nb < 0) {
       cout << "I/O error reading branch " << branch->GetName() << endl;
       return nb;
     }
     Float_t ntot= branch->GetTotBytes("*");
     Float_t nzip= branch->GetZipBytes("*");
	 
	 Int_t nbranches  = branch->GetListOfBranches()->GetEntries();
 	 Int_t basketsize = branch->GetBasketSize();

     //if (ntot>20.e6) {
     printf("BRANCH   %-43s %12.6f %12.6f          %8d %8.3f\n",
         tname, 1.e-6*(Double_t)(ntot), 1.e-6*(Double_t)(nzip), 
         nbranches, 1.e-3*basketsize );

       //hBranchSizes->Fill(branch->GetName(),1.e-6*(Double_t)(ntot));
     //}
  }
  cout << endl;

  return nbranches;
}


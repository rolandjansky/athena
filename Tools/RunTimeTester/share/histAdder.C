//********************************
// Root macro to read in and sum histograms from a number of files,
// and to result the sum back out
//
// Needs more work to ensure character buffers do not overflow.
// PS 10/04
//********************************


TObjArray* getHists(char* fname){
  //printf("getHists: start\n");
  TObjArray* hists = new TObjArray(0);
  int size = hists->GetSize();
  //printf("getHists: size on creation: %d\n",size);
  
  TFile* f = new TFile(fname);

  TIter next(f->GetListOfKeys());
  TKey *key;
  while ((key=(TKey*)next())) {
    char* name = key->GetName();
    //printf("getHists: obj name %s\n", name);
    TObject* obj = f->Get(name);
    hists->Add(obj);
  }  
  
  int size = calcSize(hists);
  printf("getHists: from %s found %d hists\n", fname, size);
  return hists;
}

int calcSize(TObjArray* col){
  //printf("calcSize: start:");
  int count = 0;
  TIter iter = col->MakeIterator();
  while(iter.Next()){++count;}
  return count;
}
bool addHists(char* fname, TObjArray* hsums, char* rfname, TObjArray* rsums){
  TObjArray* hists  = getHists(fname);
  TObjArray* rhists = getHists(rfname);

  int hsize = calcSize(hists);
  int ssize = calcSize(hsums);
  int rsize = calcSize(rhists);

  if(hsize != ssize || hsize != rsize){
    printf("unequal numbers of hists: new h, new r, sum: %d %d %d",
	   hsize, rsize, ssize);
    printf("skipping %s\n", fname);
    return 0;
  }


  accumulateHists(hists, hsums);
  accumulateHists(rhists, rsums);

  return 1;
}

void accumulateHists(TObjArray* hists, TObjArray* hsums){
  TIter h_iter(hists);
  TIter s_iter(hsums);
  TH1* h;
  while( h = (TH1*)h_iter.Next()){
    s = (TH1*)s_iter.Next();
    s->Add(h);
  }
}

void writeHists(char* sumfile, TObjArray* hists){
  cout<<"writing to..."<<sumfile<<endl;
  //printf("number of hists to write: %d\n",calcSize(hists));
  ofile = new TFile(sumfile, "recreate");
  //printf("number of hists to write: %d\n",calcSize(hists));
  hists->Write();
  ofile->Close();
}

int nameWithoutCC(char* name, int n, FILE* fp){
  //printf("nameWithoutCC: start \n");
  char temp[201];    //buffer size now doubled 10-03-05.
  char* s;

  if (s = fgets(&temp, n, fp)){
    //printf("before shortening %s", &temp);
    //////////////////////////////////
    // Mac only!!!!
    int nchar = strlen(s)-1;
    s = strncpy(name, &temp, nchar);
    //////////////////////////////////
    //////////////////////////////////
    // Linux
    // s = strcpy(name, &temp);
    //////////////////////////////////
    //printf("%d\n", nchar);
    //printf("\n");
    // printf("nameWithoutCC: end1: %s\n", s);
    return 1;
  }
  // printf("nameWithoutCC: end2\n");

  return 0;
}
bool fillInFileName(char* lp, char* fn){
  int csize = strlen(lp)+strlen(fn)+strlen("/");
  if (csize>200){
    printf("lp character buffer too short  need %d",csize);
    return 0;
  }
  strcat(lp,"/");
  strcat(lp,fn);
  return 1;
}

bool fillInRefFileName(char* lp, char* fn){
  int csize = strlen(lp)+strlen(fn)+strlen("/refFile_");
  if (csize>200){
    printf("lp character buffer too short  need %d",csize);
    return 0;
  }
  strcat(lp,"/refFile_");
  strcat(lp,fn);
  return 1;
}


int histAdder(char* fname){

  //printf("histAddr: start\n");
  //printf("histAddr: %s\n", fname);
    
  FILE *fp = fopen("histLocation.txt","r");     


  TObjArray* hists  = new TObjArray(0);
  TObjArray* rhists = new TObjArray(0);
  int fpass = 1;

  char location[201];
  char* lp = &location;
  char rlocation[201];
  char* rlp = &rlocation;

  while (nameWithoutCC(lp, 200, fp)) {

    if (strlen(lp) >200){printf("Beware: string buffers are full!");}

    strcpy(rlp, lp);

    if (!(fillInFileName(lp, fname) && fillInRefFileName(rlp, fname))){
      printf("problem with name lengths, skipping %s\n", lp);
      continue;
    }
    printf("histAdder hist name %s\n",lp);
    printf("histAdder ref hist name %s\n",rlp);
    
    if(fpass){
      fpass =0;

      hists  = getHists(lp);
      rhists = getHists(rlp);

      if(calcSize(hists) != calcSize(rhists)){
	printf("no of hists != in ref & new hist files, skipping %s\n",lp);
	fpass = 1;
      }

      if(calcSize(hists) == 0 ){
	printf("no of hists in files = 0, skipping %s\n",lp);
	fpass = 1;
      }
      
      //printf("histAdder: fpass %d\n",fpass);
    }else{
      if(!addHists(lp, hists, rlp, rhists)){
	printf("Problem accumulating hists %s\n",lp);
      }
    }
    for(int i=0; i<201; ++i){location[i]  = '\0';}
    for(int i=0; i<201; ++i){rlocation[i] = '\0';}
  }
  fclose(fp);

  writeHists(fname, hists);

  char refName[100]="refFile_";
  char* rfname = &refName;
  strcat(rfname, fname);

  writeHists(rfname, rhists);

  //int size = calcSize(hists);
  //printf("histAdder size: %d\n", size);
  hists->Delete();
  delete hists;
  return 0;
}

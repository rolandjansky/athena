//********************************
// Root macro to read in and compare histograms from a hist and ref hist file.
// Result is written to standard out which is caught by the Python popen object
// used to launch the ROOT child process that ran this macro.
//
// BS 12/04
//********************************

//=========================================================================

TObjArray* getHists(TFile * f){
  //printf("getHists: start\n");
  TObjArray* hists = new TObjArray(0);
  int size = hists->GetSize();
  //printf("getHists: size on creation: %d\n",size);  

  TIter next(f->GetListOfKeys());
  TKey *key;
  while ((key=(TKey*)next())) {
    char* name = key->GetName();
    //printf("getHists: obj name %s\n", name);
    TObject* obj = f->Get(name);
    hists->Add(obj);
  }  
  
  int size = calcSize(hists);
  //  printf("getHists: from %s found %d hists\n", fname, size);
  return hists;
}

//=========================================================================

int calcSize(TObjArray* col){
  //printf("calcSize: start:");
  int count = 0;
  TIter iter = col->MakeIterator();
  while(iter.Next()){++count;}
  return count;
}

//=========================================================================

bool binByBinComparison(hist1,hist2){
  Int_t nbins1 = hist1->GetNbinsX();
  Int_t nbins2 = hist2->GetNbinsX();
  if(nbins1 != nbins2){
    return false;
  }
  else{
    for(Int_t binNo=0;binNo < nbins1;binNo++){
      if(hist1->GetBinContent(binNo) != hist2->GetBinContent(binNo)){
	return false;
      }
    }
  }

  return true;
}

//=========================================================================

bool statisticalComparison(hist1,hist2){
  return true;
  //  return (hist1->KolmogorovTest(hist2) < probValue);
}

//=========================================================================

bool isSameHistogram(TH1* hist1, TH1* hist2, char * comparisonType){
  if (comparisonType=='bin-by-bin'){
    return binByBinComparison(hist1,hist2);
  }
  else{ //Kolmogorov test
    return statisticalComparison(hist1,hist2);
  }

  return true;
}

//=========================================================================

int cleanup(TObjArray * array1, TObjArray * array2, TFile * file1, TFile * file2){

  array1->Delete();
  delete array1;

  array2->Delete();
  delete array2;
  
  delete file1;
  delete file2;

  return 0;
}

//=========================================================================

char * toString(TObjArray * histTitles){
  TIter iter(histTitles);

  TString total;
  while( currStringObject = (TObjString*)iter.Next()){
    TString tString = currStringObject->GetString();
    total += tString->Data() + ", ";
  }

  return total->Data();
}

//=========================================================================

int RTT_histComparison(char* newHistFile, char* refHistsFile, char * compType){
  int successCode = 0;
  int errorCode = 1;

  //------------------------------------------------------------------
  // Open the 2 histogram files and extract the histograms into 2 lists
  //------------------------------------------------------------------
  TFile* hists = new TFile(newHistFile); 
  TFile* refHists = new TFile(refHistsFile); 
    
  listHists  = getHists(hists);
  listRhists = getHists(refHists);
  
  //------------------------------------------------------------------
  // Different number of histograms in the two histogram files -> EXIT
  //------------------------------------------------------------------
  if(calcSize(listHists) != calcSize(listRhists)){
    cleanup(listHists,listRhists,hists,refHists);
    cout << "RTT_histComparison.C return value: false info: the histogram files " << newHistFile << " and " << refHistsFile << " did not contain the same number of histograms" << endl;
    return errorCode;
  }

  //------------------------------------------------------------------
  // No histograms in the newly created histogram file -> EXIT
  //------------------------------------------------------------------
  if(calcSize(listHists) == 0 ){
    cleanup(listHists,listRhists,hists,refHists);
    cout << "RTT_histComparison.C return value: false info: the newly created histogram file" << newHistFile << " contains no histograms!" << endl;
    return errorCode;
  }
  
  //------------------------------------------------------------------
  // Preliminary checks over, now compare each histogram in the two files
  //------------------------------------------------------------------
  TIter new_iter(listHists);
  TIter ref_iter(listRhists);

  TObjArray* failed = new TObjArray(0);
  int sizeOnInit = failed->GetSize();

  while( newHist = (TH1*)new_iter.Next()){
    refHist = (TH1*)ref_iter.Next();
    if (!isSameHistogram(newHist,refHist,compType)){
      failed->Add(new TObjString(newHist->GetTitle()));
    }
  }

  //------------------------------------------------------------------
  // Some histograms were different -> EXIT
  //------------------------------------------------------------------
  if (failed->GetSize() > sizeOnInit){
    cout << "failed size: " << failed->GetSize() << endl;
    cout << "RTT_histComparison.C return value: false info: the following histograms were not identical in the two files " << newHistFile << " and " << refHistsFile << ": " << toString(failed) << endl;
    cleanup(listHists,listRhists,hists,refHists);
    failed->Delete();
    delete failed;
    return errorCode;
  }
  
  //------------------------------------------------------------------
  // All OK
  //------------------------------------------------------------------
  cout << "RTT_histComparison.C return value: true" << endl;
  return successCode;
}

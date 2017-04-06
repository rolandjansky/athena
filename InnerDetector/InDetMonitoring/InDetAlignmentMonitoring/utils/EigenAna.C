/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////
// EigenAna.C
//
// Analysis of the eigen values and vectors 
///////////////////////////////////////
#include <include/EigenAna.h>
/////////////////////////////////////////////////
void EigenAna(char dirname[80]="noname", char SolveOpt[80]="wholeID", Int_t NRemoveModes=-1)
{
  //gROOT->SetBatch();
  StatusCode status = SUCCESS;
  SetPrintLevel(1);

  SolveOptions = new TString(SolveOpt);
  HandleSolveOptions();
  if (QuietMode) SetPrintLevel(0);

  // 
  status = EigenAna_init(dirname); if (status == FAILURE) return;
  if (NRemoveModes>=0)   SetNModesToRemove(NRemoveModes); // only for strategy 1, fixed number of weak modes to remove
  //
  EigenAna_operate();
  EigenAna_end();

  return;
}

//////////////////////////////////////////////
StatusCode EigenAna_init(const char *dname)
{
  StatusCode status = SUCCESS;

  if (GetPrintLevel()>=1) {
    std::cout << std::endl;
    std::cout << "              +-------------------------+ " << std::endl;
    std::cout << "              |        EigenAna         | " << std::endl;
    std::cout << "              +-------------------------+ " << std::endl;
    std::cout << std::endl;
  }
  
  // timing
  TStopwatch *InitTime = new TStopwatch();  InitTime->Start();

  // init graphics style;
  InitGraphicsStyle();
    
  // setting the directory name where the big vector and the big files reside
  SetDirectory(dname);

  // Input From TFile
  SetHitCut(10);
  if (UseTFileInput) { 
    // ReadTFIle reads the bigmatrix, bigvector, hits and configuration
     if ( ReadTFile() == FAILURE) {
       return FAILURE;
     }
  }
  
  // Once the BigMatrix is read, then plot it as a 2D histogram
  // Init Canvas
  if (!QuietMode) OpenCanvas();

  // Set Ndof per module/structure
  SetActiveNdof(1,1,1,1,1,1);   // This may be used to swicth off some of the dofs from the input matrix

  // Set weak mode removal strategy
  SetWeakModeRemovalStrategy(1);

  SetNModesToRemove(0); // only for strategy 1, fixed number of weak modes to remove
  
  SetPullLimit(1.0); // for strategies 2 and above

  // Init output file
  InitOutputFile();

  // timing
  InitTime->Stop();
  if (GetPrintLevel()>=1) {
    std::cout << " <ea> Initialization time: ";
    InitTime->Print();
    std::cout << std::endl;
  }
  return status;
}

//////////////////////////////////////////////
void EigenAna_operate()
{
  // Now prepare the BigVector and BigMatrix according to the solve options
  BookBigMatrixHisto(); // plot matrix
  MatrixAndVectorPreConditioning();  
  
  // If required, print the matrix and vector that are used for 
  // the alignment corrections solving
  if (GetPrintLevel()>=5) {
    std::cout << " <ea> ---- OPERATE -------- with  BigMatrix: size " << BigMatrix.GetNrows() 
	 << " x  " << BigMatrix.GetNcols()  << " ------- " << std::endl; 
    BigMatrix.Print();
    std::cout << " <ea> ---- OPERATE -------- EO BigMatrix ------------ " << std::endl; 
    std::cout << std::endl;
    std::cout << " <ea> ---- OPERATE -------- with big vector of size " 
	 << BigVector.GetNoElements() << " --------- " << std::endl;
    BigVector.Print();
    std::cout << " <ea> ---- OPERATE -------- EO BigVector ------------ " << std::endl; 
    std::cout << std::endl;
  }

  
  if (DoSolving) {
    // Now solve by diagonalization
    SolveInDiagonalBase();
    
    // Now solve by diagonalization
    SolveWithWeakModeRemoval();    
  }
}

//////////////////////////////////////////////
void EigenAna_end()
{
  // save histograms
  EAHistDir->Write("0");
  //
  if (GetPrintLevel()>=1){
    std::cout << std::endl;
    std::cout << "                +-------------------------+ " << std::endl;
    std::cout << "                | Game Over. Insert coin  | " << std::endl;
    std::cout << "                +-------------------------+ " << std::endl;
  }
  return;
}


//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

StatusCode ReadTFile()
{
  StatusCode status=SUCCESS;

  // open the TFile

  if (GetPrintLevel()>=3) std::cout << " <ea> ReadTFile <ea> START " << std::endl;
  char fname[240];

  sprintf(fname,"%s%s",DirName->Data(),"/AlignmentTFile.root");
  if(NewAlign) sprintf(fname,"%s%s",DirName->Data(),"vector.bin");

  //////////////////////////////////////
  // open file
  //////////////////////////////////////
  InputTFileName = new TString(DirName->Data());
  if (!InputTFileName->Contains(".root")) {
    // if only the direcotry is given, it looks for the default filename: AlignmentTFile.root
    // otherwise, the file specified by the user is read.
    InputTFileName->Append("/AlignmentTFile.root"); 
  }

  InputTFile = new TFile(InputTFileName->Data(),"READ");
  std::cout << " <ea> reading " << fname << std::endl; 

  // first step -- read the scale factor
  if (GetPrintLevel()>=3) {
    std::cout << " <ea> reading ScaleFactor from " << fname << std::endl; 
  }  
  Double_t Scale=1.0;
  Scale = (*(TVectorD*)InputTFile->Get("Scale"))[0]; 
  SetScaleFactor(Scale);

  if (GetPrintLevel()>=1) {
    std::cout << std::endl << " <ea> ReadTFile <ea> Scale factor: size " << Scale << std::endl;
  }

  
  // second step -- read the big matrix
  TMatrixDSparse* InputBigMatrix;
  InputBigMatrix = (TMatrixDSparse*)InputTFile->Get("Matrix");
  if (GetPrintLevel()>9) {
    std::cout << " <ea> ------- printing InputBigMatrix: size " 
    << InputBigMatrix->GetNcols() << " x " << InputBigMatrix->GetNrows() << " ------- " << std::endl; 
    InputBigMatrix->Print();
    std::cout << " <ea> ------------ EO InputBigMatrix ------------ " << std::endl; 
    std::cout << std::endl;
  }


  // The diagonalization step needs to convert the input matris to a TMatrixDSym
  BigMatrix.ResizeTo(InputBigMatrix->GetNrows(),InputBigMatrix->GetNcols());
  BigMatrix.Zero();
  for (int ii=0;ii<InputBigMatrix->GetNcols();ii++) {
  	const TMatrixTSparseRow_const<double> myRow = (*InputBigMatrix)[ii];
	int i =  myRow.GetRowIndex();
	for (int jj=0;jj<=myRow.GetNindex()-1;jj++) {
		int j = (myRow.GetColPtr())[jj];
		const double  myElement= (myRow.GetDataPtr())[jj];
	    BigMatrix(i,j) = myElement;
	    BigMatrix(j,i) = myElement;
	}
  }       
	    

  // Apply scale factor
  BigMatrix *= 1./Scale;

  if (GetPrintLevel()>3) {
    std::cout << " <ea> ReadTFile <ea>   ------- printing BigMatrix (after normalization): size " 
	      << BigMatrix.GetNcols() << " x " << BigMatrix.GetNrows() << " ------- " << std::endl; 
    BigMatrix.Print();
    std::cout << " <ea> ------------ EO BigMatrix ------------ " << std::endl; 
    std::cout << std::endl;
  }
  
  
  // Third step -- read the big vector
  TVectorD* InputBigVector;
  InputBigVector = (TVectorD*)InputTFile->Get("Vector");
  if( !InputBigVector ){
    return FAILURE;	      
  }
  if (GetPrintLevel()>9) {
    std::cout << " <ea> ------- printing InputBigVector: size " 
	      << InputBigVector->GetNoElements() << " ------- " << std::endl; 
    InputBigVector->Print();
    std::cout << " <ea> ------------ EO InputBigVector ------------ " << std::endl; 
    std::cout << std::endl;
  }
  BigVector.ResizeTo(InputBigVector->GetNoElements());
  for (int i=0; i<InputBigVector->GetNoElements(); i++) {
    double element = 1;
    element = (*InputBigVector)[i]; 
    BigVector(i)= element;
  }

  // Apply scale factor
  BigVector *= 1./Scale;
 
  if (GetPrintLevel()>3) {
    std::cout << " <ea> ReadTFile <ea> ------- printing BigVector: size " 
	      << BigVector.GetNoElements() << " ------- " << std::endl; 
    BigVector.Print();
    std::cout << " <ea> ------------ EO BigVector ------------ " << std::endl; 
    std::cout << std::endl;
  }
  

  // fourth step -- Now read the structure identifiers
  TVectorD* InputIdentifier;
  InputIdentifier = (TVectorD*)InputTFile->Get("ModuleID");
  Identifiers.ResizeTo(InputBigVector->GetNoElements());
  for(int i=0; i<InputBigVector->GetNoElements(); i++) {
    Identifiers[i] = (*InputIdentifier)[i];
  }
  
  
  // fith step -- Now read the dof type
  TVectorD* InputDoFs;
  InputDoFs = (TVectorD*)InputTFile->Get("dof");
  double dof_tmp;
  if (InputDoFs != NULL) {
    DoFs.ResizeTo(InputBigVector->GetNoElements());
    for(int i=0; i<InputBigVector->GetNoElements(); i++) {
      dof_tmp = (*InputDoFs)[i];
      unsigned long long target;
      memcpy(&target, &dof_tmp, sizeof(target));
      unsigned long long dof = target;
      DoFs[i] = dof;
    }
  }
  
  
  // sixth step -- Read Hitmap
  TVectorD* InputHitmap;
  InputHitmap = (TVectorD*)InputTFile->Get("Hits");
  
  // Fill RowsUsed with 0s if row won't be used or with 1s if it will be used
  int tmp_i=0;
  RowsUsed.ResizeTo(InputBigVector->GetNoElements());
  for(int i=0; i<InputBigVector->GetNoElements(); i++) {
    RowsUsed[i]=1; // by default rows are used
    if((*InputHitmap)[tmp_i]<GetHitCut()) RowsUsed[i]=0;
    // trick to align IBL only if(i>5) RowsUsed[i]=0;
    if(i<(InputBigVector->GetNoElements()-1) && (*InputIdentifier)[i]!=(*InputIdentifier)[i+1]) tmp_i++;
  }
  if (GetPrintLevel()>0) {
   	std::cout << " Input Hitmap: " << std::endl;
   	InputHitmap->Print();
	std::cout << std::endl;
	std::cout << "Rows to be used using HitCut: "<< GetHitCut() << std::endl;
  	RowsUsed.Print();
  	std::cout << std::endl;
   }  
  
  // Remove  rows / columns to remove in case of low statistics
  if(GetHitCut()>0) RemoveRows();

  if (GetPrintLevel()>=1) {
    std::cout << " <ea> ReadTFile <ea> big matrix size: " <<  BigMatrix.GetNcols() << " x " << BigMatrix.GetNrows() << std::endl;
    std::cout << " <ea> ReadTFile <ea> big vector size: " <<  BigVector.GetNoElements() << std::endl;
  }

  
  //
  // Now, let's find the structure names
  // this is done in two stages
  // first find how many structures there are
  // second repeat and set the name of the structure
  // First pass
  double id;
  int nstruct = 0;
  unsigned long long id_tmp = 0;
  unsigned long long id_long = 0;

  for(int i=0; i<BigVector.GetNoElements(); i++){
    id = Identifiers[i];
    memcpy(&id_tmp, &id, sizeof(id_tmp));
    if(id_tmp != id_long) nstruct++; // a new structure is found
    id_long = id_tmp;
    //    std::cout << " Identifier " << id << std::endl;
    //std::cout << " Identifier l " << id_long << std::endl;
    //printf("Valor en binari per a %d: %32b\\n",i,id_long);
    //cout << i << "nstruct: " << nstruct << endl;
    FindDetectorType(id_long,nstruct-1,i);
  }
  SetNofStructures(nstruct);
  SetNofStructuresPerDetec();

  // second pass
  nstruct = 0; // reset
  id_tmp = 0;
  id_long = 0;
  for(int i=0; i<BigVector.GetNoElements(); i++){
    id = Identifiers[i];
    memcpy(&id_tmp, &id, sizeof(id_tmp));
    if(id_tmp != id_long) nstruct++;
    id_long = id_tmp;
    // std::cout << " <ea> Identifier " << id << " of element " << i << std::endl;
    // std::cout << " <ea> Identifier long " << id_long << std::endl;
    // printf(" << <ea> Valor en binari de l element %d: %64b \n", i, id_long);
    // std::cout <<  " std::bitset<64>(id_long) = " << std::bitset<64>(id_long) << endl;
    // cout << " <ea>  element " << i << " is in structure: " << nstruct << endl;
    ExtractName(id_long,nstruct-1);
  }

  //
  SetDoFTypes();

  // That's it !
  delete InputBigMatrix;
  delete InputBigVector;
  delete InputDoFs;
  
  if (GetPrintLevel()>=3) std::cout << " <ea> ReadTFile <ea>  COMPLETED " << std::endl;
  return status;
}


//////////////////////////////////////////////
void SetPrintLevel(int iLevel)
{
  if (iLevel<0) iLevel=0;
  if (iLevel>10) iLevel=10;
  PrintLevel = iLevel;
}
//////////////////////////////////////////////
Int_t GetPrintLevel()
{
  return PrintLevel;
}

//////////////////////////////////////////////
void HandleSolveOptions()
{
  SolveOptions->ToLower();
  if (GetPrintLevel()>=9) {
    std::cout << " <ea> Handling Solve options --> " << SolveOptions->Data() << std::endl << std::endl;
  }

  // solve
  SolveWholeID = SolveOptions->Contains("wholeid");
  UseSoftModeCut = SolveOptions->Contains("softmodecut");
  SolveLocalX2Style = SolveOptions->Contains("localx2");

  // input
  if (SolveOptions->Contains("newalign")){ // input uses the bigmatrix and bigvector binary files
    NewAlign = true;
    UseTFileInput = false;
  }  
  UseTFileInput = (SolveOptions->Contains("tfile") | UseTFileInput); // the input is from the AlignmentTFile.root  
  if (UseTFileInput) NewAlign = false; // 

  // special output options
  QuietMode = SolveOptions->Contains("quiet");
  PlotEigenVecs = SolveOptions->Contains("ploteigen");


  // check that at least one option is recognized
  Bool_t KnowWhatToDo = false;
  if (SolveWholeID)    KnowWhatToDo = true;

  // printing and input/output options
  Bool_t KnownOptions = false;
  if (UseSoftModeCut) KnownOptions = true;
  if (SolveLocalX2Style) KnownOptions = true;
  if (QuietMode)      KnownOptions = true;
  if (PlotEigenVecs)  KnownOptions = true;
  if (NewAlign)       KnownOptions = true;


  // doest it know what to do ?
  if (!KnowWhatToDo) {
    SolveWholeID = true;
    if (!QuietMode && KnownOptions) {
      std::cout << std::endl;
      std::cout << " <ea> Solving options given by user: '" << SolveOptions->Data()
	   << "' are UNKNOWN --> solve whole ID instead "
	   << std::endl << std::endl;
    }
  }
  return;
}

//////////////////////////////////////////////
void SetDirectory(const char *dname)
{
  DirName = new TString(dname);
  if (GetPrintLevel()>=1) 
    std::cout << " <ea> Loading files from directory " << DirName->Data() << std::endl;
  
  // check that directory exits
  Long_t id, size, flag, modtime;
  int status = gSystem->GetPathInfo(DirName->Data(),&id,&size,&flag,&modtime);
  
  // this has been superseeded so... use this dirty patch for the time being
  status = 0;  // this bit of code has to be cleaned up !!!

  // if status == 1 --> non existing
  if (status == 1) {
    std::cout << std::endl;
    std::cout << " <ea> ** SEVERE ERROR ** Directory " << DirName->Data() << " does not exit " << std::endl;
    std::cout << std::endl;
  }
}


//////////////////////////////////////////////
void SetWeakModeRemovalStrategy(Int_t strategy)
{
  Bool_t KnownStrategy = false;
  
  // No weak mode removal
  if (strategy == 0) KnownStrategy = true; 

  // Fixed number of modes removal
  if (strategy == 1) KnownStrategy = true; 

  // removing by pull
  if (strategy == 2) KnownStrategy = true; 

  // removing by pull, eigen value step, size of the correction;
  if (strategy == 3) KnownStrategy = true; 

  // User selection
  if (strategy == 4) KnownStrategy = true; 

  if (!KnownStrategy) {
    std::cout << std::endl;
    std::cout << " <ea> ** WARNING ** User requested strategy for weak mode removal "
	 << " (strategy = " << strategy << " ) "
	 << " is unknown. Use strategy 0 instead " 
	 << std::endl;
    strategy = 0;
  }

  WeakModeStrategy = strategy; 
}

//////////////////////////////////////////////
Int_t GetWeakModeRemovalStrategy()
{
  return WeakModeStrategy; 
}

//////////////////////////////////////////////
void SetScaleFactor(double myscale)
{
  m_ScaleFactor = myscale;
  return;
}

//////////////////////////////////////////////
Double_t GetScaleFactor()
{
  return m_ScaleFactor;
}

//////////////////////////////////////////////
void SetNModesToRemove(int nmodes)
{
  if (nmodes<0) nmodes = 6; // default case: 6 singular modes

  NModesToRemove = nmodes;
}

//////////////////////////////////////////////
Int_t GetNModesToRemove()
{
  return NModesToRemove;
}

//////////////////////////////////////////////
Int_t GetNStruct()
{
  return NofAlignableStructures;
}

//////////////////////////////////////////////
Int_t GetNMaxStructures()
{
  return NMaxStruct;
}

//////////////////////////////////////////////
void SetPullLimit(Float_t newPullLimit)
{
  if (newPullLimit<0) newPullLimit = 0;

  PullLimit = newPullLimit;
}

//////////////////////////////////////////////
Float_t GetPullLimit()
{
  return PullLimit;
}

//////////////////////////////////////////////
Bool_t IsRotation(int i)
{
  Bool_t AmIRotation = false;

  Short_t dof_type = DoFType[i];

  AmIRotation = (dof_type == DoFRx | dof_type==DoFRy | dof_type==DoFRz);

  return AmIRotation;
}

//////////////////////////////////////////////
void OpenCanvas()
{
  CanDiagonalBase = new TCanvas("diagonal base","EigenAna: diagonal base",1000,700);
  CanDiagonalBase->Divide(3,2);

  CanAlignCorr = new TCanvas("align corr","EigenAna: alignment corrections",1000,700);
  CanAlignCorr->Divide();

  CanAlignCorrErr = new TCanvas("align corr errors","EigenAna: alignment correction errors",1000,700);
  CanAlignCorrErr->Divide();

  CanBigMatrix = new TCanvas("big matrix","EigenAna: big matrix",700,700);
  CanBigMatrix->Divide();
 
  CanBigMatrixInv = new TCanvas("big matrix inv","EigenAna: big matrix inv",700,700);
  CanBigMatrixInv->Divide();
 
  CanCorrelMatrix = new TCanvas("correlation matrix","EigenAna: correlation matrix",700,700);
  CanCorrelMatrix->Divide();
 
  CAliCorrDof = new TCanvas("AliCorrByDof", "EigenAna: Alignment corrections by dof", 1200, 800);
  CAliCorrDof->Divide(3,2);

  return;
}

//////////////////////////////////////////////
void InitGraphicsStyle()
{
  // style
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetOptStat(10);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderMode(0);

  
  //gStyle->SetPadColor(kWhite);
  //gStyle->SetCanvasBorderMode(0);
  //gStyle->SetCanvasBorderSize(0);

  Float_t isize = 0.04;
  //Int_t font = 42;

  //gStyle->SetLabelFont(font,"X");
  //gStyle->SetLabelFont(font,"Y");
  //gStyle->SetLabelFont(font,"Z");
  //gStyle->SetLabelSize(isize,"x");
  //gStyle->SetLabelSize(isize,"y");
  //gStyle->SetLabelSize(isize-0.01,"z");
 
  //gStyle->SetTitleFont(font,"x");
  //gStyle->SetTitleFont(font,"y");
  //gStyle->SetTitleFont(font,"z");
  gStyle->SetTitleSize(isize,"x");  
  gStyle->SetTitleSize(isize,"y");
  //gStyle->SetTitleSize(isize,"z");
  gStyle->SetTitleYOffset(1.3);


  //gStyle->SetStatFont(font);
  //gStyle->SetStatFontSize(isize);
  //gStyle->SetStatW(0.22);
  //gStyle->SetStatColor(kWhite);
  //gStyle->SetStatBorderSize(1);   // remove shade from stat box

  //gStyle->SetStripDecimals(false);
  //TGaxis::SetMaxDigits(4);

  

  return;
}

//////////////////////////////////////////////
void  InitOutputFile()
{
  Char_t filename[350];

  sprintf(filename,"EigenAnaOutput.root");

  if (GetPrintLevel()>=0) std::cout << " <ea> ** InitOutputFile ** Creating TDirectories" << std::endl;
  EAHistFile = new TFile(filename,"RECREATE");
  EAHistDir = new TDirectoryFile("EigenAna","EigenAna");
  EAHistDir->cd();
}

//////////////////////////////////////////////
void BookBigMatrixHisto()
{
  TH2F* hBigMatrix = new TH2F("hBigMatrix","BigMatrix",
			      BigMatrix.GetNcols(), -0.5, BigMatrix.GetNcols()-0.5,
			      BigMatrix.GetNrows(), -(BigMatrix.GetNrows()-0.5), 0.5);
  // change the values in order to make the zeros more visible
  for (int i = 0; i< BigMatrix.GetNrows(); i++) {
    for (int j = 0; j< BigMatrix.GetNcols(); j++) {
      double value = BigMatrix(i,j);
      if (value == 0) hBigMatrix->Fill(j,-i, 0.);
      if (value > 0.) hBigMatrix->Fill(j,-i, 1.);
      if (value < 0.) hBigMatrix->Fill(j,-i,-1.);      
    }
  }

  // set the axis labels
  SetHMatrixLabels(hBigMatrix);

  if (!QuietMode) {
    CanBigMatrix->cd(1);
    hBigMatrix->Draw("colz");
    DrawStructLinesOnHMatrix(hBigMatrix);
  }
}

//////////////////////////////////////////////
void BookBigMatrixInvHisto()
{
  hBigMatrixInv = new TH2F("hBigMatrixInv","Inverse of Big Matrix (M^{  -1})",
			   BigMatrix_Inv.GetNcols(), -0.5, BigMatrix_Inv.GetNcols()-0.5,
			   BigMatrix_Inv.GetNrows(), -(BigMatrix_Inv.GetNrows()-0.5), 0.5);

  if (BigMatrix_Inv.GetNrows()>0) { // if matrix has been computed
    float MaxVal = -99999.;
    float MinVal =  99999.;
    for (int i = 0; i< BigMatrix_Inv.GetNrows(); i++) {
      for (int j = 0; j< BigMatrix_Inv.GetNcols(); j++) {
	double value = BigMatrix_Inv(i,j);
	hBigMatrixInv->Fill(j,-i,value);
	if (value>MaxVal) MaxVal = value;
	if (value<MinVal) MinVal = value;
      }
    }
    
    float FullRange = MaxVal;
    if (TMath::Abs(MinVal) > FullRange) FullRange = TMath::Abs(MinVal); 
    hBigMatrixInv->SetMaximum(FullRange); 
    hBigMatrixInv->SetMinimum(-FullRange); 

    // set the axis labels
    SetHMatrixLabels(hBigMatrixInv);

    if (!QuietMode) {
      CanBigMatrixInv->cd(1);
      hBigMatrixInv->Draw("colz");
      DrawStructLinesOnHMatrix(hBigMatrixInv);
    }
  }
}

//////////////////////////////////////////////
void BookCorrelMatrixHisto()
{
  TH2F* hCorrelMatrix = new TH2F("hCorrelMatrix","Correlation matrix",
			      BigMatrix_Inv.GetNcols(), -0.5, BigMatrix_Inv.GetNcols()-0.5,
			      BigMatrix_Inv.GetNrows(), -(BigMatrix_Inv.GetNrows()-0.5), 0.5);

  if (BigMatrix_Inv.GetNrows()>0) { // if matrix has been computed
    float MaxVal = -99999.;
    float MinVal =  99999.;
    for (int i = 0; i< BigMatrix_Inv.GetNrows(); i++) {
      for (int j = 0; j< BigMatrix_Inv.GetNcols(); j++) {
	double value = BigMatrix_Inv(i,j) / sqrt(BigMatrix_Inv(i,i)*BigMatrix_Inv(j,j));;
	if (BigMatrix_Inv(i,j) == 0) value = 0; 
	if (BigMatrix(i,j) == 0) value = 0; 
	if (fabs(BigMatrix_Inv(i,j)) < ZeroEquivalent) value = 0; 
	if (fabs(BigMatrix_Inv(i,j)) < (1./(10*BigMatrix_Inv.GetNrows()))) value = 0; 
	if (false) {
	  if (fabs(BigMatrix_Inv(i,j)) < 1.0E-6) value = 0; 
	} 
	hCorrelMatrix->Fill(j,-i,value);
	if (value>MaxVal) MaxVal = value;
	if (value<MinVal) MinVal = value;
      }
    }
      
    hCorrelMatrix->SetMaximum(1); 
    hCorrelMatrix->SetMinimum(-1); 

    // set the axis labels
    SetHMatrixLabels(hCorrelMatrix);

    if (!QuietMode) {
      CanCorrelMatrix->cd(1);
      hCorrelMatrix->Draw("colz");
      hCorrelMatrix->GetZaxis()->SetLabelSize(0.02);
      DrawStructLinesOnHMatrix(hCorrelMatrix);
    }
  }
}

//////////////////////////////////////////////
void BookEigenValues()
{
  // EigenValues histogram
  hEigenVal = new TH1F ("hEigenVal","Eigen values",
			BigVector.GetNoElements(), 
			0.5, 
			BigVector.GetNoElements()+0.5);
  hEigenVal->SetXTitle("eigenvalue");

  if (GetPrintLevel()>=1) 
    std::cout << " <ea> Defining histogram of eigen values with " <<  hEigenVal->GetNbinsX() << " bins " << std::endl;
  // Storing eigen values in histogram bins
  for (int i=0; i<BigVector.GetNoElements(); i++) {
    hEigenVal->SetBinContent(i+1,EigenVal(i));
  }
  if (!QuietMode) {
    CanDiagonalBase->cd(1);
    hEigenVal->Draw();
    gPad->SetLogy(1);
    gPad->SetGridx(1);
  }
  return;
}

//////////////////////////////////////////////
void BookBigVector()
{
  // store the big vector in a histogram
  hBigVector = new TH1F ("hBigVector","Big vector",
			    BigVector.GetNoElements(), 
			    0.5, 
			    BigVector.GetNoElements()+0.5);
  hBigVector->SetXTitle("dof");
  for (int i=0; i<BigVector.GetNoElements(); i++) {
    hBigVector->SetBinContent(i+1,BigVector(i));
  }

  // keep it in a histogram
  hBigVector_b = new TH1F ("hBigVector_b","Residuals vector in diagonal base",
			    BigVector.GetNoElements(), 
			    0.5, 
			    BigVector.GetNoElements()+0.5);
  hBigVector_b->SetXTitle("mode");
  for (int i=0; i<BigVector_b.GetNoElements(); i++) {
    hBigVector_b->SetBinContent(i+1,BigVector_b(i));
  }
  if (!QuietMode) {
    CanDiagonalBase->cd(3);
    hBigVector_b->Draw();
  }

  return;
}

//////////////////////////////////////////////
void BookRawAlignCorr()
{
  // Book a histogram for alignment corrections in the modules base using all modes (no weak mode removal yet)
  if (GetPrintLevel()>=5) std::cout << " <ea> -- SolveInDiagonalBase -- booking histogram for alignment corrections in modules base " << std::endl;
  hAliCorrRaw = new TH1F ("hdaAliCorrRaw","Alignment corrections",
			  BigVector.GetNoElements(), 
			  0.5, 
			  BigVector.GetNoElements()+0.5);
  hAliCorrRaw->SetXTitle("dof");
  for (int i=0; i<AlignCorrRaw.GetNoElements(); i++) {
    hAliCorrRaw->SetBinContent(i+1,AlignCorrRaw(i));
  }
  SetXAxisLabels(hAliCorrRaw);
  if (GetPrintLevel()>=5) std::cout << " <ea> -- SolveInDiagonalBase -- Alignment corrections histogram successfully booked and filled -> going to plot" << std::endl;
  if (!QuietMode) {
    CanAlignCorr->cd(1);
    hAliCorrRaw->Draw();
    DrawStructLines(hAliCorrRaw,CanAlignCorr,1);
  }


  // Histogram of alignment corrections in diagonal base, all modes included (no weak mode removal yet)
  hAliCorrRaw_b = new TH1F ("hdbAliCorrRaw","#delta b  (align. corr. in diag. base, all modes included)",
			    BigVector.GetNoElements(), 
			    0.5, 
			    BigVector.GetNoElements()+0.5);
  hAliCorrRaw_b->SetXTitle("mode");
  if (GetPrintLevel()>=5) 
    std::cout << " <ea> Defining histogram of raw alignment corrections in diagonal base with  " 
	 <<  hAliCorrRaw_b->GetNbinsX() << " bins " << std::endl;
  
  // strore alignment corrections in the daigonal base in the ad-hochistogram
  for (int i=0; i<AlignCorrRaw_b.GetNoElements(); i++) {
    hAliCorrRaw_b->SetBinContent(i+1,AlignCorrRaw_b(i));
  }
  
  if (!QuietMode) {
    CanDiagonalBase->cd(2);
    hAliCorrRaw_b->Draw();
  }
  if (GetPrintLevel()>=5) std::cout << " <ea> -- SolveInDiagonalBase -- Histogram of raw alignment correction in base b successcully booked ! " << std::endl;

  return;
}

//////////////////////////////////////////////
void MatrixAndVectorPreConditioning()
{
  // this function takes the input matrix and vector and transfer them 
  // into the BigVector and BigMatrix used for solving. The conditioning depends
  // on the user selected options (Level2->Level1, sct only, etc...)
  
  if (SolveWholeID) {
    // there is nothing to do...
  }
  
  DetectNullEigenValues();

  // Sove a la LocalX2
  if (SolveLocalX2Style) PrepareLocalX2(); 

  // scale angle units from rad to mrad (or whatever scale is used)
  if ((AngleUnitFactor != 1.)
      && (UseRx || UseRy || UseRz) // and a rotational dof is used 
      ) ChangeAngleUnits();

  // Perform actions to enable the sofmodecut
  if (UseSoftModeCut) PrepareSoftModeCut(); 
 
  if (GetPrintLevel()>=1) std::cout << " <ea> -- PreConditioning -- completed " << std::endl;
  //
  return;
}

//////////////////////////////////////////////
void PrepareLocalX2()
{
  // this function prepares the solving a la local X2
  // It makes the matrix block diagonal. No action is needed on the big vector
  // No eigenvalues need to be removed
  
  if (GetPrintLevel()>=1) std::cout << " <ea> ** PrepareLocalX2 ** Start " << std::endl;
  //
  SetWeakModeRemovalStrategy(1);
  SetNModesToRemove(0);   

  // The matrix has to be block diagonal in blocs on Ndof x Ndof (where Ndof is DOF per structure)
  for (int i=0; i < BigMatrix.GetNrows(); i++) {
    for (int j=0; j < BigMatrix.GetNcols(); j++) {
      if (GetStructID(i) != GetStructID(j)) { // if Dof correspond to different structures
	BigMatrix(i,j) = 0.;
      }
    }
  }

  //
  if (GetPrintLevel()>=1) std::cout << " <ea> ** PrepareLocalX2 ** Completed " << std::endl;
  return;
}

//////////////////////////////////////////////
Int_t GetStructID(Int_t DofID)
{
  // given an identifier of a DOF it returns the structure to which it belongs

  //Int_t StructID = DofID/GetNdofPerStruct();
  //Int_t StructID = DofID/6;
  Int_t StructID = DofBelongsToStruct[DofID];

  return StructID;
}

//////////////////////////////////////////////
void PrepareSoftModeCut()
{
  // this function prepares the soft mode cut. It affects the big matrix before the diagonalization.
  // It has to add a term to the diagonal elements of the big matrix that accounts for the limit of the 
  // modules movements: translations and rotations
  
  if (GetPrintLevel()>=1) std::cout << " <ea> ** PrepareSoftModeCut ** Start " << std::endl;
  //
  
  TVectorD sofmodecutVec(BigMatrix.GetNrows());
  std::cout << "Bigmatrix before softmodecut" << std::endl;
  std::cout << "98x98:" << BigMatrix[98][98] << std::endl;

  for(int i =0; i <BigMatrix.GetNrows(); i++){
    
  	TString tmpname = StructureName[DofBelongsToStruct[i]]->Data();
	std::cout << sct_ec_Rz_smc << std::endl;
  	if(tmpname.Contains("SCT")==1){
  		if(tmpname.Contains("EC")==1){
		  if(DoFType[i]==5)
		    if(sct_ec_Rz_smc!=0.)
		      sofmodecutVec[i] = 2./(sct_ec_Rz_smc*sct_ec_Rz_smc);
		}
	}
  }
  std::cout << GetScaleFactor () << std::endl;
  for(int i =0; i <BigMatrix.GetNrows(); i++){
    BigMatrix[i][i]+=(sofmodecutVec[i]/GetScaleFactor());
  }
  std::cout << "Bigmatrix after softmodecut" << std::endl;
  std::cout << "98x98:" << BigMatrix[98][98] << std::endl;
  if (GetPrintLevel()>=1) std::cout << " <ea> ** PrepareSoftModeCut ** Completed " << std::endl;
  return;
}

//////////////////////////////////////////////
void SolveInDiagonalBase()
{
  ///////////////////////////////////////
  // Eigen values and vectors analisys
  ///////////////////////////////////////
  TStopwatch *DiagTime = new TStopwatch();  DiagTime->Start();
  
  if (GetPrintLevel()>=1) std::cout << " <ea> -- SolveInDiagonalBase -- Starting ... " << std::endl;
  
  // The following lines of code deal with the diagonalization and extracting the eigen values and eigen
  // vectors. This is needed if one reads the raw big matrix and big vectors. The proccess can be speed up
  // if the eigen values and eigen vectors from a previous solving are read as input. In the later case the 
  // diagonalization step can be avoided
  if (GetPrintLevel()>=3) 
    std::cout << " <ea> ** SolveInDiagonalBase ** Diagonilizing BigMatrix... " << std::endl; std::cout << std::flush;
  TMatrixDEigen EigenMatrixBase = TMatrixDEigen(BigMatrix); 
  if (GetPrintLevel()>=3) 
    std::cout << " <ea> ** SolveInDiagonalBase ** Diagonilization completed " << std::endl;  
  
  // - So... first extract the matrix that changes the base from a (structures) to b (diagonal)
  // or the eigen vectors matrix
  ChangeOfBaseMatrix.ResizeTo(BigMatrix.GetNrows(),BigMatrix.GetNcols());
  ChangeOfBaseMatrix = EigenMatrixBase.GetEigenVectors();
  if (GetPrintLevel()>=5) {
    std::cout << " ++ <ea> ----------- Change of Base matrix ++ " << std::endl;
    ChangeOfBaseMatrix.Print();
    std::cout << " ++ <ea> ----- EO -- Change of Base Matrix ++ " << std::endl;
  }
  
  // Once the matrix has been diagonalized, first thing to do, before any operation is
  // to extract the eigenvalues and the eigen vectors (change of base matrix)
  EigenVal.ResizeTo(BigVector.GetNoElements());
  EigenVal = EigenMatrixBase.GetEigenValuesRe();
  if (GetPrintLevel()>=5) {
    std::cout << " <ea> ----- Diagonalization fo the big matrix. Printing eigen values ----- " << std::endl;    
    EigenVal.Print();
    std::cout << " <ea> ----- EO Eigen Values ----- " << std::endl;
    std::cout << std::endl;
  }
  
  // Flip the matrix to link it with increasing order eigen values
  // ROOT returns the eigen values in decreasing order... we want 
  // them in increasing order... so flip the vector and the matrix
  // flip eigen value vactor
  EigenVal = Flip(EigenVal);
  if (GetPrintLevel()>=1) {
    std::cout << " <ea> ----- Diagonalization fo the big matrix. Printing eigen values after flippling ----- " << std::endl;    
    EigenVal.Print();
    std::cout << " <ea> ----- EO Eigen Values after flipping ----- " << std::endl;
    std::cout << std::endl;
  }
  BookEigenValues(); // book and draw eigen values histogram

  // flip eigen matrix
  ChangeOfBaseMatrix = Flip(ChangeOfBaseMatrix);
  if (GetPrintLevel()>=5) {
    std::cout << std::endl << " ++ <ea> -------- Change of base matrix after flipping ++ " << std::endl;
    ChangeOfBaseMatrix.Print();
    std::cout << std::endl << " ++ <ea> -- EO -- Change of base matrix after flipping ++ " << std::endl;
  }  
  
  // the transposed of the change of base matrix is also needed
  TMatrixD ChangeOfBaseMatrix_T(TMatrixD::kTransposed, ChangeOfBaseMatrix); 
  if (GetPrintLevel()>=7) {
    std::cout << std::endl << " ++ <ea> Change of base matrix transposed ++ " << std::endl;
    ChangeOfBaseMatrix_T.Print();
  }

  /////////////////////////////////////////////////////
  // Big matrix in the diagonal base and its inverse
  /////////////////////////////////////////////////////
  if (GetPrintLevel()>=8) 
    std::cout << " ++ <ea> Inverting big matrix in diagonal base M_b^-1 ++ " << std::endl;
  BigMatrix_b.ResizeTo(BigMatrix.GetNrows(),BigMatrix.GetNcols());  
  BigMatrix_b.Zero();
  
  TMatrixD BigMatrix_b_Inv;
  BigMatrix_b_Inv.ResizeTo(BigMatrix.GetNrows(),BigMatrix.GetNcols()); 
  BigMatrix_b_Inv.Zero();

  for (int i=0; i<EigenVal.GetNoElements(); i++) {
    BigMatrix_b(i,i) = EigenVal(i);  // just in case it has been flipped
    BigMatrix_b_Inv(i,i) = 1./EigenVal(i);   // The inversion of the diagonal matrix is obvious:
  }
  if (GetPrintLevel()>=8) {
    std::cout << " ++ <ea> ----------- The big matrix in the diagonal base M_b ++ " << std::endl;
    BigMatrix_b.Print();
    std::cout << " ++ <ea> ----- EO -- The big matrix in the diagonal base M_b ++ " << std::endl;

    std::cout << " ++ <ea> ----------- Inverse of the big matrix in the diagonal base (M_b^-1) ++ " << std::endl;
    BigMatrix_b_Inv.Print();
    std::cout << " ++ <ea> ----- EO -- Inverse of the big matrix in the diagonal base (M_b^-1) ++ " << std::endl;
  }


  // Before extracting the alignment corrections one needs to compute the big vector in
  // the diagonale base:
  
  // Now... compute the bigvector in the new base (nu_b)
  BigVector_b.ResizeTo(BigVector.GetNoElements());
  BigVector_b = ChangeOfBaseMatrix_T*BigVector;
  if (GetPrintLevel()>=5) {
    std::cout << " ++ <ea> -------------- BigVector in diagonal base (b) -------- " << std::endl;
    BigVector_b.Print();
    std::cout << " ++ <ea> ----- EO ----- BigVector in diagonal base (b) -------- " << std::endl;
  }

  BookBigVector();

  //
  // Fianlly compute the alignment corrections in the diagonal base (b)
  // this is without any mode cut at all.
  if (GetPrintLevel()>=5) std::cout << " <ea> -- SolveInDiagonalBase -- Computing alignment corrections in the diagonal base " << std::endl;
  AlignCorrRaw_b.ResizeTo(BigVector.GetNoElements());
  AlignCorrRaw_b = BigMatrix_b_Inv*BigVector_b;
  AlignCorrRaw_b *= -1;
  if (GetPrintLevel()>=5) {
    std::cout << " ++ <ea> ---------------- Raw Alignment corrections from matrix diagonalization (all modes included, diagonal base) ----- " << std::endl;
    //PrintAlignmentCorrections(db_AlignCorrRaw);
    AlignCorrRaw_b.Print();
    std::cout << " ++ <ea> ------ EO ------ Raw Alignment corrections from matrix diagonalization (all modes included, diagonal base) ----- " << std::endl;
  }


  ////////////////////////////////
  // Alignment correction
  ////////////////////////////////
  // Last but not least: compute alignment corrections in the physical modules base 
  if (GetPrintLevel()>=5) std::cout << " <ea> -- SolveInDiagonalBase -- computing alignment corrections in modules base " << std::endl;
  AlignCorrRaw.ResizeTo(BigVector.GetNoElements());
  AlignCorrRaw = ChangeOfBaseMatrix*AlignCorrRaw_b;
  if (GetPrintLevel()>=5) {
    std::cout << " ++ <ea> --------------- Alignment corrections from matrix diagonalization (all modes included)" << std::endl;
    //PrintAlignmentCorrections(AlignCorrRaw);
    AlignCorrRaw.Print();
    std::cout << " ++ <ea> ------ EO ----- Alignment corrections from matrix diagonalization (all modes included)" << std::endl;
  }

  BookRawAlignCorr();

  // So far mark all modes as used
  InitUsedEigenModesFlag();

  ///////////
  // timing
  DiagTime->Stop();
  if (GetPrintLevel()>=1) {
    std::cout << " <ea> Diagonalization solving time: ";
    DiagTime->Print();
    std::cout << std::endl;
  }

  if (GetPrintLevel()>=1) std::cout << " <ea> -- SolveInDiagonalBase -- Completed " << std::endl;

  return;
}

//////////////////////////////////////////////
void SolveWithWeakModeRemoval()
{
  ///////////////////////////////////////
  // Remove weak modes
  ///////////////////////////////////////

  if (GetPrintLevel()>=1) std::cout << " <ea> -- SolveWithWeakModeRemoval -- Starting... " << std::endl;

  //
  // set vectors size
  AlignCorr.ResizeTo(BigVector.GetNoElements());
  AlignCorr_b.ResizeTo(BigVector.GetNoElements());
  AlignCorrError_b.ResizeTo(BigVector.GetNoElements());

  // and filled them with the raw values
  AlignCorr = AlignCorrRaw;
  AlignCorr_b = AlignCorrRaw_b;

  // get weak mode removal strategy
  Int_t Strategy = GetWeakModeRemovalStrategy();
  Int_t NEigenRemoved = 0;  // so far ... no modes removed

  //----
  // No weak modes removal
  if (Strategy == 0) {
    // nothing to do
    if (GetPrintLevel()>=1) {
      std::cout << " <ea> -- Weak mode removal -- user wants to keep all modes. None removed " <<std::endl << std::endl;
    }
  }

  //----
  // fixed number of modes removed
  if (Strategy == 1) {
    int ModesToRemove = GetNModesToRemove();
    int LastModeToRemove = ModesToRemove;
    for (int mode=0; mode < LastModeToRemove; mode++) {
      NEigenRemoved++;
      AlignCorr_b(mode) = 0;
      AlignCorrError_b(mode) = 0;
      SetFlagForUsedEigenMode(mode, kFALSE);
      if (GetPrintLevel()>=1) {
	std::cout << " <ea> -- Weak mode removal -- user wants to remove mode " << mode << std::endl;
      }
    }
  }  

  //----
  // removing weak modes because users wants to remove them
  if (Strategy == 4) {
    SetFlagForUsedEigenMode(0, kFALSE);
    SetFlagForUsedEigenMode(1, kFALSE);
    SetFlagForUsedEigenMode(2, kFALSE);
    SetFlagForUsedEigenMode(3, kFALSE);
    //SetFlagForUsedEigenMode(4, kFALSE);
    //SetFlagForUsedEigenMode(5, kFALSE);
    SetFlagForUsedEigenMode(6, kFALSE);
    SetFlagForUsedEigenMode(7, kFALSE);
    //    SetFlagForUsedEigenMode(106, kFALSE);
    //    SetFlagForUsedEigenMode(166, kFALSE);
  }


  /////////////////////////////////////////////////////////////////
  // Now once the weak modes have been removed... carry on with the solving
  /////////////////////////////////////////////////////////////////
  if (GetPrintLevel()>=5) {
    std::cout << " <ea> -- Weak mode removal COMPLETED -- strategy " << Strategy << " removes " << NEigenRemoved << " modes " << std::endl << std::endl;
    std::cout << " <ea> ------ Alignment corrections in diagonal base after weak mode removal ------" << std::endl;
    PrintAlignmentCorrections(AlignCorr_b);
    std::cout << " <ea> ------ EO Alignment corrections in diagonal base after WMremoval ------" << std::endl;
  }

  // Once the weak modes have been removed according to the chosen strategy
  // compute again tha alignment corrections in the modules base
  ComputeAlignCorr(); 

  // Alignment correction errors
  ComputeAlignCorrErrors();

  // Histogram booking for the alignment corrections, errors, covariance matrix and mode corrections
  BookAliCorrHistos();
  BookAliCorrErrHistos();
  BookAliCorrPullHistos();
  BookBigMatrixInvHisto();
  BookCorrelMatrixHisto();
  BookModeCorrections();
  BookDofCorrections();

  if (GetPrintLevel()>=1) std::cout << " <ea> -- SolveWithWeakModeRemoval -- Completed... " << std::endl;


  return;
}

//////////////////////////////////////////////
void ComputeAlignCorr() 
{
  ////////////////////////////////
  // Alignment correction 
  ////////////////////////////////
  if (GetPrintLevel()>=7) std::cout << " <ea> ** ComputeAlignCorr **  Start. " << std::endl;

  // just make sure that rejected modes are not in
  for (Int_t imode=0; imode < AlignCorr_b.GetNoElements(); imode++) { 
    if (!IsEigenModeUsed(imode)) {
      AlignCorr_b(imode) = 0;
      AlignCorrError_b(imode) = 0;
    }
  }

  AlignCorr = ChangeOfBaseMatrix*AlignCorr_b;

  if (GetPrintLevel()>=1) {
    std::cout << " <ea> ------ Alignment corrections in the modules base after weak mode removal ------" << std::endl;
    PrintAlignmentCorrections(AlignCorr);
    std::cout << " <ea> ------ EO Alignment corrections in the modules base after WMremoval ------" << std::endl;
  }

  if (GetPrintLevel()>=7) std::cout << " <ea> ** ComputeAlignCorr **  Completed " << std::endl;
  return;
}

//////////////////////////////////////////////
void ComputeAlignCorrErrors() 
{
  ////////////////////////////////
  // Alignment correction errors
  ////////////////////////////////
  if (GetPrintLevel()>=1) std::cout << std::endl <<" <ea> computing alignment correction errors. Start. " << std::endl;
  
  // first compute the alignment correction errors in the diagonal base
  // these are the 1/sqrt(eigenval)
  AlignCorrError_b.ResizeTo(BigVector.GetNoElements());
  AlignCorrError_b = EigenVal;
  AlignCorrError_b *= 2; // to get the right size of the errors
  AlignCorrError_b *= GetScaleFactor();
  AlignCorrError_b.Abs(); // just in case
  AlignCorrError_b.Invert();
  AlignCorrError_b.Sqrt();  // Now all the errors are computed but.... lets switch off those not used
  for (Int_t imode=0; imode < AlignCorr_b.GetNoElements(); imode++) { 
    if (!IsEigenModeUsed(imode)) {
      AlignCorrError_b(imode) = 0;
    }
  }
  if (GetPrintLevel()>=7) {
    std::cout << " ++ <ea> Alignment corrections errors in diagonal base base" << std::endl;
    PrintAlignmentCorrections(AlignCorrError_b);
    std::cout << std::endl;
  }

  //////////////////////////////////////////////
  // Now compute the errors in the modules base
  //////////////////////////////////////////////

  // rough calculation which is not fully correct
  AlignCorrError.ResizeTo(BigVector.GetNoElements());
  AlignCorrError = ChangeOfBaseMatrix*AlignCorrError_b;
  
  // correct way to compute the errors
  if (1==1) {
    if (GetPrintLevel()>=1) std::cout << " <ea> computing alignment correction errors with correct method... " << std::endl;
    // The above calculation is just wrong
    // one needs to compute the inverse matrix: 

    // but before that let's compute all the relevant quantities as
    // M^-1 = U D^-1 U+
    
    //TMatrixD BigMatrix_b_Inv = BigMatrix_b;
    TMatrixDSym BigMatrix_b_Inv;
    BigMatrix_b_Inv.ResizeTo(EigenVal.GetNoElements(),EigenVal.GetNoElements());
    BigMatrix_b_Inv.Zero();

    for (int i=0; i<EigenVal.GetNoElements(); i++) {
      if (IsEigenModeUsed(i)) { 
      	BigMatrix_b_Inv(i,i) = 1./EigenVal(i);  
      }  
    }
    if (GetPrintLevel()>=9) {
      std::cout << " <ea> -- START --- BigMatrix_b^-1 -----------------------------" << std::endl;
      BigMatrix_b_Inv.Print();
      std::cout << " <ea> --  END  --- BigMatrix_b^-1 -----------------------------" << std::endl;
    } 
 
    BigMatrix_Inv.ResizeTo(BigMatrix.GetNrows(),BigMatrix.GetNcols());
    BigMatrix_Inv.Zero();
    if (GetPrintLevel()>=7) std::cout << " <ea> -- multipliying matrices M_inv =  U * Mb_inv * U^T)  --" << std::endl;
   
    BigMatrix_Inv=BigMatrix_b_Inv.Similarity(ChangeOfBaseMatrix);
    
    if (GetPrintLevel()>=8) {
      std::cout << " <ea> -- START --- BigMatrix^-1 -----------------------------" << std::endl;
      BigMatrix_Inv.Print();
      std::cout << " <ea> --  END  --- BigMatrix^-1 -----------------------------" << std::endl;
    }  

    for (Int_t i=0; i < AlignCorr.GetNoElements(); i++) {
      AlignCorrError(i) = sqrt(2*BigMatrix_Inv(i,i)/GetScaleFactor());
    }
  }// end of correct method

  if (GetPrintLevel()>=1) {
    std::cout << " ++ <ea> Alignment corrections errors in modules base" << std::endl;
    PrintAlignmentCorrections(AlignCorrError);
  }

  if (GetPrintLevel()>=1) std::cout << " <ea> computing alignment correction errors. Completed. " << std::endl;
  return;
}

//////////////////////////////////////////////
TVectorD Flip(const TVectorD &inVec)
{
  if (GetPrintLevel()>=5) 
    std::cout << " <ea>  flipping vector of size " << inVec.GetNoElements() << std::endl;
  
  TVectorD AuxVec(inVec);
    
  for (int i=0, j=AuxVec.GetNoElements()-1; i<AuxVec.GetNoElements(); i++, j--) {
    AuxVec(j) = inVec(i);
  }

  return AuxVec;
}

//////////////////////////////////////////////
TMatrixD Flip(const TMatrixD &inMat)
{
  if (GetPrintLevel()>=5) 
    std::cout << " <ea>  flipping matrix of size " 
	 << inMat.GetNrows() << " x " << inMat.GetNcols() << std::endl;
  
  TMatrixD AuxMat(inMat);

  int k;
  // columns
  for (int i=0;  i<inMat.GetNrows(); i++) {
    for (int j=0; j<inMat.GetNrows(); j++) {
      k = (inMat.GetNrows()-1) - j;
      AuxMat(i,k) = inMat(i,j);
    }
  }

  return AuxMat;
}

//////////////////////////////////////////////
void InitUsedEigenModesFlag()
{
  // At initilization mark them all as used
  for (Int_t i=0; i<BigVector.GetNoElements(); i++) {
    UsedEigenMode[i] = kTRUE;
  }
}

//////////////////////////////////////////////
void SetFlagForUsedEigenMode(Int_t ix, Bool_t EVused)
{
  UsedEigenMode[ix] = EVused;
  return;
}

//////////////////////////////////////////////
Int_t GetNUsedDof()
{
  Int_t n=0;
  for (Int_t i=0; i<GetNdof(); i++) {
    if (IsEigenModeUsed(i)) n++;
  }
  return n;
}

//////////////////////////////////////////////
Bool_t IsEigenModeUsed(Int_t ix)
{
  return UsedEigenMode[ix];
}

//////////////////////////////////////////////
void SetNofStructures(int nstruct)
{
  NofAlignableStructures = nstruct;

  return;
}

//////////////////////////////////////////////
Int_t GetNdofPerStruct()
{
  return NdofPerStruct;
}

//////////////////////////////////////////////
Int_t GetNEigenPerCanvas()
{
  Int_t n = AlignCorr.GetNoElements();

  //if (n>4) n=4;
  if (n>6) n=6;

  return n;
}

//////////////////////////////////////////////
Int_t GetNEigenCanvas()
{
  // to define how many canvas are needed
  Int_t Ncanvas = 1;
  Int_t HistosPerCanvas = GetNEigenPerCanvas();

  Ncanvas = BigVector.GetNoElements() / HistosPerCanvas;
  if ((BigVector.GetNoElements()%HistosPerCanvas)>0) Ncanvas++;

  return Ncanvas;
}

//////////////////////////////////////////////
Int_t GetNdof()
{
  return BigVector.GetNoElements();
}

//////////////////////////////////////////////
void DivideEigenCanvas(TCanvas *EigCan)
{

  switch (GetNEigenPerCanvas()){
  case 4: 
    EigCan->Divide(2,2);
    break;
  case 6: 
    EigCan->Divide(2,3);
    break;
  case 12: 
    EigCan->Divide(3,4);
    break;
  case 24: 
    EigCan->Divide(6,4);
    break;
  default:
    EigCan->Divide(5,5);
  }

  return;
}

//////////////////////////////////////////////
void SetActiveNdof(Bool_t WhatTx, Bool_t WhatTy, Bool_t WhatTz, Bool_t WhatRx, Bool_t WhatRy, Bool_t WhatRz)
{
  UseTx = WhatTx;
  UseTy = WhatTy;
  UseTz = WhatTz;
  UseRx = WhatRx;
  UseRy = WhatRy;
  UseRz = WhatRz;
  
  return;
}

//////////////////////////////////////////////
void SetDoFTypes()
{
  if (GetPrintLevel()>=10) std::cout << " <ea> setting DoF types. Start " << std::endl;
  //
  DoFType.ResizeTo(BigVector.GetNoElements()); // first set the right dimension for the vector
  
  // default (Tx, Ty, Tz, Rx, Ry, Rz)
  for (int i=0; i<BigVector.GetNoElements(); i++) {
    DoFType[i] = i%6;
  }
  //
  if (DoFs.GetNoElements() != 0) {
    for (int i=0; i<BigVector.GetNoElements(); i++) {
      DoFType[i] = DoFs[i]; // save the DOF type from what was read in the file
    }
  }
  //
  if (GetPrintLevel()>10) {
    std::cout << " <ea> ------- printing DoFTypes: size " 
	      << DoFType.GetNoElements() << " ------- " << std::endl; 
        for (int i=0; i < DoFType.GetNoElements(); i++) {
      std::cout << " id = " << i << " Type = " << DoFType[i] << " (" <<  DofName[(int) DoFType[i]] <<") " << endl;
    }
    std::cout << " <ea> ------------ EO DoFType ------------ " << std::endl; 
    std::cout << std::endl;
  }
  
  if (GetPrintLevel()>=10) std::cout << " <ea> setting DoF types. Completed " << std::endl;
  // 
  return;
}


//////////////////////////////////////////////
void BookAliCorrHistos()
{
  if (GetPrintLevel()>=3) std::cout << " <ea> Booking histograms for the alignment corrections... start " << std::endl;
  
  // 
  // Histogram of alignment corrections in diagonal base, after weak mode removal.
  hAliCorr_b = new TH1F ("hAliCorr_b","#delta b  (align. corr. in diag. base, after mode cut)",
			 BigVector.GetNoElements(), 
			 0.5, 
			 BigVector.GetNoElements()+0.5);
  hAliCorr_b->SetXTitle("mode");
  hAliCorr_b->SetLineColor(kRed);
  hAliCorr_b->SetFillStyle(3001);
  hAliCorr_b->SetFillColor(kRed);
  if (GetPrintLevel()>=7) 
    std::cout << " <ea> Defining histogram of alignment corrections in diagonal base with  " 
	 <<  hAliCorr_b->GetNbinsX() << " bins " << std::endl;
  
  // strore alignment corrections in the daigonal base in the ad-hoc histogram
  for (int i=0; i<AlignCorr_b.GetNoElements(); i++) {
    hAliCorr_b->SetBinContent(i+1,AlignCorr_b(i));
  }
  
  if (!QuietMode) {
    CanDiagonalBase->cd(2);
    hAliCorr_b->Draw("same");
  }

  // Book a histogram for alignment corrections in the modules base using after weak mode removal
  // Corrections in the module (structure frame)
  // All corrections (tx, ty, tz, rx...) are included in this plot
  hAliCorr = new TH1F ("hAliCorr","Alignment corrections",
		       BigVector.GetNoElements(), 
		       0.5, 
		       BigVector.GetNoElements()+0.5);
  hAliCorr->SetXTitle("dof");
  hAliCorr->SetLineColor(kRed);
  hAliCorr->SetFillStyle(3001);
  hAliCorr->SetFillColor(kRed);
  SetXAxisLabels(hAliCorr);

  for (int i=0; i<AlignCorr.GetNoElements(); i++) {
    hAliCorr->SetBinContent(i+1,AlignCorr(i));
  }
  if (!QuietMode) { // draw histogram on thre same frame
    CanAlignCorr->cd(1);
    hAliCorr->Draw("same");
  }

  if (GetPrintLevel()>=7) std::cout << "   <ea> Booking histograms for the corrections 1 by 1 (ie Tx for all) ... " << std::endl;

  Int_t NStruct = GetNStruct();
  if (GetPrintLevel()>=3) std::cout << "   <ea> ** BookAliCorrHistos**  booking histos for " << NStruct << " structures" << std::endl;

  // compute the bins for the histrograms showing the projections
  Int_t xBins = NStruct/3;
  if (xBins<11) xBins = 11;
  
  HistoBins HistLimits;
  
  // Histogram just for the Tx
  if (GetPrintLevel()>=9) std::cout << "   <ea> Booking histograms for the Tx corrections " << std::endl;
  if (hTxAliCorr == NULL) 
    hTxAliCorr = new TH1F("hTxAliCorr","Tx alignment corrections",
			  NStruct, 0.5, NStruct+0.5);
  hTxAliCorr->SetYTitle("Tx (mm)");
  //hTxAliCorr->SetXTitle("Structure id.");
  SetXAxisStructures(hTxAliCorr);
  hTxAliCorr->SetLineWidth(2);
  hTxAliCorr->SetMarkerStyle(20);
  
  
  // Histogram just for the Ty
  if (GetPrintLevel()>=9) std::cout << "   <ea> Booking histograms for the Ty corrections " << std::endl;
  if (hTyAliCorr == NULL) 
    hTyAliCorr = new TH1F("hTyAliCorr","Ty alignment corrections",
			  NStruct, 0.5, NStruct+0.5);
  hTyAliCorr->SetYTitle("Ty (mm)");
  //hTyAliCorr->SetXTitle("Structure id.");
  SetXAxisStructures(hTyAliCorr);
  hTyAliCorr->SetLineWidth(2);
  hTyAliCorr->SetMarkerStyle(20);
  
  // Histogram just for the Tz
  if (GetPrintLevel()>=9) std::cout << "   <ea> Booking histograms for the Tz corrections " << std::endl;
  if (hTzAliCorr == NULL) 
    hTzAliCorr = new TH1F("hTzAliCorr","Tz alignment corrections",
			  NStruct, 0.5, NStruct+0.5);
  hTzAliCorr->SetYTitle("Tz (mm)");
  //hTzAliCorr->SetXTitle("Structure id.");
  SetXAxisStructures(hTzAliCorr);
  hTzAliCorr->SetLineWidth(2);
  hTzAliCorr->SetMarkerStyle(20);
  
  // Histogram just for the Rx
  if (GetPrintLevel()>=9) std::cout << "   <ea> Booking histograms for the Rx corrections " << std::endl;
  if (hRxAliCorr == NULL) 
    hRxAliCorr = new TH1F("hRxAliCorr","Rx alignment corrections",
			  NStruct, 0.5, NStruct+0.5);
  hRxAliCorr->SetYTitle("Rx (mrad)");
  //hRxAliCorr->SetXTitle("Structure id.");
  SetXAxisStructures(hRxAliCorr);
  hRxAliCorr->SetLineWidth(2);
  hRxAliCorr->SetMarkerStyle(20);

  // Histogram just for the Ry
  if (GetPrintLevel()>=9) std::cout << "   <ea> Booking histograms for the Ry corrections " << std::endl;
  if (hRyAliCorr == NULL) 
    hRyAliCorr = new TH1F("hRyAliCorr","Ry alignment corrections",
			NStruct, 0.5, NStruct+0.5);
  hRyAliCorr->SetYTitle("Ry (mrad)");
  //hRyAliCorr->SetXTitle("Structure id.");
  SetXAxisStructures(hRyAliCorr);
  hRyAliCorr->SetLineWidth(2);
  hRyAliCorr->SetMarkerStyle(20);

  // Histogram just for the Rz
  if (GetPrintLevel()>=9) std::cout << "   <ea> Booking histograms for the Rz corrections " << std::endl;
  if (hRzAliCorr == NULL) 
    hRzAliCorr = new TH1F("hRzAliCorr","Rz alignment corrections",
			  NStruct, 0.5, NStruct+0.5);
  hRzAliCorr->SetYTitle("Rz (mrad)");
  //hRzAliCorr->SetXTitle("Structure id.");
  SetXAxisStructures(hRzAliCorr);
  hRzAliCorr->SetLineWidth(2);
  hRzAliCorr->SetMarkerStyle(20);


  //------------------------------------------
  // Fill the histograms of correction by dof
  Int_t jdof = 0; // counter of the current real dof
  double thisDoFType;
  for (Int_t istruct=0; istruct< GetNStruct(); istruct++) { // looping over all structures
    // Each structure has 6 dof maximum. If less are used, jump those unused and print those used
    for (Int_t index=0; index < 6; index++) { // looping over all possible dofs

      if (index == DoFType(jdof)) {
	thisDoFType =  DoFType(jdof);
	if (thisDoFType == DoFTx) {
	  hTxAliCorr->SetBinContent(istruct+1,AlignCorr(jdof));
	  hTxAliCorr->SetBinError(istruct+1,AlignCorrError(jdof));
	}
	if (thisDoFType == DoFTy) {
	  hTyAliCorr->SetBinContent(istruct+1,AlignCorr(jdof));
	  hTyAliCorr->SetBinError(istruct+1,AlignCorrError(jdof));
	}
	if (thisDoFType == DoFTz) {
	  hTzAliCorr->SetBinContent(istruct+1,AlignCorr(jdof));
	  hTzAliCorr->SetBinError(istruct+1,AlignCorrError(jdof));
	}
	if (thisDoFType == DoFRx) {
	  hRxAliCorr->SetBinContent(istruct+1,AlignCorr(jdof));
	  hRxAliCorr->SetBinError(istruct+1,AlignCorrError(jdof));
	}
	if (thisDoFType == DoFRy) {
	  hRyAliCorr->SetBinContent(istruct+1,AlignCorr(jdof));
	  hRyAliCorr->SetBinError(istruct+1,AlignCorrError(jdof));
	}
	if (thisDoFType == DoFRz) {
	  hRzAliCorr->SetBinContent(istruct+1,AlignCorr(jdof));
	  hRzAliCorr->SetBinError(istruct+1,AlignCorrError(jdof));
	}
	jdof++;
      } // end of good dof type
    } // end of loop on indices
  } // end of loop on structures
  

  // Next step: create the projection histograms of correction by dof
  // Tx
  HistLimits = GetHistoLimits(hTxAliCorr);
  hTxAliCorr->SetMaximum(HistLimits.max);
  hTxAliCorr->SetMinimum(HistLimits.min);
  hTxAliCorrProj = new TH1F("hTxAliCorrProj","Tx alignment corrections", xBins, HistLimits.min, HistLimits.max); 
  hTxAliCorrProj->SetXTitle("Tx (mm)");
  // Ty
  HistLimits = GetHistoLimits(hTyAliCorr);
  hTyAliCorr->SetMaximum(HistLimits.max);
  hTyAliCorr->SetMinimum(HistLimits.min);
  hTyAliCorrProj = new TH1F("hTyAliCorrProj","Ty alignment corrections", xBins, HistLimits.min, HistLimits.max); 
  hTyAliCorrProj->SetXTitle("Ty (mm)");
  // Tz
  HistLimits = GetHistoLimits(hTzAliCorr);
  hTzAliCorr->SetMaximum(HistLimits.max);
  hTzAliCorr->SetMinimum(HistLimits.min);
  hTzAliCorrProj = new TH1F("hTzAliCorrProj","Tz alignment corrections", xBins, HistLimits.min, HistLimits.max); 
  hTzAliCorrProj->SetXTitle("Tz (mm)");
  // Rx
  HistLimits = GetHistoLimits(hRxAliCorr);
  hRxAliCorr->SetMaximum(HistLimits.max);
  hRxAliCorr->SetMinimum(HistLimits.min);
  hRxAliCorrProj = new TH1F("hRxAliCorrProj","Rx alignment corrections", xBins, HistLimits.min, HistLimits.max); 
  hRxAliCorrProj->SetXTitle("Rx (mrad)");
  // Ry
  HistLimits = GetHistoLimits(hRyAliCorr);
  hRyAliCorr->SetMaximum(HistLimits.max);
  hRyAliCorr->SetMinimum(HistLimits.min);
  hRyAliCorrProj = new TH1F("hRyAliCorrProj","Ry alignment corrections", xBins, HistLimits.min, HistLimits.max); 
  hRyAliCorrProj->SetXTitle("Ry (mrad)");
  // Rz
  HistLimits = GetHistoLimits(hRzAliCorr);
  hRzAliCorr->SetMaximum(HistLimits.max);
  hRzAliCorr->SetMinimum(HistLimits.min);
  hRzAliCorrProj = new TH1F("hRzAliCorrProj","Rz alignment corrections", xBins, HistLimits.min, HistLimits.max); 
  hRzAliCorrProj->SetXTitle("Rz (mrad)");
  

  // Now... once projection histograms are created with the right range, then fill them
  for (Int_t idof=0; idof < GetNdof(); idof++) {
    thisDoFType =  DoFType(idof);
    if (thisDoFType == DoFTx) hTxAliCorrProj->Fill(AlignCorr(idof));
    if (thisDoFType == DoFTy) hTyAliCorrProj->Fill(AlignCorr(idof));
    if (thisDoFType == DoFTz) hTzAliCorrProj->Fill(AlignCorr(idof));
    if (thisDoFType == DoFRx) hRxAliCorrProj->Fill(AlignCorr(idof));
    if (thisDoFType == DoFRy) hRyAliCorrProj->Fill(AlignCorr(idof));
    if (thisDoFType == DoFRz) hRzAliCorrProj->Fill(AlignCorr(idof));
  }

  /////////////////
  // Now plot the histograms
  CAliCorrDof->cd(1);
  if (UseTx) {hTxAliCorr->Draw(); gPad->SetGridx(1); gPad->SetGridy(1);}
  
  CAliCorrDof->cd(2);
  if (UseTy) {hTyAliCorr->Draw(); gPad->SetGridx(1); gPad->SetGridy(1);}

  CAliCorrDof->cd(3);
  if (UseTz) {hTzAliCorr->Draw(); gPad->SetGridx(1); gPad->SetGridy(1);}

  CAliCorrDof->cd(4);
  if (UseRx) {hRxAliCorr->Draw(); gPad->SetGridx(1); gPad->SetGridy(1);}

  CAliCorrDof->cd(5);
  if (UseRy) {hRyAliCorr->Draw(); gPad->SetGridx(1); gPad->SetGridy(1);}

  CAliCorrDof->cd(6);
  if (UseRz) {hRzAliCorr->Draw();  gPad->SetGridx(1); gPad->SetGridy(1);}

  // 
  TCanvas *CAliCorrProj = new TCanvas("AliCorrProj", "jacobana: Alignment corrections projected by dof ", 1200, 800);
  CAliCorrProj->Divide(3,2);

  CAliCorrProj->cd(1);
  if (UseTx) {hTxAliCorrProj->Draw(); gPad->SetGridx(1);}

  CAliCorrProj->cd(2);
  if (UseTy) {hTyAliCorrProj->Draw(); gPad->SetGridx(1);}

  CAliCorrProj->cd(3);
  if (UseTz) {hTzAliCorrProj->Draw(); gPad->SetGridx(1);}

  CAliCorrProj->cd(4);
  if (UseRx) {hRxAliCorrProj->Draw(); gPad->SetGridx(1);}

  CAliCorrProj->cd(5);
  if (UseRy) {hRyAliCorrProj->Draw(); gPad->SetGridx(1);}

  CAliCorrProj->cd(6);
  if (UseRz) {hRzAliCorrProj->Draw(); gPad->SetGridx(1);}

  if (GetPrintLevel()>=3) std::cout << " <ea> Booking histograms for the alignment corrections... completed " << std::endl;
  return;
}

//////////////////////////////////////////////
void BookAliCorrErrHistos()
{
  if (GetPrintLevel()>=3) std::cout << " <ea> Booking histograms for the alignment corrections... start " << std::endl;
  
  // 
  // Histogram of alignment corrections in diagonal base, after weak mode removal.
  hAliCorrErr_b = new TH1F ("hAliCorrErr_b","Errors of #delta b  (errors of align. corr. in diag. base, after mode cut)",
			 BigVector.GetNoElements(), 
			 0.5, 
			 BigVector.GetNoElements()+0.5);
  hAliCorrErr_b->SetXTitle("mode");
  hAliCorrErr_b->SetLineColor(kRed);
  hAliCorrErr_b->SetFillStyle(3001);
  hAliCorrErr_b->SetFillColor(kRed);
  if (GetPrintLevel()>=7) 
    std::cout << " <ea> Defining histogram of alignment corrections errors in diagonal base with  " 
	 <<  hAliCorrErr_b->GetNbinsX() << " bins " << std::endl;
  
  // strore alignment corrections in the daigonal base in the ad-hoc histogram
  for (int i=0; i<AlignCorrError_b.GetNoElements(); i++) {
    hAliCorrErr_b->SetBinContent(i+1,AlignCorrError_b(i));
  }
  if (!QuietMode) {
    CanDiagonalBase->cd(4);
    hAliCorrErr_b->Draw();
  }

  // 
  // Histogram of alignment corrections in module base, after weak mode removal.
  hAliCorrErr = new TH1F ("hAliCorrErr","Errors of #delta a  (errors of align. corr., after mode cut)",
			 BigVector.GetNoElements(), 
			 0.5, 
			 BigVector.GetNoElements()+0.5);
  hAliCorrErr->SetXTitle("dof");
  hAliCorrErr->SetLineColor(kOrange+1);
  hAliCorrErr->SetFillStyle(3001);
  hAliCorrErr->SetFillColor(kOrange+1);
  SetXAxisLabels(hAliCorrErr);
  if (GetPrintLevel()>=7) 
    std::cout << " <ea> Defining histogram of alignment corrections errors in module base with  " 
	 <<  hAliCorrErr->GetNbinsX() << " bins " << std::endl;
  
  // strore alignment corrections in the daigonal base in the ad-hoc histogram
  for (int i=0; i<AlignCorrError.GetNoElements(); i++) {
    hAliCorrErr->SetBinContent(i+1,AlignCorrError(i));
  }
  if (!QuietMode) {
    // draw it twice
    // - in the diagonal base plots
    CanDiagonalBase->cd(5);
    hAliCorrErr->Draw();
    // - in alignment corr. errors
    CanAlignCorrErr->cd(1);
    hAliCorrErr->Draw();
    DrawStructLines(hAliCorrErr,CanAlignCorrErr,1);
  }

  return;
}

//////////////////////////////////////////////
void BookAliCorrPullHistos()
{
  if (GetPrintLevel()>=0) std::cout << " <ea> Booking histograms for the alignment corrections pulls... start " << std::endl;
  
  // 
  // Histogram of alignment corrections in diagonal base, after weak mode removal.
  hAliCorrPull_b = new TH1F ("hAliCorrPull_b","Pull of #delta b  (Pulls of align. corr. in diag. base)",
			 BigVector.GetNoElements(), 
			 0.5, 
			 BigVector.GetNoElements()+0.5);
  hAliCorrPull_b->SetXTitle("mode");
  hAliCorrPull_b->SetLineColor(kRed);
  hAliCorrPull_b->SetFillStyle(3001);
  hAliCorrPull_b->SetFillColor(kRed);
  if (GetPrintLevel()>=0) 
    std::cout << " <ea> Defining histogram of alignment correction pulls in diagonal base with  " 
	 <<  hAliCorrPull_b->GetNbinsX() << " bins " << std::endl;
  
  // strore alignment corrections in the daigonal base in the ad-hoc histogram
  for (int i=0; i<AlignCorrError_b.GetNoElements(); i++) {
    float pullvalue = 0;
    if (IsEigenModeUsed(i)) pullvalue = AlignCorr_b(i)/AlignCorrError_b(i);
    hAliCorrPull_b->SetBinContent(i+1, pullvalue);
  }

  if (!QuietMode) {
    if (GetPrintLevel()>=0) std::cout << " <ea> drawing alignment corrections pulls..." << std::endl;
    CanDiagonalBase->cd(6);
    hAliCorrPull_b->Draw();
  }

  /*
    // Salva: from copy paste: needs to define the new histogram, fill it and draw it.
  // 
  // Histogram of alignment corrections in module base, after weak mode removal.
  hAliCorrErr = new TH1F ("hAliCorrErr","Errors of #delta a  (errors of align. corr., after mode cut)",
			 BigVector.GetNoElements(), 
			 0.5, 
			 BigVector.GetNoElements()+0.5);
  hAliCorrErr->SetXTitle("dof");
  hAliCorrErr->SetLineColor(kOrange+1);
  hAliCorrErr->SetFillStyle(3001);
  hAliCorrErr->SetFillColor(kOrange+1);
  SetXAxisLabels(hAliCorrErr);
  if (GetPrintLevel()>=7) 
    std::cout << " <ea> Defining histogram of alignment corrections errors in module base with  " 
	 <<  hAliCorrErr->GetNbinsX() << " bins " << std::endl;
  
  // strore alignment corrections in the daigonal base in the ad-hoc histogram
  for (int i=0; i<AlignCorrError.GetNoElements(); i++) {
    hAliCorrErr->SetBinContent(i+1,AlignCorrError(i));
  }
  if (!QuietMode) {
    // draw it twice
    // - in the diagonal base plots
    CanDiagonalBase->cd(5);
    hAliCorrErr->Draw();
    // - in alignment corr. errors
    CanAlignCorrErr->cd(1);
    hAliCorrErr->Draw();
    DrawStructLines(hAliCorrErr,CanAlignCorrErr,1);
  }
  */

  return;
}

//////////////////////////////////////////////
void BookModeCorrections()
{
  if (GetPrintLevel()>=3) 
    std::cout << " <ea> -- BookModeCorrections -- starting " << std::endl << std::endl;
  // book the histograms on the
  char nameh1[80];
  char nameh2[80];
  Float_t factor = 1.;
  
  for(int icol=0; icol<ChangeOfBaseMatrix.GetNcols(); icol++) {    
    if (GetPrintLevel()>=9) std::cout << "   doing mode " << icol+1 << "/" << ChangeOfBaseMatrix.GetNcols() << std::endl;
    sprintf(nameh1,"hModeCorr_%d",icol+1);
    sprintf(nameh2,"Mode %d Correction",icol+1);
    hModCorr[icol] = new TH1F(nameh1, nameh2,
			     ChangeOfBaseMatrix.GetNrows(), 
			     0.5, 
			     ChangeOfBaseMatrix.GetNrows()+0.5);
    
    if (AlignCorr.GetNoElements()<= 186) {
      SetXAxisLabels(hModCorr[icol]);
    }
    hModCorr[icol]->SetXTitle("d.o.f.");
    for(int irow=0; irow<ChangeOfBaseMatrix.GetNrows(); irow++) {
      //      factor = (IsTranslation(irow)) ? 1. : Rad2mRad; // Print mrad for the angles 
      hModCorr[icol]->SetBinContent(irow+1, ChangeOfBaseMatrix(irow,icol)*AlignCorrRaw_b(icol)*factor);
    }
  }  


  if (!QuietMode && PlotEigenVecs){

    // set color of mode corrections histograms
    SetColorsOfModeCorrHists();

    // Now Plot the eigen vectors histograms in several pages (i.e. canvas)
    // 6 histos per canvas 
    Int_t HistosPerCanvas = GetNEigenPerCanvas();
    Int_t Ncanvas = GetNEigenCanvas(); 
    
    if (Ncanvas>40) {
      Ncanvas = 40;
      if (GetPrintLevel()>=1)
	  std::cout << " <ea> -- BookModeCorrections -- WARNING too many canvas' --> Limited to "<< Ncanvas << std::endl;
    }

    const Int_t nc = Ncanvas;
    TCanvas *CanModeCorr[nc];
    Int_t FirstEigenVec=0;
    Int_t LastEigenVec=-1;
    char cname1[80];
    char cname2[80];
    Int_t ipad=0;

    // loop to create canvases
    for (Int_t icanvas=0; icanvas < Ncanvas; icanvas++) {
      FirstEigenVec = LastEigenVec+1;
      LastEigenVec = FirstEigenVec+HistosPerCanvas-1;
      sprintf(cname1,"ModCorr_%d_%d",FirstEigenVec+1,LastEigenVec+1);
      sprintf(cname2,"Mode corrections: %d -> %d ",FirstEigenVec+1,LastEigenVec+1);
      CanModeCorr[icanvas] = new TCanvas(cname1, cname2, 1000, 880);
      DivideEigenCanvas(CanModeCorr[icanvas]);
    }
    
    // loop to plot histograms
    FirstEigenVec=0;
    LastEigenVec=-1;
    for (Int_t icanvas=0; icanvas < Ncanvas; icanvas++) {
      FirstEigenVec = LastEigenVec+1;
      LastEigenVec = FirstEigenVec+HistosPerCanvas-1;
      if (LastEigenVec >= BigVector.GetNoElements()) LastEigenVec = BigVector.GetNoElements()-1; 
      ipad = 0; 
      for (Int_t iev=FirstEigenVec; iev<=LastEigenVec; iev++) {
	ipad++;
	CanModeCorr[icanvas]->cd(ipad); 
	hModCorr[iev]->SetYTitle("mm or mrad");
	hModCorr[iev]->Draw();
	DrawStructLines(hModCorr[iev],CanModeCorr[icanvas],ipad);
	gPad->Update();
      }
    }
  }

  if (true) {
    // draw all mode corrections in top of each other (in a stack)
    // open a canvas
    TCanvas *CanSumCorr = new TCanvas("SumCorr", "Sum of alignment corrections");
    CanSumCorr->cd(1);
    // book a histogram that helps as frame // hAliCorr->GetXaxis()->GetXmax()
    HistoBins HistLimits = GetHistoLimits(hAliCorr);
    TH2F *hisFrame = new TH2F("hisFrame","Sum of alignment corrections",
    			      hModCorr[0]->GetNbinsX(),
			      0.,
    			      hModCorr[0]->GetNbinsX(),
    			      1,
    			      HistLimits.min,
    			      HistLimits.max); 
    SetXAxisLabels(hisFrame, true); 

    // stack all used mode corrections. Each mode with a different color for easy diagnostic
    THStack *hstack = new THStack("hstack","Sum of alignment corrections");
    Int_t ModeLow = GetNdof(); // Lowest mode used 
    for (int i=0; i<GetNdof(); i++) {
      if (IsEigenModeUsed(i)) {
	if (i<ModeLow) ModeLow = i;
	// ModesUsed++;
	// Color_i = (int) (51+ModesUsed*ColorStep)-1;
	// if (Color_i>100) Color_i = 100;
	hstack->Add(hModCorr[i]);
	if (GetPrintLevel()>=8) {
	  std::cout << " <ea> hstack = " << hstack->GetTitle() << " after stacking " << i << " nmodes" << std::endl;
	  std::cout << "      --> hModCorr["<<i<<"] ("<< hModCorr[i]->GetTitle() << " bin #1 has content:" << hModCorr[i]->GetBinContent(1) << std::endl;
	}
      }
    }
    // plot
    gStyle->SetNumberContours(100);
    hisFrame->GetZaxis()->SetRangeUser(ModeLow,GetNdof());
    hisFrame->Fill(0.,0.);
    hisFrame->Draw("colz");
    hstack->Draw("same"); // this needs to be fixed --> there is a mismatch of bin numbers

    gPad->Update();
    TPaletteAxis *zPalette = (TPaletteAxis*)hisFrame->GetListOfFunctions()->FindObject("palette");
    zPalette->SetX1NDC(0.92);
    zPalette->SetX2NDC(0.95);
    zPalette->SetLabelSize(0.03);
    zPalette->Draw();
    gPad->SetGridx(1);
    gPad->SetGridy(1);
  }
  if (GetPrintLevel()>=3) 
    std::cout << " <ea> -- BookModeCorrections -- completed " << std::endl << std::endl;
  return;
}

//////////////////////////////////////////////
void SetColorsOfModeCorrHists()
{
  float ColorStep = 50./(GetNUsedDof()-1);  // there are 50 colors
  Int_t Color_i = 0;
  Int_t ModesUsed = 0;

  for (int i=0; i<GetNdof(); i++) {
    if (IsEigenModeUsed(i)) {
      // Modes which are used are filled with a color
      ModesUsed++;
      Color_i = (int) (51+ModesUsed*ColorStep)-1;
      if (Color_i>100) Color_i = 100;  // in case of overflow
      hModCorr[i]->SetLineColor(1);
      hModCorr[i]->SetFillColor(Color_i);
      hModCorr[i]->SetFillStyle(1001);
    }
    else {
      // Modes not used are colored in gray
      hModCorr[i]->SetLineColor(1);
      hModCorr[i]->SetFillColor(kGray+1);
      hModCorr[i]->SetFillStyle(1001);	
    }
  }

  return;
}

//////////////////////////////////////////////
void BookDofCorrections()
{
  if (GetPrintLevel()>=3) 
    std::cout << " <ea> -- BookDofCorrections -- starting " << std::endl << std::endl;
  // book the histograms on the
  char nameh1[80];
  char nameh2[80];
  Float_t factor = 1.;
  Double_t ThisDofType;

  // book the histograms with the dof corrections
  for(int idof=0; idof<GetNdof(); idof++) {    
    if (GetPrintLevel()>=9) std::cout << "   doing dof " << idof+1 << "/" << GetNdof() << std::endl;
    ThisDofType = DoFType(idof); 
    sprintf(nameh1,"hDofCorr_%d",idof+1);
    //    sprintf(nameh2,"Dof %d: corr. mode by mode: %s %s",
    //	    idof+1, StructureName[DofBelongsToStruct[idof]]->Data(), DofName[(int) ThisDofType]->Data()); 
    sprintf(nameh2,"Dof %d: corr. mode by mode: %s %s",
	    idof+1, StructureName[DofBelongsToStruct[idof]]->Data(), DofName[(int) ThisDofType].Data()); 
    hDofCorr[idof] = new TH1F(nameh1, nameh2, GetNdof(), 0.5, GetNdof()+0.5);
    hDofCorr[idof]->SetXTitle("mode");
    hDofCorr[idof]->SetLineColor(kRed);
    hDofCorr[idof]->SetFillStyle(3001);	
    hDofCorr[idof]->SetFillColor(kRed);	

    // now create the integral histogram
    sprintf(nameh1,"hDofIntegral_%d",idof+1);
    sprintf(nameh2,"Dof %d: integral of corrections mode by mode: %s %s",
	    idof+1, StructureName[DofBelongsToStruct[idof]]->Data(), DofName[(int) ThisDofType].Data());
    hDofIntegral[idof] = new TH1F(nameh1, nameh2, GetNdof(), 0.5, GetNdof()+0.5);
    hDofIntegral[idof]->SetXTitle("mode");
    
    hDofIntegral[idof]->SetLineColor(1);
    hDofIntegral[idof]->SetFillColor(kGray);
    hDofIntegral[idof]->SetFillStyle(1001);	

    // fill the histograms
    for(int imode=0; imode<GetNdof(); imode++) {
      if (IsEigenModeUsed(imode)) {
	//      factor = (IsTranslation(irow)) ? 1. : Rad2mRad; // Print mrad for the angles 
	hDofCorr[idof]->SetBinContent(imode+1, ChangeOfBaseMatrix(idof,imode)*AlignCorrRaw_b(imode)*factor);
      }
      else {
	hDofCorr[idof]->SetBinContent(imode+1, 0.);
      }
      // fill the integral
      hDofIntegral[idof]->SetBinContent(imode+1, 
					hDofIntegral[idof]->GetBinContent(imode)+
					hDofCorr[idof]->GetBinContent(imode+1));    
    }
  }  

  // once finished, change the limits for nicer presentation
  HistoBins HistLimits;
  for (int idof=0; idof < GetNdof(); idof++) {
    HistLimits = GetHistoLimits(hDofCorr[idof], hDofIntegral[idof]);
    hDofIntegral[idof]->SetMaximum(HistLimits.max);
    hDofIntegral[idof]->SetMinimum(HistLimits.min);
  }

  // draw histograms
  if (!QuietMode && PlotEigenVecs){
    // Now Plot the eigen vectors histograms in several pages (i.e. canvas)
    // 6 histos per canvas 
    Int_t HistosPerCanvas = GetNEigenPerCanvas();
    Int_t Ncanvas = GetNEigenCanvas(); 
    
    if (Ncanvas>40) {
      Ncanvas = 40;
      if (GetPrintLevel()>=1)
	  std::cout << " <ea> -- BookModeCorrections -- WARNING too many canvas' --> Limited to "<< Ncanvas << std::endl;
    }

    const Int_t nc = Ncanvas;
    TCanvas *CanModeCorr[nc];
    Int_t FirstEigenVec=0;
    Int_t LastEigenVec=-1;
    char cname1[80];
    char cname2[80];
    Int_t ipad=0;

    // loop to create canvases
    for (Int_t icanvas=0; icanvas < Ncanvas; icanvas++) {
      FirstEigenVec = LastEigenVec+1;
      LastEigenVec = FirstEigenVec+HistosPerCanvas-1;
      sprintf(cname1,"DofCorr_%d_%d",FirstEigenVec+1,LastEigenVec+1);
      sprintf(cname2,"Dof corrections: %d -> %d (%s)",FirstEigenVec+1,LastEigenVec+1,StructureName[DofBelongsToStruct[FirstEigenVec]]->Data());
      CanModeCorr[icanvas] = new TCanvas(cname1, cname2, 1000, 880);
      DivideEigenCanvas(CanModeCorr[icanvas]);
    }

    // loop to plot histograms
    FirstEigenVec=0;
    LastEigenVec=-1;
    for (Int_t icanvas=0; icanvas < Ncanvas; icanvas++) {
      FirstEigenVec = LastEigenVec+1;
      LastEigenVec = FirstEigenVec+HistosPerCanvas-1;
      if (LastEigenVec >= BigVector.GetNoElements()) LastEigenVec = BigVector.GetNoElements()-1; 
      ipad = 0; 
      for (Int_t iev=FirstEigenVec; iev<=LastEigenVec; iev++) {
	ipad++;
	CanModeCorr[icanvas]->cd(ipad); 
	hDofIntegral[iev]->SetYTitle("mrad");
	if (IsTranslation(iev))	hDofIntegral[iev]->SetYTitle("mm");
	hDofIntegral[iev]->Draw();
	hDofCorr[iev]->Draw("same");
	gPad->SetGridx(1);
	gPad->SetGridy(1);
      }
    }
  }

  if (GetPrintLevel()>=3) 
    std::cout << " <ea> -- BookDofCorrections -- completed " << std::endl << std::endl;
  return;
}

//////////////////////////////////////////////
void ChangeAngleUnits()
{
  if (GetPrintLevel()>=1) 
    std::cout << " <ea> ------ ChangeAngleUnit ---- starts ---- with AngleUnitFactor= " << AngleUnitFactor << std::endl;

  Double_t Factor = 1.;

  // first change the matrix elements
  for (Int_t i=0; i<BigMatrix.GetNrows(); i++) {

    for (Int_t j=0; j<BigMatrix.GetNcols(); j++) {
      Factor = 1.;
      if (IsRotation(i)) Factor *= AngleUnitFactor; 
      if (IsRotation(j)) Factor *= AngleUnitFactor; 

      if (Factor != 1.) {
	Double_t NewValue = Factor * BigMatrix(i,j);

	if (GetPrintLevel()>=8) 
	  std::cout << " <ea> BigMatrix(" << i << "," <<j <<") = " << BigMatrix(i,j) 
		    << " * " << Factor << " = " << NewValue << std::endl;	

	BigMatrix(i,j) = NewValue;
      } 
    }
  }
  // first change the vector elements
  for (Int_t i=0; i<BigVector.GetNoElements(); i++) {
    Factor = 1.;
    if (IsRotation(i)) Factor *= AngleUnitFactor; 
    if (Factor != 1.) {
      Double_t NewValue = Factor * BigVector(i);
      
      if (GetPrintLevel()>=8) 
	std::cout << " <ea> BigVector(" << i << ") = " << BigVector(i) 
		  << " * " << Factor << " = " << NewValue << std::endl;	
      
      BigVector(i) = NewValue;
    }
  }
  if (GetPrintLevel()>=1) 
    std::cout << " <ea> ------ ChangeAngleUnit ---- completed ... " << std::endl;
}

// //////////////////////////////////////////////
void PrintAlignmentCorrections(const TVectorD &PrVec)
{
  //
  Int_t NStruct = GetNStruct();
  if (GetPrintLevel()>=1) {  
    std::cout << " <ea> Number of structures: " << NStruct << std::endl;
  }

  // declare a summary
  Double_t Summary[6];
  for (int i=0; i<6; i++) {
    Summary[i]=0.;
  }

  Float_t factor = 1.;
  Bool_t PrintVal;
  Int_t idof = 0;

  std::cout << "  +---------------+-------------------------------------------------------------+ " << std::endl;
  std::cout << "  |   structure   |   Tx         Ty        Tz        Rx         Ry        Rz     |" << std::endl;
  std::cout << "  +---------------+-------------------------------------------------------------+ " << std::endl;
  for (Int_t istruct=0; istruct< GetNStruct(); istruct++) { // looping over all possible dofs
    // std::cout << "  |  " << setw(5) << istruct << "  | ";
    
    // print structure name
    if (istruct < GetNMaxStructures()) {
      if (StructureName[istruct] != NULL) {
	std::cout << "  | " <<  setw(12) << StructureName[istruct]->Data() << "  | ";
      }
      else {
	std::cout << "  |  " << setw(12) << istruct << "  | "; 
      }
    }
        
    // Each structure has 6 dof maximum. If less are used, jump those unused and print those used
    for (Int_t index=0; index < 6; index++) { 
      
      // this depends on the angle units //factor = (IsTranslation(iDof)) ? 1. : Rad2mRad; // Print mrad for the angles 
      factor = 1.;

      PrintVal = false;
      if (index == DoFType(idof)) PrintVal = true; 
      
      if (GetPrintLevel()>=9) {
	std::cout << std::endl
		  << " --> struct= " << istruct 
		  << "   dof within struct " << index 
		  << "   is used ? " << PrintVal
		  << "   Dof in vector is " << idof
		  << "   factor: " << factor 
		  << "   ouput-> ";
      }
      if (PrintVal) { 
	// std::cout << setw(7) << PrVec(i)*factor << "  ";
	std::cout << setw(8) << setprecision(3) << PrVec(idof)*factor << "  ";
	Summary[index] += PrVec(idof)*factor;
	idof++;
      }
      else {
	std::cout << "   --     ";
      }
    }
    std::cout << std::endl;
  }
  std::cout << "  +---------------+-------------------------------------------------------------+ " << std::endl;
  std::cout << "  +   all sum     + ";
  //  for (int i=0; i<GetNdofPerStruct(); i++) {
  for (int i=0; i<6; i++) {
    PrintVal = false;
    if (i == 0 && UseTx) PrintVal = true;
    if (i == 1 && UseTy) PrintVal = true; 
    if (i == 2 && UseTz) PrintVal = true; 
    if (i == 3 && UseRx) PrintVal = true; 
    if (i == 4 && UseRy) PrintVal = true; 
    if (i == 5 && UseRz) PrintVal = true; 
    //    factor = (i<3) ? 1. : 1000.; // imprimim mrad per als angles
    // std::cout << setw(7) << setprecision(4) << Summary[i]*factor << "  ";
    if (PrintVal) { 
      std::cout << setw(7) << setprecision(4) << Summary[i] << "  ";
    }
    else {
      std::cout << "   --      ";
    }
  }
  std::cout << std::endl;
  std::cout << "  +   average     + ";
  for (int i=0; i<GetNdofPerStruct(); i++) {
    //    factor = (i<3) ? 1. : 1000.; // imprimim mrad per als angles
    // std::cout << setw(7) << setprecision(4) << Summary[i]*factor << "  ";
    std::cout << setw(7) << setprecision(4) << Summary[i]/NStruct << "  ";
  }
  std::cout << std::endl;
  std::cout << "  +---------------+-------------------------------------------------------------+ " << std::endl;
  std::cout << std::endl;
}

////////////////////////////////////////////
void ExtractName(unsigned long long id, int n){

  // extracting name of the structures
  if (GetPrintLevel()>=8) std::cout << " <ea> ExtractName <ea> extracting structure names -- START -- " << std::endl;
  if (GetPrintLevel()>=8) std::cout << "                       id = " << std::hex << id << std::dec << "  n= " << n << std::endl;
  int id32 = (id >> 32);
  if (GetPrintLevel()>=8) std::cout << "                     id32 = " << std::hex << id32 << std::dec << "  n= " << n << std::endl;
  

  // find out the detector type
  int det = (id32 >> 27);
  if (GetPrintLevel()>=8) std::cout << "                     det = " << std::hex << det << std::dec << "  n= " << n << std::endl;
  if (GetPrintLevel()>=8) std::cout << " Detector : " << det << std::endl;

  // find out the barrel or encap value
  // posible "bec" values
  // 0: ecc, 1: barrel, 2: eca (3 in TRT \277?) 
  int bec = 0;
  bec = (id >> 57) & 3;
  if ( det == PIX) bec = (id32 >> 24) & 3;
  //  int bec = (id >> 57) & 3; // for unsigned long long
  if (GetPrintLevel()>=8) std::cout << "      Bec : " << bec << std::endl;


  // extract layer number
  int lay = 0;
  if (det == PIX) {
    // lay = (id32 >> 23) & 3;
    lay = (id32 >> 22) & 3;
  }
  else {
    // lay = (id >> 21) & 15;
    lay = (id >> 56) & 15;
  }
  if (GetPrintLevel()>=8) std::cout << "    Layer : " << lay << std::endl;
  

  // Define the string to keep the Structure Name
  TString StructName;

  // Posible "det" values
  // 0: Pixel, 1: SCT, 2: TRT
  if(det==0)
  	StructName += "PIX";
  else if(det==1)
  	StructName += "SCT";
  else
  	StructName += "TRT";


  if(bec == 0){
    if (det==2)
      StructName += "-ECA"; // bec=0 TRT ECA
    else
      StructName += "-ECC"; // bec=0 SCT and PIX ECC
  }
  else if (bec == 1)
    StructName += "-BAR";
  else{
    if(det==2)
      StructName += "-ECC"; // bec=3 TRT ECC
    else
      StructName += "-ECA"; // bec=2 SCT and PIX ECA
  }
  if(bec==1)
  	StructName += "-l";
  else
  	StructName += "-d";
  StructName += lay;
  //StructName += "\\n";

  // reset what it was done... and start again
  StructName.Clear();
  // form the names: detector
  if (det== PIX) {
    StructName += "PIX"; 
    if (bec == 1) StructName += "ECA";
    if (bec == 2) StructName += "BAR";
    if (bec == 3) StructName += "ECC";

    // add layer naumber  
    // pixel barrel starts at layer 0 (which has lay=1), all ids are shifted by one.
    // pixel barrel end cap disks ids are correct
    if ( bec == 2) { // barrel
      StructName += lay - 1;
    }
    else { // end-caps
      StructName += lay;      
    }

    // IBL
    if (bec == 2 && lay==0) {
      StructName.Clear();
      StructName += "PIX-IBL";
    }
  }

  if (det==1) StructName += "SCT"; 
  if (det==2) StructName += "TRT"; 

  // Special cases
  // level 1
  if (NumberOfPIXStruct==1 && NumberOfSCTStruct==3 && NumberOfTRTStruct==3) {
    // main
    if (det == PIX) StructName = "PIX";
    if (det == SCT) StructName = "SCT";
    if (det == TRT) StructName = "TRT";
    // barrel-ecc
    if (det == SCT | det == TRT ) {
      if(bec == 0){
	if (det == TRT)
	  StructName += "-ECA"; // bec=0 TRT ECA
	else
	  StructName += "-ECC"; // bec=0 SCT and PIX ECC
      }
      else if (bec == 1)
  	StructName += "-BAR";
      else{
	if (det == TRT)
	  StructName += "-ECC"; // bec=3 TRT ECC
	else
	  StructName += "-ECA"; // bec=2 SCT and PIX ECA
      }
    }
  }

  // if there are only 3 TRT structures. Most likely are the barrel and endcaps
  if (NumberOfPIXStruct!=1 && NumberOfSCTStruct!=3 && NumberOfTRTStruct==3) {
    if (det == TRT) {
      StructName = "TRT";
      if (bec == 1) StructName += "-BAR";
      if (bec == 0) StructName += "-ECA"; // bec=0 TRT ECA
      if (bec == 3) StructName += "-ECC"; // bec=0 TRT ECA
    }
  } // special case: 3 structures TRT

  // if there are only 2 SCT and 3 TRT structures. Most likely the two SCT are the and endcaps and the SCT barrel was not aligned
  if (NumberOfSCTStruct==2 && NumberOfTRTStruct==3) {
    if (det == SCT) {
      StructName = "SCT";
      if (bec == 1) StructName += "-BAR";
      if (bec == 0) StructName += "-ECC"; 
      if (bec == 2) StructName += "-ECA"; 
    }
    if (det == TRT) {
      StructName = "TRT";
      if (bec == 1) StructName += "-BAR";
      if (bec == 0) StructName += "-ECA"; // bec=0 TRT ECA
      if (bec == 3) StructName += "-ECC"; // bec=0 TRT ECA
    }
  } // special case: 3 structures TRT

  // if there is only one pixel structure it is probably the pixel level 1
  if (NumberOfPIXStruct==1 ){
    if (det == PIX) {
      StructName = "PIX";
    }
  }
  // level 11 pixels
  if (NumberOfPIXStruct==2 && NumberOfSCTStruct==0 && NumberOfTRTStruct== 0 ){
    if (det == PIX) {
      // IBL
      if (bec == 2 && lay==0) {
	StructName.Clear();
	StructName += "PIX-IBL";
      } 
      else {
	StructName.Clear();
	StructName += "PIX";
      }
    }
  }
  
  //
  StructureName[n] = new TString(StructName);

  if (GetPrintLevel()>=8) cout << " <ea> structure " << n << " has label " << StructureName[n]->Data() << endl;
  if (GetPrintLevel()>=8) std::cout << " <ea> ExtractName <ea> extracting structure names -- COMPLETED -- " << std::endl;

  return;
}

////////////////////////////////////////////
void FindDetectorType(unsigned long long id, int n, int dofid)
{

  //int det = (id >> 27);
  int det = (id >> 59);  // for unsigned long long
  // 0: Pixel, 1: SCT, 2: TRT
  if (GetPrintLevel()>=5)
  	cout << "Detector : " << det << endl;
  //int bec = (id >> 25) & 3;
  int bec = (id >> 57) & 3; // for unsigned long long
  // In TRT: 0: eca, 1: barrel, 3: ecc 
  // In Pix and SCT: 0: ecc, 1: barrel, 2: eca 
  if (GetPrintLevel()>=5)
	cout << "     Bec : " << bec << endl;
  int lay;
  if (det==0)
  	lay = (id >> 23) & 3;
  else
    lay = (id >> 21) & 15;
  if (GetPrintLevel()>=5)
    cout << "       Layer : " << lay << endl;
  
  TString StructName;
  if(det==0)
  	StructName += "PIX";
  else if(det==1)
  	StructName += "SCT";
  else
  	StructName += "TRT";

  if(bec == 0)
  	StructName += "-ECA";
  else if (bec == 1)
  	StructName += "-BAR";
  else
  	StructName += "-ECC";

  if(bec==1)
  	StructName += "-l";
  else
  	StructName += "-d";
  StructName += lay;

  StructDetec[n] = det; // store the detector type identifier
  DofBelongsToStruct[dofid] = n;

  return;
}
///////////////////////////////////////////////
void SetNofStructuresPerDetec()
{
  for (Int_t i=0; i<GetNStruct(); i++) {
    if (StructDetec[i] == PIX) NumberOfPIXStruct++;
    if (StructDetec[i] == SCT) NumberOfSCTStruct++;
    if (StructDetec[i] == TRT) NumberOfTRTStruct++;
  }

  if (GetPrintLevel()>=5) {
    cout << " <ea> Number and type of the structures " << endl;
    cout << "        PIX " << NumberOfPIXStruct << endl
	 << "        SCT " << NumberOfSCTStruct << endl 
	 << "        TRT " << NumberOfTRTStruct << endl;
  }
  return;
}
///////////////////////////////////////////////
HistoBins GetHistoLimits(TH1F *hist)
{
  HistoBins thisHlimit;
  thisHlimit.max = hist->GetMaximum();
  thisHlimit.min = hist->GetMinimum(); 

  float Largest = TMath::Abs(thisHlimit.max);
  if (TMath::Abs(thisHlimit.min) > Largest) Largest = TMath::Abs(thisHlimit.min); 

  if (Largest < 0.001) Largest = 0.001;

  // make ranges symmetric
  thisHlimit.max =  Largest;
  thisHlimit.min = -Largest;

  //increment the ranges a bit
  float delta = (thisHlimit.max - thisHlimit.min)*0.10; // 
  thisHlimit.max += delta; 
  thisHlimit.min -= delta; 

  if (GetPrintLevel()>=9) {
    cout << " <ea> GetHistoLimits <ea> Histogram Limits for histo : "
	 << hist->GetTitle() 
	 << " = " << thisHlimit.min 
	 << " --> " << thisHlimit.max << endl;
  }
  return thisHlimit;
}
///////////////////////////////////////////////
HistoBins GetHistoLimits(TH1F *hist1, TH1F *hist2)
{
  HistoBins thisHlimit;
  thisHlimit.max = hist1->GetMaximum();
  thisHlimit.min = hist1->GetMinimum(); 

  if (hist2 != NULL) {
    if (hist2->GetMaximum() > hist1->GetMaximum())   thisHlimit.max = hist2->GetMaximum();
    if (hist2->GetMinimum() < hist1->GetMinimum())   thisHlimit.min = hist2->GetMinimum();
  }

  float Largest = TMath::Abs(thisHlimit.max);
  if (TMath::Abs(thisHlimit.min) > Largest) Largest = TMath::Abs(thisHlimit.min); 

  // make ranges symmetric
  thisHlimit.max =  Largest;
  thisHlimit.min = -Largest;

  //increment the ranges a bit
  float delta = (thisHlimit.max - thisHlimit.min)*0.10; // 
  thisHlimit.max += delta; 
  thisHlimit.min -= delta; 

  if (GetPrintLevel()>=9) {
    cout << " <ea> GetHistoLimits <ea> Histogram Limits for histo : "
	 << hist1->GetTitle() 
	 << " = " << thisHlimit.min 
	 << " --> " << thisHlimit.max << endl;
  }
  return thisHlimit;
}

/////////////////////////////////////////////////////////////////
void SetHitCut(float hitcut){
	HitCut = hitcut;
	}
	
/////////////////////////////////////////////////////////////////
float GetHitCut(){
	return HitCut;
	}
	
/////////////////////////////////////////////////////////////////
void RemoveRows(){
  
  // the idea is to remove those structures with too few hits
  // keep iterating till no structure with fewer hits than required is found.
  // **  rows to be removed are already flagged in RowsUsed **
  // removing by blocks of consecutive bad rows.
  while(true){
    int low_index = -1;
    int up_index = -1;
    bool set_low = false;
    bool set_up = false;
    for( int i=0; i<RowsUsed.GetNoElements(); i++){
      if(RowsUsed[i]==0 && !set_low){
	low_index = i;
	set_low = true;
      }
      if(RowsUsed[i]==1 && low_index<i && low_index!=-1 && !set_up){
	up_index = i-1;
	set_up = true;
      }
      if(i==RowsUsed.GetNoElements()-1 && !set_up){
	up_index = i;
	set_up = true;
      }
      if (low_index!=-1 && up_index!=-1){
	cout << " <ea> RemoveRows <ea> Removing rows/columns between: " << low_index << " to " << up_index << endl;
	RemoveVectorRows(low_index,up_index,RowsUsed);
	RemoveVectorRows(low_index,up_index,BigVector);
	RemoveVectorRows(low_index,up_index,DoFs);
	RemoveVectorRows(low_index,up_index,Identifiers);
	RemoveMatrixRows(low_index,up_index,BigMatrix);
	break;	
      }
    } // end loop on rows

    // count the number of structures with too few hits (actully per row, but if a row has too few hits, 
    // all the rows of that structure have too few hits
    int nZeros = 0;
    for (int i=0; i<RowsUsed.GetNoElements(); i++){
      if(RowsUsed[i]==0) nZeros++;
    }
    if (nZeros == 0) break;
  }
  return;
}
				
/////////////////////////////////////////////////////////////////
void RemoveVectorRows(int lwb, int upb, TVectorD& Vector){
	TVectorT<double> VectorUp;
	TVectorT<double> VectorDown;
	if(GetPrintLevel()>6){
		cout << "Original Vector: " << endl;
		Vector.Print();
	}
	if(lwb!=0){
		VectorUp.ResizeTo(lwb);
		VectorUp = Vector.GetSub(0,lwb-1);
		if(GetPrintLevel()>6){
			cout << "Up Vector: " << endl;
			VectorUp.Print();
		}
	}
	
	if(upb!=Vector.GetNoElements()-1){
		VectorDown.ResizeTo(Vector.GetNoElements()-upb-1);
		VectorDown = Vector.GetSub(upb+1,Vector.GetNoElements()-1);
		if(GetPrintLevel()>6){
			cout << "Down Vector: " << endl;
			VectorDown.Print();
		}
	}
	
	Vector.ResizeTo(VectorUp.GetNoElements()+VectorDown.GetNoElements());
	if(lwb!=0) Vector.SetSub(0,VectorUp);
	if(upb!=Vector.GetNoElements()-1) Vector.SetSub(lwb,VectorDown);
	if(GetPrintLevel()>6){
		cout << "New Vector: " << endl;
		Vector.Print();
	}
}	

/////////////////////////////////////////////////////////////////
void RemoveMatrixRows(int lwb, int upb, TMatrixDSym& SymMatrix){
	TMatrixT<double> MatrixUpLeft;
	TMatrixT<double> MatrixUpRight;
	TMatrixT<double> MatrixDownLeft;
	TMatrixT<double> MatrixDownRight;
	TMatrixT<double> Matrix = (TMatrixT<double>) SymMatrix;
	if(lwb!=0){
		MatrixUpLeft.ResizeTo(lwb,lwb);
		MatrixUpLeft = Matrix.GetSub(0,lwb-1,0,lwb-1);
	}
	if(lwb!=0 && upb!=Matrix.GetNrows()-1){
		MatrixUpRight.ResizeTo(lwb,Matrix.GetNcols()-upb-1);
		MatrixUpRight = Matrix.GetSub(0,lwb-1,upb+1,Matrix.GetNcols()-1);
	
		MatrixDownLeft.ResizeTo(Matrix.GetNrows()-upb-1,lwb);
		MatrixDownLeft = Matrix.GetSub(upb+1, Matrix.GetNrows()-1,0,lwb-1);
	}
	if(upb!=Matrix.GetNrows()-1){
		MatrixDownRight.ResizeTo(Matrix.GetNrows()-upb-1,Matrix.GetNcols()-upb-1);
		MatrixDownRight = Matrix.GetSub(upb+1, Matrix.GetNrows()-1,upb+1, Matrix.GetNcols()-1);
	}
	SymMatrix.ResizeTo(MatrixUpLeft.GetNrows()+MatrixDownLeft.GetNrows(),MatrixUpLeft.GetNcols()+MatrixUpRight.GetNcols());
	if(lwb!=0){
		SymMatrix.SetSub(0,0,MatrixUpLeft);
	}
	if(lwb!=0 && upb!=Matrix.GetNrows()-1){
		SymMatrix.SetSub(0,lwb,MatrixUpRight);
		SymMatrix.SetSub(lwb,0,MatrixDownLeft);
	}
	if(upb!=Matrix.GetNrows()-1){
		SymMatrix.SetSub(lwb,lwb,MatrixDownRight);
	
	}
}	
		
/////////////////////////////////////////////////////////////////       	
void DrawStructLines(TH1F *hist, TCanvas *can, int c){
  int tmpdof=0;
  
  TLine myline(0,0,1,1);
  myline.SetLineColor(1);
  myline.SetLineStyle(2);
  for (int i=0; i<GetNdof(); i++){
    if (DoFs[i]<tmpdof){
      double xline = hist->GetXaxis()->GetBinUpEdge(i);
      double gap = hist->GetMaximum()-hist->GetMinimum(); 
      double ymin = hist->GetMinimum() - 0.05*gap;
      double ymax = hist->GetMaximum() + 0.05*gap;
      can->cd(c);
      myline.DrawLine(xline, ymin, xline, ymax);
      if (GetPrintLevel()>=10) {
	std::cout << " <ea> drawlines -- hist: " << hist->GetTitle() 
		  << "  X0,y0, --> x1,y1: " << xline << " " << ymin << " ---> " << xline << " " << ymax << std::endl;
      }
    }
    tmpdof = DoFs[i];
  }
  return;
}		

/////////////////////////////////////////////////////////////////       	
void DrawStructLinesOnHMatrix(TH2F *hist)
{
  TLine myline(0,0,1,1);
  myline.SetLineColor(1);
  myline.SetLineStyle(1);

  double xmin, xmax;
  double ymin, ymax;

  // X axis lines
  int tmpdof=0;  
  for (int i=0; i<GetNdof(); i++){
    if (DoFs[i]<tmpdof){
      xmin = hist->GetXaxis()->GetBinUpEdge(i);
      xmax = xmin;
      ymin = hist->GetYaxis()->GetXmin();
      ymax = hist->GetYaxis()->GetXmax();
      myline.DrawLine(xmin, ymin, xmax, ymax);
      if (GetPrintLevel()>=10) {
	std::cout << " <ea> drawlines -- hist: " << hist->GetTitle() 
		  << "  X0,y0, --> x1,y1: " << xmin << " " << ymin << " ---> " << xmax << " " << ymax << std::endl;
      }
    }
    tmpdof = DoFs[i];
  }

  // Y axis lines
  tmpdof = DoFRz;
  int thisdof;
  for (int i=0; i<GetNdof(); i++){
    thisdof = GetNdof()-1-i;
    if (DoFs[thisdof]>tmpdof){ // when dof changes --> we are in a new structure
      xmin = hist->GetXaxis()->GetXmin();
      xmax = hist->GetXaxis()->GetXmax();
      ymin = hist->GetYaxis()->GetBinUpEdge(i);
      ymax = ymin;
      myline.DrawLine(xmin, ymin, xmax, ymax);
      if (GetPrintLevel()>=10) {
	std::cout << " <ea> drawlines -- hist: " << hist->GetTitle() 
		  << "  X0,y0, --> x1,y1: " << xmin << " " << ymin << " ---> " << xmax << " " << ymax 
		  << "   i= " << i << "  dof:" << DoFs[i] << "  (" << DofName[(int) DoFs[i]] <<")" << std::endl;
	  
      }
    }
    tmpdof = DoFs[thisdof];
  }
  return;
}		

//////////////////////////////////////////////
void SetXAxisLabels(TH1F *histo)
{
  // Set the DOF names in the X axis labels

  if (GetPrintLevel()>=8) {
    std::cout << " <ea> Setting labels for histogram: '" << histo->GetTitle() << "' "<< std::endl;
  }

  Int_t Nbins = histo->GetNbinsX();
  histo->GetXaxis()->SetLabelSize(0.03);
  if (Nbins < 18) histo->GetXaxis()->SetLabelSize(0.04);
  if (Nbins < 12) histo->GetXaxis()->SetLabelSize(0.05);

  Double_t ThisDofType;
  TString thisdofname("dofname");

 for (int idof=0; idof<Nbins; idof++) {
    ThisDofType = DoFType(idof);
    thisdofname.Clear();
    thisdofname.Append(StructureName[GetStructID(idof)]->Data());
    thisdofname.Append("_");
    thisdofname.Append(DofName[(int) ThisDofType].Data());
    if (GetPrintLevel()>=8) {
      std::cout << " dof " << idof << " belongs to structure " << GetStructID(idof)
		<< " (" << StructureName[GetStructID(idof)]->Data() <<") " 
		<< "  type= " << DofName[(int) ThisDofType].Data() 
		<< "  label = " <<  thisdofname.Data() << endl;
    }
    histo->GetXaxis()->SetBinLabel(idof+1, thisdofname.Data());
  }

  if (GetPrintLevel()>=8) {
    std::cout << " <ea> Histogram: '" << histo->GetTitle() << "' has brand new X axis labels. Action completed " << std::endl;
  }
  return;
}

//////////////////////////////////////////////
void SetXAxisLabels(TH2F *histo, Bool_t nameonlyfirstofeachstruct)
{
  // Set the DOF names in the X axis labels

  if (GetPrintLevel()>=10) std::cout << " <ea> -- SetXAxisLabels -- Setting labels for histogram: " << histo->GetTitle()<< std::endl;

  Int_t Nbins = histo->GetNbinsX();
  if (GetPrintLevel()>=10) {
    std::cout << " <ea>  -- SetXAxisLabels -- histogram has " << Nbins << " bins in X "<< std::endl;
  }

  histo->GetXaxis()->SetLabelSize(0.02);

  Double_t ThisDofType;
  TString thisdofname("dofname");
  Bool_t  setlabelname = true;
  Int_t   currentstruct = -1;

  for (int idof=0; idof<Nbins; idof++) {
    if (GetPrintLevel()>=10)  std::cout << " <ea>  -- SetXAxisLabels -- loop on bins -- dealing with bin: " << idof << std::endl;
    ThisDofType = DoFType(idof);
    thisdofname.Clear();
    thisdofname.Append(StructureName[GetStructID(idof)]->Data());
    thisdofname.Append("_");
    thisdofname.Append(DofName[(int) ThisDofType].Data());
    if (GetPrintLevel()>=8) {
      std::cout << " dof " << idof << " belongs to structure " << GetStructID(idof)
		<< " (" << StructureName[GetStructID(idof)]->Data() <<") " 
		<< "  type= " << DofName[(int) ThisDofType].Data() 
		<< "  label = " <<  thisdofname.Data() << endl;
    }

    if (nameonlyfirstofeachstruct) {
      setlabelname = false ; // flag this bin not to be labelled
      if (GetStructID(idof)!= currentstruct){
	// update currentstruct
	currentstruct = GetStructID(idof);
	setlabelname = true; // flag this bin to be labelled
      }
    }

    if (setlabelname) {
      histo->GetXaxis()->SetBinLabel(idof+1, thisdofname.Data());
    }
    else {
      // set it to blank
      histo->GetXaxis()->SetBinLabel(idof+1," ");
    }
  }

  if (GetPrintLevel()>=8) {
    std::cout << " <ea> Histogram: '" << histo->GetTitle() << "' has brand new X axis labels. Action completed " << std::endl;
  }
  return;
}

//////////////////////////////////////////////
void SetHMatrixLabels(TH2F *histo)
{
  // Set the DOF names in the X and Y axis labels in bigmatrix like histograms

  if (GetPrintLevel()>=8) {
    std::cout << " <ea> -- SetHMatrixLabels -- Setting labels for histogram: '" << histo->GetTitle() << "' "<< std::endl;
  }

  Int_t Nbins = histo->GetNbinsX();
  if (GetPrintLevel()>=8) {
    std::cout << " <ea>  -- SetHMatrixLabels -- histogram has " << Nbins << " bins in X "<< std::endl;
  }

  histo->GetXaxis()->SetLabelSize(0.04);
  histo->GetXaxis()->SetLabelSize(0.04);

  TString thisbinname("binname");
  Int_t   currentstruct = -1;
  Int_t   firstbin = 0;
  Int_t   lastbin = 0;
  Double_t ThisDofType;

  
  // dealing with X axis bins
  if (Nbins < 12) {
    // if there are few bins, one can use the full name
    histo->GetXaxis()->SetLabelSize(0.03);
    for (int idof=0; idof<Nbins; idof++) {
      ThisDofType = DoFType(idof);
      thisbinname.Clear();
      thisbinname.Append(StructureName[GetStructID(idof)]->Data());
      thisbinname.Append("_");
      thisbinname.Append(DofName[(int) ThisDofType].Data());
      histo->GetXaxis()->SetBinLabel(idof+1, thisbinname.Data());
    }
  }
  else {
    // when there are many structures best is to label them once
    for (int idof=0; idof<Nbins; idof++) {
      if (GetPrintLevel()>=10)  std::cout << " <ea>  -- SetHMatrixLabels -- loop on X bins -- dealing with bin: " << idof << std::endl;
      if (GetStructID(idof) != currentstruct || idof==0 || idof == Nbins-1) {
	if (idof == 0) firstbin = idof;
	lastbin = idof;
	// one has moved to the bins of the following structure
	// set now the name of the previous structure
	int inbin = (lastbin+firstbin)/2;
	if (idof>0) histo->GetXaxis()->SetBinLabel(inbin, thisbinname.Data());
	if (GetPrintLevel()>=10)  std::cout << " <ea>  -- SetHMatrixLabels -- new structure found! structure = " << GetStructID(idof) << std::endl;
	currentstruct = GetStructID(idof);
	firstbin = idof;
      } 
      thisbinname.Clear();
      thisbinname.Append(StructureName[GetStructID(idof)]->Data());
      if (GetPrintLevel()>=8) {
	std::cout << " dof " << idof << " belongs to structure " << GetStructID(idof)
		  << " (" << StructureName[GetStructID(idof)]->Data() <<") " 
		  << "  label = " <<  thisbinname.Data() << endl;
      }
      // in principle the bin name is set to blank
      histo->GetXaxis()->SetBinLabel(idof+1,"");
    }
  }

  currentstruct = -1; // reset the structure
  // dealing with Y axis bins
  if (Nbins < 12) {
    // if there are few bins, one can use the full name for each dof
    histo->GetYaxis()->SetLabelSize(0.025);
    for (int idof=Nbins-1; idof>=0; idof--) {
      ThisDofType = DoFType(idof);
      thisbinname.Clear();
      thisbinname.Append(StructureName[GetStructID(idof)]->Data());
      thisbinname.Append("_");
      thisbinname.Append(DofName[(int) ThisDofType].Data());
      histo->GetYaxis()->SetBinLabel(Nbins-idof, thisbinname.Data());
    }
  }
  else {
    // when there are many structures best is to label them once
    for (int idof=Nbins-1; idof>=0; idof--) {
      if (GetPrintLevel()>=10)  std::cout << " <ea>  -- SetHMatrixLabels -- loop on Y bins -- dealing with bin: " << idof << std::endl;
      if (GetStructID(Nbins-idof) != currentstruct || idof==0 || idof == Nbins-1) {
	if (idof == Nbins-1) lastbin = idof;
	firstbin = idof;
	// one has moved to the bins of the following structure
	// set now the name of the previous structure
	int inbin = (lastbin+firstbin)/2+1;
	if (idof!= Nbins-1) histo->GetYaxis()->SetBinLabel(inbin, thisbinname.Data());
	if (GetPrintLevel()>=10)  std::cout << " <ea>  -- SetHMatrixLabels -- new structure found! structure = " << GetStructID(Nbins-idof) 
					    << " (" << StructureName[GetStructID(Nbins-idof)]->Data() <<") " << std::endl;
	currentstruct = GetStructID(Nbins-idof);
	lastbin = idof;
      } 
      thisbinname.Clear();
      thisbinname.Append(StructureName[GetStructID(Nbins-idof)]->Data());
      if (GetPrintLevel()>=10) {
	std::cout << "       dof " << idof << " belongs to structure " << GetStructID(Nbins-idof)
		  << " (" << StructureName[GetStructID(Nbins-idof)]->Data() <<") " 
		  << "  label = " <<  thisbinname.Data() << endl;
      }
      // in principle the bin name is set to blank
      histo->GetYaxis()->SetBinLabel(idof+1,"");
    }
  }



  if (GetPrintLevel()>=8) {
    std::cout << " <ea> Histogram: '" << histo->GetTitle() << "' has brand new X axis labels. Action completed " << std::endl;
  }
}

//////////////////////////////////////////////
void SetXAxisStructures(TH1F *histo)
{
  // Set the Structure names in the X axis labels

  if (GetPrintLevel()>=8) {
    std::cout << " <ea> Setting labels for histogram: '" << histo->GetTitle() << "' "<< std::endl;
  }

  Int_t Nbins = histo->GetNbinsX();
  histo->GetXaxis()->SetLabelSize(0.05);

  for (int i=0; i<Nbins; i++) {
    histo->GetXaxis()->SetBinLabel(i+1,StructureName[i]->Data());
  }

  if (GetPrintLevel()>=8) {
    std::cout << " <ea> Histogram: '" << histo->GetTitle() << "' has brand new X axis labels. Action completed " << std::endl;
  }
}

///////////////////////////////////////////////////////////////
Bool_t IsTranslation(int i)
{
  bool istrans = false;
  if ((int) DoFType(i) == DoFTx) istrans = true;
  if ((int) DoFType(i) == DoFTy) istrans = true;
  if ((int) DoFType(i) == DoFTz) istrans = true;
  return istrans;
}	

///////////////////////////////////////////////////////////////
void DetectNullEigenValues()
{
  // the goal is to identify Null elenemts in the diagonal and to replace them with very small values
  for (int i=0; i< BigMatrix.GetNcols(); i++) {
    if (BigMatrix[i][i] == 0) {
      std::cout << " ZERO en i= " << i << std::endl;
      BigMatrix[i][i] = ZeroEquivalent;
    }
  }

  return;
}

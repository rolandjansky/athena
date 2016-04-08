/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "bigmatrix.h"

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>

//=====================================================================
//
// bigmatrix()
//
//=====================================================================
bigmatrix::bigmatrix(string inputFile)
  :  ncols(0),
     nrows(0),
     DoFs(6),
     occupancy(0),
     version(0),
     level(0)
{
  filename = inputFile;
  status = false;
  if(Initialize()) GetSummary();
}


//=====================================================================
// ~bigmatrix()
//=====================================================================
bigmatrix::~bigmatrix() {
  cout << " ++ bigmatrix ++ Game over. Insert coin " << endl;
}


//=====================================================================
// Initialize()
//=====================================================================
int bigmatrix::Initialize() {
  reducePlot = false;
  createPlot = true;
  Logo();

  if (reducePlot) DoFs = 1;

  TStopwatch sw;
  TStopwatch swtot;

  TMatrixD newBigMatrix;

  swtot.Start();
  sw.Start(); 

  if (!ReadSymMatrix(newBigMatrix)) {
    std::cout << " ++ bigmatrix ++ problems reading big matrix... bye " << std::endl; 
    return 0;
  }
  sw.Stop();

  swtot.Stop();
  cout << endl;
  swtot.Print();
  cout << endl;

  status = true;
  return 1;
}


//=====================================================================
// Logo()
//=====================================================================
void bigmatrix::Logo() {
  cout << endl;
  cout << "***********************************" << endl;
  cout << "          bigmatrix class           " << endl;
  cout << "***********************************" << endl;
  return;
}


//=====================================================================
// CheckVersion()
//=====================================================================
int bigmatrix::CheckVersion(TMatrixD &mat) {
  // cout << endl << "CheckVersion()" << endl;

  ifstream inmat(filename.c_str(), ios::binary);
  if(inmat.fail()){
    cout << "Can not open bigmatrix : " << filename << endl;
    return 0;
  }
  // cout << "   o bigmatrix '" << filename << "' opened ok " << endl;

  float version;
  inmat.read((char*)&nrows, sizeof(nrows));
  // cout << "   o nrows = " << nrows << endl;

  inmat.read((char*)&version, sizeof(version));
  // cout << "   o version = " << version << endl;

  if (version>=2.0) newMat = true;
  else newMat = false;

  inmat.close();  
  return 1;
}


//=====================================================================
// ReadSymMatrix()
//=====================================================================
int bigmatrix::ReadSymMatrix(TMatrixD &mat) {
  cout << endl << "ReadMatrix()" << endl;

  if (!CheckVersion(mat)) {
    // cout << " ++ bigmatrix ++ problems with CheckVersion... bye " << endl; 
    return 0;
  }


  cout << "********************************************" << endl;
  cout << "********************************************" << endl;
  cout << "********************************************" << endl;
  cout << "********************************************" << endl;
  cout << "********************************************" << endl;

  isTriangular = 0;

  ifstream inmat(filename.c_str(), ios::binary);
  if(inmat.fail()){
    cout << "Can not open bigmatrix : " << filename << endl;
    return 0;
  }
  cout << "   o bigmatrix '" << filename << "' opened ok " << endl;

  inmat.read((char*)&nrows, sizeof(nrows));
  if (newMat) inmat.read((char*)&version, sizeof(version));

  if (nrows<0) {
    nrows = -nrows;
    ncols = nrows;
    isTriangular = 1;
  }

  if (createPlot) {
    if (!reducePlot) {
      hBigMat = new TH2F("hBigMat","BigMatrix at input",
			 ncols, -0.5, ncols-0.5,
			 nrows, -0.5, nrows-0.5);
      
      hBigMat->GetYaxis()->SetTitle("DoFs");
      hBigMat->GetXaxis()->SetTitle("DoFs");
    }
    else {
      hBigMat = new TH2F("hBigMat","BigMatrix at input",
			 ncols/6, -0.5, (ncols/6)-0.5,
			 nrows/6, -0.5, (nrows/6)-0.5);
      
      hBigMat->GetYaxis()->SetTitle("modules");
      hBigMat->GetXaxis()->SetNdivisions(ncols/6);
      hBigMat->GetXaxis()->SetTitle("modules");
      hBigMat->GetYaxis()->SetNdivisions(nrows/6);
    }

    hBigMat->SetTitle("bigmatrix map");
    hBigMat->SetStats(kFALSE);
  }

  //  mat.ResizeTo(nrows,ncols);

  double melem=0;

 // square format
  if(!isTriangular) {
    for(int i=0; i<nrows; i++) {
      for(int j=0; j<ncols; j++) {
	inmat.read((char*)&melem, sizeof (melem));
	// mat(i,j)=melem;

	if (melem!=0.) occupancy++;
	// cout << "(" << i << "," << j << ") = " << melem << endl; cout << flush;

	if (createPlot) {
	  if (melem == 0)
	    hBigMat->Fill(j, (nrows-1)-i,  0);
	  else if (melem > 0.)
	    hBigMat->Fill(j, (nrows-1)-i,  1.);
	  else if (melem < 0.)
	    hBigMat->Fill(j, (ncols-1)-i, -1.);
	}
      }

    }
  }
  // triangular format
  else {
    int ii = 0;
    int jj = 0;

    int inrows = 0;
    int incols = 0;

    int counter = 0;
    int counter2 = 0;
    if (reducePlot) { inrows = nrows/6; incols = ncols/6; }
    else { inrows = nrows; incols = ncols; }

    for(int i=0; i<nrows; i++) {
      for(int j=0; j<=i; j++) {
	inmat.read((char*)&melem, sizeof (melem));
	// mat(i,j)=melem;
	// mat(j,i)=melem;

	if (melem!=0.) occupancy++;
	// cout << "(" << i << "," << j << ") = " << melem << endl; cout << flush;

	if (createPlot) {
	  if (reducePlot) {
	    if(counter == 0 && counter2 == 0) {
	      /*	    
			   cout << "(" << i << "," << j << ") / " 
			   << "(" << ii << "," << jj << ") / "
			   << "(" << jj << "," << (inrows-1)-ii << ") / "
			   << "= " << melem << endl;
	      */
	      if (melem == 0.) {
		hBigMat->Fill(jj, (inrows-1)-ii,  0);
		if(ii!=jj) hBigMat->Fill(ii, (inrows-1)-jj,  0);
	      }
	      else if (melem > 0.) {
		hBigMat->Fill(jj, (inrows-1)-ii,  1.);
		if(ii!=jj) hBigMat->Fill(ii, (inrows-1)-jj,  1.);
	      }
	      else if (melem < 0.) {
		hBigMat->Fill(jj, (incols-1)-ii, -1.);
		if(ii!=jj) hBigMat->Fill(ii, (inrows-1)-jj,  -1.);
	      }
	      jj++;
	      
	    }
	  }
	  else {
	    if (melem == 0.) {
	      hBigMat->Fill(j, (inrows-1)-i,  0);
	      if(i!=j) hBigMat->Fill(i, (inrows-1)-j,  0);
	    }
	    else if (melem > 0.) {
	      hBigMat->Fill(j, (inrows-1)-i,  1.);
	      if(i!=j) hBigMat->Fill(i, (inrows-1)-j,  1.);
	    }
	    else if (melem < 0.) {
	      hBigMat->Fill(j, (incols-1)-i, -1.);
	      if(i!=j) hBigMat->Fill(i, (inrows-1)-j,  -1.);
	    }
	  }
	}

	counter2++;
	if (counter2==6) counter2=0;

      }
      // ii++;
      // jj=0;
      counter++;
      if (counter==6) { ii++; jj=0; counter=0; counter2=0; }
    }
  }
  inmat.close();

  return 1;
}


//=====================================================================
// GetSummary()
//=====================================================================
void bigmatrix::GetSummary() {
  cout << "   o version = " << version << endl;
  cout << "   o nrows = " << nrows << endl;
  cout << "   o ncols = " << ncols << endl;
  cout << "   o entries = " << ncols*nrows << endl;
  if (isTriangular) cout << "   o bigmatrix is a triangular matrix" << endl;
  else cout << "   o bigmatrix is a square matrix" << endl;

  if (reducePlot) {
    cout << "  reduced plot:" << endl ;
    cout << "   o nrows = " << nrows/6 << endl;
    cout << "   o ncols = " << ncols/6 << endl;
    cout << "   o entries = " << (ncols/6)*(nrows/6) << endl;
  }

  cout << endl;
  cout << "***********************************" << endl;
  cout << "      Summary (bigmatrix class)    " << endl;
  cout << "***********************************" << endl;
  cout << " non-zero elements  = " << occupancy << endl;
  cout << " bigmatrix size = " << Size() << endl;
  cout << " bigmatrix occupancy = " << 100*occupancy/Size() << "%" << endl;
  cout << "***********************************" << endl;

  return;
}


//=====================================================================
// DrawLines()
//=====================================================================
void bigmatrix::DrawLines(TCanvas* canvas) {

  // setTitle font  
  gPad->Update();
  TPaveText *pt = (TPaveText*)(gPad->GetPrimitive("title"));
  pt->SetFillColor(kWhite);
  pt->SetTextFont(42);
  pt->SetTextFont(42);
  pt->SetTextSize(0.03);
  gPad->Modified();
  
  // Draw lines 
  TLine* lines = new TLine();
  lines->SetLineWidth(2);

  if (reducePlot) { ncols = ncols/6; }
  int structs = ncols/DoFs;

  switch (structs) {
  case 4:
    level = 1;
    break;
  case 7:
    level = 1.5;
    break;
  case 11:
    level = 1.6;
    break;
  case 31:
    level = 2;
    break;
  case 5832:
    level = 3;
    break;
  default:
    cout << " ERROR: unknown alignment level (or not implemented)";
  }

  cout << " -> Level " << level << " bigmatrix" << endl;

  float LastDoF = (ncols/structs)-0.5;

  for (int i=0; i<4; i++) {
    lines->DrawLine(LastDoF, -0.5, LastDoF, ncols-0.5);
    lines->DrawLine(-0.5, (ncols-1)-LastDoF, ncols-0.5, (ncols-1)-LastDoF);
    LastDoF += DoFs;
  }

  canvas->Update();

  return;
}

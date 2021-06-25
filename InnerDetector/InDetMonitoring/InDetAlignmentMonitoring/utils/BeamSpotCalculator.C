/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////
//  BeamSpotCalculator
////////////////////////
// Wrote by Javier Jimenez Pena

#include "BeamSpotCalculator.h"

//Use: Copy the script into the folder with the TotalMonitoring.root file or viceversa and just run it as "root BeamSpotCalculator.C"
//     You can also modify the inputfile value (below) to point the file you want to analyse.
//     It needs to be run with ROOT 5

void BeamSpotCalculator(string inputfile="TotalMonitoring.root", Double_t min = -125 , Double_t max = 125, string trkcollection = "AlignTracks_all")
{
  Initialize (inputfile, trkcollection);
  AnalyseData (min,max);
  SliceBySliceAnalysis(min,max);

  //compare results with TAG Beamspot information
  TagBeamSpot();

  //print the results:
  cout << " "<< endl;
  cout << "************************************************************" << endl;
  cout << " X_0 = " << GeneralParam[0]<< " +- " << GeneralParamErrors[0] << endl;
  cout << " m_x = " << GeneralParam[1]<< " +- " << GeneralParamErrors[1] << endl;
  cout << " Y_0 = " << GeneralParam[2]<< " +- " << GeneralParamErrors[2] << endl;
  cout << " m_y = " << GeneralParam[3]<< " +- " << GeneralParamErrors[3] << endl;
  cout << "************************************************************" << endl;


}

////////////////////////////////////////////
void Initialize (string inputfile, string trkcollection)
{
  cout << " ** BeamSpotCalculator::Initialize ** inputfile = " << inputfile.c_str() << endl;
  cout << "                               Trk Coll = " << trkcollection.c_str() << endl;
  _InputFile = new TFile (inputfile.c_str());
  if (_InputFile->IsZombie()) {
    cout << " ** BeamSpotCalculator::Initialize ** ERROR opening file " << inputfile << endl;
    exit(-1); // leave program
  }

  // retrieve the d0 histogram
  string hisname("IDAlignMon/");
  hisname.append(trkcollection.c_str());
  hisname.append("/GenericTracks/trk_d0_vs_phi0_z0");

  //cout << "    hisname = " << hisname.c_str() << endl;
  _hd0phi0z0 = (TH3F*)_InputFile->Get(hisname.c_str());
  
  if (_hd0phi0z0 != nullptr){
    _hd0phi0z0_prof = _hd0phi0z0->Project3DProfile("yx");
  } else {
    cout << " ** BeamSpotCalculator::Initialize ** ERROR ** histogram " << hisname.c_str() << " ** Not found ** " << endl;
    exit(-1); // leave program
  }
  cout << " ** BeamSpotCalculator::Initialize ** SUCCESSFUL **"<< endl;
  return;
}

////////////////////////////////////////////
void AnalyseData (Double_t min , Double_t max)
{
 
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(11);
  // fitting code should follow
  c1 = new TCanvas("c1","h0_profile_phi0_z0",800,600);
  // define TF2 holding the d0 calculation
  TF2 *BSFit = new TF2("BSFit", d0_function, 0., 6.2832, min , max , 4);
  BSFit->SetParNames("x_{0}", "m_{x}", "y_{0}" , "m_{y}");

  //Set the initial values by fitting the central slide.
  TF1 *BSFit_slice = new TF1("BSFit_slice", d0_function_slice, 0, 6.2832, 2);
  BSFit_slice->SetParNames("x_{0}", "y_{0}" );

  if (( _hd0phi0z0_prof->GetYaxis()->GetNbins())%2==0)  int initial= (( _hd0phi0z0_prof->GetYaxis()->GetNbins())/2);
  else  int initial= (( _hd0phi0z0_prof->GetYaxis()->GetNbins())+1)/2;
  
  _hd0_vs_phi0_prof=_hd0phi0z0_prof->ProjectionX("_hd0_vs_phi0_prof",initial,initial);    
  _hd0_vs_phi0_prof->Fit(BSFit_slice);

  // Use the obtained x0 and y0 values as initial values for the 2-D function, fix them, and fit to obtain m_x and m_y

  double slide_param[2];
  BSFit_slice->GetParameters(slide_param);

  BSFit->FixParameter(0,slide_param[0]);
  BSFit->FixParameter(2,slide_param[1]);

  //cout << "FIT CON PARAMETROS FIJADOS" << endl;

  _hd0phi0z0_prof->Fit(BSFit,"R");

  //cout << "FIN DEL FIT CON PARAMETROS FIJADOS" << endl;

  
  BSFit->GetParameters(GeneralParam);
  
  //Finally, the complete 2-D Fit is done.

  BSFit->ReleaseParameter(0);
  BSFit->ReleaseParameter(2);
  BSFit->SetParameter(0,GeneralParam[0]);
  BSFit->SetParameter(1,GeneralParam[1]);
  BSFit->SetParameter(2,GeneralParam[2]);
  BSFit->SetParameter(3,GeneralParam[3]);


  _hd0phi0z0_prof->Fit(BSFit,"R");
  cout << " ** BeamSpotCalculator::Initialize ** Fit chi2= " << BSFit->GetChisquare() 
       << "   NDoF= " <<  BSFit->GetNDF() 
       << "   Chi2/NDoF= " << BSFit->GetChisquare() / BSFit->GetNDF() 
       << endl;
  _hd0phi0z0_prof->Draw("colz"); 

  BSFit->GetParameters(GeneralParam);	// saves the parameters of the 2-D fit for later use in slice by slice analysis 
  GeneralParamErrors[0]= BSFit->GetParError(0);
  GeneralParamErrors[1]= BSFit->GetParError(1);
  GeneralParamErrors[2]= BSFit->GetParError(2);
  GeneralParamErrors[3]= BSFit->GetParError(3);


  return;
}

////////////////////////////////////////////
void SliceBySliceAnalysis (Double_t min , Double_t max)
{
  c2 = new TCanvas("c2","h0_projection_phi0_z0",200,0,1200,800);
  TF1 *BSFit_slice = new TF1("BSFit_slice", d0_function_slice, 0, 6.2832, 2);
  BSFit_slice->SetParNames("x_{0}", "y_{0}" );

  double param[2],param_error[2];	

  if (( _hd0phi0z0_prof->GetYaxis()->GetNbins())%2==0)  int initial= (( _hd0phi0z0_prof->GetYaxis()->GetNbins())/2);
  else  int initial= (( _hd0phi0z0_prof->GetYaxis()->GetNbins())+1)/2;

  //first iteration is made twice to catch the initial conditions.
  _hd0_vs_phi0_prof=_hd0phi0z0_prof->ProjectionX("_hd0_vs_phi0_prof",initial,initial);    
  _hd0_vs_phi0_prof->Fit(BSFit_slice);

  c2->Clear();
  c2->Divide(6,4);

  int num_of_slices = 21;

  int aaa= num_of_slices/2;
  int bbb = aaa + num_of_slices%2 ;
  

  for (int slice=initial-aaa; slice <=initial+bbb ;slice++) 
  {
    cout << " test slice " << slice << endl;  
    
    c2->cd(slice-initial+1 +aaa);

    m_z0[slice-initial+aaa]=_hd0phi0z0_prof->GetYaxis()->GetBinCenter(slice);
    m_z0_error[slice-initial+aaa]=(_hd0phi0z0_prof->GetYaxis()->GetBinWidth(slice))/2;
    
    _hd0_vs_phi0_prof=_hd0phi0z0_prof->ProjectionX("_hd0_vs_phi0_prof",slice,slice);    
    _hd0_vs_phi0_prof->Fit(BSFit_slice, "ME");
    
    BSFit_slice->GetParameters(param); 
    m_x0[(slice-initial+aaa)]=param[0]; 
    m_y0[(slice-initial+aaa)]=param[1]; 
    m_x0_error[(slice-initial+aaa)]=BSFit_slice->GetParError(0);
    m_y0_error[(slice-initial+aaa)]=BSFit_slice->GetParError(1);

    _hd0_vs_phi0_prof->DrawClone("colz");

   cout << "Z_{0} = " << m_z0[slice-initial+aaa] << " -> x0 = " << param[0] << endl;
    
   }

  TF1 *line = new TF1("line", "x*[1]+[0]", min, max);
  
  c3 = new TCanvas("c3","x0_vs_z0",500,0,800,600);
  TGraphErrors *x0_vs_z0 = new TGraphErrors(21,m_z0,m_x0,m_z0_error,m_x0_error);
  TGraphErrors *y0_vs_z0 = new TGraphErrors(21,m_z0,m_y0,m_z0_error,m_y0_error);

  line->SetParNames("x_{0}", "m_{x}");
  line->SetParameter(0,GeneralParam[0]);   
  line->SetParameter(1,GeneralParam[1]); 	// set the initial value of the slope equal to the 2-D fit m_x and the constant equal to x0


  c3->Divide(1,2);
  c3->cd(1);
  x0_vs_z0->Fit("line","RE");
  x0_vs_z0->DrawClone("AP");

  line->SetParNames("y_{0}" , "m_{y}");
  line->SetParameter(0,GeneralParam[2]); 	// set the initial value of the slope equal to the 2-D fit m_y and the constant equal to y0
  line->SetParameter(1,GeneralParam[3]); 

  c3->cd(2);
  y0_vs_z0->Fit("line","RE");
  y0_vs_z0->DrawClone("AP");


  return;
}

////////////////////////////////////////////

void TagBeamSpot()
{

  // retrieve the BeamSpot histograms
  string bsXstring("IDAlignMon/BeamSpot/NoTriggerSelection/bsX");
  string bsYstring("IDAlignMon/BeamSpot/NoTriggerSelection/bsY");
  string bsXtiltstring("IDAlignMon/BeamSpot/NoTriggerSelection/bsTiltX");
  string bsYtiltstring("IDAlignMon/BeamSpot/NoTriggerSelection/bsTiltY");
  
  _hbsX = (TH1F*)_InputFile->Get(bsXstring.c_str());
  _hbsY = (TH1F*)_InputFile->Get(bsYstring.c_str());
  _hbsTiltX = (TH1F*)_InputFile->Get(bsXtiltstring.c_str());
  _hbsTiltY = (TH1F*)_InputFile->Get(bsYtiltstring.c_str());

  c4 = new TCanvas("c4","TAG_BeamSpot_info",600,200,800,600);
  c4->Divide(2,2);
  c4->cd(1);
  _hbsX->Draw();
  _hbsX->SetAxisRange(GeneralParam[0]-20*GeneralParamErrors[0] , GeneralParam[0]+20*GeneralParamErrors[0]);

  DrawLine(0);

  c4->cd(2);
  _hbsY->Draw();
  _hbsY->SetAxisRange(GeneralParam[2]-20*GeneralParamErrors[2] ,GeneralParam[2]+20*GeneralParamErrors[2]);

  DrawLine(2);

  c4->cd(3);
  _hbsTiltX->Draw();
  _hbsTiltX->SetAxisRange((GeneralParam[1]-8*GeneralParamErrors[1])*1e6 ,(GeneralParam[1]+8*GeneralParamErrors[1])*1e6);

  DrawLineTilt(1);

  c4->cd(4);
  _hbsTiltY->Draw();
  _hbsTiltY->SetAxisRange((GeneralParam[3]-8*GeneralParamErrors[3])*1e6 ,(GeneralParam[3]+8*GeneralParamErrors[3])*1e6);

  DrawLineTilt(3);
  
  return;
}

///////////////////////////////////////

void DrawLine(int number)
{
  TLine *l = new TLine(GeneralParam[number],1,GeneralParam[number],1000000);
  l->Draw();
  l->SetLineColor(2);

  TLine *ld = new TLine(GeneralParam[number]-GeneralParamErrors[number],1,GeneralParam[number]-GeneralParamErrors[number],1000000);
  ld->Draw();
  ld->SetLineColor(3);

  TLine *lu = new TLine(GeneralParam[number]+GeneralParamErrors[number],1,GeneralParam[number]+GeneralParamErrors[number],1000000);
  lu->Draw();
  lu->SetLineColor(3);
  
  return;
}

void DrawLineTilt(int number)
{
  TLine *l = new TLine(1e6*GeneralParam[number],1,1e6*GeneralParam[number],1000000);
  l->Draw();
  l->SetLineColor(2);

  TLine *ld = new TLine(1e6*(GeneralParam[number]-GeneralParamErrors[number]),1,1e6*(GeneralParam[number]-GeneralParamErrors[number]),1000000);
  ld->Draw();
  ld->SetLineColor(3);

  TLine *lu = new TLine(1e6*(GeneralParam[number]+GeneralParamErrors[number]),1,1e6*(GeneralParam[number]+GeneralParamErrors[number]),1000000);
  lu->Draw();
  lu->SetLineColor(3);
  
  return;
}

////////////////////////////////////////////
double d0_function(double *x, double *par)
{
  double x0  = par[0];
  double m_x = par[1];
  double y0  = par[2];
  double m_y = par[3];

  double phi0 = x[0];
  double z0   = x[1];

  double myVal = -(x0 + m_x*z0) * sin(phi0) + (y0 + m_y*z0) * cos(phi0);

  return myVal;
}

double d0_function_slice(double *x, double *par)
{
  double x0 = par[0];
  double y0 = par[1];
 
  double phi0 = x[0];

  double myVal = -x0 * sin(phi0) + y0 * cos(phi0);

  return myVal;
}

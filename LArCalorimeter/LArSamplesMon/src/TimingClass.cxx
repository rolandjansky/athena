/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/TimingClass.h"
#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/History.h"
#include "LArSamplesMon/AbsShapeErrorGetter.h"
#include "LArSamplesMon/TreeShapeErrorGetter.h"
#include "LArSamplesMon/Data.h"
#include "LArSamplesMon/OFC.h"
#include "LArCafJobs/Geometry.h"

using namespace TMath;
using namespace std;

// *************************************** //
LArSamples::TimingClass::TimingClass() 
// *************************************** //
{
  
}



// **************************************** //
LArSamples::TimingClass::~TimingClass()
// **************************************** //
{
  
}


// ***************************************************************************** //
void LArSamples::TimingClass::timePerFebAllFebs(const std::string& nrun, const std::string& name)
// ***************************************************************************** //
{

  ofstream file; 
  string fname = "TimingFile" + nrun + "_" + name + ".txt"; 
  file.open(fname.c_str(), ios::out);

  for( uint i = 0; i < m_interface->nChannels(); i++ ){ 
    if( i%10000 == 0 ) cout << "Processing entry " << i << endl;
    if( m_interface->historySize(i) == 0 ) continue;
    const History* history = m_interface->cellHistory(i); // extract the history of the cells of each channel
    if( !history )continue;
      
    int calo = Abs(history->cellInfo()->calo());
    int caloPart = history->cellInfo()->calo();; 

    int det = 9;
    if( caloPart > 0 ) det = 1; //sideA
    if( caloPart < 0 ) det = 0; //sideC
    
    int layer = history->cellInfo()->layer();
    int ft = history->cellInfo()->feedThrough() ; 
    int slot = history->cellInfo()->slot() ; 
    
    for( uint j = 0; j < history->nData(); j++ ){
      //int gain = history->data(j)->gain();
      //int NumberRun = history->data(j)->run();
      //remove bad channels     
      if( history->data(j)->problems(true) != "None" ) continue; 
      //noise cut
      if( (history->data(j)->energy()/history->data(j)->noise()) < 5 ) continue; 
      //Time cut
      if( fabs( history->data(j)->ofcTime() ) > 20.00 )continue; 
      //Energy Threshold
      double t = history->data(j)->ofcTime(); //time 
      double e = history->data(j)->energy(); //energy
      if( !EnergyThreshold( calo, layer, history->data(j)->quality(), ft, slot, e, t ) )continue;
      //Fill the timing distributions with energy weighting
      //if( calo <= 3 && gain == 0 ) h[det][ft][slot]->Fill( t,e ); 
      //if( ( calo == 4 && gain == 1 ) || ( calo == 5 && gain == 1 && NumberRun < 216867 ) || ( calo == 5 && gain == 0 && NumberRun >= 216867) )h[det][ft][slot]->Fill( t,e ); 
      //Print to file
      int detec = -1;
      if( name == "EMB" ) detec = 0; 
      else if( name == "EMEC" ) detec = 1;
      else if( name == "HEC" ) detec = 2; 
      else if( name == "FCAL" ) detec = 3; 
      file << detec << " " << det << " " << ft << " " << slot << " " << t << " "<< e << endl; 
    }    
  }
  

  return;

}

// ********************************************************************************** //
void LArSamples::TimingClass::fitTimePerFebAllFebs(const std::string& nrun, const std::string& name)
// ********************************************************************************** //
{
  
  for( int d = 0; d < 2; d++ ){ //side 
    for( int ft = 0; ft < 32; ft++ ){ //feedthrough
      for( int sl = 0; sl < 15; sl++ ) //slot
        Median[d][ft][sl] = -99.;
    }
  }  
  
  TH1F  *timefeb[2][32][16];
  TF1   *fit1 = new TF1( "fit1" , "gaus" , 3 );
  
  double Hist_entries, Fit_mean, Fit_sigma, Fit_error; 
  
  std::vector< std::vector<double> > myvec;
  string tfilename; 
  tfilename = "TimingFile" + nrun + "_" + name + ".txt"; 
  myvec = readTimingFiles(tfilename);

  string Filename = "OFCTime_PerFEB_" + name + ".root"; 
  TFile *FebTime = new TFile( Filename.c_str(), "READ"); 

  double median2, TotalE = 0.0; 
  int dete = -1, side, feedT, slot;

  std::vector<double> myvector; 
  std::vector<double> myvector2;
  std::vector<double> energy;

  ofstream ff;
  string ffname = "mediantest_" + nrun + "_" + name + ".txt";  
  ff.open(ffname.c_str(), ios::out);

  for( uint l = 0; l < (myvec.size() -1); l++ ){
    float mean = myvec[l][4]; 
    double E = myvec[l][5];
    myvector.push_back(mean); 
    myvector2.push_back(mean);
    energy.push_back(E);
    TotalE = TotalE + E; 
    //Fill the vector when the number of slot changes or when it reaches the end of the file
    if( ( myvec[l][3] != myvec[l+1][3] ) || ( l == myvec.size()-2 ) ){ 
      //Get the median of the time for each feb (defined by its slot and ft) 				
      median2 = getTimeWeightedMedian(myvector, myvector2, energy, TotalE);			
      myvector.clear(); 
      myvector2.clear(); 
      energy.clear(); 
      TotalE = 0; 
      side = int(myvec[l][1]);
      feedT = int(myvec[l][2]);
      slot = int(myvec[l][3]);
      if( name == "EMB" ) dete = 0; 
      else if( name == "EMEC" ) dete = 1;
      else if( name == "HEC" ) dete = 2; 
      else if( name == "FCAL" ) dete = 3; 
      ff << dete << " " << side << " " << feedT << " " << slot << " " << median2 << endl;			
      Median[side][feedT][slot] = median2;		
    } 	      
  } 
  
  string savename = "FEB_time_fitMean_" + nrun + "_" + name + ".txt"; 
  ofstream file; 
  file.open(savename.c_str(), ios::out); 
  
  double median_error; 
  //double mean_error;
  std::vector<double> timevector; 
  
  int det = -1;   
  if( name == "EMB" ) det = 0; 
  else if( name == "EMEC" ) det = 1;
  else if( name == "HEC" ) det = 2; 
  else if( name == "FCAL" ) det = 3; 
  
  int count0 = 0, count1 = 0, count2 = 0, count3 = 0;
   
  for( int d = 0; d < 2; d++ ){ //sides
    for( int ft = 0; ft < 32; ft++ ){ //feedthrough
      for( int sl = 0; sl < 15; sl++ ){ //slot
	
	ostringstream histname; 
	histname << "h_" << name.c_str() << "_" << d << "_" << ft << "_" << sl+1;         
	timefeb[d][ft][sl+1] = (TH1F*)FebTime->Get(histname.str().c_str());
	if( !timefeb[d][ft][sl+1] )
          break;
	
	Hist_entries = timefeb[d][ft][sl+1]->GetEntries();
	//Fit only is histograms are full
	if( Hist_entries == 0 ) continue; 

	timefeb[d][ft][sl+1]->Fit( "fit1","mE","N",-10, 10 ); 	

	//double M = fit1->GetParameter(1);
	double rms, rmsDist; 	  
	rms = fit1->GetParameter(2);
	double n1 = -99., n2 = -99.; 
	rmsDist=timefeb[d][ft][sl+1]->GetRMS(); 
	double max;
	max = timefeb[d][ft][sl+1]->GetXaxis()->GetBinCenter(timefeb[d][ft][sl+1]->GetMaximumBin());	
	
	if( name == "FCAL" ) n1 = n2 = 0.8;	   
	else if( name == "EMEC" ){
	  if( sl+1 == 13 && rmsDist > 3.0 ){ 
	    n1 = 1.5; n2 = 1.; 
	  }
	  else if( rmsDist > 3. && sl+1 > 13 ){	 	   
	    n1 = 1.5; n2 = 0.3; 	    
	  } 
	  else if( sl+1 == 9 && ( ft == 2 || ft == 9 || ft == 15 || ft == 21 ) )
	    n1 = n2 = 3; 
	  else if( ( sl+1 == 8 || sl+1 == 9 ) && rmsDist > 3.0 )
	    n1 = n2 = 2; 
	  else if( sl+1 == 8 && ft == 13 ) 
	    n1 = n2 = 3; 
	  else 
	    n1 = n2 = 1.8;
	}	   
	else if( name == "HEC" ) n1 = n2 = 1.5;
	if( name == "EMB"){
	  if( sl+1 == 9 || sl+1 == 10 ){
	    n1 = 3; n2 = 2; 	    
	  }
	  else if( sl+1 >= 2 && sl+1 < 6 ){	  
	    n1 = 1.5; n2 = 2.5;
	  }
	  else if( sl+1 == 14 ){
	    n1 = 2; n2 = 1;
	  }
	  else
	    n1 = n2 = 1.8;
	}	  
	 
	timefeb[d][ft][sl+1]->Fit("fit1","mE","N", max-n1*rms, max+n2*rms);
	Fit_mean = fit1->GetParameter(1);
	Fit_sigma = fit1->GetParameter(2);
	Fit_error = fit1->GetParError(1);      	 	      
        
	median_error = 1.253*timefeb[d][ft][sl+1]->GetRMS()/sqrt(Hist_entries); 
	//mean_error = timefeb[d][ft][sl+1]->GetRMS()/sqrt(Hist_entries);
	
	if( Hist_entries <= 50 ){
	  file << det << " " << d << " " << ft << " " <<  sl+1 << " " << Median[d][ft][sl+1] << " " << median_error << endl;  
	  count1++;	
	}	   	   
	else if( gMinuit->fCstatu == "SUCCESSFUL" && Fit_error >= Fit_mean &&  Fit_sigma > 1.5*rmsDist ){ 
	  file << det << " " << d << " " << ft << " " <<  sl+1 << " " << Median[d][ft][sl+1] << " " << median_error << endl;	    
	  count0++; 
	}	 		 	 
	else if( gMinuit->fCstatu != "SUCCESSFUL" ){	 	  	  	  
	  file << det << " " << d << " " << ft << " " <<  sl+1 << " " << Median[d][ft][sl+1] << " " << median_error << endl;  
	  count2++;	     
	}	  	    	    
	else{
	  file << det << " " << d << " " << ft << " " <<  sl+1 << " " << Fit_mean << " " << Fit_error << endl;
	  count3++;
	}		    	     
	
      } 
    }
  }

  
  ff.close();
  file.close();
  
  return;

}


/************************************************************/
void LArSamples::TimingClass::Time(int dete, const std::string& nrun)
/************************************************************/
{
  
  TH1F *h = new TH1F( Form("h_%d", dete) , Form("h_%d", dete) , 160, -20, 20 );
  h->Sumw2();

  TH1F *h1 = new TH1F( Form("h1_%d", dete) , Form("h1_%d", dete) , 100, 0, 15000 );
  TH2F *h2 = new TH2F( Form("h2_%d", dete) , Form("h2_%d", dete) , 100, 0, 15000, 160, -20, 20 );


  for( uint i = 0; i < m_interface->nChannels(); i++ ){ 
    if( m_interface->historySize(i) == 0 )continue;
    //Extract the history of the cells of each channel
    const History* history = m_interface->cellHistory(i);
    if( !history )continue;
     
    int calo = Abs( history->cellInfo()->calo() );
    //int caloPart = history->cellInfo()->calo(); 
    int layer = history->cellInfo()->layer();
    int slot = history->cellInfo()->slot(); 
    int ft = history->cellInfo()->feedThrough();
    //int detIndex = 1; 
    //if( calo == 1 )detIndex = 0;
    //int side = 1; 
    //if( caloPart < 0 )side = 0; 
    //int channel = history->cellInfo()->channel();
     
    
    for( uint j = 0; j < history->nData(); j++ ){
      int Gain = history->data(j)->gain();
      int NumberRun = history->data(j)->run();
      //Bad Channels
      if( history->data(j)->problems(true) != "None" ) continue; 
      //Noise cut
      if( ( history->data(j)->energy() / history->data(j)->noise() ) < 5 )continue; 
      //Time cut
      if( fabs( history->data(j)->ofcTime() ) > 20 )continue;
      //Energy Threshold
      double t = history->data(j)->ofcTime(); //time 
      double e = history->data(j)->energy(); //energy
      if( !EnergyThreshold( calo, layer, history->data(j)->quality(), ft, slot, e, t ) )continue;
      //Fill distributions
      if( ( calo <= 3 && Gain == 0 ) || ( calo == 4 && Gain == 1 ) || ( calo == 5 && Gain == 1 && NumberRun < 216867 ) || ( calo == 5 && Gain == 0 && NumberRun >= 216867 ) ) h->Fill( t, e );	
      if( ( calo <= 3 && Gain == 0 ) || ( calo == 4 && Gain == 1 ) || ( calo == 5 && Gain == 1 && NumberRun < 216867 ) || ( calo == 5 && Gain == 0 && NumberRun >=216867 ) ) h1->Fill( history->data(j)->quality() );	
      if( ( calo <= 3 && Gain == 0 ) || ( calo == 4 && Gain == 1 ) || ( calo == 5 && Gain == 1 && NumberRun < 216867 ) || ( calo == 5 && Gain == 0 && NumberRun >= 216867) ) h2->Fill( history->data(j)->quality(), t );	
    }
  }
  
  TCanvas *c = new TCanvas( Form("c_%d_%s", dete, nrun.c_str()), Form("c_%d_%s", dete,  nrun.c_str()), 129,165,700,600 );
  c->cd();
  h->SetTitle(Form("hist_%d", dete) );
  h->SetLineColor(1);
  h->SetLineWidth(2);
  h->GetXaxis()->SetTitle("ofcTime [ns]");
  h->SetMarkerSize(0);
  
  TF1 *fit = new TF1("fit", "gaus", 3);
  fit->SetParameter(1, h->GetMean());
  fit->SetParameter(2, h->GetRMS());
  fit->SetParLimits(1, -10, 10);
  fit->SetParLimits(2, 0, 10);
  
  double mean, rms; 
  mean = h->GetMean(); 
  rms =  h->GetRMS();
  h->Fit("fit", "", "", mean-rms, mean+0.5*rms);
  h->Draw("histsame""E2");
  
  TFile *fi = new TFile( Form("Time_%d_%s.root", dete, nrun.c_str()), "RECREATE" );
  h->Write();
  fit->Write();
  h1->Write();
  h2->Write();
  fi->Close();
  
  TString path = "Plots/"; 
  c->Print( path+Form("Time_%d_%s.eps", dete, nrun.c_str()) );
  c->Print( path+Form("Time_%d_%s.png", dete, nrun.c_str()) );
  
  return;

}


// ******************************************************************************* //
void LArSamples::TimingClass::PlotFebAverageTime(const std::string& nrun, const std::string& name)
// ******************************************************************************* //
{
  string Filename = "FEB_time_fitMean_" + nrun + "_" + name + ".txt"; 
  
  ifstream f( Filename.c_str(), ios::in );
  vector<double> mean; 
  vector<int> side; 
  
  while( !f.eof() ){       
    int n1, n2, n3, n4;
    double n5, n6;
    
    f >> n1 >> n2 >> n3 >> n4 >> n5 >> n6; 
    
    if( f.good() && !f.bad() && !f.fail() ){
      mean.push_back(n5);
      side.push_back( n2 );
    }
  }
  
  f.close();	   
  
  TH1F *h = new TH1F( Form("FEB_Av_%s", name.c_str()), Form("FEB_Av_%s", name.c_str()), 120, -15., 15. );
  TH1F *sAh = new TH1F( Form("FEB_Av_%s_sideA", name.c_str()), Form("FEB_Av_%s_sideA", name.c_str()), 120, -15., 15. );
  TH1F *sCh = new TH1F( Form("FEB_Av_%s_sideC", name.c_str()), Form("FEB_Av_%s_sideC", name.c_str()), 120, -15., 15. );

  for( unsigned int i=0; i<mean.size(); i++ ){
    h->Fill( mean[i] ); 
    //Fill sides
    if( side.at(i) == 1 )  //A
      sAh->Fill( mean[i] ); 
    else //C
      sCh->Fill( mean[i] ); 
  }
  
  h->Sumw2();
  sAh->Sumw2();
  sCh->Sumw2();
  
  TCanvas *c = new TCanvas( Form("c_FEB_Av_%s", name.c_str()), Form("c_FEB_Av_%s", name.c_str()), 129,165,700,600 );
  c->cd();
  c->SetLogy();
  h->GetXaxis()->SetTitle("<t_{FEB}> [ns]"); 
  h->GetYaxis()->SetTitle("Number of FEBs / 0.25 ns");
  h->Draw("hist");
  
  TString path = "Plots/";  
  c->Print( path+ Form("t_FEB_%s.png", name.c_str()) );
  c->Print( path+ Form("t_FEB_%s.eps", name.c_str()) );  
  c->Print( path+ Form("t_FEB_%s.pdf", name.c_str()) );
  
  TCanvas *sAc = new TCanvas( Form("c_FEB_Av_%s_sideA", name.c_str()), Form("c_FEB_Av_%s_sideA", name.c_str()), 129,165,700,600 );
  sAc->cd();
  sAc->SetLogy();
  sAh->GetXaxis()->SetTitle("<t_{FEB}> [ns]"); 
  sAh->GetYaxis()->SetTitle("Number of FEBs / 0.25 ns");
  sAh->Draw("hist");
  sAc->Print( path+ Form("t_FEB_%s_sideA.png", name.c_str()) );
  
  TCanvas *sCc = new TCanvas( Form("c_FEB_Av_%s_sideC", name.c_str()), Form("c_FEB_Av_%s_sideC", name.c_str()), 129,165,700,600 );
  sCc->cd();
  sCc->SetLogy();
  sCh->GetXaxis()->SetTitle("<t_{FEB}> [ns]"); 
  sCh->GetYaxis()->SetTitle("Number of FEBs / 0.25 ns");
  sCh->Draw("hist");
  sCc->Print( path+ Form("t_FEB_%s_sideC.png", name.c_str()) );


  std::string plotname = "FEB_average_" + nrun + "_" + name + ".root";
  TFile *fi = new TFile( plotname.c_str(), "RECREATE" ); 
  h->Write();
  fi->Close();
  
  return;

}


// ******************************************************* //
void LArSamples::TimingClass::MergeFebTime( const std::string& nrun )
// ******************************************************* //
{
  
  ofstream mergedfile;
  string name = "FEB_time_fitMean_" + nrun + ".txt";
  mergedfile.open( name.c_str(), ios::out );
 
  string detparts[4] = {"EMB", "EMEC", "HEC", "FCAL"}; 
  
  for( int i = 0; i < 4; i++ ){
    string tmpname = detparts[i]; 
    string file = "FEB_time_fitMean_" + nrun + "_" + tmpname + ".txt";    
    
    ifstream f( file.c_str(), ios::in ); 
    while( !f.eof() ){	 
      
      int n1, n2, n3, n4;
      double n5, n6;
      
      f >> n1 >> n2 >> n3 >> n4 >> n5 >> n6; 
      
      if( f.good() && !f.bad() && !f.fail() )
	mergedfile << n1 << " " << n2 << " " << n3 << " " << n4 << " "  << n5 << " " << n6 << endl;   
    }	    	   	    
            
    f.close();      
  }
 
  mergedfile.close(); 
  
 return;

}


// ****************************************************************************** //
void LArSamples::TimingClass::getFebCorrection( const std::string& nrun )
// ****************************************************************************** //
{ 
  // The structure of the Feb correction file is different than what we need
  // The corrections are needed for each slot and not for each feed through
  // This function rearranges the previously obtained feb correction file to put it in the righ format
  
  for( int i = 0; i < 4; i++ ){ 
    for( int j = 0; j < 2; j++ ){ 
      for( int k = 0; k < 16; k++ ){
	for( int m = 0; m < 32; m++ ){
	  param[i][j][m][k] = -99.;
	  error[i][j][m][k] = -99; 	       
	}
      }
    }
  }         
  
  ofstream febCorrfile;
  string name = "FEB_Corr_" + nrun + ".txt";
  febCorrfile.open( name.c_str(), ios::out );
  
  string name2 = "FEB_time_fitMean_"+nrun+".txt";
  ifstream f( name2.c_str(), ios::in ); 
           
  if( f ){
    while( !f.eof() ){	 
      int n1, n2, n3, n4;
      double n5, n6;
      
      f >> n1 >> n2 >> n3 >> n4 >> n5 >> n6; 
      
      if( f.good() && !f.bad() && !f.fail() ){
	param[n1][n2][n3][n4] = n5;  
	error[n1][n2][n3][n4] = n6;    	       	      	    	     	      	        
      }	    	   	    
    }
  }
  
  f.close();
  
  string HECFEBID[48] = {"0x3a1a0000", "0x3a520000", "0x3a820000", "0x3ab20000", "0x3a1a8000", "0x3a528000", "0x3a828000", "0x3ab28000", "0x3a1b0000", "0x3a530000", "0x3a830000", "0x3ab30000", "0x3a1b8000", "0x3a538000", "0x3a838000", "0x3ab38000", "0x3a1c0000", "0x3a540000", "0x3a840000", "0x3ab40000", "0x3a1c8000", "0x3a548000", "0x3a848000", "0x3ab48000", "0x3b1a0000", "0x3b520000", "0x3b820000", "0x3bb20000", "0x3b1a8000", "0x3b528000", "0x3b828000", "0x3bb28000", "0x3b1b0000", "0x3b530000", "0x3b830000", "0x3bb30000", "0x3b1b8000", "0x3b538000", "0x3b838000", "0x3bb38000", "0x3b1c0000", "0x3b540000", "0x3b840000", "0x3bb40000", "0x3b1c8000", "0x3b548000", "0x3b848000", "0x3bb48000" };
  string FCALFEBID[28] = {"0x3a300000", "0x3a308000", "0x3a310000", "0x3a318000", "0x3a320000", "0x3a328000", "0x3a330000", "0x3a340000", "0x3a348000", "0x3a350000", "0x3a358000", "0x3a360000", "0x3a368000", "0x3a370000", "0x3b300000", "0x3b308000", "0x3b310000", "0x3b318000", "0x3b320000", "0x3b328000", "0x3b330000", "0x3b340000", "0x3b348000", "0x3b350000", "0x3b358000", "0x3b360000", "0x3b368000", "0x3b370000" }; 

  ofstream corrfile; 
  string nameid = "FEB_Corr_FEBID_" + nrun + ".txt";   
  corrfile.open( nameid.c_str(), ios::out );
  
  int kk = 0;
  for( int i = 0; i < 4; i++ ){
    kk = 0;
    for( int j = 0; j < 2; j++ ){
      for( int k = 0; k < 16; k++ ){
	for( int m = 0; m < 32; m++ ){
	  if( param[i][j][m][k] != -99. && error[i][j][m][k] != -99.){ 	    
	    febCorrfile << i << " " << j << " " << k << " " << m << " " << param[i][j][m][k] << " " << error[i][j][m][k] << endl;
	    if( i < 2 )corrfile <<  "0x" << std::hex << (0x38000000|i<<25|j<<24|m<<19|(k-1)<<15)  << " " << std::dec <<  param[i][j][m][k] << endl;   
	    else if( i == 2 )corrfile << HECFEBID[kk] << " " <<  param[i][j][m][k] << endl;  
	    else corrfile << FCALFEBID[kk] << " " <<  param[i][j][m][k] << endl; 
	    kk++;
	  }
	}
      }
    }
  }         
  
  febCorrfile.close();  
  corrfile.close();
  
  return;

}


// *************************************** //
void LArSamples::TimingClass::PlotFebtime()
// *************************************** //
{
  //Plot the average feb time per feed through and per slot 
  TGraphErrors **g = new TGraphErrors*[15];   
  TGraphErrors **g1 = new TGraphErrors*[15];   
  TGraphErrors **g2 = new TGraphErrors*[15];   
  TGraphErrors **g3 = new TGraphErrors*[15];   
  TGraphErrors **g4 = new TGraphErrors*[15];   
  TGraphErrors **g5 = new TGraphErrors*[15]; 
  TGraphErrors **g6 = new TGraphErrors*[15]; 
     
  TGraph** gzero = new TGraph*[15];   
  TCanvas **c = new TCanvas*[4];
  TLatex *l = new TLatex();   
  l->SetTextSize(0.065);
  
  int count = 0, count1 = 0, count2 = 0, count3 = 0, count4 = 0, count5 = 0, count6 = 0;
  TString path = "Plots/";  
  
  for( int det = 0; det < 4; det++ ){  
    //Plot EMEC special crates
    int special = 1;
    if( det == 1 )special = 2;
    for( int crates = 0; crates < special; crates++ ){
      string cratename = "Standard";
      if( crates == 1 ) cratename = "Special"; 
      //
      for( int side = 0; side < 2; side++ ){
	c[side] = new TCanvas( Form("c%d%d%s", det, side, cratename.c_str() ), Form("c%d%d%s", det,side, cratename.c_str()), 1100, 600 );
	c[side]->Divide( 5, 3 );
	
	for( int i = 0; i < 15; i++ ){   
	  g[i] = new TGraphErrors(); 
	  g1[i] = new TGraphErrors(); 
	  g2[i] = new TGraphErrors(); 
	  g3[i] = new TGraphErrors(); 
	  g4[i] = new TGraphErrors(); 
	  g5[i] = new TGraphErrors();
	  g6[i] = new TGraphErrors();
	  
	  gzero[i] = new TGraph();
	  
	  for( int j = 0; j < 32; j++ ){           
	    if( det == 1 && crates == 0 && ( j == 2 || j == 3 || j == 9 || j == 10 || j == 15 || j == 16 || j ==21 || j == 22 ) )continue;
	    if( det == 1 && crates == 1 && !( j == 2 || j == 3 || j == 9 || j == 10 || j == 15 || j == 16 || j ==21 || j == 22 ) )continue; 
	    
	    gzero[i]->SetPoint( j, j, 0 );      
	    if( param[det][side][j][i+1] == -99 )continue;
	    //
	    count++;
	    g[i]->SetPoint( count-1, j, param[det][side][j][i+1] ); 
	    g[i]->SetPointError( count-1, 0, error[det][side][j][i+1] ); 
	    //
	    if( det == 0 ){
	      g[i]->SetMarkerColor(4);
	      g[i]->GetYaxis()->SetRangeUser(-5, 5); 	
	    } 
	    else{
	      if( j == 6 ){ 
		count1++;
		g1[i]->SetPoint(count1-1, j, param[det][side][j][i+1]);  
		g1[i]->SetPointError(count1-1, 0, error[det][side][j][i+1]); 
		g1[i]->SetMarkerColor(4);      
	      }       
	      else if( j == 3 || j == 10 || j == 16 || j == 22 ){ 
		if( i >= 4 && i <= 9 ){ 
		  count2++;
		  g2[i]->SetPoint(count2-1, j, param[det][side][j][i+1]);
		  g2[i]->SetPointError(count2-1, 0, error[det][side][j][i+1]);
		  g2[i]->SetMarkerColor(3);
		}
		else if( i == 0 || i == 1){ 
		  count3++;
		  g3[i]->SetPoint(count3-1, j, param[det][side][j][i+1]); 
		  g3[i]->SetPointError(count3-1, 0, error[det][side][j][i+1]);
		  g3[i]->SetMarkerColor(6);      
		}
	      }
	      else if( j == 2 || j == 9 || j == 15 || j == 21 ){ 
		count4++;
		g4[i]->SetPoint(count4-1, j, param[det][side][j][i+1]); 
		g4[i]->SetPointError(count4-1, 0, error[det][side][j][i+1]);
		g4[i]->SetMarkerColor(2); 
	      }
	      else if( i <= 12 ){ 
		if( j%2 ){
		  count5++;
		  g5[i]->SetPoint(count5-1, j, param[det][side][j][i+1]); 
		  g5[i]->SetPointError(count5-1, 0, error[det][side][j][i+1]);	   
		  g5[i]->SetMarkerColor(1+12*(j%2));
		}
		else{
		  count6++;
		  g6[i]->SetPoint(count6-1, j, param[det][side][j][i+1]);
		  g6[i]->SetPointError(count6-1, 0, error[det][side][j][i+1]);
		  g6[i]->SetMarkerColor(1+12*(j%2));
		}
	      }
	      g[i]->GetYaxis()->SetRangeUser(-5, 5);          
	    }     
	  }
	  
	  count = 0; count1 = 0; count2 = 0; count3 = 0; count4 = 0; count5 = 0; count6 = 0;
	  
	  c[side]->cd(i+1);
	  g[i]->GetXaxis()->SetTitle("FT");
	  g[i]->GetYaxis()->SetTitle("ofcTime[ns]");    
	  g[i]->GetXaxis()->SetLabelSize(0.065);
	  g[i]->GetYaxis()->SetLabelSize(0.065);
	  g[i]->SetMarkerSize(0.4);  
	  g1[i]->SetMarkerSize(0.4);   
	  g2[i]->SetMarkerSize(0.4);   
	  g3[i]->SetMarkerSize(0.4);   
	  g4[i]->SetMarkerSize(0.4); 
	  g5[i]->SetMarkerSize(0.4);  
	  g6[i]->SetMarkerSize(0.4); 
	  g[i]->SetTitle( Form("#color[2]{#bf{Slot %d}}", i+1 ) );
	  g[i]->Draw("AP");
	  g1[i]->Draw("P");
	  g2[i]->Draw("P");
	  g3[i]->Draw("P");  
	  g4[i]->Draw("P");  
	  g5[i]->Draw("P");
	  g6[i]->Draw("P");
	  
	  gzero[i]->SetMarkerSize(0.2);
	  gzero[i]->Draw("P");
	  if( i == 14 ){
	    string detname = "EMB";
	    if( det == 1 )detname = "EMEC";
	    else if( det == 2 )detname = "HEC";
	    else if( det == 3 )detname = "FCAL";
	    c[side]->Print( path+ Form("t_FEB_side%d_%s_%s.png", side, detname.c_str(), cratename.c_str()));
	  }
	}
      }
    }
  }
 
  return;

}


// **************************************************************************************************************************** //
bool LArSamples::TimingClass::EnergyThreshold( int calo, int layer, int quality, int ft, int slot, double energy, double time )
// **************************************************************************************************************************** //
{ 

  bool pass = true;
  if( quality > 4000 ) pass = false; 

  //
  if( layer == 0 ) 
    if( fabs( time ) > 10.00 )pass = false;
  
  //
  if( calo == 1 ){	  	  
    if( layer == 0 && energy < 1000. ) pass = false;
    if( layer == 1 && energy < 1000. ) pass = false;
    if( layer == 2 && energy < 3000. ) pass = false;
    if( layer == 3 && energy < 1500. ) pass = false;
  }
	  
  //
  if( calo == 2 || calo == 3 ){
    if( layer == 0 && energy < 1500. ) pass = false;
    if( layer == 1 ){
      if( calo == 3 && energy < 3000. ) pass = false;
      if( calo == 2 && energy < 1000. ) pass = false;
    }
	  
    if( layer == 2 ){
      if( calo == 3 && energy < 2000. ) pass = false;
      if( calo == 2 && energy < 3000. ) pass = false;
    }

    if( layer == 3 && energy < 2000. ) pass = false;
    if( layer == 3 && quality > 10000 ) pass = false;
    if( slot == 13 && ( ft == 2 || ft == 9 || ft == 15 || ft == 21 ) && quality > 4000 ) pass = false;
    if( slot == 14 && ( ft == 2 || ft == 9 || ft == 15 || ft == 21 ) && quality > 4000 ) pass = false;
    if( slot == 15 && ( ft == 2 || ft == 9 || ft == 15 || ft == 21 ) && quality > 4000 ) pass = false;
  }
  
  if( calo == 4 && energy < 3500. ) pass = false;
  if( calo == 5 && energy < 10000. ) pass = false;
 
  return pass;

}

// ******************************************************************************* //
vector< vector<double> > LArSamples::TimingClass::readTimingFiles(const std::string& file)
// ******************************************************************************* //
{  

  std::vector< std::vector <double> > Data; 
  
  ifstream f( file.c_str() );
  
  TH1F * h[2][32][16]; 

  string name;
  if( file.find( "EMB" ) < file.length() )name = "EMB";
  else if( file.find( "EMEC" ) < file.length() )name = "EMEC";
  else if( file.find( "HEC" ) < file.length() )name = "HEC";
  else if( file.find( "FCAL" ) < file.length() )name = "FCAL";

  for( int d = 0; d < 2; d++ ){ //sides
    for( int ft = 0; ft < 32; ft++ ){ //feedthrough
      for( int sl = 0; sl < 15; sl++ ){ //slots
	h[d][ft][sl+1] = new TH1F( Form("h_%s_%d_%d_%d", name.c_str(), d, ft, sl+1) , Form("h_%s_%d_%d_%d", name.c_str(),d,ft, sl+1), 200, -20, 20 ); 
	h[d][ft][sl+1]->Sumw2();
      }
    }
  }
  


  if( !f )cerr << "Timing File " << file << "cannot be read" << endl;
  else {
    while( !f.eof() ){		
      int n1, n2, n3, n4;
      double n5, n6; 
      
      if( f.good() && !f.bad() && !f.fail() ){
	f >> n1 >> n2 >> n3 >> n4 >> n5 >> n6;     
	//
	std::vector<double> tmp;	  
	tmp.push_back(n1);
	tmp.push_back(n2);
	tmp.push_back(n3);
	tmp.push_back(n4);
	tmp.push_back(n5);    
	tmp.push_back(n6);   
	Data.push_back(tmp);	  
	//
	h[n2][n3][n4]->Fill( n5, n6 );   
      }	    	   	    
    }
  }      
  
  f.close();    
  
  
  //Save the Histograms
  string filename = "OFCTime_PerFEB_" + name + ".root";
  TFile *ff = new TFile( filename.c_str(), "RECREATE" );
  
  for( int d = 0; d < 2; d++ ){ //side
    for( int ft = 0; ft < 32; ft++ ){ //feedthrough
      for( int sl = 0; sl < 15; sl++ ) //slot
	h[d][ft][sl+1]->Write();
    }
  }
  
  ff->Close();



  return Data;

}


// ********************************************************************************************************************************************** //
double LArSamples::TimingClass::getTimeWeightedMedian(std::vector<double> time, std::vector<double> time2, std::vector<double> weight, double totalW)
// ********************************************************************************************************************************************* //
{ 
  
  TGraph *g = new TGraph();
  double wmedian = -99., weights, w0 = -1., cumulWeights = 0.0; 
  int Size = time.size();
  bool wcondition = false; 
   
  sort( time.begin(), time.end() );
   
  for( int i = 0; i < Size; i++ ){
    int pos = std::find( time2.begin(),time2.end(), time[i] ) - time2.begin(); 
    weights = weight[pos] / totalW;
    cumulWeights = cumulWeights + weights;
    g->SetPoint( g->GetN(), cumulWeights, time[i] );	
    if( i == 0 ) w0 = weight[pos] / totalW; 
    wcondition = w0 > 0.5;      
  }
 
  if( wcondition ) wmedian = g->Eval(w0+(1-w0)/2);
  else wmedian= g->Eval(0.5);
  
  g->Delete();
  
  return wmedian; 

}

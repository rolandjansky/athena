/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <vector>
#include <string>
#include <stdint.h>

#include "TileByteStream/TileOFC.h"

using namespace std;

void Double2Int_calib( double calib, unsigned int &scale, unsigned int &offset, bool verbose )
{
  // Number of bits of the integer word (signed -1 )
  int NumberBits = 16;
  NumberBits = NumberBits - 1;
  
  // Get Scale
  scale = 0;
  double max = fabs(calib);
  if (max!=0) scale = (unsigned int) truncf(log((pow(2.,NumberBits)-1.)/max)*(1./log(2.)));
  
  // Convert to integer 
  offset = (unsigned int) roundf(calib*pow(2.,(int)scale));
  
  if (verbose) cout << "Calib = " << hex << calib 
		    << " ==> ( scale: " << scale 
		    << " , offset: " << offset 
		    << ")" << dec << endl;
  
}



void Double2Int_ofc( int w_off_size, vector<double> w_off, vector<int> &w_dsp, int &w_sum_dsp, int &scale, bool verbose )
{
  // Number of bits of the integer word (signed -1 )
  int NumberBits = 16;
  NumberBits = NumberBits - 1;
  
  // Get Absolute Maximum
  double max = -10000.;
  double sum = 0.;
  for (int i=0; i<w_off_size; i++)
    {
      sum += w_off[i]; 
      if ( fabs(w_off[i]) > max ) max = fabs(w_off[i]);
    }
  if (fabs(sum) > max) max = fabs(sum);
  
  // Get Scale at Maximum
  scale = 0;
  if (max!=0) scale = (int) truncf(log((pow(2.,NumberBits)-1.)/max)*(1./log(2.)));
  
  // Convert to Integer the weights and the sum
  for (int i=0; i<w_off_size; i++)  w_dsp.push_back( (int) roundf(w_off[i]*pow(2.,scale)) );
  w_sum_dsp = (int) roundf(sum*pow(2.,scale));
  

  if (verbose) {
    printf("\nAbsolute Max value = %15.8f -> Scale = %3d\n",max,scale);

    for (int i=0; i<w_off_size; i++)
      {
        if ( i == 0){
          printf("\n        Offline              Off*Scale             Dsp/scale    Dsp      Scale  \n");
          printf("----------------------------------------------------------------------------------\n");
          printf("  %17.10f     %17.10f    %17.10f  0x%04X    %3d   \n",
		 w_off[i],w_off[i]*pow(2.,scale), w_dsp[i]/pow(2.,scale),(unsigned int) w_dsp[i] ,scale);
        } else {
          printf("  %17.10f     %17.10f    %17.10f  0x%04X   \n",
		 w_off[i],w_off[i]*pow(2.,scale),w_dsp[i]/pow(2.,scale),(unsigned int)w_dsp[i]);
        }
      }
    printf("  %17.10f     %17.10f    %17.10f  0x%04X     <- SUM\n",
	   sum,sum*pow(2.,scale),w_sum_dsp/pow(2.,scale),(unsigned int)w_sum_dsp);
  }
  
  return ;
}

bool ConvertOFC( vector<vector<vector<vector<double> > > > w_off, vector<vector<double> > calibration, int calibtype, int runtype, vector<unsigned int> &OFC, bool verbose)
{
  bool flag;
  
  // Select number of phases and step inside DSP
  //int format = 5;
  //format = format;
  float dsp_step = 1. ;
  float dsp_min_phase = -100.;
  float dsp_max_phase = -dsp_min_phase;
  int   dsp_phases    = (int) roundf( (dsp_max_phase - dsp_min_phase)*(1./dsp_step) +1. );
  
  if (1)
    {
      cout<<"----- DSP -----"<<endl;
      cout<<"Step: "<<dsp_step<<" ns"<<endl;
      cout<<"Min Phase: "<<dsp_min_phase<<" ns"<<endl;
      cout<<"Max Phase: "<<dsp_max_phase<<" ns"<<endl;
      cout<<"Nb phases: "<<dsp_phases<<endl;
    }

  // Number of phases and step of the offline Opt. filte constants
  const float off_step = 0.1;
  float off_min_phase = -100.;
  float off_max_phase = -off_min_phase;
  int   off_phases    = (int) roundf( (off_max_phase - off_min_phase)*(1./off_step) + 1. );
  if (verbose)
    {
      cout<<"----- OFFLINE -----"<<endl;
      cout<<"Step: "<<off_step<<" ns"<<endl;
      cout<<"Min Phase: "<<off_min_phase<<" ns"<<endl;
      cout<<"Max Phase: "<<off_max_phase<<" ns"<<endl;
      cout<<"Nb phases: "<<off_phases<<endl;
    }

  int ngains   = (int) w_off.size();
  int nphases  = (int) w_off[0].size();
  int nparams  = (int) w_off[0][0].size();
  int nsamples = (int) w_off[0][0][0].size();
  
  if ( dsp_step < off_step ){
    cout<<"ERROR: DSP phase step smaller than Offline phase step"<<endl;
    return false;
  }
  if (ngains != 2){
    cout<<"ERROR: Incorrect number of gains"<<endl;
    return false;
  }
  if (off_phases != nphases){
    cout<<"ERROR: Incorrect number of phases"<<endl;
    return false;
  }
  if (dsp_min_phase < off_min_phase){
    cout<<"ERROR: Incorrect minimum phase"<<endl;
    return false;
  }
  if (dsp_max_phase > off_max_phase){
    cout<<"ERROR: Incorrecto maximum phase"<<endl;
    return false;
  }
  
  int algorithm;
  if (nparams == 3){
    algorithm = 0; // OF1
  }else if (nparams == 4){
    algorithm = 1; // OF2
  }else{
    cout<<"ERROR: Incorrect algorithm type"<<endl;
    return false;
  }

  // Get Calibration
  int ig;
  int calib_gain = (int) calibration.size();
  int calib_size = (int) calibration[0].size();
  if (calib_gain!=ngains){
    cout<<"ERROR: Number of gain in calibration vector incorrect"<<endl;
    return false;
  }
  vector<double> calibration_lo;
  vector<double> calibration_hi;
  for (int i=0; i<calib_size; i++)
    {
      ig = 0; // low gain
      calibration_lo.push_back( calibration[ig][i] );
      ig = 1; // high gain
      calibration_hi.push_back( calibration[ig][i] );
    }
  if (calibtype != 0) calibration_hi[1] = calibration_hi[1]*64.;
  
  if (verbose)
    {
      cout<<"--- calibration ---"<<endl;
      cout<<"[low  gain] -> offset: "<<calibration_lo[0]<<"  slope: "<<calibration_lo[1]<<endl; 
      cout<<"[high gain] -> offset: "<<calibration_hi[0]<<"  slope: "<<calibration_hi[1]<<endl; 
      cout<<"If calibtype != ADC then slope is multiplied by a factor 64"<<endl;
    }
  
  // Get Optimal filtering constants
  vector<double> a_lo(nsamples,0);
  vector<double> b_lo(nsamples,0);
  vector<double> c_lo(nsamples,0);
  vector<double> g_lo(nsamples,0);
  vector<double> h_lo(nsamples,0);
  vector<double> a_hi(nsamples,0);
  vector<double> b_hi(nsamples,0);
  vector<double> c_hi(nsamples,0);
  vector<double> g_hi(nsamples,0);
  vector<double> h_hi(nsamples,0);
  
  // Select phases for the DSP
  int index;
  float current_phase;

  // bln
  const float inv_off_step = 1. / off_step;
  for (int i=0; i<dsp_phases; i++){
    
    current_phase = dsp_min_phase + i*dsp_step;
    index = (int) roundf((current_phase - off_min_phase)*inv_off_step);
    if (verbose) cout<<"["<<index<<"]: Phase "<<current_phase<<" ns"<<endl;
    for (int is=0; is<nsamples; is++)
      {
	ig = 0; // low gain
	a_lo[is] = w_off[ig][index][0][is];
	b_lo[is] = w_off[ig][index][1][is];
	g_lo[is] = w_off[ig][index][nparams-1][is];
	// missing c_lo, h_lo

	ig = 1; // high gain
	a_hi[is] = w_off[ig][index][0][is];
	b_hi[is] = w_off[ig][index][1][is];
	g_hi[is] = w_off[ig][index][nparams-1][is];
	// missing c_hi, h_hi
	
      }
    
    
    // convert Format5
    if (verbose) cout<<"---- LOW GAIN ----"<<endl;
    flag = Format5( a_lo, b_lo, c_lo, g_lo, h_lo, OFC, verbose); 
    if (!flag) return flag;
    if (verbose) cout<<"---- HIGH GAIN ----"<<endl;
    flag = Format5( a_hi, b_hi, c_hi, g_hi, h_hi, OFC, verbose); 
    if (!flag) return flag;
   
  }

  cout << "ERROR!!! Format 5 in this call is incomplete (OFC::ConvertOFC)" << endl;

  flag = FormatInfo( nsamples, calibtype, algorithm, runtype, OFC, verbose);
  if (!flag) return flag;
    
  a_lo.clear();
  b_lo.clear();
  g_lo.clear();
  a_hi.clear();
  b_hi.clear();
  g_hi.clear();
  calibration_lo.clear();
  calibration_hi.clear();

  return true;
}

/**
 * Format6 
 * \brief Format used in the OFW without iterations
 * \param a vector of A weights to compute energy
 * \param b vector of B weights to compute phase
 * \param c vector of C weights to compute pedestal
 * \param g vector of G weights to compute QF
 * \param h vector of derivative of G weights to compute QF
 * \param channel_index channel index [0:48]
 * \param phase rounded scaled integer value of the channel's best phase
 * \param calibration calibration factor to be applied to the energy
 * \param OFC reference to OFW vector
 * \param verbose boolean value
 * \return bool true on success
 **/

bool Format6( std::vector<double> a, 
	      std::vector<double> b, 
	      std::vector<double> c, 
	      std::vector<double> g, 
	      std::vector<double> h, 
	      unsigned int channel_index, 
	      int phase, 
	      double calibration, 
	      std::vector<unsigned int> &OFC, 
	      bool verbose)
{
  vector<int> adsp;
  vector<int> bdsp;
  vector<int> cdsp;
  vector<int> gdsp;
  vector<int> hdsp;
  
  int dump;

  int ascale;
  int bscale;
  int cscale;
  int gscale;
  int hscale;

  Double2Int_ofc( (int) a.size(), a, adsp, dump, ascale, verbose );
  Double2Int_ofc( (int) b.size(), b, bdsp, dump, bscale, verbose );
  Double2Int_ofc( (int) c.size(), c, cdsp, dump, cscale, verbose );
  Double2Int_ofc( (int) g.size(), g, gdsp, dump, gscale, verbose );
  Double2Int_ofc( (int) h.size(), h, hdsp, dump, hscale, verbose );

  unsigned int slope_scale;
  unsigned int slope;
  
  Double2Int_calib( calibration, slope_scale, slope, verbose );

  // Start formating
  int n_2 = (int) trunc(a.size()/2.);

  // OFC for energy
  OFC.push_back( ( (adsp[0] << 16) & 0xFFFF0000) | ( (ascale-1) & 0xFFFF)   );
  for (int i=1; i<=n_2; i++)
    OFC.push_back( ( (adsp[2*i] << 16) & 0xFFFF0000) | ( adsp[2*i-1] & 0xFFFF) );

  // Calibration
  OFC.push_back( ( (slope     << 16) & 0xFFFF0000) | ( (ascale + slope_scale ) & 0xFFFF ) );

  // OFC for phase
  OFC.push_back( ( (bdsp[0] << 16) & 0xFFFF0000) | ( (bscale-4) & 0xFFFF)   );
  for (int i=1; i<=n_2; i++)
    OFC.push_back( ( (bdsp[2*i] << 16) & 0xFFFF0000) | ( bdsp[2*i-1] & 0xFFFF) );

  // OFC for pedestal
  OFC.push_back( ( (cdsp[0] << 16) & 0xFFFF0000) | ( (cscale) & 0xFFFF)   );
  for (int i=1; i<=n_2; i++)
    OFC.push_back( ( (cdsp[2*i] << 16) & 0xFFFF0000) | ( cdsp[2*i-1] & 0xFFFF) );
  
  // OFC for quality factor
  OFC.push_back( ( (hscale << 16) & 0xFFFF0000) | ( (gscale+1) & 0xFFFF)   );
  for (size_t i=0; i<g.size(); i++)
    OFC.push_back( ( (hdsp[i] << 16) & 0xFFFF0000) | ( gdsp[i] & 0xFFFF) );

  // codificar index channel y fase
  OFC.push_back( ( (channel_index << 16) & 0xFFFF0000) | (phase & 0xFFFF) ) ;

  if (verbose) cout<<"size of OFC: "<<OFC.size()<<endl;

  adsp.clear();
  bdsp.clear();
  cdsp.clear();
  gdsp.clear();
  hdsp.clear();

  return true;
}

/**
 * Format5 
 * \brief Format used in the OFW with iterations
 * \param a  vector of A weights to compute energy
 * \param b  vector of B weights to compute phase
 * \param c  vector of C weights to compute pedestal
 * \param g  vector of G weights to compute QF
 * \param h vector of derivative of G weights to compute QF
 * \param OFC reference to OFW vector
 * \param verbose boolean value
 * \return bool true on success
 **/

bool Format5( vector<double> a, 
	      vector<double> b , 
	      vector<double> c, 
	      vector<double> g, 
	      vector<double> h, 
	      vector<unsigned int> &OFC , 
	      bool verbose)
{
  vector<int> adsp;
  vector<int> bdsp;
  vector<int> cdsp;
  vector<int> gdsp;
  vector<int> hdsp;
  
  int asum;
  int bsum;
  int dump;

  int ascale;
  int bscale;
  int cscale;
  int gscale;
  int hscale;

  Double2Int_ofc( (int) a.size(), a, adsp, asum, ascale, verbose );
  Double2Int_ofc( (int) b.size(), b, bdsp, bsum, bscale, verbose );
  Double2Int_ofc( (int) c.size(), c, cdsp, dump, cscale, verbose );
  Double2Int_ofc( (int) g.size(), g, gdsp, dump, gscale, verbose );
  Double2Int_ofc( (int) h.size(), h, hdsp, dump, hscale, verbose );

  // Start formating
  int n_2 = (int) trunc(a.size()/2.);

  for (int i=0; i<n_2; i++)
    OFC.push_back( ( (adsp[2*i] << 16 )& 0xFFFF0000) | ( adsp[2*i+1] & 0x0000FFFF) ) ;
  OFC.push_back( ( (adsp[2*n_2] << 16 )& 0xFFFF0000) | ( (-asum) & 0x0000FFFF) );

  for (int i=0; i<n_2; i++)
    OFC.push_back( ( (bdsp[2*i] << 16 )& 0xFFFF0000) | ( bdsp[2*i+1] & 0x0000FFFF) ) ;
  OFC.push_back( ( (bdsp[2*n_2] << 16 )& 0xFFFF0000) | ( (-bsum) & 0x0000FFFF) );
  
  OFC.push_back( ( (bscale  << 16 )& 0xFFFF0000) | ( ascale   & 0x0000FFFF) );
  
  OFC.push_back( ( (cdsp[0] << 16 )& 0xFFFF0000) | ( cscale   & 0x0000FFFF) );
  for (int i=1; i<=n_2; i++)
    OFC.push_back( ( (cdsp[2*i] << 16 )& 0xFFFF0000) | ( cdsp[2*i-1] & 0x0000FFFF) ) ;
  
  OFC.push_back( ( (hscale  << 16 )& 0xFFFF0000) | ( gscale   & 0x0000FFFF) );
  for (size_t i=0; i<g.size();i++)
    OFC.push_back( ( (hdsp[i] << 16 )& 0xFFFF0000) | ( gdsp[i] & 0x0000FFFF) );
  
  adsp.clear();
  bdsp.clear();
  cdsp.clear();
  gdsp.clear();
  hdsp.clear();
  
  if (verbose) cout<<"size of OFC: "<<OFC.size()<<endl;
  return true;
  
}

bool Format5calib( double calib, vector<unsigned int> &OFC , bool verbose){
  
  unsigned int offset_scale = 0;
  unsigned int offset = 0;
  
  unsigned int slope_scale;
  unsigned int slope;

  Double2Int_calib( calib, slope_scale, slope, verbose );
  
  OFC.push_back( ( (offset_scale << 16) & 0xFFFF0000) | ( slope_scale & 0x0000FFFF) );
  OFC.push_back( ( (offset       << 16) & 0xFFFF0000) | ( slope       & 0x0000FFFF) );

  return true;
} 

bool FormatInfo( int nsamples, int calibrationtype, int algorithm, int runtype, vector<unsigned int> &OFC, bool verbose)
{
  int in = 1;
  if (nsamples == 9){
    in = 1;
  }else if(nsamples == 7){
    in = 0;
  }else {
    cout<<"ERROR: [FormatInfo] Incorrect number of samples"<<endl;
    return false;
  }

  if (calibrationtype > 3 || calibrationtype < 0 ) {
    cout<<"ERROR: [FormatInfo] Unknown calibration type"<<endl;
    return false;
  }

  // poner lo mismo con runtype

  if (algorithm != 0 && algorithm!=1){
    cout<<"ERROR: [FormatInfo] Unknown algorithm type"<<endl;
    return false;
  }
  
  OFC.push_back( ((calibrationtype & 0x3)<<6) | ((runtype & 0x3)<<4) | ((in & 0x1)<<3) | ((algorithm & 0x1) << 2) );
  
  if (verbose) printf("OFC info: 0x%08x\n", ((calibrationtype & 0x3)<<6) | ((runtype & 0x3)<<4) | ((in & 0x1)<<3) | ((algorithm & 0x1) << 2) );

  return true;
}

bool ReadOFfile(vector<vector<vector<vector<double> > > > &w_off, char * OFCfile, bool verbose)
{
  // OPEN FILE
  ifstream fin;
  fin.open(OFCfile, ios::in);
  if (!fin.is_open())
    {
      cout<<"Error opening file "<<(OFCfile)<<endl;
      return false;
    }

  // input file settings
  float f_samples = 9;     // 9 samples
  float f_phi     = -100;  // ns  (-100,100) ns
  float f_step    = 0.1;
  int nphases = (int) roundf(-f_phi*2./f_step + 1.);
  int ngains = 2;
  int nsamples = (int) roundf(f_samples);

  // get number of parameters
  string s;
  double tmp1;
  int nparams=0;
  getline(fin,s);
  istringstream ss(s);
  while (ss >> tmp1) {
    nparams++;
  }
  cout<<"Number of parameters: "<<nparams<<endl;
  
  // Resize w_off vector
  w_off.resize(ngains);
  for (int ig=0; ig<ngains; ig++){
    w_off[ig].resize(nphases);
    for (int it=0; it<nphases; it++){
      w_off[ig][it].resize(nparams);
      for (int ip=0; ip<nparams; ip++){
        w_off[ig][it][ip].resize(nsamples,0);
      }
    }
  }
  fin.close();

  // read file
  fin.open(OFCfile, ios::in);
  for (int it=0; it<nphases; it++)
    {
      for (int is=0; is<nsamples; is++)
	{
	  for (int ig=0; ig<ngains; ig++)
	    {
	      int ip = 0;
	      getline(fin,s);
	      istringstream ss(s);
	      while (ss >> tmp1) {
		w_off[ig][it][ip][is] = tmp1;
		ip++;
	      }
	    }
	}      
    }

  verbose = true;
  if (verbose)
    {
      cout<<"Optimal Filtering file: "<<(OFCfile)<<endl;
      cout<<"Nb phases:  "<<w_off[0].size()<<endl;
      cout<<"Nb samples: "<<w_off[0][0][0].size()<<endl;
      cout<<"Nb gains:   "<<w_off.size()<<endl;
      if ((int) w_off[0][0].size() == 3 ) cout<<"Algorithm: OF1"<<endl;
      else if ((int) w_off[0][0].size() == 4 ) cout<<"Algorithm: OF2"<<endl;
    }
  
  // CLOSE FILE
  fin.close();
  
  return true;
}



/*
bool ConvertOFC_iter( TileCalibDrawerOfc ofcDrawer, 
		      std::vector<unsigned int> &OFC, 
		      vector<vector<float> > calib, 
		      int calibtype, 
		      int runtype, 
		      int algorithm, 
		      bool verbose)
{

  bool flag;
  
  const uint32_t nphases = labs(ofcDrawer.getNPhases());
  if (nphases != 2001) {
    cout<<"ERROR: [ConvertOFC_iter] Incorrect number of phases"<<endl;
    return false;
  }
  
  const uint32_t nsamples = ofcDrawer.getNSamples();
  
  float dsp_step;
  float dsp_min_phase; 
  float dsp_max_phase;
  int   dsp_phases;

  // Select number of phases and step inside DSP
  
  switch(nsamples){
  case 7:
    dsp_step      =   1.;
    dsp_min_phase = -75.;
    dsp_max_phase = +75.;
    dsp_phases    = 151 ;
    break;
  case 9:
    dsp_step      =    1.;
    dsp_min_phase = -100.;
    dsp_max_phase = +100.;
    dsp_phases    =  201 ;
    break;
  default:
    cout<<"ERROR: [ConverOFC_iter] Incorrect number of samples"<<endl;
    return false;
  }
  
  if (0)
    {
      cout << "----- DSP -----" << endl
	   << "Step:      " << dsp_step      << " ns" << endl
	   << "Min Phase: " << dsp_min_phase << " ns" << endl
	   << "Max Phase: " << dsp_max_phase << " ns" << endl
	   << "Nb phases: " << dsp_phases    << endl;
    }

  const uint16_t ngains = ofcDrawer.getNGains();
  if (ngains !=2) {
    cout<<"ERROR: [ConvertOFC_iter] Incorrect number of phases"<<endl;
    return false;
  }
 
  // Get Optimal Filtering constants
  // A, B, C, G, H (G')
  vector<double> a_lo(nsamples,0);
  vector<double> b_lo(nsamples,0);
  vector<double> c_lo(nsamples,0);
  vector<double> g_lo(nsamples,0);
  vector<double> h_lo(nsamples,0);
  vector<double> a_hi(nsamples,0);
  vector<double> b_hi(nsamples,0);
  vector<double> c_hi(nsamples,0);
  vector<double> g_hi(nsamples,0);
  vector<double> h_hi(nsamples,0);
  
  int current_phase;
  unsigned int channel = 0;
  unsigned int adc;
  int lo = TileDspGain::Low;
  int hi = TileDspGain::High;
  
  for (int i=0; i<dsp_phases; i++)
    {
      current_phase = (int) roundf( (dsp_min_phase + i*dsp_step)*10.); // in units of ns*10.
      if (verbose) cout<<"Phase "<<current_phase/10.<<" ns"<<endl;
  
      for (unsigned int is=0; is<nsamples; is++)
	{
	  adc = 0; // low gain
	  a_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldA,  channel, adc, current_phase, is);
	  b_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldB,  channel, adc, current_phase, is);
	  c_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldC,  channel, adc, current_phase, is);
	  g_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldG,  channel, adc, current_phase, is);
	  h_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldDG, channel, adc, current_phase, is);

	  adc = 1; // high gain
	  a_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldA,  channel, adc, current_phase, is);
	  b_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldB,  channel, adc, current_phase, is);
	  c_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldC,  channel, adc, current_phase, is);
	  g_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldG,  channel, adc, current_phase, is);
	  h_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldDG, channel, adc, current_phase, is);
	}

      // convert Format5
      if (verbose) cout<<"---- LOW GAIN ----"<<endl;
      flag = Format5( a_lo, b_lo, c_lo, g_lo, h_lo, OFC, verbose); 
      if (!flag) return flag;
      if (verbose) cout<<"---- HIGH GAIN ----"<<endl;
      flag = Format5( a_hi, b_hi, c_hi, g_hi, h_hi, OFC, verbose); 
      if (!flag) return flag;  
      
    }

  double calibration_hi;
  double calibration_lo;
  
  for(size_t ich=0;ich<calib.size();ich++){
    
    calibration_lo=calib[ich][lo];
    calibration_hi=calib[ich][hi];
    
    // calibration_hi should be scaled for DSP. Ask Alberto
    if(calibtype!=0)
      calibration_hi = calibration_hi*64.;
    
    Format5calib(calibration_lo, OFC, verbose);
    Format5calib(calibration_hi, OFC, verbose);
  }
   
  flag = FormatInfo( nsamples, calibtype, algorithm, runtype, OFC, verbose);
  if (!flag) return flag;
  
  a_lo.clear();
  b_lo.clear();
  c_lo.clear();
  g_lo.clear();
  h_lo.clear();
  a_hi.clear();
  b_hi.clear();
  c_hi.clear();
  g_hi.clear();
  h_hi.clear();

  return true;
}

bool ConvertOFC_HR( TileCalibDrawerOfc ofcDrawer, 
		    TileCalibDrawerFlt phaseDrawer,
		    std::vector<unsigned int> &OFC, 
		    std::vector< std::vector<float> > calib, 
		    int calibtype, 
		    int runtype, 
		    int algorithm,
		    bool verbose)
{
  // Converter for High Rate (no iterations)
  // one phase per adc
  // The initial phases (phaseDrawer) are an input of the function
  // Update to read selected phase from database

  bool flag;

  // This should be temporary ! 
  const uint32_t nphases = labs(ofcDrawer.getNPhases());
  if (nphases != 2001) {
    cout<<"ERROR: [ConvertOFC_HR] Incorrect number of phases"<<endl;
    return false;
  } else {
    cout<<"Reading OFCs for 2001 phases from the database"<<endl;
  }
  
  // in principle we can use 7 or 9 samples!!
  const uint32_t nsamples = ofcDrawer.getNSamples();
  if (nsamples != 7 && nsamples != 9) {
    cout<<"ERROR: [ConverOFC_HR] Incorrect number of samples"<<endl;
    return false;
  }
  
  const uint16_t ngains = ofcDrawer.getNGains();
  if (ngains !=2) {
    cout<<"ERROR: [ConvertOFC_HR] Incorrect number of phases"<<endl;
    return false;
  }
  
  // Calibration for each channel (only slope)
  const uint16_t nchannel = calib.size();
  if (nchannel != 48){
    cout<<"ERROR: [ConvertOFC_HR] Incorrect number of channels"<<endl;
    return false;
  }

  // Get Optimal Filtering constants
    // A, B, C, G, H (G')
  vector<double> a_lo(nsamples,0);
  vector<double> b_lo(nsamples,0);
  vector<double> c_lo(nsamples,0);
  vector<double> g_lo(nsamples,0);
  vector<double> h_lo(nsamples,0);
  vector<double> a_hi(nsamples,0);
  vector<double> b_hi(nsamples,0);
  vector<double> c_hi(nsamples,0);
  vector<double> g_hi(nsamples,0);
  vector<double> h_hi(nsamples,0);

  double calibration_lo;
  double calibration_hi;
  int    phase_lo;
  int    phase_hi;
  int    lo = TileDspGain::Low;
  int    hi = TileDspGain::High;

  for (unsigned int ich=0; ich<nchannel; ich++){
    
    // Get phases (in units of ns/10)
    
    if(true){
      
      //const double phase2int = 10.;
      //It has been identified by the Reco Validation group that the best phase in cool
      //has inverted sign. For the sake of DB operation, we switch the sign here.
      const double phase2int = -10.;
      
      phase_lo = static_cast<int>( round(phase2int * phaseDrawer.getData(ich, lo, 0)) ); 
      if(verbose) cout << "Idx phase_lo(" << ich << ") =" << phase_lo << endl;
      
      phase_hi = static_cast<int>( round(phase2int * phaseDrawer.getData(ich, hi, 0)) );
      if(verbose) cout << "Idx phase_hi(" << ich << ") =" << phase_hi << endl;
    
    }else{

      phase_lo = 0;
      phase_hi = 0;

    }

    // Get calibration constants for current
    
    calibration_lo = (double) calib[ich][lo];
    calibration_hi = (double) calib[ich][hi];

    // calibration_hi should be scaled for DSP. Ask Alberto

    if(calibtype!=0)
      calibration_hi = calibration_hi*64.;

    for (unsigned int is=0; is<nsamples; is++){

      a_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldA,  ich, lo, phase_lo, is);
      b_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldB,  ich, lo, phase_lo, is);
      c_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldC,  ich, lo, phase_lo, is);
      g_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldG,  ich, lo, phase_lo, is);
      h_lo[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldDG, ich, lo, phase_lo, is);
      
      a_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldA,  ich, hi, phase_hi, is);
      b_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldB,  ich, hi, phase_hi, is);
      c_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldC,  ich, hi, phase_hi, is);
      g_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldG,  ich, hi, phase_hi, is);
      h_hi[is] = ofcDrawer.getOfc(TileCalibDrawerOfc::FieldDG, ich, hi, phase_hi, is);
    
    }
    
    flag = Format6( a_lo, b_lo, c_lo, g_lo, h_lo, ich, phase_lo, calibration_lo, OFC, verbose);
    if (!flag) return flag;
    flag = Format6( a_hi, b_hi, c_hi, g_hi, h_hi, ich, phase_hi, calibration_hi, OFC, verbose);
    if (!flag) return flag;
    
  }
  
  flag = FormatInfo( nsamples, calibtype, algorithm, runtype, OFC, verbose);
  if (!flag) return flag;
  
  a_lo.clear();
  b_lo.clear();
  c_lo.clear();
  g_lo.clear();
  h_lo.clear();
  a_hi.clear();
  b_hi.clear();
  c_hi.clear();
  g_hi.clear();
  h_hi.clear();
  
  return true;
}
*/

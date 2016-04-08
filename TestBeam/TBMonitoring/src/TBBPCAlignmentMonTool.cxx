/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ISvcLocator.h"

#include "TBMonitoring/TBBPCAlignmentMonTool.h"

#include "TBEvent/TBBPC.h"
#include "TBEvent/TBBPCCont.h"
#include "TBEvent/TBTrack.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "AIDA/IAxis.h"

#include <vector>
#include <math.h>

#include <iostream>
#include <fstream>
#include <iomanip>

////////////////////////////////
// constructor and destructor //
////////////////////////////////

TBBPCAlignmentMonTool::TBBPCAlignmentMonTool(const std::string & type, 
                                 const std::string & name,
                                 const IInterface* parent)
  : MonitorToolBase(type, name, parent)
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("histoPathBase",m_path = "/stat/BeamDetectors/BPC/Alignment/");
  declareProperty("MonitorBPCCalib",m_monitor_bpccalib=true);
  declareProperty("SGTrackkey", m_SGkeytrack = "BPCTrack");
  declareProperty("SGBPCkey", m_SGkeybpc = "BPCCont");
  declareProperty("BPCnames", m_bpc_names);
  declareProperty("BPCmaxPos", m_posmax);
  declareProperty("BPCminPos", m_posmin);
  declareProperty("BPCbinNum", m_posbin);
  declareProperty("AlignFile", m_calib_filename);
  declareProperty("OutputFile", m_out_file="output.dat");
  declareProperty("Widths", m_bpc_widths);
}

TBBPCAlignmentMonTool::~TBBPCAlignmentMonTool(){}

///////////////////////////////////////////////
StatusCode TBBPCAlignmentMonTool:: initialize()
///////////////////////////////////////////////
{
  ATH_MSG_DEBUG ( "in initialize()" );
  
  // this flag is set to true within bookHists() 
  m_isBooked = false;

  return StatusCode::SUCCESS;
}

////////////////////////////////////
StatusCode TBBPCAlignmentMonTool::bookHists()
////////////////////////////////////
{
  ATH_MSG_DEBUG ( "in bookHists()" );

 
  //  book BPC calibration histograms 

  if(m_monitor_bpccalib){
    
    m_bpcnum = m_bpc_names.size();
      
    m_histo_bpcX_res = new IHistogram1D*[m_bpcnum];
    m_histo_bpcY_res = new IHistogram1D*[m_bpcnum];
    m_histo_chi2x = new IHistogram1D*[1];
    m_histo_chi2y = new IHistogram1D*[1];
    m_histo_Qx = new IHistogram1D*[1];
    m_histo_Qy = new IHistogram1D*[1];
      
    std::string hname;
    std::string htitle;
      
    for(int j=0; j<m_bpcnum; j++){
         
      hname = m_path + m_bpc_names[j]+ " X offset";
      htitle = m_bpc_names[j] + ", X offset";
      m_histo_bpcX_res[j] = ToolHistoSvc()->book(hname,htitle,m_posbin[j],m_posmin[j],m_posmax[j]);
         
      hname = m_path + m_bpc_names[j] + " Y offset";
      htitle = m_bpc_names[j] + ", Y offset";
      m_histo_bpcY_res[j] = ToolHistoSvc()->book(hname,htitle,m_posbin[j],m_posmin[j],m_posmax[j]);

    }

    hname = m_path + " chi2x";
    htitle = "chi2x";
    m_histo_chi2x[0] = ToolHistoSvc()->book(hname,htitle,1000,0,100);

    hname = m_path + " chi2y";
    htitle = "chi2y";
    m_histo_chi2y[0] = ToolHistoSvc()->book(hname,htitle,1000,0,100);

    hname = m_path + " Qx";
    htitle = "Qx";
    m_histo_Qx[0] = ToolHistoSvc()->book(hname,htitle,10000,0,1); 

    hname = m_path + " Qy";
    htitle = "Qy";
    m_histo_Qy[0] = ToolHistoSvc()->book(hname,htitle,10000,0,1);
    
    m_isBooked = true;
     
  } 

  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////
StatusCode TBBPCAlignmentMonTool::fillHists()
///////////////////////////////////////////////
{
  ATH_MSG_DEBUG ( "in fillHists()"  );
  
  // get run number 

  unsigned int thisrun=0;
  EventID *thisEvent;           //EventID is a part of EventInfo
  const EventInfo* thisEventInfo;
  StatusCode sc1 = evtStore()->retrieve(thisEventInfo);
  if (sc1!=StatusCode::SUCCESS){
    ATH_MSG_WARNING ( "No EventInfo object found! Can't read run number!" );
    ATH_MSG_WARNING ( "     => can't get calib constant. Exit" );
    return sc1;
  }
  else
    {
      thisEvent=thisEventInfo->event_ID();
      thisrun = thisEvent->run_number();
    }


  // if it is a new run, get new calib constants	

  if(thisrun != m_runnumber)
    {
      m_runnumber= thisrun;
      getnewcalib();
    }



  if(m_isBooked){
  
    // fill BPC alignment histograms
    // first retrieve TBBPC container from StoreGate
    
    TBBPCCont *  bpcCont = nullptr;
    StatusCode sc = evtStore()->retrieve(bpcCont, m_SGkeybpc);
    if (sc.isFailure()){
      ATH_MSG_WARNING ( "Retrieval of TBBPCCont failed" );
    }
    
    else {

      // retrieve Track from StoreGate
      
      TBTrack * track = nullptr;
      StatusCode sc = evtStore()->retrieve(track, m_SGkeytrack);
      if (sc.isFailure()){
        ATH_MSG_WARNING ( "Retrieval of TBTrack failed" );
      }
      
      else { 

       // loop over the BPCs in the container
       
       TBBPCCont::const_iterator it_bpc   = bpcCont->begin();
       TBBPCCont::const_iterator last_bpc   = bpcCont->end();
   
       for(;it_bpc != last_bpc; it_bpc++){
       
         // discover which BPC is pointed to
	 
	 const TBBPC * bpc = (*it_bpc);	 
	 int i = 0;
	 
	 while( i < m_bpcnum ){
	   if( bpc->getDetectorName() == m_bpc_names[i] ) break;
	   else i++;
	 }      
        
	 if ( i == m_bpcnum ){
	   ATH_MSG_WARNING
             ( bpc->getDetectorName()
	       << " could not be matched to the list of BPC names" );
	 }
	 
	 else {
	
	   // calculate the x and y residuals
    
           double mx = track->getUslope();
	   double my = track->getVslope();
	   double bx = track->getUintercept();
	   double by = track->getVintercept();
    
      	   double x_expected = mx*m_bpc_posZX[i] + bx;
	   double y_expected = my*m_bpc_posZY[i] + by;
	              
           if(! (bpc->isXPosOverflow()||bpc->isXPulseOverflow()) ){	   
	     double residual_x = x_expected - ( bpc->getXPos() + m_bpc_alignX[i] );
	     m_histo_bpcX_res[i]->fill(residual_x, 1.0);
	   }
	   
	   if(! (bpc->isYPosOverflow()||bpc->isYPulseOverflow()) ){	   
	     double residual_y = y_expected - ( bpc->getYPos() + m_bpc_alignY[i] );
	     m_histo_bpcY_res[i]->fill(residual_y, 1.0);
	   }
	
           // now look at chi2s

	   m_chi2x = track->getChi2_u();
           m_chi2y = track->getChi2_v();
           m_Qx = gammq(2.0, m_chi2x/2);
           m_Qy = gammq(2.0, m_chi2y/2);
 
           m_histo_chi2x[0]->fill(m_chi2x, 1.0);
           m_histo_chi2y[0]->fill(m_chi2y, 1.0);
           m_histo_Qx[0]->fill(m_Qx, 1.0);
           m_histo_Qy[0]->fill(m_Qy, 1.0); 
           
	 } // end else
       
       } // end BPC loop
      
      }
    }

  } 
 
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////
StatusCode TBBPCAlignmentMonTool::finalHists()
///////////////////////////////////////////////
{
  ATH_MSG_DEBUG  ( "in finalHists()" );

  if (m_isBooked){
  
    xmean.resize(m_bpcnum);
    xrms.resize(m_bpcnum);
    xentries.resize(m_bpcnum);
    ymean.resize(m_bpcnum);
    yrms.resize(m_bpcnum);
    yentries.resize(m_bpcnum);
    
    for(int j=0; j<m_bpcnum; j++){   
      xmean[j] = m_histo_bpcX_res[j]->mean();
      xrms[j] =  m_histo_bpcX_res[j]->rms(); 
      ymean[j] = m_histo_bpcY_res[j]->mean();
      yrms[j] =  m_histo_bpcY_res[j]->rms();   
    }
     
    // recalculate the mean and RMS 
    
    recalculate();
    
    // print the results to a text file
   
    mean_rms_print();    
  
  }
  
  return StatusCode::SUCCESS;         
}

///////////////////////////////////////////
void TBBPCAlignmentMonTool::recalculate()
///////////////////////////////////////////
{

  // find the mode of the distribution (the bin with the maximum number 
  // of counts) and recalculate mean based on some fixed distance on either side of it

  // distances are specified for each BPC in the property "Widths"


  for(int j=0; j<m_bpcnum; j++){
  
    int nx = m_histo_bpcX_res[j]->axis().bins();
    int ny = m_histo_bpcY_res[j]->axis().bins();    

    int xmaxbin=1;
    int ymaxbin=1;
    float xmaxheight=0;
    float ymaxheight=0;

    for (int ii=1; ii<nx; ii++){
      if (m_histo_bpcX_res[j]->binHeight(ii) > xmaxheight){
        xmaxheight = m_histo_bpcX_res[j]->binHeight(ii);
        xmaxbin = ii;
      }
    }

    for (int jj=1; jj<ny; jj++){
      if (m_histo_bpcY_res[j]->binHeight(jj) > ymaxheight){
        ymaxheight = m_histo_bpcY_res[j]->binHeight(jj);
        ymaxbin = jj;
      }
    }

    float xmode = m_histo_bpcX_res[j]->axis().binUpperEdge(xmaxbin) - 0.5*m_histo_bpcX_res[j]->axis().binWidth(xmaxbin);
    float ymode = m_histo_bpcY_res[j]->axis().binUpperEdge(ymaxbin) - 0.5*m_histo_bpcY_res[j]->axis().binWidth(ymaxbin);

    //int xbegin = m_histo_bpcX_res[j]->coordToIndex(xmean[j] - num_rms*xrms[j]);
    //int xend = m_histo_bpcX_res[j]->coordToIndex(xmean[j] + num_rms*xrms[j]);
    //int ybegin = m_histo_bpcY_res[j]->coordToIndex(ymean[j] - num_rms*yrms[j]);
    //int yend = m_histo_bpcY_res[j]->coordToIndex(ymean[j] + num_rms*yrms[j]);

    int xbegin = m_histo_bpcX_res[j]->coordToIndex(xmode - m_bpc_widths[j]);
    int xend   = m_histo_bpcX_res[j]->coordToIndex(xmode + m_bpc_widths[j]);
    int ybegin = m_histo_bpcY_res[j]->coordToIndex(ymode - m_bpc_widths[j]);
    int yend   = m_histo_bpcY_res[j]->coordToIndex(ymode + m_bpc_widths[j]);

    
    double new_xmean = 0;
    double new_xrms = 0;
    double new_ymean = 0;
    double new_yrms = 0;
   
    xentries[j] = 0;
    yentries[j] = 0;
   
    // calculate the new x and y mean
    
    for(int k=xbegin; k<=xend; k++){
      
      xentries[j] += m_histo_bpcX_res[j]->binEntries(k);
      double binMiddle = m_histo_bpcX_res[j]->axis().binLowerEdge(k) 
                   + 0.5*m_histo_bpcX_res[j]->axis().binWidth(k);
      new_xmean += m_histo_bpcX_res[j]->binHeight(k) * binMiddle;      
    }
    
    for(int k=ybegin; k<=yend; k++){
      
      yentries[j] += m_histo_bpcY_res[j]->binEntries(k);
      double binMiddle = m_histo_bpcY_res[j]->axis().binLowerEdge(k) 
	                    + 0.5*m_histo_bpcY_res[j]->axis().binWidth(k);
      new_ymean += m_histo_bpcY_res[j]->binHeight(k) * binMiddle;      
    }

    new_xmean = new_xmean/xentries[j];
    new_ymean = new_ymean/yentries[j];

    // calculate the new rms
    
    for(int k=xbegin; k<=xend; k++){
      
      double binMiddle = m_histo_bpcX_res[j]->axis().binLowerEdge(k) 
                      + 0.5*m_histo_bpcX_res[j]->axis().binWidth(k);
      new_xrms += pow( (binMiddle - new_xmean), 2.0 )
                      * m_histo_bpcX_res[j]->binHeight(k);
    }
    
    for(int k=ybegin; k<=yend; k++){
      
      double binMiddle = m_histo_bpcY_res[j]->axis().binLowerEdge(k) 
	                    + 0.5*m_histo_bpcY_res[j]->axis().binWidth(k);
      new_yrms += pow( (binMiddle - new_ymean), 2.0 )
                         * m_histo_bpcY_res[j]->binHeight(k);
    }
    
    new_xrms = sqrt(new_xrms / (xentries[j] - 1) );
    new_yrms = sqrt(new_yrms / (yentries[j] - 1) );
    
    xmean[j] = new_xmean;
    ymean[j] = new_ymean;
    xrms[j] = new_xrms;
    yrms[j] = new_yrms;      
      
  }
	  
}

//////////////////////////////////////////////
void TBBPCAlignmentMonTool::mean_rms_print()
//////////////////////////////////////////////
{
  // print the alignment constants to a text file

  using namespace std;

  std::ofstream outfile;
  outfile.open(m_out_file.c_str());
  int j;

  // first print in a format that I think is useful

  for(j=0; j<m_bpcnum; j++){

    outfile << j << " " << "X" << " " << xmean[j] << " " << xrms[j] << " " << xentries[j] << endl;
     
  }

  for(j=0; j<m_bpcnum; j++){

    outfile << j << " " << "Y" << " " << ymean[j] << " " << yrms[j] << " " << yentries[j] << endl;
 
  }

  // next print in the format required by TBPlaneTrackingAlgo
  // the error on each alignment constant is taken to be rms/sqrt(entries)

  for(j=0; j < m_bpcnum; j++){

    outfile << j << " " << xmean[j] << " " << ymean[j] << " ";
    outfile << m_bpc_posZX[j] << " " << m_bpc_posZY[j] << " ";
    outfile << xrms[j]/sqrt(xentries[j]) << " " << yrms[j]/sqrt(yentries[j]) << " ";
    outfile << 0 << " " << 0 << " " << m_bpc_errmeasX[j] << " " << m_bpc_errmeasY[j] << endl;

  } 

  outfile.close();

}

StatusCode TBBPCAlignmentMonTool::getnewcalib()
{
  // Get calib constant from an ASCII file with the following structure :
  // 
  // runnumber
  // bpcnumber1 coeff1 coeff2 ... coeff8
  // bpcnumber2 coeff1 coeff2 ... coeff8
  // ...
  // bpcnumber6 coeff1 coeff2 ... coeff8
  // runnumber
  // ...
  //
  // coeff must have the following order :
  // bpcnumber posX posY posZX posZY errposX errposY errposZX errposZY errmeasX errmeasY

  ATH_MSG_INFO ( "Get new calibs for run " << m_runnumber);
  
  int bpcnumber= m_bpc_names.size();

  m_bpc_alignX.clear(); m_bpc_alignX.resize(bpcnumber);
  m_bpc_alignY.clear(); m_bpc_alignY.resize(bpcnumber);
  m_bpc_posZX.clear(); m_bpc_posZX.resize(bpcnumber);
  m_bpc_posZY.clear(); m_bpc_posZY.resize(bpcnumber);
  m_bpc_erralignX.clear(); m_bpc_erralignX.resize(bpcnumber);
  m_bpc_erralignY.clear(); m_bpc_erralignY.resize(bpcnumber);
  m_bpc_errposZX.clear(); m_bpc_errposZX.resize(bpcnumber);
  m_bpc_errposZY.clear(); m_bpc_errposZY.resize(bpcnumber);
  m_bpc_errmeasX.clear(); m_bpc_errmeasX.resize(bpcnumber);
  m_bpc_errmeasY.clear(); m_bpc_errmeasY.resize(bpcnumber);

  int pos;

  std::ifstream calibfile;
  calibfile.open(m_calib_filename.c_str());
  if(!calibfile.good()){
    ATH_MSG_INFO ( " Problem with file named "<< m_calib_filename );
    return StatusCode::FAILURE;
  }
  unsigned int runnumber;
  calibfile >> runnumber;
  pos = calibfile.tellg();
  ATH_MSG_INFO ( " Run number "<< runnumber );
  while((runnumber<m_runnumber)&&(!calibfile.eof()))
    {
      runnumber=0;
      pos = calibfile.tellg();
      // discard next lines
      for(int j=0;j<bpcnumber+1;j++) calibfile.ignore(5000,'\n');
      // check next runnumber
      calibfile >> runnumber;
      if(runnumber==0) {
        ATH_MSG_INFO ( "empty line");
        calibfile.clear();
        break;
      } // reached an empty line : exit.
      ATH_MSG_INFO ( " Run number "<< runnumber );
    }
  
  // Now we found the good set of constant (the ones following pos)
  if(runnumber==m_runnumber)  pos = calibfile.tellg();
  ATH_MSG_INFO ( " Pos = "<< pos );
  calibfile.seekg(pos);
  ATH_MSG_INFO ( " Will use the following constants :" );
  for(int j=0;j<bpcnumber;j++) 
    {
      int bpcn;
      calibfile >> bpcn; 
      calibfile >> m_bpc_alignX[j];
      calibfile >> m_bpc_alignY[j];
      calibfile >> m_bpc_posZX[j];
      calibfile >> m_bpc_posZY[j];
      calibfile >> m_bpc_erralignX[j];
      calibfile >> m_bpc_erralignY[j];
      calibfile >> m_bpc_errposZX[j];
      calibfile >> m_bpc_errposZY[j];
      calibfile >> m_bpc_errmeasX[j];
      calibfile >> m_bpc_errmeasY[j];

      ATH_MSG_WARNING ( bpcn << " "<<m_bpc_alignX[j]    
                        << " "<< m_bpc_alignY[j]    
                        << " "<< m_bpc_posZX[j]   
                        << " "<< m_bpc_posZY[j]   
                        << " "<< m_bpc_erralignX[j] 
                        << " "<< m_bpc_erralignY[j] 
                        << " "<< m_bpc_errposZX[j]  
                        << " "<< m_bpc_errposZY[j]
                        << " "<< m_bpc_errmeasX[j]
                        << " "<< m_bpc_errmeasY[j]);
    }
  
  calibfile.close();

  return StatusCode::SUCCESS;  
}

float TBBPCAlignmentMonTool::gammq(float a, float x)
{
	float gamser, gammcf, gln;
	if (x < 0.0 || a <=0.0) ATH_MSG_ERROR ( "invalid arguments in routine gammq" );
        if( x < (a+1.0) ) {
 		gser(&gamser,a,x,&gln);
		return 1.0-gamser;
	} else {
		gcf(&gammcf,a,x,&gln);
		return gammcf;
	}
}

void TBBPCAlignmentMonTool::gser(float *gamser, float a, float x, float *gln)
{
	int n;
	float sum, del, ap;

	*gln=gammln(a);
	if ( x <= 0.0 ){
          if ( x < 0.0)
            ATH_MSG_ERROR ( "x less than 0 in routine gser" );
          *gamser=0.0;
          return;
	} else {
		ap=a;
		del=sum=1.0/a;
		for (n=1; n<=100; n++) {
			++ap;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*3.0e-7){
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return;
			}
		}

		ATH_MSG_ERROR ("a too large, ITMAX too small in routine gser");
		return;
	}
}

void TBBPCAlignmentMonTool::gcf(float *gammcf, float a, float x, float *gln)
{
	int i;
	float an, b, c, d, del, h;

	*gln=gammln(a);
	b=x+1.0-a;
	c=1.0/1.0e-30;
	d=1.0/b;
	h=d;
	for (i=1; i<=100; i++){
		an = -i*(i-a);
		b += 2;
		d = an*d+b;
		if (fabs(d) < 1.0e-30) d=1.0e-30;
		c = b+an/c;
		if (fabs(c) < 1.0e-30) c=1.0e-30;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < 3.0e-7) break;
	}
	if (i > 100) ATH_MSG_ERROR ( "a too large, ITMAX too small in gcf" );
	*gammcf=exp(-x+a*log(x) - (*gln))*h;
}

float TBBPCAlignmentMonTool::gammln(float xx)
{
	double x, y, tmp, ser;
	static double cof[6]={76.18009172947149, -86.50532032941677, 24.01409824083091, -1.231739572450155, 0.1208650973866179e-2, -0.5395239384953e-5};
	int j;

	y=x=xx;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	return -tmp+log(2.5066282746310005*ser/x);
}

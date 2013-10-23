/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
using namespace std;

// ROOT include files
#include "TH2.h"
#include "TH3.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TKey.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TApplication.h"

#include <cmdl/cmdargs.h>
#include <ipc/core.h>
#include <owl/timer.h>

// OH include files
#include <oh/OHRootProvider.h>
#include <oh/OHRootReceiver.h>



#include <is/infoT.h>
#include <is/infodictionary.h>


int main( int argc, char ** argv )
{

    IPCCore::init( argc, argv );
    //TApplication theApp("App", 0, 0);
    //
    // Get command line arguments
    CmdArgStr partition_name( 'p', "partition", "partition_name",
			      "Partition name");
    CmdArgStr server_name( 's', "server", "server_name",
			   "OH (IS) Server name");
    CmdArgStr provider_name( 'n', "provider", "provider_name",
                             "Histogram provider name");
    CmdArgStr histogram_name( 'h', "histogram", "histogram_name",
                              "Histogram type name");
    CmdArgStr provider_name2( "n2", "provider", "provider_name2",
                              "Histogram provider name 2");
    CmdArgStr histogram_name2( "h2", "histogram", "histogram_name2",
			       "Histogram type name 2");
    CmdArgStr level_name( 'L', "level_name", "level_name", "level EF, L2 or L2EF");
    CmdArgInt n_sleep('d',"delay","n_sleep","delay between publishing ",1);
    
    CmdLine cmd( *argv, &partition_name, &server_name, &provider_name,&histogram_name,&level_name,&n_sleep,0);
    
    CmdArgvIter arg_iter( --argc, ++argv );
    cmd.description( "OH histogram publisher utility" );
    cmd.parse( arg_iter );

    
    if( partition_name.isNULL( ) ) partition_name = "MyTestPartition";
    if( server_name.isNULL( ) ) server_name = "Histogramming";
    if( provider_name.isNULL( ) ) provider_name = "Top-EF-EBEF-Segment";
    //if( histogram_name.isNULL( ) ) histogram_name = "/EXPERT/TrigSteer_EF/signatureAcceptance";
    if( histogram_name.isNULL( ) ) histogram_name = "signatureAcceptance";
    
    if( provider_name2.isNULL( ) ) provider_name2 = "Top-LVL2-L2-Segment";
    //if( histogram_name2.isNULL( ) ) histogram_name2 = "/EXPERT/TrigSteer_L2/signatureAcceptance";
    if( histogram_name2.isNULL( ) ) histogram_name2 = "signatureAcceptance";
    int delay=(int)n_sleep;  if(delay<1)delay=1;
    if( level_name.isNULL() )level_name="EF";
    
    std::string level((const char*)level_name);
    cout<<"publish_test_histo (delay "<<delay<<") to Partition "<<partition_name<<" server  "<<server_name<<"  level "<<level_name<<endl;
    if(level.find("L2") != std::string::npos )
      cout<<" for level L2 use provider "<<provider_name2<<" and histogram  "<<histogram_name2<<endl;
    if(level.find("EF") != std::string::npos )
      cout<<" for level EF use provider "<<provider_name<<" and histogram  "<<histogram_name<<endl;
    
    int tag = -1;
    int bins_number = 10;
    
    
    //
    // Create a ROOT provider
    IPCPartition p( partition_name );


    //publish run number 
    
    ISInfoDictionary dict( p );
    
    char pubname[100];
    ISInfoInt runNr(123456);
    int lb=1;
    cout << " new example runnr  " << runNr <<" lb="<<lb<< endl;
    try {
      sprintf( pubname, "%s.LB_CTP_%d_%d","LumiBlock",(int)runNr,lb);
      dict.insert( pubname , runNr );   
    }
    catch(...){
      sprintf( pubname, "%s.LB_CTP_%d_%d","LumiBlock", (int) runNr, lb);
      dict.update( pubname , runNr );   
    }

    
    OHRootProvider * provider = 0;
    OHRootProvider * provider2 = 0;
    cout<<" create OHRootProvider "<<endl;

    try
      {
	if(level.find("EF") != std::string::npos ) provider = new OHRootProvider( p, (const char*)server_name, (const char*)provider_name );
      }
    catch( daq::oh::Exception & ex )
    {
        ers::fatal( ex );
        return 1;
    }
    try
      {
	if(level.find("L2") != std::string::npos )	provider2 = new OHRootProvider( p, (const char*)server_name, (const char*)provider_name2 );
      }
    catch( daq::oh::Exception & ex )
    {
        ers::fatal( ex );
        return 1;
    }
    
    cout<<" create histo ! "<<endl;
    //TH1 * histo = 0;
    //histo=new TH1F((const char*)histogram_name,(const char*)histogram_name,bins_number,0.,1.);
    
    TH1 *histo2d=0;
    histo2d=new TH2I((const char*)histogram_name,(const char*)histogram_name,bins_number,0,3,bins_number,0,3);
    TH1 *histo2dL2=0;
    histo2dL2=new TH2I((const char*)histogram_name2,(const char*)histogram_name2,bins_number,0,3,bins_number,0,3);
    
    //get histo from file
    TFile *f=new TFile("debughist.root");
    TH2I *histodebug=(TH2I*)f->Get("debughist");
    TH2I *histodebugL2=(TH2I*)histodebug->Clone("debughistL2");
    
    histo2d=histodebug;
    histo2dL2=histodebugL2;
    
    // create DrawOption annotation if wanted
    std::vector< std::pair<std::string,std::string> > ann;
    ann=oh::util::EmptyAnnotation;
    
    int n_publish=0;
    for ( int ilb=0; ilb<1000; ilb++ )
      { 
	int jump=random()%6-2;
	int lb=ilb;//+jump;
	if(lb==10)lb=3;
	
	char new_histo_name[150];
	//sprintf(new_histo_name,"/EXPERT/TrigSteer_EF/signatureAcceptance_%i",lb);
	sprintf(new_histo_name,"signatureAcceptance_%i",lb);
	char new_histo_name2[150];
	//sprintf(new_histo_name2,"/EXPERT/TrigSteer_L2/signatureAcceptance_%i",lb);
	sprintf(new_histo_name2,"signatureAcceptance_%i",lb);
	
	cout<<"new LB publish  "<<lb<<"  --> histo "<<new_histo_name<<"/"<<new_histo_name2<<endl;
	histo2d->SetName(new_histo_name);
	histo2dL2->SetName(new_histo_name2);
	
	histo2dL2->SetTitle("Raw acceptance of signatures in L2 for Run Summary");
	
	for(int ix=1;ix<histo2d->GetNbinsX();ix++){
	  for(int iy=1;iy<histo2d->GetNbinsY();iy++){
	    int cc=histo2d->GetBinContent(ix,iy);
	    if(cc>0){cc++;}
	    histo2d->SetBinContent(ix,iy,cc);
	    histo2dL2->SetBinContent(ix,iy,cc);
	  }
	}
	
	int updateTimes = random()%4 + 1;
	for(int i=0;i<updateTimes;i++)
	  {
	    histo2d->Fill(1,1);
	    try
	      {	if(level.find("EF") != std::string::npos ){
		  n_publish++;
		  provider -> publish( *histo2d, (const char*)new_histo_name, tag, ann );
		  std::cout <<n_publish<< ". publish successfully " <<new_histo_name<<" (EF) "<< std::endl;
		}
	      }
	    catch( daq::oh::Exception & ex )
	      {
		ers::error ( ex );
		return 1;
	      }
	    try
	      {
		if(level.find("L2") != std::string::npos ){
		  n_publish++;
		  provider2 -> publish( *histo2dL2, (const char*)new_histo_name2, tag, ann );
		  std::cout <<n_publish<< ". publish successfully " <<new_histo_name<<" (L2) "<< std::endl;
		}
	      }
	    catch( daq::oh::Exception & ex )
	      {
		ers::error ( ex );
		return 1;
	      }
	    
	    sleep(delay);
	  }
      }
    
    delete histo2d;
    delete histo2dL2;
}

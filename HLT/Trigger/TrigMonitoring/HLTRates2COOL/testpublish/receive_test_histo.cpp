/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file publish_test_data.cpp
 * @author Andrzej Dworak andrzej.dworak@cern.ch
 * @date 13/08/2007
 * @brief Dummy data ganarator. Publishing through IS.
 */


#include <iostream>
using namespace std;

#include <cmdl/cmdargs.h>
#include <ipc/core.h>
#include <owl/regexp.h>

// OH include files
#include <TH2I.h>
#include <oh/OHRootReceiver.h>
#include <oh/OHSubscriber.h>



using namespace std;
class MyReceiver : public OHRootReceiver {
public:
  MyReceiver( int cnt )
    : counter_( 0 ),
      output_counter_( cnt ),
      maxlbdist_(10),
      lbnumbers_(0),
      lbdone_(0)

  { ; }
    
  void receive( OHRootHistogram & h)
  {
    
    //if h is not 2D histo : return 
    if ( ++counter_%output_counter_ == 0 )
      {
	std::cout << counter_ << ". notification has been received and ";
	std::string histoname=(std::string) h.histogram->GetName();
	std::cout<<" histogram "<<histoname<<std::endl;
	
	//get LB number and store
	string::size_type loc = histoname.find_first_not_of( "testhisto_", 0 );
	int LB=atoi(histoname.substr(loc).c_str());
	if(lbdone_.size()>0){
	  lbdone_.sort();
	  std::list<int>::iterator iter=lbdone_.begin();
	  int oldlb=*iter;
	  while( LB >= *iter ){
	    if( LB==*iter ){cout<<" this LB was done already! und nun ?? "<<endl;}
	    oldlb=*iter; iter++;
	  }
	}
	
	//store LB number in sorted, unique list lbnumbers_
	unsigned int oldsize=lbnumbers_.size();
	lbnumbers_.push_back(LB);
	lbnumbers_.sort();lbnumbers_.unique();
	
	TH2I *hlb=new TH2I;
	hlb=(TH2I*) h.histogram->Clone();
	lbhistos_.push_back(hlb);
	
	//this LB number not there before -->
	if(lbnumbers_.size() > oldsize){ 
	  //debug print statement
	  cout<<"new LB block,  LBnumbers: "<<endl;
	  for(std::list<int>::iterator iter = lbnumbers_.begin(); iter != lbnumbers_.end(); iter++ ) 
	    {
	      cout << *iter <<", ";
	    }
	  std::cout<<std::endl;
	}
	
	
	//if oldest LB more than maxlbdist_ ago : write out counters for first LB in lbnumbers_ and drop it from the list
	while (lbnumbers_.size() >= maxlbdist_ )
	  {
	    int lbout=lbnumbers_.front();
	    
	    std::cout<<" call unpackHisto for LB ! "<<lbout<<endl;
	    int index=getHistoIndex(lbout);
	    unpackHisto(index);
	    std::cout<<" return from unpackHisto ! "<<index<<endl;
	    
	    lbhistos_.erase(lbhistos_.begin()+index);
	    lbdone_.push_back(lbout);
	    lbnumbers_.pop_front();
	    
	  }
      }
    
    return;
  }
  
  void unpackHisto(int index)
  {
        
    cout<<"now unpack  histo "<<lbhistos_[index]->GetName()<<endl;
    //unpack 2D histo for number of events
    
    //write counters to COOL
    cout<<" and eventually write to COOL ! "<<endl;
    return;
  }
  
  int getHistoIndex(int LB)
  {
    //get histo for this lumiblock LB
    char histogramname[50];
    sprintf(histogramname,"testhisto_%i",LB);
    cout<<" get histo "<<histogramname<<" from vector of histos "<<endl;
    
    int index=0;
    std::vector<TH1*>::iterator iter = lbhistos_.begin();
    while(iter != lbhistos_.end() && strcmp(histogramname,(const char*)(*iter)->GetName()) !=0  ){
      iter++;index++;
    }
    return index;
  }
  
  void receive( vector<OHRootHistogram*> & )
  {
     
    if ( ++counter_%output_counter_ == 0 )
      {
	std::cout << counter_ << " notifications have been received" << std::endl;
      }
  }
    
  void histogramChanged( const std::string & , const OWLTime & , OHReceiver::Reason )
  {
     
    if ( ++counter_%output_counter_ == 0 )
      {
	std::cout << counter_ << " notifications have been received" << std::endl;
      }
  }
    
  void receive( OHRootGraph & )
  {
     
    if ( ++counter_%output_counter_ == 0 )
      {
	std::cout << counter_ << " notifications have been received" << std::endl;
      }
  }
    
  void receive( vector<OHRootGraph*> & )
  {
     
    if ( ++counter_%output_counter_ == 0 )
      {
	std::cout << counter_ << " notifications have been received" << std::endl;
      }
  }
    
  void receive( OHRootGraph2D & )
  {
     
    if ( ++counter_%output_counter_ == 0 )
      {
	std::cout << counter_ << " notifications have been received" << std::endl;
      }
  }
    
  void receive( vector<OHRootGraph2D*> & )
  {
     
    if ( ++counter_%output_counter_ == 0 )
      {
	std::cout << counter_ << " notifications have been received" << std::endl;
      }
  }
  
  void setMax(unsigned int max)
  {
    maxlbdist_ = max;
  }
  
      

  

private:

  int counter_;
  int output_counter_;
  unsigned int maxlbdist_;
  std::list<int> lbnumbers_;
  std::list<int>  lbdone_;
  std::vector<TH1*>lbhistos_;
};


int main( int argc, char ** argv )
{

  IPCCore::init( argc, argv );
        
  //
  // Get parameters from the command line
  //
  CmdArgStr	partition_name( 'p', "partition", "partition_name",
				"Partition name", CmdArg::isREQ );
  CmdArgStr	server_name( 's', "server", "server_name",
			     "OH (IS) Server name", CmdArg::isREQ );
  CmdArgStr	provider_name( 'n', "provider", "provider_name",
			       "Histogram provider name" );
  CmdArgStr	histogram_name( 'h', "histogram", "histogram_name",
				"Histogram type name" );
  //CmdArgInt	iters('i', "iters", "iterations", "number of iterations to report");
  //  CmdArgBool	value ('v', "value", "subscribe for receiving histogram values (default off).");
    
    
  CmdLine cmd( *argv, &partition_name, &server_name,
	       &provider_name, &histogram_name, 0 );
    

  //string partition_name = "MyTestPartition";
  //string server_name = "Histogramming";
  //string provider_name= ".*";
  //string histogram_name =".*";
    
  int iters = 1;
  bool value = true;//false;
    
  CmdArgvIter arg_iter( --argc, ++argv );
  cmd.description( "OH Histogram display utility" );
  cmd.parse(arg_iter);
    
    
    
  if( provider_name.isNULL( ) ) provider_name = ".*";
  if( histogram_name.isNULL( ) ) histogram_name = ".*";
    
  try
    {
      MyReceiver receiver( iters );
      receiver.setMax(4);
	
      IPCPartition p( partition_name );
      OHSubscriber ohhs( p, (const char *)server_name, receiver, false );
      OWLRegexp provider(provider_name);
      OWLRegexp histogram(histogram_name);
      ohhs.subscribe( provider, histogram, value );
	
      IPCServer server;
      
      std::cout << "OH test receiver for the \"" << provider_name << ":" << histogram_name <<
	"\" has been started in the \"" << partition_name << "\" partition" << std::endl;
	
      server.run();
    }
  catch ( daq::oh::Exception & ex )
    {
      ers::fatal( ex );
      return 1;
    }
    
  return 0;
}

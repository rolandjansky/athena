/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file publish_test_data.cpp
 * @author Andrzej Dworak andrzej.dworak@cern.ch
 * @date 13/08/2007
 * @brief Dummy data ganarator. Publishing through IS.
 */

#include <iostream.h>
#include <ipc/core.h>


//#include "is/infoany.h"
//#include <is/infoT.h>
//#include <is/infodictionary.h>
//#include <is/infoiterator.h>
//#include <is/inforeceiver.h>
//#include <is/serveriterator.h>
#include <ipc/partition.h>
//#include "is/infodocument.h"

#include <TH1F.h>
#include <oh/OHRootProvider.h>
#include <oh/OHRootReceiver.h>

#include <map>


using namespace std;
int main( int argc, char ** argv )
{

  string testPartition = "MyTestPartition";
  string server_name = "Histogramming";
  string provider_name= "MyTest";
  string histogram_name ="testhisto";
  
  IPCCore::init( argc, argv );
  IPCPartition partition( testPartition );
  OHRootProvider * provider = 0;
  try
    {
      provider = new OHRootProvider( partition, server_name, provider_name );
    }
  catch( daq::oh::Exception & ex )
    {
    	ers::fatal( ex );
      return 1;
    }
  

  TH1 * histo = 0;
  histo =new TH1F("testhisto","testhisto",100,0.,2.);
  
  // create DrawOption annotation if wanted
  std::vector< std::pair<std::string,std::string> > ann;
  //if (drawoption.isNULL())
  ann=oh::util::EmptyAnnotation;
  //else {
  //ann.resize(1);
  //ann[0].first="DrawOption";
  //ann[0].second=drawoption;
  // }

  int n_publish=0;
  for ( int lb=0; lb<1000; lb++ )
    {
      n_publish++;
      try
        {
          histo->Fill((float) lb);
          std::string name;std::string tmpstring; 
          tmpstring=Form(histogram_name.c_str()); tmpstring += "_%i";
          name=Form(tmpstring.c_str(),lb);
          cout<<" name ? "<<name<<endl;
          provider -> publish( *histo, name, n_publish, ann );
          cout << n_publish<<" published histo example " << name << endl;
        }
      catch(...)
        {
          cout<<" could not publish histogram! "<<endl;
        }
      
      int updateTimes = random()%10 + 1;
      for(int i=0;i<updateTimes;i++)
        {
	  
          cout << endl << "loop update no. " << i << endl;
          try{
            histo->Fill((float)lb);
            n_publish++;   
            std::string name;std::string tmpstring; 
            tmpstring=Form(histogram_name.c_str()); tmpstring += "_%i"; 
            name=Form(tmpstring.c_str(),lb);
            cout<<" name ? "<<name<<endl;
            provider -> publish( *histo, name, n_publish, ann );
            cout << n_publish<<" published histo example " << name << endl;
          }
          catch(...)
            {
              cout<<" could not publish histogram! "<<endl;
            }
          sleep( 5 );
        }
    }
  
  return 0;
}

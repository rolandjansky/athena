/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctype.h>
#include <string>

#include "TrigT1TGC/TGCConnectionASDToPP.hh"
#include "TrigT1TGC/TGCConnectionPPToSB.hh"
#include "TrigT1TGC/TGCDatabaseManager.hh"

#include "PathResolver/PathResolver.h"


namespace LVL1TGCTrigger {

//   Strip backwardLayer  ->                      0  1  2  3  4  5  6  7  8
const int TGCConnectionASDToPP::forwardLayer[] = {2, 1, 0, 4, 3, 6, 5, 8, 7};

int TGCConnectionASDToPP::getConnection(const int /*sideId*/, const int layer, const int chamber,
					const int line, int* pp, int* connector, int* channel)
{
  int i;
  for( i=0; i<totalNumberOfChannel; i+=1){
    // wiregroupID assign in ASDOut and ASD2PP.db are different.
    if((layerId[i]==layer)&&(chamberId[i]==chamber)&&(lineId[i]==line)){
    //    if((layerId[i]==layer)&&(lineId[i]==line)){
      *pp = PPId[i];
      *connector = connectorId[i];
      *channel = channelId[i];
      return 0;
    }
  }
  *pp=-2;
  *connector=-2;
  *channel=-2;
  return -1;
}

void TGCConnectionASDToPP::dump() const
{
#ifdef TGCCOUT
  int i;
  std::cout << "TGCConnectionASDToPP::dump "<<totalNumberOfChannel<< std::endl;
  std::cout << "layerId chamberID line(WireGroup)  PPID ConnectorID" << std::endl;
  for( i=0; i<totalNumberOfChannel; i+=1)
    std::cout<<layerId[i]<<" "<<chamberId[i]<<" "<<lineId[i]<<" "<<PPId[i]<<" "<<connectorId[i]<<" "<<channelId[i]<< std::endl;
#endif
}


bool TGCConnectionASDToPP::readData(TGCRegionType region, int type, 
				    TGCForwardBackwardType forwardBackward)
{
  this->type = type;
  this->forwardBackward = forwardBackward;

  enum { BufferSize = 1024 };
  char buf[BufferSize];

  std::string fn, fullName ;
//  fn = "ASD2PP.db" ;
  fn = TGCDatabaseManager::getFilename(0);

  fullName = PathResolver::find_file(fn.c_str(), "PWD");
  if( fullName.length() == 0 ) 
    fullName = PathResolver::find_file(fn.c_str(), "DATAPATH");
  std::ifstream file(fullName.c_str() ,std::ios::in);
  if (!file) return false;

  std::string PPType;
  while(file.getline(buf,BufferSize)){
    if((buf[0]=='E')||(buf[0]=='F')){

      std::istringstream line(buf);
      int nChannel;
      line >> PPType >> nChannel;
      // find a entry matches in region and Patch Panel type.
      if(((region==Endcap)&&
          (  (PPType=="EWT"&&type==WTPP)||(PPType=="EWD"&&type==WDPP)
           ||(PPType=="EST"&&type==STPP)||(PPType=="ESD"&&type==SDPP)
           ||(PPType=="EWI"&&type==WIPP)||(PPType=="ESI"&&type==SIPP) ) )||
         ((region==Forward)&&
          (  (PPType=="FWT"&&type==WTPP)||(PPType=="FWD"&&type==WDPP)
           ||(PPType=="FST"&&type==STPP)||(PPType=="FSD"&&type==SDPP)
           ||(PPType=="FWI"&&type==WIPP)||(PPType=="FSI"&&type==SIPP) ) )    ){
        totalNumberOfChannel = nChannel;

        layerId = new int [totalNumberOfChannel]; 
        chamberId = new int [totalNumberOfChannel]; 
        lineId = new int [totalNumberOfChannel]; 
        PPId = new int [totalNumberOfChannel]; 
        connectorId = new int [totalNumberOfChannel]; 
        channelId = new int [totalNumberOfChannel]; 
         

        //******************************************************
        // ChamberID in kmura's def. start from 1 in T1 station.
        int chamberIdBase=0;
        if((region==Endcap)&&(PPType=="EWT"||PPType=="EST")) chamberIdBase=1;
        //******************************************************
        int lineIdBase=0;
	// initialize array
        for(int i=0; i<totalNumberOfChannel; i+=1){
	  layerId[i]     = 0;
	  chamberId[i]   = 0;
	  lineId[i]      = 0;
	  PPId[i]        = 0;
	  connectorId[i] = 0;
	  channelId[i]   = 0;
	}
	// read lines
        for(int i=0; i<totalNumberOfChannel; i+=1){
          file.getline(buf,BufferSize);
          std::istringstream line(buf);
          line >> layerId[i] >> chamberId[i] >> lineId[i] 
               >> PPId[i] >> connectorId[i] >> channelId[i]; 

	  // DB is Backward
          if(PPType=="FST"||PPType=="FSD"||PPType=="EST"||PPType=="ESD"||PPType=="FSI"||PPType=="ESI"){
            if(forwardBackward==ForwardSector){
               layerId[i] = forwardLayer[layerId[i]];
            }
          }


          //******************************************************
          // ChamberID in kmura's def. start from 1 in T1 station.
          chamberId[i]+=chamberIdBase;
          //******************************************************
          //******************************************************
          // hitID assign for each Module in kmura's def., 
          // not for each chamber like in hasuko's def. 
          if( (type==WTPP)||(type==WDPP)||(type==WIPP) ){
            if((i!=0)&&(chamberId[i]!=chamberId[i-1])){
              if(layerId[i]==layerId[i-1])
                lineIdBase=lineId[i-1]+1;
              else
                lineIdBase=0;
            }
            lineId[i]+=lineIdBase;
          }
          //******************************************************
        }
        break;
      }
    }
  }
  return true;
}

TGCConnectionASDToPP::TGCConnectionASDToPP(const TGCConnectionASDToPP& right)
{
  totalNumberOfChannel = right.totalNumberOfChannel;
  type = right.type;
  forwardBackward = right.forwardBackward;
  layerId = 0;
  chamberId = 0;
  lineId = 0;
  PPId = 0;
  connectorId = 0;
  channelId = 0;

  if(totalNumberOfChannel!=0){
    layerId = new int [totalNumberOfChannel];
    chamberId = new int [totalNumberOfChannel];
    lineId = new int [totalNumberOfChannel];
    PPId = new int [totalNumberOfChannel];
    connectorId = new int [totalNumberOfChannel];
    channelId = new int [totalNumberOfChannel];

    for( int i=0; i<totalNumberOfChannel; i+=1){
      layerId[i]=right.layerId[i];
      chamberId[i]=right.chamberId[i];
      lineId[i]=right.lineId[i];
      PPId[i]=right.PPId[i];
      connectorId[i]=right.connectorId[i];
      channelId[i]=right.channelId[i];
    }
  }
}

TGCConnectionASDToPP&
TGCConnectionASDToPP::operator=(const TGCConnectionASDToPP& right)
{
  if(this != &right) {
    totalNumberOfChannel = right.totalNumberOfChannel;
    if(layerId!=0) {
      delete [] layerId;
      delete [] chamberId;
      delete [] lineId;
      delete [] PPId;
      delete [] connectorId;
      delete [] channelId;
    }
    if(totalNumberOfChannel!=0){
      layerId = new int [totalNumberOfChannel];
      chamberId = new int [totalNumberOfChannel];
      lineId = new int [totalNumberOfChannel];
      PPId = new int [totalNumberOfChannel];
      connectorId = new int [totalNumberOfChannel];
      channelId = new int [totalNumberOfChannel];
      for( int i=0; i<totalNumberOfChannel; i+=1){
        layerId[i]=right.layerId[i];
        chamberId[i]=right.chamberId[i];
        lineId[i]=right.lineId[i];
        PPId[i]=right.PPId[i];
        connectorId[i]=right.connectorId[i];
        channelId[i]=right.channelId[i];
      }
    }
  } 
  return *this; 
}

TGCConnectionASDToPP::TGCConnectionASDToPP():
  totalNumberOfChannel(0),type(0),forwardBackward(ForwardSector),
  layerId(0),chamberId(0),lineId(0),
  PPId(0),connectorId(0),channelId(0)
{
}


TGCConnectionASDToPP::~TGCConnectionASDToPP()
{
  if(layerId!=0) delete [] layerId;
  layerId=0;
  if(chamberId!=0) delete [] chamberId;
  chamberId=0;
  if(lineId!=0) delete [] lineId;
  lineId=0;
  if(PPId!=0) delete [] PPId;
  PPId=0;
  if(connectorId!=0) delete [] connectorId;
  connectorId=0;
  if(channelId!=0) delete [] channelId;
  channelId=0;
}

} //end of namespace bracket

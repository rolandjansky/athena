/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
const int TGCConnectionASDToPP::s_forwardLayer[] = {2, 1, 0, 4, 3, 6, 5, 8, 7};

int TGCConnectionASDToPP::getConnection(const int /*sideId*/, const int layer, const int chamber,
					const int line, int* pp, int* connector, int* channel)
{
  int i;
  for( i=0; i<m_totalNumberOfChannel; i+=1){
    // wiregroupID assign in ASDOut and ASD2PP.db are different.
    if((m_layerId[i]==layer)&&(m_chamberId[i]==chamber)&&(m_lineId[i]==line)){
    //    if((m_layerId[i]==layer)&&(m_lineId[i]==line)){
      *pp = m_PPId[i];
      *connector = m_connectorId[i];
      *channel = m_channelId[i];
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
  std::cout << "TGCConnectionASDToPP::dump "<<m_totalNumberOfChannel<< std::endl;
  std::cout << "layerId chamberID line(WireGroup)  PPID ConnectorID" << std::endl;
  for( i=0; i<m_totalNumberOfChannel; i+=1)
    std::cout<<m_layerId[i]<<" "<<m_chamberId[i]<<" "<<m_lineId[i]<<" "<<m_PPId[i]<<" "<<m_connectorId[i]<<" "<<m_channelId[i]<< std::endl;
#endif
}


bool TGCConnectionASDToPP::readData(TGCRegionType region, int type, 
				    TGCForwardBackwardType forwardBackward)
{
  this->m_type = type;
  this->m_forwardBackward = forwardBackward;

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
      // find a entry matches in region and Patch Panel m_type.
      if(((region==Endcap)&&
          (  (PPType=="EWT"&&m_type==WTPP)||(PPType=="EWD"&&m_type==WDPP)
           ||(PPType=="EST"&&m_type==STPP)||(PPType=="ESD"&&m_type==SDPP)
           ||(PPType=="EWI"&&m_type==WIPP)||(PPType=="ESI"&&m_type==SIPP) ) )||
         ((region==Forward)&&
          (  (PPType=="FWT"&&m_type==WTPP)||(PPType=="FWD"&&m_type==WDPP)
           ||(PPType=="FST"&&m_type==STPP)||(PPType=="FSD"&&m_type==SDPP)
           ||(PPType=="FWI"&&m_type==WIPP)||(PPType=="FSI"&&m_type==SIPP) ) )    ){
        m_totalNumberOfChannel = nChannel;

        m_layerId = new int [m_totalNumberOfChannel]; 
        m_chamberId = new int [m_totalNumberOfChannel]; 
        m_lineId = new int [m_totalNumberOfChannel]; 
        m_PPId = new int [m_totalNumberOfChannel]; 
        m_connectorId = new int [m_totalNumberOfChannel]; 
        m_channelId = new int [m_totalNumberOfChannel]; 
         

        //******************************************************
        // ChamberID in kmura's def. start from 1 in T1 station.
        int chamberIdBase=0;
        if((region==Endcap)&&(PPType=="EWT"||PPType=="EST")) chamberIdBase=1;
        //******************************************************
        int lineIdBase=0;
	// initialize array
        for(int i=0; i<m_totalNumberOfChannel; i+=1){
	  m_layerId[i]     = 0;
	  m_chamberId[i]   = 0;
	  m_lineId[i]      = 0;
	  m_PPId[i]        = 0;
	  m_connectorId[i] = 0;
	  m_channelId[i]   = 0;
	}
	// read lines
        for(int i=0; i<m_totalNumberOfChannel; i+=1){
          file.getline(buf,BufferSize);
          std::istringstream line(buf);
          line >> m_layerId[i] >> m_chamberId[i] >> m_lineId[i] 
               >> m_PPId[i] >> m_connectorId[i] >> m_channelId[i]; 

	  // DB is Backward
          if(PPType=="FST"||PPType=="FSD"||PPType=="EST"||PPType=="ESD"||PPType=="FSI"||PPType=="ESI"){
            if(forwardBackward==ForwardSector){
               m_layerId[i] = s_forwardLayer[m_layerId[i]];
            }
          }


          //******************************************************
          // ChamberID in kmura's def. start from 1 in T1 station.
          m_chamberId[i]+=chamberIdBase;
          //******************************************************
          //******************************************************
          // hitID assign for each Module in kmura's def., 
          // not for each chamber like in hasuko's def. 
          if( (m_type==WTPP)||(m_type==WDPP)||(m_type==WIPP) ){
            if((i!=0)&&(m_chamberId[i]!=m_chamberId[i-1])){
              if(m_layerId[i]==m_layerId[i-1])
                lineIdBase=m_lineId[i-1]+1;
              else
                lineIdBase=0;
            }
            m_lineId[i]+=lineIdBase;
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
  m_totalNumberOfChannel = right.m_totalNumberOfChannel;
  m_type = right.m_type;
  m_forwardBackward = right.m_forwardBackward;
  m_layerId = 0;
  m_chamberId = 0;
  m_lineId = 0;
  m_PPId = 0;
  m_connectorId = 0;
  m_channelId = 0;

  if(m_totalNumberOfChannel!=0){
    m_layerId = new int [m_totalNumberOfChannel];
    m_chamberId = new int [m_totalNumberOfChannel];
    m_lineId = new int [m_totalNumberOfChannel];
    m_PPId = new int [m_totalNumberOfChannel];
    m_connectorId = new int [m_totalNumberOfChannel];
    m_channelId = new int [m_totalNumberOfChannel];

    for( int i=0; i<m_totalNumberOfChannel; i+=1){
      m_layerId[i]=right.m_layerId[i];
      m_chamberId[i]=right.m_chamberId[i];
      m_lineId[i]=right.m_lineId[i];
      m_PPId[i]=right.m_PPId[i];
      m_connectorId[i]=right.m_connectorId[i];
      m_channelId[i]=right.m_channelId[i];
    }
  }
}

TGCConnectionASDToPP&
TGCConnectionASDToPP::operator=(const TGCConnectionASDToPP& right)
{
  if(this != &right) {
    m_totalNumberOfChannel = right.m_totalNumberOfChannel;
    if(m_layerId!=0) {
      delete [] m_layerId;
      delete [] m_chamberId;
      delete [] m_lineId;
      delete [] m_PPId;
      delete [] m_connectorId;
      delete [] m_channelId;
    }
    if(m_totalNumberOfChannel!=0){
      m_layerId = new int [m_totalNumberOfChannel];
      m_chamberId = new int [m_totalNumberOfChannel];
      m_lineId = new int [m_totalNumberOfChannel];
      m_PPId = new int [m_totalNumberOfChannel];
      m_connectorId = new int [m_totalNumberOfChannel];
      m_channelId = new int [m_totalNumberOfChannel];
      for( int i=0; i<m_totalNumberOfChannel; i+=1){
        m_layerId[i]=right.m_layerId[i];
        m_chamberId[i]=right.m_chamberId[i];
        m_lineId[i]=right.m_lineId[i];
        m_PPId[i]=right.m_PPId[i];
        m_connectorId[i]=right.m_connectorId[i];
        m_channelId[i]=right.m_channelId[i];
      }
    }
  } 
  return *this; 
}

TGCConnectionASDToPP::TGCConnectionASDToPP():
  m_totalNumberOfChannel(0),m_type(0),m_forwardBackward(ForwardSector),
  m_layerId(0),m_chamberId(0),m_lineId(0),
  m_PPId(0),m_connectorId(0),m_channelId(0)
{
}


TGCConnectionASDToPP::~TGCConnectionASDToPP()
{
  if(m_layerId!=0) delete [] m_layerId;
  m_layerId=0;
  if(m_chamberId!=0) delete [] m_chamberId;
  m_chamberId=0;
  if(m_lineId!=0) delete [] m_lineId;
  m_lineId=0;
  if(m_PPId!=0) delete [] m_PPId;
  m_PPId=0;
  if(m_connectorId!=0) delete [] m_connectorId;
  m_connectorId=0;
  if(m_channelId!=0) delete [] m_channelId;
  m_channelId=0;
}

} //end of namespace bracket

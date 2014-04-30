/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCConnectionPPToSL.hh"
#include "TrigT1TGC/TGCDatabaseManager.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "PathResolver/PathResolver.h"

namespace LVL1TGCTrigger {

TGCConnectionPPToSL::TGCConnectionPPToSL():
  PPToSB(),
  SBToHPB(),
  HPBToSL()
{
}

TGCConnectionPPToSL::TGCConnectionPPToSL(const TGCConnectionPPToSL& right):
  PPToSB(right.PPToSB),
  SBToHPB(right.SBToHPB),
  HPBToSL(right.HPBToSL)
{
}


bool TGCConnectionPPToSL::readData(TGCRegionType type)
{
  int id,port,idHPB,idSB1,idSB2;
  std::string Region,Name;

  std::string fn, fullName ;
  //  fn = "PP2SL.db" ;
  fn = TGCDatabaseManager::getFilename(2);

  fullName = PathResolver::find_file (fn.c_str(), "PWD");
  if( fullName.length() == 0 ) 
    fullName = PathResolver::find_file (fn.c_str(), "DATAPATH");
  std::ifstream inputfile(fullName.c_str() ,std::ios::in);

  enum{ BufferSize = 1024 };
  char buf[1024];

  // create arrays of number of boards for each type. 
  int* NHPB = new int [HPBToSL.getNumberOfType()];
  int* NSB = new int [SBToHPB.getNumberOfType()];
  int* NPP = new int [PPToSB.getNumberOfType()];

  // find entries match in region type.
  bool isMatched = false;
  while ( inputfile && inputfile.getline(buf,BufferSize)) {
    std::istringstream line(buf);
    line >> Region ;
    isMatched =  (Region=="Endcap"&&type==Endcap)
              || (Region=="Forward"&&type==Forward);
    if (isMatched) break;
  } 
  if (!isMatched) {
#ifdef TGCDEBUG
    std::cout << "TGCConnectionPPToSL::readData : fail to read out"
	      << std::endl;
#endif    
    delete [] NHPB;
    delete [] NSB;
    delete [] NPP;
    return false;
  }
  
  // read entries for HighPtBoard
  if(inputfile.getline(buf,BufferSize)){
    std::istringstream infileS(buf);
    infileS >> Name >> NHPB[WHPB] >> NHPB[SHPB];
#ifdef TGCDEBUG
    std::cout<<Name<<" "<<NHPB[WHPB]<<" "<<NHPB[SHPB]<<std::endl;
#endif
    for(int i=0; i<HPBToSL.getNumberOfType(); i+=1){
      HPBToSL.setNumber(i,NHPB[i]);
      for (int j=0; j<NHPB[i]; j+=1) {
	inputfile.getline(buf,BufferSize);
	std::istringstream infileS2(buf);
	infileS2 >> id >> port;
	HPBToSL.setId(i,j,id);
#ifdef TGCDEBUG
	std::cout<<"HPBID="<<HPBToSL.getId(i,j)<<std::endl;
#endif
	HPBToSL.setSLPortToHPB(i,id,port);
      }
    }
  }
  
  // read entries for SlaveBoard
  if(inputfile.getline(buf,BufferSize)){
    std::istringstream infileS(buf);
    infileS >> Name >> NSB[WTSB] >> NSB[WDSB] >> NSB[STSB] >> NSB[SDSB];
#ifdef TGCDEBUG
    std::cout<<Name<<" "<<NSB[WTSB]<<" "<<NSB[WDSB]<<" "<<NSB[STSB]<<" "<<NSB[SDSB]<<std::endl;
#endif
    for(int i=0; i<SBToHPB.getNumberOfType(); i+=1){
      // No HPB for Inner
      if ( i == WISB ) continue;
      if ( i == SISB ) continue;
      SBToHPB.setNumber(i,NSB[i]);
      for (int  j=0; j<NSB[i]; j+=1) {
	inputfile.getline(buf,BufferSize);
	std::istringstream infileS2(buf);
	infileS2 >> id >> idHPB >> port;
#ifdef TGCDEBUG
	std::cerr<<  id <<" " << idHPB <<" " << port <<std::endl;
#endif
	SBToHPB.setId(i,j,id); //BoardType, Number in a type, id
	SBToHPB.setHPBIdToSB(i,id,idHPB);
	SBToHPB.setHPBPortToSB(i,id,port);
      }
    }
  }
  
  // read entries for PatchPanel
  if(inputfile.getline(buf,BufferSize)){
    std::istringstream infileS(buf);
    infileS >> Name 
	    >> NPP[WTPP] >> NPP[WDPP]
	    >> NPP[STPP] >> NPP[SDPP]
	    >> NPP[WIPP] >> NPP[SIPP];
#ifdef TGCDEBUG
    std::cout<<Name<<" "<<NPP[WTPP]<<" "<<NPP[WDPP]
	     <<" "<<NPP[STPP]<<" "<<NPP[SDPP]
	     <<" "<<NPP[WIPP]<<" "<<NPP[SIPP]
	     <<std::endl;
#endif
    for(int i=0; i<PPToSB.getNumberOfType(); i+=1){
      PPToSB.setNumber(i,NPP[i]);
      for(int j=0; j<NPP[i]; j+=1) {
	inputfile.getline(buf,BufferSize);
	std::istringstream infileS2(buf);
	infileS2 >> id; //PP ID
	PPToSB.setId(i,j,id);

	infileS2 >> idSB1; //SB ID for Port0
	PPToSB.setSBIdToPP(i,0,j,idSB1);//!! assume id = index 
	infileS2 >> idSB2; //SB ID for Port1
	PPToSB.setSBIdToPP(i,1,j,idSB2);//!! assume id = index 

#ifdef TGCDEBUG
	if ((i==WIPP)||(i==SIPP)) {
	  std::cout << "type: " << i 
		    << " PP id: " << id 
		    << " SBid:  " << idSB1 << "," << idSB2 
		    << std::endl;
	}
#endif
      }
    }
  }
  
  delete [] NHPB;
  delete [] NSB;
  delete [] NPP;

  return true;
}



} //end of namespace bracket

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


TGCConnectionPPToSL& TGCConnectionPPToSL::operator =(const TGCConnectionPPToSL& right)
{ 
  if (this != &right) {
    PPToSB  = right.PPToSB;
    SBToHPB = right.SBToHPB;
    HPBToSL = right.HPBToSL;
  }
  return *this;
}

bool TGCConnectionPPToSL::readData(TGCRegionType type)
{
  int id,port,idHPB,idSB1,idSB2;
  std::string Region,Name;

  std::string fn, fullName ;
  //  fn = "PP2SL.db" ;
  fn = TGCDatabaseManager::getFilename(2);

  fullName = PathResolver::find_file(fn.c_str(), "PWD");
  if( fullName.length() == 0 ) 
    fullName = PathResolver::find_file (fn.c_str(), "DATAPATH");
  std::ifstream inputfile(fullName.c_str() ,std::ios::in);

  enum{ BufferSize = 1024 };
  char buf[1024];

  // create arrays of number of boards for each type. 
  const int NHPB= HPBToSL.getNumberOfType();
  int* aNHPB = new int [NHPB];
  const int NSB = SBToHPB.getNumberOfType();
  int* aNSB = new int [NSB];
  const int NPP = PPToSB.getNumberOfType();
  int* aNPP = new int [NPP];

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
    delete [] aNHPB;
    delete [] aNSB;
    delete [] aNPP;
    return false;
  }
  
  // read entries for HighPtBoard
  if(inputfile.getline(buf,BufferSize)){
    std::istringstream infileS(buf);
    infileS >> Name >> aNHPB[WHPB] >> aNHPB[SHPB];
#ifdef TGCDEBUG
    std::cout<<Name<<" "<<aNHPB[WHPB]<<" "<<aNHPB[SHPB]<<std::endl;
#endif
    for(int i=0; i<NHPB; i++) {
      HPBToSL.setNumber(i, aNHPB[i]);
      for (int j=0; j<aNHPB[i]; j+=1) {
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
    infileS >> Name >> aNSB[WTSB] >> aNSB[WDSB] >> aNSB[STSB] >> aNSB[SDSB];
#ifdef TGCDEBUG
    std::cout<<Name<<" "<<aNSB[WTSB]<<" "<<aNSB[WDSB]<<" "<<aNSB[STSB]<<" "<<aNSB[SDSB]<<std::endl;
#endif
    for(int i=0; i<NSB; i++){
      if(i>=TotalNumSlaveBoardType) continue;   // for temporary coverity fix
      // No HPB for Inner
      if ( i == WISB ) continue;
      if ( i == SISB ) continue;
      SBToHPB.setNumber(i,aNSB[i]);
      for (int  j=0; j<aNSB[i]; j+=1) {
	inputfile.getline(buf,BufferSize);
	std::istringstream infileS2(buf);
	infileS2 >> id >> idHPB >> port;
#ifdef TGCDEBUG
	std::cerr<<  id <<" " << idHPB <<" " << port <<std::endl;
#endif
        if (id<0 || idHPB<0 || port<0) continue;
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
	    >> aNPP[WTPP] >> aNPP[WDPP]
	    >> aNPP[STPP] >> aNPP[SDPP]
	    >> aNPP[WIPP] >> aNPP[SIPP];
#ifdef TGCDEBUG
    std::cout<<Name<<" "<<aNPP[WTPP]<<" "<<aNPP[WDPP]
	     <<" "<<aNPP[STPP]<<" "<<aNPP[SDPP]
	     <<" "<<aNPP[WIPP]<<" "<<aNPP[SIPP]
	     <<std::endl;
#endif
    for(int i=0; i<NPP; i+=1){
      PPToSB.setNumber(i,aNPP[i]);
      for(int j=0; j<aNPP[i]; j+=1) {
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
  
  delete [] aNHPB;
  delete [] aNSB;
  delete [] aNPP;

  return true;
}



} //end of namespace bracket

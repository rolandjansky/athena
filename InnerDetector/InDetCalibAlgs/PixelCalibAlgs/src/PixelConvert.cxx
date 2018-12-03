/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCalibAlgs/PixelConvert.h" 
#include <algorithm>

namespace PixelConvert {

  const int NLayer=3;
  const int BSMax[]={11,19,26};
  const int EtaMax=6;
  const int NinSector=6;
  const int NinDisk=48;
  const std::string Error="InvalidChannel";
  
  std::string PackOfflineID(int system,
			    int subSystem,
			    int endcap,
			    int layer,
			    int phi,
			    int eta) {
    std::ostringstream s;
    s << "[" << system << "." << subSystem << "." << endcap << "."
      << layer << "." << phi << "." << eta << ".0]";
    return s.str();
  }

  int UnpackOfflineID(std::string offlineID,
		       int& system,
		       int& subSystem,
		       int& endcap,
		       int& layer,
		       int& phi,
		       int& eta) {
    std::istringstream s(offlineID);
    char c;
    s >> c >> system;
    s >> c >> subSystem;
    s >> c >> endcap;
    s >> c >> layer;
    s >> c >> phi;
    s >> c >> eta;
    if (s.bad()) return 0;
    else return 1;
  }

  std::string OnlineID(int system, 
		  int subSystem, 
		  int endcap, 
		  int layer,
		  int phi,
		  int eta) {
    std::ostringstream s;
    if ( system!=2 && subSystem!=1 ) return Error;    // not InDet/Pixel
    if (layer<0 || layer>=NLayer) return Error;       // invalid layer
    switch (endcap) {
    case 0:
      if (phi<0 || phi>=2*BSMax[layer]) return Error; // invalid phi position
      if (abs(eta)>EtaMax) return Error;              // invalid eta position
      {
	// now set the right offset, so that phi=0 corresponds to 
	// B01-S01. This is different for different layers
	switch (layer) { 
	case 0: // BLayer, first bistave corresponds to phi=1
	  phi-=1;
	  break;
	case 1: // Layer1, first bistave corresponds to phi=0
	  break;
	case 2: // Layer2, first bistave corresponds to phi=51
	  phi+=1;
	  break;
	default:
	  return Error;
	}
	if (phi<0) phi+=2*BSMax[layer];
	else if (phi>=2*BSMax[layer]) phi-=2*BSMax[layer];
	int bist=phi/2+1;
	int stav=phi%2+1;
	s << "L" << layer;
	if (bist<10) s << "-B0";
	else s << "-B"; 
	s << bist << "-S0" << stav << "-";
        if ( stav==1 ) {
	  if ( eta<=0 ) s << "C7";
	  else s << "A6";
	} else {
	  if ( eta<0 ) s << "C6";
	  else s << "A7";
	}
	s << "-M" << abs(eta);
      }
      if ( eta>0 ) s << "A";
      else if (eta<0) s<< "C"; 
      break;
    case 2:
    case -2:
      if (phi<0 || phi>=NinDisk ) return Error;       // invalid phi position
      if (eta!=0) return Error;                       // invalid eta position
      s << "D" << layer+1;
      if (endcap>0) s << "A";
      else s << "C";
      {
	int sector  = phi/NinSector+1;
	int inSector= phi%NinSector;
	s << "-S0" << sector << "-M";
	if (endcap>0) {
	  if ((inSector%2)==0) // A side, facing IP
	    s << inSector/2+1;
	  else                 // A side, away from IP
	    s << NinSector-(inSector/2);
	} else {
	  if ((inSector%2)==0) // C side, away IP
	    s << inSector/2+4;
	  else 
	    s << NinSector-3-inSector/2;
	}
      }
      break;
    default:
      return Error;  // invalid barrel/endcap selection
    }
    return s.str();
  }

  std::string OnlineID(std::string offlineID) {
    int system;
    int subSystem;
    int endcap;
    int layer;
    int phi;
    int eta;
    if ( UnpackOfflineID(offlineID,system,subSystem,endcap,layer,phi,eta) )
      return OnlineID(system,
		      subSystem,
		      endcap,
		      layer,
		      phi,
		      eta);
    else return Error;
  }

  std::string OnlineID(int hashID) {
    return OnlineID(OfflineID(hashID));
  }


  int OfflineID(std::string onlineID,
		int& system, 
		int& subSystem, 
		int& endcap, 
		int& layer,
		int& phi,
		int& eta) {
    std::istringstream s(onlineID);
    system = 2; // ID
    subSystem=1; // Pixel
    char c1, c;
    s >> c1;
    switch (c1) {
    case 'D':
      endcap=2;
      eta=0;
      s >> layer;
      if (layer<=0 || layer>NLayer ) return 0;
      layer--;
      s >> c;
      switch (c) {
      case 'C': // change sign of endcap;
	endcap=-endcap;
      case 'A': // do nothing
	break;
      default: // invalid endcap
	return 0;
      }
      s >> c >> c;
      if ( c!='S' ) return 0;
      { 
	int sect, insect;
	s >> sect;
	if ( sect<=0 || sect>(NinDisk/NinSector)) return 0;
	s >> c >> c;
	if ( c != 'M' ) return 0;
        s >> insect;
        if ( insect<=0 || insect>NinSector ) return 0;
	if (insect<=NinSector/2) { // facing IP
	  if ( endcap>0)
	    phi=NinSector*(sect-1)+2*(insect-1);
	  else
	    phi=NinSector*(sect-1)+NinSector-2*insect+1;
	} else {                   // away from IP
	  if ( endcap>0)
	    phi=NinSector*(sect-1)+NinSector-2*(insect-NinSector/2)+1;
	  else
	    phi=NinSector*(sect-1)+2*(insect-NinSector/2-1);
	}
      } 
      break;
    case 'L':
      endcap=0;
      s >> layer;
      if ( s.bad() || layer<0 || layer>=NLayer ) return 0;
      s >> c >> c;
      if ( c!='B' ) return 0;
      {
	int bist, stav;
	s >> bist >> c >> c >> stav;
	if ( c!='S' || bist<=0 || bist>BSMax[layer] || stav<=0 || stav>2 ) 
	  return 0;
        phi=2*(bist-1)+stav-1;
	// now set the right offset, so that phi=0 corresponds to 
	// B01-S01. This is different for different layers
	switch (layer) { 
	case 0: // BLayer, first bistave corresponds to phi=1
	  phi+=1;
	  break;
	case 1: // Layer1, first bistave corresponds to phi=0
	  break;
	case 2: // Layer2, first bistave corresponds to phi=51
	  phi-=1;
	  break;
	default:
	  return 0;
	}
	if (phi<0) phi+=2*BSMax[layer];
	else if (phi>=2*BSMax[layer]) phi-=2*BSMax[layer];
      }
      s >> c >> c >> c >> c >> c;
      if ( c!='M' ) return 0;
      s >> eta;
      if ( eta<0 || eta > EtaMax) return 0;
      if (eta>0) { 
	s >> c;
	switch (c) {
	case 'C': // change sign of eta;
	  eta=-eta;
	case 'A': // do nothing
	  break;
	default: // invalid side
	  return 0;
	}
      }
      break;
    default:
      return 0; // invalid endcap/barrel selection
    }
      if (s.bad()) return 0;
      else return 1;
  }

  std::string OfflineID(std::string onlineID) {
    int system, subSystem, endcap, layer, phi, eta;
    if ( OfflineID(onlineID,system,subSystem,endcap,layer,phi,eta) )
      return PackOfflineID(system,subSystem,endcap,layer,phi,eta);
    else return Error;
  }

  int OfflineID(int hashID,
		int& system, 
		int& subSystem, 
		int& endcap, 
		int& layer,
		int& phi,
		int& eta) {
    system    = ((hashID>>29)& 0x7)+2;
    subSystem = ((hashID>>27)& 0x3)+1;
    endcap    = ((hashID>>25)& 0x3);
    endcap = 2*(endcap-1);
    layer     = ((hashID>>23)& 0x3);
    phi       = ((hashID>>17)&0x3f);
    eta       = ((hashID>>13)& 0xf);
    // if (endcap==0) eta-=6;
    eta-=6;
    return 1;
  }

  std::string OfflineID(int hashID) {
    int system, subSystem, endcap, layer, phi, eta;
    OfflineID(hashID,system,subSystem,endcap,layer,phi,eta);
    return PackOfflineID(system,subSystem,endcap,layer,phi,eta);
  }

  int HashID(std::string ID) {
    int system, subSystem, endcap, layer, phi, eta;
    int success;
    if ( ID.at(0)=='[' ) // already offline ID
      success=UnpackOfflineID(ID,system,subSystem,endcap,layer,phi,eta);
    else                 // probably it is online ID
      success=OfflineID(ID,system,subSystem,endcap,layer,phi,eta);
    if (success)
      return HashID(system,subSystem,endcap,layer,phi,eta);
    else 
      return -1;
  }

  int HashID(int system,
	     int subSystem,
	     int endcap,
	     int layer,
	     int phi,
	     int eta) {
    // if (endcap==0) eta+=6;
    eta+=6;
    endcap   = (endcap+2)/2;
    system   = 0;
    subSystem= 0;
    return ((  system& 0x7)<<29)
      +    ((subSystem&0x3)<<27)
      +    ((  endcap& 0x3)<<25)
      +    ((   layer& 0x3)<<23)
      +    ((     phi&0x3f)<<17)
      +    ((     eta& 0xf)<<13);
  }

  std::string DCSID(std::string OnlineID) {
    std::string ID=OnlineID, newID(Error);
    size_t pos=0;
    char ch;
    while ( (pos=ID.find('-',pos))!=std::string::npos ) ID.replace(pos,1,"_");
    if ( ID.at(0)=='L' ) {
      pos = ID.find('S');                // stave in DCS is S1/S2, not S01/S02
      if ( pos!=std::string::npos ) { 
	ch = ID.at(pos+1);
        if ( ch=='0' ) ID.erase(pos+1,1);
      }
      return ID;
    }
    pos=5;                                    // do not rely a priori on  
    ch=ID.at(pos);                       // about input data being S0#
    if ( ch=='0' ) {pos++; ch=ID.at(pos);}    // or S#
    switch (ch) {
    case '1':
      newID="B01_S2";
      break;
    case '2':
      newID="B02_S1";
      break;
    case '3':
      newID="B02_S2";
      break;
    case '4':
      newID="B03_S1";
      break;
    case '5':
      newID="B03_S2";
      break;
    case '6':
      newID="B04_S1";
      break;
    case '7':
      newID="B04_S2";
      break;
    case '8':
      newID="B01_S1";
      break;
    default:
      return newID;
    }
    ID.replace(4,pos-3,newID);
    return ID;
  }

  std::string OnlineIDfromDCSID(std::string DCSID) {
    std::string ID=DCSID, newID(Error);
    size_t pos=0;
    while ( (pos=ID.find('_',pos))!=std::string::npos ) ID.replace(pos,1,"-");
    if ( ID.at(0)=='L' ) return ID;
    std::istringstream in(DCSID);
    int b,s,dum;
    char c, c1, c2;
    in >> c >> dum >> c >> c >> c1 >> b >> c >> c2 >> s;
    if ( c1!='B' || c2!='S' ) return newID; 
    int sector=b*2-3+s;
    int Nsector=NinDisk/NinSector;
    if (sector==0) sector=Nsector;
    else if (sector<0 || sector>=Nsector) return Error;
    int start=ID.find('-',0)+1;   // first '-' is before B
    int stop =ID.find('-',start); // iterate twice to get third '-'
    stop=ID.find('-',stop+1);       // after sector ID
    std::ostringstream out;
    out << "S0" << sector;
    ID.replace(start,stop-start,out.str());
    return ID;
  }

  typedef std::map<unsigned int,unsigned int> modulemap;

  modulemap* hashIndex=0;
  modulemap* SNIndex=0;
  const std::string datafile("NamingConversions.txt");

  /*
   * This function provides the module hashID if it is given as input
   * moduleID between 510000 and 519999, which are possible serial number
   * of production modules.
   * Otherwise, it interprets the input argument as an hash identifier
   * and gives back the corresponding module SN.
   * In case of failure the return value is 0.
   *
   * The table in which the correspondance is given is contained in the
   * file named as in the string datafile initialized above.
   */

  int ReadMap(const std::string filename ) {
    std::cerr << "INFO: Opening data file " << filename << std::endl; 
    std::ifstream f(filename.c_str());
    if ( !f.good() ) {
      std::cerr << "ERROR: Cannot open data file " << filename << std::endl; 
      return -1;
    }
    hashIndex= new modulemap;
    SNIndex= new modulemap;
    unsigned int hashID, SN;
    std::string dummy;
    getline(f,dummy); // read header line
    while (1) {
      f >> dummy >> hashID >> dummy >> dummy >> SN 
	>> dummy >> dummy >> dummy >> dummy;
      // std::cerr << "Read from " << filename << "\t" << hashID << "\t" << SN << std::endl; 
      if ( f.bad() ) {
	std::cerr << "ERROR: error reading " << filename 
		  << " after " << hashIndex->size() << " entries" 
		  << std::endl;
	f.close();
	return -2;
      } else if ( f.eof() ) break;
      hashIndex->insert(std::make_pair(hashID,SN));
      SNIndex->insert(std::make_pair(SN,hashID));
    }
    std::cerr << "INFO: end of file " << filename 
	      << " after " << hashIndex->size() << " entries" 
	      << std::endl;
    f.close();
    return 0;
  }

  unsigned int GetID(const unsigned int moduleID) {
    // if maps are not previously loaded, read in the file 
    if ( !hashIndex || !SNIndex ) ReadMap(datafile);
    if ( moduleID>=510000 && moduleID<=519999 ) 
      return (*SNIndex)[moduleID];
    else 
      return (*hashIndex)[moduleID];
  }

}









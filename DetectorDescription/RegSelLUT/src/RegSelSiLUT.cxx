/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File:         RegSelSiLUT.cxx        
 **
 **   Description:  RegionSelector Si LUT code  
 **                   
 **                   
 ** 
 **   Author:       M.Sutton    
 **
 **   Created:      Wed Apr  4 16:04:30 BST 2007
 **   Modified:     
 **                   
 **                   
 **
 **************************************************************************/ 

#include "RegSelLUT/RegSelSiLUT.h"
#include "RegSelLUT/RegSelModule.h"
#include "RegSelLUT/RegSelTimer.h"

#include "RegSelLUT/RegSelROB.h"

#include <iostream>
#include <fstream>
#include <set>




RegSelSiLUT::RegSelSiLUT(DET id) : m_ID(id) { construct(); }

RegSelSiLUT::RegSelSiLUT(const RegSelSiLUT& r) : 
  RegSelName(r.getName()), m_ID(r.ID()) { 
  construct(); 
  addModules(r.getModules()); 
  initialise(); 
}

RegSelSiLUT::RegSelSiLUT(const std::string& s) : 
  m_ID(UNDEF) { 
  read(s); 
}


// this is no longer needed, since the RegSelSiLUT now configures 
// the number of dubdetectors and layers for each subdetector
// automatically 

void RegSelSiLUT::construct() 
{   
  const std::string s[5] = { "", "pixel", "sct", "trt", "ftk" };
  m_name = s[m_ID];
}




// clear the look up table
void RegSelSiLUT::reset() {
  for ( unsigned i=0 ; i<m_SubDet.size() ; i++ )  m_SubDet[i].reset();
  m_map.clear();
}




// initialise the look up table from the main vector of RegSelModules

void RegSelSiLUT::initialise() {

  //  struct timeval inittimer = gtttimer_start();

  // find out how many subdetectors there are
  
  std::vector<int> subdetid;
  
  for ( std::vector<RegSelModule>::iterator mptr = m_modules.begin() ; 
	mptr!=m_modules.end() ; mptr++ ) { 
    int detid = mptr->detector();
    
    bool found = false;
    for ( int j=subdetid.size() ; j-- ; )  if ( subdetid[j]==detid  ) found = true;

    if ( !found ) subdetid.push_back(detid);
  }

  //  std::cout << "\tRegSelSiLUT::initialise() creating detector with " << subdetid.size() << " subdetectors" << std::endl;
  
  // std::cout << "RegSelSiLUT::initialise()" << std::endl;

  m_SubDet.reserve(subdetid.size());

  // push each one to the list  
  for ( int i=subdetid.size() ; i-- ; ) { 
    m_SubDet.push_back( RegSelSubDetector(subdetid[i]) );
    m_idmap.insert( std::map<int, int>::value_type(subdetid[i], i) );

    int ii = subdetid.size()-1-i; 
    std::cout << "\tsubdet " << ii << "\tid " << subdetid[ii] << std::endl;
  }
	
  
  
					   
  // initialise the subdetector look up tables
  for ( unsigned i=0 ; i<m_SubDet.size() ; i++ )  m_SubDet[i].addModules(m_modules);

  //  std::cout << "\tRegSelSiLUT " << getName() << std::endl;
  //  write(getName()+".map");

  //  std::cout << "RegSelSiLUT::initialise() creating hash map" << std::endl;

  // and save pointers to all the modules in a map by hashid
  for ( std::vector<RegSelModule>::iterator mptr = m_modules.begin() ; 
	mptr!=m_modules.end() ; mptr++ ) { 
    m_map[(*mptr).hashID()] = &(*mptr);
  }

  //  std::cout << "RegSelSiLUT::initialise() exiting" << std::endl;

  //  double inittime = gtttimer_stop(inittimer);
  //  std::cout << "initialise time=" << inittime << std::endl;

  initialiseDisabledModules();
  initialiseFullScan();

  /// uncomment this if you want to print out the list of all the module ids
  /// assigned to each rob
  /// RegSelROBList roblist( m_modules );

}


void RegSelSiLUT::initialiseFullScan() { 

  m_allHashes.clear();
  m_allROBids.clear();
  
  std::set<uint32_t> robids;

  std::vector<RegSelModule>::const_iterator mptr(m_modules.begin()); 
  std::vector<RegSelModule>::const_iterator eptr(m_modules.end());
  for ( ; mptr!=eptr ; mptr++ ) { 
    if ( (*mptr).enabled() ) { 
      m_allHashes.push_back((*mptr).hashID());
      robids.insert((*mptr).robID());
    }
  }

  std::set<uint32_t>::iterator ritr(robids.begin());
  while ( ritr!=robids.end() ) { 
    m_allROBids.push_back(*ritr++);
  }

}



// initialise the disabled module lists for 
// each subdetector layer

void RegSelSiLUT::initialiseDisabledModules() {  
  for ( std::vector<RegSelSubDetector>::iterator sptr(m_SubDet.begin()) ; 
	sptr!=m_SubDet.end() ; sptr++ ) { 
    (*sptr).setupDisabledModules();
  }
}



// find a module from it's hashID
const RegSelModule* RegSelSiLUT::Module(const IdentifierHash& h) const {  
  std::map<const IdentifierHash, const RegSelModule*>::const_iterator hptr(m_map.find(h));
  if ( hptr!=m_map.end() ) return hptr->second;
  else                     return NULL;
}




void RegSelSiLUT::disableSubDetector(int id) { 
    std::map<int,int>::iterator sitr = m_idmap.find(id);
    if ( sitr!=m_idmap.end() )  m_SubDet[sitr->second].disable();
}

void RegSelSiLUT::enableSubDetector(int id) { 
    std::map<int,int>::iterator sitr = m_idmap.find(id);
    if ( sitr!=m_idmap.end() ) m_SubDet[sitr->second].enable();
}

void RegSelSiLUT::disableLayer(int id, unsigned layer) { 
    std::map<int,int>::iterator sitr = m_idmap.find(id);
    if ( sitr!=m_idmap.end() ) m_SubDet[sitr->second].disableLayer(layer);
}

void RegSelSiLUT::enableLayer(int id, unsigned layer) { 
    std::map<int,int>::iterator sitr = m_idmap.find(id);
    if ( sitr!=m_idmap.end() ) m_SubDet[sitr->second].enableLayer(layer);
}



bool RegSelSiLUT::getRoIData(const RegSelRoI& roi, std::vector<const RegSelModule*>& modules) const
{
  //  std::cout << "RegSelSiLUT::getRoIData() ID=" << ID() << "\t" << roi << std::endl;

  bool inDet = false;

  for ( unsigned i=0 ; i<m_SubDet.size() ; i++ ) {
    bool inSubDet = m_SubDet[i].inRoI(roi);
    inDet |= inSubDet;
    //  std::cout << "\tRegSelSiLUT::getRoIData() SubDet id " << m_SubDet[i].ID() << std::endl;
    if ( inSubDet )   m_SubDet[i].getModules(roi, modules);    
  }

  //  std::cout << "RegSelSiLUT::getRoIData() Nmodules=" << modules.size() << std::endl;  
  //  for ( int i=0 ; i<modules.size() ; i++ ) modules[i]->paw(); 

  return inDet;
}




void RegSelSiLUT::getModules( unsigned layer, std::vector<const RegSelModule*>& modules) const { 
    modules.reserve(256);

    /// determine whether an layer in a specific subdetector is requested
    bool subdetflag = layer/1000;
    int  subdet = 0;
    int  ilayer = layer%100;
    if ( subdetflag ) subdet = (layer%1000)/100;
    
    for ( unsigned i=0 ; i<m_SubDet.size() ; i++ ) { 
      if ( !subdetflag || ( subdetflag && subdet==std::fabs(m_SubDet[i].ID()) ) ) m_SubDet[i].getModules( ilayer, modules);  
    }
  }



/// here if layer is of the format 1XYY where X=0 (barrel), X=2 (endcap) and YY is the layer within the sub-system
/// then the specific layer in the sub-system is used.
/// If it is of the form YY, then simple layer number is used
 
bool RegSelSiLUT::getRoIData(const RegSelRoI& roi, unsigned layer, std::vector<const RegSelModule*>& modules) const
{
  //  std::cout << "RegSelSiLUT::getRoIData() ID=" << ID() << std::endl;

  bool inDet = false;


  int ilayer = layer%100;
  /// determine whether an layer in a specific subdetector is requested
  bool subdetflag = layer/1000;
  int  subdet = 0;
  if ( subdetflag ) subdet = (layer%1000)/100;

  for ( unsigned i=0 ; i<m_SubDet.size() ; i++ ) {

    if ( !subdetflag || ( subdetflag && subdet==std::fabs(m_SubDet[i].ID()) ) ) {  
      bool inSubDet = m_SubDet[i].inRoI(roi);
      inDet |= inSubDet;
      if ( inSubDet ) m_SubDet[i].getModules(roi, ilayer, modules);
    }
  }
 



  //  std::cout << "RegSelSiLUT::getRoIData() Nmodules=" << modules.size() << std::endl;  
  //  for ( int i=0 ; i<modules.size() ; i++ ) modules[i]->paw(); 

  return inDet;
}





bool RegSelSiLUT::getRoIData(const RegSelRoI& roi, unsigned subdetector, unsigned layer, std::vector<const RegSelModule*>& modules) const
{
  //  std::cout << "RegSelSiLUT::getRoIData() ID=" << ID() << std::endl;

  bool inDet = false;
  
  if ( subdetector<m_SubDet.size() ) { 
    inDet = m_SubDet[subdetector].inRoI(roi);
    if ( inDet ) m_SubDet[subdetector].getModules(roi, layer, modules);
  }
  //  std::cout << "RegSelSiLUT::getRoIData() Nmodules=" << modules.size() << std::endl;  
  //  for ( int i=0 ; i<modules.size() ; i++ ) modules[i]->paw(); 

  return inDet;
}






double phiprime(double x, double y, double phi, double r, bool& status) {

  if ( x*x+y*y>r*r ) {
    //    std::cerr << "duffer x=" << x << "\ty=" << y << "\tr=" << r << std::endl; 
    status = false; 
    return 0;
  } 

  double a = (y*cos(phi)-x*sin(phi))/r;

  // this is bad, getting the return value to issue an error code.
  // ouch! should maybe define an exception and catch it?
  // this should never happen
  if ( fabs(a)>1 ) { status = false; return 0; } 

  status &= true;

  double phip = asin(a)+phi;

  //  std::cout << "phiprime()\tx=" << x << "\ty=" << y << "\tphi=" << phi << "\tr=" << r << "\tphiprime=" << phip << " before" << std::endl;

  // make sure wrapped between -pi to pi
  if ( phip>M_PI )  phip = phip-2*M_PI;
  if ( phip<-M_PI ) phip = phip+2*M_PI;

  //  std::cout << "phiprime()\tx=" << x << "\ty=" << y << "\tphi=" << phi << "\tr=" << r << "\tphiprime=" << phip << std::endl;

  return phip;

}


bool philimits( double x, double y,  double rmin, double rmax, double& phimin, double& phimax ) {

  double phi[4];

  bool status = true;

  //  std::cout << "phimin=" << phimin << "\tphimax=" << phimax << "\tlimits" << std::endl;

  phi[0] = phiprime(x, y, phimin, rmin, status);
  phi[1] = phiprime(x, y, phimin, rmax, status);

  phi[2] = phiprime(x, y, phimax, rmin, status);
  phi[3] = phiprime(x, y, phimax, rmax, status);

  //  for ( int i=0 ; i<4 ; i++ ) std::cout << "phi[" << i << "]=" << phi[i] << std::endl;

  if ( status ) { 

    // this is needed to take account of the phi=pi boundary, it's not rigorous, but it works
 
    if ( phi[0]*phi[1]<0 && (phi[0]>0.5*M_PI||phi[1]>0.5*M_PI) ) phimin = ( phi[0]<phi[1] ? phi[1] : phi[0] ); 
    else                                                         phimin = ( phi[0]<phi[1] ? phi[0] : phi[1] );  
  
    if ( phi[2]*phi[3]<0 && (phi[2]>0.5*M_PI||phi[3]>0.5*M_PI) ) phimax = ( phi[2]>phi[3] ? phi[3] : phi[2] ); 
    else                                                         phimax = ( phi[2]>phi[3] ? phi[2] : phi[3] );  

  }
  
  //  std::cout << "phimin=" << phimin << "\tphimax=" << phimax << "\tlimits (out)" << std::endl;

  return status;

}


bool RegSelSiLUT::getRoIData(const RegSelRoI& roi, std::vector<const RegSelModule*>& modules, double x, double y) const
{

  if ( x==0 && y==0 ) return getRoIData(roi, modules);
 
  bool inDet = false;

  for ( unsigned i=0 ; i<m_SubDet.size() ; i++ ) {

#if 1

    const std::vector<RegSelLayer>& layers = m_SubDet[i].getLayers(); 

    for ( unsigned j=0 ; j<layers.size() ; j++ ) { 

      double rmin = layers[j].rMin();
      double rmax = layers[j].rMax();

      double phimin = roi.getphiMin();
      double phimax = roi.getphiMax();

      //      std::cout << layers[j] << "\t" << rmin << " " << rmax << std::endl;

      //      std::cout << "lyr[" << j << "] " << "\tphimin=" << phimin << "\tphimax=" << phimax << "\tbefore" << std::endl;

      if ( philimits(x, y, rmin, rmax, phimin, phimax) ) { 

	//	std::cout << layers[j] << "\tphimin=" << phimin << "\tphimax=" << phimax << std::endl;
	
	RegSelRoI troi( roi.getzMin(), roi.getzMax(), phimin, phimax, roi.getetaMin(), roi.getetaMax() ); 
       
	
	bool inLayer = layers[j].inRoI(troi);

	inDet |= inLayer;
	
	if ( inLayer ) layers[j].getModules(troi, modules);
	
      } 

    }

#else


      double rmin = m_SubDet[i].rMin();
      double rmax = m_SubDet[i].rMax();

      double phimin = roi.getphiMin();
      double phimax = roi.getphiMax();

      //      std::cout << layers[j] << "\t" << rmin << " " << rmax << std::endl;

      philimits(x, y, rmin, rmax, phimin, phimax);

      //      std::cout << layers[j] << "\tphimin=" << phimin << "\tphimax=" << phimax << std::endl;

      RegSelRoI troi( roi.getzMin(), roi.getzMax(), phimin, phimax, roi.getetaMin(), roi.getetaMax() ); 

      bool inSubDet = m_SubDet[i].inRoI(troi);
      inDet |= inSubDet;
      if ( inSubDet )   m_SubDet[i].getModules(troi, modules);    

#endif

  }
  //  std::cout << "getRoIData() time=" << _time << " ms (shifted)" << std::endl;

  //  std::cout << "RegSelSiLUT::getRoIData() Nmodules=" << modules.size() << std::endl;  

  return inDet;
}




bool RegSelSiLUT::getRoIData(const RegSelRoI& roi, unsigned subdetector, unsigned layer, std::vector<const RegSelModule*>& modules, double x, double y) const
{

  if ( x==0 && y==0 ) return getRoIData(roi, subdetector, layer, modules);

  //  std::cout << "RegSelSiLUT::getRoIData() ID=" << ID() << std::endl;

  //  struct timeval _timer = gtttimer_start();  

  bool inDet = false;

  if ( subdetector<m_SubDet.size() ) { 

    const std::vector<RegSelLayer>& layers = m_SubDet[subdetector].getLayers(); 

    if ( layer<layers.size() ) { 

      double rmin = layers[layer].rMin();
      double rmax = layers[layer].rMax();

      double phimin = roi.getphiMin();
      double phimax = roi.getphiMax();

      //      std::cout << layers[j] << "\t" << rmin << " " << rmax << std::endl;
      //      std::cout << "lyr[" << subdetector << "] " << "\tphimin=" << phimin << "\tphimax=" << phimax << "\tbefore" << std::endl;

      if ( philimits(x, y, rmin, rmax, phimin, phimax) ) { 

	//      std::cout << layers[j] << "\tphimin=" << phimin << "\tphimax=" << phimax << std::endl;
	//	std::cout << "lyr[" << subdetector << "] " << "\tphimin=" << phimin << "\tphimax=" << phimax << std::endl;

	RegSelRoI _roi( roi.getzMin(), roi.getzMax(), phimin, phimax, roi.getetaMin(), roi.getetaMax() ); 
       
	bool inLayer = layers[layer].inRoI(_roi);

	inDet |= inLayer;
	
	if ( inLayer ) layers[layer].getModules(_roi, modules);
	
      } 
    }
  }

  //  double _time = gtttimer_stop(_timer);

  //  std::cout << "getRoIData() time=" << _time << " ms (shifted)" << std::endl;

  //  std::cout << "RegSelSiLUT::getRoIData() Nmodules=" << modules.size() << std::endl;  
  //  for ( int i=0 ; i<modules.size() ; i++ ) modules[i]->paw(); 

  return inDet;
}






void RegSelSiLUT::write(std::ostream& s) const { 
  s << m_modules.size() << std::endl; 
  for ( unsigned i=0 ; i<m_modules.size() ; i++ ) { 
    s << m_modules[i] << std::endl;
  } 
}

void RegSelSiLUT::write(const std::string& s) const { 
  std::ofstream outfile(s.c_str());
  write(outfile);
}




// get the rob list and hash id's for an roi
void  RegSelSiLUT::getRobHashList(const RegSelRoI& roi, std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const 
{
  std::vector<const RegSelModule*> modules;
  if ( modules.size()<256 ) modules.reserve(256);

  // get the modules
  getRoIData(roi,modules);

  // fill the rob and hashid lists

  std::set<uint32_t> uroblist;

  hashlist.reserve(modules.size());
  roblist.reserve(modules.size());
  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 
  while (  mptr!=modules.end() ) { 
    hashlist.push_back((*mptr)->hashID());
    roblist.push_back((*mptr++)->robID());
  } 
  //  removeDuplicates( hashlist );
  removeDuplicates( roblist );
  
  //  roblist.reserve(uroblist.size());
  //  std::set<uint32_t>::iterator plist(uroblist.begin());
  //  while ( plist!=uroblist.end() ) { 
  //    roblist.push_back(*plist++);
  //  } 

}



// get the rob list and hash id's for an roi
void  RegSelSiLUT::getRobHashList(const RegSelRoI& roi, unsigned layer, std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const 
{
  std::vector<const RegSelModule*> modules;
  if ( modules.size()<256 ) modules.reserve(256);

  // get the modules
  getRoIData(roi, layer, modules);

  // fill the rob and hashid lists

  std::set<uint32_t> uroblist;

  hashlist.reserve(modules.size());
  roblist.reserve(modules.size());
  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 
  while (  mptr!=modules.end() ) { 
    hashlist.push_back((*mptr)->hashID());
    roblist.push_back((*mptr++)->robID());
  } 
  // removeDuplicates( hashlist );
  removeDuplicates( roblist ); 

  //   roblist.reserve(uroblist.size());
  //   std::set<uint32_t>::iterator plist(uroblist.begin());
  //   while ( plist!=uroblist.end() ) { 
  //     roblist.push_back(*plist++);
  //   } 

}


void RegSelSiLUT::getRobList(const RegSelRoI& roi, std::vector<uint32_t>& roblist, bool removeduplicates ) const { 

  std::vector<const RegSelModule*> modules;
  if ( modules.size()<256 ) modules.reserve(256);

  //  struct timeval datatimer = gtttimer_start();
  getRoIData(roi,modules);
  //  double datatime = gtttimer_stop(datatimer);

#if 0
  // get the list of unique rob identifiers
  // FIXME: this uses a set, sould be *much* faster
  // with a seperate list of robids with their modules'
  // corresponding extents in r, phi and z etc
  std::set<uint32_t> uroblist;

  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 
  while ( mptr!=modules.end() ) { 
    uroblist.insert((*mptr++)->robID());
  }

  roblist.reserve(uroblist.size());
  std::set<uint32_t>::iterator plist(uroblist.begin());
  while ( plist!=uroblist.end() ) { 
    roblist.push_back(*plist++);
  } 

#endif

  /// NB: using a vector and removing duplicates is faster than 
  ///     using a set
  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 

  roblist.reserve(modules.size());
  while ( mptr!=modules.end() ) roblist.push_back((*mptr++)->robID());
  if ( removeduplicates || roblist.size()>2000 ) removeDuplicates( roblist );

  //  double vectortime = gtttimer_stop(datatimer);

  //  std::cout << "RegSelSiLUT::getRobList() datatime=" << datatime 
  //       << "\tvectortime=" << vectortime << "\tf=" 
  //       << 100*(vectortime-datatime)/vectortime << std::endl;

}







void RegSelSiLUT::getRobList(const RegSelRoI& roi, std::vector<uint32_t>& roblist, double x, double y ) const { 

  std::vector<const RegSelModule*> modules;
  if ( modules.size()<256 ) modules.reserve(256);

  //  struct timeval datatimer = gtttimer_start();
  getRoIData( roi, modules, x, y );
  //  double datatime = gtttimer_stop(datatimer);

#if 0
  std::set<uint32_t> uroblist;

  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 
  while ( mptr!=modules.end() ) { 
    uroblist.insert((*mptr++)->robID());
  }

  roblist.reserve(uroblist.size());
  std::set<uint32_t>::iterator plist(uroblist.begin());
  while ( plist!=uroblist.end() ) { 
    roblist.push_back(*plist++);
  } 

#endif

  roblist.reserve(modules.size());
  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 
  while ( mptr!=modules.end() ) roblist.push_back((*mptr++)->robID());
  removeDuplicates( roblist );

}






// enable all the modules
void RegSelSiLUT::enableRobs() {
  std::vector<RegSelModule>::iterator itr(m_modules.begin()); 
  while ( itr!=m_modules.end() ) (*itr++).enable();
  initialiseDisabledModules();
  initialiseFullScan();
} 



// disable all the modules
void RegSelSiLUT::disableRobs() {
  std::vector<RegSelModule>::iterator itr(m_modules.begin()); 
  while ( itr!=m_modules.end() ) (*itr++).disable();
  initialiseDisabledModules();
  initialiseFullScan();
} 




// disable the modules from requested robs in the look up tables
void RegSelSiLUT::disableRobList(const std::vector<uint32_t>& roblist) { 

  std::set<uint32_t> robset;
  for ( unsigned i=0 ; i<roblist.size() ; i++ ) robset.insert(roblist[i]);
 
  std::vector<RegSelModule>::iterator itr(m_modules.begin()); 
  for ( ; itr!=m_modules.end() ; itr++ ) { 
    // this module's rob is on the list, so disable it 
    if ( robset.find((*itr).robID())!=robset.end() )  (*itr).disable();                                              
  }
  initialiseDisabledModules();
  initialiseFullScan();
}



// enable modules from the robs in the look up tables
void RegSelSiLUT::enableRobList(const std::vector<uint32_t>& roblist) { 
  
  std::set<uint32_t> robset;
  for ( unsigned i=0 ; i<roblist.size() ; i++ ) robset.insert(roblist[i]);
 
  std::vector<RegSelModule>::iterator itr(m_modules.begin()); 
  for ( ; itr!=m_modules.end() ; itr++ ) { 
    // this module's rob is on the list, so enable it 
    if ( robset.find((*itr).robID())!=robset.end() )  (*itr).enable();
  }
  initialiseDisabledModules();
  initialiseFullScan();
}






// disable modules in the look up tables
void RegSelSiLUT::disableModuleList(const std::vector<IdentifierHash>& hashlist) { 

  std::set<IdentifierHash> hashset;
  for ( unsigned i=0 ; i<hashlist.size() ; i++ ) hashset.insert(hashlist[i]);
  
  std::vector<RegSelModule>::iterator itr(m_modules.begin()); 
  for ( ; itr!=m_modules.end() ; itr++ ) { 
    // this module is on the list, so disable it 
    if ( hashset.find((*itr).hashID())!=hashset.end() )  (*itr).disable();                                        
  }
  initialiseDisabledModules();
  initialiseFullScan();
}



// enable modules in the look up tables
void RegSelSiLUT::enableModuleList(const std::vector<IdentifierHash>& hashlist) { 
  
  std::set<IdentifierHash> hashset;
  for ( unsigned i=0 ; i<hashlist.size() ; i++ ) hashset.insert(hashlist[i]);
  
  std::vector<RegSelModule>::iterator itr(m_modules.begin()); 
  for ( ; itr!=m_modules.end() ; itr++ ) { 
    // this module is on the list, so enable it 
    if ( hashset.find((*itr).hashID())!=hashset.end() )  (*itr).enable();                                        
  }
  initialiseDisabledModules();
  initialiseFullScan();
}




  


// get hash id's for all enabled modules in the roi 

void RegSelSiLUT::getHashList(const RegSelRoI& roi, std::vector<IdentifierHash>& hashlist) const { 
  
  //  bool hashempty = hashlist.empty();

  std::vector<const RegSelModule*> modules;
  if ( modules.size()<256 ) modules.reserve(256);

  //  struct timeval datatimer = gtttimer_start();
  getRoIData(roi,modules);
  //  double datatime = gtttimer_stop(datatimer);

  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 

  hashlist.reserve(modules.size());
  for ( ; mptr!=modules.end() ; mptr++ ) { 
    hashlist.push_back((*mptr)->hashID());
  } 

  //  if ( !hashempty ) removeDuplicates( hashlist );

  //  double vectortime = gtttimer_stop(datatimer);

  //  std::cout << "RegSelSiLUT::getHashList() datatime=" << datatime << "\tvectortime=" << vectortime << "\tf=" 
  //       << 100*(vectortime-datatime)/vectortime << std::endl;

}




void RegSelSiLUT::getHashList(const RegSelRoI& roi, std::vector<IdentifierHash>& hashlist, double x, double y ) const { 

  std::vector<const RegSelModule*> modules;
  if ( modules.size()<256 ) modules.reserve(256);

  //  struct timeval datatimer = gtttimer_start();
  getRoIData( roi, modules, x, y );
  //  double datatime = gtttimer_stop(datatimer);

  //  bool hashempty = hashlist.empty();

  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 

  hashlist.reserve(modules.size());
  for ( ; mptr!=modules.end() ; mptr++ ) { 
    hashlist.push_back((*mptr)->hashID());
  } 

  //  if ( !hashempty ) removeDuplicates( hashlist );

}




// get hash id's for all enabled modules 

void RegSelSiLUT::getHashList(std::vector<IdentifierHash>& hashlist) const { 

#ifndef OLDFULLSCAN
  hashlist.reserve(m_allHashes.size());
  hashlist = m_allHashes;
  return;
#else

  hashlist.clear();
  hashlist.reserve(m_modules.size());

  std::vector<RegSelModule>::const_iterator mptr(m_modules.begin()); 
  std::vector<RegSelModule>::const_iterator eptr(m_modules.end());
  for ( ; mptr!=eptr ; mptr++ ) { 
    if ( (*mptr).enabled() ) hashlist.push_back((*mptr).hashID());
  }
#endif
}




// get hash id's for all enabled modules in a layer

void RegSelSiLUT::getHashList( unsigned layer, std::vector<IdentifierHash>& hashlist) const { 

  std::vector<const RegSelModule*> modules;
 
  //  bool hashempty = hashlist.empty();

  getModules( layer, modules );

  hashlist.reserve(modules.size());
  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 
  while (  mptr!=modules.end() ) { 
    hashlist.push_back((*mptr++)->hashID());
  } 

  //  if ( !hashempty ) removeDuplicates( hashlist );

}





// get robs corresponding to all enabled modules 

void RegSelSiLUT::getRobList(std::vector<uint32_t>& roblist) const { 

#ifndef OLDFULLSCAN
  roblist.reserve(m_allROBids.size());
  roblist = m_allROBids;
  return;
#else
  roblist.reserve(256);

  std::set<uint32_t> uroblist;
  std::vector<RegSelModule>::const_iterator mptr(m_modules.begin()); 
  for ( ; mptr!=m_modules.end() ; mptr++ ) { 
    if ( (*mptr).enabled() ) uroblist.insert((*mptr).robID());
  }

  std::set<uint32_t>::iterator rlist(uroblist.begin());
  while ( rlist!=uroblist.end() ) { 
    roblist.push_back(*rlist++);
  } 
#endif
}

// get robs and hash id's for all enabled modules 

void RegSelSiLUT::getRobHashList(std::vector<uint32_t>& roblist, std::vector<IdentifierHash>& hashlist) const { 

#ifndef OLDFULLSCAN
  hashlist.reserve(m_allHashes.size());
  roblist.reserve(m_allROBids.size());
  roblist  = m_allROBids;
  hashlist = m_allHashes;
  return;
#else
  roblist.reserve(256);
  hashlist.reserve(m_modules.size());

  std::set<uint32_t> uroblist;
  std::vector<RegSelModule>::const_iterator mptr(m_modules.begin()); 
  for ( ; mptr!=m_modules.end() ; mptr++ ) { 
    if ( (*mptr).enabled() ) { 
      uroblist.insert((*mptr).robID());
      hashlist.push_back((*mptr).hashID());
    }
  }
  
  std::set<uint32_t>::iterator rlist(uroblist.begin());
  while ( rlist!=uroblist.end() ) { 
    roblist.push_back(*rlist++);
  } 
#endif
}


// get modules in the roi for a specified layer

void RegSelSiLUT::getHashList(const RegSelRoI& roi, unsigned layer, std::vector<IdentifierHash>& hashlist) const { 

  //  bool hashempty = hashlist.empty();

  std::vector<const RegSelModule*> modules;
  if ( modules.size()<256 ) modules.reserve(256);


  getRoIData( roi, layer, modules);
  
  //  struct timeval datatimer = gtttimer_start();
  
  //  double datatime = gtttimer_stop(datatimer);

  hashlist.reserve(modules.size());
  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 
  while (  mptr!=modules.end() ) { 
    hashlist.push_back((*mptr++)->hashID());
  } 

  //  if ( !hashempty ) removeDuplicates( hashlist );

  //  double vectortime = gtttimer_stop(datatimer);

  //  std::cout << "RegSelSiLUT::getHashList() datatime=" << datatime << "\tvectortime=" << vectortime << "\tf=" 
  //       << 100*(vectortime-datatime)/vectortime << std::endl;

}




// get robs in the roi for a specified layer

void RegSelSiLUT::getRobList(const RegSelRoI& roi, unsigned layer, std::vector<uint32_t>& roblist, bool removeduplicates ) const { 

  std::vector<const RegSelModule*> modules;
  if ( modules.size()<256 ) modules.reserve(256);

    
  getRoIData( roi, layer, modules);

#if 0
  for ( std::vector<RegSelSubDetector>::const_iterator sptr(m_SubDet.begin()) ; 
	sptr!=m_SubDet.end() ; sptr++ ) { 
    
    if ( (*sptr).inRoI(roi) ) { 
      
      const std::vector<RegSelLayer>& lyrs = (*sptr).getLayers();
      
      if ( layer<lyrs.size() ) { 
	const RegSelLayer& lyr = lyrs[layer];	
	if ( lyr.inRoI(roi) )  lyr.getModules(roi,modules); 
      }
    }
  }
#endif  

  //  struct timeval datatimer = gtttimer_start();
  
  //  double datatime = gtttimer_stop(datatimer);

  //  std::vector<uint32_t> uroblist;
  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 
  while ( mptr!=modules.end() ) { 
    roblist.push_back((*mptr++)->robID());
  }

  //  std::set<uint32_t>::iterator rlist(uroblist.begin());
  //  while ( rlist!=uroblist.end() ) { 
  //    roblist.push_back(*rlist++);
  //  } 
  
  if ( removeduplicates || roblist.size()>4000 ) removeDuplicates( roblist );

}









// get robs in the roi for a specified layer

void RegSelSiLUT::getRobList( unsigned layer, std::vector<uint32_t>& roblist) const { 

  std::vector<const RegSelModule*> modules;

  getModules( layer, modules );

  roblist.reserve(256);
  
  std::set<uint32_t> uroblist;
  std::vector<const RegSelModule*>::const_iterator mptr(modules.begin()); 
  for ( ; mptr!=modules.end() ; mptr++ ) { 
    if ( (*mptr)->enabled() ) roblist.push_back((*mptr)->robID());
  }
  removeDuplicates( roblist );

  //  std::set<uint32_t>::iterator rlist(uroblist.begin());
  //  while ( rlist!=uroblist.end() ) roblist.push_back(*rlist++);

}









// get disabled modules in the roi for a specified layer

void RegSelSiLUT::getDisabledHashList(const RegSelRoI& roi, unsigned layer, std::vector<IdentifierHash>& hashlist) const { 

  std::vector<const RegSelModule*> modules;
  if ( modules.size()<256 ) modules.reserve(256);

  //  for ( std::vector<RegSelSubDetector>::const_iterator sptr(getSubDet().begin()) ; 
  for ( std::vector<RegSelSubDetector>::const_iterator sptr(m_SubDet.begin()) ; 
	sptr!=m_SubDet.end() ; sptr++ ) { 
    
    if ( (*sptr).inRoI(roi) ) { 
      
      const std::vector<RegSelLayer>& lyrs = (*sptr).getLayers();
      
      if ( layer<lyrs.size() ) { 
	//	const RegSelLayer& lyr = lyrs.at(layer);
	const RegSelLayer& lyr = lyrs[layer];
	
	if ( lyr.inRoI(roi) )  lyr.getDisabledModules(roi,modules); 
      }
    }
  }
 
  hashlist.reserve(m_modules.size()); 
  std::vector<const RegSelModule*>::iterator mptr(modules.begin()); 
  while (  mptr!=modules.end() ) { 
    hashlist.push_back((*mptr++)->hashID());
  } 
}



void RegSelSiLUT::getDisabledLayers(const RegSelRoI& roi, int subdetector, std::vector<int>& layers) const { 

  //  layers.clear();

  std::set<int> disabled_layers;

  for ( std::vector<RegSelSubDetector>::const_iterator sptr(m_SubDet.begin()) ; 
	sptr!=m_SubDet.end() ; sptr++ ) { 
    
    //    std::cout << "subdet id = " << sptr->ID() << std::endl;

    if ( subdetector!=sptr->ID() ) continue;

    if ( (*sptr).inRoI(roi) ) { 
      
      const std::vector<RegSelLayer>& lyrs = (*sptr).getLayers();
      
      for ( int layer=lyrs.size() ; layer-- ; ) { 

	std::vector<const RegSelModule*> modules;
	if ( modules.size()<256 ) modules.reserve(256);

	//	const RegSelLayer& lyr = lyrs.at(layer);
	const RegSelLayer& lyr = lyrs[layer];
	
	if ( lyr.inRoI(roi) )  { 
	  lyr.getDisabledModules(roi,modules);
	  //	  if ( modules.size() ) disabled_layers.insert(layer);
  	  if ( modules.size() ) layers.push_back(layer);
	}
      }
    }
  }
  
#if 0
  // copy to output
  std::set<int>::iterator bitr(disabled_layers.begin());
  std::set<int>::iterator eitr(disabled_layers.end());
  while ( bitr!=eitr ) { layers.push_back(*bitr++); }
#endif

}









bool RegSelSiLUT::read(const std::string& s) {
  std::ifstream infile(s.c_str());
  // std::cout << "RegSelSiLUT::read() from file " << s << std::endl;
  return read(infile);
}


bool RegSelSiLUT::read(std::istream& s) {

  unsigned Nmodules = 0;

  // old method - read number of modules and try tp read them in 
  // new method - just read until module list exhausted, 
  // this code will allow either

  bool newformat = false;

  s >> Nmodules;

  newformat=s.fail();

  // couldn't read number of modules - try new format
  if ( newformat ) s.clear();

  unsigned modcount=0;

  for ( ; newformat || modcount<Nmodules ; modcount++  ) {
    RegSelModule m;
    if ( !getModule(s, m) ) break;
    addModule(m);
  } 

  //  std::cout << "read() newformat " << newformat << std::endl;
  //  std::cout << "read() read " << modcount << " modules" << std::endl;
  //  std::cout << "read() Nmodules " << Nmodules << "\tsize " << m_modules.size() << std::endl; 

  //  std::cout << "RegSelSiLUT::read() creating tables" << std::endl;
  initialise();

  if ( (!newformat) && (modcount<Nmodules ) ) return false;

  return true;
}




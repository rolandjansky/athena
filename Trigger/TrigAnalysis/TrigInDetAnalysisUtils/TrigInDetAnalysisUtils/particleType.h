/* emacs: this is -*- c++ -*- */
/**
 **     @file    particleType.h
 **
 **     @brief   a class of my own because ttying to access the  
 **              ParticleDataTable in athena is frankly more 
 **              trouble than it is worth        
 **
 **              As it happens, this class has no data members,
 **              but derives from an std::map, although a *private* 
 **              std::map so neatly sidestepping all the sound reasons 
 **              why one should not inherit from std classes 
 **
 **     @author  mark sutton
 **     @date    Thu 14 Jul 2011 09:50:36 BST 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#ifndef PARTICLETYPE_H
#define PARTICLETYPE_H

#include <iostream>
#include <string>
#include <map>


class particleType :  private std::map<int, std::string> {

public:

  using std::map<int, std::string>::begin;
  using std::map<int, std::string>::end;

public:

  particleType() {

    std::map<int, std::string>& ptype = *this;

    ptype[22]    = "gamma";

    ptype[111]   = "pi0";
    ptype[211]   = "pi+";
    ptype[-211]  = "pi-";
    ptype[310]   = "K0";
    ptype[-310]  = "K0 bar";
    ptype[321]   = "K+";
    ptype[-321]  = "K-";
    ptype[130]   = "K0L"; 
    //    ptype[-130]  = "K0L bar ???";
    ptype[310]   = "K0S"; 
    //    ptype[-310]  = "K0S bar ???";
    //    ptype[311]   = "K0"; 
    //    ptype[-311]  = "K0 bar";
    ptype[113]   = "rho0"; 
    ptype[213]   = "rho+"; 
    ptype[-213]  = "rho-"; 

    ptype[-20213] = "a1(1260)-"; 
    ptype[20213]  = "a1(1260)+"; 


    ptype[11]    = "e-";
    ptype[-11]   = "e+";
    ptype[12]    = "nue";
    ptype[-12]   = "nue bar";
    ptype[13]    = "mu-";
    ptype[-13]   = "mu+";
    ptype[14]    = "numu";
    ptype[-14]   = "numu bar";
    ptype[15]    = "tau-";
    ptype[-15]   = "tau+";
    ptype[16]    = "nutau";
    ptype[-16]   = "nutau bar";
    ptype[17]    = "tau'";
    ptype[-17]   = "tau'-";
    ptype[18]    = "nutau' bar";
    
    
    ptype[2212]   = "p+"; 
    ptype[-2212]  = "p-"; 
    ptype[2112]   = "n"; 
    ptype[-2112]  = "nbar"; 

    ptype[221]   = "eta"; 
    ptype[-221]  = "eta bar"; 
    

    ptype[3122]   = "lambda";  /// strange baryons - will decay weakly 
    ptype[-3122]  = "lambda bar";

    ptype[3112]   = "Sigma-"; ///Sigma
    ptype[-3112]  = "Sigmabar+";  
    ptype[3222]   = "Sigma+"; /// Xi, Omega etc
    ptype[-3222]  = "Sigmabar-";
    ptype[3312]   = "Xi-"; 
    ptype[-3312]  = "Xbar+";
    ptype[3322]   = "Xi0"; 
    ptype[-3322]  = "Xi0bar";
    ptype[3334]   = "Omega-"; 
    ptype[-3334]  = "Omegabar+";

    //    ptype[25]    = "h0";
       
    ptype[-1] = "offline";
 } 

  ~particleType() { } 

  std::string type(int id) { 
    if ( id>1000000000 ) return "nucleus";
    const_iterator ptype = find( id );
    if ( ptype==end() ) return "unknown";
    else                return ptype->second;
  }



  double charge(int id) { 
    if ( id>1000000000 ) return 0; // "nucleus" - don't care about charge in this context
    const_iterator ptype = find( id );
    if ( ptype==end() ) return 0; // "unknown" - don't care about charge in this context
    else { 
      if ( ptype->second.find("-")!=std::string::npos ) return -1;
      if ( ptype->second.find("+")!=std::string::npos ) return +1;
    }
    return 0;
  }

};


inline std::ostream& operator<<( std::ostream& s, const particleType& p ) {
  std::map<int, std::string>::const_iterator pitr = p.begin();
  std::map<int, std::string>::const_iterator pend = p.end();

  while ( pitr!=pend ) { 
    s << "p[" << pitr->first << "]=" << pitr->second << std::endl;
    pitr++;
  }
 
  return s;
}


#endif  // TIDAUTILS_PARTICLETYPE_H 











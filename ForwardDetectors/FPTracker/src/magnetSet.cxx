/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/magnetSet.h"
#include "FPTracker/magnetFactory.h"
#include "FPTracker/ConfigData.h"
#include "FPTracker/STLHelpers.h"
#include "FPTracker/beamlineXPosition.h"
#include "FPTracker/TransversePoint.h"
#include "openFile.tpl"
#include <istream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <cassert>
//#include  <iostream>
namespace FPTracker
{
  
  double findIPCentre(int magver, Side  side, int ip)
  {
    if ( magver == 1 )
      {
	if ( (side == beam2) && (ip == 1) ) { return 26658.88320;}
	if ( (side == beam2) && (ip == 5) ) { return 13329.593967;}
	if ( (side == beam1) && (ip == 1) ) { return 0.;}
	if ( (side == beam1) && (ip == 5) ) { return 13329.289233;}
	assert(false);
      }
    
    
    
    if ( magver == 2 )
      {
	if ( (side == beam2) && (ip == 1) ) { return 13329.28923;}
	if ( (side == beam2) && (ip == 5) ) { return 13329.59397;}
	if ( (side == beam1) && (ip == 1) ) { return 13329.59397;}
	if ( (side == beam1) && (ip == 5) ) { return 13329.28923;}
	assert(false);
      }    

    if ( magver == 3 ) //ALFA, from D Pelikan
      {
	if ( (side == beam2) && (ip == 1) ) { return 0.;}
	if ( (side == beam2) && (ip == 5) ) { return 13329.593967;}
	if ( (side == beam1) && (ip == 1) ) { return 0.;}
	if ( (side == beam1) && (ip == 5) ) { return 13329.289233;}
	assert(false);
      }    
    // check that the argument values are in have expected values
    assert(false);
    
    return 0;
  }
  
  // Magnet::Container_t magnetSet(const ConfigData& cData, int sideSelect, const std::vector< std::istream& >& magfiles){
  Magnet::Container_t magnetSet(const ConfigData& cData, const Side& side, int magver, boost::shared_ptr< std::ifstream> magfile)
  {
    //  Coordinate system: take z = forwards, x= to left , y = up.
    //  For quadrupoles, input the quantity K1L.  type = 1,2 if K1L is +,-ve.
    //  Input type = 3 here to let the program decide.
    //  HKICK and VKICK are read and treated as bending magnets. 
    
    
    
    //-------- Arrange to read the magnet data from CERN files.
    //         twiss_b1.txt and twiss_b2.txt for beam 1 and beam 2
    //         beam 2 is on side=0 here and beam 1 is on side=1
    //         Read through the files to pick up the requested magnets.
    
    
    const int&    IP         = cData.IP;
    const double& apermb     = cData.apermb;
    const double& brho       = cData.brho;
    const float& absZMagMax  = cData.absZMagMax;
    
    Magnet::Container_t magnets;
    
    double  centreIP = findIPCentre(magver, side, IP);
    
    std::ifstream& myMags = *magfile;
    
    
    //std::cout<<"Open magnets file for beam "<<ifile<<std::endl; 
    //--- Skip introductory material, indicated by the 1st character of a record.
    while (myMags.peek() =='@' || myMags.peek() =='*' || myMags.peek() =='$')
      {
	myMags.ignore(1000,'\n');
      }
    
    // side 1 == direction to +ve s of IP  == beam 1.
    // NB "dist" is -Delta(s) but magcen, i.e. z,  is then -dist.
    
    while(true)
      { 
	
	double endpos=0, maglength=0, K0L=0, K1L=0, K2L=0, K3L=0, HKICK=0, VKICK=0;
	float  BETX=0,ALFX=0,MUX=0,DX=0,DPX=0,X=0,PX=0,BETY=0,ALFY=0,MUY=0,DY=0,DPY=0,Y=0,PY=0,A1=0,A2=0,A3=0,A4=0;
	
	std::string magname,magsort, basicmag ;
	
	std::string aperture;
	
	if( magver==1 )
	  {
	  myMags>>magname>>magsort>>endpos>>maglength>>K0L>>K1L>>K2L>>K3L>>HKICK>>VKICK
		>>BETX>>ALFX>>MUX>>DX>>DPX>>X>>PX>>BETY>>ALFY>>MUY>>DY>>DPY>>Y>>PY
		>>aperture>>A1>>A2>>A3>>A4;
	  }
	else if( magver==2 or magver==3 )
	  {
	    myMags>>magname>>magsort>>basicmag>>endpos>>maglength>>HKICK>>VKICK>>K0L
		  >>K1L>>K2L>>K3L>>X>>Y>>PX>>PY>>BETX>>BETY>>ALFX>>ALFY
		  >>MUX>>MUY>>DX>>DY>>DPX>>DPY>>aperture>>A1>>A2>>A3>>A4;
	  }
	else
	  {
	    assert(false);
	  }
	
	if( (magname == "\"IP1\"" && IP==1) || (magname == "\"IP5\"" && IP==5))
	  
	  {// this is the IP
	    
	    if(magver==1)
	      {
		PY = PY * 142.5/142. ; PX = PX * 142.5/142.;
	      }
	    
	    //** a fudge because the file is only to 3 sig fig. 
	    Magnet::set_y0IP(Y); 
	    Magnet::set_ay0IP(PY);   
	    
	    Magnet::set_x0IP(X);
	    Magnet::set_ax0IP(PX);
	    
	    // if(IP==1) {thisIP= "IP1"; else thisIP = "IP5";} PS: not used?
	  }
	
	if(magver==1)
	  {       // Some more fudges to give more precision
	    if(std::fabs(K0L)==0.000188) K0L=K0L*188.175/188.0  ;   
	    if(std::fabs(K0L)==0.001129) K0L=K0L*1129.049/1129. ;   
	    if(std::fabs(K1L)==0.055577) K0L=K0L*55576.561/55577. ; 
	    if(std::fabs(K1L)==0.047986) K0L=K0L*47986.041/47986. ;
	    if(std::fabs(VKICK)==0.00020) VKICK=VKICK*20.171/20. ; 
	    if(std::fabs(VKICK)==0.00019) VKICK=VKICK*19.17/19. ; 
	  }
	
	double dist = centreIP - endpos + 0.5*maglength ;
	
	float adist=std::fabs(dist) ;

	// decide how to progress down the magnet files
	// the file formats are not constant. first
	// consider the magver=1 or 2 (these are two formats used by AFP.
	if ( magver == 1 or magver == 2 )
	  {
	    if (side == beam1)
	      {
		if( dist > 0.          ) {continue;}
		if( adist > absZMagMax ) {break;}
	      }
	    else
	      {
		if( adist > absZMagMax ) {continue;}
		if ( dist <=  0.       ) {break;}
	      }
	  }
	else if( magver == 3 ) //ALFA. from D. Pelikan
	  {
		if( dist > 0.          ) {continue;}
		if( adist > absZMagMax ) {break;}
	  }

	Magnet::Type mmagtype    = Magnet::notAMagnet;  
	double magstrength = 0;
	if (K0L != 0.)  
	  { 
	    mmagtype     =  Magnet::hbDipole; //type = 0
	    magstrength =  K0L;
	    if(side == beam2)
	      {
		magstrength = -magstrength;
	      }
	    
	  } 
	else if(K1L != 0.)
	  {
	    magstrength              =  K1L;
	    mmagtype = magstrength >= 0 ? Magnet::hfQuadrupole:Magnet::vfQuadrupole;  // type = 3 ->1,2 
	    assert(maglength > 0.);
	    magstrength  = std::fabs(brho*magstrength/maglength);
	  } 
	else if(HKICK != 0.)
	  { 
	    mmagtype                  = Magnet::hbDipole;  // type = 0
	    magstrength              =  -HKICK;
	  } 
	else if(VKICK != 0.)
	  { 
	    mmagtype                  = Magnet::vbDipole;  // type = 4
	    magstrength              =  VKICK;
	  } 
	
	if(mmagtype == Magnet::notAMagnet)  {continue;}  // --(not a magnet) 
	
	
	double mmagcen          = -dist;


	// more differences between AFP and ALFA files (from D. Pelikan)
	if (magver == 3)
	  {
	    if (side == beam2 ){ mmagcen = dist; }

	  }
	

	int mmagapertype = 0;
	// CLassify apertures.
	if(aperture == "\"NONE\"") 
	  {
	    mmagapertype              = 0;
	  } 
	else if(aperture == "\"CIRCLE\"" && A1 == 0.) 
	  {
	    mmagapertype              =0;
	  } 
	else if(aperture == "\"CIRCLE\"" && A1 >  0.)
	  {
	    mmagapertype              =1;
	  } 
	else if(aperture == "\"RECTELLIPSE\"") 
	  {
	    mmagapertype              =2;
	  } 
	else if(aperture == "\"OCTAGON\"") 
	  {
	    mmagapertype              =3;
	  } 
	else 
	  {
	    std::stringstream sstr;
	    sstr<<" Unknown magnet aperture type "<<aperture<<'\n' ;
	    throw std::runtime_error(sstr.str().c_str());
	  }
	
	/*
	  std::cout<<magname<<" "<<endpos<<" "<<maglength<<" "<<K0L<<" "<<K1L<<" "
	  <<" --> "<<mmagcen<<" "<<maglength
	  <<" "<<magstrength
	  <<" A1 "<<A1
	  <<" A2 "<<A2
	  <<" A3 "<<A3
	  <<" A4 "<<A4
	  <<std::endl; 
	*/
	
	
	if(apermb > 0)
	  {      // -------alter the MB apertures
	    int pos = magname.find("MB.");
	    if(pos==1)
	      { 
		A3=apermb; A4=apermb; 
	      }
	  }
	
	// note the sign convention for xdis. This shift of the magnet
	// x location accounts for the splitting of the beams somewhere
	// around 155. m. The x coordinate is wrt to beamline position,
	// and not the magnet center.
	double mmagnet_x  = beamlineXPosition(mmagcen)[0];
	double mmagnet_y  = beamlineXPosition(mmagcen)[1];
	
	Magnet::ConstPtr_t mptr = magnetFactory(mmagnet_x,
						mmagnet_y,
						mmagcen,
						magstrength,
						maglength,
						mmagapertype,
						A1,
						A2,
						A3,
						A4,
						X,
						cData.pbeam0,
						side,
						mmagtype);
	// std::cout<<*mptr<<std::endl;
	magnets.push_back(mptr);
      }
    std::sort(magnets.begin(), magnets.end(), absZGreater< Magnet::ConstPtr_t >);
    return magnets;
  }
}


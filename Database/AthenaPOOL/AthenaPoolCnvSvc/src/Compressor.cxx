/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Compressor.cpp
 *  Compressor
 *
 *  Created by Ilija Vukotic on 05/07/07.
 *  <ivukotic@cern.ch>
 *
 */

#include "AthenaPoolCnvSvc/Compressor.h"
using namespace std;

// ---------- STANDARD
void Compressor::reduceToUS(const std::vector<float> &vf, std::vector<unsigned short> &vc ){
	std::vector<float>::const_iterator i=vf.begin();
	unsigned int max_short_flt = 0x7f7f7fff;
	union {unsigned int u;float f;} m;
	vc.reserve(vf.size());
	for (;i<vf.end();i++){
		m.f=(*i);
		if ( (m.u & 0x7fffffff) > max_short_flt) vc.push_back (m.u>>16);
		else vc.push_back((m.u+0x8000)>>16);
	}
}

void Compressor::expandFromUStoFloat(const std::vector<unsigned short> &vc, std::vector<float> &vf){
	std::vector<unsigned short>::const_iterator i=vc.begin();
	union {unsigned int u;float f;} m;
	vf.reserve(vc.size());
	for (;i<vc.end();i++){
		unsigned int ui((*i)<<16);
		m.u=ui;
		vf.push_back(m.f);
	}
}

// ------------- WITH BITS AND SIGN
void Compressor::reduce(const std::vector<float> &vf, std::vector<unsigned int> &vi){

	std::vector<float>::const_iterator i=vf.begin();
	vi.reserve(vf.size()*m_bits/16+17);//this is roughly. can be made precisse. depends on sign stripping too.
	// composing format word
	unsigned int format=0;
	unsigned int vfs=vf.size();
	format |= vfs;  // writes down 25 bits of vector size
	format=format<<6; 
	format |= m_bits & 63;// six bits
	format =format<<1;
	format |= m_sign;
	vi.push_back(format);
	
	// initializing variables
	int bshift=32-m_bits;
	int rounding=0x1<<(bshift-1);
	unsigned int vmax= 0x7f7<<20;
	vmax |= 0x000fffff xor (rounding);
	unsigned int rems=0xffffffff;
	if(m_sign) 
		rems>>=(bshift+1); 
	else 
		rems>>=bshift;
	
	union {unsigned int u;float f;} m;
		
	
	if (m_bitStrip){
		int F=32; 
		int L;
		if (m_sign) L=m_bits-1; else L=m_bits;
		unsigned int CUR=0; int IN=0;
		for (;i<vf.end();++i){ 
			m.f=(*i);
			if ( (m.u & 0x7fffffff) > vmax) IN=m.u>>bshift;
			else IN=(m.u+rounding)>>bshift;
			
			IN&=rems; 
			
			if (F>L){ 
				F-=L;
				CUR|=(IN<<F); 			
				}
			else if(F==L){
				F-=L;
				CUR|=(IN<<F);
				vi.push_back(CUR);
				CUR=0;F=32;
			}
			else{
				int D=L-F;
				CUR|=(IN>>D);
				vi.push_back(CUR);
				CUR=0; F=32-D;
                                // Avoid int overflow in shift.
                                IN &= (1U<<D)-1;
				CUR|=(IN<<F);
				}
		}
		vi.push_back(CUR);
		
	}
	else
		for (;i<vf.end();i++){
			m.f=(*i);
			vi.push_back(m.u);
			}
	
	return;
}


void Compressor::expandToFloat(const std::vector<unsigned int> & vi, std::vector<float> & vf){
        vf.clear();
        if (vi.empty())
          return;

	std::vector<unsigned int>::const_iterator i=vi.begin();
	
	int format=(*i); i++;
	m_sign= 1 & format;
//	if (m_sign) cout<<"Sign was neglected"<<endl;
	format=format>>1;
	m_bits= 63 & format; 
//	cout<<"Uncompressing "<<m_bits<<" bits!"<<endl;
	unsigned int vecs=(format>>6)&0xfffffff;
//	cout<<"vec size: "<<vecs<<endl;
	int bshift=32-m_bits;
	
	vf.reserve(vi.size()*m_bits/16+17); //this is roughly. can be made precisse. depends on sign stripping too.
	
	int L;
	if (m_sign) L=m_bits-1; else L=m_bits;  
	int CP=0; int FP=0; int REM=0;
	unsigned int V=0xffffffff>>(32-L); unsigned int R=0;
	unsigned int ui(*i);

	union {unsigned int u;float f;} m;
	
	while (vecs){
		FP = CP + L;	// Future point = Current point + lenght 
		if (FP<=32){	// all of it is inside this integer 
			R = ( ui >> (32-FP) ) & V;
			R <<= bshift;
			m.u=R;
			vf.push_back(m.f); 
                        if (FP < 32)
                          CP=FP;
                        else {
                          CP = 0;
                          i++;
                          ui = (*i); // take next integer
                        }
		}
		else{			// part of the float is in the next integer
			REM = FP - 32;	// Remainder = Future point - 32
			R = ( ui & (0xffffffff >> CP) ) << REM; // find first part
			i++;
			ui = (*i); // take next integer
			R |=  ui >> (32-REM) ;
			R <<= bshift;
			if (m_sign) R &= 0x7fffffff;
			m.u=R;
			vf.push_back(m.f);  
			CP = REM;	// move Current point
		}
		--vecs;
	}
	return;
}

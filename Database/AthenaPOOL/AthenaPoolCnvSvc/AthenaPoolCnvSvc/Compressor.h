/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 *  Compressor.h
 *  Compressor
 *
 *  Created by Ilija Vukotic on 05/07/07.
 *  <ivukotic@cern.ch>
 *
 */


#ifndef ATHENAPOOLCNVSVC_COMPRESSOR_H
#define ATHENAPOOLCNVSVC_COMPRESSOR_H
 
// TO DO :
// add double to int compressor
// enable 32 bit "compression"
// should be smart enough to call compression to short when 16 bits required. or maybe people should be aware that 16 bits is faster than 17?
 
#include <vector>
#include <iostream>

class Compressor{

public:
	Compressor(){
		m_sign=false;
		m_bits=16;
                m_bitStrip = false;
	}
	~Compressor(){}
	
	void setNrBits(int bits){
		m_bits=bits;
		m_bitStrip=true; // will be used to disable compression (already programmed for compressor but not decompressor.)
		if (bits<12){
			std::cout<<"WARNING -> Too large compression requested. Will leave 12 bits instead!"<<std::endl;
			m_bits=12;
		}
		else if(bits>=32 || bits==0){
			std::cout<<"WARNING -> Compression to "<<bits<<" bits is not possible. Compressing to 31 bits !"<<std::endl;
			m_bits=31;
		}
	}
	
	void setIgnoreSign(){m_sign=true;}

	// standard compression 32 to 16 bits - precision is better than 0.4%
	void reduceToUS(const std::vector<float> &vf, std::vector<unsigned short> &vi);
	
	// standard uncompression from 16 bits to 32
	void expandFromUStoFloat(const std::vector<unsigned short> &vi, std::vector<float>&vf);
	
	//------------
	
	// compression with variable sign, range and precision
	void reduce(const std::vector<float> & vf, std::vector<unsigned int> &vi);
	
	// uncompression with variable sign,range and precision
	void  expandToFloat(const std::vector<unsigned int> & vc, std::vector<float> & vf);
	
private:
	int  m_bits;
	bool m_sign;
	bool m_bitStrip;
};

#endif // not ATHENAPOOLCNVSVC_COMPRESSOR_H

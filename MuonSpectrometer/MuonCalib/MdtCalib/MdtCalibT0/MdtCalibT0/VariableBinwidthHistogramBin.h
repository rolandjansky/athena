/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VARIABLEBINWIDTHHISTOGRAMBIN_H
#define VARIABLEBINWIDTHHISTOGRAMBIN_H

#include <cstdlib>
#include <iostream>
#include <vector>
#include <TString.h> // for Form

namespace MuonCalib {

/** @class VariableBinwidthHistogramBin
    A bin of a VariableBinwidthHistogram
    @author Felix.Rauscher@Physik.Uni-Muenchen.De
    @date February 2006

*/
class VariableBinwidthHistogramBin {
	public:
//----------------------------------constructor---------------------------------

/** Default constructor*/
	inline VariableBinwidthHistogramBin(): m_center(0.0), m_width(0.0), m_content(0) {}
	
/** Initializing constructor
    @param center the center of the bin
    @param width the width of the bin
    @param content the number of entries in the bin

*/
	inline VariableBinwidthHistogramBin(double center, double width, unsigned int content): m_center(center), m_width(width), m_content(content) {}
	
	
//-----------------------------------public member functions--------------------

/** Set bin
    @param center the center of the bin
    @param width the width of the bin
    @param content the number of entries in the bin

*/
	inline void Set(double center, double width, unsigned int content)
		{
		m_center=center;
		m_width=width;
		m_content=content;
		}

/**set content
   @param n new bin content
   
*/
	inline void SetContent(unsigned int n) {m_content=n;}

/**add to bin content
   @param n is added

*/
	inline  VariableBinwidthHistogramBin operator += (unsigned int n) {m_content+=n; return *this;}
/**add to bin content
   @param n is added

*/
	inline VariableBinwidthHistogramBin operator + (unsigned int n) {VariableBinwidthHistogramBin ret; ret+=n; return ret;}

/**move right bin boarder
   @param new_right right bin boarder
   	
*/
	void MoveRight(double new_right)
		{
		double left=Left();
		if(new_right< left) {
			throw std::runtime_error(Form("File: %s, Line: %d\nVariableBinwidthHistogramBin::MoveRight() - new right is too small!", __FILE__, __LINE__));
		}
		m_width = new_right - left;
		m_center = 0.5 * (left + new_right);
		}

/**move left bin boarder
   @param new_left left bin boarder
   	
*/
	void MoveLeft(double new_left)
		{
		double right=Right();
		if(new_left>right) {
			throw std::runtime_error(Form("File: %s, Line: %d\nVariableBinwidthHistogramBin::MoveRight() - new left is too small!", __FILE__, __LINE__));
		}
		m_width = right - new_left;
		m_center = 0.5 * (new_left + right);
		}
	

/**Get bin center */
	inline double Center() const {return m_center;}
	
/**Get width of the bin */
	inline double Width() const {return m_width;}
	
/**Get left (lower) bin border */
	inline double Left() const {return m_center - 0.5 * m_width;}
	
/**Get right (upper) bin border */
	inline double Right() const {return m_center + 0.5 * m_width;}

/**Get number of entries in the bin */
	inline unsigned int Entries() const {return m_content;}

/**Get density=Entries()/Width()*/
	inline double Density() const {return static_cast<double>(m_content) / m_width;}
	

	private:
//-----------------------------------------private data members-----------------
	
	double m_center, m_width;
	unsigned int m_content;
	};

/** @class VBHBinPtrSrt
    A pointer to a VariableBinwidthHistogramBin which supports sorting
    @author Felix.Rauscher@Physik.Uni-Muenchen.De
    @date February 2006

*/
class VBHBinPtrSrt {
public:
//-----------------------------------constructor--------------------------------
/** default constructor */
	inline VBHBinPtrSrt() {}

/** initializing constructor
	@param bin Pointer to bin
	
*/
	inline VBHBinPtrSrt(VariableBinwidthHistogramBin *bin): m_bin(bin) {}

//--------------------------------------public member functions-----------------

/** initialize
	@param bin Pointer to bin
	
*/
	inline void Initialize(VariableBinwidthHistogramBin *bin) {m_bin=bin;}
	
/** Get reference to bin

*/
	inline VariableBinwidthHistogramBin & Bin() {return *m_bin;}

/**Operator > for sorting bins by content
	@param other Other bin
	
*/
	inline bool operator > (const VBHBinPtrSrt &other) const
		{
		return m_bin->Width() > other.m_bin->Width();
		}

/**Operator < for sorting bins by content
	@param other Other bin
	
*/
	inline bool operator < (const VBHBinPtrSrt &other) const
		{
		return m_bin->Width() < other.m_bin->Width();
		}
private:
//------------------------------private data members----------------------------

	/// pointer to bin
		VariableBinwidthHistogramBin *m_bin;
	};
	
}

#endif

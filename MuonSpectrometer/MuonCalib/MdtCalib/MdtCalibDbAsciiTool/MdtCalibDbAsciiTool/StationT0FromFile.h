/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 01.02.2007, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_StationT0FromFileH
#define MuonCalib_StationT0FromFileH

//:::::::::::::::::::::::::::::::::
//:: CLASS StationT0FromFile ::
//:::::::::::::::::::::::::::::::::

/// \class StationT0FromFile
///
/// This class is used to read a t0 text file for a given station and to
/// provide the t0s to the user. It is meant to be an auxiliary class for
/// the stand-alone calibration programme. It might be possible in the future
/// to drop this class.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 01.02.2007

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>
#include <string>

namespace MuonCalib {

class StationT0FromFile {

public:
// Constructors //
	StationT0FromFile(void) {
		init();
		}
		///< Default constructor.

// Methods //
// get methods //
	double t0(const unsigned int & ml, const unsigned int & ly,
					const unsigned int & tb) const;
					///< get the t0 value of the tube tb
					///< in layer ly of multilayer ml;
					///< tb, ly, ml>0

// set methods //
	void readT0File(std::istream &infile);
					///< read the t0s stored in the file
					///< "file_name"

private:
// t0s //
	std::vector< std::vector< std::vector <double> > > m_t0; // t0 values

// private methods //
	void init(void); // initialization method

};

}

#endif

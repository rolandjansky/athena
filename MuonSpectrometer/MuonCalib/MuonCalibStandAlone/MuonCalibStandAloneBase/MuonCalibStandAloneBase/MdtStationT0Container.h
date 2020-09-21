/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_MdtStationT0ContainerH
#define MuonCalib_MdtStationT0ContainerH

//:::::::::::::::::::::::::::::::::
//:: CLASS MdtStationT0Container ::
//:::::::::::::::::::::::::::::::::

/// \class MdtStationT0Container
///
/// This class is used to read a t0 text file for a given station and to
/// provide the t0s to the user. It is meant to be an auxiliary class for
/// the stand-alone calibration programme. It might be possible in the future
/// to drop this class.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 01.02.2007

#include <vector>
#include <string>

namespace MuonCalib {

class MdtStationT0Container {

public:
// Constructors //
	MdtStationT0Container(void) :
m_t0(2, std::vector< std::vector<double> >(4,  std::vector<double>(78, 9e9))),
m_adc(2, std::vector< std::vector<double> >(4,  std::vector<double>(78, 9e9))),
m_t0_loaded(false)
	 {
	 }
		///< Default constructor.

	MdtStationT0Container(const std::string & file_name): 
m_t0(2, std::vector< std::vector<double> >(4,  std::vector<double>(78, 9e9))),
m_adc(2, std::vector< std::vector<double> >(4,  std::vector<double>(78, 9e9))),
m_t0_loaded(false)
	{
		readT0File(file_name);
	}
		///< Constructor: Initialize the class with the t0s stored in
		///<              the file "file_name".

// Methods //
// get methods //
	double t0(const unsigned int & ml, const unsigned int & ly,
					const unsigned int & tb) const;
					///< get the t0 value of the tube tb
					///< in layer ly of multilayer ml;
					///< tb, ly, ml>0
	double adc(const unsigned int & ml, const unsigned int & ly,
					const unsigned int & tb) const;
					///< get the t0 value of the tube tb
					///< in layer ly of multilayer ml;
					///< tb, ly, ml>0

// set methods //
	void readT0File(const std::string & file_name);
					///< read the t0s stored in the file
					///< "file_name"

	bool Sett0(const unsigned int & ml,
                                        const unsigned int & ly,
                                        const unsigned int & tb, double t0,
					double adc=0);
					///< set t0 value of the tube tb
                                        ///< in layer ly of multilayer ml;
                                        ///< tb, ly, ml>0

// return true if the t0s are loaded correctly //
	inline bool t0_loaded() const
		{
		return m_t0_loaded;
		}
		

private:
// t0s //
	std::vector< std::vector< std::vector <double> > > m_t0; // t0 values
	std::vector< std::vector< std::vector <double> > > m_adc; // t0 values

// private methods //
//	void init(void); // initialization method

//is true if t0s are loaded correctly
	bool m_t0_loaded;

};

}

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 31.03.1999, AUTHOR: OLIVER KORTNER
// Modified: 30.05.1999, removed dynamic memory management due to copy problems
//           02.03.2000, t(const HepDouble & r, const HepInt & option) added.
//           24.04.2000, error calculation improved.
//           25.05.2001, correct interpolation in r(r, option) and
//                       t(r, option).
// 19.06.2003: ALESSANDRO DE SALVO
//	     19.06.2003, added support for GARFIELD input format
// 02.10.2003: DANIELA REBUZZI
//	     02.10.2003, adapted for Athena MDT_Digitizer
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///////////////////////////////////////////////////////////
// THIS CLASS CONTAINS THE r-t RELATION FOT MDT TUBES. //
///////////////////////////////////////////////////////////

#ifndef RtrelationH
#define RtrelationH

#define GARFIELD 1
#define CALIB 0

#include <string>
#include <iostream>
#include <fstream>

class Rt_relation {

private:
// units: [mm], [ns]
	int nb_pairs;  // number of (r,t) pairs
	int rt_type;   // type of rt file-format (CALIB or GARFIELD)
	double rtstep; // step width
	std::string  rtname;   // rt-relation name
	double r_drift[600], t_drift[600]; // pointer to the array of
					      // (r,t) pairs
	double r_err[600]; // pointer to the array of the errors of r
	double t_err[600]; // pointer to the array of the errors of t
	inline void copy(const Rt_relation & rt); // copy routine
	Rt_relation & operator=(const Rt_relation &right); // assignment operator
	
public:
// constructors
	// default constructor
	Rt_relation(void) {
		nb_pairs = 0;
		rt_type  = 0;
                rtstep = 0.;
                rtname = "";
                for (unsigned int i=0;i<600;i++){
                  r_drift[i] = 0.0;
                  t_drift[i] = 0.0;
                  r_err[i] = 0.0;
                  t_err[i] = 0.0;
                }
	}	
	// copy constructor				
	Rt_relation(const Rt_relation & rt) {
		copy(rt);
	}				

// methods
// get the number of (r,t) pairs
	inline int number_of_pairs(void) const { return nb_pairs; }
	
// get the step					
	inline double rt_step(void) const { return rtstep; }
	
// get the rt-relation name						
	inline std::string name(void) const { return rtname; }
	
// get r for the k-th pair, 0 <= k < number of pairs						
	inline int rt_format(void)       const { return rt_type; }
	inline double r(const int & k) const;

// get r(t), t[0] <= t <= t[nb_pairs], 
// option = 1 means: throw a warning if t is out of range.
// option = 0 means: don't throw a warning.
// In both cases, edge values are used in case of error.
	inline double r(const double & t, const int & option) const;

// get t for the k-th pair, 0 <= k < number of pairs
	inline double t(const int & k) const;

// get t(r), r[0] <= r <= r[nb_pairs],
// option = 1 means: throw a warning if r is out of range.
// option = 0 means: don't throw a warning.
// In both cases, edge values are used in case of error.
	inline double t(const double & r, const int & option) const;

// get the error on r for the k-th pair, 0 <= k < number of pairs						
	inline double error(const int & k) const;
						 
// get error(r(t)), t[0] <= t <= t[nb_pairs],
// option = 1 means: throw a warning if t is out of range.
// option = 0 means: don't throw a warning.
// In both cases, edge values are used in case of error.
	inline double error(const double & t, const int & option)const;
	
// get error(t(r)), t[0] <= t <= t[nb_pairs], 
// option = 1 means: throw a warning if t is out of range.
// option = 0 means: don't throw a warning.
// In both cases, edge values are used in case of error.						
	inline double t_error(const double & t, const int & option) const;

// get error on t(r), r[0] <= r <= r[nb_pairs-1],
// option = 1 means: throw a warning if t is out of range.
// option = 0 means: don't throw a warning.
// In both cases, edge values are used in case of error.
	inline double t_error_garfield(const double & r, const int & option) const;

// set the rt format ID, a call to this routine removes a stored
// r-t relation,rt_format = CALIB or GARFIELD
	inline void set_rt_format(const int & rt_format);
						
// set the rt-relation name
	inline void set_name(const std::string name) { rtname = name; };

// set the number of pairs to n_pairs, a call to this
// routine removes a stored r-t relation, n_pairs <= 600						
	inline void set_number_of_pairs(const int & n_pairs);
						
// set the k-th (r,t) pair to (r,t), set the error of r to
// error, 0 <= k < number of pairs
	inline void set_r_t(const int & k, const double & r,
				const double & t, const double & error);
						
// read r-t relation from file rt_file
	inline void read_rt(std::ifstream & rt_file);

};


#include "MDT_Digitization/rt_relation.icc"

#endif

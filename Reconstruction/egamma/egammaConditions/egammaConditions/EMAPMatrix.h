/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////
/** @class EMAPMatrix

    The EMAPMatrix class is a templete class which was design to represent a
    sort of a n-dimensional Histogram. Each entry of the matrix must provide
    a copy constructor and a addition function. It is based on APMatrix in 
    InsituPerformance, by Matthias Schott, but simplified to remove
    dependencies and features not used. (EMClusterErrors is not additive,
    so all the addition routines are removed.)

    @author Jovan Mitrevski (this version), Matthias Schott (original)
**/
/////////////////////////////////////////////////////////////////////////////

#ifndef EMAPMATRIX_H
#define EMAPMATRIX_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "GaudiKernel/StatusCode.h"

// a helper class
class EMAPMatrixAxis 
{
public:
	
  /** Setup the axis again. All old information will be destroyed
      @param name :	Name of axis, e.g. "Eta"
      @param bins : stl std::vector with binning values - must be in increasing order
  */		
  EMAPMatrixAxis(std::string name, const std::vector <double> &bins): m_name(std::move(name)), m_vecBins(bins) {};

		
  /** Default Destructor */
  ~EMAPMatrixAxis() {};  
		
  std::vector<double> getBinningInformation() const {return m_vecBins; };

  /** Return number of bins*/
  unsigned int getNumberOfBins() const;
		
  /** Returns number of index of bin in which the value x falls*/
  int getBinIndex(double x) const;
  /** returns mean of bin at index i*/
//   double getMeanOfBin(unsigned int i) const;         
//   /** returns width of bin at index i*/
//   double getWidthOfBin(unsigned int i) const;
		
  bool isInRange(double x) const;

//   /** set name of axis*/
//   void     setName(std::string name);
//   /** return name of axis*/
  std::string   getName() const {return m_name; };
	
private:
  std::string m_name;
  std::vector<double> m_vecBins;
};


template<class T>
class EMAPMatrix
{
public:
  // Constructors
		
  /** Default constructor*/
  EMAPMatrix();
  /** Constructor with std::vector of axes to define dimensions and binnging of this matrix
      @param axes: std::vector of EMAPMatrixAxis objects
  */
  EMAPMatrix(const std::vector<EMAPMatrixAxis>& axes, const std::string& textDescription);
  /** Constructor with std::vector of axes to define dimensions and binnging of this matrix
      @param axes: std::vector of EMAPMatrixAxis objects
      @emptyObject: Standard object which is used to fill the matrix if it is supposed to be empty/cleaned*/
  // 		EMAPMatrix(std::vector<EMAPMatrixAxis> axes, T emptyObject);

  /** Copy constructor */
  EMAPMatrix(const EMAPMatrix& ob) = default;
				
  /** Default destructor */
  ~EMAPMatrix();
		
  /** Operator = Overload */
  EMAPMatrix& operator = (const EMAPMatrix& ob) = default;
		
  // Constructor Methods
  /** Setup Matrix via given std::vector of EMAPMatrixAxis objects */
  StatusCode setupMatrix(std::vector<EMAPMatrixAxis> axes);
  // 		/** Setup matrix with std::vector of axes to define dimensions and binnging of this matrix
  // 			@param axes: std::vector of EMAPMatrixAxis objects
  // 			@emptyObject: Standard object which is used to fill the matrix if it is supposed to be empty/cleaned*/
  // 		void setupMatrix(std::vector<EMAPMatrixAxis> axes, T emptyObject);
		
  // Accessing the Matrix via Functions
  /** Set Bin-content to "T" in bin which corresponds to value "x". Can only used for 1-dimensional matrix
      @param x: value on x-axis
      @param T: value which should be set*/
  StatusCode setBinContent(double x, T value);
  /** Set Bin-content to "T" in bin which corresponds to value "x" and "y". Can only used for 2-dimensional matrix
      @param x: value on x-axis
      @param y: value on y-axis
      @param T: value which should be set*/
  StatusCode setBinContent(double x, double y, T value);
  /** Set Bin-content to "T" in bin which corresponds to value "x,y,z". Can only used for 3-dimensional matrix
      @param x: value on x-axis
      @param y: value on y-axis
      @param z: value on z-axis
      @param T: value which should be set*/
  StatusCode setBinContent(double x, double y, double z, T value);
  /** Set Bin-content to "T" in bin which corresponds to value "x,y,z,u". Can only used for 4-dimensional matrix*/
  StatusCode setBinContent(double x, double y, double z, double u, T value);
  /** Set Bin-content to "T" in bin which corresponds to value "x,y,z,u,v". Can only used for 5-dimensional matrix*/
  StatusCode setBinContent(double x, double y, double z, double u, double v, T value);
  /** Set Bin-content to "T" in bin which corresponds to value std::vector x. Can only used for n-dimensional 
      matrix if the length of the std::vector x ind n.
      @param x: std::vector of length n which represents the values on all axes
      @param T: value which should be set*/
  StatusCode setBinContent(const std::vector<double>& x, T value);

  /** Get Bin-content to "T" in bin which corresponds to value "x". Can only used for 1-dimensional matrix
      @param x: value on x-axis
      @param T: value which is received*/
  const T* getBinContent(double x) const;
  /** Get Bin-content to "T" in bin which corresponds to value "x,y". Can only used for 2-dimensional matrix
      @param x: value on x-axis
      @param y: value on y-axis
      @param T: value which is received*/
  const T* getBinContent(double x, double y) const;
  /** Get Bin-content to "T" in bin which corresponds to value "x,y,z". Can only used for 3-dimensional matrix
      @param x: value on x-axis
      @param y: value on y-axis
      @param z: value on z-axis
      @param T: value which is received*/
  const T* getBinContent(double x, double y, double z) const;
  /** Get Bin-content to "T" in bin which corresponds to value "x,y,z,u". Can only used for 4-dimensional matrix*/
  const T* getBinContent(double x, double y, double z, double u) const;
  /** Get Bin-content to "T" in bin which corresponds to value "x,y,z,u,v". Can only used for 5-dimensional matrix*/
  const T* getBinContent(double x, double y, double z, double u, double v) const;
  /** Get Bin-content to "T" in bin which corresponds to value std::vector x. Can only used for n-dimensional 
      matrix if the length of the std::vector x ind n.
      @param x: std::vector of length n which represents the values on all axes
      @param T: value which should be recieved*/
  const T* getBinContent(std::vector<double> x) const;

  // 		/** Add "T" to bin which corresponds to value std::vector x. Can only used for n-dimensional 
  // 			matrix if the length of the std::vector x ind n.
  // 			@param x: std::vector of length n which represents the values on all axes
  // 			@param T: value which should be added*/
  // 		bool	addBinContent(std::vector<double> x, T value);
		
  // Get StatusCode
  /** Return status code of last operations. Some access operations might have failed. In this case, the statusCode is set to false*/
  // 		bool getStatusCode() 			{	return m_StatusCode;}
  /** Return number of dimensions of this matrix*/
  int  getDimensions() const		{	return m_dimensions;}

  // Get Axis Information
  EMAPMatrixAxis  getAxis(unsigned int i) const	{	return m_axis[i];}
				
  /** return the std::vector<T> which contains all content information of the matrix. Note that the binning and dimensions is not coded inside*/
  std::vector<T> getValues() const 	{	return m_matrix;}

		
  // Clearing functions
		
  /** Clear all Matrix information (Also binning,...)*/
  void  clear();
  /** Clear Matrix content but not binning, dimensions and axis*/
  void  clearEntries();

  bool isInRange(std::vector<double> x) const;
		
  /** The user can add a Description of what this matrix contains and how it was produced, e.g. cuts*/
  void setTextDescription(const std::string& text);
  /** The user can add a Description of what this matrix contains and how it was produced, e.g. cuts*/
  std::string getTextDescription() const;
		
protected:	

  // T			*m_emptyObject;
  unsigned int		m_dimensions;
  // 		bool			m_StatusCode;
  std::vector<EMAPMatrixAxis>	m_axis;

  std::vector< T >  		m_matrix;	/// representation of the content of matrix
  std::vector< unsigned int > 	m_base;		/// helper object which is used to "convert" the 1dim m_matrix into a n-dimensional object
  std::string			m_textDescription;	/// Description of the content of this matrix
		
  //	private:
  //static const unsigned int 	m_dimMax=5;
				
  StatusCode setupEntries();
		
  unsigned int				Index(std::vector<unsigned int> x) const;
  std::vector<unsigned int> 		Index(unsigned int id);
  // 		T& 					Access(std::vector<unsigned int> x);
};

#include "egammaConditions/EMAPMatrix.icc"

#endif

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataContainer.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ANALYSISCALIBRATIONDATACONTAINER_H
#define ANALYSISCALIBRATIONDATACONTAINER_H

#include <vector>
#include <map>
#include <string>

#include "TMap.h"
#include "THashList.h"

class TH1;

#include "CalibrationDataInterface/CalibrationDataVariables.h"

namespace Analysis {

  /** 
      The following typedef is for convenience: most uncertainties can be asymmetric.
      The pair holds the positive (first) and negative (second) uncertainties. Note that
      in the context of systematic uncertainties, "positive" and "negative" may reflect
      what happens if the uncertainty parameter is varied. This means that the "positive"
      uncertainty result may be negative etc.
   */

  typedef std::pair<double, double> UncertaintyResult;

   /**
      Maximum dimensionality of the calibration parametrisation.
      This is just so that variables do not need to be carried around in various methods.
   */

  const unsigned int MaxCalibrationVars = 10;

  /** @class CalibrationDataContainer

      This is the interface for the objects to be stored in the calibration ROOT file.
      The base class inherits from a TMap, the idea bing that the map's keys are TObjString
      objects while the values are the objects of interest. The keys are meant to provide
      access in a flexible manner.
   */


  class CalibrationDataContainer: public TMap {
  public:
    CalibrationDataContainer(const char* name = "default"); // default ctor needed for persistence
    virtual ~CalibrationDataContainer(); // virtual dtor needed for persistence

    /** @enum CalibrationParametrization

	The following enums are intended to contain the list of (kinematic or other)
	variables in terms of which results (and the corresponding uncertainties) are given.
	They are useful mainly for internal purposes, but the user may access them nevertheless.
    */

    enum CalibrationParametrization {
      kPt = 0,       // calibrated jet pt
      kEta = 1,      // jet eta
      kAbsEta = 2,   // jet |eta|
      kTagWeight = 3 // tagging output (relevant for "continuous" calibration)
    };

    /** @brief retrieve the list of "uncertainties" accessible to this object.
	A few uncertainty names are predetermined: "result", "comment", "statistics", "systematics".
	Individual sources of systematic uncertainty can be added by the user.
     */
    std::vector<std::string> listUncertainties() const;

    /**
       retrieve the list of "uncertainties" accessible to this object.
       A few uncertainty names are predetermined: "result", "comment", "statistics", "systematics".
       Individual sources of systematic uncertainty can be added by the user.
     */
    CalibrationStatus getUncertainties(const CalibrationDataVariables& x,
				       std::map<std::string, Analysis::UncertaintyResult>& all) const;

    /** @brief retrieve the comments entered for this calibration, if any */
    std::string getComment() const;

    /** @brief retrieve the 'hadronisation reference' entered for this calibration, if any */
    std::string getHadronisation() const;

    /**
       retrieve the calibration result.
       @param  x           user-supplied (kinematic or other) variables
       @param  result      requested result
       @param  obj         object holding the requested result (it will be computed if not provided)
       @param  extrapolate flag that extrapolation applies (should only be relevant when using eigenvector variations)
       @return status code (see above)
    */
    virtual CalibrationStatus getResult(const CalibrationDataVariables& x,
					double& result,
					TObject* obj = 0, bool extrapolate = false) const = 0;

    /**
       retrieve the calibration statistical uncertainty.
       @param  x      user-supplied (kinematic or other) variables
       @param  result requested statistical uncertainty
       @return status code (see above)
       Note the changed signature compared to getUncertainty(), getResult() etc.:
       this is because the statistical uncertainty computation always needs the result
       object, and only in case of the function interface also the covariance matrix
    */
    virtual CalibrationStatus getStatUncertainty(const CalibrationDataVariables& x,
						 double& result) const = 0;

    /**
       retrieve the calibration total systematic uncertainty
       @see    getUncertainty()
    */
    CalibrationStatus         getSystUncertainty(const CalibrationDataVariables& x,
						 UncertaintyResult& result,
						 TObject* obj = 0) const;

    /**
       retrieve the calibration uncertainty due to the given source.
       @param  x      user-supplied (kinematic or other) variables
       @param  unc    uncertainty specification
       @param  result requested uncertainty (for both positive and negative variation, if available)
       @param  obj    object holding the requested uncertainty information (it will be computed if not provided)
       @return status code (see above)
    */
    virtual CalibrationStatus getUncertainty(const std::string& unc,
					     const CalibrationDataVariables& x,
					     UncertaintyResult& result, TObject* obj = 0) const = 0;
    
    /** @brief insert the main object for this calibration */
    void setResult(TObject* obj);

    /** @brief insert the given text as comment for this calibration */
    void setComment(const std::string& text);

    /** @brief insert the given text as the 'hadronisation reference' for this calibration */
    void setHadronisation(const std::string& text);

    /** @brief insert the relevant object for the requested source of 'uncertainty' */
    void setUncertainty(const std::string& unc, TObject* obj);

    /**
       If true, this will restrict the variables used to be within the (specified) range of validity.
       Note that this is a policy decision and as such not intrinsic to the data; but it is
       cumbersome to carry this information around everywhere.
    */
    void restrictToRange(bool restrict) { m_restrict = restrict; }

    /** @brief allow the user to inspect the above information */
    bool isRangeRestricted() const { return m_restrict; }

    /** retrieve the lower bound of validity for the requested variable type
	@param vartype      variable type
	@param extrapolate  true only if an extrapolation uncertainty is requested
     */
    double getLowerBound(unsigned int vartype, bool extrapolate = false) const;

    /** retrieve the upper bound of validity for the requested variable type
	@param vartype      variable type 
	@param extrapolate  true only if an extrapolation uncertainty is requested
     */
    double getUpperBound(unsigned int vartype, bool extrapolate = false) const;

    /** @brief allow the user to inspect the bounds of validity */
    std::vector<std::pair<double, double> > getBounds() const;

    /** utility to retrieve variable types  */
    std::vector<unsigned int> getVariableTypes() const;

    /** utility for comparison of doubles */
    static bool isNearlyEqual (double a, double b);

  protected:

    /** Connection between variable names (on histogram axes etc.) and variable 'types' as used in actual evaluations.
	Normal result values are positive (or 0); a negative return value indicates that the string is not known)
     */
    int typeFromString(const std::string& key) const;

    // /** compute the variables to be used for the given 'uncertainty'
    // 	@param  x user-supplied (kinematic or other) variables
    // 	@param  unc given source of uncertainty (can also be the central value)
    // 	@return false if the given variables are outside the parametrisation's validity range
    // */
    // bool computeVariables(const std::string& unc,
    // 			  const CalibrationDataVariables& x) const;

    /** Compute the variables to be used.
	@param  x           user-supplied (kinematic or other) variables
	@param  extrapolate set to "true" for the evaluation of extrapolation uncertainties
	@return kSuccess, kRange, or kExtrapolatedRange, depending on the kinematic variables
    */
    CalibrationStatus computeVariables(const CalibrationDataVariables& x, bool extrapolate = false) const;

    /** @brief decode the 'uncertainty' objects' names to determine the relevant variable types */
    virtual void computeVariableTypes() const = 0;

    mutable std::vector<double> m_lowerBounds;   // lower validity bounds
    mutable std::vector<double> m_upperBounds;   // upper validity bounds
    // persistency not needed for the following (relevant for histogram uncertainties only and computed automatically)
    mutable std::vector<double> m_lowerBoundsExtrapolated;   //! (possibly looser) lower validity bounds for extrapolation
    mutable std::vector<double> m_upperBoundsExtrapolated;   //! (possibly looser) upper validity bounds for extrapolation

    /** cached variables for code speed-up */
    mutable TObject* m_objResult;                //! don't persistify
    mutable TObject* m_objSystematics;           //! don't persistify

    mutable double m_vars[MaxCalibrationVars];   //! don't persistify

    /** @brief specification of variable type per object (result / uncertainty) */
    mutable std::vector<unsigned int> m_variables;  //! persistency not needed for this variable
    // mutable std::map<std::string, std::vector<unsigned int> > m_variables; //! persistency not needed for this variable
    // mutable std::vector<unsigned int>* m_variablesResult;                  //! persistency not needed for this variable

  private:

    /** @brief specifies whether the performance evaluation is to be done strictly within the range of validity */
    bool m_restrict;

    ClassDef(CalibrationDataContainer, 1);  // base class for calibration data objects

  };




  /** @class CalibrationDataHistogramContainer

      This is the class holding information for histogram-based calibration results.
      
   */

//__________________________________________________________________________________________

  class CalibrationDataHistogramContainer: public CalibrationDataContainer {
  public:
    CalibrationDataHistogramContainer(const char* name = "default"); // default ctor needed for persistence
    virtual ~CalibrationDataHistogramContainer();    // virtual dtor needed for persistence
    virtual CalibrationStatus getResult(const CalibrationDataVariables& x, double& result,
					TObject* obj = 0, bool extrapolate = false) const;
    virtual CalibrationStatus getStatUncertainty(const CalibrationDataVariables& x, double& result) const;
    virtual CalibrationStatus getUncertainty(const std::string& unc, const CalibrationDataVariables& x,
					     UncertaintyResult& result, TObject* obj = 0) const;

    /** Indicate whether the given uncertainty is correlated from bin to bin or not
	(note that this function is to be used only for _systematic_ uncertainties) */
    bool isBinCorrelated(const std::string& unc) const;

    /** Indicate that the given uncertainty is to be treated uncorrelated from bin to bin
	(note that the default is for all systematic uncertainties to be treated as correlated) */
    void setUncorrelated(const std::string& unc);

    /** Indicate whether results are to be interpolated between bins or not
	(this feature is thought to be useful mostly for MC efficiencies) */
    void setInterpolated(bool doInterpolate);

    /** Indicate whether histogram interpolation is used or not */
    virtual bool isInterpolated() const;

    /** Test whether this calibration object is one for "continuous" calibration
	(this has some subtle consequences for the treatment of bin-to-bin correlations).
	The return value will be -1 in case this is not a "continuous" calibration object,
	and the axis number (0 for X, 1 for Y, 2 for Z) otherwise. */
    virtual int getTagWeightAxis() const;

    /** Retrieve the bin boundaries for the specified variable type (which should be a CalibrationParametrization enum).
	An empty vector will be returned if the specified variable is not actually used. */
    virtual std::vector<double> getBinBoundaries(unsigned int vartype) const;

    /** Retrieve the number of eigenvectors to be retained for the purpose of eigenvector variation reduction strategies.
	@param  choice  specification of the reduction option (integer corresponding to the CalibrationDataInterfaceROOT::EVReductionStrategy enum)
	@return         number of eigenvector variations to be retained; -1 if the required information is lacking
    */
    virtual int getEigenvectorReduction(unsigned int choice) const;
    
  protected:

    /** Cache for bin boundary information */
    mutable std::map<unsigned int, std::vector<double> > m_binBoundaries; //! no need to persistify

  private:

    /** @brief list here the _systematic_ uncertainties that are not correlated from bin to bin */
    THashList m_uncorrelatedSyst;

    /** If true, interpolate between bins rather than doing a straight bin-wise evaluation */
    bool m_interpolate;

    /** Retrieve interpolated result (utility function) */
    double getInterpolatedResult(TH1* hist) const;

    /** Retrieve interpolated result (utility function) */
    double getInterpolatedUncertainty(TH1* hist) const;

    /** check the bounds of validity for this calibration object */
    void checkBounds();

    // /** utility for determining global bin number, subject to extrapolation constraints.
    // 	@param  hist          pointer to histogram containing actual bin boundary information
    // 	@param  doExtrapolate specifies whether or not an extrapolation uncertainty is being requested
    // */
    // Int_t findBin(const TH1* hist, bool doExtrapolate) const;

    /** decode the 'uncertainty' objects' names to determine the relevant variable types */
    virtual void computeVariableTypes() const;

    ClassDef(CalibrationDataHistogramContainer, 2);  // histogram based calibration data object
  };

  /** @class CalibrationDataMappedHistogramContainer

      This is the class holding information for histogram-based calibration results,
      in cases where 'irregular' binning is used (e.g., if the binning in one variable depends on another variable),
      or where the dimensionality becomes too high for even 3D histograms (so that a mapping is needed to use THx).

      Using this class instead of the 'regular' histogram container will imply a performance penalty, as bin
      numberings are not assumed to feature any regularity.
      
   */

//__________________________________________________________________________________________

  class CalibrationDataMappedHistogramContainer: public CalibrationDataHistogramContainer {
  public:
    CalibrationDataMappedHistogramContainer(const char* name = "default"); // default ctor needed for persistence
    virtual ~CalibrationDataMappedHistogramContainer();    // virtual dtor needed for persistence
    virtual CalibrationStatus getResult(const CalibrationDataVariables& x, double& result,
					TObject* obj = 0, bool extrapolate = false) const;
    virtual CalibrationStatus getStatUncertainty(const CalibrationDataVariables& x, double& result) const;
    virtual CalibrationStatus getUncertainty(const std::string& unc, const CalibrationDataVariables& x,
					     UncertaintyResult& result, TObject* obj = 0) const;

    /** Indicate whether histogram interpolation is used or not. Not applicable here. */
    virtual bool isInterpolated() const { return false; }

    /** Test whether this calibration object is one for "continuous" calibration
	(this has some subtle consequences for the treatment of bin-to-bin correlations).
	The return value will be -1 in case this is not a "continuous" calibration object,
	and the axis number (0 for X, 1 for Y, 2 for Z) otherwise. */
    virtual int getTagWeightAxis() const;

    /** Set (by hand) the variables that will be mapped onto a single histogram axis */
    void setMappedVariables(const std::vector<std::string>& variables);

    /** List which variables get mapped onto a single histogram axis */
    std::vector<std::string> getMappedVariables() const;

    /** Retrieve the bin boundaries for the specified variable type (which should be a CalibrationParametrization enum).
	An empty vector will be returned if the specified variable is not actually used. */
    virtual std::vector<double> getBinBoundaries(unsigned int vartype) const;

    /** Helper class for the specification of custom binning.
	This is a very simple class, containing only the bin specification and associated access methods.
     */
    class Bin {
    public:
      Bin();                                                 // default constructor (for persistency)
      Bin(unsigned int dimension, double* low, double* up);
      Bin(const Bin& other);
      Bin& operator=(const Bin& other);
      ~Bin();
      // return the number of dimensions
      unsigned int getDimension() const { return m_dimension; }
      bool   contains(double* x) const;
      double getUpperBound(unsigned int dim) const;
      double getLowerBound(unsigned int dim) const;
    private:
      unsigned int m_dimension;
      double*      m_low; //[m_dimension]
      double*      m_up;  //[m_dimension]
    };

    /** Add mapping bin.
	The return value is the bin number. */
    unsigned int addBin(const Bin& bin);

    /** return the number of mapped bins */
    unsigned int getNMappedBins() const;

  private:

    /** mapped variables. These have to be specified when creating the object,
	for there is no way to transmit this information otherwise. */
    std::vector<std::string>  m_mapped;

    /** starting position of mapped variables */
    mutable unsigned int m_beginMapped;                   //! don't persistify

    /** calibration bins */
    std::vector<Bin> m_bins;
    // cache index to the last used bin
    mutable unsigned int m_lastBin;                       //! don't persistify

    /** find the bin number corresponding to the input variables */
    Int_t findBin() const;
    Int_t findMappedBin(double* x) const;

    /** check the bounds of validity for this calibration object */
    void checkBounds();

    /** @brief decode the 'uncertainty' objects' names to determine the relevant variable types */
    virtual void computeVariableTypes() const;

    ClassDef(CalibrationDataMappedHistogramContainer, 1);  // 'mapped' histogram based calibration data object
  };


  /** @class CalibrationDataFunctionContainer

      This is the class holding information for function-based calibration results.
   */

//__________________________________________________________________________________________

  class CalibrationDataFunctionContainer: public CalibrationDataContainer {
  public:
    CalibrationDataFunctionContainer(const char* name = "default"); // default ctor needed for persistence
    virtual ~CalibrationDataFunctionContainer();   // virtual dtor needed for persistence
    virtual CalibrationStatus getResult(const CalibrationDataVariables& x, double& result,
					TObject* obj = 0, bool /* extrapolate */ = false) const;
    virtual CalibrationStatus getStatUncertainty(const CalibrationDataVariables& x, double& result) const;
    virtual CalibrationStatus getUncertainty(const std::string& unc, const CalibrationDataVariables& x,
					     UncertaintyResult& result,  TObject* obj = 0) const;

    /** Set the lower bound of validity for the given variable */
    inline void setLowerBound(int vartype, double bound) {m_lowerBounds[vartype] = bound; }
    
    /** Set the lower bound of validity for the given variable */
    inline void setUpperBound(int vartype, double bound) {m_upperBounds[vartype] = bound; }
    
  private:

    mutable TObject* m_objStatistics;      //! cached

    /** @brief decode the 'uncertainty' objects' names to determine the relevant variable types */
    virtual void computeVariableTypes() const;

    ClassDef(CalibrationDataFunctionContainer, 1); // function based calibration data object
  };

} // end of namespace

#endif 

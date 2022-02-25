/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*! @file TElectronMCShifterTool.h
    @brief A Tool used to shift MC varibles.

    @author Rob Roy Fletcher (robflet@sas.upenn.edu)
    @version 0.1
*/

/*! \page MCShiftingToolInstructions_page Instructions for the MC Shifting Tool.

    This tool is used to shift Monte Carlo distributions to correct against some
    of the known problems in the simulation. These distributions are shifted by
    comparing them to data distributions that are well understood (Z->ee in this case).
    Not all of the variables are shifted by this tool. Currently the shifted variable are:
        - el_reta
        - el_wstot
        - el_ws3
        - el_f1
        - el_f3
        - el_weta2
        .
    all other variable will just have their unshifted value returned. Currently the shift values are common for a
    given eta bin. In otherwords variable are shifted by the same amount for all Et bins in a given eta bin. Functionality
    for shifting in both Et and eta bins is already included and may be revisited.

    Note that ws3=w3strips=weta1=w1=weta1c, and fside=fracm=fracs1, so these names may be used interchangably at times.

    Documentation (which includes this) can be found at http://hn.hep.upenn.edu/Analysis/robflet/html/index.html

    To use this tool you can just insert one of the shifting functions in some place where the variable
    you wish to shift is used. There are a few options for inputs.

    <ul>
    <li>shiftVar(double eT,double eta,ElePIDNames::Var var,double val)
        <ul>
        <li>The inputs here are the eT and eta values of the electron, the name of the variable using the enum defined here, and the unshifted value of the desired variable.
        <li>This function will return the shifted value of the variable.
        <li>Variable that should not be shifted automatically return the input value, so you do not need to worry about skipping over those variables if for instance you are looping over a list of them.However, if there is a variable that is one of those shifted by this tool that you do not want to shift, then you must not call this function on that one.
        </ul>
    <li>shiftVarByBin(int eTBin,int etaBin,ElePIDNames::Var var,double val)
        <ul>
        <li>The operation of this function is essentially the same as the above, but the inputs are slightly different.If you use eT and eta bin numbers instead of their respective values you may use this function.
        <li>The bins are defined as <BR> eTBin = [ 7, 10, 15, 20, 30, 40] (GeV) So the input should be an int 0-5 <BR>
                               etaBin = [ 0.00, 0.60, 0.80, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37]  input is an int 0-8
                               output test.
        </ul>
    </ul>


*/

#ifndef TELECTRONMCSHIFTERTOOL_H
#define TELECTRONMCSHIFTERTOOL_H

#include <map>
#include <vector>


/*! @namespace ElePIDNames

  Namespace for the PID names.
*/
namespace ElePIDNames{
  /*! @enum Var

      Enum used to hold names of variables used in the shifter tool. This is
      so the tool may select the correct shift value.
  */
  enum Var {
     DeltaPoverP
    ,TRTHighTOutliersRatio
    ,d0significance
    ,deltaeta1
    ,deltaphi2
    ,deltaphiRescaled
    ,eratio
    ,f1
    ,f3
    ,fside
    ,reta
    ,rhad
    ,rphi
    ,trackd0pvunbiased
    ,weta2
    ,ws3
    ,wstot
    ,e277
    ,DeltaE
    ,NumVars
  };
}

  //! The base class for the Shifter tool. Holds all shift values.
  /*!

  */
class TElectronMCShifterTool{
public:
  //! Default constructor.
  TElectronMCShifterTool();

  //! Default destructor.
  ~TElectronMCShifterTool();

 //!Get the shift amount
 /*!
	Returns only the shift amount. This is the number in the Shifts table that is added to the
	variable to obtain the shifted result.

	@param eT The eT value of the electron.
	@param eta The eta value of the electron.
	@param var enum member corresponding to the variable. Type is ElePIDNames::Var
	@return The shift value for the current electron/variable.
	@sa getShiftDeltaByBin()
 */
 double getShiftDelta(double eT, double eta, ElePIDNames::Var var){
   int ibin_combined = getShifterEtBin(eT)*6+getShifterEtaBin(eta); //Convert the 2d
   return Shifts[var].at(ibin_combined);
 };

 //! Get the shift value from bin numbers.
 /*!
	@param eT The eT bin number [0-5].
	@param eta The eta bin number [0-8].
	@param var enum member corresponding to the variable. Type is ElePIDNames::Var
	@return The shift value for the current electron/variable.
	@sa getShiftDelta()
 */
 double getShiftDeltaByBin(int eT, int eta, ElePIDNames::Var var){
   int ibin_combined = getShifterEtBin(eT)*6+getShifterEtaBin(eta); //Convert the 2d
   return Shifts[var].at(ibin_combined);
 };

 //! Shift a variable.
 /*!
	Retuns the value of the variable given plus the shift amount as defined in the
	table of shifts. Variables shifted are reta, f1, f3, wstot, ws3, and weta2. All
	other variable will just return the input value.

	@param eT The eT value of the electron.
	@param eta The eta value of the electron.
	@param var enum member corresponding to the variable. Type is ElePIDNames::Var
	@param val The unshifted value of the variable.
	@return The shift value for the current electron/variable.
	@sa shiftVarByBin(), shiftStruct()
 */
 double shiftVar(double eT,double eta,ElePIDNames::Var var,double val);

 //! Shift a variable by the eT and eta bins.
 /*!
	@param eT The eT bin of the electron [0-5].
	@param eta The eta bin of the electron[0-8].
	@param var enum member corresponding to the variable. Type is ElePIDNames::Var
	@param val The unshifted value of the variable.
	@return The shift value for the current electron/variable.
	@sa shiftVarByBin(), shiftStruct()
 */
 double shiftVarByBin(int eTBin,int etaBin,ElePIDNames::Var var,double val);

 //! shift all variables as references.
 /*!
	Takes in references to all of the variable values and modifies them
	directly. This is so it can be used in the xAOD wrapper class along
	with the photon tool.

	@param eT The eT value of the electron.
	@param eta The eta value of the electron.
	@return None
  */
 void shiftAll( float eT,
                float eta,
                float& rhad1  ,
                float& rhad   ,
                float& reta   ,
                float& rphi   ,
                float& weta2  ,
                float& f1     ,
                float& f3     ,
                float& fside  ,
                float& ws3   ,
                float& eratio ,
                float& e277   ,
                float& DeltaE ,
                float& deltaeta1,
                float& deltaphiRescaled
              ) const;

 //static const double Shifts[17][54]; // 17 variables x 9 eta bins x 6 et bins
 std::vector<float> Shifts[ElePIDNames::NumVars];
 /** @brief vector to hold the widths to be applied */
 std::vector<float> Widths[ElePIDNames::NumVars];

private :
 //! Matrix of shift values.
 /*!
	2D array holding all of the shift values. Dimensions are [17][54] for 17 variables x (9 eta bins x 6 et bins).
	The eT and eta bins are combined into one dimension of length [54]
 */

 //! Convert the eT value to eT bin.
 /*!
	@param eT The value of eT.
	@return The bin associated with the given eT value.
 */
 static unsigned int getShifterEtBin(double eT) ;

 //! Convert the eta value to eta bin.
 /*!
	@param eta The value of eta.
	@return The bin associated with the given eta value.
 */
 static unsigned int getShifterEtaBin(double eta) ;

};

#endif

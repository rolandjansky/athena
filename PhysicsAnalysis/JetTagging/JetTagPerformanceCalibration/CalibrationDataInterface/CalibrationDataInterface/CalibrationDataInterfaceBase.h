/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// 
// for some details on weighting event using the SF, see
// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/TopCommonObjects#B_tagging
// http://indico.cern.ch/getFile.py/access?contribId=4&resId=0&materialId=slides&confId=106205
// http://cdsweb.cern.ch/record/1269912
// 

#ifndef ANALYSISCALIBRATIONDATAINTERFACEBASE_H
#define ANALYSISCALIBRATIONDATAINTERFACEBASE_H

//#include "Rtypes.h"
#include "TNamed.h"
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <algorithm>

class TF1;

namespace Analysis 
{

  const double CalibZERO = 1.e-6;
  const double dummyValue = -1.;
  typedef std::pair<double,double> CalibResult;
  // The following result is returned upon error
  const CalibResult dummyResult(dummyValue,dummyValue);
  //  const CalibResult dummyResult(std::make_pair(dummyValue,dummyValue));

  /** @class CalibrationDataInterfaceBase 

      This class provides some common functionality for other classes wishing to
      access Flavour Tagging performance estimates.

      @author  Frank Filthaut <F.Filthaut@science.ru.nl>
  */  

  class CalibrationDataInterfaceBase : public TNamed {
    public:
      CalibrationDataInterfaceBase();

       /** default destructor */
      virtual ~CalibrationDataInterfaceBase() = 0;
      
      /**
	 Main interface methods accessing the flavour tagging performance information.
	 Note that for both of the following, the label is assumed to adhere to the
	 TruthInfo conventions (see package PhysicsAnalysis/JetTagging/JetTagInfo).
      */

      const std::string& EffCalibrationName(const std::string& flavour, unsigned int mapIndex = 0) const;
      void setEffCalibrationNames(const std::map<std::string, std::vector<std::string> >& names);
      
      const std::string& SFCalibrationName(const std::string& flavour) const;
      void setSFCalibrationNames(const std::map<std::string, std::string>& names);
      
      /** @brief known variable types that can be used as function arguments */
      enum variableType { kEta, kAbsEta, kPt };

    private:

      /** @brief this simply collects the per-flavour properties.
	  NB there ought to be no need for the 'mutable' qualifier here... */
      mutable std::map<std::string, std::vector<std::string> > m_calibrationEffNames;
      mutable std::map<std::string, std::string> m_calibrationSFNames;
      
    protected:

      // /** @brief auxiliary function for string concatenation */
      // std::string getBasename (const std::string& OP, const std::string& flavour,
      // 			       const std::string& extra, bool SF, unsigned int mapIndex = 0) const;

      /** @brief auxiliary function for retrieval of container name */
      std::string getContainername (const std::string& flavour,
				    bool SF, unsigned int mapIndex = 0) const;

      /** auxiliary function for retrieval of name within the directory */
      std::string getBasename (const std::string& name) const;

      /** @brief utility function for combination of statistical and (a priori asymmetric)
          systematic uncertainty.
	  NB perhaps this should be in its own */
      double combinedUncertainty (double stat, const std::pair<double, double>& syst) const;

      /** @brief tagging algorithm name */
      std::string m_taggerName;

      // make it possible to persistify this class (for PROOF purposes)
      ClassDef(CalibrationDataInterfaceBase,1)
    }; 
} // end of namespace

#endif 

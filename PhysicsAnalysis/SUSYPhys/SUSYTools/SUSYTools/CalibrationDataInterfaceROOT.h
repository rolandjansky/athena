/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceROOT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SUSYTOOLS_ANALYSISCALIBRATIONDATAINTERFACEROOT_H
#define SUSYTOOLS_ANALYSISCALIBRATIONDATAINTERFACEROOT_H


#include "SUSYTools/CalibrationDataInterfaceBase.h"

#include "TObject.h"
#include "Rtypes.h"
#include <vector>
#include <string>
#include <map>
#include <utility>

class TObject;
class TFile;

namespace SUSY
{

  /** @class CalibrationDataInterfaceROOT 

      This tool provides an interface to flavour tagging performance estimates.

      A separate instance should be used for each different tagging algorithm.
      For each instance, all appropriate jet collections and tagger operating points
      need to be specified.

      The model:
      - b-jets:
      *   data-MC scale factor (factorised 2D function of eta, pt)
      *   MC reference
      *   the product is the data efficiency; alternatively, the scale factor may be used
      - c-jets:
      *   as for b-jets, but with a different MC reference
      - light-flavour jets:
      *   data-MC scale factor (factorised 2D function of eta, pt)
      *   MC reference
      Besides the results, it is also possible to retrieve associated uncertainties. This need not
      be configured, and a choice as to the uncertainty component can be made on a case-by-case
      basis.

      The idea is to use the same physical ROOT file that is also accessed through COOL, but to do
      so in a stand-alone fashion, so that there is no COOL or Athena dependence. Apart from this,
      the same infrastructure and limitations as with COOL access (@class CalibrationDataInterfaceTool)
      hold: in particular, "channel" (jet author) aliasing is possible. The difference is that
      run-time information is to be provided in a text file to be interpreted using ROOT's TEnv and
      

      DB representation (all residing under a common root folder):
      .../<jet author>/<tagger>/<operating point>/<flavour>/<object name>

      @author  Frank Filthaut <F.Filthaut@science.ru.nl>
  */  

  // Package all jet variables into a single structure

  struct CalibrationDataVariables {
    std::string jetAuthor;
    double jetPt;
    double jetEta;
  };

  enum Uncertainty { None, Total, Statistical, Systematic };

  class CalibrationDataInterfaceROOT: public CalibrationDataInterfaceBase
    {
    public:
      CalibrationDataInterfaceROOT(const std::string& taggerName, std::string configname = "BTagCalibration.env", std::string pathname = "");
      CalibrationDataInterfaceROOT(); //For PROOF object retrieval only

       /** default destructor */
      virtual ~CalibrationDataInterfaceROOT();
      
      /**
	 Main interface methods accessing the flavour tagging performance information.
	 Note that for both of the following, the label is assumed to adhere to the
	 TruthInfo conventions (see package PhysicsAnalysis/JetTagging/JetTagInfo).
      */

      /** retrieve either the total uncertainty or only the statistical or systematic components.
	  Note that this enum is a duplication of that in class ICalibrationDataInterfaceTool:
	  not desirable but nontrivial to get around */
    

      /** efficiency retrieval */
      CalibResult getEfficiency (const CalibrationDataVariables& variables, const std::string& label,
				 const std::string& OP, Uncertainty unc = None) const;
      
      /** "MC" efficiency retrieval */
      CalibResult getMCEfficiency (const CalibrationDataVariables& variables, const std::string& label,
				   const std::string& OP, Uncertainty unc = None) const;
      
      /** efficiency scale factor retrieval */
      CalibResult getScaleFactor (const CalibrationDataVariables& variables, const std::string& label,
				  const std::string& OP, Uncertainty unc = None) const;

      /**  "MC" inefficiency scale factor retrieval    */
      CalibResult getInefficiencyScaleFactor(const CalibrationDataVariables& variables, const std::string& label,
					     const std::string& OP, Uncertainty unc = None) const;

      /**   inefficiency retrieval */
      CalibResult getInefficiency (const CalibrationDataVariables& variables, const std::string& label,
				   const std::string& OP, Uncertainty unc = None) const;

      /**     "MC" inefficiency retrieval */
      CalibResult getMCInefficiency (const CalibrationDataVariables& variables, const std::string& label,
				     const std::string& OP, Uncertainty unc = None) const;

      /** initialization for PROOF usage */
		void initialize(const std::string& jetauthor, const std::string& OP, Uncertainty unc);
						
#ifndef SUSY_ATHENA
   		ClassDef(CalibrationDataInterfaceROOT,1) 
#endif
      
    private:

      /** @brief auxiliary function to retrieve values of function arguments */
      void retrieveFunctionArguments(const CalibrationDataVariables& variables,
				     const std::string& object, Double_t* x) const;

      /** @brief pointer to the TFile object providing access to the calibrations */
      TFile* m_fileEff; //! Do not attempt to persistify (PROOF)
      TFile* m_fileSF; //! Do not attempt to persistify (PROOF)

      /** @brief jet author aliases (there is no single CalibrationBroker object here to
	  take care of this, so we do it in this class) */
      std::map<std::string, std::string> m_aliases;

      /** @brief cache the objects themselves (so that the user will not have to delete
	  them after each call etc.) */
      mutable std::map<std::string, TObject*> m_objects;

      /** @brief utility function taking care of object retrieval */
      TObject* retrieveTObject(const std::string& name, bool eff) const;

      /** @brief associated alias retrieval method */
      std::string getAlias(const std::string& author) const;

      /** @ brief construct the full object pathname from its individual components */
      std::string fullName(const std::string& author, const std::string& name) const;

    }; 
} // end of namespace

#endif 

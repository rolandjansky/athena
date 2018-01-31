/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceROOT.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ANALYSISCALIBRATIONDATAINTERFACEROOT_H
#define ANALYSISCALIBRATIONDATAINTERFACEROOT_H


#include "CalibrationDataInterface/CalibrationDataVariables.h"
#include "CalibrationDataInterface/CalibrationDataInterfaceBase.h"

//#include "Rtypes.h"
#include "TMatrixDSym.h"
#include "TH1.h"
#include <vector>
#include <string>
#include <map>

class TFile;
class TMap;

namespace Analysis 
{
  class CalibrationDataContainer;

  class CalibrationDataEigenVariations;

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
      where <operating point> can be a weight cut (represented as a string, with the period replaced
      with an underscore) or the string "Continuous" (for continuous calibration purposes).

      @author  Frank Filthaut <F.Filthaut@science.ru.nl>
  */  

  /** specification of type information requested by the user */
  enum Uncertainty { None, Total, Statistical, Systematic, Extrapolation, SFEigen, SFNamed, TauExtrapolation };

  /** specification of Eigenvector variations reduction strategy. For implementation reasons the possibilities are given in "reverse" order. */
  enum EVReductionStrategy { Tight = 0, Medium = 1, Loose = 2};
  
  /** strategies for dealing with specified kinematic variables being outside their range of validity:
      ignore any items, flag items (this will merely produce a summary at the end of the job),
      giving up when even a specified extrapolation range is exceeded, or giving up upon any out-of-bound
      condition. The strategy can be set in the configuration file. Note that the "GiveUpExtrapolated"
      strategy (give up upon finding that the kinematic variables are outside even the extrapolation range)
      implementation is only partial, as this will be flagged only if extrapolation uncertainties are
      requested. */
  enum OutOfBoundsStrategy { Ignore, Flag, GiveUpExtrapolated, GiveUp };

  /** counter types (to be used when flagging out-of-bounds cases) */
  enum OutOfBoundsType { Main, Extrapolated, Eta, TagWeight };

  class CalibrationDataInterfaceROOT: public CalibrationDataInterfaceBase
    {
    public:
      /** main constructor for "stand-alone" use (with information fed in from a .env configuration file read by TEnv) */
      CalibrationDataInterfaceROOT(const std::string& taggerName, std::string configname = "BTagCalibration.env", std::string pathname = "");

      /** alternative constructor passing configuration information explicitly (so that no .env file is needed) */
      CalibrationDataInterfaceROOT(const std::string& taggerName,
				   const char* fileSF, const char* fileEff,
				   const std::vector<std::string>& jetAliases,
				   const std::map<std::string, std::string>& SFNames,
				   const std::map<std::string, std::vector<std::string> >& EffNames,
				   const std::map<std::string, std::vector<std::string> >& excludeFromEV,
				   const std::map<std::string, Analysis::EVReductionStrategy> EVReductions,
				   bool useEV = true, bool useMCMCSF = true, bool useTopologyRescaling = false);

      /** default constructor for PROOF object retrieval */
      CalibrationDataInterfaceROOT();

      /** copy constructor */
      CalibrationDataInterfaceROOT(const CalibrationDataInterfaceROOT& other);
      
      /** default destructor */
      virtual ~CalibrationDataInterfaceROOT();
      
      // ------------------------------------------------------------------------------------------

      // Main jet-by-jet interface methods accessing the flavour tagging performance information.
      // Note that for both of the following, the label is assumed to adhere to the
      // TruthInfo conventions (see package PhysicsAnalysis/JetTagging/JetTagInfo).

      /** efficiency scale factor retrieval by name. */
      CalibResult getScaleFactor (const CalibrationDataVariables& variables,
				  const std::string& label, const std::string& OP,
				  Uncertainty unc, unsigned int numVariation = 0,
				  unsigned int mapIndex = 0) const;

     /** efficiency retrieval by name */
      CalibResult getEfficiency (const CalibrationDataVariables& variables,
				 const std::string& label, const std::string& OP,
				 Uncertainty unc, unsigned int numVariation = 0,
				 unsigned int mapIndex = 0) const;
      
      /** "MC" inefficiency scale factor retrieval by name */
      CalibResult getInefficiencyScaleFactor(const CalibrationDataVariables& variables,
					     const std::string& label, const std::string& OP,
					     Uncertainty unc, unsigned int numVariation = 0,
					     unsigned int mapIndex = 0) const;

      /** inefficiency retrieval by name */
      CalibResult getInefficiency (const CalibrationDataVariables& variables,
				   const std::string& label, const std::string& OP,
				   Uncertainty unc, unsigned int numVariation = 0,
				   unsigned int mapIndex = 0) const;

      /** "MC" efficiency retrieval by name */
      CalibResult getMCEfficiency (const CalibrationDataVariables& variables, const std::string& label,
     				   const std::string& OP, Uncertainty unc = None, unsigned int mapIndex = 0) const;
     
      /** "MC" inefficiency retrieval by name */
      CalibResult getMCInefficiency (const CalibrationDataVariables& variables, const std::string& label,
     				     const std::string& OP, Uncertainty unc = None, unsigned int mapIndex = 0) const;


      /** @brief retrieve the list of "uncertainties" relevant to the calibration object.
	  A few uncertainty names are predetermined: "result", "comment", "statistics", "systematics".
	  Other sources of systematic uncertainty may be added.
	  Note that the "systematics" source does not give access to correlations between bins.
	  If the 'named' argument is true, the list does not include all uncertainties but only those
	  excluded from the eigenvector construction (this option is only relevant if eigenvector use
	  has been switched on to begin with). In this case the order of the uncertainties listed is
	  important since it indicates the index by which the given named uncertainty is identified.
      */
      std::vector<std::string> listScaleFactorUncertainties(const std::string& author,
							    const std::string& label,
							    const std::string& OP,
							    bool named = false) const;

      /** @brief retrieve the number of variations relevant to the calibration object.
	  The Uncertainty enum is used to specify the category.
      */
      unsigned int getNumVariations(const std::string& author, const std::string& label,
				    const std::string& OP, Uncertainty unc) const;

      // ------------------------------------------------------------------------------------------

      // Main jet-by-jet interface methods accessing the flavour tagging performance information.
      // The difference with the previous set of methods is that no label and operating point are
      // passed; instead, it is assumed that the user has retrieved the (unsigned) integer index
      // corresponding to the label/OP/jet collection using the retrieveCalibrationIndex() method
      // below. The methods below are not protected against wrong usage of this index!

      /** Retrieve the index of the calibration object (container) starting from the label and operating point.
	  The return value will be false if the requested object cannot be found. */
      bool retrieveCalibrationIndex (const std::string& label, const std::string& OP,
				     const std::string& author, bool isSF, unsigned int& index,
				     unsigned int mapIndex = 0) const;

      /** Retrieve the name of the calibration object (container) given its index */
      std::string nameFromIndex (unsigned int index) const;

      /** efficiency scale factor retrieval by index */
      CalibResult getScaleFactor (const CalibrationDataVariables& variables,
				  unsigned int indexSF, unsigned int indexEff,
				  Uncertainty unc, unsigned int numVariation = 0) const;

     /** efficiency retrieval by index */
      CalibResult getEfficiency (const CalibrationDataVariables& variables,
				 unsigned int indexSF, unsigned int indexEff,
				 Uncertainty unc, unsigned int numVariation = 0) const;
      
      /**  "MC" inefficiency scale factor retrieval by index */
      CalibResult getInefficiencyScaleFactor(const CalibrationDataVariables& variables,
					     unsigned int indexSF, unsigned int indexEff,
					     Uncertainty unc, unsigned int numVariation = 0) const;

      /**   inefficiency retrieval by index */
      CalibResult getInefficiency (const CalibrationDataVariables& variables,
				   unsigned int indexSF, unsigned int indexEff,
                                   Uncertainty unc, unsigned int numVariation = 0) const;

      /** "MC" efficiency retrieval by index */
      CalibResult getMCEfficiency (const CalibrationDataVariables& variables, unsigned int index,
     				   Uncertainty unc = None) const;
     
      /** "MC" inefficiency retrieval by index */
      CalibResult getMCInefficiency (const CalibrationDataVariables& variables, unsigned int index,
     				     Uncertainty unc = None) const;

      /** MC/MC scale factor retrieval. Normally this is to be used only internally; however, since
	  this information may be of interest it is made public anyway. */
      double      getMCMCScaleFactor(const CalibrationDataVariables& variables,
				     unsigned indexSF, unsigned int indexEff) const;

      /** @brief retrieve the list of "uncertainties" relevant to the calibration object.
	  A few uncertainty names are predetermined: "result", "comment", "statistics", "systematics".
	  Other sources of systematic uncertainty may be added.
	  Note that the "systematics" source does not give access to correlations between bins.
	  If the 'named' argument is true, the list does not include all uncertainties but only those
	  excluded from the eigenvector construction (this option is only relevant if eigenvector use
	  has been switched on to begin with). In this case the order of the uncertainties listed is
	  important since it indicates the index by which the given named uncertainty is identified.
      */
      std::vector<std::string> listScaleFactorUncertainties(unsigned int index,
							    bool named = false) const;

      /** @brief retrieve the number of variations relevant to the calibration object.
	  The Uncertainty enum is used to specify the category.
      */
      unsigned int getNumVariations(unsigned int index, Uncertainty unc) const;

      /** @ brief construct the full object pathname from its individual components */
      std::string fullName(const std::string& author, const std::string& OP,
			   const std::string& label, bool isSF, unsigned mapIndex = 0) const;

      // ------------------------------------------------------------------------------------------

      // Main jet-by-jet interface methods accessing the flavour tagging performance information.
      // The difference with the previous set of methods is in the signatures of the methods, where
      // the return value is a status code while the actual calibration result is to be passed as
      // one of the arguments. Other than this, the same comments hold as for the previous set of
      // methods.

      /** efficiency scale factor retrieval by index */
      CalibrationStatus getScaleFactor (const CalibrationDataVariables& variables,
					unsigned int indexSF, unsigned int indexEff,
					Uncertainty unc, unsigned int numVariation,
					CalibResult& result) const;

     /** efficiency retrieval by index */
      CalibrationStatus getEfficiency (const CalibrationDataVariables& variables,
				       unsigned int indexSF, unsigned int indexEff,
				       Uncertainty unc, unsigned int numVariation,
				       CalibResult& result) const;
      
      /**  "MC" inefficiency scale factor retrieval by index */
      CalibrationStatus getInefficiencyScaleFactor(const CalibrationDataVariables& variables,
						   unsigned int indexSF, unsigned int indexEff,
						   Uncertainty unc, unsigned int numVariation,
						   CalibResult& result) const;

      /**   inefficiency retrieval by index */
      CalibrationStatus getInefficiency (const CalibrationDataVariables& variables,
					 unsigned int indexSF, unsigned int indexEff,
					 Uncertainty unc, unsigned int numVariation,
					 CalibResult& result) const;

      /** "MC" efficiency retrieval by index */
      CalibrationStatus getMCEfficiency (const CalibrationDataVariables& variables,
					 unsigned int index, Uncertainty unc,
					 CalibResult& result) const;
     
      // ------------------------------------------------------------------------------------------

      // Main jet-by-jet interface methods accessing "pseudo-continuous" flavour tagging performance
      // information. This refers to data/MC scale factors in tag weight bins rather than scale factors
      // for jets to pass a fixed tag weight cut.
      // Both versions (one using names, one using integer indices) are available. As for the latter: like
      // for the regular (non-continuous) case, the method is not protected against wrong usage of this index!

      /** efficiency scale factor retrieval by name */
      CalibResult getWeightScaleFactor (const CalibrationDataVariables& variables, const std::string& label,
					Uncertainty unc, unsigned int numVariation = 0, unsigned int mapIndex = 0) const;

      /** efficiency scale factor retrieval by index */
      CalibResult getWeightScaleFactor (const CalibrationDataVariables& variables,
					unsigned int indexSF, unsigned int indexEff,
					Uncertainty unc, unsigned int numVariation = 0) const;

      /** efficiency scale factor retrieval by index, with different signature */
      CalibrationStatus getWeightScaleFactor (const CalibrationDataVariables& variables,
					      unsigned int indexSF, unsigned int indexEff,
					      Uncertainty unc, unsigned int numVariation,
					      CalibResult& result) const;

      // ------------------------------------------------------------------------------------------

      // Utilities

      /** retrieve the MC efficiency (central values) object for the given flavour label and operating point.
	  A null result will be returned in case of error (e.g. if the calibration object isn't
	  binned to begin with). It is the user's responsibility to verify whether the object derives from
	  a TH1 or a TF1.
       */
      const TObject* getMCEfficiencyObject (const std::string& author, const std::string& label,
					    const std::string& OP, unsigned int mapIndex = 0) const;

      /** retrieve the binned calibration object for the given flavour label and operating point.
	  A null result will be returned in case of error (e.g. if the calibration object isn't
	  binned to begin with).
       */
      const TH1* getBinnedScaleFactors (const std::string& author, const std::string& label,
					const std::string& OP) const;

      /** retrieve the binned calibration object for the given flavour label and operating point,
	  with the result shifted by the given number of standard deviations for the given
	  systematic uncertainty.
	  A null result will be returned in case of error (e.g. if the calibration object isn't
	  binned to begin with, or if the uncertainty asked for isn't fully correlated from bin
	  to bin).
       */
      const TH1* getShiftedScaleFactors (const std::string& author, const std::string& label,
					 const std::string& OP, const std::string& unc,
					 double sigmas) const;

      /** retrieve the named covariance matrix element corresponding to the binned calibration object.
	  The unc argument should correspond to a given source of statistical or systematic uncertainty,
	  or "all" (in case the full covariance matrix is required)
	  For 2D and 3D histograms, the bin numbering follows the "global" bin number as defined by class TH1.
       */
      TMatrixDSym getScaleFactorCovarianceMatrix (const std::string& author, const std::string& label,
						  const std::string& OP, const std::string& unc = "all") const;

      // ------------------------------------------------------------------------------------------

      // Helper class dealing with the hadronisation reference information.
      // Rootcint requires this to be public.

      class HadronisationReferenceHelper {
      public:
	HadronisationReferenceHelper(TMap* mapSF, TMap* mapEff);
	// default constructor (for PROOF purposes)
	HadronisationReferenceHelper() {;}
	/**
	   Retrieve the (full) name of the reference histogram, given the hadronisation specification.
	   The return value will be false if the specification cannot be found, and true otherwise.
	*/
	bool getReference(const std::string& spec, std::string& ref) const;

      private:
	/** map from hadronisation specification to container name */
	std::map<std::string, std::string> m_refs;
      };

      // ------------------------------------------------------------------------------------------

      /** initialization for PROOF usage */
      void initialize(const std::string& jetauthor, const std::string& OP, Uncertainty unc);

    private:

      /** @brief pointer to the TFile object providing access to the calibrations */
      TFile* m_fileEff; //! Do not attempt to persistify (PROOF)
      TFile* m_fileSF; //! Do not attempt to persistify (PROOF)

      /** @brief jet author aliases (there is no single CalibrationBroker object here to
	  take care of this, so we do it in this class) */
      std::map<std::string, std::string> m_aliases;

      /** @brief cache the objects themselves (so that the user will not have to delete
	  them after each call etc.). The caching is done so that objects can be retrieved
	  by number as well as by (OP, flavour, calibration name) combination. */
      std::vector<CalibrationDataContainer*> m_objects;
      std::map<std::string, unsigned int>    m_objectIndices;

      /** @brief utility function taking care of object retrieval */
      // CalibrationDataContainer* retrieveContainer(const std::string& name, bool isSF);
      CalibrationDataContainer* retrieveContainer(const std::string& dir, const std::string& cntname, bool isSF);

      /** @brief utility function taking care of eigenvector objects retrieval */
      //      const CalibrationDataEigenVariations* retrieveCalibrationDataEV(const CalibrationDataContainer* calibrationDataContainer) const;

      /** @brief associated alias retrieval method */
      std::string getAlias(const std::string& author) const;

      /** @brief in addition, store also the filenames themselves (needed for the copy constructor) */
      std::string m_filenameSF;
      std::string m_filenameEff;
      
      // ------------------------------------------------------------------------------------------

      // Eigenvector variations functionality

      /** store the eigenvector class and associate to its CalibrationDataContainer **/
      mutable std::map<const CalibrationDataContainer*,const CalibrationDataEigenVariations*> m_eigenVariationsMap;

      /** decide whether to run the eigenvector method or not **/
      bool m_runEigenVectorMethod;

      /** Eigenvector reduction strategy (per flavour) **/
      std::map<std::string, Analysis::EVReductionStrategy> m_EVReductions;
      
      /** store the uncertainties which should be excluded from building the full covariance matrix **/
      std::map<std::string, std::vector<std::string> > m_excludeFromCovMatrix;

      // ------------------------------------------------------------------------------------------

      // MC/MC scale factors functionality

      /** specify whether or not to use MC/MC (hadronisation) scale factors
          (the fact that this is steerable is intended to be temporary only) */
      bool m_useMCMCSF;
      /** specify whether or not to use MC/MC (topology) scale factors
	  (also this steering option may be removed) */
      bool m_useTopologyRescaling;

      /** the following maps (one for each directory) specify the name of the container
	  serving as the 'hadronisation' reference for each object */
      mutable std::map<std::string, HadronisationReferenceHelper*> m_refMap;
      /** store the 'hadronisation' reference for each object (-1 means no reference found) */
      mutable std::vector<int>                                     m_hadronisationReference;

      // ------------------------------------------------------------------------------------------

      // Bounds checking functionality

      /** |eta| bounds and strategy for dealing with out-of-bounds conditions */
      double               m_maxAbsEta;
      OutOfBoundsStrategy  m_absEtaStrategy;
      OutOfBoundsStrategy  m_otherStrategy;
      void                 checkAbsEta(const CalibrationDataVariables& variables, unsigned int index) const;
      /** counters for flagging out-of-bound cases */
      std::vector<unsigned int> m_etaCounters;
      std::vector<unsigned int> m_mainCounters;
      std::vector<unsigned int> m_extrapolatedCounters;
      // std::vector<unsigned int> m_tagWeightCounters;
      void increaseCounter(unsigned int index, OutOfBoundsType oob = Main);

      // for (one-time) checks of scale factors
      void checkWeightScaleFactors(unsigned int indexSF, unsigned int indexEff) const;
      mutable std::vector<std::pair<unsigned int, unsigned int> > m_checkedWeightScaleFactors;
      double               m_maxTagWeight;

      // make it possible to persistify this class (for PROOF purposes)
      ClassDef(CalibrationDataInterfaceROOT,1)   // platform-independent (main) interface for access to b-tagging information
    }; 
  
} // end of namespace

#endif // ANALYSISCALIBRATIONDATAINTERFACEROOT_H

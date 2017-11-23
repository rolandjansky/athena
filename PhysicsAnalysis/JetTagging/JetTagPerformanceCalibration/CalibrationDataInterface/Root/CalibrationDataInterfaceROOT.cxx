/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                          //
// CalibrationDataInterfaceROOT                                                             //
/* Begin_Html
<h2>Purpose</h2>
<p>This class provides the main interface to b-tagging calibration information.
   Each instance can be used to handle a single tagger (so if multiple taggers' information
   needs to be accessed, multiple CalibrationDataInterfaceROOT instances need to be created).<br />
   Its action is steered by a configuration file (which is parsed using TEnv and which is
   specified in the CalibrationDataInterfaceROOT constructor).
</p>
<p>
   Further details can be found below, arranged by topic: <a href="#basic">basic usage</a>,
   <a href="#ev">eigenvector variation usage</a>, and <a href="#configuration">configuration file
   specification</a>.
</p>
<a name="basic"><h2>Basic usage</h2></a>
<p>
   The main functionality is to retrieve data/MC calibration scale factors and MC efficiencies,
   on a jet-by-jet basis. This is done using the following methods:
   <pre>
       getScaleFactor  (variables, label, OP, unc)
       getMCEfficiency (variables, label, OP, unc, mapIndex)
   </pre>
   with the following meaning for the arguments:
   <pre>
       variables:  a CalibrationDataVariables object that should be filled by the user with all
                   the information needed to extract the calibration information
       label:      flavour label. Currently recognised labels follow the Athena TruthInfo conventions
                   (see package PhysicsAnalysis/JetTagging/JetTagInfo): "B", "C", "T", "N/A", and in
		   addition "Light" (the "N/A" is internally converted to "Light")
       OP:         tagger working point. This should correspond to a documented weight cut for the
                   tagger under consideration, but converted to a string, and with any period (".")
		   replaced with an underscore for technical reasons. Alternatively, "Continuous" may
		   be used if a "continuous tagging" calibration object exists for the tagger under
		   consideration. (Note that the use of this method for "continuous tagging" is not
		   in general to be recommended, as it does not not allow for scale factor rescaling,
		   which is ne
       unc:        enum specifying what uncertainty information is to be retrieved. Recognised values
                   for normal usage are <b>None</b> (retrieval of central values only), <b>Statistical</b>
		   (statistical uncertainty), <b>Systematic</b> (systematic uncertainty), <b>Total</b>
		   (combined statistical and systematic uncertainty). (Other choices exist for more advanced
		   usage and will be described <a href="#ev">below</a>.)
       mapIndex:   index specifying the precise MC efficiency calibration to be used. This is relevant if
                   multiple "efficiency calibration" names are specified for the given flavour label (see
		   the <a href="#configuration">configuration section</a>).
   </pre>
   All methods return a pair&lt;double, double&gt; (the stated <b>CalibResult</b> return type is a simple typedef),
   the first member of which contains the central value and the second the requested uncertainty (or 0 if no
   uncertainty was requested). Note that central values are retricted to be within the physical region (between 0
   and 1 for the MC efficiencies, and positive for the scale factors).
</p>
<p>
   A few additional methods exist, based on (and internally using) the above methods:
   <pre>
       getEfficiency               (variables, label, OP, unc, mapIndex): data efficiency calculated as MC efficiency times SF
       getInefficiency             (variables, label, OP, unc, mapIndex): 1 - data efficiency
       getMCInefficiency           (variables, label, OP, unc, mapIndex): 1 - MC efficiency
       getInefficiencyScaleFactor  (variables, label, OP, unc, mapIndex): ratio of data to MC inefficiencies
   </pre>
   Especially the last of these methods is likely to be of interest when data/MC scale factors are applied;
   <a href="http://cdsweb.cern.ch/record/1269912">this note</a> provides more detail. Restrictions to the physical
   range apply, as is the case for the data/MC scale factor and MC efficiency methods.
</p>
<a name="speed-up"><h3>Code speed-up using variable caching</h3></a>
<p>
   Internally, pointers to the calibration objects are cached. However, the string matching needed to
   exploit this feature is slow and leads to a non-negligible CPU overhead. Since internally all pointers
   are stored in a vector, it is possible to request the position in this vector for a given calibration
   object. This position can then be used in subsequent alternative calls to retrieve information:
   <pre>
       getScaleFactor  (variables, indexSF, indexEff, unc)
       getMCEfficiency (variables, index, unc)
   </pre>
   Here the "index" variables replace the specification of the flavour label and operating point. They can
   be retrieved by calling
   <pre>
       retrieveCalibrationIndex(label, OP, author, isSF, index)
   </pre>
   where the following additional variables are to be used:
   <pre>
       author:      jet collection name
       isSF:        set to true (false) if the object concerned is a scale factor object (a MC efficiency object)
       index:       requested information
   </pre>
   Note that this method has a boolean return value, indicating whether or not the retrieval succeeded.
   This return value should be checked by the user, as no checks are carried in the alternative getScaleFactor()
   etc. methods as to the validity of the index specified. Note also that for all methods except those using
   only the MC information, both indices need to be provided.
</p>
<a name="continuous"><h3>Continuous tagging</h3></a>
<p>
   Rather than merely inquiring whether the tag weight discriminant resulting from a given tagger satisfies a
   given criterion, it may be useful to use more detailed information. In particular, "continuous tagging"
   information can be made available. This is essentially a calibrated version of the (binned) tag weight
   discriminant distribution, or to be more precise, the corresponding data/MC ratio.
</p>
<p>
   The use of this information differs somewhat from the regular calibration usage, as the "transport" from
   the calibration sample(s) to the sample used in physics becomes a non-trivial extension of the efficiency
   and inefficiency scale factors used for the regular calibrations. Therefore, two separate methods exist
   which should be used for such cases:
   <pre>
       getWeightScaleFactor (variables, label, unc, numVariation, mapIndex)
       getWeightScaleFactor (variables, indexSF, indexEff, unc, numVariation)
   </pre>
   The meaning of the variables is as above.
</p>
<a name="ev"><h2>Eigenvector variations</h2></a>
<p>
   The 'basic usage' described above does not do justice to the fact that data/MC calibration scale factors
   are derived in kinematic bins, with uncertainties not being fully correlated from bin to bin (indeed, the
   methods themselves do not know anything about binning to start with). It is however possible to arrive at
   a statistically more correct approach by considering 'variations'. This exploits the fact that in the case
   of scale factor calibrations, besides statistical and total systematic uncertainties also individual
   systematic uncertainties are stored (along with a model of their bin-to-bin correlations). The typical use
   of this, applicable for a systematic uncertainty that is fully correlated (or anti-correlated) between bins,
   would be to consider the effect on all calibration scale factors from a &pm; 1 standard deviation of the
   underlying source of systematic.
</p>
<p>
   In practice, this approach is a bit cumbersome due to the sometimes large number of contributions to the
   systematic uncertainty. In addition, no infrastructure exists at present to deal with uncertainties that are
   not, or only partially, correlated from bin to bin. An eigenvector decomposition technique allows to address
   both of these issues at the same time.
</p>
<p>
   This method starts from the scale factor covariance matrix that can be constructed on the basis of the
   available information (uncertainties plus a model of their bin-to-bin correlations). This covariance matrix
   can be diagonalised and its eigenvectors and corresponding eigenvalues determined. The product of an eigenvector
   and the square root of its corresponding eigenvalue is what is referred to as an 'eigenvector variation'.
   The number of such (statistically independent) variations equals the number of calibration bins, and the set
   of variations is the minimal set needed to (re-)construct the covariance matrix. After this eigenvector
   decomposition, the eigenvector variations may be used instead of the variations corresponding to the original
   sources of uncertainty.
</p>
<p>
   One further refinement can be made, related to the fact that sometimes sources of uncertainty affecting
   the calibration analyses also affect the physics analysis in which the calibration results are used.
   Including these sources of uncertainty in the eigenvector decomposition would not allow to correlate the
   effect on the calibration properly with the effect on the physics analysis; therefore it is possible to
   exclude these sources of uncertainty from being used in the construction of the covariance matrix that is
   diagonalised, and consider their variations separately (note, per the above, that this will be done
   correctly only in the case of full bin-to-bin correlations).
</p>
<p>
   The use of the eigenvector variation is not enabled by default and must be switched on in the configuration
   file (see <a href="#configuration">Configuration</a>).
</p>
<p>
   The above can be used to obtain jet-by-jet information by a slight modification of the arguments to the
   normal methods for information retrieval also employed for <a href="#basic">basic usage</a>:
   <pre>
       getScaleFactor  (variables, label, OP, unc, numVariation)
   </pre>
   Here, the first three variables function exactly as in the basic usage case. As for the two last arguments:
   <pre>
       unc:          this should take the value <b>SFEigen</b> or <b>SFNamed</b> for eigenvector variations or named variations, respectively.
       numVariation: this indicates exactly which eigenvector variation or named variation is to be considered.
   </pre>
   In addition, it should be noticed that the methods' return values have a different meaning in this context:
   while in normal usage they are (value, uncertainty) pairs, here they are (up, down) pairs, with up and down
   meaning the result of an up- or downward eigenvector variation or named variation.
</p>
<p>
   The number of valid variations for each type can be retrieved using
   <pre>
       getNumVariations(author, label, OP, Uncertainty unc)
   </pre>
   with "unc" set appropriately. Finally, calling
   <pre>
       listScaleFactorUncertainties(author, label, OP, named)
   </pre>
   can be used to retrieve information about uncertainties in two ways, depending on the value of the last argument:
   <pre>
       named = false (default): retrieve an unsorted list of all the uncertainties associated with the relevant scale factor
                                calibration object. This can be used to identify sources of uncertainty to be excluded from
				the eigenvector decomposition.
       named = true:            the list in this case is restricted to the named uncertainties (these should have been specified
                                in the configuration file by the user) and ordered: the position in the vector constitutes the
				link between the name and the index to be used as the "numVariation" argument to the
				getScaleFactor() etc. method.
   </pre>
   Note that as in the basic usage case, the code can be sped up by replacing the jet author/label/OP specification by the appropriate
   integer index (see the <a href="#speed-up">corresponding section</a>).
</p>
<a name="configuration"><h2>Configuration</h2></a>
<p>
   The configuration file may specify the following settings:
</p>
<ul>
   <li>calibration file specification:
   <pre>

       filename:    common ROOT file for calibration scale factors and MC efficiencies
                    (default: BTaggingPerformanceCalibrations.root)
       filenameEff: separate ROOT file for MC efficiencies
       filenameSF:  separate ROOT file for calibration scale factors
   </pre>
   Note that it is not necessary to specify all three files. The most common use for the specification
   of more than one file is likely to be the specification of analysis-specific MC efficiencies (the
   efficiencies provided in the common file are expected to be sufficient for many analysis purposes,
   but depending on the accuracy needed it may be desirable to add custom efficiencies).
   </li>
   <li>choice of calibration:
   <pre>

       ScaleFactorCalibrationBName:  name for scale factor calibration object to be used for b-jets (default: default)
       EfficiencyCalibrationBName:   name for MC efficiencies object(s) to be used for b-jets (default: default).
                                     Note that multiple (semicolon-separated) MC efficiency objects may be specified;
				     internally they will be converted to a vector. The position in the vector can
				     then be used as the 'mapIndex' argument in the methods involving MC information
				     (see e.g. the <a href="#basic">basic usage</a> section).
   </pre>
   Analogous keywords (with B replaced with C, T, Light) can be used to specify the objects to be used for
   charm, tau, and light-flavour jets, respectively. Note that the "default" object should always exist and reflect
   the recommended choice of object unless analysis-specific needs dictate otherwise.
   </li>
   <li>jet aliasing:
   <pre>
       aliases:    semicolon-separated list of jet collection aliases
   </pre>
   The feature of jet aliasing follows the strategy also used in the CalibrationBroker Athena class.
   It allows for the use of a calibration for different jet collection names than those actually
   employed in physics analysis (caveat emptor!). Each alias consists of the specification
   <pre>
                   source-&gt;target
   </pre>
   where source indicates the name used in physics analysis, while target is the name to be used for calibration purposes.
   </li>
   <li>eigenvector decomposition:
   <pre>
       runEigenVectorMethod:  if set to true this will carry out an eigenvector decomposition, the results
                              of which can subsequently be used as calibration scale factor "variations"
			      correctly accounting for bin-to-bin correlations. For detailed usage of
			      b-tagging results (e.g. in profile likelihood fits) this is recommended (default: false)
       excludeFromCovMatrix:  semicolon-separated list of uncertainties to be excluded from the eigenvector
                              decomposition, for all flavours.
       excludeFromBCovMatrix: semicolon-separated list of uncertainties to be excluded from the eigenvector
                              decomposition for b jets
       excludeFromCCovMatrix: semicolon-separated list of uncertainties to be excluded from the eigenvector
                              decomposition for c jets
       excludeFromTCovMatrix: semicolon-separated list of uncertainties to be excluded from the eigenvector
                              decomposition for tau "jets"
       excludeFromLightCovMatrix: semicolon-separated list of uncertainties to be excluded from the eigenvector
                              decomposition for light-flavour jets
   </pre>
   </li>
</ul>
End_Html */
//
// CalibrationDataInterfaceROOT.cxx, (c) ATLAS Detector software                            //
//////////////////////////////////////////////////////////////////////////////////////////////

#include "CalibrationDataInterface/CalibrationDataInterfaceROOT.h"
#include "CalibrationDataInterface/CalibrationDataContainer.h"

#include "CalibrationDataInterface/CalibrationDataEigenVariations.h"

#include "TMath.h"
#include "TEnv.h"
#include "TFile.h"
#include "TObjString.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cassert>
#include <cstring>

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using Analysis::CalibrationDataContainer;
using Analysis::UncertaintyResult;
using Analysis::CalibrationDataEigenVariations;

#ifndef __CINT__
ClassImp(Analysis::CalibrationDataInterfaceROOT)
#endif

// local utility function: trim leading and trailing whitespace in the configuration .env file

namespace {
  std::string trim(const std::string& str,
		   const std::string& whitespace = " \t") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
  }
}

//________________________________________________________________________________
Analysis::CalibrationDataInterfaceROOT::CalibrationDataInterfaceROOT(const string& taggerName, string configname, string pathname)
:     m_runEigenVectorMethod(false), m_absEtaStrategy(GiveUp), m_otherStrategy(Flag)
{
  // Normal constructor.
  //
  //     taggerName:  this should correspond to the tagger name as used in the calibration ROOT file
  //     configname:  full name of the configuration file
  //     pathname:    directory specification for separate scale factor or efficiency ROOT file

  m_taggerName = taggerName;

  TEnv env(configname.c_str());

  // ROOT file containing the calibrations
  TString filename = env.GetValue("File", "BTaggingPerformanceCalibrations.root");
  m_filenameEff = trim(string(env.GetValue("FileEff", "")));
  m_filenameSF = trim(string(env.GetValue("FileSF", "")));
  if (m_filenameEff == "") {
    m_filenameEff = pathname + filename.Data();
  }
  if (m_filenameSF == "") {
    m_filenameSF = pathname + filename.Data();
  }

  cout << "=== CalibrationDataInterfaceROOT::CalibrationDataInterfaceROOT ===" << endl;
  cout << " Config name          : " << configname << endl;
  cout << " taggerName           : " << taggerName << endl;
  cout << " Efficiency file name : " << m_filenameEff << endl 
       << " SF         file name : " << m_filenameSF << endl;

  m_fileEff = TFile::Open(m_filenameEff.c_str(), "READ");
  if (m_filenameEff == m_filenameSF)
    m_fileSF = m_fileEff;
  else
    m_fileSF = TFile::Open(m_filenameSF.c_str(), "READ");

  TObjString* s;
  m_fileSF->GetObject("VersionInfo/BuildNumber", s);
  if (s) cout << " CDI file build number: " << s->GetName() << endl;
  cout << endl;

  std::string flavours[] = { "B", "C", "T", "Light" };
  string testPrefix(taggerName); testPrefix += ".";

  // Since TEnv doesn't allow for straight retrieval of vectors of strings, expect
  // semicolon-separated entries (semicolon because ROOT considers this as a "special"
  // token anyway in object names).
  string::size_type end;

  // Calibration names for the efficiencies
  std::map<string, std::vector<string> > effNames;
  for (auto const& flavour : flavours) {
    string test(testPrefix); test += "EfficiencyCalibration"; test += flavour; test += "Name";
    string calibrationName(trim(string(env.GetValue(test.c_str(), "default"))));
    std::vector<string> names;
    if (calibrationName.size() > 0) {
      do {
	end = calibrationName.find(";");
	names.push_back(trim(calibrationName.substr(0,end)));
	if (end != string::npos) calibrationName = calibrationName.substr(end+1);
      } while (end != string::npos);
    }
    effNames[flavour] = names;
  }
  setEffCalibrationNames(effNames);

  // Calibration names for the efficiency scale factors
  std::map<string, string> SFNames;
  for (auto const& flavour : flavours) {
    string test(testPrefix); test += "ScaleFactorCalibration"; test += flavour; test += "Name";
    string calibrationName(trim(string(env.GetValue(test.c_str(), "default"))));
    SFNames[flavour] = calibrationName;
  }
  setSFCalibrationNames(SFNames);

  // Since TEnv doesn't allow for straight retrieval of vectors of strings, expect
  // semicolon-separated entries (semicolon because ROOT considers this as a "special"
  // token anyway in object names).
  // Don't prefix this since the aliases are common to all taggers (even if they are read again for each tagger).
  string AL(env.GetValue("aliases", ""));
  if (AL.size() > 0) {
    do {
      end = AL.find(";");
      string alias = AL.substr(0, end);
      // Each alias specification uses an arrow ("->"). Forget about entries
      // not properly following this specification.
      // NB: TEnv imposes a maximum string length of 1024 characters -- is this a problem?
      string::size_type arrow = alias.find("->");
      if (arrow == string::npos) continue;
      m_aliases[trim(alias.substr(0,arrow))] = trim(alias.substr(arrow+2));
      if (end != string::npos) AL = AL.substr(end+1);
    } while (end != string::npos);
  }

  //run egenvector method or not?
  string test="runEigenVectorMethod";
  m_runEigenVectorMethod=(bool)env.GetValue(test.c_str(),0);

  if (m_runEigenVectorMethod) {
    // Retrieve the list of systematic uncertainties not to be considered when building up 
    // the full covariance matrix used for the eigenvector method.
    // We do this in two steps: first, for backward compatibility reasons, a flavour-independent list is scanned.
    // Second, flavour-specific lists are scanned.
    test = "excludeFromCovMatrix";
    string uncList(env.GetValue(test.c_str(), ""));
    string::size_type end2;
    std::vector<std::string> to_exclude;
    if (uncList.size() > 0) {
      do {
        end2 = uncList.find(";");
        to_exclude.push_back(trim(uncList.substr(0,end2)));
        if (end2 != string::npos) uncList = uncList.substr(end2+1);
      } while (end2 != string::npos);
    }
    // Copy the resulting list to all flavours
    for (auto const& flavour : flavours) {
      m_excludeFromCovMatrix[flavour] = to_exclude;
    }
    for (auto const& flavour : flavours) {
      to_exclude.clear();
      test = "excludeFrom"; test += flavour; test += "CovMatrix";
      uncList = env.GetValue(test.c_str(), "");
      if (uncList.size() > 0) {
	do {
	  end2 = uncList.find(";");
	  to_exclude.push_back(trim(uncList.substr(0,end2)));
	  if (end2 != string::npos) uncList = uncList.substr(end2+1);
	} while (end2 != string::npos);
      }
      // Append to the existing list
      m_excludeFromCovMatrix[flavour].insert(m_excludeFromCovMatrix[flavour].end(), to_exclude.begin(), to_exclude.end());
    }
    
    cout << " List of uncertainties to exclude:";
    unsigned int n_excluded = 0;
    for (auto const& flavour : flavours) {
      n_excluded += m_excludeFromCovMatrix[flavour].size();
    }
    if (n_excluded == 0) cout << " none";
    for (auto const& flavour : flavours) {
      if (m_excludeFromCovMatrix[flavour].size() > 0) {
	cout << "\n\t" << flavour << ":\t";
	for (unsigned int i = 0; i < m_excludeFromCovMatrix[flavour].size(); ++i) {
	  cout << m_excludeFromCovMatrix[flavour][i];
	  if (i+1 != m_excludeFromCovMatrix[flavour].size()) cout << ";  ";
	}
	cout << endl;
      }
    }
    cout << endl;

    // determine also the eigenvector reduction strategies
    std::map<string, EVReductionStrategy> mappings;
    mappings["Loose"] = Loose;
    mappings["Medium"] = Medium;
    mappings["Tight"] = Tight;
    for (auto const& flavour : flavours) {
      test = testPrefix; test += "EigenvectorReduction"; test += flavour;
      std::string reduction = trim(string(env.GetValue(test.c_str(), "Loose")));
      m_EVReductions[flavour] = mappings.find(reduction) == mappings.end() ? mappings["Loose"] : mappings.find(reduction)->second;
    }
  }

  // determine |eta| validity range
  m_maxAbsEta = env.GetValue("MaxAbsEta", 2.5);
  if (m_maxAbsEta < 0) m_maxAbsEta = 2.5;

  // set validation / protection strategy in case an out-of-bounds eta value is specified
  string strategy = trim(string(env.GetValue("OutOfBoundsEta", "GiveUp")));
  if (strategy == "GiveUp") m_absEtaStrategy = GiveUp;
  else if (strategy == "Flag") m_absEtaStrategy = Flag;
  else if (strategy == "Ignore") m_absEtaStrategy = Ignore;
  else {
    cerr << "unknown |eta| extrapolation strategy: " << strategy << ", setting to GiveUp" << endl;
    m_absEtaStrategy = GiveUp;
  }

  // set validation / protection strategy in case out-of-bounds variables are specified
  strategy = trim(string(env.GetValue("OutOfBoundsOther", "Flag")));
  if (strategy == "GiveUp") m_otherStrategy = GiveUp;
  else if (strategy == "GiveUpExtrapolated") m_otherStrategy = GiveUpExtrapolated;
  else if (strategy == "Flag") m_otherStrategy = Flag;
  else if (strategy == "Ignore") m_otherStrategy = Ignore;
  else {
    cerr << "unknown general extrapolation strategy: " << strategy << ", setting to Flag" << endl;
    m_otherStrategy = Flag;
  }

  // maximum tag weight to accept
  m_maxTagWeight = env.GetValue("MaxTagWeight", 10.0);

  // MC/MC (hadronisation) scale factors: making this user-steerable is intended to be *temporary* only
  m_useMCMCSF = (bool) env.GetValue("useMCMCSF", 1);
  // MC/MC (topology) scale factors: making this user-steerable is intended to be *temporary* only
  m_useTopologyRescaling = (bool) env.GetValue("useTopologySF", 0);

  cout << "======= end of CalibrationDataInterfaceROOT instantiation ========" << endl;
}


//________________________________________________________________________________
Analysis::CalibrationDataInterfaceROOT::CalibrationDataInterfaceROOT(const std::string& taggerName,
								     const char* fileSF, const char* fileEff,
								     const std::vector<std::string>& jetAliases,
								     const std::map<std::string, std::string>& SFNames,
								     const std::map<std::string, std::vector<std::string> >& EffNames,
								     const std::map<std::string, std::vector<std::string> >& excludeFromEV,
								     const std::map<std::string, EVReductionStrategy> EVReductions,
								     bool useEV, bool useMCMCSF, bool useTopologyRescaling) :
  m_filenameSF(fileSF), m_filenameEff(""),
  m_runEigenVectorMethod(useEV), m_EVReductions(EVReductions), m_useMCMCSF(useMCMCSF), m_useTopologyRescaling(useTopologyRescaling),
  m_maxAbsEta(2.5), m_absEtaStrategy(GiveUp),
  m_otherStrategy(Flag), m_maxTagWeight(10.0)
{
  // Normal constructor avoiding the need for a .env file.
  //
  //     taggerName:     this should correspond to the tagger name as used in the calibration ROOT file
  //     fileSF:         full path of the calibration ROOT file containing the calibration scale factors
  //     fileEff:        optional full path name of a ROOT file containing additional MC efficiency maps
  //                     (use a null pointer to disable the use of such additional file)
  //     jetAliases:     this can be used to convert jet collection names to the corresponding names in the
  //                     calibration ROOT file (this may be useful as e.g. the collection names in the
  //                     calibration ROOT file have the JVF criterion attached as a suffix).
  //                     Each alias is specified as
  //                                        nameOrig->nameTarget,
  //                     where nameOrig and nameTarget are the names of the input jet collection and the
  //                     jet collection name as used in the calibration ROOT file, respectively.
  //     SFNames:        map specifying for each of the calibration flavours ("B", "C", "T", "Light") the
  //                     name of the scale factor calibration object
  //     EffNames:       map specifying for each of the calibration flavours ("B", "C", "T", "Light") the
  //                     names of the possibly relevant efficiency calibration objects
  //     excludeFromEV:  map specifying for each of the calibration flavours ("B", "C", "T", "Light") the
  //                     systematic uncertainties to be excluded from the Eigenvector variation treatment
  //                     (this is used only if Eigenvector variations are used to begin with)
  //     EVReductions:   Eigenvector variation reduction strategies for "B", "C", "Light" jets (again,
  //                     this is only relevant if Eigenvector variations are used to begin with)
  //     useEV:          switch specifying if Eigenvector variations will be used or not
  //     useMCMCSF:      switch specifying if generator-dependent scale factors are to be applied or not

  // Note: at present, the means to change the strategies and maximum values initialized above do not exist
  // when using this constructor

  cout << "=== CalibrationDataInterfaceROOT::CalibrationDataInterfaceROOT ===" << endl;
  cout << " taggerName           : " << taggerName.c_str() << endl;
  if (fileEff) cout << " Efficiency file name : " << fileEff << endl;
  cout << " SF         file name : " << fileSF << endl
       << endl;

  m_taggerName = taggerName;

  m_fileSF = TFile::Open(fileSF, "READ");
  if (fileEff && strcmp(fileSF, fileEff) != 0) {
    m_filenameEff = string(fileEff);
    m_fileEff = TFile::Open(fileEff, "READ");
  } else
    m_fileEff = m_fileSF;

  TObjString* s;
  m_fileSF->GetObject("VersionInfo/BuildNumber", s);
  if (s) cout << " CDI file build number: " << s->GetName() << endl;
  cout << endl;

  for (unsigned int i = 0; i < jetAliases.size(); ++i) {
    // Each alias specification uses an arrow ("->"). Forget about entries
    // not properly following this specification.
    string::size_type arrow = jetAliases[i].find("->");
    if (arrow == string::npos) continue;
    m_aliases[jetAliases[i].substr(0,arrow)] = jetAliases[i].substr(arrow+2);
  }

  setEffCalibrationNames(EffNames);
  setSFCalibrationNames(SFNames);

  if (m_runEigenVectorMethod) {
    m_excludeFromCovMatrix = excludeFromEV;

    cout << " List of uncertainties to exclude:";
    std::string flavours[] = { "B", "C", "T", "Light" };
    unsigned int n_excluded = 0;
    for (auto const& flavour : flavours) {
      n_excluded += m_excludeFromCovMatrix[flavour].size();
    }
    if (n_excluded == 0) cout << " none";
    for (auto const& flavour : flavours) {
      if (m_excludeFromCovMatrix[flavour].size() > 0) {
	cout << "\n\t" << flavour << ":\t";
	for (unsigned int i = 0; i < m_excludeFromCovMatrix[flavour].size(); ++i) {
	  cout << m_excludeFromCovMatrix[flavour][i];
	  if (i+1 != m_excludeFromCovMatrix[flavour].size()) cout << ";  ";
	}
	cout << endl;
      }
    }
    cout << endl;
  }
  cout << "======= end of CalibrationDataInterfaceROOT instantiation ========" << endl;
}

//________________________________________________________________________________
Analysis::CalibrationDataInterfaceROOT::CalibrationDataInterfaceROOT()
{ 
  // Default constructor for PROOF purposes

  m_fileEff=0; 
  m_fileSF=0; 
} 

//________________________________________________________________________________
Analysis::CalibrationDataInterfaceROOT::CalibrationDataInterfaceROOT(const Analysis::CalibrationDataInterfaceROOT& other) :
  Analysis::CalibrationDataInterfaceBase(other), m_aliases(other.m_aliases), m_objects(), m_objectIndices(),
  m_filenameSF(other.m_filenameSF), m_filenameEff(other.m_filenameEff),
  m_eigenVariationsMap(), m_runEigenVectorMethod(other.m_runEigenVectorMethod),
  m_excludeFromCovMatrix(other.m_excludeFromCovMatrix), m_useMCMCSF(other.m_useMCMCSF), m_useTopologyRescaling(other.m_useTopologyRescaling),
  m_refMap(), m_hadronisationReference(),
  m_maxAbsEta(other.m_maxAbsEta), m_absEtaStrategy(other.m_absEtaStrategy), m_otherStrategy(other.m_otherStrategy),
  m_etaCounters(other.m_etaCounters), m_mainCounters(other.m_mainCounters), m_extrapolatedCounters(other.m_extrapolatedCounters),
  m_checkedWeightScaleFactors(other.m_checkedWeightScaleFactors), m_maxTagWeight(other.m_maxTagWeight)
{
  // Copy constructor. Note that the "cacheable" items aren't copied (they will be re-created if needed)

  // The TFile objects cannot be copied. Therefore, create duplicate objects starting from the filenames
  m_fileSF = TFile::Open(m_filenameSF.c_str(), "READ");
  if (m_filenameEff == m_filenameSF)
    m_fileSF = m_fileEff;
  else
    m_fileEff = TFile::Open(m_filenameEff.c_str(), "READ");
}

//________________________________________________________________________________
Analysis::CalibrationDataInterfaceROOT::~CalibrationDataInterfaceROOT()
{
  // Destructor

  if ((m_fileEff!=0) && (m_fileSF!=0)) { 
    if (m_fileEff == m_fileSF) { 
      m_fileEff->Close(); 
      delete m_fileEff; m_fileEff = 0; 
    } else { 
      m_fileEff->Close(); 
      m_fileSF->Close(); 
      delete m_fileEff; m_fileEff = 0; 
      delete m_fileSF;  m_fileSF = 0; 
    } 
  }

  // delete also the stored objects (these are owned by us)
  for (std::vector<CalibrationDataContainer*>::iterator it = m_objects.begin();
       it != m_objects.end(); ++it) {
    if (*it) {
      if (m_runEigenVectorMethod && m_eigenVariationsMap[*it]) { delete m_eigenVariationsMap[*it]; m_eigenVariationsMap[*it] = 0;}
      delete *it; *it = 0;
    }
  }

  // Print summary output on out-of-bounds issues
  if (m_absEtaStrategy == Flag) {
    bool found = false;
    cout << "\t\tCalibrationDataInterfaceROOT |eta| out-of-bounds summary:" << endl;
    for (unsigned int index = 0; index < m_mainCounters.size(); ++index)
      if (m_etaCounters[index] > 0) {
	found = true;
	cout << "\t\t\t" << nameFromIndex(index) << ": " << m_etaCounters[index] << endl;
      }
    if (!found) cout << "\t\t\tNo issues found" << endl;
  }
  if (m_otherStrategy == Flag) {
    bool found = false;
    cout << "\t\tCalibrationDataInterfaceROOT object out-of-bounds summary:" << endl;
    for (unsigned int index = 0; index < m_mainCounters.size(); ++index)
      if (m_mainCounters[index] + m_extrapolatedCounters[index] > 0) {
	found = true;
	cout << "\t\t\t" << nameFromIndex(index)
	     << " general: "       << m_mainCounters[index]
	     << ", extrapolated: " << m_extrapolatedCounters[index]
	     << endl;
      }
    if (!found) cout << "\t\t\tNo issues found" << endl;
  }
}

//________________________________________________________________________________
bool
Analysis::CalibrationDataInterfaceROOT::retrieveCalibrationIndex (const std::string& label,
								  const std::string& OP,
								  const std::string& author,
								  bool isSF, unsigned int& index,
								  unsigned int mapIndex) const
{
  // Retrieve the integer index corresponding to a given combination of 
  // flavour label / tagger / working point / jet collection name, and separately
  // for calibration scale factors and MC efficiencies (all these ingredients are needed
  // to specify fully the calibration object).
  // In fact this method will also trigger the retrieval of the object itself, if not already
  // done, and will cache it internally. The absence of the requested calibration object will
  // be flagged by a false return value.
  // This method is used internally but should also be called by users in order to exploit the
  // "code speed-up" features documented above.
  //
  //    label:     jet flavour label
  //    OP:        tagger working point
  //    author:    jet collection name
  //    isSF:      set to true (false) for scale factors (MC efficiencies)
  //    index:     resulting index (meaningful only for a 'true' function return value)
  //    mapIndex:  index to the MC efficiency map to be used

  index = 0;

  // construct the full name from the label, operating point, SF/Eff choice;
  // then look up this full name

  string name = fullName(author, OP, label, isSF, mapIndex);
  std::map<string, unsigned int>::const_iterator it = m_objectIndices.find(name);
  if (it == m_objectIndices.end()) {
    // If no container is found, attempt to retrieve it here (this is so that users won't
    // have to call the named scale factor etc. methods once just to retrieve the container).
    string flavour = (label == "N/A") ? "Light" : label;
    string dirname = m_taggerName + "/" + getAlias(author) + "/" + OP + "/" + flavour;
    string cntname = getContainername(flavour, isSF, mapIndex);
    const_cast<Analysis::CalibrationDataInterfaceROOT*>(this)->retrieveContainer(dirname, cntname, isSF);
    it = m_objectIndices.find(name);
    if (it == m_objectIndices.end()) return false;
  }

  index = it->second;
  return true;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getScaleFactor (const CalibrationDataVariables& variables,
							const string& label, const string& OP,
							Uncertainty unc, unsigned int numVariation,
							unsigned int mapIndex) const
{
  // Scale factor retrieval identifying the requested calibration object by name.
  // The return value is either a (value, uncertainty) or an (up, down) variation pair, as documented
  // above, and will be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    label:         jet flavour label
  //    OP:            tagger operating point
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    mapIndex:      index to the efficiency map to be used (this is needed for MC/MC scale factor
  //                   application)

  unsigned int indexEff, indexSF;
  if (! (retrieveCalibrationIndex (label, OP, variables.jetAuthor, false, indexEff, mapIndex) &&
	 retrieveCalibrationIndex (label, OP, variables.jetAuthor, true, indexSF))) {
    cerr << "getScaleFactor: unable to find SF calibration for object "
	 << fullName(variables.jetAuthor, OP, label, false, mapIndex)
	 << " or SF calibration for object "
	 << fullName(variables.jetAuthor, OP, label, true) << endl;
    // Return a dummy result if the object is not found
    return Analysis::dummyResult;
  }

  Analysis::CalibResult result;
  return (getScaleFactor(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getScaleFactor (const CalibrationDataVariables& variables,
							unsigned int indexSF, unsigned int indexEff,
							Uncertainty unc, unsigned int numVariation) const
{
  // Scale factor retrieval identifying the requested calibration object by index.
  // The return value is either a (value, uncertainty) or an (up, down) variation pair, as documented
  // above, and will be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to scale factor calibration object
  //    indexEff:      index to MC efficiency object
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)

  Analysis::CalibResult result;
  return (getScaleFactor(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibrationStatus
Analysis::CalibrationDataInterfaceROOT::getScaleFactor (const CalibrationDataVariables& variables,
							unsigned int indexSF, unsigned int indexEff,
							Uncertainty unc, unsigned int numVariation,
							Analysis::CalibResult& result) const
{
  // Scale factor retrieval identifying the requested calibration object by index.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to scale factor calibration object
  //    indexEff:      index to MC efficiency object
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    result:        (value, uncertainty) or (up, down) variation pair, depending on the unc value.
  //                   A dummy value will be returned in case of an error.

  CalibrationDataContainer* container = m_objects[indexSF];
  if (! container) {
    cerr << "getScaleFactor: error retrieving container!" << endl;
    return Analysis::kError;
  }

  // perform out-of-bound check of jet eta
  checkAbsEta(variables, indexSF);

  // retrieve the MC/MC scale factor
  double MCMCSF = m_useMCMCSF ? getMCMCScaleFactor(variables, indexSF, indexEff) : 1;

  if (!m_runEigenVectorMethod && (unc == SFEigen || unc == SFNamed))
  {
     cerr << " ERROR. Trying to call eigenvector method but initialization not switched on in b-tagging .env config file." << endl;
     cerr << " Please correct your .env config file first. Nominal uncertainties used. " << endl;
  }
  if (unc == SFEigen || unc == SFNamed) {
    const CalibrationDataEigenVariations* eigenVariation=m_eigenVariationsMap[container];
    if (! eigenVariation) {
      cerr << " Could not retrieve eigenvector variation, while it should have been there." << endl;
      return Analysis::kError;
    }
    unsigned int maxVariations = (unc == SFEigen) ? eigenVariation->getNumberOfEigenVariations() : eigenVariation->getNumberOfNamedVariations();
    if (numVariation > maxVariations-1) {
      cerr << "Asked for " << ((unc == SFEigen) ? "eigenvariation" : "named variation") << " number: " << numVariation << 
	" but overall number of available variations is: " << maxVariations << endl;
      return Analysis::kError;
    }
    TH1* up=0;
    TH1* down=0;
    bool isOK = (unc == SFEigen) ?
      eigenVariation->getEigenvectorVariation(numVariation,up,down) :
      eigenVariation->getNamedVariation(numVariation,up,down);
    if (!isOK) {
      cerr << "Eigenvector object is there but cannot retrieve up and down uncertainty histograms." << endl;
      return Analysis::kError;
    }
    // the 'extrapolation' uncertainty (always a named one) needs a somewhat special treatment
    bool extrapolate = (unc == SFNamed) ? eigenVariation->isExtrapolationVariation(numVariation) : false;
    
    double valueUp;
    double valueDown;
    Analysis::CalibrationStatus statUp   = container->getResult(variables, valueUp,  up,   extrapolate);
    Analysis::CalibrationStatus statDown = container->getResult(variables, valueDown,down, extrapolate);
    if (statUp == Analysis::kError || statDown == Analysis::kError)
      return Analysis::kError;
    if (m_otherStrategy == GiveUp)
      assert (statUp != Analysis::kRange); // no need to test also statDown
    else if (m_otherStrategy == GiveUpExtrapolated)
      assert (statUp != Analysis::kExtrapolatedRange); // no need to test also statDown
    else if (m_otherStrategy == Flag) {
      if (statUp == Analysis::kRange)
	const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF);
      else if (statUp == Analysis::kExtrapolatedRange)
	const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF, Extrapolated);
    }

    result.first  = MCMCSF*valueUp;
    result.second = MCMCSF*valueDown;
    // Prevent negative return values. Should the comparison be against a strict 0?
    result.first  = std::max(Analysis::CalibZERO, result.first);
    result.second = std::max(Analysis::CalibZERO, result.second);
    return statUp;
  } //end eigenvector method

  //Proceed with no-eigenvector result

  // always retrieve the result itself
  double value;
  Analysis::CalibrationStatus status = container->getResult(variables, value);
  if (status == Analysis::kError) {
    cerr << "getScaleFactor: error retrieving result in non-EV context!" << endl;
    return status;
  }
  if (m_otherStrategy == GiveUp)
    assert (status != Analysis::kRange);
  else if (m_otherStrategy == GiveUpExtrapolated)
    assert (status != Analysis::kExtrapolatedRange);
  else if (m_otherStrategy == Flag) {
    if (status == Analysis::kRange)
      const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF);
    else if (status == Analysis::kExtrapolatedRange)
      const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF, Extrapolated);
  }

  // retrieve the statistical uncertainty if desired
  double stat(0);
  if (unc == Total || unc == Statistical) {
    if (container->getStatUncertainty(variables, stat) == Analysis::kError) {
      cerr << "getScaleFactor: error retrieving Scale factor parameter covariance matrix!"
	   << endl;
      return Analysis::kError;
    }
  }
  Analysis::UncertaintyResult resSyst(0,0);
  if (unc == Total || unc == Systematic) {
    if (container->getSystUncertainty(variables, resSyst) == Analysis::kError) {
      cerr << "getScaleFactor: error retrieving Scale factor parameter systematic uncertainty!"
	   << endl;
      return Analysis::kError;
    }
  } else if (unc == Extrapolation) {
    // this uncertainty is special, since it is not normally to be combined into the overall systematic uncertainty
    if (container->getUncertainty("extrapolation", variables, resSyst) == Analysis::kError)
      cerr << "getScaleFactor: error retrieving Scale factor parameter extrapolation uncertainty!"
	   << endl;
  } else if (unc == TauExtrapolation) {
    // also this uncertainty is special, since it it singles out an uncertainty relevant only for tau "jets",
    // and some care has to be taken not to duplicate or omit uncertainties
    if (container->getUncertainty("extrapolation from charm", variables, resSyst) == Analysis::kError)
      cerr << "getScaleFactor: error retrieving Scale factor parameter extrapolation uncertainty!"
	   << endl;
  }

  double uncertainty = combinedUncertainty(stat, resSyst);
  result.first  = MCMCSF*value;
  result.second = MCMCSF*uncertainty;

  // Prevent negative return values. Should the comparison be against a strict 0?
  result.first = std::max(Analysis::CalibZERO, result.first);
  return status;

}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getMCEfficiency (const CalibrationDataVariables& variables,
							 const string& label, const string& OP,
							 Uncertainty unc, unsigned int mapIndex) const
{
  // MC efficiency retrieval identifying the requested calibration object by name.
  // The return value is a (value, uncertainty) pair, as documented above, and will
  // be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    label:         jet flavour label
  //    OP:            tagger operating point
  //    unc:           keyword indicating what uncertainties to evaluate
  //    mapIndex:      index to the efficiency map to be used

  unsigned int index;
  if (! retrieveCalibrationIndex (label, OP, variables.jetAuthor, false, index, mapIndex)) {
    cerr << "getMCEfficiency: unable to find Eff calibration for object "
	 << fullName(variables.jetAuthor, OP, label, false, mapIndex) << endl;
    // Return a dummy result if the object is not found
    return Analysis::dummyResult;
  }
  
  Analysis::CalibResult result;
  return (getMCEfficiency(variables, index, unc, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getMCEfficiency (const CalibrationDataVariables& variables,
							 unsigned int index, Uncertainty unc) const
{
  // MC efficiency retrieval identifying the requested calibration object by index.
  // The return value is a (value, uncertainty) pair, as documented above, and will
  // be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    index:         index to calibration object
  //    unc:           keyword indicating what uncertainties to evaluate

  Analysis::CalibResult result;
  return (getMCEfficiency(variables, index, unc, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibrationStatus
Analysis::CalibrationDataInterfaceROOT::getMCEfficiency (const CalibrationDataVariables& variables,
							 unsigned int index, Uncertainty unc,
							 Analysis::CalibResult& result) const
{
  // MC efficiency retrieval identifying the requested calibration object by index.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    index:         index to calibration object
  //    unc:           keyword indicating what uncertainties to evaluate
  //    result:        (value, uncertainty) variation pair.
  //                   A dummy value will be returned in case of an error.

  CalibrationDataContainer* container = m_objects[index];
  if (! container) return Analysis::kError;
  
  // perform out-of-bound check of jet eta
  checkAbsEta(variables, index);

  // always retrieve the result itself
  double value;
  Analysis::CalibrationStatus status = container->getResult(variables, value);
  if (status == Analysis::kError) return status;
  if (m_otherStrategy == GiveUp)
    assert (status != Analysis::kRange); // no need to test also statDown
  else if (m_otherStrategy == Flag)
    if (status == Analysis::kRange)
      const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(index);

  // retrieve the statistical uncertainty if desired
  double stat(0);
  if (unc == Total || unc == Statistical) {
    if (container->getStatUncertainty(variables, stat) == Analysis::kError) {
      cerr << "getMCEfficiency: error retrieving MC efficiency parameter covariance matrix!"
	   << endl;
    return Analysis::kError;
    }
  }

  // Temporary(?) hack: comment this out since the present MC results don't have "systematics" contributions
  // Analysis::UncertaintyResult resSyst(0,0);
  // if (unc == Total || unc == Systematic) {
  //   if (container->getSystUncertainty(variables, resSyst) == Analysis::kError)
  //     cerr << "getScaleFactor: error retrieving Scale factor parameter covariance matrix!"
  // 	   << endl;
  // }

  // since there is no combination of stat/syst uncertainties to be made, comment this out too
  double uncertainty = stat; // combinedUncertainty(stat, resSyst);
  result.first  = std::max(0., std::min(1., value));
  result.second = uncertainty;

  return status;
}

//====================== efficiency retrieval ==========================================

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getEfficiency (const CalibrationDataVariables& variables,
						       const string& label,
						       const string& OP, Uncertainty unc,
                                                       unsigned int numVariation, unsigned int mapIndex) const
{
  // Data efficiency retrieval identifying the requested calibration objects by name.
  // The data efficiency is computed as the product of MC efficiency and data/MC efficiency scale factor.
  // The return value is either a (value, uncertainty) or an (up, down) variation pair, as documented
  // above, and will be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    label:         jet flavour label
  //    OP:            tagger operating point
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    mapIndex:      index to the efficiency map to be used

  unsigned int indexSF, indexEff;
  if (! (retrieveCalibrationIndex (label, OP, variables.jetAuthor, false, indexEff, mapIndex) &&
	 retrieveCalibrationIndex (label, OP, variables.jetAuthor, true, indexSF))) {
    cerr << "getEfficiency: unable to find Eff calibration for object "
	 << fullName(variables.jetAuthor, OP, label, false, mapIndex)
	 << " or SF calibration for object "
	 << fullName(variables.jetAuthor, OP, label, true) << endl;
    // Return a dummy result if the object is not found
    return Analysis::dummyResult;
  }
  
  Analysis::CalibResult result;
  return (getEfficiency(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getEfficiency (const CalibrationDataVariables& variables,
						       unsigned int indexSF, unsigned int indexEff,
						       Uncertainty unc, unsigned int numVariation) const
{
  // Data efficiency retrieval identifying the requested calibration objects by index.
  // The data efficiency is computed as the product of MC efficiency and data/MC efficiency scale factor.
  // The return value is either a (value, uncertainty) or an (up, down) variation pair, as documented
  // above, and will be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to scale factor calibration object
  //    indexEff:      index to MC efficiency object
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)

  Analysis::CalibResult result;
  return (getEfficiency(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibrationStatus
Analysis::CalibrationDataInterfaceROOT::getEfficiency (const CalibrationDataVariables& variables,
						       unsigned int indexSF, unsigned int indexEff,
						       Uncertainty unc, unsigned int numVariation,
						       Analysis::CalibResult& result) const
{
  // Data efficiency retrieval identifying the requested calibration objects by index.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to scale factor calibration object
  //    indexEff:      index to MC efficiency object
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    result:        (value, uncertainty) or (up, down) variation pair, depending on the unc value.
  //                   A dummy value will be returned in case of an error.

  Analysis::CalibResult sfResult;
  Analysis::CalibrationStatus sfStatus = getScaleFactor(variables, indexSF, indexEff, unc, numVariation, sfResult);
  if (sfStatus == Analysis::kError) return sfStatus;
  Analysis::CalibResult effResult;
  Analysis::CalibrationStatus effStatus= getMCEfficiency(variables, indexEff, unc, effResult);
  if (effStatus == Analysis::kError) return effStatus;

  double relative = 0;
  double value = effResult.first;
  if (TMath::Abs(sfResult.first) > Analysis::CalibZERO) {
    value = std::min(effResult.first*sfResult.first, 1.);

    // Treat the scale factor variation cases separately since the contents of the CalibResult are different
    // (e.g. 'value' above contains the upward variation)
    if (unc == SFEigen || unc == SFNamed) {
      double valueDown = effResult.first*sfResult.second;
      result.first  = value;
      result.second = valueDown;
      return sfStatus;
    }
    if (value > 0.) {
      relative = effResult.second/effResult.first;
      double sfRelative = sfResult.second/sfResult.first;
      /*
	cout << "sferr=" << sfResult.second
	<< "btag Calib relative=" << relative << " sfRelative=" << sfRelative << endl;
      */
      relative = TMath::Sqrt(sfRelative*sfRelative + relative*relative);
    }
  } else {
    // now never happens due to protection of SF return value:
    cerr << "ERROR: CalibrationDataInterfaceROOT::getEfficiency: SF null result, SF=" << sfResult.first
	 << " MC eff=" << effResult.first 
	 << "; setting SF=1."
	 << endl;
    relative = Analysis::dummyValue;
  }

  result.first  = value;
  result.second = value*relative;
  // "Select" the status code for the actual calibration (it is subject to more constraints)
  return sfStatus;
}


//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getInefficiencyScaleFactor (const CalibrationDataVariables& variables,
								    const string& label,
								    const string& OP, Uncertainty unc,
								    unsigned int numVariation, unsigned int mapIndex) const
{
  // Inefficiency scale factor retrieval identifying the requested calibration objects by name.
  // The data efficiency is computed as the product of MC efficiency and data/MC efficiency scale factor;
  // the inefficiency scale factor is then computed as the ratio of data to MC inefficiencies.
  // The return value is either a (value, uncertainty) or an (up, down) variation pair, as documented
  // above, and will be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    label:         jet flavour label
  //    OP:            tagger operating point
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    mapIndex:      index to the efficiency map to be used

  unsigned int indexSF, indexEff;
  if (! (retrieveCalibrationIndex (label, OP, variables.jetAuthor, false, indexEff, mapIndex) &&
	 retrieveCalibrationIndex (label, OP, variables.jetAuthor, true, indexSF))) {
    cerr << "getInefficiencyScaleFactor: unable to find Eff calibration for object "
	 << fullName(variables.jetAuthor, OP, label, false, mapIndex)
	 << " or SF calibration for object "
	 << fullName(variables.jetAuthor, OP, label, true) << endl;
    // Return a dummy result if the object is not found
    return Analysis::dummyResult;
  }
  
  Analysis::CalibResult result;
  return (getInefficiencyScaleFactor(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getInefficiencyScaleFactor(const CalibrationDataVariables& variables,
								   unsigned int indexSF, unsigned int indexEff,
								   Uncertainty unc, unsigned int numVariation) const
{
  // Inefficiency scale factor retrieval identifying the requested calibration objects by index.
  // The data efficiency is computed as the product of MC efficiency and data/MC efficiency scale factor;
  // the inefficiency scale factor is then computed as the ratio of data to MC inefficiencies.
  // The return value is either a (value, uncertainty) or an (up, down) variation pair, as documented
  // above, and will be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to scale factor calibration object
  //    indexEff:      index to MC efficiency object
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)

  Analysis::CalibResult result;
  return (getInefficiencyScaleFactor(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibrationStatus
Analysis::CalibrationDataInterfaceROOT::getInefficiencyScaleFactor(const CalibrationDataVariables& variables,
								   unsigned int indexSF, unsigned int indexEff,
								   Uncertainty unc, unsigned int numVariation,
								   Analysis::CalibResult& result) const
{
  // Inefficiency scale factor retrieval identifying the requested calibration objects by index.
  // The data efficiency is computed as the product of MC efficiency and data/MC efficiency scale factor;
  // the inefficiency scale factor is then computed as the ratio of data to MC inefficiencies.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to scale factor calibration object
  //    indexEff:      index to MC efficiency object
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    result:        (value, uncertainty) or (up, down) variation pair, depending on the unc value.
  //                   A dummy value will be returned in case of an error.

  Analysis::CalibResult sfResult;
  Analysis::CalibrationStatus sfStatus = getScaleFactor(variables, indexSF, indexEff, unc, numVariation, sfResult);
  if (sfStatus == Analysis::kError) return sfStatus;
  Analysis::CalibResult effResult;
  Analysis::CalibrationStatus effStatus= getMCEfficiency(variables, indexEff, unc, effResult);
  if (effStatus == Analysis::kError) return effStatus;

  double eff = std::min(effResult.first, 1.);
  double efferr = effResult.second;
  double sf = sfResult.first; 
  double sferr = sfResult.second; 

  double val = 0.; // Analysis::dummyValue;
  double err = 0.; // Analysis::dummyValue;
  if (1. - eff > CalibZERO) {
    // Protect against negative scale factors
    val = std::max((1. - eff*sf), CalibZERO) / (1. - eff);
    // Treat the scale factor variation cases separately since the contents of the CalibResult are different
    // ('sf' and 'sferr' above contain the upward and downward variations, respectively).
    if (unc == SFEigen || unc == SFNamed) {
      double valDown = std::max((1. - eff*sferr), CalibZERO) / (1. - eff);
      result.first  = val;
      result.second = valDown;
      return sfStatus;
    }
    err = pow((1. - sf) / (1. - eff) * efferr, 2) + pow(eff*sferr, 2);
    if (err > 0.)
      err = 1./(1. - eff) * TMath::Sqrt(err);
    // cout << "btag Calib Ineff err=" << err << endl;
  }

  result.first  = std::max(CalibZERO, val);
  result.second = err;
  // "Select" the status code for the actual calibration (it is subject to more constraints)
  return sfStatus;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getInefficiency (const CalibrationDataVariables& variables,
							 const string& label,
							 const string& OP, Uncertainty unc,
							 unsigned int numVariation, unsigned int mapIndex) const
{
  // Data inefficiency retrieval identifying the requested calibration objects by name.
  // The data efficiency is computed as the product of MC efficiency and data/MC efficiency scale factor;
  // the inefficiency is then computed as the 1 minus the efficiency.
  // The return value is either a (value, uncertainty) or an (up, down) variation pair, as documented
  // above, and will be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    label:         jet flavour label
  //    OP:            tagger operating point
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    mapIndex:      index to the efficiency map to be used

  unsigned int indexSF, indexEff;
  if (! (retrieveCalibrationIndex (label, OP, variables.jetAuthor, false, indexEff, mapIndex) &&
	 retrieveCalibrationIndex (label, OP, variables.jetAuthor, true, indexSF))) {
    cerr << "getInefficiency: unable to find Eff calibration for object "
	 << fullName(variables.jetAuthor, OP, label, false, mapIndex)
	 << " or SF calibration for object "
	 << fullName(variables.jetAuthor, OP, label, true) << endl;
    // Return a dummy result if the object is not found
    return Analysis::dummyResult;
  }
  
  Analysis::CalibResult result;
  return (getInefficiency(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getInefficiency (const CalibrationDataVariables& variables,
							 unsigned int indexSF, unsigned int indexEff,
							 Uncertainty unc, unsigned int numVariation) const
{
  // Data inefficiency retrieval identifying the requested calibration objects by index.
  // The data efficiency is computed as the product of MC efficiency and data/MC efficiency scale factor;
  // the inefficiency is then computed as the 1 minus the efficiency.
  // The return value is either a (value, uncertainty) or an (up, down) variation pair, as documented
  // above, and will be a dummy value in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to scale factor calibration object
  //    indexEff:      index to MC efficiency object
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)

  Analysis::CalibResult result;
  return (getInefficiency(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibrationStatus
Analysis::CalibrationDataInterfaceROOT::getInefficiency (const CalibrationDataVariables& variables,
							 unsigned int indexSF, unsigned int indexEff,
							 Uncertainty unc, unsigned int numVariation,
							 Analysis::CalibResult& result) const
{
  // Data inefficiency retrieval identifying the requested calibration objects by index.
  // The data efficiency is computed as the product of MC efficiency and data/MC efficiency scale factor;
  // the inefficiency is then computed as the 1 minus the efficiency.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to scale factor calibration object
  //    indexEff:      index to MC efficiency object
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    result:        (value, uncertainty) or (up, down) variation pair, depending on the unc value.
  //                   A dummy value will be returned in case of an error.

  Analysis::CalibResult sfResult;
  Analysis::CalibrationStatus sfStatus = getScaleFactor(variables, indexSF, indexEff, unc, numVariation, sfResult);
  if (sfStatus == Analysis::kError) return sfStatus;
  Analysis::CalibResult effResult;
  Analysis::CalibrationStatus effStatus= getMCEfficiency(variables, indexEff, unc, effResult);
  if (effStatus == Analysis::kError) return effStatus;
  
  double val = std::max(0., 1. - effResult.first * sfResult.first);
  double err = 0.; // Analysis::dummyValue;

  // Bail out here if not both results are strictly positive
  if (effResult.first <= 0. || sfResult.first <= 0.) return Analysis::kError;

  // Treat the scale factor variation cases separately since the contents of the CalibResult are different
  // (e.g. 'val' above contains the upward variation)
  if (unc == SFEigen || unc == SFNamed) {
    double valDown = std::max(0., 1. - effResult.first*sfResult.second);

    result.first = val;
    result.second = valDown;
  } else {
    // safer than pow(x, 2):
    err = effResult.second/effResult.first*effResult.second/effResult.first
      + sfResult.second/sfResult.first*sfResult.second/sfResult.first;
    err = val*TMath::Sqrt(err);

    result.first  = std::max(0., std::min(1., val));
    result.second = err;
  }

  // "Select" the status code for the actual calibration (it is subject to more constraints)
  return sfStatus;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getMCInefficiency (const CalibrationDataVariables& variables,
							   const string& label, const string& OP,
							   Uncertainty unc, unsigned int mapIndex) const
{
  // Data inefficiency retrieval identifying the requested calibration objects by name.
  // The inefficiency is computed as the 1 minus the efficiency.
  // The return value is a (value, uncertainty), as documented above, and will be a dummy value
  // in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    label:         jet flavour label
  //    OP:            tagger operating point
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    mapIndex:      index to the efficiency map to be used

  Analysis::CalibResult effResult = getMCEfficiency(variables, label, OP, unc, mapIndex);
  return std::make_pair(std::max(0., 1. - effResult.first), effResult.second);
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getMCInefficiency (const CalibrationDataVariables& variables,
							   unsigned int index, Uncertainty unc) const
{
  // MC inefficiency retrieval identifying the requested calibration object by index.
  // The inefficiency is computed as the 1 minus the efficiency.
  // The return value is a (value, uncertainty), as documented above, and will be a dummy value
  // in case an error occurs.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    index:         index to MC efficiency object
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)

  Analysis::CalibResult effResult = getMCEfficiency(variables, index, unc);
  return std::make_pair(std::max(0., 1. - effResult.first), effResult.second);
}

//________________________________________________________________________________
double
Analysis::CalibrationDataInterfaceROOT::getMCMCScaleFactor(const CalibrationDataVariables& variables,
							   unsigned indexSF, unsigned int indexEff) const
{
  // Retrieve the MC/MC scale factor given the set of scale factor and efficiency indices.
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to scale factor calibration object
  //    indexEff:      index to MC efficiency object
  
  // If either reference doesn't exist, or if they are the same, nothing can / needs to be done.
  int indexSFRef = m_hadronisationReference[indexSF], indexEffRef = m_hadronisationReference[indexEff];
  if (indexSFRef < 0 || indexEffRef < 0 || indexSFRef == indexEffRef) return 1;

  // Verify also that the individual efficiencies are physically meaningful.
  double effSFRef;  m_objects[indexSFRef]->getResult(variables, effSFRef);
  double effEffRef; m_objects[indexEffRef]->getResult(variables, effEffRef);
  return (effSFRef > 0 && effEffRef > 0) ? effSFRef/effEffRef : 1;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getWeightScaleFactor (const CalibrationDataVariables& variables,
							      const string& label, Uncertainty unc,
							      unsigned int numVariation, unsigned int mapIndex) const
{
  // Tag weight fraction scale factor retrieval identifying the requested calibration object by name.
  // The return value is either a (value, uncertainty) or (if eigenvector or named variations are specified)
  // an (up, down) variation pair, and will be a dummy value in case an error occurs.
  // Note that in contrast to the "regular" (non-continuous) case, the computation of the scale factor in
  // general needs the (selection- or even process-specific) MC tag weight fractions, in order to rescale
  // scale factors. This is used to ensure that the tag weight fractions (both in data and in MC) sum up to
  // unity for each given kinematic bin.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    label:         jet flavour label
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    mapIndex:      index to the MC efficiency map to be used for scale factor rescaling

  static const string cont("Continuous");

  unsigned int indexSF, indexEff;
  if (! (retrieveCalibrationIndex (label, cont, variables.jetAuthor, false, indexEff, mapIndex) &&
	 retrieveCalibrationIndex (label, cont, variables.jetAuthor, true, indexSF))) {
    cerr << "getWeightScaleFactor: unable to find Eff calibration for object "
	 << fullName(variables.jetAuthor, cont, label, false, mapIndex)
	 << " or SF calibration for object "
	 << fullName(variables.jetAuthor, cont, label, true) << endl;
    return Analysis::dummyResult;
  }

  Analysis::CalibResult result;
  return (getWeightScaleFactor(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibResult
Analysis::CalibrationDataInterfaceROOT::getWeightScaleFactor (const CalibrationDataVariables& variables,
							      unsigned int indexSF, unsigned int indexEff,
							      Uncertainty unc, unsigned int numVariation) const
{
  // Tag weight fraction scale factor retrieval identifying the requested calibration object by index.
  // The return value is either a (value, uncertainty) or (if eigenvector or named variations are specified)
  // an (up, down) variation pair, and will be a dummy value in case an error occurs.
  // Note that in contrast to the "regular" (non-continuous) case, the computation of the scale factor in
  // general needs the (selection- or even process-specific) MC tag weight fractions, in order to rescale
  // scale factors. This is used to ensure that the tag weight fractions (both in data and in MC) sum up to
  // unity for each given kinematic bin.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to calibration object
  //    indexEff:      index to MC tag weight
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)

  Analysis::CalibResult result;
  return (getWeightScaleFactor(variables, indexSF, indexEff, unc, numVariation, result) == Analysis::kError) ?
    Analysis::dummyResult : result;
}

//________________________________________________________________________________
Analysis::CalibrationStatus
Analysis::CalibrationDataInterfaceROOT::getWeightScaleFactor (const CalibrationDataVariables& variables,
							      unsigned int indexSF, unsigned int indexEff,
							      Uncertainty unc, unsigned int numVariation,
							      Analysis::CalibResult& result) const
{
  // Tag weight fraction scale factor retrieval identifying the requested calibration object by index.
  // Note that in contrast to the "regular" (non-continuous) case, the computation of the scale factor in
  // general needs the (selection- or even process-specific) MC tag weight fractions, in order to rescale
  // scale factors. This is used to ensure that the tag weight fractions (both in data and in MC) sum up to
  // unity for each given kinematic bin.
  //
  //    variables:     object holding kinematic (and other) information needed to compute the result
  //    indexSF:       index to calibration object
  //    indexEff:      index to MC tag weight
  //    unc:           keyword indicating what uncertainties to evaluate (or whether eigenvector or
  //                   named variations are to be computed)
  //    numVariation:  variation index (in case of eigenvector or named variations)
  //    result:        (value, uncertainty) or (up, down) variation pair, depending on the unc value.
  //                   A dummy value will be returned in case of an error.

  CalibrationDataContainer* container = m_objects[indexSF];
  if (! container) return Analysis::kError;
  CalibrationDataContainer* effContainer = m_objects[indexEff];
  if (! effContainer) return Analysis::kError;

  // the first time this combination of scale factor and "efficiency" objects is given, check on the
  // scale factors that will result from their combination (where the computations reproduce those
  // shown below)
  checkWeightScaleFactors(indexSF, indexEff);

  // perform out-of-bound check of jet eta
  checkAbsEta(variables, indexSF);

  // Always retrieve the result itself 
  double value;
  Analysis::CalibrationStatus status = container->getResult(variables, value);
  if (status == Analysis::kError) return status;
  if (m_otherStrategy == GiveUp) assert (status != Analysis::kRange);
  else if (m_otherStrategy == GiveUpExtrapolated) assert (status != Analysis::kExtrapolatedRange);
  else if (m_otherStrategy == Flag) {
    if (status == Analysis::kRange)
      const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF);
    else if (status == Analysis::kExtrapolatedRange)
      const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF, Extrapolated);
  }

  // Retrieve the reference MC tag weight fraction (corresponding to the calibration scale factors)
  Analysis::UncertaintyResult refMCResult(0,0);
  if (container->getUncertainty("MCreference", variables, refMCResult) == Analysis::kError)
    return Analysis::kError;
  double fracMCref = refMCResult.first;

  // Retrieve the MC reference information, if requested (the initialisation below is to make sure
  // that no exceptions in the code will be needed)
  double fracSFref = fracMCref, fracEffref = fracMCref;
  if (m_useMCMCSF) {
    int indexSFref = m_hadronisationReference[indexSF], indexEffref = m_hadronisationReference[indexEff];
    if (indexSFref < 0 || indexEffref < 0) {
      cerr << "getWeightScaleFactor: error: generator-specific corrections requested but necessary reference containers lacking " << endl;
      return Analysis::kError;
    } else {
      m_objects[indexSFref]->getResult(variables, fracSFref);
      m_objects[indexEffref]->getResult(variables, fracEffref);
      if (! (fracSFref > 0. && fracEffref > 0.)) {
	cerr << "getWeightScaleFactor: error: invalid reference tag weight fraction" << endl;
	return Analysis::kError;
      }
    }
  }

  // Retrieve the MC tag weight fraction for the sample we need to reweight to
  double fracMCnew;
  Analysis::CalibrationStatus effStatus = effContainer->getResult(variables, fracMCnew);
  if (effStatus == Analysis::kError) return effStatus;
  if (m_otherStrategy == GiveUp) assert (effStatus != Analysis::kRange);
  else if (m_otherStrategy == Flag)
    if (effStatus == Analysis::kRange) const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexEff);
  // since we need to divide by this quantity, check that it is well-defined
  if (!(fracMCnew > 0.)) {
    cerr << "getWeightScaleFactor: error: null fracMCnew would lead to invalid operation" << endl;
    return Analysis::kError;
  }

  if (!m_runEigenVectorMethod && (unc == SFEigen || unc == SFNamed)) {
     cerr << "getWeightScaleFactor: ERROR. Trying to call eigenvector method but initialization not switched on in b-tagging .env config file." << endl;
     cerr << "                             Please correct your .env config file first. Nominal uncertainties used. " << endl;
  }
  if (unc == SFEigen || unc == SFNamed) {
    const CalibrationDataEigenVariations* eigenVariation=m_eigenVariationsMap[container];
    if (! eigenVariation) {
      cerr << "getWeightScaleFactor: could not retrieve eigenvector variation, while it should have been there." << endl;
      return Analysis::kError;
    }
    unsigned int maxVariations = (unc == SFEigen) ? eigenVariation->getNumberOfEigenVariations() : eigenVariation->getNumberOfNamedVariations();
    if (numVariation > maxVariations-1) {
      cerr << "getWeightScaleFactor: asked for " << ((unc == SFEigen) ? "eigenvariation" : "named variation") << " number: " << numVariation << 
	" but overall number of available variations is: " << maxVariations << endl;
      return Analysis::kError;
    }
    TH1* up=0;
    TH1* down=0;
    bool isOK = (unc == SFEigen) ?
      eigenVariation->getEigenvectorVariation(numVariation,up,down) :
      eigenVariation->getNamedVariation(numVariation,up,down);
    if (!isOK) {
      cerr << "getWeightScaleFactor: Eigenvector object is there but cannot retrieve up and down uncertainty histograms." << endl;
      return Analysis::kError;
    }
    // the 'extrapolation' uncertainty (always a named one) needs a somewhat special treatment
    bool extrapolate = SFNamed ? eigenVariation->isExtrapolationVariation(numVariation) : false;
    
    double valueUp;
    double valueDown;
    Analysis::CalibrationStatus statusUp   = container->getResult(variables, valueUp,  up,   extrapolate);
    Analysis::CalibrationStatus statusDown = container->getResult(variables, valueDown,down, extrapolate);
    if (statusUp == Analysis::kError || statusDown == Analysis::kError) return Analysis::kError;

    // now carry out the rescaling. Protect against unphysical or suspiciously large scale factors
    double variationUp = valueUp - value;
    double variationDown = valueDown - value;
    // First step: from the calibration sample to its reference sample
    if (m_useTopologyRescaling) value = 1.0 + (value - 1.0) * (fracMCref / fracSFref);
    // Second step: from the calibration reference sample to the MC object's reference sample
    if (m_useMCMCSF) value *= (fracSFref / fracEffref);
    // Third step: from the MC object's reference sample to the MC sample itself
    if (m_useTopologyRescaling) value = 1.0 + (value - 1.0) * (fracEffref / fracMCnew);
    // Since all transformations of the scale factor itself are linear, the transformation of the variations is simpler.
    if (m_useTopologyRescaling) {
      double f = (fracMCref / fracMCnew);
      variationUp   *= f;
      variationDown *= f;
    } else if (m_useMCMCSF) {
      double f = (fracSFref / fracEffref);
      variationUp   *= f;
      variationDown *= f;
    }
    valueUp   = value + variationUp;
    valueDown = value + variationDown;
    if (valueUp < 0) {
      valueUp = 0; const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF, TagWeight);
    } else if (valueUp > m_maxTagWeight) {
      valueUp = m_maxTagWeight; const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF, TagWeight);
    }
    if (valueDown < 0) {
      valueDown = 0; const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF, TagWeight);
    } else if (valueDown > m_maxTagWeight) {
      valueDown = m_maxTagWeight; const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF, TagWeight);
    }

    result.first  = valueUp;
    result.second = valueDown;
    return statusUp;
  } //end eigenvector method

  //Proceed with no-eigenvector result
    
  // retrieve the statistical uncertainty if desired
  double stat(0);
  if (unc == Total || unc == Statistical) {
    if (container->getStatUncertainty(variables, stat) == Analysis::kError) {
      cerr << "getWeightScaleFactor: error retrieving Scale factor parameter covariance matrix!"
	   << endl;
      return Analysis::kError;
    }
  }
  Analysis::UncertaintyResult uncertaintyResult(0,0);
  if (unc == Total || unc == Systematic) {
    if (container->getSystUncertainty(variables, uncertaintyResult) == Analysis::kError) {
      cerr << "getWeightScaleFactor: error retrieving Scale factor parameter systematic uncertainty!"
           << endl;
      return Analysis::kError;
    }
  } else if (unc == Extrapolation) {
    // this uncertainty is special, since it is not normally to be combined into the overall systematic uncertainty
    if (container->getUncertainty("extrapolation", variables, uncertaintyResult) == Analysis::kError)
      cerr << "getWeightScaleFactor: error retrieving Scale factor parameter extrapolation uncertainty!" << endl;
  } else if (unc == TauExtrapolation) {
    // also this uncertainty is special, since it it singles out an uncertainty relevant only for tau "jets",
    // and some care has to be taken not to duplicate or omit uncertainties
    if (container->getUncertainty("extrapolation from charm", variables, uncertaintyResult) == Analysis::kError)
      cerr << "getWeightScaleFactor: error retrieving Scale factor parameter extrapolation uncertainty!" << endl;
  }  

  double uncertainty = combinedUncertainty(stat, uncertaintyResult);

  // Now carry out the rescaling. Again protect against unphysical or suspiciously large scale factors
  // First step: from the calibration sample to its reference sample
  if (m_useTopologyRescaling) value = 1.0 + (value - 1.0) * (fracMCref / fracSFref);
  // Second step: from the calibration reference sample to the MC object's reference sample
  if (m_useMCMCSF) value *= (fracSFref / fracEffref);
  // Third step: from the MC object's reference sample to the MC sample itself
  if (m_useTopologyRescaling) value = 1.0 + (value - 1.0) * (fracEffref / fracMCnew);
  if (value < 0) {
    value = 0; const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF, TagWeight);
  } else if (value > m_maxTagWeight) {
    value = m_maxTagWeight; const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(indexSF, TagWeight);
  }
  // Since all transformations of the scale factor itself are linear, the transformation of the uncertainty is simpler.
  if (m_useTopologyRescaling) {
    uncertainty *= (fracMCref / fracMCnew);
  } else if (m_useMCMCSF) {
    uncertainty *= (fracSFref / fracEffref);
  }

  result.first  = std::max(0., value);
  result.second = uncertainty;
  // "Select" the status code for the actual calibration object (it is subject to more constraints)
  return status;
}

//________________________________________________________________________________
void
Analysis::CalibrationDataInterfaceROOT::checkWeightScaleFactors(unsigned int indexSF,
								unsigned int indexEff) const
{
  // Check the tag weight scale factors that would result from the combination of
  // the provided scale factor and MC tag weight objects.
  // The way this is done is by determining the binning that would apply to the
  // combination of the two individual inputs, and then by explicitly computing
  // the scale factors in each of these resulting bins.

  std::vector<std::pair<unsigned int, unsigned int> >::const_iterator it =
    std::find(m_checkedWeightScaleFactors.begin(), m_checkedWeightScaleFactors.end(),
	      std::make_pair(indexSF, indexEff));
  if (it != m_checkedWeightScaleFactors.end()) return;

  // Assume that only histogram containers are involved here (this should be the case
  // as at least a strict tag weight binning should be applied).
  const CalibrationDataHistogramContainer* container = dynamic_cast<const CalibrationDataHistogramContainer*>(m_objects[indexSF]);
  if (! container) {
    cerr << "CalibrationDataInterfaceROOT::checkWeightScaleFactors: error: container for object "
	 << nameFromIndex(indexSF) << " not found!" << endl;
    return;
  } else if (! container->GetValue("MCreference")) {
    cerr << "CalibrationDataInterfaceROOT::checkWeightScaleFactors: error: no MCreference histogram for object "
	 << nameFromIndex(indexSF) << "!" << endl;
    return;
  }
  const CalibrationDataHistogramContainer* effContainer = dynamic_cast<const CalibrationDataHistogramContainer*>(m_objects[indexEff]);
  if (! effContainer) {
    cerr << "CalibrationDataInterfaceROOT::checkWeightScaleFactors: error: container for object "
	 << nameFromIndex(indexEff) << " not found!" << endl;
    return;
  }

  // Retrieve the variable types and corresponding bin boundaries
  std::vector<unsigned int> vars    = container->getVariableTypes();
  std::vector<unsigned int> effVars = effContainer->getVariableTypes();
  // Retrieve the corresponding bin boundaries
  std::map<unsigned int, std::vector<double> > boundaries, effBoundaries, mergedBoundaries;
  for (unsigned int t = 0; t < vars.size(); ++t)
    boundaries[vars[t]] = container->getBinBoundaries(vars[t]);
  for (unsigned int t = 0; t < effVars.size(); ++t)
    effBoundaries[effVars[t]] = effContainer->getBinBoundaries(effVars[t]);

  // Special case: handle |eta| versus eta differences, by transforming to the latter
  if (boundaries.find(CalibrationDataContainer::kEta) == boundaries.end() &&
      boundaries.find(CalibrationDataContainer::kAbsEta) != boundaries.end()) {
    boundaries[CalibrationDataContainer::kEta] = boundaries[CalibrationDataContainer::kAbsEta];
    boundaries.erase(CalibrationDataContainer::kAbsEta);
  }
  if (effBoundaries.find(CalibrationDataContainer::kEta) == effBoundaries.end() &&
      effBoundaries.find(CalibrationDataContainer::kAbsEta) != effBoundaries.end()) {
    effBoundaries[CalibrationDataContainer::kEta] = effBoundaries[CalibrationDataContainer::kAbsEta];
    effBoundaries.erase(CalibrationDataContainer::kAbsEta);
  }
  if (boundaries.find(CalibrationDataContainer::kEta) != boundaries.end() &&
      effBoundaries.find(CalibrationDataContainer::kEta) != effBoundaries.end()) {
    std::vector<double>& v = boundaries[CalibrationDataContainer::kEta];
    std::vector<double>& vEff = effBoundaries[CalibrationDataContainer::kEta];
    if (v[0] < 0 && vEff[0] >= 0) {
      // in this case, supplement the positive entries in vEff with their negative analogues
      std::vector<double> vtmp(vEff);
      for (std::vector<double>::iterator it = vtmp.begin(); it != vtmp.end(); ++it)
	if (*it > 0) vEff.insert(vEff.begin(), -(*it));
    } else if (v[0] >= 0 && vEff[0] < 0) {
      // in this case, supplement the positive entries in v with their negative analogues
      std::vector<double> vtmp(v);
      for (std::vector<double>::iterator it = vtmp.begin(); it != vtmp.end(); ++it)
	if (*it > 0) v.insert(v.begin(), -(*it));
    }
  }

  // Now that the individual sets of boundaries have been determined, merge these
  for (unsigned int t = 0; t < vars.size(); ++t) {
    if (effBoundaries.find(vars[t]) == effBoundaries.end())
      // Variables not present in the efficiency object can go in unmodified
      mergedBoundaries[vars[t]] = boundaries[vars[t]];
    else {
      // Merge the boundaries for variables existing in both objects.
      // Take the MC array as a starting point, as it's likely to be the longest.
      mergedBoundaries[vars[t]] = effBoundaries[vars[t]];
      
      for (std::vector<double>::iterator it = boundaries[vars[t]].begin();
	   it != boundaries[vars[t]].end(); ++it) {
	std::vector<double>::iterator itcmp = mergedBoundaries[vars[t]].begin();
	// Iterate until we've found a value in the target array equal to
	// or larger than the given element
	while ((! CalibrationDataContainer::isNearlyEqual(*itcmp, *it)) &&
	       *itcmp < *it && itcmp != mergedBoundaries[vars[t]].end()) ++itcmp;
	// Nothing needs to be done if the values are "nearly identical"
	if (CalibrationDataContainer::isNearlyEqual(*itcmp, *it)) continue;
	// Otherwise insert the given element (this can mean adding to the end)
	mergedBoundaries[vars[t]].insert(itcmp, *it);
      }
    }
  }
  // Variables not present in the scale factor object still need to go in
  for (unsigned int t = 0; t < effVars.size(); ++t)
    if (boundaries.find(effVars[t]) == boundaries.end())
      mergedBoundaries[effVars[t]] = effBoundaries[effVars[t]];
  // Carry out a rudimentary cross-check of the tag weight bin
  // (the binning used for the scale factor and MC objects should be identical).
  if (boundaries.find(CalibrationDataContainer::kTagWeight) == boundaries.end()) {
    cerr << "CalibrationDataInterfaceROOT::checkWeightScaleFactors: "
	 << "no tag weight axis found for object " << nameFromIndex(indexSF) << endl;
  } else if (effBoundaries.find(CalibrationDataContainer::kTagWeight) == effBoundaries.end()) {
    cerr << "CalibrationDataInterfaceROOT::checkWeightScaleFactors: "
	 << "no tag weight axis found for object " << nameFromIndex(indexEff) << endl;
  } else if (boundaries[CalibrationDataContainer::kTagWeight].size() !=
  	     effBoundaries[CalibrationDataContainer::kTagWeight].size()) {
    cerr << "CalibrationDataInterfaceROOT::checkWeightScaleFactors: "
  	 << "different tag weight binning for objects " << nameFromIndex(indexSF) << " (";
    std::vector<double>& v = boundaries[CalibrationDataContainer::kTagWeight];
    for (unsigned int ib = 0; ib < v.size()-1; ++ib) cerr << v[ib] << ",";
    cerr << v[v.size()-1] << ") and " << nameFromIndex(indexEff) << " (";
    v = effBoundaries[CalibrationDataContainer::kTagWeight];
    for (unsigned int ib = 0; ib < v.size()-1; ++ib) cerr << v[ib] << ",";
    cerr << v[v.size()-1] << ") do not match!" << endl;
  } else {
    // Make sure that (possibly) dummy vectors exist for _all_ known variables
    // (this is a mere technicality allowing to loop over all variables explicitly).
    if (mergedBoundaries.find(CalibrationDataContainer::kPt) == mergedBoundaries.end()) {
      std::vector<double> v; v.push_back(20.); v.push_back(300.); mergedBoundaries[CalibrationDataContainer::kPt] = v;
    }
    if (mergedBoundaries.find(CalibrationDataContainer::kEta) == mergedBoundaries.end()) {
      std::vector<double> v; v.push_back(-2.5); v.push_back(2.5); mergedBoundaries[CalibrationDataContainer::kEta] = v;
    }
    // Finally, carry out the cross-check that all this is about: recompute the scale factor
    // in each pseudo-bin
    cout << "CalibrationDataInterfaceROOT::checkWeightScaleFactors: cross-checking scale factors for objects "
	 << nameFromIndex(indexSF) << " and " << nameFromIndex(indexEff) << "\n"
	 << std::setfill('-') << std::setw(100) << "-" << endl;
    cout << std::setfill(' ');
    CalibrationDataVariables x;
    std::vector<double>& vPt = mergedBoundaries[CalibrationDataContainer::kPt],
      vEta = mergedBoundaries[CalibrationDataContainer::kEta],
      vTagWeight = mergedBoundaries[CalibrationDataContainer::kTagWeight];
    for (unsigned int ipt = 0; ipt < vPt.size()-1; ++ipt) {
      x.jetPt = (vPt[ipt] + vPt[ipt+1]) * 500.; // account for MeV -> GeV conversion
      for (unsigned int ieta = 0; ieta < vEta.size()-1; ++ieta) {
	x.jetEta = (vEta[ieta] + vEta[ieta+1]) / 2.;
	for (unsigned int iwt = 0; iwt < vTagWeight.size()-1; ++iwt) {
	  x.jetTagWeight = (vTagWeight[iwt] + vTagWeight[iwt+1]) / 2.;
	  // Retrieve the central scale factor value and the old and new MC tag weight fractions
	  double value;
	  container->getResult(x, value);
	  Analysis::UncertaintyResult uncertaintyResult(0,0);
	  container->getUncertainty("MCreference", x, uncertaintyResult);
	  double fracMCref = uncertaintyResult.first;
	  double fracMCnew;
	  effContainer->getResult(x, fracMCnew);
	  // Compute the new scale factor value
	  if (!(fracMCnew > 0.)) {
	    cout << "\tfor (pt=" << x.jetPt << ",eta=" << x.jetEta << ",tagweight=" << x.jetTagWeight
		 << "): invalid new MC fraction: " << fracMCnew << endl;
	  } else {
	    double newvalue = 1.0 + (value - 1.0) * fracMCref/fracMCnew;
	    if (newvalue <= 0 || newvalue > m_maxTagWeight)
	      cout << "\tfor (pt=" << x.jetPt << ",eta=" << x.jetEta << ",tagweight=" << x.jetTagWeight
		   << "): old (value=" << value << ",MC=" << fracMCref << "), new (value="
		   << newvalue << ",MC=" << fracMCnew << ")" << endl;
	  }
	}
      }
    }
  }

  m_checkedWeightScaleFactors.push_back(std::make_pair(indexSF, indexEff));
}

//________________________________________________________________________________
void
Analysis::CalibrationDataInterfaceROOT::checkAbsEta(const CalibrationDataVariables& variables,
						    unsigned int index) const
{
  // Check whether the jet eta value is outside the range of validity, subject to the strategy
  // specified in the configuration file.

  if (m_absEtaStrategy == Ignore) return;
  switch (m_absEtaStrategy) {
  case GiveUp:
    assert(!(std::fabs(variables.jetEta) > m_maxAbsEta)); break;
  case Flag:
  default:
    if (std::fabs(variables.jetEta) > m_maxAbsEta)
      const_cast<CalibrationDataInterfaceROOT*>(this)->increaseCounter(index, Eta);
  }
}

//________________________________________________________________________________
std::string
Analysis::CalibrationDataInterfaceROOT::nameFromIndex(unsigned int index) const
{
  // Return the object name corresponding to the given index.

  for (std::map<std::string, unsigned int>::const_iterator it = m_objectIndices.begin();
       it != m_objectIndices.end(); ++it)
    if (it->second == index) return it->first;

  // This should never happen..
  return string("");
}

//________________________________________________________________________________
void
Analysis::CalibrationDataInterfaceROOT::increaseCounter(unsigned int index,
							OutOfBoundsType oob)
{
  // Internal method bumping the relevant counter out-of-bounds counter for the specified object.
  //
  //   oob:    further classification of out-of-bounds case
  //   index:  object index

  // make sure the vectors are appropriately dimensioned
  if (index >= m_mainCounters.size()) {
    unsigned int minsize = (index == 0) ? 2 : 2*index;
    m_mainCounters.resize(minsize, 0);
    m_etaCounters.resize(minsize, 0);
    m_extrapolatedCounters.resize(minsize, 0);
  }
  switch (oob) {
  case Main:
    m_mainCounters[index]++; break;
  case Eta:
    m_etaCounters[index]++; break;
  case Extrapolated:
  default:
    m_extrapolatedCounters[index]++;
  }
}

//________________________________________________________________________________
std::vector<string>
Analysis::CalibrationDataInterfaceROOT::listScaleFactorUncertainties(const string& author,
								     const string& label,
								     const string& OP,
								     bool named) const
{
  // Retrieve the sources of uncertainty relevant for the given scale factor calibration object,
  // identifying the object by name.
  //
  //    author:  jet collection name
  //    label:   jet flavour label
  //    OP:      tagger working point
  //    named:   if false, an unsorted list of sources of uncertainties will be returned.
  //             if true, only 'named' uncertainties will be returned, and the position in
  //             the vector that is the return value determines the 'numVariation' index
  //             that is to be used if named variations are to be retrieved.

  unsigned int index;
  if (! retrieveCalibrationIndex (label, OP, author, true, index)) {
    // Return a dummy result if the object is not found
    cerr << "listScaleFactorUncertainties: unable to find SF calibration for object "
	 << fullName(author, OP, label, true) << endl;
    std::vector<string> dummy;
    return dummy;
  }
  return listScaleFactorUncertainties(index, named);
}

//________________________________________________________________________________
std::vector<string>
Analysis::CalibrationDataInterfaceROOT::listScaleFactorUncertainties(unsigned int index,
								     bool named) const
{
  // Retrieve the sources of uncertainty relevant for the given scale factor calibration object,
  // identifying the object by index.
  //
  //    index:   index to scale factor calibration object
  //    named:   if false, an unsorted list of sources of uncertainties will be returned.
  //             if true, only 'named' uncertainties will be returned, and the position in
  //             the vector that is the return value determines the 'numVariation' index
  //             that is to be used if named variations are to be retrieved.

  std::vector<string> dummy;
  CalibrationDataContainer* container = m_objects[index];
  if (container) {
    if (named) {
      // Find out which uncertainties are excluded from eigenvector construction
      if (! m_runEigenVectorMethod) return dummy;
      const CalibrationDataEigenVariations* eigenVariation=m_eigenVariationsMap[container];
      std::vector<string> unordered = eigenVariation->listNamedVariations();
      std::vector<string> ordered(unordered.size());
      for (unsigned int i = 0; i < unordered.size(); ++i)
	ordered[eigenVariation->getNamedVariationIndex(unordered[i])] = unordered[i];
      return ordered;
    }
    return container->listUncertainties();
  }

  return dummy;
}

//________________________________________________________________________________
unsigned int
Analysis::CalibrationDataInterfaceROOT::getNumVariations(const std::string& author,
							 const std::string& label,
							 const std::string& OP,
							 Uncertainty unc) const
{
  // Retrieve the number of eigenvector variations or named variations relevant for
  // the given scale factor calibration object, identifying the object by name.
  //
  //    author:  jet collection name
  //    label:   jet flavour label
  //    OP:      tagger working point
  //    unc:     should be set to SFEigen or SFNamed for the cases of
  //             eigenvector variations or named variations, respectively

  unsigned int index;

  if (! retrieveCalibrationIndex (label, OP, author, true, index)) return 0;
  return getNumVariations(index, unc);
}

//________________________________________________________________________________
unsigned int
Analysis::CalibrationDataInterfaceROOT::getNumVariations(unsigned int index,
							 Uncertainty unc) const
{
  // Retrieve the number of eigenvector variations or named variations relevant for
  // the given scale factor calibration object, identifying the object by index.
  //
  //    index:   index to calibration scale factor object
  //    unc:     should be set to SFEigen or SFNamed for the cases of
  //             eigenvector variations or named variations, respectively

  if (! (unc == SFEigen || unc == SFNamed)) return 0;
  CalibrationDataContainer* container = m_objects[index];
  if (! container) return 0;
  const CalibrationDataEigenVariations* eigenVariation=m_eigenVariationsMap[container];
  return (unc == SFEigen) ?
    eigenVariation->getNumberOfEigenVariations() :
    eigenVariation->getNumberOfNamedVariations();
}

//________________________________________________________________________________
const TH1*
Analysis::CalibrationDataInterfaceROOT::getBinnedScaleFactors (const std::string& author,
							       const std::string& label,
							       const std::string& OP) const
{
  // Retrieve the actual histogrammed calibration scale factors, identifying the object by name.
  //
  //    author:  jet collection name
  //    label:   jet flavour label
  //    OP:      tagger working point

  unsigned int index;
  if (! retrieveCalibrationIndex (label, OP, author, true, index)) {
    // Return a dummy result if the object is not found
    cerr << "getBinnedScaleFactors: unable to find SF calibration for object "
	 << fullName(author, OP, label, true) << endl;
    return 0;
  }
  CalibrationDataHistogramContainer* container = dynamic_cast<CalibrationDataHistogramContainer*>(m_objects[index]);
  return (container) ? dynamic_cast<TH1*>(container->GetValue("result")) : 0;
}

//________________________________________________________________________________
const TObject*
Analysis::CalibrationDataInterfaceROOT::getMCEfficiencyObject (const std::string& author,
							       const std::string& label,
							       const std::string& OP,
							       unsigned int mapIndex) const
{
  // Retrieve the actual central values object for the MC efficiences, identifying the object by name.
  // The object returned can be either a TH1 or a TF1; it is up to the user to determine which.
  //
  //    author:   jet collection name
  //    label:    jet flavour label
  //    OP:       tagger working point
  //    mapIndex: index to the efficiency map to be used

  unsigned int index;
  if (! retrieveCalibrationIndex (label, OP, author, false, index, mapIndex)) {
    // Return a dummy result if the object is not found
    cerr << "getMCEfficiencyObject: unable to find efficiency calibration for object "
	 << fullName(author, OP, label, false, mapIndex) << endl;
    return 0;
  }
  CalibrationDataContainer* container = m_objects[index];
  return (container) ? container->GetValue("result") : 0;
}

//====================== retrieval of shifted calibration object ===========================

//________________________________________________________________________________
const TH1*
Analysis::CalibrationDataInterfaceROOT::getShiftedScaleFactors (const std::string& author,
								const std::string& label,
								const std::string& OP,
								const std::string& unc,
								double sigmas) const
{
  // Retrieve the actual histogrammed calibration scale factors, identifying the object by name
  // and with the scale factors shifted by the uncertainties due to the given source of uncertainty
  // (where bin-to-bin correlations are accounted for, i.e., shifts may be either positive or negative).
  //
  //    author:  jet collection name
  //    label:   jet flavour label
  //    OP:      tagger working point
  //    unc:     source of uncertainty to consider
  //    sigmas:  number of standard deviations by which to shift the scale factor central values

  // quick sanity check
  if (unc == "comment" || unc == "result" || unc == "combined" || unc == "statistics") return 0;

  unsigned int index;
  if (! retrieveCalibrationIndex (label, OP, author, true, index)) {
    // Return a null result if the object is not found
    cerr << "getShiftedScaleFactors: unable to find SF calibration for object "
	 << fullName(author, OP, label, true) << endl;
    return 0;
  }
  CalibrationDataHistogramContainer* container = dynamic_cast<CalibrationDataHistogramContainer*>(m_objects[index]);
  if (! container) return 0;

  TH1* result = dynamic_cast<TH1*>(container->GetValue("result"));
  TH1* hunc = dynamic_cast<TH1*>(container->GetValue(unc.c_str()));
  // another sanity check...
  if ((! hunc) || (! result)) return 0;
  if (hunc->GetDimension() != result->GetDimension() || hunc->GetNbinsX() != result->GetNbinsX() ||
      hunc->GetNbinsX() != result->GetNbinsX() || hunc->GetNbinsX() != result->GetNbinsX())
    return 0;
  // also check that the uncertainty is to be treated as correlated from bin to bin
  // (for the variation is applied coherently, which isn't appropriate for uncertainties
  // that aren't correlated from bin to bin)
  if (! container->isBinCorrelated(unc)) return 0;

  // if everything is consistent, the actual operation simply consists of adding histograms...
  std::string name(container->GetName()); name += "_"; name += unc; name += "_";
  TH1* shifted = dynamic_cast<TH1*>(result->Clone(name.c_str()));
  shifted->Add(hunc, sigmas);
  return shifted;
}

//====================== put some utility functions here ===================================

namespace {
  // Construct the (diagonal) covariance matrix for the statistical uncertainties on the "ref" results
  TMatrixDSym getStatCovarianceMatrix(const TH1* hist) {
    Int_t nbinx = hist->GetNbinsX()+2, nbiny = hist->GetNbinsY()+2, nbinz = hist->GetNbinsZ()+2;
    Int_t rows = nbinx;
    if (hist->GetDimension() > 1) rows *= nbiny;
    if (hist->GetDimension() > 2) rows *= nbinz;
    TMatrixDSym stat(rows);
    for (Int_t binx = 1; binx < nbinx; ++binx)
      for (Int_t biny = 1; biny < nbiny; ++biny)
	for (Int_t binz = 1; binz < nbinz; ++binz) {
	  Int_t bin = hist->GetBin(binx, biny, binz);
	  double err = hist->GetBinError(bin);
	  stat(bin, bin) = err*err;
	}
    return stat;
  }

  // Construct the covariance matrix assuming that histogram "unc" contains systematic uncertainties
  // pertaining to the "ref" results, and that the uncertainties are fully correlated from bin to bin
  // (unless option "doCorrelated" is false, in which bins are assumed uncorrelated)
  TMatrixDSym getSystCovarianceMatrix(const TH1* ref, const TH1* unc, bool doCorrelated) {
    Int_t nbinx = ref->GetNbinsX()+2, nbiny = ref->GetNbinsY()+2, nbinz = ref->GetNbinsZ()+2;
    Int_t rows = nbinx;
    if (ref->GetDimension() > 1) rows *= nbiny;
    if (ref->GetDimension() > 2) rows *= nbinz;
    TMatrixDSym cov(rows);

    // Carry out a minimal consistency check
    if (unc->GetNbinsX()+2 != nbinx || unc->GetNbinsY()+2 != nbiny || unc->GetNbinsZ()+2 != nbinz
	|| unc->GetDimension() != ref->GetDimension()) {
      cout << "getSystCovarianceMatrix: inconsistency found in histograms "
	   << ref->GetName() << " and " << unc->GetName() << endl;
      return cov;
    }

    // Special case: uncertainties not correlated from bin to bin
    if (! doCorrelated) {
      for (Int_t binx = 1; binx < nbinx; ++binx)
	for (Int_t biny = 1; biny < nbiny; ++biny)
	  for (Int_t binz = 1; binz < nbinz; ++binz) {
	    Int_t bin = ref->GetBin(binx, biny, binz);
	    double err = ref->GetBinError(bin);
	    cov(bin,bin) = err*err;
	  }
      return cov;
    }

    for (Int_t binx = 1; binx < nbinx; ++binx)
      for (Int_t biny = 1; biny < nbiny; ++biny)
	for (Int_t binz = 1; binz < nbinz; ++binz) {
	  Int_t bin = ref->GetBin(binx, biny, binz);
	  double err = ref->GetBinError(bin);
	  for (Int_t binx2 = 1; binx2 < nbinx; ++binx2)
	    for (Int_t biny2 = 1; biny2 < nbiny; ++biny2)
	      for (Int_t binz2 = 1; binz2 < nbinz; ++binz2) {
		Int_t bin2 = ref->GetBin(binx2, biny2, binz2);
		double err2 = ref->GetBinError(bin2);
		cov(bin, bin2) = err*err2;
	      }
	}
    return cov;
  }

}

//====================== retrieval of calibration covariance matrix ========================

//________________________________________________________________________________
TMatrixDSym 
Analysis::CalibrationDataInterfaceROOT::getScaleFactorCovarianceMatrix (const std::string& author,
									const std::string& label,
									const std::string& OP,
									const std::string& unc) const
{
  // Return the scale factor covariance matrix for the given calibration object.
  // This function is deprecated since its functionality is duplicated in the
  // CalibrationDataEigenVariations class.
  //
  //    author:  jet collection name
  //    label:   jet flavour label
  //    OP:      tagger working point
  //    unc:     source of uncertainty to consider

  // Catch issues with the specified input as early as possible
  TMatrixDSym dummy;
  if (unc == "comment" || unc == "result" || unc == "combined") return dummy;

  unsigned int index;
  if (! retrieveCalibrationIndex (label, OP, author, true, index)) {
  // Return a dummy result if the object is not found
    cerr << "getScaleFactorCovarianceMatrix: unable to find SF calibration for object "
	 << fullName(author, OP, label, true) << endl;
    return dummy;
  }
  CalibrationDataHistogramContainer* container = dynamic_cast<CalibrationDataHistogramContainer*>(m_objects[index]);
  if (!container) return dummy;

  // retrieve the central calibration and its axes
  TH1* result = dynamic_cast<TH1*>(container->GetValue("result"));
  if (! result) return dummy;
  // std::vector<unsigned int> varResult = CalibrationDataContainer::getVariableTypes(result);

  // "normal" case: single source of uncertainty
  if (unc != "all") {
    if (unc == "statistics") {
      return getStatCovarianceMatrix(result);
    } else {
      TH1* hunc = dynamic_cast<TH1*>(container->GetValue(unc.c_str()));
      if (! hunc) {
	cout << "getScaleFactorCovarianceMatrix: no uncertainty object found "
	     << "corresponding to name " << unc << endl;
	return dummy;
      }
      return getSystCovarianceMatrix(result, hunc, container->isBinCorrelated(unc));
    }
  }

  // special case: complete covariance matrix. This is to be constructed
  // as the sum over all individual contributions.
  // First, treat the statistics separately (as above)
  TMatrixDSym cov = getStatCovarianceMatrix(result);

  // Then loop through the list of (other) uncertainties
  std::vector<string> uncs = container->listUncertainties();
  for (unsigned int t = 0; t < uncs.size(); ++t) {
    if (uncs[t] == "comment" || uncs[t] == "result" ||
	uncs[t] == "combined" || uncs[t] == "statistics") continue;
    TH1* hunc = dynamic_cast<TH1*>(container->GetValue(uncs[t].c_str()));
    cov += getSystCovarianceMatrix(result, hunc, container->isBinCorrelated(uncs[t]));
  }

  return cov;
}

//________________________________________________________________________________
void 
Analysis::CalibrationDataInterfaceROOT::initialize(const string& jetauthor, const string& OP, Uncertainty unc) 
{ 
  // Preload objects necessary so that the input calibration file can be closed. 
  // This functionality is only needed when using PROOF. 

  if((!m_fileEff)||(!m_fileSF)) { 
    cerr << "initialize can only be called once per CalibrationDataInterfaceROOT object" << endl; 
    return; 
  } else { 
    cout << "initializing BTagCalibrationDataInterfaceROOT for PROOF with jetAuthor = " << jetauthor  
	 << ", tagger = " << m_taggerName << ", operating point = " << OP << ", uncertainty = " << unc << endl; 
  } 
 		 
  // quark flavours 
  std::vector<string> label; 
  label.push_back("Light");
  label.push_back("T");
  label.push_back("C");
  label.push_back("B"); 
 		 
  CalibrationDataVariables BTagVars; 
  BTagVars.jetAuthor = jetauthor; 
  BTagVars.jetPt  = 100000.; //Irrelevant, just has to be valid to retrieve objects 
  BTagVars.jetEta = 1.5; //Irrelevant, just has to be valid to retrieve objects 
 		 
  std::vector<string>::iterator flav = label.begin(); 
  for(; flav!=label.end(); flav++) { 
    std::pair<double, double> BTagCalibResult; 
    BTagCalibResult = getScaleFactor(BTagVars,(*flav), OP, unc); 
 		 
    std::pair<double, double> BTagCalibMCEff; 
    BTagCalibMCEff = getMCEfficiency(BTagVars,(*flav), OP, unc); 
  }      
 		 
  if (m_fileEff != m_fileSF) { 
    m_fileEff->Close(); 
    delete m_fileEff; 
  } 
  m_fileSF->Close(); 
  delete m_fileSF; 
  m_fileEff = 0; //prevents repeat deletion in destructor 
  m_fileSF = 0; //prevents repeat deletion in destructor 
} 

//________________________________________________________________________________
CalibrationDataContainer*
Analysis::CalibrationDataInterfaceROOT::retrieveContainer(const string& dir, const string& cntname, bool isSF)
{
  // Attempt to retrieve the given container from file. Note that also the corresponding
  // "hadronisation" reference is retrieved (if possible and not yet done).
  //
  //     dir:     name of the directory containing the requested container
  //     cntname: name of the requested container itself (not including the full path)
  //     isSF:    set to false (true) if the object is to be retrieved from the MC efficiencies
  //              file (the calibration scale factor file). Note that it is assumed that scale
  //              factor objects will always be retrieved from the calibration scale factor file.

  // construct the full object name
  string name = dir + "/" + cntname;

  // If the object cannot be found, then each call will result in a new attempt to
  // retrieve the object from the ROOT file. Hopefully this will not happen too often...
  unsigned int idx = m_objectIndices[name] = m_objects.size();
  // CalibrationDataContainer* cnt =
  //   dynamic_cast<CalibrationDataContainer*>((isSF ? m_fileSF : m_fileEff) ->Get(name.c_str()));
  CalibrationDataContainer* cnt;
  (isSF ? m_fileSF : m_fileEff)->GetObject(name.c_str(), cnt);
  // If the requested object is a MC efficiency container and is not found, make a second attempt
  // to retrieve it from the calibration scale factor file. This will avoid the need to duplicate
  // efficiency containers so that the MC efficiency file needs to store only those containers
  // not already present in the calibration scale factor file. Of course this is meaningful only
  // if separate files are used to begin with.
  if (!isSF && !cnt && m_fileSF != m_fileEff) m_fileSF->GetObject(name.c_str(), cnt);
  m_objects.push_back(cnt);
  if (!cnt) {
    cout << "btag Calib: retrieveContainer: failed to retrieve container named " << name.c_str() << " from file" << endl;
    return 0;
  }

  // If the requested object is a MC efficiency container, make sure to retrieve the corresponding
  // calibration scale factor container first (a feature first thought to be necessary, erroneously,
  // but left in since this ordering should not hurt in any case).
  if (m_refMap.find(dir) == m_refMap.end()) {
    if (isSF) {
      // Retrieve the mapping objects from both files and merge their information using the 'helper' class.
      // The map resulting from this is used to retrieve the information required to compute MC/MC scale factors.
      string hadronisationRefs(dir + "/MChadronisation_ref");
      TMap* mapSF = 0; m_fileSF->GetObject(hadronisationRefs.c_str(), mapSF);
      TMap* mapEff = 0; if (m_fileEff != m_fileSF) m_fileEff->GetObject(hadronisationRefs.c_str(), mapEff);
      m_refMap[dir] = new HadronisationReferenceHelper(mapSF, mapEff);
      delete mapSF;
      delete mapEff;
    } else {
      string SFCalibName = getContainername(getBasename(dir), true);
      if (m_objectIndices.find(SFCalibName) == m_objectIndices.end()) retrieveContainer(dir, SFCalibName, true);
    }
  }

  // Attempt to find the corresponding hadronisation reference container needed for the application of
  // MC/MC scale factors.
  if (idx+1 > m_hadronisationReference.size()) m_hadronisationReference.resize(idx+1, -1);
  m_hadronisationReference[idx] = -1;
  string spec = cnt->getHadronisation();
  if (spec != "") {
    std::map<string, HadronisationReferenceHelper*>::const_iterator mapit = m_refMap.find(dir);
    if (mapit != m_refMap.end()) {
      string ref;
      if (mapit->second->getReference(spec, ref)) {
	// Retrieve the hadronisation reference if not already done. Note that the "isSF" is left unchanged:
	// this allows to retrieve the reference from the same file as the scale factor object. An exception
	// is the reference for the calibration scale factor object, which should always be obtained from
	// the scale factor file. 
	// An efficiency container can be its own hadronisation reference (this is not "protected" against).
	string refname(dir + "/" + ref);
	std::map<string, unsigned int>::const_iterator it = m_objectIndices.find(refname);
	// If the reference cannot be found, assume that it hasn't yet been retrieved so attempt it now.
	if (it == m_objectIndices.end()) { 
	  retrieveContainer(dir, ref, isSF); it = m_objectIndices.find(refname);
	}
	m_hadronisationReference[idx] = it->second;
      }
    } else if (m_useMCMCSF)
      cout << "btag Calib: retrieveContainer: MC hadronisation reference map not found -- this should not happen!" << endl;
  }
  if (m_hadronisationReference[idx] == -1 || ! m_objects[m_hadronisationReference[idx]])
    // Not being able to construct the MC/MC scale factors will lead to a potential bias.
    // However, this is not considered sufficiently severe that we will flag it as an error.
    if (m_useMCMCSF)
      cout << "btag Calib: retrieveContainer: warning: unable to apply MC/MC scale factors for container "
	   << name << " with hadronisation reference = '" << spec << "'" << endl;

  // Initialize the Eigenvector variation object corresponding to this object, if applicable. Note that the
  // dual use of "isSF" (both referring to the file and to the object, see above) requires another protection here.
  if (m_runEigenVectorMethod && isSF && name.find("_SF") != string::npos) {
    const CalibrationDataHistogramContainer* histoContainer=dynamic_cast<const CalibrationDataHistogramContainer*>(cnt);
    if (histoContainer==0) {
      cerr << "Could not cast Container to a HistogramContainer. " << endl;
      return 0;
    }
    CalibrationDataEigenVariations* newEigenVariation=new CalibrationDataEigenVariations(histoContainer);

    // At this point we may also want to reduce the number of eigenvector variations.
    // The choices are stored with the container object; but first we need to know what flavour we are dealing with.
    string flavour = dir.substr(dir.find_last_of("/")+1);

    auto const listEnd  = m_excludeFromCovMatrix[flavour].end();
    //    for (std::vector<std::string>::const_iterator listIter= m_excludeFromCovMatrix[flavour].begin(); listIter != listEnd; ++listIter) {
    for (auto listIter= m_excludeFromCovMatrix[flavour].begin(); listIter != listEnd; ++listIter) {
      newEigenVariation->excludeNamedUncertainty(*listIter);
    }

    newEigenVariation->initialize();
    int to_retain = histoContainer->getEigenvectorReduction(m_EVReductions[flavour]);
    if (to_retain > -1) {
      cout << "btag Calib: reducing number of eigenvector variations for flavour " << flavour << " to " << to_retain << endl;
      // The merged variations will end up as the first entry in the specified list, i.e., as the last of the variations to be "retained"
      newEigenVariation->mergeVariationsFrom(size_t(to_retain-1));
    } else if (m_EVReductions[flavour] != Loose)
      cerr << "btag Calib: unable to retrieve eigenvector reduction information for flavour " << flavour << " and scheme "
	   << m_EVReductions[flavour] << "; not applying any reduction" << endl;

    m_eigenVariationsMap[cnt]=newEigenVariation;
  }

  return cnt;
}

//________________________________________________________________________________
string
Analysis::CalibrationDataInterfaceROOT::getAlias(const string& author) const
{
  // Return the alias for the given jet collection name, if an alias exists.
  // If this is not the case, the return value will simply equal the input jet collection name.

  std::map<string,string>::const_iterator it = m_aliases.find(author);
  return (it == m_aliases.end()) ? author : it->second;
}

//________________________________________________________________________________
string
Analysis::CalibrationDataInterfaceROOT::fullName(const string& author, const string& OP,
						 const string& label, bool isSF,
						 unsigned mapIndex) const
{
  // Construct the full calibration object's pathname within the calibration ROOT file.
  //
  //    author:   jet collection name
  //    OP:       tagger working point
  //    label:    jet flavour label
  //    isSF:     set to true (false) for scale factors (MC efficiencies)
  //    mapIndex: index in the list of MC efficiency calibration objects

  string flavour = (label == "N/A") ? "Light" : label;
  string full(m_taggerName + "/" + getAlias(author) + "/" + OP + "/" + flavour + "/");
  full += getContainername(flavour, isSF, mapIndex);
  // full += getAlias(author); full += "/";
  // string name = (isSF) ?
  //   getBasename(OP, label, "_SF", true) :
  //   getBasename(OP, label, "_Eff", false, mapIndex);
  // full += name;
  return full;
}

//________________________________________________________________________________
Analysis::CalibrationDataInterfaceROOT::HadronisationReferenceHelper::HadronisationReferenceHelper(TMap* mapSF, TMap* mapEff)
{
  // Create the map from hadronisation specifications to reference container names for
  // a given ROOT file directory.
  //
  //    mapSF:  reference specification as extracted from calibration scale factor file
  //    mapEff: reference specification as extracted from MC efficiency file
  //            (null if the two files are identical)

  // First take the scale factor file's map
  if (mapSF) {
    TMapIter next(mapSF); TObjString* spec;
    while ((spec = (TObjString*) next())) {
      TObjString* ref = (TObjString*) mapSF->GetValue(spec);
      m_refs[string(spec->GetName())] = string(ref->GetName());
    }
  }
  // Then do the same with the efficiency file's map. The result will be to override any
  // items from the SF file's map. An exception is made for the scale factor calibration object,
  // for which (for the sake of consistency) the SF reference must be retained.
  if (mapEff) {
    TMapIter next(mapEff); TObjString* spec;
    while ((spec = (TObjString*) next())) {
      TObjString* ref = (TObjString*) mapEff->GetValue(spec);
      m_refs[string(spec->GetName())] = string(ref->GetName());
    }
  }
}

//________________________________________________________________________________
bool
Analysis::CalibrationDataInterfaceROOT::HadronisationReferenceHelper::getReference(const string& spec,
										   string& ref) const
{
  // Extract the reference histogram name corresponding to the given hadronisation specification (if existing).
  // The return value is used to indicate whether the specification could be found.
  //
  //    spec:             hadronisation specification
  //    ref:              container name corresponding to this specification

  std::map<string, string>::const_iterator it = m_refs.find(spec);
  if (it == m_refs.end()) return false;

  ref = it->second;
  return true;
}

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef egammaMVACalibNew_H
#define egammaMVACalibNew_H

#include <map>
#include <vector>
#include <iostream>
#include <memory>
#include <string>

#include <TString.h>
#include <TObject.h>
#include <TFile.h>
#include "AsgTools/AsgMessaging.h"

class TTreeFormula;
class TList;
class TTree;
class TPRegexp;
class TObjArray;
class TXMLEngine;
class TH2Poly;
class TAxis;
class TF1;

namespace TMVA { class Reader; } // forward decl
namespace MVAUtils { class BDT; } // forward decl

// WARNING: all info to build the key should be defined as a variable or spectator in at least one of the readers

/** MVA calibration for electrons and photons
  * authors: Bruno Lenzi, Ruggero Turra : 2012 - 2013
  *
  * Computes the energy of electrons or photons from a set of input variables
  * like the energy deposits in each layer of the calorimeter, the position
  * of the EM shower, conversion information for converted photons, etc.
  * using a set of corrections stored in xml files (TMVA outputs) or
  * ROOT files (storing custom Boosted Regression Trees, extracted from TMVA)
  *
  * There are two ways to get the MVA response:
  *   - the variables can be passed explicitly calling getMVAEnergyElectron /
  *     getMVAEnergyPhoton. In this case one should first call InitTree(0)
  *   - the variables can be read automatically from a TTree. One should first call
  *     InitTree(tree) and use getMVAEnergy(index) where index is used for vectors
  *
  * There is one MVA for each bin (particle, eta, Et accordion) defined by ReaderID
  * and stored in a TH2Poly (eta, Et)
  *
  * The README file in this package contains information about the installation and usage
  * see also https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/EgammaMVACalibration
  *
  **/
class egammaMVACalib : public asg::AsgMessaging
{
  public:
    enum egammaType {egPHOTON, egELECTRON, NEGAMMATYPES};
    enum ParticleType {UNCONVERTED=0, CONVERTED=1, SiSi=2, ELECTRON=3, NPARTICLETYPES=4, INVALIDPARTICLE=-1};
    enum CalibrationType {correctEcluster, correctEaccordion, fullCalibration, NCalibrationTypes };
    enum ShiftType {NOSHIFT=0, PEAKTOTRUE, MEANTOTRUE, MEDIANTOTRUE, MEAN10TOTRUE, MEAN20TOTRUE, MEDIAN10TOTRUE, MEDIAN20TOTRUE, NSHIFTCORRECTIONS};

    struct ReaderID
    {
	ParticleType particleType;
	int bin; //etaBin; int energyBin;
	bool operator< (const ReaderID &other) const
	{
	    if (this->particleType != other.particleType)
		return (this->particleType < other.particleType);
	    return (this->bin < other.bin);
	}
    };

    typedef std::map<TString, TString> AdditionalInfoMap;
    typedef std::map< std::pair< egammaMVACalib::ReaderID, egammaMVACalib::ShiftType>, std::unique_ptr<TF1> > ShiftMap;

    /** struct to store the formula, the expression, the type and
     * a float associated to each variable **/
    struct VarFormula
    {
	float variable;
	TString expression;
	TString infoType;
	TString valueType;
	TTreeFormula* formula;
	bool external;
    };

    /** Constructor
      * @param particle : photon or electron, @see egammaType
      * @param newBDTs  : use new BDTs or TMVA ones
      * @param folder : where to find the xml or ROOT files
      * @param method : optimisation method (BDTG by default)
      * @param calibrationType : @see CalibrationType
      * @param etaBinVar : variable used to define the eta binning
      * @param energyBinVar : variable used to define the energy binning
      * @param particleTypeVar : variable used to define the particle type (unconv, SiSi, etc)
      * @param fileName : the expected pattern of the xml files
      * @param ignoreSpectators : do not create formulae for the spectators
      **/
    egammaMVACalib(int particle,
		   bool useNewBDTs = true,
		   TString folder = "",
		   const TString & method = "BDTG",
		   int calibrationType = correctEaccordion,
		   bool debug=false,
		   const TString & etaBinVar = "",
		   const TString & energyBinVar = "",
		   const TString & particleTypeVar = "",
		   TString filePattern = "",
		   bool ignoreSpectators = true);

    virtual ~egammaMVACalib();

    void setPeakCorrection(ShiftType shift_type) { m_shiftType = shift_type; };
    /** Set the peak correction by a capital string **/
    void setPeakCorrection(TString shift_type);

    /** Use cluster energy if MVA response is 0 **/
    void useClusterIf0(bool useCluster = true) { m_clusterEif0 = useCluster; }

    /** Define an expression to replace the given variable **/
    void setDefinition(const TString & variable, const TString & expression);

     /** Define all the TTreeFormula instances needed for calculating the MVA response
       * If doNotify is true and a chain is passed, call Notify each time
       * the tree changes
       **/
    void InitTree(TTree* tree, bool doNotify = true);

    /** Evaluate each formula defined by the readers and store the output
     * in the corresponding float variable (@see VarFormula)
     * It is the user responsibility to call TTree::GetEntry before this command
     **/
    virtual void LoadVariables(int index); // TODO: move to private

    /** Get the MVA output for the given index (or 0 if the corresponding TMVA::Reader is not found)
     * It is the user responsibility to call TTree::GetEntry before this command **/
    float getMVAOutput(int index = 0);

    /** Get the MVA energy estimation (= MVA output * initialEnergy) for the given index
     * Return 0 if the TMVA::Reader or the intialEnergy formula cannot be found
     * It is the user responsibility to call TTree::GetEntry before this command
     * If shift_type is not specified it uses the one set with setPeakCorrection
     * function.
     **/
    float getMVAEnergy(int index = 0, ShiftType shift_type = NSHIFTCORRECTIONS);

    float getMVAEnergyPhoton(float ph_rawcl_Es0,
			     float ph_rawcl_Es1,
			     float ph_rawcl_Es2,
			     float ph_rawcl_Es3,
			     float ph_cl_eta,
			     float ph_cl_E,
			     float ph_cl_etaCalo,
			     float ph_cl_phiCalo,
			     float ph_ptconv,
			     float ph_pt1conv,
			     float ph_pt2conv,
			     int ph_convtrk1nPixHits,
			     int ph_convtrk1nSCTHits,
			     int ph_convtrk2nPixHits,
			     int ph_convtrk2nSCTHits,
			     float ph_Rconv);

    float getMVAEnergyElectron(float el_rawcl_Es0,
			       float el_rawcl_Es1,
			       float el_rawcl_Es2,
			       float el_rawcl_Es3,
			       float el_cl_eta,
			       float el_cl_E,
			       float el_cl_etaCalo,
			       float el_cl_phiCalo);

    /** return the address of a particular input **/
    float* getAddress(const TString & input_name);

    /** do not compute the variable from the TTree.
     * The user should change it for every event
     *  using getAddress to reference the internal variable  **/
    void setExternal(const TString & input_name);

    /** Print the values of the previous evaluation for debugging **/
    void printValueInput();

    /** Return a tree with the MVA response for each event
     * If the input tree uses vector branches, the MVA response is stored as a vector,
     * except if flatten is false
     * @param tree the input tree
     * @param Nentries #entries to process
     * @param branchName Name of the output branch (method name by default)
     * @param copyBranches Branches to copy to MVA tree, comma-separated (input branches by default)
     * @param update Display the progress after N events
     **/
    TTree* getMVAResponseTree(TTree *tree, int Nentries = -1,
			      TString branchName = "", TString copyBranches = "input",
			      int first_event = 0, bool flatten = false, int update=10000);

    /** Return the shift corresponding to the ReaderID and type **/
    float getShift(float Et, ReaderID key, ShiftType shift_type) const;

    /** Return the TTreeFormula associated to the given variable (or a null pointer) **/
    const TTreeFormula* getFormula(const TString & variable) {
	   return (m_formulae.count(variable) ? m_formulae[variable].formula : 0);
    }

    /** Return a list of the TBranches used in the calculation of the MVA response **/
    TObjArray* getListOfBranches();

    /** Activate the branches used in the calculation of the MVA response **/
    void activateBranches();

    /** Get the number of particles in the current event. Used by egammaMVACalibMulti.
     * TTree::GetEntry has to be called beforehand
     **/
    int getNdata();

    /** Return a clone of the TH2Poly object used for defining the binning **/
    TH2Poly* getTH2Poly() const;

    /** Write a ROOT file (MVACalib_<particle>.root) to be used with the new BDT model **/
    void writeROOTfile(const TString& directory, int particle = (int) INVALIDPARTICLE);

    /** Return the reader that corresponds to the given key (or null pointer) **/
    TMVA::Reader* getReader(egammaMVACalib::ReaderID key)
    {
	std::map< egammaMVACalib::ReaderID, TMVA::Reader* >::const_iterator it = m_readers.find(key);
	if (it == m_readers.end()) return 0;
	return it->second;
    }

    /** Return the key that corresponds to the current entry
     * (LoadVariables or getMVAEnergy has to be called beforehand)
     **/
    egammaMVACalib::ReaderID getReaderID();

    /** Return the BDT that corresponds to the given key (or null pointer) **/
    MVAUtils::BDT* getBDT(egammaMVACalib::ReaderID key)
    {
	std::map< egammaMVACalib::ReaderID, MVAUtils::BDT* >::const_iterator it = m_BDTs.find(key);
	if (it == m_BDTs.end()) return 0;
	return it->second;
    }

    /** Called when a TChain changes TTree (overridden from TObject) **/
    //virtual bool Notify();


    // Static methods

    /** Add the branches used by the given formula to the given array **/
    static void addBranchesFromFormula(TObjArray* branches, TTreeFormula *formula);

    /** Return TString from TObjString **/
    static TString getString(TObject*);

    /** Return the list of variables used by a TMVA xml file **/
    static TObjArray* getVariables(const TString &xmlFileName);

    /** Instantiate a dummy TMVA::Reader **/
    static TMVA::Reader* getDummyReader(const TString &xmlFileName);

    /** Get the list of variables separated by comma **/
    static TString* getVariables(TMVA::Reader *reader);

    /** Add the information of a TMVA::Reader with "BDTG" method to TObjArrays **/
    static void addReaderInfoToArrays(TMVA::Reader *reader,
				      TObjArray *trees,
				      TObjArray *variables,
				      int index);


 protected:
    /** Return a string representing the calibration type (Eaccordion, Ecluster, ...) **/
    TString getCalibTypeString();

    /** Instantiate one reader for each xml file and set them up,
     *  calling setupReader for each.
     *  Also initialize the additional information from the xmls
     **/
    void getReaders(const TString & folder);

    /** Instantiate and setup one MVAUtils::BDT for each bin **/
    void getBDTs(const std::string& folder);

    struct XmlVariableInfo {
	TString expression;
	TString label;
	TString varType;
	TString nodeName;
    };

    /** Parse a xml file. Return a vector where each element is a
     * XmlVariableInfo containing information about the variables
     * and spectators. The full filename must be passed.
     **/
    static std::vector<XmlVariableInfo> parseXml(const TString & xmlFilename);

    /** Called by parseXml to extract information about the variables and spectators **/
    static std::vector<XmlVariableInfo> parseVariables(TXMLEngine *xml, void* node, const TString & nodeName);

    /** Call parseXml, define formulae add variables associated to the reader.
      * The full filename must be passed **/
    void setupReader(TMVA::Reader* reader, const TString & xml_filename);

    /** Instantiate and setup BDT from ROOT file **/
    void setupBDT(const TString& fileName);

    /** Setup the functions and pointers used to define the key for each reader **/
    void setupFormulasForReaderID();

    /** Pre-define a formula: store its name, type and definition in m_formulae
      * but do not instantiate TTreeFormula (done in InitTree)
      **/
    void predefineFormula(const TString & name, const TString & expression,
			  const TString & varInfo, const TString & valueType = "F");

    /** Called by getReaders to define the eta/energy range associated to the given file.
     * Return true if the file matches the expected pattern and the definition of
     * the fields and ranges was successful **/
    bool parseFileName(const TString & fileName, egammaMVACalib::ReaderID &key);

    /** Called by getBDTs to define the particle associated to the given file.
      * Return true if the file matches the expected pattern
      **/
    bool parseFileName(const TString & fileName, egammaMVACalib::ParticleType &pType);

    /** Return the variable / field associated to the given string (or an empty string).
     * Used by parseFileName to define the field associated to each part of
     * fileName.
     **/
    TString getBinName( const TString & binField );

    /** Return the bin that corresponds to the interval of eta/Et covered by
      * a weights file. Add the bin if not previously defined
      **/
    int getBin(float etaMin, float etaMax, float energyMin, float energyMax);

    /** Print information about the readers: the total number, eta and energy ranges **/
    void printReadersInfo() const;

    /** Define a formula according to the given name and expression,
      * delete the previous one
      **/
    TTreeFormula* defineFormula(const TString & varName,
				const TString & expression,
				TTree *tree);

    /** Return the ParticleType corresponding to the given string,
      * used by parseFileName
      **/
    static egammaMVACalib::ParticleType getParticleType( const TString & s );

    // Functions to define key
    int getParticleType() const;
    int getBin() const;

    /** Print the definition of each formula **/
    void printFormulae() const;

    /** Update and check the formula if a new tree was loaded **/
    void checkFormula(TTreeFormula *formula);

    /** Define the formula that gives the initial energy estimate
      * depending on the calibrationType: Ecluster, Eaccordion or nothing (Efull)
      **/
    void defineInitialEnergyFormula();

    /** Return the user infomation added in the xml files in the <UserInfo>
     * section as a dictionary
     **/
    static AdditionalInfoMap getUserInfo(const TString & xmlfilename);

    /** Used by getMVAResponseTree: clone input tree, activating the branches defined by
     * <copyBranches> and deactivating all of them first if <deactivateFirst>=true
     **/
    TTree* getOutputTree(TString copyBranches, bool deactivateFirst=true);

    /** Check if the given bin is consistent with the previous definitions **/
    bool checkBin(int bin, float etaMin, float etaMax, float etMin, float etMax);

    /** Define the formulae for the given key **/
    void defineShiftFormula(ReaderID key);

    /** Return the string that corresponds to the shift **/
    const TString& getShiftName(ShiftType shift) const;

    /** Define the formula to get the cluster energy **/
    void defineClusterEnergyFormula();

    /** Check if shower depth variable is present or if it needs to be calculated **/
    void checkShowerDepth(TTree *tree);

    int getNreaders() const { return (m_useNewBDTs ? m_BDTs.size() : m_readers.size()); }

    /** Create an internal TTree when InitTree(0) is called **/
    TTree* createInternalTree(TTree *tree);

    //////////////////////////////////////////////////////////////

    // Parameters defined in the constructor
    egammaMVACalib::egammaType m_egammaType; //! electron or photon
    bool m_useNewBDTs; //! use egamma::BDT or TMVA::Reader
    TString fMethodName; //! BDTG
    egammaMVACalib::CalibrationType m_calibrationType; //! correct Eaccordion, Ecluster...
    // Variables that define the key and the initial energy (for getMVAEnergy)
    TString m_etaVar, m_energyVar, m_particleTypeVar;
    bool m_ignoreSpectators;
    bool m_hasEnergyBins;
    int m_binMultiplicity;

    bool m_clusterEif0; //! Use clusterE if MVA response is 0

    // Variables that point to the ones that define the key
    // and the initial energy (for getMVAEnergy)
    float *m_particleType, *m_eta, *m_energy, *m_initialEnergy;

    // Bin definitions
    //std::map< ParticleType, TAxis* > m_etaAxis, m_eAxis;
    //TAxis *fAxisEta, *fAxisEnergy;
    TH2Poly* m_hPoly;

    // the tree or chain
    TTree *m_tree, *m_input_tree;

    // use internal TTree
    bool m_useInternalTree;

    // Pattern of xml file names
    TPRegexp *m_fileNamePattern;

    // MVA response
    float m_mvaOutput;

    // Dummy float to be used for spectators if needed
    float m_dummyFloat;

    ShiftType m_shiftType;
    // additional information (mean, median, ...) for each xml file
    std::map< egammaMVACalib::ReaderID, egammaMVACalib::AdditionalInfoMap> m_additional_infos;

    // Map containing TTreeFormulae to get the shifts for each ReaderID, ShiftType
    ShiftMap m_shiftMap;

    // TMVA readers
    std::map< egammaMVACalib::ReaderID, TMVA::Reader* > m_readers;

    // New BDT readers
    std::map< egammaMVACalib::ReaderID, MVAUtils::BDT* > m_BDTs;

    // Variables and formulae for the reader
    std::map< TString, egammaMVACalib::VarFormula > m_formulae;

    // Formula to get cluster energy (std calibration)
    TTreeFormula* m_clusterFormula;

  public:
    static std::vector<double> get_radius(double eta);
    static double get_shower_depth(double eta,
				   double raw_cl_0,
				   double raw_cl_1,
				   double raw_cl_2,
				   double raw_cl_3);
  private:
    class NotifyDispatcher : public TObject
    {
    public:
      NotifyDispatcher() = default;
      NotifyDispatcher(const std::vector<TObject*>& objs) : m_objs(objs) { }
      void add_object(TObject* obj) { m_objs.push_back(obj); }
      virtual Bool_t Notify() {
         for (auto obj : m_objs) { obj->Notify(); } return true;
      }
    private:
      std::vector<TObject*> m_objs;
    };

    NotifyDispatcher m_notify_dispatcher; //!
};

#endif

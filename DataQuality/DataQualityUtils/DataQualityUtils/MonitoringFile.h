/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef dqutilsMonitoringFile_h
#define dqutilsMonitoringFile_h

#include <map>
#include <string>
#include <vector>
#include <set>

#include <TObject.h>
#include <TFile.h>
#include <TH1.h>
#include <TString.h>
#include "RooRealVar.h"

#ifndef __CINT__
#include <boost/regex_fwd.hpp>
#else
// good enough for CINT
namespace boost { class regex; }
#endif

class TDirectory;
class TFile;
class TGraph;
class TH1;
class TH1F;
class TH2F;
class TH1D;
class TH2D;
class TH1I;
class TH3F;
class TH2;
class TH3;
class TProfile2D;
class TObjArray;
class TKey;
class TTree;
class TString;
class RooPlot;
class TEfficiency;


namespace dqutils {

  typedef std::map<std::string, std::vector<int> > keycyclemap;
  void populateKeyMapping(TDirectory*, keycyclemap&);

  class MonitoringFile : public TObject {
  public:

    struct MetaData {
      inline MetaData(std::string name_, std::string interval_, std::string chain_, std::string merge_)
	: name(name_), interval(interval_), chain(chain_), merge(merge_) { }
      std::string name;
      std::string interval;
      std::string chain;
      std::string merge;
    };


    class OutputMetadata {
    public:
      OutputMetadata(TTree* metadata);
      virtual ~OutputMetadata();

      virtual void fill(std::string name, 
			 std::string interval, 
			 std::string chain, 
			 std::string merge);

    protected:
      void copyString(char* to, const std::string& from);
      void adjustAddresses(const char* branchName, void* ptr, const char* branchstr);
      const int m_charArrSize;
      TTree* m_metadata;
      char* m_nameData;
      char* m_intervalData;
      char* m_chainData;
      char* m_mergeData;
    };


    MonitoringFile();

    MonitoringFile(std::string fileName);

    virtual ~MonitoringFile();

    typedef std::map< std::string, TDirectory* >  DirMap_t;

    static void getAllDirs(DirMap_t& dirmap, TDirectory* dir, std::string dirName);

    static TDirectory* createDir(DirMap_t& dirmap, TDirectory* dir, std::string parent, std::string path);

    static TKey* getObjKey(TDirectory* dir, std::string path);

    //  enum MergeMethod_t { TH1Add = 0, effAsPerCent, weightedAverage, weightedEff, mergeRebinned, weightedAverage2D, eventSample };

    void mergeDirectory(TDirectory* outputDir, const std::vector<TFile*>& inputFiles, bool has_multiple_runs = false, std::map< TFile*, std::string >* prefixes = 0);

    static void fillMetaDataMap(std::map<std::string, dqutils::MonitoringFile::MetaData>& mdMap, TDirectory* dir);

    void mergeFiles(std::string outFileName, const std::vector<std::string>& files);

    void mergeFiles(std::string outFileName, std::string listFileName);

    static void merge_effAsPerCent(TH2& a, const TH2& b);

    static void merge_perBinEffPerCent(TH1& a, const TH1& b);

    static void merge_effAsPerCentAlt(TH1& a, const TH1& b);

    static void merge_weightedAverage(TH1& a, const TH1& b);

    static void merge_weightedAverage2D(TH2& a, const TH2& b);

    static void merge_weightedEff(TH1& a, const TH1& b);

    static void merge_Rebinned(TH1& a, TH1& b);

    static void merge_eventSample(TH2& a, const TH2& b);

    static void merge_RMS(TH1& a, const TH1& b);

    static void merge_RMSpercentDeviation(TH1& a, const TH1& b);

    static void merge_lowerLB(TH1& a, const TH1& b);
    /**
     * Methods for creating ID Alignment summary plots on merged files
     */
    
    //static void   MakeBowingFit(TProfile2D* p2d, TH1F* hm, TH1F* hb);
    static void   fitMergedFile_IDPerfMonManager(std::string inFileName, bool isIncremental = false);
    static void   fitMergedFile_DiMuMonManager(std::string inFileName, bool isIncremental = false);
    static void   fitMergedFile_IDAlignMonManager(std::string inFileName, bool isIncremental = false);
    static bool   CheckHistogram(TFile* f, const char* HistoName);
    static void   fitMergedFile_IDAlignMonResiduals(TFile* f, std::string run_dir, std::string TriggerName);
    static void   fitMergedFile_IDAlignMonTrackSegments(TFile* file, std::string run_dir, std::string tracksName);
    static void   fitMergedFile_IDAlignMonGenericTracks(TFile* file, std::string run_dir, std::string tracksName);
    static void   fitMergedFile_IDAlignMonPVbiases(TFile* file, std::string run_dir, std::string tracksName);
    static void   fitMergedFile_IDPerfMonKshort(TFile* f, std::string run_dir, std::string TriggerName);
    static void   fitMergedFile_IDPerfMonJpsi(TFile* f, std::string run_dir, 
					       std::string TriggerName);
    static void   fitMergedFile_IDPerfMonUpsilon(TFile* f, std::string run_dir, 
						  std::string TriggerName);
    static void   fitMergedFile_IDPerfMonZee(TFile* f, std::string run_dir, std::string TriggerName);
    static void   fitMergedFile_IDPerfMonWenu(TFile* f, std::string run_dir, std::string TriggerName);
    static void   fitMergedFile_IDPerfMonZMM (TFile* f, std::string run_dir, std::string TriggerName);
    static void   fitJpsiHistograms(TH1F* hmass, TH1F* hwidth, TH1F* h1[], int n);
    static void   fitUpsilonHistograms(TH1F* hmass, TH1F* hwidth, TH1F* h1[], int n);
    static void   fitZmumuMassPlot(TH1F* hmass);
    static std::pair<RooRealVar, RooRealVar>   fitZmumuMass(TH1F* hmass);
    static void   fitZmumuHistograms(TH1F* hmass, TH1F* hwidth, std::vector<TH1F*> hvec);
    static void   processModule(TFile* f, std::string run_dir, TKey* key_module, std::string moduleName);
    static void   fitMergedFile_DiMuMonAll(TFile* f, std::string run_dir, std::string resonName, std::string triggerName);
    static void   fitHistos(TH2F* hin, std::vector<TH1F*> hout, int mode, std::string triggerName, std::string resonName, TH1F* m_chi2);
    static void   fillGaussianMeanOrWidth(TH2F* h2d, TH1F* h, float fitMin, float fitMax, int iopt);
    static void   fillMeanOrWidth(TH2F* h2d, TH1F* h, int iopt);
    static void   fillDetPaperMeanRMS(TH2F* h2d, TH1F* h, int iopt);
   
    static void   meanRMSProjections2D(TH2F* h2d, TH1F* h, int iopt);
    static void   meanRMSProjections3D(TH3F* h3d, TH2F* h, int iopt);
    static void   meanRMSProjections3D_DMRPlot(TH3F* h3d, TH1F* h, int iopt);
    static void   meanRMSProjections3D_XY(TH3F* h3d, TH1F* h, int iXY, int iopt);
    static TH1F*  combineHistos(const char* name, const char* title, TObjArray* plots, int gap);
    static void   setMinWindow(TH1* h1, float min, float max);
    static float  getMedian(TH1* h1);
    static void   ProcessAsymHistograms(TH1F* m_neg, TH1F* m_pos, TH1F* m_asym);
    static void   Make1DProfile(TH1* output, TH2* histo);
    static void   MakeMap(TH2* outputhist, TH3* hist);
    static int    IterativeGaussFit(TH1* hist, double &mu, double &mu_err, double &sigma, double &sigma_err); 

    // Muon CSC
    static void CSCPostProcess(std::string inFilename, bool isIncremental = false);
    static void CSCChamberEfficiency(TFile* f, TString& run_dir);
    static void CSCClusterFit(TFile* f, TString& run_dir);

    // given bin- content and bin-error of numerator, denominator, compute efficiency (eff)
    // and efficiency error (err) according to errtype = 0(binomial), =1(UPX) or Sumw2
    static void GetCscEffAndErr(float& num, float& num_err, float& den, float& den_err, float& eff, float& err, int errtype);

    // For MuonTrackMonitoring
    static void MuonTrackPostProcess(std::string inFileName, bool isIncremental = false);
    static void MuonTrack_Main(std::string inFileName, TString dirname);

    //For Muon MDT
    static void  MDTPostProcess(std::string inFilename, int txtFileOutputType = 0);
    static void  MDTChamReAlign(std::string inFilename, std::string title="MDT");
    static void  MDTChamEff(std::string inFilename,  std::string title="MDT", int txtFileOutputType = 0 );
    static void  MDTChamOcc(std::string inFilename, std::string title="MDT");
    static void  MDTTDCSum(std::string inFilename, std::string title="MDT");
    static void  MDTLowStat(std::string inFilename, std::string title="MDT");
    static bool  MDTCheckAlign(std::string inFilename, std::string title="MDT");
    static bool  MDTCheck(std::string inFilename);
    static void MDTTubeEff(const TString & dirName, const TH1F*, const TH1F*, TH1F*, float &countsML1, float &countsML2, float &entriesML1, float &entriesML2);
    static void MDT2DHWName(TString, TString &, TString &, TString &, TString &, TString &);
    static void MDTFitTDC(TH1F* h, double &t0, double &t0Err, double &tmax, double &tmaxErr);
    static void MDTSetMetaData(TDirectory* targetDir, TH1* h1, TH1* h2=0, TH1* h3=0);
    static void MDTSet2DRangeZ(TH2F* h);
    static void MDTResetContents(TH2F* h);
    static void MDTFinalizeWriteTH1FChar(TH1F* h);
    static double     fittzero(double *, double *);
    static double     fittmax(double *, double *);
    static void GetTubeLength(double & tubeLength, TString dirName);
    static TString returnString(int a);
    static TString returnString(double a);
    static TString ConvertTubeVectorToString(std::vector<int> & v, TH1F* h, TString chamber);
    static TString ConvertLayerVectorToString(std::vector<int> & v, TString chamber);
    static TString ConvertMezzVectorToString(std::vector<int> & v, TH1F* h, TString chamber);
    static TString ConvertVectorToString(std::vector<int> & v);
    static TString TubeID_to_ID_L_ML(int & tubeID, const TString & hardware_name, int totalTubes);
    static TString Layer_to_L_ML(int & tubeID, const TString & hardware_name);
    static TString Mezz_to_ML_mezz(int tubeID, const TString & hardware_name, int totalTubes);
    static bool tdcSort(const TH1* h1, const TH1* h2);
    class MDTPostProcessor : public TFile { //class to keep keep track of histograms we collect
    public:
      MDTPostProcessor(std::string inFilename, std::string name);
      ~MDTPostProcessor(); //delete all objects accessed from the TFile whose SetDirectory(0)

      //function to automatically getObject, obj->SetDirectory(target), if target==0 save target for future deletion
      //option to supply baseDir to get object from subdir of TFile rather than from TFile itself
      //target if nothing is supplied thatn the object will be set to 0 (protects against f->Write() writing unintended copies of your histogram
      template <class T>
      void get(const char* namecycle, T* &ptr, TDirectory* baseDir=0, TDirectory* target=0); //use TDirectory->GetObject(const char* namecycle, void* &ptr);
      void setDirectory(TH1* h, TDirectory* dir = 0);
      void setDirectory(TH2* h, TDirectory* dir = 0);
      void setMetaData(TDirectory* targetDir, TH1* h1, TH1* h2=0, TH1* h3=0);
      void error();//call this function if you want the underlying TFile not to write i.e. recover error in program
    private:
      std::set<TObject*> m_garbage;
      std::string m_name;
      bool m_error;
    };

    static void  RPCPostProcess(std::string inFilename, bool isIncremental = false);
    static bool  RPCCheckHistogram(TFile* f, const char* HistoName);

    //For TGC
    static void TGCPostProcess(std::string inFilename, bool isIncremental = false);
    static void TGCChannelOccupancy(std::string inFilename, std::vector< std::pair< std::string, float > >& p);
    static void TGCChamberOccupancy(std::string inFilename, std::vector< std::pair< std::string, float > >& phigh, std::vector< std::pair< std::string, float > >& plow);
    static void TGCChamberEfficiency(std::string inFilename, std::vector< std::pair< std::string, float > >& p);
    static void TGCChamberTiming(std::string inFilename, std::vector< std::pair< std::string, float > >& pro, std::vector< std::pair< std::string, float > >& ptrg);
    //static void TGCDQResultsToCool();
    static void TGCsubsectbin2stationeta(int subsect, int bin, int& station, int& eta);
    static void TGCsubsect2sectorphi(int subsect, int& sector, int& phi4);
    static void TGCRawHistogramDivision(std::string inFilename);
    static void TGCLV1HistogramDivision(std::string inFilename);
    static int  nTGCWireStripMap(int ws, int etac, int phi48);
    static int  getTGCNumberOfWires(const int istationName, const int layer, const int istationEta, const int istationPhi);
    static void TGCResetContents(TH1* h);

    //For MDTvsTGC
    static void MDTvsTGCPostProcess(std::string inFilename, bool isIncremental = false);
    static void MDTvsTGCEfficiency(std::string inFilename);
    static void MDTvsTGCResetContents(TH1* h);

    class PostProcessorFileWrapper : public TFile { //class to keep keep track of histograms we collect
    public:
      PostProcessorFileWrapper(std::string inFilename, std::string name);
      ~PostProcessorFileWrapper(); //delete all objects accessed from the TFile whose SetDirectory(0)

      //function to automatically getObject, obj->SetDirectory(target), if target==0 save target for future deletion
      //option to supply baseDir to get object from subdir of TFile rather than from TFile itself
      //target if nothing is supplied thatn the object will be set to 0 (protects against f->Write() writing unintended copies of your histogram
      template <class T>
      void get(const char* namecycle, T* &ptr, TDirectory* baseDir=0, TDirectory* target=0); //use TDirectory->GetObject(const char* namecycle, void* &ptr);
      void setDirectory(TH1* h, TDirectory* dir = 0);
      void setDirectory(TH2* h, TDirectory* dir = 0);
      void setDirectory(TGraph* g, TDirectory* dir = 0);
      void setMetaData(TDirectory* targetDir, TH1* h1, TH1* h2=0, TH1* h3=0);
      void error();//call this function if you want the underlying TFile not to write i.e. recover error in program
    private:
      std::set<TObject*> m_garbage;
      std::string m_name;
      bool m_error;
    };
    //
    //static bool TGCCheckFile(std::string inFilename, std::string& run_dir);
    static bool TGCCheckHistogram(TFile *f, TString& hname);
    static void TGCSetMetaData(TDirectory* targetDir, TH1* h1, TH1* h2=0, TH1* h3=0);

    //For HLT Muon
    static void HLTMuonPostProcess(std::string inFilename, bool isIncremental = false);
    static void HLTMuonHistogramDivision(std::string inFilename, TString& run_dir);
    static void HLTMuonTriggerOverlapMatrix(std::string inFilename, TString& run_dir);
    static bool HLTMuonCheckHistogram(TFile* f, TString& hname);
    static void HLTMuonHDiv(PostProcessorFileWrapper& mf, TString sdir, TString snum, TString sden, TString seff, TString seffg);

    //HLT Egamma
    static void HLTEgammaPostProcess(const std::string &inFilename, bool isIncremental = false);

    static void HLTEgammaEfficiencyOff(TFile* f, TDirectory* trig_dir, TDirectory* off_dir, const TString &pathNum, 
					const TString &pathEff, const std::vector<TString> &varName);
    static void HLTEgammaEfficiencyRel(TFile* f, TDirectory* trig_dir, const TString &pathPre, const TString &pathRej, 
					const std::vector<TString> &objStage, const std::vector<TString> &varName);

    //HLT tau
    static void HLTTauPostProcess(std::string inFilename, bool isIncremental = false);

    static void HLTTauPostProcess(TFile* f, TDirectory* dir, 
                                   TString pathApp, TString pathAppEff, 
                                   std::vector<TString> lvlN, std::vector<TString> varN, 
                                   std::vector< std::pair< int, int > > ratioIndex, int nameStyle, 
                                   TString nameApp = "");

    static std::string getPath (TDirectory* dir);

    // HLT MET
    static void HLTMETPostProcess(std::string inFileName, bool isIncremental = false);
    static void HLTMETAveragePhivsEtaMaps(TFile *f, TString& run_dir);
    static size_t HLTMETGetDQLBNRange(TDirectory*& run_dir, std::vector<TString>& lbnDirs);
    static int HLTMETGetStatusPerBin(TH1I *&hist, int ymin, int ymax, int rmin, int rmax);
    static void HLTMETDQFlagSummary(TFile *f, TString& run_dir);

    // HLT Jet
    static void HLTCaloPostProcess(std::string inFileName, bool isIncremental = false);
    static void HLTCaloAveragePtPhiEtaMaps(TFile* f, TString& run_dir);

    // HLT Jet
    static void HLTJetPostProcess(std::string inFileName, bool isIncremental = false);
    static void HLTJetCalcEfficiencyAndRate(TFile* f, TString& run_dir);

    //JetTagging
    static void BJetTaggingPostProcess(std::string inFileName, bool isIncremental = false);
    static void BJetTaggingAdjustRanges(TFile * f);
    static void BJetTaggingNormalise(TFile * f);
    
    //HLT MinBiasMon
    static void HLTMinBiasMonPostProcess(std::string inFileName, bool isIncremental = false);
    static void HLTMinBiasMonGetTargetHistos(TDirectory *source, std::vector< std::pair<TString,TString> >& targetNames);

    // primary vertex monitoring
    static void pv_PrimaryVertexMonitoring_calcResoAndEfficiency(std::string inFilename, bool isIncremental = false);

    // primary vertex multiplicity monitoring
    static void VxMon_move(std::string inFilename, bool isIncremental=false);

    // L1Calo
    static void L1CaloPostProcess(std::string inFileName, bool isIncremental = false);
    static void L1CaloStabilityRMS(TFile* f, const TString& nameDir, const TString& nameTag);
    static void L1CaloFillWithError(TFile* f, const TString& nameDir, const TString& nameData,
                                              const TString& nameError);
    static void L1CaloResetEfficiencies(TFile* f, const TString& effDir,
                                        const TString& nameDen, const TString& nameEff,
				        int items, double threshold, int binSkip);

    // SCT
    static void PixelPostProcess(std::string inFilename, bool isIncremental = false);
    static void SCTPostProcess(std::string inFilename, bool isIncremental = false);
 
    // Other helper methods
    // Compute Bin Errors a la Ullrich, Paterno, Xu (see source file for more details)
    static int ComputeUPXBinErrors(TH1F *hnum, TH1F *hden, std::vector<float>& errors);

    /**
     * Clears all previous data and opens the file with the given name for analysis, returning a
     * boolean indicating whether the file was opened successfully or not.
     */

    virtual bool setFile(std::string fileName);


    virtual void printDirectories() const;

    virtual void printStatistics() const;


    /**
     * Copy the indicated set of histograms to an output file
     */

    virtual bool copyHistograms(std::string outFileName, 
				 std::string dirName = "all");

    static std::string getHanResults( std::string hanResultsDir, std::string input, 
				       std::string hcfg, std::string hcfg_min10, std::string hcfg_min30);

    virtual void printHanConfig() const;

    virtual std::string FindCommon(std::string name1, std::string name2) const;
  protected:

    static std::string getIndentation(std::string pathName, std::string leadingSpace = "");

    class HistogramOperation {
    public:
      virtual ~HistogramOperation() { }
      virtual bool execute(TH1* hist) = 0;
      virtual bool execute(TGraph* graph) = 0;
      virtual bool execute(TEfficiency* efficiency) = 0;
      virtual bool executeMD(TH1* hist, const MetaData&) { return execute(hist); }
      virtual bool executeMD(TGraph* graph, const MetaData&) { return execute(graph); }
      virtual bool executeMD(TEfficiency* efficiency, const MetaData&) {return execute(efficiency); }
    };

    class CopyHistogram : public HistogramOperation {
    public:
      CopyHistogram(TDirectory* target, std::string dirName);
      virtual ~CopyHistogram();
      virtual bool execute(TH1* hist);
      virtual bool execute(TGraph* graph);
      virtual bool execute(TEfficiency* eff);
      virtual bool executeMD(TH1* hist, const MetaData& md);
      virtual bool executeMD(TGraph* graph, const MetaData& md);
      virtual bool executeMD(TEfficiency* eff, const MetaData& md);

    protected:
      void copyString(char* to, const std::string& from);
      TDirectory*  m_target;
      std::string  m_dirName;
      const int m_charArrSize;
      TTree* m_metadata;
      char* m_nameData;
      char* m_intervalData;
      char* m_chainData;
      char* m_mergeData;
    };

    class GatherStatistics : public HistogramOperation {
    public:
      GatherStatistics(std::string dirName);
      virtual bool execute(TH1* hist);
      virtual bool execute(TGraph* graph);
      virtual bool execute(TEfficiency* eff);

      std::string  m_dirName;
      int m_nHist1D;
      int m_nHist1DBins;
      int m_nGraph;
      int m_nGraphPoints;
      int m_nHist2D;
      int m_nHist2DBins;
      int m_nEfficiency;
      int m_nEfficiencyBins;
    };

    class GatherNames : public HistogramOperation {
    public:
      GatherNames();
      virtual bool execute(TH1* hist);
      virtual bool execute(TGraph* graph);
      virtual bool execute( TEfficiency* eff );

      std::vector<std::string> m_names;
    };


    virtual void clearData();


    static bool dirHasHistogramsInMetadata(TDirectory* dir);

    static void loopOnHistograms(HistogramOperation& fcn, TDirectory* dir);

    static bool loopOnHistogramsInMetadata(HistogramOperation& fcn, TDirectory* dir);

    static bool setListFromFile(std::vector<std::string>& filelist, 
				 const std::string& listFileName);

    TFile*  m_file;

    /*   Methods for merging luminosity block intervals
     */
  private:
    enum debugLevel_t { none = 0, DEBUG, VERBOSE };
    typedef std::map<TDirectory*, std::vector<TDirectory*> > map_dir_vdir;

    static TObject* mergeObjsMultiCycles(const std::string&, 
					 const std::vector<int>&, 
					 TDirectory*, std::string&, 
					 TObject* = 0);
    static int mergeObjs(TObject*, TObject*, std::string, debugLevel_t debugLevel = none);
    static int mergeLB_createListOfHistos(TDirectory*, TDirectory*, std::vector<std::string>&, debugLevel_t&);
    static int mergeLB_recursiveDirCopy(TDirectory*, TDirectory*, TDirectory*, std::vector<std::string>&, debugLevel_t&);
    static int mergeLB_processLBinterval(std::vector<TDirectory*>&, TDirectory*, debugLevel_t&);
    static void buildLBToIntervalMap(std::vector<TDirectory*>&, std::vector<TDirectory*>&, map_dir_vdir&, debugLevel_t&);
    static int mergeLB_processLB(std::vector<TDirectory*>&, std::vector<TDirectory*>&, debugLevel_t&);
    static int mergeLB_processRun(TDirectory*, debugLevel_t&);

    static Int_t getNumBins(const TH1& hist);
    boost::regex *m_mergeMatchHistoRE;
    boost::regex *m_mergeMatchDirRE;
    std::string m_mergeMatchHistoREString;
    std::string m_mergeMatchDirREString;
    bool m_useRE;
    static int m_debugLevel;
    static int m_fileCompressionLevel;
  public:
    static int mergeLBintervals(std::string, std::string debugLevel = "none");
    static int getDebugLevel();
    static void setDebugLevel(int level);
    static int getCompressionLevel(){return m_fileCompressionLevel;}
    static void setCompressionLevel(int level){m_fileCompressionLevel=level;}
    bool setHistogramRegEx(const std::string& re);
    bool setDirectoryRegEx(const std::string& re);

    ClassDef(MonitoringFile, 0) // Provides functions to manipulate and analyze data-quality monitoring files

      };

  std::string getInputDirectory(std::string outputDirName, TFile* input, bool has_multiple_runs, std::map< TFile*, std::string >* prefixes);

  std::string getOutputDirectory(std::string inputDirName, TFile* input, bool has_multiple_runs, std::map< TFile*, std::string >* prefixes);

  std::string getOutputDirectory(TKey* key, TFile* input, bool has_multiple_runs, std::map< TFile*, std::string >* prefixes);

  template <class T>
  void MonitoringFile::PostProcessorFileWrapper::get(const char* namecycle, T* &ptr, TDirectory* baseDir, TDirectory* target){

    if (baseDir) baseDir->GetObject(namecycle, ptr); // suggested root function
    else this->GetObject(namecycle, ptr); //suggested root function
    if(!ptr) return;

    if((T::Class())->InheritsFrom("TH1")){
      TH1 *obj = reinterpret_cast<TH1*> (ptr);

      obj->SetDirectory(target);
      if(target == 0) m_garbage.insert(obj);
    }
    else{
      TObject *obj = reinterpret_cast<TObject*>(ptr);

      if(target == 0) m_garbage.insert(obj);
    }
  }
} // namespace dqutils

// #include "DataQualityUtils/MonitoringFile_PPFileWrapper.cxx"
#endif

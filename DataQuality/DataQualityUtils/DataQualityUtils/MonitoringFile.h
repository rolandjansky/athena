/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
      inline MetaData(const std::string & name_, const std::string & interval_, const std::string & chain_, const std::string & merge_)
	: name(name_), interval(interval_), chain(chain_), merge(merge_) { }
      std::string name;
      std::string interval;
      std::string chain;
      std::string merge;
    };


    class OutputMetadata {
    public:
      OutputMetadata(TTree* metadata);
      virtual ~OutputMetadata() = default;

      virtual void fill(const std::string & name, 
			 const std::string & interval, 
			 const std::string & chain, 
			 const std::string & merge);

    protected:
      void makeBranch(const char* branchName, const char* branchstr);
      TTree* m_metadata;
    };


    MonitoringFile();

    MonitoringFile(const std::string & fileName);

    virtual ~MonitoringFile();

    typedef std::map< std::string, TDirectory* >  DirMap_t;

    static void getAllDirs(DirMap_t& dirmap, TDirectory* dir, const std::string & dirName);

    static TDirectory* createDir(DirMap_t& dirmap, TDirectory* dir, const std::string & parent, const std::string & path);

    static TKey* getObjKey(TDirectory* dir, const std::string & path);

    //  enum MergeMethod_t { TH1Add = 0, effAsPerCent, weightedAverage, weightedEff, mergeRebinned, weightedAverage2D, eventSample };

    void mergeDirectory(TDirectory* outputDir, const std::vector<TFile*>& inputFiles, bool has_multiple_runs = false, std::map< TFile*, std::string >* prefixes = 0);

    static void fillMetaDataMap(std::map<std::string, dqutils::MonitoringFile::MetaData>& mdMap, TDirectory* dir);

    void mergeFiles(const std::string & outFileName, const std::vector<std::string>& files);

    void mergeFiles(const std::string & outFileName, const std::string & listFileName);

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

    static void merge_identical(TH1& a, const TH1& b);
    /**
     * Methods for creating ID Alignment summary plots on merged files
     */
    
    //static void   MakeBowingFit(TProfile2D* p2d, TH1F* hm, TH1F* hb);
    static void   fitMergedFile_IDPerfMonManager(const std::string & inFileName, bool isIncremental = false);
    static void   fitMergedFile_DiMuMonManager(const std::string & inFileName, bool isIncremental = false);
    static void   fitMergedFile_IDAlignMonManager(const std::string & inFileName, bool isIncremental = false);
    static bool   CheckHistogram(TFile* f, const char* HistoName);
    static void   fitMergedFile_IDAlignMonResiduals(TFile* f, const std::string & run_dir, const std::string & TriggerName);
    static void   fitMergedFile_IDAlignMonTrackSegments(TFile* file, const std::string & run_dir, const std::string & tracksName);
    static void   fitMergedFile_IDAlignMonGenericTracks(TFile* file, const std::string & run_dir, const std::string & tracksName);
    static void   fitMergedFile_IDAlignMonPVbiases(TFile* file, const std::string & run_dir, const std::string & tracksName);
    static void   fitMergedFile_IDPerfMonKshort(TFile* f, const std::string & run_dir, const std::string & TriggerName);
    static void   fitMergedFile_IDPerfMonJpsi(TFile* f, const std::string & run_dir, 
					       const std::string & TriggerName);
    static void   fitMergedFile_IDPerfMonUpsilon(TFile* f, const std::string & run_dir, 
						  const std::string & TriggerName);
    static void   fitMergedFile_IDPerfMonZee(TFile* f, const std::string & run_dir, const std::string & TriggerName);
    static void   fitMergedFile_IDPerfMonWenu(TFile* f, const std::string & run_dir, const std::string & TriggerName);
    static void   fitMergedFile_IDPerfMonZMM (TFile* f, const std::string & run_dir, const std::string & TriggerName);
    static void   fitJpsiHistograms(TH1F* hmass, TH1F* hwidth, TH1F* h1[], int n);
    static void   fitUpsilonHistograms(TH1F* hmass, TH1F* hwidth, TH1F* h1[], int n);
    static void   fitZmumuMassPlot(TH1F* hmass);
    static std::pair<RooRealVar, RooRealVar>   fitZmumuMass(TH1F* hmass);
    static void   fitZmumuHistograms(TH1F* hmass, TH1F* hwidth, std::vector<TH1F*> hvec);
    static void   processModule(TFile* f, const std::string & run_dir, TKey* key_module, const std::string & moduleName);
    static void   fitMergedFile_DiMuMonAll(TFile* f, const std::string & run_dir, const std::string & resonName, const std::string & triggerName);
    static void   fitHistos(TH2F* hin, std::vector<TH1F*> hout, int mode, const std::string & triggerName, const std::string & resonName, TH1F* m_chi2);
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
    static void CSCPostProcess(const std::string & inFilename, bool isIncremental = false);
    static void CSCChamberEfficiency(TFile* f, TString& run_dir);
    static void CSCClusterFit(TFile* f, TString& run_dir);

    // given bin- content and bin-error of numerator, denominator, compute efficiency (eff)
    // and efficiency error (err) according to errtype = 0(binomial), =1(UPX) or Sumw2
    static void GetCscEffAndErr(float& num, float& num_err, float& den, float& den_err, float& eff, float& err, int errtype);

    // For MuonTrackMonitoring
    static void MuonTrackPostProcess(const std::string & inFileName, bool isIncremental = false);
    static void MuonTrack_Main(const std::string & inFileName, TString dirname);

    //For RPC
    static void  RPCPostProcess(const std::string & inFilename, bool isIncremental = false);
    static bool  RPCCheckHistogram(TFile* f, const char* HistoName);

    //For TGC
    static void TGCPostProcess(const std::string & inFilename, bool isIncremental = false);
    static void TGCChannelOccupancy(const std::string & inFilename, std::vector< std::pair< std::string, float > >& p);
    static void TGCChamberOccupancy(const std::string & inFilename, std::vector< std::pair< std::string, float > >& phigh, std::vector< std::pair< std::string, float > >& plow);
    static void TGCChamberEfficiency(const std::string & inFilename, std::vector< std::pair< std::string, float > >& p);
    static void TGCChamberTiming(const std::string & inFilename, std::vector< std::pair< std::string, float > >& pro, std::vector< std::pair< std::string, float > >& ptrg);
    //static void TGCDQResultsToCool();
    static void TGCsubsectbin2stationeta(int subsect, int bin, int& station, int& eta);
    static void TGCsubsect2sectorphi(int subsect, int& sector, int& phi4);
    static void TGCRawHistogramDivision(const std::string & inFilename);
    static void TGCLV1HistogramDivision(const std::string & inFilename);
    static int  nTGCWireStripMap(int ws, int etac, int phi48);
    static int  getTGCNumberOfWires(const int istationName, const int layer, const int istationEta, const int istationPhi);
    static void TGCResetContents(TH1* h);

    //For MDTvsTGC
    static void MDTvsTGCPostProcess(const std::string & inFilename, bool isIncremental = false);
    static void MDTvsTGCEfficiency(const std::string & inFilename);
    static void MDTvsTGCResetContents(TH1* h);

    class PostProcessorFileWrapper : public TFile { //class to keep keep track of histograms we collect
    public:
      PostProcessorFileWrapper(const std::string & inFilename, const std::string & name);
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
    static void HLTMuonPostProcess(const std::string & inFilename, bool isIncremental = false);
    static void HLTMuonHistogramDivision(const std::string & inFilename, TString& run_dir);
    static void HLTMuonTriggerOverlapMatrix(const std::string & inFilename, TString& run_dir);
    static bool HLTMuonCheckHistogram(TFile* f, TString& hname);
    static void HLTMuonHDiv(PostProcessorFileWrapper& mf, TString sdir, TString snum, TString sden, TString seff, TString seffg);

    //HLT Egamma
    static void HLTEgammaPostProcess(const std::string &inFilename, bool isIncremental = false);

    static void HLTEgammaEfficiencyOff(TFile* f, TDirectory* trig_dir, TDirectory* off_dir, const TString &pathNum, 
					const TString &pathEff, const std::vector<TString> &varName);
    static void HLTEgammaEfficiencyRel(TFile* f, TDirectory* trig_dir, const TString &pathPre, const TString &pathRej, 
					const std::vector<TString> &objStage, const std::vector<TString> &varName);

    //HLT tau
    static void HLTTauPostProcess(const std::string & inFilename, bool isIncremental = false);

    static void HLTTauPostProcess(TFile* f, TDirectory* dir, 
                                   TString pathApp, TString pathAppEff, 
                                   std::vector<TString> lvlN, std::vector<TString> varN, 
                                   std::vector< std::pair< int, int > > ratioIndex, int nameStyle, 
                                   TString nameApp = "");

    static std::string getPath (TDirectory* dir);

    // HLT MET
    static void HLTMETPostProcess(const std::string & inFileName, bool isIncremental = false);
    static void HLTMETAveragePhivsEtaMaps(TFile *f, TString& run_dir);
    static size_t HLTMETGetDQLBNRange(TDirectory*& run_dir, std::vector<TString>& lbnDirs);
    static int HLTMETGetStatusPerBin(TH1I *&hist, int ymin, int ymax, int rmin, int rmax);
    static void HLTMETDQFlagSummary(TFile *f, TString& run_dir);

    // HLT Jet
    static void HLTCaloPostProcess(const std::string & inFileName, bool isIncremental = false);
    static void HLTCaloAveragePtPhiEtaMaps(TFile* f, TString& run_dir);

    // HLT Jet
    static void HLTJetPostProcess(const std::string & inFileName, bool isIncremental = false);
    static void HLTJetCalcEfficiencyAndRate(TFile* f, TString& run_dir);

    //JetTagging
    static void BJetTaggingPostProcess(const std::string & inFileName, bool isIncremental = false);
    static void BJetTaggingAdjustRanges(TFile * f);
    static void BJetTaggingNormalise(TFile * f);
    
    //HLT MinBiasMon
    static void HLTMinBiasMonPostProcess(const std::string & inFileName, bool isIncremental = false);
    static void HLTMinBiasMonGetTargetHistos(TDirectory *source, std::vector< std::pair<TString,TString> >& targetNames);

    // primary vertex monitoring
    static void pv_PrimaryVertexMonitoring_calcResoAndEfficiency(const std::string & inFilename, bool isIncremental = false);

    // primary vertex multiplicity monitoring
    static void VxMon_move(const std::string & inFilename, bool isIncremental=false);

    // L1Calo
    static void L1CaloPostProcess(const std::string & inFileName, bool isIncremental = false);
    static void L1CaloStabilityRMS(TFile* f, const TString& nameDir, const TString& nameTag);
    static void L1CaloFillWithError(TFile* f, const TString& nameDir, const TString& nameData,
                                              const TString& nameError);
    static void L1CaloResetEfficiencies(TFile* f, const TString& effDir,
                                        const TString& nameDen, const TString& nameEff,
				        int items, double threshold, int binSkip);

    // Pixel
    static void PixelPostProcess(const std::string & inFilename, bool isIncremental = false);
 
    // Other helper methods
    // Compute Bin Errors a la Ullrich, Paterno, Xu (see source file for more details)
    static int ComputeUPXBinErrors(TH1F *hnum, TH1F *hden, std::vector<float>& errors);

    /**
     * Clears all previous data and opens the file with the given name for analysis, returning a
     * boolean indicating whether the file was opened successfully or not.
     */

    virtual bool setFile(const std::string & fileName);


    virtual void printDirectories() const;

    virtual void printStatistics() const;


    /**
     * Copy the indicated set of histograms to an output file
     */

    virtual bool copyHistograms(const std::string & outFileName, 
				 const std::string & dirName = "all");

    static std::string getHanResults( const std::string & hanResultsDir, const std::string & input, 
				       const std::string & hcfg, const std::string & hcfg_min10, const std::string & hcfg_min30);

    virtual void printHanConfig() const;

    virtual std::string FindCommon(const std::string & name1, const std::string & name2) const;
  protected:

    static std::string getIndentation(const std::string & pathName, const std::string & leadingSpace = "");

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
      CopyHistogram(TDirectory* target, const std::string & dirName);
      virtual ~CopyHistogram();
      virtual bool execute(TH1* hist);
      virtual bool execute(TGraph* graph);
      virtual bool execute(TEfficiency* eff);
      virtual bool executeMD(TH1* hist, const MetaData& md);
      virtual bool executeMD(TGraph* graph, const MetaData& md);
      virtual bool executeMD(TEfficiency* eff, const MetaData& md);

    protected:
      void fillMD(const MetaData& md);
      TDirectory*  m_target;
      std::string  m_dirName;
      TTree* m_metadata;
    };

    class GatherStatistics : public HistogramOperation {
    public:
      GatherStatistics(const std::string & dirName);
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
					 TDirectory*, const std::string&, 
					 TObject* = 0);
    static int mergeObjs(TObject*, TObject*, const std::string &, debugLevel_t debugLevel = none);
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

  std::string getInputDirectory(const std::string & outputDirName, TFile* input, bool has_multiple_runs, std::map< TFile*, std::string >* prefixes);

  std::string getOutputDirectory(const std::string & inputDirName, TFile* input, bool has_multiple_runs, std::map< TFile*, std::string >* prefixes);

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

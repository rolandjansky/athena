/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOMONITORINGTOOLS_TRIGT1CALOLWHISTOGRAMTOOL_H
#define TRIGT1CALOMONITORINGTOOLS_TRIGT1CALOLWHISTOGRAMTOOL_H

#include <string>
#include <vector>

#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/AsgTool.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"

class TH1;
class TH1F;
class TH2F;

class LWHist;
class LWHist1D;
class LWHist2D;
class TH1F_LW;
class TH2F_LW;
class TH2I_LW;
class TProfile_LW;
class TProfile2D_LW;

class IInterface;
class InterfaceID;
class StatusCode;

namespace TrigConf {
  class ILVL1ConfigSvc;
}
// ============================================================================
namespace LVL1
{
// ============================================================================
/** Tool to provide histogramming utilities
 *
 *  Utilities for labelling, booking and filling histograms in a standard way
 *  for all L1Calo Athena monitoring.
 *
 *  If the MonGroup is set before booking the histograms are registered
 *  with the monitoring framework when booked, if not they remain private.
 *
 *  Now uses Light Weight Histogram classes
 *
 *  <b>StoreGate Containers Used:</b>
 *
 *  <table>
 *  <tr><th> Container    </th><th> Comment                         </th></tr>
 *  <tr><td> @c EventInfo </td><td> For event number via @c EventID </td></tr>
 *  </table>
 *
 *  <b>Services Used:</b>
 *
 *  <table>
 *  <tr><th> Service                     </th><th> Description          </th></tr>
 *  <tr><td> @c TrigConf::ILVL1ConfigSvc </td><td> @copydoc m_configSvc </td></tr>
 *  </table>
 *
 *  <b>JobOption Properties:</b>
 *
 *  <table>
 *  <tr><th> Property         </th><th> Description              </th></tr>
 *  <tr><td> @c LVL1ConfigSvc </td><td> @copydoc m_configSvc     </td></tr>
 *  <tr><td> @c EventSamples  </td><td> @copydoc m_eventSamples  </td></tr>
 *  <tr><td> @c ShrinkEtaBins </td><td> @copydoc m_shrinkEtaBins </td></tr>
 *  </table>
 *
 *  <b>Related Documentation:</b>
 *
 *  <a href="https://twiki.cern.ch/twiki/bin/viewauth/Atlas/UsingLightWeightHistograms">
 *  Using Light Weight Histogram classes</a><br>
 *
 *  @author Peter Faulkner
 */

class TrigT1CaloLWHistogramTool : public asg::AsgTool {
  ASG_TOOL_INTERFACE(TrigT1CaloLWHistogramTool)
  ASG_TOOL_CLASS0(TrigT1CaloLWHistogramTool)
 public:
   TrigT1CaloLWHistogramTool(const std::string& name);
   virtual ~TrigT1CaloLWHistogramTool();

   virtual StatusCode initialize();
   virtual StatusCode finalize();

   /// Set current MonGroup
   void setMonGroup(ManagedMonitorToolBase::MonGroup* group)
     { m_monGroup = group; }
   /// Unset current MonGroup
   void unsetMonGroup() { m_monGroup = 0; }

   ////////////////////////////////
   // Labelling Utilities - General
   ////////////////////////////////

   /// Return int/double pair as a string
   std::string intDoubleString(int num, double val, int precision = 2);
   /// Label bins with number pairs
   void numberPairs(LWHist* hist, int firstMin, int firstMax,
                    int secondMin, int secondMax, int step = 1,
		    int offset = 0, bool xAxis = true);
   /// Label bins with number pairs without skipping bins when stepping
   void numberPairs2(LWHist* hist, int firstMin, int firstMax,
                     int secondMin, int secondMax, int step = 1,
		     int offset = 0, bool xAxis = true);
   /// Label bins with numbers
   void numbers(LWHist* hist, int min, int max, int step = 1, int offset = 0,
                                                           bool xAxis = true);
   /// Split long names for Y axis
   std::string splitLine(const std::string& word, bool xAxis = true);
   /// Label bins with sub-status error bit names
   void subStatus(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Get list of threshold names for given type
   bool thresholdNames(const std::string& type,
                             std::vector<std::string>& names);
   /// Label bins with threshold names
   void thresholds(LWHist* hist, const std::string& type, int offset = 0,
                                                          bool xAxis = true);
   /// Put threshold hit values into a string suitable for printing
   std::string thresholdString(int val, int nThresh, int nBits = 1);
   /// Flag which threshold hit values are non-zero and the same
   int thresholdsSame(int val1, int val2, int nThresh, int nBits);
   ///  Flag which threshold hit values are different
   int thresholdsDiff(int val1, int val2, int nThresh, int nBits);
   /// Get threshold bit mask for given type
   unsigned int thresholdMask(const std::string& type);
   // Get threshold bit mask for EM
   unsigned int thresholdMaskEm();
   // Get threshold bit mask for Tau
   unsigned int thresholdMaskTau();

   ////////////////////////////////
   // Labelling Utilities - CPM
   ////////////////////////////////

   /// Label bins with CPM chip/local coordinate (3bit/3bit)
   void cpmChipLocalCoord(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with CMX-CP TOB chip/local coordinate (4bit/2bit)
   void cpmTobChipLocalCoord(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with CPM/CMX crate/module
   void cpmCMXCrateModule(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with CPM crate/CMX
   void cpmCrateCMX(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with CPM crate/module
   void cpmCrateModule(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with CPM RoI threshold names
   void cpmThresholds(LWHist* hist, int offset = 0, bool xAxis = true);

   ////////////////////////////////
   // Labelling Utilities - JEM
   ////////////////////////////////

   /// Label bins with JEM crate/module
   void jemCrateModule(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with JEM/CMX crate/module
   void jemCMXCrateModule(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with JEM crate/CMX
   void jemCrateCMX(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with JEM frame/local coord
   void jemFrameLoc(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with JEM RoI threshold names
   void jemThresholds(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with Main Jet threshold names
   void mainJetThresholds(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with Backward Jet threshold names
   void backwardJetThresholds(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with Forward Jet threshold names
   void forwardJetThresholds(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with JetEt threshold names
   void jetEtThresholds(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with MissingEt threshold names
   void missingEtThresholds(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with SumEt threshold names
   void sumEtThresholds(LWHist* hist, int offset = 0, bool xAxis = true);
   /// Label bins with MissingEtSig threshold names
   void missingEtSigThresholds(LWHist* hist, int offset = 0, bool xAxis = true);

   ////////////////////////////////
   // Labelling Utilities - PPM
   ////////////////////////////////

   /// Label bins with PPM crate/module
   void ppmCrateModule(LWHist* hist, int firstCrate, int lastCrate,
                                     int offset = 0, bool xAxis = true);
   /// Label bins with PPM error bit names
   void ppmErrors(LWHist* hist, int offset = 0, bool xAxis = true);

   /// Label bins with PPM submodule/channel
   void ppmSubmoduleChannel(LWHist* hist, int offset = 0, bool xAxis = true);

   ////////////////////////////////
   // Booking Utilities - General
   ////////////////////////////////

   /// Book and register a 1D histogram
   TH1F_LW* book1F(const std::string& name, const std::string& title,
                                            int nx, double xmin, double xmax);
   /// Book and register a 1D histogram with variable width bins
   TH1F_LW* book1F(const std::string& name, const std::string& title,
                                            int nx, const double* xbins);
   /// Book and register a 1D profile histogram
   TProfile_LW* bookProfile(const std::string& name, const std::string& title,
                                            int nx, double xmin, double xmax);
   /// Book and register a 1D profile histogram with variable width bins
   TProfile_LW* bookProfile(const std::string& name, const std::string& title,
                                            int nx, const double* xbins);
   /// Book and register a 2D histogram
   TH2F_LW* book2F(const std::string& name, const std::string& title,
                                            int nx, double xmin, double xmax,
		                            int ny, double ymin, double ymax);
   /// Book and register a 2D histogram with variable width bins
   TH2F_LW* book2F(const std::string& name, const std::string& title,
                                            int nx, const double* xbins,
					    int ny, double ymin, double ymax);
   /// Book and register a 2D histogram with variable width bins both axes
   TH2F_LW* book2F(const std::string& name, const std::string& title,
                                            int nx, const double* xbins,
					    int ny, const double* ybins);
   /// Book and register a 2D histogram of integers displayed as text
   TH2I_LW* book2I(const std::string& name, const std::string& title,
                                            int nx, double xmin, double xmax,
					    int ny, double ymin, double ymax);
   /// Book and register a 2D profile histogram
   TProfile2D_LW* bookProfile2D(const std::string& name,
                                const std::string& title,
                                int nx, double xmin, double xmax,
			        int ny, double ymin, double ymax);
   /// Book and register a 2D profile histogram with variable width bins
   TProfile2D_LW* bookProfile2D(const std::string& name,
                                const std::string& title,
                                int nx, const double* xbins,
				int ny, double ymin, double ymax);
   /// Book and register a 2D profile histogram with variable width bins both axes
   TProfile2D_LW* bookProfile2D(const std::string& name,
                                const std::string& title,
                                int nx, const double* xbins,
				int ny, const double* ybins);
   /// Book and register a 2D histogram containing event numbers as bin contents
   TH2I_LW* bookEventNumbers(const std::string& name, const std::string& title,
                                         int ny, double ymin, double ymax);
   /// Register a histogram
   void registerHist(LWHist* hist);

   /// ROOT versions

   /// Book and register a 1D histogram
   TH1F* bookTH1F(const std::string& name, const std::string& title,
                                           int nx, double xmin, double xmax);
   /// Book and register a 2D histogram
   TH2F* bookTH2F(const std::string& name, const std::string& title,
                                           int nx, double xmin, double xmax,
	                                   int ny, double ymin, double ymax);
   /// Register a histogram
   void registerHist(TH1* hist);

   ////////////////////////////////
   // Booking Utilities - CPM
   ////////////////////////////////

   /// Book CPM crate/module vs chip/local coordinate (3bits/3bits)
   TH2F_LW* bookCPMCrateModuleVsChipLocalCoord(const std::string& name,
                                               const std::string& title);
   /// Book CPM crate/module vs TOB chip/local coordinate (4bits/2bits)
   TH2F_LW* bookCPMCrateModuleVsTobChipLocalCoord(const std::string& name,
						  const std::string& title);
   /// Book CPM crate/module vs FPGA
   TH2F_LW* bookCPMCrateModuleVsFPGA(const std::string& name,
                                     const std::string& title);
   /// Book CPM crate/module vs thresholds
   TH2F_LW* bookCPMCrateModuleVsThreshold(const std::string& name,
                                          const std::string& title);
   /// Book CPM eta vs phi
   TH2F_LW* bookCPMEtaVsPhi(const std::string& name, const std::string& title);
   /// Book CPM RoI eta vs phi
   TH2F_LW* bookCPMRoIEtaVsPhi(const std::string& name,
                               const std::string& title);
   /// Book CPM events with error/mismatch vs crate/module
   TH2I_LW* bookCPMEventVsCrateModule(const std::string& name,
                                      const std::string& title);
   /// Book CPM module vs crate
   TH2F_LW* bookCPMModuleVsCrate(const std::string& name,
                                 const std::string& title);
   /// Book CPM module vs crate/CMX
   TH2F_LW* bookCPMModuleVsCrateCMX(const std::string& name,
                                    const std::string& title);
   /// Book CPM crate/module vs CMX
   TH2F_LW* bookCPMCrateModuleVsCMX(const std::string& name,
				    const std::string& title);
   /// Book CPM sub-status errors vs crate/module
   TH2F_LW* bookCPMSubStatusVsCrateModule(const std::string& name,
                                          const std::string& title);
   /// Book CPM Sum/CMX
   TH1F_LW* bookCPMSumCMX(const std::string& name,const std::string& title);
   /// Book CPM Sum vs Threshold
   TH2F_LW* bookCPMSumVsThreshold(const std::string& name,
                                  const std::string& title);
   /// Book CPM Sum vs Threshold by CMX
   TH2F_LW* bookCPMSumVsThreshold(const std::string& name,
				  const std::string& title, int cmx);
   ////////////////////////////////
   // Booking Utilities - JEM
   ////////////////////////////////

   /// Book JEM crate/module vs EX,Ey,Et
   TH2F_LW* bookJEMCrateModuleVsExEyEt(const std::string& name,
                                       const std::string& title);
   /// Book JEM crate/module vs frame/local coord
   TH2F_LW* bookJEMCrateModuleVsFrameLoc(const std::string& name,
                                         const std::string& title);
   /// Book JEM crate/module vs thresholds
   TH2F_LW* bookJEMCrateModuleVsThresholds(const std::string& name,
                                           const std::string& title);
   /// Book JEM events with error/mismatch vs crate/module
   TH2I_LW* bookJEMEventVsCrateModule(const std::string& name,
                                      const std::string& title);
   /// Book JEM module Vs crate
   TH2F_LW* bookJEMModuleVsCrate(const std::string& name,
                                 const std::string& title);
   /// Book JEM eta
   TH1F_LW* bookJEMEta(const std::string& name, const std::string& title);
   /// Book JEM eta vs phi
   TH2F_LW* bookJEMEtaVsPhi(const std::string& name, const std::string& title);
   /// Book JEM RoI eta vs phi
   TH2F_LW* bookJEMRoIEtaVsPhi(const std::string& name,
                               const std::string& title);
   /// Book JEM energy with bins matching QuadLinear encoding
   TH1F_LW* bookJEMQuadLinear(const std::string& name, const std::string& title,
			      int scale = 1);
   /// Book JEM main jet thresholds
   TH1F_LW* bookMainJetThresholds(const std::string& name,
                                  const std::string& title);
   /// Book JEM backward jet thresholds
   TH1F_LW* bookBackwardJetThresholds(const std::string& name,
                                      const std::string& title);
   /// Book JEM forward jet thresholds
   TH1F_LW* bookForwardJetThresholds(const std::string& name,
                                     const std::string& title);
   /// Book JEM JetEt thresholds
   TH1F_LW* bookJetEtThresholds(const std::string& name,
                                const std::string& title);
   /// Book JEM MissingEt thresholds
   TH1F_LW* bookMissingEtThresholds(const std::string& name,
                                    const std::string& title);
   /// Book JEM SumEt thresholds
   TH1F_LW* bookSumEtThresholds(const std::string& name,
                                const std::string& title);
   /// Book JEM MissingEtSig thresholds
   TH1F_LW* bookMissingEtSigThresholds(const std::string& name,
                                       const std::string& title);
   /// Book JEM sub-status errors vs crate/module
   TH2F_LW* bookJEMSubStatusVsCrateModule(const std::string& name,
                                          const std::string& title);
   /// Book JEM sub-status errors vs crate
   TH2F_LW* bookJEMSubStatusVsCrate(const std::string& name,
                                    const std::string& title);

   ////////////////////////////////
   // Booking Utilities - PPM
   ////////////////////////////////

   /// Book PPM Em eta
   TH1F_LW* bookPPMEmEta(const std::string name,const std::string title);
   /// Book PPM Had eta
   TH1F_LW* bookPPMHadEta(const std::string name,const std::string title);
   /// Book PPM Em eta vs phi
   TH2F_LW* bookPPMEmEtaVsPhi(const std::string name, const std::string title);
   /// Book PPM Had eta vs phi
   TH2F_LW* bookPPMHadEtaVsPhi(const std::string name, const std::string title);
   /// Book PPM Em eta vs phi profile
   TProfile2D_LW* bookProfilePPMEmEtaVsPhi(const std::string name,
                                           const std::string title);
   /// Book PPM Had eta vs phi profile
   TProfile2D_LW* bookProfilePPMHadEtaVsPhi(const std::string name,
                                            const std::string title);
   /// Book PPM events with error/mismatch vs crate/module
   TH2I_LW* bookPPMEventVsCrateModule(const std::string& name,
                                      const std::string& title,
				      int firstCrate, int lastCrate);
   /// Book PPM Crate/Module vs Submodule/Channel
   TH2F_LW* bookPPMCrateModuleVsSubmoduleChannel(const std::string& name,
                                                 const std::string& title,
				   	         int firstCrate, int lastCrate);
   /// Book PPM Crate/Module vs Submodule/Channel profile
   TProfile2D_LW* bookProfilePPMCrateModuleVsSubmoduleChannel(
                      const std::string& name, const std::string& title,
		      int firstCrate, int lastCrate);
   /// Book PPM SubStatus vs crate/module
   TH2F_LW* bookPPMSubStatusVsCrateModule(const std::string& name,
                                          const std::string& title,
   				          int firstCrate, int lastCrate);
   /// Book PPM ASIC errors vs crate/module
   TH2F_LW* bookPPMErrorsVsCrateModule(const std::string& name,
                                       const std::string& title,
				       int firstCrate, int lastCrate);

   ////////////////////////////////
   // Filling Utilities - General
   ////////////////////////////////

   /// Get Minimum and Maximun bins and contents of 2D histogram
   bool getMinMaxBin(LWHist2D* hist, int& minBinx, int& minBiny,
              int& maxBinx, int& maxBiny, double& minValue, double& maxValue);
   /// Get Minimum and Maximun bins and contents of 2D Profile histogram
   bool getMinMaxBin(TProfile2D_LW* hist, int& minBinx, int& minBiny,
              int& maxBinx, int& maxBiny, double& minValue, double& maxValue);
   /// Fill Error/Mismatch event number
   void fillEventNumber(LWHist2D* hist, double y);
   /// Fill weighted thresholds 1D
   void fillThresholds(LWHist1D* hist, int val, int nThresh, int nBits,
                                                             int offset = 0);
   /// Fill weighted thresholds 2D, X axis
   void fillThresholdsVsY(LWHist2D* hist, int val, int y, int nThresh,
                                                   int nBits, int offset = 0);
   /// Fill weighted thresholds 2D, Y axis
   void fillXVsThresholds(LWHist2D* hist, int x, int val, int nThresh,
                                                 int nBits, int offset = 0);
   /// Replace the contents of one 2D histogram with those of another
   void replaceContents(LWHist2D* histTo, LWHist2D* histFrom);
   /// Replace the contents of one 2D Profile histogram with those of another
   void replaceContents(TProfile2D_LW* histTo, TProfile2D_LW* histFrom);

   ////////////////////////////////
   // Filling Utilities - CPM
   ////////////////////////////////

   /// Fill CPM eta vs phi
   void fillCPMEtaVsPhi(LWHist2D* hist, double eta, double phi,
                                                    double weight = 1.);
   /// Fill CPM RoI eta vs phi
   void fillCPMRoIEtaVsPhi(LWHist2D* hist, double eta, double phi,
                                                       double weight = 1.);

   ////////////////////////////////
   // Filling Utilities - JEM
   ////////////////////////////////

   ///Fill JEM eta vs phi
   void fillJEMEtaVsPhi(LWHist2D* hist, double eta, double phi,
                                                    double weight = 1.);
   /// Fill JEM RoI eta vs phi
   void fillJEMRoIEtaVsPhi(LWHist2D* hist, double eta, double phi,
                                                       double weight = 1.);
   /// Fill JEM phi allowing for granularity varying with eta
   void fillJEMPhi(LWHist1D* hist, double eta, double phi, double weight = 1.);

   ////////////////////////////////
   // Filling Utilities - PPM
   ////////////////////////////////

   /// Fill PPM Em eta vs phi
   void fillPPMEmEtaVsPhi(LWHist2D* hist, double eta, double phi,
                                                      double weight = 1.);
   /// Fill PPM Had eta vs phi
   void fillPPMHadEtaVsPhi(LWHist2D* hist, double eta, double phi,
                                                       double weight = 1.);
   /// Fill PPM Em eta vs phi profile
   void fillPPMEmEtaVsPhi(TProfile2D_LW* hist, double eta, double phi,
                                               double z, double weight = 1.);
   /// Fill PPM Had eta vs phi profile
   void fillPPMHadEtaVsPhi(TProfile2D_LW* hist, double eta, double phi,
                                                double z, double weight = 1.);
   /// Fill PPM phi allowing for granularity varying with eta
   void fillPPMPhi(LWHist1D* hist, double eta, double phi, double weight = 1.);
   /// Fill PPM phi profile allowing for granularity varying with eta
   void fillPPMPhi(TProfile_LW* hist, double eta, double phi, double z,
                                                           double weight = 1.);
   /// Find bin in Em eta vs phi
   void findBinPPMEmEtaVsPhi(LWHist* hist, double eta, double phi,
                                           int& binx, int& biny);
   /// Find bin in Had eta vs phi
   void findBinPPMHadEtaVsPhi(LWHist* hist, double eta, double phi,
                                            int& binx, int& biny);
   /// Set bin content and optionally error in Em eta vs phi
   void setBinPPMEmEtaVsPhi(LWHist2D* hist, int binx, int biny,
                                         double content, double error = -1.);
   /// Set bin content and optionally error in Had eta vs phi
   void setBinPPMHadEtaVsPhi(LWHist2D* hist, int binx, int biny,
                                         double content, double error = -1.);

   ////////////////////////////////
   // Merge Utilities
   ////////////////////////////////

   /// Scale histogram quantities by number of events for weightedAverage merge
   void scaleForMerge(LWHist2D* hist, int nevents);
   /// Compute efficiencies and errors for perBinEffPerCent merge method
   void efficienciesForMerge(LWHist* lw1, LWHist* lw2, LWHist* lw3);


 private:

   /// Trigger configuration service for threshold names
   ServiceHandle<TrigConf::ILVL1ConfigSvc> m_configSvc;
   /// Current MonGroup or 0 if not wanted
   ManagedMonitorToolBase::MonGroup* m_monGroup;
   /// Phi scale for trigger tower eta/phi plots
   double m_phiScaleTT;
   /// Phi scale for jet element eta/phi plots
   double m_phiScaleJE;
   /// Number of Error Event Number Samples
   int m_eventSamples;
   /// Shrink eta bins to same size flag
   bool m_shrinkEtaBins;

};
// ============================================================================
} // end namespace
// ============================================================================
#endif

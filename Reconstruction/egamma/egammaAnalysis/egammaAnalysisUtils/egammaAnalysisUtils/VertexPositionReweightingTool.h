/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef VERTEXPOSITIONREWEIGHTINGTOOL_H
#define VERTEXPOSITIONREWEIGHTINGTOOL_H

#include "TFile.h"
#include "TH1.h"

#include <string>

/*
  Tool written originally by M. Bellomo for 2010 analysis
  Adapted for 2011+2012 analysis by J. Kretzschmar

  Currently provided reweights are taken from a full Z->ee selection
  and may minimally differ for other analysis selections

  Usage instructions:
  Four sets of weights are provided
  * 2011 data and MC11(a/b/c) (weight ~constant for all 2011 data perioods)
     * MC11 with wide z-beam spot (sigma(z) ~ 90 mm)
     * MC11 with narrow z-beam spot (sigma(z) ~ 75 mm)
  * 2012 data and MC12a (weights derived from HCP data set, periods A-E5, ~13/fb)
  * NEW: 2012 run dependent weights - this is not that well tested, but seems to work ok;
    for standard physics analyses it's recommended to use the integrated data weights, as overal effects
    seem very small

  The reweight factors obviously differ and have to be selected accordingly.
  The histograms containing the weights are in the share directory "zvtx_weights_2011_2012.root".
  You may have to copy these to an appropriate place or mostify the sectond argument of the contructor.

  For this two mechanisms are suported via different constructors:
  * 1st constructor with explicit enum values to be set by the user, e.g.:
      VertexPositionReweightingTool *tool = new VertexPositionReweightingTool(VertexPositionReweightingTool::MC11wide);
      VertexPositionReweightingTool *tool = new VertexPositionReweightingTool(VertexPositionReweightingTool::MC11narrow);
      VertexPositionReweightingTool *tool = new VertexPositionReweightingTool(VertexPositionReweightingTool::MC12a);

  * 2nd constructor with the MC set name including the relevant production tags and the tool
    will try to determine the correct configuration, e.g.:
      VertexPositionReweightingTool *tool = new VertexPositionReweightingTool("mc11_7TeV.106046.PythiaZee_no_filter.merge.AOD.e815_s1272_s1274_r3043_r2993")
      VertexPositionReweightingTool *tool = new VertexPositionReweightingTool("mc11_7TeV.106087.McAtNloZee_no_filter.merge.NTUP_SMWZ.e966_s1310_s1300_r3043_r2993_p1035")
      VertexPositionReweightingTool *tool = new VertexPositionReweightingTool("mc12_8TeV.147806.PowhegPythia8_AU2CT10_Zee.merge.NTUP_EGAMMA.e1169_s1482_s1473_r3542_r3549_p1005")
    or fully equivalent:
      VertexPositionReweightingTool *tool = new VertexPositionReweightingTool("s1272")
      VertexPositionReweightingTool *tool = new VertexPositionReweightingTool("s1310")
      VertexPositionReweightingTool *tool = new VertexPositionReweightingTool("s1482")

   For more details see
   https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AtlasProductionGroupMC11c#Production_details
   https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AtlasProductionGroupMC12a

   The weight is accessed by the functions:
   * GetWeight(zv) for integrated data period weights (recommended)
   * GetWeight(zv, nrun) for weights tailored to each 2012 run (so far A-D5)

   The reweighting should be done using the generated z-position of the hard interaction.
   On D3PDs the variable mc_vx_z[i] does in principle contain the correct value for "small i", i ~ 0..10.
   You may have to look for the first non-zero value in this array, as some generators fill mc_vx_z[0] with 0.
   mc_vx_z[2] seems to be a safe choice for most generators.
   However, make sure that:
   * your distribution for the generated z-vertex makes sense (no spikes, expected mean and RMS)
   * data/MC ratio in zvtx after reweight is ~flat across the whole range (apart from some possible effects at the edges)


   Note that there is a chance, that the sum of weights (before any cuts) is not 100% conserved by the weights,
   because the weights are derived from reconstructed distributions, but applied to generated.
   Therefore any analysis should recalculate the "MC luminosity" after application of all relevant weights
   by the usual formula
     lumi = (sum weights)/(crosss section*filter efficiency)
   This effect should however be small, ~0.5%; if you see large effects (5-10%), then likely you have
   selected the wrong set of weights!


*/


class VertexPositionReweightingTool
{

  //===============================================================================================================
  // public
  //
 public:
  
  enum eDataMCBeamspot {MC11wide, MC11narrow, MC12a};

  VertexPositionReweightingTool(eDataMCBeamspot dataMCsel,
				std::string weightsFileName  = "egammaAnalysisUtils/share/zvtx_weights_2011_2012.root");

  VertexPositionReweightingTool(std::string mcProductionTag,
				std::string weightsFileName  = "egammaAnalysisUtils/share/zvtx_weights_2011_2012.root");

  ~VertexPositionReweightingTool();

  float GetWeight( float vxp_z ); // give vertex position in mm
  float GetWeight( float vxp_z, int runnumber ); // give vertex position in mm, run number
  

  //===============================================================================================================
  // private
  //
 private:

  // histogram with weights
  TH1* m_h_weights;
  // histograms with run dependent parameters (for 2012 data)
  TH1* m_h_mean_nrun;
  TH1* m_h_sigma_nrun;

};

#endif // VERTEXPOSITIONREWEIGHTINGTOOL_H

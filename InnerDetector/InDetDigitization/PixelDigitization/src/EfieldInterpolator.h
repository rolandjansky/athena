/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file PixelDigitization/EfieldInterpolator.h
 * @author Lennart Adam <lennart.adam@cern.ch>
 * @date January, 2020
 * @brief
 *	Instances of this class create a map (TH1D) describing the electric field profile along the pixeldepth.
 *	E field maps based on TCAD simulations available only for limited number of values of fluence and bias voltage.
 *	Interpolate inbetween these simulations to save computing time.
 *
 *	The TCAD simulations are performed for the FEI4 pixel sensor with a pixeldepth of 200um (IBL) or 250um (B layers)
 */

#ifndef PIXELDIGITIZATION_EFIELDINTERPOLATOR_H
#define PIXELDIGITIZATION_EFIELDINTERPOLATOR_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TH1.h"

// Different options to retrieve E field
// default: interspline
enum interpolationMethod {
  TCAD,               //use Precomputed TCAD map
  interspline,        //use interpolation based on cubic splines
  interinvdist,       //use inverse distance weighted estimate
  linearField         //linear field according to bias voltage
};

class EfieldInterpolator: public AthAlgTool {
public:
  EfieldInterpolator(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~EfieldInterpolator();
  void setLayer(int layer);
  //Recommended constructor
  StatusCode loadTCADlist(const std::string TCADfileListToLoad);
  //defFct

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  // Member Functions
  const std::string loadTCADfiles(const std::string targetList = "");
  const std::string createInterpolationFromTCADtree(const std::string fTCAD);//TTree* tTCAD);
  bool initializeFromFile(const std::string finpath);
  bool initializeFromDirectory(const std::string fpath);
  double estimateEfield(std::vector<double> vvol, std::vector<double> vflu, std::vector<std::vector<double> > vfluvvol,
                        double aimFlu, double aimVol, const std::string prepend = "", bool debug = false);
  double estimateEfieldInvDistance(std::vector<double> vvol, std::vector<double> vflu,
                                   std::vector<std::vector<double> > vfluvvol, double aimFlu, double aimVol,
                                   double measure = 1.);

  TH1D* createEfieldProfile(double aimFluence, double aimVoltage);
  TH1D* getEfield(double aimFluence, double aimVoltage);
  TH1D* loadEfieldFromDat(const std::string fname, bool fillEdges = true);
  void scaleIntegralTo(TH1* hin, double aimInt, int first = 1, int last = -1);
  void reliabilityCheck(double aimFluence, std::vector<double> fluences, double aimVoltage,
                        std::vector<double> voltages);
private:
  // Member variables
  Gaudi::Property<bool> m_initialized
  {
    this, "initialized", false, "Flag whether already initalized"
  };

  Gaudi::Property<bool> m_useSpline
  {
    this, "useSpline", true, "Flag whether to use cubic splines for interpolation"
  };

  Gaudi::Property<int> m_sensorDepth
  {
    this, "sensorDepth", 200, "Depth of E fields in sensors in um"
  };

  interpolationMethod m_efieldOrigin;
  TH1D* m_efieldProfile;            //Final efield profile
  std::string m_fInter;  //path to .root file for saving interpolation TTree, i.e. ordered by pixeldepth z
  std::vector<std::vector<TString> > list_files(TString fileList_TCADsamples);
  double extrapolateLinear(double x1, double y1, double x2, double y2, double xaim);
  int fillXYvectors(std::vector<double> vLoop, int ifix, std::vector<std::vector<double> > v2vsv1,
                    std::vector<double>& xx, std::vector<double>& yy, bool regularOrder = true);
  void fillEdgeValues(TH1D* hin);
  bool isInterpolation(const std::vector<double>& vval, double aimval)
  {return(vval.front() <= aimval && aimval <= vval.back());};
  bool isInterpolation(std::vector<double>* vval, double aimval)
  {return(vval->front() <= aimval && aimval <= vval->back());};
  double relativeDistance(double x1, double x2);                            //difference between x1 x2 scaled to x1
  double relativeDistance(double x1, double y1, double x2, double y2);
  double estimateEfieldLinear(double aimVoltage);
  void saveTGraph(std::vector<double> vvol, std::vector<double> vflu, std::vector<std::vector<double> > vfluvvol,
                  double aimFlu, double aimVol, const std::string prepend, bool skipNegative = true);
};

#endif //> !PIXELDIGITIZATION_EFIELDINTERPOLATOR_H

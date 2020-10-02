// Dear Emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef EGAMMA_LAYER_RECALIB_TOOL
#define EGAMMA_LAYER_RECALIB_TOOL

/////////////////////////////////////////////////////////////////////////////////
/// Name    : egammaLayerRecalibTool.h
/// Package : egammaLayerRecalibTool
/// Author  : R. Turra
/// Created : May 2013
/// Twiki   : https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/egammaLayerRecalibTool
///
/// DESCRIPTION:
///
/// This class provide corrected layer energies.

/////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <memory>

#include "AsgTools/AsgTool.h"
#include "AsgMessaging/AsgMessaging.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEventInfo/EventInfo.h"
#include "PATInterfaces/CorrectionCode.h"

#include <TH1.h>
#include <TH2.h>
#include <TFormula.h>

#include "egammaLayerRecalibTool/corr_HV_EMBPS.h"
#include "xAODEgamma/Egamma.h"
#include "xAODCaloEvent/CaloCluster.h"


struct StdCalibrationInputs
{
  unsigned int RunNumber;   // only for HV presampler correction
  double eta;
  double phi;               // only for HV presampler correction
  double E0raw;
  double E1raw;
  double E2raw;
  double E3raw;
};


// TODO: add protections for invalid inputs (histogram overflows)
struct GetAmountBase
{
  virtual float operator()(const StdCalibrationInputs & input) const = 0;
  virtual ~GetAmountBase() { };
  virtual GetAmountBase* clone() const = 0;
};


// object adaptor (prefer composition vs inheritance)
struct GetAmountHVPSGuillaume : public GetAmountBase
{
  virtual float operator()(const StdCalibrationInputs & input) const;
private:
  virtual GetAmountHVPSGuillaume* clone() const { return nullptr; };
  corr_HV_EMBPS m_tool;
};


struct GetAmountFormula : public GetAmountBase
{
  GetAmountFormula(const TFormula & formula) : m_formula(formula) { };
  virtual GetAmountFormula* clone() const { return new GetAmountFormula(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
protected:
  TFormula m_formula;
};


struct GetAmountHisto1D : public GetAmountBase
{
  GetAmountHisto1D(const TH1& histo) : m_histo(static_cast<TH1*>(histo.Clone())) { m_histo->SetDirectory(nullptr); };
  GetAmountHisto1D(const GetAmountHisto1D & oth) : m_histo(static_cast<TH1*>(oth.m_histo->Clone())) { m_histo->SetDirectory(nullptr); };
  GetAmountHisto1D& operator= (const GetAmountHisto1D & oth){
    if (this != &oth) // protect against invalid self-assignment
      {
	m_histo.reset(static_cast<TH1*>(oth.m_histo->Clone()));
	m_histo->SetDirectory(nullptr);
      }
    return *this;
  }
  virtual GetAmountHisto1D* clone() const { return new GetAmountHisto1D(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
protected:
  std::unique_ptr<TH1> m_histo;
};


struct GetAmountHisto1DUp : public GetAmountHisto1D
{
  GetAmountHisto1DUp(const TH1& histo) : GetAmountHisto1D(histo) { };
  virtual GetAmountHisto1D* clone() const { return new GetAmountHisto1DUp(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
};


struct GetAmountHisto1DDown : public GetAmountHisto1D
{
  GetAmountHisto1DDown(const TH1& histo) : GetAmountHisto1D(histo) { };
  virtual GetAmountHisto1D* clone() const { return new GetAmountHisto1DDown(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
};


struct GetAmountHisto1DErrorUp : public GetAmountHisto1D
{
  GetAmountHisto1DErrorUp(const TH1& histo) : GetAmountHisto1D(histo) { };
  virtual GetAmountHisto1D* clone() const { return new GetAmountHisto1DErrorUp(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
};


struct GetAmountHisto1DErrorDown : public GetAmountHisto1D
{
  GetAmountHisto1DErrorDown(const TH1& histo) : GetAmountHisto1D(histo) { };
  virtual GetAmountHisto1D* clone() const { return new GetAmountHisto1DErrorDown(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
};


struct GetAmountHisto2D : public GetAmountBase
{
  GetAmountHisto2D(const TH2F& histo) : m_histo(histo) { m_histo.SetDirectory(nullptr); };
  virtual GetAmountHisto2D* clone() const { return new GetAmountHisto2D(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
protected:
  TH2F m_histo;
};


struct GetAmountFixed : public GetAmountBase
{
public:
  GetAmountFixed(float amount) : m_amount(amount) { }
  virtual GetAmountFixed* clone() const { return new GetAmountFixed(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
private:
  float m_amount;
};


class GetAmountDecoratorBase : public GetAmountBase
{
public:
  GetAmountDecoratorBase(GetAmountBase* getter) : m_getter(getter) { };
  virtual ~GetAmountDecoratorBase() { delete m_getter; }
  virtual float operator()(const StdCalibrationInputs & input) const {
    return (*m_getter)(input);
  }
private:
  GetAmountBase* m_getter;
};


struct InputModifier
{
  enum NullPoint {ZEROBASED, ONEBASED, ZEROBASED_ALPHA, ONEBASED_ALPHA, SHIFT};

  InputModifier(NullPoint base) : m_base(base) { };
  CP::CorrectionCode operator()(StdCalibrationInputs&, float amount) const;
  virtual InputModifier* clone() const = 0;
  virtual ~InputModifier() { };
private:
  InputModifier() { }; // privatize default constructor
  // here we are one based (amount == 1 <=> null scale)
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const=0;
  NullPoint m_base;
};


struct ScaleE0 : public InputModifier
{
  ScaleE0(NullPoint base) : InputModifier(base) { };
  ScaleE0* clone() const { return new ScaleE0(*this); };
private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
};


struct ScaleE1 : public InputModifier
{
  ScaleE1(NullPoint base) : InputModifier(base) { };
  ScaleE1* clone() const { return new ScaleE1(*this); };
private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
};


struct ScaleE2 : public InputModifier
{
  ScaleE2(NullPoint base) : InputModifier(base) { };
  ScaleE2* clone() const { return new ScaleE2(*this); };
private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
};


struct ScaleE3 : public InputModifier
{
  ScaleE3(NullPoint base) : InputModifier(base) { };
  ScaleE3* clone() const { return new ScaleE3(*this); };
private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
};


struct ScaleE1overE2 : public InputModifier
{
  ScaleE1overE2(NullPoint base) : InputModifier(base) { };
  ScaleE1overE2* clone() const { return new ScaleE1overE2(*this); };
private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
};


struct ScaleEaccordion : public InputModifier
{
  ScaleEaccordion(NullPoint base) : InputModifier(base) { };
  ScaleEaccordion* clone() const { return new ScaleEaccordion(*this); };
private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
};


struct ScaleEcalorimeter : public InputModifier
{
  ScaleEcalorimeter(NullPoint base) : InputModifier(base) { };
  ScaleEcalorimeter* clone() const { return new ScaleEcalorimeter(*this); };
private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
};


class egammaLayerRecalibTool : public asg::AsgMessaging
{
public:
  typedef std::vector<std::pair<InputModifier*, GetAmountBase*> > ModifiersList;
  /**
   * @param tune string to configure the tuning
   - "" the tool has no effect
   - as default it is "current_default"
   - "test1" just for testing
  **/
  egammaLayerRecalibTool(const std::string& name, const std::string& tune);
  egammaLayerRecalibTool(const std::string& tune);
  ~egammaLayerRecalibTool() { clear_corrections(); }

  CP::CorrectionCode applyCorrection(xAOD::Egamma &, const xAOD::EventInfo& event_info) const;

  /**
   * helper to create a tool from a string (useful for command line arguments)
   **/
  static std::pair<std::string, egammaLayerRecalibTool*> create(const std::string& type,
								const std::string& args);

  /**
   * apply layer calibration to the @param inputs
   **/
  CP::CorrectionCode scale_inputs(StdCalibrationInputs & inputs) const;
  /**
   * add custom layer scale correction. Can be called multiple times.
   **/
  void add_scale(InputModifier * modifier, GetAmountBase * amount);
  /**
   * add scale correction from string. Can be called multiple times.
   * The list of valid values is on the twiki
   **/
  void add_scale(const std::string& scale);
  /**
   * remove all the scale corrections
   **/
  void clear_corrections();
private:
  std::string m_tune;
  const std::string resolve_path(std::string filename) const;
  std::string resolve_alias(const std::string& tune) const;
  ModifiersList m_modifiers;
};

#endif // EGAMMA_LAYER_RECALIB_TOOL

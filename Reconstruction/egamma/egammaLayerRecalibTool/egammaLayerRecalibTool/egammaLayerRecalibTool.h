/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <TH1.h>
#include <TH2.h>
#include <TFormula.h>

#include "egammaLayerRecalibTool/corr_HV_EMBPS.h"

struct StdCalibrationInputs
{
  int RunNumber;   // only for HV presampler correction
  float eta;
  float phi;       // only for HV presampler correction
  float E0raw;
  float E1raw;
  float E2raw;
  float E3raw;
  float Rconv;

StdCalibrationInputs() : RunNumber(0), eta(0), phi(0), E0raw(0), E1raw(0), E2raw(0), E3raw(0), Rconv(0) { }
    
StdCalibrationInputs(int run_number, float Eta, float e0_raw, float e1_raw, float e2_raw, float e3_raw, float r_conv)
: RunNumber(run_number), eta(Eta), E0raw(e0_raw), E1raw(e1_raw), E2raw(e2_raw), E3raw(e3_raw), Rconv(r_conv) { }

  StdCalibrationInputs operator/(const StdCalibrationInputs & oth) const {
    StdCalibrationInputs result;
    // using 0 / 0 = 1
    result.eta = (eta != oth.eta ? eta / oth.eta : 1.);
    result.phi = (phi != oth.phi ? phi / oth.phi : 1.);
    result.E0raw = (E0raw != oth.E0raw ? E0raw / oth.E0raw : 1.);
    result.E1raw = (E1raw != oth.E1raw ? E1raw / oth.E1raw : 1.);
    result.E2raw = (E2raw != oth.E2raw ? E2raw / oth.E2raw : 1.);
    result.E3raw = (E3raw != oth.E3raw ? E3raw / oth.E3raw : 1.);
    result.Rconv = (Rconv != oth.Rconv ? Rconv / oth.Rconv : 1.);
    return result;
  };
  StdCalibrationInputs & operator/=(const StdCalibrationInputs & oth) {
    // using 0 / 0 = 1
    if (eta != oth.eta) eta /= oth.eta; else eta = 1.;
    if (phi != oth.phi) phi /= oth.phi; else phi = 1.;
    if (E0raw != oth.E0raw) E0raw /= oth.E0raw; else E0raw = 1.;
    if (E1raw != oth.E1raw) E1raw /= oth.E1raw; else E1raw = 1.;
    if (E2raw != oth.E2raw) E2raw /= oth.E2raw; else E2raw = 1.;
    if (E3raw != oth.E3raw) E3raw /= oth.E3raw; else E3raw = 1.;
    if (Rconv != oth.Rconv) Rconv /= oth.Rconv; else Rconv = 1.;
    return *this;
  };
};


// TODO: add protections for invalid inputs (histogram overflows)
struct GetAmountBase
{
  virtual float operator()(const StdCalibrationInputs & input) const = 0;
  virtual float get_amount_error(const StdCalibrationInputs & /*input*/) const { return 0; };
  virtual ~GetAmountBase() { };
  virtual GetAmountBase* clone() const = 0;
};


// object adaptor (prefer composition vs inheritance)
struct GetAmountHVPSGuillaume : public GetAmountBase
{
  virtual float operator()(const StdCalibrationInputs & input) const;
 private:
  virtual GetAmountHVPSGuillaume* clone() const { return 0; };
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
 GetAmountHisto1D(const TH1& histo) : m_histo(static_cast<TH1*>(histo.Clone())) { m_histo->SetDirectory(0); };
 GetAmountHisto1D(const GetAmountHisto1D & oth) : m_histo(static_cast<TH1*>(oth.m_histo->Clone())) { m_histo->SetDirectory(0); };
  GetAmountHisto1D& operator= (const GetAmountHisto1D & oth){
    if (this != &oth) // protect against invalid self-assignment
      {
	m_histo = static_cast<TH1*>(oth.m_histo->Clone()); 
	m_histo->SetDirectory(0);
      }
    return *this;
  }
  virtual GetAmountHisto1D* clone() const { return new GetAmountHisto1D(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
  virtual float get_amount_error(const StdCalibrationInputs & input) const;
  virtual ~GetAmountHisto1D() { delete m_histo; };
 protected:
  TH1* m_histo;
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
 GetAmountHisto2D(const TH2F& histo) : m_histo(histo) { m_histo.SetDirectory(0); };
  virtual GetAmountHisto2D* clone() const { return new GetAmountHisto2D(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
 protected:
  TH2F m_histo;
};


struct GetAmountFixed : public GetAmountBase
{
 public:
 GetAmountFixed(float amount) : m_amount(amount), m_error (0) { };
 GetAmountFixed(float amount, float error) : m_amount(amount), m_error(error) { };
  virtual GetAmountFixed* clone() const { return new GetAmountFixed(*this); };
  virtual float operator()(const StdCalibrationInputs & input) const;
  virtual float get_amount_error(const StdCalibrationInputs & input) const;
 private:
  float m_amount;
  float m_error;
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
  void operator()(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs rel_error(const StdCalibrationInputs& inputs,
				 float amount, float amount_error) const;
  virtual InputModifier* clone() const = 0;
  virtual ~InputModifier() { };
private:
  InputModifier() { }; // privatize default constructor
  // here we are one based (amount == 1 <=> null scale)
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const=0;
  virtual StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
						 float amount, float amount_error) const;
  NullPoint m_base;
};


struct ScaleE0 : public InputModifier
{
 ScaleE0(NullPoint base) : InputModifier(base) { };
  ScaleE0* clone() const { return new ScaleE0(*this); };
 private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
					 float amount, float amount_error) const;
};


struct ScaleE1 : public InputModifier
{
 ScaleE1(NullPoint base) : InputModifier(base) { };
  ScaleE1* clone() const { return new ScaleE1(*this); };
 private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
					 float amount, float amount_error) const;
};


struct ScaleE2 : public InputModifier
{
 ScaleE2(NullPoint base) : InputModifier(base) { };
  ScaleE2* clone() const { return new ScaleE2(*this); };
 private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
					 float amount, float amount_error) const;
};


struct ScaleE3 : public InputModifier
{
 ScaleE3(NullPoint base) : InputModifier(base) { };
  ScaleE3* clone() const { return new ScaleE3(*this); };
 private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
					 float amount, float amount_error) const;
};


struct ScaleE1overE2 : public InputModifier
{
 ScaleE1overE2(NullPoint base) : InputModifier(base) { };
  ScaleE1overE2* clone() const { return new ScaleE1overE2(*this); };
 private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
					 float amount, float amount_error) const;
};


struct ScaleEaccordion : public InputModifier
{
 ScaleEaccordion(NullPoint base) : InputModifier(base) { };
  ScaleEaccordion* clone() const { return new ScaleEaccordion(*this); };
 private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
					 float amount, float amount_error) const;
};


struct ScaleEcalorimeter : public InputModifier
{
 ScaleEcalorimeter(NullPoint base) : InputModifier(base) { };
  ScaleEcalorimeter* clone() const { return new ScaleEcalorimeter(*this); };
 private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
					 float amount, float amount_error) const;
};


struct ScaleRadius : public InputModifier
{
 ScaleRadius(NullPoint base) : InputModifier(base) { };
  ScaleRadius* clone() const { return new ScaleRadius(*this); };
 private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
					 float amount, float amount_error) const;
};


struct ScaleEta : public InputModifier
{
 ScaleEta(NullPoint base) : InputModifier(base) { };
  ScaleEta* clone() const { return new ScaleEta(*this); };
 private:
  virtual void scale_inputs(StdCalibrationInputs&, float amount) const;
  StdCalibrationInputs compute_rel_error(const StdCalibrationInputs& inputs,
					 float amount, float amount_error) const;
};


class egammaLayerRecalibTool
{
 public:
  typedef std::vector<std::pair<InputModifier*, GetAmountBase*> > ModifiersList;
  /**
   * @param tune string to configure the tuning
   - "" the tool has no effect
   - as default it is "current_default"
   - "test1" just for testing
  **/
  egammaLayerRecalibTool(const std::string& tune);
  ~egammaLayerRecalibTool() { clear_corrections(); }

  /**
   * helper to create a tool from a string (useful for command line arguments)
   **/
  static std::pair<std::string, egammaLayerRecalibTool*> create(const std::string& type, 
								const std::string& args);

  /**
   * apply layer calibration to the @param inputs
   **/
  void scale_inputs(StdCalibrationInputs & inputs) const;   
  /**
   * return relative errors
   **/
  StdCalibrationInputs rel_error(const StdCalibrationInputs & inputs) const;
  /**
   * return the multiplicative corrections
   **/
  StdCalibrationInputs values(const StdCalibrationInputs & inputs) const;
  /**
   * apply layer calibration and return multiplicative corrections
   **/
  StdCalibrationInputs scale_and_values(StdCalibrationInputs & inputs) const;    
  /**
   * add custom layer scale correction. Can be called multiple times.
   **/
  void add_scale(InputModifier * modifier,
		 GetAmountBase * amount);
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
  const std::string resolve_path(std::string filename) const;
  const std::string& resolve_alias(const std::string& tune) const;
  ModifiersList m_modifiers;
};

#endif // EGAMMA_LAYER_RECALIB_TOOL

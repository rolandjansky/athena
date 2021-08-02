/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/Data.h"
#include "TMath.h"

#include "LArCafJobs/SimpleShape.h"
#include "LArSamplesMon/MonitorBase.h"
#include "LArSamplesMon/History.h"
#include "LArCafJobs/CellInfo.h"
#include "LArSamplesMon/ShapeFitter.h"
#include "LArCafJobs/EventData.h"
#include "LArSamplesMon/ShapeErrorData.h"
#include "LArSamplesMon/OFC.h"
#include "LArSamplesMon/ClassCounts.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

using namespace LArSamples;


double Data::m_timeShift = 0;

int Data::run()       const { return eventData()->run(); }
int Data::event()     const { return eventData()->event(); }
int Data::lumiBlock() const { return eventData()->lumiBlock(); }
int Data::bunchId()   const { return eventData()->bunchId(); }

Data::Data(const DataContainer& container, const EventData& eventData, const History* history, int index, bool takeOwnership)
   : AbsShape(), m_container(takeOwnership ? &container : new DataContainer(container)), m_eventData(&eventData),
     m_history(history), m_index(index) 
{
  ClassCounts::incrementInstanceCount("Data"); 
}


Data::Data(const Data& other, const EventData* eventData, const History* history, int index) 
  : AbsShape(), m_container(new DataContainer(other.container())), 
    m_eventData(eventData ? eventData : other.eventData()),
    m_history(history ? history : other.m_history), m_index(index >= 0 ? index : other.m_index)
{ 
  ClassCounts::incrementInstanceCount("Data"); 
}


Data::~Data() 
{ 
  ClassCounts::decrementInstanceCount("Data"); 
  delete m_container;
}

      
const DataContainer* Data::dissolve()
{
  const DataContainer* container = new DataContainer(*m_container);
  delete this;
  return container;
}


double Data::noise() const 
{ 
  return (m_container->corrs().size() == 0 ? 0 : TMath::Sqrt(m_container->corrs()[0]));
}

double Data::adcMax_new() const
{
  return ((energy()/noise())*pedestalRMS());
 
}

double Data::covariance(unsigned int i, unsigned int j) const
{
  if (i >= nSamples() || j >= nSamples()) return 0;
  const std::vector<float>& corrs = m_container->corrs();
  unsigned int index = TMath::Abs(int(i) - int(j));  // we use time-translation-invariant correlations...
  if (index >= corrs.size()) return 0;
  double corr = corrs[index];
  return corr*pedestalRMS()*pedestalRMS()/(noise()*noise()); // hack to correct the noise per sample/ADC_max noise error
}


double Data::covarianceMeV(unsigned int i, unsigned int j) const
{
  double adc2mev = adc2MeV();
  if (adc2mev < 0) return 0;
  return covariance(i,j)*adc2mev*adc2mev; // we use time-translation-invariant correlations...
  //return (i == j ? m_corrs[0] : 0); // temporary
}


std::vector<float> Data::pedestalSubtractedSamples() const
{
  std::vector<float> samples;
  
  for (unsigned int i = 0; i < nSamples(); i++)
    samples.push_back(pedestalSubtractedSample(i));
  
  return samples;
}


bool Data::isDisconnected() const
{
  return ((status() >> 16) & 0x1) != 0;
}

bool Data::febError(LArFebErrorSummary::LArFebErrorType error) const
{
  return ((status() >> (error + 17)) & 0x1) != 0;
}

TString Data::str(LArFebErrorSummary::LArFebErrorType error)
{
  if (error == LArFebErrorSummary::Parity)         return "Parity";
  if (error == LArFebErrorSummary::BCID)           return "BCID";
  if (error == LArFebErrorSummary::SampleHeader)   return "SampleHeader";
  if (error == LArFebErrorSummary::EVTID)          return "EVTID";
  if (error == LArFebErrorSummary::ScacStatus)     return "ScacStatus";
  if (error == LArFebErrorSummary::ScaOutOfRange)  return "ScaOutOfRange";
  if (error == LArFebErrorSummary::GainMismatch)   return "GainMismatch";
  if (error == LArFebErrorSummary::TypeMismatch)   return "TypeMismatch";
  if (error == LArFebErrorSummary::NumOfSamples)   return "NumOfSamples";
  if (error == LArFebErrorSummary::EmptyDataBlock) return "EmptyDataBlock";
  if (error == LArFebErrorSummary::DspBlockSize)   return "DspBlockSize";
  if (error == LArFebErrorSummary::CheckSum)       return "CheckSum";
  if (error == LArFebErrorSummary::MissingHeader)  return "MissingHeader";
  return "";
}


TString Data::problems(bool sayNone) const
{
  if (isDisconnected()) return "DISCONNECTED";

/*   
  TString problems = "";
  for (CaloBadChannel::CALOPROBLEM pb = CaloBadChannel::DEAD; pb < CaloBadChannel::NPB; pb = CaloBadChannel::CALOPROBLEM(pb + 1))
    if (problem(pb)) {
      if (problems != "") problems += " ";
      problems += str(pb);
    }
*/  

  TString problems = "";
  if (problems = "" && sayNone) problems = "None";
  return problems;
}


bool Data::isValid() const
{
  if (!m_container) return false;
  if (!container().isValid()) return false;
  return true;
}


short Data::maxSample() const
{
  int maxPos = maxPosition();
  if (maxPos >= 0) return sample(maxPos);
  return -1;
}

short Data::minSample() const
{
  int minPos = minPosition();
  if (minPos >= 0) return sample(minPos);
  return -1;
}


SimpleShape* Data::timeAlignedShape() const
{
  return new SimpleShape(*this, 1, -ofcTime());
}


SimpleShape* Data::referenceShape() const
{
  if (!m_history) return 0;
  return m_history->referenceShape(m_index);
}


const ScaledErrorData* Data::scaledErrorData() const
{
  if (!m_history) return 0;
  return m_history->scaledErrorData(m_index);
}


double Data::chi2() const
{
  if (!m_history) return 0;
  return m_history->chi2(m_index);
}


double Data::_chi2_noCorr(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->chi2(m_index, -1, -1, DefaultChi2, NoShapeError);
}


double Data::_chi2_ringCorr(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->chi2(m_index, -1, -1, DefaultChi2, RingShapeError);
}


double Data::_chi2_cellCorr(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->chi2(m_index, -1, -1, DefaultChi2, CellShapeError);
}


double Data::_chi2_k(const DataFuncArgs& args) const
{
  if (!m_history) return 0;
  return m_history->chi2_k(m_index, args.x1);
}


bool Data::calcRefit(double& chi2, double& k, double& dT) const
{
  ShapeFitter fitter;
  //cout << "We're refitting " << _i << " of " << _history->cellInfo()->location(1) << endl;
  SimpleShape* reference = referenceShape();
  const ScaledErrorData* sed = scaledErrorData();
  if (!reference) return false;
  bool result = fitter.fit(*this, *reference, k, dT, chi2, sed);
  delete reference;
  return result;
}


double Data::_refitDeltaT(const DataFuncArgs&) const
{
  double chi2, k, dT;
  if (!calcRefit(chi2, k, dT)) return -9999;
  return dT;
}


double Data::_refitScale(const DataFuncArgs&) const
{
  double chi2, k, dT;
  if (!calcRefit(chi2, k, dT)) return -1;
  return k;
}


double Data::_refitChi2(const DataFuncArgs&) const
{
  double chi2, k, dT;
  if (!calcRefit(chi2, k, dT)) return -1;
  return chi2;
}


bool Data::calcAdjust(double& k, double& dT) const
{
  if (!m_history) return false;
  OFC* ofc = m_history->ofc(m_index);
  if (!ofc) return false;
  
  k = ofc->A(*this)/adcMax();
  dT = ofc->time(*this);
  delete ofc;
  return true;
}


double Data::_adjDeltaT(const DataFuncArgs&) const
{
  double k, dT;
  if (!calcAdjust(k, dT)) return -9999;
  return dT;
}


double Data::_adjScale(const DataFuncArgs&) const
{
  double k, dT;
  if (!calcAdjust(k, dT)) return -1;
  return k;
}


int Data::_iEta(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->cellInfo()->iEta();
}


int Data::_iPhi(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->cellInfo()->iPhi();
}


int Data::_region(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->cellInfo()->region();
}


int Data::_calo(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->cellInfo()->calo();
}


int Data::_layer(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->cellInfo()->layer();
}


int Data::_feedThrough(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->cellInfo()->feedThrough();
}


int Data::_slot(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->cellInfo()->slot();
}


int Data::_channel(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->cellInfo()->channel();
}


int Data::_hash(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->hash();
}


int Data::_ring(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->cellInfo()->globalPhiRing();
}


int Data::_nData(const DataFuncArgs&) const
{
  if (!m_history) return 0;
  return m_history->nData();
}


double Data::_timeNoTOF(const DataFuncArgs&) const
{
  if (!m_history) return -9999;
  return ofcTime() + m_history->cellInfo()->position().Mag()/(TMath::C()*1E-6); // 10^-6 to convert to mm/ns
}


double Data::_timeForSplash1(const DataFuncArgs& args) const
{
  if (!m_history) return -9999;
  return _timeNoTOF(args) + m_history->cellInfo()->position().z()/(TMath::C()*1E-6); // 10^-6 to convert to mm/ns
}


double Data::_timeForSplash2(const DataFuncArgs& args) const
{
  if (!m_history) return -9999;
  return _timeNoTOF(args) - m_history->cellInfo()->position().z()/(TMath::C()*1E-6); // 10^-6 to convert to mm/ns
}


double Data::_peakSignif(const DataFuncArgs&) const
{
  return maxValue()/pedestalRMS();
}


double Data::_energySignif(const DataFuncArgs&) const
{
  return energy()/noise();
}


TVectorD Data::deltas() const
{
  if (!m_history) return TVectorD();
  return m_history->deltas(m_index);
}


TVectorD Data::residuals() const
{
  if (!m_history || adcMax() < 0) return TVectorD();
  TVectorD resv = m_history->deltas(m_index);
  for (int i = 0; i < resv.GetNrows(); i++) resv(i) /= adcMax();
  return resv;
}


double Data::delta(short sample) const
{
  double val = 0;
  if (!m_history || !m_history->delta(m_index, sample, val)) return -9999;
  return val;
}


double Data::residual(short sample) const
{
  double val = 0;
  if (!m_history || adcMax() < 0 || !m_history->delta(m_index, sample, val)) return -9999;
  return val/adcMax();
}


double Data::resCorrN(ShapeErrorType type) const
{
  if (!m_history) return -999;
  const ShapeErrorData* sed = m_history->shapeErrorData(gain(), type);
  return (sed ? sed->n() : 0);
}


double Data::residualOffset(short sample, bool scale) const
{
  double val = 0;
  if (!m_history || !m_history->residualOffset(m_index, sample, val, (scale ? -1 : 1))) return -9999;
  return val;
}


double Data::xi(short sample, ShapeErrorType type, CaloGain::CaloGain g, bool xip) const
{
  if (!m_history) return -999;
  //njpb don't see the point of next line, commenting out
  //if (g != CaloGain::UNKNOWNGAIN && type != RingShapeError) type = CellShapeError;
  const ShapeErrorData* sed = m_history->shapeErrorData(g == CaloGain::UNKNOWNGAIN ? gain() : g, type);
  if (!sed) return -999;
  double val = (sed->isInRange(sample) ? (xip ? sed->xip()(sample) : sed->xi()(sample)) : -999);
  delete sed;
  return val;
}


double Data::xiNorm(ShapeErrorType type) const
{
  if (!m_history) return -1;
  const ShapeErrorData* sed = m_history->shapeErrorData(gain(), type);
  if (!sed) return -1;
  double val = TMath::Sqrt(sed->xi().Norm2Sqr());
  delete sed;
  return val;
}


double Data::xip(short sample, ShapeErrorType type, CaloGain::CaloGain g) const
{
  return xi(sample, type, g, true);
}


double Data::residualError(short sample1, short sample2, bool scale) const
{
  double val = 0;
  if (!m_history || !m_history->residualError(m_index, sample1, sample2, val, scale)) return -9999;
  return val;
}


double Data::_x(const DataFuncArgs&)   const { return  m_history->cellInfo()->position().X(); }
double Data::_y(const DataFuncArgs&)   const { return  m_history->cellInfo()->position().Y(); }
double Data::_z(const DataFuncArgs&)   const { return  m_history->cellInfo()->position().Z(); }
double Data::_rt(const DataFuncArgs&)  const { return  m_history->cellInfo()->rt(); }
double Data::_eta(const DataFuncArgs&) const { return  m_history->cellInfo()->eta(); }
double Data::_phi(const DataFuncArgs&) const { return  m_history->cellInfo()->phi(); }


TString Data::gainStr(CaloGain::CaloGain gain) 
{
  if (gain == CaloGain::LARHIGHGAIN) return "HIGH";
  if (gain == CaloGain::LARMEDIUMGAIN) return "MEDIUM";
  if (gain == CaloGain::LARLOWGAIN) return "LOW";
  return "";
}


TString Data::description(unsigned int verbosity) const
{
  TString desc = "";
  desc += Form("%d samples, ", nSamples());
  desc += "gain = " + gainStr(gain()) + ", ";
  desc += Form("energy = %6.1f MeV, ", energy());
  if (verbosity >= 2) desc += Form("ADCMax = %6.1f, ", adcMax());
  desc += Form("time = %6.1f ns, ", ofcTime());
  if (verbosity >= 2) desc += Form("chi^2/n = %6.1f, ", chi2());
  desc += Form("run = %d, event = %d, ", run(), event());
  desc += Form("problems = %s", problems(true).Data());
  return desc;
}


bool Data::isPassed(const TString& bitName) const 
{ 
  return (eventData() ? eventData()->isPassed(bitName) : false); 
}


bool Data::goodForShapeCorr() const
{
  if (adcMax() < 0) return false;
 
  CaloId caloId = (m_history ? m_history->cellInfo()->calo()  : UNKNOWN_CALO);
  //short   layer = (m_history ? m_history->cellInfo()->layer() : -1);

  //if (Id::matchCalo(caloId, FCAL)       && energy()/noise() < 250) return false;
  if (Id::matchCalo(caloId, FCAL)) return false;
  if (Id::matchCalo(caloId, HEC)        && energy()/noise() < 20) return false;
  if (Id::matchCalo(caloId, EMEC_INNER) && energy()/noise() < 250) return false;
  if (Id::matchCalo(caloId, EMEC_OUTER) && energy()/noise() < 50) return false;
  if (Id::matchCalo(caloId, EMB)        && energy()/noise() < 30) return false;

  if (Id::matchCalo(caloId, HEC)) {
    if (TMath::Abs(ofcTime()) > 10) return false;
  }
  else {
    if (TMath::Abs(ofcTime()) > 5) return false;
  }

  return true;
}

double Data::upstreamEnergy() const
{
  if (!m_history) return -1;
  return m_history->upstreamEnergy(m_index);
}

double Data::_ofcSigma(const DataFuncArgs& args) const
{
  double result = -9999;
  if (!m_history) return result;
  OFC* ofc = m_history->ofc(m_index);
  if (ofc->isInRange(args.i1))
    result = TMath::Sqrt(ofc->Gamma()(args.i1, args.i1));
  delete ofc;
  return result;
}


double Data::_ofcGamma(const DataFuncArgs& args) const
{
  double result = -9999;
  if (!m_history) return result;
  OFC* ofc = m_history->ofc(m_index);
  if (ofc->isInRange(args.i1) && ofc->isInRange(args.i2))
    result = ofc->Gamma()(args.i1, args.i2);
  delete ofc;
  return result;
}
      
      
double Data::_upstreamERatio(const DataFuncArgs&) const 
{ 
  double upe = upstreamEnergy();
  if (upe < 0 || energy() == 0) return -1;
  return upe/energy(); 
}


double Data::_chi2Anomaly(const DataFuncArgs& args) const
{
  if (!m_history) return -1;
  return m_history->chi2Anomaly(args.x1);
}

double Data::timeClCorr() const
{  
  if (!m_history) return -9999.;
  Double_t timeClCorr=-99.;
  
  if(run()==203636) timeClCorr = ofcTime() + 0.04;
  else if(run()==203680) timeClCorr = ofcTime() + 0.109155;
  else if(run()==203719) timeClCorr = ofcTime() + 0.141762;
  else if(run()==203745) timeClCorr = ofcTime() - 0.107597;
  else if(run()==203779) timeClCorr = ofcTime() - 0.0032581;
  else if(run()==203876) timeClCorr = ofcTime() + 0.109328;
  else if(run()==203934) timeClCorr = ofcTime() - 0.0404719;
  else if(run()==204073) timeClCorr = ofcTime() - 0.0244807;
  else if(run()==204153) timeClCorr = ofcTime() + 0.185949;
  else if(run()==204158) timeClCorr = ofcTime() + 0.0383347;
  else if(run()==204240) timeClCorr = ofcTime() + 0.0635586;
  else if(run()==204265) timeClCorr = ofcTime() + 0.13898;
  else if(run()==204416) timeClCorr = ofcTime() + 0.1074;
  else if(run()==204564) timeClCorr = ofcTime() + 0.0369925;
  else if(run()==210302) timeClCorr = ofcTime() - 0.2;
  else if(run()==210308) timeClCorr = ofcTime() - 0.2;
  else timeClCorr = ofcTime();

  return timeClCorr;  
}  

double Data::timeECorr() const
{  
  if (!m_history) return -9999.;
  Double_t timeCorr=-99.;  
      
  //   Int_t calo =  m_history->cellInfo()->calo();
  //   Double_t p0, p1, p2, p3;
  
  timeCorr=ofcTime();
  
  return timeCorr;  
}  

double Data::_timeECorr(const DataFuncArgs&) const
{
  // Modify this to add Isabelle's correction
  return ofcTime();
}

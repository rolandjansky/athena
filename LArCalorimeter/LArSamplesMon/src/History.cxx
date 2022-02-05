/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/History.h"

#include "LArCafJobs/CellInfo.h"
#include "LArCafJobs/ShapeInfo.h"

#include "LArSamplesMon/AbsShapeErrorGetter.h"
#include "LArSamplesMon/Data.h"
#include "LArCafJobs/SimpleShape.h"
#include "LArSamplesMon/GraphShape.h"
#include "LArSamplesMon/ShapeDrawer.h"
#include "LArSamplesMon/ShapeFitter.h"
#include "LArSamplesMon/DataTweaker.h"
#include "LArSamplesMon/OFC.h"
#include "LArCafJobs/Definitions.h"
#include "LArSamplesMon/ScaledShiftedShape.h"
#include "LArSamplesMon/Averager.h"
#include "LArSamplesMon/ShapeErrorData.h"
#include "LArSamplesMon/ScaledErrorData.h"
#include "LArSamplesMon/UniformShapeErrorGetter.h"
#include "LArSamplesMon/CombinedShapeErrorGetter.h"
#include "LArSamplesMon/Residual.h"
#include "LArCafJobs/Geometry.h"
#include "LArSamplesMon/Interface.h"
#include "LArSamplesMon/ClassCounts.h"

#include "TMath.h"
#include <iostream>

using std::cout;
using std::endl;

using namespace LArSamples;


History::History(const HistoryContainer& container, 
                 const std::vector<const EventData*>& eventData, unsigned hash, 
                 const AbsShapeErrorGetter* shapeErrorGetter)
  : m_cellInfo(*container.cellInfo()),
    m_eventData(eventData),
    m_hash(hash), m_shapeErrorGetter(shapeErrorGetter)
{
  ClassCounts::incrementInstanceCount("History");
  if (container.nDataContainers() != eventData.size()) return;
  for (unsigned int i = 0; i < container.nDataContainers(); i++) 
    m_data.push_back(new Data(*container.dataContainer(i), *eventData[i], this, i));
}


History::History(const std::vector<const Data*>& data, const CellInfo& info, 
		 const std::vector<const EventData*>& eventData, 
		 unsigned int hash, const AbsShapeErrorGetter* shapeErrorGetter)
  : m_data(data), m_cellInfo(info), m_eventData(eventData), m_hash(hash), 
    m_shapeErrorGetter(shapeErrorGetter) 
{
  ClassCounts::incrementInstanceCount("History");
  unsigned int i = 0;
  for (const Data* data : m_data)
    data->setCallBacks(this, i);
}
      

History::~History()
{
  ClassCounts::decrementInstanceCount("History");
  for (const Data* data : m_data)
    delete data;
  for (const EventData* eventData : m_eventData)
    delete eventData;  
}


HistoryContainer* History::dissolve()
{
  HistoryContainer* histCont = new HistoryContainer(new CellInfo(*cellInfo()));
  for (unsigned int k = 0; k < nData(); k++) {
    Data* newData = new Data(*data(k));
    histCont->add(newData->dissolve());
  }
  delete this;
  return histCont;
}


const Data* History::data(unsigned int i) const 
{ 
  if (i >= nData()) return nullptr;
  return m_data[i]; 
}


const Data* History::data_for_event(int event, int run) const
{
  for (unsigned int k = 0; k < nData(); k++) {
    if (data(k)->event() != event) continue;
    if (run > 0 && data(k)->run() != run) continue;
    return data(k);
  }
  return nullptr;
}


const Data* History::data_for_event(const EventData& eventData) const
{
  // turn off caching

  //if (m_dataForEvent.size() == 0) 
  //  for (unsigned int k = 0; k < nData(); k++)
  //    m_dataForEvent[data(k)->event()] = std::make_pair(data(k)->run(), data(k));

  //std::unordered_map<int, std::pair<int, const Data*> >::const_iterator
  //  data = m_dataForEvent.find(eventData.event());
  //if (data == m_dataForEvent.end()) return 0;
  //if (data->second.first != eventData.run()) return data_for_event(eventData.event(), eventData.run()); // wrong run: revert to pedestrian method...
  //return data->second.second;

  return data_for_event(eventData.event(), eventData.run());
}


bool History::sum(SimpleShape*& sum, SimpleShape*& reference) const
{
  reference = sum = nullptr;
  if (nData() == 0) return false;
  
  sum = new SimpleShape(m_data[0]->nSamples());
  reference = nullptr;

  for (unsigned int j = 0; j < nData(); j++) {
    if (sum->nPoints() !=  m_data[j]->nSamples()) return false;
    for (unsigned int k = 0; k < m_data[j]->nSamples(); k++)
      sum->add(k, m_data[j]->pedestalSubtractedSample(k), m_data[j]->error(k));
    SimpleShape* thisRef = referenceShape(j);
    if (!thisRef) { delete sum; return false; }
    if (reference) {
      reference->add(*thisRef);
      delete thisRef;
    }
    else reference = thisRef;
  }
    
  return true;
}


bool History::isValid() const
{
  if (!m_cellInfo.isValid()) return false;
  if (nData() == 0) return false;
  
  for (const Data* data : m_data)
    if (!data->isValid()) return false;
  
  return true;
}


double History::chi2(int i, int lwb, int upb, int chi2Params, ShapeErrorType shapeErrorType, unsigned int* nDof) const
{
  SimpleShape* reference = referenceShape(i);
  if (!reference) return -1;
  Chi2Calc c2c(chi2Params);
  if ( m_data[i]->isDisconnected()) return -1;
  const ScaledErrorData* sea = scaledErrorData(i, -1, Definitions::none, shapeErrorType);
  if (!sea && shapeErrorType != NoShapeError && shapeErrorType != BestShapeError) return -1;
  double chi2Value = c2c.chi2(*m_data[i], *reference, sea, lwb, upb);
  if (nDof) *nDof = c2c.nDof();
  if (sea) delete sea;
  delete reference;
  return chi2Value;
}


double History::chi2_k(int i, double k, int lwb, int upb, int chi2Params) const
{
  const AbsShapeErrorGetter* oldGetter = shapeErrorGetter();
  UniformShapeErrorGetter* uniGetter = new UniformShapeErrorGetter(k);
  CombinedShapeErrorGetter* combGetter = new CombinedShapeErrorGetter();
  if (oldGetter) combGetter->add(*oldGetter);
  combGetter->add(*uniGetter);
  setShapeErrorGetter(combGetter);
  double chi2Value = chi2(i, lwb, upb, chi2Params);
  setShapeErrorGetter(oldGetter);
  delete combGetter;
  delete uniGetter;
  return chi2Value;
}


double History::maxChi2(int lwb, int upb, int chi2Params) const
{
  double maxChi2 = -2;  
  for (unsigned int i = 0; i < nData(); i++) {
    double chi2Value = chi2(i, lwb, upb, chi2Params);
    if (chi2Value > maxChi2) maxChi2 = chi2Value;
  }
  return maxChi2;
}


OFC* History::ofc(unsigned int k, int lwb, int upb, double time, bool withAutoCorr) const
{
  if (k >= nData()) return nullptr;
  if (Definitions::isNone(time)) {
    time = data(k)->ofcTime();
    //cout << "Using reference time = " << time << endl;
  }
  SimpleShape* reference = referenceShape(k, 1, time); // ADC=1 : we need a normalized shape
  if (!reference) return nullptr;
  OFC* result = new OFC(*reference, *m_data[k], lwb, upb, shapeErrorData(CaloGain::LARHIGHGAIN), withAutoCorr); // FixMe
  delete reference;
  if (!result) return nullptr;
  if (result->g().GetNrows() == 0) { delete result; result = nullptr; }
  return result;
}


bool History::refVal(unsigned int k, unsigned int sample, double& val, double& err) const
{
  if (k >= nData()) return false;
  SimpleShape* reference = referenceShape(k);
  if (!reference) return false;  
  if (reference->interpolate(m_data[k]->time(sample), val, err) != 0) { delete reference; return false; }
  delete reference;
  return true;
}


History* History::refit(Chi2Params pars) const
{
  std::vector<const Data*> datas;
  DataTweaker tw;
  tw.setRefit(true);
  tw.setFitParams(pars);

  for (unsigned int j = 0; j < nData(); j++) {
    Data* refitData = tw.tweak(*data(j));
    if (!refitData) {
      for (unsigned int k = 0; k < datas.size(); k++) delete datas[k];
      return nullptr;
    }
    datas.push_back(refitData);
  }

  std::vector<const EventData*> eventData;
  for (const EventData* event : m_eventData)
    eventData.push_back(new EventData(*event));

  return new History(datas, *cellInfo(), eventData, hash(), shapeErrorGetter());
}


History* History::adjust() const
{
  std::vector<const Data*> datas;
  DataTweaker tw;
  tw.setAdjust(true);

  for (unsigned int j = 0; j < nData(); j++) {
    Data* newData = tw.tweak(*data(j));
    if (!newData) {
      for (unsigned int k = 0; k < datas.size(); k++) delete datas[k];
      return nullptr;
    }
    datas.push_back(newData);
  }

  std::vector<const EventData*> eventData;
  for (const EventData* event : m_eventData)
    eventData.push_back(new EventData(*event));

  return new History(datas, *cellInfo(), eventData, hash(), shapeErrorGetter());
}


History* History::filter(const TString& cuts) const
{
  FilterParams f;
  if (!f.set(cuts)) return nullptr;
  
  std::vector<const Data*> datas;

  for (unsigned int j = 0; j < nData(); j++) {
    if (!f.pass(hash(), *this, j)) continue;
    datas.push_back(new Data(*data(j)));
  }

  std::vector<const EventData*> eventData;
  for (const EventData* event : m_eventData)
    eventData.push_back(new EventData(*event));

  return new History(datas, *cellInfo(), eventData, hash(), shapeErrorGetter());
}


const ShapeErrorData* History::shapeErrorData(CaloGain::CaloGain gain, ShapeErrorType shapeErrorType, const Residual* res) const
{
  if (shapeErrorType == NoShapeError || !shapeErrorGetter()) return nullptr;
  if (shapeErrorType == BestShapeError) {
    for (unsigned int i = 0; i < NShapeErrorTypes; i++) {
      const ShapeErrorData* sed = shapeErrorData(gain, (ShapeErrorType)i, res);
      if (sed) return sed;
    }
    return nullptr;
  }
  
  if (shapeErrorType == CellShapeError) {
    const ShapeErrorData* sed = shapeErrorGetter()->shapeErrorData(hash(), gain, res);
    if (!sed) return nullptr;
    sed->setShapeErrorType(CellShapeError);
    return sed;
  }
  
  if (shapeErrorType == LowGainCellShapeError || shapeErrorType == MedGainCellShapeError || shapeErrorType == HighGainCellShapeError) {
    CaloGain::CaloGain fbGain = (shapeErrorType == LowGainCellShapeError ? CaloGain::LARLOWGAIN :
                                (shapeErrorType == MedGainCellShapeError ? CaloGain::LARMEDIUMGAIN :CaloGain::LARHIGHGAIN));
    const ShapeErrorData* sed = shapeErrorGetter()->shapeErrorData(hash(), fbGain, res);
    if (!sed) return nullptr;
    sed->setShapeErrorType(shapeErrorType);
    return sed;
  }
  
  if (shapeErrorType == RingShapeError) {
    const ShapeErrorData* sed = shapeErrorGetter()->phiSymShapeErrorData(cellInfo()->globalPhiRing(), gain, res);
    if (!sed) return nullptr;
    sed->setShapeErrorType(RingShapeError);
    return sed;
  }

  if (shapeErrorType == LowGainRingShapeError || shapeErrorType == MedGainRingShapeError || shapeErrorType == HighGainRingShapeError) {
    CaloGain::CaloGain fbGain = (shapeErrorType == LowGainRingShapeError ? CaloGain::LARLOWGAIN :
                                (shapeErrorType == MedGainRingShapeError ? CaloGain::LARMEDIUMGAIN :CaloGain::LARHIGHGAIN));
    const ShapeErrorData* sed = shapeErrorGetter()->phiSymShapeErrorData(cellInfo()->globalPhiRing(), fbGain, res);
    if (!sed) return nullptr;
    sed->setShapeErrorType(shapeErrorType);
    return sed;
  }
 
  return nullptr;
}


const ScaledErrorData* History::scaledErrorData(unsigned int k, double adcMax, double time,
                                                ShapeErrorType shapeErrorType) const
{
  if (shapeErrorType == NoShapeError || !shapeErrorGetter()) return nullptr;
  if (k >= nData() || data(k)->adcMax() <= 0) return nullptr;

  const ShapeErrorData* sed = shapeErrorData(data(k)->gain(), shapeErrorType, residual(k, false));
  if (!sed) return nullptr;

  double sf = (adcMax < 0 ? data(k)->adcMax() : adcMax);
  double ts = (Definitions::isNone(time) ? data(k)->ofcTime() : time);  
  ScaledErrorData* sced = new ScaledErrorData(*sed, sf, ts);
  delete sed;
  return sced;
}


bool History::delta(unsigned int k, unsigned int sample, double& del) const
{
  if (k >= nData()) return 0;
  TVectorD dv = deltas(k, sample, sample);
  if (dv.GetNrows() == 0) return false;
  del = dv[sample];
  return true;
}


TVectorD History::deltas(unsigned int k, int lwb, int upb, bool correct) const
{
  if (k >= nData()) return TVectorD();
  SimpleShape* reference = referenceShape(k);
  if (!reference) return TVectorD();  
  Chi2Calc c2c;
  CovMatrix errors;
  const ScaledErrorData* sea = (correct ? scaledErrorData(k) : nullptr);
  TVectorD dv = c2c.deltas(*data(k), *reference, errors, sea, lwb, upb);
  if (sea) delete sea;
  delete reference;
  return dv;
}


bool History::residualOffset(unsigned int k, short sample, double& offset, double adcMax, double time) const
{
  const ScaledErrorData* sea = scaledErrorData(k, adcMax, time);
  if (!sea) return false;
  if (!sea->isInRange(sample)) { delete sea; return false; }
  offset = sea->offsets()(sample);
  delete sea;
  return true;
}


bool History::residualError(unsigned int k, short sample1, short sample2, double& error, double adcMax, double time) const
{
  const ScaledErrorData* sea = scaledErrorData(k, adcMax, time);
  if (!sea) return false;
  if (!sea->isInRange(sample1) || !sea->isInRange(sample2)) { delete sea; return false; }
  error = sea->errors()(sample1, sample2);
  delete sea;
  return true;
}

      
bool History::allShape(GraphShape*& allData, SimpleShape*& allRef) const
{
  allData = nullptr;
  allRef  = nullptr;
  
  for (unsigned int j = 0; j < nData(); j++) {
   if ( m_data[j]->isDisconnected()) continue; 
   if (m_data[j]->adcMax() < 1) continue;
    GraphShape* thisData = new GraphShape(*m_data[j], 1/m_data[j]->adcMax(), -m_data[j]->ofcTime());
    if (allData) {
      allData->add(*thisData);
      delete thisData;
    }
    else allData = thisData;
    SimpleShape* thisRef = referenceShape(j, 1); // normalized to 1, like the data
    if (allRef) {
      allRef->add(*thisRef);
      delete thisRef;
    }
    else allRef = thisRef;
  }

  return true;
}


double History::allChi2(Chi2Params pars) const
{
  Chi2Calc c2c(pars);
  
  GraphShape* allData = nullptr;
  SimpleShape* allRef = nullptr;
  if (!allShape(allData, allRef)) return -1;
  double chi2Value = c2c.chi2(*allData, *allRef);
  delete allData;
  delete allRef;
  return chi2Value;
}


bool History::drawWithReference(int k, const TString& atlasTitle) const
{
  if ((unsigned int)k >= nData()) return false;
  SimpleShape* refShape = referenceShape(k);
  SimpleShape* smpShape = referenceShape(k, -1, Definitions::none, true);
  
  if (!refShape || !smpShape) return false;
  int pars = DataFirst | Legend;
  TString title = "";
  if (atlasTitle != "") { // Make sure AtlasStyle is set in ROOT, otherwise will not be as pretty...
    pars |= AtlasStyle;
    title = atlasTitle;
  }
  else 
    title = Form("%s, run %d, event %d", cellInfo()->location(1).Data(), m_data[k]->run(), m_data[k]->event());
  ShapeDrawer drawer(pars);
  bool result = drawer.draw(title, m_data[k], refShape, smpShape);
  delete refShape;
  delete smpShape;
  return result;
}


bool History::drawSumWithReference() const
{
  SimpleShape* dataShape, *refShape;
  if (!sum(dataShape, refShape)) return false;
    
  ShapeDrawer drawer(DataFirst | Legend);
  return drawer.drawAndDelete("", dataShape, refShape);
}


bool History::drawAllWithReference(bool doRefit) const
{  
  if (!nData()) {
    cout << "No data" << endl;
    return false;
  }
  
  if (doRefit) {
    History* refitted_history = refit(DefaultChi2);
    if (!refitted_history) return false;
    bool result = refitted_history->drawAllWithReference(false);
    delete refitted_history;
    return result;
  }
  
  // Use the shape of the gain of the first pulse...
  SimpleShape* refShape = referenceShape(0, 1000, 0);
  SimpleShape* smpShape = referenceShape(0, 1000, 0, true);
  std::vector<const AbsShape*> shapes;
  
  for (unsigned int i = 0; i < nData(); i++) {
    if (m_data[i]->adcMax() < 1) continue;
    SimpleShape* shape = new SimpleShape(*m_data[i], 1000/m_data[i]->adcMax(), -m_data[i]->ofcTime());
    const ScaledErrorData* sed = scaledErrorData(i, 1000);
    if (sed) {
      for (unsigned int k = 0; k < shape->nPoints(); k++)
        if (sed->isInRange(k))
          shape->set(k, shape->value(k) - sed->offsets()(k));
      delete sed;
    }
    shapes.push_back(shape);
  }
  
  ShapeDrawer drawer(Legend);
  return drawer.drawAndDelete(Form("%s (Normalized Shape, max at 1000)", cellInfo()->location(2).Data()), shapes, refShape, smpShape);
}


bool History::drawResiduals(int k, bool errors, bool rescale) const
{  
  std::vector<const AbsShape*> shapes;
  for (unsigned int i = 0; i < nData(); i++) {
    if (m_data[i]->adcMax() < 1) continue;
    if (k >= 0 && k != (int)i) continue;
    SimpleShape* shape = deltaShape(i);
    if (!errors) 
      for (unsigned int k = 0; k < shape->nPoints(); k++) shape->setError(k, 0);
    if (!shape) continue;
    if (rescale) {
      SimpleShape* scaled = new SimpleShape(*shape, 1/m_data[i]->adcMax(), 9.0*i/nData() - m_data[i]->ofcTime());
      delete shape;
      shape = scaled;
    }
    shapes.push_back(shape);
  }
  ShapeDrawer drawer(DataFirst);
  TString title = (rescale ? "Normalized " : "") + TString("residuals for %s");
  return drawer.drawAndDelete(Form(title.Data(), cellInfo()->location(2).Data()), shapes);  
}


SimpleShape* History::referenceShape(unsigned int k, double adcMax, double time, 
                                                             bool samplesOnly) const
{
  if (!cellInfo()->shape(m_data[k]->gain())) return nullptr;
  if (adcMax < 0) adcMax = m_data[k]->adcMax();
  if (Definitions::isNone(time)) time = m_data[k]->ofcTime();
  SimpleShape* shape = new SimpleShape(*cellInfo()->shape(m_data[k]->gain()), adcMax, time, samplesOnly);
  return shape;
}


SimpleShape* History::deltaShape(unsigned int k, int lwb, int upb) const
{
  if (k >= nData()) return nullptr;
  SimpleShape* reference = referenceShape(k);
  if (!reference) return nullptr;  
  Chi2Calc c2c;
  CovMatrix errors;
  const ScaledErrorData* sea = scaledErrorData(k);
  TVectorD dv = c2c.deltas(*data(k), *reference, errors, sea, lwb, upb);
  if (sea) delete sea;
  delete reference;
  if (dv.GetNrows() == 0) return nullptr;
  SimpleShape* shape = new SimpleShape(dv.GetNrows(), Definitions::samplingInterval, data(k)->time(c2c.lwb()) + data(k)->ofcTime());
  for (int l = c2c.lwb(); l <= c2c.upb(); l++) shape->set(l - c2c.lwb(), dv(l), TMath::Sqrt(errors(l, l)));
  return shape;
}


TString History::description(unsigned int verbosity) const
{
  TString desc = "";
  for (unsigned int i = 0; i < nData(); i++)
    desc += Form("  #%-2d : ", i) + data(i)->description(verbosity) + "\n";
  if (desc == "") return desc;
  return cellInfo()->location(2) + "\n" + desc;
}


Averager* History::calculatePedestal(int i) const
{
  Averager* avg = new Averager(1);
  for (unsigned int k = 0; k < nData(); k++) {
    if (i >= 0 && i != (int)k) continue;
    SimpleShape* reference = referenceShape(k);
    if (!reference) continue;  
    double v,e;
    for (unsigned int l = 0; l < data(k)->nPoints(); l++)
      if (reference->interpolate(data(k)->time(l), v, e) == -1) { // we're before the ref shapeError
        TVectorD ped(1);
        ped(0) = data(k)->value(l);
        avg->fill(ped);
      }
    delete reference;
  }
  return avg;
}


Residual* History::residual(unsigned int k, bool correct, bool zeroTime) const
{
  if (k >= nData()) return nullptr;
  TVectorD del = deltas(k, -1, -1, correct);
  return new Residual(del, data(k)->run(), data(k)->event(), data(k)->adcMax(), 
                      (zeroTime ? 0 : data(k)->ofcTime() /*- Definitions::samplingTime(del.GetLwb()) */));
}


Residuals* History::residuals(CaloGain::CaloGain gain, double absResCut, bool correct, bool zeroTime) const
{
  Chi2Calc c2c;
  CovMatrix dummyErrors;
  Residuals* residuals = new Residuals();
  for (unsigned int k = 0; k < nData(); k++) {
    if (gain != CaloGain::LARNGAIN && data(k)->gain() != gain) continue;
    //if (goodForCorrOnly && !data(k)->goodForShapeCorr()) continue;    
    Residual* res = residual(k, correct, zeroTime);
    if (!res) { cout << "Error calculating residual for hash = " << m_hash << ", index = " << k << endl; return nullptr; }
    if (residuals->size() > 0 && !residuals->hasSameRange(*res)) {
      cout << "Warning for hash = " << m_hash << ", index = " << k << " : index interval changed from [" 
           << residuals->lwb() << ", " << residuals->upb() << "] to " << res->rangeStr() << endl;      
      delete res;
      delete residuals;
      return nullptr;
    }
    bool pass = true;
    for (int i = res->lwb(); i < res->upb(); i++) {
      if (absResCut > 0 && TMath::Abs(res->scaledDelta(i)) > absResCut) {
        pass = false;
        break;
      }
    }
    if (pass) residuals->add(*res);
    delete res;
  }
  return residuals;
}


double History::upstreamEnergy(unsigned int k) const
{
  if (!m_interface || !cellInfo()) return -1;
  if (k >= nData() || !data(k)->eventData()) return -1;
  std::vector<unsigned int> upstreamNeighbors;
  if (!m_interface->firstNeighbors(hash(), upstreamNeighbors, cellInfo()->layer() - 1)) return -1;
  if (upstreamNeighbors.empty()) return -1;
  std::vector<const Data*> unData;
  if (!m_interface->data(upstreamNeighbors, *data(k)->eventData(), unData)) return -1;
  double upstreamE = 0;
  for (const Data* data : unData) {
    upstreamE += data->energy();
    delete data;
  }
  return upstreamE;
}


double History::chi2Anomaly(double chi2Cut, unsigned int nDof) const
{
  if (nData() == 0 || chi2Cut <= 0) return -1;
  double nBadChi2 = 0;
  for (unsigned int k = 0; k < nData(); k++) if (chi2(k) > chi2Cut) nBadChi2++;
  double effBad = nBadChi2/nData();
  double effRef = TMath::Prob(chi2Cut, nDof);
  double dEff = sqrt(effRef*(1 - effRef))/sqrt(nData());
  if (dEff == 0) dEff = 1/sqrt(nData());
  return (effBad - effRef)/dEff;
}

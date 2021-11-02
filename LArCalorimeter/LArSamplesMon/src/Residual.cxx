/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/Residual.h"

#include "TMath.h"
#include <algorithm>
#include "LArSamplesMon/Averager.h"
#include "LArSamplesMon/ShapeErrorData.h"
#include "LArSamplesMon/ClassCounts.h"

#include "TRandom.h"
#include "TH1D.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


short Residual::comparisonSample = 0;


Residual::Residual(const TVectorD& deltas, int run, int event, double adcMax, double time) 
  : m_deltas(deltas), m_run(run), m_event(event), 
    m_adcMax(adcMax), m_time(time) 
{
  ClassCounts::incrementInstanceCount("Residual"); 
}


Residual::~Residual()
{
  ClassCounts::decrementInstanceCount("Residual"); 
}


TVectorD Residual::scaledDeltas() const
{
  TVectorD vect = deltas(); 
  vect *= 1/adcMax();
  return vect;
}


double Residual::scaledDelta(short i) const
{
  if (!isInRange(i) || adcMax() <= 0) return 0;
  return deltas()(i)/adcMax();
}


TVectorD Residual::scaledDeltasAndTime() const
{
  TVectorD vect(lwb(), upb() + 1);
  vect.SetSub(lwb(), scaledDeltas());
  vect(upb() + 1) = time();
  return vect;
}


bool ResidualCompare::operator()(const Residual& r1, const Residual& r2) const
{
  if (!r1.hasSameRange(r2)) {
    cout << "WARNING : residuals have different ranges, this may crash the sorting..." << endl;
    return false;
  }

  // Very, very important! Since samples are integers, it *will* happen that some residuals will have *identical* deltas. 
  // It turns out that (at least in the way used here), operator<(double, double) is not a strict weak ordering: we will
  // have cases where residual1[4] == residual2[4], and then we will have residual1[4] < residual2[4] and residual2[4] < residual1[4]
  // both true, which will confuse std::sort and cause it to crash... so add this safety (lesson of 2 days of debugging)
  // Note: if we write a<<b iff a < b - epsilon, then << is indeed a strict weak ordering so all is fine. It does mean there
  // will be "sorting errors", i.e. residuals that differ by less than epsilon may be sorted the wrong way. But that's fine for our usage.
  static double epsilon = 1E-5;
//   cout << "Comparing sample " << m_sampling << ", = " 
//        << (m_sampling == r1.upb() + 1 ? r1.time() : r1.scaledDelta(m_sampling)) << " >/< "
//        << (m_sampling == r2.upb() + 1 ? r2.time() : r2.scaledDelta(m_sampling)) << " " 
//        << ((m_sampling == r1.upb() + 1 ? r1.time() : r1.scaledDelta(m_sampling)) < (m_sampling == r2.upb() + 1 ? r2.time() : r2.scaledDelta(m_sampling))) << endl;

  if (r1.isInRange(m_sampling)) return r1.scaledDelta(m_sampling) < r2.scaledDelta(m_sampling) - epsilon;
  if (m_sampling == r1.upb() + 1) return r1.time() < r2.time() - epsilon;
  cout << "WARNING : comparison on an unknown sample, this may crash the sorting..." << endl;
  return false;
}

      
bool Residuals::medianVars(TVectorD& medians, TVectorD& widths) const
{
  if (size() == 0) return false;
  medians.ResizeTo(lwb(), upb() + 1);
  widths.ResizeTo (lwb(), upb() + 1);
  
  double halfSigmaQuantile = TMath::Prob(1,1)/2;
  double medianQuantile = 0.5;
  
  std::vector<Residual> sortedResiduals = m_residuals;
    
  for (short i = lwb(); i <= upb() + 1; i++) {
//     cout << "test-sorting sample " << i << " (n = " << m_residuals.size() << ")" << endl;
//     std::vector<const Residual*> testSort(m_residuals.size());
//     ResidualCompare comparer(i);
//     for (unsigned int i1 = 0; i1 < m_residuals.size(); i1++) {
//       const Residual& res1 = m_residuals[i1];
//       unsigned int nBefore = 0;
//       for (unsigned int i2 = 0; i2 < m_residuals.size(); i2++) {
//         if (i1 == i2) continue;
//         const Residual& res2 = m_residuals[i2];
//         //cout << "comparing " << i1 << " " << i2 << endl;
//         if (comparer(res2, res1)) nBefore++;
//         if ((i1 == 3617 || i1 == 3886) && comparer(res2, res1)) 
//           cout << i2 << " is bfore " << i1 << " " << res2.scaledDelta(i) << " < " << res1.scaledDelta(i) 
//                << (res2.scaledDelta(i) < res1.scaledDelta(i)) << " " << (res2.scaledDelta(i) < res1.scaledDelta(i) - 1E-3) << endl;
//       }
//       cout << "index " << i1 << " : nBefore = " << nBefore << " " << res1.scaledDelta(i) << endl;
//       while (testSort[nBefore]) { nBefore++; if (nBefore == testSort.size()) cout << "ERROR!!! " << i1  << endl; }
//       testSort[nBefore] = &res1;
//     }
//     cout << "done sorting" << endl;
//     for (unsigned int i1 = 0; i1 < testSort.size(); i1++) {
//       const Residual* res1 = testSort[i1];
//       if (!res1) cout << "ERROR : null at index " << i1 << endl;
//       for (unsigned int i2 = 0; i2 < i1; i2++) {
//         const Residual* res2 = testSort[i2];
//         if (comparer(*res1, *res2)) cout << "Wrong order " << i1 << " " << i2 << endl;
//       }
//     }
//     cout << "done checking" << endl;
    std::sort(sortedResiduals.begin(), sortedResiduals.end(), ResidualCompare(i));
    Residual& medianRes      = sortedResiduals[(unsigned int)(sortedResiduals.size()*medianQuantile)];
    Residual& loHalfSigmaRes = sortedResiduals[(unsigned int)(sortedResiduals.size()*halfSigmaQuantile)];
    Residual& hiHalfSigmaRes = sortedResiduals[(unsigned int)(sortedResiduals.size()*(1 - halfSigmaQuantile))];
    double median      = (i < upb() + 1 ? medianRes.scaledDelta(i) : medianRes.time()) ;
    double loHalfSigma = (i < upb() + 1 ? loHalfSigmaRes.scaledDelta(i) : loHalfSigmaRes.time());
    double hiHalfSigma = (i < upb() + 1 ? hiHalfSigmaRes.scaledDelta(i) : hiHalfSigmaRes.time());
    //cout << loHalfSigma << " " << median << " " << hiHalfSigma << endl;
    medians[i] = median;
    widths[i]  = (hiHalfSigma - median > median - loHalfSigma ? median - loHalfSigma : hiHalfSigma - median);
  }
  return true;
}


Residuals* Residuals::truncate(double nWidthsRes, double nWidthsTime, unsigned int nMax) const
{
  if (size() == 0) return new Residuals();
  TVectorD medians, widths;

  if (nMax > 0) {
    Residuals* original = new Residuals();
    for (unsigned int i = 0; i < nMax; i++) original->add(*residual(i));
    if (!original->medianVars(medians, widths)) { delete original; return nullptr;}
    delete original;
  }
  else {
    if (!medianVars(medians, widths)) return nullptr;
  }
  Residuals* truncated = new Residuals();

  for (const Residual& residual : m_residuals) {
    bool pass = true;
    if (nMax > 0 && truncated->size() == nMax) break;
    for (short i = lwb(); i <= upb(); i++) {
      if (nWidthsRes > 0 && TMath::Abs(residual.scaledDelta(i) - medians[i]) > nWidthsRes*widths[i]) {
        pass = false;
        break;
      }
    }
    if (!pass) continue;
    if (nWidthsTime > 0 && TMath::Abs(residual.time() - medians[upb() + 1]) > nWidthsTime*widths[upb() + 1]) continue;
    truncated->add(residual);
  }
  return truncated;
}


TH1D* Residuals::histogram(short sample, const TString& name, int nBins, double xMin, double xMax) const
{
  TH1D* h = new TH1D(name, "", nBins, xMin, xMax);
  for (const Residual& residual : m_residuals)
    h->Fill(sample <= upb() ? residual.scaledDelta(sample) : residual.time());
  return h;
}


ResidualCalculator* Residuals::calculator(bool weigh) const
{           
  if (size() == 0) return nullptr;

  ResidualCalculator* calc = new ResidualCalculator(lwb(), upb(), weigh);
  for (const Residual& residual : m_residuals)
    calc->fill(residual);
  
  return calc;
}


int ResidualCalculator::find(int r, int e) const
{
  for (unsigned int i = 0; i < size(); i++)
    if (event(i) == e && run(i) == r) return i;
  return -1;
}


bool ResidualCalculator::add(int r, int e)
{
  m_runs.push_back(r);
  m_events.push_back(e);
  return true;
}


bool ResidualCalculator::fill(const Residual& residual)
{ 
  if (find(residual.run(), residual.event()) >= 0) return true;
  add(residual.run(), residual.event());
  return fill_with_weight(residual, +1);
} 


bool ResidualCalculator::remove(const Residual& residual) 
{ 
  //std::pair<int, int> ev(residual.run(), residual.event());
  //std::map< std::pair<int, int>, bool>::iterator findEv = m_events.find(ev);
  //if (findEv == m_events.end()) return true;
  //cout << "Will remove run = " << ev.first << ", event = " << ev.second << endl;  
  //m_events.erase(findEv);
  int index = find(residual.run(), residual.event());
  if (index == -1) return true;
  m_events[index] = -1;
  m_runs[index] = -1;
  return fill_with_weight(residual, -1); 
}


ShapeErrorData* ResidualCalculator::shapeErrorData() const
{
  TVectorD xi = regresser()->means().GetSub(lwb(), upb(), "I");
  CovMatrix xiErr = regresser()->meanErrorMatrix().GetSub(lwb(), upb(), lwb(), upb(), "I");
  double tbar = regresser()->mean(upb() + 1);
  
  double denom = regresser()->covarianceMatrix()(upb() + 1, upb() + 1);   
  if (denom < 1E-6) {
    TVectorD xip(lwb(), upb());
    CovMatrix xipErr(lwb(), upb());
    // happens for size==2 if we are removing one of the residuals. 
    if (size() > 2) cout << "WARNING: variance of t < 1E-6, returning correction without derivative term. (V = " << denom << ", N = " << size() << ")" << endl;
    return new ShapeErrorData(xi, xip, xiErr, xipErr, tbar, regresser()->nEntries());
  }  
   
  TVectorD xip = TVectorD(regresser()->covarianceMatrix()[upb() + 1]).GetSub(lwb(), upb(), "I");
  xip *= -1/denom;  
  
  TVectorD xipErrVect  = TVectorD(regresser()->covarianceMatrixErrors()[upb() + 1]).GetSub(lwb(), upb(), "I");
  xipErrVect *= 1/denom;
  CovMatrix xipErr(lwb(), upb());
  for (int k1 = lwb(); k1 <= upb(); k1++) xipErr(k1, k1) = TMath::Power(xipErrVect(k1), 2);

  return new ShapeErrorData(xi, xip, xiErr, xipErr, tbar, regresser()->nEntries());
}


double  ResidualCalculator::weight(const Residual& residual) const
{
  if (!weigh()) return 1;
  return TMath::Power(residual.adcMax(), 2);
}


bool ResidualCalculator::fill_with_weight(const Residual& residual, double w)
{
  w *= weight(residual);
  return m_regresser.fill(residual.scaledDeltasAndTime(), w);
}


bool ResidualCalculator::append(const ResidualCalculator& other)
{
  if (!m_regresser.append(*other.regresser())) return false;
  //for (std::map< std::pair<int, int >, bool >::const_iterator event = other.events().begin();
  //     event != other.events().end(); event++)
  //  m_events[event->first] = event->second;
  for (unsigned int i = 0; i < other.size(); i++) add(other.run(i), other.event(i));
  return true;
}


TString ResidualCalculator::description() const
{
  TString str = "\n";
  /*for (std::map< std::pair<int, int>, bool>::const_iterator ev = m_events.begin();
       ev != m_events.end(); ev++)
   str += Form("run %6d event %10d\n", ev->first.first, ev->first.second);
  */
  for (unsigned int i = 0; i < size(); i++)
    str += Form("run %6d event %10d\n", run(i), event(i));

  return str;
}


bool Residual::test()
{
  Residuals testVect;
  TRandom r;
  for (unsigned int i = 0; i < 100000; i++) {
    TVectorD d(5);
    //for (unsigned short j = 0; j < 5; j++) d(j) = 0.01*i;
    for (unsigned short j = 0; j < 5; j++) d(j) = r.Gaus(0, 1);
    testVect.add(Residual(d, i, i, i, i));
  }
  
//   TVectorD m,w;
//   if (!getMedianVars(testVect, m, w)) return false;
//   m.Print();
//   w.Print();

  Residuals* truncated = testVect.truncate(2);
  if (!truncated) return false;
 
  TH1D* h = truncated->histogram(0, "h", 100, -5, 5);
  delete truncated;
  h->Draw();
  return true;
}


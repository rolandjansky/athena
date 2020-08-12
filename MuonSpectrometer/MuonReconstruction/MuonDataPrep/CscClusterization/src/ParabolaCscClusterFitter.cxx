/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ParabolaCscClusterFitter.h"

#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonPrepRawData/CscStripPrepData.h"
#include "MuonPrepRawData/CscPrepData.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include <sstream>
#include <iomanip>

using Muon::CscClusterStatus;
using Muon::CscStripPrepData;
using Muon::CscPrepData;
using MuonGM::CscReadoutElement;

typedef ICscClusterFitter::DataNames DataNames;
typedef ICscClusterFitter::Result Result;
typedef std::vector<Result> Results;

namespace {
  std::string splane(CscPlane plane) {
    switch(plane) {
    case CSS_ETA: return "CSS eta";
    case CSL_ETA: return "CSL eta";
    case CSS_PHI: return "CSS phi";
    case CSL_PHI: return "CSL phi";
    case UNKNOWN_PLANE: return "no such plane";
    }
    return "no such plane";
  }

  CscPlane findPlane(int station, bool measphi){
    if ( station == 1 ) {
      if ( measphi ) return CSS_PHI;
      else return CSS_ETA;
    } else if ( station == 2 ) {
      if ( measphi ) return CSL_PHI;
      else return CSL_ETA;
    }
    return UNKNOWN_PLANE;
  }
}

/** Correct the raw parabola interpolation, based on CSCPlane.
The correction function takes the output of the parabola method,
the peak position, and returns a position that is closer to the
predicted position. The correction depends on the strip pitch 
and was found experimentally during various beam tests and cosmics tests. 
@param plane The plane of the detector: Large or Small, X or Y.
@param raw The uncorrected value
@return The corrected value, often in range from -0.5 to 0.5.
*/
double ParabolaCscClusterFitter::ParabolaCorrection(CscPlane &plane, double &raw)const {
  double a, b, c; // correction values  
  switch (plane){
  case CSS_ETA:  { // small Chamber, X strips
    a = 2.97043e-01; // from segment186049.root
    b = 8.73895e+00;
    c = 2.08465e-01;
    break;
  }
  case CSS_PHI: { // small chamber, Y strips
    a = 0.2662; // 2rt test with small chambers, longy.root
    b = 62.19;
    c = 0.2191;
    break;
  }
  case CSL_ETA: { // large chamber, X strips
    a = 2.99744e-01; // from segment186049.root
    b = 8.30484e+00;
    c = 2.07538e-01;
    break;
  } 
  case CSL_PHI: { // large chamber, Y strips
    a = 0.2766; // Run 1782 in bat 184
    b = 50.08;
    c = 0.0301;
    break;
  }
  default: { // invalid plane, should not happen, check beforehand.
    return -1.0;
  }
  } // end switch

  return a * std::atan (b * raw) + c * raw;
}
    



//*************************************************************************

ParabolaCscClusterFitter::ParabolaCscClusterFitter(std::string type, std::string aname, const IInterface* parent) :
  AthAlgTool(type, aname, parent) {
  declareInterface<ICscClusterFitter>(this);
  m_max_width.push_back(5);  // CSS eta
  m_max_width.push_back(5);  // CSL eta
  m_max_width.push_back(3);  // CSS phi
  m_max_width.push_back(3);  // CSL phi
  declareProperty("max_width",  m_max_width);  // Maximum width (strips) for unspoiled clusters
  declareProperty("error_tantheta", m_error_tantheta = 0.57); // in mm
  declareProperty("xtan_css_eta_offset", m_xtan_css_eta_offset = 0.0015); // in mm
  declareProperty("xtan_css_eta_slope", m_xtan_css_eta_slope = 0.000137); // in mm
  declareProperty("xtan_csl_eta_offset", m_xtan_csl_eta_offset = -.0045); // in mm
  declareProperty("xtan_csl_eta_slope", m_xtan_csl_eta_slope = 0.000131); // in mm
  declareProperty("multi",  m_multi=3.1);  //  threshold multiplier  for cluster peak finding
}

//**********************************************************************
StatusCode ParabolaCscClusterFitter::initialize() {
  
  ATH_MSG_VERBOSE ( "Initalizing " << name() );

  ATH_CHECK(m_idHelperSvc.retrieve());

  ATH_MSG_DEBUG ( "Properties for " << name() << ":" );
  ATH_MSG_DEBUG ( "    tan(theta) error coeff: " << m_error_tantheta );
  ATH_MSG_DEBUG ( "  CSS eta pos-slope offset: " << m_xtan_css_eta_offset );
  ATH_MSG_DEBUG ( "   CSS eta pos-slope slope: " << m_xtan_css_eta_slope );
  ATH_MSG_DEBUG ( "  CSL eta pos-slope offset: " << m_xtan_csl_eta_offset );
  ATH_MSG_DEBUG ( "   CSL eta pos-slope slope: " << m_xtan_csl_eta_slope );
  // retrieve MuonDetectorManager from the conditions store     
  ATH_CHECK(m_DetectorManagerKey.initialize()); 

  return StatusCode::SUCCESS;
}

/** data names for ntuple output  in csc_cluster tree */
const DataNames& ParabolaCscClusterFitter::dataNames() const {
  static DataNames dnames;
  
  dnames.push_back("qA");
  dnames.push_back("qB");
  dnames.push_back("qC");
  
  return dnames;
}
  


/** Main fit method.
    The fit is really an interpolation, which uses a parabola through the 
    3 points given by (-1, qA), (0, qB), and (1, qC). This
    raw estimate is corrected to account for non-linearites and
    provides a position in the range of -0.5, 0.5.
    Finally, the pitch is used to convert the position in mm 
    relative to the center of the chamber.
    @param sfits The list of amplitudes in the StripFitList.
    @param tantheta The theta angle of the track, used for error estimate.
    @Note How do you get the track angle before this hit reconstruction?
    @return The Results vector, usually containing only one result,
    that of the largest hit.
*/
Results ParabolaCscClusterFitter::fit(const StripFitList& sfits, double tantheta) const {
  ATH_MSG_VERBOSE ( "Parabola fit with tool " << name() );
  
  Results results; // Vector of fit results
  
  // Check that the input has at least three strips.
  unsigned int nstrip = sfits.size(); // number of strips in this cluster
  if ( nstrip < 3 ) {
    ATH_MSG_VERBOSE ( " CscStatusNarrow: Input has fewer than three strips." );
    results.push_back(Result(1, Muon::CscStatusNarrow));
    return results;
  }

  // Check the input array for NULL pointers.
  for ( unsigned int istrip=0; istrip<nstrip; istrip++) {
    if ( sfits[istrip].strip == 0 ) {
      ATH_MSG_WARNING ( "Strip pointer is null." );
      results.push_back(Result(2));
      return results;
    }
  }

  // Use the first strip to extract the layer parameters.
  const CscStripPrepData* pstrip = sfits[0].strip;
  Identifier idStrip0 = pstrip->identify();

  // retrieve MuonDetectorManager from the conditions store  
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};     
  const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr();     
  if(MuonDetMgr==nullptr){       
    ATH_MSG_ERROR("Null pointer to the MuonDetectorManager conditions object");       
    return results;     
  }
  const CscReadoutElement* pro = MuonDetMgr->getCscReadoutElement(idStrip0);

  //  const CscReadoutElement* pro = pstrip->detectorElement(); fixed by Woochun
  bool measphi = m_idHelperSvc->cscIdHelper().CscIdHelper::measuresPhi(idStrip0);
  double pitch = pro->cathodeReadoutPitch(0, measphi);
  unsigned int maxstrip = pro->maxNumberOfStrips(measphi);
  unsigned int strip0 = m_idHelperSvc->cscIdHelper().strip(idStrip0) - 1;
  int station = m_idHelperSvc->cscIdHelper().stationName(idStrip0) - 49;    // 1=CSS, 2=CSL
  CscPlane plane = findPlane(station, measphi);
  if ( plane == UNKNOWN_PLANE ) {
    ATH_MSG_WARNING ( "Invalid CSC plane: station=" << station << "; measphi=" << measphi );
    results.push_back(Result(3));
    return results;
  }
  
  // Count strips above threshold:
  ATH_MSG_VERBOSE ( "Parabola fitter input has " << nstrip << " strips:" );
  unsigned int nstrip_threshold =0; 
  for ( unsigned int istrip=0; istrip<nstrip; ++istrip ) {
    Identifier id = sfits[istrip].strip->identify();
    if (sfits[istrip].charge>=20000) ++nstrip_threshold;
    ATH_MSG_VERBOSE ( " index: " << istrip << " chn:" << m_idHelperSvc->cscIdHelper().strip(id) << " amp:" << (int)(sfits[istrip].charge/1000) << " ke." );
  }
  
  // Find the highest peak and count all peaks above threshold.
  // Peaks have to be above threshold to avoid counting noise fluctuations
  unsigned int istrip_peak = 0;
  int numPeaks = 0; // count peaks above threshold
  float qpeak = -1; // charge of the peak strip
  // Loop over strips excluding the first and last strip
  double charge_clu = sfits[0].charge + sfits[nstrip-1].charge; // cluster sum
  for ( unsigned int istrip=1; istrip<nstrip-1; ++istrip ) {
    float qthis = sfits[istrip  ].charge;
    float qlast = sfits[istrip-1].charge;
    float qnext = sfits[istrip+1].charge;
    double thr = m_multi * sfits[istrip].dcharge / 10; // correct noise*10 
    Identifier id = sfits[istrip].strip->identify();
    ATH_MSG_VERBOSE ( " index: " << istrip << " chn:" << m_idHelperSvc->cscIdHelper().strip(id) << " amp:" << (int)(sfits[istrip].charge/1000) << " ke, thr: " << (int)(thr/1000) << " ke " << ((qthis>thr)?"signal":"noise") << sfits[istrip].dcharge/1000);
    charge_clu += qthis;

    // Peak if the adjacent strips have less charge.    
    if ((qthis >= qlast)  &&  (qthis >= qnext)) { // There is a peak.
      if (qthis > qpeak) { // larger peak then before
	istrip_peak = istrip; // record new peak location
	qpeak = qthis;
      }
      if (qthis>thr) numPeaks ++; // Only count peaks above a threshold
    }
  } // next istrip

  ATH_MSG_VERBOSE(" Peak is at index " << istrip_peak << " amp = " << qpeak/1000);
  if (numPeaks>1) { // Error if more than one peak above threshold was found.
    results.push_back(Result(6, Muon::CscStatusMultiPeak));
    ATH_MSG_VERBOSE(" CscStatusMultiPeak: multiple peaks are found: " << numPeaks);
    return results;
  }

  if (istrip_peak == 0) { // no peak (even below threshold) was found.
    results.push_back(Result(11));
    ATH_MSG_VERBOSE(" No peak was found.");
    return results;
  } 

  // Check that the peak is not on the chamber edge. 
  // This cannot happen due to the prev. loop
  /** @todo Remove this useless test. */
  if ( strip0+istrip_peak<=0 || strip0+istrip_peak>=maxstrip-1 ) {
    results.push_back(Result(4, Muon::CscStatusEdge));
    ATH_MSG_VERBOSE(" CscStatusEdge: strip0+istrip_peak = " 
		    << strip0+istrip_peak);
    return results;
  }

  // Cluster width cut, should not be done...
  /*****************************************************
  if ( nstrip_threshold > m_max_width[plane] ) {
    results.push_back(Result(5, Muon::CscStatusWide));
    ATH_MSG_VERBOSE(" CscStatusWide: nstrip_threshold = "
		    << nstrip_threshold);
    return results;
  }
  ********************************************************/

  // Cluster is spoiled if peak is not at the center.
  // should not be done
  /*******************************************************
  bool is_even = 2*(nstrip/2) == nstrip;
  bool atcenter = istrip_peak == nstrip/2 ||
                  (is_even && istrip_peak+1 == nstrip/2);
  if ( ! atcenter ) {
    results.push_back(Result(7, Muon::CscStatusSkewed));
    ATH_MSG_VERBOSE(" CscStatusSkewed: peak is not at the center.");
    return results;
  }
  *********************************************************/

  // reconstructed position in strip number units:
  double savg;

  // Charge amplitude per strip:
  double qA = sfits[istrip_peak-1].charge;
  double qB = sfits[istrip_peak  ].charge;
  double qC = sfits[istrip_peak+1].charge;

  // charge errors
  double dqA = sfits[istrip_peak-1].dcharge;
  double dqB = sfits[istrip_peak  ].dcharge;
  double dqC = sfits[istrip_peak+1].dcharge;


  double raw;                      // uncorrected parabola peak position
  double denominator = 2*qB-qA-qC; // denominator for parabola
  if (denominator <= 0) { // not a peak, should not happen
    ATH_MSG_WARNING ( "  Bad parabola denominator: " << denominator);
    results.push_back(Result(9));
    return results;
  } else {
    raw = 0.5*(qC-qA)/denominator; // peak of parabola through (qA, qB, qC) 
  }

  double pos = ParabolaCorrection (plane, raw);
  /** @todo check segment finding efficiency with Y pos = strip number. */
  double cog = 1.2*(qC-qA)/(qA+qB+qC); // for comparism
  //pos = cog;

  // error calculation: S/N =  charge sum / ave charge error.
  double dpos = (dqA+dqB+dqC)/3 /(qA+qB+qC) * pitch * std::sqrt(2.0); // pos error
  dpos = 0.08; // 80 micron error
  if ( measphi ) dpos = 2.5;  // worse phi resolution of ~2.5 mm
  //dpos = 200;//*= 2; /** todo Tune this to correct chi 2 of segments */
  /** @todo Base errors on error propagation in parabola.
      @todo Check errors against residuals.
      @todo Check for saturation and increase errors.
      @todo Check for large amps and increase errors. 
      @todo Check for small amps and increase errors.
      @todo Check cluster profile for overlapping hits.*/

  // Calculate strip position.
  savg = pos + strip0 + istrip_peak; // in strip numbers
  //  savg = 0.1 + strip0 + istrip_peak; // in strip numbers
  ATH_MSG_VERBOSE ( " Parabola correction: plane = '" << splane(plane) 
                    << "' qA=" << qA << " qB=" << qB << " qC=" << qC
                    << " raw=" << raw << " pos=" << pos 
		    << ", cog=" << cog);

  // Position error contribution from incident angle:
  double dpostht = m_error_tantheta*std::abs(tantheta);


  DataMap dmap;
  dmap["qA"] = qA;
  dmap["qB"] = qB;
  dmap["qC"] = qC;
  
  // Set return values.
  Result res(0, Muon::CscStatusUnspoiled); // status 0???
  res.position = pitch*(savg + 0.5 - 0.5*maxstrip);
  res.dposition = std::sqrt(dpos*dpos + dpostht*dpostht); // pos. error estimate
  res.strip = istrip_peak; // strip number in this cluster
  res.fstrip = 0;
  res.lstrip = nstrip-1; // cluster width
  res.charge = charge_clu; // total cluster charge over all strips
  res.time = sfits[res.strip].time; // peaking time at center strip
  res.qleft = qA; // left charge
  res.qpeak = qB; // center charge
  res.qright = qC; // right charge
  res.dataMap = dmap;

  ATH_MSG_VERBOSE(" Position: pos=" << res.position << " dpos:dtht="
		    << dpos << ":" << dpostht
                    << " ==>" << res.dposition << "  at tanth = " << tantheta);
  
  
  results.push_back(res);
  return results;
}





/**Correct the positon error for track angle.
@note This should be done in the segment, not here.
@todo Read, understand, and verify correctness.
@pclu The CSC Prep Data for the cluster
@slope The track angle
@return The new error estimate.
*/
double ParabolaCscClusterFitter::getCorrectedError(const CscPrepData* pclu, double slope) const {

  // Cluster position.
  Trk::ParamDefs icor = Trk::loc1;
  Trk::ParamDefs ierr = Trk::loc1;
  double pos  = pclu->localPosition()[icor];
  double dpos = Amg::error(pclu->localCovariance(),ierr);

  Identifier idStrip0 = pclu->identify();
  int station = m_idHelperSvc->cscIdHelper().stationName(idStrip0) - 49; // 1=CSS, 2=CSL
  // Calculate the angle of incidence.
  double tantht = 0.0;
  if ( station == 1 ) {
    tantht = m_xtan_css_eta_offset + m_xtan_css_eta_slope*pos;
  } else {
    tantht = m_xtan_csl_eta_offset + m_xtan_csl_eta_slope*pos;
  }
  // Correct the error using this angle.
  double old_dpostht = m_error_tantheta*std::abs(tantht);
  
  double new_dpostht = m_error_tantheta*std::abs(slope);

  double newError = std::sqrt(dpos*dpos
                         - old_dpostht*old_dpostht
                         + new_dpostht*new_dpostht);

  
  ATH_MSG_VERBOSE ( "   Position :: pos=" << pos
                    << " dpos:newdpos=" << dpos << " : " << newError 
                    << "  " << old_dpostht << "  " << new_dpostht );

  
  return newError;

}




//**********************************************************************
Results ParabolaCscClusterFitter::fit(const StripFitList& sfits) const {
  Results results = fit(sfits, 0.0);
  Results new_results;
  for ( unsigned int iresult=0; iresult<results.size(); ++iresult ) {
    Result res = results[iresult];
    if ( res.fitStatus ) {
      new_results.push_back(res);
      continue;
    }
    // Fetch the chamber type.
    const CscStripPrepData* pstrip = sfits[0].strip;
    Identifier idStrip0 = pstrip->identify();
    int station = m_idHelperSvc->cscIdHelper().stationName(idStrip0) - 49; // 1=CSS, 2=CSL
    // Calculate the angle of incidence.
    double tantht = 0.0;
    double pos = res.position;
    if ( station == 1 ) {
      tantht = m_xtan_css_eta_offset + m_xtan_css_eta_slope*pos;
    } else {
      tantht = m_xtan_csl_eta_offset + m_xtan_csl_eta_slope*pos;
    }
    // Correct the error using this angle.
    double dpostht = m_error_tantheta*std::abs(tantht);
    double dpos = res.dposition;
    res.dposition = std::sqrt(dpos*dpos + dpostht*dpostht);
    
    // Return the updated result.
    new_results.push_back(res);
  }
  
  return new_results;
}

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "QratCscClusterFitter.h"

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

enum CscStation { UNKNOWN_STATION, CSS, CSL };
enum CscPlane { CSS_ETA, CSL_ETA, CSS_PHI, CSL_PHI, UNKNOWN_PLANE };

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

//******************************************************************************

int qrat_correction(CscPlane plane, double qrat, double& cor, double& dcordqrat) {
  std::vector<double> pfac;
  if ( plane == CSS_ETA ) {
    if ( qrat < 0.095 ) return 1;
    if ( qrat > 1.01 ) return 1;
    if ( qrat < 0.15 ) {
      pfac.push_back(-5.483);
      pfac.push_back(130.6);
      pfac.push_back(-1296.);
      pfac.push_back(5994.);
      pfac.push_back(-10580.);
    } else if ( qrat < 0.30 ) {
      pfac.push_back(-0.9225);
      pfac.push_back(5.569);
      pfac.push_back(2.908);
      pfac.push_back(-66.19);
      pfac.push_back(108.5);
    } else {
      pfac.push_back(-0.4246);
      pfac.push_back(2.619);
      pfac.push_back(-3.642);
      pfac.push_back(2.837);
      pfac.push_back(-0.8916);
    }
  } else if ( plane == CSL_ETA ) {
    if ( qrat < 0.105 ) return 1;
    if ( qrat > 1.01 ) return 1;
    if ( qrat < 0.25 ) {
      pfac.push_back(-2.823);
      pfac.push_back(42.71);
      pfac.push_back(-279.1);
      pfac.push_back(879.2);
      pfac.push_back(-1062.);
    } else {
      pfac.push_back(-0.5409);
      pfac.push_back(3.110);
      pfac.push_back(-4.630);
      pfac.push_back(3.800);
      pfac.push_back(-1.241);
    }
  } else {
    return 3;
  }
  cor = 0.0;
  dcordqrat = 0.0;
  double term = 1.0;
  for ( unsigned int ipow=0; ipow<pfac.size(); ++ipow ) {
    dcordqrat += ipow*pfac[ipow]*term;
    if ( ipow ) term *= qrat;
    cor +=  pfac[ipow]*term;
  }
  return 0;
}
    
//******************************************************************************

// Calculate QRAT correction by interpolation.
// Input array runs from 0.0 to 1.0.
// Output is shifted by 0.5.
// The value in each bin corresponds to upper edge of the bin.
//   qrmin = min value of the qrat distribution
//   corvals = array of corrections
//   qrat = input value of qrat
//   cor = output correction
//   dcordqrat = derivative of cor w.r.t. qrat

int qrat_interpolation(double qrmin, const std::vector<double>& corvals,
                       double qrat, double& cor, double& dcordqrat) {
  int nbin = corvals.size();
  if ( ! nbin ) return 1;
  // Treat any QRAT below the minimum as if it were at the minumum.
  if ( qrat < qrmin ) qrat = qrmin;
  // Find the bin holding dqrat.
  double dqrat = 1.0/nbin;
  int bin = int(qrat/dqrat);
  // Extract the value for this bin (x1) and the preceding (x0)
  // and the following (x2).
  double x1 = corvals[bin];
  if ( x1 == 0.0 ) return 2;
  double x0 = bin==0 ? 0.0 : corvals[bin-1];
  double x2 = bin==nbin-1 ? 1.0 : corvals[bin+1];
  // Assign the qrat values for these bins. If this is the first
  // bin with data, then use the input qrmin.
  double qrat0 = dqrat*bin;
  double qrat1 = qrat0 + dqrat;

  if ( x0 == 0.0 ) {
    if ( qrmin > qrat0 && qrmin < qrat1 ) {
      qrat0 = qrmin;
    }
  }
  // Calculate correction and derivative.
  // Use quadratic interpolation with the high edge of this bin,
  // the preceding and the following. For the last bin, use linear
  // interpolation.
  if ( bin == nbin-1 ) {
    double a = x0;
    double b = (x1-x0)/dqrat;
    cor = a + b*(qrat-qrat0);
    dcordqrat = b;
  } else {
    double d0 = qrat1 - qrat0;
    double d = dqrat;
    double w = 1.0/(d0*d*d+d0*d0*d);
    double a = x1;
    double b = w*d*d*(x1-x0) + w*d0*d0*(x2-x1); 
    double c = w*d0*(x2-x1) - w*d*(x1-x0);
    cor = a + b*(qrat-qrat1) + c*(qrat-qrat1)*(qrat-qrat1);
    dcordqrat = b + 2.0*c*(qrat-qrat1);
  }
  if ( cor < 0.0 ) cor = 0.0;
  // Shift correction to center of bin.
  cor -= 0.50;
  return 0;
}

    
//****************************************************************************

/** Calculate QRAT correction from inverse hyperbolic tangent
    based on a fit to the plot of pos x vs charge ratio qrat to the function
    qrat(x) = a + b* tanh(c*(x-x0))
    Here we use the inverse of the above function
    x =  atanh((qrat-a)/b)/c+x0
    to obtain positions from charge ratios.
    Output is shifted by 0.5, so the center of the strip is at x=0.

@param   a = parameter of correction function
@param   b = parameter of correction function
@param   c = parameter of correction function
@param   x0 = parameter of correction function
@param   qrat = input value of qrat = Q_right/Q_peak
@param   cor = output of corrected position in the range -0.5 to 0.5
@param   dcordqrat = derivative of cor w.r.t. qrat for error estimates
@return  0 if no error, 1 if qrat is negative
*/
int qrat_atanh(const double a, const double b, double c, const double x0, 
	       double qrat, double& cor, double& dcordqrat) {
  if (qrat<=0) return 1; // avoid trouble in error calculation 
  // minimum qrat value (at pos=-1) or use pos = -0.5?
  double qrmin = a + b*tanh(c*(-1-x0));
  
  // Treat any QRAT below the minimum as if it were at the minumum.
  if ( qrat < qrmin ) qrat = qrmin;
  double z = (qrat-a)/b;
  cor = atanh(z)/c+x0;
  dcordqrat = 1.0/(1.0-z*z)/b/c;

  return 0;
}


//****************************************************************************

QratCscClusterFitter::QratCscClusterFitter(std::string type, std::string aname, const IInterface* parent) :
    AthAlgTool(type, aname, parent),
    m_alignmentTool("CscAlignmentTool/CscAlignmentTool", this)
{
  declareInterface<ICscClusterFitter>(this);
  m_max_width.push_back(5);  // CSS eta
  m_max_width.push_back(5);  // CSL eta
  m_max_width.push_back(3);  // CSS phi
  m_max_width.push_back(2);  // CSL phi
  declareProperty("max_width",  m_max_width);  // Maximum width (strips) for unspoiled clusters
  declareProperty("position_option_eta",  m_posopt_eta = "ATANH");
  declareProperty("position_option_phi",  m_posopt_phi = "NONE");
  declareProperty("error_option_eta",  m_erropt_eta = "CHARGE");
  declareProperty("error_option_phi",  m_erropt_phi = "NONE");
  declareProperty("error_eta", m_error_eta = 0.050); // in mm this is for fixed error
  declareProperty("error_phi", m_error_phi = 0.140); // in mm this is for fixed error

  declareProperty("precisionErrorScaler", m_precisionErrorScaler = 1.0); // in mm this is for fixed error
  
  declareProperty("qrat_maxdiff",  m_qrat_maxdiff = 0.15); // in strips
  declareProperty("qrat_maxsig",  m_qrat_maxsig = 6.0); // in strips
  declareProperty("error_tantheta", m_error_tantheta = 0.57); // in mm
  declareProperty("xtan_css_eta_offset", m_xtan_css_eta_offset = 0.0015); // in mm
  declareProperty("xtan_css_eta_slope", m_xtan_css_eta_slope = 0.000137); // in mm
  declareProperty("xtan_csl_eta_offset", m_xtan_csl_eta_offset = -0.0045); // in mm
  declareProperty("xtan_csl_eta_slope", m_xtan_csl_eta_slope = 0.000131); // in mm
  declareProperty("qratmin_css_eta", m_qratmin_css_eta = 0.0940459);
  declareProperty("qratmin_csl_eta", m_qratmin_csl_eta = 0.108975);
  declareProperty("qratcor_css_eta", m_qratcor_css_eta); // in strips
  declareProperty("qratcor_csl_eta", m_qratcor_csl_eta); // in strips


  declareProperty("atanh_a_css_eta", m_atanh_a_css_eta = 1.5);
  declareProperty("atanh_b_css_eta", m_atanh_b_css_eta = 1.411);
  declareProperty("atanh_c_css_eta", m_atanh_c_css_eta = 2.329);
  declareProperty("atanh_x0_css_eta", m_atanh_x0_css_eta = 0.6601);
  declareProperty("atanh_a_csl_eta", m_atanh_a_csl_eta = 1.516);
  declareProperty("atanh_b_csl_eta", m_atanh_b_csl_eta = 1.427);
  declareProperty("atanh_c_csl_eta", m_atanh_c_csl_eta = 2.35);
  declareProperty("atanh_x0_csl_eta",m_atanh_x0_csl_eta = 0.6615);

  declareProperty("dposmin", m_dposmin = 0.082);

  declareProperty("CscAlignmentTool", m_alignmentTool );

}

//**********************************************************************

StatusCode QratCscClusterFitter::initialize() {
  
  ATH_MSG_VERBOSE ( "Initalizing " << name() );

  // retrieve MuonDetectorManager from the conditions store     
  ATH_CHECK(m_DetectorManagerKey.initialize()); 

  ATH_CHECK(m_idHelperSvc.retrieve());

  if ( m_alignmentTool.retrieve().isFailure() )   {
    ATH_MSG_WARNING ( name() << ": unable to retrieve cluster fitter " << m_alignmentTool );
  } else {
    ATH_MSG_DEBUG ( name() << ": retrieved " << m_alignmentTool );
  }



  ATH_MSG_DEBUG ( "Properties for " << name() << ":" );
  ATH_MSG_DEBUG ( "       Eta position option: " << m_posopt_eta );
  ATH_MSG_DEBUG ( "       Phi position option: " << m_posopt_phi );
  ATH_MSG_DEBUG ( "          Eta error option: " << m_erropt_eta );
  ATH_MSG_DEBUG ( "          Phi error option: " << m_erropt_phi );
  ATH_MSG_DEBUG ( "        Eta assigned error: " << m_error_eta );
  ATH_MSG_DEBUG ( "        Phi assigned error: " << m_error_phi );
  ATH_MSG_DEBUG ( "        Max strip pos diff: " << m_qrat_maxdiff );
  ATH_MSG_DEBUG ( "         Max strip pos sig: " << m_qrat_maxsig );
  ATH_MSG_DEBUG ( "    Non-normal error coeff: " << m_error_tantheta );
  ATH_MSG_DEBUG ( "  CSS eta pos-slope offset: " << m_xtan_css_eta_offset );
  ATH_MSG_DEBUG ( "   CSS eta pos-slope slope: " << m_xtan_css_eta_slope );
  ATH_MSG_DEBUG ( "  CSL eta pos-slope offset: " << m_xtan_csl_eta_offset );
  ATH_MSG_DEBUG ( "   CSL eta pos-slope slope: " << m_xtan_csl_eta_slope );
  ATH_MSG_DEBUG ( "      CSS eta table offset: " << m_qratmin_css_eta );
  ATH_MSG_DEBUG ( "      CSL eta table offset: " << m_qratmin_csl_eta );
  ATH_MSG_DEBUG ( "        CSS eta table size: " << m_qratcor_css_eta.size() );
  ATH_MSG_DEBUG ( "        CSL eta table size: " << m_qratcor_csl_eta.size() );

  ATH_MSG_DEBUG ( "atanh_a_css_eta:            " << m_atanh_a_css_eta );
  ATH_MSG_DEBUG ( "atanh_b_css_eta:            " << m_atanh_b_css_eta );
  ATH_MSG_DEBUG ( "atanh_c_css_eta:            " << m_atanh_c_css_eta );
  ATH_MSG_DEBUG ( "atanh_x0_css_eta:           " << m_atanh_x0_css_eta );
  ATH_MSG_DEBUG ( "atanh_a_csl_eta:            " << m_atanh_a_csl_eta );
  ATH_MSG_DEBUG ( "atanh_b_csl_eta:            " << m_atanh_b_csl_eta );
  ATH_MSG_DEBUG ( "atanh_c_csl_eta:            " << m_atanh_c_csl_eta );
  ATH_MSG_DEBUG ( "atanh_x0_csl_eta:           " << m_atanh_x0_csl_eta );

  ATH_MSG_DEBUG ( "Minimum pos error:          " << m_dposmin );

  return StatusCode::SUCCESS;
}

//**********************************************************************

const DataNames& QratCscClusterFitter::dataNames() const {
  auto init = [&](){
    DataNames dnames;
    bool dofixed = false;
    bool docharge = false;
    if ( m_posopt_phi == "POLYNOMIAL"  ||  m_posopt_phi == "TABLE"  ||  m_posopt_phi == "ATANH") {
      if ( m_erropt_phi == "FIXED" ) dofixed = true;
      if ( m_erropt_phi == "CHARGE" ) docharge = true;
    }
    if ( m_posopt_eta == "POLYNOMIAL"  ||  m_posopt_eta == "TABLE"  ||  m_posopt_eta == "ATANH" ) {
      if ( m_erropt_eta == "FIXED" ) dofixed = true;
      if ( m_erropt_eta == "CHARGE" ) docharge = true;
    }
    if ( dofixed || docharge ) {
      dnames.push_back("scor1");
      dnames.push_back("scor2");
      dnames.push_back("scor");
    }
    if ( docharge ) {
      dnames.push_back("dscor1");
      dnames.push_back("dscor2");
      dnames.push_back("dscor");
      dnames.push_back("scordiff");
      dnames.push_back("dscordiff");
    }
    return dnames;
  };
  static DataNames dnames ATLAS_THREAD_SAFE = init();
  return dnames;
}
  
//**********************************************************************

Results QratCscClusterFitter::fit(const StripFitList& sfits, double tantheta) const {
  ATH_MSG_VERBOSE ( "QRAT fit with tool " << name() );

  Results results;
  
  // Check input has at least three strips.
  unsigned int nstrip = sfits.size();
  if ( nstrip < 3 ) {
    ATH_MSG_VERBOSE ( "  Input has fewer than three strips." );
    if (nstrip ==2) {
      Muon::CscTimeStatus tstatus = (sfits[0].charge > sfits[1].charge) ? sfits[0].timeStatus : sfits[1].timeStatus;
      results.push_back(Result(1, Muon::CscStatusNarrow, tstatus));
    } else if (nstrip==1) {
      Muon::CscTimeStatus tstatus = sfits[0].timeStatus;
      results.push_back(Result(1, Muon::CscStatusNarrow, tstatus));
    } 
    return results;
  }

  // Fetch the number of strips and check the input arrays.
  for ( unsigned int istrip=0; istrip<nstrip; ++istrip ) {
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
  
  // Display input strips.
  ATH_MSG_VERBOSE ( "QRAT fittter input has " << nstrip << " strips" );
  for ( unsigned int istrip=0; istrip<nstrip; ++istrip ) {
    Identifier id = sfits[istrip].strip->identify();
    ATH_MSG_VERBOSE ( "  " <<  station << " : " << measphi << "  " << m_idHelperSvc->cscIdHelper().wireLayer(id)
                      << "  " << istrip << " " << m_idHelperSvc->cscIdHelper().strip(id)
                      << " " << sfits[istrip].charge );
  }
  
  // Find the peak strip and check the shape.
  unsigned int istrip_peak = 0; // strip number within cluster
  // Loop over strips excluding the edges.
  double charge_clu = sfits[0].charge + sfits[nstrip-1].charge;
  for ( unsigned int istrip=1; istrip<nstrip-1; ++istrip ) {
    StripFit sfit = sfits[istrip];
    float qthis = sfit.charge;
    float qlast = sfits[istrip-1].charge;
    float qnext = sfits[istrip+1].charge;
    charge_clu += qthis;
    // Peak if the adjacent strips have less charge.
    bool ispeak = qthis > qlast  &&  qthis > qnext;
    // Special case: next strip has the same charge.
    // Require the previous strip has less charge and the next following
    // strip be absent or have less charge.
    if ( ! ispeak ) {
      if ( qthis == qnext ) {
        ispeak = ( qthis > qlast ) &&
                 ( istrip+2 == nstrip || sfits[istrip+2].charge < qthis );
      }
    }
    // Special case: first and second strips have the same charge.
    // Require the third strip has less charge.
    if ( ! ispeak ) {
      if ( istrip == 1 ) {
        if ( qthis == qlast ) {
          ispeak = qthis > qnext; // bug found 10/13/07
        }
      }
    }
    // Record if peak.
    if ( ispeak ) {
      if ( istrip_peak ) {      // Error if multiple peaks are found.
        results.push_back(Result(6, Muon::CscStatusMultiPeak)); // Time status should be defined in SimpleClusterFit...
        return results;
      }
      istrip_peak = istrip;
    }
  }
  ATH_MSG_VERBOSE ( "  Peak is at " << istrip_peak << "th strip in cluster");

  // Check we are not on the edge.
  if ( strip0<=0 || strip0+nstrip>maxstrip ) {
    results.push_back(Result(4, Muon::CscStatusEdge, sfits[istrip_peak].timeStatus));
    return results;
  }

  // MS: remove this check since width is amplitude dependent.
  // use saturation check instead
  /**************************************
  if ( nstrip > m_max_width[plane] ) {
    //  if ( nstrip_threshold > m_max_width[plane] ) {
    results.push_back(Result(5, Muon::CscStatusWide, sfits[istrip_peak].timeStatus));
    return results;
  }
  ***************************************/

  // Cluster is spoiled if peak is not at the center.
  bool is_even = 2*(nstrip/2) == nstrip;
  bool atcenter = istrip_peak == nstrip/2 ||
                  (is_even && istrip_peak+1 == nstrip/2);
  if ( ! atcenter ) {
    results.push_back(Result(7, Muon::CscStatusSkewed, sfits[istrip_peak].timeStatus));
    return results;
  }


  if ( sfits[istrip_peak].stripStatus == Muon::CscStrStatSaturated
       || sfits[istrip_peak-1].stripStatus == Muon::CscStrStatSaturated
       || sfits[istrip_peak+1].stripStatus == Muon::CscStrStatSaturated ) {
    results.push_back(Result(15, Muon::CscStatusSaturated, sfits[istrip_peak].timeStatus));
    return results;
  }


  // left/peak/right strip should have good time information....
  if ( sfits[istrip_peak].stripStatus != Muon::CscStrStatSuccess
       //       || sfits[istrip_peak-1].stripStatus != Muon::CscStrStatSuccess
       //       || sfits[istrip_peak+1].stripStatus != Muon::CscStrStatSuccess ) {
       ) {
    results.push_back(Result(14, Muon::CscStatusStripFitFailed, sfits[istrip_peak].timeStatus));
    return results;
  } else
    if ( sfits[istrip_peak-1].stripStatus == Muon::CscStrStatHot
         || sfits[istrip_peak-1].stripStatus == Muon::CscStrStatDead 
         || sfits[istrip_peak+1].stripStatus == Muon::CscStrStatHot
         || sfits[istrip_peak+1].stripStatus == Muon::CscStrStatDead ) {
      results.push_back(Result(14, Muon::CscStatusStripFitFailed, sfits[istrip_peak].timeStatus));
      return results;
    }  
    

  ///////////////////////////////////////////////////////////////
  // Set initial strip position.
  double savg = istrip_peak;

  // Calculate QRAT correction to strip position.
  std::string posopt = m_posopt_eta;
  std::string erropt = m_erropt_eta;
  double dpos = 0.0;
  if ( measphi ) {
    posopt = m_posopt_phi;
    erropt = m_erropt_phi;
  }
  double q1 = sfits[istrip_peak-1].charge;
  double q0 = sfits[istrip_peak].charge;
  double q2 = sfits[istrip_peak+1].charge;
  double qrat1 = q1/q0;
  double qrat2 = q2/q0;
  double dq1 = sfits[istrip_peak-1].dcharge;
  double dq0 = sfits[istrip_peak].dcharge;
  double dq2 = sfits[istrip_peak+1].dcharge;

  ATH_MSG_VERBOSE ( "  QRAT charge ratios: " << qrat1 << " " << qrat2 );
  double scor1 = 0.; // left side correction.
  double dscordqrat1 =0.;
  double scor2 = 0; // right side correction.
  double dscordqrat2 = 0.;
  int stat1 = 0;
  int stat2 = 0;
  if ( posopt == "POLYNOMIAL" ) {
    stat1 = qrat_correction(plane, qrat1, scor1, dscordqrat1);
    stat2 = qrat_correction(plane, qrat2, scor2, dscordqrat2);
  } else if ( posopt == "TABLE" ) {
    double qrmin = 0.0;
    const std::vector<double>* pcor = 0;
    if ( plane == CSS_ETA ) {
      qrmin = m_qratmin_css_eta;
      pcor = &m_qratcor_css_eta;
    } else if ( plane == CSL_ETA ) {
      qrmin = m_qratmin_csl_eta;
      pcor = &m_qratcor_csl_eta;
    } else {
      ATH_MSG_WARNING ( "    Invalid QRAT plane: " << splane(plane) );
      results.push_back(Result(8));
      return results;
    }
    if ( pcor ) {
      stat1 = qrat_interpolation(qrmin, *pcor, qrat1, scor1, dscordqrat1);
      stat2 = qrat_interpolation(qrmin, *pcor, qrat2, scor2, dscordqrat2);
    } else {
      stat1 = 99;
    }
  } else if ( posopt == "ATANH" ) { // MS: atanh parametrization
    double a, b, c, x0; // parameters
    if ( plane == CSS_ETA ) {
      a  = m_atanh_a_css_eta;
      b  = m_atanh_b_css_eta;
      c  = m_atanh_c_css_eta;
      x0 = m_atanh_x0_css_eta;
    } else if ( plane == CSL_ETA ) {
      a  = m_atanh_a_csl_eta;
      b  = m_atanh_b_csl_eta;
      c  = m_atanh_c_csl_eta;
      x0 = m_atanh_x0_csl_eta;
    } else {
      ATH_MSG_WARNING ( "    Invalid QRAT plane: " << splane(plane) );
      results.push_back(Result(8));
      return results;
    }
    stat1 = qrat_atanh (a, b, c, x0, qrat1, scor1, dscordqrat1);
    stat2 = qrat_atanh (a, b, c, x0, qrat2, scor2, dscordqrat2);
     
  } else {
    ATH_MSG_WARNING ( "    Invalid position option: " << posopt );
    results.push_back(Result(9));
    return results;
  }
  if ( stat1 || stat2 ) {
    ATH_MSG_VERBOSE ( "    QRAT correction failed: SPOILED" );
    results.push_back(Result(10));
    return results;
  }
  ATH_MSG_VERBOSE ( "  QRAT strip corrs: " << scor1 << " " << scor2 );
  ATH_MSG_VERBOSE ( "  QRAT derivatives: " << dscordqrat1 << " " << dscordqrat2 );

  // Compare left and right.
  // Flip sign of the left side correction.
  scor1 = -scor1;
  dscordqrat1 = -dscordqrat1;
  double scor = 0.0;
  DataMap dmap;
  // Calculation weighting corrections by their derivatives, i.e. assuming the
  // two charge ratios have the same error.
  if ( erropt == "FIXED" ) {
    double w1 = 1/(dscordqrat1*dscordqrat1);
    double w2 = 1/(dscordqrat2*dscordqrat2);
    scor = (w1*scor1 + w2*scor2)/(w1+w2);
    double scor_diff = std::abs(scor2 - scor1);
    ATH_MSG_VERBOSE ( "  Combined corr: " << scor );
    dpos = measphi ? m_error_phi : m_error_eta;
    // Fill data map.
    dmap["scor1"] = scor1;
    dmap["scor2"] = scor2;
    dmap["scor"] = scor;
    // Exit if measurements are inconsistent.
    if ( scor_diff > m_qrat_maxdiff ) {
      ATH_MSG_VERBOSE ( "  SPOILED (scor_diff=" << scor_diff << ")" );
      results.push_back(Result(11, Muon::CscStatusQratInconsistent));
      return results;
    }
  // Calculation using the (independent) errors in the three charges.
  } else if ( erropt == "CHARGE" ) {
    // Calculate intermediate variables.
    double x1 = dscordqrat1*qrat1;
    double x2 = dscordqrat2*qrat2;
    double dqq0 = dq0/q0;
    double dqq1 = dq1/q1;
    double dqq2 = dq2/q2;
    double rnum = (-x1*x1*dqq1*dqq1 + x2*x2*dqq2*dqq2 + (x2*x2-x1*x1)*dqq0*dqq0);
    double rden = (x1*x1*dqq1*dqq1 + x2*x2*dqq2*dqq2 + (x2-x1)*(x2-x1)*dqq0*dqq0);
    double rfac = rnum/rden;
    // Calculate the significance of the difference between the measurements.
    double dscor_diff = sqrt(rden);
    double scor_diff = scor2 - scor1;
    double scor_sig = std::abs(scor_diff)/dscor_diff;
    // Calculate the weighted average of the corrections.
    double w1 = 0.5*(1.0+rfac);
    double w2 = 0.5*(1.0-rfac);
    scor = w1*scor1 + w2*scor2;
    // Calculate the error in this average.
    double ddscor1 = w1*x1*dqq1;
    double ddscor2 = w2*x2*dqq2;
    double ddscor0 = (w1*x1 + w2*x2)*dqq0;
    double dscor = sqrt(ddscor1*ddscor1 + ddscor2*ddscor2 + ddscor0*ddscor0);
    dpos = pitch*dscor;
    // add minimum error (in mm) in quadrature:
    dpos = sqrt(dpos*dpos + m_dposmin*m_dposmin);
    // Fill data map.
    double dscor1 = std::abs(x1)*sqrt(dqq1*dqq1+dqq0*dqq0);
    double dscor2 = std::abs(x2)*sqrt(dqq2*dqq2+dqq0*dqq0);
    dmap["scor1"] = scor1;
    dmap["dscor1"] = dscor1;
    dmap["scor2"] = scor2;
    dmap["dscor2"] = dscor2;
    dmap["scordiff"] = scor_diff;
    dmap["dscordiff"] = dscor_diff;
    dmap["scor"] = scor;
    dmap["dscor"] = dscor;
    // Debugging.
    ATH_MSG_VERBOSE ( "QRAT CHARGE calculation" );
    ATH_MSG_VERBOSE ( "       q1, q0, q2: " << q1 << " " << q0 << " " << q2 );
    ATH_MSG_VERBOSE ( "    dq1, dq0, dq2: " << dq1 << " " << dq0 << " " << dq2 );
    ATH_MSG_VERBOSE ( "  dscordqrat1, x1: " << dscordqrat1 << " " << x1 );
    ATH_MSG_VERBOSE ( "  dscordqrat2, x2: " << dscordqrat2 << " " << x2 );
    ATH_MSG_VERBOSE ( "     scor1 = " << scor1 << " +/- " << dscor1 );
    ATH_MSG_VERBOSE ( "     scor2 = " << scor2 << " +/- " << dscor2 );
    ATH_MSG_VERBOSE ( "      scor = " << scor << " +/- " << dscor );
    ATH_MSG_VERBOSE ( "  scordiff = " << scor_diff << " +/- " << dscor_diff );
    ATH_MSG_VERBOSE ( "  scor_sig = " << scor_sig);

    // Exit if measurements are inconsistent.
    if ( scor_sig > m_qrat_maxsig ) {
      ATH_MSG_VERBOSE ( "  SPOILED (scor_sig=" << scor_sig << ")" );
      results.push_back(Result(12, Muon::CscStatusQratInconsistent, sfits[istrip_peak].timeStatus));
      return results;
    }
  } else {
    ATH_MSG_WARNING ( "    Invalid error option: " << erropt );
    results.push_back(Result(13, Muon::CscStatusUndefined, sfits[istrip_peak].timeStatus));
    return results;
  }

  // Correct strip position.
  savg += scor+strip0;
  ATH_MSG_VERBOSE ( "    QRAT corr " << splane(plane) << " nstrip=" << nstrip
                    << " savg=" << savg << " qrat1=" << qrat1 << " qrat2=" << qrat2
                    << " scor1=" << scor1 << " scor2=" << scor2 );

  // Error due to incident angle.
  double dpostht = m_error_tantheta*std::abs(tantheta);

  // Scale error on dpos
  dpos = dpos*m_precisionErrorScaler; 
  
  // Set return values.
  Result res(0, Muon::CscStatusUnspoiled);
  res.strip = istrip_peak;
  res.position = pitch*(savg + 0.5 - 0.5*maxstrip);

  // internal alignment ... 
  Identifier id = sfits[res.strip].strip->identify();
  double offset = m_alignmentTool->getAlignmentOffset(id);
  res.position -= offset;


  res.dposition = sqrt(dpos*dpos + dpostht*dpostht);

  res.fstrip = 0;
  res.lstrip = nstrip-1;
  res.time = sfits[istrip_peak].time;
  res.time_beforeT0Corr = sfits[istrip_peak].time_beforeT0Corr;
  res.time_beforeBPCorr = sfits[istrip_peak].time_beforeBPCorr;
  res.timeStatus = sfits[istrip_peak].timeStatus;


  res.qleft = q1;
  res.qpeak = q0;
  res.qright = q2;

  //  res.diff = dmap["scordiff"]; 
  //  res.sig  = dmap["scordiff"]/dmap["dscordiff"]; 
  
  // cluster charge should be qsum over three strip... 3/21/2011
  res.charge = res.qleft+res.qpeak+res.qright;
  res.charge_beforeBPCorr = sfits[istrip_peak].charge_beforeBPCorr
    +sfits[istrip_peak-1].charge_beforeBPCorr
    +sfits[istrip_peak+1].charge_beforeBPCorr;
  
  res.dataMap = dmap;

  ATH_MSG_VERBOSE ( "   Position :: pos=" << res.position << " dpos:dtht=" << dpos << ":" << dpostht
                    << " ==>" << res.dposition << "  at tanth=" << tantheta );
  
  results.push_back(res);
  return results;
}
//int QratCscClusterFitter::
//fit(const StripList&, const StripFitList& sfits,
//    double& pos, double& dpos, Muon::CscClusterStatus& clustatus,
//    unsigned int& /*istrip*/, double& /*charge*/, double& /*time*/, DataMap* /*pdmap*/) const { return 0;}
//**********************************************************************

double QratCscClusterFitter::getCorrectedError(const CscPrepData* pclu, double slope) const {

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

  double newError = sqrt(dpos*dpos
                         - old_dpostht*old_dpostht
                         + new_dpostht*new_dpostht);

  
  ATH_MSG_VERBOSE ( "   Position :: pos=" << pos
                    << " dpos:newdpos=" << dpos << " : " << newError 
                    << "  " << old_dpostht << "  " << new_dpostht );

  if (slope<-990) {
    newError = sqrt(dpos*dpos
                    + old_dpostht*old_dpostht);
  }
  
  return newError;
}


//**********************************************************************

Results QratCscClusterFitter::fit(const StripFitList& sfits) const {
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
    
    res.dposition = sqrt(dpos*dpos + dpostht*dpostht);
    
    // Return the updated result.
    new_results.push_back(res);
  }
  
  return new_results;
}

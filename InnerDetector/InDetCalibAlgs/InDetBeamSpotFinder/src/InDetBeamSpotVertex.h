/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_INDETBEAMSPOTVERTEX_H
#define INDET_INDETBEAMSPOTVERTEX_H

////////////////////////////////////////////////////////////
// Author jwalder@cern.ch
// Concrete implementation of the IInDetBeamSpotTool class
// for Vertex methods of Beamspot determination.
////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetBeamSpotFinder/IInDetBeamSpotTool.h"
#include "TrkEventPrimitives/VertexType.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "TMinuit.h"
#include "TMath.h"
#include "TTree.h"
#include <atomic>
#include <string>
#include <vector>
#include <map>

namespace InDet {
  /** A concrete implementation of IInDetBeamSpotTool, using primary vertex information to 
      determine the position of the beamspot using a chi2 and/or log-likelihood method.
      @copydoc InDet::IInDetBeamSpotTool
  */
  class InDetBeamSpotVertex : public AthAlgTool, 
    virtual public IInDetBeamSpotTool {
    public:
    // Constructor
    InDetBeamSpotVertex( const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent);
    // Standard Destructor
    virtual ~InDetBeamSpotVertex() {}

    //Copy Constructor
    InDetBeamSpotVertex( const InDetBeamSpotVertex& rhs );
    

    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual FitStatus fit(std::vector< BeamSpot::VrtHolder >&);
    virtual FitID getFitID() const { return (m_getLLres ? vertexLL : vertexChi2  );}
    IInDetBeamSpotTool *Clone() {return new InDetBeamSpotVertex(*this);}

    double  getX( double  z) const {
      return (m_getLLres ? m_pLL(1) + m_pLL(3)*z  : m_p(1) + m_p(2)*z);}
    double  getY(double z) const {
      return (m_getLLres ? m_pLL(2) + m_pLL(4)*z  : m_p(3) + m_p(4)*z);}

    double  getZ() const { return (m_getLLres ? m_pLL(9):m_zSolved);}
    double  getErrX( double  z) const {
      return (m_getLLres ? sqrt(m_VLL(1,1) + 2*z*m_VLL(3,1) + z*z*m_VLL(3,3)):
	      sqrt(m_V(1,1) + 2*z*m_V(2,1) + z*z*m_V(2,2)));
    }
    double  getErrY( double  z) const{
	return  (m_getLLres ? sqrt(m_VLL(2,2) + 2*z*m_VLL(4,2) + z*z*m_VLL(4,4)):
		 sqrt(m_V(3,3) + 2*z*m_V(4,3) + z*z*m_V(4,4)) );
    }
    double  getErrZ() const {
      return (m_getLLres ?  (m_fixInputK ? sqrt(m_VLL(8,8)) : sqrt(m_VLL(9,9))):m_zErrSolved);} // defined as centroid of beam
    
    double  getSigmaX(double) const { 
      return (m_getLLres ? m_pLL(5) : m_def_sx);}
    double  getSigmaY(double) const  { 
      return (m_getLLres ? m_pLL(6) : m_def_sy);}
    double  getSigmaZ() const { 
      return (m_getLLres ? m_pLL(10) : m_def_sz);}
    double  getErrSigmaX(double) const {
      return (m_getLLres ? sqrt(m_VLL(5,5)) :0.);}
    double  getErrSigmaY(double) const {
      return (m_getLLres ? sqrt(m_VLL(6,6)) :0.);}
    double  getErrSigmaZ() const  {
      return (m_getLLres ?  (m_fixInputK ? sqrt(m_VLL(9,9)) : sqrt(m_VLL(10,10))) :0.);}

    double  getTiltX() const {
      return (m_getLLres ? m_pLL(3) :m_p(2));}
    double  getTiltY() const {
      return (m_getLLres ? m_pLL(4) :m_p(4));}
    double  getErrTiltX() const { 
      return (m_getLLres ? sqrt(m_VLL(3,3)):sqrt( m_V(2,2)));}
    double  getErrTiltY() const { 
      return (m_getLLres ? sqrt(m_VLL(4,4)):sqrt( m_V(4,4)));}

    //specific for Vertex
    double getRhoXY() const { return (m_getLLres ? m_pLL(7) : 0.);}
    double getK() const     { return (m_getLLres ? m_pLL(8) : 0.);}
    double getErrRhoXY() const { return  (m_getLLres ? sqrt(m_VLL(7,7)) : 0.);}
    double getErrK() const { return  (m_getLLres ?  (m_fixInputK ? 0. : sqrt(m_VLL(8,8)) ) : 0.);}

    double getSigmaXY(double z) const {return getRhoXY()*getSigmaX(z)*getSigmaY(z);}
    
    double getErrSigmaXY(double z) const {
      // Error proporagtion multiplied thorughout by sigmaXY to remove denominator ->  safer against zeros

      double rhoXY = getRhoXY();
      double sigmaX = getSigmaX(z);
      double sigmaY = getSigmaY(z);

      double errRhoXY = getErrRhoXY();
      double errSigmaX = getErrSigmaX(z);
      double errSigmaY = getErrSigmaY(z);

      double errSigmaXY = sqrt((errRhoXY*errRhoXY) * (sigmaX*sigmaX) * (sigmaY*sigmaY) +
			       (rhoXY*rhoXY) * (errSigmaX*errSigmaX) * (sigmaY*sigmaY) +
			       (rhoXY*rhoXY) * (sigmaX*sigmaX) * (errSigmaY*errSigmaY));
      return errSigmaXY;
    }
    CLHEP::HepSymMatrix getCov(double z) const; //x(z),y(z),tiltx,tilty

    const CLHEP::HepVector & getLLpos() const { return m_pLL;}
    const CLHEP::HepSymMatrix & getLLcov() const { return m_VLL;}
    virtual std::map<std::string,double> getCovMap() const;
    virtual std::map<std::string,double> getParamMap() const;
    void clear();
    private:

    //updated per vertex
    CLHEP::HepVector m_x;
    CLHEP::HepSymMatrix m_cov;
    double m_z, m_zErr;

    //solved matrices
    CLHEP::HepVector m_p;
    CLHEP::HepSymMatrix m_V;
    double m_zSolved, m_zErrSolved;

    //for LL output
    const int m_NPARS;
    CLHEP::HepVector m_pLL;
    CLHEP::HepSymMatrix m_VLL;

    long m_vertexCount;

    //    std::string m_containerName;

    bool m_useLL; // true if using the full log-likelihood method    
    bool m_getLLres; //use LL results, or fallback on Chi2
    std::vector< BeamSpot::VrtHolder > m_vertexData;

    
    ///////////////////////////////////
    // Initial fit values: set in Job options.
    // Note, it using Chi2, these may be overriding by the
    // results of the Chi2 fit.
    
    double m_init_x, m_init_y, m_init_z, m_init_sx,m_init_sy,m_init_sz;
    double m_init_ax, m_init_ay, m_init_k, m_init_rhoxy;
    
    double m_init_min_x, m_init_min_y, m_init_min_z, m_init_min_sx,m_init_min_sy,m_init_min_sz;
    double m_init_min_ax, m_init_min_ay, m_init_min_k, m_init_min_rhoxy;
    
    double m_init_max_x, m_init_max_y, m_init_max_z, m_init_max_sx,m_init_max_sy,m_init_max_sz;
    double m_init_max_ax, m_init_max_ay, m_init_max_k, m_init_max_rhoxy;

    int m_minuit_maxIter; // max iterations for Minuit to perform

    double m_def_x0,m_def_y0,m_def_z,m_def_ax,m_def_ay,m_def_sx,m_def_sy,m_def_sz; //default values: controled by jobOptions

    // selections based on chi2-estimated position of beamspot
    double m_sigTr; // number of sigmas from total beam-spread width
    double m_maxVtxErTr; // max uncertainty in vertex error in transverse direction
    double m_widthFail; // sigma value to 'not trust LL fit'.
    double m_rhoFail;   // rho value to 'not trust LL fit'.
    int m_singleIterationMax; // max allowed vertices to reject in a single iteration (-1 => all)
    bool m_truncatedRMS;
    float m_fractionRMS;
    bool m_setInitialRMS;

    FitStatus m_fitStatus;
    bool solveChi2();
    bool solveLL();
    bool applyOutlierRemoval();
    double m_outlierChi2Tr;
    int m_maxOutlierLoops; // maximum number of resursive loops in outlier removal to perform
    //LL setup
    int setInitialPars( TMinuit * minuit);
    int setParsFromChi2( TMinuit * minuit);

    bool setOutput( TMinuit * minuit);

    bool successfulFit( TMinuit * , 
			std::pair<int, std::string> & );
    void doFit2(TMinuit * ); 

    bool m_doFitSanityCheck; //<! determine that fit is really converged
    bool m_doChi2OutlierRemoval; //<! for chi2 only fit, run outlier removal as well.
    double m_kMaxFail, m_kMinFail; // max and min sanity values for k
    double m_minVtxProb; // probability cut on chi2/ndf
    
    bool m_fixInputK; // fix in LL fit the K factor to it's initial value
    bool m_useLLNorm; // use the normalisation mode of the pdf...
    bool m_fixWidth;
    
    int m_nUsed;

    mutable std::atomic<int> m_rCount{0}; // counter used for recussive mode
  };
}
#endif

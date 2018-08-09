/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_INDETBEAMSPOTROOFIT_H
#define INDET_INDETBEAMSPOTROOFIT_H

////////////////////////////////////////////////////////////
// Author jwalder@cern.ch
// Concrete implementation of the IInDetBeamSpotTool class
// for RooFit method of Beamspot determination.
////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetBeamSpotFinder/IInDetBeamSpotTool.h"
#include "TrkEventPrimitives/VertexType.h"
#include "CLHEP/Matrix/Vector.h"
#include "TMinuit.h"
#include "TMath.h"
#include "TTree.h"
#include <string>
#include <vector>
#include <RooAbsReal.h>
#include <RooRealVar.h>
#include "BeamSpotPdf.h"
#include "RooFitResult.h"

namespace InDet {
  /** A concrete implementation of IInDetBeamSpotTool, using primary vertex information to 
      determine the position of the beamspot using a chi2 and/or log-likelihood method.
      @copyDoc InDet::IInDetBeamSpotTool
  */
  class InDetBeamSpotRooFit : public AthAlgTool, 
    virtual public IInDetBeamSpotTool {
    public:
    // Constructor
    InDetBeamSpotRooFit( const std::string& type, 
			 const std::string& name, 
			 const IInterface* parent);
    // Standard Destructor
    virtual ~InDetBeamSpotRooFit() {}
    
    //Copy Constructor
    InDetBeamSpotRooFit( const InDetBeamSpotRooFit& rhs );
    
    //assignment
    InDetBeamSpotRooFit & operator =(const InDetBeamSpotRooFit & other) =default;
 
    //These are the member functions to keep:
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual std::map<std::string,double> getCovMap() const; 
    virtual std::map<std::string,double> getParamMap() const;
    virtual FitID getFitID() const { return RooFitLL; }
    virtual FitStatus fit(std::vector< BeamSpot::VrtHolder >&);
    IInDetBeamSpotTool *Clone() { return new InDetBeamSpotRooFit(*this); }
    //std::string combineCuts( RooDataSet & );

    private:
    //m_cov is the covariance matrix from the fit
    TMatrixDSym m_cov;
    //Fit parameter results
    double m_ax;
    double m_ay;
    double m_k;
    double m_mx;
    double m_my;
    double m_mz;
    double m_rho;
    double m_sx;
    double m_sy;
    double m_sz;
    int m_nUsed;
    std::vector< BeamSpot::VrtHolder > m_vertexData;
    long m_vertexCount;
    FitStatus m_fitStatus;
    std::string m_vtxCutString;
    double m_vtxResCut;
    double m_kStart;
    double m_rmsCutNum;
    bool m_kConst;
  };
}



#endif

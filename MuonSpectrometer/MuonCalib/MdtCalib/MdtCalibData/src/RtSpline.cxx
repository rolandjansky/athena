/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// c- c++
#include "iostream"

// this
#include "MdtCalibData/RtSpline.h"

// root
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "TSpline.h"

namespace MuonCalib {

    RtSpline::~RtSpline() {
        // as long as there is no default constructor, we can delete the TSpline3 without a check
        delete p_sp3;
    }

    void RtSpline::_init() {
        MsgStream log(Athena::getMessageSvc(), "RtSpline");
        // check for minimum number of parameters
        if (nPar() < 6) {
            log << MSG::ERROR << "Not enough parameters!" << endmsg;
            log << MSG::ERROR << "Minimum number of parameters is 6!" << endmsg;
            throw 1;
        }
        // check if the number of parameters is even
        if ((nPar() % 2) != 0) {
            log << MSG::ERROR << "RtSpline::_init(): Odd number of parameters!" << endmsg;
            throw 2;
        }
        // create spline
        Double_t *x = new Double_t[nPar() / 2];
        Double_t *y = new Double_t[nPar() / 2];
        for (unsigned int i = 0; i < nPar() / 2; i++) {
            x[i] = par(2 * i);
            y[i] = par(2 * i + 1);
        }
        p_sp3 = new TSpline3("Rt Relation", x, y, nPar() / 2, "b2e2", 0, 0);
        delete[] x;
        delete[] y;
    }  // end RtSpline::_init

    double RtSpline::radius(double t) const {
        // check for t_min and t_max
        if (t > p_sp3->GetXmax()) return p_sp3->Eval(p_sp3->GetXmax());
        if (t < p_sp3->GetXmin()) return p_sp3->Eval(p_sp3->GetXmin());
        double r = p_sp3->Eval(t);
        return r >= 0 ? r : 0;
    }

    double RtSpline::driftvelocity(double t) const { return p_sp3->Derivative(t); }

    double RtSpline::tLower(void) const { return par(0); }

    double RtSpline::tUpper(void) const { return par(nPar() - 2); }

}  // namespace MuonCalib

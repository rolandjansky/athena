/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//! PanTau includes
#include "PanTauAlgs/HelperFunctions.h"
#include "PanTauAlgs/TauConstituent.h"
#include "xAODPFlow/PFO.h"
#include "xAODTau/TauJet.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"

//! ROOT includes
#include "Math/SpecFuncMathMore.h"
#include "TMath.h"
#include "TRandom.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"
#include "TMatrixDSymEigen.h"
#include <vector>

#include <sstream>
#include <vector>
#include <cmath>
 


void PanTau::HelperFunctions::dumpFourMomentum(TLorentzVector FourMom) const {
    ATH_MSG_DEBUG("\t\tFourMomentum: Pt, Eta, Phi, M: " << FourMom.Pt() << " / " << FourMom.Eta() << " / " << FourMom.Phi() << " / " << FourMom.M());
    return;
}


void PanTau::HelperFunctions::dumpTauConstituent2(PanTau::TauConstituent2* tauConstituent) const {
    TLorentzVector tlv_TauConst = tauConstituent->p4();
    ATH_MSG_DEBUG("\t\tTauConstituent2 has charge " << tauConstituent->getCharge() << " and types " << tauConstituent->getTypeNameString() << " with BDT Value " << tauConstituent->getBDTValue());
    ATH_MSG_DEBUG("\t\tTauConstituent2 has pT, Eta, Phi, m: " << tlv_TauConst.Perp() << " / " << tlv_TauConst.Eta() << " / " << tlv_TauConst.Phi() << " / " << tlv_TauConst.M());
    
    return;
}


PanTau::TauConstituent2* PanTau::HelperFunctions::getNeutralConstWithLargestAngle(  TLorentzVector charged, 
										   std::vector<PanTau::TauConstituent2*> neutral) {
    ATH_MSG_DEBUG("getNeutralConstWithLargestAngle");
    if(neutral.size() == 0) return 0;
    //loop through neutrals to find the one with largest angle
    unsigned int    idx_Neutral = -1;
    double          angle_Neutral  = -1.;
    for(unsigned int iNeut=0; iNeut<neutral.size(); iNeut++) {
        TLorentzVector tlv_CurNeut = neutral[iNeut]->p4();
        double angle = charged.Angle(tlv_CurNeut.Vect());
        if(angle > angle_Neutral) {
            angle_Neutral = angle;
            idx_Neutral = iNeut;
        }
    }//end loop neutrals ());
    
    return neutral[idx_Neutral];
}



std::string PanTau::HelperFunctions::convertNumberToString(double x) const {
    std::stringstream tmpStream;
    tmpStream << x;
    return tmpStream.str();
}


// Will: moved to TauPi0ClusterScaler 
// void PanTau::HelperFunctions::vertexCorrection_PFOs(const xAOD::TauJet* tauJet, xAOD::PFO* pfo) const{
//     
//     const xAOD::Vertex* tauVertex = tauJet->vertexLink().cachedElement();
//     if(tauVertex == 0) {
//         ATH_MSG_WARNING("Did not find tau origin. No vertex correction will be applied to neutral PFO");
//         return;
//     }
//     ATH_MSG_DEBUG("Vtx link x/y/z = " << tauVertex->x() << ", " << tauVertex->y() << ", " << tauVertex->z());
//     ATH_MSG_DEBUG("Old cluster eta/phi: " << pfo->eta() << ", " << pfo->phi() << " CenterMag = " << pfo->centerMag());
//     
//     double clusterEta = pfo->eta();
//     double clusterPhi = pfo->phi();
//     double centerMag = pfo->centerMag();
// 
//     double radius = centerMag/cosh(clusterEta);
// 
//     double EtaVertexCorr = 0.0;
//     double PhiVertexCorr = 0.0;
// 
//     if (radius > 1.0 && centerMag > 1e-3){
//         EtaVertexCorr = (-tauVertex->z()/cosh(clusterEta) + (tauVertex->x()*cos(clusterPhi) + tauVertex->y()*sin(clusterPhi))*tanh(clusterEta))/radius;
//         PhiVertexCorr = (tauVertex->x()*sin(clusterPhi) - tauVertex->y()*cos(clusterPhi))/radius;
//     }
// 
//     clusterEta += EtaVertexCorr;
//     clusterPhi += PhiVertexCorr;
//     
//     pfo->setP4(pfo->pt(), clusterEta, clusterPhi, pfo->m());
//     ATH_MSG_DEBUG("New cluster eta/phi: " << clusterEta << ", " << clusterPhi);
// }



int PanTau::HelperFunctions::getBinIndex(std::vector<double> binEdges, double value) const {
    int resBin = -1;
    for(unsigned int i=0; i<binEdges.size()-1; i++) {
        double lowerEdge = binEdges[i];
        double upperEdge = binEdges[i+1];
        if(lowerEdge <= value && value < upperEdge) resBin = i;
    }
    if(resBin == -1) {
        ATH_MSG_WARNING("Could not find matching bin for value " << value << " in these bin edges:");
        for(unsigned int i=0; i<binEdges.size(); i++) ATH_MSG_WARNING("\tbin edge " << i << ": " << binEdges[i]);
    }
    return resBin;
}



double PanTau::HelperFunctions::stddev(double sumOfSquares, double sumOfValues, int numConsts) const {
    // calculate standard deviations according to:
    // sigma^2 = (sum_i x_i^2) / N - ((sum_i x_i)/N)^2 (biased maximum-likelihood estimate)
    // directly set sigma^2 to 0 in case of N=1, otherwise numerical effects may yield very small negative sigma^2
    if(numConsts == 1) return 0;
    double a = sumOfSquares / ((double)numConsts);
    double b = sumOfValues / ((double)numConsts);
    double stdDev = a - b*b;
    if(stdDev < 0.) stdDev = 0;
    return sqrt(stdDev);
}



double PanTau::HelperFunctions::deltaRprime(const TVector3& vec1, const TVector3& vec2) const {
    const double a = vec1.DeltaPhi(vec2);
    const double b = vec1.Theta() - vec2.Theta();
    double dRprime = sqrt(a*a + b*b);
    return dRprime;
}



int PanTau::HelperFunctions::iPow(int man, int exp) const {
    int ans = 1;
    for (int k = 0; k < exp; k++) {
        ans = ans * man;
    }
    return ans;
}



double PanTau::HelperFunctions::ulAngle(double x, double y) const {
    Double_t ulangl = 0;
    Double_t r = TMath::Sqrt(x * x + y * y);
    if (r < 1.0E-20) {
        return ulangl;
    }
    if (TMath::Abs(x) / r < 0.8) {
        ulangl = sign(TMath::ACos(x / r), y);
    } else {
        ulangl = TMath::ASin(y / r);
        if (x < 0. && ulangl >= 0.) {
            ulangl = TMath::Pi() - ulangl;
        } else if (x < 0.) {
            ulangl = -TMath::Pi() - ulangl;
        }
    }
    return ulangl;
}



double PanTau::HelperFunctions::sign(double a, double b) const {
    if (b < 0) {
        return -TMath::Abs(a);
    } else {
        return TMath::Abs(a);
    }
}



// JetProperties Thrust:  code stolen from ftp://ftp.slac.stanford.edu/groups/lcd/Physics_tools/ (by M. Iwasaki)
std::vector<double> PanTau::HelperFunctions::calcThrust(std::vector<TauConstituent2*>* tauConstituents, bool& calcIsValid) const{
    TRandom random;
    int maxpart = 1000;
    double dDeltaThPower(0);
    int iFast(4);
    double dConv(0.0001);
    int iGood(2);
    TMatrix dAxes;
    dAxes.ResizeTo(4, 4);
    //To make this look like normal physics notation the
    //zeroth element of each array, mom[i][0], will be ignored
    //and operations will be on elements 1,2,3...
    TMatrix mom(maxpart, 6);
    Double_t tmax = 0;
    Double_t phi = 0.;
    Double_t the = 0.;
    Double_t sgn;
    TMatrix fast(iFast + 1, 6);
    TMatrix work(11, 6);
    Double_t tdi[4] = {0., 0., 0., 0.};
    Double_t tds;
    Double_t tpr[4] = {0., 0., 0., 0.};
    Double_t thp;
    Double_t thps;
    TMatrix temp(3, 5);
    Int_t np = 0;

    Double_t dThrust[4];

    for (std::vector<TauConstituent2*>::iterator p4iter = tauConstituents->begin(); p4iter != tauConstituents->end(); p4iter++) {
        TVector3 p = (*p4iter)->p4().Vect();

        if (np >= maxpart) {
            calcIsValid = false;
            return std::vector<double>(0);
        }
        
        mom(np, 1) = p.X();
        mom(np, 2) = p.Y();
        mom(np, 3) = p.Z();
        mom(np, 4) = p.Mag();
        
        if (TMath::Abs(dDeltaThPower) <= 0.001) {
            mom(np, 5) = 1.0;
        } else {
            mom(np, 5) = TMath::Power(mom(np, 4), dDeltaThPower);
        }
        tmax = tmax + mom(np, 4) * mom(np, 5);
        np++;
    }
    if (np < 2) {
        dThrust[1] = -1.0;
        return std::vector<double>(0);
    }
    // for pass = 1: find thrust axis.
    // for pass = 2: find major axis.
    for (Int_t pass = 1; pass < 3; pass++) {
        if (pass == 2) {
            phi = ulAngle(dAxes(1, 1), dAxes(1, 2));
            ludbrb(&mom, 0, -phi, 0., 0., 0.);
            for (Int_t i = 0; i < 3; i++) {
                for (Int_t j = 1; j < 4; j++) {
                    temp(i, j) = dAxes(i + 1, j);
                }
                temp(i, 4) = 0;
            }
            ludbrb(&temp, 0., -phi, 0., 0., 0.);
            for (Int_t ib = 0; ib < 3; ib++) {
                for (Int_t j = 1; j < 4; j++) {
                    dAxes(ib + 1, j) = temp(ib, j);
                }
            }
            the = ulAngle(dAxes(1, 3), dAxes(1, 1));
            ludbrb(&mom, -the, 0., 0., 0., 0.);
            for (Int_t ic = 0; ic < 3; ic++) {
                for (Int_t j = 1; j < 4; j++) {
                    temp(ic, j) = dAxes(ic + 1, j);
                }
                temp(ic, 4) = 0;
            }
            ludbrb(&temp, -the, 0., 0., 0., 0.);
            for (Int_t id = 0; id < 3; id++) {
                for (Int_t j = 1; j < 4; j++) {
                    dAxes(id + 1, j) = temp(id, j);
                }
            }
        }
        for (Int_t ifas = 0; ifas < iFast + 1 ; ifas++) {
            fast(ifas, 4) = 0.;
        }
        // Find the iFast highest momentum particles and
        // put the highest in fast[0], next in fast[1],....fast[iFast-1].
        // fast[iFast] is just a workspace.
        for (Int_t i = 0; i < np; i++) {
            if (pass == 2) {
                mom(i, 4) = TMath::Sqrt(mom(i, 1) * mom(i, 1)
                                        + mom(i, 2) * mom(i, 2));
            }
            for (Int_t ifas = iFast - 1; ifas > -1; ifas--) {
                if (mom(i, 4) > fast(ifas, 4)) {
                    for (Int_t j = 1; j < 6; j++) {
                        fast(ifas + 1, j) = fast(ifas, j);
                        if (ifas == 0) fast(ifas, j) = mom(i, j);
                    }
                } else {
                    for (Int_t j = 1; j < 6; j++) {
                        fast(ifas + 1, j) = mom(i, j);
                    }
                    break;
                }
            }
        }
        // Find axis with highest thrust (case 1)/ highest major (case 2).
        for (Int_t ie = 0; ie < work.GetNrows(); ie++) {
            work(ie, 4) = 0.;
        }
        Int_t p = TMath::Min(iFast, np) - 1;
        // Don't trust Math.pow to give right answer always.
        // Want nc = 2**p.
        Int_t nc = iPow(2, p);
        for (Int_t n = 0; n < nc; n++) {
            for (Int_t j = 1; j < 4; j++) {
                tdi[j] = 0.;
            }
            for (Int_t i = 0; i < TMath::Min(iFast, n); i++) {
                sgn = fast(i, 5);
                if (iPow(2, (i + 1))*((n + iPow(2, i)) / iPow(2, (i + 1))) >= i + 1) {
                    sgn = -sgn;
                }
                for (Int_t j = 1; j < 5 - pass; j++) {
                    tdi[j] = tdi[j] + sgn * fast(i, j);
                }
            }
            tds = tdi[1] * tdi[1] + tdi[2] * tdi[2] + tdi[3] * tdi[3];
            for (Int_t iw = TMath::Min(n, 9); iw > -1; iw--) {
                if (tds > work(iw, 4)) {
                    for (Int_t j = 1; j < 5; j++) {
                        work(iw + 1, j) = work(iw, j);
                        if (iw == 0) {
                            if (j < 4) {
                                work(iw, j) = tdi[j];
                            } else {
                                work(iw, j) = tds;
                            }
                        }
                    }
                } else {
                    for (Int_t j = 1; j < 4; j++) {
                        work(iw + 1, j) = tdi[j];
                    }
                    work(iw + 1, 4) = tds;
                }
            }
        }
        // Iterate direction of axis until stable maximum.
        dThrust[pass] = 0;
        thp = -99999.;
        Int_t nagree = 0;
        for (Int_t iw = 0;
                iw < TMath::Min(nc, 10) && nagree < iGood; iw++) {
            thp = 0.;
            thps = -99999.;
            while (thp > thps + dConv) {
                thps = thp;
                for (Int_t j = 1; j < 4; j++) {
                    if (thp <= 1E-10) {
                        tdi[j] = work(iw, j);
                    } else {
                        tdi[j] = tpr[j];
                        tpr[j] = 0;
                    }
                }
                for (Int_t i = 0; i < np; i++) {
                    sgn = sign(mom(i, 5),
                               tdi[1] * mom(i, 1) +
                               tdi[2] * mom(i, 2) +
                               tdi[3] * mom(i, 3));
                    for (Int_t j = 1; j < 5 - pass; j++) {
                        tpr[j] = tpr[j] + sgn * mom(i, j);
                    }
                }
                thp = TMath::Sqrt(tpr[1] * tpr[1]
                                  + tpr[2] * tpr[2]
                                  + tpr[3] * tpr[3]) / tmax;
            }
            // Save good axis. Try new initial axis until enough
            // tries agree.
            if (thp < dThrust[pass] - dConv) {
                break;
            }
            if (thp > dThrust[pass] + dConv) {
                nagree = 0;
                sgn = iPow(-1, (Int_t)TMath::Nint(random.Rndm()));
                for (Int_t j = 1; j < 4; j++) {
                    dAxes(pass, j) = sgn * tpr[j] / (tmax * thp);
                }
                dThrust[pass] = thp;
            }
            nagree = nagree + 1;
        }
    }
    // Find minor axis and value by orthogonality.
    sgn = iPow(-1, (Int_t)TMath::Nint(random.Rndm()));
    dAxes(3, 1) = -sgn * dAxes(2, 2);
    dAxes(3, 2) = sgn * dAxes(2, 1);
    dAxes(3, 3) = 0.;
    thp = 0.;
    for (Int_t i = 0; i < np; i++) {
        thp += mom(i, 5) * TMath::Abs(dAxes(3, 1) * mom(i, 1) +
                                      dAxes(3, 2) * mom(i, 2));
    }
    
    if(tmax==0) {
        calcIsValid = false;
        return std::vector<double>(0);
    }
    
    calcIsValid = true;
    
    dThrust[3] = thp / tmax;

    
    // Rotate back to original coordinate system.
    for (Int_t i6 = 0; i6 < 3; i6++) {
        for (Int_t j = 1; j < 4; j++) {
            temp(i6, j) = dAxes(i6 + 1, j);
        }
        temp(i6, 4) = 0;
    }
    ludbrb(&temp, the, phi, 0., 0., 0.);
    for (Int_t i7 = 0; i7 < 3; i7++) {
        for (Int_t j = 1; j < 4; j++) {
            dAxes(i7 + 1, j) = temp(i7, j);
        }
    }
    TVector3 thrustAxis(dAxes(1, 1), dAxes(1, 2), dAxes(1, 3));
    TVector3 majorAxis(dAxes(2, 1), dAxes(2, 2), dAxes(2, 3));
    std::vector<double> values(0);
    values.push_back(dThrust[1]);
    values.push_back(dThrust[2]);
    values.push_back(dThrust[3]);
    return values;
}



// JetProperties ludbrb:  This is used in calcThrust
void PanTau::HelperFunctions::ludbrb(TMatrix* mom,
        double the,
        double phi,
        double bx,
        double by,
        double bz) const {
    // Ignore "zeroth" elements in rot,pr,dp.
    // Trying to use physics-like notation.
    TMatrix rot(4, 4);
    Double_t pr[4];
    Double_t dp[5];
    Int_t np = mom->GetNrows();
    if (the*the + phi*phi > 1.0E-20) {
        rot(1, 1) = TMath::Cos(the) * TMath::Cos(phi);
        rot(1, 2) = -TMath::Sin(phi);
        rot(1, 3) = TMath::Sin(the) * TMath::Cos(phi);
        rot(2, 1) = TMath::Cos(the) * TMath::Sin(phi);
        rot(2, 2) = TMath::Cos(phi);
        rot(2, 3) = TMath::Sin(the) * TMath::Sin(phi);
        rot(3, 1) = -TMath::Sin(the);
        rot(3, 2) = 0.0;
        rot(3, 3) = TMath::Cos(the);
        for (Int_t i = 0; i < np; i++) {
            for (Int_t j = 1; j < 4; j++) {
                pr[j] = (*mom)(i, j);
                (*mom)(i, j) = 0;
            }
            for (Int_t jb = 1; jb < 4; jb++) {
                for (Int_t k = 1; k < 4; k++) {
                    (*mom)(i, jb) = (*mom)(i, jb) + rot(jb, k) * pr[k];
                }
            }
        }
        Double_t beta = TMath::Sqrt(bx * bx + by * by + bz * bz);
        if (beta*beta > 1.0E-20) {
            if (beta >  0.99999999) {
                //send message: boost too large, resetting to <~1.0.
                bx = bx * (0.99999999 / beta);
                by = by * (0.99999999 / beta);
                bz = bz * (0.99999999 / beta);
                beta =   0.99999999;
            }
            Double_t gamma = 1.0 / TMath::Sqrt(1.0 - beta * beta);
            for (Int_t i = 0; i < np; i++) {
                for (Int_t j = 1; j < 5; j++) {
                    dp[j] = (*mom)(i, j);
                }
                Double_t bp = bx * dp[1] + by * dp[2] + bz * dp[3];
                Double_t gbp = gamma * (gamma * bp / (1.0 + gamma) + dp[4]);
                (*mom)(i, 1) = dp[1] + gbp * bx;
                (*mom)(i, 2) = dp[2] + gbp * by;
                (*mom)(i, 3) = dp[3] + gbp * bz;
                (*mom)(i, 4) = gamma * (dp[4] + bp);
            }
        }
    }
    return;
}



// JetProperties FoxWolfram Moments:  See here: http://cepa.fnal.gov/psm/simulation/mcgen/lund/pythia_manual/pythia6.3/pythia6301/node215.html
std::vector<double> PanTau::HelperFunctions::calcFWMoments(std::vector<TauConstituent2*>* tauConstituents, bool& calcIsValid) const{

    const unsigned int nMomentsToCalc = 5;
    std::vector<double> fwValues = std::vector<double>(nMomentsToCalc, -9.999);
    
    calcIsValid = true;
    if(tauConstituents->size() == 0) {
        calcIsValid = false;
        return fwValues;
    }
    double jetE = 0.;

    for (std::vector<TauConstituent2*>::iterator p4iter1 = tauConstituents->begin(); p4iter1 != tauConstituents->end(); p4iter1++) {
        TVector3 p1 = (*p4iter1)->p4().Vect();
        jetE += (*p4iter1)->p4().E();

        for (std::vector<TauConstituent2*>::iterator p4iter2 = tauConstituents->begin(); p4iter2 != tauConstituents->end(); p4iter2++) {

            TVector3 p2 = (*p4iter2)->p4().Vect();

            //find angle between vectors
            double angle = (double) p1.Angle(p2) ;
            
            //new calculation
            for(unsigned int iMoment=0; iMoment<nMomentsToCalc; iMoment++) {
                if(p1.Mag() == 0) continue; //curMoment will be 0 and not change anything in this case
                if(p2.Mag() == 0) continue; //
                double curMoment = (p1.Mag() * p2.Mag() * ROOT::Math::legendre(  iMoment, TMath::Cos(angle)));
                fwValues[iMoment] += curMoment;
            }//end loop over moments
        }//end loop over second particle
    }//end loop over first particle

    
    if(jetE == 0) {
        calcIsValid = false;
        return fwValues;
    }
    
    double jetE2 = (double)(jetE * jetE);
    
    //divide by squared jet energy
    for(unsigned int iMom=0; iMom<nMomentsToCalc; iMom++) {
        fwValues[iMom] = fwValues[iMom]/jetE2;
    }
    
    //check if division by 0 will happen. if yes, set flag that calculation was not possible & return
    // the returned values will not be used because the calcIsValid was set to false
    if(fwValues[0] == 0) {
        calcIsValid = false;
        return fwValues;
    }
    
    //normalize to 0th moment
    for(unsigned int iMom=0; iMom<nMomentsToCalc; iMom++) {
        fwValues[iMom] = fwValues[iMom]/fwValues[0];
    }
    
    return fwValues;
}



//! //////////////////////////////////////////
/// JetProperties: Sphericity
//! //////////////////////////////////////////
std::vector<double> PanTau::HelperFunctions::calcSphericity(std::vector<TauConstituent2*>* tauConstituents, bool& calcIsValid) const{

    double sphTensor[9];
    double norm = 0.;
    for (unsigned int i = 0; i < 9;i++) {
        sphTensor[i] = 0.;
    }


    for (std::vector<TauConstituent2*>::iterator p4iter = tauConstituents->begin(); p4iter != tauConstituents->end(); p4iter++) {

        sphTensor[0] += (*p4iter)->p4().Px() * (*p4iter)->p4().Px();
        sphTensor[1] += (*p4iter)->p4().Px() * (*p4iter)->p4().Py();
        sphTensor[2] += (*p4iter)->p4().Px() * (*p4iter)->p4().Pz();
        sphTensor[3] += (*p4iter)->p4().Py() * (*p4iter)->p4().Px();
        sphTensor[4] += (*p4iter)->p4().Py() * (*p4iter)->p4().Py();
        sphTensor[5] += (*p4iter)->p4().Py() * (*p4iter)->p4().Pz();
        sphTensor[6] += (*p4iter)->p4().Pz() * (*p4iter)->p4().Px();
        sphTensor[7] += (*p4iter)->p4().Pz() * (*p4iter)->p4().Py();
        sphTensor[8] += (*p4iter)->p4().Pz() * (*p4iter)->p4().Pz();

        norm += (*p4iter)->p4().Vect() * (*p4iter)->p4().Vect();

    }
    
    if(norm == 0) {
        calcIsValid = false;
        return std::vector<double>(0);
    }
    calcIsValid = true;
    
    for (int i = 0; i < 9 ; i++) {
        sphTensor[i] = sphTensor[i] / norm;
    }


    //find eigenvalues
    TMatrixDSym matrix(3);
    matrix.SetMatrixArray(sphTensor);

    TMatrixDSymEigen eigen(matrix);
    TVectorD E = eigen.GetEigenValues();

    //get variables
    double Emax = E[0];
    double Emin = E[0];
    int j = 0, k = 0;
    for (int i = 0; i <= 2 ; ++i) {
        if (E[i] > Emax) {
            Emax = E[i];
            j = i;
        }
        if (E[i] < Emin) {
            Emin = E[i];
            k = i;
        }
    }

    int l = 3 - j - k;


    //sphericity: 3/2*(lamba2+lambda3)
    double sph = Emax;
    sph = 3. / 2. * (E[k] + E[l]);

    //aplanrity: 3/2*(lambda3)
    double apl = 3. / 2. * E[k];

    //planarity: lambda2-lambda3
    double pla = E[l] - E[k];

    std::vector<double> values(0);
    values.push_back(sph);
    values.push_back(apl);
    values.push_back(pla);

    return values;
}






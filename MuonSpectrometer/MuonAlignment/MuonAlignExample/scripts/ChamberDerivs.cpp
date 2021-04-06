/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ChamberDerivs.h"

#include <cmath>

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "TDecompBK.h"
#include "TDecompSVD.h"
#include "TMatrix.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"

using namespace CLHEP;

//________________________________________________________________________
ChamberDerivs::ChamberDerivs() : m_npar(0), m_nchambers(0), m_nindices(0) {}

//________________________________________________________________________
ChamberDerivs::ChamberDerivs(int npar, int nchambers) :
    m_npar(npar),
    m_nchambers(nchambers),
    m_nindices(npar * nchambers),
    m_chamberIds(new int[m_nchambers]),
    m_matrixIndices(new int[m_nindices]),
    m_hitmap(new int[m_nchambers * m_nchambers]),
    m_secDeriv(new double[m_nindices * m_nindices]),
    m_firstDeriv(new double[m_nindices]) {
    for (int i = 0; i < m_nchambers; i++) { m_chamberIds[i] = -1; }
    for (int j = 0; j < m_nchambers * m_nchambers; j++) { m_hitmap[j] = 0; }
    for (int i = 0; i < m_nindices; i++) {
        m_firstDeriv[i] = 0.;
        m_matrixIndices[i] = 0;
    }
    for (int j = 0; j < m_nindices * m_nindices; j++) { m_secDeriv[j] = 0.; }
}

//________________________________________________________________________
ChamberDerivs::~ChamberDerivs() {
    delete[] m_secDeriv;
    delete[] m_firstDeriv;
    delete[] m_chamberIds;
    delete[] m_matrixIndices;
    delete[] m_hitmap;
}

/*
//________________________________________________________________________
void ChamberDerivs::incrementHitmap(int ich, int jch,
                                    int ntracks)
{
  m_hitmap[ich*m_nchambers+jch]+=ntracks;
}
*/

//________________________________________________________________________
double ChamberDerivs::robustAlignPar(int index) const { return -(this->firstDeriv(index) / this->secondDeriv(index, index)); }

//________________________________________________________________________
double ChamberDerivs::robustAlignErr(int index) const { return std::sqrt(2. / std::fabs(this->secondDeriv(index, index))); }

//________________________________________________________________________
double ChamberDerivs::robustAlignPar(int ich, int ipar) const {
    return -(this->firstDeriv(ich, ipar) / this->secondDeriv(ich, ipar, ich, ipar));
}

//________________________________________________________________________
double ChamberDerivs::robustAlignErr(int ich, int ipar) const { return std::sqrt(2. / std::fabs(this->secondDeriv(ich, ipar, ich, ipar))); }

//________________________________________________________________________
void ChamberDerivs::PrintSecondDerivs(int ich) const {
    if (ich < 0) {
        TMatrixDSym a(m_nindices, m_secDeriv);
        // a.Print();
    }
    if (ich >= m_nchambers) return;

    cout << endl << endl << "secndDeriv: " << endl;
    if (ich < 0) {
        for (int i_index = 0; i_index < m_nindices; i_index++) {
            for (int j_index = 0; j_index < m_nindices; j_index++) { cout << this->secondDeriv(i_index, j_index) << " "; }
            cout << endl;
        }
    } else {
        for (int ipar = 0; ipar < m_npar; ipar++) {
            for (int jpar = 0; jpar < m_npar; jpar++) { cout << this->secondDeriv(ich, ipar, ich, jpar) << " "; }
            cout << endl;
        }
    }
}

//________________________________________________________________________
void ChamberDerivs::PrintFirstDerivs(int ich) const {
    if (ich >= m_nchambers) return;
    cout << endl << endl << "firstDeriv: " << endl;
    if (ich < 0) {
        int index = 0;
        for (int ich = 0; ich < m_nchambers; ich++) {
            for (int ipar = 0; ipar < m_npar; ipar++) { cout << "[" << index++ << "] = " << this->firstDeriv(ich, ipar) << endl; }
        }
    } else {
        for (int ipar = 0; ipar < m_npar; ipar++) { cout << "[" << ipar << "] = " << this->firstDeriv(ich, ipar) << endl; }
    }
}

//________________________________________________________________________
void ChamberDerivs::PrintChambers(int ich) const {
    if (ich < 0) {
        cout << endl << "chambers: " << endl;
        for (int iich = 0; iich < m_nchambers; iich++) {
            cout << "[" << iich << "]: " << this->chamberId(iich) << ", nhits: " << this->nhits(iich, iich) << endl;
        }
    } else {
        cout << endl << "chamber[" << ich << "]: " << this->chamberId(ich) << ", nhits: " << this->nhits(ich, ich) << endl;
    }
}

//________________________________________________________________________
ChamberDerivs ChamberDerivs::trimmedChamberDerivs(int minHitsOnDiagonal, int minHitsOffDiagonal) const {
    // see how many chambers have enough hits on diagonal
    cout << "starting with " << this->nchambers() << " chambers" << endl;

    vector<int> goodchamber_ich;
    for (int ich = 0; ich < this->nchambers(); ich++) {
        int nhits = this->nhits(ich, ich);
        cout << "ich=" << ich << "/" << this->nchambers() << ", nhits: " << nhits << endl;
        if (nhits >= minHitsOnDiagonal) { goodchamber_ich.push_back(ich); }
    }
    int ngoodchambers = goodchamber_ich.size();
    cout << "ngoodchambers: " << ngoodchambers << endl;

    // create new chamber derivs
    ChamberDerivs chDerivs(m_npar, ngoodchambers);
    for (int ich = 0; ich < ngoodchambers; ich++) {
        int old_ich = goodchamber_ich[ich];
        chDerivs.setChamberId(ich, this->chamberId(old_ich));
        for (int ipar = 0; ipar < m_npar; ipar++) {
            chDerivs.setFirstDeriv(ich, ipar, this->firstDeriv(old_ich, ipar));
            chDerivs.setMatrixIndex(ich, ipar, this->matrixIndex(old_ich, ipar));

            for (int jch = ich; jch < ngoodchambers; jch++) {
                int old_jch = goodchamber_ich[jch];
                int nOffDiagHits = this->nhits(old_ich, old_jch);
                if (jch != ich) {
                    if (nOffDiagHits < minHitsOffDiagonal) continue;
                }
                if (ipar == 0) chDerivs.setNHits(ich, jch, this->nhits(old_ich, old_jch));
                for (int jpar = 0; jpar < m_npar; jpar++) {
                    chDerivs.setSecondDeriv(ich, ipar, jch, jpar, this->secondDeriv(old_ich, ipar, old_jch, jpar));
                }
            }
        }
    }
    return chDerivs;
}

//________________________________________________________________________
ChamberDerivs ChamberDerivs::trimmedChamberDerivsBySV(double singularValueCut, int minHitsOffDiagonal) const {
    cout << "trimmedChamberDerivsBySV" << endl;

    cout << "starting with " << this->nchambers() << " chambers" << endl;

    // see how many chambers have good singular value
    vector<int> goodchamber_ich;
    for (int ich = 0; ich < this->nchambers(); ich++) {
        vector<double> sv;
        const TMatrixD *u, *vinv;
        this->singularValues(sv, u, vinv, ich);
        delete u;
        delete vinv;

        double minval = 1.e20;
        // cout<<"getting min"<<endl;
        for (int ipar = 0; ipar < (int)sv.size(); ipar++) {
            // cout<<"sv["<<ipar<<"]="<<sv[ipar]<<endl;
            if (sv[ipar] < minval) { minval = sv[ipar]; }
        }
        // cout<<endl;

        // cout<<"ich="<<ich<<"/"<<this->nchambers()<<endl;
        if (minval > singularValueCut) { goodchamber_ich.push_back(ich); }
    }

    int ngoodchambers = goodchamber_ich.size();
    cout << "ngoodchambers: " << ngoodchambers << endl;

    // create new chamber derivs
    ChamberDerivs chDerivs(m_npar, ngoodchambers);
    for (int ich = 0; ich < ngoodchambers; ich++) {
        int old_ich = goodchamber_ich[ich];
        chDerivs.setChamberId(ich, this->chamberId(old_ich));
        for (int ipar = 0; ipar < m_npar; ipar++) {
            chDerivs.setFirstDeriv(ich, ipar, this->firstDeriv(old_ich, ipar));
            chDerivs.setMatrixIndex(ich, ipar, this->matrixIndex(old_ich, ipar));

            for (int jch = ich; jch < ngoodchambers; jch++) {
                int old_jch = goodchamber_ich[jch];
                int nOffDiagHits = this->nhits(old_ich, old_jch);
                if (jch != ich) {
                    if (nOffDiagHits < minHitsOffDiagonal) continue;
                }
                if (ipar == 0) chDerivs.setNHits(ich, jch, this->nhits(old_ich, old_jch));
                for (int jpar = 0; jpar < m_npar; jpar++) {
                    chDerivs.setSecondDeriv(ich, ipar, jch, jpar, this->secondDeriv(old_ich, ipar, old_jch, jpar));
                }
            }
        }
    }
    return chDerivs;
}

//________________________________________________________________________
ChamberDerivs* ChamberDerivs::oneChamberDerivs(int ich) const {
    ChamberDerivs* localChDerivs = new ChamberDerivs(m_npar, 1);
    localChDerivs->setChamberId(0, this->chamberId(ich));
    localChDerivs->setNHits(0, this->nhits(ich, ich));
    for (int ipar = 0; ipar < m_npar; ipar++) {
        localChDerivs->setMatrixIndex(ipar, this->matrixIndex(ich, ipar));
        localChDerivs->setFirstDeriv(ipar, this->firstDeriv(ich, ipar));
        for (int jpar = 0; jpar < m_npar; jpar++) {
            double sd = this->secondDeriv(ich, ipar, ich, jpar);
            localChDerivs->setSecondDeriv(ipar, jpar, sd);
        }
    }
    return localChDerivs;
}

//________________________________________________________________________
double ChamberDerivs::condition(int ich) const {
    cout << "getting secderiv, ich " << ich << endl;
    const TMatrixDSym* mat = this->secDerivMatrixROOT(ich);

    cout << "got matrix, decomposing" << endl;
    TDecompSVD decompMat(*mat);
    decompMat.Decompose();

    TVectorD sig = decompMat.GetSig();
    // cout<<"Sig: "<<endl;
    // sig.Print();

    double d1, d2;
    decompMat.Det(d1, d2);
    // cout<<"d1: "<<d1<<", d2: "<<d2<<endl;

    double cond = decompMat.Condition();
    // cout<<"condition: "<<cond<<endl;

    delete mat;
    return cond;
}

//________________________________________________________________________
void ChamberDerivs::singularValues(vector<double>& sv, const TMatrixD*& u, const TMatrixD*& vinv, int ich) const {
    TMatrixDSym* mat = this->secDerivMatrixROOT(ich);

    TDecompSVD decompMat(*mat);
    decompMat.Decompose();
    const TVectorD& svV = decompMat.GetSig();

    // cout<<"getting elements"<<endl;
    sv.clear();
    for (int i = 0; i < svV.GetNoElements(); i++) {
        // cout<<"sv["<<i<<"]="<<svV[i]<<endl;
        sv.push_back(svV[i]);
    }

    u = new TMatrixD(decompMat.GetU());
    cout << "U:" << endl;
    u->Print();

    TMatrixD v = decompMat.GetV();
    vinv = new TMatrixD(v.Invert());
    cout << "Vinv:" << endl;
    vinv->Print();

    delete mat;

    return;
}

//________________________________________________________________________
double ChamberDerivs::conditionChamberRemoved(int ichToRemove) const {
    int nindices = m_nindices - m_npar;
    double* array = new double[nindices * nindices];
    // cout<<"array size: "<<nindices*nindices<<endl;

    int index = 0;
    for (int ich = 0; ich < m_nchambers; ich++) {
        if (ich == ichToRemove) continue;
        for (int ipar = 0; ipar < m_npar; ipar++) {
            for (int jch = 0; jch < m_nchambers; jch++) {
                if (jch == ichToRemove) continue;
                for (int jpar = 0; jpar < m_npar; jpar++) {
                    array[index] = this->secondDeriv(ich, ipar, jch, jpar);
                    // cout<<"array["<<index<<"]="<<array[index]<<endl;
                    index++;
                }
            }
        }
    }

    TMatrixDSym* mat = new TMatrixDSym(nindices);
    index = 0;
    for (int i = 0; i < nindices; i++) {
        for (int j = 0; j < nindices; j++) {
            (*mat)[i][j] = array[index++];
            // cout<<"mat["<<i<<"]["<<j<<"]="<<(*mat)[i][j]<<endl;
        }
    }
    delete[] array;

    // cout<<"decomposing"<<endl;
    TDecompSVD decompMat(*mat);
    decompMat.Decompose();

    TVectorD sig = decompMat.GetSig();
    // cout<<"Sig: "<<endl;
    // sig.Print();

    // cout<<"getting condition"<<endl;
    double cond = decompMat.Condition();
    // cout<<"done"<<endl;

    delete mat;
    return cond;
}

//________________________________________________________________________
void ChamberDerivs::constrainLowestEigenvector(const std::map<int, string>& chamberIdMap) {
    int npar = this->npar();
    for (int ich = 0; ich < this->nchambers(); ich++) {
        vector<double> sv;
        const TMatrixD *u, *vinv;
        this->singularValues(sv, u, vinv, ich);

        cout << "2nd deriv before: " << endl;
        this->PrintSecondDerivs(ich);

        const int id = chamberId(ich);
        cout << "id: " << id << endl;

        std::map<int, string>::const_iterator it = chamberIdMap.find(id);
        string name = it->second;

        cout << "ch: " << name << "(before)" << endl;
        for (int i = 0; i < npar; i++) cout << "sv[" << i << "]=" << sv[i] << endl;

        cout << "u: " << endl;
        u->Print();
        // cout<<"vinv: "<<endl; vinv->Print();

        TMatrixDSym sd(npar);
        for (int ipar = 0; ipar < npar - 2; ipar++) sd[ipar][ipar] = sv[ipar];
        sd[npar - 2][npar - 2] = sv[0];
        sd[npar - 1][npar - 1] = sv[0];
        TMatrixD D = (*u) * sd * (*vinv);
        D.Print();
        for (int ipar = 0; ipar < npar; ipar++) {
            for (int jpar = 0; jpar < npar; jpar++) {
                double val = D[ipar][jpar];
                setSecondDeriv(ich, ipar, ich, jpar, val);
            }
        }
        delete u;
        delete vinv;

        cout << "2nd deriv after: " << endl;
        this->PrintSecondDerivs(ich);

        this->singularValues(sv, u, vinv, ich);
        cout << "ich: " << name << "(after)" << endl;
        for (int i = 0; i < npar; i++) cout << "sv[" << i << "]=" << sv[i] << endl;
        cout << "u: " << endl;
        u->Print();

        delete u;
        delete vinv;
    }
    return;
}

//________________________________________________________________________
double ChamberDerivs::determinant(int ich) const {
    const TMatrixDSym* mat = this->secDerivMatrixROOT(ich);

    cout << "getting det" << endl;
    double val = mat->Determinant();
    delete mat;
    cout << "val: " << val << endl;
    return val;
}

//________________________________________________________________________
HepSymMatrix* ChamberDerivs::secDerivMatrixCLHEP(int ich) const {
    HepSymMatrix* newmat(0);
    if (ich < 0) {
        newmat = new HepSymMatrix(m_nindices);
        for (int i = 0; i < m_nindices; i++) {
            for (int j = i; j < m_nindices; j++) {
                int index = i * m_nindices + j;
                (*newmat)[i + 1][j + 1] = m_secDeriv[index];
                if (i != j) (*newmat)[j + 1][i + 1] = m_secDeriv[index];
                // cout<<"newmat["<<i<<"]["<<j<<"]="<<m_secDeriv[index]<<endl;
            }
        }
    } else {
        newmat = new HepSymMatrix(m_npar);
        for (int ipar = 0; ipar < m_npar; ipar++) {
            for (int jpar = ipar; jpar < m_npar; jpar++) {
                double sd = this->secondDeriv(ich, ipar, ich, jpar);
                (*newmat)[ipar + 1][jpar + 1] = sd;
                if (ipar != jpar) (*newmat)[jpar + 1][ipar + 1] = sd;
                // cout<<"newmat["<<ipar<<"]["<<jpar<<"]="<<sd<<endl;
            }
        }
    }
    return newmat;
}

//________________________________________________________________________
TMatrixDSym* ChamberDerivs::secDerivMatrixROOT(int ich, int npar) const {
    // this->PrintSecondDerivs(ich);

    int nindices = 0;
    double* array = 0;
    if (ich < 0) {
        cout << "nindices: " << nindices << endl;
        nindices = m_nindices;
        array = new double[nindices * nindices];
        for (int i = 0; i < m_nindices; i++) {
            for (int j = 0; j < m_nindices; j++) {
                int index = i * m_nindices + j;
                double sd = m_secDeriv[index];
                array[index] = sd;
                index = j * m_nindices + i;
                array[index] = sd;
            }
        }
    } else {
        nindices = (npar == 0) ? m_npar : npar;
        array = new double[nindices * nindices];
        for (int ipar = 0; ipar < nindices; ipar++) {
            for (int jpar = ipar; jpar < nindices; jpar++) {
                double sd = this->secondDeriv(ich, ipar, ich, jpar);
                int index = ipar * nindices + jpar;
                array[index] = sd;
                index = jpar * nindices + ipar;
                array[index] = sd;
            }
        }
    }

    TMatrixDSym* mat = new TMatrixDSym(nindices);
    int index(0);
    for (int i = 0; i < nindices; i++) {
        for (int j = 0; j < nindices; j++) { (*mat)[i][j] = array[index++]; }
    }
    // mat->Print();

    delete[] array;
    return mat;
}

//________________________________________________________________________
double* ChamberDerivs::solve(double*& err, int ich, SolveType solveType) const {
    if (ich >= 0) {
        ChamberDerivs* localChDeriv = oneChamberDerivs(ich);
        double* res = localChDeriv->solve(err, -1, solveType);
        cout << "res[0]=" << res[0] << endl;
        delete localChDeriv;
        return res;
    }

    double* res = (ich == -1) ? new double[m_nindices] : new double[m_npar];
    err = (ich == -1) ? new double[m_nindices] : new double[m_npar];

    int ngoodentries = this->nindices();
    cout << "ngoodentries=" << ngoodentries << endl;

    double* secderiv = new double[ngoodentries * ngoodentries];
    double* firstderiv = new double[ngoodentries];
    for (int i = 0; i < ngoodentries; i++) {
        firstderiv[i] = this->firstDeriv(i);
        // if (ngoodentries<10)
        cout << "firstderiv[" << i << "]=" << firstderiv[i] << endl;
        for (int j = i; j < ngoodentries; j++) {
            int index = i * ngoodentries + j;
            double sd = this->secondDeriv(i, j);
            secderiv[index] = sd;
            index = j * ngoodentries + i;
            secderiv[index] = sd;
            // if (ngoodentries<10)
            cout << "secderiv[" << i << "][" << j << "]=" << sd << endl;
        }
    }

    if (solveType == CLHEP) {
        if (ngoodentries < 10) cout << "solving CLHEP" << endl;
        HepSymMatrix d2Chi2(ngoodentries, 0);
        HepVector dChi2(ngoodentries, 0);
        for (int i = 0; i < ngoodentries; i++) {
            dChi2[i] = firstderiv[i];
            for (int j = 0; j < ngoodentries; j++) {
                int index = i * ngoodentries + j;
                d2Chi2[i][j] = secderiv[index];
            }
        }

        if (ngoodentries < 10) {
            cout << "d2Chi2: " << d2Chi2 << endl;
            cout << "dChi2: " << dChi2 << endl;
        }

        HepSymMatrix cov(d2Chi2);
        int ierr = 0;
        cov.invert(ierr);

        if (ngoodentries < 10) cout << "cov: " << cov << endl;

        HepVector delta = cov * dChi2;
        for (int i = 0; i < ngoodentries; i++) {
            res[i] = -delta[i];
            err[i] = std::sqrt(2. * std::fabs(cov(i + 1, i + 1)));
            cout << "i: " << i << ", res/err: " << res[i] << "/" << err[i] << endl;
        }
    } else if (solveType == ROOT) {
        TMatrixDSym a(ngoodentries, secderiv);
        TVectorD b(ngoodentries, firstderiv);

        if (0) {
            cout << "First derivatives: " << endl;
            b.Print();

            cout << "Second derivatives: " << endl;
            a.Print();
        }

        TDecompBK cm(a);
        Bool_t status;
        TMatrixDSym ainv(cm.Invert(status));
        TVectorD r(b.GetNrows());
        if (status) r = cm.Solve(b, status);

        for (int i = 0; i < ngoodentries; i++) {
            res[i] = -r[i];
            err[i] = std::sqrt(2. * std::fabs(ainv(i, i)));
        }
    } else {
        delete[] res;
        res = 0;
    }

    return res;
}

//________________________________________________________________________
int ChamberDerivs::getChIndex(int id) const {
    for (int ich = 0; ich < m_nchambers; ich++) {
        if (m_chamberIds[ich] == id) return ich;
    }
    return -1;
}

//________________________________________________________________________
void ChamberDerivs::addConstraint(int ich, int ipar, double constraint) {
    cout << "before constraint: " << endl;
    this->PrintChambers(ich);
    this->PrintSecondDerivs(ich);
    this->addSecondDeriv(ich, ipar, ich, ipar, constraint);

    cout << endl << "after constraint: " << endl;
    this->PrintChambers(ich);
    this->PrintSecondDerivs(ich);
}

//________________________________________________________________________
void ChamberDerivs::addFirstDerivConstraint(int ich, int ipar, double constraint) { this->addFirstDeriv(ich, ipar, constraint); }

/*
//________________________________________________________________________
ChamberDerivs ChamberDerivs::addParameter(int parameterToAdd, int totalNPar,
                                          double* valueToAdd)
{
  int mapNewToOldPar[totalNPar];

  int oldpar(0);
  for (int ipar=0;ipar<totalNPar;ipar++) {
    if (ipar==parameterToAdd) mapNewToOldPar[ipar]=-1;
    else {
      mapNewToOldPar[ipar]=oldpar;
      oldpar++;
    }
    cout<<"mapNewToOldPar["<<ipar<<"]="<<mapNewToOldPar[ipar]<<endl;
  }

  ChamberDerivs newChDerivs(totalNPar, this->nchambers());
  for (int ich=0;ich<this->nchambers();ich++) {

    newChDerivs.setChamberId(ich,this->chamberId(ich));

    for (int ipar=0;ipar<totalNPar;ipar++) {
      int old_ipar=mapNewToOldPar[ipar];

      double oldFirstDeriv=this->firstDeriv(ich,old_ipar);

      newChDerivs.setFirstDeriv(ich,ipar,oldFirstDeriv);
      newChDerivs.setMatrixIndex(ich,ipar,this->matrixIndex(ich,old_ipar));

      for (int jch=ich;jch<this->nchambers();jch++) {

        if (ipar==0) newChDerivs.setNHits(ich,jch,this->nhits(ich,jch));

        for (int jpar=0;jpar<totalNPar;jpar++) {
          int old_jpar=mapNewToOldPar[jpar];

          double secndDeriv=this->secndDeriv(ich,old_ipar,jch,old_jpar);
          if (ipar==parameterToAdd) secndDeriv+=valueToAdd[jpar];
          if (jpar==parameterToAdd && ipar!=jpar) secndDeriv+=valueToAdd[ipar];
          newChDerivs.setSecndDeriv(ich,ipar,jch,jpar,secndDeriv);
        }
      }
    }
  }
  return newChDerivs;
}
*/

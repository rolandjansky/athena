/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaAnalysisUtils/FsrPhotons.h"
#include <algorithm>
#include <cmath>


using namespace std;

FsrPhotons::FsrPhotons() {

    //DO NOT CHANGE: cuts to be used for FSR photon selection
    m_HIGH_ET_MIN   = 3500.;   // to select egamma Photon with Et>3.5GeV
    m_OVERLAP_EL_PH = 0.01;  // overlap in dR
    m_OVERLAP_EL_MU = 0.001; // overlap in theta and phi

}

FsrPhotons::~FsrPhotons() { }


bool
FsrPhotons::getFsrPhotons(float mueta, 
                          float muphi,  
                          float muidtheta, 
                          float muidphi, 
                          std::vector<float>* pheta, 
                          std::vector<float>* phphi, 
                          std::vector<float>* phEt, 
                          std::vector<float>* phf1, 
                          std::vector<float>* ph_topoEtcone40,  
                          std::vector<int>*   phauthor, 
                          std::vector<float>* eleta, 
                          std::vector<float>* elphi, 
                          std::vector<float>* elEt, 
                          std::vector<float>* elf1,  
                          std::vector<float>* eltracktheta, 
                          std::vector<float>* eltrackphi, 
                          FsrCandidate& candidate,
                          std::vector<int>*   phtight)
{

    // Look for both "near" and "far" FSR photons

    std::vector<FsrPhotons::FsrCandidate>* cands = getFsrPhotons(mueta, 
                                                                 muphi, 
                                                                 muidtheta, 
                                                                 muidphi, 
                                                                 pheta, 
                                                                 phphi, 
                                                                 phEt, 
                                                                 phf1, 
                                                                 ph_topoEtcone40,  
                                                                 phauthor, 
                                                                 eleta, 
                                                                 elphi, 
                                                                 elEt, 
                                                                 elf1, 
                                                                 eltracktheta, 
                                                                 eltrackphi,
                                                                 phtight);
    if (cands->size() > 0) {
        candidate = cands->at(0);
        return true;
    }
    candidate = FsrCandidate();
    return false;
}



std::vector<FsrPhotons::FsrCandidate>* 
FsrPhotons::getFsrPhotons(float mueta, 
                          float muphi, 
                          float muidtheta, 
                          float muidphi, 
                          std::vector<float>* pheta, 
                          std::vector<float>* phphi, 
                          std::vector<float>* phEt, 
                          std::vector<float>* phf1, 
                          std::vector<float>* ph_topoEtcone40,  
                          std::vector<int>*   phauthor, 
                          std::vector<float>* eleta, 
                          std::vector<float>* elphi, 
                          std::vector<float>* elEt, 
                          std::vector<float>* elf1, 
                          std::vector<float>* eltracktheta, 
                          std::vector<float>* eltrackphi,
                          std::vector<int>* phtight,
                          double DR_CUT, 
                          double DR_TOPO_CUT, 
                          double ET_CUT, 
                          double F1_TOPO_CUT, 
                          double F1_CUT) {

    // Look for both "near" and "far" FSR photons

    // NEAR FSR:

    // pair.first : ph index; pair.second: dR
    std::vector< std::pair <unsigned int,double> > phfsr;
    phfsr.reserve(5);
    m_lookingForFarFsr = false;

    // Loop over photon candidates and choose the FSR photons
    for (unsigned int pindx=0; pindx < (unsigned int) pheta->size(); pindx++ ) {
        if( ( ((phauthor->at(pindx)==4) || (phauthor->at(pindx)==16)) && 
              (phEt->at(pindx) > m_HIGH_ET_MIN) && (phEt->at(pindx) > ET_CUT) && (phf1->at(pindx) > F1_CUT) ) || 

            ((phauthor->at(pindx)==128) && (phf1->at(pindx) > F1_TOPO_CUT) && 
             ((ET_CUT < phEt->at(pindx)) && (phEt->at(pindx) <= m_HIGH_ET_MIN)))  ) {
            double dr = deltaR(mueta, muphi, pheta->at(pindx), phphi->at(pindx));
            // ph_cl_eta/phi should be used in duplicate
            if (dr < DR_TOPO_CUT || (phauthor->at(pindx)!=128 && dr<DR_CUT)) {
                phfsr.push_back(make_pair(pindx, dr));
            }
        }  
    }

    // look for fsr photons in electron container
    std::vector< std::pair <unsigned int,double> >  elfsr;
    elfsr.reserve(5);
    for (unsigned int eindx = 0; eindx < (unsigned int) eleta->size(); eindx++ ) {
        // remove overlap with fsr already found
        if ((phfsr.size() > 0) && isOverlap(eleta->at(eindx),      
                                            elphi->at(eindx), 
                                            &phfsr, 
                                            pheta, 
                                            phphi )) continue;
        bool elmutrackmatch = (fabs(eltracktheta->at(eindx)-muidtheta) < m_OVERLAP_EL_MU && 
                               fabs(eltrackphi->at(eindx)-muidphi)     < m_OVERLAP_EL_MU);
        if( (elmutrackmatch) && (elEt->at(eindx) > ET_CUT) && (elEt->at(eindx) > m_HIGH_ET_MIN) && 
            (elf1->at(eindx) > F1_CUT)) {
            double dr = deltaR(mueta, muphi, eleta->at(eindx), elphi->at(eindx));
            // ph_cl_eta/phi should be used in duplicate
            if (dr < DR_CUT) {
                elfsr.push_back(make_pair(eindx, dr));
            }
        }
    }
  

    // FAR FSR:

    if (phfsr.empty() && elfsr.empty() && phtight != NULL ) {
        // Only look for far FSR if near FSR has not been found
        getFarFsrCandidates(mueta, muphi, pheta,  phphi, phEt, ph_topoEtcone40, phauthor, 
                            phtight, phfsr);
    }


    return sortFsrCandidates(phfsr, pheta,  phphi, phEt, phf1, ph_topoEtcone40,
                             elfsr, eleta, elphi,  elEt,  elf1 );

}


bool
FsrPhotons::getFsrPhotons(float eltrketa,
                          float eltrkphi,
                          std::vector<float>* pheta,
                          std::vector<float>* phphi,
                          std::vector<float>* phEt,
                          std::vector<float>* phf1,
                          std::vector<float>* ph_topoEtcone40,  
                          std::vector<int>*   phauthor,
                          std::vector<int>*   phtight,
                          FsrCandidate& candidate)
{

    // Look only for "near" FSR
    m_lookingForFarFsr = false;

    std::vector<FsrPhotons::FsrCandidate>* cands = getFsrPhotons(eltrketa,
                                                                 eltrkphi,
                                                                 pheta,
                                                                 phphi,
                                                                 phEt,
                                                                 phf1,
                                                                 ph_topoEtcone40,  
                                                                 phauthor,
                                                                 phtight);

    if (cands->size() > 0) {
        candidate = cands->at(0);
        return true;
    }
    candidate = FsrCandidate();
    return false;
}


std::vector<FsrPhotons::FsrCandidate>*
FsrPhotons::getFsrPhotons(float eltrketa,
                          float eltrkphi,
                          std::vector<float>* pheta,
                          std::vector<float>* phphi,
                          std::vector<float>* phEt,
                          std::vector<float>* phf1,
                          std::vector<float>* ph_topoEtcone40,  
                          std::vector<int>* phauthor,
                          std::vector<int>* phtight)
{

    // Look ONLY for "far" FSR

    // pair.first : ph index; pair.second: dR
    std::vector< std::pair <unsigned int,double> > fsrlist;
    fsrlist.reserve(5);
    // for far fsr we have only candidates from photons so electron list is empty
    std::vector< std::pair <unsigned int,double> > nulllist;
    nulllist.clear();

    getFarFsrCandidates(eltrketa, eltrkphi, pheta,  phphi, phEt, ph_topoEtcone40, phauthor, 
                        phtight, fsrlist);

    return sortFsrCandidates(fsrlist, pheta,  phphi, phEt, phf1, ph_topoEtcone40, nulllist);

}


bool
FsrPhotons::getFarFsrCandidates(float               lepeta, 
                                float               lepphi,
                                std::vector<float>* pheta, 
                                std::vector<float>* phphi,
                                std::vector<float>* phEt,  
                                std::vector<float>* ph_topoEtcone40,  
                                std::vector<int>*   phauthor,
                                std::vector<int>*   phtight,
                                std::vector< std::pair <unsigned int,double> >& farFsrCandList,
                                double              FAR_FSR_DR_CUT,
                                double              FAR_FSR_ET_CUT,
                                double              FAR_FSR_ETCONE_CUT)
{

    // Set flag to differentiate between near and far FSR
    m_lookingForFarFsr = true;

    farFsrCandList.clear();
    farFsrCandList.reserve(pheta->size());

    for (unsigned int pindx = 0; pindx < (unsigned int) pheta->size(); pindx++ ) {
        if( (phauthor->at(pindx) == 4 || phauthor->at(pindx) == 16)
            && (phEt->at(pindx) > FAR_FSR_ET_CUT) && (phtight->at(pindx) == 1) 
            && (ph_topoEtcone40->at(pindx) < FAR_FSR_ETCONE_CUT) ) {
                         
            double dr = deltaR(lepeta, lepphi, pheta->at(pindx), phphi->at(pindx));

            if(dr > FAR_FSR_DR_CUT) farFsrCandList.push_back(make_pair(pindx, dr));
        }
    }

    return farFsrCandList.size();

}

std::vector<FsrPhotons::FsrCandidate>* 
FsrPhotons::sortFsrCandidates(std::vector< std::pair <unsigned int,double> > phfsr,
                              std::vector<float>* pheta,
                              std::vector<float>* phphi,
                              std::vector<float>* phEt,
                              std::vector<float>* phf1,
                              std::vector<float>* ph_topoEtcone40,  
                              std::vector< std::pair <unsigned int,double> > elfsr,
                              std::vector<float>* eleta,
                              std::vector<float>* elphi,
                              std::vector<float>* elEt,
                              std::vector<float>* elf1,
                              std::string option)
{

    m_fsrPhotons.clear();
    m_fsrPhotons.reserve(phfsr.size() + elfsr.size());
    for (unsigned int i=0; i < phfsr.size(); i++ ) {
        FsrCandidate c;
        c.container    = "photon";
        c.index        = phfsr.at(i).first;
        c.deltaR       = phfsr.at(i).second;
        c.f1           = phf1->at(c.index);
        c.eta          = pheta->at(c.index);
        c.phi          = phphi->at(c.index);
        c.Et           = phEt->at(c.index);
        c.topoEtcone40 = ph_topoEtcone40->at(c.index);
        c.type         = (m_lookingForFarFsr) ? FsrCandidate::FsrFar : FsrCandidate::FsrNear;
        if(c.deltaR <0.05) c.Et = phEt->at(c.index)-(400/cosh(pheta->at(c.index)));
        m_fsrPhotons.push_back(c);
    }
    for (unsigned int i=0; i < elfsr.size(); i++ ) {
        FsrCandidate c;
        c.container = "electron";
        c.index     = elfsr.at(i).first;
        c.deltaR    = elfsr.at(i).second;
        c.f1        = elf1->at(c.index);
        c.eta       = eleta->at(c.index);
        c.phi       = elphi->at(c.index);
        c.Et        = elEt->at(c.index);
        c.type      = (m_lookingForFarFsr) ? FsrCandidate::FsrFar : FsrCandidate::FsrNear;
        if(c.deltaR <0.05) c.Et = elEt->at(c.index)-(400/cosh(eleta->at(c.index)));
        m_fsrPhotons.push_back(c);
    }
    // sort FSR candidates w.r.t min dR or ET
    if (option=="ET") std::sort(m_fsrPhotons.begin(), m_fsrPhotons.end(), compareEt);
    else std::sort(m_fsrPhotons.begin(), m_fsrPhotons.end());
    return &m_fsrPhotons;
}

bool FsrPhotons::isOverlap(float eleta, 
                           float elphi, 
                           std::vector< std::pair <unsigned int, double> >* phfsr, 
                           std::vector<float>* pheta, 
                           std::vector<float>* phphi) {

    for (unsigned int indx=0; indx < phfsr->size(); indx++ ) {
        int phfsrindex = phfsr->at(indx).first;
        double dr = deltaR(eleta, elphi, pheta->at(phfsrindex), phphi->at(phfsrindex));
        if (dr < m_OVERLAP_EL_PH) return true;
    }
    return false;
}


double FsrPhotons::deltaR(float meta, float mphi, float peta, float pphi) {

    double deta= fabs(meta - peta);
    double dphi= fabs(mphi - pphi);
    if (dphi > M_PI) dphi=2*M_PI-dphi;
    double dR=sqrt((dphi*dphi)+(deta*deta));
 
    return dR;
}

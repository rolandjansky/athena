/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
#include "Prophecy4fMerger.h"
#include "TLorentzRotation.h"
#include "TLorentzVector.h"
#include "TVector3.h"
/* Utils */
#include "LHEF.h"
#include <string>
#include <iostream>
#include <AsgMessaging/MessageCheck.h>

using namespace asg::msgUserCode;

Prophecy4fMerger::Prophecy4fMerger() : m_phEvent(false), m_debug(false), m_rand(0) { 
}

Prophecy4fMerger::~Prophecy4fMerger(){
}

void Prophecy4fMerger::setRandomSeed(unsigned long long seed)
{
    m_rand.SetSeed(seed);
    ANA_MSG_INFO("Prophecy4fMerger::setRandomSeed: set random seed to " << seed << ".");
}

int Prophecy4fMerger::alulb4(double *ps, double *pi, double *pf){

    // transform pi rest to lab frame of ps - output pf
    // For description, see LOREN4 doc in CERNLIB

    static double fn, pf4;
  
    if(ps[3] == ps[4]){
        pf[0] = pi[0];
        pf[1] = pi[1];
        pf[2] = pi[2];
        pf[3] = pi[3];
    }
    else{
        pf4 = (pi[0] * ps[0] + pi[1] * ps[1] + pi[2] * ps[2] + pi[3] * ps[3]) / ps[4];
        fn = (pf4 + pi[3]) / (ps[3] + ps[4]);
        pf[0] = pi[0] + fn * ps[0];
        pf[1] = pi[1] + fn * ps[1];
        pf[2] = pi[2] + fn * ps[2];
        pf[3] = pf4;
    }
    return 0;
  
}

int Prophecy4fMerger::alulf4(double *ps, double *pi, double *pf){

    // transform pi lab to rest frame of ps - output pf
    // For description, see LOREN4 doc in CERNLIB
  
    static double fn, pf4;
  
    if(ps[3] == ps[4]){
        pf[0] = pi[0];
        pf[1] = pi[1];
        pf[2] = pi[2];
        pf[3] = pi[3];
    }
    else{
        pf4 = (pi[3] * ps[3] - pi[2] * ps[2] - pi[1] * ps[1] - pi[0] * ps[0]) / ps[4]; 
        fn = (pf4 + pi[3]) / (ps[3] + ps[4]); 
        pf[0] = pi[0] - fn * ps[0]; 
        pf[1] = pi[1] - fn * ps[1]; 
        pf[2] = pi[2] - fn * ps[2]; 
        pf[3] = pf4;
    }
    return 0;
  
}

int Prophecy4fMerger::alulob(double *ps, double *pi, double *pf){
  
    //extern int alulb4(double *, double *, double *);
  
    alulb4(ps, pi, pf);
    pf[4] = pi[4];
    return 0;
  
} 

int Prophecy4fMerger::alulof(double *ps, double *pi, double *pf){
  
    //extern int alulf4(double *, double *,  double *);
  
    alulf4(ps, pi, pf);
    pf[4] = pi[4];
  
    return 0;
  
} 

double Prophecy4fMerger::alupcm(double em0, double em1, double em2){

    // calculate the momentum of particles 1 and 2 in the rest frame of 0 given the three masses

    double ret_val;
    static double emd, ems;
  
    //ems = abs(em1 + em2);
    //emd = abs(em1 - em2);
    ems = (em1 + em2);
    if(ems < 0){
        ems=-(em1 + em2);
    }
    emd = (em1 - em2);
    if(emd < 0){
        emd = -(em1 - em2);
    }
  
    if(em0 < ems || em0 < emd){
        ret_val = -1.f;
    }
    else if(em0 == ems || em0 == emd){
        ret_val = 0.f;
    }
    else{
        ret_val = sqrt((em0 + emd) * (em0 - emd) * (em0 + ems) * (em0 - ems)) * .5f / em0;
    }
    return ret_val;
  
}

int Prophecy4fMerger::rescms(double *p, double *p1, double *p2, double m1, double m2){

    // p1 and p2 are the four-vector decay products of p. Here we change the masses of p1 and p2 to be
    // m1 and m2. Note that for Prophecy, the masses in p1 and p2 are zero.
  
    static double m;
    static int il;
    static double mo1, mo2, po1[5], po2[5], pcm, pcmo;
      
    m = p[4];
    mo1 = p1[4];
    mo2 = p2[4];
  
    // transform p1 to po1 to be in rest frame of p, sampe for p2
    alulof(p, p1, po1);
    alulof(p, p2, po2);
  
    // Calculate the momentum of p1/p2 in the rest frame of p (po1/po2) for the original mass and new
    // masses, and then rescale the rest frame momenta po1 and po2. (Note: eo1 = eo2 in cms of p, so
    // po1 and po2 in cms of p change according to e^2 - m^2 as m changes.)
    if (std::max(mo1,mo2) < 1e-6){
        pcmo = m / 2.;
    }
    else{
        pcmo = alupcm(m, mo1, mo2);
    }
    pcm = alupcm(m, m1, m2);
  
    //rescale the cms momenta, po1 and po2, to account for the new masses used 
    for (il = 0; il < 4; il++) {
        po1[il] = pcm / pcmo * po1[il];
        po2[il] = pcm / pcmo * po2[il];
    }
  
    //set energy and mass
    po1[3] = sqrt(pcm*pcm + m1*m1);
    po2[3] = sqrt(pcm*pcm + m2*m2);
    po1[4] = m1;
    po2[4] = m2;
  
    //boost back
    alulob(p, po1, p1);
    alulob(p, po2, p2);	
  
    return 0;
  
}

void Prophecy4fMerger::setIO(const std::string& powheg,
                             const std::string& prophecy4e,
                             const std::string& prophecy4mu,
                             const std::string& prophecy2e2mu,
                             const std::string& outlhe,
                             bool debug) 
{
    m_inPowheg        = powheg;
    m_inProphecy4e    = prophecy4e;
    m_inProphecy4mu   = prophecy4mu;
    m_inProphecy2e2mu = prophecy2e2mu;
    m_outLHE          = outlhe;
    m_phEvent         = false;
    m_debug           = debug;
  
}

void Prophecy4fMerger::merge(){

    print(" Opening Powheg LHE file ... " + m_inPowheg);
    if( !fileExists(m_inPowheg) ){
        ANA_MSG_ERROR("Input Powheg LHE not Found!"
                  << " Aborting ... ");
        return;
    }
    LHEF::Reader readH( m_inPowheg.c_str() );
  
    print(" Opening Prophecy4f LHE file for 4e ... " + m_inProphecy4e);
    if( !fileExists(m_inProphecy4e) ){
        ANA_MSG_INFO( "Input Prophecy4f for 4e LHE not Found! " << m_inProphecy4e
                  << " Aborting ... " );
        return;
    }
    LHEF::Reader read4e( m_inProphecy4e.c_str() );
  
    print(" Opening Prophecy4f LHE file for 4mu ... " + m_inProphecy4mu);
    if( !fileExists(m_inProphecy4mu) ){
        ANA_MSG_ERROR("Input Prophecy4f for 4mu LHE not Found! " << m_inProphecy4mu
                  << " Aborting ... ");
        return;
    }
    LHEF::Reader read4mu( m_inProphecy4mu.c_str() );
  
    print(" Opening Prophecy4f for 2e2mu LHE file ... " + m_inProphecy2e2mu);
    if( !fileExists(m_inProphecy2e2mu) ){
        ANA_MSG_ERROR("Input Prophecy4f for 2e2mu LHE not Found! " << m_inProphecy2e2mu
                  << " Aborting ... ");
        return;
    }
    LHEF::Reader read2e2mu( m_inProphecy2e2mu.c_str() );
  
    print(" Opening Out LHE file ... " + m_outLHE);
    LHEF::Writer writeLHE( m_outLHE.c_str() );
  
    writeLHE.headerBlock() << readH.headerBlock;
    writeLHE.initComments() << readH.initComments;
    writeLHE.heprup = readH.heprup;
    writeLHE.init();

    long neve = 0;
    LHEF::Reader* read4f = 0;
  
  
    // Read in each Higgs event and one Prophecy4f event, merge them and write out the merged event
    // randomly chose between 4mu, 4e, 2mu2e and 2e2mu. Note that 2mu2e is always taken as 2e2mu,
    // since it is generated this way.
  
    while( readH.readEvent() ){
        // randomly choose between 4mu, 4e, 2mu2e and 2e2mu
        int ID_l[4] = {-999, -999, -999, -999};
        int decayModeType = 4.0 * m_rand.Rndm();   
	 
        if (decayModeType == 0){
            //4e
                        read4f = &read4e;
            for(int i = 0; i < 4; i++) ID_l[i] = m_electronID;
        }
        else if (decayModeType == 1 || decayModeType == 2){
            //2e2mu
                        read4f = &read2e2mu;
            for(int i = 0; i < 2; i++) ID_l[i] = m_electronID;
            for(int i = 2; i < 4; i++) ID_l[i] = m_muonID;
        }
        else {
            //4mu
                        read4f = &read4mu;
            for(int i = 0; i < 4; i++) ID_l[i] = m_muonID;
        }
    
        if( !read4f->readEvent() ){
            ANA_MSG_INFO("Still Powheg events but no more Prophecy4f ");
            ANA_MSG_INFO("events! Writing out LHE file ... Events processed so far " << neve );
            break;
        }
    
        ++neve;
        if(neve%1000==0){
            print("Events processed so far ", neve);
        }
        if( readH.outsideBlock.length() )
            ANA_MSG_INFO( readH.outsideBlock );
    
        int nup_org=readH.hepeup.NUP;

        for(int nup=0; nup<nup_org; nup++){
            if(readH.hepeup.idup(nup)==m_higgsID)
                readH.hepeup.istup(nup)=2; // set Higgs status code to 2
        }
        writeLHE.hepeup = readH.hepeup;

        double P4_Z[5][2] = {{0}}; // momentum and mass of Zi i=1,2 set to 0 P4_Z[5] = (px, py, pz, E, mZ)
        double P4_l[5][5];
    
        double Pph[5] = {-999., -999., -999., -999., -999.};
        double daughter2[5][5];
    
        for(int i=0; i<nup_org; i++){
            if(readH.hepeup.idup(i)==m_higgsID) {
                TLorentzVector higgs( readH.hepeup.pup(i,0),
                                      readH.hepeup.pup(i,1),
                                      readH.hepeup.pup(i,2),
                                      readH.hepeup.pup(i,3) );
	
                TVector3 boostvec = higgs.BoostVector();
                TLorentzRotation boost(-boostvec);
                TLorentzRotation backboost(boostvec);
	
                /* boost to Higgs rest frame */
                TLorentzVector r_higgs(higgs);
                r_higgs*=boost;
	
                /* Here add daughter particles to Higgs in rest
                   boost back with backboost */
                writeLHE.hepeup.resize(nup_org+read4f->hepeup.NUP);
	
                TLorentzVector sum_daughter;
                TLorentzVector sum_daughter_rest;
                TLorentzVector sum_daughter_rest_init;
	
                for(int k=0; k<read4f->hepeup.NUP; k++){
                    TLorentzVector daughter(read4f->hepeup.pup(k,0),
                                            read4f->hepeup.pup(k,1),
                                            read4f->hepeup.pup(k,2),
                                            read4f->hepeup.pup(k,3));
                    sum_daughter_rest_init+=daughter;
                }
	
                for(int j=0; j<read4f->hepeup.NUP; j++){
                    TLorentzVector daughter(read4f->hepeup.pup(j,0),
                                            read4f->hepeup.pup(j,1),
                                            read4f->hepeup.pup(j,2),
                                            read4f->hepeup.pup(j,3));
                    sum_daughter_rest+=daughter;
	  
                    daughter *= backboost;
                    daughter *= higgs.M()/sum_daughter_rest_init.M();
	  
                    /* POWHEG event with Higgs off-mass shell */
                    m_phEvent=isPHevent(higgs,sum_daughter_rest_init);
                    if( m_phEvent ){
                        break;
                    }
	  
                    sum_daughter+=daughter;
	  
                    if( std::abs(read4f->hepeup.idup(j))==m_electronID ||
                        std::abs(read4f->hepeup.idup(j))==m_muonID      ){
	    
                        //ID_l[j]    = read4f->hepeup.idup(j);
                        P4_l[0][j] = daughter.Px();
                        P4_l[1][j] = daughter.Py();
                        P4_l[2][j] = daughter.Pz();
                        P4_l[3][j] = daughter.E();
	    
                    }		
                    else if( std::abs(read4f->hepeup.idup(j))==m_photonID ){
                        Pph[0] = daughter.Px();
                        Pph[1] = daughter.Py();
                        Pph[2] = daughter.Pz();
                        Pph[3] = daughter.E();
                        Pph[4] =0.0;
                    }
                }			
	
                for(int n=0; n<4; n++){
                    if( std::abs(ID_l[n])==m_electronID ||
                        std::abs(ID_l[n])==m_muonID  ) {
	    
                        // Z1 -> l1 + l2 and Z2 -> l3 + l4
                        P4_Z[n][0] = P4_l[n][0] + P4_l[n][1];
                        P4_Z[n][1] = P4_l[n][2] + P4_l[n][3];
                    }
                }
	
                double mass1, mass2;
                P4_Z[4][0] = sqrt(P4_Z[3][0] * P4_Z[3][0] - P4_Z[0][0] * P4_Z[0][0] - P4_Z[1][0] * P4_Z[1][0] - P4_Z[2][0] * P4_Z[2][0]);
                P4_Z[4][1] = sqrt(P4_Z[3][1] * P4_Z[3][1] - P4_Z[0][1] * P4_Z[0][1] - P4_Z[1][1] * P4_Z[1][1] - P4_Z[2][1] * P4_Z[2][1]);
                mass1 = setParticleMass(ID_l[0]);
                mass2 = setParticleMass(ID_l[2]);
	
                double p1[5], p2[5], pZ1[5], p3[5], p4[5], pZ2[5];
                for(int u=0; u<5; u++){
	  
                    pZ1[u] = P4_Z[u][0];
                    p1[u]  = P4_l[u][0];
                    p2[u]  = P4_l[u][1];
                    pZ2[u] = P4_Z[u][1];
                    p3[u]  = P4_l[u][2];
                    p4[u]  = P4_l[u][3];
	  
                }
	
                p1[4] = 0.0;
                p2[4] = 0.0;
                p3[4] = 0.0;
                p4[4] = 0.0;
	
                // Give masses to the leptons using Alpgen/FHerwig routines
                // Note: prophecy leptons are generated massless
                rescms(pZ1, p1, p2, mass1, mass1);
                rescms(pZ2, p3, p4, mass2, mass2);

                for(int d =0; d<5; d++){
                    daughter2[0][d] = p1[d];
                    daughter2[1][d] = p2[d];
                    daughter2[2][d] = p3[d];
                    daughter2[3][d] = p4[d];
                    daughter2[4][d] = Pph[d];
                }

                // print out children
                bool doDebug = false;
                doDebug = true;
                if (doDebug) {

                    ANA_MSG_DEBUG("Event: " << neve);
                    ANA_MSG_DEBUG(" Higgs and children, m, pt, eta, phi" );
                    ANA_MSG_DEBUG("Higgs " << higgs.M() << ", " << higgs.Pt() << "," << higgs.Eta() << "," << higgs.Phi());

                    
                    TLorentzVector higgsFromChildren;
                    for(int d =0; d < read4f->hepeup.NUP; d++){
                        TLorentzVector child;
                        child.SetXYZM( daughter2[d][0],
                                       daughter2[d][1],
                                       daughter2[d][2],
                                       daughter2[d][4] );

                        ANA_MSG_DEBUG("child " << d << " " << child.M() << ", " << child.Pt() << ", " << child.Eta() << ", " << child.Phi());
                        higgsFromChildren += child;
                    }
                    ANA_MSG_DEBUG("Higgs mass " << higgs.M() << ", from children " << higgsFromChildren.M() << ", diff " << higgs.M() - higgsFromChildren.M());
                }
                break;
            }
        }

        /* Write LHE */
        if(m_phEvent){
            writeLHE.hepeup = readH.hepeup;
        }
        else{
      
            for(int jp=0; jp<read4f->hepeup.NUP; jp++){
                if (jp < 4){
                    writeLHE.hepeup.idup(nup_org+jp) = (jp % 2 == 0)?  ID_l[jp] : -ID_l[jp];

                }
                else{
                    writeLHE.hepeup.idup(nup_org+jp) = read4f->hepeup.idup(jp);		  
                }  
        
                writeLHE.hepeup.istup(nup_org+jp)=read4f->hepeup.istup(jp);
                if( read4f->hepeup.icolup1(jp)==0 &&
                    read4f->hepeup.icolup2(jp)==0  ) {
                    writeLHE.hepeup.icolup1(nup_org+jp)=0;
                    writeLHE.hepeup.icolup2(nup_org+jp)=0;
                }
                else {
                    writeLHE.hepeup.icolup1(nup_org+jp)=read4f->hepeup.icolup1(jp)+100;
                    writeLHE.hepeup.icolup2(nup_org+jp)=read4f->hepeup.icolup2(jp)+100;
                }  
                writeLHE.hepeup.mothup1(nup_org+jp)=3;//i-1;
                writeLHE.hepeup.mothup2(nup_org+jp)=3;//i-1;
                writeLHE.hepeup.pup(nup_org+jp,0)=daughter2[jp][0];
                writeLHE.hepeup.pup(nup_org+jp,1)=daughter2[jp][1];
                writeLHE.hepeup.pup(nup_org+jp,2)=daughter2[jp][2];
                writeLHE.hepeup.pup(nup_org+jp,3)=daughter2[jp][3];
                writeLHE.hepeup.pup(nup_org+jp,4)=daughter2[jp][4];
                writeLHE.hepeup.vtimup(nup_org+jp)=read4f->hepeup.vtimup(jp);
                writeLHE.hepeup.spinup(nup_org+jp)=read4f->hepeup.spinup(jp);
            }
        }

        // set powheg weights to negative, if prophecy weight is -1

        // Get Prophecy4f weight
            writeLHE.hepeup.XWGTUP *= read4f->hepeup.XWGTUP;
            double prophecyWeight = read4f->hepeup.XWGTUP;
        if ( writeLHE.hepeup.weights.size() > 0 ) {
            for ( int i = 1, N = writeLHE.hepeup.weights.size(); i < N; ++i ) {
                writeLHE.hepeup.weights[i].first *= prophecyWeight;
    
            }
        }
        
        writeLHE.eventComments() << readH.eventComments;
        writeLHE.writeEvent();
    }
}

bool Prophecy4fMerger::isPHevent(TLorentzVector higgs,
                                 TLorentzVector sum_daugh_rest_init){

    if( std::abs(higgs.M()-sum_daugh_rest_init.M())>m_deltaM ){
        ANA_MSG_INFO("POWHEG event with Higgs off-mass shell: " << higgs.M() << " GeV");
        return true;
    }
    return false;
  
}


double Prophecy4fMerger::setParticleMass(int id) const {
  
    double mass = 0.0;

    if( std::abs(id) == m_neutrinoEl ||
        std::abs(id) == m_neutrinoMu ||
        std::abs(id) == m_neutrinoTau ){
        mass = 0.0;
    }
    else if( std::abs(id) == m_electronID ){
        mass = m_electronMass;
    }
    else if( std::abs(id) == m_muonID ){
        mass = m_muonMass;
    }
    else if( std::abs(id) == m_tauID ){
        mass = m_tauMass;
    }
  
    return mass;
  
}

bool Prophecy4fMerger::fileExists(const std::string& filename){
  
    std::ifstream ifile(filename.c_str());
    if( ifile.good() ){
        ifile.close();
        return true;
    }
    ifile.close();
    return false;

}

void Prophecy4fMerger::print(const std::string& field){
    if(m_debug){
        ANA_MSG_DEBUG(field);
    }
}

void Prophecy4fMerger::print(const std::string& field,
                             int value){

    if(m_debug){
        ANA_MSG_DEBUG("DEBUG:: " << value << " " << field);
    }
  
}


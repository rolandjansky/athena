/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MC2HEP_H
#define MC2HEP_H

/*
 * Access to Epos Fortran variables by C structures
 * A hack from crmchep.h
 *
 * Marco Leite - Dec 2012
 * leite@cernch
 */
extern "C"
{
  extern struct
  {
    float sigtot;
    float sigcut;
    float sigela;
    float sloela;
    float sigsd;
    float sigine;
    float sigdif;
    float sigineaa;
    float sigtotaa;
    float sigelaaa;
    float sigcutaa;
  } hadr5_; //crmc-aaa.f
}

extern "C"
{
  extern struct
  {
    // nevt .......... error code. 1=valid event, 0=invalid event
    // bimevt ........ absolute value of impact parameter
    // phievt ........ angle of impact parameter
    // kolevt ........ number of collisions
    // koievt ........ number of inelastic collisions
    // pmxevt ........ reference momentum
    // egyevt ........ pp cm energy (hadron) or string energy (lepton)
    // npjevt ........ number of primary projectile participants
    // ntgevt ........ number of primary target participants
    // npnevt ........ number of primary projectile neutron spectators
    // nppevt ........ number of primary projectile proton spectators
    // ntnevt ........ number of primary target neutron spectators
    // ntpevt ........ number of primary target proton spectators
    // jpnevt ........ number of absolute projectile neutron spectators
    // jppevt ........ number of absolute projectile proton spectators
    // jtnevt ........ number of absolute target neutron spectators
    // jtpevt ........ number of absolute target proton spectators
    // xbjevt ........ bjorken x for dis
    // qsqevt ........ q**2 for dis
    // sigtot ........ total cross section
    // nglevt ........ number of collisions acc to  Glauber
    // zppevt ........ average Z-parton-proj
    // zptevt ........ average Z-parton-targ
    // ng1evt ........ number of Glauber participants with at least one IAs
    // ng2evt ........ number of Glauber participants with at least two IAs
    // ikoevt ........ number of elementary parton-parton scatterings
    // typevt ........ type of event (1=Non Diff, 2=Double Diff, 3=Single Diff
    float phievt;
    int nevt;
    float bimevt;
    int kolevt;
    int koievt;
    float pmxevt;
    float egyevt;
    int npjevt;
    int ntgevt;
    int npnevt;
    int nppevt;
    int ntnevt;
    int ntpevt;
    int jpnevt;
    int jppevt;
    int jtnevt;
    int jtpevt;
    float xbjevt;
    float qsqevt;
    int nglevt;
    float zppevt;
    float zptevt;
    int minfra;
    int maxfra;
    int kohevt;
  } cevt_; //epos.inc
}

extern "C"
{
  extern struct
  {
    int ng1evt;
    int ng2evt;
    float rglevt;
    float sglevt;
    float eglevt;
    float fglevt;
    int ikoevt;
    float typevt;
  } c2evt_; //epos.inc
}

#endif //ifdef MC2HEP_H

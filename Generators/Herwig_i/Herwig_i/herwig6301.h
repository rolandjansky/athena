/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __HERWIG63_HH__
#define __HERWIG63_HH__

/* Common Block Declarations */

struct Hwbeam_t {
    int ipart1, ipart2;
} ;

extern  Hwbeam_t* gHwbeam;

struct Hwbmch_t {
    char part1[8], part2[8];
} ;

extern  Hwbmch_t* gHwbmch;

struct Hwproc_t {
    double ebeam1, ebeam2, pbeam1, pbeam2;
    int iproc, maxev;
} ;
extern  Hwproc_t* gHwproc;

struct Hwpram_t {
    double afch[32], alphem, b1lim, betaf, btclm, cafac,
	   cffac, clmax, clpow, clsmr[2], cspeed, ensof, etamix, f0mix, f1mix, 
	    f2mix, gamh, gamw, gamz, gamzp, gev2nb, h1mix, pdiqk, pgsmx, 
	  pgspl[4], phimix, pifac, prsof, psplt[2], ptrms, pxrms, qcdl3, qcdl5,
	     qcdlam, qdiqk, qfch[16], qg, qspac, qv, scabi, swein, tmtop, 
	    vfch[32], vckm[9], 
	    vgcut, vqcut, vpcut, zbinm,
            effmin, omhmix, et2mix, ph3mix, gcutme; 
    int ioprem, iprint, ispac, lrsud, lwsud, modpdf[2], nbtry, ncolo, 
	    nctry, ndtry, netry, nflav, ngspl, nstru, nstry, nzbin,
            iop4jt[2], nprfmt;
    int azsoft, azspin;
    int cldir[2];
    int hardme, nospac, prndec, prvtx, softme, zprime;
    int prndef, prntex, prnweb;
} ;
extern  Hwpram_t* gHwpram ;

struct Hwprch_t {
    char autpdf[2][20], bdecay[4];
} ;

extern  Hwprch_t* gHwprch ;

struct Hwpart_t {
    int nevpar, npar, istpar[500], idpar[500], jmopar[1000],
	    jdapar[1000];
    double ppar[2500], vpar[2000];
} ;
extern  Hwpart_t* gHwpart ;

struct Hwparp_t {
    double decpar[1000], phipar[1000], rhopar[1000];
    int tmpar[500];
} ;
extern  Hwparp_t* gHwparp ;

struct Hwbosc_t {
    double alpfac, brhig[12], enhanc[12], gammax, rhohep[12000];
    int iophig, modbos[5];
} ;

extern  Hwbosc_t* gHwbosc ;

struct Hwparc_t {
    int jcopar[2000];
} ;
extern  Hwparc_t* gHwparc ;

struct Hwbrch_t {
    double anomsc[4], hardst, ptint[6], xfact;
    int inhad, jnhad, nspac[7], islent;
    int breit, frost, usecmf;
} ;

extern  Hwbrch_t* gHwbrch ;

struct Hwevnt_t {
    double avwgt, evwgt, gamwt, tlout, wbigst, wgtmax, wgtsum, wsqsum;
    int idhw[4000], ierror, istat, lwevt, maxer, maxpr;
    int nowgt;
    int nrn[2], numer, numeru, nwgts;
    int gensof;
} ;

extern  Hwevnt_t* gHwevnt ;

struct Hwhard_t {
    double asfixd, clq[42], coss, costh, ctmax, disf[26],
            emlst, emmax, emmin, empow, emsca, epoln[3],
	    gcoef[7], gpoln, omega0, phomas, ppoln[3], ptmax, ptmin, ptpow, 
      q2max, q2min, q2pow, q2wwmn, q2wwmx, qlim, sins, thmax, y4jt, tmnisr, 
	    tqwt, xx[2], xlmin, xxmin, ybmax, ybmin, yjmax, yjmin, ywwmax, 
	    ywwmin, whmin, zjmax, zmxisr;
    int iaphig, ibrn[2], ibsh, ico[10], idcmf, idn[10], iflmax, iflmin, 
	    ihpro, ipro, mapq[6], maxfl;
    int bgshat, colisr, fstevt, fstwgt, genev, hvfcen, tpol, durham;
} ;

extern  Hwhard_t* gHwhard ;

struct Hwprop_t {
    double rltim[501], rmass[501], rspin[501];
    int ichrg[501], idpdg[501], iflav[501], nres;
    int vtocdk[501], vtordk[501];
    int qorqqb[501], qborqq[501];
} ;

extern  Hwprop_t* gHwprop ;

struct Hwunam_t {
    char rname[4008];  /* 8*501 */
    char txname[37074];   /*  2*37*501  */
} ;

extern  Hwunam_t* gHwunam ;

struct Hwupdt_t{
    double brfrac[4000], cmmom[4000], dkltm[500];
    int idk[4000], idkprd[20000], lnext[4000], 
	    lstrt[500], ndkys, nme[4000], nmodes[500], nprods[4000];
    int dkpset, rstab[501];
} ;

extern  Hwupdt_t*  gHwupdt ;

struct Hwuwts_t{
    double repwt[100], sngwt, decwt, qwt[3], 
	    pwt[12], swtef[500];
} ;

extern  Hwuwts_t* gHwuwts ;

struct Hwuclu_t{
    double cldkwt[4000], cthrpw[144], preco, 
	    resn[144], rmin[144];
    int locn[144], ncldk[4000], nreco;
    int clreco;
} ;

extern  Hwuclu_t* gHwuclu ;

struct Hwdist_t {
    double exag, gev2mm, hbar, pltcut, vmin2, vtxpip[4], xmix[2], xmrct[2]
	    , ymix[2], ymrct[2];
    int iopdkl;
    int maxdkl, mixing, pipsmr;
} ;

extern  Hwdist_t* gHwdist ;

struct Hwqdks_t{
    double vtxqdk[80];
    int imqdk[20], locq[20], nqdk;
} ;

extern  Hwqdks_t* gHwqdks ;

struct Hwusud_t{
    double accur, qev[6144], sud[6144];
    int inter, nqev, nsud, sudord;
} ;

extern  Hwusud_t* gHwusud ;

// v 6.1
struct Hwsusy_t {
    double tanb, alphah, cosbpa, sinbpa, cosbma, sinbma, cosa, sina,
          cosb, sinb, cotb, zmixss[16], zmxnss[16], zsgnss[4],
          lfch[16], rfch[16], slfch[64], srfch[64], wmxuss[4],
          wmxvss[4], wsgnss[2], qmixss[24], lmixss[24],
          thetat, thetab, thetal, atss, abss, alss, muss, factss,
          ghwwss[3], ghzzss[3], ghddss[4], ghuuss[4], ghwhss[3],
          ghsqss[96], xlmnss, rmmnss, dmssm, senhnc[24], ssparity;
    int susyin;
} ;

extern  Hwsusy_t* gHwsusy ;

struct Hwrpar_t {
    double lamda1[27], lamda2[27], lamda3[27];
    int hrdcol[10];
    int rparty, colupd;
} ;

extern  Hwrpar_t* gHwrpar ;

struct Hwminb_t {
    double pmbn1, pmbn2, pmbn3, pmbk1, pmbk2, pmbm1, pmbm2,
          pmbp1, pmbp2, pmbp3;
} ;

extern  Hwminb_t* gHwminb ;

struct Hwclus_t {
    double ppcl[2500];
    int idcl[500], ncl;
} ;

extern  Hwclus_t* gHwclus ;

  // end v6.1 adds

struct Hwgrav_t {
    double grvlam, emgrv, gamgrv;
} ;

extern  Hwgrav_t* gHwgrav ;

struct Hw6202_t {
    double vipwid[3], dxrcyl, dxzmax, dxrsph;
    int wzrfr, fix4jt;
    int imssm, ihiggs, parity, lrsusy;
} ;

extern  Hw6202_t* gHw6202 ;

//end v 6202 adds

// begin v 6301 adds
struct Hw6300_t {
    doublereal mjjmin, chnprb[20];
    integer iopstp, iopsh;
    logical optm, chon[20];
} ;

extern  Hw6300_t* gHw6300 ;

struct  Hwfmrs_t {
    doublereal fmrs[44688]	/* was [3][8][49][38] */;
} ;

extern  Hwfmrs_t* gHwfmrs ;

struct Hwcirc_t {
    integer circop, circac, circvr, circrv, circch;
} ;

extern  Hwcirc_t* gHwcirc ;
//end v6301 adds

struct Hwumsc_t {
    int lhwout;
} ;

extern Hwumsc_t* gHwumsc;

#endif

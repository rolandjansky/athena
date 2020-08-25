/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/SusySubprocessFinder.h"
#include "TruthUtils/PIDHelpers.h"

SusySubprocessFinder::SusySubprocessFinder(const std::string& name, ISvcLocator* pSvcLocator)
  : GenFilter(name, pSvcLocator)
{
  declareProperty("BeforeFilter", m_beforeFilter = true);
  declareProperty("Findmode", m_findmode = 2);
  declareProperty("Verbose", m_verbose = false);
  declareProperty("VerboseMode", m_verbose_mode = "all");
  declareProperty("VerboseNPart", m_verbose_nPart = 99999);

    // initialize all members in ctor
  for (int i=0; i < nSubprocMax; ++i) m_nSubproc[i] = 0;
}


StatusCode SusySubprocessFinder::filterInitialize() {
  if (m_beforeFilter) {
    m_beforeORafter = "BeforeFilter ";
  } else {
    m_beforeORafter = "AfterFilter  ";
  }

  ATH_MSG_INFO("SusySubprocessFinder::filterInitialize " << m_beforeORafter );

  for (int i = 0; i < nSubprocMax; ++i) {
    m_nSubproc[i] = 0;
  }

  return StatusCode::SUCCESS;
}


StatusCode SusySubprocessFinder::filterFinalize() {
  ATH_MSG_INFO("SusySubprocessFinder::filterFinalize " << m_beforeORafter);

  printf("SusySubprocessFinder  Before/After   Process :  nEvents \n");

  for (int iSub = 0; iSub < nSubprocMax; ++iSub) {
    bool isaCateg = ( (   iSub >=   0 && iSub <=   4)
                      || (iSub >=  51 && iSub <=  52)
                      || (iSub >=  55 && iSub <=  58)
                      || (iSub >=  61 && iSub <=  62)
                      || (iSub >=  71 && iSub <=  78)
                      || (iSub >=  81 && iSub <=  88)
                      || (iSub >= 111 && iSub <= 118)
                      || (iSub >= 122 && iSub <= 128)
                      || (iSub >= 133 && iSub <= 138)
                      || (iSub >= 144 && iSub <= 148)
                      || (iSub >= 157 && iSub <= 158)
                      || (iSub >= 167 && iSub <= 168)
                      || (iSub >= 201 && iSub <= 213)
                      || (iSub >= 216 && iSub <= 220)
                      || (iSub == nSubprocMax-1) );

    if ( !(isaCateg) && m_nSubproc[iSub] > 0) {
      printf("SusySubprocessFinder::WARNING  is not a categ, but has entries: subproc = %i\n", iSub);
    }

    if (!isaCateg) continue;

    printf("SusySubprocessFinder  %s   %7i  %-16s  %7i \n", m_beforeORafter.data(),
           iSub, subProcessText(iSub).data(), m_nSubproc[iSub]);
  }

  return StatusCode::SUCCESS;
}


StatusCode SusySubprocessFinder::filterEvent() {
  // FIND THE TWO INITIAL SPARTICLES
  int ID[2];
  findInitialSusy(ID, m_findmode);

  // GET SUBPROCESS
  int subproc = subProcessFromPdgids(ID[0],ID[1]);

  // If subproc not found, print the two initial sparticles
  if (subproc == -1) {
    ATH_MSG_WARNING(" Subprocess not found for IDs  " << ID[0] << "  " << ID[1]);
  }

  if (subproc == -1) subproc = nSubprocMax-1;  //use last entry in array for unknown subprocesses

  m_nSubproc[subproc]++;
  m_nSubproc[0]++;  // this contains the sum of all subprocesses (for quick checking)

  if (m_verbose) verboseMC(); // by default off ; is only for debugging (allows printing gen table)

  char cdum[200];
  sprintf(cdum, "SusySubprocessFinder  %s  %4i  %-16s  %8i  %8i",
          m_beforeORafter.data(), subproc, subProcessText(subproc).data(), ID[0], ID[1]);
  ATH_MSG_INFO(cdum);
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}


void SusySubprocessFinder::findInitialSusy(int ID[], int findmode) {
  int iP = 0;
  for (int i = 0; i < 2; i++) ID[i] = 0;

  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);

    // Loop over all particles in the event
    int ipitr = 0;
#ifdef HEPMC3
    for (auto pitr: genEvt->particles()) {

      if (findmode == 0 || findmode == 1 || findmode == 2) {
        // ######################################################## findmode == 0, 1
        // ##### findmode 0 works for [maybe, is fragile]:  Pythia, Herwig(fortran) [and Herwigpp? no, don't think so]
        // ##### findmode 1 works for Pythia and Herwig, but not Herwigpp (initial spart do not have two mothers)
        // ##### fomdmode 2 works for

        int pdg = pitr->pdg_id();
        int pdg_initial = 0;
        if (isSUSY(pdg)) {

          // Very simplistic finder: choose two first sparticles
          if (findmode == 0)
            pdg_initial= pdg;

          // Very simplistic finder: choose two first sparticles which have two parents
          else if (findmode == 1) {
            // accept as initial sparticle only if has TWO parents (always the case?)
            if ( pitr->production_vertex() != NULL ) {
              if ( pitr->production_vertex()->particles_in().size() == 2) pdg_initial = pdg;
            }
          }

          // Simplistic finder: choose two first sparticles wh
          else if (findmode == 2) {
            // accept as initial sparticle if parents are non-SUSY
            int nSusyParents = 0;
            if ( pitr->production_vertex() != NULL ) {
              for (auto  thisParent: pitr->production_vertex()->particles_out()){
                if (isSUSY( thisParent->pdg_id() )) nSusyParents++;
              }
            } else {
              ATH_MSG_WARNING("SUSY particle without parents  iPart = " << ipitr);
            }
            if(nSusyParents == 0) pdg_initial = pdg;
          }

        }

        // test if have a hit
        if (pdg_initial) {
          if (iP < 2) ID[iP++] = pdg_initial;

          // checks etc.
          if (findmode == 0 && iP == 2) break;
          if (iP > 2) {
            ATH_MSG_WARNING("Neglecting additional find: ipitr="<< ipitr << "   pdgid=" << pdg_initial);
          }
	      //if (iP == 2) break; // have found two sparticles, break off
        }
        // ########################################################
      }
    }
#else
    HepMC::GenEvent::particle_const_iterator pitr;
    for (pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {

      if (findmode == 0 || findmode == 1 || findmode == 2) {
        // ######################################################## findmode == 0, 1
        // ##### findmode 0 works for [maybe, is fragile]:  Pythia, Herwig(fortran) [and Herwigpp? no, don't think so]
        // ##### findmode 1 works for Pythia and Herwig, but not Herwigpp (initial spart do not have two mothers)
        // ##### fomdmode 2 works for

        int pdg = (*pitr)->pdg_id();
        int pdg_initial = 0;
        if (isSUSY(pdg)) {

          // Very simplistic finder: choose two first sparticles
          if (findmode == 0)
            pdg_initial= pdg;

          // Very simplistic finder: choose two first sparticles which have two parents
          else if (findmode == 1) {
            // accept as initial sparticle only if has TWO parents (always the case?)
            if ( (*pitr)->production_vertex() != NULL ) {
              if ( (*pitr)->production_vertex()->particles_in_size() == 2) pdg_initial = pdg;
            }
          }

          // Simplistic finder: choose two first sparticles wh
          else if (findmode == 2) {
            // accept as initial sparticle if parents are non-SUSY
            int nSusyParents = 0;
            if ( (*pitr)->production_vertex() != NULL ) {

              HepMC::GenVertex::particle_iterator firstParent =	(*pitr)->production_vertex()->particles_begin(HepMC::parents);
              HepMC::GenVertex::particle_iterator endParent = (*pitr)->production_vertex()->particles_end(HepMC::parents);
              HepMC::GenVertex::particle_iterator thisParent = firstParent;

              for (; thisParent != endParent; ++thisParent){
                if (isSUSY( (*thisParent)->pdg_id()) ) nSusyParents++;
              }
            } else {
              ATH_MSG_WARNING("SUSY particle without parents  iPart = " << ipitr);
            }
            if(nSusyParents == 0) pdg_initial = pdg;
          }

        }

        // test if have a hit
        if (pdg_initial) {
          if (iP < 2) ID[iP++] = pdg_initial;

          // checks etc.
          if (findmode == 0 && iP == 2) break;
          if (iP > 2) {
            ATH_MSG_WARNING("Neglecting additional find: ipitr="<< ipitr << "   pdgid=" << pdg_initial);
          }
	      //if (iP == 2) break; // have found two sparticles, break off
        }
        // ########################################################
      }
    }
#endif
    ipitr++;
  }

  // We should by now have filled ID[2] with the two initial sparticles
  if (iP < 2) {
    ATH_MSG_WARNING("Irregular number of initial sparticles identified: " << iP);
  }
}


bool SusySubprocessFinder::isSUSY(int pdg) {
  //return abs(pdg) > 1000000 && abs(pdg) < 2000050;
  return MC::PID::isSUSY(pdg);
}


int SusySubprocessFinder::subProcessFromPdgids(int pdg1, int pdg2) {
  // From the two initial pdgids find the appropriate subprocess code
  // (Maybe a bit tedious...)
  //
  // NB: two first squark generations are treated in common as squark and antisquark
  //     sbottom1/2 and stop1/2 are treated separately
  //     nsquarkTot is the sum of all squark flavours and particle/antiparticle (used in sq-X subprocess)

  int ID[2];
  ID[0] = pdg1;
  ID[1] = pdg2;
  int absID[2];
  absID[0] = abs(ID[0]);
  absID[1] = abs(ID[1]);

  int ngluino(0);
  int nsquark(0);
  int nantisquark(0);
  int nchi01(0);
  int nchi02(0);
  int nchi03(0);
  int nchi04(0);
  int nch1plus(0);
  int nch1minus(0);
  int nch2plus(0);
  int nch2minus(0);
  int nsbottom1(0);
  int nsbottom2(0);
  int nstop1(0);
  int nstop2(0);
  int nselLplus(0), nselLminus(0), nsmuLplus(0), nsmuLminus(0);
  int nstau1plus(0), nstau1minus(0), nstau2plus(0), nstau2minus(0);
  int nselRplus(0), nselRminus(0), nsmuRplus(0), nsmuRminus(0);
  int nsnue(0), nsnum(0), nsnut(0);
  int ngrav(0);

  int nother(0);

  // LOOP OVER THE TWO INITIALS
  for (int k = 0; k < 2; k++) {
    if (absID[k]==1000021) ngluino++;
    else if ((absID[k]>=1000001 && absID[k]<=1000004) ||
             (absID[k]>=2000001 && absID[k]<=2000004)) {
      if (ID[k]>0) nsquark++;
      else nantisquark++;
    }
    else if (absID[k]==1000005) nsbottom1++;  //FRAGILE: do not distinguish part/antipart
    else if (absID[k]==2000005) nsbottom2++;
    else if (absID[k]==1000006) nstop1++;
    else if (absID[k]==2000006) nstop2++;
    else if (ID[k]==1000022) nchi01++;
    else if (ID[k]==1000023) nchi02++;
    else if (ID[k]==1000025) nchi03++;
    else if (ID[k]==1000035) nchi04++;
    else if (ID[k]== 1000024)    nch1plus++;
    else if (ID[k]==-1000024)    nch1minus++;
    else if (ID[k]== 1000037)    nch2plus++;
    else if (ID[k]==-1000037)    nch2minus++;

    else if (ID[k] ==  1000011) nselLplus++;
    else if (ID[k] == -1000011) nselLminus++;
    else if (ID[k] ==  2000011) nselRplus++;
    else if (ID[k] == -2000011) nselRminus++;

    else if (ID[k] ==  1000013) nsmuLplus++;
    else if (ID[k] == -1000013) nsmuLminus++;
    else if (ID[k] ==  2000013) nsmuRplus++;
    else if (ID[k] == -2000013) nsmuRminus++;

    else if (ID[k] ==  1000015) nstau1plus++;
    else if (ID[k] == -1000015) nstau1minus++;
    else if (ID[k] ==  2000015) nstau2plus++;
    else if (ID[k] == -2000015) nstau2minus++;

    else if (absID[k] == 1000012) nsnue++;
    else if (absID[k] == 1000014) nsnum++;
    else if (absID[k] == 1000016) nsnut++;

    else if (ID[k] == 1000039) ngrav++;

    else nother++;
  }


  // FINALLY CLASSIFY EVENT

  //int nselL = nselLplus + nselLminus;
  //int nsmuL = nsmuLplus + nsmuLminus;
  int nstau1 = nstau1plus + nstau1minus;
  int nstau2 = nstau2plus + nstau2minus;
  //int nselR = nselRplus + nselRminus;
  //int nsmuR = nsmuRplus + nsmuRminus;

  int nsbottom = nsbottom1 + nsbottom2;
  int nstop = nstop1 + nstop2;

  int nsquarkTot = nsquark + nantisquark + nsbottom + nstop;

  int fs = -1;

  // CLASS: gl + X
  if      (ngluino==1 && nchi01==1) fs = 71;
  else if (ngluino==1 && nchi02==1) fs = 72;
  else if (ngluino==1 && nchi03==1) fs = 73;
  else if (ngluino==1 && nchi04==1) fs = 74;
  else if (ngluino==1 && nch1plus==1) fs = 75;
  else if (ngluino==1 && nch2plus==1) fs = 76;
  else if (ngluino==1 && nch1minus==1) fs = 77;
  else if (ngluino==1 && nch2minus==1) fs = 78;

  // CLASS: sq + X
  else if (nsquarkTot==1 && nchi01==1) fs = 81;
  else if (nsquarkTot==1 && nchi02==1) fs = 82;
  else if (nsquarkTot==1 && nchi03==1) fs = 83;
  else if (nsquarkTot==1 && nchi04==1) fs = 84;
  else if (nsquarkTot==1 && nch1plus==1) fs = 85;
  else if (nsquarkTot==1 && nch2plus==1) fs = 86;
  else if (nsquarkTot==1 && nch1minus==1) fs = 87;
  else if (nsquarkTot==1 && nch2minus==1) fs = 88;

  // CLASS: gl + sq
  else if (ngluino==1 && (nsquark==1 || nantisquark==1)) fs = 1;
  else if (ngluino==1 && nsbottom==1)  fs = 1;
  else if (ngluino==1 && nstop==1)  fs = 1;

  // CLASS: gl + gl
  else if (ngluino==2) fs = 2;

  // CLASS: sq + sq
  else if (nsquark==2) fs = 3;
  else if (nantisquark==2) fs = 3;

  // CLASS: sq + sqbar
  else if (nsquark==1 && nantisquark==1) fs = 4;

  // CLASS: sb + sb   //FRAGILE: do not distinguish part/antipart
  else if (nsbottom1==2) fs = 51;
  else if (nsbottom2==2) fs = 52;

  // CLASS: st + st   //FRAGILE: do not distinguish part/antipart
  else if (nstop1==2) fs = 61;
  else if (nstop2==2) fs = 62;

  // CLASS: sb + st: non-standard (not in Prospino)   //FRAGILE: do not distinguish part/antipart
  else if (nsbottom1==1 and nstop1==1) fs = 55;
  else if (nsbottom1==1 and nstop2==1) fs = 56;
  else if (nsbottom2==1 and nstop1==1) fs = 57;
  else if (nsbottom2==1 and nstop2==1) fs = 58;

  // NN
  else if (nchi01 == 2) fs = 111;
  else if (nchi01 == 1 && nchi02 == 1) fs = 112;
  else if (nchi01 == 1 && nchi03 == 1) fs = 113;
  else if (nchi01 == 1 && nchi04 == 1) fs = 114;
  else if (nchi02 == 2) fs = 122;
  else if (nchi02 == 1 && nchi03 == 1) fs = 123;
  else if (nchi02 == 1 && nchi04 == 1) fs = 124;
  else if (nchi03 == 2) fs = 133;
  else if (nchi03 == 1 && nchi04 == 1) fs = 134;
  else if (nchi04 == 2) fs = 144;

  // CC
  else if (nch1plus == 1 && nch1minus == 1) fs = 157;
  else if (nch2plus == 1 && nch2minus == 1) fs = 168;
  else if (nch1plus == 1 && nch2minus == 1) fs = 158;
  else if (nch2plus == 1 && nch1minus == 1) fs = 167;

  // NC
  else if (nchi01 == 1 && nch1plus == 1) fs = 115;
  else if (nchi01 == 1 && nch2plus == 1) fs = 116;
  else if (nchi01 == 1 && nch1minus == 1) fs = 117;
  else if (nchi01 == 1 && nch2minus == 1) fs = 118;

  else if (nchi02 == 1 && nch1plus == 1) fs = 125;
  else if (nchi02 == 1 && nch2plus == 1) fs = 126;
  else if (nchi02 == 1 && nch1minus == 1) fs = 127;
  else if (nchi02 == 1 && nch2minus == 1) fs = 128;

  else if (nchi03 == 1 && nch1plus == 1) fs = 135;
  else if (nchi03 == 1 && nch2plus == 1) fs = 136;
  else if (nchi03 == 1 && nch1minus == 1) fs = 137;
  else if (nchi03 == 1 && nch2minus == 1) fs = 138;

  else if (nchi04 == 1 && nch1plus == 1) fs = 145;
  else if (nchi04 == 1 && nch2plus == 1) fs = 146;
  else if (nchi04 == 1 && nch1minus == 1) fs = 147;
  else if (nchi04 == 1 && nch2minus == 1) fs = 148;

  //sleptons (sel, stau)
  else if (nselLplus == 1 && nselLminus == 1) fs = 201;
  else if (nselRplus == 1 && nselRminus == 1) fs = 202;
  else if (nsnue == 2) fs = 203;
  else if (nselLplus == 1 && nsnue == 1) fs = 204;
  else if (nselLminus == 1 && nsnue == 1) fs = 205;

  else if (nstau1plus == 1 && nstau1minus == 1) fs = 206;
  else if (nstau2plus == 1 && nstau2minus == 1) fs = 207;
  else if (nstau1 == 1 && nstau2 == 1) fs = 208;
  else if (nsnut == 2) fs = 209;
  else if (nstau1plus == 1 && nsnut == 1) fs = 210;
  else if (nstau1minus == 1 && nsnut == 1) fs = 211;
  else if (nstau2plus == 1 && nsnut == 1) fs = 212;
  else if (nstau2minus == 1 && nsnut == 1) fs = 213;

  //sleptons (smu): non-standard since Prospino treats sel and smu alike
  else if (nsmuLplus == 1 && nsmuLminus == 1) fs = 216;
  else if (nsmuRplus == 1 && nsmuRminus == 1) fs = 217;
  else if (nsnum == 2) fs = 218;
  else if (nsmuLplus == 1 && nsnum == 1) fs = 219;
  else if (nsmuLminus == 1 && nsnum == 1) fs = 220;

  else fs = -1;

  return fs;
  // if the subprocess is not found, -1 is returned (so should check for that)

}


//---------------------------------------------------------------------------
std::string SusySubprocessFinder::subProcessText(int sub) {
//---------------------------------------------------------------------------
  // max string length returned: 16

  std::string text = "unknown_subproc";

  if      (sub == 0) {text = "all_subprocs";}
  else if (sub == 1) {text = "gl__sq";}
  else if (sub == 2) {text = "gl__gl";}
  else if (sub == 3) {text = "sq__sq";}
  else if (sub == 4) {text = "sq__antisq";}

  else if (sub == 51) {text = "sbot1__antisbot1";}
  else if (sub == 52) {text = "sbot2__antisbot2";}
  else if (sub == 61) {text = "stop1__antistop1";}
  else if (sub == 62) {text = "stop2__antistop2";}

  // Non-standard (not in Prospino):
  else if (sub == 55) {text = "sbot1__antistop1";}
  else if (sub == 56) {text = "sbot1__antistop2";}
  else if (sub == 57) {text = "sbot2__antistop1";}
  else if (sub == 58) {text = "sbot2__antistop2";}


  else if (sub == 71) {text = "gl__N1";}
  else if (sub == 72) {text = "gl__N2";}
  else if (sub == 73) {text = "gl__N3";}
  else if (sub == 74) {text = "gl__N4";}
  else if (sub == 75) {text = "gl__C1+";}
  else if (sub == 76) {text = "gl__C2+";}
  else if (sub == 77) {text = "gl__C1-";}
  else if (sub == 78) {text = "gl__C2-";}

  else if (sub == 81) {text = "sq__N1";}
  else if (sub == 82) {text = "sq__N2";}
  else if (sub == 83) {text = "sq__N3";}
  else if (sub == 84) {text = "sq__N4";}
  else if (sub == 85) {text = "sq__C1+";}
  else if (sub == 86) {text = "sq__C2+";}
  else if (sub == 87) {text = "sq__C1-";}
  else if (sub == 88) {text = "sq__C2-";}

  else if (sub == 111) {text = "N1__N1";}
  else if (sub == 112) {text = "N1__N2";}
  else if (sub == 113) {text = "N1__N3";}
  else if (sub == 114) {text = "N1__N4";}
  else if (sub == 115) {text = "N1__C1+";}
  else if (sub == 116) {text = "N1__C2+";}
  else if (sub == 117) {text = "N1__C1-";}
  else if (sub == 118) {text = "N1__C2-";}

  else if (sub == 122) {text = "N2__N2";}
  else if (sub == 123) {text = "N2__N3";}
  else if (sub == 124) {text = "N2__N4";}
  else if (sub == 125) {text = "N2__C1+";}
  else if (sub == 126) {text = "N2__C2+";}
  else if (sub == 127) {text = "N2__C1-";}
  else if (sub == 128) {text = "N2__C2-";}

  else if (sub == 133) {text = "N3__N3";}
  else if (sub == 134) {text = "N3__N4";}
  else if (sub == 135) {text = "N3__C1+";}
  else if (sub == 136) {text = "N3__C2+";}
  else if (sub == 137) {text = "N3__C1-";}
  else if (sub == 138) {text = "N3__C2-";}

  else if (sub == 144) {text = "N4__N4";}
  else if (sub == 145) {text = "N4__C1+";}
  else if (sub == 146) {text = "N4__C2+";}
  else if (sub == 147) {text = "N4__C1-";}
  else if (sub == 148) {text = "N4__C2-";}

  else if (sub == 157) {text = "C1+__C1-";}
  else if (sub == 158) {text = "C1+__C2-";}
  else if (sub == 167) {text = "C2+__C1-";}
  else if (sub == 168) {text = "C2+__C2-";}

  else if (sub == 201) {text = "selL+__selL-";}
  else if (sub == 202) {text = "selR+__selR-";}
  else if (sub == 203) {text = "snuel__snuel";}
  else if (sub == 204) {text = "selL+__snuel";}
  else if (sub == 205) {text = "selL-__snuel";}

  else if (sub == 206) {text = "sta1+__sta1-";}
  else if (sub == 207) {text = "sta2+__sta2-";}
  else if (sub == 208) {text = "sta1+-__sta2-+";}
  else if (sub == 209) {text = "snuta__snuta";}
  else if (sub == 210) {text = "sta1+__snuta";}
  else if (sub == 211) {text = "sta1-__snuta";}
  else if (sub == 212) {text = "sta2+__snuta";}
  else if (sub == 213) {text = "sta2-__snuta";}

  else if (sub == 216) {text = "smuL+__smuL-";}
  else if (sub == 217) {text = "smuR+__smuR-";}
  else if (sub == 218) {text = "snumu__snumu";}
  else if (sub == 219) {text = "smuL+__snumu";}
  else if (sub == 220) {text = "smuL-__snumu";}

  return text; // of sub not found, return default "unknown_subproc"

}


/// The procedure below is not really part of the finder, it is used for debug purposes
/// and will never be on in a production situation
void SusySubprocessFinder::verboseMC() {
  printf("SUSYVERBOSESTART\n");
  McEventCollection::const_iterator itr;
  for (itr = events()->begin(); itr != events()->end(); ++itr) {
    const HepMC::GenEvent* genEvt = (*itr);

    int iPart = 0, iSusy = 0;
    // Loop over all particles in the event
#ifdef HEPMC3
    for (auto pitr: genEvt->particles()) {

      // ########################################################
      // Very simplistic finder: choose two first sparticles

      char c[100];

      int pdg = pitr->pdg_id();
      int status = pitr->status();
      int barcode = HepMC::barcode(pitr);
      double mass = pitr->generated_mass();

      double px = pitr->momentum().px();

      // Possibilities to filter / break off listing
      if (! (m_verbose_mode=="all" || (m_verbose_mode=="susy" && isSUSY(pdg)) ) ) continue;
      if (iPart > m_verbose_nPart) continue;

      int particles_in_size = 0;
      int particles_out_size = 0;
      // Children ------------------------------------
      std::string childrenT = "";
      //std::string childrenbarT = "";
      int nChildren = 0;

      if ( pitr->end_vertex() != NULL ) {
        particles_out_size = pitr->end_vertex()->particles_out().size();
        for (auto thisChild: pitr->end_vertex()->particles_out()) {
          sprintf(c,"%4i", thisChild->pdg_id());
          childrenT.append(c);
          sprintf(c,"(%i)", HepMC::barcode(thisChild));
          childrenT.append(c);
          childrenT.append(" ");
          nChildren++;
        }
      }

      // Parents --------------------------------------
      std::string parentsT = "";
      int nParents = 0;

      if ( pitr->production_vertex()  ) {

        particles_in_size = pitr->production_vertex()->particles_in().size();
        for (auto thisParent: pitr->production_vertex()->particles_in()) {
          sprintf(c,"%4i", thisParent->pdg_id());
          parentsT.append(c);
          sprintf(c,"(%i)", HepMC::barcode(thisParent));
          parentsT.append(c);
          parentsT.append(" ");
          nParents++;
        }

      }

      iPart++;
      if (isSUSY(pdg)) iSusy++;
      if (isSUSY(pdg) || iPart <= 30) printf("SUSYVERBOSE   %4i  %4i  st:%5i  CLHEP::bar:%5i  pdg: %8i  m: %5.1f  px: %6.1f | (%i->%i) |%i M: %s ||%i C: %s | \n", iPart, iSusy, status, barcode, pdg, mass, px/1000., particles_in_size, particles_out_size, nParents, parentsT.data(), nChildren, childrenT.data());

      if ( (particles_in_size != nParents) || (particles_out_size != nChildren) ) {
        ATH_MSG_WARNING("Not identical    prod: " << particles_in_size << " vs " << nParents << "     decay: " << particles_out_size << " vs " << nChildren);
      }

    }
#else
    HepMC::GenEvent::particle_const_iterator pitr;
    for (pitr = genEvt->particles_begin(); pitr != genEvt->particles_end(); ++pitr) {

      // ########################################################
      // Very simplistic finder: choose two first sparticles

      char c[100];

      int pdg = (*pitr)->pdg_id();
      int status = (*pitr)->status();
      int barcode = (*pitr)->barcode();
      double mass = (*pitr)->generated_mass();

      double px = (*pitr)->momentum().px();

      // Possibilities to filter / break off listing
      if (! (m_verbose_mode=="all" || (m_verbose_mode=="susy" && isSUSY(pdg)) ) ) continue;
      if (iPart > m_verbose_nPart) continue;

      int particles_in_size = 0;
      int particles_out_size = 0;
      // Children ------------------------------------
      std::string childrenT = "";
      //std::string childrenbarT = "";
      int nChildren = 0;

      if ( (*pitr)->end_vertex() != NULL ) {
        particles_out_size = (*pitr)->end_vertex()->particles_out_size();
        HepMC::GenVertex::particle_iterator firstChild = (*pitr)->end_vertex()->particles_begin(HepMC::children);
        HepMC::GenVertex::particle_iterator endChild = (*pitr)->end_vertex()->particles_end(HepMC::children);
        HepMC::GenVertex::particle_iterator thisChild = firstChild;
        for (; thisChild != endChild; ++thisChild) {
          sprintf(c,"%4i", (*thisChild)->pdg_id());
          childrenT.append(c);
          sprintf(c,"(%i)", (*thisChild)->barcode());
          childrenT.append(c);
          childrenT.append(" ");
          nChildren++;
        }
      }

      // Parents --------------------------------------
      std::string parentsT = "";
      int nParents = 0;

      if ( (*pitr)->production_vertex() != NULL ) {

        particles_in_size = (*pitr)->production_vertex()->particles_in_size();

        HepMC::GenVertex::particle_iterator firstParent =	(*pitr)->production_vertex()->particles_begin(HepMC::parents);
        HepMC::GenVertex::particle_iterator endParent = (*pitr)->production_vertex()->particles_end(HepMC::parents);
        HepMC::GenVertex::particle_iterator thisParent = firstParent;

        for (; thisParent != endParent; ++thisParent) {
          sprintf(c,"%4i", (*thisParent)->pdg_id());
          parentsT.append(c);
          sprintf(c,"(%i)", (*thisParent)->barcode());
          parentsT.append(c);
          parentsT.append(" ");
          nParents++;
        }

      }

      iPart++;
      if (isSUSY(pdg)) iSusy++;
      if (isSUSY(pdg) || iPart <= 30) printf("SUSYVERBOSE   %4i  %4i  st:%5i  CLHEP::bar:%5i  pdg: %8i  m: %5.1f  px: %6.1f | (%i->%i) |%i M: %s ||%i C: %s | \n", iPart, iSusy, status, barcode, pdg, mass, px/1000., particles_in_size, particles_out_size, nParents, parentsT.data(), nChildren, childrenT.data());

      if ( (particles_in_size != nParents) || (particles_out_size != nChildren) ) {
        ATH_MSG_WARNING("Not identical    prod: " << particles_in_size << " vs " << nParents << "     decay: " << particles_out_size << " vs " << nChildren);
      }

    }
#endif
  }
}

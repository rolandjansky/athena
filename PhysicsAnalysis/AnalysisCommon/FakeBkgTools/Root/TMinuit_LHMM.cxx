/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FakeBkgTools/TMinuit_LHMM.h"

#ifdef FAKEBKGTOOLS_ATLAS_ENVIRONMENT
ClassImp(TMinuit_LHMM)
#endif

void TMinuit_LHMM::mnrazz(Double_t ynew, Double_t *pnew, Double_t *y, Int_t &jh, Int_t &jl)
{
//*-*-*-*-*Called only by MNSIMP (and MNIMPR) to add a new point*-*-*-*-*-*-*
//*-*      =====================================================
//*-*        and remove an old one from the current simplex, and get the
//*-*        estimated distance to minimum.
//
//*-*        This is a verbatim copy of the base class function, except that
//*-*        printf's about the function not appearing to depend on the 
//*-*        fit parameters now are invoked only when DEBUG-level output
//*-*        is requested by the user.
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   /* Local variables */
   Double_t pbig, plit;
   Int_t i, j, nparp1;

   /* Function Body */
   for (i = 1; i <= fNpar; ++i) { fP[i + jh*fMaxpar - fMaxpar-1] = pnew[i-1]; }
   y[jh-1] = ynew;
   if (ynew < fAmin) {
      for (i = 1; i <= fNpar; ++i) { fX[i-1] = pnew[i-1]; }
      mninex(fX);
      fAmin   = ynew;
      fCstatu = "PROGRESS  ";
      jl      = jh;
   }
   jh     = 1;
   nparp1 = fNpar + 1;
   for (j = 2; j <= nparp1; ++j) { if (y[j-1] > y[jh-1]) jh = j; }
   fEDM = y[jh-1] - y[jl-1];
   if (fEDM <= 0) goto L45;
   for (i = 1; i <= fNpar; ++i) {
      pbig = fP[i-1];
      plit = pbig;
      for (j = 2; j <= nparp1; ++j) {
         if (fP[i + j*fMaxpar - fMaxpar-1] > pbig) pbig = fP[i + j*fMaxpar - fMaxpar-1];
         if (fP[i + j*fMaxpar - fMaxpar-1] < plit) plit = fP[i + j*fMaxpar - fMaxpar-1];
      }
      fDirin[i-1] = pbig - plit;
   }
L40:
   return;
L45:
   if (fISW[4] >= 0) {
     Printf("  FUNCTION VALUE DOES NOT SEEM TO DEPEND ON ANY OF THE %d VARIABLE PARAMETERS.",fNpar);
     Printf("          VERIFY THAT STEP SIZES ARE BIG ENOUGH AND CHECK FCN LOGIC.");
     Printf(" *******************************************************************************");
     Printf(" *******************************************************************************");
   }
   goto L40;
} /* mnrazz_ */

void TMinuit_LHMM::mnmnos()
{
//*-*-*-*-*-*-*-*-*-*-*Performs a MINOS error analysis*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  ===============================
//*-*        Performs a MINOS error analysis on those parameters for
//*-*        which it is requested on the MINOS command by calling
//*-*        MNMNOT for each parameter requested.
//
//*-*        This is a verbatim copy of the base class function, except that
//*-*        printf's about a new minimum being found are now invoked only 
//*-*        when DEBUG-level output
//*-*        is requested by the user.
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   /* Local variables */
   Double_t val2mi, val2pl;
   Int_t nbad, ilax, ilax2, ngood, nfcnmi, iin, knt;

   if (fNpar <= 0) goto L700;
   ngood = 0;
   nbad = 0;
   nfcnmi = fNfcn;
//*-*-                                     . loop over parameters requested
   for (knt = 1; knt <= fNpar; ++knt) {
      if (Int_t(fWord7[1]) == 0) {
         ilax = fNexofi[knt-1];
      } else {
         if (knt >= 7) break;
         ilax = Int_t(fWord7[knt]);
         if (ilax == 0) break;
         if (ilax > 0 && ilax <= fNu) {
            if (fNiofex[ilax-1] > 0) goto L565;
         }
         Printf(" PARAMETER NUMBER %3d NOT A VARIABLE. IGNORED.",ilax);
         continue;
      }
L565:
//*-*-                                        calculate one pair of M E s
      ilax2 = 0;
      mnmnot(ilax, ilax2, val2pl, val2mi);
      if (fLnewmn) goto L650;
//*-*-                                         update NGOOD and NBAD
      iin = fNiofex[ilax-1];
      if (fErp[iin-1] > 0) ++ngood;
      else                   ++nbad;
      if (fErn[iin-1] < 0) ++ngood;
      else                   ++nbad;
   }
//*-*-                                          end of loop . . . . . . .
//*-*-                                       . . . . printout final values .
   fCfrom  = "MINOS   ";
   fNfcnfr = nfcnmi;
   fCstatu = "UNCHANGED ";
   if (ngood == 0 && nbad == 0) goto L700;
   if (ngood > 0 && nbad == 0)  fCstatu = "SUCCESSFUL";
   if (ngood == 0 && nbad > 0)  fCstatu = "FAILURE   ";
   if (ngood > 0 && nbad > 0)   fCstatu = "PROBLEMS  ";
   if (fISW[4] >= 0)    mnprin(4, fAmin);
   if (fISW[4] >= 2)    mnmatu(0);
   return;
//*-*-                                       . . . new minimum found . . . .
L650:
   fCfrom  = "MINOS   ";
   fNfcnfr = nfcnmi;
   fCstatu = "NEW MINIMU";
   if (fISW[4] >= 0) {
     mnprin(4, fAmin);
     Printf(" NEW MINIMUM FOUND.  GO BACK TO MINIMIZATION STEP.");
     Printf(" =================================================");
     Printf("                                                  V");
     Printf("                                                  V");
     Printf("                                                  V");
     Printf("                                               VVVVVVV");
     Printf("                                                VVVVV");
     Printf("                                                 VVV");
     Printf("                                                  V");
     Printf(" ");
   }
   return;
L700:
   Printf(" THERE ARE NO MINOS ERRORS TO CALCULATE.");
} /* mnmnos_ */

void TMinuit_LHMM::mncuve()
{
//*-*-*-*-*-*-*-*Makes sure that the current point is a local minimum*-*-*-*-*
//*-*            ====================================================
//*-*        Makes sure that the current point is a local
//*-*        minimum and that the error matrix exists,
//*-*        or at least something good enough for MINOS and MNCONT
//*-*
//*-*        Modified so that printout ("FUNCTION MUST BE MINIMIZED...")
//*-*        only appears if verbose output is requested
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   /* Local variables */
   Double_t dxdi, wint;
   Int_t ndex, iext, i, j;

   if (fISW[3] < 1) {
     if (fISW[4] >= 0) {
       Printf(" FUNCTION MUST BE MINIMIZED BEFORE CALLING %s",(const char*)fCfrom);
     }
     fApsi = fEpsi;
     mnmigr();
   }
   if (fISW[1] < 3) {
      mnhess();
      if (fISW[1] < 1) {
         mnwarn("W", fCfrom, "NO ERROR MATRIX.  WILL IMPROVISE.");
         for (i = 1; i <= fNpar; ++i) {
            ndex = i*(i-1) / 2;
            for (j = 1; j <= i-1; ++j) {
               ++ndex;
               fVhmat[ndex-1] = 0;
            }
            ++ndex;
            if (fG2[i-1] <= 0) {
               wint = fWerr[i-1];
               iext = fNexofi[i-1];
               if (fNvarl[iext-1] > 1) {
                  mndxdi(fX[i-1], i-1, dxdi);
                  if (TMath::Abs(dxdi) < .001) wint = .01;
                  else                   wint /= TMath::Abs(dxdi);
               }
               fG2[i-1] = fUp / (wint*wint);
            }
            fVhmat[ndex-1] = 2 / fG2[i-1];
         }
         fISW[1] = 1;
         fDcovar = 1;
      } else  mnwerr();
   }
} /* mncuve_ */

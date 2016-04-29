

c Define common block containing all coupling constants and masses
c  which are used in the HELAS routines.
c
c  These include masses, widths and real/complex couplings.
c
c  This file can be built automatically from particles.dat and
c  interactions.dat
c
c====================================================================
c
c
c
c     QCD 
c
      double complex       gg(2)
      double precision     g
      common /COUPL_QCD/   g,gg   
c
c    kinematical masses
c
      double precision     hmass, wmass, zmass, amass,
     &                     tmass, bmass, lmass, cmass
      common /COUPL_MASS/  hmass, wmass, zmass, amass,
     &                     tmass, bmass, lmass, cmass
c
c   widths
c
      double precision     hwidth, wwidth, zwidth, 
     &                     twidth, lwidth, awidth
      common /COUPL_WIDTH/ hwidth, wwidth, zwidth, 
     &                     twidth, lwidth, awidth
c
c  couplings in the feynman rules
c
      double complex       gal(2), gad(2), gau(2), gwf(2),
     &                     gzn(2), gzl(2), gzd(2), gzu(2)
      double precision     gw, gwwa, gwwz
      common /COUPL_GAUGE/ gal   , gad   , gau   , gwf   ,
     &                     gzn   , gzl   , gzd   , gzu   ,
     &                     gw, gwwa, gwwz
c
      double complex       gwfc(2),  gwfs(2), gwfm(2)
      common /coupl_ckm/   gwfc,     gwfs   , gwfm	
c
      double complex       gwwh, gzzh, gwwhh, gzzhh, ghhh, ghhhh
      common /COUPL_SCAL/  gwwh, gzzh, gwwhh, gzzhh, ghhh, ghhhh
c
      double complex       ghtop(2), ghbot(2), ghtau(2), ghcha(2)
      common /COUPL_YUK/   ghtop   , ghbot   , ghtau   , ghcha


      double complex   GGH,GHND12(2),GND12HD1(2),GND12ZD(2),GZDE(2),GZDMU(2),GZDTA(2),GZDU(2),GZDD(2),GHD1ZD
      common /DC_COUPL/ GGH,GHND12,GND12HD1,GND12ZD,GZDE,GZDMU,GZDTA,GZDU,GZDD,GHD1ZD
      double precision ZDMASS,HD1MASS,ND1MASS,ND2MASS,ME,MMU
      double precision ZDWID,HD1WID,ND1WID,ND2WID,WE,WMU
      common /USR_MASS/  ZDMASS,HD1MASS,ND1MASS,ND2MASS,ME,MMU
      common /USR_DECAY/ ZDWID,HD1WID,ND1WID,ND2WID,WE,WMU

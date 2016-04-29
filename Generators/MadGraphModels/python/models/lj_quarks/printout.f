      subroutine printout
      implicit none
c
c     local
c
      integer i,iformat
      character*2 ab(2)
      real*8 ene
      double precision  Zero, One, Two, Three, Four, Half, Rt2
      parameter( Zero = 0.0d0, One = 1.0d0, Two = 2.0d0 )
c
c     include
c
      include 'coupl.inc'
      include 'input.inc'
c
c     Common to lh_readin and printout
c
      double precision  alpha, gfermi, alfas
      double precision  mtMS,mbMS,mcMS,mtaMS!MSbar masses
      double precision  Vud,Vus             !CKM matrix elements
      common/values/    alpha,gfermi,alfas,
     &                  mtMS,mbMS,mcMS,mtaMS,
     &                  Vud
c
c output all info
c
 10   format( 1x,a16,2x,f7.3,' GeV        ',a16,2x,f7.4,' GeV' )
 11   format( 1x,a13,2x,f11.5,2x,f11.5,2x,a13,2x,f11.5,2x,f11.5 )
 12   format( 1x,a13,2x,f6.2,a )
 13   format( 1x,a13,2x,f6.4,a )
 14   format( 1x,2(a13,2x,f10.7,2x,f10.7) )
 15   format( 1x,a13,2x,f9.5,a )
 16   format( 1x,a13,2x,f7.5 )
 17   format( 1x,a13,2x,f8.4 )
 18   format( 1x,a13,2x,f8.4,' GeV' )
 19   format( 1x,a13,2x,f6.4,a13,2x,f6.4 )
 20   format( 1x,a13,2x,f11.5,1x,f11.5 )
 21   format( 1x,a13,2x,f8.4,' GeV',1x,a13,2x,f8.4,' GeV' )
 22   format( 1x,a13,2x,f10.8,a13,2x,f6.4 )
 23   format( 1x,a13,2x,f8.4)
 24   format( 1x,a16,2x,f9.3,' GeV        ',a16,2x,f7.4,' GeV  (calc @ LO)')
 25   format( 1x,a16,2x,f7.3,' GeV        ',a16,2x,f7.4,' GeV')


      write(*,*) '*****************************************************'
      write(*,*) '*                    MadEvent                       *'
      write(*,*) '*        --------------------------------           *'
      write(*,*) '*          http://madgraph.hep.uiuc.edu             *'
      write(*,*) '*          http://madgraph.phys.ucl.ac.be           *'
      write(*,*) '*          http://madgraph.roma2.infn.it            *'
      write(*,*) '*        --------------------------------           *'
      write(*,*) '*                                                   *'
      write(*,*) '*         INTEGRATION CHANNEL LOG FILE              *'
      write(*,*) '*                                                   *'
      write(*,*) '*****************************************************'
      write(6,*)
      write(*,*) '*****************************************************'
      write(*,*) '*          SUMMARY OF THE SM PARAMETERS             *'
      write(*,*) '*****************************************************'
      write(6,*)
      write(6,*)  ' EW Parameters:'
      write(6,*)  '---------------'
      write(6,*)
      write(6,23) ' GF (10^-5*GeV^-2) = ',gfermi*1d5
      write(6,23) ' 1/alpha           = ',1d0/alpha
      write(6,23) ' M_Z   (GeV)       = ',zmass
      write(6,*)
      write(6,*)
      write(6,*)  'Boson masses and widths:'
      write(6,*)  '------------------------'
      write(6,*)
      write(6,24) 'Z mass  =  ',zmass, 'Z width  = ',zwidth
      write(6,24) 'W mass  =  ',wmass, 'W width  = ',wwidth
      write(6,24) 'H mass  =  ',hmass, 'H width  = ',hwidth
      write(6,*)
      write(6,*)  'Fermion masses and widths:'
      write(6,*)  '--------------------------'
      write(6,*)
      write(6,24) 'top    mass  =  ', tmass, 'top    width  = ', twidth
      write(6,10) 'bottom mass  =  ', bmass, 'bottom width  = ', Zero
      write(6,10) 'charm  mass  =  ', cmass, 'charm  width  = ', Zero
      write(6,10) 'tau    mass  =  ', lmass, 'tau    width  = ', Zero
      write(6,*)  'all other quark and lepton masses set to zero'
      write(6,*)
      write(6,*)  'User Model couplings:'
      write(6,*)  '---------------------'
      write(6,*)
      write(6,20) 'GGH  =  ', GGH
      write(6,11) 'GHND12(L)  =  ', GHND12(1),'GHND12(R)  =  ', GHND12(2)
      write(6,11) 'GND12HD1(L)  =  ', GND12HD1(1),'GND12HD1(R)  =  ', GND12HD1(2)
      write(6,11) 'GND12ZD(L)  =  ', GND12ZD(1),'GND12ZD(R)  =  ', GND12ZD(2)
      write(6,11) 'GZDE(L)  =  ', GZDE(1),'GZDE(R)  =  ', GZDE(2)
      write(6,11) 'GZDMU(L)  =  ', GZDMU(1),'GZDMU(R)  =  ', GZDMU(2)
      write(6,11) 'GZDTA(L)  =  ', GZDTA(1),'GZDTA(R)  =  ', GZDTA(2)
      write(6,11) 'GZDU(L)  =  ', GZDU(1),'GZDU(R)  =  ', GZDU(2)
      write(6,11) 'GZDD(L)  =  ', GZDD(1),'GZDD(R)  =  ', GZDD(2)
      write(6,20) 'GHD1ZD  =  ', GHD1ZD
      write(6,*)
      write(6,*) 'Boson couplings:'
      write(6,*) '----------------'
      write(6,*)
      write(6,20) 'gwwa  = ', gwwa
      write(6,20) 'gwwz  = ', gwwz
      write(6,20) 'gwwh  = ', gwwh
      write(6,20) 'gzzh  = ', gzzh
      write(6,20) 'ghhh  = ', ghhh
      write(6,*)
      write(6,20) 'gwwhh = ', gwwhh
      write(6,20) 'gzzhh = ', gzzhh
      write(6,20) 'ghhhh = ', ghhhh
      write(6,*)
      write(6,*) 'FFV couplings:'
      write(6,*) '--------------'
      write(6,*)
      write(6,11) 'gal(L)   =  ',gal(1), 'gal(R)   =  ',gal(2)
      write(6,11) 'gau(L)   =  ',gau(1), 'gau(R)   =  ',gau(2)
      write(6,11) 'gad(L)   =  ',gad(1), 'gad(R)   =  ',gad(2)
      write(6,*)
      write(6,11) 'gwf(L)   =  ',gwf(1), 'gwf(R)   =  ',gwf(2)
      write(6,*)
      write(6,11) 'gzn(L)   =  ',gzn(1), 'gzn(R)   =  ',gzn(2)
      write(6,11) 'gzl(L)   =  ',gzl(1), 'gzl(R)   =  ',gzl(2)
      write(6,11) 'gzu(L)   =  ',gzu(1), 'gzu(R)   =  ',gzu(2)
      write(6,11) 'gzd(L)   =  ',gzd(1), 'gzd(R)   =  ',gzd(2)
      write(6,*)
      write(6,*) 'FFH couplings:'
      write(6,*) '--------------'
      write(6,*)
      write(6,14) 'gHtop(L) =  ',ghtop(1), 'gHtop(R) =  ',ghtop(2)
      write(6,14) 'gHbot(L) =  ',ghbot(1), 'gHbot(R) =  ',ghbot(2)
      write(6,14) 'gHcha(L) =  ',ghcha(1), 'gHcha(R) =  ',ghcha(2)
      write(6,14) 'gHtau(L) =  ',ghtau(1), 'gHtau(R) =  ',ghtau(2)
      write(6,*)
      write(6,*) 'Strong couplings:'
      write(6,*) '-----------------'
      write(6,*)
      write(6,14) 'gg(1)    =  ',gg(1)   , 'gg(2)    =  ',gg(2)
      write(6,*)
      write(6,*) 'User Model masses and widths:'
      write(6,*) '-----------------------------'
      write(6,*)
      write(6,24) 'ZDMASS  =  ', ZDMASS, 'ZDWID    = ', ZDWID
      write(6,24) 'HD1MASS  =  ', HD1MASS, 'HD1WID    = ', HD1WID
      write(6,24) 'ND1MASS  =  ', ND1MASS, 'ND1WID    = ', ND1WID
      write(6,24) 'ND2MASS  =  ', ND2MASS, 'ND2WID    = ', ND2WID
      write(6,24) 'ME  =  ', ME, 'WE    = ', WE
      write(6,24) 'MMU  =  ', MMU, 'WMU    = ', WMU
      write(6,*)
      write(6,*) 'User Model Parameters:'
      write(6,*) '----------------------'
      write(6,24) 'a_cp   =  ', a_cp 
      write(6,24) 'b_cp   =  ', b_cp 
      write(6,24) 'c_cp   =  ', c_cp 
      write(6,24) 'd_cp   =  ', d_cp 
      write(6,24) 'e_cp   =  ', e_cp 
      write(6,24) 'f1_cp   =  ', f1_cp 
      write(6,24) 'f2_cp   =  ', f2_cp 
      write(6,24) 'g_cp   =  ', g_cp 
      write(6,24) 'h_cp   =  ', h_cp 
      write(6,*)

      return
      end


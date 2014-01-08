!
!> Computhe the covariance matrix on the fitted parameters
!> \author J.F.Laporte
!
 SUBROUTINE MATCOV
 USE M_MB_Control
 USE M_MB_General_CONSTANTS, ONLY : Pi, TwoPi
 USE M_MB_MuGeom
 USE M_MB_Digis
 USE M_MB_Reco_Tracks
 USE M_MB_COMUSC
 USE M_MB_Fit
 USE M_MB_InfoSim
 USE M_MB_EventInfo
!----Ali 
 USE M_MB_CTRABAZ
!----  
 IMPLICIT NONE
 INTEGER, PARAMETER :: nparmax=40
 INTEGER, PARAMETER :: nnnmax=nzxmax+nsxmax+2
 INTEGER :: ltopar(nparmax)
 REAL(8) :: vparin(nparmax),vparin0(nparmax)
 REAL(8) :: sens,evar(nparmax),evark(nparmax)
 REAL(8) :: dimpact(nnnmax,nparmax),dimpactpm(nnnmax,nparmax,2)
 REAL(8) :: thechi2k(nparmax,2)
 REAL(8) :: covmat(nparmax,nparmax)
 REAL(8) :: rtowir0(nzxmax),ftowir0(nzxmax),etowir0(nzxmax),wtowir0(nzxmax),stowir0(nsxmax),utowir0(nsxmax)
 REAL(8) :: rtowir(nzxmax) ,ftowir(nzxmax) ,etowir(nzxmax) ,wtowir(nzxmax) ,stowir(nsxmax) ,utowir(nsxmax)
 REAL(8) :: correl(nsxmax)
 INTEGER :: itypfo, nparam, iprloc, npp
 INTEGER :: ican, l, i, ivar, isens, j, i1, i2, jsim, nsens
 INTEGER :: nbdefr, is, iturn, iret, nzs, isc, ict, ip, ntest, nsdof
 INTEGER :: nisc, lisc(10), nec, nba
 INTEGER :: jtyp, jff, jzz, job, jspli, jsl, jsz, jtube
 INTEGER :: StationEta, StationPhi, MultiLayer, TubeLayer, Tube
 INTEGER :: DoubletR, DoubletZ, DoubletPhi, GasGap, MeasuresPhi, Strip, IsStrip, Channel, ChamberLayer, WireLayer
 REAL(8) :: SZflag
 REAL(8) :: enmasco, ytowir0, ztowir0
 REAL(8) :: thechi20, ytowir, ztowir, dytowir, dztowir, thechi2, thechi2m
 REAL(8) :: cone, dtet, dphi, dpar(5), poid(5,5), chi2, dchi(5), ali
 CHARACTER(1) :: staname(3)
 CHARACTER(2) :: car2
 CHARACTER(3) :: car3
 TYPE(trkSim) :: trSim
 REAL(8), PARAMETER :: thechi2level = 25.d0  !> Set minimal chi2  level wanted
 INTEGER, PARAMETER :: iturnmax = 2          !> Set the maximun nber of turns
 REAL(8) :: RCHAMOLD, ZCHAMOLD
 INTEGER :: intpos
 LOGICAL :: killTrack, tryAgain
!
!----Ali 
 INTEGER :: I_ISC, I_INOF,I_INOZ, nparam0
 REAL(8) :: ditemp
!
!---- Set type fit
   itypfo  = ItypFit
   ItypFit = 3
   enmasco = enmasc
!
   iprloc = 0
#if !defined(ATLAS_NOPRINTMU)
   if( MBPri >= 1 ) iprloc = 1
   if( iprloc >= 1 ) write(*,*) '**matcov  for event ',ievt
#endif
!
! number of fitted parameters : usually 5, but may be 4 when momentum fixed
   npp = 5
   IF( abs(FixMoCur) > 0.1d0 ) npp = 4
!
!---- Set external limits for tracking
   rchama = rcylin(4)
   zchama = zcylin(4)
!
!---- Loop on tracks
   do ican=1,nbcan
      if( chican(ican) >= 999.d0 )  CYCLE
!     iprloc = 0
!     if( flgcan(ican) == 1 ) iprloc = 2
!     if( iprloc >= 1 ) write(*,*) '**matcov  for event ',ievt
!     if( flgcan(ican) == 1 ) then
!       cxxcan(1,ican) = 0.d0
!       cxxcan(3,ican) = 0.d0
!       ndfcan(ican)   = 1
!       CYCLE
!     endif
#if !defined(ATLAS_NOPRINTMU)
     if( iprloc >= 1 ) print *,'  ican  ', ican
#endif
!
!---- Set matter centers arrays
     nbmasc     = nbmsxx(ican)
     enmasc     = enmsxx(ican)
     rrmasc(1:nbmasc) = rrmsxx(1:nbmasc,ican)
     llmasc(1:nbmasc) = llmsxx(1:nbmasc,ican)
     jvmasc(1:nbmasc) = 0
     x0masc(1:nbmasc) = x0msxx(1:nbmasc,ican)
     s1masc(1:nbmasc) = s1msxx(1:nbmasc,ican)
     s2masc(1:nbmasc) = s2msxx(1:nbmasc,ican)
     thmasc(1:nbmasc) = thmsxx(1:nbmasc,ican)
     almasc(1:nbmasc) = eamsxx(1:nbmasc,ican)
     blmasc(1:nbmasc) = ebmsxx(1:nbmasc,ican)
!
!---- Set internal limits for tracking
     rchami = rcylin(3)
     zchami = zcylin(3)
!---- Store central values for the parameters
     ltopar(1:5)  = 0
     vparin0(1:5) = paxcan(1:5,ican)
     if ( flgcan(ican) == 1 ) vparin0(1:5) = pazcan(1:5,ican)
     nparam = 5
     do l=1,nbmasc
       if( llmasc(l) >= 4 .and. llmasc(l) <= 5 ) then
         nparam = nparam + 1
         ltopar(nparam)  = l
         vparin0(nparam) = s1masc(l)
         nparam = nparam + 1
         ltopar(nparam) = -l
         vparin0(nparam) = 0.d0
         llmasc(l) = llmasc(l) + 2   ! allow bending in 2 dim for error calculation
       endif
     enddo
     if( nparam > nparmax ) then
       write(*,*) ' fatal problem in MATCOV,  nparam =',nparam,' => increase nparmax'
       stop
     endif
!
!---- Store central impact parameters, statistical chi2 and Ndf
!---- Set parameters
     vparin(1:nparam) = vparin0(1:nparam)
!---- Set matter parameters
     do i=1,nparam
       if( ltopar(i) > 0  ) s1masc( ltopar(i)) = vparin(i)
       if( ltopar(i) < 0  ) s2masc(-ltopar(i)) = vparin(i)
     enddo
!---- Track
     if ( flgcan(ican) == 1 ) then
       call setvdeb(3,vparin)
     else
       call setvdeb(1,vparin)
     endif 
!----Ali 
     if (DoAligFit > 0) then
       call TRAFAXALI_Reset
       TrafaxAli_CollectFix = 1
     endif
!---- 
     call TRAFAX_P(ican,3,rtowir0,ftowir0,etowir0,wtowir0,stowir0,utowir0)
!----Ali
     nparam0 = nparam
     if (DoAligFit > 0) then
       TrafaxAli_CollectFix = 0
       TrafaxAli_ValidN = 0
bisc:  do I_ISC=1,NMATMA
         do I_INOF=1,8
           do I_INOZ=1,NZM
             if ( TrafaxAli_NStations(I_ISC,I_INOF,I_INOZ) < 1 ) cycle
             ditemp = REAL( TrafaxAli_NStations(I_ISC,I_INOF,I_INOZ) )
             TrafaxAli_Zfix(I_ISC,I_INOF,I_INOZ) = TrafaxAli_Zfix(I_ISC,I_INOF,I_INOZ) / ditemp
             TrafaxAli_Tfix(I_ISC,I_INOF,I_INOZ) = TrafaxAli_Tfix(I_ISC,I_INOF,I_INOZ) / ditemp
             
             if( TrafaxAli_ValidN >= TrafaxAliMax ) then
               write(*,*) ' Error in Muonboy/Matcov : TrafaxAliMax too small'
               exit bisc
             endif
             TrafaxAli_ValidN = TrafaxAli_ValidN + 1
             
             TrafaxAli_2Valid(I_ISC,I_INOF,I_INOZ) = TrafaxAli_ValidN
             
             TrafaxAli_IZ         (TrafaxAli_ValidN) = 0
             TrafaxAli_IA         (TrafaxAli_ValidN) = 0
             TrafaxAli_DZ         (TrafaxAli_ValidN) = AligST(I_ISC)
             IF( I_ISC <= ncoubas2 ) then
               TrafaxAli_DA       (TrafaxAli_ValidN) = ResAngBa
             ELSE
               TrafaxAli_DA       (TrafaxAli_ValidN) = ResAngEc
             ENDIF
             TrafaxAli_WDZ        (TrafaxAli_ValidN) = 0.d0
             TrafaxAli_WDA        (TrafaxAli_ValidN) = 0.d0
             TrafaxAli_ApplyAliCor(TrafaxAli_ValidN) = 0

             if( nparam+2 > nparmax ) then
               write(*,*) ' Error in Muonboy/Matcov : nparmax too small'
               exit bisc
             endif
             IF( TrafaxAli_DZ(TrafaxAli_ValidN) > 0.00001d0 ) THEN    !< z displacement
               nparam = nparam + 1
               vparin0(nparam) = 0.d0
               vparin(nparam)  = vparin0(nparam)
               ltopar(nparam)  = TrafaxAli_ValidN
             ENDIF
             IF( TrafaxAli_NStations(I_ISC,I_INOF,I_INOZ) < 2 ) CYCLE
             IF( TrafaxAli_DA(TrafaxAli_ValidN) > 0.000001d0 ) THEN   !< theta displacement only if more than 2 tube
               nparam = nparam + 1
               vparin0(nparam) = 0.d0
               vparin(nparam)  = vparin0(nparam)
               ltopar(nparam)  =-TrafaxAli_ValidN
             ENDIF
           enddo
         enddo
       enddo bisc
     endif
!----    
     call getCorrelatedStrips(nsxcan(ican),isxcan(1:nsxcan(ican),ican),nsdof,correl)
     killTrack = .false.
     do i=1,nzxcan(ican)
       if( abs(etowir0(i)) > 10000.d0 ) then
         killTrack = .true.
         exit
       endif
     enddo
     if( .NOT.killTrack ) then
       do i=1,nsxcan(ican)
         if( abs(utowir0(i)) > 10000.d0 ) then
           killTrack = .true.
           exit
         endif
       enddo
     endif
     if( killTrack ) then
       chican(ican)= chican(ican) + 1000.d0
       write(*,*) ' WARNING in MATCOV: inconsistency detected, unable to compute error matrix ... kill track'
       CYCLE
     endif
     etowir0(1:nzxcan(ican)) = rdccan(1:nzxcan(ican),ican) !< get back errors used during the fit

     ytowir0 = REXTRA(1)
     ztowir0 = ZEXTRA(1)
     dytowir = DREXTR(1)
     dztowir = DZEXTR(1)
!     if( iprloc >= 2 ) then
!       print 9000,rtowir0(1:nzxcan(ican))
!       print 9002,stowir0(1:nsxcan(ican))
!     endif
!9000 format(' rtowir0 ',25F20.10)
!9002 format(' stowir0 ',25F20.10)
!---- Compute Ndf and chi2
     thechi20 = 0.d0
     do i=1,nzxcan(ican)
       thechi20 = thechi20 + ((rzxcan(i,ican)-rzican(i,ican))/etowir0(i))**2
     enddo
     do i=1,nsxcan(ican)
       is = isxcan(i,ican)
       thechi20 = thechi20 + correl(i)*((sbanhi(is)-stowir0(i))/utowir0(i))**2
     enddo
     do i=6,nparam
       thechi20 = thechi20 + vparin(i)**2
     enddo
     nbdefr = nzxcan(ican) + nsdof - 5
     if( npp == 4 )  nbdefr = nbdefr + 1
!!!     if( AtCosm == 0 ) then
!!!       thechi20 = thechi20 + (REXTRA(1)/DREXTR(1))**2 + (ZEXTRA(1)/DZEXTR(1))**2
!!!       nbdefr = nbdefr + 2
!!!     endif
     cxxcan(1,ican) = thechi20
     cxxcan(3,ican) = (REXTRA(1)/DREXTR(1))**2 + (ZEXTRA(1)/DZEXTR(1))**2
     ndfcan(ican)   = nbdefr
!
! save errors and initialise quantities related to simulated track
     ezifit(1:nzxcan(ican),ican) = etowir0(1:nzxcan(ican))
     esifit(1:nsxcan(ican),ican) = utowir0(1:nsxcan(ican))
     ytowir = 0.d0
     ztowir = 0.d0
!

     if( flgcan(ican) .ne. 1 ) then
       if( NTrSpSim >= 1 ) then
         jsim = 0
         cone = 999.d0
         do i=1,NTrSpSim   ! keep the nearest muon found
           trSim = TrSpSim(i)
           if( abs( trSim%codePDG ) == 13 ) then
             dtet = ACOS( trSim%posz / SQRT( trSim%posx**2 + trSim%posy**2 + trSim%posz**2 ) ) - vparin(1)
             dphi = ATAN2( trSim%posy, trSim%posx) - vparin(2)
             if( dphi >  Pi ) dphi = dphi - TwoPi
             if( dphi < -Pi ) dphi = dphi + TwoPi
             if( jsim == 0 .OR. sqrt( dphi**2 + dtet** 2 ) < cone ) then
               jsim = i
               cone = sqrt( dphi**2 + dtet** 2 )
             endif
           endif
         enddo
         if( jsim /= 0 ) then
           trSim = TrSpSim(jsim)
           vdeb(1,1) = trSim%posx
           vdeb(2,1) = trSim%posy
           vdeb(3,1) = trSim%posz
           vdeb(7,1) = SQRT( trSim%momx**2 + trSim%momy**2 + trSim%momz**2 )
           vdeb(4,1) = trSim%momx / vdeb(7,1)
           vdeb(5,1) = trSim%momy / vdeb(7,1)
           vdeb(6,1) = trSim%momz / vdeb(7,1)
           vdeb(8,1) = trSim%charge
           s1masc(1:nbmasc) = 0.d0
           s2masc(1:nbmasc) = 0.d0
           rchamold = rchami
           zchamold = zchami
           if( SQRT( vdeb(1,1)**2 + vdeb(2,1)**2 ) < rchami .AND. abs( vdeb(3,1) ) < zchami ) then
             rchami = SQRT( vdeb(1,1)**2 + vdeb(2,1)**2 )
             zchami = abs( vdeb(3,1) )
             InsideCylinder = .true.
           endif
           call TRAFAX_P(ican,2,rzisim(:,ican),ftowir,etowir,wtowir,rsisim(:,ican),utowir)
           call tkvfin2
           vparin(1:5) = vthetaphi(1:5)
           rchami = rchamold
           zchami = zchamold
           InsideCylinder = .false.
           ytowir = REXTRA(1)
           ztowir = ZEXTRA(1)
           dpar(1:5) = vparin0(1:5) - vparin(1:5)
           if( dpar(2) >  Pi ) dpar(2) = dpar(2) - TwoPi
           if( dpar(2) < -Pi ) dpar(2) = dpar(2) + TwoPi
           if( dpar(4) >  Pi ) dpar(4) = dpar(4) - TwoPi
           if( dpar(4) < -Pi ) dpar(4) = dpar(4) + TwoPi
         endif
       endif
     endif
!
#if !defined(ATLAS_NOPRINTMU)
     if(MBPri >= 3) then
       write(*,*) ' ===> rec param ',vparin0(1:5)
       if( flgcan(ican) /= 1 .AND. NTrSpSim >= 1 ) then
         if( jsim /= 0 )  write(*,*) ' ===> sim param ',vparin(1:5)
       endif
       write(*,'(5X,"I",15X,"X",9X,"Y",9X,"Z",8X,"Rzx",7X,"Rzi",7X,"Rto",7X,"Dif",7X,"Err",7X,"Sim",7X,"Sig",5X,"Sta")')
       do i=1,nzxcan(ican)
         isc = ( 1 + ktubhi(izxcan(i,ican))/10000 ) / 2
         car2 = '  '
         if( isc > NMATTR ) car2 = '-Z'
         if( isc > NMATTR .AND. rtubhi(izxcan(i,ican)) < -10.d0 ) car2 = '-S'
         write(*,'(I6,2X,A3,A2,2X,10F10.4,I5)') izxcan(i,ican),carisc(isc),car2,xdccan(i,ican),ydccan(i,ican),zdccan(i,ican)   &
                ,rzxcan(i,ican),rzican(i,ican),rtowir0(i),rzxcan(i,ican)-rtowir0(i),etowir0(i),rzisim(i,ican)               &
                ,(rzxcan(i,ican)-rzican(i,ican))/etowir0(i),ltubhi(izxcan(i,ican))
       enddo
       write(*,'(5X,"I",14X,"Ssi",7X,"Sto",7X,"Err",7X,"Sim",7X,"Sig",6X,"Time")')
       do i=1,nsxcan(ican)
         is  = isxcan(i,ican)
         ict =  kbanhi(is) / 1000
         ip  =  kbanhi(is)/10 - ict*100
         car3 = 'RPC'
         if( ict > NTRIBA ) car3 = 'TGC'
         car2 = '-Z'
         if( ip <= n1band(irptgc(ict)) ) car2 = '-S'
         write(*,'(I6,2X,A3,A2,2X,6F10.4)') is,car3,car2,sbanhi(is),stowir0(i),utowir0(i),rsisim(i,ican)   &
                                                        ,(sbanhi(is)-stowir0(i))/utowir0(i),tbanhi(is)
       enddo
       write(*,'(" number of outliers ",I6)') nzocan(ican)
       write(*,'(" number of holes    ",I6)') nhxcan(ican)
       do i=1,nhxcan(ican)
         select case( tyhcan(i,ican) )
           case( 1 )
             call DecoIdentAMDCMDT(ihxcan(i,ican), staname,jtyp,jff,jzz,job,jsl,jtube,             &
                                   StationEta,StationPhi,MultiLayer,TubeLayer,Tube)
             write(*,'(" MDT hole eta-phi-ML-lay-tub ",3A1,6I4)')                                  & 
                           staname,StationEta,StationPhi,MultiLayer,TubeLayer,Tube
           case( 2 )
             call DecoIdentAMDCRPC(ihxcan(i,ican), staname,jtyp,jff,jzz,job,jspli,jsl,jsz,jtube,   &
                                   StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,GasGap,MeasuresPhi,Strip )
             write(*,'(" RPC hole eta-phi-dr-dz-dphi-gap-measphi-strip ",3A1,8I4)')                        &
                           staname,StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,GasGap,MeasuresPhi,Strip
           case( 3 )
             call DecoIdentAMDCTGC(ihxcan(i,ican), staname,jtyp,jff,jzz,job,jspli,jsl,jsz,jtube,   &
                                   StationEta,StationPhi,GasGap,IsStrip,Channel )
             write(*,'(" TGC hole eta-phi-gap-isstrip-channel ",3A1,5I4)')                         & 
                           staname,StationEta,StationPhi,GasGap,IsStrip,Channel
           case( 4 )
             call DecoIdentAMDCCSC(ihxcan(i,ican), staname,jtyp,jff,jzz,job,jsl,jtube,szflag,      &
                                   StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip )
             write(*,'(" CSC hole eta-phi-chamblay-wireLay-measphi-strip ",3A1,6I4)')              & 
                           staname,StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip
           case default
             write(*,'(" hole from an alien technology ??? tyhcan ",I12)') tyhcan(i,ican)
         end select
       enddo
       if( AtCosm == 0 ) then
         write(*,'(" I.P. constraint Rrec-ErrR-Rsim-Sig ",4F8.2)') ytowir0,dytowir,ytowir,ytowir0/dytowir
         write(*,'(" I.P. constraint Zrec-ErrZ-Zsim-Sig ",4F8.2)') ztowir0,dztowir,ztowir,ztowir0/dztowir
       endif
       write(*,'(/," global chi2",F9.2," with",I3," dof and chican",2F8.2,/)') thechi20,nbdefr,chican(ican),cxxcan(2,ican)
     endif
#endif
!
!---- Set guessed variations amplitudes
     if ( flgcan(ican) == 1 ) then
       evark(1)        =  5.0d0
       evark(2)        =  0.2d0
       evark(3)        = 20.d0 * 0.2d0 /1000.d0
       evark(4)        = 20.d0 * 0.02d0/1000.d0
     else
       evark(1)        = 20.d0 * 0.05d0/1000.d0
       evark(2)        = 20.d0 * 2.d0  /1000.d0
       evark(3)        = evark(1)
       evark(4)        = evark(2)
     endif
     evark(5)        = 20.d0 * abs(vparin0(5))* 0.01d0
     evark(6:nparam) = sqrt(thechi2level)
!
!------------------------------------------------------------------------
!---- Do several turns to adjust parameters variations
     do 1100 iturn=1,iturnmax
#if !defined(ATLAS_NOPRINTMU)
       if( iprloc >= 2 ) print 7000,iturn,evark(1:nparam)
#endif
       evar(1:nparam) = evark(1:nparam)
       nsens = 1
       if( iturn == iturnmax ) nsens = 2
!
!---- Vary parameters one by one
       do 1200 ivar=1,nparam
!
!---- Do not compute effect of momentum when it is fixed (or no field)
         if( npp == 4 .AND. ivar == 5 ) CYCLE
!
!---- Set direction of variation
         ntest = 0
1250     tryAgain = .false.
         do 1300 isens=1,nsens
!
!---- Set actual variations
           sens = (-1.d0)**isens
!---- Set track parameters
           vparin(1:nparam) = vparin0(1:nparam)
           vparin(ivar)     = vparin0(ivar) + sens*evar(ivar)
!---- Set matter/Eloss parameters
           do i=1,nparam0
             if( ltopar(i) > 0 ) s1masc( ltopar(i)) = vparin(i)
             if( ltopar(i) < 0 ) s2masc(-ltopar(i)) = vparin(i)
           enddo
!---- Ali 
           if (DoAligFit > 0) then
             TrafaxAli_ApplyAliCor(1:TrafaxAli_ValidN) = 0
             if( ivar > nparam0 ) then
               i = ltopar(ivar)
               if( i > 0 ) then
                 TrafaxAli_IZ(i) = 1
                 TrafaxAli_IA(i) = 0
                 TrafaxAli_WDZ(i) = vparin(ivar)
                 TrafaxAli_ApplyAliCor(i) = 1
               else
                 i = -i
                 TrafaxAli_IZ(i) = 0
                 TrafaxAli_IA(i) = 1
                 TrafaxAli_WDA(i) = vparin(ivar)
                 TrafaxAli_ApplyAliCor(i) = 1
               endif
             endif
           endif
!---- 
!---- Track
           if ( flgcan(ican) == 1 ) then
             call setvdeb(3,vparin)
           else
             call setvdeb(1,vparin)
           endif 
           call TRAFAX_P(ican,3,rtowir,ftowir,etowir,wtowir,stowir,utowir)
!----Ali 
           if (DoAligFit > 0) TrafaxAli_ApplyAliCor(1:TrafaxAli_ValidN) = 0
!---- 

!---- Store impact deviations
           do i=1,nzxcan(ican)
             dimpactpm(i,ivar,isens)= (rtowir(i)-rtowir0(i))*sens/etowir0(i)   &
                                    + (ftowir(i)-ftowir0(i))*sens/2000.d0
! (very loose) phi constraint added on tubes to avoid NaN when no phi RPC/TGC
           enddo
           do i=nzxcan(ican)+1,nzxcan(ican)+nsxcan(ican)
             j = i-nzxcan(ican)
             dimpactpm(i,ivar,isens)= sqrt(correl(j))*(stowir(j)-stowir0(j))*sens/utowir0(j)
           enddo
           nzs = nzxcan(ican) + nsxcan(ican)
           if( AtCosm == 0 ) then
             nzs = nzs + 1
             dimpactpm(nzs,ivar,isens)= (REXTRA(1)-ytowir0)*sens/dytowir
             nzs = nzs + 1
             dimpactpm(nzs,ivar,isens)= (ZEXTRA(1)-ztowir0)*sens/dztowir
           endif
!---- Compute and store the "chi2"
           thechi2 = SUM( dimpactpm(1:nzs,ivar,isens)**2 )
           if( ivar > 5 )  thechi2 = thechi2 + (vparin(ivar)-vparin0(ivar))**2
           thechi2k(ivar,isens) = thechi2

           killTrack = .false.
           do i=1,nzxcan(ican)
             if( abs(etowir(i)) > 10000.d0 ) then
               killTrack = .true.
               exit
             endif
           enddo
           if( .NOT.killTrack ) then
             do i=1,nsxcan(ican)
               if( abs(utowir(i)) > 10000.d0 ) then
                 killTrack = .true.
                 exit
               endif
             enddo
           endif
           if( killTrack ) then
             if( ntest <= 4 ) then
               ntest = ntest + 1
               evar(ivar) = 0.5d0 * evar(ivar)
               tryAgain = .true.
               exit
             else if( MBPri >= 1 ) then
               write(*,*) ' Muonboy:matcov unreliable computation of error matrix '
             endif
           endif
1300     ENDDO
         if( tryAgain) go to 1250
!
!---- Set variations amplitudes for next turn
         thechi2m = thechi2k(ivar,1)
         if( nsens == 2 ) thechi2m = (thechi2k(ivar,1) + thechi2k(ivar,2))*0.5d0
         if( thechi2m > 0.d0 )  evark(ivar) = evar(ivar)*sqrt(thechi2level/thechi2m)
! put some protection to avoid too big variations when no sensitivity (cosmic without RPC-TGC strips)
         if( flgcan(ican) /= 1  ) then
           if( ( ivar == 2 .or. ivar == 4 ) .and. evark(ivar) > 0.7d0  ) evark(ivar) = 0.7d0
         else
           if( ivar == 1 .and. evark(ivar) > 50.d0  ) evark(ivar) = 50.d0
           if( ivar == 3 .and. evark(ivar) > 0.7d0  ) evark(ivar) = 0.7d0
         endif
1200   ENDDO
!
1100 ENDDO
!------------------------------------------------------------------------
!
     nzs = nzxcan(ican) + nsxcan(ican)
     if( AtCosm == 0 )  nzs = nzs + 2
!
!---- Average deviations
     dimpact(1:nzs,1:nparam) = (dimpactpm(1:nzs,1:nparam,1)+dimpactpm(1:nzs,1:nparam,2))*0.5d0
!
!!---- Print out general stuff
!     if( iprloc >= 2 ) then
!       print 8005,(ivar,ivar=1,nparam)
!       print 8006,thechi2k(1:nparam,1)
!       print 8008,thechi2k(1:nparam,2)
!       print 8010, thechi20
!       print 8012, nbdefr
!       print 6200,(ivar,ivar=1,nparam)
!     endif
!!---- Print out deviations
!     if( iprloc >= 2 ) then
!       print 8000,(ivar,ivar=1,nparam)
!       do l=1,nzxcan(ican)+nsxcan(ican)+2
!         if (l <= nzxcan(ican)) then
!           print 8002,l,dimpact(l,1:nparam)
!         else
!           print 8004,l,dimpact(l,1:nparam)
!         endif
!       enddo
!     endif
!
!---- Do not use 1/p variation when momentum fixed
     if( npp == 4 ) then
       dimpact(1:nzs,5:nparam-1) = dimpact(1:nzs,6:nparam)
       evar(5:nparam-1) = evar(6:nparam)
       nparam = nparam - 1
     endif
!
!---- Effect of misalignment
! nisc is the number of stations to be misaligned
     if( DoAligFit <= 0 ) then
       nisc = 0
       nba  = 0
       nec  = 0
       do i=1,nzxcan(ican)
         isc = ( 1 + ktubhi(izxcan(i,ican))/10000 ) / 2
         if( intpos(isc,lisc,nisc) > 0 )                          cycle
         if( AligST(isc) < 0.0001d0 )                             cycle
         if( isc > NMATTR .AND. rtubhi(izxcan(i,ican)) < -10.d0 ) cycle
         if( nisc >= 10 .or. nparam+nisc >= nparmax ) then
           write(*,*) ' Error in Muonboy/matcov => nparam nisc nparmax ',nparam,nisc,nparmax
           cycle
         endif
         nisc = nisc + 1
         lisc(nisc) = isc
         if( isc <= NCOUBAS2 ) then
           nba = nba + 1
         else
           nec = nec + 1
         endif
       enddo
       if( nisc > 0 ) then
         dimpact(1:nzs,nparam+1:nparam+nisc) = 0.d0
         do i=1,nzxcan(ican)
           isc = ( 1 + ktubhi(izxcan(i,ican))/10000 ) / 2
           if( isc > NMATTR .AND. rtubhi(izxcan(i,ican)) < -10.d0 ) cycle
           ali = AligST(isc)
!!$           if( isc <= NCOUBAS2 ) then
!!$             if( nba <= nec ) then
!!$               ali = sqrt( ali**2 + AligBaEc**2 )
!!$             endif
!!$           else
!!$             if( nba > nec ) then
!!$               ali = sqrt( ali**2 + AligBaEc**2 )
!!$             endif
!!$           endif
           do j=1,nisc
             if( lisc(j) == isc ) dimpact(i,nparam+j) = ali * wtowir0(i) / etowir0(i)
           enddo
         enddo
         evar(nparam+1:nparam+nisc) = 1.d0
         nparam = nparam + nisc
       endif
     endif
!
!---- Compute the inverse covariance matrix
!---- Hits/Strips terms
     do i1=1,nparam
       do i2=i1,nparam
         covmat(i1,i2) = SUM( dimpact(1:nzs,i1) * dimpact(1:nzs,i2) )
         covmat(i1,i2) = covmat(i1,i2)/evar(i1)/evar(i2)
         covmat(i2,i1) = covmat(i1,i2)
       enddo
     enddo
!---- MS/Eloss terms (+ potential misalignment)
     do i1=npp+1,nparam
       covmat(i1,i1) = covmat(i1,i1) + 1.d0
     enddo
!
#if !defined(ATLAS_NOPRINTMU)
!--- Print out general stuff
     if( iprloc >= 2 ) then
       print 8005,(ivar,ivar=1,nparam)
       print 8006,thechi2k(1:nparam,1)
       print 8008,thechi2k(1:nparam,2)
       print 8010, thechi20
       print 8012, nbdefr
       print 6200,(ivar,ivar=1,nparam)
     endif
!---- Print out deviations
     if( iprloc >= 2 ) then
       print 8000,(ivar,ivar=1,nparam)
       do l=1,nzxcan(ican)+nsxcan(ican)+2
         if (l <= nzxcan(ican)) then
           print 8002,l,dimpact(l,1:nparam)
         else
           print 8004,l,dimpact(l,1:nparam)
         endif
       enddo
     endif
#endif
!
!---- Do matrices inversions
     call icovmat(npp,covmat,iret,nparam,nparmax,iprloc)
     if( npp == 4 ) then
       covmat(1:4,5) = 0.d0
       covmat(5,1:4) = 0.d0
       covmat(5,5  ) = 999.d0  ! arbitrary
     endif
!
!---- Store covariance matrix
     coxcan(1:5,1:5,ican) = covmat(1:5,1:5)
     if ( flgcan(ican) == 1 ) then
       COYCAN(1:5,1:5,ican ) = COXCAN(1:5,1:5,ican)
       COZCAN(1:5,1:5,ican ) = COYCAN(1:5,1:5,ican)
     endif
!
#if !defined(ATLAS_NOPRINTMU)
!---- Print out  covariance matrix
     if( iprloc >= 1 .and. iret /= 0 ) write(*,'(" flag for weight matrix inversion ",I3)') iret
     if (iprloc >= 2) then
       write(*,'(" flag for matrix inversion ",I3)') iret
       print 8100,(i2,i2=1,nparam)
       do i1=1,nparam
         print 8102,i1,covmat(i1,1:nparam)
       enddo
     endif
!---- Print out  covariance matrix in compact format
     if (iprloc >= 1) then
       print 8104,(i2,i2=1,5)
       do i1=1,5
        print 8106,i1,(covmat(i1,i2)*100./sqrt(covmat(i1,i1)*covmat(i2,i2)),i2=1,5)
       enddo
       print 8108
       print 8110,vparin0(1:5)
       print 8112,(sqrt(covmat(i,i)),i=1,5)
     endif

     IF( MBPri >= 3 ) THEN
       IF( flgcan(ican) /= 1 .AND. NTrSpSim >= 1 ) THEN
         IF( jsim /= 0 ) THEN
           poid = covmat(1:npp,1:npp)
           call icovmat(npp,poid,iret,npp,5,iprloc)
           if( iret /= 0 ) write(*,'(" flag for cov matrix inversion ",I3)') iret
           if( npp == 5 ) then
             chi2 = poid(1,1)*dpar(1)**2 + 2.d0*poid(1,2)*dpar(1)*dpar(2) + 2.d0*poid(1,3)*dpar(1)*dpar(3)   &
                                         + 2.d0*poid(1,4)*dpar(1)*dpar(4) + 2.d0*poid(1,5)*dpar(1)*dpar(5)   &
                  + poid(2,2)*dpar(2)**2 + 2.d0*poid(2,3)*dpar(2)*dpar(3) + 2.d0*poid(2,4)*dpar(2)*dpar(4)   &
                                                                          + 2.d0*poid(2,5)*dpar(2)*dpar(5)   &
                  + poid(3,3)*dpar(3)**2 + 2.d0*poid(3,4)*dpar(3)*dpar(4) + 2.d0*poid(3,5)*dpar(3)*dpar(5)   &
                  + poid(4,4)*dpar(4)**2 + 2.d0*poid(4,5)*dpar(4)*dpar(5)                                    &
                  + poid(5,5)*dpar(5)**2
           else
             chi2 = poid(1,1)*dpar(1)**2 + 2.d0*poid(1,2)*dpar(1)*dpar(2) + 2.d0*poid(1,3)*dpar(1)*dpar(3)   &
                                         + 2.d0*poid(1,4)*dpar(1)*dpar(4)                                    &
                  + poid(2,2)*dpar(2)**2 + 2.d0*poid(2,3)*dpar(2)*dpar(3) + 2.d0*poid(2,4)*dpar(2)*dpar(4)   &
                  + poid(3,3)*dpar(3)**2 + 2.d0*poid(3,4)*dpar(3)*dpar(4)                                    &
                  + poid(4,4)*dpar(4)**2
           endif
           write(*,'(" chi2 of compatibility between sim/rec ",F9.2," for ",I1," dof ")') chi2,npp
           write(*,'(" with pulls for the ",I1," parameters ",5F6.2)') npp,(dpar(i1)/sqrt(covmat(i1,i1)),i1=1,npp)
           do i1=1,npp
             do i2=1,npp
               if( i1 == i2 ) then
                 dchi(i2) = dpar(i2)**2 / covmat(i2,i2)
               else
                 dchi(i2) = ( covmat(i2,i2)*dpar(i1)*dpar(i1) - covmat(i1,i2)*dpar(i1)*dpar(i2)    &
                            - covmat(i2,i1)*dpar(i2)*dpar(i1) + covmat(i1,i1)*dpar(i2)*dpar(i2) )  &
                          / ( covmat(i1,i1)*covmat(i2,i2) - covmat(i1,i2)*covmat(i2,i1) )
               endif
             enddo
             write(*,'(" with partial chi2 for the 1/2 parameters ",I2,5F7.1)') i1,dchi(1:npp)
           enddo
         ENDIF
       ENDIF
     ENDIF
#endif
   enddo
!
!---- Reset the number of m.s. centers to 0,
!---- ItypFit to its original value and tracking limits to their std. values
!----Ali 
     if (DoAligFit > 0) call TRAFAXALI_Reset
!----
   CALL Reset_MB_COMUSC
   ItypFit = itypfo
   enmasc  = enmasco
   rchami  = rcylin(3)
   zchami  = zcylin(3)
   rchama  = rcylin(4)
   zchama  = zcylin(4)
!
!---- Transports covariance matrix to  R-Z-CYLIN(2)  and to Perigee
   call TRAMATCOV
!
6200 format(' Devmax  ',3x,25i8)
!
7000 format(' Iturn   ',i3,25e8.1)
!
8000 format(' Dimpact ',3x,25i8)
8002 format(' Hits    ',i3,25f8.4)
8004 format(' Strips  ',i3,25f8.4)
8005 format(' Thechi2 ',3x,25i8)
8006 format('        +',3x,25f8.1)
8008 format('        -',3x,25f8.1)
8010 format(' Thechi20',3x,f8.1)
8012 format(' NdF     ',3x,I8)
!
8100 format(' Covmat  ',3x,25i8)
8102 format('         ',i3,25e8.1)
8104 format(' Norm    ',3x,25i8)
8106 format('         ',i3,25f8.2)
8108 format(' Parameters ')
8110 format(' vthetaphi(1 to 5)        = ',5f13.7)
8112 format('                        +/- ',5f13.7)
!
 END SUBROUTINE MATCOV
!
 SUBROUTINE getCorrelatedStrips(Nstr, IStr, Ndof, Cstr)
 USE M_MB_MuGeom
 USE M_MB_Digis
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: Nstr, Istr(*)
 INTEGER, INTENT(OUT) :: Ndof
 REAL(8), INTENT(OUT) :: Cstr(*)
 INTEGER :: I, J, ICODE, ICT, IP, IRPC
 REAL(8) :: dof
!
   dof = 0.d0
   Cstr(1:Nstr) = 1.d0
   DO I=1,Nstr
     ICODE = KBANHI(IStr(I)) / 10
     ICT   =  ICODE / 100
     IP    =  ICODE - ICT*100
     IRPC  = IRPTGC(ICT)
     DO J=1,Nstr
       IF( I == J )                            CYCLE
       ICODE = KBANHI(IStr(J)) / 10
       IF( ICODE / 100 /= ICT )                CYCLE
       IF( IP <= N1BAND(IRPC) ) THEN
         IF( ICODE - ICT*100 > N1BAND(IRPC) )  CYCLE
       ELSE
         IF( ICODE - ICT*100 <= N1BAND(IRPC) ) CYCLE
       ENDIF
       Cstr(I) = Cstr(I) + 1.d0
     ENDDO
     Cstr(I) = 1.d0 / Cstr(I)
     dof = dof + Cstr(I)
   ENDDO
   ndof = NINT(dof)
   IF( ABS(dof-ndof) > 0.0001d0 ) write(*,*) ' MuonboyCore:getCorrelatedStrips inconsistency ',dof,ndof
!
 END SUBROUTINE getCorrelatedStrips
!

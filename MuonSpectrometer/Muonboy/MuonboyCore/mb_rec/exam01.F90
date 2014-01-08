!
!> \author J.F.Laporte   11/02/97
!
 SUBROUTINE EXAM01(IDEVE)
 USE M_MB_Digis
 USE M_MB_General_CONSTANTS
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: IDEVE
 INTEGER :: knoi, ic, isc, ilohi, iw, inof, inoz, igeom, imdt
 INTEGER :: ip, itdeb, itfin, it
 REAL(8) :: ff0,zz0,tt0, direc, zce,rce,xce,yce, extle1,extle2
 REAL(8) :: tubmax,tubmin, ztub,rtub,xtub,ytub, tublen,tublenp,tublenn
 REAL(8) :: xpthit,ypthit,zpthit
 INTEGER, SAVE      :: ncal01 = 0
 INTEGER, PARAMETER :: ncalma = 2, knoima = 3
!
!> limit the number of calls to exam01
    ncal01=ncal01+1
    if (ncal01 == ncalma+1) print 4000,ncalma
    if (ncal01 > ncalma)    RETURN
!
    print 2000,ideve
!
    knoi=0
!
!> loop on all mutlilayer type
    do 100 ic=1,ncouch
!> compute the station and up/dow multilayer indexes
      isc   = (ic+1)/2
      ilohi = ic + 2 - 2*isc
!
!>> loop on all hit mutlilayers of a given mutlilayer type
      do 110 iw=1,nchahi(ic)
!>> take the Phi, Z, MDT and RPC indexes of the current hit mutlilayer
        inof = ifcha(ic,iw)
        inoz = izcha(ic,iw)
        igeom = igeoma(ic,inof,inoz)
        imdt = ISTMDTV(igeom)
!>> take the Phi, Z and T of the current hit mutlilayer
        ff0  = FCECHAV(igeom)
        zz0  = ZCECHAV(igeom)
        tt0  = TCECHAV(igeom)
!>> direc < 0 if one is in the "negative" end cap
        direc = 1.d0
        if(tt0 < 0.d0) direc = -1.d0
!>> compute the dist. to beam  and coordinates of this chamber center
        zce = zz0
        rce = tt0
        if (ic > ncouba) zce = tt0
        if (ic > ncouba) rce = zz0
        xce = rce * coschav(igeom)
        yce = rce * sinchav(igeom)
!>> compute the maximum and minimum lenght of the tubes in this chamber
        extle1 = SPACHAV(igeom)-SNACHAV(igeom)
        extle2 = SPICHAV(igeom)-SNICHAV(igeom)
        tubmax = max(extle1,extle2)
        tubmin = min(extle1,extle2)
!
!>>> loop on all the tube layer of the current hit mutlilayer
        do 120 ip=1,nltub(imdt)
!>>> take the starting and ending hit indexes of the current tube layer
          itdeb = ntubh0(ic,iw,ip) + 1
          itfin = ntubh0(ic,iw,ip) + ntubhi(ic,iw,ip)
!
!>>>> loop on all hit in the current tube layer
          do it=itdeb,itfin
!>>>> compute the dist. to beam and coordinates of this tube center
            ztub = ztubhi(it)
            rtub = tt0 + direc*tltub(imdt,ip,1)
            if (ic > ncouba) ztub = tt0 + direc*tltub(imdt,ip,1)
            if (ic > ncouba) rtub = ztubhi(it)
            xtub = rtub * coschav(igeom)
            ytub = rtub * sinchav(igeom)
!>>>> compute tube lengths
            tublen = ( (SPACHAV(igeom)-SNACHAV(igeom)) -(SPICHAV(igeom)-SNICHAV(igeom)) )    &
                       * (ztub-(zce-ZLOCHAV(igeom)/2.D0)) * ZLOCHAV_inv(igeom)                   &
                   + SPICHAV(igeom) - SNICHAV(igeom)
            tublenp = ( SPACHAV(igeom)-SPICHAV(igeom) ) * (ztub-(zce-ZLOCHAV(igeom)/2.D0))*ZLOCHAV_inv(igeom) + SPICHAV(igeom)
            tublenn = (-SNACHAV(igeom)+SNICHAV(igeom) ) * (ztub-(zce-ZLOCHAV(igeom)/2.D0))*ZLOCHAV_inv(igeom) - SNICHAV(igeom)
!>>>> compute coordinates of the hit point on the wire
            xpthit = xtub - stubhi(it) * sinchav(igeom)
            ypthit = ytub + stubhi(it) * coschav(igeom)
            zpthit = ztub
!>>>> print all infos and limit the nber of non muon hits printed
            if (jtubhi(it) /= 5 .AND. jtubhi(it) /= 6) then
              knoi=knoi+1
              if (knoi == knoima+1) print 3000,knoima
              if (knoi > knoima)    CYCLE
            endif
!
            print 1000, it
            print 1001
!
            print 1002, ztub, dtubhi(it)
!
            print 1004, jtubhi(it), rtubhi(it), stubhi(it), xpthit, ypthit, zpthit
!
            print 1006, ic,inof,inoz, carisc(isc),calohi(ilohi), rce, ff0*180./pi, xce,yce,zce,   &
                        ZLOCHAV(igeom), nltub(imdt), pastub(imdt), raytub(imdt), tubmax,tubmin
!
            print 1008, ip, rtub, xtub,ytub,ztub,tublen,tublenp,tublenn
!
          ENDDO
120     ENDDO
110   ENDDO
100 ENDDO
!
!
4000 format(/'=>Maximum calls (',I5,') to exam01 reached')
3000 format(/'=>Maximum nber (',I5,') of prints for non muon hits reached for this evt')
2000 format(/'**********************************',/'*>',     &
            /'*> EXAM01 ROUTINE ON EVT:',i10,     /'*>')
1000 format(/'*tube hit: hit nber=',i10)
1001 format( '******************************')
1002 format(' >data  info:    z of the hit                  =',f10.2,   &
           /' ------------    drift time                    =',f10.2)
1004 format(' >MC    info:    particule type                =',I10,     &
           /' ------------    impact parameter              =',f10.2,   &
           /'                 s parameter                   =',f10.2,   &
           /'                 point hit on the wire          ',         &
           /'                                       xpos    =',f10.2,   &
           /'                                       ypos    =',f10.2,   &
           /'                                       zpos    =',f10.2)
1006 format(' >Chber info:    multilayer index     IC       =',I10,     &
            /' ------------    Phi multilayer index INOF     =',I10,     &
           /'                 Z   multilayer index INOZ     =',I10,     &
           /'                 Multilayer name               =',1x,A3,   &
                                                               1x,A5,   &
           /'                 dist. to beam of chber center =',f10.2,   &
           /'                 Phi        of chber center    =',f10.2,   &
           /'                 X position of chber center    =',f10.2,   &
           /'                 Y position of chber center    =',f10.2,   &
           /'                 Z position of chber center    =',f10.2,   &
           /'                 Length of the chber           =',f10.2,   &
           /'                 Nber of tube layers           =',I10,     &
           /'                 Pitch                         =',f10.2,   &
           /'                 Tube radius                   =',f10.2,   &
           /'                 Maximun tube length           =',f10.2,   &
           /'                 Minimum tube length           =',f10.2)
1008 format(' >Tube  info:    Tube layer index     IP       =',I10,    &
           /' ------------    dist. to beam of tube  center =',f10.2,  &
           /'                 X position of tube  center    =',f10.2,  &
           /'                 Y position of tube  center    =',f10.2,  &
           /'                 Z position of tube  center    =',f10.2,  &
           /'                 tube length                   =',f10.2,  &
           /'                 tube length w.r.t. tube center ',        &
           /'                  on S>0 direction             =',f10.2,  &
           /'                  on S<0 direction             =',f10.2)
!
 END SUBROUTINE EXAM01
!

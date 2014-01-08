!
!> Track fit using hits (used when ItypFit = 3 or 6)
!> \author M.Virchaux
!
 SUBROUTINE TRAFAX(NVDEB,IFLAG)
 USE M_MB_Control
 USE M_MB_Types
 USE M_MB_MuGeom
 USE M_MB_FAX
 USE M_MB_CTRABAZ
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NVDEB, IFLAG
 INTEGER :: IVDEB, NN, ISC, J, IC, INOF, INOZ, IL01, ILIN, IL, NSFI
 INTEGER :: IGEOM, IZTPLA, ICT, IDU1,IDU2,IDU3, NTOT, IOPT
 REAL(8) :: RDEB, ZDEB, ZZTT, VVTZ, VVZT, SSSS, VVSS, CTOT
 TYPE(DcaPoint), SAVE :: dcapoin, dcapla(NMATMA,2)
 TYPE(DcaStrip), SAVE :: dcastr
 REAL(8), PARAMETER :: EPS = 0.0100D0, BIG = 9999999.D0
 INTEGER,SAVE :: ICTdeja(100)
 INTEGER :: Ndeja
 INTEGER, EXTERNAL :: intpos
!
   NZFIXE(1:NBFAX,1:2,1:NVDEB) = 0
   NSFIXE(1:NBFAX,1:2,1:NVDEB) = 0
   DO IVDEB=1,NVDEB
     WHERE( IOFFAX(1:NBFAX,1:2) <= 0 .OR. IOZFAX(1:NBFAX,1:2) <= 0 )
       DZFIXE(1:NBFAX,1:2,IVDEB) = 0.D0
     ELSEWHERE
       DZFIXE(1:NBFAX,1:2,IVDEB) = BIG
     END WHERE
   ENDDO
   DSFIXE(1:NBFAX,1:2,1:NVDEB) = 0.D0
   IF( saveDcaTrk ) THEN
     JZFIXE(:,:,1:NBFAX,1:2)   = 0
     IZFIXE(:,:,1:NBFAX,1:2)   = 0
     JSFIXE(:,1:NBFAX,1:2)     = 0
     ISFIXE(:,1:NBFAX,1:2)     = 0
     CSFIXE(:,1:NBFAX,1:2)     = 0.D0
   ENDIF
!
!>> Do not track if outside range
   RDEB = SQRT( VDEB(1,1)**2 + VDEB(2,1)**2 )
   ZDEB = ABS( VDEB(3,1) )
   IF( RDEB > RCHAMA+EPS .OR. ZDEB > ZCHAMA+EPS .OR.   &
      (RDEB < RCHAMI-EPS.AND.ZDEB < ZCHAMI-EPS) .OR. VDEB(7,1) < 0.5D0 )   RETURN
!
!>> Set module CTRABAZ
   IZTPLA = 0
   Ndeja  = 0
   DO NN=1,NBFAX
     ISC = ISCFAX(NN)
     DO J=1,2
       IC   = 2*ISC + 1 - J
       INOF = IOFFAX(NN,J)
       INOZ = IOZFAX(NN,J)
       IL01 = 0
       IF( INOF < 1 .OR. INOZ < 1 ) THEN
         INOF = IOFFAX(NN,3-J)
         INOZ = IOZFAX(NN,3-J)
         IL01 = 1
         IF( INOF < 1 .OR. INOZ < 1 ) CYCLE
       ENDIF
       IGEOM = IGEOMA(IC,INOF,INOZ)
       IF( IGEOM <= 0 )               CYCLE
       ILIN = ISTLINV(IGEOM)
       DO IL=IL01,NBLINK(ILIN)
         IF( IL <= 0 ) THEN
           IF( isOffChav(IGEOM) )     CYCLE
         ELSE     ! avoid duplication of RPC/TGC linked to several MDT stations
           ICT = ICTRIG(IL,ILIN)
           IF( isOffTrig(ICT)                  )  CYCLE
           IF( intpos(ICT, ICTdeja, Ndeja) > 0 )  CYCLE
         ENDIF
         IZTPLA = MIN( IZTPLA+1 , MAXPLA )
         CA0PLA(IZTPLA) = 1.D0
         SA0PLA(IZTPLA) = 0.D0
         IF(IL <= 0) THEN
           TT0PLA(IZTPLA) = TCECHAV(IGEOM)
           CF0PLA(IZTPLA) = COSCHAV(IGEOM)
           SF0PLA(IZTPLA) = SINCHAV(IGEOM)
           IF( IC > NCOUCH ) THEN
             CA0PLA(IZTPLA) = COS(GCECHAV(IGEOM))
             SA0PLA(IZTPLA) = SIN(GCECHAV(IGEOM))
           ENDIF
         ELSE
           ICT  = ICTRIG(IL,ILIN)
           Ndeja = MIN( Ndeja+1, 100 )
           ICTdeja(Ndeja) = ICT
           TT0PLA(IZTPLA) = TCETRI(ICT)
           CF0PLA(IZTPLA) = COSTRI(ICT)
           SF0PLA(IZTPLA) = SINTRI(ICT)
         ENDIF
         CALL CODEIN(0,IL,0,NN,0,J,IC,INOF,INOZ,ICT, IZTPLA)
       ENDDO
     ENDDO
   ENDDO
   IF( NCFAX > 0 ) THEN
     NSFIXE(NBFAX+1,1:2,1:NVDEB) = 0
     DSFIXE(NBFAX+1,1:2,1:NVDEB) = 0.0D0
     JSFIXE(:,NBFAX+1,1:2)       = 0
     ISFIXE(:,NBFAX+1,1:2)       = 0
     CSFIXE(:,NBFAX+1,1:2)       = 0.D0
     DO NN=1,NCFAX
       ICT = ICTFAX(NN)
       IF( intpos(ICT, ICTdeja, Ndeja) > 0 )  CYCLE
       IF( isOffTrig(ICT)                  )  CYCLE
       IF( IZTPLA >= MAXPLA )                 EXIT
       IZTPLA = IZTPLA + 1
       Ndeja = MIN( Ndeja+1, 100 )
       ICTdeja(Ndeja) = ICT
       TT0PLA(IZTPLA) = TCETRI(ICT)
       CF0PLA(IZTPLA) = COSTRI(ICT)
       SF0PLA(IZTPLA) = SINTRI(ICT)
       J = 2 - MOD(NN,2)
       CALL CODEIN(0,1,0,NBFAX+1,0,J,0,0,0,ICT,IZTPLA)   
     ENDDO
   ENDIF
!>> Total number of planes
   NZTPLA = IZTPLA
!
   CALL ReorderPlanes(IZTPLA)
!
!>> Track and catch the crossings
   IOPT = 1
   IF( IFLAG == 1 ) IOPT = 2
   CALL TRABAZ(NVDEB,2,IOPT)
!
   DO IVDEB=1,NVDEB
!>> Loop on crossings
     DO IZTPLA=1,NZTPLA
       IF( ICRPLA(IZTPLA) <= 0 )   CYCLE
       CALL DECOIN(IZTPLA,  IDU1,IDU2,NN,IDU3,J,IC,INOF,INOZ,ICT)
       ZZTT  = ZTTPLA(IZTPLA,IVDEB)
       VVTZ  = VTZPLA(IZTPLA,IVDEB)
       VVZT  = VZTPLA(IZTPLA,IVDEB)
       SSSS  = SSSPLA(IZTPLA,IVDEB)
       VVSS  = VSSPLA(IZTPLA,IVDEB)
       IF( ICT <= 0 ) THEN
         CALL CHIZZZ(IC,INOF,INOZ,ZZTT,SSSS,VVZT,VVTZ,VVSS,NTOT,CTOT, dcapoin)
         NZFIXE(NN,J,IVDEB) = NTOT
         DZFIXE(NN,J,IVDEB) = CTOT
         IF( DoAligFit >= 2 ) dcapla(NN,J) = dcapoin
         IF( IVDEB == 1 .AND. saveDcaTrk ) THEN
           WHERE( dcapoin%JJJ(:,:) /= 0 )   ! car sinon non initialises => inutile de les copier
             JZFIXE(:,:,NN,J) = dcapoin%JJJ(:,:)
             IZFIXE(:,:,NN,J) = dcapoin%III(:,:)
             IDZFIX(:,:,NN,J) = dcapoin%IDE(:,:)
             RZFIXE(:,:,NN,J) = dcapoin%RHIT(:,:)
             RIFIXE(:,:,NN,J) = dcapoin%RTRA(:,:)
             XDFIXE(:,:,NN,J) = dcapoin%XDCA(:,:)
             YDFIXE(:,:,NN,J) = dcapoin%YDCA(:,:)
             ZDFIXE(:,:,NN,J) = dcapoin%ZDCA(:,:)
             XDdFIXE(:,:,NN,J)= dcapoin%XDIR(:,:)
             YDdFIXE(:,:,NN,J)= dcapoin%YDIR(:,:)
             ZDdFIXE(:,:,NN,J)= dcapoin%ZDIR(:,:)
             ENFIXE(:,:,NN,J) = dcapoin%EDCA(:,:)
             RDFIXE(:,:,NN,J) = dcapoin%RDCA(:,:)
             SDFIXE(:,:,NN,J) = dcapoin%SDCA(:,:)
           END WHERE
         ENDIF
       ELSE
         CALL CHISSS(ICT,ZZTT,SSSS,VVZT,VVTZ,VVSS,NTOT,CTOT, dcastr)
         IF( IVDEB == 1 .AND. saveDcaTrk ) THEN
           IF( NTOT >= 1 .AND. NSFIXE(NN,J,IVDEB)+NTOT.LE.N8M ) THEN
             NSFI = NSFIXE(NN,J,IVDEB)
             JSFIXE(NSFI+1:NSFI+NTOT,NN,J)   = dcastr%JJJ(1:NTOT)
             ISFIXE(NSFI+1:NSFI+NTOT,NN,J)   = dcastr%III(1:NTOT)
             CSFIXE(NSFI+1:NSFI+NTOT,NN,J)   = dcastr%CCC(1:NTOT)
             XSDFIXE(NSFI+1:NSFI+NTOT,NN,J)  = dcastr%XDCA(1:NTOT)
             YSDFIXE(NSFI+1:NSFI+NTOT,NN,J)  = dcastr%YDCA(1:NTOT)
             ZSDFIXE(NSFI+1:NSFI+NTOT,NN,J)  = dcastr%ZDCA(1:NTOT)
             XSDdFIXE(NSFI+1:NSFI+NTOT,NN,J) = dcastr%XDir(1:NTOT)
             YSDdFIXE(NSFI+1:NSFI+NTOT,NN,J) = dcastr%YDir(1:NTOT)
             ZSDdFIXE(NSFI+1:NSFI+NTOT,NN,J) = dcastr%ZDir(1:NTOT)
             ESNFIXE(NSFI+1:NSFI+NTOT,NN,J)  = dcastr%EneDCA(1:NTOT)
             RSDFIXE(NSFI+1:NSFI+NTOT,NN,J)  = dcastr%ResDCA(1:NTOT)
           ENDIF
         ENDIF
         NSFIXE(NN,J,IVDEB) = NSFIXE(NN,J,IVDEB) + NTOT
         DSFIXE(NN,J,IVDEB) = DSFIXE(NN,J,IVDEB) + CTOT
       ENDIF
     ENDDO
     IF( DoAligFit >= 2 ) CALL misAlignementFit(IVDEB,NZTPLA, dcapla)
   ENDDO
!
 END SUBROUTINE TRAFAX
!
!
!
 SUBROUTINE misAlignementFit(ivdeb, npla, dcapla)
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_Types
 USE M_MB_CTRABAZ
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER,        INTENT(IN) :: ivdeb, npla
 TYPE(DcaPoint), INTENT(IN) :: dcapla(NMATMA,2)
 REAL(8) :: sa01, sb01, saa1, sab1, sbb1
 REAL(8) :: sa02, sb02, saa2, sab2, sbb2
 REAL(8) :: sa0, sb0, saa, sab, sbb, sigtran, sigrota
 REAL(8) :: amin, bmin, chiold, dchi2, dt, fac
 INTEGER :: I1, IDU1, IDU2, IDU3, NN1, J1, IC1, INOF1, INOZ1, ICT
 INTEGER :: I2, NN2, J2, IC2, INOF2, INOZ2, ISC
 LOGICAL :: alreadyDone(NMATMA,2)
!
    alreadyDone(:,:) = .false.
    DO I1=1,npla
      IF( ICRPLA(I1) <= 0 )     CYCLE
      CALL DECOIN(I1, IDU1,IDU2,NN1,IDU3,J1,IC1,INOF1,INOZ1,ICT)
      IF( ICT > 0 )             CYCLE
      IF( alreadyDone(NN1,J1) ) CYCLE
      ISC = (IC1+1)/2
      CALL getDeltaChi2Parameters(dcapla(NN1,J1), sa01,sb01,saa1,sab1,sbb1)
      DO I2=I1+1,npla
        IF( ICRPLA(I2) <= 0 )     CYCLE
        CALL DECOIN(I2, IDU1,IDU2,NN2,IDU3,J2,IC2,INOF2,INOZ2,ICT)
        IF( ICT > 0 )             CYCLE
        IF( alreadyDone(NN2,J2) ) CYCLE
        IF( (IC2+1)/2 /= ISC )    CYCLE
        IF( INOZ1 /= INOZ2 )      CYCLE
! we find two multilayers of the same station, treat them together
        CALL getDeltaChi2Parameters(dcapla(NN2,J2), sa02,sb02,saa2,sab2,sbb2)
! if a1,b1 and a2,b2 are the translation and rotation of ML 1 and 2, we
! have the constraint a1=a+b*dt*(1+tan**2), a2=a-b*dt*(1+tan**2), b1=b2=b
        dt = 0.5d0 * (TT0PLA(I1)-TT0PLA(I2)) * (1.d0+VZTPLA(I1,ivdeb)*VZTPLA(I2,ivdeb))
        sa0 =  sa01+sa02
        sb0 = (sa01-sa02)*dt + sb01 + sb02
        saa =  saa1+saa2
        sab = (saa1-saa2)*dt + sab1 + sab2 
        sbb = (saa1+saa2)*dt**2 + 2.d0*(sab1-sab2)*dt + (sbb1+sbb2)
        alreadyDone(NN1,J1) = .true.
        alreadyDone(NN2,J2) = .true.
        EXIT
      ENDDO
      IF( .NOT.alreadyDone(NN1,J1) ) THEN  ! we have only one multilayer for that station
        NN2 = 0
        sa0 = sa01
        sb0 = sb01
        saa = saa1
        sab = sab1
        sbb = sbb1
        alreadyDone(NN1,J1) = .true.
      ENDIF
      sigtran = MAX( AligST(ISC), 0.0020d0 )  !< 20 µm
      IF( isc <= ncoubas2 ) then
        sigrota = MAX( ResAngBa, 0.000010d0 ) !< 10 µrad
      ELSE
        sigrota = MAX( ResAngEc, 0.000010d0 ) !< 10 µrad
      ENDIF
      saa = saa + 2.d0/sigtran**2
      sbb = sbb + 2.d0/sigrota**2
      amin = ( - sbb*sa0 + sab*sb0 ) / ( saa*sbb - sab**2 )  !< fitted translation
      bmin = (   sab*sa0 - saa*sb0 ) / ( saa*sbb - sab**2 )  !< fitted rotation
      chiold = DZFIXE(NN1,J1,ivdeb)
      IF( NN2 > 0 ) chiold = chiold + DZFIXE(NN2,J2,ivdeb)
      dchi2  = sa0*amin + sb0*bmin + 0.5d0*saa*amin**2 + sab*amin*bmin + 0.5d0*sbb*bmin**2
      IF( dchi2 < -chiold ) THEN
        DZFIXE(NN1,J1,ivdeb) = 0.d0
        IF( NN2 > 0 ) DZFIXE(NN2,J2,ivdeb) = 0.d0
      ELSE
        IF( NN2 > 0 ) THEN   ! we dispatch the chi2 decrease proportionnaly in the two ML
          fac = DZFIXE(NN1,J1,ivdeb) / chiold
          IF( fac*dchi2 < -DZFIXE(NN1,J1,ivdeb) ) THEN
            DZFIXE(NN2,J2,ivdeb) = DZFIXE(NN2,J2,ivdeb) + DZFIXE(NN1,J1,ivdeb) + dchi2
            DZFIXE(NN1,J1,ivdeb) = 0.d0
          ELSE IF( (1.d0-fac)*dchi2 < -DZFIXE(NN2,J2,ivdeb) ) THEN
            DZFIXE(NN1,J1,ivdeb) = DZFIXE(NN1,J1,ivdeb) + DZFIXE(NN2,J2,ivdeb) + dchi2
            DZFIXE(NN2,J2,ivdeb) = 0.d0
          ELSE
            DZFIXE(NN1,J1,ivdeb) = DZFIXE(NN1,J1,ivdeb) + fac*dchi2
            DZFIXE(NN2,J2,ivdeb) = DZFIXE(NN2,J2,ivdeb) + (1.d0-fac)*dchi2
          ENDIF
        ELSE
          DZFIXE(NN1,J1,ivdeb) = DZFIXE(NN1,J1,ivdeb) + dchi2
        ENDIF
      ENDIF
    ENDDO
!
 END SUBROUTINE misAlignementFit
!
!
!> get the parameters of the developement : dchi2 = sa0*a + sb0*b + 0.5*saa*a**2 + sab*a*b + 0.5*sbb*b**2
!> with a the translation and b the rotation
!
 SUBROUTINE getDeltaChi2Parameters(dca, sa0,sb0,saa,sab,sbb)
 USE M_MB_Types
 USE M_MB_Fit
 IMPLICIT NONE
 TYPE(DcaPoint), INTENT(IN) :: dca
 REAL(8),       INTENT(OUT) :: sa0, sb0, saa, sab, sbb
 REAL(8) :: da0, db0, daa, dab, dbb, drr, fac
 INTEGER :: IP, I
!
    sa0 = 0.d0
    sb0 = 0.d0
    saa = 0.d0
    sab = 0.d0
    sbb = 0.d0
    DO IP=1,N4M
      DO I=1,4
        IF( dca%JJJ(IP,I) == 0 ) EXIT
        IF( dca%III(IP,I) <= 0 )                                              CYCLE
        IF( abs(dca%dRodZ(IP,I)) < 1.d-9 .AND. abs(dca%dRodR(IP,I)) < 1.d-9 ) CYCLE
        drr = dca%rtra(IP,I) - dca%rhit(IP,I)
        da0 = 2.d0*drr*dca%dRodZ(IP,I) 
        db0 = 2.d0*drr*dca%dRodR(IP,I)
        daa =                               2.d0*(dca%dRodZ(IP,I))**2
        dab = 2.d0*drr*dca%d2RodZdR(IP,I) + 2.d0* dca%dRodZ(IP,I)*dca%dRodR(IP,I) 
        dbb = 2.d0*drr*dca%d2RodRdR(IP,I) + 2.d0*(dca%dRodR(IP,I))**2
        fac = dca%scal(IP,I) / ( dca%rdca(IP,I) )**2
        sa0 = sa0 + fac*da0
        sb0 = sb0 + fac*db0
        saa = saa + fac*daa
        sab = sab + fac*dab
        sbb = sbb + fac*dbb
      ENDDO
    ENDDO 
!
 END SUBROUTINE getDeltaChi2Parameters
!

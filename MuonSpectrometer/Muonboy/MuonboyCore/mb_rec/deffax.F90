!
!> Give the list of stations crossed by the track candidate, to be used for the following fit
!> \author M.Virchaux
!
 SUBROUTINE DEFFAX(TOLZ,TOLS)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_General_CONSTANTS
 USE M_MB_MuGeom
 USE M_MB_FAX
 USE M_MB_CTRABAZ
 USE M_MB_Fit
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: TOLZ, TOLS
 LOGICAL :: NAKCHA, NAKCSC, NOKTRI
 INTEGER :: intpos
 INTEGER :: NOK, NN, ISC, ID, J, IW, IC, INOF, INOZ, IGEOM, IWDRATfun
 INTEGER :: IZTPLA, IOK, IDU1, IDU2, IDU3, IDU4, IDU5, Itrans
 INTEGER :: IL, ILIN, ICT, ISCmod, INOFP, INOFM, IDFI, NDFI
 REAL(8) :: DTHEOK, DPHIOK, ZZOK, TTOK, CA0, SA0, THEC, PHIC
 REAL(8) :: THEDIF, PHIDIF, TTZZ, ZZTT, SSSS, Tidea,Zidea,Sidea,WZidea,WTidea,VZTidea,VSTidea
 REAL(8) :: PHIOK(60), THEOK(60)
 REAL(8) :: VVTZ, VVZT, VVSS, SS, ZZ, TT, WZ, WT, VZT, VST, Z, S
 REAL(8) :: AXX, AYY, AZZ, AVX, AVY, AVZ, COSF, SINF
 REAL(8) :: BSS, BZZ, BTT, BVS, BVZ, BVT
 LOGICAL :: NotOKFI(8)
!
   ISCFAX = Zero
   IOFFAX = Zero
   IOZFAX = Zero
   IF( AtCosm == 0 ) THEN
!!$     DTHEOK = Pi / 10.D0
!!$     DPHIOK = Pi /  5.D0
     NDFI   = 1
   ELSE
!!$     DTHEOK = Pi /  2.D0
!!$     DPHIOK = Pi /  2.D0
     NDFI   = 3
   ENDIF
!
!>> Define the Theta and Phi sectors of the track
!>> and the default "FAX" stations from the "FIXE" stations
   NBFAX = NBFIXE
   NCFAX = 0
   NOK   = 0
   NotOKFI(:) = .true.
   DO 31 NN=1,NBFIXE
     ISC = ISCFIXE(NN)
     ID  = IDFIXE(NN)
     ISCFAX(NN) = ISC
     DO J=1,2
       IW = ABS( IWDRATfun(ID,J,ISC) )
       IF(IW <= 0)  CYCLE
       IC   = 2*ISC + 1 - J
       INOF = IFCHA(IC,IW)
       INOZ = IZCHA(IC,IW)
       IOFFAX(NN,J) = INOF
       IOZFAX(NN,J) = INOZ
       IGEOM = IGEOMA(IC,INOF,INOZ)
       NOK = MIN( NOK+1 , 60 )
       IF( IC <= NCOUBA ) THEN
         ZZOK = ZCECHAV(IGEOM)
         TTOK = TCECHAV(IGEOM)
       ELSEIF( IC <= NCOUCH ) THEN
         ZZOK = TCECHAV(IGEOM)
         TTOK = ZCECHAV(IGEOM)
       ELSE
         CA0 = COS(GCECHAV(IGEOM))
         SA0 = SIN(GCECHAV(IGEOM))
         ZZOK = - SA0 * ZCECHAV(IGEOM) + CA0 * TCECHAV(IGEOM)
         TTOK =   CA0 * ZCECHAV(IGEOM) + SA0 * TCECHAV(IGEOM)
       ENDIF
       THEOK(NOK) = ATAN2( TTOK , ZZOK )
       PHIOK(NOK) = FCECHAV(IGEOM)
       NotOKFI(INOF) = .false.
       INOFP = INOF
       INOFM = INOF
       DO IDFI=1,NDFI
         INOFP = INOFP + 1
         INOFM = INOFM - 1
         IF( INOFP == 9 ) INOFP = 1
         IF( INOFM == 0 ) INOFM = 8
         NotOKFI(INOFP) = .false.
         NotOKFI(INOFM) = .false.
       ENDDO
     ENDDO
     ISCFOX(NN) = ISCFAX(NN)
! special treatment for BIL JF=5 JZ=NZMS2-1/NZMS2 which are at a different T
     IF( ISC == 2 .AND. ( IOFFAX(NN,1) == 5 .OR. IOFFAX(NN,2) == 5 ) ) THEN
       IF( IOZFAX(NN,1) == NZMS2 .OR. IOZFAX(NN,1) == NZMS2-1 .OR.   & 
           IOZFAX(NN,2) == NZMS2 .OR. IOZFAX(NN,2) == NZMS2-1 ) THEN
         ISCFOX(NN) = ISCFAX(NN) + 50
       ENDIF
     ENDIF
31 ENDDO
!
!>> Set common CTRABAZ
   IZTPLA = 0
   DO ISC=1,NMACSC
     IF( AtCosm == 0 ) THEN
       DTHEOK = Pi / 10.D0
       DPHIOK = Pi /  5.D0
     ELSE IF( intpos(ISC,ISCFIXE,NBFIXE) > 0 ) THEN
       DTHEOK = Pi / 10.D0
       DPHIOK = Pi /  5.D0
     ELSE
       DTHEOK = Pi /  2.D0
       DPHIOK = Pi /  2.D0
     ENDIF
     DO J=1,2
       IC   = 2*ISC + 1 - J
       DO 111 INOF=1,8
         IF( NotOKFI(INOF) )             CYCLE
b110:    DO 110 INOZ=1,NZM
           IGEOM = IGEOMA(IC,INOF,INOZ)
           IF( IGEOM <= 0 )              CYCLE
           IF( ZLOCHAV(IGEOM) < 1.D0 )   CYCLE
           DO NN=1,NBFAX
             IF( ISC == ISCFAX(NN) .AND. INOF == IOFFAX(NN,J)   &
                                   .AND. INOZ == IOZFAX(NN,J) )   CYCLE b110
           ENDDO
           CA0 = 1.D0
           SA0 = 0.D0
           IF(IC <= NCOUBA) THEN
             ZZOK = ZCECHAV(IGEOM)
             TTOK = TCECHAV(IGEOM)
           ELSEIF( IC.LE.NCOUCH ) THEN
             ZZOK = TCECHAV(IGEOM)
             TTOK = ZCECHAV(IGEOM)
           ELSE
             CA0 = COS(GCECHAV(IGEOM))
             SA0 = SIN(GCECHAV(IGEOM))
             ZZOK = - SA0 * ZCECHAV(IGEOM) + CA0 * TCECHAV(IGEOM)
             TTOK =   CA0 * ZCECHAV(IGEOM) + SA0 * TCECHAV(IGEOM)
           ENDIF
           THEC = ATAN2( TTOK , ZZOK )
           PHIC = FCECHAV(IGEOM)
           DO IOK=1,NOK
             THEDIF = ABS( THEC - THEOK(IOK) )
             IF( THEDIF < DTHEOK ) THEN
               PHIDIF = ABS( PHIC - PHIOK(IOK) )
               PHIDIF = MIN( PHIDIF , ABS(PHIDIF-TWOPI) )
               IF( PHIDIF < DPHIOK ) THEN
                 IZTPLA = MIN( IZTPLA+1 , MAXPLA )
                 CF0PLA(IZTPLA) = COSCHAV(IGEOM)
                 SF0PLA(IZTPLA) = SINCHAV(IGEOM)
                 CA0PLA(IZTPLA) = CA0
                 SA0PLA(IZTPLA) = SA0
                 TT0PLA(IZTPLA) = TCECHAV(IGEOM)
                 CALL CODEIN(0,0,0,0,0,J,IC,INOF,INOZ,0, IZTPLA)
                 CYCLE b110
               ENDIF
             ENDIF
           ENDDO
110      ENDDO b110
111    ENDDO
     ENDDO
   ENDDO
!>> Total number of planes
   NZTPLA = IZTPLA
!
   CALL ReorderPlanes(IZTPLA)
!
!>> Track and catch the crossings
   CALL TRABAZ(1,2,1)
!
!>> Loop on crossings
   DO IZTPLA=1,NZTPLA
     IF( ICRPLA(IZTPLA) <= 0 )                        CYCLE
     CALL DECOIN(IZTPLA, IDU1,IDU2,IDU3,IDU4,J,IC,INOF,INOZ,IDU5)
     TTZZ = TT0PLA(IZTPLA)
     ZZTT = ZTTPLA(IZTPLA,1)
     SSSS = SSSPLA(IZTPLA,1)
     VVTZ = VTZPLA(IZTPLA,1)
     VVZT = VZTPLA(IZTPLA,1)
     VVSS = VSSPLA(IZTPLA,1)
     IGEOM = IGEOMA(IC,INOF,INOZ)
     Itrans = ItransCHA( IGEOM )
     CALL SZTWU_Ideal_from_True( Itrans, SSSS,ZZTT,TTZZ,VVSS,VVZT,VVTZ, Sidea,Zidea,Tidea,WZidea,WTidea,VZTidea,VSTidea )
     Z = Zidea + (TTZZ-Tidea) * VZTidea
     S = Sidea + (TTZZ-Tidea) * VSTidea
     IF( IC <= NCOUCH ) THEN
       IF( NAKCHA(IC,INOZ,IGEOM,Z,S,TTZZ,TOLZ,TOLS) .OR. isOffChav(IGEOM) ) THEN
! track does not cross the MDT, but check if it would not nevertheless cross one RPC/TGC ?
         ILIN = ISTLINV(IGEOM)
         IF( NBLINK(ILIN) > 0 ) THEN
           COSF = COSCHAV(IGEOM)
           SINF = SINCHAV(IGEOM)
           CALL MuboSZTtoXYZ( (IC<=NCOUBA), SINF,COSF, SSSS,ZZTT,TTZZ, AXX,AYY,AZZ)
           CALL MuboSZTtoXYZ( (IC<=NCOUBA), SINF,COSF, VVSS,VVZT,VVTZ, AVX,AVY,AVZ)
           DO IL=1,NBLINK(ILIN)
             ICT = ICTRIG(IL,ILIN)
             IF( isOffTrig(ICT)               ) CYCLE
             IF( intpos(ICT,ICTFAX,NCFAX) > 0 ) CYCLE
             COSF  = COSTRI(ICT)
             SINF  = SINTRI(ICT)
             CALL MuboXYZtoSZT( (ICT<=NTRIBA), SINF,COSF, AXX,AYY,AZZ, BSS,BZZ,BTT)
             CALL MuboXYZtoSZT( (ICT<=NTRIBA), SINF,COSF, AVX,AVY,AVZ, BVS,BVZ,BVT)
             Itrans = ItransTRI(ICT)
             CALL SZTWU_Ideal_from_True(Itrans, BSS,BZZ,BTT,BVS,BVZ,BVT, SS,ZZ,TT,WZ,WT,VZT,VST)
             Z = ZZ + (TCETRI(ICT)-TT) * VZT
             S = SS + (TCETRI(ICT)-TT) * VST
             IF( NOKTRI(ICT,Z,S,TOLZ,TOLS) ) CYCLE
             IF( NCFAX < NMATMA ) THEN
               NCFAX = NCFAX + 1
               ICTFAX(NCFAX) = ICT
             ENDIF
           ENDDO
         ENDIF
         CYCLE
       ENDIF
     ELSE
       IF( NAKCSC(IGEOM,Z,S,TOLZ,TOLS) ) CYCLE
       IF( isOffChav(IGEOM)            ) CYCLE
     ENDIF
     ISC = (IC+1)/2
     ISCmod = ISC
     IF( ISC == 2 .AND. INOF == 5 .AND. ( INOZ == NZMS2 .OR. INOZ == NZMS2-1 ) ) ISCmod = ISC + 50
     DO NN=1,NBFAX
       IF( ISCmod == ISCFOX(NN) ) GO TO 190
     ENDDO
     NBFAX = NBFAX + 1
     NN = NBFAX
     ISCFAX(NN)   = ISC
     ISCFOX(NN)   = ISCmod
190  IOFFAX(NN,J) = INOF
     IOZFAX(NN,J) = INOZ
   ENDDO
!
   IF( NCFAX > 0 ) THEN  ! avoid double use of RPC/TGC (when additionnal ones are also linked)
fax: DO NN=1,NBFAX
       ISC = ISCFAX(NN)
       DO J=1,2
         IC   = 2*ISC + 1 - J
         INOF = IOFFAX(NN,J)
         INOZ = IOZFAX(NN,J)
         IF( INOF < 1 .OR. INOZ < 1 ) THEN
           INOF = IOFFAX(NN,3-J)
           INOZ = IOZFAX(NN,3-J)
           IF( INOF < 1 .OR. INOZ < 1 ) CYCLE
         ENDIF
         IF(IGEOMA(IC,INOF,INOZ) <= 0)  CYCLE
         ILIN = ISTLINV( IGEOMA(IC,INOF,INOZ) )
         IF( NBLINK(ILIN) > 0 ) THEN
           DO IL=1,NBLINK(ILIN)
             IOK = intpos(ICTRIG(IL,ILIN),ICTFAX,NCFAX)
             IF( IOK > 0 ) THEN
               NCFAX = NCFAX - 1
               IF( NCFAX <= 0 ) EXIT fax
               ICTFAX(IOK:NCFAX) = ICTFAX(IOK+1:NCFAX+1)
             ENDIF
           ENDDO
         ENDIF
       ENDDO
     ENDDO fax
   ENDIF
!
 END SUBROUTINE DEFFAX
!

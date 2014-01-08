!
!> \author SaMuSoG
!
 SUBROUTINE MATALIGNEMENT
 USE M_MB_General_CONSTANTS
 USE M_MB_COMATI
 IMPLICIT NONE
!
      REAL(8) :: VV(50)
      REAL(8) :: XR(20),YR(20),DX(4),DY(4),DX0(4),DY0(4),ZBOX(10)
      REAL(8) :: VV1(12),VV2(12),U1(3),U2(3),U3(3),UB(3)
      REAL(8) :: XYZC(3),XYZCBOX(3,9)
      REAL(8) ::  YC1A(99,4), YC1B(99,4)
      REAL(8) :: SIPIS8,COPIS8, Fali, DXBOX,DYBOX,DZBOX
      REAL(8) :: DXSUPPORT,DYSUPPORT,DZSUPPORT, A,B, S, D, EPSX
      REAL(8) :: ROAL,RIAL, OFFSETBOX,DS, EPSZ, DBOXA,DBOXB
      INTEGER :: JFF1A(99,4),JFF1B(99,4),JFF1C(99,4),JFF1D(99,4)
      INTEGER ::  IV1A(99,4), IV1B(99,4), IV1C(99,4), IV1D(99,4)
      INTEGER :: JFF1E(99), IV1E(99)
      INTEGER :: JFF, L, JZZL, NB, IB, IVMDEB_Z, JTYP, ITRANS, LB
      INTEGER :: LL, LBOX,NBOX, I, JMIN,JMAX, JZZ, JFFF, NMIN,NMAX
      INTEGER :: NTUBEMIN,NTUBEMAX, IVMFIN_Z
      CHARACTER(3) :: ID_Bar(4)
      CHARACTER(4) :: CAR4
      CHARACTER(8) :: CAR8
      INTEGER, EXTERNAL :: I10JFF, JTROU3
!
#include "AmdcStand/comamu.inc"
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      IVMDEB_Z = NVMATI + 1
!
      SIPIS8    = SIN(Pi/8.D0)
      COPIS8    = COS(Pi/8.D0)
!
      JFF1A(:,:) = 0
      JFF1B(:,:) = 0
      JFF1C(:,:) = 0
      JFF1D(:,:) = 0
      JFF1E(:)   = 0
      YC1A(:,:)  = 0.D0
      YC1B(:,:)  = 0.D0
!
!---------------------------------------------------------------------------
      DO 900 JFF=1,8
       Fali = (JFF-1)*Pi/4.D0 - Pi/2.D0
!
!---------------------------------------------------------------------------
!- Define alignement tubes with boxes and supports as new volumes
       DO 300 L=1,Nb_Align_Tub
         IF(I10JFF( Lfi_Align(L) , 9-JFF ) <= 0) CYCLE
         JZZL = (L+2) / 3
!
!- Define projective Barrel alignment boxes and supports
         IF( Name_Align(L)(1:1) /= 'B' ) GO TO 150
!
         DXBOX     =  6.
         DYBOX     =  6.
         DZBOX     =  6.
         DXSUPPORT = 25.
         DYSUPPORT =  1.5
         DZSUPPORT =  4.5
         NB = 3
         DO IB=1,NB
          U1(1:3) = EndPoint_Align(1:3,2,L) - EndPoint_Align(1:3,1,L)
          CALL NORM3V( U1 )
          UB(1:3) = (/ Zero, Zero, One /)
          CALL PVEC3V( U1,UB, U2 )
          CALL NORM3V( U2 )
          CALL PVEC3V( U1,U2, U3 )
          IF(IB == 1)THEN
            XYZC(1:3) = EndPoint_Align(1:3,1,L)
          ELSEIF(IB == 2)THEN
            JTYP = JTROU3('BML',MTYP,CARTYP)
            XYZC(2) = Pos_R(JTYP,JFF,1)/10.D0 + Geo_dz(JTYP,1,8)/10. - DYSUPPORT - 0.05D0
            A = U2(2)*XYZC(2) - DOT_PRODUCT(U2, EndPoint_Align(1:3,1,L))
            B = U3(2)*XYZC(2) - DOT_PRODUCT(U3, EndPoint_Align(1:3,1,L))
            XYZC(1) = -(A*U3(3)-B*U2(3)) / (U2(1)*U3(3)-U3(1)*U2(3))
            XYZC(3) = (-B-U3(1)*XYZC(1)) / U3(3)
          ELSE
            XYZC(1:3) = EndPoint_Align(1:3,2,L)
          ENDIF
          VV1( 1: 3)=XYZC(1:3)+DXBOX*U1(1:3)+DYBOX*U2(1:3)+DZBOX*U3(1:3)
          VV1( 4: 6)=XYZC(1:3)+DXBOX*U1(1:3)-DYBOX*U2(1:3)+DZBOX*U3(1:3)
          VV1( 7: 9)=XYZC(1:3)+DXBOX*U1(1:3)-DYBOX*U2(1:3)-DZBOX*U3(1:3)
          VV1(10:12)=XYZC(1:3)+DXBOX*U1(1:3)+DYBOX*U2(1:3)-DZBOX*U3(1:3)
          VV2( 1: 3)=XYZC(1:3)-DXBOX*U1(1:3)+DYBOX*U2(1:3)+DZBOX*U3(1:3)
          VV2( 4: 6)=XYZC(1:3)-DXBOX*U1(1:3)-DYBOX*U2(1:3)+DZBOX*U3(1:3)
          VV2( 7: 9)=XYZC(1:3)-DXBOX*U1(1:3)-DYBOX*U2(1:3)-DZBOX*U3(1:3)
          VV2(10:12)=XYZC(1:3)-DXBOX*U1(1:3)+DYBOX*U2(1:3)-DZBOX*U3(1:3)
          WRITE(CAR4,'(I4)') L
          CAR8 = 'AlBb'//CAR4
!- Define boxes
          IF( JFF1A(L,IB) <= 0 .OR. ABS(XYZC(2)-YC1A(L,IB)) > 0.0010D0 ) THEN
            CALL NEWMAT( CAR8, MATALI1, JFF, JZZL )
            CALL PLAMATXX( Fali, 4, VV1 )
            CALL PLAMATXX( Fali, 4, VV2 )
            CALL CLOMAT
            CALL ENDMAT
            JFF1A(L,IB) = JFF
            IV1A(L,IB)  = NVMATI
            YC1A(L,IB)  = XYZC(2)
          ELSE
            ITRANS = JFF - JFF1A(L,IB) + 1
            CALL COPYMAT( IV1A(L,IB), ITRANS, CAR8,MATALI1,JFF,JZZL )
          ENDIF
          XYZCBOX(1:3,IB) = XYZC(1:3)
!- Define supports of boxes
          S=1.
          IF(EndPoint_Align(1,1,L) < 0.) S=-1.
          D = (XYZC(1)-S*DYBOX*U2(1))*U2(1) + (XYZC(2)-S*DYBOX*U2(2))*U2(2) + (XYZC(3)-S*DYBOX*U2(3))*U2(3)
          VV1( 1) = XYZC(1) - S*DXSUPPORT
          VV1( 2) = XYZC(2) +   DYSUPPORT
          VV1( 3) = XYZC(3) -   DZSUPPORT
          VV1( 4) = XYZC(1) - S*DXSUPPORT
          VV1( 5) = XYZC(2) -   DYSUPPORT
          VV1( 6) = XYZC(3) -   DZSUPPORT
          VV1( 7) = XYZC(1) - S*DXSUPPORT
          VV1( 8) = XYZC(2) -   DYSUPPORT
          VV1( 9) = XYZC(3) +   DZSUPPORT
          VV1(10) = XYZC(1) - S*DXSUPPORT
          VV1(11) = XYZC(2) +   DYSUPPORT
          VV1(12) = XYZC(3) +   DZSUPPORT
          EPSX=S*0.01
          VV2(1)   = (-EPSX + (D-U2(2)*VV1(2) -U2(3)*VV1(3) )/U2(1) )
          VV2(2:3) = VV1(2:3)
          VV2(4)   = (-EPSX + (D-U2(2)*VV1(5) -U2(3)*VV1(6) )/U2(1) )
          VV2(5:6) = VV1(5:6)
          VV2(7)   = (-EPSX + (D-U2(2)*VV1(8) -U2(3)*VV1(9) )/U2(1) )
          VV2(8:9) = VV1(8:9)
          VV2(10)  = (-EPSX + (D-U2(2)*VV1(11)-U2(3)*VV1(12))/U2(1) )
          VV2(11:12) = VV1(11:12)
          CAR8 = 'AlBs'//CAR4
          IF( JFF1B(L,IB) <= 0 .OR. ABS(XYZC(2)-YC1B(L,IB)) > 0.0010D0 ) THEN
            CALL NEWMAT( CAR8, MATALI1, JFF, JZZL )
            CALL PLAMATXX( Fali, 4, VV1 )
            CALL PLAMATXX( Fali, 4, VV2 )
            CALL CLOMAT
            CALL ENDMAT
            JFF1B(L,IB) = JFF
            IV1B(L,IB)  = NVMATI
            YC1B(L,IB)  = XYZC(2)
          ELSE
            ITRANS = JFF - JFF1B(L,IB) + 1
            CALL COPYMAT( IV1B(L,IB), ITRANS, CAR8,MATALI1,JFF,JZZL )
          ENDIF
         ENDDO
!
150      CONTINUE
!
!- Define Endcap alignment boxes as new volumes
         IF( Name_Align(L)(1:1) /= 'E' ) GO TO 250

         DXBOX = 8.
         DYBOX = 8.
         DZBOX = 8.
         IF(Name_Align(L)(1:3) == 'EC1') THEN
           NB=3
           ID_Bar(1)='BA2'
           ID_Bar(2)='BA3'
           ID_Bar(3)='BA4'
         ELSEIF(Name_Align(L)(1:3) == 'EC2') THEN
           NB=4
           ID_Bar(1)='BA1'
           ID_Bar(2)='BA3'
           ID_Bar(3)='BA4'
           ID_Bar(4)='BA5'
         ELSEIF(Name_Align(L)(1:3) == 'EC3') THEN
           NB=3
           ID_Bar(1)='BA1'
           ID_Bar(2)='BA4'
           ID_Bar(3)='BA5'
         ENDIF
         DO IB=1,NB
          LB=JTROU3(ID_Bar(IB),Nb_Align_Bar,Name_Align(Nb_Align_Tub+1)) + Nb_Align_Tub
          U1(1:3) = EndPoint_Align(1:3,2,L)  - EndPoint_Align(1:3,1,L)
          CALL NORM3V( U1 )
          UB(1:3) = EndPoint_Align(1:3,2,LB) - EndPoint_Align(1:3,1,LB)
          CALL NORM3V( UB )
          CALL PVEC3V( U1,UB, U2 )
          CALL NORM3V( U2 )
          CALL PVEC3V( U1,U2, U3 )
          IF(IB == 1)THEN
            XYZC(1:3) = EndPoint_Align(1:3,1,L)
          ELSEIF( IB.EQ.2 .OR. (IB.EQ.3.AND.NB.EQ.4) )THEN
            XYZC(3) = EndPoint_Align(3,2,LB)
            A = U2(3)*XYZC(3) - DOT_PRODUCT(U2, EndPoint_Align(1:3,1,L))
            B = U3(3)*XYZC(3) - DOT_PRODUCT(U3, EndPoint_Align(1:3,1,L))
            XYZC(1) = -(A*U3(2)-B*U2(2)) / (U2(1)*U3(2)-U3(1)*U2(2))
            XYZC(2) = (-A-U2(1)*XYZC(1)) / U2(2)
          ELSE
            XYZC(1:3) = EndPoint_Align(1:3,2,L)
          ENDIF
          VV1( 1: 3)=XYZC(1:3)+DXBOX*U1(1:3)+DYBOX*U2(1:3)+DZBOX*U3(1:3)
          VV1( 4: 6)=XYZC(1:3)+DXBOX*U1(1:3)-DYBOX*U2(1:3)+DZBOX*U3(1:3)
          VV1( 7: 9)=XYZC(1:3)+DXBOX*U1(1:3)-DYBOX*U2(1:3)-DZBOX*U3(1:3)
          VV1(10:12)=XYZC(1:3)+DXBOX*U1(1:3)+DYBOX*U2(1:3)-DZBOX*U3(1:3)
          VV2( 1: 3)=XYZC(1:3)-DXBOX*U1(1:3)+DYBOX*U2(1:3)+DZBOX*U3(1:3)
          VV2( 4: 6)=XYZC(1:3)-DXBOX*U1(1:3)-DYBOX*U2(1:3)+DZBOX*U3(1:3)
          VV2( 7: 9)=XYZC(1:3)-DXBOX*U1(1:3)-DYBOX*U2(1:3)-DZBOX*U3(1:3)
          VV2(10:12)=XYZC(1:3)-DXBOX*U1(1:3)+DYBOX*U2(1:3)-DZBOX*U3(1:3)
          CAR8 = 'AlEb'//CAR4
          IF( JFF1C(L,IB) <= 0 ) THEN
            CALL NEWMAT( CAR8, MATALI1, JFF, JZZL )
            CALL PLAMATXX( Fali, 4, VV1 )
            CALL PLAMATXX( Fali, 4, VV2 )
            CALL CLOMAT
            CALL ENDMAT
            JFF1C(L,IB) = JFF
            IV1C(L,IB)  = NVMATI
          ELSE
            ITRANS = JFF - JFF1C(L,IB) + 1
            CALL COPYMAT( IV1C(L,IB), ITRANS, CAR8,MATALI1,JFF,JZZL )
          ENDIF
          XYZCBOX(1:3,IB) = XYZC(1:3)
         ENDDO

250      CONTINUE
!
!- Define alignement tubes
         IF(Name_Align(L)(1:1) == 'B') CAR8 = 'AlBt'//CAR4
         IF(Name_Align(L)(1:1) == 'E') CAR8 = 'AlEt'//CAR4
         ROAL = Diameter_Align(L)/2.D0
         RIAL = Diameter_Align(L)/2.D0 - Thicknes_Align(L)
         EPSX = 0.2
         DO IB=1,NB-1
           IF( JFF1D(L,IB) <= 0 ) THEN
             CALL NEWMAT( CAR8, MATALI1, JFF, JZZL )
             CALL DVPIPE( Fali, 12, Zero,Z360,                  &
               XYZCBOX(1,IB  )+(DXBOX+EPSX)*U1(1),              &
               XYZCBOX(2,IB  )+(DXBOX+EPSX)*U1(2),              &
               XYZCBOX(3,IB  )+(DXBOX+EPSX)*U1(3), ROAL,RIAL,   &
               XYZCBOX(1,IB+1)-(DXBOX+EPSX)*U1(1),              &
               XYZCBOX(2,IB+1)-(DXBOX+EPSX)*U1(2),              &
               XYZCBOX(3,IB+1)-(DXBOX+EPSX)*U1(3), ROAL,RIAL )
             CALL ENDMAT
             JFF1D(L,IB) = JFF
             IV1D(L,IB)  = NVMATI
           ELSE
             ITRANS = JFF - JFF1D(L,IB) + 1
             CALL COPYMAT( IV1D(L,IB), ITRANS, CAR8,MATALI1,JFF,JZZL )
           ENDIF
         ENDDO
!
300    ENDDO
!---------------------------------------------------------------------------
!
!
!---------------------------------------------------------------------------
!- Define alignement bars as new volumes
      DO L=Nb_Align_Tub+1,Nb_Align_Tub+Nb_Align_Bar
        IF( I10JFF( Lfi_Align(L) , 9-JFF ) <= 0  )  CYCLE
        LL = L - Nb_Align_Tub
        JZZL = (LL+2) / 3
        WRITE(CAR4,'(I4)') LL
        CAR8 = 'AlEc'//CAR4
        IF( JFF1E(LL) <= 0 ) THEN
          CALL NEWMAT( CAR8, MATALI2, JFF, JZZL )
          ROAL = Diameter_Align(L)/2.D0
          RIAL = Diameter_Align(L)/2.D0 - Thicknes_Align(L)
          CALL DVPIPE( Fali,  12, Zero,Z360,    &
            EndPoint_Align(1,1,L),              &
            EndPoint_Align(2,1,L),              &
            EndPoint_Align(3,1,L), ROAL,RIAL,   &
            EndPoint_Align(1,2,L),              &
            EndPoint_Align(2,2,L),              &
            EndPoint_Align(3,2,L), ROAL,RIAL )
          CALL ENDMAT
          JFF1E(LL) = JFF
          IV1E(LL)  = NVMATI
        ELSE
          ITRANS = JFF - JFF1E(LL) + 1
          CALL COPYMAT( IV1E(LL), ITRANS, CAR8,MATALI1,JFF,JZZL )
        ENDIF
      ENDDO
!---------------------------------------------------------------------------
!
!
!---------------------------------------------------------------------------
!- Define axial Barrel alignment tubes and boxes as new volumes
      DO 500 L=Nb_Align_Tub+Nb_Align_Bar+1,Nb_Align_Tub+Nb_Align_Bar+Nb_Align_Axl
        IF( I10JFF( Lfi_Align(L) , 9-JFF ) <= 0 ) CYCLE
        LL = L - Nb_Align_Tub - Nb_Align_Bar
        JZZL = (LL+2) / 3
        WRITE(CAR4,'(I4)') L-Nb_Align_Tub-Nb_Align_Bar
!
!- Define axial alignement boxes
        IF(MOD(L,2) == 0) GOTO 450

        ZBOX      =  0.
        OFFSETBOX =  4.
        DXBOX     =  7.
        DYBOX     =  6.
        LBOX      = 20.
        DX0(1)= DXBOX
        DY0(1)= DYBOX
        DX0(2)=-DXBOX
        DY0(2)= DYBOX
        DX0(3)=-DXBOX
        DY0(3)=-DYBOX
        DX0(4)= DXBOX
        DY0(4)=-DYBOX
        DS = 1.
        IF(EndPoint_Align(1,1,L) < 0.)  DS = -1.
        DO I=1,4
          IF(Name_Align(L)(3:3) == 'S' .OR. Name_Align(L)(3:3) == 'F') THEN
            DX(I)=    COPIS8*DX0(I)+DS*SIPIS8*DY0(I)
            DY(I)=-DS*SIPIS8*DX0(I)+   COPIS8*DY0(I)
          ELSE
            DX(I)= DX0(I)
            DY(I)= DY0(I)
          ENDIF
        ENDDO
        DO I=1,4
          IF(Name_Align(L)(3:3) == 'S' .OR. Name_Align(L)(3:3) == 'F') THEN
            XR(I)= ( EndPoint_Align(1,1,L)+EndPoint_Align(1,1,L+1) )/2. + DX(I) - DS*SIPIS8 * OFFSETBOX
            YR(I)= ( EndPoint_Align(2,1,L)+EndPoint_Align(2,1,L+1) )/2. + DY(I) - COPIS8 * OFFSETBOX
          ELSE
            XR(I)= ( EndPoint_Align(1,1,L)+EndPoint_Align(1,1,L+1) )/2. + DX(I)
            YR(I)= ( EndPoint_Align(2,1,L)+EndPoint_Align(2,1,L+1) )/2. + DY(I) - OFFSETBOX
          ENDIF
        ENDDO
        VV(1:12) = (/ (XR(I),YR(I),Zero, I=1,4) /)
        JTYP = JTROU3(Name_Align(L),MTYP,CARTYP)
        NBOX =  0
        JMIN =  1
        JMAX = 10
        DO JZZ=JMIN,JMAX
          JFFF=JFF
          IF( Name_Align(L) == 'BOS' .AND. JFF == 6) JFFF=JFF-1
          IF( Name_Align(L) == 'BIL' .AND. JFF == 7) JFFF=JFF-1
          IF( Name_Align(L) == 'BMF' .AND. JFF == 8) JFFF=JFF-1
          IF( Name_Align(L) == 'BOF' .AND. JFF == 8) JFFF=JFF-1
          IF( Pos_Z(JTYP,JFFF,JZZ) > 0.D0 ) THEN
            ZBOX(JZZ)=Pos_Z(JTYP,JFFF,JZZ)/10.D0
            NBOX=NBOX+1
          ENDIF
        ENDDO

        IF( Name_Align(L) == 'BIL' ) THEN
          IF(JFF <= 5.OR.JFF == 7)ZBOX(6)=Pos_Z(JTYP,JFF,6)/10.D0
          IF(JFF == 7)THEN
            ZBOX(6)=ZBOX(6)+15.
            ZBOX(7)=510.
            NBOX=NBOX+1
          ENDIF
        ELSEIF( Name_Align(L) == 'BMS' ) THEN
          ZBOX(3)=ZBOX(3)+20.
          ZBOX(5)=ZBOX(5)+20.
          ZBOX(7)=310.
          ZBOX(8)=660.
          NBOX=NBOX+2
        ELSEIF( Name_Align(L) == 'BIR' ) THEN
          ZBOX(2)=ZBOX(2)+10.
          ZBOX(5)=ZBOX(5)+10.
          ZBOX(7)=138.5
          ZBOX(8)=480.5
          NBOX=NBOX+2
        ELSEIF( Name_Align(L) == 'BOF' ) THEN
          ZBOX(2)=ZBOX(2)+10.
          ZBOX(3)=ZBOX(3)+10.
          ZBOX(4)=ZBOX(4)+10.
          ZBOX(5)=260.
          ZBOX(6)=580.
          ZBOX(7)=860.
          NBOX=NBOX+3
       ELSEIF( Name_Align(L) == 'BMF' ) THEN
          ZBOX(2)=ZBOX(2)+10.
          ZBOX(3)=ZBOX(3)+10.
          ZBOX(4)=260.
          ZBOX(5)=580.
          NBOX=NBOX+2
        ENDIF
        NMIN=1
        NMAX=NBOX+1
        IF( Name_Align(L) == 'BML' ) NMAX=7
        IF( Name_Align(L) == 'BIS' ) NMAX=8

        IF(Name_Align(L)(3:3) == 'S' .OR. Name_Align(L)(3:3) == 'F') THEN
          ZBOX(NMIN)=EndPoint_Align(3,2,L+1)
          ZBOX(NMAX)=EndPoint_Align(3,1,L)
        ELSE
          ZBOX(NMIN)=EndPoint_Align(3,2,L)
          ZBOX(NMAX)=EndPoint_Align(3,1,L+1)
        ENDIF

        CALL CROIST(ZBOX,NMAX)

        CAR8 = 'AlAb'//CAR4
        CALL NEWMAT( CAR8, MATALI3, JFF, JZZL )
        CALL DVGENE(Fali,4,VV,3,ZBOX(NMIN)-LBOX/4.,ZBOX(NMIN)+LBOX/4.)
        DO JZZ=NMIN+1,NMAX-1
          CALL DVGENE(Fali,4,VV,3,ZBOX(JZZ)-LBOX/2.,ZBOX(JZZ)+LBOX/2.)
        ENDDO
        CALL DVGENE(Fali,4,VV,3,ZBOX(NMAX)-LBOX/4.,ZBOX(NMAX)+LBOX/4.)
        CALL ENDMAT

450     CONTINUE
!
!- Define axial alignement tubes
        ROAL = Diameter_Align(L)/2.D0
        RIAL = Diameter_Align(L)/2.D0 - Thicknes_Align(L)
        IF(MOD(L,2) == 0)THEN
          NTUBEMIN=NMIN+1
          NTUBEMAX=NMAX-1
        ELSE
          NTUBEMIN=NMIN
          NTUBEMAX=NMAX-2
        ENDIF
        EPSZ=0.0D0
        DO JZZ=NTUBEMIN,NTUBEMAX
          CAR8 = 'AlAt'//CAR4
          CALL NEWMAT( CAR8, MATALI3, JFF, JZZL )
          DBOXA=LBOX/2.
          DBOXB=LBOX/2.
          IF(JZZ == NTUBEMIN .AND. MOD(L,2) /= 0)DBOXA=LBOX/4.
          IF(JZZ == NTUBEMAX .AND. MOD(L,2) == 0)DBOXB=LBOX/4.
          CALL DVPIPE( Fali,  8, Zero,Z360,      &
            EndPoint_Align(1,1,L),               &
            EndPoint_Align(2,1,L),               &
            ZBOX(JZZ)+DBOXA+EPSZ, ROAL,RIAL,     &
            EndPoint_Align(1,2,L),               &
            EndPoint_Align(2,2,L),               &
            ZBOX(JZZ+1)-DBOXB-EPSZ, ROAL,RIAL )
          CALL ENDMAT
        ENDDO
500    ENDDO
!
900   ENDDO
!
!- Copy volumes to perform Z+/Z- symetry
      IVMFIN_Z = NVMATI
      CALL SYZMAT(IVMDEB_Z,IVMFIN_Z)
!
 END SUBROUTINE MATALIGNEMENT
!
!
!> \author M.Virchaux
!
 PURE INTEGER FUNCTION I10JFF(NN,II)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NN, II
 INTEGER :: N1, N2
!
   N1 = NN / 10**II
   N2 = NN / 10**(II-1)
   I10JFF = N2 - 10*N1
!
 END FUNCTION I10JFF
!

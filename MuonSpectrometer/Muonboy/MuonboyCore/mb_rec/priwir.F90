!
!> \author M.Virchaux
!
 SUBROUTINE PRIWIR
 USE M_MB_Control
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER :: ISC, ILOHI, IC, INOF, INOZ, IGEOM, IMDT, IP, LOCZS, IRPC
 INTEGER :: IS, IZMA, IZ, ILIN, MMMM, ICT, JTYP, JFF, JZZ, JOB, IS0
 REAL(8) :: SIGZ, DIREC, TTT, ZZT, S0P, S0N, S00, SST
 REAL(8) :: ZSLOC(800)
!
   DO ISC=1,NMATTR
     DO 302 ILOHI=1,2
       IC = (ISC-1)*2 + ILOHI
       DO 301 INOF=1,8
         DO 300 INOZ=1,NZM
           IGEOM = IGEOMA(IC,INOF,INOZ)
           SIGZ  = 1.D0
           IF( IC <= NCOUBA .AND. INOZ < NZMS2 ) SIGZ = MDTnu
           IF( ZLOCHAV(IGEOM) < 1.D0 )  CYCLE
           PRINT 1000
1000       FORMAT(/'  IC    Type    InoF InoZ Ilay    Phi  ',    &
                   '        T    Nb.of wires        Z of wires')
           IMDT = ISTMDTV(IGEOM)
           DIREC = 1.D0
           IF(TCECHAV(IGEOM) < 0.D0) DIREC = -1.D0
           DO 250 IP=1,NLTUB(IMDT)
             TTT = TCECHAV(IGEOM) + DIREC*TLTUB(IMDT,IP,1)
             LOCZS = 0
             IZMA = NINT( ZLOCHAV(IGEOM) * PASTUB_inv(IMDT) )
             DO IZ=0,IZMA+1
               ZZT = ZCECHAV(IGEOM) + SIGZ*( OLTUB(IMDT,IP,1) + IZ*PASTUB(IMDT) - ZLOCHAV(IGEOM)/2.D0 )
               IF( ABS(ZZT-ZCECHAV(IGEOM)-0.1000D0) > ZLOCHAV(IGEOM)/2.D0 )  CYCLE
               LOCZS = LOCZS + 1
               ZSLOC(LOCZS) = ZZT
             ENDDO
             IF(IP == 1) PRINT 2000,IC,CARISC(ISC),CALOHI(ILOHI),      &
                                    INOF,INOZ,IP,FCECHAV(IGEOM),TTT,   &
                                    LOCZS,ZSLOC(1:3),ZSLOC(LOCZS-2:LOCZS)
             IF(IP /= 1) PRINT 3000,IP,TTT,LOCZS,ZSLOC(1:3),ZSLOC(LOCZS-2:LOCZS)
250        ENDDO
           ILIN = ISTLINV(IGEOM)
           MMMM = MIN( 12 , NBLINK(ILIN) )
           PRINT 4100,NBLINK(ILIN),ICTRIG(1:MMMM,ILIN)
           IF( NBLINK(ILIN) >= 13 ) THEN
             MMMM = MIN( 24 , NBLINK(ILIN) )
             PRINT 4200,ICTRIG(13:MMMM,ILIN)
           ENDIF
300      ENDDO
301    ENDDO
302  ENDDO
   ENDDO
!
2000 FORMAT(I4,2X,A3,1X,A5,I4,2I5,F10.4,F12.4,I8,F12.4,2F10.4,'  ... ',3F10.4)
3000 FORMAT(24X,I5,10X,F12.4,I8,F12.4,2F10.4,'  ... ',3F10.4)
4100 FORMAT('  Nb of linked trigger chambers =',I3,'  ICTs =',12I5)
4200 FORMAT(44X,12I5)
!
   DO ICT=1,NTRITO
     PRINT 4500
     S0P = ( SPITRI(ICT) + SPATRI(ICT) ) / 2.D0
     S0N = ( SNITRI(ICT) + SNATRI(ICT) ) / 2.D0
     S00 = ( S0P + S0N ) / 2.D0
     CALL IDBTRI(ICT, JTYP,JFF,JZZ,JOB)
     IRPC = IRPTGC(ICT)
     DIREC = 1.D0
     IF(TCETRI(ICT).LT.0.D0) DIREC = -1.D0
     DO 350 IP=1,N1BAND(IRPC)
       TTT = TCETRI(ICT) + DIREC*TBAND(IRPC,IP)
       LOCZS = 0
       IS0 = (S0P-OBAND(IRPC,IP,1)-S00) / DSBAND(IRPC)
       DO IS=0,400
         IF( IS == IS0 )  CYCLE
         IF( ICT.LE.NTRIBA ) THEN
           SST = S0N + OBAND(IRPC,IP,1) + IS*DSBAND(IRPC)
           IF( IS > IS0 .AND. OBAND(IRPC,IP,2) > -900.D0 )    &
           SST = S00 + OBAND(IRPC,IP,2) + (IS-IS0)*DSBAND(IRPC)
         ELSE
           SST = S0N + OBAND(IRPC,IP,1) + IS*DSBAND(IRPC)
         ENDIF
         IF( SST < MIN(SNITRI(ICT),SNATRI(ICT)) .OR.   &
             SST > MAX(SPITRI(ICT),SPATRI(ICT)) )   CYCLE
         LOCZS = LOCZS + 1
         ZSLOC(LOCZS) = SST
       ENDDO
       IF(IP == 1) PRINT 4300,ICT,JTYP,JFF,JZZ,JOB,IP, FCETRI(ICT),TTT,    &
                         LOCZS,ZSLOC(1:3),ZSLOC(LOCZS-2:LOCZS)
       IF(IP /= 1) PRINT 4400,IP,TTT,LOCZS,ZSLOC(1:3),ZSLOC(LOCZS-2:LOCZS)
350  ENDDO
     PRINT 5000
     DO 450 IP=N1BAND(IRPC)+1,N2BAND(IRPC)
       TTT = TCETRI(ICT) + DIREC*TBAND(IRPC,IP)
       LOCZS = 0
       DO IZ=0,400
         IF( ICT <= NTRIBA .AND. ZCETRI(ICT) < -3.d0 ) THEN  ! -3. needed for BOG0
           ZZT = ZCETRI(ICT) + ZLOTRI(ICT)/2.D0 - OBAND(IRPC,IP,1) - IZ*DZBAND(IRPC)
         ELSE
           ZZT = ZCETRI(ICT) - ZLOTRI(ICT)/2.D0 + OBAND(IRPC,IP,1) + IZ*DZBAND(IRPC)
!.... special case of CSC's wire gangs to be coded ...
         ENDIF
         IF( ABS(ZZT-ZCETRI(ICT)-0.1000D0) > ZLOTRI(ICT)/2.D0 )  CYCLE
         LOCZS = LOCZS + 1
         ZSLOC(LOCZS) = ZZT
       ENDDO
       PRINT 4400,IP,TTT,LOCZS,ZSLOC(1:3),ZSLOC(LOCZS-2:LOCZS)
450  ENDDO
   ENDDO
!
4500 FORMAT('  Ict   Jtyp Jff Jzz Job  Jsl',    &
            '    Phi         T    Nb.of S strips      S of strips')
4300 FORMAT(I5,I6,3I4,I5,F10.4,F12.4,I8,F12.4,2F10.4,'  ... ',3F10.4)
4400 FORMAT(23X,I5,10X,F12.4,I8,F12.4,2F10.4,'  ... ',3F10.4)
5000 FORMAT(50X,'Nb.of Z strips      Z of strips')
!
 END SUBROUTINE PRIWIR
!

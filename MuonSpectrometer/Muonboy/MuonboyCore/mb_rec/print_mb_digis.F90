!
!> Print out of digitisation module M_MB_Digis
!> \author M.Virchaux
!
 SUBROUTINE Print_MB_Digis
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER :: IC, ISC, ILOHI, IW, INOF, INOZ, IGEOM, IMDT, ILIN, IL
 INTEGER :: ICT, IRPC, IP, ISDEB, ISFIN, IS, Icode, IPside
 INTEGER :: ITDEB, ITFIN, IT
 CHARACTER(5) :: ZClustUsed, ZClustNotUsed, SClustUsed, SClustNotUsed
!
   PRINT 2000
!
   DO IC=1,NCOCSC
     IF(NCHAHI(IC) >= 1) THEN
       IF(IC <= NCOUBA) THEN
         PRINT 3500
       ELSE
         PRINT 3600
       ENDIF
     ENDIF
     ISC   = (IC+1)/2
     ILOHI = IC + 2 - 2*ISC
     DO IW=1,NCHAHI(IC)
       INOF = IFCHA(IC,IW)
       INOZ = IZCHA(IC,IW)
#if !defined(ATLAS_NOCHECKMU)
       IF( IWCHA(IC,INOF,INOZ) /= IW ) THEN
         PRINT 7701,IC,IW,INOF,INOZ,IWCHA(IC,INOF,INOZ)
7701     FORMAT(/' IC,IW,INOF,INOZ,IWCHA(IC,INOF,INOZ) =',5I5/' ============> STOP !!!')
         STOP 776
       ENDIF
#endif
       IGEOM = IGEOMA(IC,INOF,INOZ)
       IMDT = ISTMDTV(IGEOM)
       ILIN = ISTLINV(IGEOM)
       PRINT 4500,IC,CARISC(ISC),CALOHI(ILOHI),IW,INOF,INOZ,IGEOM,IMDT,ILIN,NBLINK(ILIN),  &
                  FCECHAV(IGEOM),GCECHAV(IGEOM),TCECHAV(IGEOM),ZCECHAV(IGEOM),STACHAV(IGEOM)
!
       IF(ILIN >= 1) PRINT 3700
       DO IL=1,NBLINK(ILIN)
         ICT  = ICTRIG(IL,ILIN)
         IRPC = IRPTGC(ICT)
         DO IP=1,N2BAND(IRPC)
           ISDEB = NBANH0(ICT,IP) + 1
           ISFIN = NBANH0(ICT,IP) + NBANHI(ICT,IP)
           DO IS=ISDEB,ISFIN
             Icode  = KBANHI(IS) / 10
             IPside = KBANHI(IS) - 10*Icode
             IF(IP <= N1BAND(IRPC)) THEN
               PRINT 5000,IL,ICT,IRPC,IP,IPside,IS,IS-ISDEB+1,SBANHI(IS),TBANHI(IS),JBANHI(IS),IBANHI(IS)
             ELSE
               IF(ICT <= NTRIBA) THEN
                 PRINT 5100,IL,ICT,IRPC,IP,IPside,IS,IS-ISDEB+1,SBANHI(IS),TBANHI(IS),JBANHI(IS),IBANHI(IS)
               ELSE
                 PRINT 5200,IL,ICT,IRPC,IP,IPside,IS,IS-ISDEB+1,SBANHI(IS),TBANHI(IS),JBANHI(IS),IBANHI(IS)
               ENDIF
             ENDIF
           ENDDO
         ENDDO
       ENDDO
!
       IF( ExternalClust == 1 .OR. ExternalClust == -1 ) THEN
         ZClustUsed    = "clust"
         ZClustNotUsed = "recon"
       ELSE
         ZClustUsed    = "recon"
         ZClustNotUsed = "clust"
       ENDIF
       IF( ExternalClust == 1 .OR. ExternalClust == -2 ) THEN
         SClustUsed    = "clust"
         SClustNotUsed = "recon"
       ELSE
         SClustUsed    = "recon"
         SClustNotUsed = "clust"
       ENDIF
!
       IF(IC <= NCOUCH) THEN
         PRINT 3800
       ELSE
         PRINT 3900
       ENDIF
       DO IP=1,NLTUB(IMDT)
         ITDEB = NTUBH0(IC,IW,IP) + 1
         ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
         DO IT=ITDEB,ITFIN
           IF( IC <= NCOUBA ) THEN
             PRINT 5500,IP,IT,IT-ITDEB+1,ZTUBHI(IT),DTUBHI(IT),LTUBHI(IT),ITUBHI(IT)
           ELSE IF( IC <= NCOUCH ) THEN
             PRINT 5600,IP,IT,IT-ITDEB+1,ZTUBHI(IT),DTUBHI(IT),LTUBHI(IT),ITUBHI(IT)
           ELSE IF( RTUBHI(IT) < -10.D0 ) THEN
             PRINT 6100,SClustUsed,IP,IT,IT-ITDEB+1,STUBHI(IT),DTUBHI(IT),LTUBHI(IT),ITUBHI(IT)
           ELSE IF( RTUBHI(IT) < 0.D0 ) THEN
             PRINT 6200,ZClustUsed,IP,IT,IT-ITDEB+1,ZTUBHI(IT),DTUBHI(IT),LTUBHI(IT),ITUBHI(IT)
           ELSE IF( RTUBHI(IT) < 10.D0 ) THEN
             PRINT 6300,IP,IT,IT-ITDEB+1,STUBHI(IT),DTUBHI(IT),LTUBHI(IT),ITUBHI(IT)
           ELSE IF( RTUBHI(IT) < 100.D0 ) THEN
             PRINT 6400,IP,IT,IT-ITDEB+1,ZTUBHI(IT),DTUBHI(IT),LTUBHI(IT),ITUBHI(IT)
           ELSE IF( RTUBHI(IT) < 110.D0 ) THEN
             PRINT 6500,SClustNotUsed,IP,IT,IT-ITDEB+1,STUBHI(IT),DTUBHI(IT),LTUBHI(IT),ITUBHI(IT)
           ELSE
             PRINT 6600,ZClustNotUsed,IP,IT,IT-ITDEB+1,ZTUBHI(IT),DTUBHI(IT),LTUBHI(IT),ITUBHI(IT)
           ENDIF
         ENDDO
       ENDDO
!
     ENDDO
   ENDDO
!
2000 FORMAT(/' Module  M_MB_Digis  :'/'        ************ ')
3500 FORMAT(90('-')/'  Ic    Type      Iw  Inof Inoz Igeo Imdt Ilin',  &
            ' Nblink     Phi0         Alf0         T0           Z0')
3600 FORMAT(90('-')/'  Ic    Type      Iw  Inof Inoz Igeo Imdt Ilin',  &
            ' Nblink     Phi0         Alf0         Z0           T0')
3700 FORMAT(21X,'Il  Ict  Irpc  Ip  IpSide   Is        S/Z/T       Time')
3800 FORMAT(36X,'Ip      It        Z/T      ADCcount')
3900 FORMAT(36X,'Ip      It         z          S         Charge')
4500 FORMAT(I4,2X,A3,1X,A5,7I5,4F13.4,F13.2)
5000 FORMAT(11X,'Strip S',5I5,I8,I3,2F11.4,2I16)
5100 FORMAT(11X,'Strip Z',5I5,I8,I3,2F11.4,2I16)
5200 FORMAT(11X,'Strip T',5I5,I8,I3,2F11.4,2I16)
5500 FORMAT(27X,'Tube Z',I5,I8,I3,2F11.4,2I16)
5600 FORMAT(27X,'Tube T',I5,I8,I3,2F11.4,2I16)
6100 FORMAT(27X,'S*',A5  ,I4,I8,I3,  11X,F11.4,F11.4,2I16)
6200 FORMAT(27X,'z*',A5  ,I4,I8,I3,F11.4,  11X,F11.4,2I16)
6300 FORMAT(27X,'Strip S',I4,I8,I3,  11X,F11.4,F11.2,2I16)
6400 FORMAT(27X,'Strip z',I4,I8,I3,F11.4,  11X,F11.2,2I16)
6500 FORMAT(27X,'S ',A5  ,I4,I8,I3,  11X,F11.4,F11.4,2I16)
6600 FORMAT(27X,'z ',A5  ,I4,I8,I3,F11.4,  11X,F11.4,2I16)
 END SUBROUTINE Print_MB_Digis


!
 SUBROUTINE FILTEM(ICC,IWW)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_COTEMH
 USE M_MB_PACONS, ONLY : DTcut
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: ICC(2), IWW(2)
 INTEGER :: J, IC, IW, INOF, INOZ, INOZMI, INOZMA, IGEOM, IMDT, IP
 INTEGER :: ITDEB, ITFIN, IT, IZ
 REAL(8) :: DIREC, ZOLTUB
 REAL(8) :: InvPASTUB
!
   ITEMC(:) = 0
   NTEMH0(:,:,:) = 0
   NTEMHI(:,:,:) = 0
!
   DO J=1,2
     IF( IWW(J) < 1 ) CYCLE
     IC     = ICC(J)
     IW     = IWW(J)
     INOF   = IFCHA(IC,IW)
     INOZ   = IZCHA(IC,IW)
     DIREC  = SIGN( 1.D0 , TCECHAV(IGEOMA(IC,INOF,INOZ)) )
     INOZMI =  1
     INOZMA = NZM
     IF( IC > NCOUBA ) THEN
       IF( DIREC > 0.D0 ) THEN
         INOZMA = NZMS2
       ELSE
         INOZMI = NZMS2 + 1
       ENDIF
     ENDIF
     ITEMC(3*J-1) = IC
     ITEMF(3*J-1) = INOF
     ITEMZ(3*J-1) = INOZ
     IF( INOZ-1 >= INOZMI ) THEN
       IF( IWCHA(IC,INOF,INOZ-1) >= 1 ) THEN
         ITEMC(3*J-2) = IC
         ITEMF(3*J-2) = INOF
         ITEMZ(3*J-2) = INOZ-1
       ENDIF
     ENDIF
     IF( INOZ+1 <= INOZMA ) THEN
       IF( IWCHA(IC,INOF,INOZ+1) >= 1 ) THEN
         ITEMC(3*J  ) = IC
         ITEMF(3*J  ) = INOF
         ITEMZ(3*J  ) = INOZ+1
       ENDIF
     ENDIF
   ENDDO
!
   DO J=1,6
     IF( ITEMC(J) < 1 ) CYCLE
     IC    = ITEMC(J)
     INOF  = ITEMF(J)
     INOZ  = ITEMZ(J)
     IW    = IWCHA(IC,INOF,INOZ)
     IGEOM = IGEOMA(IC,INOF,INOZ)
     IMDT  = ISTMDTV(IGEOM)
     InvPASTUB = PASTUB_inv(IMDT)
     ITEMF(J)   = INOF
     ITEMZ(J)   = INOZ
     ITEMM(J)   = IMDT
     ITEMA(J)   = ItransCHA(IGEOM)
     LTEMP(J)   = IadDedChan(IGEOM) >= 1 .OR. IadDisChan(IGEOM) >= 1
     ITEMG(J)   = IGEOM
     TEMFF(J)   = FCECHAV(IGEOM)
     TEMTT(J)   = TCECHAV(IGEOM)
     TEMZI(J)   = ZCECHAV(IGEOM) - ZLOCHAV(IGEOM)*0.5d0
     TEMZA(J)   = ZCECHAV(IGEOM) + ZLOCHAV(IGEOM)*0.5d0
     TEMSI(J)   = 1.D0
     SiTEMFF(J) = SINCHAV(IGEOM)
     CoTEMFF(J) = COSCHAV(IGEOM)
     IF( IC <= NCOUBA .AND. INOZ < NZMS2 ) TEMSI(J) = MDTnu
     IF( TEMSI(J) < 0.D0 ) THEN
       TEMZO(J) = TEMZA(J)
     ELSE
       TEMZO(J) = TEMZI(J)
     ENDIF
     DO IP=1,NLTUB(IMDT)
       ZOLTUB = TEMZO(J) + TEMSI(J)*OLTUB(IMDT,IP,1)
       ITDEB = NTUBH0(IC,IW,IP) + 1
       ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
       DO IT=ITDEB,ITFIN
         IZ = NINT( TEMSI(J)*(ZTUBHI(IT)-ZOLTUB) * InvPASTUB )
         IF( ABS(ZTUBHI(IT)-ZOLTUB-TEMSI(J)*IZ*PASTUB(IMDT)) > 0.1D0 .OR.   &
             IZ.LT.-10 .OR. IZ.GT.100 ) THEN
           PRINT 7700,IZ,IT,ZTUBHI(IT),ZOLTUB+TEMSI(J)*IZ*PASTUB(IMDT)
7700       FORMAT(//' In FILTEM : IZ,IT,ZTUBHI,Z... =',I5,I8,2F12.4,'  =====>  STOP !')
#if defined(ATLAS_PERSINT)
           CALL MboyToPersint('FILTEM')
#else
           STOP
#endif
         ENDIF
         IF( NTEMHI(J,IP,IZ) < 1 ) THEN
           NTEMH0(J,IP,IZ) = IT - 1
           NTEMHI(J,IP,IZ) = 1
         ELSE
           NTEMHI(J,IP,IZ) = NTEMHI(J,IP,IZ) + 1
           IF( IT /= NTEMH0(J,IP,IZ)+NTEMHI(J,IP,IZ) ) THEN
             PRINT 7701,J,IP,IZ,IT,NTEMH0(J,IP,IZ)+NTEMHI(J,IP,IZ)
7701         FORMAT(//' In FILTEM : J,IP,IZ,IT,NTEMH0+NTEMHI =',3I5,2I8,'  =====>  STOP !')
             STOP
           ENDIF
         ENDIF
       ENDDO
     ENDDO
   ENDDO
!
! no segment between two consecutive stations if not at the same T position
   IF( ITEMC(1) > 0 .AND. ABS(TEMTT(1)-TEMTT(2)) > DTcut ) ITEMC(1) = 0
   IF( ITEMC(3) > 0 .AND. ABS(TEMTT(3)-TEMTT(2)) > DTcut ) ITEMC(3) = 0
   IF( ITEMC(4) > 0 .AND. ABS(TEMTT(4)-TEMTT(5)) > DTcut ) ITEMC(4) = 0
   IF( ITEMC(6) > 0 .AND. ABS(TEMTT(6)-TEMTT(5)) > DTcut ) ITEMC(6) = 0
!
 END SUBROUTINE FILTEM
!

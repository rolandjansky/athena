!
!> Create a new trigger (RPC or TGC) station
!> \author M.Virchaux
!
 SUBROUTINE CRERPC(N1BABA,N2BABA,DSBABA,DZBABA,RABABA,  &
                   OBABA1,OBABA2,TBABA, NSZBABA,SZBABA, NRPCTO,JRPC)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: N1BABA, N2BABA
 INTEGER, INTENT(IN)    :: NSZBABA(N6M)
 REAL(8), INTENT(IN)    :: DSBABA, DZBABA, RABABA
 REAL(8), INTENT(IN)    :: OBABA1(N6M),OBABA2(N6M),TBABA(N6M)
 REAL(8), INTENT(IN)    :: SZBABA(200,N6M)
 INTEGER, INTENT(INOUT) :: NRPCTO
 INTEGER, INTENT(OUT)   :: JRPC
 INTEGER :: L, K, I
!
   IF( N2BABA <= 0 ) THEN
     JRPC = 0
     RETURN
   ELSE
     DO 50 L=1,NRPCTO
       IF( N1BABA /= N1BAND(L) .OR. N2BABA /= N2BAND(L) )              CYCLE
       IF( ABS(DSBABA-DSBAND(L)) > 0.0010D0 )                          CYCLE
       IF( ABS(DZBABA-DZBAND(L)) > 0.0010D0 )                          CYCLE
       IF( ABS(RABABA-RABAND(L)) > 0.0010D0 )                          CYCLE
       DO K=1,N2BABA
         IF( ABS(OBABA1(K)-OBAND(L,K,1)) > 0.0010D0 )                  GO TO 50
         IF( ABS(OBABA2(K)-OBAND(L,K,2)) > 0.0010D0 )                  GO TO 50
         IF( ABS(TBABA (K)-TBAND(L,K))   > 0.0010D0 )                  GO TO 50
         IF( NSZBABA(K) == 0 .AND. ISZBAND(L,K,2) == 0 ) CYCLE
         IF( NSZBABA(K) /= ISZBAND(L,K,2)-ISZBAND(L,K,1)+1 )           GO TO 50
         DO I=1,NSZBABA(K)
          IF( ABS(SZBABA(I,K)-SZBAND(ISZBAND(L,K,1)+I-1)) > 0.0010D0 ) GO TO 50
         ENDDO
       ENDDO
       JRPC = L
       RETURN
50   CONTINUE
     NRPCTO = NRPCTO + 1
     IF( NRPCTO > NRPCMA ) THEN
       WRITE(*,'(/" In CRERPC : Total number of RPC types is too large ======> STOP !!!")')
       STOP
     ENDIF
     N1BAND(NRPCTO) = N1BABA
     N2BAND(NRPCTO) = N2BABA
     DSBAND(NRPCTO) = DSBABA
     DZBAND(NRPCTO) = DZBABA
     RABAND(NRPCTO) = RABABA
     DO K=1,N2BABA
       OBAND (NRPCTO,K,1)   = OBABA1(K)
       OBAND (NRPCTO,K,2)   = OBABA2(K)
       TBAND (NRPCTO,K)     = TBABA(K)
       ISZBAND(NRPCTO,K,1:2) = 0
       IF( NSZBABA(K) > 0 ) THEN
         ISZBAND(NRPCTO,K,1) = NSZBANDtot + 1
         SZBAND(NSZBANDtot+1:NSZBANDtot+NSZBABA(K)) = SZBABA(1:NSZBABA(K),K)
         NSZBANDtot = NSZBANDtot + NSZBABA(K)
         IF( NSZBANDtot > NSZBAMA ) THEN
           WRITE(*,'(/" In CRERPC : Total number TGC wire gangs and strips is too large ======> STOP !!!")')
           STOP
         ENDIF
         ISZBAND(NRPCTO,K,2) = NSZBANDtot
       ENDIF
     ENDDO
     JRPC = NRPCTO
     RETURN
   ENDIF
!
 END SUBROUTINE CRERPC
!

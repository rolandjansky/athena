!
!> same as noktri.F90, but taking the inactive region into account
!> (uses some assumption on geometry, checked in ConsistencyCheck)
!> We use only the positions of the first (ip=1, for S) and last 
!> (ip=N2BAND(irpc)), for Z) layers.
!
 LOGICAL FUNCTION NAKTRI(ict,ip,zz,ss,tolz,tols)
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: ict, ip
 REAL(8), INTENT(IN) :: zz, ss, tolz, tols
 REAL(8) :: tolzB, tolsB, s0, s0p, d0p 
 INTEGER :: irpc, ips, ipz
 LOGICAL :: NOKTRI
!
#if defined(ATLAS_STAND)
 INTEGER :: icta, ipa
 REAL(8) :: s0n, d0n
 LOGICAL, SAVE :: first=.true.
!
   IF( first ) THEN  ! check some assumptions on geometry which simplify the computation
     first = .false.
     DO icta=NTRIBA+1,NTRITO
       irpc = IRPTGC(icta)
       s0n  = 0.5d0 * ( SNITRI(icta) + SNATRI(icta) )
       s0p  = 0.5d0 * ( SPITRI(icta) + SPATRI(icta) )
       DO ipa=1,N2BAND(irpc)
         IF( ISZBAND(irpc,ipa,2) > 0 ) THEN
           IF( ipa <= N1BAND(irpc) ) THEN
             d0n = szband(iszband(irpc,ipa,1))
             d0p = szband(iszband(irpc,ipa,2))
             IF( abs(s0p+s0n) > 0.001d0 .OR. abs(d0p+d0n) > 0.001d0 .OR. s0p < s0n .OR. d0p < d0n ) THEN
               WRITE(*,*) ' probleme s0,d0 NAKTRI ',s0n,s0p,d0n,d0p
               STOP
             ENDIF
           ELSE
             d0n = OBAND(irpc,ipa,1) + SZBAND(ISZBAND(irpc,ipa,1))
             d0p = ZLOTRI(icta) - SZBAND(ISZBAND(irpc,ipa,2)) - d0n
             IF( abs(d0p-d0n) > 0.001d0 ) THEN
               WRITE(*,*) ' probleme d0 NAKTRI ',d0n,d0p
               STOP
             ENDIF
           ENDIF
         ENDIF
       ENDDO
     ENDDO
!
     DO icta=1,NTRITO
       irpc = IRPTGC(icta)
       IF( N2BAND(irpc) /= 2*N1BAND(irpc) ) THEN
         WRITE(*,*) ' probleme ip NAKTRI ',N1BAND(irpc),N2BAND(irpc)
         STOP
       ENDIF
     ENDDO
   ENDIF
#endif
!
   irpc = IRPTGC(ict)
   IF( ip == 0 ) THEN
     ips = 1
     ipz = N2BAND(irpc)
   ELSE IF( ip <= N1BAND(irpc) ) THEN
     ips = ip
     ipz = ip + N1BAND(irpc)
   ELSE
     ips = ip - N1BAND(irpc)
     ipz = ip
   ENDIF
   IF( ips <= 0 .or. ips > N1BAND(irpc) .or. ipz <= N1BAND(irpc) .or. ipz > N2BAND(irpc) ) THEN
     write(*,*) ' probleme NAKTRI ',ip,ips,ipz,N1BAND(irpc),N2BAND(irpc)
     stop
   ENDIF
!
   tolzb = tolz
   tolsb = tols
!
   IF( ict <= NTRIBA ) THEN       ! RPC
     tolzb = tolz - OBAND(irpc,ipz,1) - DZBAND(irpc)/2.d0
     tolsb = tols - OBAND(irpc,ips,1) - DSBAND(irpc)/2.d0
     IF( OBAND(irpc,ips,2) > -900.d0 ) THEN
       s0 = ( SNITRI(ict) + SNATRI(ict) + SPITRI(ict) + SPATRI(ict) ) / 4.d0
       IF( abs(ss-s0) < OBAND(irpc,ips,2) + DSBAND(irpc)/2.d0 - tols ) THEN
         NAKTRI = .true.
         RETURN
       ENDIF
     ENDIF
   ELSE                           ! TGC
     IF( ISZBAND(irpc,ips,2) <= 0 ) THEN   ! standard case
       tolsb = tols - OBAND(irpc,ips,1) - DSBAND(irpc)/2.d0
     ELSE                                ! variable width
       s0p = 0.5d0 * ( SPITRI(ict) + SPATRI(ict) )
       d0p = szband(iszband(irpc,ips,2))
       tolsb = tols - ( abs(s0p) - abs(d0p) )
     ENDIF
     IF( ISZBAND(irpc,ipz,2) <= 0 ) THEN   ! standard case
       tolzb = tolz - OBAND(irpc,ipz,1) - DZBAND(irpc)/2.d0
     ELSE                                ! variable width
       tolzb = tolz - OBAND(irpc,ipz,1) - SZBAND(ISZBAND(irpc,ipz,1))
     ENDIF
   ENDIF
!
   NAKTRI = NOKTRI(ict,zz,ss,tolzb,tolsb)
!
 END FUNCTION NAKTRI
!

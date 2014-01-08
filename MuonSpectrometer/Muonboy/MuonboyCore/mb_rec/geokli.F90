!
 SUBROUTINE GEOKLI
 USE M_MB_Control
 USE M_MB_General_CONSTANTS
 USE M_MB_MuGeom
 USE M_MB_CODRAT
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER :: NN, ISC, ID, IPIRANGE
 REAL(8) :: FFFIXE, EEFIXE, TTTT, ZZZZ, SSSS, RRRR, FFFF
 REAL(8) :: AFFMAX, AEEMAX, AFFDIF, AEEDIF, FFCUR, EECUR
 REAL(8) :: PIMIPI, PITOPI
 INTEGER :: NGoodTrCommonHits
 INTEGER :: JFF
 integer :: idf, iscf, iscl
 real(8) :: dtf, dzf, dtl, dzl, dt0, dz0, sil, sif, anf, anl, an0
 logical :: isemeo
!
   NBOKLI = 0
   DO NN=1,NBLIBR
     ISC = ISCLIBR(NN)
     DO ID=1,NBDRAT(ISC)
       IDOKLI(ID,ISC) = 0
     ENDDO
   ENDDO
!
! Find (Eta,Phi) region of the tracking
   IF( NBFIXE < 1 ) RETURN
   FFFIXE = 0.D0
   EEFIXE = 0.D0
   IPIRANGE = 2
   DO NN=1,NBFIXE
     ISC = ISCFIXE(NN)
     IF( IWcross(ISC,1)+IWcross(ISC,2) > 0 ) CYCLE
     ID  = IDFIXE(NN)
     TTTT = (TTDRAT(ID,1,ISC)+TTDRAT(ID,2,ISC)) * 0.5d0
     ZZZZ = (ZZDRAT(ID,1,ISC)+ZZDRAT(ID,2,ISC)) * 0.5d0
     SSSS = (SSDRAT(ID,1,ISC)+SSDRAT(ID,2,ISC)) * 0.5d0
     RRRR = SQRT( TTTT**2 + SSSS**2 )
     FFFF = FFDRAT(ID,ISC) + ATAN2(SSSS,TTTT)
     IF( NN == 1 ) THEN
       IF( ABS(PIMIPI(FFFF)) > Pi/2.D0 ) IPIRANGE = 0
     ENDIF
     IF( IPIRANGE < 1 ) THEN
       FFFIXE = FFFIXE + PITOPI(FFFF)
     ELSE
       FFFIXE = FFFIXE + PIMIPI(FFFF)
     ENDIF
     EEFIXE = EEFIXE + ATAN2(RRRR,ZZZZ)
   ENDDO
   FFFIXE = FFFIXE / NBFIXE
   FFFIXE = PIMIPI( FFFIXE )
   EEFIXE = EEFIXE / NBFIXE
!
! and counts (and flags) the candidate segments in this region
   DO NN=1,NBLIBR
     ISC = ISCLIBR(NN)
     IF( IWcross(ISC,1)+IWcross(ISC,2) > 0 ) THEN
       IDOKLI(1,ISC) = 1
       NBOKLI = NBOKLI + 1
     ELSE
       IF( ISC <= NCOUBAS2 ) THEN
         AFFMAX = AFFMAXbarrel
         AEEMAX = AEEMAXbarrel
       ELSE
         AFFMAX = AFFMAXendcap
         AEEMAX = AEEMAXendcap
       ENDIF
       DO ID=1,NBDRAT(ISC)
         TTTT   = (TTDRAT(ID,1,ISC)+TTDRAT(ID,2,ISC)) * 0.5d0
         ZZZZ   = (ZZDRAT(ID,1,ISC)+ZZDRAT(ID,2,ISC)) * 0.5d0
         SSSS   = (SSDRAT(ID,1,ISC)+SSDRAT(ID,2,ISC)) * 0.5d0
         RRRR   = SQRT( TTTT**2 + SSSS**2 )
         EECUR  = ATAN2(RRRR,ZZZZ )
         AEEDIF = ABS( EECUR - EEFIXE )
         IF( AEEDIF > AEEMAX ) CYCLE
         FFFF   = FFDRAT(ID,ISC) + ATAN2(SSSS,TTTT)
         FFCUR  = PIMIPI(FFFF)
         AFFDIF = ABS( FFCUR - FFFIXE )
         AFFDIF = MIN( AFFDIF , ABS(AFFDIF-TWOPI) )
         IF( AFFDIF > AFFMAX ) CYCLE
! check if digits from this segment are not already used by a "very good" track
         IF( NGoodTrCommonHits(NUDRAT(ID,ISC), IUDRAT(1:NUDRAT(ID,ISC),ID,ISC)) >= 1 ) CYCLE
! check if in compatible sector
         IF( AtCosm == 0 ) THEN
           IF( isLarge(ISC) .EQV. isLarge(ISCFIXE(1)) ) THEN
             IF( JFDRAT(ID,ISC) /= JFDRAT(IDFIXE(1),ISCFIXE(1)) ) CYCLE
           ELSE
             JFF = JFDRAT(IDFIXE(1),ISCFIXE(1))
             IF( JFF /= JFDRAT(ID,ISC) ) THEN
               IF( isLarge(ISC) ) THEN
                 JFF = JFF + 1
                 IF( JFF == 9 ) JFF = 1
                 IF( JFF /= JFDRAT(ID,ISC) )                      CYCLE
               ELSE
                 JFF = JFF - 1
                 IF( JFF == 0 ) JFF = 8
                 IF( JFF /= JFDRAT(ID,ISC) )                      CYCLE
               ENDIF
             ENDIF
           ENDIF
         ENDIF
         IDOKLI(ID,ISC) = 1
         NBOKLI = NBOKLI + 1
       ENDDO
     ENDIF
   ENDDO
!
   if( nbfixe /= 1 ) return
   if( nbokli == 0 ) return
! checks compatibily of angles when only one fixed station
   iscf = iscfixe(1)
   idf  = idfixe(1)
   dtf  = ttdrat(idf,1,iscf) - ttdrat(idf,2,iscf)
   dzf  = zzdrat(idf,1,iscf) - zzdrat(idf,2,iscf)
   anf  = sqrt( dtf**2 + dzf**2 )
   do nn=1,nblibr
     iscl   = isclibr(nn)
     isemeo = .false.
     if( iscf <= ncoubas2 ) then
       if( iscl >  ncoubas2 ) return
     else
       if( iscl <= ncoubas2 ) return
       if( iscf >= 13 .and. iscf <= 16 ) then 
         if( iscl >= 13 .and. iscl <= 16 ) isemeo = .true.
       endif
     endif
     do id=1,nbdrat(iscl)
       if( idokli(id,iscl) == 0 ) cycle
       if( abs(ffdrat(idf,iscf)-ffdrat(id,iscl)) > 0.01d0 ) cycle
       dtl = ttdrat(id,1,iscl) - ttdrat(id,2,iscl)
       dzl = zzdrat(id,1,iscl) - zzdrat(id,2,iscl)
       anl = sqrt( dtl**2 + dzl**2 )
       dt0 = ttdrat(id,1,iscl) + ttdrat(id,2,iscl) - ttdrat(idf,1,iscf) - ttdrat(idf,2,iscf)
       dz0 = zzdrat(id,1,iscl) + zzdrat(id,2,iscl) - zzdrat(idf,1,iscf) - zzdrat(idf,2,iscf)
       an0 = sqrt( dt0**2 + dz0**2 )
       sif = ( dtf*dz0 - dzf*dt0 ) / ( anf * an0 )
       sil = ( dtl*dz0 - dzl*dt0 ) / ( anl * an0 )
       if( abs( sif + sil ) > 0.30d0 ) then
         idokli(id,iscl) = 0
         nbokli = nbokli - 1
         if( nbokli <= 0 ) return
       else if( isemeo ) then ! special case for EM-EO pair of segments
         if( abs(sif) > 0.10d0 .or. abs(sil) > 0.10d0 ) then
           idokli(id,iscl) = 0
           nbokli = nbokli - 1
           if( nbokli <= 0 ) return
         endif
       endif
     enddo
   enddo
!
 END SUBROUTINE GEOKLI
!

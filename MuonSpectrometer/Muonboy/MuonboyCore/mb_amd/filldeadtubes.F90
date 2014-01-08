!
!> \author SamuSog
!
!--------------------------------------------------------------------
!> This was originaly in amdcox
!--------------------------------------------------------------------
 SUBROUTINE FillDeadTubes
 USE M_MB_MuGeom
 IMPLICIT NONE
!
      INTEGER :: jtyp, jff, jzz, job, JLAY
      INTEGER :: JGEO, JTEC, JSTA
      INTEGER :: IGEOM, NTUTU, KTUTU
      INTEGER :: IC,INOF,INOZ,ICT 
      INTEGER :: Nword 
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      IadDedChan(:)  = 0
      IadDisChan(:)  = 0
      NbitDedChan    = 0
      IbitDedChan(:) = 0
!
      DO  JTYP=1,MTYP
       DO  JFF=1,8
        DO  JZZ=-MZZ,MZZ
         JGEO = IGEO(JTYP,JFF,JZZ)
         IF( JGEO < 1 ) CYCLE
         DO JOB=1,NOBJ(JTYP,JGEO)
          JTEC = ITEC(JTYP,JGEO,JOB)
          if ( CARTEC(JTEC) .ne. 'MDT') CYCLE
          JSTA = ISTA(JTYP,JGEO,JOB)
!
          CALL GETIND(Jtyp,Jff,Jzz,Job, IC,INOF,INOZ, ICT)
          IGEOM = IGEOMA(IC,INOF,INOZ)
          NTUTU = NLAZ(JTEC,JSTA)
          KTUTU = NINT( Geo_Le(JTYP,JGEO,JOB)/STAPP(JTEC,JSTA))
!
          CALL SetBitDedChan( JTYP,JFF,JZZ,JOB, NTUTU,KTUTU,   &
              Nword,IbitDedChan(NbitDedChan+1) )
          IF( Nword > 0 ) THEN
           IF( NbitDedChan + Nword > NDEDMA ) THEN
            PRINT 8882,NDEDMA
8882        FORMAT(/' In FillDeadTubes of MUONBOY : Parameter  NDEDMA =',   &
                 I7,'  is not big enough  ====>  STOP !!!')
            STOP
           ENDIF
           IadDedChan(IGEOM) = NbitDedChan + 1
           NbitDedChan = NbitDedChan + Nword
          ENDIF
          CALL SetBitDisChan( JTYP,JFF,JZZ,JOB, NTUTU,KTUTU,   &
             Nword,IbitDedChan(NbitDedChan+1) )
          IF( Nword > 0 ) THEN
           IF( NbitDedChan + Nword > NDEDMA ) THEN
            PRINT 8882,NDEDMA
            STOP
           ENDIF
           IadDisChan(IGEOM) = NbitDedChan + 1
           NbitDedChan = NbitDedChan + Nword
          ENDIF
!
         ENDDO
        ENDDO
       ENDDO
      ENDDO
!
 END SUBROUTINE FillDeadTubes
!
!--------------------------------------------------------------------
!> This routine stores 
!> the ids Jtyp/Jff/Jzz/Jobj/Jsl/Jtube of a dead unit
!> This reproduces the reading of C lignes which is in readmdb
!--------------------------------------------------------------------
 SUBROUTINE FillDeadTubesAmdc
 USE M_MB_MuGeom 
 IMPLICIT NONE
!
      INTEGER :: jtyp, jff, jzz, job, JLAY
      INTEGER :: JGEO, JTEC, JSTA
      INTEGER :: C(20)
      INTEGER :: I, K 
      INTEGER :: ier
      INTEGER :: NTUTU
!
#include "AmdcStand/amdcsim_com.inc"
!
      call resetcrlines
!
      DO  JTYP=1,MTYP
       DO  JFF=1,8
        DO JZZ=-MZZ,MZZ
         JGEO = IGEO_V(JTYP_F_Z(JTYP,JFF,JZZ))
         IF( JGEO < 1 ) CYCLE
         DO JOB=1,NOBJ(JTYP,JGEO)
          JTEC = ITEC_V(JTYP_G_O(JTYP,JGEO,JOB))
          if ( CARTEC(JTEC) .ne. 'MDT') CYCLE
          JSTA = ISTA_V(JTYP_G_O(JTYP,JGEO,JOB))
!
          NTUTU     = NLAZ(JTEC,JSTA)
!
          DO  JLAY=1,NTUTU
!
           K =0
           C(:)  = 0
           DO I=1,MDTOffNber
            if ( JTYP.NE.MDTOffTyp(i) ) CYCLE
            if ( JFF .NE.MDTOffJff(i) ) CYCLE
            if ( JZZ .NE.MDTOffJzz(i) ) CYCLE
            if ( JOB .NE.MDTOffJob(i) ) CYCLE
            if ( JLAY.NE.MDTOffJsl(i) ) CYCLE
            K =K+1
            C(K) = MDTOffJtube(i)
            if ( K .EQ. 20 ) THEN
              CALL setclines (JTYP,JFF,JZZ,JOB,JLAY, C, ier)
              K = 0
              C(:)  = 0
            ENDIF
           ENDDO
           if (K .NE.0 ) THEN
            CALL setclines (JTYP,JFF,JZZ,JOB,JLAY, C, ier)
            K = 0
            C(:)  = 0
           ENDIF
!
          ENDDO
!
         ENDDO
        ENDDO
       ENDDO
      ENDDO
!
 END SUBROUTINE FillDeadTubesAmdc
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
 SUBROUTINE WiteDeadTubes
 USE M_MB_MuGeom
 IMPLICIT NONE
!
      INTEGER :: jtyp, jff, jzz, job, JLAY
      INTEGER :: JGEO, JTEC, JSTA
      INTEGER :: IGEOM, NTUTU, KTUTU
      INTEGER :: JTUB 
      INTEGER :: IC,INOF,INOZ,ICT 
      CHARACTER(3) :: ctyp
      LOGICAL :: IsTubeDead 
!
#include "AmdcStand/amdcsim_com.inc"
!
      DO  JTYP=1,MTYP
       DO  JFF=1,8
        DO  JZZ=-MZZ,MZZ
         JGEO = IGEO_V(JTYP_F_Z(JTYP,JFF,JZZ))
         IF( JGEO < 1 ) CYCLE
         DO JOB=1,NOBJ(JTYP,JGEO)
          JTEC = ITEC_V(JTYP_G_O(JTYP,JGEO,JOB))
          if ( CARTEC(JTEC) .ne. 'MDT') CYCLE
          JSTA = ISTA_V(JTYP_G_O(JTYP,JGEO,JOB))
!
          CALL GETIND(Jtyp,Jff,Jzz,Job, IC,INOF,INOZ, ICT)
          IGEOM = IGEOMA(IC,INOF,INOZ)
          NTUTU = NLAZ(JTEC,JSTA)
          KTUTU = NINT( Geo_Le_V(JTYP_G_O(JTYP,JGEO,JOB))/STAPP(JTEC,JSTA))
!
          DO JLAY=1,NTUTU
           DO JTUB=1,KTUTU
!
            IF( IsTubeDead(IGEOM,JLAY,JTUB) ) THEN
             CALL FGetStationType(jtyp, ctyp)
             WRITE(*,1000) ctyp,jff,jzz,job,JLAY,JTUB
1000         FORMAT(" MDT station ",A3,3I3," has dead tube Jlay=",I3," Jtube ",I3)
            ENDIF
!
           ENDDO
          ENDDO
!
         ENDDO
        ENDDO
       ENDDO
      ENDDO
!
 END SUBROUTINE WiteDeadTubes
!

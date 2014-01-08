!
!> Deal with Fit stuff
!> \author Samusog
!
!------------------------------------------------------------------------
 SUBROUTINE ResetDealWithFit
   USE M_MB_CODRAT
   USE M_MB_MuGeom, ONLY : NMACSC
   USE M_MB_Reco_Segments
!
    CALL Reset_MB_Reco_Segments
    NBDRAT(1:NMACSC) = 0
    NBDRIT(1:NMACSC) = 0
!
   call ResetCODRAT
   call ResetCOFITO
!
 END SUBROUTINE ResetDealWithFit
!
!------------------------------------------------------------------------
 SUBROUTINE ResetCODRAT
   USE M_MB_CODRAT
!
   call Reset_CODRAT
!
 END SUBROUTINE ResetCODRAT
!
!------------------------------------------------------------------------
 SUBROUTINE AddToCODRAT(&
        ISC,CCDRAT_i,FFDRAT_i,GGDRAT_i,EZDRAT_i, &
        SSDRAT_i1,ZZDRAT_i1,TTDRAT_i1,IWDRAT_i1, &
        SSDRAT_i2,ZZDRAT_i2,TTDRAT_i2,IWDRAT_i2, &
        JFDRAT_i, &
        KOUNTERDRA_i)
   USE M_MB_CODRAT
   IMPLICIT NONE
   INTEGER, INTENT(IN) :: ISC
   REAL(8), INTENT(IN) :: CCDRAT_i,FFDRAT_i,GGDRAT_i,EZDRAT_i
   REAL(8), INTENT(IN) :: SSDRAT_i1,ZZDRAT_i1,TTDRAT_i1 
   REAL(8), INTENT(IN) :: SSDRAT_i2,ZZDRAT_i2,TTDRAT_i2
   INTEGER, INTENT(IN) :: IWDRAT_i1
   INTEGER, INTENT(IN) :: IWDRAT_i2
   INTEGER, INTENT(IN) :: JFDRAT_i
   INTEGER, INTENT(IN) :: KOUNTERDRA_i
!
   CALL Add_To_CODRAT(&
      ISC,CCDRAT_i,FFDRAT_i,GGDRAT_i,EZDRAT_i, &
      SSDRAT_i1,ZZDRAT_i1,TTDRAT_i1,IWDRAT_i1, &
      SSDRAT_i2,ZZDRAT_i2,TTDRAT_i2,IWDRAT_i2, &
      JFDRAT_i, &
      KOUNTERDRA_i)
!
 END SUBROUTINE AddToCODRAT
!
!------------------------------------------------------------------------
 SUBROUTINE ResetCOFITO
   USE M_MB_Fit
!
   call Reset_COFITO
!
 END SUBROUTINE ResetCOFITO
!
!------------------------------------------------------------------------
 SUBROUTINE AddToCOFITO(ISC_i,KOUNTERDRA_i)
   USE M_MB_CODRAT
   USE M_MB_Fit
   IMPLICIT NONE
   INTEGER, INTENT(IN) :: ISC_i,KOUNTERDRA_i
   INTEGER :: IDFIXE_i
!
   do IDFIXE_i=1,NBDRAT(ISC_i)
     if (KOUNTERDRA(IDFIXE_i,ISC_i) .eq. KOUNTERDRA_i) then
       CALL Add_To_COFITO(ISC_i,IDFIXE_i)
       exit
     endif
   enddo
!      
 END SUBROUTINE AddToCOFITO
!
!------------------------------------------------------------------------
 SUBROUTINE GetNBFIXE(NBFIXE_o)
   USE M_MB_Fit
   IMPLICIT NONE
   INTEGER, INTENT(OUT) :: NBFIXE_o
!
   NBFIXE_o = NBFIXE
!
 END SUBROUTINE GetNBFIXE
!
!------------------------------------------------------------------------
 SUBROUTINE GetKOUNTERDRAFIX(Icur,KOUNTERDRA_o)
   USE M_MB_CODRAT
   USE M_MB_Fit
   IMPLICIT NONE
   INTEGER, INTENT(IN)  :: Icur
   INTEGER, INTENT(OUT) :: KOUNTERDRA_o
!
   KOUNTERDRA_o = KOUNTERDRA(IDFIXE(Icur),ISCFIXE(Icur))
!
 END SUBROUTINE GetKOUNTERDRAFIX 
!
!------------------------------------------------------------------------
 SUBROUTINE DoTheFit(MboyStatusInput,ISC0,ISC1,iok)
   USE M_MB_Control
   USE M_MB_Reco_Tracks
   USE M_MB_COMUSC
   USE M_MB_FAX
   USE M_MB_COSURE
   USE M_MB_Fit
   USE M_MB_CODRAT
   IMPLICIT NONE
   INTEGER, INTENT(IN)  :: MboyStatusInput,ISC0,ISC1
   INTEGER, INTENT(OUT) :: iok
!
    iok = 1
!
    IF( MBPri >= 3 ) THEN
      WRITE(*,*) 'DoTheFit MboyStatusInput ',MboyStatusInput
      WRITE(*,*) ' NBFIXE ',NBFIXE,' ISCFIXE ',ISCFIXE(1:NBFIXE)
      WRITE(*,*) ' NBLIBR ',NBLIBR,' ISCLIBR ',ISCLIBR(1:NBLIBR)
    ENDIF   
!
    CALL RecoTraStart
!
    NFSCAN = 0
    ISENFI = 1
!
!FROM RECOTRA BEFORE TRACKFING
! Reset the number of m.s. centers and NBFAX and NZSure to 0,
! ItypFit to its original value and tracking limits to their std. values
    CALL Reset_MB_COMUSC
    CALL Reset_MB_FAX
    NZSure    = 0
    IFSure(:) = 999
    ItypFit = ItypFit_STD
    RCHAMI  = RCYLIN(3)
    ZCHAMI  = ZCYLIN(3)
    RCHAMA  = RCYLIN(4)
    ZCHAMA  = ZCYLIN(4)
    IWcross(:,:) = 0
!
! Here one should do the primary fit
  if ( MboyStatusInput == 0 ) then
    call DoThePrimaryFit(1,ISC0,ISC1,0, iok)
    IF( MBPri >= 3 ) WRITE(*,*) 'DoTheFit DoThePrimaryFit iok ',iok
    return
  endif
!
! Here one should do the secondary fit
  if ( MboyStatusInput == 1 ) then
    call DoTheSecondaryFit(0,iok)
    IF( MBPri >= 3 ) WRITE(*,*) 'DoTheFit DoTheSecondaryFit iok ',iok
    return
  endif
!
! Here one should do the main fit
  if ( MboyStatusInput >= 2 .and. MboyStatusInput < 100 ) then 
    call DoTheMainFit(iok)
    IF( MBPri >= 3 ) WRITE(*,*) 'DoTheFit DoTheMainFit iok ',iok
    return
  endif
!
! Here one should do the ultimate fit
  if ( MboyStatusInput >= 100 ) then
    call DoTheRefineFit(0, iok)
    CALL CHECAN
    CALL RecoTraEnd 
    IF( MBPri >= 3 ) WRITE(*,*) 'DoTheFit DoTheRefineFit iok ',iok
    return
  endif
!
!
  IF( MBPri >= 3 ) WRITE(*,*) 'DoTheFit END iok ',iok
!
 END SUBROUTINE DoTheFit 
!

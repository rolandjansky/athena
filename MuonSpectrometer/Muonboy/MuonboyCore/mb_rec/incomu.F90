!
 SUBROUTINE INCOMU
 USE M_MB_KindDef
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_Fit
 IMPLICIT NONE
 LOGICAL, SAVE :: LFIRST = .FALSE.
!
!>> Basic initialisation of MUONBOY Modules
!
!>> Perform basic initialisation only once !
    IF( LFIRST ) RETURN
    LFIRST = .TRUE.
!
!>> Check precision on this compiler
    CALL Check_Compiler_Compatibility
!
!
!>> Initialisation of Module  M_MB_Control
    CALL GET_MUONBOY_VERSION( Muonboy_Version )
!>> Type of fit : 1 ==> 1, then 3
!>>               2 ==> 2, then 1, then 3
    ItypFit_STD   = 2
    ItypFit       = 2
    Lun_RecInfo   = 0
    Lun_Timing    = 0
    Ignore_Misali = 0
    IVersAmdc     = 0
!
!>> Initialisation of Modules  M_MB_General_Volumes + M_MB_Xml + M_MB_Xml_TREE
    CALL Init_MB_General_Volumes
    CALL Init_MB_Xml
!
!>> Initialisation of Module M_MB_MuGeom
    NMATTR = 0
    NCOUCH = 0
    NCOUBA = 0
    NMACSC = 0
    NCOCSC = 0
    NLINTO = 0
    NTRITO = 0
    NTRIBA = 0
    PASTUB(:) = 0.D0
    RAYTUB(:) = 0.D0
    DEDTUB(:) = 0.D0
    RESTUB(:) = 0.D0
    RADTUB(:) = 0.D0
    DRITUB(:) = 0.D0
    ACUTUB(:) = 0.D0
    OLTUB(:,:,:) = 0.D0
    TLTUB(:,:,:) = 0.D0
    NLTUB(:) = 0
!
!>> Initialisation of Module M_MB_COCYLI
    RCYLIN(1) =    5.D0
    ZCYLIN(1) =   10.D0
!>> Entrance of calos
!** RCYLIN(2) =  114.D0
!** ZCYLIN(2) =  338.D0
!** RCYLIN(2) =  108.D0
!** ZCYLIN(2) =  340.D0
    RCYLIN(2) =  105.D0
    ZCYLIN(2) =  320.D0
!>> Entrance of muon spectro.
    RCYLIN(3) =  425.D0
    ZCYLIN(3) =  675.D0
!>> Exit of muon spectro.
    RCYLIN(4) = 1260.D0   ! redefined in amdcox according to layout
    ZCYLIN(4) = 2250.D0
    RCYLI2(1:4) = RCYLIN(1:4)**2
!
!>> Initialisation of part of Module M_MB_Fit
!>> Internal and external limits for tracking
    RCHAMI = RCYLIN(3)
    ZCHAMI = ZCYLIN(3)
    RCHAMA = RCYLIN(4)
    ZCHAMA = ZCYLIN(4)
!
 END SUBROUTINE INCOMU
!

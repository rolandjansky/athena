!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE PUT_XML(MLEVEL,I_AGDD_Matter,NbFLAG)
 USE M_MB_Increm_Index
 USE M_MB_Xml
 USE M_MB_General_CONSTANTS, ONLY : IproblemBOO
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: MLEVEL, I_AGDD_Matter
 INTEGER, INTENT(OUT) :: NbFLAG
 CHARACTER(80) ::     NAM(MLEVEL)
 INTEGER ::           N(0:MLEVEL)
 INTEGER ::            NP(MLEVEL)
 INTEGER ::       NREPEAT(MLEVEL)
 INTEGER :: INDEXML(Mxmli,MLEVEL)
 INTEGER ::   INDEX(Mxmli)
 INTEGER ::      IVP_STAK(MLEVEL), IVP_AXIS(MLEVEL), IGS_AXIS(MLEVEL)
 REAL(8) ::      STA_AXIS(MLEVEL), CEN_AXIS(MLEVEL), EDG_AXIS(MLEVEL)
 REAL(8) ::      TRA(3,  MLEVEL)
 REAL(8) ::      ROT(3,3,MLEVEL)
 REAL(8) :: TRACA(3), TRANEW(3)
 REAL(8) :: ROTCA(3,3), ROTCB(3,3), ROTNEW(3,3), ROTUNIT(3,3)
 CHARACTER(80) :: NAMNEW,NAMPRE,NAMENV,NameObj
 CHARACTER(80) :: NameVar,NameVarI,NameVarFor
 CHARACTER(6)  :: CAR6
 CHARACTER(80) :: ATAB(20)
 INTEGER ::       ITAB(20)
 REAL(8) ::       RTAB(20)
 REAL(8) ::       VXYZ(3)
 INTEGER :: I_Curre_Ref_Syst, Nb_Curre_xob_ind, Iobj, IELE, Info, IVOL, Icreat
 INTEGER :: IvolBOOL, IcolBOOL, ImatBOOL, L, Lremov, Lstack, IVP, IELEPRE, NPRE
 INTEGER :: NCAR, IA,II,IR, I,J, I_Beg,I_End, Ie, Kremov, IVP0, Jrep, IGS
 INTEGER :: LoopFor,IwheFor
 REAL(8) :: EDGIN,EDGOUT, CENIN,CENOUT, TRADIR, BegiFor,StepFor,ValuFor, GAPSHI
 LOGICAL :: IS_IT_REMOVED, IS_IT_EMPTY
!
 DATA ROTUNIT/ 1.D0, 0.D0, 0.D0,    &
               0.D0, 1.D0, 0.D0,    &
               0.D0, 0.D0, 1.D0 /
!
    NbFLAG = 0
    IF( Nb_xml_vis <= 0 ) RETURN
!
    IVP_STAK(:) = 0
    IVP_AXIS(:) = 0
    IGS_AXIS(:) = 0
    STA_AXIS(:) = 0.D0
    CEN_AXIS(:) = 0.D0
    EDG_AXIS(:) = 0.D0
!
    Ivma_info(:,:) = 0
!
!
!>> Define the tree of volume starting from  Nam_xml_vis(1)
!>> and flag (Ivol_info(..) = -1) the solids and/or boolean volumes
!>> that will have to be created as generic volumes in this tree
    CALL Form_Xml_Tree(MLEVEL, -1, Nam_xml_vis(1), NbFLAG)
    IF( NbFLAG < 1 )      RETURN
!
!
!>> Create generic volumes in the Default (e.g. Persint) ref. frame !!! ========
!>> (and without indexes in the names of the volumes) ==========================
    CALL Get_Atlas_Ref_Syst( I_Curre_Ref_Syst )
    CALL Set_Atlas_Ref_Syst( 0 )
    Nb_Curre_xob_ind = Nb_xob_ind
    Nb_xob_ind       = 0
    CALL NEWSUV('Generic_volumes')
!>> Basic volumes = solids !!! -----------------------------------------
    DO Iobj=1,Nb_xob
      IELE = Iele_xob(Iobj)
      IF( IELE < Iel0_xml_solid .OR. IELE >= Iel1_xml_cmpac ) CYCLE
      CALL GIV_XML_OBJ_INFO(Iobj,Info)
      IF( Ivol_info(1,Info) >  -1 )                           CYCLE
      CALL Get_Last_Volume_Number( IVOL )
      Ivol_info(1,Info) = IVOL + 1
      IF( IELE < Iel0_xml_cmpac ) THEN
        CALL CREAT_XML_SOLI(Iobj)
      ELSE
        CALL CREAT_XML_COMPACT(Iobj)
      ENDIF
      CALL Get_Last_Volume_Number( Ivol_info(2,Info) )
    ENDDO
!>> The folowing statement : CALL ENDSUV is not necessary but it is a security...
    CALL ENDSUV
!>> Boolean volumes !!! ------------------------------------------------
    IF( .NOT.L_xml_ignore_boolean ) THEN
100   Icreat = 0
      DO Iobj=1,Nb_xob
        IELE = Iele_xob(Iobj)
        IF( IELE < Iel0_xml_bool .OR. IELE > Iel1_xml_bool ) CYCLE
        CALL GIV_XML_OBJ_INFO(Iobj,Info)
        IF( Ivol_info(1,Info) > -1 )                         CYCLE
        CALL CREAT_XML_BOOL(MLEVEL,Iobj, IvolBOOL,IcolBOOL,ImatBOOL)
        IF( IproblemBOO > 0 ) THEN
          CALL GIV_NAM_XML_OBJ( Iobj , NameObj )
          PRINT 7771,TRIM(NameObj)
7771      FORMAT(' In creation of boolean volume : ',A)
          IproblemBOO = 0
        ENDIF
        IF( IvolBOOL < 1 )                                 CYCLE
        Ivol_info(1:2,Info) = IvolBOOL
        Icol_info(    Info) = IcolBOOL
        Imat_info(    Info) = ImatBOOL
        Icreat = Icreat + 1
      ENDDO
      IF( Icreat >= 1 ) GO TO 100
!>> The folowing statement : CALL ENDSUV is not necessary but it is a security...
      CALL ENDSUV
    ENDIF
    CALL Set_Atlas_Ref_Syst( I_Curre_Ref_Syst )
    Nb_xob_ind       = Nb_Curre_xob_ind
!===============================================================================
!
!
!>> Position (multiple) copies of these volumes ================================
!>> in the specified ref. frame i.e. Atlas or Default (Persint) !!! ============
    L = 1
    N(              0) = 0
    NAM(            1) = Nam_xml_vis(1)
    N(              1) = 0
    NP(             1) = 0
    NREPEAT(        1) = 0
    TRA(1:3,        1) = 0.D0
    ROT(1:3,1:3,    1) = ROTUNIT(1:3,1:3)
    INDEXML(1:Mxmli,1) = -999999
!
    CALL NEWSUV(NAM(1))
    NAMENV = 'none'
    Lremov = 999999
    Lstack = 999999
    IF( I_AGDD_Matter < 1 ) THEN
      CALL Get_Last_Volume_Number(      IVP)
    ELSE
      CALL Get_Last_AGDD_Mat_Vol_Number(IVP)
    ENDIF
!
400 IF( NREPEAT(L) <= 1 ) THEN
      CALL Get_Next_Xml_Child( NP(L), N(L) )
      NREPEAT(L) =  0
    ELSE
      NREPEAT(L) = NREPEAT(L) - 1
    ENDIF
    IELEPRE = 0
    IF( L > 1 ) IELEPRE = Iele_xob(N(L-1))
!>> Deal with stacks (i.e. axis-based compositions ) -------------------
    IF(IELEPRE >= Iel0_xml_stack .AND. IELEPRE <= Iel1_xml_stack) THEN
      IF( IVP_STAK(L-1) <= 0 ) IVP_STAK(L-1) = IVP + 1
      IF( IVP_AXIS(L-1) <= 0 ) IVP_AXIS(L-1) = IVP + 1
      IF( IVP > 0 .AND. IVP >= IVP_AXIS(L-1) ) THEN
        CALL GIV_NAM_XML_ELE(IELEPRE, NAMPRE,NCAR)
        VXYZ(1:3) = ROT(1,1:3,L-1)
        IF( NAMPRE == 'stackY' ) VXYZ(1:3) = ROT(2,1:3,L-1)
        IF( NAMPRE == 'stackZ' ) VXYZ(1:3) = ROT(3,1:3,L-1)
        IF( I_AGDD_Matter < 1 ) THEN
          CALL SHIFT_VOL(          VXYZ, IVP_AXIS(L-1), IVP, IGS_AXIS(L-1), CEN_AXIS(L-1), CENOUT,    &
                          EDG_AXIS(L-1), EDGOUT )
        ELSE
          CALL SHIFT_AGDD_MAT_VOL( VXYZ, IVP_AXIS(L-1), IVP, IGS_AXIS(L-1), CEN_AXIS(L-1), CENOUT,    &
                          EDG_AXIS(L-1), EDGOUT )
        ENDIF
        IVP_AXIS(L-1) = IVP + 1
        CEN_AXIS(L-1) = CENOUT + 0.0010D0
        EDG_AXIS(L-1) = EDGOUT + 0.0010D0
!>> End of stack. Do we have to shift the whole thing ?
        IF( N(L) > Nb_xob ) THEN
          CALL GIV_VAL_XML_ATT(N(L-1),'origin', IA,II,IR,ATAB,ITAB,RTAB)
          IF( ATAB(1) == 'atCentre' ) THEN
            CENIN = ( STA_AXIS(L-1) - EDG_AXIS(L-1) ) /2.D0
            EDGIN = 0.D0
            CALL GIV_XML_OBJ_INFO(N(L-1),Info)
            IF( I_AGDD_Matter < 1 ) THEN
              CALL SHIFT_VOL(          VXYZ, IVP_STAK(L-1), IVP, 1, CENIN, CENOUT, EDGIN, EDGOUT )
            ELSE
              CALL SHIFT_AGDD_MAT_VOL( VXYZ, IVP_STAK(L-1), IVP, 1, CENIN, CENOUT, EDGIN, EDGOUT )
            ENDIF
          ELSEIF( ATAB(1) /= 'atStart' ) THEN
            PRINT 7772,TRIM(NAMPRE),TRIM(ATAB(1))
7772        FORMAT(/' In an xml element ',A,' the origin attribute : ',A,    &
                   /' is unknown ====================> One STOPs !!!')
            STOP
          ENDIF
        ENDIF
      ENDIF
    ENDIF
!>> --------------------------------------------------------------------
    IF( N(L) > Nb_xob ) THEN
      L = L - 1
      IF( L <= Lremov ) Lremov = 999999
      IF( L <= Lstack ) Lstack = 999999
      IF( L <= 0 ) GO TO 500
      GO TO 400
    ENDIF
    IELE = Iele_xob(N(L))
    IF( IELE < Iel0_xml_varia .OR. IELE > Iel0_xml_forea ) GO TO 400
    IF( IELE >= Iel0_xml_bool .AND. IELE <= Iel1_xml_bool .AND. L_xml_ignore_boolean ) IELE = Iel0_xml_compo
    NPRE = Iob_xob_sup(N(L))
    CALL GIV_NAM_XML_OBJ( N(L), NameObj )
    IF( NameObj /= NAM(L) .AND. NPRE /= N(L-1) )           GO TO 400
    IF( IS_IT_REMOVED(NAM(L)) .OR. IS_IT_REMOVED(NameObj) ) THEN
      IF( L <= Lstack)                                 GO TO 400
      Lremov = MIN( L , Lremov )
    ENDIF
    IF( IELE >= Iel0_xml_varia .AND. IELE <= Iel1_xml_varia ) THEN
!>> Variable(s) !!! ----------------------------------------------------
!>> NameObj = "name" attribute of the <var>, <array> or <table> object
      CALL GIV_NAM_XML_ELE(IELE,ATAB(1),NCAR)
      IF( ATAB(1) == 'var' ) THEN
        CALL GIV_VAL_XML_ATT(N(L),'value',I,J,IR,ATAB,ITAB,RTAB)
        CALL SetVar_MB_XML( NameObj, RTAB(1) )
      ELSEIF( ATAB(1) == 'array' ) THEN
        CALL GIV_VAL_XML_ATT(N(L),'values',I,J,IR,ATAB,ITAB,RTAB)
        DO I=1,IR
          WRITE (CAR6,'(I6)') I-1
          CAR6 = ADJUSTL(CAR6)
          NameVar = TRIM(NameObj) // '%' // TRIM(CAR6)
          CALL SetVar_MB_XML( NameVar, RTAB(I) )
        ENDDO
      ELSEIF( ATAB(1) == 'table' ) THEN
        I_Beg = Iob_xob_inf0(N(L))
        I_End = Iob_xob_inf1(N(L))
        DO I=I_Beg,I_End
          Ie = Iele_xob(I)
          CALL GIV_NAM_XML_ELE(Ie,ATAB(2),NCAR)
          IF( ATAB(2) /= 'row' ) THEN
            PRINT 7770,TRIM(ATAB(2)),TRIM(ATAB(1))
7770        FORMAT(/' An XML element of type         : ',A,    &
                   /' is embedded in another of type : ',A,    &
                   /' ===============================> One STOPs !!!')
            STOP
          ENDIF
          CALL GIV_VAL_XML_ATT(I,'values',IA,II,IR,ATAB,ITAB,RTAB)
          WRITE (CAR6,'(I6)') I-I_Beg
          CAR6 = ADJUSTL(CAR6)
          NameVar = TRIM(NameObj) // '%' // TRIM(CAR6)
          DO J=1,IR
            WRITE (CAR6,'(I6)') J-1
            CAR6 = ADJUSTL(CAR6)
            NameVarI = TRIM(NameVar) // '%' // TRIM(CAR6)
            CALL SetVar_MB_XML( NameVarI, RTAB(J) )
          ENDDO
        ENDDO
      ELSEIF( ATAB(1) == 'defaults' ) THEN
        CALL GIV_VAL_XML_ATT(N(L),'unit_length', IA,II,IR,ATAB,ITAB,RTAB)
        IF( IA == 1 ) Default_unit_length = ATAB(1)
        CALL GIV_VAL_XML_ATT(N(L),'unit_angle', IA,II,IR,ATAB,ITAB,RTAB)
        IF( IA == 1 ) Default_unit_angle  = ATAB(1)
      ENDIF
      GO TO 400
    ELSEIF( Ivol_info(1,Info_xob(N(L))) >= 1 ) THEN
!>> Solid or compact or boolean volume created as generic volume !!! ---
!>> NameObj = "name" attribute of the solid or bool. vol. object
      IF( L == 4 .OR. L == 5 ) THEN
        CALL ENDSUV
        CALL NEWSUV(NameObj)
      ENDIF
      Kremov = 0
      IF( L >= Lremov ) Kremov = -11
      IF( I_AGDD_Matter < 1 ) THEN
        CALL PUT_XML_VOL(          N(L), TRA(1,L), ROT(1,1,L), INDEXML(1,L), Kremov,  IVP)
      ELSE
        CALL PUT_XML_AGDD_MAT_VOL( N(L), TRA(1,L), ROT(1,1,L), INDEXML(1,L), IVP )
      ENDIF
      GO TO 400
    ELSEIF( IELE >= Iel1_xml_cmpac .AND. IELE <= Iel1_xml_stack .AND. NameObj == NAM(L) ) THEN
!>> Compact (proper) or composition or stack !!! -----------------------
!>> NameObj = "name" attribute of the composition object
      IF( L == 4 .OR. L == 5 ) THEN
        CALL ENDSUV
        CALL NEWSUV(NameObj)
      ENDIF
      CALL GIV_XML_OBJ_INFO(N(L),Info)
      IF( IELE >= Iel0_xml_stack ) THEN
        IF( .NOT.IS_IT_EMPTY(NAM(L)) ) Lstack = MIN( L, Lstack )
      ENDIF
      TRANEW(1:3)     = 0.D0
      ROTNEW(1:3,1:3) = ROTUNIT(1:3,1:3)
      INDEX(1:Mxmli)  = -999999
      NAMNEW          = ' / '
!>>         'envelope' ===============> 2
      CALL GIV_VAL_XML_ATT(N(L),  2  ,IA,II,IR,ATAB,ITAB,RTAB)
      NAMENV          = ATAB(1)
      IVP0 = 0
      IF( NAMENV == 'none' ) IVP0 = IVP + 1
      IF( IELE >= Iel0_xml_stack ) THEN
        CALL GIV_NAM_XML_ELE(IELE, NAMPRE,NCAR)
        VXYZ(1:3) = ROT(1,1:3,L)
        IF( NAMPRE == 'stackY' ) VXYZ(1:3) = ROT(2,1:3,L)
        IF( NAMPRE == 'stackZ' ) VXYZ(1:3) = ROT(3,1:3,L)
        TRADIR = DOT_PRODUCT( TRA(1:3,L) , VXYZ(1:3) )
      ELSE
        TRADIR = 0.D0
      ENDIF
      IVP_STAK(L)     = IVP0
      IVP_AXIS(L)     = IVP0
      STA_AXIS(L)     = TRADIR
      CEN_AXIS(L)     = TRADIR
      EDG_AXIS(L)     = TRADIR
    ELSEIF( IELE >= Iel0_xml_sipos .AND. IELE <= Iel1_xml_mupos .AND. NPRE == N(L-1) ) THEN
!>> Single or multiple positioning !!! ---------------------------------
!>> NameObj = "volume" attribute of the positioning object
      IF( IELE <= Iel1_xml_sipos ) THEN
        Jrep = 1
      ELSE
        IF( NREPEAT(L) < 1 ) THEN
!>>            'ncopy' or 'wedge_number' => 2
          CALL GIV_VAL_XML_ATT(N(L),  2  ,IA,II,IR,ATAB,ITAB,RTAB)
          NREPEAT(L) = ITAB(1)
        ENDIF
        Jrep = NREPEAT(L)
      ENDIF
      CALL XML_TRA(N(L),N(L-1),Jrep, TRACA,ROTCA,IGS,GAPSHI)
      IF( Jrep < 1 ) GO TO 400
      CALL XML_ROT(N(L),             ROTCB)
      CALL XML_IND(N(L),       Jrep, INDEX)
      CALL MULT3M3V(ROT(1,1,L),TRACA, TRANEW)
      CALL MULT3M3M(ROTCA,ROTCB,      ROTNEW)
      NAMNEW          = NameObj
      IGS_AXIS(L-1)   = IGS
      IF( IGS >= 1 ) THEN
        CEN_AXIS(L-1) = CEN_AXIS(L-1) + GAPSHI
      ELSE
        EDG_AXIS(L-1) = EDG_AXIS(L-1) + GAPSHI
      ENDIF
    ELSEIF( IELE == Iel0_xml_forea .AND. NPRE == N(L-1) ) THEN
!>> Foreach !!! --------------------------------------------------------
      CALL GIV_XML_ATT( N(L), 'index', NameVarFor )
      CALL GIV_XML_ATT( N(L), 'loops', LoopFor )
      CALL GIV_XML_ATT( N(L), 'begin', BegiFor )
      CALL GIV_XML_ATT( N(L), 'step',  StepFor )
      IF( NREPEAT(L) < 1 ) NREPEAT(L) = LoopFor
      ValuFor = BegiFor + (LoopFor-NREPEAT(L))*StepFor
      CALL SetVar_MB_XML( NameVarFor, ValuFor )
      CALL GIV_XML_ATT( N(L), 'when',  IwheFor )
      IF( IwheFor < 1 ) GO TO 400
      NAMNEW          = ''
      TRANEW(1:3)     = 0.D0
      ROTNEW(1:3,1:3) = ROTUNIT(1:3,1:3)
      INDEX(1:Mxmli)  = -999999
    ELSE
!>> Anything else !!! --------------------------------------------------
      GO TO 400
    ENDIF
    CALL Increm_Index(L,MLEVEL,'MLEVEL')
    NAM(L)         = NAMNEW
    N(L)           = 0
    NP(L)          = N(L-1)
    NREPEAT(L)     = 0
    TRA(1:3,L)     = TRA(1:3,L-1) + TRANEW(1:3)
    CALL MULT3M3M(ROT(1,1,L-1),ROTNEW, ROT(1,1,L))
    DO I=1,Mxmli
      IF( INDEXML(I,L-1) /= -999999 .AND. INDEX(I) /= -999999 ) THEN
        INDEXML(I,L) = INDEXML(I,L-1) + INDEX(I)
      ELSEIF( INDEXML(I,L-1) /= -999999) THEN
        INDEXML(I,L) = INDEXML(I,L-1)
      ELSEIF( INDEX(I) /= -999999 ) THEN
        INDEXML(I,L) = INDEX(I)
      ELSE
        INDEXML(I,L) = -999999
      ENDIF
    ENDDO
    IF( NAMENV /= 'none' ) THEN
!>> (Single) positionning of the envelope of the previous composition (or stack)
      CALL Increm_Index(L,MLEVEL,'MLEVEL')
      NAM(L)         = NAMENV
      N(L)           = 0
      NP(L)          = N(L-2)
      NREPEAT(L)     = 0
      TRA(1:3,L)     = TRA(1:3,L-1)
      ROT(1:3,1:3,L) = ROT(1:3,1:3,L-1)
      INDEXML(1:Mxmli,L) = INDEXML(1:Mxmli,L-1)
      NAMENV         = 'none'
    ENDIF
    GO TO 400
!===============================================================================
!
!
500 CONTINUE
    CALL ENDSUV
!
!>> Eliminate generic volumes
    CALL ELIM_SUV_NAME('Generic_volumes')
!
!>> Eliminate, from the "removed" tree branches, volumes that
!>> have been kept up to here because of the stacks
!>> (and that are flagged: IclVolu(Iv) = Kremov = -11)...
    IF( Nb_xml_vis >= 2 ) THEN
      CALL MB_Vol_Garbage_Collection
    ENDIF
!
 END

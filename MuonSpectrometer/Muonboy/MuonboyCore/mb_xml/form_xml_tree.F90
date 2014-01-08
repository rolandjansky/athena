!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE Form_Xml_Tree(MLEVEL,IFLAG,CAR80, NbFLAG)
 USE M_MB_Increm_Index
 USE M_MB_Xml
 USE M_MB_Xml_TREE
 IMPLICIT NONE
 INTEGER,       INTENT(IN)  :: MLEVEL, IFLAG
 CHARACTER(80), INTENT(IN)  :: CAR80
 INTEGER,       INTENT(OUT) :: NbFLAG
!
 CHARACTER(80) :: NAM(MLEVEL)
 INTEGER       :: IFL(0:MLEVEL), N(0:MLEVEL), NP(MLEVEL), I_Envelope(MLEVEL), LastKLev(0:MLEVEL)
!>> NamePANIC is defined because of Linux F90 compiler bug !!!
 CHARACTER(80)  :: NAMNEW,NAMENV,NameObj,NameVar,NameVarI, NamePANIC
 CHARACTER(6)   :: CAR6
 CHARACTER(800) :: CAR800A,CAR800B, COMPOS_CAR800,Compos_Branch_xml_800
 LOGICAL        :: L_Store_Tree
 CHARACTER(80)  ::  ATAB(10)
 INTEGER :: ITAB(20)
 REAL(8) :: RTAB(20)
 INTEGER :: L, LPRE, Kstop, K, LP, NUA, LEV, LL, IELE, NPRE, IFLNEW, I_Beg,I_End,Ie, NCAR
 INTEGER :: I, J, IR, IA, II, Info
!
    L = 1
    LPRE = 0
    NAM(1)   = CAR80
    IFL(0)   = 0
    IFL(1)   = 1
    N(0)     = 0
    N(1)     = 0
    NP(1)    = 0
    NAMENV   = 'none'
    I_Envelope(:) = 0
    LastKLev(:)   = 0
    NbFLAG   = 0
!
    L_Store_Tree = IFLAG <= -2
    IF( L_Store_Tree ) THEN
      MTREE = 0.70 * Nb_xob
      MTREE = MAX( 1000 , MTREE )
      CALL ALLOC_Tree
      NTree         = 0
      NamTree(:)    = ''
      LevTree(:)    = 0
      IflTree(:)    = 0
      KpreLev(:)    = 0
      Ivol_info(:,:)= 0
      L_xml_tree_is_formed = .true.
    ENDIF
!
!=======================================================================
100 CALL Get_Next_Xml_Child( NP(L), N(L) )
    IF( N(L) > Nb_xob ) THEN
      IF( L < LPRE .OR. .NOT.L_Store_Tree ) GO TO 150
      CAR800A = COMPOS_CAR800(L,NAM)
      Kstop = MAX( 1 , NTree-2000 )
      DO K=NTree,Kstop,-1
        NamePANIC = NamTree(K)
        IF( NamePANIC /= NAM(L) ) CYCLE
        CAR800B = Compos_Branch_xml_800(K)
        IF( CAR800A == CAR800B )            GO TO 150
      ENDDO
      CAR800B = Compos_Branch_xml_800(NTree)
      DO LP=L-1,1,-1
        CAR800A = COMPOS_CAR800(LP,NAM)
        NUA = LEN_TRIM(CAR800A)
        IF( CAR800A(1:NUA) == CAR800B(1:NUA) ) EXIT
      ENDDO
      LEV = 0
      DO LL=1,L
        IF( LEN_TRIM(NAM(LL)) <= 0 ) CYCLE
        LEV = LEV + 1
        IF( LL <= LP )               CYCLE
        CALL Increm_Index(NTree,MTREE,'MTREE')
        NamTree(NTree) = NAM(LL)
        LevTree(NTree) = LEV
        IflTree(NTree) = IFL(LL)
        IenTree(NTree) = SUM( I_Envelope(1:LL-1) )
        KpreLev(NTree) = LastKLev(LEV-1)
        LastKLev(LEV)  = NTree
      ENDDO
150   LPRE = L
      I_Envelope(L) = 0
      L = L - 1
      IF( L <= 0 ) GO TO 500
      GO TO 100
    ENDIF
    IELE = Iele_xob(N(L))
    NPRE = Iob_xob_sup(N(L))
    IF( IELE > Iel0_xml_forea .AND. IELE <= Iel0_xml_additional .OR. IELE < Iel0_xml_varia )  GO TO 100
    IF( IELE >= Iel0_xml_bool .AND. IELE <= Iel1_xml_bool .AND. L_xml_ignore_boolean ) IELE = Iel0_xml_compo
    CALL GIV_NAM_XML_OBJ( N(L), NameObj )
    IF( IELE >= Iel0_xml_varia .AND. IELE <= Iel1_xml_varia ) THEN
!>> Variables !!! --------------------------------------------------------
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
            PRINT 777,TRIM(ATAB(2)),TRIM(ATAB(1))
777         FORMAT(/' An XML element of type         : ',A,      &
                   /' is embedded in another of type : ',A,      &
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
        CALL GIV_VAL_XML_ATT(N(L),'unit_length',IA,II,IR,ATAB,ITAB,RTAB)
        IF( IA == 1 ) Default_unit_length = ATAB(1)
        CALL GIV_VAL_XML_ATT(N(L),'unit_angle',IA,II,IR,ATAB,ITAB,RTAB)
        IF( IA == 1 ) Default_unit_angle  = ATAB(1)
      ENDIF
      GO TO 100
    ELSEIF( IELE >= Iel0_xml_solid .AND. IELE <= Iel1_xml_solid .AND. NameObj == NAM(L) ) THEN
!>> Solid !!! ------------------------------------------------------------
      CALL GIV_XML_OBJ_INFO(N(L),Info)
      IF( IFLAG <= 0 ) THEN
        Ivol_info(1:2,Info) = IFLAG
      ELSE
        Icol_info(    Info) = IFLAG
      ENDIF
      NbFLAG = NbFLAG + 1
      GO TO 100
    ELSEIF( ( IELE >= Iel0_xml_cmpac .AND. IELE <  Iel1_xml_cmpac .OR. IELE > Iel0_xml_additional )   &
                                          .AND. ( NameObj == NAM(L) .OR. NPRE == N(L-1) ) ) THEN
!>> Compact (specific) !!! -----------------------------------------------
      CALL GIV_XML_OBJ_INFO(N(L),Info)
      IF( IFLAG <= 0 ) THEN
        Ivol_info(1:2,Info) = IFLAG
      ELSE
        Icol_info(    Info) = IFLAG
      ENDIF
      NbFLAG = NbFLAG + 1
      CALL Increm_Index(L,MLEVEL,'MLEVEL')
      NAM(L) = NameObj
      IFL(L) = IFL(L-1)
      N(L)   = 999999999
      NP(L)  = N(L-1)
      GO TO 100
    ELSEIF( IELE >= Iel0_xml_bool .AND. IELE <= Iel1_xml_bool .AND. NameObj == NAM(L) ) THEN
!>> Union or intersection or subtraction !!! -----------------------------
      CALL GIV_XML_OBJ_INFO(N(L),Info)
      IF( IFLAG <= 0 ) THEN
        Ivol_info(1:2,Info) = IFLAG
      ELSE
        Icol_info(    Info) = IFLAG
      ENDIF
      NbFLAG = NbFLAG + 1
      NAMNEW = ''
      IFLNEW = 3
    ELSEIF( IELE >= Iel1_xml_cmpac .AND. IELE <= Iel1_xml_stack .AND. NameObj == NAM(L) ) THEN
!>> Compact (proper) or composition or stack !!! -------------------------
      NAMNEW = ''
      IFLNEW = 1
!>>   'envelope' ===============> 2
      CALL GIV_VAL_XML_ATT(N(L),  2  ,IA,II,IR,ATAB,ITAB,RTAB)
      NAMENV = ATAB(1)
    ELSEIF( IELE >= Iel0_xml_sipos .AND. IELE <= Iel1_xml_mupos .AND. NPRE == N(L-1) ) THEN
!>> Positioning !!! ------------------------------------------------------
      NAMNEW = NameObj
      IFLNEW = 0
    ELSEIF( IELE == Iel0_xml_forea .AND. NPRE == N(L-1) ) THEN
!>> Foreach !!! ----------------------------------------------------------
      NAMNEW = ''
      IFLNEW = 0
    ELSE
      GO TO 100
    ENDIF
    CALL Increm_Index(L,MLEVEL,'MLEVEL')
    NAM(L)   = NAMNEW
    IF( IFL(L-1) == 2 .AND. IFLNEW == 3 ) IFLNEW = 4
    IFL(L)   = MAX( IFLNEW , IFL(L-1) )
    N(L)     = 0
    NP(L)    = N(L-1)
    IF( NAMENV /= 'none' ) THEN
!>> Positionning of the envelope of the previous composition (or stack)
      I_Envelope(L) = + 1
      CALL Increm_Index(L,MLEVEL,'MLEVEL')
      NAM(L)   = NAMENV
      IFL(L)   = 2
      N(L)     = 0
      NP(L)    = N(L-2)
      NAMENV   = 'none'
    ENDIF
    GO TO 100
!=======================================================================
!
500 CONTINUE
!
 END

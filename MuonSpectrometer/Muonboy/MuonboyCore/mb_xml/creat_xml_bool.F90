!
 SUBROUTINE CREAT_XML_BOOL(MLEVEL,Iobj, IvBOO1,IcolBOO1,ImatBOO1)
 USE M_MB_Increm_Index
 USE M_MB_Xml
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: MLEVEL, Iobj
 INTEGER, INTENT(OUT) :: IvBOO1,IcolBOO1,ImatBOO1
!
!>>  Create the generic volume coresponding to the XML "boolean volume"
!>>  (i.e. union or intersection or subtraction) object Iobj
!
 CHARACTER(80) :: NAM(MLEVEL)
 INTEGER       :: N(0:MLEVEL), NREPEAT(MLEVEL)
 REAL(8)       :: TRA(3,MLEVEL), ROT(3,3,MLEVEL)
 REAL(8)       :: TRACA(3), TRANEW(3), ROTCA(3,3), ROTCB(3,3), ROTNEW(3,3)
 CHARACTER(80) :: NAMNEW, NAMBOO, NAMREF, NameVarFor
 CHARACTER(80) :: ATAB( 5)
 INTEGER       :: ITAB(10)
 REAL(8)       :: RTAB(10)
 REAL(8), DIMENSION(3,3), PARAMETER :: ROTUNIT = RESHAPE( (/ 1.D0, 0.D0, 0.D0,    &
                                                             0.D0, 1.D0, 0.D0,    &
                                                             0.D0, 0.D0, 1.D0 /), (/3, 3/) )
 REAL(8) :: XcBOO1,YcBOO1,ZcBOO1, BegiFor,StepFor, GAPbido, ValuFor
 INTEGER :: IELE, NN, KBOOL1,KBOOL2, IA,II,IR, IobjR, IVREF, L, NPRE, Jrep, LoopFor,IwheFor
 INTEGER :: IvBOO2,IvBOO3, IGSbido
!
    IvBOO1   = 0
    IcolBOO1 = 0
    ImatBOO1 = 0
    XcBOO1   = 0.D0
    YcBOO1   = 0.D0
    ZcBOO1   = 0.D0
!
    IELE = Iele_xob(Iobj)
    CALL GIV_NAM_XML_ELE(IELE, NAM(1),NN)
    IF( NAM(1) == 'union' ) THEN
      KBOOL1 = 1
      KBOOL2 = 1
    ELSEIF( NAM(1) == 'intersection' ) THEN
      KBOOL1 = 0
      KBOOL2 = 0
    ELSEIF( NAM(1) == 'subtraction' ) THEN
      KBOOL1 = 1
      KBOOL2 = 0
    ELSE
      GO TO 700
    ENDIF
!
!>>   'name' =================> 1
    CALL GIV_VAL_XML_ATT(Iobj,  1  ,IA,II,IR,ATAB,ITAB,RTAB)
    NAMBOO = ATAB(1)
!>>   'ref_volume' ===========> 2
    CALL GIV_VAL_XML_ATT(Iobj,  2  ,IA,II,IR,ATAB,ITAB,RTAB)
    NAMREF = ATAB(1)
    IF( LEN_TRIM(NAMREF) /= 4 .OR. NAMREF(1:4) /= 'none' ) THEN
!>> A reference volume is defined
      DO IobjR=1,Nb_xob
        IVREF = Ivol_info(1,Info_xob(IobjR))
        IF( IVREF < 1 )         CYCLE
!>>       'name' ==================> 1
        CALL GIV_VAL_XML_ATT(IobjR,  1  ,IA,II,IR,ATAB,ITAB,RTAB)
        IF( ATAB(1) /= NAMREF ) CYCLE
        CALL COPY_VOL_IV( IVREF, NAMBOO,1,0, IvBOO1 )
        CALL GET_CENVOL_IV(IvBOO1,XcBOO1,YcBOO1,ZcBOO1)
        IcolBOO1 = Icol_info(Info_xob(IobjR))
        ImatBOO1 = Imat_info(Info_xob(IobjR))
        GO TO 100
      ENDDO
      GO TO 700
    ENDIF
!
!===============================================================================
100 L = 1
    N(          0) = Iobj
    NAM(        1) = '?'
    N(          1) = 0
    NREPEAT(    1) = 0
    TRA(1:3,    1) = 0.D0
    ROT(1:3,1:3,1) = ROTUNIT(1:3,1:3)
!
200 IF( NREPEAT(L) <= 1 ) THEN
      CALL Get_Next_Xml_Child( N(L-1), N(L) )
      NREPEAT(L) =  0
    ELSE
      NREPEAT(L) = NREPEAT(L) - 1
    ENDIF
    IF( N(L) > Nb_xob ) THEN
      L = L - 1
      IF( L <= 0 ) GO TO 700
      GO TO 200
    ENDIF
    IELE = Iele_xob(N(L))
    NPRE = Iob_xob_sup(N(L))
!>>  'name' or 'volume' ======> 1
    CALL GIV_VAL_XML_ATT(N(L),  1  ,IA,II,IR,ATAB,ITAB,RTAB)
    IF( ( (IELE >= Iel0_xml_solid .AND. IELE <= Iel1_xml_solid) .OR.  (IELE >= Iel0_xml_bool  .AND. IELE <= Iel1_xml_bool) )   &
                                                                .AND. ATAB(1) == NAM(L) ) THEN
!>> basic volumes (solids or bool.) !!! --------------------------------
      IF( Ivol_info(1,Info_xob(N(L))) >= 1 ) THEN
        CALL PUT_XML_VOL(N(L), TRA(1,L), ROT(1,1,L), ITAB(1), 0,  IvBOO2)
        IF( IvBOO1 <= 0 ) THEN
!>> ..... First pass ===> reference volume (if not already defined)
          IvBOO1   = IvBOO2
          IcolBOO1 = Icol_info(Info_xob(N(L)))
          ImatBOO1 = Imat_info(Info_xob(N(L)))
        ELSE
!>> ..... Perform boolean operation: IvBOO1.IvBOO2 -> IvBOO3
          CALL VOLABOOLB_IV( KBOOL1,KBOOL2,IvBOO1,IvBOO2,NAMBOO,0, IvBOO3)
!>> ..... Elimination of volumes must be performed in the inverse order
!>> ..... of their creation (i.e. IvBOO2 before IvBOO1)
          CALL ELIM_VOL_IV(IvBOO2)
          CALL ELIM_VOL_IV(IvBOO1)
!>> ..... As volumes IvBOO2 and IvBOO1 were destroyed by the above calls,
!>> ..... the address (IvBOO3) of the created boolean volume is shifted by 2
          IvBOO1 = IvBOO3 - 2
        ENDIF
        GO TO 200
      ELSE
        IF( IvBOO1 >= 1 ) THEN
          CALL ELIM_VOL_IV(IvBOO1)
          IvBOO1 = 0
        ENDIF
        GO TO 700
      ENDIF
    ELSEIF( IELE >= Iel0_xml_compo .AND. IELE <= Iel1_xml_stack .AND. ATAB(1) == NAM(L) ) THEN
!>> composition (or stack) !!! -----------------------------------------
      TRANEW(1:3)     = 0.D0
      ROTNEW(1:3,1:3) = ROTUNIT(1:3,1:3)
      NAMNEW          = '#'
    ELSEIF( IELE >= Iel0_xml_sipos .AND. IELE <= Iel1_xml_mupos .AND. NPRE == N(L-1) ) THEN
!>> Single or multiple positioning !!! ---------------------------------
      IF( IELE <= Iel1_xml_sipos ) THEN
        Jrep = 1
      ELSE
        IF( NREPEAT(L) < 1 ) THEN
!>>     'ncopy' or 'wedge_number' => 2
          CALL GIV_VAL_XML_ATT(N(L),  2  ,IA,II,IR,ATAB,ITAB,RTAB)
          NREPEAT(L) = ITAB(1)
        ENDIF
        Jrep = NREPEAT(L)
      ENDIF
      CALL XML_TRA(N(L),N(L-1),Jrep, TRACA,ROTCA,IGSbido,GAPbido)
      IF( Jrep < 1 ) GO TO 200
      CALL XML_ROT(N(L),              ROTCB)
      CALL MULT3M3V(ROT(1,1,L),TRACA, TRANEW)
      CALL MULT3M3M(ROTCA,ROTCB,      ROTNEW)
      NAMNEW          = ATAB(1)
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
      IF( IwheFor < 1 ) GO TO 200
      NAMNEW          = ''
      TRANEW(1:3)     = 0.D0
      ROTNEW(1:3,1:3) = ROTUNIT(1:3,1:3)
    ELSE
      GO TO 200
    ENDIF
    CALL Increm_Index(L,MLEVEL,'MLEVEL')
    NAM(L)         = NAMNEW
    N(L)           = 0
    NREPEAT(L)     = 0
    TRA(1:3,L)     = TRA(1:3,L-1) + TRANEW(1:3)
    CALL MULT3M3M(ROT(1,1,L-1),ROTNEW, ROT(1,1,L))
    GO TO 200
!===============================================================================
!
700 CONTINUE
!
!>> Center of the boolean volume is the center of the reference volume when
!>> there is one... and the reference system of the boolean operation otherwise !!!
    CALL DEF_CENVOL_IV(IvBOO1,XcBOO1,YcBOO1,ZcBOO1)
!
 END SUBROUTINE CREAT_XML_BOOL
!

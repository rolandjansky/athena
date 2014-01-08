!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE GetCharAndNum( Line, Form,  JA,JI,JR,AAA,III,RRR, Ierror )
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN)    :: Line,Form
 INTEGER,           INTENT(OUT)   :: JA,JI,JR,Ierror
 CHARACTER (len=*), INTENT(INOUT) :: AAA(*)
 INTEGER,           INTENT(INOUT) :: III(*)
 REAL(8),           INTENT(INOUT) :: RRR(*)
!
 INTEGER           :: LenLine,LenForm,Nrepe,ILine,IForm,L09,Length,Isisi,Ipoint
 CHARACTER (len=1) :: CurLine,CurForm
 INTEGER, EXTERNAL :: J_ValNum_Char1
!
    JA     = 0
    JI     = 0
    JR     = 0
    Ierror = 0
!
    LenLine = LEN_TRIM(Line)
    LenForm = LEN_TRIM(Form)
    ILine   = 0
    IForm   = 0
    Nrepe   = 0
    Length  = 1
!
10  IF( Length < 1 ) GO TO 900
    IF( Nrepe > 0 ) THEN
       Nrepe = Nrepe -1
       IF( Nrepe > 0 ) GO TO 30
    ENDIF
!
20  IForm = IForm + 1
    IF( IForm > LenForm ) GO TO 800
    CurForm = Form(IForm:IForm)
    IF( CurForm/= 'A' .AND. CurForm /= 'I' .AND. CurForm /= 'R' ) THEN
       L09 = J_ValNum_Char1( CurForm )
       IF( L09 >= 0 ) THEN
          Nrepe = 10*Nrepe + L09
          GO TO 20
       ELSE
          GO TO 900
       ENDIF
    ENDIF
!
30  IF( CurForm == 'A' ) GO TO 100
    IF( CurForm == 'I' ) GO TO 200
    IF( CurForm == 'R' ) GO TO 300
!
!>> Characters <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
100 JA = JA + 1
    AAA(JA) = ''
    Length  = 0
110 ILine = ILine + 1
    IF( ILine > LenLine ) GO TO 10
    CurLine = Line(ILine:ILine)
    IF( CurLine == ' ' ) THEN
       IF( Length > 0 )   GO TO 10
    ELSE
       Length  = Length + 1
       AAA(JA)(Length:Length) = CurLine
    ENDIF
    GO TO 110
!
!>> Integers <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
200 JI = JI + 1
    III(JI) =  0
    Length  = -1
    Isisi   =  1
210 ILine = ILine + 1
    IF( ILine > LenLine ) GO TO 10
    CurLine = Line(ILine:ILine)
    IF(     CurLine == ' ' ) THEN
       IF( Length > 0 ) GO TO 10
    ELSEIF( CurLine == '+' ) THEN
       IF( Length >= 0 ) GO TO 900
       Isisi  =  1
       Length =  0
    ELSEIF( CurLine == '-' ) THEN
       IF( Length >= 0 ) GO TO 900
       Isisi  = -1
       Length =  0
    ELSE
       L09 = J_ValNum_Char1( CurLine )
       IF( L09 >= 0 ) THEN
          III(JI) = III(JI)*10 + Isisi*L09
          Length = MAX(Length,0) + 1
       ELSE
          GO TO 900
       ENDIF
    ENDIF
    GO TO 210
!
!>> Reals <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
300 JR = JR + 1
    RRR(JR) = 0.D0
    Length  = -1
    Isisi   =  1
    Ipoint  =  0
310 ILine = ILine + 1
    IF( ILine > LenLine ) GO TO 10
    CurLine = Line(ILine:ILine)
    IF(     CurLine == ' ' ) THEN
       IF( Length > 0 ) GO TO 10
    ELSEIF( CurLine == '+' ) THEN
       IF( Length >= 0 ) GO TO 900
       Isisi  =  1
       Length =  0
    ELSEIF( CurLine == '-' ) THEN
       IF( Length >= 0 ) GO TO 900
       Isisi  = -1
       Length =  0
    ELSEIF( CurLine == '.' ) THEN
       IF( Ipoint > 0 ) GO TO 900
       Ipoint = 1
    ELSE
       L09 = J_ValNum_Char1( CurLine )
       IF( L09 >= 0 ) THEN
          IF( Ipoint < 1 ) THEN
             RRR(JR) = RRR(JR)*10.D0 + Isisi*L09
          ELSE
             RRR(JR) = RRR(JR) + Isisi*L09/10.D0**Ipoint
             Ipoint  = Ipoint + 1
          ENDIF
          Length = MAX(Length,0) + 1
       ELSE
          GO TO 900
       ENDIF
    ENDIF
    GO TO 310
!
800 Ierror = 0
    RETURN
!
900 Ierror = 1
    RETURN
!
 END

 SUBROUTINE InfSetStatTShift( StationName, JZZ ,JFF, TShift )
 CHARACTER(3), INTENT(IN) :: StationName
 INTEGER, INTENT(IN) :: JZZ ,JFF
 REAL(8), INTENT(IN) :: TShift
 CHARACTER (Len=1) :: StationNameChar1(3)

#if !defined(ATLAS_STAND)
 StationNameChar1(1) = StationName(1:1)
 StationNameChar1(2) = StationName(2:2)
 StationNameChar1(3) = StationName(3:3)
 call incsetstattshift( StationNameChar1,JZZ ,JFF, TShift )
#endif
 
 END SUBROUTINE InfSetStatTShift

      FVOIDP FUNCTION PYQPAR_ADDRESS() 
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      COMMON /PYQPAR/ T0,TAU0,NF,IENGLU,IANGLU
      FVOIDP GETADDR
      PYQPAR_ADDRESS = GETADDR(T0)
      
      RETURN
      END

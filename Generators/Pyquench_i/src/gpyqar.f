      INTEGER*4 FUNCTION PYQPAR_ADDRESS() 
      IMPLICIT NONE
      INTEGER nfu,ienglu,ianglu
      DOUBLE PRECISION T0u,tau0u
      COMMON/PYQPAR/ T0u,tau0u,nfu,ienglu,ianglu
      INTEGER*4 GETADDR

      PYQPAR_ADDRESS = GETADDR(T0u)
      RETURN
      END

C-----------------------------------------------------------------------
C                           H E R W I G
C
C            a Monte Carlo event generator for simulating
C        +---------------------------------------------------+
C        | Hadron Emission Reactions With Interfering Gluons |
C        +---------------------------------------------------+
C I.G. Knowles(*), G. Marchesini(+), M.H. Seymour($) and B.R. Webber(#)
C-----------------------------------------------------------------------
C with Minimal Supersymmetric Standard Model Matrix Elements by
C                S. Moretti(&,X) and K. Odagiri(^)
C-----------------------------------------------------------------------
C R parity violating Supersymmetric Decays and Matrix Elements by
C                          P. Richardson(#)
C-----------------------------------------------------------------------
C matrix element corrections to top decay and Drell-Yan type processes
C                         by G. Corcella(")
C-----------------------------------------------------------------------
C Deep Inelastic Scattering and Heavy Flavour Electroproduction by
C                  G. Abbiendi(@) and L. Stanco(%)
C-----------------------------------------------------------------------
C and Jet Photoproduction in Lepton-Hadron Collisions by J. Chyla(~)
C-----------------------------------------------------------------------
C(*)  Department of Physics & Astronomy, University of Edinburgh
C(+)  Dipartimento di Fisica, Universita di Milano-Bicocca
C($)  Department of Physics & Astronomy, University of Manchester
C(#)  Cavendish Laboratory, Cambridge
C(&)  Theory Division, CERN
C(X)  Institute of Particle Physics Phenomenology, University of Durham
C(^)  Theory Group, KEK
C(")  Max Planck Institute, Munich
C(@)  Dipartimento di Fisica, Universita di Bologna
C(%)  Dipartimento di Fisica, Universita di Padova
C(~)  Institute of Physics, Prague
C-----------------------------------------------------------------------
C                  Version 6.504 - 12 August 2003
C-----------------------------------------------------------------------
C Main references:
C
C    G.Corcella, I.G.Knowles, G.Marchesini, S.Moretti, K.Odagiri,
C    P.Richardson, M.H.Seymour and B.R.Webber, JHEP 0101 (2001) 010
C
C    G.Marchesini,  B.R.Webber,  G.Abbiendi,  I.G.Knowles,  M.H.Seymour,
C    and L.Stanco, Computer Physics Communications 67 (1992) 465.
C-----------------------------------------------------------------------
C Please see the official HERWIG information page:
C    http://hepwww.rl.ac.uk/theory/seymour/herwig/
C-----------------------------------------------------------------------
CDECK  ID>, CIRCEE.
*CMZ :-        -03/07/01  17.07.47  by  Bryan Webber
*-- Author :    Bryan Webber
C-----------------------------------------------------------------------
      FUNCTION CIRCEE (X1, X2)
C-----------------------------------------------------------------------
C     DUMMY FUNCTION: DELETE AND SET CIRCOP NON-ZERO
C     IN MAIN PROGRAM IF YOU USE CIRCE BEAM SPECTRUM PACKAGE
C-----------------------------------------------------------------------
      DOUBLE PRECISION CIRCEE, X1, X2
      WRITE (6,10)
   10 FORMAT(/10X,'CIRCEE CALLED BUT NOT LINKED')
      CIRCEE = 0.0D0
      STOP
      END

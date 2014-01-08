!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE Init_MB_Xml
 USE M_MB_Xml
 USE M_MB_Xml_TREE
 IMPLICIT NONE
!
!>> Initialisation of Module  M_MB_Xml
   Nb_xml_ele     = 0
   Nb_xob         = 0
   Nb_xob_ind     = 0
   Xml_car(1)     = 'A'
   Xml_car(2)     = 'r'
   Xml_car(3)     = 'e'
   Xml_car(4)     = 'f'
   Xml_car(5)     = 'I'
   Xml_car(6)     = 'R'
   Xml_car(7)     = '0'
   Xml_car(8)     = '.'
   Xml_car(9)     = '#'
   Nb_xml_var     = 0
   Ntot_xml_car   = 9
   Nb_xml_vis     = 0
   Nb_xml_digit   = 0
   Nadd_xml_files = 0
   Nam_xml_top    = 'No AGDD xml file has been read yet'
   Current_xml_file = ''
   L_xml_tree_is_formed = .false.
   L_xml_ignore_boolean = .false.
!
   IF( ASSOCIATED(Iele_xob)     ) DEALLOCATE(Iele_xob)
   IF( ASSOCIATED(Iob_xob_sup)  ) DEALLOCATE(Iob_xob_sup)
   IF( ASSOCIATED(Info_xob)     ) DEALLOCATE(Info_xob)
   IF( ASSOCIATED(Iob_xob_inf0) ) DEALLOCATE(Iob_xob_inf0)
   IF( ASSOCIATED(Iob_xob_inf1) ) DEALLOCATE(Iob_xob_inf1)
   IF( ASSOCIATED(Ibeg_xob_att) ) DEALLOCATE(Ibeg_xob_att)
   IF( ASSOCIATED(Iend_xob_att) ) DEALLOCATE(Iend_xob_att)
   IF( ASSOCIATED(Iob_xob_ref)  ) DEALLOCATE(Iob_xob_ref)
   ALLOCATE( Iele_xob(    Mxmlo) )
   ALLOCATE( Iob_xob_sup( Mxmlo) )
   ALLOCATE( Info_xob(    Mxmlo) )
   ALLOCATE( Iob_xob_inf0(Mxmlo) )
   ALLOCATE( Iob_xob_inf1(Mxmlo) )
   ALLOCATE( Ibeg_xob_att(Mxmlo,Mxmla) )
   ALLOCATE( Iend_xob_att(Mxmlo,Mxmla) )
   ALLOCATE( Iob_xob_ref( Mxmlo,Mxmlr) )
   IF( ASSOCIATED(Ipos_xml_var) ) DEALLOCATE(Ipos_xml_var)
   IF( ASSOCIATED(Val_xml_var)  ) DEALLOCATE(Val_xml_var)
   ALLOCATE( Ipos_xml_var(2,Mxmlv) )
   ALLOCATE( Val_xml_var(   Mxmlv) )
!
   CALL ALLOC_info
!>> Initialisation of Module  M_MB_Xml_TREE
   NTree      = 0
   L_DIS_TREE = 0
   L_ENV_TREE = 0
!
 END SUBROUTINE Init_MB_Xml

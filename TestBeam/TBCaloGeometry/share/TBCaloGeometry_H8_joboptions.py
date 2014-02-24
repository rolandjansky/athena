if not 'TBCALOGEOMETRY_TBCALOGEOMETRY_HAS_BEEN_INCLUDED' in dir():
    TBCALOGEOMETRY_TBCALOGEOMETRY_HAS_BEEN_INCLUDED = 1
    print "now executing TBCaloGeometry - for Atlas will do nothing"

#    theApp.Dlls += [ "TBCaloGeometry" ]
    
    ToolSvc = Service( "ToolSvc" )
    from TBCaloGeometry.TBCaloGeometryConf import TBCaloCoordinate
    ToolSvc += TBCaloCoordinate()
    # ------------------------------------------------
    # TBCaloCoordinate :  --> AlgTool Properties
    #
    #  The numbers called calo_xxx correspond to a global shift
    #  of the calorimeter enveloppe, added on top of the known
    #  movements... for H8 data only !!!! ( DB Read = 0 belo )
    #
    #  A Transform is : first a rotation around z (phi angle),
    #                   then a rotation around y (theta angle),
    #                   then a rotation around x (psi angle, not used in G4)
    #                   then translation ( 3 coordinates, in mm )
    #

    # phi shift observed with respect to Pix+SCT, release 11.0.3
    ToolSvc.TBCaloCoordinate.calo_phi_shift= -0.012

    # eta observed with respect to Pix+SCT, release 11.0.3
    # this number has to be updated because the transformations are
    # now applied in a different order.
    ToolSvc.TBCaloCoordinate.calo_theta_shift= -0.025

    # not used/needed yet, release 10.0.0
    ToolSvc.TBCaloCoordinate.calo_psi_shift= 0.
    ToolSvc.TBCaloCoordinate.calo_y_shift= 0.
    ToolSvc.TBCaloCoordinate.calo_z_shift= 0.
    ToolSvc.TBCaloCoordinate.calo_x_shift= 0.
    #
    # ------------------------------------------------
    # These properties define if the CondDB/Pool should be used or
    # not to overwrite the default table_eta : 0 = Yes, -1 = No

    if (globalflags.DetGeo == 'atlas'):
        ToolSvc.TBCaloCoordinate.DBRead=-1
        ToolSvc.TBCaloCoordinate.PoolRead=-1

    else:
        
        # the tool which knows how to read the table position in DB
        class CondDBCool:
           TILE="<dbConnection>oracle://ATLAS_COOLPROD;schema=ATLAS_COOLONL_TILE;dbname=TBDP200</dbConnection>"
#           TILE="<dbConnection>COOLONL_TILE/TBDP200</dbConnection>"
#           TILE="<dbConnection>impl=cool;techno=logical;schema=COOLONL_TILE;X:TBPROD</dbConnection>"

        include ("TBCaloConditions/TBCaloCoolConditions_jobOptions.py")
        # the tool which knows how to read the table position in Pool
        include ("TBDetDescrAlg/TBDetDescrLoader_jobOptions.py")

        if (globalflags.DataSource() == 'data'):
            ToolSvc.TBCaloCoordinate.DBRead=-1
            ToolSvc.TBCaloCoordinate.PoolRead=0
        else:
            ToolSvc.TBCaloCoordinate.DBRead=-1
            ToolSvc.TBCaloCoordinate.PoolRead=0

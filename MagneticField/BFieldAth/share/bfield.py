#--------------------------------------------------------------
# BFieldAth
#
if not 'MuonLayout' in dir():
    # default value
    MagFieldAthenaSvc.NameOfTheBfieldFile = "bmagatlas02.data"
else:
    if ( MuonLayout == "P03-DC2" or MuonLayout == "P03-DC1" ):
        MagFieldAthenaSvc.NameOfTheBfieldFile = "bmagatlas02.data"
    else:
        if (MuonLayout == "Q02_initial" or MuonLayout == "Q02") :
#           MagFieldAthenaSvc.NameOfTheBfieldFile = "bmagatlas03_test.data"
            MagFieldAthenaSvc.NameOfTheBfieldFile = "bmagatlas03_test2.data"
        else:
            if (MuonLayout == "CTB2004" ):
                print " WARNING magnetic field map for testbeam !!!! "
                MagFieldAthenaSvc.NameOfTheBfieldFile = "mbps1-all-id-800-mbps2-muons-800.data"
            else:
                raise RuntimeError, 'ERROR : Unknown  MuonLayout allowed values are \nP03-DC1\nP03-DC2\nQ02_initial\nQ02\nCTB2004'

print "Selecting B field map ",MagFieldAthenaSvc.NameOfTheBfieldFile

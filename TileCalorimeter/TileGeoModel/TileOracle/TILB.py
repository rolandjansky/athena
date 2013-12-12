# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################
# TILB.sql generator
# ---------------------------------------------------------------------------------------------------------------
def TILBHeader(num):
    outfile = open("TILB.sql","wt")
    outfile.write("SET echo OFF;\n")
    outfile.write("set linesize 132;\n")
#   outfile.write("drop   table TILB_data2tag cascade constraints;\n")
#   outfile.write("drop   table TILB_data cascade constraints;\n")
    outfile.write("create table TILB_data (\n")
    outfile.write("TILB_data_id     number(10),\n")
    outfile.write("SECTION          number(10),\n")
    outfile.write("NPERIOD          number(10),\n")
    outfile.write("NMODUL           number(10),\n")
    outfile.write("RMIN             float(63),\n")
    outfile.write("RMAX             float(63),\n")
    outfile.write("RMINIMAL         float(63),\n")
    outfile.write("RMAXIMAL         float(63),\n")
    outfile.write("DZPERIO          float(63),\n")
    outfile.write("DRFRONT          float(63),\n")
    outfile.write("DZEND            float(63),\n")
    outfile.write("FLANGEX          float(63),\n")
    outfile.write("FLANGEY          float(63),\n")
    outfile.write("ZOFFSET          float(63),\n")
    outfile.write("PHIGAP           float(63),\n")
    outfile.write("ISCI_ST          number(10),\n")
    outfile.write("DZMODUL          float(63),\n")
    outfile.write("DZMAST           float(63),\n")
    outfile.write("DZSPAC           float(63),\n")
    outfile.write("DZEND1           float(63),\n")
    outfile.write("DZEND2           float(63),\n")
    outfile.write("NGIRDER          number(10),\n")
    outfile.write("NSCIN            number(10),\n")
    outfile.write("FINGPATTERN      number(10),\n")
    outfile.write("NPERCUTPOS       number(10),\n")
    outfile.write("NPERCUTNEG       number(10),\n")
    outfile.write("CURSCINT         number(10),\n")
    outfile.write("DZGIR            float(63)\n")
    outfile.write(") ;\n")
    outfile.write("\n")
    outfile.write("alter table TILB_data add constraint TILB_data_pk\n")
    outfile.write("primary key (TILB_data_id);\n")
    outfile.write("\n")
    outfile.write("create table TILB_data2tag (\n")
    outfile.write("  TILB_vers      varchar2(255),\n")
    outfile.write("  TILB_data_id   number(10)\n")
    outfile.write(") ;\n")
    
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
def TILBComments(num):
    outfile = open("TILB.sql","awt")
    outfile.write("\n")
    outfile.write("comment on column TILB_data.TILB_data_id is 'TileCal TILB, Section';\n")
    outfile.write("comment on column TILB_data.SECTION  is '1-barrel, 2-extended, 3-ITC1, 4-ITC2, 5-Gap, 6-Crack';\n")
    outfile.write("comment on column TILB_data.NPERIOD  is 'Number of periods in the module';\n")
    outfile.write("comment on column TILB_data.NMODUL   is 'Number of modules,in full cylinder, ")
    outfile.write("used for Dphi calcuations only  (Dphi = 2 pi / Nmodules)';\n")
    outfile.write("comment on column TILB_data.RMIN     is 'Minimal active radius';\n")
    outfile.write("comment on column TILB_data.RMAX     is 'Maximal active radius';\n")
    outfile.write("comment on column TILB_data.RMINIMAL is 'Minimal radius of section mother (G4)';\n")
    outfile.write("comment on column TILB_data.RMAXIMAL is 'Maximal radius of section mother (G4)';\n")
    outfile.write("comment on column TILB_data.DZPERIO  is 'Dummy';\n")
    outfile.write("comment on column TILB_data.DRFRONT  is 'Thickness of the front plate (in R)';\n")
    outfile.write("comment on column TILB_data.DZEND    is 'Thickness of the endplates (in Z) if they are equal';\n")
    outfile.write("comment on column TILB_data.FLANGEX  is 'End plate square hole dimension or min width for gap envelope';\n")
    outfile.write("comment on column TILB_data.FLANGEY  is 'End plate square hole position or max width for gap envelope';\n")
    outfile.write("comment on column TILB_data.ZOFFSET  is 'Offset of the module center along Z';\n")
    outfile.write("comment on column TILB_data.PHIGAP   is 'Design gap between modules';\n")
    outfile.write("comment on column TILB_data.ISCI_ST  is 'ID of the first scintillator';\n")
    outfile.write("comment on column TILB_data.DZMODUL  is 'Z-length of the module';\n")
    outfile.write("comment on column TILB_data.DZMAST   is 'Master plate thickness';\n")
    outfile.write("comment on column TILB_data.DZSPAC   is 'Spacer thickness';\n")
    outfile.write("comment on column TILB_data.DZEND1   is 'Thickens of the end plate 1';\n")
    outfile.write("comment on column TILB_data.DZEND2   is 'Thickens of the end plate 2';\n")
    outfile.write("comment on column TILB_data.NGIRDER  is 'Number of girder elements';\n")
    outfile.write("comment on column TILB_data.NSCIN    is 'Number of scintillators in the period';\n")
    outfile.write("comment on column TILB_data.FINGPATTERN  is 'Finger type of 2 fingers in barrel or one ")
    outfile.write("finger in ext.barrel standard barrel fingers: 11 (both L and R present), ")
    outfile.write("10 (L), 13 (R), 1 (no L, no R), standard ext.barrel fingers: 2 (finger present) or 0';\n")
    outfile.write("comment on column TILB_data.NPERCUTPOS   is 'number of cut periods at higher Z ( Z > 0 )';\n")
    outfile.write("comment on column TILB_data.NPERCUTNEG   is 'number of cut periods at lower Z side ( Z < 0 )';\n")
    outfile.write("comment on column TILB_data.CURSCINT     is 'scintillator number to describe special ITC cells';\n")
    outfile.write("comment on column TILB_data.DZGIR        is 'length of girder in special ITC cells';\n")
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
def TILB(num):
    import string 
    inpfile = open("TILB-GEO-01","rt")
    outfile = open("TILB.sql","awt")
    line = modname = comment = volline = cindx = ' '
    nmax = 0
    n1 = n2 = n3 = n15 = n21 = n22 = n23 = n24 = n25 = n26 = 0
    r4 = r5 = r6 = r7 = r8 = r9 = r10 = 0.0
    r11 = r12 = r13 = r14 = r16 = r17 = r18 = r19 = r20 = r27 = 0.0

    outfile.write("\n")

    indx = -1
    while 1 :
        line = inpfile.readline()
        indx = indx + 1
        cindx = str(indx)
        if line == "":
            break

        name = string.split(line)
        nmax = len(name)
        try:
            if nmax < 1:
                raise IndexError
            if nmax > 27:
                raise IndexError

            if name[0] == "###":
                break
            
            n1 = int(name[0])
            n2 = int(name[1])
            n3 = int(name[2])
            r4 = float(name[3])    
            r5 = float(name[4])
            r6 = float(name[5])
            r7 = float(name[6])
            r8 = float(name[7])
            r9 = float(name[8])
            r10 = float(name[9])
            r11 = float(name[10])
            r12 = float(name[11])
            r13 = float(name[12])
            r14 = float(name[13])
            n15 = int(name[14])
            r16 = float(name[15]) 
            r17 = float(name[16])
            r18 = float(name[17])
            r19 = float(name[18])
            r20 = float(name[19])
            n21 = int(name[20])
            n22 = int(name[21])
            n23 = int(name[22])
            n24 = int(name[23])
            n25 = int(name[24])
            n26 = int(name[25])
            r27 = float(name[26])
                                
            volline = str(indx)+","+str(n1)+","+str(n2)+","+str(n3)+","+str(r4)+","+str(r5)+","+str(r6)
            volline = volline+","+str(r7)+","+str(r8)+","+str(r9)+","+str(r10)+","+str(r11)+","+str(r12)
            volline = volline+","+str(r13)+","+str(r14)+","+str(n15)+","+str(r16)+","+str(r17)+","+str(r18)
            volline = volline+","+str(r19)+","+str(r20)+","+str(n21)+","+str(n22)+","+str(n23)+","+str(n24)
            volline = volline+","+str(n25)+","+str(n26)+","+str(r27)

            outfile.write("insert into TILB_data (TILB_data_id,SECTION,NPERIOD,NMODUL,RMIN,RMAX,RMINIMAL,RMAXIMAL,\n")
            outfile.write("DZPERIO,DRFRONT,DZEND,FLANGEX,FLANGEY,ZOFFSET,PHIGAP,ISCI_ST,DZMODUL,DZMAST,DZSPAC,\n")
            outfile.write("DZEND1,DZEND2,NGIRDER,NSCIN,FINGPATTERN,NPERCUTPOS,NPERCUTNEG,CURSCINT,DZGIR) values (\n")
            outfile.write(volline)
            outfile.write(");\n")
            outfile.write("insert into TILB_data2tag values ('TILB-GEO-01', ")
            outfile.write(cindx)
            outfile.write(");\n")
            
        except IndexError: # coments are in line
            print ' IndexError : ',nmax,' ',name
            
    inpfile.close()
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
print 'Tile TILB wirh python '
print ' Tile Header',TILBHeader('Header')
print ' Tile Values ',TILB('TILB-GEO-01')
print ' Tile Comments',TILBComments('Comments')
  

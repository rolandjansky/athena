# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################
# SCNT.sql generator
# ---------------------------------------------------------------------------------------------------------------
def SCNTHeader(num):
    outfile = open("SCNT.sql","wt")
    outfile.write("SET echo OFF;\n")
    outfile.write("set linesize 132;\n")
#   outfile.write("drop   table SCNT_data2tag cascade constraints;\n")
#   outfile.write("drop   table SCNT_data cascade constraints;\n")
    outfile.write("create table SCNT_data (\n")
    outfile.write("SCNT_data_id number(10),\n")
    outfile.write("ITEM         number(10),\n")
    outfile.write("DR           float(63),\n")
    outfile.write("RC           float(63),\n")
    outfile.write("ZP           float(63),\n")
    outfile.write("DRW          float(63),\n")
    outfile.write("DT           float(63),\n")
    outfile.write("DTW          float(63),\n")
    outfile.write("DPHI         float(63) \n")
    outfile.write(") ;\n")
    outfile.write("\n")
    outfile.write("alter table SCNT_data add constraint SCNT_data_pk\n")
    outfile.write("primary key (SCNT_data_id);\n")
    outfile.write("\n")
    outfile.write("create table SCNT_data2tag (\n")
    outfile.write("  SCNT_vers      varchar2(255),\n")
    outfile.write("  SCNT_data_id   number(10)\n")
    outfile.write(") ;\n")
    
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
def SCNTComments(num):
    outfile = open("SCNT.sql","awt")
    outfile.write("\n")
    outfile.write("comment on column SCNT_data.SCNT_data_id is 'TileCal SCNT, Section';\n")
    outfile.write("comment on column SCNT_data.ITEM is 'Scintillator number:")
    outfile.write(" 1-bar,2-ext,3-ITC1,4-ITC2,5-Gap,6-Crack';\n")
    outfile.write("comment on column SCNT_data.DR   is 'Dimension along R';\n")
    outfile.write("comment on column SCNT_data.RC   is 'R position with respect to the lower edge of module';\n")
    outfile.write("comment on column SCNT_data.ZP   is 'Z position';\n")
    outfile.write("comment on column SCNT_data.DRW  is 'Radial space for wrapping';\n")
    outfile.write("comment on column SCNT_data.DT   is 'Scintillator thickness in Z';\n")
    outfile.write("comment on column SCNT_data.DTW  is 'Wrapping thickness in Z';\n")
    outfile.write("comment on column SCNT_data.DPHI is 'Distance between scintillator and absorber edge in phi';\n")
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
def SCNT(num):
    import string 
    inpfile = open("SCNT-GEO-03","rt")
    outfile = open("SCNT.sql","awt")
    line =modname =comment =volline =cindx =' '
    nmax =0
    n1 =n2 =0
    r3 =r4 =r5 =r6 =r7 =r8 =r9 =0.0

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
            if nmax > 9:
                raise IndexError

            if name[0] == "###":
                break

            n1 =   int(name[0])
            r2 = float(name[1])
            r3 = float(name[2])
            r4 = float(name[3])    
            r5 = float(name[4])
            r6 = float(name[5])
            r7 = float(name[6])
            r8 = float(name[7])
                                
            volline = str(indx)+","+str(n1)+","+str(r2)+","+str(r3)+","+str(r4)+","+str(r5)
            volline =   volline+","+str(r6)+","+str(r7)+","+str(r8)

            outfile.write("insert into SCNT_data (SCNT_data_id, ITEM, DR,RC,ZP,DRW,DT,DTW,DPHI) values (\n")
            outfile.write(volline)
            outfile.write(");\n")
            outfile.write("insert into SCNT_data2tag values ('SCNT-GEO-03', ")
            outfile.write(cindx)
            outfile.write(");\n")
            
        except IndexError: # coments are in line
            print ' IndexError : ',nmax,' ',name
            
    inpfile.close() 
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
print 'Tile SCNT wirh python '
print ' Tile Header', SCNTHeader('Header')
print ' Tile Values ',SCNT('SCNT-GEO-03')
print ' Tile Coments',SCNTComments('Comments')


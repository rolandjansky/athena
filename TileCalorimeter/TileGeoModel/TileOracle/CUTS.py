# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################
# CUTS.sql generator
# ---------------------------------------------------------------------------------------------------------------
def CUTSHeader(num):
    outfile = open("CUTS.sql","wt")
    outfile.write("SET echo OFF;\n")
    outfile.write("set linesize 132;\n")
#   outfile.write("drop   table CUTS_data2tag cascade constraints;\n")
#   outfile.write("drop   table CUTS_data cascade constraints;\n")
    outfile.write("create table CUTS_data (\n")
    outfile.write("CUTS_data_id number(10),\n")
    outfile.write("volume       varchar2(255),\n")
    outfile.write("dX1          float(63),\n")
    outfile.write("dX2          float(63),\n")
    outfile.write("dY1          float(63),\n")
    outfile.write("dY2          float(63),\n")
    outfile.write("dZ1          float(63),\n")
    outfile.write("Xpos         float(63),\n")
    outfile.write("Ypos         float(63) \n")
    outfile.write(") ;\n")
    outfile.write("\n")
    outfile.write("alter table CUTS_data add constraint CUTS_data_pk\n")
    outfile.write("primary key (CUTS_data_id);\n")
    outfile.write("\n")
    outfile.write("create table CUTS_data2tag (\n")
    outfile.write("  CUTS_vers      varchar2(255),\n")
    outfile.write("  CUTS_data_id   number(10)\n")
    outfile.write(") ;\n")
    
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
def CUTSComments(num):
    outfile = open("CUTS.sql","awt")
    outfile.write("\n")
    outfile.write("comment on column CUTS_data.CUTS_data_id is 'TileCal CUTS, Section';\n")
    outfile.write("comment on column CUTS_data.volume   is 'Volume name';\n")
    outfile.write("comment on column CUTS_data.dX1      is 'TRD shape, dX1/2 [mm]';\n")
    outfile.write("comment on column CUTS_data.dX2      is 'TRD shape, dX2/2 [mm]';\n")
    outfile.write("comment on column CUTS_data.dY1      is 'TRD shape, dY1/2 [mm]';\n")
    outfile.write("comment on column CUTS_data.dY2      is 'TRD shape, dY2/2 [mm]';\n")
    outfile.write("comment on column CUTS_data.dZ1      is 'TRD shape, dZ1/2 [mm]';\n")
    outfile.write("comment on column CUTS_data.Xpos     is 'X Position [mm]';\n")
    outfile.write("comment on column CUTS_data.Ypos     is 'Y Position [mm]';\n")
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
def CUTS(num):
    import string 
    inpfile = open("CUTS-00","rt")
    outfile = open("CUTS.sql","awt")
    line =volname =modname =comment =volline =cindx =' '
    nmax =0
    n1 =0
    volname =' '
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

            volname =  name[1]
            r2 = float(name[2])
            r3 = float(name[3])    
            r4 = float(name[4])
            r5 = float(name[5])
            r6 = float(name[6])
            r7 = float(name[7])
            r8 = float(name[8])
                                
            volline = str(indx)+",'"+volname+"',"+str(r2)+","+str(r3)+","+str(r4)+","+str(r5)
            volline =   volline+","+str(r6)+","+str(r7)+","+str(r8)

            outfile.write("insert into CUTS_data (CUTS_data_id, volume,dX1,dX2,dY1,dY2,dZ1,Xpos,Ypos) values (\n")
            outfile.write(volline)
            outfile.write(");\n")
            outfile.write("insert into CUTS_data2tag values ('CUTS-00', ")
            outfile.write(cindx)
            outfile.write(");\n")
            
        except IndexError: # coments are in line
            print ' IndexError : ',nmax,' ',name
            
    inpfile.close() 
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
print 'Tile CUTS wirh python '
print ' Tile Header', CUTSHeader('Header')
print ' Tile Values ',CUTS('CUTS-00')
print ' Tile Coments',CUTSComments('Comments')
  

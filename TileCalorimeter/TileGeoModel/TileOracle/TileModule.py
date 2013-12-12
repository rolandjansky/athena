# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###########################
# TileModule.sql generator
# ---------------------------------------------------------------------------------------------------------------
def TileHeader(num):
    outfile = open("TileModule.sql","wt")
    outfile.write("SET echo OFF;\n")
    outfile.write("#set linesize 132;\n")
#   outfile.write("drop   table TileModule_data2tag cascade constraints;\n")
#   outfile.write("drop   table TileModule_data cascade constraints;\n")
    outfile.write("create table TileModule_data (\n")
    outfile.write("TileModule_data_id     number(10),\n")
    outfile.write("PART                   number(10),\n")
    outfile.write("MODNUM                 number(10),\n")
    outfile.write("NAM                    varchar2(255),\n")
    outfile.write("TYP                    number(10),\n")
    outfile.write("COMM                   varchar2(255)\n")
    outfile.write(") ;\n")
    outfile.write("\n")
    outfile.write("alter table TileModule_data add constraint TileModule_data_pk\n")
    outfile.write("primary key (TileModule_data_id);\n")
    outfile.write("\n")
    outfile.write("create table TileModule_data2tag (\n")
    outfile.write("  TileModule_vers      varchar2(255),\n")
    outfile.write("  TileModule_data_id   number(10)\n")
    outfile.write(") ;\n")
    
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
def TileComments(num):
    outfile = open("TileModule.sql","awt")
    outfile.write("\n")
    outfile.write("comment on column TileModule_data.TileModule_data_id is ")
    outfile.write("'Tile Modules description, version 05';\n")
    outfile.write("comment on column TileModule_data.PART                is ")
    outfile.write("'Tile parts 0-TB Barrel, 1-Barrel, 2-Ext.Bar Neg, 3-Ext.Bar Pos, 4 - ITC Neg, 5 - ITC Pos';\n")
    outfile.write("comment on column TileModule_data.MODNUM              is 'Module index';\n")
    outfile.write("comment on column TileModule_data.NAM                 is 'Module name (production label)';\n")
    outfile.write("comment on column TileModule_data.TYP                 is 'Module type of contents 1,2,6050403';\n")
    outfile.write("comment on column TileModule_data.COMM                is 'Module Comments';\n")
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
def TileModule(num):
    import string 
    inpfile = open("TileModule-GEO-00","rt")
    outfile = open("TileModule.sql","awt")
    line = modname = comment = volline = cindx = ' '
    n1 = n2 = n3 = nmax = 0

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
            if nmax < 4:
                raise IndexError
            if nmax > 5:
                raise IndexError

            n1 = int(name[0])
            n2 = int(name[1])
            modname = name[2]
            n3 = int(name[3])

            if nmax >= 4:
                comment = ''
                                
            if nmax == 5:
                comment = name[4]

            volline = str(indx)+","+str(n1)+","+str(n2)+",'"+modname+"',"+str(n3)+",'"+comment+"'"
                        
            outfile.write("insert into TileModule_data (TileModule_data_id,PART,MODNUM,NAM,TYP,COMM) values (\n")
            outfile.write(volline)
            outfile.write(");\n")
            outfile.write("insert into TileModule_data2tag values ('TileModule-GEO-00', ")
            outfile.write(cindx)
            outfile.write(");\n")
            
        except IndexError: # coments are in line
            print ' IndexError : ',nmax,' ',name
            
    inpfile.close()
    outfile.close()
    return num

# ---------------------------------------------------------------------------------------------------------------
print 'Tile wirh python '
print ' Tile Header',TileHeader('Header')
print ' Tile Values ',TileModule('TileModule-GEO-00')
print ' Tile Coments',TileComments('Comments')


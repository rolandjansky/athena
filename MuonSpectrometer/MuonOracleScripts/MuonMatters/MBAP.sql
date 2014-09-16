SET echo OFF;
set linesize 132;
drop   table MBAP_data2tag cascade constraints;
drop   table MBAP_data cascade constraints;
create table MBAP_data (
  MBAP_data_id   number(10),
P12S1DZ1       float(63),
P12S1DZ2       float(63),
P23S1DZ1       float(63),
P23S1DZ2       float(63),
P34S1DZ1       float(63),
P34S1DZ2       float(63),
P14S3DZ       float(63),
P14S3DX       float(63),
PLAS1DX1       float(63),
PLAS1DX2       float(63),
PFLHE       float(63),
PS01POSX       float(63),
PS01POSY       float(63),
PS11POSX       float(63),
PS11POSY       float(63),
PS14POSX       float(63),
PS14POSY       float(63),
PS14ANGZ       float(63),
P12POSZ       float(63),
P23POSZ       float(63),
P34POSZ       float(63),
XYZREF       float(63),
PFL12XA       float(63),
PFL23XA       float(63),
PFL34XA       float(63),
PFLYA       float(63),
PFL12XB       float(63),
PFL23XB       float(63),
PFL34XB       float(63),
PFLYB       float(63),
PFL12XC       float(63),
PFL23XC       float(63),
PFL34XC       float(63),
PFLYC       float(63),
PFL12XD       float(63),
PFL23XD       float(63),
PFL34XD       float(63),
PFLYD       float(63),
PLATRARO       float(63),
PLATRARI       float(63),
PLATRAHE       float(63),
PL12RAZE       float(63),
PL23RAZE       float(63),
PL34RAZE       float(63),
PLATRHYP       float(63),
PLATBTLE       float(63),
PRLS01DX       float(63),
PRLS11DX       float(63),
PRLS01DY       float(63),
PRLS11DY       float(63),
PSSS3DY1       float(63),
PSSS3DY2       float(63),
PSSS3DX1       float(63),
PSSS3DX2       float(63),
PSSS3DX3       float(63),
PSSS3AN       float(63),
PSSS3ZP1       float(63),
PSSS3ZP2       float(63),
PSSS3ZP3       float(63),
PSSS3ZP4       float(63),
PSSS3ZP5       float(63),
PSSS3ZP6       float(63),
PLATRAZ1       float(63),
PLATRAY1       float(63),
PLATRAZ2       float(63),
PLATRAY2       float(63),
PL12RAZ3       float(63),
PL23RAZ3       float(63),
PL34RAZ3       float(63),
PLATRAY3       float(63),
PL12RAZ4       float(63),
PL23RAZ4       float(63),
PL34RAZ4       float(63),
PLATRAY4       float(63),
PL12RAZ0       float(63),
PL23RAZ0       float(63),
PL34RAZ0       float(63),
PLATRAY0       float(63),
PL12RHZP       float(63),
PL23RHZP       float(63),
PL34RHZP       float(63),
PLATRVZE       float(63),
PLATRVYP       float(63),
PLATCRYP       float(63),
PL12CRZ1       float(63),
PL12CRZ2       float(63),
PL23CRZ1       float(63),
PL23CRZ2       float(63),
PL34CRZ1       float(63),
PL34CRZ2       float(63),
PL12CRAN       float(63),
PL23CRAN       float(63),
PL34CRAN       float(63),
PL12CRLE       float(63),
PL23CRLE       float(63),
PL34CRLE       float(63),
PLAT2RAX       float(63),
PL12BTZ1       float(63),
PL23BTZ1       float(63),
PL34BTZ1       float(63),
PL12BTZ2       float(63),
PL23BTZ2       float(63),
PL34BTZ2       float(63),
PLATBTXP       float(63),
PRLS01XP       float(63),
PRLS11XP       float(63),
PRLS01YP       float(63),
PRLS11YP       float(63),
PL12RLZ1       float(63),
PL23RLZ1       float(63),
PL34RLZ1       float(63),
PL12RLZ2       float(63),
PL23RLZ2       float(63),
PL34RLZ2       float(63),
PRLS01AN       float(63),
PRLS11AN       float(63),
PRLS01LE       float(63),
PRLS11LE       float(63),
PLATBSXP       float(63),
PLATBTLS       float(63),
PL12FRZP       float(63),
PL23FRZP       float(63),
PL34FRZP       float(63),
PLATFRYP       float(63),
PLATFRXP       float(63),
PSSS3DYT       float(63),
PSSS3DXT       float(63),
PSSS3X0       float(63),
PSSS3Y0       float(63),
PSSS3X1       float(63),
PSSS3Y1       float(63),
PSSS3X2       float(63),
PSSS3Y2       float(63),
PSSS3X3       float(63),
PSSS3Y3       float(63),
PSSS3X4       float(63),
PSSS3Y4       float(63),
PSSS3X5       float(63),
PSSS3Y5       float(63),
PSSS3X6       float(63),
PSSS3Y6       float(63),
PSSRAXP       float(63),
PSSRAYP       float(63),
PSSZRP1       float(63),
PSSZRP2       float(63),
PSSZRP3       float(63),
PSSZRP4       float(63),
PSSZRP5       float(63),
PSSZRP6       float(63),
PRHS3ZP1       float(63),
PRHS3ZP2       float(63),
PRHS3ZP3       float(63),
PRHS3ZP4       float(63),
PRHS3ZP5       float(63),
PRHS3ZP6       float(63),
PRHS3ZP7       float(63),
PL14RAZE       float(63),
PL14RAZP       float(63),
PL14RAY1       float(63),
PL14RAY2       float(63),
PL14RVZE       float(63),
PL14RVYP       float(63),
PL14CRY1       float(63),
PL14CRY2       float(63),
P12S3CRA       float(63),
P12S3CRL       float(63),
P23S3CRA       float(63),
P23S3CRL       float(63),
P34S3CRA       float(63),
P34S3CRL       float(63),
PL14CRZ1       float(63),
PL14CRZ2       float(63),
PL14CRZ3       float(63),
PL14CRZ4       float(63),
PL14CRZ5       float(63),
PL14CRZ6       float(63),
PL14CRZ7       float(63),
PL14CRZ8       float(63),
PL14CRZ9       float(63),
PL14CRZA       float(63),
P14S3BFL       float(63),
P14S3BFA       float(63),
P14S3BFX       float(63),
P14S3BFY       float(63),
P14S3FLA       float(63),
P14S3FLX       float(63),
P14S3FLY       float(63),
P14S3FLZ       float(63)
) ;

alter table MBAP_data add constraint MBAP_data_pk
primary key (MBAP_data_id);

create table MBAP_data2tag (
  MBAP_vers      varchar2(255),
  MBAP_data_id   number(10)
) ;

alter table MBAP_data2tag add constraint MBAP_data2tag_pk
primary key (MBAP_vers, MBAP_data_id);

alter table MBAP_data2tag add constraint MBAP_data2tag_vers_fk
foreign key (MBAP_vers) references hvs_nodevers (vers_name);
alter table MBAP_data2tag add constraint MBAP_data2tag_data_fk
foreign key (MBAP_data_id) references MBAP_data (MBAP_data_id);

delete from hvs_relation where CHILD_NODENAME = 'MBAP';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'MBAP';
delete from hvs_node     where NODE_NAME = 'MBAP';

insert into hvs_node     values ('MBAP','MuonSpectrometer', '');
insert into hvs_nodevers values ('MBAP', 'MBAP-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','MBAP','MBAP-01');
insert into MBAP_data (
MBAP_data_id,
P12S1DZ1,
P12S1DZ2,
P23S1DZ1,
P23S1DZ2,
P34S1DZ1,
P34S1DZ2,
P14S3DZ,
P14S3DX,
PLAS1DX1,
PLAS1DX2,
PFLHE,
PS01POSX,
PS01POSY,
PS11POSX,
PS11POSY,
PS14POSX,
PS14POSY,
PS14ANGZ,
P12POSZ,
P23POSZ,
P34POSZ,
XYZREF,
PFL12XA,
PFL23XA,
PFL34XA,
PFLYA,
PFL12XB,
PFL23XB,
PFL34XB,
PFLYB,
PFL12XC,
PFL23XC,
PFL34XC,
PFLYC,
PFL12XD,
PFL23XD,
PFL34XD,
PFLYD,
PLATRARO,
PLATRARI,
PLATRAHE,
PL12RAZE,
PL23RAZE,
PL34RAZE,
PLATRHYP,
PLATBTLE,
PRLS01DX,
PRLS11DX,
PRLS01DY,
PRLS11DY,
PSSS3DY1,
PSSS3DY2,
PSSS3DX1,
PSSS3DX2,
PSSS3DX3,
PSSS3AN,
PSSS3ZP1,
PSSS3ZP2,
PSSS3ZP3,
PSSS3ZP4,
PSSS3ZP5,
PSSS3ZP6,
PLATRAZ1,
PLATRAY1,
PLATRAZ2,
PLATRAY2,
PL12RAZ3,
PL23RAZ3,
PL34RAZ3,
PLATRAY3,
PL12RAZ4,
PL23RAZ4,
PL34RAZ4,
PLATRAY4,
PL12RAZ0,
PL23RAZ0,
PL34RAZ0,
PLATRAY0,
PL12RHZP,
PL23RHZP,
PL34RHZP,
PLATRVZE,
PLATRVYP,
PLATCRYP,
PL12CRZ1,
PL12CRZ2,
PL23CRZ1,
PL23CRZ2,
PL34CRZ1,
PL34CRZ2,
PL12CRAN,
PL23CRAN,
PL34CRAN,
PL12CRLE,
PL23CRLE,
PL34CRLE,
PLAT2RAX,
PL12BTZ1,
PL23BTZ1,
PL34BTZ1,
PL12BTZ2,
PL23BTZ2,
PL34BTZ2,
PLATBTXP,
PRLS01XP,
PRLS11XP,
PRLS01YP,
PRLS11YP,
PL12RLZ1,
PL23RLZ1,
PL34RLZ1,
PL12RLZ2,
PL23RLZ2,
PL34RLZ2,
PRLS01AN,
PRLS11AN,
PRLS01LE,
PRLS11LE,
PLATBSXP,
PLATBTLS,
PL12FRZP,
PL23FRZP,
PL34FRZP,
PLATFRYP,
PLATFRXP,
PSSS3DYT,
PSSS3DXT,
PSSS3X0,
PSSS3Y0,
PSSS3X1,
PSSS3Y1,
PSSS3X2,
PSSS3Y2,
PSSS3X3,
PSSS3Y3,
PSSS3X4,
PSSS3Y4,
PSSS3X5,
PSSS3Y5,
PSSS3X6,
PSSS3Y6,
PSSRAXP,
PSSRAYP,
PSSZRP1,
PSSZRP2,
PSSZRP3,
PSSZRP4,
PSSZRP5,
PSSZRP6,
PRHS3ZP1,
PRHS3ZP2,
PRHS3ZP3,
PRHS3ZP4,
PRHS3ZP5,
PRHS3ZP6,
PRHS3ZP7,
PL14RAZE,
PL14RAZP,
PL14RAY1,
PL14RAY2,
PL14RVZE,
PL14RVYP,
PL14CRY1,
PL14CRY2,
P12S3CRA,
P12S3CRL,
P23S3CRA,
P23S3CRL,
P34S3CRA,
P34S3CRL,
PL14CRZ1,
PL14CRZ2,
PL14CRZ3,
PL14CRZ4,
PL14CRZ5,
PL14CRZ6,
PL14CRZ7,
PL14CRZ8,
PL14CRZ9,
PL14CRZA,
P14S3BFL,
P14S3BFA,
P14S3BFX,
P14S3BFY,
P14S3FLA,
P14S3FLX,
P14S3FLY,
P14S3FLZ
) values (    1,
  3000.0000,  2500.0000,  2700.0000,  2200.0000,  2200.0000,  1700.0000,
  8600.0000,   540.0000,  1225.0000,   600.0000,    20.0000,  7800.0000,
 -1200.0000,  5000.0000, -6500.0000,  6730.0000,  5300.0000,    45.0000,
  3425.0000,  6691.2500,  9545.0000,     0.0000,  1500.0000,  1350.0000,
  1100.0000,     0.0000,  1500.0000,  1350.0000,  1100.0000,   600.0000,
  1250.0000,  1100.0000,   850.0000,   600.0000,  1250.0000,  1100.0000,
   850.0000,  1224.9000,    17.0000,    13.0000,  1200.0000,  3000.0000,
  2700.0000,  2200.0000,   600.0000,  1350.0000,   600.0000,   800.0000,
  1200.0000,   800.0000,   242.5000,   393.0000,   393.0000,  1432.0000,
   566.0000,    45.0000,  1975.0000,  4875.0000,  5400.0000,  7982.5000,
  8507.5000, 10582.5000,     0.0000,     0.0000,     0.0000,  1200.0000,
  3000.0000,  2700.0000,  2200.0000,  1200.0000,  3000.0000,  2700.0000,
  2200.0000,     0.0000,  1500.0000,  1350.0000,  1100.0000,     0.0000,
  1500.0000,  1350.0000,  1100.0000,  1200.0000,   600.0000,   600.0000,
   750.0000,  2250.0000,   675.0000,  2025.0000,   550.0000,  1650.0000,
    38.6598,    41.6335,    47.4896,  1920.8373,  1806.1392,  1627.7821,
  1259.0000,     0.0000,     0.0000,     0.0000,  3000.0000,  2700.0000,
  2200.0000,   675.0000,   959.0000,   859.0000,   600.0000,   400.0000,
     0.0000,     0.0000,     0.0000,  3000.0000,  2700.0000,  2200.0000,
    63.4349,    45.0000,  1341.6407,  1131.3708,   629.5000,  1259.0000,
 -1500.0000, -1350.0000, -1100.0000,   -27.0000,   -17.0000,   635.5000,
  2391.0000,  1195.5000,     0.0000,     0.0000,     0.0000,     0.0000,
   242.5000,   393.0000,   635.5000,  1825.0000,   635.5000,  2391.0000,
   242.5000,  2391.0000,     0.0000,  -635.5000,   393.0000,     0.0000,
  2900.0000,  3425.0000,  6007.5000,  6532.5000,  8607.5000,   725.0000,
  1450.0000,  2175.0000,  4070.6250,  4716.2500,  5361.8750,  7570.0000,
  8590.5000,  4312.2500,   716.0000,  1432.0000,  1398.0000,   716.0000,
   358.0000,  1074.0000,    44.6422,  1018.8607,    47.9587,   963.9994,
    34.6104,  1260.4801,   362.5000,  1087.5000,  1812.5000,  2537.5000,
  3747.8125,  4393.4375,  5039.0625,  5684.6875,  7051.2500,  8088.7500,
   555.7859,   -45.0000,  -439.0000,   196.5000,   -45.0000,  -419.9081,
   215.5919,  4303.7500
);
insert into MBAP_data2tag values ('MBAP-01',    1);

comment on column MBAP_data.MBAP_data_id is 'Unique identifier';
comment on column MBAP_data.P12S1DZ1   is '1353 P12S1DZ1                                   ';
comment on column MBAP_data.P12S1DZ2   is '1354 P12S1DZ2                                   ';
comment on column MBAP_data.P23S1DZ1   is '1355 P23S1DZ1                                   ';
comment on column MBAP_data.P23S1DZ2   is '1356 P23S1DZ2                                   ';
comment on column MBAP_data.P34S1DZ1   is '1357 P34S1DZ1                                   ';
comment on column MBAP_data.P34S1DZ2   is '1358 P34S1DZ2                                   ';
comment on column MBAP_data.P14S3DZ    is '1359 P14S3DZ                                    ';
comment on column MBAP_data.P14S3DX    is '1360 P14S3DX                                    ';
comment on column MBAP_data.PLAS1DX1   is '1361 PLAS1DX1                                   ';
comment on column MBAP_data.PLAS1DX2   is '1362 PLAS1DX2                                   ';
comment on column MBAP_data.PFLHE      is '1363 PFLHE                                      ';
comment on column MBAP_data.PS01POSX   is '1364 PS01POSX                                   ';
comment on column MBAP_data.PS01POSY   is '1365 PS01POSY                                   ';
comment on column MBAP_data.PS11POSX   is '1366 PS11POSX                                   ';
comment on column MBAP_data.PS11POSY   is '1367 PS11POSY                                   ';
comment on column MBAP_data.PS14POSX   is '1368 PS14POSX                                   ';
comment on column MBAP_data.PS14POSY   is '1369 PS14POSY                                   ';
comment on column MBAP_data.PS14ANGZ   is '1370 PS14ANGZ                                   ';
comment on column MBAP_data.P12POSZ    is '1371 P12POSZ                                    ';
comment on column MBAP_data.P23POSZ    is '1372 P23POSZ                                    ';
comment on column MBAP_data.P34POSZ    is '1373 P34POSZ                                    ';
comment on column MBAP_data.XYZREF     is '1374 XYZREF                                     ';
comment on column MBAP_data.PFL12XA    is '1375 PFL12XA                                    ';
comment on column MBAP_data.PFL23XA    is '1376 PFL23XA                                    ';
comment on column MBAP_data.PFL34XA    is '1377 PFL34XA                                    ';
comment on column MBAP_data.PFLYA      is '1378 PFLYA                                      ';
comment on column MBAP_data.PFL12XB    is '1379 PFL12XB                                    ';
comment on column MBAP_data.PFL23XB    is '1380 PFL23XB                                    ';
comment on column MBAP_data.PFL34XB    is '1381 PFL34XB                                    ';
comment on column MBAP_data.PFLYB      is '1382 PFLYB                                      ';
comment on column MBAP_data.PFL12XC    is '1383 PFL12XC                                    ';
comment on column MBAP_data.PFL23XC    is '1384 PFL23XC                                    ';
comment on column MBAP_data.PFL34XC    is '1385 PFL34XC                                    ';
comment on column MBAP_data.PFLYC      is '1386 PFLYC                                      ';
comment on column MBAP_data.PFL12XD    is '1387 PFL12XD                                    ';
comment on column MBAP_data.PFL23XD    is '1388 PFL23XD                                    ';
comment on column MBAP_data.PFL34XD    is '1389 PFL34XD                                    ';
comment on column MBAP_data.PFLYD      is '1390 PFLYD                                      ';
comment on column MBAP_data.PLATRARO   is '1391 PLATRARO                                   ';
comment on column MBAP_data.PLATRARI   is '1392 PLATRARI                                   ';
comment on column MBAP_data.PLATRAHE   is '1393 PLATRAHE                                   ';
comment on column MBAP_data.PL12RAZE   is '1394 PL12RAZE                                   ';
comment on column MBAP_data.PL23RAZE   is '1395 PL23RAZE                                   ';
comment on column MBAP_data.PL34RAZE   is '1396 PL34RAZE                                   ';
comment on column MBAP_data.PLATRHYP   is '1397 PLATRHYP                                   ';
comment on column MBAP_data.PLATBTLE   is '1398 PLATBTLE                                   ';
comment on column MBAP_data.PRLS01DX   is '1399 PRLS01DX                                   ';
comment on column MBAP_data.PRLS11DX   is '1400 PRLS11DX                                   ';
comment on column MBAP_data.PRLS01DY   is '1401 PRLS01DY                                   ';
comment on column MBAP_data.PRLS11DY   is '1402 PRLS11DY                                   ';
comment on column MBAP_data.PSSS3DY1   is '1403 PSSS3DY1                                   ';
comment on column MBAP_data.PSSS3DY2   is '1404 PSSS3DY2                                   ';
comment on column MBAP_data.PSSS3DX1   is '1405 PSSS3DX1                                   ';
comment on column MBAP_data.PSSS3DX2   is '1406 PSSS3DX2                                   ';
comment on column MBAP_data.PSSS3DX3   is '1407 PSSS3DX3                                   ';
comment on column MBAP_data.PSSS3AN    is '1408 PSSS3AN                                    ';
comment on column MBAP_data.PSSS3ZP1   is '1409 PSSS3ZP1                                   ';
comment on column MBAP_data.PSSS3ZP2   is '1410 PSSS3ZP2                                   ';
comment on column MBAP_data.PSSS3ZP3   is '1411 PSSS3ZP3                                   ';
comment on column MBAP_data.PSSS3ZP4   is '1412 PSSS3ZP4                                   ';
comment on column MBAP_data.PSSS3ZP5   is '1413 PSSS3ZP5                                   ';
comment on column MBAP_data.PSSS3ZP6   is '1414 PSSS3ZP6                                   ';
comment on column MBAP_data.PLATRAZ1   is '1415 PLATRAZ1                                   ';
comment on column MBAP_data.PLATRAY1   is '1416 PLATRAY1                                   ';
comment on column MBAP_data.PLATRAZ2   is '1417 PLATRAZ2                                   ';
comment on column MBAP_data.PLATRAY2   is '1418 PLATRAY2                                   ';
comment on column MBAP_data.PL12RAZ3   is '1419 PL12RAZ3                                   ';
comment on column MBAP_data.PL23RAZ3   is '1420 PL23RAZ3                                   ';
comment on column MBAP_data.PL34RAZ3   is '1421 PL34RAZ3                                   ';
comment on column MBAP_data.PLATRAY3   is '1422 PLATRAY3                                   ';
comment on column MBAP_data.PL12RAZ4   is '1423 PL12RAZ4                                   ';
comment on column MBAP_data.PL23RAZ4   is '1424 PL23RAZ4                                   ';
comment on column MBAP_data.PL34RAZ4   is '1425 PL34RAZ4                                   ';
comment on column MBAP_data.PLATRAY4   is '1426 PLATRAY4                                   ';
comment on column MBAP_data.PL12RAZ0   is '1427 PL12RAZ0                                   ';
comment on column MBAP_data.PL23RAZ0   is '1428 PL23RAZ0                                   ';
comment on column MBAP_data.PL34RAZ0   is '1429 PL34RAZ0                                   ';
comment on column MBAP_data.PLATRAY0   is '1430 PLATRAY0                                   ';
comment on column MBAP_data.PL12RHZP   is '1431 PL12RHZP                                   ';
comment on column MBAP_data.PL23RHZP   is '1432 PL23RHZP                                   ';
comment on column MBAP_data.PL34RHZP   is '1433 PL34RHZP                                   ';
comment on column MBAP_data.PLATRVZE   is '1434 PLATRVZE                                   ';
comment on column MBAP_data.PLATRVYP   is '1435 PLATRVYP                                   ';
comment on column MBAP_data.PLATCRYP   is '1436 PLATCRYP                                   ';
comment on column MBAP_data.PL12CRZ1   is '1437 PL12CRZ1                                   ';
comment on column MBAP_data.PL12CRZ2   is '1438 PL12CRZ2                                   ';
comment on column MBAP_data.PL23CRZ1   is '1439 PL23CRZ1                                   ';
comment on column MBAP_data.PL23CRZ2   is '1440 PL23CRZ2                                   ';
comment on column MBAP_data.PL34CRZ1   is '1441 PL34CRZ1                                   ';
comment on column MBAP_data.PL34CRZ2   is '1442 PL34CRZ2                                   ';
comment on column MBAP_data.PL12CRAN   is '1443 PL12CRAN                                   ';
comment on column MBAP_data.PL23CRAN   is '1444 PL23CRAN                                   ';
comment on column MBAP_data.PL34CRAN   is '1445 PL34CRAN                                   ';
comment on column MBAP_data.PL12CRLE   is '1446 PL12CRLE                                   ';
comment on column MBAP_data.PL23CRLE   is '1447 PL23CRLE                                   ';
comment on column MBAP_data.PL34CRLE   is '1448 PL34CRLE                                   ';
comment on column MBAP_data.PLAT2RAX   is '1449 PLAT2RAX                                   ';
comment on column MBAP_data.PL12BTZ1   is '1450 PL12BTZ1                                   ';
comment on column MBAP_data.PL23BTZ1   is '1451 PL23BTZ1                                   ';
comment on column MBAP_data.PL34BTZ1   is '1452 PL34BTZ1                                   ';
comment on column MBAP_data.PL12BTZ2   is '1453 PL12BTZ2                                   ';
comment on column MBAP_data.PL23BTZ2   is '1454 PL23BTZ2                                   ';
comment on column MBAP_data.PL34BTZ2   is '1455 PL34BTZ2                                   ';
comment on column MBAP_data.PLATBTXP   is '1456 PLATBTXP                                   ';
comment on column MBAP_data.PRLS01XP   is '1457 PRLS01XP                                   ';
comment on column MBAP_data.PRLS11XP   is '1458 PRLS11XP                                   ';
comment on column MBAP_data.PRLS01YP   is '1459 PRLS01YP                                   ';
comment on column MBAP_data.PRLS11YP   is '1460 PRLS11YP                                   ';
comment on column MBAP_data.PL12RLZ1   is '1461 PL12RLZ1                                   ';
comment on column MBAP_data.PL23RLZ1   is '1462 PL23RLZ1                                   ';
comment on column MBAP_data.PL34RLZ1   is '1463 PL34RLZ1                                   ';
comment on column MBAP_data.PL12RLZ2   is '1464 PL12RLZ2                                   ';
comment on column MBAP_data.PL23RLZ2   is '1465 PL23RLZ2                                   ';
comment on column MBAP_data.PL34RLZ2   is '1466 PL34RLZ2                                   ';
comment on column MBAP_data.PRLS01AN   is '1467 PRLS01AN                                   ';
comment on column MBAP_data.PRLS11AN   is '1468 PRLS11AN                                   ';
comment on column MBAP_data.PRLS01LE   is '1469 PRLS01LE                                   ';
comment on column MBAP_data.PRLS11LE   is '1470 PRLS11LE                                   ';
comment on column MBAP_data.PLATBSXP   is '1471 PLATBSXP                                   ';
comment on column MBAP_data.PLATBTLS   is '1472 PLATBTLS                                   ';
comment on column MBAP_data.PL12FRZP   is '1473 PL12FRZP                                   ';
comment on column MBAP_data.PL23FRZP   is '1474 PL23FRZP                                   ';
comment on column MBAP_data.PL34FRZP   is '1475 PL34FRZP                                   ';
comment on column MBAP_data.PLATFRYP   is '1476 PLATFRYP                                   ';
comment on column MBAP_data.PLATFRXP   is '1477 PLATFRXP                                   ';
comment on column MBAP_data.PSSS3DYT   is '1478 PSSS3DYT                                   ';
comment on column MBAP_data.PSSS3DXT   is '1479 PSSS3DXT                                   ';
comment on column MBAP_data.PSSS3X0    is '1480 PSSS3X0                                    ';
comment on column MBAP_data.PSSS3Y0    is '1481 PSSS3Y0                                    ';
comment on column MBAP_data.PSSS3X1    is '1482 PSSS3X1                                    ';
comment on column MBAP_data.PSSS3Y1    is '1483 PSSS3Y1                                    ';
comment on column MBAP_data.PSSS3X2    is '1484 PSSS3X2                                    ';
comment on column MBAP_data.PSSS3Y2    is '1485 PSSS3Y2                                    ';
comment on column MBAP_data.PSSS3X3    is '1486 PSSS3X3                                    ';
comment on column MBAP_data.PSSS3Y3    is '1487 PSSS3Y3                                    ';
comment on column MBAP_data.PSSS3X4    is '1488 PSSS3X4                                    ';
comment on column MBAP_data.PSSS3Y4    is '1489 PSSS3Y4                                    ';
comment on column MBAP_data.PSSS3X5    is '1490 PSSS3X5                                    ';
comment on column MBAP_data.PSSS3Y5    is '1491 PSSS3Y5                                    ';
comment on column MBAP_data.PSSS3X6    is '1492 PSSS3X6                                    ';
comment on column MBAP_data.PSSS3Y6    is '1493 PSSS3Y6                                    ';
comment on column MBAP_data.PSSRAXP    is '1494 PSSRAXP                                    ';
comment on column MBAP_data.PSSRAYP    is '1495 PSSRAYP                                    ';
comment on column MBAP_data.PSSZRP1    is '1496 PSSZRP1                                    ';
comment on column MBAP_data.PSSZRP2    is '1497 PSSZRP2                                    ';
comment on column MBAP_data.PSSZRP3    is '1498 PSSZRP3                                    ';
comment on column MBAP_data.PSSZRP4    is '1499 PSSZRP4                                    ';
comment on column MBAP_data.PSSZRP5    is '1500 PSSZRP5                                    ';
comment on column MBAP_data.PSSZRP6    is '1501 PSSZRP6                                    ';
comment on column MBAP_data.PRHS3ZP1   is '1502 PRHS3ZP1                                   ';
comment on column MBAP_data.PRHS3ZP2   is '1503 PRHS3ZP2                                   ';
comment on column MBAP_data.PRHS3ZP3   is '1504 PRHS3ZP3                                   ';
comment on column MBAP_data.PRHS3ZP4   is '1505 PRHS3ZP4                                   ';
comment on column MBAP_data.PRHS3ZP5   is '1506 PRHS3ZP5                                   ';
comment on column MBAP_data.PRHS3ZP6   is '1507 PRHS3ZP6                                   ';
comment on column MBAP_data.PRHS3ZP7   is '1508 PRHS3ZP7                                   ';
comment on column MBAP_data.PL14RAZE   is '1509 PL14RAZE                                   ';
comment on column MBAP_data.PL14RAZP   is '1510 PL14RAZP                                   ';
comment on column MBAP_data.PL14RAY1   is '1511 PL14RAY1                                   ';
comment on column MBAP_data.PL14RAY2   is '1512 PL14RAY2                                   ';
comment on column MBAP_data.PL14RVZE   is '1513 PL14RVZE                                   ';
comment on column MBAP_data.PL14RVYP   is '1514 PL14RVYP                                   ';
comment on column MBAP_data.PL14CRY1   is '1515 PL14CRY1                                   ';
comment on column MBAP_data.PL14CRY2   is '1516 PL14CRY2                                   ';
comment on column MBAP_data.P12S3CRA   is '1517 P12S3CRA                                   ';
comment on column MBAP_data.P12S3CRL   is '1518 P12S3CRL                                   ';
comment on column MBAP_data.P23S3CRA   is '1519 P23S3CRA                                   ';
comment on column MBAP_data.P23S3CRL   is '1520 P23S3CRL                                   ';
comment on column MBAP_data.P34S3CRA   is '1521 P34S3CRA                                   ';
comment on column MBAP_data.P34S3CRL   is '1522 P34S3CRL                                   ';
comment on column MBAP_data.PL14CRZ1   is '1523 PL14CRZ1                                   ';
comment on column MBAP_data.PL14CRZ2   is '1524 PL14CRZ2                                   ';
comment on column MBAP_data.PL14CRZ3   is '1525 PL14CRZ3                                   ';
comment on column MBAP_data.PL14CRZ4   is '1526 PL14CRZ4                                   ';
comment on column MBAP_data.PL14CRZ5   is '1527 PL14CRZ5                                   ';
comment on column MBAP_data.PL14CRZ6   is '1528 PL14CRZ6                                   ';
comment on column MBAP_data.PL14CRZ7   is '1529 PL14CRZ7                                   ';
comment on column MBAP_data.PL14CRZ8   is '1530 PL14CRZ8                                   ';
comment on column MBAP_data.PL14CRZ9   is '1531 PL14CRZ9                                   ';
comment on column MBAP_data.PL14CRZA   is '1532 PL14CRZA                                   ';
comment on column MBAP_data.P14S3BFL   is '1533 P14S3BFL                                   ';
comment on column MBAP_data.P14S3BFA   is '1534 P14S3BFA                                   ';
comment on column MBAP_data.P14S3BFX   is '1535 P14S3BFX                                   ';
comment on column MBAP_data.P14S3BFY   is '1536 P14S3BFY                                   ';
comment on column MBAP_data.P14S3FLA   is '1537 P14S3FLA                                   ';
comment on column MBAP_data.P14S3FLX   is '1538 P14S3FLX                                   ';
comment on column MBAP_data.P14S3FLY   is '1539 P14S3FLY                                   ';
comment on column MBAP_data.P14S3FLZ   is '1540 P14S3FLZ                                   ';

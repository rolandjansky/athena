SET echo OFF;
set linesize 132;
drop   table ABRT_data2tag cascade constraints;
drop   table ABRT_data cascade constraints;
create table ABRT_data (
  ABRT_data_id   number(10),
CRYORMIN       float(63),
CRYORMAX       float(63),
CRYOZMAX       float(63),
CRYORCUR       float(63),
CRYORADI       float(63),
CRYORADT       float(63),
CRYATTD0       float(63),
CRYATTD1       float(63),
CRYATTAL       float(63),
CRYATTXH       float(63),
CRYATTTH       float(63),
CRYATTRX       float(63),
CRYATWIY       float(63),
CRYATWXP       float(63),
CRYATWXN       float(63),
CRYATWBO       float(63),
CRYATWTH       float(63),
CRYATWZE       float(63),
CRYATWRA       float(63),
CRYATWYC       float(63),
CRYRIBYW       float(63),
CRYRIBZL       float(63),
CRYRIBTH       float(63),
CRYRIWYP       float(63),
CRYRIWYN       float(63),
CRYRIWXP       float(63),
CRYRIWXN       float(63),
CRYRIWTH       float(63),
CRYRNGRM       float(63),
CRYRNGZM       float(63),
CRYRNGRA       float(63),
STRTRMAX       float(63),
STRTRMIN       float(63),
STRTYLEN       float(63),
STRTZWID       float(63),
STRTRTHI       float(63),
STRTZTHI       float(63),
STRWRMAX       float(63),
STRWYMAX       float(63),
STRWRMIN       float(63),
STRWYMIN       float(63),
STRWZTHI       float(63),
STRWYTHI       float(63),
STRWZLEN       float(63),
ZVOUSS_0       float(63),
ZVOUSS_1       float(63),
ZVOUSS_2       float(63),
ZVOUSS_3       float(63),
ZVOUSS_4       float(63),
ZVOUSS_5       float(63),
ZVOUSS_6       float(63),
ZVOUSS_7       float(63),
ZRIB_0       float(63),
ZRIB_1       float(63),
ZRIB_2       float(63),
ZRIB_3       float(63),
ZRIB_4       float(63),
ZRIB_5       float(63),
ZRIB_6       float(63),
CRYRIXHI       float(63),
STRTRMOY       float(63),
STRTYLNP       float(63),
CRYATTXS       float(63),
COMARMIN       float(63),
COMARMAX       float(63),
COMAZMAX       float(63),
COMARCUI       float(63),
COMARTHI       float(63),
COMAYTHI       float(63),
COMARIBY       float(63),
COMARIBZ       float(63),
COMABOSS       float(63),
COMARIBX       float(63),
BIELYHEI       float(63),
BIELYSO1       float(63),
BIELYSO2       float(63),
BIELXSOL       float(63),
BIELXTH1       float(63),
BIELXTH2       float(63),
BIELZTH1       float(63),
BIELRHOL       float(63),
BIELYCHO       float(63),
BIELATL1       float(63),
BIELATL2       float(63),
BIELATTH       float(63),
BIELATHE       float(63),
VOUSBLXH       float(63),
VOUSPLIX       float(63),
VOUSPLOX       float(63),
VOUSBLYW       float(63),
VOUBLYWS       float(63),
VOURCYWI       float(63),
VOURPYWI       float(63),
VOUBLZLE       float(63),
VOUBLZLS       float(63),
VOUBZWTH       float(63),
VOUCUTZE       float(63),
VOURCUTR       float(63),
VOURECSL       float(63),
CNBCOYEX       float(63),
CNBCOXEX       float(63),
CNBCOXSU       float(63),
CNBCOXIN       float(63),
CNBEAHEI       float(63),
CNBEACXI       float(63),
CNBEACTL       float(63),
CNBEACTU       float(63),
CNBEACZW       float(63),
CNBEACZE       float(63),
CNBEACPL       float(63),
CNBCADMA       float(63),
CNBCADMI       float(63),
CNBCAZEX       float(63),
CNBOXZEX       float(63),
VOUSMBRA       float(63),
CNBXMBRA       float(63),
VOUCRCYR       float(63),
VOUSRCXW       float(63),
VOUSCXOF       float(63),
VOUSCYPO       float(63),
VOUSCZPO       float(63),
CNBEAXTP       float(63),
CNBEAYTP       float(63),
CNBEAXBT       float(63),
CNBEAYBT       float(63),
CNBECXOL       float(63),
CNBECYOL       float(63),
CNBECYIL       float(63),
CNBECOHE       float(63),
CNBECXOU       float(63),
CNBECYOU       float(63),
CNBECYIU       float(63),
CNBECZPO       float(63),
CNBIECZP       float(63),
CNBCAZIN       float(63),
VOUSSRAD       float(63),
CNBOXRAD       float(63)
) ;

alter table ABRT_data add constraint ABRT_data_pk
primary key (ABRT_data_id);

create table ABRT_data2tag (
  ABRT_vers      varchar2(255),
  ABRT_data_id   number(10)
) ;

alter table ABRT_data2tag add constraint ABRT_data2tag_pk
primary key (ABRT_vers, ABRT_data_id);
alter table ABRT_data2tag add constraint ABRT_data2tag_vers_fk
foreign key (ABRT_vers) references hvs_nodevers (vers_name);
alter table ABRT_data2tag add constraint ABRT_data2tag_data_fk
foreign key (ABRT_data_id) references ABRT_data (ABRT_data_id);

delete from hvs_relation where CHILD_NODENAME = 'ABRT';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'ABRT';
delete from hvs_node     where NODE_NAME = 'ABRT';

insert into hvs_node     values ('ABRT','MuonSpectrometer', '');
insert into hvs_nodevers values ('ABRT', 'ABRT-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','ABRT','ABRT-01');
insert into ABRT_data (
ABRT_data_id,
CRYORMIN,
CRYORMAX,
CRYOZMAX,
CRYORCUR,
CRYORADI,
CRYORADT,
CRYATTD0,
CRYATTD1,
CRYATTAL,
CRYATTXH,
CRYATTTH,
CRYATTRX,
CRYATWIY,
CRYATWXP,
CRYATWXN,
CRYATWBO,
CRYATWTH,
CRYATWZE,
CRYATWRA,
CRYATWYC,
CRYRIBYW,
CRYRIBZL,
CRYRIBTH,
CRYRIWYP,
CRYRIWYN,
CRYRIWXP,
CRYRIWXN,
CRYRIWTH,
CRYRNGRM,
CRYRNGZM,
CRYRNGRA,
STRTRMAX,
STRTRMIN,
STRTYLEN,
STRTZWID,
STRTRTHI,
STRTZTHI,
STRWRMAX,
STRWYMAX,
STRWRMIN,
STRWYMIN,
STRWZTHI,
STRWYTHI,
STRWZLEN,
ZVOUSS_0,ZVOUSS_1,ZVOUSS_2,ZVOUSS_3,ZVOUSS_4,ZVOUSS_5,ZVOUSS_6,ZVOUSS_7,
ZRIB_0,ZRIB_1,ZRIB_2,ZRIB_3,ZRIB_4,ZRIB_5,ZRIB_6,
CRYRIXHI,
STRTRMOY,
STRTYLNP,
CRYATTXS,
COMARMIN,
COMARMAX,
COMAZMAX,
COMARCUI,
COMARTHI,
COMAYTHI,
COMARIBY,
COMARIBZ,
COMABOSS,
COMARIBX,
BIELYHEI,
BIELYSO1,
BIELYSO2,
BIELXSOL,
BIELXTH1,
BIELXTH2,
BIELZTH1,
BIELRHOL,
BIELYCHO,
BIELATL1,
BIELATL2,
BIELATTH,
BIELATHE,
VOUSBLXH,
VOUSPLIX,
VOUSPLOX,
VOUSBLYW,
VOUBLYWS,
VOURCYWI,
VOURPYWI,
VOUBLZLE,
VOUBLZLS,
VOUBZWTH,
VOUCUTZE,
VOURCUTR,
VOURECSL,
CNBCOYEX,
CNBCOXEX,
CNBCOXSU,
CNBCOXIN,
CNBEAHEI,
CNBEACXI,
CNBEACTL,
CNBEACTU,
CNBEACZW,
CNBEACZE,
CNBEACPL,
CNBCADMA,
CNBCADMI,
CNBCAZEX,
CNBOXZEX,
VOUSMBRA,
CNBXMBRA,
VOUCRCYR,
VOUSRCXW,
VOUSCXOF,
VOUSCYPO,
VOUSCZPO,
CNBEAXTP,
CNBEAYTP,
CNBEAXBT,
CNBEAYBT,
CNBECXOL,
CNBECYOL,
CNBECYIL,
CNBECOHE,
CNBECXOU,
CNBECYOU,
CNBECYIU,
CNBECZPO,
CNBIECZP,
CNBCAZIN,
VOUSSRAD,
CNBOXRAD
) values (    1,
  4720.0000, 10030.0000, 12650.0000,  1000.0000,   550.0000,    15.0000,
   603.0000,   550.0000,     8.0000,   630.0000,    15.0000,  6061.0000,
   600.0000,   590.0000,   -41.0000,    40.0000,    25.0000,   680.0000,
   578.0000,   150.0000,   540.0000,   290.0000,    20.0000,   350.0000,
  1090.0000,   550.0000,  -180.0000,    30.0000,  8760.0000,  -800.0000,
   340.0000,  9100.0000,  8460.0000,  5650.0000,   360.0000,    50.0000,
    40.0000, 10050.0000,   440.0000,  8760.0000,   280.0000,    50.0000,
    70.0000,   400.0000,-10845.0000, -8245.0000, -5137.5000, -1712.5000,
  1712.5000,  5137.5000,  8245.0000, 10845.0000, -9540.0000, -6760.0000,
 -3490.0000,     0.0000,  3490.0000,  6760.0000,  9540.0000,  2105.0000,
  8780.0000,  5685.0000,   188.5573,  4870.0000,  9880.0000, 12500.0000,
   800.0000,   800.0000,   320.0000,   300.0000,   120.0000,   937.2583,
  2385.7144,  1110.0000,    50.0000,    60.0000,   120.0000,    20.0000,
    40.0000,   230.0000,    60.0000,   115.0000,  1000.0000,   150.0000,
   280.0000,   140.0000,   850.0000,   120.0000,    80.0000,  2850.0000,
   120.0000,   815.0000,    50.0000,   500.0000,    50.0000,    50.0000,
   500.0000,   300.0000,    30.0000,   880.0000,  2367.0000,   834.0000,
   634.0000,   850.0000,   350.0000,   120.0000,    80.0000,    50.0000,
   135.0000,    80.0000,   518.0000,   468.0000,   780.0000,   550.0000,
  5723.0000,  6942.0000,   440.0000,   650.0000,    20.0000,   897.5000,
   275.0000,  1183.5000,   562.5053,   858.2191,  -222.7923,   793.2756,
   -66.0047,   117.6060,   650.0000,  1042.0198,   534.5170,   821.1609,
   415.0000,   300.0000,   177.8842,  6148.0000,  6062.0000
);
insert into ABRT_data2tag values ('ABRT-01',    1);

comment on column ABRT_data.ABRT_data_id is 'Unique identifier';
comment on column ABRT_data.CRYORMIN   is 'CRYORMIN                                        ';
comment on column ABRT_data.CRYORMAX   is 'CRYORMAX                                        ';
comment on column ABRT_data.CRYOZMAX   is 'CRYOZMAX                                        ';
comment on column ABRT_data.CRYORCUR   is 'CRYORCUR                                        ';
comment on column ABRT_data.CRYORADI   is 'RADIUS                                          ';
comment on column ABRT_data.CRYORADT   is 'RADTHICK                                        ';
comment on column ABRT_data.CRYATTD0   is 'CRYOATTDIAM0                                    ';
comment on column ABRT_data.CRYATTD1   is 'CRYOATTDIAM1                                    ';
comment on column ABRT_data.CRYATTAL   is 'CRYOATTALFA                                     ';
comment on column ABRT_data.CRYATTXH   is 'CRYOATTXHIGH                                    ';
comment on column ABRT_data.CRYATTTH   is 'CRYOATTTHICK                                    ';
comment on column ABRT_data.CRYATTRX   is 'CRYOATTRMAX                                     ';
comment on column ABRT_data.CRYATWIY   is 'CRYOATTWINGY                                    ';
comment on column ABRT_data.CRYATWXP   is 'CRYOATTWINGXP                                   ';
comment on column ABRT_data.CRYATWXN   is 'CRYOATTWINGXN                                   ';
comment on column ABRT_data.CRYATWBO   is 'CRYOATTWINGBOSS                                 ';
comment on column ABRT_data.CRYATWTH   is 'CRYOATTWINGTHICK                                ';
comment on column ABRT_data.CRYATWZE   is 'CRYOATTWINGZESP                                 ';
comment on column ABRT_data.CRYATWRA   is 'CRYOATTWINGRAD                                  ';
comment on column ABRT_data.CRYATWYC   is 'CRYOATTWINGYCUT                                 ';
comment on column ABRT_data.CRYRIBYW   is 'CRYORIBYWID                                     ';
comment on column ABRT_data.CRYRIBZL   is 'CRYORIBZLEN                                     ';
comment on column ABRT_data.CRYRIBTH   is 'CRYORIBTHICK                                    ';
comment on column ABRT_data.CRYRIWYP   is 'CRYORIBWINGYP                                   ';
comment on column ABRT_data.CRYRIWYN   is 'CRYORIBWINGYN                                   ';
comment on column ABRT_data.CRYRIWXP   is 'CRYORIBWINGXP                                   ';
comment on column ABRT_data.CRYRIWXN   is 'CRYORIBWINGXN                                   ';
comment on column ABRT_data.CRYRIWTH   is 'CRYORIBWINGTHICK                                ';
comment on column ABRT_data.CRYRNGRM   is 'CRYORINGRMED                                    ';
comment on column ABRT_data.CRYRNGZM   is 'CRYORINGZMED                                    ';
comment on column ABRT_data.CRYRNGRA   is 'CRYORINGRADIUS                                  ';
comment on column ABRT_data.STRTRMAX   is 'STRTRMAX                                        ';
comment on column ABRT_data.STRTRMIN   is 'STRTRMIN                                        ';
comment on column ABRT_data.STRTYLEN   is 'STRTYLEN                                        ';
comment on column ABRT_data.STRTZWID   is 'STRUTZWIDTH                                     ';
comment on column ABRT_data.STRTRTHI   is 'STRUTRTHICK                                     ';
comment on column ABRT_data.STRTZTHI   is 'STRUTZTHICK                                     ';
comment on column ABRT_data.STRWRMAX   is 'STRUTWINGRMAX                                   ';
comment on column ABRT_data.STRWYMAX   is 'STRUTWINGYMAX                                   ';
comment on column ABRT_data.STRWRMIN   is 'STRUTWINGRMIN                                   ';
comment on column ABRT_data.STRWYMIN   is 'STRUTWINGYMIN                                   ';
comment on column ABRT_data.STRWZTHI   is 'STRUTWINGZTHICK                                 ';
comment on column ABRT_data.STRWYTHI   is 'STRUTWINGYTHICK                                 ';
comment on column ABRT_data.STRWZLEN   is 'STRUTWINGZLEN                                   ';
comment on column ABRT_data.ZVOUSS_0   is 'ARRAY                                           ';
comment on column ABRT_data.ZRIB_0     is 'ARRAY                                           ';
comment on column ABRT_data.CRYRIXHI   is 'CRYORIBXHIGH                                    ';
comment on column ABRT_data.STRTRMOY   is 'STRTRMOY                                        ';
comment on column ABRT_data.STRTYLNP   is 'STRUTYLENP                                      ';
comment on column ABRT_data.CRYATTXS   is 'CRYOATTXSEC                                     ';
comment on column ABRT_data.COMARMIN   is 'COLDMASSRMIN                                    ';
comment on column ABRT_data.COMARMAX   is 'COLDMASSRMAX                                    ';
comment on column ABRT_data.COMAZMAX   is 'COLDMASSZMAX                                    ';
comment on column ABRT_data.COMARCUI   is 'COLDMASSRCURVI                                  ';
comment on column ABRT_data.COMARTHI   is 'COLDMASSRTHICK                                  ';
comment on column ABRT_data.COMAYTHI   is 'COLDMASSYTHICK                                  ';
comment on column ABRT_data.COMARIBY   is 'COLDMASSRIBY                                    ';
comment on column ABRT_data.COMARIBZ   is 'COLDMASSRIBZ                                    ';
comment on column ABRT_data.COMABOSS   is 'COLDMASSBOSS                                    ';
comment on column ABRT_data.COMARIBX   is 'COLDMASSRIBX                                    ';
comment on column ABRT_data.BIELYHEI   is 'BIELLYHEIGHT                                    ';
comment on column ABRT_data.BIELYSO1   is 'BIELLYSOLE1                                     ';
comment on column ABRT_data.BIELYSO2   is 'BIELLYSOLE2                                     ';
comment on column ABRT_data.BIELXSOL   is 'BIELLXSOLE                                      ';
comment on column ABRT_data.BIELXTH1   is 'BIELLXTHICK1                                    ';
comment on column ABRT_data.BIELXTH2   is 'BIELLXTHICK2                                    ';
comment on column ABRT_data.BIELZTH1   is 'BIELLZTHICK1                                    ';
comment on column ABRT_data.BIELRHOL   is 'BIELLRADHOLE                                    ';
comment on column ABRT_data.BIELYCHO   is 'BIELLYCENHOLE                                   ';
comment on column ABRT_data.BIELATL1   is 'BIELLATTLENG1                                   ';
comment on column ABRT_data.BIELATL2   is 'BIELLATTLENG2                                   ';
comment on column ABRT_data.BIELATTH   is 'BIELLATTTHICK                                   ';
comment on column ABRT_data.BIELATHE   is 'BIELLATTHEIGHT                                  ';
comment on column ABRT_data.VOUSBLXH   is 'VOUSSBLOCKXHEIGHT                               ';
comment on column ABRT_data.VOUSPLIX   is 'VOUSSPLATEINNERXWID                             ';
comment on column ABRT_data.VOUSPLOX   is 'VOUSSPLATEOUTERXWID                             ';
comment on column ABRT_data.VOUSBLYW   is 'VOUSSBLOCKYWID                                  ';
comment on column ABRT_data.VOUBLYWS   is 'VOUSSBLOCKYWIDSUB                               ';
comment on column ABRT_data.VOURCYWI   is 'VOUSSRECTCUTYWID                                ';
comment on column ABRT_data.VOURPYWI   is 'VOUSSREINFORCEPLATYWID                          ';
comment on column ABRT_data.VOUBLZLE   is 'VOUSSBLOCKZLEN                                  ';
comment on column ABRT_data.VOUBLZLS   is 'VOUSSBLOCKZLENSUB                               ';
comment on column ABRT_data.VOUBZWTH   is 'VOUSSBLOCKZWALLTH                               ';
comment on column ABRT_data.VOUCUTZE   is 'VOUSSCUTZEXT                                    ';
comment on column ABRT_data.VOURCUTR   is 'VOUSSROUNDCUTRAD                                ';
comment on column ABRT_data.VOURECSL   is 'VOUSSRECTCUTXYSLANT                             ';
comment on column ABRT_data.CNBCOYEX   is 'CONBOXCOREYEXTENSION                            ';
comment on column ABRT_data.CNBCOXEX   is 'CONBOXCOREXEXTENSION                            ';
comment on column ABRT_data.CNBCOXSU   is 'CONBOXCOREXSUP                                  ';
comment on column ABRT_data.CNBCOXIN   is 'CONBOXCOREXINF                                  ';
comment on column ABRT_data.CNBEAHEI   is 'CONBOXEARHEIGHT                                 ';
comment on column ABRT_data.CNBEACXI   is 'CONBOXEARCUTXIN                                 ';
comment on column ABRT_data.CNBEACTL   is 'CONBOXEARCUTPLATETHICKLOWOUT                    ';
comment on column ABRT_data.CNBEACTU   is 'CONBOXEARCUTPLATETHICKUP                        ';
comment on column ABRT_data.CNBEACZW   is 'CONBOXEARCUTZWALL                               ';
comment on column ABRT_data.CNBEACZE   is 'CONBOXEARCUTZEXTR                               ';
comment on column ABRT_data.CNBEACPL   is 'CONBOXINNEREARCUTPLATE                          ';
comment on column ABRT_data.CNBCADMA   is 'CONBOXCAVITYDMAX                                ';
comment on column ABRT_data.CNBCADMI   is 'CONBOXCAVITYDMIN                                ';
comment on column ABRT_data.CNBCAZEX   is 'CONBOXCAVITYZEXT                                ';
comment on column ABRT_data.CNBOXZEX   is 'CONBOXZEXTENSION                                ';
comment on column ABRT_data.VOUSMBRA   is 'VOUSSOIRMAINBLOCKRADIUS                         ';
comment on column ABRT_data.CNBXMBRA   is 'CONBOXMAINBLOCKRADIUS                           ';
comment on column ABRT_data.VOUCRCYR   is 'VOUSSCENTRRECTCUTYREACH                         ';
comment on column ABRT_data.VOUSRCXW   is 'VOUSSRECTCUTXWID                                ';
comment on column ABRT_data.VOUSCXOF   is 'VOUSSCUTXOFFSET                                 ';
comment on column ABRT_data.VOUSCYPO   is 'VOUSSOIRCUTYPOS                                 ';
comment on column ABRT_data.VOUSCZPO   is 'VOUSSOIRCUTZPOS                                 ';
comment on column ABRT_data.CNBEAXTP   is 'CONBOXEARXTOP                                   ';
comment on column ABRT_data.CNBEAYTP   is 'CONBOXEARYTOP                                   ';
comment on column ABRT_data.CNBEAXBT   is 'CONBOXEARXBOT                                   ';
comment on column ABRT_data.CNBEAYBT   is 'CONBOXEARYBOT                                   ';
comment on column ABRT_data.CNBECXOL   is 'CONBOXEARCUTXOUTLOW                             ';
comment on column ABRT_data.CNBECYOL   is 'CONBOXEARCUTYOUTLOW                             ';
comment on column ABRT_data.CNBECYIL   is 'CONBOXEARCUTYINLOW                              ';
comment on column ABRT_data.CNBECOHE   is 'CONBOXEARCUTOUTHEIGHT                           ';
comment on column ABRT_data.CNBECXOU   is 'CONBOXEARCUTXOUTUP                              ';
comment on column ABRT_data.CNBECYOU   is 'CONBOXEARCUTYOUTUP                              ';
comment on column ABRT_data.CNBECYIU   is 'CONBOXEARCUTYINUP                               ';
comment on column ABRT_data.CNBECZPO   is 'CONBOXEARCUTZPOS                                ';
comment on column ABRT_data.CNBIECZP   is 'CONBOXINNEREARCUTZPOS                           ';
comment on column ABRT_data.CNBCAZIN   is 'CONBOXCAVITYZINTER                              ';
comment on column ABRT_data.VOUSSRAD   is 'VOUSSOIRRADIUS                                  ';
comment on column ABRT_data.CNBOXRAD   is 'CONBOXRADIUS                                    ';

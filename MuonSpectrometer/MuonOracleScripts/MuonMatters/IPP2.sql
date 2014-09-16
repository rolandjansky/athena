SET echo OFF;
set linesize 132;
drop   table IPP2_data2tag cascade constraints;
drop   table IPP2_data cascade constraints;
create table IPP2_data (
  IPP2_data_id   number(10),
PIXPOBHE       float(63),
PIXPOBDZ       float(63),
PIXPOBDX       float(63),
PIXPOC2E       float(63),
PIXPOC3E       float(63),
PIXNTBHE       float(63),
PIXNTBDZ       float(63),
PIXNTBDX       float(63),
PIXNTC2E       float(63),
PIXNTC3E       float(63),
PIXENBHE       float(63),
PIXENBDZ       float(63),
PIXENBDX       float(63),
PIXENC2E       float(63),
PIXENC3E       float(63),
PIXENBXP       float(63),
PIXENBZP       float(63),
IDRMABHE       float(63),
IDRMABDZ       float(63),
IDRMABDX       float(63),
IDRMAC2E       float(63),
IDRMAC3E       float(63),
IDRMABXP       float(63),
IDRMABZP       float(63),
TRTPSBHE       float(63),
TRTPSBDZ       float(63),
TRTPSBDX       float(63),
TRTPSC2E       float(63),
TRTPSC3E       float(63),
TRTPSBXP       float(63),
TRTPSBZ1       float(63),
TRTPSBY1       float(63),
TRTPSBZ2       float(63),
TRTPSBZ3       float(63),
TRTPSBZ4       float(63),
TRTPSBZ5       float(63),
TRTPSBZ6       float(63),
TRTPSBY6       float(63),
TRTPSBZ7       float(63),
TRTPSBZ8       float(63),
TRTPSYGA       float(63),
PLATFLTH       float(63),
PLATFLDZ       float(63),
PLATFLDX       float(63),
PLATFLXP       float(63),
PLATFLZP       float(63),
PLATBFTH       float(63),
PLATBFDZ       float(63),
PLATBFDX       float(63),
PLATBFZA       float(63),
PLATBFZB       float(63),
PLATBFZC       float(63),
PLATRARO       float(63),
PLATRARI       float(63),
PLATRAHE       float(63),
PLATRAZE       float(63),
PLATRADH       float(63),
XPOS       float(63),
YPOS       float(63),
ZPOS       float(63),
PIXPOC2P       float(63),
PIXPOC3P       float(63),
PIXPOBY1       float(63),
PIXPOBY2       float(63),
PIXPOBY3       float(63),
PIXNTC2P       float(63),
PIXNTC3P       float(63),
PIXNTBY1       float(63),
PIXNTBY2       float(63),
PIXNTBY3       float(63),
PIXNTBXO       float(63),
PIXENC2P       float(63),
PIXENC3P       float(63),
PIXENBYP       float(63),
IDRMAC2X       float(63),
IDRMAC3X       float(63),
IDRMABYP       float(63),
TRTPSC2X       float(63),
TRTPSC3X       float(63),
TRTPSBY2       float(63),
TRTPSBY3       float(63),
TRTPSBY4       float(63),
TRTPSBY5       float(63),
TRTPSBY7       float(63),
TRTPSBY8       float(63),
PLATFLYP       float(63),
PLATBFXP       float(63),
PLATBFYP       float(63),
PLATBFZ1       float(63),
PLATBFZ2       float(63),
PLATBFZ3       float(63),
PLATRAZ1       float(63),
PLATRAY1       float(63),
PLATRAZ2       float(63),
PLATRAY2       float(63),
PLATRAZ3       float(63),
PLATRAY3       float(63),
PLATRAZ4       float(63),
PLATRAY4       float(63),
PLATRAZ5       float(63),
PLATRAY5       float(63),
PLATRAZ6       float(63),
PLATRAY6       float(63),
PLATRVZ1       float(63),
PLATRVY1       float(63),
PLATRVZ2       float(63),
PLATRVY2       float(63),
PLATRHZ1       float(63),
PLATRHY1       float(63),
PLATRHZ2       float(63),
PLATRHY2       float(63),
PLATRHZE       float(63),
PLATRVZE       float(63),
PLATRHZP       float(63),
PLATRHYP       float(63),
PLATRAXA       float(63),
PLATRAXB       float(63)
) ;

alter table IPP2_data add constraint IPP2_data_pk
primary key (IPP2_data_id);

create table IPP2_data2tag (
  IPP2_vers      varchar2(255),
  IPP2_data_id   number(10)
);

alter table IPP2_data2tag add constraint IPP2_data2tag_pk
primary key (IPP2_vers, IPP2_data_id);

alter table IPP2_data2tag add constraint IPP2_data2tag_vers_fk
foreign key (IPP2_vers) references hvs_nodevers (vers_name);
alter table IPP2_data2tag add constraint IPP2_data2tag_data_fk
foreign key (IPP2_data_id) references IPP2_data (IPP2_data_id);

delete from hvs_relation where CHILD_NODENAME = 'IPP2';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'IPP2';
delete from hvs_node     where NODE_NAME = 'IPP2';

insert into hvs_node     values ('IPP2','MuonSpectrometer', '');
insert into hvs_nodevers values ('IPP2', 'IPP2-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','IPP2','IPP2-01');
insert into IPP2_data (
IPP2_data_id,
PIXPOBHE,
PIXPOBDZ,
PIXPOBDX,
PIXPOC2E,
PIXPOC3E,
PIXNTBHE,
PIXNTBDZ,
PIXNTBDX,
PIXNTC2E,
PIXNTC3E,
PIXENBHE,
PIXENBDZ,
PIXENBDX,
PIXENC2E,
PIXENC3E,
PIXENBXP,
PIXENBZP,
IDRMABHE,
IDRMABDZ,
IDRMABDX,
IDRMAC2E,
IDRMAC3E,
IDRMABXP,
IDRMABZP,
TRTPSBHE,
TRTPSBDZ,
TRTPSBDX,
TRTPSC2E,
TRTPSC3E,
TRTPSBXP,
TRTPSBZ1,
TRTPSBY1,
TRTPSBZ2,
TRTPSBZ3,
TRTPSBZ4,
TRTPSBZ5,
TRTPSBZ6,
TRTPSBY6,
TRTPSBZ7,
TRTPSBZ8,
TRTPSYGA,
PLATFLTH,
PLATFLDZ,
PLATFLDX,
PLATFLXP,
PLATFLZP,
PLATBFTH,
PLATBFDZ,
PLATBFDX,
PLATBFZA,
PLATBFZB,
PLATBFZC,
PLATRARO,
PLATRARI,
PLATRAHE,
PLATRAZE,
PLATRADH,
XPOS,
YPOS,
ZPOS,
PIXPOC2P,
PIXPOC3P,
PIXPOBY1,
PIXPOBY2,
PIXPOBY3,
PIXNTC2P,
PIXNTC3P,
PIXNTBY1,
PIXNTBY2,
PIXNTBY3,
PIXNTBXO,
PIXENC2P,
PIXENC3P,
PIXENBYP,
IDRMAC2X,
IDRMAC3X,
IDRMABYP,
TRTPSC2X,
TRTPSC3X,
TRTPSBY2,
TRTPSBY3,
TRTPSBY4,
TRTPSBY5,
TRTPSBY7,
TRTPSBY8,
PLATFLYP,
PLATBFXP,
PLATBFYP,
PLATBFZ1,
PLATBFZ2,
PLATBFZ3,
PLATRAZ1,
PLATRAY1,
PLATRAZ2,
PLATRAY2,
PLATRAZ3,
PLATRAY3,
PLATRAZ4,
PLATRAY4,
PLATRAZ5,
PLATRAY5,
PLATRAZ6,
PLATRAY6,
PLATRVZ1,
PLATRVY1,
PLATRVZ2,
PLATRVY2,
PLATRHZ1,
PLATRHY1,
PLATRHZ2,
PLATRHY2,
PLATRHZE,
PLATRVZE,
PLATRHZP,
PLATRHYP,
PLATRAXA,
PLATRAXB
) values (    1,
   236.0000,   400.0000,   380.0000,   320.0000,   400.0000,   112.0000,
   355.0000,   255.0000,   300.0000,   300.0000,   112.0000,   115.0000,
   115.0000,   200.0000,   200.0000,    50.0000,   100.0000,   100.0000,
   150.0000,   340.0000,    80.0000,    80.0000,   150.0000,   -70.0000,
   420.0000,   290.0000,   205.0000,    80.0000,   233.0000,   150.0000,
  -350.0000,     0.0000,  -650.0000,  -950.0000,  -650.0000,  -950.0000,
  -350.0000,   430.0000,  -650.0000,  -950.0000,    10.0000,    28.0000,
  2400.0000,  1000.0000,    50.0000,  -300.0000,    34.0000,    40.0000,
  1080.0000, -1150.0000,     0.0000,  1150.0000,    17.0000,    12.0000,
  1000.0000,  2300.0000,    80.0000,  6000.0000,  -200.0000,  3500.0000,
   350.0000,   400.0000,   118.0000,   354.0000,   590.0000,   277.5000,
   327.5000,   -56.0000,  -168.0000,  -280.0000,   -62.5000,   157.5000,
   157.5000,   764.0000,  -210.0000,   210.0000,   758.0000,   142.5000,
  -219.0000,  -126.0000,  -126.0000,   304.0000,   304.0000,   734.0000,
   734.0000,  -350.0000,    50.0000,  -381.0000, -1450.0000,  -300.0000,
   850.0000, -1450.0000,  -364.0000, -1450.0000,   556.0000, -1370.0000,
   636.0000,   770.0000,   636.0000,   850.0000,   556.0000,   850.0000,
  -364.0000,  -300.0000,  -364.0000,  -300.0000,   636.0000, -1450.0000,
   136.0000,   850.0000,   136.0000,  2300.0000,  1000.0000,  -300.0000,
   136.0000,  -467.0000,   567.0000
);
insert into IPP2_data2tag values ('IPP2-01',    1);

comment on column IPP2_data.IPP2_data_id is 'Unique identifier';
comment on column IPP2_data.PIXPOBHE   is '1236 PIXPOBHE                                   ';
comment on column IPP2_data.PIXPOBDZ   is '1237 PIXPOBDZ                                   ';
comment on column IPP2_data.PIXPOBDX   is '1238 PIXPOBDX                                   ';
comment on column IPP2_data.PIXPOC2E   is '1239 PIXPOC2E                                   ';
comment on column IPP2_data.PIXPOC3E   is '1240 PIXPOC3E                                   ';
comment on column IPP2_data.PIXNTBHE   is '1241 PIXNTBHE                                   ';
comment on column IPP2_data.PIXNTBDZ   is '1242 PIXNTBDZ                                   ';
comment on column IPP2_data.PIXNTBDX   is '1243 PIXNTBDX                                   ';
comment on column IPP2_data.PIXNTC2E   is '1244 PIXNTC2E                                   ';
comment on column IPP2_data.PIXNTC3E   is '1245 PIXNTC3E                                   ';
comment on column IPP2_data.PIXENBHE   is '1246 PIXENBHE                                   ';
comment on column IPP2_data.PIXENBDZ   is '1247 PIXENBDZ                                   ';
comment on column IPP2_data.PIXENBDX   is '1248 PIXENBDX                                   ';
comment on column IPP2_data.PIXENC2E   is '1249 PIXENC2E                                   ';
comment on column IPP2_data.PIXENC3E   is '1250 PIXENC3E                                   ';
comment on column IPP2_data.PIXENBXP   is '1251 PIXENBXP                                   ';
comment on column IPP2_data.PIXENBZP   is '1252 PIXENBZP                                   ';
comment on column IPP2_data.IDRMABHE   is '1253 IDRMABHE                                   ';
comment on column IPP2_data.IDRMABDZ   is '1254 IDRMABDZ                                   ';
comment on column IPP2_data.IDRMABDX   is '1255 IDRMABDX                                   ';
comment on column IPP2_data.IDRMAC2E   is '1256 IDRMAC2E                                   ';
comment on column IPP2_data.IDRMAC3E   is '1257 IDRMAC3E                                   ';
comment on column IPP2_data.IDRMABXP   is '1258 IDRMABXP                                   ';
comment on column IPP2_data.IDRMABZP   is '1259 IDRMABZP                                   ';
comment on column IPP2_data.TRTPSBHE   is '1260 TRTPSBHE                                   ';
comment on column IPP2_data.TRTPSBDZ   is '1261 TRTPSBDZ                                   ';
comment on column IPP2_data.TRTPSBDX   is '1262 TRTPSBDX                                   ';
comment on column IPP2_data.TRTPSC2E   is '1263 TRTPSC2E                                   ';
comment on column IPP2_data.TRTPSC3E   is '1264 TRTPSC3E                                   ';
comment on column IPP2_data.TRTPSBXP   is '1265 TRTPSBXP                                   ';
comment on column IPP2_data.TRTPSBZ1   is '1266 TRTPSBZ1                                   ';
comment on column IPP2_data.TRTPSBY1   is '1267 TRTPSBY1                                   ';
comment on column IPP2_data.TRTPSBZ2   is '1268 TRTPSBZ2                                   ';
comment on column IPP2_data.TRTPSBZ3   is '1269 TRTPSBZ3                                   ';
comment on column IPP2_data.TRTPSBZ4   is '1270 TRTPSBZ4                                   ';
comment on column IPP2_data.TRTPSBZ5   is '1271 TRTPSBZ5                                   ';
comment on column IPP2_data.TRTPSBZ6   is '1272 TRTPSBZ6                                   ';
comment on column IPP2_data.TRTPSBY6   is '1273 TRTPSBY6                                   ';
comment on column IPP2_data.TRTPSBZ7   is '1274 TRTPSBZ7                                   ';
comment on column IPP2_data.TRTPSBZ8   is '1275 TRTPSBZ8                                   ';
comment on column IPP2_data.TRTPSYGA   is '1276 TRTPSYGA                                   ';
comment on column IPP2_data.PLATFLTH   is '1277 PLATFLTH                                   ';
comment on column IPP2_data.PLATFLDZ   is '1278 PLATFLDZ                                   ';
comment on column IPP2_data.PLATFLDX   is '1279 PLATFLDX                                   ';
comment on column IPP2_data.PLATFLXP   is '1280 PLATFLXP                                   ';
comment on column IPP2_data.PLATFLZP   is '1281 PLATFLZP                                   ';
comment on column IPP2_data.PLATBFTH   is '1282 PLATBFTH                                   ';
comment on column IPP2_data.PLATBFDZ   is '1283 PLATBFDZ                                   ';
comment on column IPP2_data.PLATBFDX   is '1284 PLATBFDX                                   ';
comment on column IPP2_data.PLATBFZA   is '1285 PLATBFZA                                   ';
comment on column IPP2_data.PLATBFZB   is '1286 PLATBFZB                                   ';
comment on column IPP2_data.PLATBFZC   is '1287 PLATBFZC                                   ';
comment on column IPP2_data.PLATRARO   is '1288 PLATRARO                                   ';
comment on column IPP2_data.PLATRARI   is '1289 PLATRARI                                   ';
comment on column IPP2_data.PLATRAHE   is '1290 PLATRAHE                                   ';
comment on column IPP2_data.PLATRAZE   is '1291 PLATRAZE                                   ';
comment on column IPP2_data.PLATRADH   is '1292 PLATRADH                                   ';
comment on column IPP2_data.XPOS       is '1293 XPOS                                       ';
comment on column IPP2_data.YPOS       is '1294 YPOS                                       ';
comment on column IPP2_data.ZPOS       is '1295 ZPOS                                       ';
comment on column IPP2_data.PIXPOC2P   is '1296 PIXPOC2P                                   ';
comment on column IPP2_data.PIXPOC3P   is '1297 PIXPOC3P                                   ';
comment on column IPP2_data.PIXPOBY1   is '1298 PIXPOBY1                                   ';
comment on column IPP2_data.PIXPOBY2   is '1299 PIXPOBY2                                   ';
comment on column IPP2_data.PIXPOBY3   is '1300 PIXPOBY3                                   ';
comment on column IPP2_data.PIXNTC2P   is '1301 PIXNTC2P                                   ';
comment on column IPP2_data.PIXNTC3P   is '1302 PIXNTC3P                                   ';
comment on column IPP2_data.PIXNTBY1   is '1303 PIXNTBY1                                   ';
comment on column IPP2_data.PIXNTBY2   is '1304 PIXNTBY2                                   ';
comment on column IPP2_data.PIXNTBY3   is '1305 PIXNTBY3                                   ';
comment on column IPP2_data.PIXNTBXO   is '1306 PIXNTBXO                                   ';
comment on column IPP2_data.PIXENC2P   is '1307 PIXENC2P                                   ';
comment on column IPP2_data.PIXENC3P   is '1308 PIXENC3P                                   ';
comment on column IPP2_data.PIXENBYP   is '1309 PIXENBYP                                   ';
comment on column IPP2_data.IDRMAC2X   is '1310 IDRMAC2X                                   ';
comment on column IPP2_data.IDRMAC3X   is '1311 IDRMAC3X                                   ';
comment on column IPP2_data.IDRMABYP   is '1312 IDRMABYP                                   ';
comment on column IPP2_data.TRTPSC2X   is '1313 TRTPSC2X                                   ';
comment on column IPP2_data.TRTPSC3X   is '1314 TRTPSC3X                                   ';
comment on column IPP2_data.TRTPSBY2   is '1315 TRTPSBY2                                   ';
comment on column IPP2_data.TRTPSBY3   is '1316 TRTPSBY3                                   ';
comment on column IPP2_data.TRTPSBY4   is '1317 TRTPSBY4                                   ';
comment on column IPP2_data.TRTPSBY5   is '1318 TRTPSBY5                                   ';
comment on column IPP2_data.TRTPSBY7   is '1319 TRTPSBY7                                   ';
comment on column IPP2_data.TRTPSBY8   is '1320 TRTPSBY8                                   ';
comment on column IPP2_data.PLATFLYP   is '1321 PLATFLYP                                   ';
comment on column IPP2_data.PLATBFXP   is '1322 PLATBFXP                                   ';
comment on column IPP2_data.PLATBFYP   is '1323 PLATBFYP                                   ';
comment on column IPP2_data.PLATBFZ1   is '1324 PLATBFZ1                                   ';
comment on column IPP2_data.PLATBFZ2   is '1325 PLATBFZ2                                   ';
comment on column IPP2_data.PLATBFZ3   is '1326 PLATBFZ3                                   ';
comment on column IPP2_data.PLATRAZ1   is '1327 PLATRAZ1                                   ';
comment on column IPP2_data.PLATRAY1   is '1328 PLATRAY1                                   ';
comment on column IPP2_data.PLATRAZ2   is '1329 PLATRAZ2                                   ';
comment on column IPP2_data.PLATRAY2   is '1330 PLATRAY2                                   ';
comment on column IPP2_data.PLATRAZ3   is '1331 PLATRAZ3                                   ';
comment on column IPP2_data.PLATRAY3   is '1332 PLATRAY3                                   ';
comment on column IPP2_data.PLATRAZ4   is '1333 PLATRAZ4                                   ';
comment on column IPP2_data.PLATRAY4   is '1334 PLATRAY4                                   ';
comment on column IPP2_data.PLATRAZ5   is '1335 PLATRAZ5                                   ';
comment on column IPP2_data.PLATRAY5   is '1336 PLATRAY5                                   ';
comment on column IPP2_data.PLATRAZ6   is '1337 PLATRAZ6                                   ';
comment on column IPP2_data.PLATRAY6   is '1338 PLATRAY6                                   ';
comment on column IPP2_data.PLATRVZ1   is '1339 PLATRVZ1                                   ';
comment on column IPP2_data.PLATRVY1   is '1340 PLATRVY1                                   ';
comment on column IPP2_data.PLATRVZ2   is '1341 PLATRVZ2                                   ';
comment on column IPP2_data.PLATRVY2   is '1342 PLATRVY2                                   ';
comment on column IPP2_data.PLATRHZ1   is '1343 PLATRHZ1                                   ';
comment on column IPP2_data.PLATRHY1   is '1344 PLATRHY1                                   ';
comment on column IPP2_data.PLATRHZ2   is '1345 PLATRHZ2                                   ';
comment on column IPP2_data.PLATRHY2   is '1346 PLATRHY2                                   ';
comment on column IPP2_data.PLATRHZE   is '1347 PLATRHZE                                   ';
comment on column IPP2_data.PLATRVZE   is '1348 PLATRVZE                                   ';
comment on column IPP2_data.PLATRHZP   is '1349 PLATRHZP                                   ';
comment on column IPP2_data.PLATRHYP   is '1350 PLATRHYP                                   ';
comment on column IPP2_data.PLATRAXA   is '1351 PLATRAXA                                   ';
comment on column IPP2_data.PLATRAXB   is '1352 PLATRAXB                                   ';

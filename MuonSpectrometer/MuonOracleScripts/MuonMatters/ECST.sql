SET echo OFF;
set linesize 132;
drop   table ECST_data2tag cascade constraints;
drop   table ECST_data cascade constraints;
create table ECST_data (
  ECST_data_id   number(10),
STOTHIC1       float(63),
STOTHIC2       float(63),
STOTHIC3       float(63),
STOTHIC4       float(63),
STOLENGT       float(63),
STOLENGA       float(63),
STOXPREF       float(63),
STOYPREF       float(63),
STOHEIGH       float(63),
STOHEIG1       float(63),
STOHEIG2       float(63),
STOHEIG3       float(63),
STOHEIG4       float(63),
STODZBOT       float(63),
STODZIN1       float(63),
STODZIN2       float(63),
STODZIN3       float(63),
STODZIN4       float(63),
STODZIN5       float(63),
STODZTOP       float(63),
STOANGLE       float(63),
STOYPOS       float(63),
STOGECXZ       float(63),
STOGECHE       float(63),
STOCLCZP       float(63),
STOANGD2       float(63),
STOANGD4       float(63),
STOLENG1       float(63),
STOFXPOS       float(63),
STOZPOS       float(63),
STOUEYP       float(63),
STOTEYP       float(63),
STOTEZP       float(63),
STOFLXA       float(63),
STOFLYA       float(63),
STOFLXB       float(63),
STOFLYB       float(63),
STOFLXC       float(63),
STOFLYC       float(63),
STOFLXD       float(63),
STOFLYD       float(63),
STOFLXE       float(63),
STOFLYE       float(63),
STOFLXF       float(63),
STOFLYF       float(63),
STOFLXG       float(63),
STOFLYG       float(63),
STOFLXH       float(63),
STOFLYH       float(63),
STOFLXI       float(63),
STOFLYI       float(63),
STOCOXA       float(63),
STOCOYA       float(63),
STOCOXA1       float(63),
STOCOYA1       float(63),
STOCOXB       float(63),
STOCOYB       float(63),
STOCOXB1       float(63),
STOCOYB1       float(63),
STOCOXC       float(63),
STOCOYC       float(63),
STOCOXC1       float(63),
STOCOYC1       float(63),
STOCOXD       float(63),
STOCOYD       float(63),
STOCOXD1       float(63),
STOCOYD1       float(63),
STOCOXE       float(63),
STOCOYE       float(63),
STOCOXE1       float(63),
STOCOYE1       float(63),
STOCOXF       float(63),
STOCOYF       float(63),
STOCOXF1       float(63),
STOCOYF1       float(63),
STOCOXG       float(63),
STOCOYG       float(63),
STOCOXG1       float(63),
STOCOYG1       float(63),
STOCOXH       float(63),
STOCOYH       float(63),
STOCOXH1       float(63),
STOCOYH1       float(63),
STOCOXI       float(63),
STOCOYI       float(63),
STOCOXI1       float(63),
STOCOYI1       float(63),
STOCO3DX       float(63),
STOCO3DY       float(63),
STOCO3DZ       float(63),
STOCO3ZP       float(63),
STOCO3YP       float(63),
STOUEXA       float(63),
STOUEYA       float(63),
STOUEXB       float(63),
STOUEYB       float(63),
STOUEXC       float(63),
STOUEYC       float(63),
STOUECXA       float(63),
STOUECYA       float(63),
STOUECXB       float(63),
STOUECYB       float(63),
STOUECZP       float(63),
STOUECYP       float(63)
) ;

alter table ECST_data add constraint ECST_data_pk
primary key (ECST_data_id);

create table ECST_data2tag (
  ECST_vers      varchar2(255),
  ECST_data_id   number(10)
) ;

alter table ECST_data2tag add constraint ECST_data2tag_pk
primary key (ECST_vers, ECST_data_id);
alter table ECST_data2tag add constraint ECST_data2tag_vers_fk
foreign key (ECST_vers) references hvs_nodevers (vers_name);
alter table ECST_data2tag add constraint ECST_data2tag_data_fk
foreign key (ECST_data_id) references ECST_data (ECST_data_id);

delete from hvs_relation where CHILD_NODENAME = 'ECST';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'ECST';
delete from hvs_node     where NODE_NAME = 'ECST';

insert into hvs_node     values ('ECST','MuonSpectrometer', '');
insert into hvs_nodevers values ('ECST', 'ECST-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','ECST','ECST-01');
insert into ECST_data (
ECST_data_id,
STOTHIC1,
STOTHIC2,
STOTHIC3,
STOTHIC4,
STOLENGT,
STOLENGA,
STOXPREF,
STOYPREF,
STOHEIGH,
STOHEIG1,
STOHEIG2,
STOHEIG3,
STOHEIG4,
STODZBOT,
STODZIN1,
STODZIN2,
STODZIN3,
STODZIN4,
STODZIN5,
STODZTOP,
STOANGLE,
STOYPOS,
STOGECXZ,
STOGECHE,
STOCLCZP,
STOANGD2,
STOANGD4,
STOLENG1,
STOFXPOS,
STOZPOS,
STOUEYP,
STOTEYP,
STOTEZP,
STOFLXA,
STOFLYA,
STOFLXB,
STOFLYB,
STOFLXC,
STOFLYC,
STOFLXD,
STOFLYD,
STOFLXE,
STOFLYE,
STOFLXF,
STOFLYF,
STOFLXG,
STOFLYG,
STOFLXH,
STOFLYH,
STOFLXI,
STOFLYI,
STOCOXA,
STOCOYA,
STOCOXA1,
STOCOYA1,
STOCOXB,
STOCOYB,
STOCOXB1,
STOCOYB1,
STOCOXC,
STOCOYC,
STOCOXC1,
STOCOYC1,
STOCOXD,
STOCOYD,
STOCOXD1,
STOCOYD1,
STOCOXE,
STOCOYE,
STOCOXE1,
STOCOYE1,
STOCOXF,
STOCOYF,
STOCOXF1,
STOCOYF1,
STOCOXG,
STOCOYG,
STOCOXG1,
STOCOYG1,
STOCOXH,
STOCOYH,
STOCOXH1,
STOCOYH1,
STOCOXI,
STOCOYI,
STOCOXI1,
STOCOYI1,
STOCO3DX,
STOCO3DY,
STOCO3DZ,
STOCO3ZP,
STOCO3YP,
STOUEXA,
STOUEYA,
STOUEXB,
STOUEYB,
STOUEXC,
STOUEYC,
STOUECXA,
STOUECYA,
STOUECXB,
STOUECYB,
STOUECZP,
STOUECYP
) values (    1,
    50.0000,    50.0000,    50.0000,    50.0000,  2770.0000,  2084.0000,
     0.0000,     0.0000,  6930.0000,   940.0000,  3204.0000,  1135.0000,
   815.0000,  1650.0000,  2168.0000,   741.0000,   527.0000,   944.0000,
   666.0000,  1915.0000,    60.0000,  5800.0000,   310.0000,  1300.0000,
   260.0000,    30.0000,    15.0000,  2670.0000,  1360.0000,  2500.0000,
  5547.5000,  6522.5000,   957.5000,     0.0000,     0.0000,     0.0000,
  4980.0000,   527.0000,  4980.0000,   527.0000,  3204.0000,  1268.0000,
  3204.0000,  2168.0000,  1645.1543,  2168.0000,   940.0000,  1650.0000,
   940.0000,  1650.0000,     0.0000,     0.0000,     0.0000,    50.0000,
     0.0000,     0.0000,  4980.0000,    50.0000,  4980.0000,   527.0000,
  4980.0000,   477.0000,  4980.0000,   527.0000,  3204.0000,   477.0000,
  3154.0000,  1268.0000,  3204.0000,  1239.1324,  3154.0000,  2168.0000,
  1645.1543,  2118.0000,  1631.7568,  2168.0000,   940.0000,  2118.0000,
   990.0000,  1650.0000,   940.0000,  1600.0000,   990.0000,  1650.0000,
     0.0000,  1600.0000,     0.0000,  1550.0000,    50.0000,  2670.0000,
   825.0000,    25.0000,  1385.0000,     0.0000,  1385.0000,   666.0000,
  1042.0000,   944.0000,  1335.0000,    50.0000,  1335.0000,   477.0000,
  -430.5000,  -975.0000
);
insert into ECST_data2tag values ('ECST-01',    1);

comment on column ECST_data.ECST_data_id is 'Unique identifier';
comment on column ECST_data.STOTHIC1   is 'STOTHIC1                                        ';
comment on column ECST_data.STOTHIC2   is 'STOTHIC2                                        ';
comment on column ECST_data.STOTHIC3   is 'STOTHIC3                                        ';
comment on column ECST_data.STOTHIC4   is 'STOTHIC4                                        ';
comment on column ECST_data.STOLENGT   is 'STOLENGT                                        ';
comment on column ECST_data.STOLENGA   is 'STOLENGA                                        ';
comment on column ECST_data.STOXPREF   is 'STOXPREF                                        ';
comment on column ECST_data.STOYPREF   is 'STOYPREF                                        ';
comment on column ECST_data.STOHEIGH   is 'STOHEIGH                                        ';
comment on column ECST_data.STOHEIG1   is 'STOHEIG1                                        ';
comment on column ECST_data.STOHEIG2   is 'STOHEIG2                                        ';
comment on column ECST_data.STOHEIG3   is 'STOHEIG3                                        ';
comment on column ECST_data.STOHEIG4   is 'STOHEIG4                                        ';
comment on column ECST_data.STODZBOT   is 'STODZBOT                                        ';
comment on column ECST_data.STODZIN1   is 'STODZIN1                                        ';
comment on column ECST_data.STODZIN2   is 'STODZIN2                                        ';
comment on column ECST_data.STODZIN3   is 'STODZIN3                                        ';
comment on column ECST_data.STODZIN4   is 'STODZIN4                                        ';
comment on column ECST_data.STODZIN5   is 'STODZIN5                                        ';
comment on column ECST_data.STODZTOP   is 'STODZTOP                                        ';
comment on column ECST_data.STOANGLE   is 'STOANGLE                                        ';
comment on column ECST_data.STOYPOS    is 'STOYPOS                                         ';
comment on column ECST_data.STOGECXZ   is 'STOGECXZ                                        ';
comment on column ECST_data.STOGECHE   is 'STOGECHE                                        ';
comment on column ECST_data.STOCLCZP   is 'STOCLCZP                                        ';
comment on column ECST_data.STOANGD2   is 'STOANGD2                                        ';
comment on column ECST_data.STOANGD4   is 'STOANGD4                                        ';
comment on column ECST_data.STOLENG1   is 'STOLENG1                                        ';
comment on column ECST_data.STOFXPOS   is 'STOFXPOS                                        ';
comment on column ECST_data.STOZPOS    is 'STOZPOS                                         ';
comment on column ECST_data.STOUEYP    is 'STOUEYP                                         ';
comment on column ECST_data.STOTEYP    is 'STOTEYP                                         ';
comment on column ECST_data.STOTEZP    is 'STOTEZP                                         ';
comment on column ECST_data.STOFLXA    is 'STOFLXA                                         ';
comment on column ECST_data.STOFLYA    is 'STOFLYA                                         ';
comment on column ECST_data.STOFLXB    is 'STOFLXB                                         ';
comment on column ECST_data.STOFLYB    is 'STOFLYB                                         ';
comment on column ECST_data.STOFLXC    is 'STOFLXC                                         ';
comment on column ECST_data.STOFLYC    is 'STOFLYC                                         ';
comment on column ECST_data.STOFLXD    is 'STOFLXD                                         ';
comment on column ECST_data.STOFLYD    is 'STOFLYD                                         ';
comment on column ECST_data.STOFLXE    is 'STOFLXE                                         ';
comment on column ECST_data.STOFLYE    is 'STOFLYE                                         ';
comment on column ECST_data.STOFLXF    is 'STOFLXF                                         ';
comment on column ECST_data.STOFLYF    is 'STOFLYF                                         ';
comment on column ECST_data.STOFLXG    is 'STOFLXG                                         ';
comment on column ECST_data.STOFLYG    is 'STOFLYG                                         ';
comment on column ECST_data.STOFLXH    is 'STOFLXH                                         ';
comment on column ECST_data.STOFLYH    is 'STOFLYH                                         ';
comment on column ECST_data.STOFLXI    is 'STOFLXI                                         ';
comment on column ECST_data.STOFLYI    is 'STOFLYI                                         ';
comment on column ECST_data.STOCOXA    is 'STOCOXA                                         ';
comment on column ECST_data.STOCOYA    is 'STOCOYA                                         ';
comment on column ECST_data.STOCOXA1   is 'STOCOXA1                                        ';
comment on column ECST_data.STOCOYA1   is 'STOCOYA1                                        ';
comment on column ECST_data.STOCOXB    is 'STOCOXB                                         ';
comment on column ECST_data.STOCOYB    is 'STOCOYB                                         ';
comment on column ECST_data.STOCOXB1   is 'STOCOXB1                                        ';
comment on column ECST_data.STOCOYB1   is 'STOCOYB1                                        ';
comment on column ECST_data.STOCOXC    is 'STOCOXC                                         ';
comment on column ECST_data.STOCOYC    is 'STOCOYC                                         ';
comment on column ECST_data.STOCOXC1   is 'STOCOXC1                                        ';
comment on column ECST_data.STOCOYC1   is 'STOCOYC1                                        ';
comment on column ECST_data.STOCOXD    is 'STOCOXD                                         ';
comment on column ECST_data.STOCOYD    is 'STOCOYD                                         ';
comment on column ECST_data.STOCOXD1   is 'STOCOXD1                                        ';
comment on column ECST_data.STOCOYD1   is 'STOCOYD1                                        ';
comment on column ECST_data.STOCOXE    is 'STOCOXE                                         ';
comment on column ECST_data.STOCOYE    is 'STOCOYE                                         ';
comment on column ECST_data.STOCOXE1   is 'STOCOXE1                                        ';
comment on column ECST_data.STOCOYE1   is 'STOCOYE1                                        ';
comment on column ECST_data.STOCOXF    is 'STOCOXF                                         ';
comment on column ECST_data.STOCOYF    is 'STOCOYF                                         ';
comment on column ECST_data.STOCOXF1   is 'STOCOXF1                                        ';
comment on column ECST_data.STOCOYF1   is 'STOCOYF1                                        ';
comment on column ECST_data.STOCOXG    is 'STOCOXG                                         ';
comment on column ECST_data.STOCOYG    is 'STOCOYG                                         ';
comment on column ECST_data.STOCOXG1   is 'STOCOXG1                                        ';
comment on column ECST_data.STOCOYG1   is 'STOCOYG1                                        ';
comment on column ECST_data.STOCOXH    is 'STOCOXH                                         ';
comment on column ECST_data.STOCOYH    is 'STOCOYH                                         ';
comment on column ECST_data.STOCOXH1   is 'STOCOXH1                                        ';
comment on column ECST_data.STOCOYH1   is 'STOCOYH1                                        ';
comment on column ECST_data.STOCOXI    is 'STOCOXI                                         ';
comment on column ECST_data.STOCOYI    is 'STOCOYI                                         ';
comment on column ECST_data.STOCOXI1   is 'STOCOXI1                                        ';
comment on column ECST_data.STOCOYI1   is 'STOCOYI1                                        ';
comment on column ECST_data.STOCO3DX   is 'STOCO3DX                                        ';
comment on column ECST_data.STOCO3DY   is 'STOCO3DY                                        ';
comment on column ECST_data.STOCO3DZ   is 'STOCO3DZ                                        ';
comment on column ECST_data.STOCO3ZP   is 'STOCO3ZP                                        ';
comment on column ECST_data.STOCO3YP   is 'STOCO3YP                                        ';
comment on column ECST_data.STOUEXA    is 'STOUEXA                                         ';
comment on column ECST_data.STOUEYA    is 'STOUEYA                                         ';
comment on column ECST_data.STOUEXB    is 'STOUEXB                                         ';
comment on column ECST_data.STOUEYB    is 'STOUEYB                                         ';
comment on column ECST_data.STOUEXC    is 'STOUEXC                                         ';
comment on column ECST_data.STOUEYC    is 'STOUEYC                                         ';
comment on column ECST_data.STOUECXA   is 'STOUECXA                                        ';
comment on column ECST_data.STOUECYA   is 'STOUECYA                                        ';
comment on column ECST_data.STOUECXB   is 'STOUECXB                                        ';
comment on column ECST_data.STOUECYB   is 'STOUECYB                                        ';
comment on column ECST_data.STOUECZP   is 'STOUECZP                                        ';
comment on column ECST_data.STOUECYP   is 'STOUECYP                                        ';

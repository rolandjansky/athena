SET echo OFF;
set linesize 132;
drop   table FEET_data2tag cascade constraints;
drop   table FEET_data cascade constraints;
create table FEET_data (
  FEET_data_id   number(10),
MAINPLXO       float(63),
MNPLGXWI       float(63),
MNPLRCDX       float(63),
MNPLDXEX       float(63),
MNPLUPDX       float(63),
MNPLUPXW       float(63),
MNPLMIDX       float(63),
MAINPLYO       float(63),
MAINPLHE       float(63),
MAINPLH1       float(63),
MNPLRCYP       float(63),
MNPLSRCY       float(63),
MNPLUPDY       float(63),
MNPLMIDY       float(63),
MNPLPEHE       float(63),
MAINPLDZ       float(63),
MNPLZSEP       float(63),
MNPLRCRA       float(63),
MNPLSRCR       float(63),
MINCPLXW       float(63),
MINCPLYH       float(63),
MINCPLTZ       float(63),
MINCPLZL       float(63),
SLACPLXW       float(63),
SLACPLYH       float(63),
SLACPLTZ       float(63),
SLACPLZL       float(63),
GRNDPLZL       float(63),
VCNPLXWI       float(63),
VCNPLYHE       float(63),
VCNPLDXE       float(63),
VCNPLDYE       float(63),
UCNPLXWI       float(63),
UCNPLYHE       float(63),
XWIDTH       float(63),
UPPERHEY       float(63),
UPPERLEZ       float(63),
LOWERHEY       float(63),
LOWERLEZ       float(63),
TOTALHEY       float(63),
CENTRLEZ       float(63),
CENTRXTH       float(63),
MIDLLYTH       float(63),
VERTIZTH       float(63),
MIDLLYPO       float(63),
EXTREHEY       float(63),
EXTRELEZ       float(63),
CENTRHEY       float(63),
CNFEVOXW       float(63),
CNFEVOYH       float(63),
CNFEVOZL       float(63),
CNFEVOXT       float(63),
CNFEVOYT       float(63),
CNFEVOZT       float(63),
G12LPXWT       float(63),
G12IPSXO       float(63),
G12IPSLX       float(63),
G12IPLLX       float(63),
G12IPLSX       float(63),
G12RPXWI       float(63),
G12LPXW1       float(63),
G12LPX12       float(63),
G12IPXWI       float(63),
G12UMPWI       float(63),
G12SMPWI       float(63),
G12YORIG       float(63),
G12LPYHT       float(63),
G12LPYH1       float(63),
G12LPYH2       float(63),
G12LPYTH       float(63),
G12UPYTH       float(63),
G12RPYDI       float(63),
G12UMPYH       float(63),
G12UPZLE       float(63),
G12UMPZL       float(63),
G12BPZLE       float(63),
G12LPZLE       float(63),
G12RPZLE       float(63),
G12SMPZL       float(63),
G23LPXWT       float(63),
G23IPSXO       float(63),
G23IPSLX       float(63),
G23IPLLX       float(63),
G23IPLSX       float(63),
G23RPXWI       float(63),
G23LPXW1       float(63),
G23LPX12       float(63),
G23IPXWI       float(63),
G23UMPWI       float(63),
G23SMPWI       float(63),
G34LPXWT       float(63),
G34IPSXO       float(63),
G34IPSLX       float(63),
G34IPLLX       float(63),
G34IPLSX       float(63),
G34RPXWI       float(63),
G34LPXW1       float(63),
G34LPX12       float(63),
G34IPXWI       float(63),
G34UMPWI       float(63),
G34SMPWI       float(63),
EXMPHEIG       float(63),
EXMPRCRA       float(63),
EXMPRCDX       float(63),
EXMPRCYP       float(63),
EXMCPYHE       float(63),
EXMCPZLE       float(63),
EXGPZLEN       float(63),
EXSCPZLE       float(63),
EXRSZOFF       float(63),
EXMPHEI1       float(63),
EXMPHEI2       float(63),
EXMPXWID       float(63),
EXMPXWI1       float(63),
EXMPXWI2       float(63),
EXMPYHI1       float(63),
EXMPYHI2       float(63),
EXMPYHI3       float(63),
EXMPYHI4       float(63),
EXMPXDI1       float(63),
EXMPXDI2       float(63),
EXMPYDI2       float(63),
EXVCPLDX       float(63),
EXVCPLDY       float(63),
EXMPZSEP       float(63),
EXSPRCRA       float(63),
EXSPRCOF       float(63),
EXVCPTHI       float(63),
EXVCPXEX       float(63),
EXVCPHEI       float(63),
EXVCPYCU       float(63),
EXVCPXCU       float(63),
EXCFVXWI       float(63),
EXCFVYHE       float(63),
EXCFVZLE       float(63),
EXRSUZLE       float(63),
EXRSLDZL       float(63),
EXRSTYHE       float(63),
EXRSLYHE       float(63),
EXRSEYHE       float(63),
EXRSCYHE       float(63),
EXRSCZLE       float(63),
EXRSMZLE       float(63),
EXRSCXWI       float(63),
EXRSMXWI       float(63),
EXRSVXWI       float(63),
EXRSVZI1       float(63),
EXRSEZLE       float(63),
EXRSEZOF       float(63),
EXRSC1DE       float(63),
EXRSC1WI       float(63),
EXRSC1XE       float(63),
EXRSC2XE       float(63),
EXRSC2Z1       float(63),
EXRSC2Z2       float(63),
EXRSMCDE       float(63),
EXRSMCWI       float(63),
EXRSMCZO       float(63),
EXRSMCY1       float(63),
EXRSMCY2       float(63),
STDFOOXP       float(63),
STDFOOYP       float(63),
ZPOSFEE1       float(63),
ZPOSFEE2       float(63),
ZPOSFEE3       float(63),
ZPOSFEE4       float(63),
ZPOSFEE5       float(63),
GIRDYHEI       float(63),
GIRDXPOS       float(63),
MPLAHCEC       float(63),
DUMZ       float(63),
DUMPHI       float(63),
MAINPLXA       float(63),
MAINPLYA       float(63),
MAINPLXB       float(63),
MAINPLYB       float(63),
MAINPLXC       float(63),
MAINPLYC       float(63),
MAPLRCXC       float(63),
MAPLRCYC       float(63),
MAIPLXC3       float(63),
MAIPLYC3       float(63),
MPLXEXTR       float(63),
MPLYEXTR       float(63),
MPLREXTR       float(63),
MAIPLXC2       float(63),
MAIPLYC2       float(63),
MPLXCEC1       float(63),
MPLYCEC1       float(63),
MPLXCEC2       float(63),
MPLYCEC2       float(63),
A       float(63),
B       float(63),
APRIME       float(63),
BPRIME       float(63),
ASECONDE       float(63),
BSECONDE       float(63),
MAINPLXD       float(63),
MAINPLYD       float(63),
MAINPLXE       float(63),
MAINPLYE       float(63),
MAIPLXC1       float(63),
MAIPLYC1       float(63),
MAINPLXF       float(63),
MAINPLYF       float(63),
MAINPLXG       float(63),
MAINPLYG       float(63),
MAINPLXH       float(63),
MAINPLYH       float(63),
MAINPLXI       float(63),
MAINPLYI       float(63),
MAINPLXJ       float(63),
MAINPLYJ       float(63),
ALPHA1       float(63),
COAL1       float(63),
SIAL1       float(63),
MICPDIAG       float(63),
MICPXPOS       float(63),
MICPYPOS       float(63),
ALPHA2       float(63),
COAL2       float(63),
SIAL2       float(63),
SLCPDIAG       float(63),
SLCPXPOS       float(63),
SLCPYPOS       float(63),
GRNPXWID       float(63),
GRNPYHEI       float(63),
GRNPXPOS       float(63),
GRNPYPOS       float(63),
VECPXPOS       float(63),
VECPYPOS       float(63),
VECPZLEN       float(63),
UPCPXPOS       float(63),
UPCPYPOS       float(63),
UPCPZLEN       float(63),
MIDYPOSI       float(63),
POSX       float(63),
POSY       float(63),
CNFEVOXP       float(63),
CNFEVOYP       float(63),
G12BOPXA       float(63),
G12BOPXB       float(63),
G12BOPXC       float(63),
G12BOPXD       float(63),
G12BOPXE       float(63),
G12BOPXF       float(63),
G12BOPYA       float(63),
G12BOPYB       float(63),
G12BOPYC       float(63),
G12BOPYD       float(63),
G12BOPYE       float(63),
G12BOPYF       float(63),
G12IPZLE       float(63),
G12IPLYH       float(63),
G12IPLYP       float(63),
G12IPSYH       float(63),
G12IPSYP       float(63),
G12IPSX1       float(63),
G12IPLX1       float(63),
G12IPLX2       float(63),
G12IPSX2       float(63),
G12UPXWI       float(63),
G12UPYHE       float(63),
G12UPYPO       float(63),
G12REPXA       float(63),
G12REPXB       float(63),
G12REPXC       float(63),
G12REPXD       float(63),
G12REPYA       float(63),
G12REPYB       float(63),
G12REPYC       float(63),
G12REPYD       float(63),
G12REPZP       float(63),
G12UMPYP       float(63),
G12UMPXP       float(63),
G12SMPYH       float(63),
G12SMPYP       float(63),
G12SMPXP       float(63),
G12SMPZP       float(63),
G23BOPXA       float(63),
G23BOPXB       float(63),
G23BOPXC       float(63),
G23BOPXD       float(63),
G23BOPXE       float(63),
G23BOPXF       float(63),
G23IPSX1       float(63),
G23IPLX1       float(63),
G23IPLX2       float(63),
G23IPSX2       float(63),
G23UPXWI       float(63),
G23UPYHE       float(63),
G23REPXA       float(63),
G23REPXB       float(63),
G23REPXC       float(63),
G23REPXD       float(63),
G23REPYA       float(63),
G23REPYB       float(63),
G23REPYC       float(63),
G23REPYD       float(63),
G23UMPXP       float(63),
G23SMPXP       float(63),
G34BOPXA       float(63),
G34BOPXB       float(63),
G34BOPXC       float(63),
G34BOPXD       float(63),
G34BOPXE       float(63),
G34BOPXF       float(63),
G34IPSX1       float(63),
G34IPLX1       float(63),
G34IPLX2       float(63),
G34IPSX2       float(63),
G34UPXWI       float(63),
G34UPYHE       float(63),
G34REPXA       float(63),
G34REPXB       float(63),
G34REPXC       float(63),
G34REPXD       float(63),
G34REPYA       float(63),
G34REPYB       float(63),
G34REPYC       float(63),
G34REPYD       float(63),
G34UMPXP       float(63),
G34SMPXP       float(63),
STDFVOYP       float(63),
EXMPRCXC       float(63),
EXMPRCYC       float(63),
EXMPLXC3       float(63),
EXMPLYC3       float(63),
EXMPLXC4       float(63),
EXMPLYC4       float(63),
EXMPLXC5       float(63),
EXMPLYC5       float(63),
A2       float(63),
B2       float(63),
A2PRIME       float(63),
B2PRIME       float(63),
EXMPLAXD       float(63),
EXMPLAYD       float(63),
EXMPC2C1       float(63),
EXMPLXC1       float(63),
EXMPLYC1       float(63),
EXMPYC1A       float(63),
EXMPXC1A       float(63),
EXMPXC1B       float(63),
EXMPYC1B       float(63),
ALPHA3       float(63),
COAL3       float(63),
SIAL3       float(63),
EXMCPDIA       float(63),
EXMCPXPO       float(63),
EXMCPYPO       float(63),
EXMPLAXF       float(63),
EXMPLAYF       float(63),
EXMPLXF1       float(63),
EXMPLYF1       float(63),
EXMPLXF2       float(63),
EXMPLYF2       float(63),
EXMPLYF3       float(63),
EXMPLXF3       float(63),
EXMPLXF4       float(63),
EXMPLYF4       float(63),
EXMPLXF5       float(63),
EXMPLYF5       float(63),
EXMPLXF6       float(63),
EXMPLYF6       float(63),
EXMPLXF7       float(63),
EXMPLYF7       float(63),
EXMPLAXI       float(63),
EXMPLAXH       float(63),
EXMPLAYH       float(63),
EXUCPXPO       float(63),
EXUCPZLE       float(63),
EXVCPZLE       float(63),
EXVCPLXA       float(63),
EXVCPLYA       float(63),
EXVCPLXB       float(63),
EXVCPLYB       float(63),
EXVCPLXC       float(63),
EXVCPLYC       float(63),
EXVCPLXD       float(63),
EXVCPLYD       float(63),
EXVCPLXE       float(63),
EXVCPLYE       float(63),
EXVCPLXF       float(63),
EXVCPLYF       float(63),
EXVCPLXP       float(63),
EXVCPLYP       float(63),
EXVCPLAX       float(63),
EXVCPLAY       float(63),
EXCNFEVX       float(63),
EXCNFEVY       float(63),
EXRSLZLE       float(63),
EXRSVYHE       float(63),
EXRSMXPO       float(63),
EXRSVXPO       float(63),
EXRSEYPO       float(63),
EXRSUYPO       float(63),
EXRSLYPO       float(63),
EXRSCYPO       float(63),
EXRSVYPO       float(63),
EXRSVZP1       float(63),
EXRSVZP2       float(63),
EXRSMZPO       float(63),
EXRSEZPO       float(63),
EXRSC1YE       float(63),
EXRSC2YE       float(63),
EXRSC1XP       float(63),
EXRSC2XP       float(63),
EXRSC1ZP       float(63),
EXRSC3XP       float(63),
EXRSC3ZP       float(63),
EXRSC4Z1       float(63),
EXRSC4ZP       float(63),
EXRSMCXA       float(63),
EXRSMCYA       float(63),
EXRSMCXB       float(63),
EXRSMCYB       float(63),
EXRSMCXC       float(63),
EXRSMCYC       float(63),
EXRSMCXP       float(63),
EXRSMCZ1       float(63),
EXRSMCZP       float(63),
EXRSPOSX       float(63),
EXRSPOSY       float(63),
EXRSPOSZ       float(63),
EXRSPXAP       float(63),
EXRSPXAM       float(63),
EXRSPYAS       float(63),
GIR12ZPO       float(63),
GIR12YPO       float(63),
GIR23ZPO       float(63),
GIR34ZPO       float(63)
) ;

alter table FEET_data add constraint FEET_data_pk
primary key (FEET_data_id);

create table FEET_data2tag (
  FEET_vers      varchar2(255),
  FEET_data_id   number(10)
) ;

alter table FEET_data2tag add constraint FEET_data2tag_pk
primary key (FEET_vers, FEET_data_id);
alter table FEET_data2tag add constraint FEET_data2tag_vers_fk
foreign key (FEET_vers) references hvs_nodevers (vers_name);
alter table FEET_data2tag add constraint FEET_data2tag_data_fk
foreign key (FEET_data_id) references FEET_data (FEET_data_id);

delete from hvs_relation where CHILD_NODENAME = 'FEET';
delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-01';
delete from hvs_nodevers where NODE_NAME = 'FEET';
delete from hvs_node     where NODE_NAME = 'FEET';

insert into hvs_node     values ('FEET','MuonSpectrometer', '');
insert into hvs_nodevers values ('FEET', 'FEET-01', '');
insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-01','');
insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-01','FEET','FEET-01');
insert into FEET_data (
FEET_data_id,
MAINPLXO,
MNPLGXWI,
MNPLRCDX,
MNPLDXEX,
MNPLUPDX,
MNPLUPXW,
MNPLMIDX,
MAINPLYO,
MAINPLHE,
MAINPLH1,
MNPLRCYP,
MNPLSRCY,
MNPLUPDY,
MNPLMIDY,
MNPLPEHE,
MAINPLDZ,
MNPLZSEP,
MNPLRCRA,
MNPLSRCR,
MINCPLXW,
MINCPLYH,
MINCPLTZ,
MINCPLZL,
SLACPLXW,
SLACPLYH,
SLACPLTZ,
SLACPLZL,
GRNDPLZL,
VCNPLXWI,
VCNPLYHE,
VCNPLDXE,
VCNPLDYE,
UCNPLXWI,
UCNPLYHE,
XWIDTH,
UPPERHEY,
UPPERLEZ,
LOWERHEY,
LOWERLEZ,
TOTALHEY,
CENTRLEZ,
CENTRXTH,
MIDLLYTH,
VERTIZTH,
MIDLLYPO,
EXTREHEY,
EXTRELEZ,
CENTRHEY,
CNFEVOXW,
CNFEVOYH,
CNFEVOZL,
CNFEVOXT,
CNFEVOYT,
CNFEVOZT,
G12LPXWT,
G12IPSXO,
G12IPSLX,
G12IPLLX,
G12IPLSX,
G12RPXWI,
G12LPXW1,
G12LPX12,
G12IPXWI,
G12UMPWI,
G12SMPWI,
G12YORIG,
G12LPYHT,
G12LPYH1,
G12LPYH2,
G12LPYTH,
G12UPYTH,
G12RPYDI,
G12UMPYH,
G12UPZLE,
G12UMPZL,
G12BPZLE,
G12LPZLE,
G12RPZLE,
G12SMPZL,
G23LPXWT,
G23IPSXO,
G23IPSLX,
G23IPLLX,
G23IPLSX,
G23RPXWI,
G23LPXW1,
G23LPX12,
G23IPXWI,
G23UMPWI,
G23SMPWI,
G34LPXWT,
G34IPSXO,
G34IPSLX,
G34IPLLX,
G34IPLSX,
G34RPXWI,
G34LPXW1,
G34LPX12,
G34IPXWI,
G34UMPWI,
G34SMPWI,
EXMPHEIG,
EXMPRCRA,
EXMPRCDX,
EXMPRCYP,
EXMCPYHE,
EXMCPZLE,
EXGPZLEN,
EXSCPZLE,
EXRSZOFF,
EXMPHEI1,
EXMPHEI2,
EXMPXWID,
EXMPXWI1,
EXMPXWI2,
EXMPYHI1,
EXMPYHI2,
EXMPYHI3,
EXMPYHI4,
EXMPXDI1,
EXMPXDI2,
EXMPYDI2,
EXVCPLDX,
EXVCPLDY,
EXMPZSEP,
EXSPRCRA,
EXSPRCOF,
EXVCPTHI,
EXVCPXEX,
EXVCPHEI,
EXVCPYCU,
EXVCPXCU,
EXCFVXWI,
EXCFVYHE,
EXCFVZLE,
EXRSUZLE,
EXRSLDZL,
EXRSTYHE,
EXRSLYHE,
EXRSEYHE,
EXRSCYHE,
EXRSCZLE,
EXRSMZLE,
EXRSCXWI,
EXRSMXWI,
EXRSVXWI,
EXRSVZI1,
EXRSEZLE,
EXRSEZOF,
EXRSC1DE,
EXRSC1WI,
EXRSC1XE,
EXRSC2XE,
EXRSC2Z1,
EXRSC2Z2,
EXRSMCDE,
EXRSMCWI,
EXRSMCZO,
EXRSMCY1,
EXRSMCY2,
STDFOOXP,
STDFOOYP,
ZPOSFEE1,
ZPOSFEE2,
ZPOSFEE3,
ZPOSFEE4,
ZPOSFEE5,
GIRDYHEI,
GIRDXPOS,
MPLAHCEC,
DUMZ,
DUMPHI,
MAINPLXA,
MAINPLYA,
MAINPLXB,
MAINPLYB,
MAINPLXC,
MAINPLYC,
MAPLRCXC,
MAPLRCYC,
MAIPLXC3,
MAIPLYC3,
MPLXEXTR,
MPLYEXTR,
MPLREXTR,
MAIPLXC2,
MAIPLYC2,
MPLXCEC1,
MPLYCEC1,
MPLXCEC2,
MPLYCEC2,
A,
B,
APRIME,
BPRIME,
ASECONDE,
BSECONDE,
MAINPLXD,
MAINPLYD,
MAINPLXE,
MAINPLYE,
MAIPLXC1,
MAIPLYC1,
MAINPLXF,
MAINPLYF,
MAINPLXG,
MAINPLYG,
MAINPLXH,
MAINPLYH,
MAINPLXI,
MAINPLYI,
MAINPLXJ,
MAINPLYJ,
ALPHA1,
COAL1,
SIAL1,
MICPDIAG,
MICPXPOS,
MICPYPOS,
ALPHA2,
COAL2,
SIAL2,
SLCPDIAG,
SLCPXPOS,
SLCPYPOS,
GRNPXWID,
GRNPYHEI,
GRNPXPOS,
GRNPYPOS,
VECPXPOS,
VECPYPOS,
VECPZLEN,
UPCPXPOS,
UPCPYPOS,
UPCPZLEN,
MIDYPOSI,
POSX,
POSY,
CNFEVOXP,
CNFEVOYP,
G12BOPXA,
G12BOPXB,
G12BOPXC,
G12BOPXD,
G12BOPXE,
G12BOPXF,
G12BOPYA,
G12BOPYB,
G12BOPYC,
G12BOPYD,
G12BOPYE,
G12BOPYF,
G12IPZLE,
G12IPLYH,
G12IPLYP,
G12IPSYH,
G12IPSYP,
G12IPSX1,
G12IPLX1,
G12IPLX2,
G12IPSX2,
G12UPXWI,
G12UPYHE,
G12UPYPO,
G12REPXA,
G12REPXB,
G12REPXC,
G12REPXD,
G12REPYA,
G12REPYB,
G12REPYC,
G12REPYD,
G12REPZP,
G12UMPYP,
G12UMPXP,
G12SMPYH,
G12SMPYP,
G12SMPXP,
G12SMPZP,
G23BOPXA,
G23BOPXB,
G23BOPXC,
G23BOPXD,
G23BOPXE,
G23BOPXF,
G23IPSX1,
G23IPLX1,
G23IPLX2,
G23IPSX2,
G23UPXWI,
G23UPYHE,
G23REPXA,
G23REPXB,
G23REPXC,
G23REPXD,
G23REPYA,
G23REPYB,
G23REPYC,
G23REPYD,
G23UMPXP,
G23SMPXP,
G34BOPXA,
G34BOPXB,
G34BOPXC,
G34BOPXD,
G34BOPXE,
G34BOPXF,
G34IPSX1,
G34IPLX1,
G34IPLX2,
G34IPSX2,
G34UPXWI,
G34UPYHE,
G34REPXA,
G34REPXB,
G34REPXC,
G34REPXD,
G34REPYA,
G34REPYB,
G34REPYC,
G34REPYD,
G34UMPXP,
G34SMPXP,
STDFVOYP,
EXMPRCXC,
EXMPRCYC,
EXMPLXC3,
EXMPLYC3,
EXMPLXC4,
EXMPLYC4,
EXMPLXC5,
EXMPLYC5,
A2,
B2,
A2PRIME,
B2PRIME,
EXMPLAXD,
EXMPLAYD,
EXMPC2C1,
EXMPLXC1,
EXMPLYC1,
EXMPYC1A,
EXMPXC1A,
EXMPXC1B,
EXMPYC1B,
ALPHA3,
COAL3,
SIAL3,
EXMCPDIA,
EXMCPXPO,
EXMCPYPO,
EXMPLAXF,
EXMPLAYF,
EXMPLXF1,
EXMPLYF1,
EXMPLXF2,
EXMPLYF2,
EXMPLYF3,
EXMPLXF3,
EXMPLXF4,
EXMPLYF4,
EXMPLXF5,
EXMPLYF5,
EXMPLXF6,
EXMPLYF6,
EXMPLXF7,
EXMPLYF7,
EXMPLAXI,
EXMPLAXH,
EXMPLAYH,
EXUCPXPO,
EXUCPZLE,
EXVCPZLE,
EXVCPLXA,
EXVCPLYA,
EXVCPLXB,
EXVCPLYB,
EXVCPLXC,
EXVCPLYC,
EXVCPLXD,
EXVCPLYD,
EXVCPLXE,
EXVCPLYE,
EXVCPLXF,
EXVCPLYF,
EXVCPLXP,
EXVCPLYP,
EXVCPLAX,
EXVCPLAY,
EXCNFEVX,
EXCNFEVY,
EXRSLZLE,
EXRSVYHE,
EXRSMXPO,
EXRSVXPO,
EXRSEYPO,
EXRSUYPO,
EXRSLYPO,
EXRSCYPO,
EXRSVYPO,
EXRSVZP1,
EXRSVZP2,
EXRSMZPO,
EXRSEZPO,
EXRSC1YE,
EXRSC2YE,
EXRSC1XP,
EXRSC2XP,
EXRSC1ZP,
EXRSC3XP,
EXRSC3ZP,
EXRSC4Z1,
EXRSC4ZP,
EXRSMCXA,
EXRSMCYA,
EXRSMCXB,
EXRSMCYB,
EXRSMCXC,
EXRSMCYC,
EXRSMCXP,
EXRSMCZ1,
EXRSMCZP,
EXRSPOSX,
EXRSPOSY,
EXRSPOSZ,
EXRSPXAP,
EXRSPXAM,
EXRSPYAS,
GIR12ZPO,
GIR12YPO,
GIR23ZPO,
GIR34ZPO
) values (    1,
     0.0000,  1800.0000,   868.0000,   733.0000,  1234.5000,  1315.0000,
   380.0000,     0.0000,  5300.0000,  1605.0000,  2173.0000,  1958.0000,
   790.0000,  1813.0000,    60.0000,    80.0000,   980.0000,   615.0000,
   400.0000,   290.0000,    70.0000,    22.5000,  1100.0000,  2964.0000,
    70.0000,   -67.5000,  1100.0000,  1100.0000,    85.0000,  1890.0000,
   150.0000,  1140.0000,    70.0000,   790.0000,   525.0000,   120.0000,
  1190.0000,    95.0000,  1100.0000,   890.0000,  1080.0000,   100.0000,
    80.0000,    80.0000,   400.0000,    15.0000,   700.0000,   675.0000,
   455.0000,   850.0000,   500.0000,    60.0000,    80.0000,    80.0000,
  2355.0000,    35.0000,   995.0000,   335.0000,   960.0000,  2015.0000,
  1558.0000,   263.5000,    30.0000,   135.0000,   135.0000,     0.0000,
   445.0000,   375.0000,   260.0000,    40.0000,    40.0000,   330.0000,
    40.0000,   640.0000,   630.0000,   460.0000,    40.0000,    40.0000,
    50.0000,  2135.0000,    35.0000,  1025.0000,   305.0000,   740.0000,
  1795.0000,  1338.0000,   263.5000,    30.0000,   135.0000,   135.0000,
  1680.0000,    35.0000,   895.0000,   335.0000,   385.0000,  1340.0000,
   883.0000,   263.5000,    30.0000,   135.0000,   135.0000,  5420.0000,
   385.0000,  1058.0000,  2780.0000,    75.0000,  1480.0000,  1480.0000,
  1480.0000,    60.0000,  1727.0000,  1558.0000,  1635.0000,  1013.0000,
   510.0000,    93.0000,  4480.0000,   740.0000,   100.0000,  1070.0000,
   688.0000,  2834.0000,   100.0000,  1075.0000,  1360.0000,   230.0000,
  -400.0000,    80.0000,   284.0000,  2005.0000,   337.0000,   450.0000,
   135.0000,   850.0000,   500.0000,  1820.0000,    20.0000,   790.0000,
    90.0000,    20.0000,   560.0000,  1820.0000,  1205.0000,    80.0000,
   222.5000,   222.5000,   420.0000,  1090.0000,   180.0000,   217.0000,
   950.0000,   300.0000,   320.0000,   604.0000,   200.0000,   149.0000,
   860.0000,   206.0000,   300.0000,   600.0000,  2700.0000,-10980.0000,
     0.0000,  3425.0000,  6630.0000,  9380.0000, 12080.0000,  4020.0000,
  2735.5000,    50.0000,  2000.0000,    36.0000,     0.0000,    60.0000,
  1800.0000,    60.0000,  1800.0000,  1665.0000,   932.0000,  2233.0000,
   497.1293,  2667.8706,  1067.0000,  2018.0000,   400.0000,  1220.0734,
  1648.4482,  1067.0000,  1618.0000,   932.0000,  1618.0000,     1.0000,
  2170.7415,    -2.4142,  6010.5845,     0.4142,  1143.0773,  1124.6639,
  3295.4053,   565.5000,  4645.3467,  1720.9237,  1855.9072,   565.5000,
  5300.0000,  -749.5000,  5300.0000,  -749.5000,  4510.0000,  -380.0000,
  2730.4011,     0.0000,  1873.0000,    36.0704,     0.8083,     0.5888,
   149.1643,  1600.3551,  1768.0823,    68.8529,     0.3608,     0.9327,
  1482.4132,  1212.9104,  3173.8191,  1800.0000,    60.0000,   900.0000,
    30.0000,   107.5000,  2085.0000,   900.0000,  -784.5000,  4905.0000,
   980.0000,   440.0000,   303.0000,  5300.0000, -1047.0000,  4905.0000,
   779.0000,  1042.5000,  1177.5000,  1177.5000,  1042.5000,   779.0000,
    30.0000,   145.0000,   145.0000,   185.0000,   185.0000,    70.0000,
   460.0000,   375.0000,   257.5000,   260.0000,   315.0000, -1127.5000,
  -167.5000,   167.5000,  1127.5000,  2355.0000,    40.0000,   465.0000,
  1007.5000,  1007.5000,  1177.5000,  1177.5000,     0.0000,   115.0000,
   115.0000,   445.0000,   250.0000,   505.0000,  1110.0000,   330.0000,
   280.0000,  1110.0000,   295.0000,   669.0000,   932.5000,  1067.5000,
  1067.5000,   932.5000,   669.0000, -1017.5000,   -27.5000,   277.5000,
  1017.5000,  2135.0000,    40.0000,   897.5000,   897.5000,  1067.5000,
  1067.5000,     0.0000,   115.0000,   115.0000,   445.0000,  1000.0000,
  1000.0000,   441.5000,   705.0000,   840.0000,   840.0000,   705.0000,
   441.5000,  -790.0000,    70.0000,   405.0000,   790.0000,  1680.0000,
    40.0000,   670.0000,   670.0000,   840.0000,   840.0000,     0.0000,
   115.0000,   115.0000,   445.0000,   772.5000,   772.5000, -6075.0000,
   742.0000,  2840.0000,   507.6269,  3145.4409,   358.4650,  2873.5549,
   319.3403,  2286.6008,     0.7673,  2755.9253,    -2.4142,  6010.5845,
  1022.9821,  3540.8870,   207.0518,  1720.7646,  1856.2910,  1618.0000,
  1145.4794,   932.0000,  1618.0000,    37.0002,     0.7986,     0.6018,
   149.7707,  1601.1528,  1766.1564,   565.5000,  5420.0000,   -56.5000,
  5420.0000,   -56.5000,  5327.0000,  5180.0000,  -460.3792,  -559.5000,
  5180.0000,  -600.9214,  5280.0000, -1069.5000,  5280.0000, -1069.5000,
  4540.0000,  -382.0000,  -762.0090,  4540.0000, -1104.5000,  1360.0000,
  1280.0000,     0.0000,     0.0000,   -80.0000,     0.0000,   -80.0000,
  1512.5005,  -290.0896,  1974.3854,  -216.1793,  2005.0000,     0.0000,
  1512.5005,  -188.6523,  1834.0189,   100.0000,  1135.0000, -1207.0000,
  4905.0000,  1800.0000,   560.0000,   151.2500,   151.2500,    10.0000,
   730.0000,    65.0000,   390.0000,   390.0000,  -530.0000,   755.0000,
   112.5000,   175.0000,   550.0000,   950.0000,  1045.5000,  1245.5000,
   179.0000,   245.5000,   -96.0000,   750.0000,   454.0000,   430.0000,
   300.0000,   430.0000,   900.0000,   281.0000,  1049.0000,  1162.5000,
   626.0000,   156.0000,   303.0000,  5400.0000,  -172.5000,  3003.0000,
 -3003.0000, -5580.0000,  1712.5000, -6960.0000,  5027.5000,  8005.0000
);
insert into FEET_data2tag values ('FEET-01',    1);

comment on column FEET_data.FEET_data_id is 'Unique identifier';
comment on column FEET_data.MAINPLXO   is 'MAINPLATEXORIGIN                                ';
comment on column FEET_data.MNPLGXWI   is 'MAINPLATE_GROUNDXWIDTH                          ';
comment on column FEET_data.MNPLRCDX   is 'MAINPLATEROUNDCUTDX                             ';
comment on column FEET_data.MNPLDXEX   is 'MAINPLATEDXEXTR                                 ';
comment on column FEET_data.MNPLUPDX   is 'MAINPLATEUPPERDX                                ';
comment on column FEET_data.MNPLUPXW   is 'MAINPLATEUPPERXWIDTH                            ';
comment on column FEET_data.MNPLMIDX   is 'MAINPLATEMIDDLEDX                               ';
comment on column FEET_data.MAINPLYO   is 'MAINPLATEYORIGIN                                ';
comment on column FEET_data.MAINPLHE   is 'MAINPLATEHEIGHT                                 ';
comment on column FEET_data.MAINPLH1   is 'MAINPLATEHEIGHT_INTERMEDIATE1                   ';
comment on column FEET_data.MNPLRCYP   is 'MAINPLATEROUNDCUT_Y                             ';
comment on column FEET_data.MNPLSRCY   is 'MAINPLATESECONDARYROUNDCUT_Y                    ';
comment on column FEET_data.MNPLUPDY   is 'MAINPLATEUPPERDY                                ';
comment on column FEET_data.MNPLMIDY   is 'MAINPLATEMIDDLEDY                               ';
comment on column FEET_data.MNPLPEHE   is 'MAINPLATE_PEDESTALHEIGHT                        ';
comment on column FEET_data.MAINPLDZ   is 'MAINPLATEDZ                                     ';
comment on column FEET_data.MNPLZSEP   is 'MAINPLATEZSEP                                   ';
comment on column FEET_data.MNPLRCRA   is 'MAINPLATEROUNDCUT_R                             ';
comment on column FEET_data.MNPLSRCR   is 'MAINPLATESECONDARYROUNDCUT_R                    ';
comment on column FEET_data.MINCPLXW   is 'MINICONNPLATE_XWIDTH                            ';
comment on column FEET_data.MINCPLYH   is 'MINICONNPLATE_YHEIGHT                           ';
comment on column FEET_data.MINCPLTZ   is 'MINICONNPLATE_THETAZ                            ';
comment on column FEET_data.MINCPLZL   is 'MINICONNPLATE_ZLENGTH                           ';
comment on column FEET_data.SLACPLXW   is 'SLANTEDCONNPLATE_XWIDTH                         ';
comment on column FEET_data.SLACPLYH   is 'SLANTEDCONNPLATE_YHEIGHT                        ';
comment on column FEET_data.SLACPLTZ   is 'SLANTEDCONNPLATE_THETAZ                         ';
comment on column FEET_data.SLACPLZL   is 'SLANTEDCONNPLATE_ZLENGTH                        ';
comment on column FEET_data.GRNDPLZL   is 'GROUNDPLATE_ZLENGTH                             ';
comment on column FEET_data.VCNPLXWI   is 'VERTICALCONNPLATE_XWIDTH                        ';
comment on column FEET_data.VCNPLYHE   is 'VERTICALCONNPLATE_YHEIGHT                       ';
comment on column FEET_data.VCNPLDXE   is 'VERTICALCONNPLATE_DX                            ';
comment on column FEET_data.VCNPLDYE   is 'VERTICALCONNPLATE_DY                            ';
comment on column FEET_data.UCNPLXWI   is 'UPPERCONNPLATE_XWIDTH                           ';
comment on column FEET_data.UCNPLYHE   is 'UPPERCONNPLATE_YHEIGHT                          ';
comment on column FEET_data.XWIDTH     is 'XWIDTH                                          ';
comment on column FEET_data.UPPERHEY   is 'RAILSUPPORTUPPER_YHEIGHT                        ';
comment on column FEET_data.UPPERLEZ   is 'RAILSUPPORTUPPER_ZLENGTH                        ';
comment on column FEET_data.LOWERHEY   is 'RAILSUPPORTLOWER_YHEIGHT                        ';
comment on column FEET_data.LOWERLEZ   is 'RAILSUPPORTLOWER_ZLENGTH                        ';
comment on column FEET_data.TOTALHEY   is 'RAILSUPPORTTOTAL_YHEIGHT                        ';
comment on column FEET_data.CENTRLEZ   is 'RAILSUPPORTCENTRAL_ZLENGTH                      ';
comment on column FEET_data.CENTRXTH   is 'RAILSUPPORTCENTRAL_XTHICK                       ';
comment on column FEET_data.MIDLLYTH   is 'RAILSUPPORTMIDDLE_YTHICK                        ';
comment on column FEET_data.VERTIZTH   is 'RAILSUPPORTVERTICAL_ZTHICK                      ';
comment on column FEET_data.MIDLLYPO   is 'RAILSUPPORTMIDDLE_YPOS                          ';
comment on column FEET_data.EXTREHEY   is 'RAILSUPPORTEXTR_YHEIGHT                         ';
comment on column FEET_data.EXTRELEZ   is 'RAILSUPPORTEXTR_ZLENGTH                         ';
comment on column FEET_data.CENTRHEY   is 'RAILSUPPORTCENTRAL_YHEIGHT                      ';
comment on column FEET_data.CNFEVOXW   is 'CONNFEETVOUSS_XWIDTH                            ';
comment on column FEET_data.CNFEVOYH   is 'CONNFEETVOUSS_YHEIGHT                           ';
comment on column FEET_data.CNFEVOZL   is 'CONNFEETVOUSS_ZLENGTH                           ';
comment on column FEET_data.CNFEVOXT   is 'CONNFEETVOUSS_XTHICK                            ';
comment on column FEET_data.CNFEVOYT   is 'CONNFEETVOUSS_YTHICK                            ';
comment on column FEET_data.CNFEVOZT   is 'CONNFEETVOUSS_ZTHICK                            ';
comment on column FEET_data.G12LPXWT   is 'GIRDER12_LATERALPLATE_XWIDTH_TOTAL              ';
comment on column FEET_data.G12IPSXO   is 'GIRDER12_INNERPLATESMALLOFFSET_XDIST            ';
comment on column FEET_data.G12IPSLX   is 'GIRDER12_INNERPLATESMALL2LARGE_XDIST            ';
comment on column FEET_data.G12IPLLX   is 'GIRDER12_INNERPLATELARGE2LARGE_XDIST            ';
comment on column FEET_data.G12IPLSX   is 'GIRDER12_INNERPLATELARGE2SMALL_XDIST            ';
comment on column FEET_data.G12RPXWI   is 'GIRDER12_REINFORCEMENTPLATE_XWIDTH              ';
comment on column FEET_data.G12LPXW1   is 'GIRDER12_LATERALPLATE_XWIDTH_INTER1             ';
comment on column FEET_data.G12LPX12   is 'GIRDER12_LATERALPLATE_XWIDTH_INTER1INTER        ';
comment on column FEET_data.G12IPXWI   is 'GIRDER12_INNERPLATE_XWIDTH                      ';
comment on column FEET_data.G12UMPWI   is 'GIRDER12_UPPERMINIPLATE_XWIDTH                  ';
comment on column FEET_data.G12SMPWI   is 'GIRDER12_SIDEMINIPLATE_XWIDTH                   ';
comment on column FEET_data.G12YORIG   is 'GIRDER12_YORIGIN                                ';
comment on column FEET_data.G12LPYHT   is 'GIRDER12_LATERALPLATE_YHEIGHT_TOTAL             ';
comment on column FEET_data.G12LPYH1   is 'GIRDER12_LATERALPLATE_YHEIGHT_INTER1            ';
comment on column FEET_data.G12LPYH2   is 'GIRDER12_LATERALPLATE_YHEIGHT_INTER2            ';
comment on column FEET_data.G12LPYTH   is 'GIRDER12_LOWERPLATE_YTHICK                      ';
comment on column FEET_data.G12UPYTH   is 'GIRDER12_UPPERPLATE_YTHICK                      ';
comment on column FEET_data.G12RPYDI   is 'GIRDER12_REINFORCEMENTPLATE_YDIST               ';
comment on column FEET_data.G12UMPYH   is 'GIRDER12_UPPERMINIPLATE_YHEIGHT                 ';
comment on column FEET_data.G12UPZLE   is 'GIRDER12_UPPERPLATE_ZLENGTH                     ';
comment on column FEET_data.G12UMPZL   is 'GIRDER12_UPPERMINIPLATE_ZLENGTH                 ';
comment on column FEET_data.G12BPZLE   is 'GIRDER12_BOTTOMPLATE_ZLENGTH                    ';
comment on column FEET_data.G12LPZLE   is 'GIRDER12_LATERALPLATE_ZLENGTH                   ';
comment on column FEET_data.G12RPZLE   is 'GIRDER12_REINFORCEMENTPLATE_ZLENGTH             ';
comment on column FEET_data.G12SMPZL   is 'GIRDER12_SIDEMINIPLATE_ZLENGTH                  ';
comment on column FEET_data.G23LPXWT   is 'GIRDER23_LATERALPLATE_XWIDTH_TOTAL              ';
comment on column FEET_data.G23IPSXO   is 'GIRDER23_INNERPLATESMALLOFFSET_XDIST            ';
comment on column FEET_data.G23IPSLX   is 'GIRDER23_INNERPLATESMALL2LARGE_XDIST            ';
comment on column FEET_data.G23IPLLX   is 'GIRDER23_INNERPLATELARGE2LARGE_XDIST            ';
comment on column FEET_data.G23IPLSX   is 'GIRDER23_INNERPLATELARGE2SMALL_XDIST            ';
comment on column FEET_data.G23RPXWI   is 'GIRDER23_REINFORCEMENTPLATE_XWIDTH              ';
comment on column FEET_data.G23LPXW1   is 'GIRDER23_LATERALPLATE_XWIDTH_INTER1             ';
comment on column FEET_data.G23LPX12   is 'GIRDER23_LATERALPLATE_XWIDTH_INTER1INTER        ';
comment on column FEET_data.G23IPXWI   is 'GIRDER23_INNERPLATE_XWIDTH                      ';
comment on column FEET_data.G23UMPWI   is 'GIRDER23_UPPERMINIPLATE_XWIDTH                  ';
comment on column FEET_data.G23SMPWI   is 'GIRDER23_SIDEMINIPLATE_XWIDTH                   ';
comment on column FEET_data.G34LPXWT   is 'GIRDER34_LATERALPLATE_XWIDTH_TOTAL              ';
comment on column FEET_data.G34IPSXO   is 'GIRDER34_INNERPLATESMALLOFFSET_XDIST            ';
comment on column FEET_data.G34IPSLX   is 'GIRDER34_INNERPLATESMALL2LARGE_XDIST            ';
comment on column FEET_data.G34IPLLX   is 'GIRDER34_INNERPLATELARGE2LARGE_XDIST            ';
comment on column FEET_data.G34IPLSX   is 'GIRDER34_INNERPLATELARGE2SMALL_XDIST            ';
comment on column FEET_data.G34RPXWI   is 'GIRDER34_REINFORCEMENTPLATE_XWIDTH              ';
comment on column FEET_data.G34LPXW1   is 'GIRDER34_LATERALPLATE_XWIDTH_INTER1             ';
comment on column FEET_data.G34LPX12   is 'GIRDER34_LATERALPLATE_XWIDTH_INTER1INTER        ';
comment on column FEET_data.G34IPXWI   is 'GIRDER34_INNERPLATE_XWIDTH                      ';
comment on column FEET_data.G34UMPWI   is 'GIRDER34_UPPERMINIPLATE_XWIDTH                  ';
comment on column FEET_data.G34SMPWI   is 'GIRDER34_SIDEMINIPLATE_XWIDTH                   ';
comment on column FEET_data.EXMPHEIG   is 'EXTREMITYMAINPLATE_HEIGHT                       ';
comment on column FEET_data.EXMPRCRA   is 'EXTREMITYMAINPLATEROUNDCUT_R                    ';
comment on column FEET_data.EXMPRCDX   is 'EXTREMITYMAINPLATEROUNDCUT_DX                   ';
comment on column FEET_data.EXMPRCYP   is 'EXTREMITYMAINPLATEROUNDCUT_Y                    ';
comment on column FEET_data.EXMCPYHE   is 'EXTREMITYMINICONNPLATE_YHEIGHT                  ';
comment on column FEET_data.EXMCPZLE   is 'EXTREMITYMINICONNPLATE_ZLENGTH                  ';
comment on column FEET_data.EXGPZLEN   is 'EXTREMITYGROUNDPLATE_ZLENGTH                    ';
comment on column FEET_data.EXSCPZLE   is 'EXTREMITYSLANTEDCONNPLATE_ZLENGTH               ';
comment on column FEET_data.EXRSZOFF   is 'EXTREMITYRAILSUPPORT_ZOFFSET                    ';
comment on column FEET_data.EXMPHEI1   is 'EXTREMITYMAINPLATE_HEIGHT_INTERMEDIATE1         ';
comment on column FEET_data.EXMPHEI2   is 'EXTREMITYMAINPLATE_HEIGHT_INTERMEDIATE2         ';
comment on column FEET_data.EXMPXWID   is 'EXTREMITYMAINPLATE_XWIDTH                       ';
comment on column FEET_data.EXMPXWI1   is 'EXTREMITYMAINPLATE_XWIDTH_INTER1                ';
comment on column FEET_data.EXMPXWI2   is 'EXTREMITYMAINPLATE_XWIDTH_INTER2                ';
comment on column FEET_data.EXMPYHI1   is 'EXTREMITYMAINPLATE_YHEIGHT_INTER1               ';
comment on column FEET_data.EXMPYHI2   is 'EXTREMITYMAINPLATE_YHEIGHT_INTER2               ';
comment on column FEET_data.EXMPYHI3   is 'EXTREMITYMAINPLATE_YHEIGHT_INTER3               ';
comment on column FEET_data.EXMPYHI4   is 'EXTREMITYMAINPLATE_YHEIGHT_INTER4               ';
comment on column FEET_data.EXMPXDI1   is 'EXTREMITYMAINPLATE_XDIST_INTER1                 ';
comment on column FEET_data.EXMPXDI2   is 'EXTREMITYMAINPLATE_XDIST_INTER2                 ';
comment on column FEET_data.EXMPYDI2   is 'EXTREMITYMAINPLATE_YDIST_INTER1                 ';
comment on column FEET_data.EXVCPLDX   is 'EXTREMITYVERTICALCONNPLATE_DX                   ';
comment on column FEET_data.EXVCPLDY   is 'EXTREMITYVERTICALCONNPLATE_DY                   ';
comment on column FEET_data.EXMPZSEP   is 'EXTREMITYMAINPLATE_ZSEP                         ';
comment on column FEET_data.EXSPRCRA   is 'EXTREMITYSLANTEDCONNPLATEROUNDCUT_RADIUS        ';
comment on column FEET_data.EXSPRCOF   is 'EXTREMITYSLANTEDCONNPLATEROUNDCUT_OFFSET        ';
comment on column FEET_data.EXVCPTHI   is 'EXTREMITYVERTICALCONNPLATE_THICKNESS            ';
comment on column FEET_data.EXVCPXEX   is 'EXTREMITYVERTICALCONNPLATE_XEXT                 ';
comment on column FEET_data.EXVCPHEI   is 'EXTREMITYVERTICALCONNPLATE_HEIGHT               ';
comment on column FEET_data.EXVCPYCU   is 'EXTREMITYVERTICALCONNPLATE_CUTY                 ';
comment on column FEET_data.EXVCPXCU   is 'EXTREMITYVERTICALCONNPLATE_CUTX                 ';
comment on column FEET_data.EXCFVXWI   is 'EXTREMITYCONNFEETVOUSS_XWIDTH                   ';
comment on column FEET_data.EXCFVYHE   is 'EXTREMITYCONNFEETVOUSS_YHEIGHT                  ';
comment on column FEET_data.EXCFVZLE   is 'EXTREMITYCONNFEETVOUSS_ZLENGTH                  ';
comment on column FEET_data.EXRSUZLE   is 'EXTREMITYRAILSUPPORTUPPER_ZLENGTH               ';
comment on column FEET_data.EXRSLDZL   is 'EXTREMITYRAILSUPPORTLOWER_DZLENGTH              ';
comment on column FEET_data.EXRSTYHE   is 'EXTREMITYRAILSUPPORTTOTAL_YHEIGHT               ';
comment on column FEET_data.EXRSLYHE   is 'EXTREMITYRAILSUPPORTLOWER_YHEIGHT               ';
comment on column FEET_data.EXRSEYHE   is 'EXTREMITYRAILSUPPORTEXTR_YHEIGHT                ';
comment on column FEET_data.EXRSCYHE   is 'EXTREMITYRAILSUPPORTCENTRAL_YHEIGHT             ';
comment on column FEET_data.EXRSCZLE   is 'EXTREMITYRAILSUPPORTCENTRAL_ZLENGTH             ';
comment on column FEET_data.EXRSMZLE   is 'EXTREMITYRAILSUPPORTMIDDLE_ZLENGTH              ';
comment on column FEET_data.EXRSCXWI   is 'EXTREMITYRAILSUPPORTCENTRAL_XWIDTH              ';
comment on column FEET_data.EXRSMXWI   is 'EXTREMITYRAILSUPPORTMIDDLE_XWIDTH               ';
comment on column FEET_data.EXRSVXWI   is 'EXTREMITYRAILSUPPORTVERTICAL_XWIDTH             ';
comment on column FEET_data.EXRSVZI1   is 'EXTREMITYRAILSUPPORTVERTICAL_ZPOS_INTER1        ';
comment on column FEET_data.EXRSEZLE   is 'EXTREMITYRAILSUPPORTEXTR_ZLENGTH                ';
comment on column FEET_data.EXRSEZOF   is 'EXTREMITYRAILSUPPORTEXTR_ZOFFSET                ';
comment on column FEET_data.EXRSC1DE   is 'EXTREMITYRAILSUPPORTCUT1_DEPTH                  ';
comment on column FEET_data.EXRSC1WI   is 'EXTREMITYRAILSUPPORTCUT1_WIDTH                  ';
comment on column FEET_data.EXRSC1XE   is 'EXTREMITYRAILSUPPORTCUT1_DUMXEXT                ';
comment on column FEET_data.EXRSC2XE   is 'EXTREMITYRAILSUPPORTCUT2_DUMXEXT                ';
comment on column FEET_data.EXRSC2Z1   is 'EXTREMITYRAILSUPPORTCUT1_ZOFFSET1               ';
comment on column FEET_data.EXRSC2Z2   is 'EXTREMITYRAILSUPPORTCUT1_ZOFFSET2               ';
comment on column FEET_data.EXRSMCDE   is 'EXTREMITYRAILSUPPORTMIDDLECUT_DEPTH             ';
comment on column FEET_data.EXRSMCWI   is 'EXTREMITYRAILSUPPORTMIDDLECUT_WIDTH             ';
comment on column FEET_data.EXRSMCZO   is 'EXTREMITYRAILSUPPORTMIDDLECUT_ZOFFSET           ';
comment on column FEET_data.EXRSMCY1   is 'EXTREMITYRAILSUPPORTMIDDLECUT_DUMY1             ';
comment on column FEET_data.EXRSMCY2   is 'EXTREMITYRAILSUPPORTMIDDLECUT_DUMY2             ';
comment on column FEET_data.STDFOOXP   is 'STANDARDFOOT_XPOS                               ';
comment on column FEET_data.STDFOOYP   is 'STANDARDFOOT_YPOS                               ';
comment on column FEET_data.ZPOSFEE1   is 'ZPOSFEE1                                        ';
comment on column FEET_data.ZPOSFEE2   is 'ZPOSFEE2                                        ';
comment on column FEET_data.ZPOSFEE3   is 'ZPOSFEE3                                        ';
comment on column FEET_data.ZPOSFEE4   is 'ZPOSFEE4                                        ';
comment on column FEET_data.ZPOSFEE5   is 'ZPOSFEE5                                        ';
comment on column FEET_data.GIRDYHEI   is 'YHEIGHTGIRDER                                   ';
comment on column FEET_data.GIRDXPOS   is 'XPOSGIRDER                                      ';
comment on column FEET_data.MPLAHCEC   is 'FEETMAINPLATE_HCEC                              ';
comment on column FEET_data.DUMZ       is 'DUMZ                                            ';
comment on column FEET_data.DUMPHI     is 'DUMPHI                                          ';
comment on column FEET_data.MAINPLXA   is 'MAINPLXA                                        ';
comment on column FEET_data.MAINPLYA   is 'MAINPLYA                                        ';
comment on column FEET_data.MAINPLXB   is 'MAINPLXB                                        ';
comment on column FEET_data.MAINPLYB   is 'MAINPLYB                                        ';
comment on column FEET_data.MAINPLXC   is 'MAINPLXC                                        ';
comment on column FEET_data.MAINPLYC   is 'MAINPLYC                                        ';
comment on column FEET_data.MAPLRCXC   is 'MAINPLATEROUNDCUT_XC                            ';
comment on column FEET_data.MAPLRCYC   is 'MAINPLATEROUNDCUT_YC                            ';
comment on column FEET_data.MAIPLXC3   is 'MAIPLXC3                                        ';
comment on column FEET_data.MAIPLYC3   is 'MAIPLYC3                                        ';
comment on column FEET_data.MPLXEXTR   is 'FEETMAINPLATE_XEXTR                             ';
comment on column FEET_data.MPLYEXTR   is 'FEETMAINPLATE_YEXTR                             ';
comment on column FEET_data.MPLREXTR   is 'FEETMAINPLATE_REXTR                             ';
comment on column FEET_data.MAIPLXC2   is 'MAIPLXC2                                        ';
comment on column FEET_data.MAIPLYC2   is 'MAIPLYC2                                        ';
comment on column FEET_data.MPLXCEC1   is 'MPLXCEC1                                        ';
comment on column FEET_data.MPLYCEC1   is 'MPLYCEC1                                        ';
comment on column FEET_data.MPLXCEC2   is 'MPLXCEC2                                        ';
comment on column FEET_data.MPLYCEC2   is 'MPLYCEC2                                        ';
comment on column FEET_data.A          is 'A                                               ';
comment on column FEET_data.B          is 'B                                               ';
comment on column FEET_data.APRIME     is 'APRIME                                          ';
comment on column FEET_data.BPRIME     is 'BPRIME                                          ';
comment on column FEET_data.ASECONDE   is 'ASECONDE                                        ';
comment on column FEET_data.BSECONDE   is 'BSECONDE                                        ';
comment on column FEET_data.MAINPLXD   is 'MAINPLXD                                        ';
comment on column FEET_data.MAINPLYD   is 'MAINPLYD                                        ';
comment on column FEET_data.MAINPLXE   is 'MAINPLXE                                        ';
comment on column FEET_data.MAINPLYE   is 'MAINPLYE                                        ';
comment on column FEET_data.MAIPLXC1   is 'MAIPLXC1                                        ';
comment on column FEET_data.MAIPLYC1   is 'MAIPLYC1                                        ';
comment on column FEET_data.MAINPLXF   is 'MAINPLXF                                        ';
comment on column FEET_data.MAINPLYF   is 'MAINPLYF                                        ';
comment on column FEET_data.MAINPLXG   is 'MAINPLXG                                        ';
comment on column FEET_data.MAINPLYG   is 'MAINPLYG                                        ';
comment on column FEET_data.MAINPLXH   is 'MAINPLXH                                        ';
comment on column FEET_data.MAINPLYH   is 'MAINPLYH                                        ';
comment on column FEET_data.MAINPLXI   is 'MAINPLXI                                        ';
comment on column FEET_data.MAINPLYI   is 'MAINPLYI                                        ';
comment on column FEET_data.MAINPLXJ   is 'MAINPLXJ                                        ';
comment on column FEET_data.MAINPLYJ   is 'MAINPLYJ                                        ';
comment on column FEET_data.ALPHA1     is 'ALPHA1                                          ';
comment on column FEET_data.COAL1      is 'COAL1                                           ';
comment on column FEET_data.SIAL1      is 'SIAL1                                           ';
comment on column FEET_data.MICPDIAG   is 'MINICONNPLATE_DIAG                              ';
comment on column FEET_data.MICPXPOS   is 'MINICONNPLATE_X                                 ';
comment on column FEET_data.MICPYPOS   is 'MINICONNPLATE_Y                                 ';
comment on column FEET_data.ALPHA2     is 'ALPHA2                                          ';
comment on column FEET_data.COAL2      is 'COAL2                                           ';
comment on column FEET_data.SIAL2      is 'SIAL2                                           ';
comment on column FEET_data.SLCPDIAG   is 'SLANTEDCONNPLATE_DIAG                           ';
comment on column FEET_data.SLCPXPOS   is 'SLANTEDCONNPLATE_X                              ';
comment on column FEET_data.SLCPYPOS   is 'SLANTEDCONNPLATE_Y                              ';
comment on column FEET_data.GRNPXWID   is 'GROUNDPLATE_XWIDTH                              ';
comment on column FEET_data.GRNPYHEI   is 'GROUNDPLATE_YHEIGHT                             ';
comment on column FEET_data.GRNPXPOS   is 'GROUNDPLATE_X                                   ';
comment on column FEET_data.GRNPYPOS   is 'GROUNDPLATE_Y                                   ';
comment on column FEET_data.VECPXPOS   is 'VERTICALCONNPLATE_X                             ';
comment on column FEET_data.VECPYPOS   is 'VERTICALCONNPLATE_Y                             ';
comment on column FEET_data.VECPZLEN   is 'VERTICALCONNPLATE_ZLENGTH                       ';
comment on column FEET_data.UPCPXPOS   is 'UPPERCONNPLATE_X                                ';
comment on column FEET_data.UPCPYPOS   is 'UPPERCONNPLATE_Y                                ';
comment on column FEET_data.UPCPZLEN   is 'UPPERCONNPLATE_ZLENGTH                          ';
comment on column FEET_data.MIDYPOSI   is 'RAILSUPPORTMIDDLE_Y                             ';
comment on column FEET_data.POSX       is 'POSX                                            ';
comment on column FEET_data.POSY       is 'POSY                                            ';
comment on column FEET_data.CNFEVOXP   is 'CONNFEETVOUSS_X                                 ';
comment on column FEET_data.CNFEVOYP   is 'CONNFEETVOUSS_Y                                 ';
comment on column FEET_data.G12BOPXA   is 'GIRDER12_BOTTOMPLATE_XA                         ';
comment on column FEET_data.G12BOPXB   is 'GIRDER12_BOTTOMPLATE_XB                         ';
comment on column FEET_data.G12BOPXC   is 'GIRDER12_BOTTOMPLATE_XC                         ';
comment on column FEET_data.G12BOPXD   is 'GIRDER12_BOTTOMPLATE_XD                         ';
comment on column FEET_data.G12BOPXE   is 'GIRDER12_BOTTOMPLATE_XE                         ';
comment on column FEET_data.G12BOPXF   is 'GIRDER12_BOTTOMPLATE_XF                         ';
comment on column FEET_data.G12BOPYA   is 'GIRDER12_BOTTOMPLATE_YA                         ';
comment on column FEET_data.G12BOPYB   is 'GIRDER12_BOTTOMPLATE_YB                         ';
comment on column FEET_data.G12BOPYC   is 'GIRDER12_BOTTOMPLATE_YC                         ';
comment on column FEET_data.G12BOPYD   is 'GIRDER12_BOTTOMPLATE_YD                         ';
comment on column FEET_data.G12BOPYE   is 'GIRDER12_BOTTOMPLATE_YE                         ';
comment on column FEET_data.G12BOPYF   is 'GIRDER12_BOTTOMPLATE_YF                         ';
comment on column FEET_data.G12IPZLE   is 'GIRDER12_INNERPLATE_ZLENGTH                     ';
comment on column FEET_data.G12IPLYH   is 'GIRDER12_INNERPLATELARGE_YHEIGHT                ';
comment on column FEET_data.G12IPLYP   is 'GIRDER12_INNERPLATELARGE_YPOS                   ';
comment on column FEET_data.G12IPSYH   is 'GIRDER12_INNERPLATESMALL_YHEIGHT                ';
comment on column FEET_data.G12IPSYP   is 'GIRDER12_INNERPLATESMALL_YPOS                   ';
comment on column FEET_data.G12IPSX1   is 'GIRDER12_INNERPLATESMALL1_XPOS                  ';
comment on column FEET_data.G12IPLX1   is 'GIRDER12_INNERPLATELARGE1_XPOS                  ';
comment on column FEET_data.G12IPLX2   is 'GIRDER12_INNERPLATELARGE2_XPOS                  ';
comment on column FEET_data.G12IPSX2   is 'GIRDER12_INNERPLATESMALL2_XPOS                  ';
comment on column FEET_data.G12UPXWI   is 'GIRDER12_UPPERPLATE_XWIDTH                      ';
comment on column FEET_data.G12UPYHE   is 'GIRDER12_UPPERPLATE_YHEIGHT                     ';
comment on column FEET_data.G12UPYPO   is 'GIRDER12_UPPERPLATE_YPOS                        ';
comment on column FEET_data.G12REPXA   is 'GIRDER12_REINFORCEMENTPLATE_XA                  ';
comment on column FEET_data.G12REPXB   is 'GIRDER12_REINFORCEMENTPLATE_XB                  ';
comment on column FEET_data.G12REPXC   is 'GIRDER12_REINFORCEMENTPLATE_XC                  ';
comment on column FEET_data.G12REPXD   is 'GIRDER12_REINFORCEMENTPLATE_XD                  ';
comment on column FEET_data.G12REPYA   is 'GIRDER12_REINFORCEMENTPLATE_YA                  ';
comment on column FEET_data.G12REPYB   is 'GIRDER12_REINFORCEMENTPLATE_YB                  ';
comment on column FEET_data.G12REPYC   is 'GIRDER12_REINFORCEMENTPLATE_YC                  ';
comment on column FEET_data.G12REPYD   is 'GIRDER12_REINFORCEMENTPLATE_YD                  ';
comment on column FEET_data.G12REPZP   is 'GIRDER12_REINFORCEMENTPLATE_ZPOS                ';
comment on column FEET_data.G12UMPYP   is 'GIRDER12_UPPERMINIPLATE_YPOS                    ';
comment on column FEET_data.G12UMPXP   is 'GIRDER12_UPPERMINIPLATE_XPOS                    ';
comment on column FEET_data.G12SMPYH   is 'GIRDER12_SIDEMINIPLATE_YHEIGHT                  ';
comment on column FEET_data.G12SMPYP   is 'GIRDER12_SIDEMINIPLATE_YPOS                     ';
comment on column FEET_data.G12SMPXP   is 'GIRDER12_SIDEMINIPLATE_XPOS                     ';
comment on column FEET_data.G12SMPZP   is 'GIRDER12_SIDEMINIPLATE_ZPOS                     ';
comment on column FEET_data.G23BOPXA   is 'GIRDER23_BOTTOMPLATE_XA                         ';
comment on column FEET_data.G23BOPXB   is 'GIRDER23_BOTTOMPLATE_XB                         ';
comment on column FEET_data.G23BOPXC   is 'GIRDER23_BOTTOMPLATE_XC                         ';
comment on column FEET_data.G23BOPXD   is 'GIRDER23_BOTTOMPLATE_XD                         ';
comment on column FEET_data.G23BOPXE   is 'GIRDER23_BOTTOMPLATE_XE                         ';
comment on column FEET_data.G23BOPXF   is 'GIRDER23_BOTTOMPLATE_XF                         ';
comment on column FEET_data.G23IPSX1   is 'GIRDER23_INNERPLATESMALL1_XPOS                  ';
comment on column FEET_data.G23IPLX1   is 'GIRDER23_INNERPLATELARGE1_XPOS                  ';
comment on column FEET_data.G23IPLX2   is 'GIRDER23_INNERPLATELARGE2_XPOS                  ';
comment on column FEET_data.G23IPSX2   is 'GIRDER23_INNERPLATESMALL2_XPOS                  ';
comment on column FEET_data.G23UPXWI   is 'GIRDER23_UPPERPLATE_XWIDTH                      ';
comment on column FEET_data.G23UPYHE   is 'GIRDER23_UPPERPLATE_YHEIGHT                     ';
comment on column FEET_data.G23REPXA   is 'GIRDER23_REINFORCEMENTPLATE_XA                  ';
comment on column FEET_data.G23REPXB   is 'GIRDER23_REINFORCEMENTPLATE_XB                  ';
comment on column FEET_data.G23REPXC   is 'GIRDER23_REINFORCEMENTPLATE_XC                  ';
comment on column FEET_data.G23REPXD   is 'GIRDER23_REINFORCEMENTPLATE_XD                  ';
comment on column FEET_data.G23REPYA   is 'GIRDER23_REINFORCEMENTPLATE_YA                  ';
comment on column FEET_data.G23REPYB   is 'GIRDER23_REINFORCEMENTPLATE_YB                  ';
comment on column FEET_data.G23REPYC   is 'GIRDER23_REINFORCEMENTPLATE_YC                  ';
comment on column FEET_data.G23REPYD   is 'GIRDER23_REINFORCEMENTPLATE_YD                  ';
comment on column FEET_data.G23UMPXP   is 'GIRDER23_UPPERMINIPLATE_XPOS                    ';
comment on column FEET_data.G23SMPXP   is 'GIRDER23_SIDEMINIPLATE_XPOS                     ';
comment on column FEET_data.G34BOPXA   is 'GIRDER34_BOTTOMPLATE_XA                         ';
comment on column FEET_data.G34BOPXB   is 'GIRDER34_BOTTOMPLATE_XB                         ';
comment on column FEET_data.G34BOPXC   is 'GIRDER34_BOTTOMPLATE_XC                         ';
comment on column FEET_data.G34BOPXD   is 'GIRDER34_BOTTOMPLATE_XD                         ';
comment on column FEET_data.G34BOPXE   is 'GIRDER34_BOTTOMPLATE_XE                         ';
comment on column FEET_data.G34BOPXF   is 'GIRDER34_BOTTOMPLATE_XF                         ';
comment on column FEET_data.G34IPSX1   is 'GIRDER34_INNERPLATESMALL1_XPOS                  ';
comment on column FEET_data.G34IPLX1   is 'GIRDER34_INNERPLATELARGE1_XPOS                  ';
comment on column FEET_data.G34IPLX2   is 'GIRDER34_INNERPLATELARGE2_XPOS                  ';
comment on column FEET_data.G34IPSX2   is 'GIRDER34_INNERPLATESMALL2_XPOS                  ';
comment on column FEET_data.G34UPXWI   is 'GIRDER34_UPPERPLATE_XWIDTH                      ';
comment on column FEET_data.G34UPYHE   is 'GIRDER34_UPPERPLATE_YHEIGHT                     ';
comment on column FEET_data.G34REPXA   is 'GIRDER34_REINFORCEMENTPLATE_XA                  ';
comment on column FEET_data.G34REPXB   is 'GIRDER34_REINFORCEMENTPLATE_XB                  ';
comment on column FEET_data.G34REPXC   is 'GIRDER34_REINFORCEMENTPLATE_XC                  ';
comment on column FEET_data.G34REPXD   is 'GIRDER34_REINFORCEMENTPLATE_XD                  ';
comment on column FEET_data.G34REPYA   is 'GIRDER34_REINFORCEMENTPLATE_YA                  ';
comment on column FEET_data.G34REPYB   is 'GIRDER34_REINFORCEMENTPLATE_YB                  ';
comment on column FEET_data.G34REPYC   is 'GIRDER34_REINFORCEMENTPLATE_YC                  ';
comment on column FEET_data.G34REPYD   is 'GIRDER34_REINFORCEMENTPLATE_YD                  ';
comment on column FEET_data.G34UMPXP   is 'GIRDER34_UPPERMINIPLATE_XPOS                    ';
comment on column FEET_data.G34SMPXP   is 'GIRDER34_SIDEMINIPLATE_XPOS                     ';
comment on column FEET_data.STDFVOYP   is 'STANDARDFOOTVOUSSOIR_YPOS                       ';
comment on column FEET_data.EXMPRCXC   is 'EXTREMITYMAINPLATEROUNDCUT_XC                   ';
comment on column FEET_data.EXMPRCYC   is 'EXTREMITYMAINPLATEROUNDCUT_YC                   ';
comment on column FEET_data.EXMPLXC3   is 'EXTREMITYMAINPLATE_XC3                          ';
comment on column FEET_data.EXMPLYC3   is 'EXTREMITYMAINPLATE_YC3                          ';
comment on column FEET_data.EXMPLXC4   is 'EXTREMITYMAINPLATE_XC4                          ';
comment on column FEET_data.EXMPLYC4   is 'EXTREMITYMAINPLATE_YC4                          ';
comment on column FEET_data.EXMPLXC5   is 'EXTREMITYMAINPLATE_XC5                          ';
comment on column FEET_data.EXMPLYC5   is 'EXTREMITYMAINPLATE_YC5                          ';
comment on column FEET_data.A2         is 'A2                                              ';
comment on column FEET_data.B2         is 'B2                                              ';
comment on column FEET_data.A2PRIME    is 'A2PRIME                                         ';
comment on column FEET_data.B2PRIME    is 'B2PRIME                                         ';
comment on column FEET_data.EXMPLAXD   is 'EXTREMITYMAINPLATE_XD                           ';
comment on column FEET_data.EXMPLAYD   is 'EXTREMITYMAINPLATE_YD                           ';
comment on column FEET_data.EXMPC2C1   is 'EXTREMITYMAINPLATE_LENGTHC2C1                   ';
comment on column FEET_data.EXMPLXC1   is 'EXTREMITYMAINPLATE_XC1                          ';
comment on column FEET_data.EXMPLYC1   is 'EXTREMITYMAINPLATE_YC1                          ';
comment on column FEET_data.EXMPYC1A   is 'EXTREMITYMAINPLATE_YC1A                         ';
comment on column FEET_data.EXMPXC1A   is 'EXTREMITYMAINPLATE_XC1A                         ';
comment on column FEET_data.EXMPXC1B   is 'EXTREMITYMAINPLATE_XC1B                         ';
comment on column FEET_data.EXMPYC1B   is 'EXTREMITYMAINPLATE_YC1B                         ';
comment on column FEET_data.ALPHA3     is 'ALPHA3                                          ';
comment on column FEET_data.COAL3      is 'COAL3                                           ';
comment on column FEET_data.SIAL3      is 'SIAL3                                           ';
comment on column FEET_data.EXMCPDIA   is 'EXTREMITYMINICONNPLATE_DIAG                     ';
comment on column FEET_data.EXMCPXPO   is 'EXTREMITYMINICONNPLATE_X                        ';
comment on column FEET_data.EXMCPYPO   is 'EXTREMITYMINICONNPLATE_Y                        ';
comment on column FEET_data.EXMPLAXF   is 'EXTREMITYMAINPLATE_XF                           ';
comment on column FEET_data.EXMPLAYF   is 'EXTREMITYMAINPLATE_YF                           ';
comment on column FEET_data.EXMPLXF1   is 'EXTREMITYMAINPLATE_XF1                          ';
comment on column FEET_data.EXMPLYF1   is 'EXTREMITYMAINPLATE_YF1                          ';
comment on column FEET_data.EXMPLXF2   is 'EXTREMITYMAINPLATE_XF2                          ';
comment on column FEET_data.EXMPLYF2   is 'EXTREMITYMAINPLATE_YF2                          ';
comment on column FEET_data.EXMPLYF3   is 'EXTREMITYMAINPLATE_YF3                          ';
comment on column FEET_data.EXMPLXF3   is 'EXTREMITYMAINPLATE_XF3                          ';
comment on column FEET_data.EXMPLXF4   is 'EXTREMITYMAINPLATE_XF4                          ';
comment on column FEET_data.EXMPLYF4   is 'EXTREMITYMAINPLATE_YF4                          ';
comment on column FEET_data.EXMPLXF5   is 'EXTREMITYMAINPLATE_XF5                          ';
comment on column FEET_data.EXMPLYF5   is 'EXTREMITYMAINPLATE_YF5                          ';
comment on column FEET_data.EXMPLXF6   is 'EXTREMITYMAINPLATE_XF6                          ';
comment on column FEET_data.EXMPLYF6   is 'EXTREMITYMAINPLATE_YF6                          ';
comment on column FEET_data.EXMPLXF7   is 'EXTREMITYMAINPLATE_XF7                          ';
comment on column FEET_data.EXMPLYF7   is 'EXTREMITYMAINPLATE_YF7                          ';
comment on column FEET_data.EXMPLAXI   is 'EXTREMITYMAINPLATE_XI                           ';
comment on column FEET_data.EXMPLAXH   is 'EXTREMITYMAINPLATE_XH                           ';
comment on column FEET_data.EXMPLAYH   is 'EXTREMITYMAINPLATE_YH                           ';
comment on column FEET_data.EXUCPXPO   is 'EXTREMITYUPPERCONNPLATE_X                       ';
comment on column FEET_data.EXUCPZLE   is 'EXTREMITYUPPERCONNPLATE_ZLENGTH                 ';
comment on column FEET_data.EXVCPZLE   is 'EXTREMITYVERTICALCONNPLATE_ZLENGTH              ';
comment on column FEET_data.EXVCPLXA   is 'EXTREMITYVERTICALCONNPLATE_XA                   ';
comment on column FEET_data.EXVCPLYA   is 'EXTREMITYVERTICALCONNPLATE_YA                   ';
comment on column FEET_data.EXVCPLXB   is 'EXTREMITYVERTICALCONNPLATE_XB                   ';
comment on column FEET_data.EXVCPLYB   is 'EXTREMITYVERTICALCONNPLATE_YB                   ';
comment on column FEET_data.EXVCPLXC   is 'EXTREMITYVERTICALCONNPLATE_XC                   ';
comment on column FEET_data.EXVCPLYC   is 'EXTREMITYVERTICALCONNPLATE_YC                   ';
comment on column FEET_data.EXVCPLXD   is 'EXTREMITYVERTICALCONNPLATE_XD                   ';
comment on column FEET_data.EXVCPLYD   is 'EXTREMITYVERTICALCONNPLATE_YD                   ';
comment on column FEET_data.EXVCPLXE   is 'EXTREMITYVERTICALCONNPLATE_XE                   ';
comment on column FEET_data.EXVCPLYE   is 'EXTREMITYVERTICALCONNPLATE_YE                   ';
comment on column FEET_data.EXVCPLXF   is 'EXTREMITYVERTICALCONNPLATE_XF                   ';
comment on column FEET_data.EXVCPLYF   is 'EXTREMITYVERTICALCONNPLATE_YF                   ';
comment on column FEET_data.EXVCPLXP   is 'EXTREMITYVERTICALCONNPLATE_XPOS                 ';
comment on column FEET_data.EXVCPLYP   is 'EXTREMITYVERTICALCONNPLATE_YPOS                 ';
comment on column FEET_data.EXVCPLAX   is 'EXTREMITYVERTICALCONNPLATE_X                    ';
comment on column FEET_data.EXVCPLAY   is 'EXTREMITYVERTICALCONNPLATE_Y                    ';
comment on column FEET_data.EXCNFEVX   is 'EXTREMITYCONNFEETVOUSS_X                        ';
comment on column FEET_data.EXCNFEVY   is 'EXTREMITYCONNFEETVOUSS_Y                        ';
comment on column FEET_data.EXRSLZLE   is 'EXTREMITYRAILSUPPORTLOWER_ZLENGTH               ';
comment on column FEET_data.EXRSVYHE   is 'EXTREMITYRAILSUPPORTVERTICAL_YHEIGHT            ';
comment on column FEET_data.EXRSMXPO   is 'EXTREMITYRAILSUPPORTMIDDLE_XPOS                 ';
comment on column FEET_data.EXRSVXPO   is 'EXTREMITYRAILSUPPORTVERTICAL_XPOS               ';
comment on column FEET_data.EXRSEYPO   is 'EXTREMITYRAILSUPPORTEXTR_Y                      ';
comment on column FEET_data.EXRSUYPO   is 'EXTREMITYRAILSUPPORTUPPER_Y                     ';
comment on column FEET_data.EXRSLYPO   is 'EXTREMITYRAILSUPPORTLOWER_Y                     ';
comment on column FEET_data.EXRSCYPO   is 'EXTREMITYRAILSUPPORTCENTRAL_Y                   ';
comment on column FEET_data.EXRSVYPO   is 'EXTREMITYRAILSUPPORTVERTICAL_Y                  ';
comment on column FEET_data.EXRSVZP1   is 'EXTREMITYRAILSUPPORTVERTICAL1_ZPOS              ';
comment on column FEET_data.EXRSVZP2   is 'EXTREMITYRAILSUPPORTVERTICAL2_ZPOS              ';
comment on column FEET_data.EXRSMZPO   is 'EXTREMITYRAILSUPPORTMIDDLE_ZPOS                 ';
comment on column FEET_data.EXRSEZPO   is 'EXTREMITYRAILSUPPORTEXTR_ZPOS                   ';
comment on column FEET_data.EXRSC1YE   is 'EXTREMITYRAILSUPPORTCUT1_YEXT                   ';
comment on column FEET_data.EXRSC2YE   is 'EXTREMITYRAILSUPPORTCUT2_YEXT                   ';
comment on column FEET_data.EXRSC1XP   is 'EXTREMITYRAILSUPPORTCUT1_XPOS                   ';
comment on column FEET_data.EXRSC2XP   is 'EXTREMITYRAILSUPPORTCUT2_XPOS                   ';
comment on column FEET_data.EXRSC1ZP   is 'EXTREMITYRAILSUPPORTCUT1_ZPOS                   ';
comment on column FEET_data.EXRSC3XP   is 'EXTREMITYRAILSUPPORTCUT3_XPOS                   ';
comment on column FEET_data.EXRSC3ZP   is 'EXTREMITYRAILSUPPORTCUT3_ZPOS                   ';
comment on column FEET_data.EXRSC4Z1   is 'EXRSC4Z1                                        ';
comment on column FEET_data.EXRSC4ZP   is 'EXTREMITYRAILSUPPORTCUT4_ZPOS                   ';
comment on column FEET_data.EXRSMCXA   is 'EXTREMITYRAILSUPPORTMIDDLECUT_XA                ';
comment on column FEET_data.EXRSMCYA   is 'EXTREMITYRAILSUPPORTMIDDLECUT_YA                ';
comment on column FEET_data.EXRSMCXB   is 'EXTREMITYRAILSUPPORTMIDDLECUT_XB                ';
comment on column FEET_data.EXRSMCYB   is 'EXTREMITYRAILSUPPORTMIDDLECUT_YB                ';
comment on column FEET_data.EXRSMCXC   is 'EXTREMITYRAILSUPPORTMIDDLECUT_XC                ';
comment on column FEET_data.EXRSMCYC   is 'EXTREMITYRAILSUPPORTMIDDLECUT_YC                ';
comment on column FEET_data.EXRSMCXP   is 'EXTREMITYRAILSUPPORTMIDDLE_CUT_XPOS             ';
comment on column FEET_data.EXRSMCZ1   is 'EXRSMCZ1                                        ';
comment on column FEET_data.EXRSMCZP   is 'EXTREMITYRAILSUPPORTMIDDLE_CUT_ZPOS             ';
comment on column FEET_data.EXRSPOSX   is 'EXTREMITYRAILSUPPORT_X                          ';
comment on column FEET_data.EXRSPOSY   is 'EXTREMITYRAILSUPPORT_Y                          ';
comment on column FEET_data.EXRSPOSZ   is 'EXTREMITYRAILSUPPORT_Z                          ';
comment on column FEET_data.EXRSPXAP   is 'EXTREMITYRAILSUPPORT_ASSEMBLED_PLUS_X           ';
comment on column FEET_data.EXRSPXAM   is 'EXTREMITYRAILSUPPORT_ASSEMBLED_MINUS_X          ';
comment on column FEET_data.EXRSPYAS   is 'EXTREMITYRAILSUPPORT_ASSEMBLED_Y                ';
comment on column FEET_data.GIR12ZPO   is 'GIR12ZPO                                        ';
comment on column FEET_data.GIR12YPO   is 'GIR12YPO                                        ';
comment on column FEET_data.GIR23ZPO   is 'GIR23ZPO                                        ';
comment on column FEET_data.GIR34ZPO   is 'GIR34ZPO                                        ';

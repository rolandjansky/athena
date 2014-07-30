/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//1234567890123901234 123456789023456 123456789023456123123456789023456123 1234567890123 1234567890123 1234565 4444412344326567890
#ifndef NSWgeometry_h
#define NSWgeometry_h

// NSW STGC (c) Daniel.Lellouch@cern.ch February 28, 2013
// modified by sarka.todorova@cern.ch April 6,2013 - adapt to current layout (stations.v1.63.xml)

static const float H1[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 1016.0, 1016.0, 1016.0, 1016.0} //D0
, { 1457.5, 1535.0, 1622.5, 1710.0} //D1
, { 2538.0, 2538.0, 2538.0, 2538.0} //D2
, { 3507.5, 3507.5, 3507.5, 3507.5} //D3
}
,
{ // SMALL_CONFIRM
 { 1016.0, 1016.0, 1016.0, 1016.0} //D0
, { 1457.5, 1535.0, 1622.5, 1710.0} //D1
, { 2538.0, 2538.0, 2538.0, 2538.0} //D2
, { 3507.5, 3507.5, 3507.5, 3507.5} //D3
}
,
{ // LARGE_PIVOT
 { 982.0, 982.0, 982.0, 982.0} //D0
, { 1545.5, 1633.0, 1720.5, 1808.0} //D1
, { 2538.0, 2538.0, 2538.0, 2538.0} //D2
, { 3598.0, 3598.0, 3598.0, 3598.0} //D3
}
,
{ // LARGE_CONFIRM
 { 982.0, 982.0, 982.0, 982.0} //D0
, { 1545.5, 1633.0, 1720.5, 1808.0} //D1
, { 2538.0, 2538.0, 2538.0, 2538.0} //D2
, { 3598.0, 3598.0, 3598.0, 3598.0} //D3
}
,
{ //TOY_PIVOT
 { -413.5, -413.5, -413.5, -413.5} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { -413.5, -413.5, -413.5, -413.5} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}

};

static const float H2[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 1447.5, 1525.0, 1612.5, 1700.0} //D0
, { 2528.0, 2528.4, 2528.0, 2528.0} //D1
, { 3497.5, 3497.5, 3497.5, 3497.5} //D2
, { 4467.0, 4467.0, 4467.0, 4467.0} //D3
}
,
{ // SMALL_CONFIRM
 { 1447.5, 1525.0, 1612.5, 1700.0} //D0
, { 2528.0, 2528.4, 2528.0, 2528.0} //D1
, { 3497.5, 3497.5, 3497.5, 3497.5} //D2
, { 4467.0, 4467.0, 4467.0, 4467.0} //D3
}
,
{ // LARGE_PIVOT
 { 1535.5, 1613.0, 1710.5, 1798.0} //D1
, { 2528.0, 2528.0, 2528.0, 2528.0} //D1
, { 3588.0, 3588.0, 3588.0, 3588.0} //D2
, { 4648.0, 4648.0, 4648.0, 4648.0} //D3
}
,
{ // LARGE_CONFIRM
 { 1535.5, 1613.0, 1710.5, 1798.0} //D1
, { 2528.0, 2528.0, 2528.0, 2528.0} //D1
, { 3588.0, 3588.0, 3588.0, 3588.0} //D2
, { 4648.0, 4648.0, 4648.0, 4648.0} //D3
}
,
{ //TOY_PIVOT
 { 413.5, 413.5, 413.5, 413.5} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { 413.5, 413.5, 413.5, 413.5} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};


static const float H3[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF } //D1
, { UNDEF , UNDEF , UNDEF , UNDEF } //D2
, { UNDEF , UNDEF , UNDEF , UNDEF } //D3
}
,
{ // SMALL_CONFIRM
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF } //D1
, { UNDEF , UNDEF , UNDEF , UNDEF } //D2
, { UNDEF , UNDEF , UNDEF , UNDEF } //D3
}
,
{ // LARGE_PIVOT
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF } //D1
, { UNDEF , UNDEF , UNDEF , UNDEF } //D2
, { 4648.0, 4648.0, 4648.0, 4648.0} //D3
}
,
{ // LARGE_CONFIRM
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF } //D1
, { UNDEF , UNDEF , UNDEF , UNDEF } //D2
, { 4648.0, 4648.0, 4648.0, 4648.0} //D3
}
,
{ //TOY_PIVOT
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};


// ST the following not updated yet

/*

static const float A[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 384.0, 384.0, 384.0, 384.0} //D0
, { 677.5, 664.2, 650.9, 637.6} //D1
, { 1056.5, 1056.5, 1056.5, 1056.5} //D2
, { 1260.3, 1260.3, 1260.3, 1260.3} //D3
}
,
{ // SMALL_CONFIRM
 { 418.0, 418.0, 418.0, 418.0} //D0
, { 711.5, 698.2, 684.9, 671.6} //D1
, { 1090.5, 1090.5, 1090.5, 1090.5} //D2
, { 1294.3, 1294.3, 1294.3, 1294.3} //D3
}
,
{ // LARGE_PIVOT
 { 598.0, 598.0, 598.0, 598.0} //D0
, { 843.4, 832.8, 822.2, 811.7} //D1
, { 1149.1, 1149.1, 1149.1, 1149.1} //D2
, { 1777.1, 1777.1, 1777.1, 1777.1} //D3
}
,
{ // LARGE_CONFIRM
 { 632.0, 632.0, 632.0, 632.0} //D0
, { 877.4, 866.8, 856.2, 845.7} //D1
, { 1183.1, 1183.1, 1183.1, 1183.1} //D2
, { 1811.1, 1811.1, 1811.1, 1811.1} //D3
}
,
{ //TOY_PIVOT
 { 834.0, 834.0, 834.0, 834.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { 834.0, 834.0, 834.0, 834.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};


static const float B[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 677.5, 664.2, 650.9, 637.6} //D0
, { 1054.4, 1054.4, 1054.4, 1054.4} //D1
, { 1258.2, 1258.2, 1258.2, 1258.2} //D2
, { 1462.0, 1462.0, 1462.0, 1462.0} //D3
}
,
{ // SMALL_CONFIRM
 { 711.5, 698.2, 684.9, 671.6} //D0
, { 1088.4, 1088.4, 1088.4, 1088.4} //D1
, { 1292.2, 1292.2, 1292.2, 1292.2} //D2
, { 1496.0, 1496.0, 1496.0, 1496.0} //D3
}
,
{ // LARGE_PIVOT
 { 843.4, 832.8, 822.2, 811.7} //D0
, { 1143.2, 1143.2, 1143.2, 1143.2} //D1
, { 1771.1, 1771.1, 1771.1, 1771.1} //D2
, { 2251.0, 2251.0, 2251.0, 2251.0} //D3
}
,
{ // LARGE_CONFIRM
 { 877.4, 866.8, 856.2, 845.7} //D0
, { 1177.2, 1177.2, 1177.2, 1177.2} //D1
, { 1805.1, 1805.1, 1805.1, 1805.1} //D2
, { 2285.0, 2285.0, 2285.0, 2285.0} //D3
}
,
{ //TOY_PIVOT
 { 834.0, 834.0, 834.0, 834.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { 834.0, 834.0, 834.0, 834.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};



static const float H1_ACTIVE[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 1032.9, 1032.9, 1032.9, 1032.9} //D0
, { 1694.9, 1664.9, 1634.9, 1604.9} //D1
, { 2554.9, 2554.9, 2554.9, 2554.9} //D2
, { 3524.4, 3524.4, 3524.4, 3524.4} //D3
}
,
{ // SMALL_CONFIRM
 { 1032.9, 1032.9, 1032.9, 1032.9} //D0
, { 1694.9, 1664.9, 1634.9, 1604.9} //D1
, { 2554.9, 2554.9, 2554.9, 2554.9} //D2
, { 3524.4, 3524.4, 3524.4, 3524.4} //D3
}
,
{ // LARGE_PIVOT
 { 995.5, 995.5, 995.5, 995.5} //D0
, { 1691.5, 1661.5, 1631.5, 1601.5} //D1
, { 2551.5, 2551.5, 2551.5, 2551.5} //D2
, { 3611.5, 3611.5, 3611.5, 3611.5} //D3
}
,
{ // LARGE_CONFIRM
 { 995.5, 995.5, 995.5, 995.5} //D0
, { 1691.5, 1661.5, 1631.5, 1601.5} //D1
, { 2551.5, 2551.5, 2551.5, 2551.5} //D2
, { 3611.5, 3611.5, 3611.5, 3611.5} //D3
}
,
{ //TOY_PIVOT
 { -400.0, -400.0, -400.0, -400.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { -400.0, -400.0, -400.0, -400.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};

static const float H2_ACTIVE[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 1667.9, 1637.9, 1607.9, 1577.9} //D0
, { 2517.9, 2517.9, 2517.9, 2517.9} //D1
, { 3470.9, 3470.9, 3470.9, 3470.9} //D2
, { 4440.4, 4440.4, 4440.4, 4440.4} //D3
}
,
{ // SMALL_CONFIRM
 { 1667.9, 1637.9, 1607.9, 1577.9} //D0
, { 2517.9, 2517.9, 2517.9, 2517.9} //D1
, { 3470.9, 3470.9, 3470.9, 3470.9} //D2
, { 4440.4, 4440.4, 4440.4, 4440.4} //D3
}
,
{ // LARGE_PIVOT
 { 1664.5, 1634.5, 1604.5, 1574.5} //D0
, { 2514.5, 2514.5, 2514.5, 2514.5} //D1
, { 3558.0, 3558.0, 3558.0, 3558.0} //D2
, { 4368.0, 4368.0, 4368.0, 4368.0} //D3
}
,
{ // LARGE_CONFIRM
 { 1664.5, 1634.5, 1604.5, 1574.5} //D0
, { 2514.5, 2514.5, 2514.5, 2514.5} //D1
, { 3558.0, 3558.0, 3558.0, 3558.0} //D2
, { 4368.0, 4368.0, 4368.0, 4368.0} //D3
}
,
{ //TOY_PIVOT
 { 400.0, 400.0, 400.0, 400.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { 400.0, 400.0, 400.0, 400.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};

static const float H3_ACTIVE[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF } //D1
, { UNDEF , UNDEF , UNDEF , UNDEF } //D2
, { UNDEF , UNDEF , UNDEF , UNDEF } //D3
}
,
{ // SMALL_CONFIRM
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF } //D1
, { UNDEF , UNDEF , UNDEF , UNDEF } //D2
, { UNDEF , UNDEF , UNDEF , UNDEF } //D3
}
,
{ // LARGE_PIVOT
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF } //D1
, { UNDEF , UNDEF , UNDEF , UNDEF } //D2
, { 4618.0, 4618.0, 4618.0, 4618.0} //D3
}
,
{ // LARGE_CONFIRM
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF } //D1
, { UNDEF , UNDEF , UNDEF , UNDEF } //D2
, { 4618.0, 4618.0, 4618.0, 4618.0} //D3
}
,
{ //TOY_PIVOT
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { UNDEF , UNDEF , UNDEF , UNDEF } //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};

static const float A_ACTIVE[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 356.8, 356.8, 356.8, 356.8} //D0
, { 650.3, 637.0, 623.7, 610.4} //D1
, { 1025.5, 1025.5, 1025.5, 1025.5} //D2
, { 1229.3, 1229.3, 1229.3, 1229.3} //D3
}
,
{ // SMALL_CONFIRM
 { 390.8, 390.8, 390.8, 390.8} //D0
, { 684.3, 671.0, 657.7, 644.4} //D1
, { 1059.5, 1059.5, 1059.5, 1059.5} //D2
, { 1263.3, 1263.3, 1263.3, 1263.3} //D3
}
,
{ // LARGE_PIVOT
 { 569.2, 569.2, 569.2, 569.2} //D0
, { 814.7, 804.1, 793.5, 782.9} //D1
, { 1124.5, 1124.5, 1124.5, 1124.5} //D2
, { 1752.5, 1752.5, 1752.5, 1752.5} //D3
}
,
{ // LARGE_CONFIRM
 { 603.2, 603.2, 603.2, 603.2} //D0
, { 848.7, 838.1, 827.5, 816.9} //D1
, { 1158.5, 1158.5, 1158.5, 1158.5} //D2
, { 1786.5, 1786.5, 1786.5, 1786.5} //D3
}
,
{ //TOY_PIVOT
 { 800.0, 800.0, 800.0, 800.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { 800.0, 800.0, 800.0, 800.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};


static const float B_ACTIVE[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 638.3, 625.0, 611.7, 598.4} //D0
, { 1015.2, 1015.2, 1015.2, 1015.2} //D1
, { 1218.1, 1218.1, 1218.1, 1218.1} //D2
, { 1421.9, 1421.9, 1421.9, 1421.9} //D3
}
,
{ // SMALL_CONFIRM
{ 672.3, 659.0, 645.7, 632.4} //D0
, { 1049.2, 1049.2, 1049.2, 1049.2} //D1
, { 1252.1, 1252.1, 1252.1, 1252.1} //D2
, { 1455.9, 1455.9, 1455.9, 1455.9} //D3
}
,
{ // LARGE_PIVOT
 { 805.2, 794.6, 784.0, 773.4} //D0
, { 1104.9, 1104.9, 1104.9, 1104.9} //D1
, { 1720.8, 1720.8, 1720.8, 1720.8} //D2
, { 2200.6, 2200.6, 2200.6, 2200.6} //D3
}
,
{ // LARGE_CONFIRM
{ 839.2, 828.6, 818.0, 807.4} //D0
, { 1138.9, 1138.9, 1138.9, 1138.9} //D1
, { 1754.8, 1754.8, 1754.8, 1754.8} //D2
, { 2234.6, 2234.6, 2234.6, 2234.6} //D3
}
,
{ //TOY_PIVOT
 { 800.0, 800.0, 800.0, 800.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { 800.0, 800.0, 800.0, 800.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};



static const float A_ADAPTERS[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 428.0, 428.0, 428.0, 428.0} //D0
, { 787.5, 774.2, 760.9, 747.6} //D1
, { 1136.5, 1136.5, 1136.5, 1136.5} //D2
, { 1340.3, 1340.3, 1340.3, 1340.3} //D3
}
,
{ // SMALL_CONFIRM
 { 462.0, 462.0, 462.0, 462.0} //D0
, { 821.5, 808.2, 794.9, 781.6} //D1
, { 1170.5, 1170.5, 1170.5, 1170.5} //D2
, { 1374.3, 1374.3, 1374.3, 1374.3} //D3
}
,
{ // LARGE_PIVOT
 { 642.0, 642.0, 642.0, 642.0} //D0
, { 953.4, 942.8, 932.2, 921.7} //D1
, { 1229.1, 1229.1, 1229.1, 1229.1} //D2
, { 1857.1, 1857.1, 1857.1, 1857.1} //D3
}
,
{ // LARGE_CONFIRM
 { 676.0, 676.0, 676.0, 676.0} //D0
, { 987.4, 976.8, 966.2, 955.7} //D1
, { 1263.1, 1263.1, 1263.1, 1263.1} //D2
, { 1891.1, 1891.1, 1891.1, 1891.1} //D3
}
,
{ //TOY_PIVOT
 { 878.0, 878.0, 878.0, 878.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { 878.0, 878.0, 878.0, 878.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};


static const float B_ADAPTERS[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 721.5, 708.2, 694.9, 681.6} //D0
, { 1098.4, 1098.4, 1098.4, 1098.4} //D1
, { 1338.2, 1338.2, 1338.2, 1338.2} //D2
, { 1542.0, 1542.0, 1542.0, 1542.0} //D3
}
,
{ // SMALL_CONFIRM
{ 755.5, 742.2, 728.9, 715.6} //D0
, { 1132.4, 1132.4, 1132.4, 1132.4} //D1
, { 1372.2, 1372.2, 1372.2, 1372.2} //D2
, { 1576.0, 1576.0, 1576.0, 1576.0} //D3
}
,
{ // LARGE_PIVOT
 { 887.4, 876.8, 866.2, 855.7} //D0
, { 1187.2, 1187.2, 1187.2, 1187.2} //D1
, { 1851.1, 1851.1, 1851.1, 1851.1} //D2
, { 2331.0, 2331.0, 2331.0, 2331.0} //D3
}
,
{ // LARGE_CONFIRM
{ 921.4, 910.8, 900.2, 889.7} //D0
, { 1221.2, 1221.2, 1221.2, 1221.2} //D1
, { 1885.1, 1885.1, 1885.1, 1885.1} //D2
, { 2365.0, 2365.0, 2365.0, 2365.0} //D3
}
,
{ //TOY_PIVOT
 { 878.0, 878.0, 878.0, 878.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { 878.0, 878.0, 878.0, 878.0} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};

*/

static const float PAD_PHI_SUBDIVISION = 1.5; // Module 1 wrt Modules 2 and 3
static const float PAD_PHI_DIVISION = 0.130900; // Modules 2 and 3 in radians


static const float H_PAD_ROW_0[STGC_TYPES][STGC_LAYERS]={
 { 922.5, 883.8, 920.3, 881.5} // SMALL_PIVOT
 ,{ 861.1, 824.2, 859.0, 822.1} // SMALL_CONFIRM
 ,{ 930.0, 891.0, 927.7, 888.6} // LARGE_PIVOT
 ,{ 930.0, 891.0, 927.7, 888.6} // LARGE_CONFIRM
 ,{ -397.5, -437.5, -402.5, -442.5} //TOY_PIVOT
 ,{ -417.5, -457.5, -422.5, -462.5} // TOY_CONFIRM
 };


static const float PAD_HEIGHT[STGC_TYPES][STGC_LAYERS]={
 { 80.00, 80.12, 80.24, 80.37} // SMALL_PIVOT
 ,{ 76.33, 76.45, 76.57, 76.69} // SMALL_CONFIRM
 ,{ 80.65, 80.77, 80.89, 81.01} // LARGE_PIVOT
 ,{ 80.65, 80.77, 80.89, 81.01} // LARGE_CONFIRM
 ,{ 80.00, 80.00, 80.00, 80.00} //TOY_PIVOT
 ,{ 80.00, 80.00, 80.00, 80.00} // TOY_CONFIRM
 };


static const int FIRST_PAD_ROW_DIVISION[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 2, 3, 2, 3} //D0
, { 7, 9, 9, 11} //D1   adjusted - allow 2 row less
, { 21, 21, 21, 21} //D2
, { 33, 33, 33, 33} //D3
}
,
{ // SMALL_CONFIRM
{ 3, 3, 3, 3} //D0
, { 8, 8, 10, 12} //D1  adjusted - allow 2 row less
, { 22, 23, 22, 23} //D2
, { 35, 36, 35, 36} //D3
}
,
{ // LARGE_PIVOT
 { 2, 2, 2, 2} //D0
, { 8, 10, 10, 12} //D1     adjusted - allow 2 rows less 
, { 20, 21, 20, 21} //D2
, { 34, 34, 34, 34} //D3
}
,
{ // LARGE_CONFIRM
{ 2, 2, 2, 2} //D0
, { 7, 8, 7, 8} //D1   adjusted - allow 2 rows less  
, { 20, 21, 20, 21} //D2
, { 34, 34, 34, 34} //D3
}
,
{ //TOY_PIVOT
 { 1, 1, 1, 1} //D0
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
}
,
{ // TOY_CONFIRM
 { 1, 1, 1, 2} //D0
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
}
};

static const int PAD_ROWS[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 5, 6, 7, 8} //D0       
, { 12, 12, 12, 10} //D1    
, { 12, 12, 12, 12} //D2
, { 12, 12, 12, 12} //D3
}
,
{ // SMALL_CONFIRM
{ 5, 7, 7, 9} //D0     
, { 12, 12, 12, 11} //D1     
, { 12, 12, 12, 12} //D2
, { 12, 12, 12, 12} //D3
}
,
{ // LARGE_PIVOT
 { 6, 8, 8, 10} //D0      
, { 12, 11, 10, 11} //D1    
, { 12, 12, 12, 12} //D2
, { 12, 12, 12, 12} //D3
}
,
{ // LARGE_CONFIRM
{ 6, 8, 8, 10} //D0       
, { 12, 11, 10, 11} //D1    
, { 12, 12, 12, 12} //D2
, { 12, 12, 12, 12} //D3
}
,
{ //TOY_PIVOT
 { 10, 11, 10, 11} //D0
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
}
,
{ // TOY_CONFIRM
 { 10, 11, 11, 10} //D0
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
}
};



static const float PAD_COL_PHI0[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 0.002717, -0.040916, -0.002717, -0.046351} //D0
, { 0.002717, -0.040916, -0.002717, -0.046351} //D1
, { 0.002717, -0.062732, -0.002717, -0.068167} //D2
, { 0.002717, -0.062732, -0.002717, -0.068167} //D3
}
,
{ // SMALL_CONFIRM
{ -0.019099, -0.062732, -0.024534, -0.068167} //D0
, { -0.019099, -0.062732, -0.024534, -0.068167} //D1
, { -0.030008, -0.095457, -0.035442, -0.100892} //D2
, { -0.030008, -0.095457, -0.035442, -0.100892} //D3
}
,
{ // LARGE_PIVOT
 { 0.002717, -0.040916, -0.002717, -0.046351} //D0
, { 0.002717, -0.040916, -0.002717, -0.046351} //D1
, { 0.002717, -0.062732, -0.002717, -0.068167} //D2
, { 0.002717, -0.062732, -0.002717, -0.068167} //D3
}
,
{ // LARGE_CONFIRM
{ -0.019099, -0.062732, -0.024534, -0.068167} //D0
, { -0.019099, -0.062732, -0.024534, -0.068167} //D1
, { -0.030008, -0.095457, -0.035442, -0.100892} //D2
, { -0.030008, -0.095457, -0.035442, -0.100892} //D3
}
,
{ //TOY_PIVOT
 { -317.5, -357.5, -322.5, -362.5} //D0 actually X (mm)
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
,
{ // TOY_CONFIRM
 { -337.5, -377.5, -342.5, -302.5} //D0
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
}
};

static const int INDEX_LEFTMOST_COL[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { -2, -2, -2, -2} //D0
, { -2, -1, -2, -1} //D1
, { -1, -1, -1, -1} //D2
, { -1, -1, -1, -1} //D3
}
,
{ // SMALL_CONFIRM
{ -2, -1, -2, -1} //D0
, { -1, -1, -1, -1} //D1
, { -1, -1, -1, -1} //D2
, { -1, -1, -1, -1} //D3
}
,
{ // LARGE_PIVOT
 { -2, -2, -2, -2} //D0
, { -2, -1, -2, -1} //D1
, { -2, -1, -2, -1} //D2
, { -2, -1, -2, -1} //D3
}
,
{ // LARGE_CONFIRM
{ -2, -2, -2, -2} //D0
, { -2, -1, -2, -1} //D1
, { -2, -1, -2, -1} //D2
, { -2, -1, -2, -1} //D3
}
,
{ //TOY_PIVOT
 { 1, 1, 1, 1} //D0 actually 1st column
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
}
,
{ // TOY_CONFIRM
 { 1, 1, 1, 2} //D0 actually 1st column
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
}
};



static const int INDEX_RIGHTMOST_COL[STGC_TYPES][STGC_DETECTORS][STGC_LAYERS]={
{ // SMALL_PIVOT
 { 2, 3, 2, 3} //D0
, { 2, 2, 2, 2} //D1
, { 2, 2, 2, 2} //D2
, { 1, 2, 1, 2} //D3
}
,
{ // SMALL_CONFIRM
{ 2, 3, 3, 3} //D0
, { 2, 2, 2, 2} //D1
, { 2, 2, 2, 2} //D2
, { 2, 2, 2, 2} //D3
}
,
{ // LARGE_PIVOT
 { 3, 3, 3, 3} //D0
, { 2, 2, 2, 2} //D1
, { 2, 2, 2, 2} //D2
, { 2, 2, 2, 2} //D3
}
,
{ // LARGE_CONFIRM
{ 3, 3, 3, 3} //D0
, { 2, 2, 2, 2} //D1
, { 2, 3, 2, 3} //D2
, { 2, 3, 2, 3} //D3
}
,
{ //TOY_PIVOT
 { 9, 10, 9, 10} //D0
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
}
,
{ // TOY_CONFIRM
 { 9, 10, 10, 10} //D0
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
, { UNDEFINED , UNDEFINED , UNDEFINED , UNDEFINED }
}
};

static const float Z_CENTER_STGC_DETECTOR[STGC_TYPES][STGC_LAYERS]={
 { 7407.5, 7418.8, 7430.2, 7441.5} // SMALL_PIVOT
, { 7067.5, 7078.8, 7090.2, 7101.5} // SMALL_CONFIRM
, { 7467.5, 7478.8, 7490.2, 7501.5} // LARGE_PIVOT
, { 7807.5, 7818.8, 7830.2, 7841.5} // LARGE_CONFIRM
, { 13.0, 24.3, 35.7, 47.0} // TOY_PIVOT
, { 353.0, 364.3, 375.7, 387.0} // TOY_CONFIRM
};

static const std::string STGC_LAYER_NAMES[STGC_LAYERS]={
 "Layer 1"
, "Layer 2"
, "Layer 3"
, "Layer 4"
};

static const std::string STGC_DETECTOR_NAMES[STGC_DETECTORS]={
 "STGC Detector 0"
, "STGC Detector 1"
, "STGC Detector 2"
, "STGC Detector 3"
};

static const std::string STGC_MODULE_NAMES[STGC_MODULES]={
 "STGC Module 1"
, "STGC Module 2"
, "STGC Module 3"
};

static const std::string PIVOT_CONFIRM_NAMES[STGC_WEDGES]={
 "Pivot"
, "Confirm"
};

static const std::string SECTOR_NAMES[SECTOR_TYPES]={
 "Small"
, "Large"
, "Toy"
};

static const std::string WEDGE_NAMES[STGC_TYPES]={
 "SmallPivot"
, "SmallConfirm"
, "LargePivot"
, "LargeConfirm"
, "ToyPivot"
, "ToyConfirm"
};

static const std::string SHORT_WEDGE_NAMES[STGC_TYPES]={ "SP" , "SC"
 , "LP" , "LC"
 , "TP" , "TC" };
static const int MM_TYPE[SECTORS_IN_OCTANT][MM_MODULES]={
 { REGULAR , CORNERED , REGULAR , REGULAR }
, { REGULAR , REGULAR , REGULAR , EXTRUDED }
};

static const float MM_H0[SECTORS_IN_OCTANT][MM_MODULES]={
 { UNDEF , 1966.0, UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
};

static const float MM_H1[SECTORS_IN_OCTANT][MM_MODULES]={
 { 1016.0, 1966.0, 2816.0, 3666.0}
, { 982.0, 1932.0, 2882.0, 3832.0}
};

static const float MM_H2[SECTORS_IN_OCTANT][MM_MODULES]={
 { 1946.0, 2796.0, 3646.0, 4471.0}
, { 1912.0, 2862.0, 3812.0, 4647.0}
};

static const float MM_H3[SECTORS_IN_OCTANT][MM_MODULES]={
 { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , 4647.0}
};

static const float MM_D[SECTORS_IN_OCTANT][MM_MODULES]={
 { UNDEF , 1158.4, UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , UNDEF }
};

static const float MM_A[SECTORS_IN_OCTANT][MM_MODULES]={
 { 371.4, 1233.2, 1392.9, 1571.6}
, { 582.3, 1145.1, 1707.9, 2270.7}
};

static const float MM_A_ACTIVE[SECTORS_IN_OCTANT][MM_MODULES]={
 { 272.0, 626.0, 943.0, 1260.0}
, { 478.0, 992.0, 1506.0, 2020.0}
};

static const float MM_B[SECTORS_IN_OCTANT][MM_MODULES]={
 { 1141.8, 1388.7, 1567.4, 1740.8}
, { 1133.3, 1696.1, 2258.9, 2540.3}
};

static const float MM_B_ACTIVE[SECTORS_IN_OCTANT][MM_MODULES]={
 { 619.0, 935.0, 1252.0, 1560.0}
, { 981.0, 1495.0, 2009., 2020.0}
};

static const float MM_C[SECTORS_IN_OCTANT][MM_MODULES]={
 { UNDEF , UNDEF , UNDEF , UNDEF }
, { UNDEF , UNDEF , UNDEF , 2220.0}
};

static const float MM_ZMIN[SECTORS_IN_OCTANT] ={ 7104.5, 7504.5};
static const float MM_ZMAX[SECTORS_IN_OCTANT] ={ 7384.5, 7664.5};

static const std::string MM_MODULE_NAMES[MM_MODULES]={
 "MM Module 1"
, "MM Module 2"
, "MM Module 3"
, "MM Module 4"
};

static const std::string MM_TYPE_NAMES[MM_TYPES]={
 "Regular"
, "Cornered"
, "Extruded"
};

static const float MDT_LOST = 72.0;
static const float MDT_ZMIN[SECTORS_IN_OCTANT] ={ 14458.0, 14049.0};
static const float MDT_ZMAX[SECTORS_IN_OCTANT] ={ 14124.0, 13715.0};

static const float MDT_HMIN[SECTORS_IN_OCTANT][MDT_MODULES] ={
{ 1770.0, 3725.0, 5680.0, 7635.0}
,{ 1770.0, 3485.0, 5440.0, 7395.0}
};

static const float MDT_HMAX[SECTORS_IN_OCTANT][MDT_MODULES] ={
{ 3690.0, 5645.0, 7600.0, 9555.0}
,{ 3450.0, 5405.0, 7360.0, 9315.0}
};

static const float MDT_HALF_B[SECTORS_IN_OCTANT] ={ 417.5, 593.3};
static const float MDT_HALF_OPENING[SECTORS_IN_OCTANT] ={ 8.5, 14.0};

// stereo angle defined here
//static const float MM_STEREO_ANGLE[4] = {0.,0.,0.02618, -0.02618};
static const float MM_STEREO_ANGLE[4] = {0.,0.,0., 0.};

//static const float MM_STEREO_ANGLE_ML1[4] = {0.,0.,0.02618, -0.02618};
//static const float MM_STEREO_ANGLE_ML2[4] = {0.02618,-0.02618,0.,0.};
static const float MM_STEREO_ANGLE_ML1[4] = {0.,0.,0.,0.};
static const float MM_STEREO_ANGLE_ML2[4] = {0.,0.,0.,0.};

// readout at entry (1) or exit(-1) from sensitive volume ( for particle coming from IP )
static const int MM_READOUT[4] = {-1, 1,-1, 1};  

#endif

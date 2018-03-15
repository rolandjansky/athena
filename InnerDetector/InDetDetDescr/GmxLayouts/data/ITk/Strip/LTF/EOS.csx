<?xml version="1.0" encoding="UTF-8" ?>
<!DOCTYPE consolidx SYSTEM "./ConsolidX.dtd" [
<!ENTITY materials  SYSTEM "Materials.gmx"> 
]>

<consolidx name="BarrelEOSBoard" version="1.0"
  xmlns="http://www.nikhef.nl/%7Er29/ConsolidX">

&materials;

<!-- Ref: E-mail from Stanitzki dated 17/01/16 11:45 EoS Material Budget -->

<shapes>
  <box name="shEOS_PCB_FR4"       xhalflength="100./2" yhalflength="45./2" zhalflength="1.650/2" />
  <box name="shEOS_PCB_Cu"        xhalflength="100./2" yhalflength="45./2" zhalflength="0.125/2" />

  <box name="shGBT_FR4"           xhalflength=" 20./2" yhalflength="20./2" zhalflength="0.400/2" />
  <box name="shGBT_Cu"            xhalflength=" 20./2" yhalflength="20./2" zhalflength="0.050/2" />
  <box name="shGBT_Tin"           xhalflength=" 20./2" yhalflength="20./2" zhalflength="0.100/2" />

  <box name="shGBT_Die_Si"        xhalflength="  8./2" yhalflength=" 8./2" zhalflength="0.300/2" />
  <box name="shGBT_Die_Cu"        xhalflength="  8./2" yhalflength=" 8./2" zhalflength="0.100/2" />

  <box name="shVersatileLinkCu"   xhalflength=" 50./2" yhalflength="10./2" zhalflength="0.250/2" />
  <box name="shVersatileLinkFR4"  xhalflength=" 50./2" yhalflength="10./2" zhalflength="0.650/2" />

  <box name="shEOS"               xhalflength="100./2" yhalflength="45./2" zhalflength="5.000/2"/>

</shapes>

<logvol name="EOS_PCB_FR4"      shape="shEOS_PCB_FR4"      material="FR4"/>
<logvol name="EOS_PCB_Cu"       shape="shEOS_PCB_Cu"       material="CuMetal"/>

<logvol name="GBT_FR4"          shape="shGBT_FR4"          material="FR4"/>
<logvol name="GBT_Cu"           shape="shGBT_Cu"           material="CuMetal"/>
<logvol name="GBT_Tin"          shape="shGBT_Tin"          material="SnMetal"/>

<logvol name="GBT_Die_Si"       shape="shGBT_Die_Si"       material="SiMetal"/>
<logvol name="GBT_Die_Cu"       shape="shGBT_Die_Cu"       material="CuMetal"/>

<logvol name="VersatileLinkCu"  shape="shVersatileLinkCu"  material="CuMetal"/>
<logvol name="VersatileLinkFR4" shape="shVersatileLinkFR4" material="CuMetal"/>

<endshape name="EOS" shape="shEOS" activearea="100. * 45.">

  <logvolref ref="EOS_PCB_FR4"      quantity="1"/>
  <logvolref ref="EOS_PCB_Cu"       quantity="1"/>
  <logvolref ref="GBT_FR4"          quantity="1"/>
  <logvolref ref="GBT_Cu"           quantity="1"/>
  <logvolref ref="GBT_Tin"          quantity="1"/>
  <logvolref ref="GBT_Die_Si"       quantity="1"/>
  <logvolref ref="GBT_Die_Cu"       quantity="1"/>
  <logvolref ref="VersatileLinkCu"  quantity="1"/>
  <logvolref ref="VersatileLinkFR4" quantity="1"/>

</endshape>

</consolidx>

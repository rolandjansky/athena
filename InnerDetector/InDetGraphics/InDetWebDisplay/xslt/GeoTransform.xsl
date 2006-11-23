<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:math="http://exslt.org/math" xmlns:svg="http://www.w3.org/2000/svg"
  xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:sar="shaunsNamespace" exclude-result-prefixes="svg" extension-element-prefixes="math sar xs">
  <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"
    doctype-public="-//W3C//DTD SVG 1.0//EN"
    doctype-system="http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd"/>

  <!--Default to no output -->
  <xsl:template match="text()|@*"/>

  <!-- Global sizes -->
  <xsl:variable name="viewXMax">
    <xsl:value-of select="199"/>
  </xsl:variable>
  <xsl:variable name="viewYMax" select="199"/>

  <!-- In SVG, the y coordinate increases from top to bottom -->
  <!-- Global transform to reverse the y axis and scale the graphic -->
  <xsl:attribute-set name="globalTransform">
    <xsl:attribute name="transform">translate(100,110) scale(0.8,-0.8)</xsl:attribute>
  </xsl:attribute-set>

  <!--Main template should match document root -->
  <xsl:template match="AGeometry">
    <xsl:comment>This file is generated from AGeometry.xml by an XSLT 2.0 transform
    Author: Shaun Roe
    Version: 1.0</xsl:comment>
    <svg width="20cm" height="20cm" viewBox="0 0 200 200">
      <defs>
        <g id="globalTransform" xsl:use-attribute-sets="globalTransform"/>
        <xsl:apply-templates mode="defineElements"/>
      </defs>
      <xsl:call-template name="frame"/>
      <g id="detectorPicture" xsl:use-attribute-sets="globalTransform">
        <xsl:apply-templates mode="draw"/>
      </g>
    </svg>
  </xsl:template>
  <xsl:template match="ABarrelSiliconDetector" mode="defineElements">
    <xsl:variable name="gid" select="concat('Barrel',@n,@layer)"/>
    <g id="{$gid}" style="stroke:black; stroke-width: 0.1;fill:grey; fill-opacity: 0.6">
      <!--rectangle rotated about its corner; is this correct? -->
      <rect width="{@thickness}" height="{@width}" x="{-@thickness div 2}" y="{-@width div 2}" transform="rotate({@tilt},0,0)"/>
    </g>
  </xsl:template>
  <xsl:template match="ABarrelTRTDetector" mode="defineElements">
    <xsl:variable name="gid" select="concat('Barrel',@n,@layer)"/>
    <g id="{$gid}" >
      <xsl:variable name="deltaPhi" select="360 div @nphi"/>
      <xsl:variable name="y0" select="@rMin * sar:sinDeg(@phiIn)"/>
      <xsl:variable name="x0" select="@rMin * sar:cosDeg(@phiIn)"/>
      <xsl:variable name="y1" select="@rMax * sar:sinDeg(@phiOut)"/>
      <xsl:variable name="x1" select="@rMax * sar:cosDeg(@phiOut)"/>
      <xsl:variable name="y2" select="@rMax * sar:sinDeg(@phiOut + $deltaPhi)"/>
      <xsl:variable name="x2" select="@rMax * sar:cosDeg(@phiOut +$deltaPhi)"/>
      <xsl:variable name="y3" select="@rMin * sar:sinDeg(@phiIn + $deltaPhi)"/>
      <xsl:variable name="x3" select="@rMin * sar:cosDeg(@phiIn + $deltaPhi)"/>
      <polygon points="{$x0},{$y0} {$x1},{$y1} {$x2},{$y2} {$x3},{$y3}"/>
    </g>
  </xsl:template>
  <!-- Draw the silicon detectors -->
  <xsl:template match="ABarrelSiliconDetector" mode="draw">
    <xsl:variable name="gid" select="concat('Barrel',@n,@layer)"/>
    <xsl:variable name="layerName" select="concat($gid,'Layer')"/>
    <xsl:variable name="deltaPhi" select="360.0 div @nphi"/>
    <xsl:variable name="radius" as="xs:double" select="@r0"/>
    <xsl:variable name="y0" select="$radius * sar:sinDeg(@phi0)"/>
    <xsl:variable name="x0" select="$radius * sar:cosDeg(@phi0)"/>
    <xsl:variable name="lastPhi" as="xs:integer" select="(@nphi - 1) cast as xs:integer"/>
    <xsl:variable name="phiSequence" as="xs:integer*">
      <xsl:sequence select="for $i in 0 to $lastPhi return $i"/>
    </xsl:variable>
    <g id="{$layerName}">
      <xsl:for-each select="$phiSequence">
        <use xlink:href="{concat('#',$gid)}" x="{$x0}" y="{$y0}"
          transform="rotate({. * $deltaPhi},0,0)"/>
        <!--<text x="{$x0}" y="{$y0}" style="font-size: 2pt" transform="rotate({. * $deltaPhi},0,0) scale(1,-1)"><xsl:value-of select="."/></text>-->
      </xsl:for-each>
    </g>
  </xsl:template>
  <!-- Draw the TRT -->
  <xsl:template match="ABarrelTRTDetector" mode="draw">
    <xsl:variable name="gid" select="concat('Barrel',@n,@layer)"/>
    <xsl:variable name="layerName" select="concat($gid,'Layer')"/>
    <xsl:variable name="deltaPhi" select="360.0 div @nphi"/>
    <xsl:variable name="lastPhi" as="xs:integer" select="(@nphi - 1) cast as xs:integer"/>
    <xsl:variable name="phiSequence" as="xs:integer*">
      <xsl:sequence select="for $i in 0 to $lastPhi return $i"/>
    </xsl:variable>
    <g id="{$layerName}">
      <xsl:for-each select="$phiSequence">
        <xsl:variable name="style">
        <xsl:choose>
        <xsl:when test="(. !=6) and (.!=7) and (.!=23) and (.!=22)">
          <xsl:value-of select="'stroke:black; stroke-width: 0.1;fill:blue; fill-opacity: 0.4'"/>
        </xsl:when>
          <xsl:otherwise>
            <xsl:value-of select="'stroke:black; stroke-width: 0.1;fill:blue; fill-opacity: 0.6'"/>
          </xsl:otherwise>
        </xsl:choose>
        </xsl:variable>
        <use xlink:href="{concat('#',$gid)}" x="0" y="0" transform="rotate({ . * $deltaPhi} ,0,0)" style="{$style}"/>
      </xsl:for-each>
    </g>
  </xsl:template>
  <!-- Named templates -->
  <xsl:template name="frame">
    <g id="frame" style="stroke:black; stroke-width: 1; fill:none">
      <rect width="{($viewXMax)}" height="{($viewYMax)}" x="1" y="1"/>
      <rect width="{$viewXMax}" height="15" x="1" y="1" style="fill: #FFFF99; fill-opacity=0.4"/>
    </g>
    <g id="title" style="font-family:serif">
      <text x="5" y="13" >ATLAS</text>
      <text x="50" y="13" style="font-size: 9pt">sedux</text>
      <text x="5" y="198" style="font-size: 4pt">Simple Event Display Using XSLT</text>
    </g>
  </xsl:template>
  <!-- EXSLT Maths functions; May be implementation dependent! -->
  <xsl:function name="sar:sinDeg">
    <xsl:param name="deg"/>
    <xsl:variable name="inRangeDeg" select="$deg mod 360.0"/>
    <xsl:value-of select="math:sin($inRangeDeg div 57.2957795)"/>
  </xsl:function>
  <xsl:function name="sar:cosDeg">
    <xsl:param name="deg"/>
     <xsl:variable name="inRangeDeg" select="$deg mod 360.0"/>
    <xsl:value-of select="math:cos($inRangeDeg div 57.2957795)"/>
  </xsl:function>
</xsl:stylesheet>

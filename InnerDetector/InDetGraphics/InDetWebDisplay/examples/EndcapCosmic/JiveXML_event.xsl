<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:svg="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
  xmlns:math="http://exslt.org/math" extension-element-prefixes="math" version="1.0" >
  <!-- Experiment with this to get web display -->
  <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"
    doctype-public="-//W3C//DTD SVG 1.0//EN" 
    doctype-system="http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd"/>
  <!--<xsl:output method="html" indent="yes" doctype-public="-//W3C//DTD SVG 1.0//EN"
    doctype-system="http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd"/>-->

  <!--Default to no output -->
  <xsl:template match="text()|@*"/>
  <!-- geometry svg file name -->
  <xsl:variable name="localFile" select="'ffGeometry.svg'"/>
  <xsl:variable name="webFile" select="'http://sroe.home.cern.ch/sroe/svg/ffGeometry.svg'"/>
  <!-- Pi, for trig calculations -->
  <xsl:variable name="pi" select="3.14159265359"/>
  <xsl:variable name="geoFile">
 <xsl:choose>
    <xsl:when test="count(document($localFile)/*) != 0">
      <xsl:message>Local geometry file found, using this.</xsl:message>
    <xsl:value-of select="$localFile"/>
    </xsl:when>
   <xsl:otherwise>
     <xsl:choose>
     <xsl:when test="count(document($webFile)/*) !=0">
       <xsl:message>Using the geometry file at http://sroe.home.cern.ch/sroe/svg/geometry.svg</xsl:message>
     <xsl:value-of select="$webFile"/>
     </xsl:when>
       <xsl:otherwise>
       <xsl:message>Your XSLT processor could not find the geometry file; try downloading it locally
         from http://sroe.home.cern.ch/sroe/svg/geometry.svg</xsl:message>
         <xsl:value-of select="''"/>
       </xsl:otherwise>
     </xsl:choose>
   </xsl:otherwise>
</xsl:choose>
  </xsl:variable>
  <!-- get the transform used for the geometry scaling -->
  <xsl:attribute-set name="globalTransform">
    <xsl:attribute name="transform">
      <xsl:value-of
        select="document($geoFile)/svg:svg/svg:defs/svg:g[@id='globalTransform']/@transform"/>
    </xsl:attribute>
  </xsl:attribute-set>
  <!-- Match root element, copy the geometry file to the output -->
  <xsl:template match="Event">
    <xsl:if test="not(contains(document($geoFile)/svg:svg/svg:defs/svg:g[@id='globalTransform']/@transform,'translate')) ">
    <xsl:message>This transform will not succeed because elements of the geometry file could not be retrieved.
    The most likely cause is that an old version of xsltproc is being used.</xsl:message>
    </xsl:if>
    <xsl:comment xml:space="preserve">
This file is generated from a JiveXML event display file using an XSLT 1.0 transform
It also requires a file 'geometry.svg' which defines the detector geometry
Author: Shaun Roe
Version: 2.0</xsl:comment>
    <svg:svg  xmlns:svg="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width="20cm" height="20cm" viewBox="0 0 200 200">
      <!--Add a cluster and a hit marker to the original svg definitions-->
      <svg:defs>
        <svg:g id="cluster">
          <svg:circle cx="0" cy="0" r="0.7"
            style="fill: black; stroke: yellow; stroke-width: 0.1;fill-opacity: 1.0"/>
        </svg:g>
        <svg:g id="hit">
          <svg:circle cx="0" cy="0" r="0.5"
            style="fill: green; stroke: yellow; stroke-width: 0.1;fill-opacity: 0.8"/>
        </svg:g>
        <!-- Copy the original definitions in -->
        <xsl:copy-of select="document($geoFile)/svg:svg/svg:defs/*"/>
      </svg:defs>

      <!-- Copy all the graphics elements in -->
      <xsl:copy-of select="document($geoFile)/svg:svg/svg:g"/>
      <xsl:apply-templates/>
      <svg:g id="eventTitle" style="font-family:serif; font-size:5pt">
        <svg:text x="55" y="13">Run <xsl:value-of select="@runNumber"/>, event <xsl:value-of
            select="@eventNumber"/></svg:text>
      </svg:g>
    </svg:svg>
  </xsl:template>
  <!-- Match to silicon hits -->
  <xsl:template match="STC">
    <xsl:variable name="xstring" select="normalize-space(x0)"/>
    <xsl:variable name="ystring" select="normalize-space(y0)"/>
    <svg:g id="hits" xsl:use-attribute-sets="globalTransform">
      <xsl:call-template name="mergeHitCoords">
        <xsl:with-param name="xList" select="$xstring"/>
        <xsl:with-param name="yList" select="$ystring"/>
        <xsl:with-param name="marker" select="'#hit'"/>
      </xsl:call-template>
    </svg:g>
  </xsl:template>
  <!-- Match to silicon space points -->
  <xsl:template match="S3D">
    <xsl:variable name="xstring" select="normalize-space(x)"/>
    <xsl:variable name="ystring" select="normalize-space(y)"/>
    <svg:g id="SCT_clusters" xsl:use-attribute-sets="globalTransform">
      <xsl:call-template name="mergeHitCoords">
        <xsl:with-param name="xList" select="$xstring"/>
        <xsl:with-param name="yList" select="$ystring"/>
        <xsl:with-param name="marker" select="'#cluster'"/>
      </xsl:call-template>
    </svg:g>
  </xsl:template>
  <!-- Match to TRT hits -->
  <xsl:template match="TRT">
    <xsl:variable name="rstring" select="normalize-space(rhoz)"/>
    <xsl:variable name="pstring" select="normalize-space(phi)"/>
    <svg:g id="TRT_clusters" xsl:use-attribute-sets="globalTransform">
      <xsl:call-template name="mergeRphiCoords">
        <xsl:with-param name="rList" select="$rstring"/>
        <xsl:with-param name="phiList" select="$pstring"/>
        <xsl:with-param name="marker" select="'#hit'"/>
      </xsl:call-template>
    </svg:g>
  </xsl:template>
  <xsl:template match="xKal|Track">
    <xsl:variable name="xstring" select="normalize-space(polylineX)"/>
    <xsl:variable name="ystring" select="normalize-space(polylineY)"/>
    <svg:g id="{concat('xKalTracks',@storeGateKey,@count)}" style="stroke: red; fill: none; stroke-width: 1;stroke-opacity: 0.1"
      xsl:use-attribute-sets="globalTransform">
      <xsl:variable name="pathCoords">
        <xsl:call-template name="mergeTrackCoords">
          <xsl:with-param name="xList" select="$xstring"/>
          <xsl:with-param name="yList" select="$ystring"/>
          <xsl:with-param name="npts" select="polylineX/@multiple * 2"/>
        </xsl:call-template>
      </xsl:variable>

      <svg:path d="{$pathCoords}"/>
    </svg:g>
  </xsl:template>
  <!-- Collect x,y coordinates together and put markers there -->
  <xsl:template name="mergeHitCoords">
    <xsl:param name="xList"/>
    <xsl:param name="yList"/>
    <xsl:param name="marker"/>
    <xsl:choose>
      <xsl:when test="contains($xList,' ')">
        <svg:use xlink:href="{$marker}" x="{substring-before($xList,' ')}"
          y="{substring-before($yList,' ')}"/>
        <!--<xsl:value-of
          select="concat(substring-before($xList,' '),',',substring-before($yList,' '),' ')"/> -->
        <xsl:call-template name="mergeHitCoords">
          <xsl:with-param name="xList" select="substring-after($xList,' ')"/>
          <xsl:with-param name="yList" select="substring-after($yList,' ')"/>
          <xsl:with-param name="marker" select="$marker"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <svg:use xlink:href="{$marker}" x="{$xList}" y="{$yList}"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <!-- Collect r,phi coordinates together and put markers there -->
  <xsl:template name="mergeRphiCoords">
    <xsl:param name="rList"/>
    <xsl:param name="phiList"/>
    <xsl:param name="marker"/>
    <xsl:choose>
      <xsl:when test="contains($rList,' ')">
        <xsl:variable name="r" select="substring-before($rList,' ')"/>
        <!-- phi angle is in radians and 0 = positive x axis-->
        <xsl:variable name="phi" select="substring-before($phiList,' ')"/>
        <xsl:variable name="sinPhi">
          <xsl:call-template name="sine">
            <xsl:with-param name="angle" select="$phi"/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="cosPhi">
          <xsl:call-template name="cosine">
            <xsl:with-param name="angle" select="$phi"/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="y" select="$r * $sinPhi"/>
        <xsl:variable name="x" select="$r * $cosPhi"/>
        <svg:use xlink:href="{$marker}" x="{$x}" y="{$y}"/>
        <!--<xsl:value-of
          select="concat(substring-before($xList,' '),',',substring-before($yList,' '),' ')"/> -->
        <xsl:call-template name="mergeRphiCoords">
          <xsl:with-param name="rList" select="substring-after($rList,' ')"/>
          <xsl:with-param name="phiList" select="substring-after($phiList,' ')"/>
          <xsl:with-param name="marker" select="$marker"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:variable name="r" select="$rList"/>
        <xsl:variable name="phi" select="$phiList"/>
        <xsl:variable name="sinPhi">
          <xsl:call-template name="sine">
            <xsl:with-param name="angle" select="$phi"/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="cosPhi">
          <xsl:call-template name="cosine">
            <xsl:with-param name="angle" select="$phi"/>
          </xsl:call-template>
        </xsl:variable>
        <xsl:variable name="y" select="$r * $sinPhi"/>
        <xsl:variable name="x" select="$r * $cosPhi"/>
        <svg:use xlink:href="{$marker}" x="{$x}" y="{$y}"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <!-- Collect x,y points on a track together and draw a line between them -->
  <xsl:template name="mergeTrackCoords">
    <xsl:param name="xList"/>
    <xsl:param name="yList"/>
    <xsl:param name="npts"/>
    <xsl:param name="iteration" select="$npts"/>
    <xsl:variable name="instruction">
      <!-- See whether this is at the beginning of a line, so 'move to', or in a line, so 'draw to' -->
      <xsl:choose>
        <xsl:when test="($iteration mod $npts) != 0">
          <xsl:value-of select="'L '"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="'M '"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="contains($xList,' ')">
        <xsl:value-of
          select="concat($instruction,substring-before($xList,' '),',',substring-before($yList,' '),' ')"/>
        <xsl:call-template name="mergeTrackCoords">
          <xsl:with-param name="xList" select="substring-after($xList,' ')"/>
          <xsl:with-param name="yList" select="substring-after($yList,' ')"/>
          <xsl:with-param name="npts" select="$npts"/>
          <xsl:with-param name="iteration" select="$iteration - 1"/>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="concat('L ',$xList,',',$yList)"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
   <!-- robust sine and cosine functions -->
  <!-- (only the default solution works right now) -->
  <xsl:template name="sine">
    <xsl:param name="angle"/>
    <xsl:variable name="a" select="$angle mod (2.0* $pi)"/>
    <xsl:choose>
      <xsl:when test="function-available('math:sin')">
        <xsl:value-of select="math:sin($angle)"/>
      </xsl:when>
      <xsl:otherwise>
       <xsl:call-template name="approxSin">
       <xsl:with-param name="rad" select="$angle"/>
       </xsl:call-template>
        
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template name="cosine">
    <xsl:param name="angle"/>
    <xsl:variable name="a" select="$angle mod (2.0* $pi)"/>
    <xsl:choose>
      <xsl:when test="(function-available('math:cos'))">
        <xsl:value-of select="math:cos($angle)"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="approxSin">
          <xsl:with-param name="rad" select="$angle + ($pi div 2)"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <!-- Maclaurin expansion for sin -->
  <!-- This code is under development, and doesnt yet work. -->
  <!-- The default solution assumes the presence of the EXSLT math: extensions -->
  <xsl:template name="approxSin">
    <xsl:param name="rad"/>
  <xsl:variable name="a" select="$rad mod (2.0* $pi)"/>
    <xsl:variable name="pi2" select="$pi div 2"/>
    <xsl:variable name="phi0">
      <xsl:choose>
        <xsl:when test="$a &gt; ($pi + $pi2)">
          <xsl:value-of select="$a - (2 * $pi)"/>
        </xsl:when>
        <xsl:when test="$a &lt; -($pi + $pi2)">
          <xsl:value-of select="$a + (2 * $pi)"/>
        </xsl:when>
        <xsl:when test="$a &gt; $pi2">
          <xsl:value-of select="$pi - $a"/>
        </xsl:when>
        <xsl:when test="$a &lt; -$pi2">
          <xsl:value-of select="- $a - $pi"/>
        </xsl:when>
        <xsl:otherwise>
        <xsl:value-of select="$a"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="phi2" select="$phi0 * $phi0"/>
    <xsl:variable name="term1" select="($phi0 div 6.0) * $phi2"/>
    <xsl:variable name="term2" select="$phi2 * $term1 div 20.0"/>
    <xsl:variable name="term3" select="$phi2 * $term2 div 42.0"/>
    <xsl:value-of select="$phi0 - $term1 +$term2 -$term3"/>

  </xsl:template>
</xsl:stylesheet>

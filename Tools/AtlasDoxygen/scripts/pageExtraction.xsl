<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:param  name="path"/>
	<xsl:param  name="packageName"/>
	<xsl:output indent="yes" method="html"/>
	<!--Default to no output -->
	<xsl:template match="text()|@*"/>
	<!-- match to root element-->
	<xsl:template match="tagfile">

		<!--	
			<li>
				<xsl:for-each select="compound">
					<ul><xsl:value-of select="class"/></ul>
				</xsl:for-each>
			</li>
		-->
		<h3><xsl:value-of select="$packageName"/> (<a href="{$path}index.html">mainpage</a>)</h3>
				<ul>
					<xsl:apply-templates/>
				</ul>
	</xsl:template>

	<xsl:template match="compound[@kind='class']">
		<li>
<!--			<xsl:message><xsl:value-of select="$path"/></xsl:message>	
			<a href="/afs/cern.ch/atlas/software/builds/nightlies/dev/AtlasOffline/latest_doxygen/InstallArea/doc/TrkTrack/html/{filename}">-->
				<a href="{$path}{filename}">
				<xsl:value-of select="name"/></a>
		</li>
	</xsl:template>
	
<!--		
	<xsl:template match="compound[@kind='class']">
		<li>
			<a href="{name}"><xsl:value-of select="class"/></a>
		</li>
	</xsl:template>

	<xsl:template match="name">
A name
	</xsl:template>
-->

</xsl:stylesheet>

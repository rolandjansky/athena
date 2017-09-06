<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xd="http://www.oxygenxml.com/ns/doc/xsl"
    exclude-result-prefixes="xs xd" version="1.0">
    <xd:doc scope="stylesheet">
        <xd:desc>
            <xd:p><xd:b>Created on:</xd:b> Mar 22, 2016</xd:p>
            <xd:p><xd:b>Author:</xd:b> sroe</xd:p>
            <xd:p>Produces a plain text format of the hdef xml file.</xd:p>
        </xd:desc>
    </xd:doc>
    <xsl:output method="text"/>
    <xsl:template match="@*|node()"/>
    <xsl:template match="hdef">
        <xsl:apply-templates/>
    </xsl:template>
    <xsl:template match="h">
        <xsl:variable name="s" select="' '"/>
        <xsl:variable name="cr" select="'&#x0A;'"/>
        <xsl:variable name="q" select="'&quot;'"/>
        <xsl:value-of select="concat(@id,$s,@type,$s,$q,@title,$q,$s,x/@n,$s,x/@lo,$s,x/@hi,$s,$q,x/@title,$q,$s,y/@n,$s,y/@lo,$s,y/@hi,$s,$q,y/@title,$q,$s)"/>
<xsl:value-of select="$cr"/>
    </xsl:template>
</xsl:stylesheet>

<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:math="http://www.exslt.org/math" exclude-result-prefixes="math"
    xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
    <xsl:output method="xml" indent="yes"/>
    <xsl:param name="typ" select="'Frequency'"/>
    <xsl:param name="var" select="'thresholds'"/>
    <xsl:variable name="nodes" select="/histogram/b"/>
    <xsl:param name="minx" select="0"/>
    <xsl:param name="maxx" select="8"/>
    <xsl:param name="miny" select="0"/>
    <xsl:param name="maxy" select="7000"/>
    <xsl:param name="xtick" select="1"/>
    <xsl:param name="ytick" select="500"/>
    <xsl:param name="xwindow" select="600.0"/>
    <xsl:param name="ywindow" select="400.0"/>
    <!-- scaling factors -->
    <xsl:variable name="sx" select="$xwindow div ($maxx - $minx)"/>
    <xsl:variable name="sy" select="$ywindow div ($maxy - $miny)"/>
    <!-- number of ticks -->
    <xsl:variable name="xnticks" select="floor(($maxx - $minx) div $xtick)"/>
    <xsl:variable name="ynticks" select="floor(($maxy - $miny) div $ytick)"/>
    <!--Default to no output -->
    <xsl:template match="text()|@*"/>
    <!-- root template -->
    <xsl:template match="histogram">
        <xsl:message><xsl:value-of select="$maxy"/></xsl:message>
        <svg width="{$xwindow}px" height="{$ywindow}px"
            viewBox="-60 -40 {$xwindow+60} {$ywindow+60}">
            <!--Add a marker-->
            <defs>
                <g id="mark">
                    <circle cx="0" cy="0" r="0.7" fill="black" stroke="yellow" stroke-width="1px"/>
                </g>
                <g id="ytick">
                    <line x1="0" y1="0" x2="10" y2="0" stroke="grey" stroke-width="1px"/>
                </g>
                <g id="xtick">
                    <line x1="0" y1="0" x2="0" y2="10" stroke="darkgrey" stroke-width="1px"/>
                </g>
            </defs>
            <rect id="frame" width="{$xwindow}" height="{$ywindow}" x="0" y="0" stroke="grey"
                fill="white" stroke-width="1px"/>
            <g id="axes" transform="translate (0,{$ywindow}) scale(1,-1)">
                <xsl:call-template name="xticks">
                    <xsl:with-param name="nticks" select="$xnticks"/>
                </xsl:call-template>
                <xsl:call-template name="yticks">
                    <xsl:with-param name="nticks" select="$ynticks"/>
                </xsl:call-template>
                <xsl:call-template name="ytickLabels">
                    <xsl:with-param name="nticks" select="$ynticks"/>
                </xsl:call-template>
                <xsl:call-template name="xtickLabels">
                    <xsl:with-param name="nticks" select="$xnticks"/>
                </xsl:call-template>
            </g>
            <g id="title">
                <xsl:call-template name="caption"/>
            </g>
            <g id="datapoints" transform="translate (0,{$ywindow}) scale(1,-1)">
                <xsl:for-each select="$nodes">
                    <xsl:variable name="xval" select="@x -$minx"/>
                    <xsl:variable name="xscreen" select="$xval * $sx"/>
                    <xsl:variable name="ylabel" select="concat(@x,', ',.)"/>
                    <xsl:variable name="yval" select=". - $miny"/>
                    <xsl:variable name="yscreen" select="$yval * $sy"/>
                    <!-- put a mark at this point-->
                    <circle cx="{$xscreen}" cy="{$yscreen}" r="3px" fill="blue" title="{$ylabel}"
                        id="{$xval}"/>
                </xsl:for-each>
            </g>
        </svg>
    </xsl:template>
    <xsl:template name="caption">
        <text x="200" y="50"><xsl:value-of select="$typ"/> values for <xsl:value-of select="$var"
            /></text>
    </xsl:template>
    <xsl:template name="yticks">
        <xsl:param name="nticks" select="10"/>
        <xsl:if test="$nticks != 0">
            <use xlink:href="#ytick" x="0" y="{$nticks * $ywindow div $ynticks}"/>
            <xsl:call-template name="yticks">
                <xsl:with-param name="nticks" select="$nticks - 1"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>
    <xsl:template name="xticks">
        <xsl:param name="nticks" select="10"/>
        <xsl:if test="$nticks != 0">
            <use xlink:href="#xtick" x="{$nticks * $xwindow div $xnticks}" y="0"/>
            <xsl:call-template name="xticks">
                <xsl:with-param name="nticks" select="$nticks - 1"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>
    <xsl:template name="ytickLabels">
        <xsl:param name="nticks" select="10"/>
        <xsl:if test="$nticks != 0">
            <text x="-5" y="{-$nticks * $ywindow div $ynticks + 4}" transform="scale(1,-1)"
                font-size="12px" text-anchor="end">
                <xsl:value-of select="format-number($nticks * $ytick + $miny,'#.###')"/>
            </text>
            <xsl:call-template name="ytickLabels">
                <xsl:with-param name="nticks" select="$nticks - 1"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>
    <xsl:template name="xtickLabels">
        <xsl:param name="nticks" select="10"/>
        <xsl:if test="$nticks != 0">
            <text x="{$nticks * $xwindow div $xnticks + 4}" y="10" transform="scale(1,-1)"
                font-size="12px" text-anchor="end">
                <xsl:value-of select="format-number($nticks * $xtick + $minx,'#.##')"/>
            </text>
            <xsl:call-template name="xtickLabels">
                <xsl:with-param name="nticks" select="$nticks - 1"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>
   
</xsl:stylesheet>

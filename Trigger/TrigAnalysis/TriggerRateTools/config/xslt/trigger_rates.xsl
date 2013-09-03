<?xml version="1.0" encoding="ISO-8859-1"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="cell_backg">#ffffff</xsl:variable>
<xsl:variable name="L1_budget">45000</xsl:variable>
<xsl:variable name="L2_budget">1000</xsl:variable>
<xsl:variable name="EF_budget">200</xsl:variable>
<xsl:variable name="doColour">yes</xsl:variable>

<xsl:template match="/">
    <html>
    <body>

    <p>L1 bandwidth budget: <xsl:value-of select="$L1_budget"/> Hz</p>
    <p>L2 bandwidth budget: <xsl:value-of select="$L2_budget"/> Hz</p>
    <p>EF bandwidth budget: <xsl:value-of select="$EF_budget"/> Hz</p>

    <xsl:for-each select="trigger">
        <p>Cross-section: <xsl:value-of select="Xsection"/> b</p>
        <p>Luminosity: <xsl:value-of select="Luminosity"/> 1/b 1/s</p>
        
        <center>
        <h2>(<xsl:value-of select="n_evts"/> events)</h2>
        <table border="1">
            <tr bgcolor="#e3e3ff">
                <th align="left">Signature</th>
                <th align="left">Algorithm</th>
                <th align="left">Cut Name</th>
                <th align="left">Threshold</th>
            </tr>
        </table>
        </center>
        <xsl:for-each select="level">
        <center>
        <h2><xsl:value-of select="lvl_name"/>Single Signatures</h2>
        <table border="1">
            <tr bgcolor="#e3e3ff">
                <th align="left">Signature</th>
                <th align="left">Prescale</th>
                <th align="left">Pass-Through</th>
                <th align="left">#Evts Accepted</th>
                <th align="left">Efficiency</th>
                <th align="left">Efficiency Error</th>
                <th align="left">Prescaled Eff.</th>
                <th align="left">Prescaled Eff. Error</th>
                <th align="left">Rate (Hz)</th>
                <th align="left">Rate Error (Hz)</th>
                <th align="left">Indep. Frac.</th>
                <th align="left">Unique Frac.</th>
                <th align="left">Unique Rate (Hz)</th>
                <!--<th align="left">Largest Overlap Sig</th>
                <th align="left">Largest Overlap</th>-->
            </tr>
            <tr>
                <td>Overall</td>
                <td align="center">---</td>
                <td align="center">---</td>
                <td align="center">---</td>
                <td align="center"><xsl:value-of select="lvl_eff"/></td>
                <td align="center"><xsl:value-of select="lvl_eff_err"/></td>
                <td align="center"><xsl:value-of select="lvl_prescaled_eff"/></td>
                <td align="center"><xsl:value-of select="lvl_prescaled_eff_err"/></td>
                <td align="center"><xsl:value-of select="lvl_rate"/></td>
                <td align="center"><xsl:value-of select="lvl_rate_err"/></td>
                <td align="center">---</td>
                <td align="center">---</td>
                <td align="center">---</td>
                <!--<td align="center"></td>
                <td align="center"></td>-->
            </tr>
            <xsl:for-each select="signature">
            <tr>
                <xsl:variable name="f_rate">
                    <xsl:call-template name="Scientific">
                        <xsl:with-param name="Num" select="rate"/>
                    </xsl:call-template>
                </xsl:variable>
                <xsl:variable name="cell_backg">
                    <xsl:if test="$doColour='yes'">
                        <xsl:choose>
                            <xsl:when test="type='dummy'">
                                #7f7f7f
                            </xsl:when>
                            <xsl:when test="efficiency='0.00e+00'">
                                #7f7fff
                            </xsl:when>
                            <xsl:when test="../lvl_name='L1'">
                                <xsl:if test="$f_rate &gt; $L1_budget">
                                    #ff7f7f
                                </xsl:if>
                            </xsl:when>
                            <xsl:when test="../lvl_name='L2'">
                                <xsl:if test="$f_rate &gt; $L2_budget">
                                    #ff7f7f
                                </xsl:if>
                            </xsl:when>
                            <xsl:when test="../lvl_name='EF'">
                                <xsl:if test="$f_rate &gt; $EF_budget">
                                    #ff7f7f
                                </xsl:if>
                            </xsl:when>
                            <xsl:otherwise>
                                #ffffff
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:if>
                    <xsl:if test="not($doColour='yes')">
                        #ffffff
                    </xsl:if>
                </xsl:variable>
                <td align="left"   bgcolor="{$cell_backg}"><xsl:value-of select="sig_name"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="prescale"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="passthrough"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="evts_passed"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="efficiency"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="efficiency_err"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="prescaled_efficiency"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="prescaled_efficiency_err"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="rate"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="rate_err"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="indep_fraction"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="unique_fraction"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="unique_rate"/></td>
                <!--<td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="largest_overlap_name"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="largest_overlap"/></td>-->
            </tr>
            </xsl:for-each>
        </table>
        </center>
        </xsl:for-each>
    
        <center>
        <h2>Complex Signatures</h2>
        <table border="1">
            <tr bgcolor="#e3e3ff">
                <th align="left">Signature</th>
                <th align="left">Type</th>
                <th align="left">Prescale</th>
                <th align="left">Efficiency</th>
                <th align="left">Efficiency Error</th>
                <th align="left">Prescaled Eff.</th>
                <th align="left">Prescaled Eff. Error</th>
                <th align="left">Rate (Hz)</th>
                <th align="left">Rate Error (Hz)</th>
                <!--<th align="left">Largest Overlap Sig</th>
                <th align="left">Largest Overlap</th>-->
                <th align="left">Components</th>
            </tr>
            <xsl:for-each select="cplx_signature">
            <tr>
                <xsl:variable name="f_rate">
                    <xsl:call-template name="Scientific">
                        <xsl:with-param name="Num" select="rate"/>
                    </xsl:call-template>
                </xsl:variable>
                <xsl:variable name="cell_backg">
                    <xsl:if test="$doColour='yes'">
                        <xsl:choose>
                            <xsl:when test="type='dummy'">
                                #7f7f7f
                            </xsl:when>
                            <xsl:when test="efficiency='0.00e+00'">
                                #7f7fff
                            </xsl:when>
                            <!--<xsl:when test="lvl_name='L1'">
                                <xsl:if test="$f_rate &gt; $L1_budget">
                                    #ff7f7f
                                </xsl:if>
                            </xsl:when>
                            <xsl:when test="lvl_name='L2'">
                                <xsl:if test="$f_rate &gt; $L2_budget">
                                    #ff7f7f
                                </xsl:if>
                            </xsl:when>
                            <xsl:when test="lvl_name='EF'">
                                <xsl:if test="$f_rate &gt; $EF_budget">
                                    #ff7f7f
                                </xsl:if>
                            </xsl:when>-->
                            <xsl:when test="$f_rate &gt; $EF_budget">
                                    #ff7f7f
                            </xsl:when>
                            <xsl:otherwise>
                                #ffffff
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:if>
                    <xsl:if test="not($doColour='yes')">
                        #ffffff
                    </xsl:if>
                </xsl:variable>
                <td align="left"   bgcolor="{$cell_backg}"><xsl:value-of select="sig_name"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="type"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="prescale"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="efficiency"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="efficiency_err"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="prescaled_efficiency"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="prescaled_efficiency_err"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="rate"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="rate_err"/></td>
                <!--<td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="largest_overlap_name"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="largest_overlap"/></td>-->
                <td align="left"   bgcolor="{$cell_backg}"><table border="0">
                    <xsl:for-each select="components/sig">
                    <tr><xsl:value-of select="sig_name"/></tr>
                    </xsl:for-each>
                </table></td>
            </tr>
            </xsl:for-each>
        </table>
        </center>

        <!--<center>
        <h2>Streams</h2>
        <table border="1">
            <tr bgcolor="#e3e3ff">
                <th align="left">Stream</th>
                <th align="left">Prescale</th>
                <th align="left">Efficiency</th>
                <th align="left">Efficiency Error</th>
                <th align="left">Prescaled Eff.</th>
                <th align="left">Prescaled Eff. Error</th>
                <th align="left">Rate (Hz)</th>
                <th align="left">Rate Error (Hz)</th>
                <th align="left">Unique Rate (Hz)</th>
                <th align="left">Largest Overlap Sig</th>
                <th align="left">Largest Overlap</th>
                <th align="left">Components</th>
            </tr>
            <xsl:for-each select="stream">
            <tr>
                <xsl:variable name="f_rate">
                    <xsl:call-template name="Scientific">
                        <xsl:with-param name="Num" select="rate"/>
                    </xsl:call-template>
                </xsl:variable>
                <xsl:variable name="cell_backg">
                    <xsl:if test="$doColour='yes'">
                        <xsl:choose>
                            <xsl:when test="type='dummy'">
                                #7f7f7f
                            </xsl:when>
                            <xsl:when test="efficiency='0.00e+00'">
                                #7f7fff
                            </xsl:when>
                            <xsl:when test="$f_rate &gt; $EF_budget">
                                    #ff7f7f
                            </xsl:when>
                            <xsl:otherwise>
                                #ffffff
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:if>
                    <xsl:if test="not($doColour='yes')">
                        #ffffff
                    </xsl:if>
                </xsl:variable>
                <td align="left"   bgcolor="{$cell_backg}"><xsl:value-of select="sig_name"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="prescale"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="efficiency"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="efficiency_err"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="prescaled_efficiency"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="prescaled_efficiency_err"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="rate"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="rate_err"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="unique_rate"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="largest_overlap_name"/></td>
                <td align="center" bgcolor="{$cell_backg}"><xsl:value-of select="largest_overlap"/></td>
                <td align="left"   bgcolor="{$cell_backg}"><table border="0">
                    <xsl:for-each select="components/sig">
                        <tr><xsl:value-of select="sig_name"/></tr>
                    <xsl:value-of select="sig_name"/>; 
                    </xsl:for-each>
                </table></td>
            </tr>
            </xsl:for-each>
        </table>
        </center>-->


        <!--<xsl:for-each select="level">
        <center>
        <h2><xsl:value-of select="lvl_name"/> Signatures Overlap = N(Sx + Sy) / N(Sy)</h2>
        <table border="1">
            <tr bgcolor="#e3e3ff">
                <th align="left">
                    <table border="0">
                        <tr>
                            <th align="center">
                                <tr bgcolor="#e3e3ff">
                                    <th align="left">Signature</th>
                                </tr>
                                <tr bgcolor="#e3e3ff">
                                    <th align="center">Sy</th>
                                </tr>
                            </th>
                            <th bgcolor="#e3e3ff" align="center">Sx</th>
                        </tr>
                    </table>
                </th>
                <xsl:for-each select="signature[1]/overlap/sig">
                    <th align="center"><xsl:value-of select="sig_name"/></th>
                </xsl:for-each>
            </tr>
            <xsl:for-each select="signature">
            <tr>
                <th bgcolor="#e3e3ff" align="left"><xsl:value-of select="sig_name"/></th>
                <xsl:for-each select="overlap/sig">
                <td align="center"><xsl:value-of select="sig_overlap"/></td>
                </xsl:for-each>
            </tr>
            </xsl:for-each>
        </table>
        </center>
        </xsl:for-each>-->

        <!--
        <center>
        <h2>Streams Overlap = N(Sx + Sy) / N(Sy)</h2>
        <table border="1">
            <tr bgcolor="#e3e3ff">
                <th align="left">
                    <table border="0">
                        <tr>
                            <th align="center">
                                <tr bgcolor="#e3e3ff">
                                    <th align="left">Stream</th>
                                </tr>
                                <tr bgcolor="#e3e3ff">
                                    <th align="center">Sy</th>
                                </tr>
                            </th>
                            <th bgcolor="#e3e3ff" align="center">Sx</th>
                        </tr>
                    </table>
                </th>
                <xsl:for-each select="stream[1]/overlap/sig">
                    <th align="center"><xsl:value-of select="sig_name"/></th>
                </xsl:for-each>
            </tr>
            <xsl:for-each select="stream">
            <tr>
                <th bgcolor="#e3e3ff" align="left"><xsl:value-of select="sig_name"/></th>
                <xsl:for-each select="overlap/sig">
                <td align="center"><xsl:value-of select="sig_overlap"/></td>
                </xsl:for-each>
            </tr>
            </xsl:for-each>
        </table>
        </center>-->
    </xsl:for-each>

    </body>
    </html>

</xsl:template>

<xsl:template name="Scientific">
    <xsl:param name="Num"/>
    <xsl:if test="boolean(number(substring-after($Num,'e')))">
        <xsl:call-template name="Scientific_Helper">
            <xsl:with-param name="m" select="substring-before($Num,'e')"/>
            <xsl:with-param name="e" select="substring-after($Num,'e')"/>
        </xsl:call-template>
    </xsl:if>
    <xsl:if test="boolean(number(substring-after($Num,'e+')))">
        <xsl:call-template name="Scientific_Helper">
            <xsl:with-param name="m" select="substring-before($Num,'e+')"/>
            <xsl:with-param name="e" select="substring-after($Num,'e+')"/>
        </xsl:call-template>
    </xsl:if>
</xsl:template>

<xsl:template name="Scientific_Helper">
    <xsl:param name="m"/>
    <xsl:param name="e"/>
    <xsl:choose>
        <xsl:when test="$e = 0 or not(boolean($e))">
            <xsl:value-of select="$m"/>
        </xsl:when>
        <xsl:when test="$e &gt; 0">
            <xsl:call-template name="Scientific_Helper">
                <xsl:with-param name="m" select="$m * 10"/>
                <xsl:with-param name="e" select="$e - 1"/>
            </xsl:call-template>
        </xsl:when>
        <xsl:when test="$e &lt; 0">
            <xsl:call-template name="Scientific_Helper">
                <xsl:with-param name="m" select="$m div 10"/>
                <xsl:with-param name="e" select="$e + 1"/>
            </xsl:call-template>
        </xsl:when>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>

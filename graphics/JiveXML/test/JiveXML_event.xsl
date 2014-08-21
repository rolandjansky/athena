<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

    <xsl:template match="/">
        <html>         
            <head>
                <style type="text/css">
                    table { background-color: #eeeeff; font-family: sans-serif; font-size:10pt; border: solid 1px black; }
                    td,th { border: solid 1px black; margin: 0px; padding: 2px; }
                </style>
            </head>
            <body>
                <table> 
                    <tr style="font-weight:bold;background-color:#bbbbff;">
                        <th>Tag Name</th>
                        <th>Count Value</th>
                        <th>Store Gate Key</th>
                        <th>Size</th>
                    </tr>
                    <xsl:for-each select="Event/*">
                        <tr>
                            <td><xsl:value-of select="name(.)" /></td>
                            <td><xsl:value-of select="@count" /></td>
                            <td><xsl:value-of select="@storeGateKey" /></td>
                            <td style="text-align: right;"><xsl:value-of select="string-length(.)" /></td>
                        </tr>
                    </xsl:for-each>
                </table>
            </body>
        </html>
    </xsl:template>

</xsl:stylesheet>

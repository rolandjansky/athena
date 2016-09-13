<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:xd="http://www.oxygenxml.com/ns/doc/xsl"
    exclude-result-prefixes="xs xd" version="1.0">
    <xd:doc scope="stylesheet">
        <xd:desc>
            <xd:p><xd:b>Created on:</xd:b> Mar 22, 2016</xd:p>
            <xd:p><xd:b>Author:</xd:b> sroe</xd:p>
            <xd:p>Produces an html representation of the hdef xml file.</xd:p>
        </xd:desc>
    </xd:doc>
    <xsl:output indent="yes" method="html"/>
    <xsl:template match="@*|node()"/>
    <xsl:template match="hdef">
        <html>
            <head>
                <title>Histogram definitions</title>
                <style type="text/css">
table{
  width: 800px;
  border-collapse: collapse;
}
th, td{
  border: none;
}
th{
  background-color: #000;
  color: #fff;
}
tr:nth-of-type(even){ background-color: #F3F3F3;
}
tr:nth-of-type(odd) { background-color:#ddd;
}

                </style>
            </head>
            <body>
                <h1>Histogram Definitions</h1>
                <table>
                    <thead>
                        <tr>
                            <th>Unique id</th>
                            <th>Type</th>
                            <th>Title</th>
                            <th>x:Nbins</th>
                            <th>x:lo</th>
                            <th>x:hi</th>
                            <th>y:Title</th>
                            <th>y:Nbins</th>
                            <th>y:lo</th>
                            <th>y:hi</th>
                            <th>y:Title</th>
                        </tr>
                    </thead>
                    <tbody>
                        
                            <xsl:apply-templates/>
                        
                    </tbody>
                </table>
            </body>
        </html>
    </xsl:template>
    <xsl:template match="h">
        <tr>
        <td>
            <xsl:value-of select="@id"/>
        </td>
        <td>
            <xsl:value-of select="@type"/>
        </td>
        <td>
            <xsl:value-of select="@title"/>
        </td>
        <td>
            <xsl:value-of select="./x/@n"/>
        </td>
        <td>
            <xsl:value-of select="./x/@lo"/>
        </td>
        <td>
            <xsl:value-of select="./x/@hi"/>
        </td>
        <td>
            <xsl:value-of select="./x/@title"/>
        </td>
        <td>
            <xsl:value-of select="./y/@n"/>
        </td>
        <td>
            <xsl:value-of select="./y/@lo"/>
        </td>
        <td>
            <xsl:value-of select="./y/@hi"/>
        </td>
        <td>
            <xsl:value-of select="./y/@title"/>
        </td>
            </tr>
    </xsl:template>
</xsl:stylesheet>

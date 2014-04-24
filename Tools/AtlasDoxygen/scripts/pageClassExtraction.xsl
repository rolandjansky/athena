<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
	<xsl:output indent="yes" method="html"/>
	<!--Default to no output -->
	<xsl:template match="text()|@*"/>
	<!-- match to root element-->
	<xsl:template match="doxygen">
		<html>
			<head>
				<title>My test</title>
				<link href="doxygen.css" rel="stylesheet" type="text/css"></link>
				<link href="tabs.css" rel="stylesheet" type="text/css"></link>
			</head>
			<body>

					<table class="memname">
						<xsl:apply-templates/>
					</table>

			</body>
		</html>
	</xsl:template>
	<!-- match to compoundef, seems t be top element for class -->
	<xsl:template match="compounddef">
		<h2>
			<xsl:value-of select="compoundname"/>
		</h2>
<!--		<xsl:value-of select="detaileddescription"/> -->
		
		<xsl:apply-templates/>
	</xsl:template>
	<!-- match to the public definitions section, the context is now 'compoundef' if called from above -->
	<xsl:template match="sectiondef[@kind='public-func']">
		<!-- now we are in sectiondef-->
		<!-- iterate over the members for which the 'kind' attribute = 'function' -->
		
		<!-- <xsl:for-each select="memberdef[(@kind='function' ) ]"> -->
		<xsl:variable name="ClassName">
			<xsl:value-of select="substring-after( ../compoundname, '::' )"/>
		</xsl:variable>
		<xsl:for-each select="memberdef[(@kind='function') and ( (name!=$ClassName) and not(starts-with(name, '~')) and not(starts-with(name, 'operator')) and not(starts-with(name, 'clone'))) ]">
			<tr>
				<td BGCOLOR="#EEF3F5">
					<b><xsl:value-of select="definition"/></b>        
					<xsl:value-of select="argsstring"/> : 
				</td>
				<td BGCOLOR="#F0F8FF">
					<xsl:value-of select="briefdescription"/>
					<!-- <xsl:value-of select="detaileddescription"/> -->
				
				<xsl:if test= "name='px'">
					 Px 
				</xsl:if>
				<xsl:if test= "name='py'">
					 Py 
				</xsl:if>
				<xsl:if test= "name='pz'">
					 Pz
				</xsl:if>
				<xsl:if test= "name='m'">
					 Mass
				</xsl:if>
				<xsl:if test= "name='m2'">
					 Mass squared
				</xsl:if>
				<xsl:if test= "name='e'">
					 Energy
				</xsl:if>
				<xsl:if test= "name='et'">
					 Transverse energy
				</xsl:if>
				<xsl:if test= "name='rapidity'">
					 Rapidity
				</xsl:if>
				<xsl:if test= "name='p'">
					 Momentum
				</xsl:if>
				<xsl:if test= "name='p2'">
					 Momentum squared
				</xsl:if>
				<xsl:if test= "name='eta'">
					 Psuedo-rapidity
				</xsl:if>
				<xsl:if test= "name='phi'">
					 Phi
				</xsl:if>
				<xsl:if test= "name='pt'">
					 Transverse momentum
				</xsl:if>
				<xsl:if test= "name='iPt'">
					 Inverse transverse momentum
				</xsl:if>
				<xsl:if test= "name='cosPhi'">
					 Cosine phi
				</xsl:if>
				<xsl:if test= "name='sinPhi'">
					 Sine phi
				</xsl:if>
				<xsl:if test= "name='sinTh'">
					 Sine theta
				</xsl:if>
				<xsl:if test= "name='cosTh'">
					 Cosine theta
				</xsl:if>
				<xsl:if test= "name='tanTh'">
					 Tangent theta
				</xsl:if>
				<xsl:if test= "name='cotTh'">
					 Cotangent theta
				</xsl:if>
				<xsl:if test= "name='hlv'">
					 Lorentz Vector
				</xsl:if>
				<xsl:if test= "name='setIPt'">
					 Set transverse momentum
				</xsl:if>
			        <xsl:if test= "name='setCotTh'">
					 Set cotangent theta
				</xsl:if>
				<xsl:if test= "name='setPhi'">
					 Set phi
				</xsl:if>
				<xsl:if test= "name='setM'">
					 Set mass
				</xsl:if>
				<xsl:if test= "name='setPx'">
					 Set Px
				</xsl:if>
				<xsl:if test= "name='setPy'">
					 Set Py
				</xsl:if>
				<xsl:if test= "name='setPz'">
					 Set Pz
				</xsl:if>  
				<xsl:if test= "name='setE'" >
				         Set Energy
				</xsl:if>
					<xsl:if test= "name='pxi'">
					 Px 
				</xsl:if>
				<xsl:if test= "name='pyi'">
					 Py 
				</xsl:if>
				<xsl:if test= "name='pzi'">
					 Pz
				</xsl:if>		
				<xsl:if test= "name='ei'">
					 Energy
				</xsl:if> 
				<xsl:if test= "name='set4Mom'">
					 Set four-momentum
				</xsl:if>
				<xsl:if test= "name='dump'">
					 Output stream debubing information
				</xsl:if>
				<xsl:if test= "name='set_origin'">
					 Assign vertex to particle
				</xsl:if>
				<xsl:if test= "name='set_charge'">
					 Assign charge to particle
				</xsl:if>    
				<xsl:if test= "name='set_dataType'">
					 Assign data type to particle
				</xsl:if>
				<xsl:if test= "name='set_pdgId'">
					 Assign particle id to particle
				</xsl:if>
				<xsl:if test= "name='dataType'">
					Data type: 0=data,1=full simulation,2=fast shower, 3= fast simulation,4= Monte Carlo truth 
				</xsl:if>
				<xsl:if test= "name='origin'">
					RecVertex corresponding to particle Origin		 
				</xsl:if>
				<xsl:if test= "name='hasCharge'">
					Check if charge information is available		 
				</xsl:if>
				<xsl:if test= "name='charge'">
					Charge		 
				</xsl:if>
				<xsl:if test= "name='hasPdgId'">
					Check if particle data identifier exists		 
				</xsl:if>
				<xsl:if test= "name='pdgId'">
					Particle identificaton number		 
				</xsl:if>	
				
				</td>
			</tr>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>

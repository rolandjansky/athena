package atlantis.geometry;

import java.util.regex.Pattern;

import atlantis.config.ADefaultValues;
import atlantis.utils.ALogger;


/**
 * AAtlasDetector
 *   contains one instance of
 *   ABoxDetectors (which contains all instances of ABoxDetector)   (MDT,RPC in barrel)
 *   ATrapezoidDetectors (which contains all instances of ATrapezoidDetector) (MDT,TGC endcap)
 *   AGeneralDetectors (which contains all instances of AGeneralPathDetector,
 *                       ADiscDetector, ARectangleDetector) (all others)
 */
public class AAtlasDetector extends ADetectorSystem
{
    private static ALogger logger = ALogger.getLogger(AAtlasDetector.class);
    private static ADetectorSystem theDetector;
    
    public AAtlasDetector(String geomIdCalo)
    {
        super(new ADetectors[]
        {
            new AGeneralDetectors(
                new AGeometryFromXMLReader().readDetectorGeometryFromXML(geomIdCalo))
        });

        logger.info("Using geometry file: " + geomIdCalo);

        setGeometryName(geomIdCalo);

    } // AAtlasDetector(String fileName) ------------------------------------



    public AAtlasDetector(String geomIdCalo, String geomMuon)
    {
        super(new ADetectors[]
        {
            new ABoxDetectors(
                  new AGeometryFromXMLReader().readDetectorGeometryFromXML(geomMuon)),
            new ATrapezoidDetectors(
                  new AGeometryFromXMLReader().readDetectorGeometryFromXML(geomMuon)),
            new AGeneralDetectors(
                  new AGeometryFromXMLReader().readDetectorGeometryFromXML(geomMuon)),
            new AGeneralDetectors(
                  new AGeometryFromXMLReader().readDetectorGeometryFromXML(geomIdCalo))
        });

        logger.info("Using geometry file: " + geomIdCalo);
        logger.info("Using geometry file: " + geomMuon);

        setGeometryName(geomIdCalo);

    } // AAtlasDetector(String fileName, String muFileName) -----------------



    private static void setGeometryName(String absFileName)
    {
        // default geometry file, shall always be AGeometry.xml
        // arbitrary, named geometry file, geometry name after '_' character
        // (but there is also AGeometryTB.xml)
        String name = "AGeometry";
        String ext = ".xml";
        String pattern = ".*" + name + "_?.*" + ext;

        if(Pattern.compile(pattern).matcher(absFileName).matches())
        {
            int s = absFileName.lastIndexOf(name);
            int e = absFileName.lastIndexOf(ext);
            String r = absFileName.substring(s + name.length(), e);
            if(r.startsWith("_"))
            {
                // arbitrary, named geometry
                geometryName = r.substring(1); // without leading _
                ADefaultValues.set("GeometryName", geometryName);
            }
            else if("".equals(r))
            {
                // default geometry (no name given: "")
                geometryName = "<default>";
                ADefaultValues.set("GeometryName", "");
            }
            else
            {
                geometryName = r;
                ADefaultValues.set("GeometryName", geometryName);
            }
        }
        else
        {
            logger.warn("Geometry name not matching regular expression: " +
                          pattern + "\n  geometry name will be: n/a");
            geometryName = "n/a";
        }

    } // setGeometryName() --------------------------------------------------


	/**
	 * @return the detector
	 */
	public static ADetectorSystem getDetector() {
		return theDetector;
	}


	/**
	 * @param theDetector the ATLAS detector geometry to be used
	 */
	public static void setDetector(ADetectorSystem theDetector) {
		AAtlasDetector.theDetector = theDetector;
	}


} // class class AAtlasDetector =============================================

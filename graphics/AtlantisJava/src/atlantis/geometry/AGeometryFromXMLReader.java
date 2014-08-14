package atlantis.geometry;

import java.io.InputStream;
import java.util.List;
import java.util.ArrayList;

import atlantis.output.AExceptionHandler;
import atlantis.utils.xml.AXMLErrorHandler;
import atlantis.utils.AUtilities;
import org.xml.sax.helpers.*;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.Attributes;


class AGeometryFromXMLReader extends DefaultHandler {

  private static final boolean VALIDATION=false;
  private SAXParser parser;

  private static List<ADetector> detectors;

  public AGeometryFromXMLReader() {
    try {
      SAXParserFactory factory=SAXParserFactory.newInstance();

      factory.setValidating(true);
      factory.setNamespaceAware(true);
      parser=factory.newSAXParser();
      if(VALIDATION)
        parser.getXMLReader().setErrorHandler(new AXMLErrorHandler());
    } catch(Exception e) {
      AExceptionHandler.processException("Cannot create AGeometryFromXMLReader", e);
    }
  }

  protected List<ADetector> readDetectorGeometryFromXML(String fileName) 
  {
	  detectors = new ArrayList<ADetector>(2500);
	  try
	  {
		  InputStream isGeom = AUtilities.getFileAsStream(fileName);
		  parser.parse(isGeom, this);
	  }
	  catch(Exception e)
	  {
		  String msg = "Error when reading geometry file:\n" + e.getMessage();
		  AExceptionHandler.processException(msg, e);
	  }

	  return detectors;
  }

  static private double calculatePhi(String name, double phi0, int phiIndex) {
      String technology = name.substring(0, 3);
      if (technology.equals("TGC")) {
          if (name.charAt(6) == 'E' && name.charAt(5) != '4')
              return Math.toRadians(phi0) + (phiIndex - 1) * Math.PI / 24.;
          else
              return Math.toRadians(phi0) + (phiIndex - 1) * Math.PI / 12.;
      } else {
          return Math.toRadians(phi0) + (phiIndex - 1) * Math.PI / 4.;
      }
  }

  // ContentHandler Implementation

  public void startElement(String namespaceURI, String localName, String qName, Attributes atts) {
    if(localName.equals("ABox")||localName.equals("ATBx")) {
      String name=atts.getValue("n");
      double zi=Double.parseDouble(atts.getValue("zi"));
      double zo=Double.parseDouble(atts.getValue("zo"));
      double ri=Double.parseDouble(atts.getValue("ri"));
      double ro=Double.parseDouble(atts.getValue("ro"));
      double w=Double.parseDouble(atts.getValue("w"));
      double dPhi=Double.parseDouble(atts.getValue("dphi"));
      double rpci=Double.parseDouble(atts.getValue("RPCi"));
      double rpco=Double.parseDouble(atts.getValue("RPCo"));
      double s=Double.parseDouble(atts.getValue("sh"));
      int stationEta=Integer.parseInt(atts.getValue("eta"));
      String[] phis=atts.getValue("phi").split(" ");

      if(localName.equals("ABox")) {
        for(int i=0; i<phis.length; ++i)
          for(int zSide=-1; zSide<=1; zSide+=2) {
            int stationPhi = Integer.parseInt(phis[i]);
            double phi = calculatePhi(name, dPhi, stationPhi);
            detectors.add(new ABoxDetector(name, zSide*zi, zSide*zo, ri+rpci, ro-rpco, phi, s, w,
                                           zSide*stationEta, stationPhi));
            if(rpci>0.)
              detectors.add(new ABoxDetector("RPC"+name.substring(3)+"_I", zSide*zi, zSide*zo, ri,
                  ri+rpci, phi, s, w, zSide*stationEta, stationPhi));
            if(rpco>0.)
              detectors.add(new ABoxDetector("RPC"+name.substring(3)+"_O", zSide*zi, zSide*zo,
                  ro-rpco, ro, phi, s, w, zSide*stationEta, stationPhi));
          }
      } else if(localName.equals("ATBx")) {
        double zis=Double.parseDouble(atts.getValue("zis"));
        double zos=Double.parseDouble(atts.getValue("zos"));
        double ws=Double.parseDouble(atts.getValue("ws"));
        double or=Double.parseDouble(atts.getValue("or"));
        for(int i=0; i<phis.length; ++i)
          for(int zSide=-1; zSide<=1; zSide+=2) {
            int stationPhi = Integer.parseInt(phis[i]);
            double phi = calculatePhi(name, dPhi, stationPhi);
            detectors.add(new ATBxDetector(name, zSide*zi, zSide*zo, ri+rpci, ro-rpco, phi, s, w,
                zSide*zis, zSide*zos, ws, or, zSide*stationEta, stationPhi));
            if(rpci>0.)
              detectors.add(new ATBxDetector("RPC"+name.substring(3)+"_I", zSide*zi, zSide*zo, ri,
                  ri+rpci, phi, s, w, zSide*zis, zSide*zos, ws, or, zSide*stationEta, stationPhi));
            if(rpco>0.)
              detectors.add(new ATBxDetector("RPC"+name.substring(3)+"_O", zSide*zi, zSide*zo,
                  ro-rpco, ro, phi, s, w, zSide*zis, zSide*zos, ws, or, zSide*stationEta, stationPhi));
          }
      }

    } else if(localName.equals("ATrd")) {
      String name=atts.getValue("n");
      double zi=Double.parseDouble(atts.getValue("zi"));
      double zo=Double.parseDouble(atts.getValue("zo"));
      double ri=Double.parseDouble(atts.getValue("ri"));
      double ro=Double.parseDouble(atts.getValue("ro"));

      double wi=Double.parseDouble(atts.getValue("wi"));
      double wo=Double.parseDouble(atts.getValue("wo"));
      double dPhi=Double.parseDouble(atts.getValue("dphi"));
      double alpha=Double.parseDouble(atts.getValue("a"));
      int stationEta=Integer.parseInt(atts.getValue("eta"));
      String[] phis=atts.getValue("phi").split(" ");

      for(int i=0; i<phis.length; ++i) {
          for(int zSide = -1; zSide <= 1; zSide += 2) {
              int stationPhi = Integer.parseInt(phis[i]);
              double phi = calculatePhi(name, dPhi, stationPhi);
              detectors.add(new ATrapezoidDetector(name, zSide*zi, zSide*zo, ri, ro, phi, wi, wo,
                  alpha, zSide*stationEta, stationPhi));
          }
      }
    } else if(localName.equals("ABarrelCalorimeter")) {
      String name = atts.getValue("n");
      String color = atts.getValue("c");
      int sampling = Integer.parseInt(atts.getValue("sampling"));
      int region = Integer.parseInt(atts.getValue("region"));
      double rMin = Double.parseDouble(atts.getValue("rMin"));
      double rMax = Double.parseDouble(atts.getValue("rMax"));
      double zMin = Double.parseDouble(atts.getValue("zMin"));
      double zMax = Double.parseDouble(atts.getValue("zMax"));
      double eta0 = Double.parseDouble(atts.getValue("eta0"));
      double deta = Double.parseDouble(atts.getValue("deta"));
      int neta = Integer.parseInt(atts.getValue("neta"));
      int meta = Integer.parseInt(atts.getValue("meta"));
      double phi0 = Double.parseDouble(atts.getValue("phi0"));
      int nphi = Integer.parseInt(atts.getValue("nphi"));
      detectors.add(new ABarrelCalorimeterDetector(name, color, sampling, region, rMin, rMax,
                                                   zMin, zMax, eta0, deta, neta, meta, phi0, nphi));
    } else if(localName.equals("AEndcapCalorimeter")) {
      String name = atts.getValue("n");
      String color = atts.getValue("c");
      int sampling = Integer.parseInt(atts.getValue("sampling"));
      int region = Integer.parseInt(atts.getValue("region"));
      double rMin = Double.parseDouble(atts.getValue("rMin"));
      double rMax = Double.parseDouble(atts.getValue("rMax"));
      double zMin = Double.parseDouble(atts.getValue("zMin"));
      double zMax = Double.parseDouble(atts.getValue("zMax"));
      double eta0 = Double.parseDouble(atts.getValue("eta0"));
      double deta = Double.parseDouble(atts.getValue("deta"));
      int neta = Integer.parseInt(atts.getValue("neta"));
      int meta = Integer.parseInt(atts.getValue("meta"));
      double phi0 = Double.parseDouble(atts.getValue("phi0"));
      int nphi = Integer.parseInt(atts.getValue("nphi"));
      detectors.add(new AEndcapCalorimeterDetector(name, color, sampling, region, rMin, rMax,
                                                   zMin, zMax, eta0, deta, neta, meta, phi0, nphi));
    } else if(localName.equals("AEndcapCryostat")) {
        String name = atts.getValue("n");
        String color = atts.getValue("c");
        int sampling = Integer.parseInt(atts.getValue("sampling"));
        int region = Integer.parseInt(atts.getValue("region"));
        double rMin = Double.parseDouble(atts.getValue("rMin"));
        double rMax = Double.parseDouble(atts.getValue("rMax"));
        double zMin = Double.parseDouble(atts.getValue("zMin"));
        double zMax = Double.parseDouble(atts.getValue("zMax"));
        int neta = Integer.parseInt(atts.getValue("neta"));
        int nphi = Integer.parseInt(atts.getValue("nphi"));
        detectors.add(new AEndcapCryostatDetector(name, color, sampling, region, rMin, rMax, zMin,
                                                   zMax, neta, nphi));
    } else if(localName.equals("AGapCalorimeter")) {
      String name = atts.getValue("n");
      String color = atts.getValue("c");
      int sampling = Integer.parseInt(atts.getValue("sampling"));
      int region = Integer.parseInt(atts.getValue("region"));
      double rMin = Double.parseDouble(atts.getValue("rMin"));
      double rMax = Double.parseDouble(atts.getValue("rMax"));
      double zMin = Double.parseDouble(atts.getValue("zMin"));
      double zMax = Double.parseDouble(atts.getValue("zMax"));
      int eta = Integer.parseInt(atts.getValue("eta"));
      double phi0 = Double.parseDouble(atts.getValue("phi0"));
      int nphi = Integer.parseInt(atts.getValue("nphi"));
      detectors.add(new AGapCalorimeterDetector(name, color, sampling, region, rMin, rMax,
                                                zMin, zMax, eta, phi0, nphi));
    } else if(localName.equals("ABarrelSiliconDetector")) {
      String name = atts.getValue("n");
      String color = atts.getValue("c");
      int layer = Integer.parseInt(atts.getValue("layer"));
      double length = Double.parseDouble(atts.getValue("length"));
      double width = Double.parseDouble(atts.getValue("width"));
      double thickness = Double.parseDouble(atts.getValue("thickness"));
      int nphi = Integer.parseInt(atts.getValue("nphi"));
      int nz = Integer.parseInt(atts.getValue("nz"));
      double tilt = Double.parseDouble(atts.getValue("tilt"));
      double r0 = Double.parseDouble(atts.getValue("r0"));
      double phi0 = Double.parseDouble(atts.getValue("phi0"));
      double zMin = Double.parseDouble(atts.getValue("zMin"));
      double zMax = Double.parseDouble(atts.getValue("zMax"));
      detectors.add(new ABarrelSiliconDetector(name, color, layer, length, width, thickness, nz,
                                               nphi, tilt, r0, phi0, zMin, zMax));
    } else if(localName.equals("AEndcapSiliconDetector")) {
      String name = atts.getValue("n");
      String color = atts.getValue("c");
      int layer = Integer.parseInt(atts.getValue("layer"));
      double length = Double.parseDouble(atts.getValue("length"));
      double width = Double.parseDouble(atts.getValue("width"));
      double thickness = Double.parseDouble(atts.getValue("thickness"));
      int nphi = Integer.parseInt(atts.getValue("nphi"));
      int nz = Integer.parseInt(atts.getValue("nz"));
      double rMin = Double.parseDouble(atts.getValue("rMin"));
      double rMax = Double.parseDouble(atts.getValue("rMax"));
      double phi0 = Double.parseDouble(atts.getValue("phi0"));
      double zMin = Double.parseDouble(atts.getValue("zMin"));
      double zMax = Double.parseDouble(atts.getValue("zMax"));
      detectors.add(new AEndcapSiliconDetector(name, color, layer, length, width, thickness, nz,
                                               nphi, rMin, rMax, phi0, zMin, zMax));
    } else if(localName.equals("ABarrelTRTDetector")) {
      String name = atts.getValue("n");
      String color = atts.getValue("c");
      int layer = Integer.parseInt(atts.getValue("layer"));
      int nphi = Integer.parseInt(atts.getValue("nphi"));
      double rMin = Double.parseDouble(atts.getValue("rMin"));
      double rMax = Double.parseDouble(atts.getValue("rMax"));
      double phiIn = Double.parseDouble(atts.getValue("phiIn"));
      double phiOut = Double.parseDouble(atts.getValue("phiOut"));
      double zMin = Double.parseDouble(atts.getValue("zMin"));
      double zMax = Double.parseDouble(atts.getValue("zMax"));
      detectors.add(new ABarrelTRTDetector(name, color, layer, nphi, rMin, rMax, phiIn, phiOut, zMin, zMax));
    } else if(localName.equals("AEndcapTRTDetector")) {
      String name = atts.getValue("n");
      String color = atts.getValue("c");
      int layer = Integer.parseInt(atts.getValue("layer"));
      int nphi = Integer.parseInt(atts.getValue("nphi"));
      double rMin = Double.parseDouble(atts.getValue("rMin"));
      double rMax = Double.parseDouble(atts.getValue("rMax"));
      double phi0 = Double.parseDouble(atts.getValue("phi0"));
      double zMin = Double.parseDouble(atts.getValue("zMin"));
      double zMax = Double.parseDouble(atts.getValue("zMax"));
      detectors.add(new AEndcapTRTDetector(name, color, layer, nphi, rMin, rMax, phi0, zMin, zMax));
    } else if(localName.equals("ADisc")) {
      String name=atts.getValue("n");
      String projection=atts.getValue("p");
      String color=atts.getValue("c");
      double rInner=Double.parseDouble(atts.getValue("rIn"));
      double rOuter=Double.parseDouble(atts.getValue("rOut"));
      int nInner=Integer.parseInt(atts.getValue("nIn"));
      int nOuter=Integer.parseInt(atts.getValue("nOut"));

      detectors.add(new ADiscDetector(name, projection, color, rInner, rOuter, nInner, nOuter));
    } else if(localName.equals("ARectangle")) {
      String name=atts.getValue("n");
      String projection=atts.getValue("p");
      String color=atts.getValue("c");
      double xMin=Double.parseDouble(atts.getValue("xMin"));
      double xMax=Double.parseDouble(atts.getValue("xMax"));
      double yMin=Double.parseDouble(atts.getValue("yMin"));
      double yMax=Double.parseDouble(atts.getValue("yMax"));
      boolean xReflect=atts.getValue("xR").equals("YES");
      boolean yReflect=atts.getValue("yR").equals("YES");

      detectors.add(new ARectangleDetector(name, projection, color, xMin, xMax, yMin, yMax));
      if(xReflect) detectors.add(new ARectangleDetector(name, projection, color, -xMin, -xMax, yMin,
            yMax));
      if(yReflect) detectors.add(new ARectangleDetector(name, projection, color, xMin, xMax, -yMin,
            -yMax));
      if(xReflect&&yReflect) detectors.add(new ARectangleDetector(name, projection, color, -xMin,
            -xMax, -yMin, -yMax));
    }
  }
}
